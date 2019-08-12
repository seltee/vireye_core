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
#include "sound.h"
#include "config.h"

#define FLASH_KEY1 ((uint32_t)0x45670123)
#define FLASH_KEY2 ((uint32_t)0xCDEF89AB)

extern Display_ILI9341 display;

const unsigned char *entryPoint = 0;
unsigned short ramProgOccupied;

void displaySync(){
	display.draw();
}

unsigned short getFPS(){
	return display.getFPS();
}

void setFPS(unsigned short limit){
	display.setFPS(limit);
}

int run(char *path, char *ramBuffer){
	return loadGame(path, ramBuffer) && runGame();
}

bool sCmp(const char *cmd, const char *name){
	return (strlen(name) > 2 && name[0] == 'c' && name[1] == '.' && cmp((char *)cmd, (char *)name+2));
}

void *getCmd(char *name){
	// Drawing
	if (sCmp("setLineClear", name)){
		return (void *)Engine::setLineClear;
	}
	if (sCmp("setFillColor", name)){
		return (void *)Engine::setFillColor;
	}
	if (sCmp("setSpriteLimit", name)){
		return (void *)Engine::setSpriteLimit;
	}
	if (sCmp("displaySprite", name)){
		return (void *)Engine::displaySprite;
	}
	if (sCmp("displaySpriteBitMask", name)){
		return (void *)Engine::displaySpriteBitMask;
	}
	if (sCmp("displaySpriteByteMask", name)){
		return (void *)Engine::displaySpriteByteMask;
	}
	if (sCmp("displaySpriteMatrix", name)){
		return (void *)Engine::displaySpriteMatrix;
	}
	if (sCmp("displayFilledRect", name)){
		return (void *)Engine::displayFilledRect;
	}
	if (sCmp("displayRect", name)){
		return (void *)Engine::displayRect;
	}
	if (sCmp("displayText", name)){
		return (void *)Text::displayString;
	}
	if (sCmp("sync", name)){
		return (void *)displaySync;
	}
	if (sCmp("setFPS", name)){
		return (void *)setFPS;
	}
	if (sCmp("setPalette", name)){
		return (void *)Engine::setPalette;
	}
	
	// Input
	if (sCmp("getButtonState", name)){
		return (void *)Input::getState;
	}
	if (sCmp("getXAxis", name)){
		return (void *)Input::getXAxis;
	}
	if (sCmp("getYAxis", name)){
		return (void *)Input::getYAxis;
	}
	
	// FS
	if (sCmp("setFSMemory", name)){
		return (void *)SDEnable;
	}
	if (sCmp("readDir", name)){
		return (void *)FSReadDir;
	}
	if (sCmp("readNextFile", name)){
		return (void *)FSReadNextFile;
	}
	if (sCmp("closeDir", name)){
		return (void *)FSCloseDir;
	}
	if (sCmp("openToRead", name)){
		return (void *)FSReadFile;
	}
	if (sCmp("openToWrite", name)){
		return (void *)FSWriteFile;
	}
	if (sCmp("readFile", name)){
		return (void *)FSRead;
	}
	if (sCmp("writeFile", name)){
		return (void *)FSWrite;
	}
	/*
	if (cmp("seekFile", name)){
		return (void *)FSSeek;
	}
	*/
	if (sCmp("closeFile", name)){
		return (void *)FSClose;
	}
	
	// Sound
	if (sCmp("enableSoundMono", name)){
		return (void *)enableSoundMono;
	}
	if (sCmp("disableSound", name)){
		return (void *)disableSound;
	}
	
	// Config
	if (sCmp("getConfig", name)){
		return (void *)getConfig;
	}
	if (sCmp("applyConfig", name)){
		return (void *)applyConfig;
	}
	if (sCmp("saveConfig", name)){
		return (void *)saveConfig;
	}
	
	// Timer
	if (sCmp("getTimer", name)){
		return (void *)getTimer;
	}
	if (sCmp("getTimerWithClear", name)){
		return (void *)getTimerWithClear;
	}
	if (sCmp("clearTimer", name)){
		return (void *)clearTimer;
	}
	
	// Memory
	if (sCmp("malloc", name)){
		return (void *)malloc;
	}
	
	if (sCmp("free", name)){
		return (void *)free;
	}
	
	if (sCmp("getFreeMem", name)){
		return (void *)getFreeMem;
	}
	
	// Default
	if (sCmp("cmp", name)){
		return (void *)cmp;
	}
	if (sCmp("itoa", name)){
		return (void *)itoa;
	}
	if (sCmp("memset", name)){
		return (void *)memset;
	}
	if (sCmp("memcpy", name)){
		return (void *)memcpy;
	}
	if (sCmp("strlen", name)){
		return (void *)strlen;
	}
	if (sCmp("rand", name)){
		return (void *)rand;
	}

	// System
	if (sCmp("run", name)){
		return (void *)run;
	}
	
	// Compiler features
	if (sCmp("__umodsi3", name)){
		return (void *)__umodsi3;
	}
	if (sCmp("__modsi3", name)){
		return (void *)__modsi3;
	}
	if (sCmp("__udivsi3", name)){
		return (void *)__udivsi3;
	}
	if (sCmp("__divsi3", name)){
		return (void *)__divsi3;
	}
	if (sCmp("_Unwind_Resume", name)){
		return (void *)1;
	}
	// new[unsigned int]
	if (sCmp("_Znwj", name)){
		return (void *)malloc;
	}
	// delete
	if (sCmp("_ZdlPv", name)){
		return (void *)free;
	}
	
	return 0;
}

bool flash_ready(void) {
	return !(FLASH->SR & FLASH_SR_BSY);
}

void clearMemory(const unsigned char *address){
	for (int i = 0; i < ROM_PROG_SIZE; i+= 1024){
		FLASH->CR|= FLASH_CR_PER; // Bit for clearing one page
		FLASH->AR = (unsigned int)(address+i); // Address
		FLASH->CR|= FLASH_CR_STRT; // Start flashing
		while(!flash_ready()); // Waiting
		FLASH->CR&= ~FLASH_CR_PER;
	}
}

void saveCode(const char *data, const char *address, unsigned int count){
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

bool checkHeader(VexMainHeader *header){
	if (header->mark[0] == 'V' && header->mark[1] == 'E' && header->mark[2] == 'E' && header->mark[3] == 'X'){
		if (header->romSize + header->codeSize > ROM_PROG_SIZE || header->ramSize > RAM_SIZE){
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

bool debugInternal(){
	if (checkHeader((VexMainHeader*)ROM_ADDRESS_PROG)){
		FileWorker *fileWorker = FSWriteFile("/debug.vex");
		if (!fileWorker) return false;
		if (!FSWrite(fileWorker, (char*)ROM_ADDRESS_PROG, ROM_PROG_SIZE)) return false;
		FSClose(fileWorker);
		return true;
	}
	return false;
}

// Needs around 6 kb
bool loadGame(char *path, char *ramBuffer){
	VexMainHeader header;
	VexSubHeader subHeader;
	VexCodeSliceHeader codeSliceHeader;
	
	unsigned int totalBlockSize, relCount, cmd, jmpFrom;
	unsigned short p1, p2, ramPredefinedSize = 0;
	unsigned int p, inRamShift;
	char *name;
	VexCodeRelocation *verRel;
	FileWorker *fileWorker = FSReadFile(path);
	if (fileWorker){
		FSRead(fileWorker, &header, sizeof(VexMainHeader));

		// Check header and is system suitable to running this file
		if (checkHeader(&header)){
			// Unlock memory to write
			unlockMemory();
			
			// Clear memory
			clearMemory((const unsigned char *)ROM_ADDRESS_PROG);
			
			const char *romRomShift = (const char *)ROM_ADDRESS_PROG+header.codeSize;
			
			// Main loop of sections
			while(1){
				FSRead(fileWorker, &subHeader, sizeof(VexSubHeader));
				switch(subHeader.type){
					case VEX_BLOCK_TYPE_CODE_PART:
						// Code blocks
						for (int i = 0; i < subHeader.size; i++){
							// Code block information
							FSRead(fileWorker, &codeSliceHeader, sizeof(VexCodeSliceHeader));
							totalBlockSize = codeSliceHeader.codeLength + codeSliceHeader.symNameTableLength + (codeSliceHeader.relocationsCount*sizeof(VexCodeRelocation));
							relCount = codeSliceHeader.relocationsCount;
							
							FSRead(fileWorker, ramBuffer, totalBlockSize);
							
							for (int rel = 0; rel < relCount; rel++){
								verRel = (VexCodeRelocation *)(ramBuffer + codeSliceHeader.codeLength + codeSliceHeader.symNameTableLength + rel*sizeof(VexCodeRelocation));
								inRamShift = verRel->shift - codeSliceHeader.globalShift;
								
								switch(verRel->type){
									case VEX_BLOCK_TYPE_MAP:
									case VEX_REL_TYPE_CODE:
										if (verRel->source != 1){
											FSClose(fileWorker);
											return false;
										}
										
										if (verRel->bind == 1){
											if (verRel->type == VEX_BLOCK_TYPE_MAP){
												cmd = (unsigned int)((const unsigned char *)ROM_ADDRESS_PROG + verRel->targetShift) + 1;
											} else {
												name = (char*)ramBuffer + codeSliceHeader.codeLength + verRel->nameShift;
												cmd = (unsigned int)getCmd(name);
												if (!cmd){
													FSClose(fileWorker);
													return false;
												}
											}
											
											if ((*((int*)(&ramBuffer[inRamShift]))) == 0){
												*((int*)(&ramBuffer[inRamShift])) = cmd;
											}else{
												jmpFrom = (unsigned int)((const unsigned char *)ROM_ADDRESS_PROG + codeSliceHeader.globalShift + inRamShift);
												p = ((int)cmd - (int)jmpFrom - 4) & 0x7FFFFF;
												p1 = (p >> 12) & 0x07ff;
												p2 = (p >> 1) & 0x07ff;
												
												*((short int*)(&ramBuffer[inRamShift])) = (*((short int*)(&ramBuffer[inRamShift])) & 0xf800) + p1;
												*((short int*)(&ramBuffer[inRamShift+2])) = (*((short int*)(&ramBuffer[inRamShift+2])) & 0xf800) + p2;
											}
										}
										
										if (verRel->bind == 0){
											*((int*)(&ramBuffer[inRamShift])) = (int)(romRomShift + verRel->targetShift);
										}
									break;
											
									case VEX_REL_TYPE_ROM:
										*((int*)(&ramBuffer[inRamShift])) += (int)(romRomShift + verRel->targetShift);
									break;

									case VEX_REL_TYPE_RAM:
										*((int*)(&ramBuffer[inRamShift])) += (int)(ram + verRel->targetShift);
									break;
									
									default:
										FSClose(fileWorker);
										return false;
								}
							}
							
							saveCode(ramBuffer, (char*)((const char *)ROM_ADDRESS_PROG + codeSliceHeader.globalShift), codeSliceHeader.codeLength);
						}
					break;
						
					case VEX_BLOCK_TYPE_RODATA:
						if (subHeader.size){
							FSRead(fileWorker, ramBuffer, subHeader.size);
							saveCode(ramBuffer, romRomShift, subHeader.size);
						}
					break;
					
					case VEX_BLOCK_TYPE_RAM:
						if (subHeader.size){
							ramPredefinedSize = subHeader.size;
							FSRead(fileWorker, ram, subHeader.size);
						}
					break;
						
					case VEX_BLOCK_TYPE_END:
						//todo OwO, what this?
						memset(ram, 0, RAM_SIZE - ramPredefinedSize);
						entryPoint = (unsigned char *)ROM_ADDRESS_PROG + header.entry;		
						ramProgOccupied = header.ramSize;
						FSClose(fileWorker);
						return true;

					default:
						FSClose(fileWorker);
						return false;
				}
			}
		}
	}

	return false;
}

int runGame(){
	enableMemory(ram + ramProgOccupied, RAM_SIZE - ramProgOccupied);
	Engine::setPalette(0);
	const unsigned char *prog = entryPoint + 1;
	typedef int func(void);
	func* f = (func*)(prog);
	int b = f();
	return b;
}
