#include<stm32f10x.h>
#include<stm32f10x_rcc.h>
#include<stm32f10x_gpio.h>
#include<stm32f10x_spi.h>
#include<stm32f10x_dma.h>
#include<string.h>

#include "display_ili9341.h"
#include "sdcard.h"
#include "hardware.h"
#include "terminal.h"
#include "helpers.h"

#define GO_IDLE_STATE 0 											//Программная перезагрузка
#define SEND_IF_COND 8 												//Для SDC V2 - проверка диапазона напряжений
#define READ_SINGLE_BLOCK 17 									//Чтение указанного блока данных
#define WRITE_SINGLE_BLOCK 24 								//Запись указанного блока данных
#define SD_SEND_OP_COND 41 										//Начало процесса инициализации
#define APP_CMD 55 														//Главная команда из ACMD команд
#define READ_OCR 58 													//Чтение регистра OCR

#define CS_ENABLE         GPIOB->BSRR = GPIO_BSRR_BR12;         
#define CS_DISABLE    	  GPIOB->BSRR = GPIO_BSRR_BS12;  

#define CLUSTERS_PER_PAGE (512/4)

extern Display_ILI9341 display;

unsigned char *buffer = 0;
uint8_t  SDHC;  
uint8_t SDError = 0;
unsigned int currentSector = 0xffffffff;

unsigned char sectorsPerCluster;
unsigned char fatCount;
unsigned int fatSize;
unsigned int fatStartAddr;
unsigned int cluster2Addr;
unsigned int rootClusterAddr;
unsigned int rootAddr;

uint8_t spiSend(uint8_t data){ 
  while (!(SPI2->SR & SPI_SR_TXE));      //убедиться, что предыдущая передача завершена
  SPI2->DR = data;                       //загружаем данные для передачи
  while (!(SPI2->SR & SPI_SR_RXNE));     //ждем окончания обмена
  return (SPI2->DR);		         				 //читаем принятые данные
}

uint8_t spiRead (void){ 
  return spiSend(0xff);		  						 //читаем принятые данные
}

void spiInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	// GPIO
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	// Spi2
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; 
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; 
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
	
	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, ENABLE);
}

uint8_t SDSendCommand(uint8_t cmd, uint32_t arg)
{
  uint8_t response, wait=0, tmp;     
 
  //для карт памяти SD выполнить коррекцию адреса, т.к. для них адресация побайтная 
  if(SDHC == 0)		
  if(cmd == READ_SINGLE_BLOCK || cmd == WRITE_SINGLE_BLOCK )  {arg = arg << 9;}
  //для SDHC коррекцию адреса блока выполнять не нужно(постраничная адресация)	
 
  CS_ENABLE;
 
  //передать код команды и ее аргумент
  spiSend(cmd | 0x40);
  spiSend(arg>>24);
  spiSend(arg>>16);
  spiSend(arg>>8);
  spiSend(arg);
 
  //передать CRC (учитываем только для двух команд)
  if(cmd == SEND_IF_COND) spiSend(0x87);            
  else                    spiSend(0x95); 
 
  //ожидаем ответ
  while((response = spiRead()) == 0xff) 
   if(wait++ > 0xfe) break;                //таймаут, не получили ответ на команду
 
  //проверка ответа если посылалась команда READ_OCR
  if(response == 0x00 && cmd == 58)     
  {
    tmp = spiRead();                       //прочитать один байт регистра OCR            
    if(tmp & 0x40) SDHC = 1;               //обнаружена карта SDHC 
    else           SDHC = 0;               //обнаружена карта SD
    //прочитать три оставшихся байта регистра OCR
    spiRead(); 
    spiRead(); 
    spiRead(); 
  }
 
  spiRead();
 
  CS_DISABLE; 
 
  return response;
}

uint8_t SDInit(void)
{
  uint8_t   i;
  uint8_t   response;
  uint8_t   SD_version = 2;
  uint16_t  retry = 0 ;
 
  spiInit();                             //init spi                      
  for(i=0;i<20;i++) spiSend(0xff);      //send > 74 bits  
 
  //выполним программный сброс карты
  CS_ENABLE;
  while(SDSendCommand(GO_IDLE_STATE, 0)!=0x01)                                   
    if(retry++>0x20)  return 1;                    
  CS_DISABLE;
  spiSend (0xff);
  spiSend (0xff);
 
  retry = 0;                                     
  while(SDSendCommand(SEND_IF_COND,0x000001AA)!=0x01)
  { 
    if(retry++>0xfe) 
    { 
      SD_version = 1;
      break;
    } 
  }
 
 retry = 0;                                     
 do
 {
   response = SDSendCommand(APP_CMD,0); 
   response = SDSendCommand(SD_SEND_OP_COND,0x40000000);
   retry++;
   if(retry>0xffe) return 1;                     
 }while(response != 0x00);                      
 
 
 //читаем регистр OCR, чтобы определить тип карты
 retry = 0;
 SDHC = 0;
 if (SD_version == 2)
 { 
   while(SDSendCommand(READ_OCR,0)!=0x00)
	 if(retry++>0xfe)  break;
 }
 
 return 0; 
}

bool SDReadSector(unsigned int sector)
{ 
	if (buffer && sector != currentSector){
		currentSector = sector;
		uint8_t *buff = buffer;
		uint16_t i=0;
	 
		//послать команду "чтение одного блока" с указанием его номера
		if(SDSendCommand(READ_SINGLE_BLOCK, sector)) return false;  
		
		CS_ENABLE;
		//ожидание  маркера данных
		while(spiRead() != 0xfe)                
		if(i++ > 0xfffe) {CS_DISABLE; return false;}       
	 
		//чтение 512 байт	выбранного сектора
		for(i=0; i<512; i++) *buff++ = spiRead();
	 
		spiRead(); 
		spiRead(); 
		spiRead(); 
	 
		CS_DISABLE;
		
		return true;
	}
  return false;
}

bool SDEnable(unsigned char *newBuffer){
	SDError = SDInit();
	if (SDError == 0){
		buffer = newBuffer;
		if (!SDReadSector(0)) return false;
		if (SDGetWord(0x1fe) != 0xaa55) return false;
		
		unsigned short bootSector = SDGetWord(0x1c6);
		if (!SDReadSector(bootSector)) return false;
		
		sectorsPerCluster = SDGetByte(0x0D);
		fatCount = SDGetByte(0x0C);
		fatSize = SDGetDWord(0x24);
		fatStartAddr = SDGetWord(0x0E) + bootSector;
		cluster2Addr = fatStartAddr+(fatSize*fatCount);
		rootClusterAddr = SDGetDWord(0x2c);
		rootAddr = cluster2Addr+((rootClusterAddr-2)*sectorsPerCluster);
		// unsigned int rootFolderSize = (fileSize(rootClusterAddr)*sectorsPerCluster);
		
		return true;
	}
	return false;
}

void SDDisable(){
	buffer = 0;
}

unsigned char SDGetByte(unsigned int address){
	return buffer[address];
}

unsigned short SDGetWord(unsigned int address){
	return buffer[address] + (buffer[address+1] << 8);
}

unsigned int SDGetDWord(unsigned int address){
	return buffer[address] + (buffer[address+1] << 8) + (buffer[address+2] << 16) + (buffer[address+3] << 24);
}

bool SDIsHC(){
	return SDHC ? true : false;
}

unsigned char getSDError(){
	return SDError;
}

void FSGetNameFromFileStruct(unsigned char* fStruct, char *fileName){
	unsigned char nameCounter = 0;
	for (int i = 0; i < 8; i++){
		fileName[nameCounter] = fStruct[i];
		nameCounter++;
	}
			
	// Remove spaces
	while(nameCounter > 0){
		nameCounter--;
		if (fileName[nameCounter] != ' ') break;
	}
	nameCounter++;
	
	// Extention
	if (fStruct[8] != ' '){
		fileName[nameCounter] = '.';
		nameCounter++;

		for (int i = 0; i < 3; i++){
			fileName[nameCounter] = fStruct[i+8];
			nameCounter++;
		}
				
		// Remove spaces
		while(nameCounter > 0){
			nameCounter--;
			if (fileName[nameCounter] != ' ') break;
		}
		nameCounter++;
	}

	fileName[nameCounter] = 0;
}

int FSGetCluster(char *filePath, FileInfo *fileInfo){
	char nameBuffer[32];
	char cmpName[14];
	short int searcher = 0, counter;
	unsigned int sectorStart, sectorCounter, inSectorShift;
	int cluster = -1;
	
	if (buffer){
		if (filePath[0] == '/'){
			cluster = 2;
			searcher = 1;
			sectorStart = rootAddr;
		}
		
		while(filePath[searcher]){
			counter = 0;
			while(filePath[searcher] && filePath[searcher] != '/'){ 
				nameBuffer[counter] = filePath[searcher];
				searcher++;
				counter++;
			}
			nameBuffer[counter] = 0;

			// Searching the file
			if (strlen(nameBuffer)){
				sectorCounter = 0;
				inSectorShift = 0;
				
				while(1){
					SDReadSector(sectorStart+sectorCounter);
					
					if (buffer[inSectorShift] != 0xE5 && buffer[inSectorShift] != 0x05 && buffer[inSectorShift] != 0x2E && buffer[inSectorShift+0x0b] != 0x0f){
						// File not found
						if (buffer[inSectorShift] == 0) return -1; 
						
						// Compare file names and get cluster if match
						FSGetNameFromFileStruct(buffer+inSectorShift, cmpName);
						if (cmpi(cmpName, nameBuffer)){
							cluster = SDGetWord(inSectorShift+0x1A) + (SDGetWord(inSectorShift+0x14) << 16);
							break;
						}
					}
					
					inSectorShift+=32;
					if (inSectorShift >= 512){
						inSectorShift = 0;
						sectorCounter++;
						if (sectorCounter >= sectorsPerCluster){
							// Todo - next cluster in folder
							return -1;
						}
					}					
				}
				
				if (filePath[searcher] == '/'){
					// Todo - check if file is directory to continue deeper
					while(filePath[searcher] == '/') searcher++;
				}else{
					// Todo - check if directory
					if (filePath[searcher] == 0 && fileInfo){
						fileInfo->fileSize = SDGetDWord(inSectorShift+0x1C);
					}					
				}
				nameBuffer[0] = 0;
			} else {
				return cluster;
			}
		}
	}
	return cluster;
}

unsigned int FSGetStartSector(int cluster){
	return rootAddr+((cluster-2)*sectorsPerCluster);
}

int FSGetNextCluster(int cluster){
	unsigned int clusterPage = cluster / 128;
	unsigned int clusterPageShift = (cluster%128)*4;
	SDReadSector(fatStartAddr+clusterPage);
	return SDGetDWord(clusterPageShift);
}

bool FSReadDir(char *path, DirectoryReader *dirReader){
	if (buffer){
		int cluster = FSGetCluster(path);
		if (cluster == -1) return false;
		dirReader->currentCluster = cluster;
		dirReader->sectorShift = 0;
		dirReader->inSectorShift = 0;
		return true;
	}
	return false;
}

bool FSReadNextFile(char *fileName, int fileNameLength, DirectoryReader *dirReader, FileInfo *fileInfo){
	char len;
	unsigned short shift;
	fileNameLength--;
	
	if (fileNameLength < 13){
		return false;
	}	
	
	if (buffer && dirReader->currentCluster != -1){
		while(1){
			shift = dirReader->inSectorShift;
			
			dirReader->inSectorShift += 32;
			if (dirReader->inSectorShift >= 512){
				dirReader->inSectorShift = 0;
				dirReader->sectorShift++;
				if (dirReader->sectorShift >= sectorsPerCluster){
					dirReader->currentCluster = -1;
					return false;
				}
			}
			
			SDReadSector(FSGetStartSector(dirReader->currentCluster) + dirReader->sectorShift);

			// Empty
			if (buffer[shift] == 0) continue;
			
			// File deleted
			if (buffer[shift] == 0xE5) continue;
			
			// Long name
			if (buffer[shift+0x0b] == 0x0f){
				// TODO - do it
			} else {
				// File info
				FSGetNameFromFileStruct(buffer+shift, fileName);
				len = strlen(fileName);
				
				fileInfo->fileSize = SDGetDWord(shift+0x1C);
				fileInfo->permissions = SDGetWord(shift+0x14);
				fileInfo->flags = 0;
				
				if (SDGetByte(shift+0x0B) & 0x02){
					fileInfo->flags |= FILE_FLAGS_HIDDEN;
				}
				
				if (SDGetByte(shift+0x0B) & 0x04){
					fileInfo->flags |= (FILE_FLAGS_HIDDEN | FILE_FLAGS_SYSTEM);
				}
				
				if (fileName[len-3] == 'V' && fileName[len-2] == 'E' && fileName[len-1] == 'X'){
					fileInfo->fileType = FILE_TYPE_RUNNABLE;
				}else{
					if (SDGetByte(shift+0x0B) & 0x10){
						fileInfo->fileType = FILE_TYPE_DIRECTORY;
					} else {
						fileInfo->fileType = FILE_TYPE_UNKNOWN;
					}
				}
				return true;
			}
		}
	}
	return false;
}

bool FSReadFile(char *filePath, FileWorker *fileWorker){
	if (buffer){
		FileInfo fileInfo;
		int cluster = FSGetCluster(filePath, &fileInfo);
		if (cluster == -1) return false;
		fileWorker->currentCluster = cluster;
		fileWorker->inSectorShift = 0;
		fileWorker->sectorShift = 0;
		fileWorker->buffer = 0;
		fileWorker->mode = FS_FILE_MODE_R;
		fileWorker->fileSize = fileInfo.fileSize;
		fileWorker->readed = 0;
		return true;
	}	
	return false;
}

unsigned int FSRead(FileWorker *fileWorker, void *dst, unsigned int length){
	unsigned int readedInSession = 0;
	unsigned int startSectorInCluster = FSGetStartSector(fileWorker->currentCluster);
	char *c = (char*)dst;
	
	while(length){
		if (fileWorker->readed >= fileWorker->fileSize) break;
		
		fileWorker->readed++;
		SDReadSector(startSectorInCluster + fileWorker->sectorShift);
		
		*c = buffer[fileWorker->inSectorShift];
		c++;
		fileWorker->inSectorShift++;
		
		if (fileWorker->inSectorShift >= 512){
			fileWorker->inSectorShift = 0;
			fileWorker->sectorShift++;
			if (fileWorker->sectorShift >= sectorsPerCluster){
				fileWorker->sectorShift = 0;
				fileWorker->currentCluster = FSGetNextCluster(fileWorker->currentCluster);
				if (fileWorker->currentCluster >= 0x0fffffff){
					fileWorker->fileSize = 0;
					break;
				}
				startSectorInCluster = FSGetStartSector(fileWorker->currentCluster);
			}
		}
		
		length--;
	}
	
	return readedInSession;
}

//unsigned int FSWrite(FileWorker *fileWorker, unsigned char *buffer, int length){
//	return 0;
//}

bool FSSeek(unsigned int shift, FileWorker *fileWorker){
	return false;
}

bool FSClose(FileWorker *fileWorker){
	fileWorker->fileSize = 0;
	return true;
}







