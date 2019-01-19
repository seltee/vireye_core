#include <string.h>
#include <stm32f10x.h>

#include "vex_loader.h"
#include "terminal.h"
#include "engine.h"
#include "display_ili9341.h"
#include "text.h"
#include "mem.h"
#include "helpers.h"
#include "input.h"
#include "hardware.h"
#include "sdcard.h"

#define FLASH_KEY1 ((uint32_t)0x45670123)
#define FLASH_KEY2 ((uint32_t)0xCDEF89AB)

extern Display_ILI9341 display;

const unsigned char *entryPoint = 0;

void displaySync(){
	display.draw();
}

unsigned short getFPS(){
	return display.getFPS();
}

void setFPS(unsigned short limit){
	display.setFPS(limit);
}

int run(char *path, unsigned char *ramBuffer){
	return loadGame(path, ramBuffer) && runGame();
}

void *getCmd(char *name){
	// Drawing
	if (cmp("c.dSetLineClear", name)){
		return (void *)Engine::setLineClear;
	}
	if (cmp("c.dSetFillColor", name)){
		return (void *)Engine::setFillColor;
	}
	if (cmp("c.dSetSpriteMemory", name)){
		return (void *)Engine::setSpriteMemory;
	}
	if (cmp("c.dDisplaySprite", name)){
		return (void *)Engine::displaySprite;
	}
	if (cmp("c.dDisplaySpriteBitMask", name)){
		return (void *)Engine::displaySpriteBitMask;
	}
	if (cmp("c.dDisplaySpriteByteMask", name)){
		return (void *)Engine::displaySpriteByteMask;
	}
	if (cmp("c.dDisplaySpriteMatrix", name)){
		return (void *)Engine::displaySpriteMatrix;
	}
	if (cmp("c.dDisplayText", name)){
		return (void *)Text::displayString;
	}
	if (cmp("c.dSync", name)){
		return (void *)displaySync;
	}
	if (cmp("c.dGetFPS", name)){
		return (void *)getFPS;
	}
	if (cmp("c.dSetFPS", name)){
		return (void *)setFPS;
	}
	if (cmp("c.dSetPalette", name)){
		return (void *)Engine::setPalette;
	}
	
	// Input
	if (cmp("c.iGetState", name)){
		return (void *)Input::getState;
	}
	if (cmp("c.iGetXAxis", name)){
		return (void *)Input::getXAxis;
	}
	if (cmp("c.iGetYAxis", name)){
		return (void *)Input::getYAxis;
	}
	
	// FS
	if (cmp("c.fsInit", name)){
		return (void *)SDEnable;
	}
	if (cmp("c.fsReadDir", name)){
		return (void *)FSReadDir;
	}
	if (cmp("c.fsReadNextFile", name)){
		return (void *)FSReadNextFile;
	}
	if (cmp("c.fsReadFile", name)){
		return (void *)FSReadFile;
	}
	if (cmp("c.fsRead", name)){
		return (void *)FSRead;
	}
	if (cmp("c.fsSeek", name)){
		return (void *)FSSeek;
	}
	if (cmp("c.fsClose", name)){
		return (void *)FSClose;
	}

	// Timer
	if (cmp("c.tGetTimer", name)){
		return (void *)getTimer;
	}
	if (cmp("c.tGetTimerWithClear", name)){
		return (void *)getTimerWithClear;
	}
	if (cmp("c.tClearTimer", name)){
		return (void *)clearTimer;
	}
	
	// Helpers
	if (cmp("c.hCmp", name)){
		return (void *)cmp;
	}
	if (cmp("c.hItoa", name)){
		return (void *)itoa;
	}
	
	// Default
	if (cmp("c.memset", name)){
		return (void *)memset;
	}
	if (cmp("c.memcpy", name)){
		return (void *)memcpy;
	}
	if (cmp("c.strlen", name)){
		return (void *)strlen;
	}

	// System
	if (cmp("c.sRun", name)){
		return (void *)run;
	}
	
	return 0;
}

bool flash_ready(void) {
	return !(FLASH->SR & FLASH_SR_BSY);
}

void clearMemory(const unsigned char *address, unsigned int size){
	for (int i = 0; i < size; i+= 1024){
		FLASH->CR|= FLASH_CR_PER; // Bit for clearing one page
		FLASH->AR = (unsigned int)(address+i); // Address
		FLASH->CR|= FLASH_CR_STRT; // Start flashing
		while(!flash_ready()); // Waiting
		FLASH->CR&= ~FLASH_CR_PER;
	}
}

void saveCode(const unsigned char *data, const unsigned char *address, unsigned int count){
	FLASH->CR |= FLASH_CR_PG; // Allow flash programming
	for (int i = 0; i < count; i+=4){
		while(!flash_ready()); // Waiting
		unsigned int bytes4 = *((unsigned int*)(&data[i])); // 4 bytes to write
		*(__IO uint16_t*)address = (uint16_t)bytes4; 
		while(!flash_ready()); // Waiting
		address += 2;
		bytes4 >>= 16;
		*(__IO uint16_t*)address = (uint16_t)bytes4;
		while(!flash_ready());
		address += 2;
	}
	FLASH->CR &= ~(FLASH_CR_PG); // Disallow flash programming
}

bool checkHeader(VexMainHeader *header, int romSize){
	if (header->mark[0] == 'V' && header->mark[1] == 'E' && header->mark[2] == 'E' && header->mark[3] == 'X'){
		if (header->romSize + header->codeSize > romSize){
			return false;
		}
		
		if (header->ramSize > RAM_SIZE){
			return false;
		}
		
		return true;
	}
	return false;
}

void unlockMemory(){
	FLASH->KEYR = FLASH_KEY1;
	FLASH->KEYR = FLASH_KEY2;
}

bool loadGameInternal(){
	const unsigned char *reader = fileRom;
	unsigned char *partData = ram+(8*1024);
	char *name;
	unsigned int relCount, cmd;
	unsigned int totalBlockSize;
	unsigned short p1, p2;
	unsigned int p;
	unsigned int jmpFrom;
		
	// Starting the program
	VexMainHeader header;
	VexSubHeader subHeader;
	VexCodeSliceHeader codeSliceHeader;

	memcpy(&header, reader, sizeof(VexMainHeader));			
	reader += sizeof(VexMainHeader);
	
	// Check if we can run program
	if (!checkHeader(&header, 24*1024)) return false;

	// Unlock memory to write
	unlockMemory();

	// Clear memory for game
	clearMemory(gameRom, 24*1024);
	
	//Note: using terminal may broke your code, because they may change data stored in after loading ram section
	const unsigned char *romRomShift = gameRom+header.codeSize;
	while(1){
		memcpy(&subHeader, reader, sizeof(VexSubHeader));			
		reader += sizeof(VexSubHeader);

		switch(subHeader.type){
			case VEX_BLOCK_TYPE_CODE_PART:
			
				for (int i = 0; i < subHeader.size; i++){
					memcpy(&codeSliceHeader, reader, sizeof(VexCodeSliceHeader));
					
					reader += sizeof(VexCodeSliceHeader);
					totalBlockSize = codeSliceHeader.codeLength + codeSliceHeader.symNameTableLength + (codeSliceHeader.relocationsCount*sizeof(VexCodeRelocation));
					relCount = codeSliceHeader.relocationsCount;
								
					memcpy(partData, reader, totalBlockSize);
					reader += totalBlockSize;
					
					for (int rel = 0; rel < relCount; rel++){
						VexCodeRelocation *verRel = (VexCodeRelocation *)(partData + codeSliceHeader.codeLength + codeSliceHeader.symNameTableLength + rel*sizeof(VexCodeRelocation));
						switch(verRel->type){
							case VEX_BLOCK_TYPE_MAP:
							case VEX_REL_TYPE_CODE:
								if (verRel->source != 1){
									Terminal::sendString("Wr bnd src");
									return false;
								}
								
								if (verRel->bind == 1){
									if (verRel->type == VEX_BLOCK_TYPE_MAP){
										cmd = (unsigned int)(gameRom + verRel->targetShift) + 1;
									} else {
										name = (char*)partData + codeSliceHeader.codeLength + verRel->nameShift;
										cmd = (unsigned int)getCmd(name);
										if (!cmd){
											Terminal::sendString("Unk cmd");
											return false;
										}
									}
									
									jmpFrom = (unsigned int)(gameRom + codeSliceHeader.globalShift + verRel->shift);
									p = ((int)cmd - (int)jmpFrom - 4) & 0x7FFFFF;
									p1 = (p >> 12) & 0x07ff;
									p2 = (p >> 1) & 0x07ff;
									
									*((short int*)(&partData[verRel->shift])) = (*((short int*)(&partData[verRel->shift])) & 0xf800) + p1;
									*((short int*)(&partData[verRel->shift+2])) = (*((short int*)(&partData[verRel->shift+2])) & 0xf800) + p2;
								}
								
								if (verRel->bind == 0){
									*((int*)(&partData[verRel->shift])) = (int)(romRomShift + verRel->targetShift);
								}
							break;
																	
							case VEX_REL_TYPE_ROM:
								*((int*)(&partData[verRel->shift])) = (int)(romRomShift + verRel->targetShift);
							break;

							case VEX_REL_TYPE_RAM:
								*((int*)(&partData[verRel->shift])) = (int)(ram + verRel->targetShift);
							break;
							
							default:
								Terminal::sendString("Unk tp\n");
								Terminal::sendNumber(verRel->type);
								return false;
						}
					}
					saveCode(partData, (unsigned char*)(gameRom + codeSliceHeader.globalShift), codeSliceHeader.codeLength);
				}
			break;
				
			case VEX_BLOCK_TYPE_RODATA:
				saveCode(reader, romRomShift, subHeader.size);
				reader += subHeader.size;
			break;
			
			case VEX_BLOCK_TYPE_RAM:
				memcpy(ram, reader, subHeader.size);
				reader += subHeader.size;
			break;
				
			case VEX_BLOCK_TYPE_END:
				entryPoint = gameRom + header.entry;
				return true;

			default:
				Terminal::sendString("Unk bl tp\n");
				Terminal::sendNumber(subHeader.type);
				return false;
		}
	}
}

void showError(char *error){
	Terminal::setMemory(ram);
	Terminal::sendString(error);
	while(1){
		Terminal::draw();
		display.draw();
	}
}

// Needs around 6 kb
bool loadGame(char *path, unsigned char *ramBuffer){
	FileWorker fileWorker;
	VexMainHeader header;
	VexSubHeader subHeader;
	VexCodeSliceHeader codeSliceHeader;
	
	unsigned int totalBlockSize, relCount, cmd, jmpFrom;
	unsigned short p1, p2;
	unsigned int p;
	char *name;
	
	if (FSReadFile(path, &fileWorker)){
		FSRead(&fileWorker, &header, sizeof(VexMainHeader));
		// Check header and is system suitable to running this file
		if (checkHeader(&header, 48*1024)){
			
			// Unlock memory to write
			unlockMemory();
			
			// Clear memory
			clearMemory(fileRom, 48*1024);
			
			const unsigned char *romRomShift = fileRom+header.codeSize;
			// Main loop of sections
			while(1){
				FSRead(&fileWorker, &subHeader, sizeof(VexSubHeader));
				switch(subHeader.type){
					case VEX_BLOCK_TYPE_CODE_PART:

						// Code blocks
						for (int i = 0; i < subHeader.size; i++){
							// Code block information
							FSRead(&fileWorker, &codeSliceHeader, sizeof(VexCodeSliceHeader));
							totalBlockSize = codeSliceHeader.codeLength + codeSliceHeader.symNameTableLength + (codeSliceHeader.relocationsCount*sizeof(VexCodeRelocation));
							relCount = codeSliceHeader.relocationsCount;
							
							FSRead(&fileWorker, ramBuffer, totalBlockSize);
							
							for (int rel = 0; rel < relCount; rel++){
								VexCodeRelocation *verRel = (VexCodeRelocation *)(ramBuffer + codeSliceHeader.codeLength + codeSliceHeader.symNameTableLength + rel*sizeof(VexCodeRelocation));

								switch(verRel->type){
									case VEX_BLOCK_TYPE_MAP:
									case VEX_REL_TYPE_CODE:
										if (verRel->source != 1){
											showError("Wr bnd src");
											return false;
										}
										
										if (verRel->bind == 1){
											if (verRel->type == VEX_BLOCK_TYPE_MAP){
												cmd = (unsigned int)(fileRom + verRel->targetShift) + 1;
											} else {
												name = (char*)ramBuffer + codeSliceHeader.codeLength + verRel->nameShift;
												cmd = (unsigned int)getCmd(name);
												if (!cmd){
													showError("Unk cmd");
													return false;
												}
											}
											
											jmpFrom = (unsigned int)(fileRom + codeSliceHeader.globalShift + verRel->shift);
											p = ((int)cmd - (int)jmpFrom - 4) & 0x7FFFFF;
											p1 = (p >> 12) & 0x07ff;
											p2 = (p >> 1) & 0x07ff;
											
											*((short int*)(&ramBuffer[verRel->shift])) = (*((short int*)(&ramBuffer[verRel->shift])) & 0xf800) + p1;
											*((short int*)(&ramBuffer[verRel->shift+2])) = (*((short int*)(&ramBuffer[verRel->shift+2])) & 0xf800) + p2;
										}
										
										if (verRel->bind == 0){
											*((int*)(&ramBuffer[verRel->shift])) = (int)(romRomShift + verRel->targetShift);
										}
									break;
											
									case VEX_REL_TYPE_ROM:
										*((int*)(&ramBuffer[verRel->shift])) = (int)(romRomShift + verRel->targetShift);
									break;

									case VEX_REL_TYPE_RAM:
										*((int*)(&ramBuffer[verRel->shift])) = (int)(ram + verRel->targetShift);
									break;
									
									default:
										showError("Unk tp\n");
										return false;
								}
							}
							
							// fileRom - start of our 48 kb rom
							saveCode(ramBuffer, (unsigned char*)(fileRom + codeSliceHeader.globalShift), codeSliceHeader.codeLength);
						}
					break;
						
					case VEX_BLOCK_TYPE_RODATA:
						if (subHeader.size){
							FSRead(&fileWorker, ramBuffer, subHeader.size);
							saveCode(ramBuffer, romRomShift, subHeader.size);
						}
					break;
					
					case VEX_BLOCK_TYPE_RAM:
						if (subHeader.size){
							FSRead(&fileWorker, ram, subHeader.size);
						}
					break;
						
					case VEX_BLOCK_TYPE_END:
						entryPoint = fileRom + header.entry;				
						return true;

					default:
						//Terminal::sendString("Unk bl tp\n");
						//Terminal::sendNumber(subHeader.type);
						return false;
				}
			}
		}
	}
	return false;
}

int runGame(){
	const unsigned char *prog = entryPoint + 1;
	typedef int func(void);
	func* f = (func*)(prog);
	int b = f();
	return b;
}
