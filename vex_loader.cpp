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

#define FLASH_KEY1 ((uint32_t)0x45670123)
#define FLASH_KEY2 ((uint32_t)0xCDEF89AB)

extern Display_ILI9341 display;

void displaySync(){
	display.draw();
}

unsigned short getFPS(){
	return display.getFPS();
}

void setFPS(unsigned short limit){
	display.setFPS(limit);
}

void *getCmd(char *name){
	// Drawing
	if (cmp("cr.dSetLineClear", name)){
		return (void *)Engine::setLineClear;
	}
	if (cmp("cr.dSetFillColor", name)){
		return (void *)Engine::setFillColor;
	}
	if (cmp("cr.dSetSpriteMemory", name)){
		return (void *)Engine::setSpriteMemory;
	}
	if (cmp("cr.dDisplaySprite", name)){
		return (void *)Engine::displaySprite;
	}
	if (cmp("cr.dDisplaySpriteMask", name)){
		return (void *)Engine::displaySpriteMask;
	}
	if (cmp("cr.dDisplaySpriteMatrix", name)){
		return (void *)Engine::displaySpriteMatrix;
	}
	if (cmp("cr.dDisplayText", name)){
		return (void *)Text::displayString;
	}
	if (cmp("cr.dSync", name)){
		return (void *)displaySync;
	}
	if (cmp("cr.dGetFPS", name)){
		return (void *)getFPS;
	}
	if (cmp("cr.dSetFPS", name)){
		return (void *)setFPS;
	}
	if (cmp("cr.dSetPalette", name)){
		return (void *)Engine::setPalette;
	}
	
	// Input
	if (cmp("cr.iGetState", name)){
		return (void *)Input::getState;
	}
	if (cmp("cr.iGetXAxis", name)){
		return (void *)Input::getXAxis;
	}
	if (cmp("cr.iGetYAxis", name)){
		return (void *)Input::getYAxis;
	}
	
	// Helpers
	if (cmp("cr.hCmp", name)){
		return (void *)cmp;
	}
	if (cmp("cr.hItoa", name)){
		return (void *)itoa;
	}
	if (cmp("cr.hMemcpy", name)){
		return (void *)memcpy;
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

bool loadGame(){
	const unsigned char *reader = fileRom;
	unsigned char *partData = ram+(8*1024);
	char *name;
	unsigned int relCount, cmd;
	unsigned int totalBlockSize;
	unsigned short p1, p2;
	unsigned int p;
		
	//unlock memory to write
	FLASH->KEYR = FLASH_KEY1;
	FLASH->KEYR = FLASH_KEY2;
	
	// clear memory for game
	clearMemory(gameRom, 24*1024);
	
	// Starting the program
	VexMainHeader header;
	VexSubHeader subHeader;
	VexCodeSliceHeader codeSliceHeader;


	memcpy(&header, reader, sizeof(VexMainHeader));			
	Terminal::sendString("PROGRAM");
	Terminal::sendNumber(header.maxCodeBlockSize);
	Terminal::sendNumber(header.ramSize);
	Terminal::sendNumber(header.codeSize);
	Terminal::sendNumber(header.romSize);
	Terminal::sendNumber(header.entry);

	reader += sizeof(VexMainHeader);
	
	const unsigned char *romRomShift = gameRom+header.codeSize;
	unsigned int totalRomSize = header.codeSize + header.romSize;
	unsigned int jmpFrom;
	
	if (totalRomSize > 24*1024){
		Terminal::sendString("Nt en rm");
		return false;
	}
	
	if (header.ramSize > RAM_SIZE){
		Terminal::sendString("Nt en rm");
		return false;
	}
	
	if (header.entry != 0){
		Terminal::sendString("Entry nt zr");
		return false;
	}
	
	//Note: using terminal may broke your code, because they may change data stored in after loading ram section
	
	bool end = false;
	while(!end){
		memcpy(&subHeader, reader, sizeof(VexSubHeader));			
		reader += sizeof(VexSubHeader);
		//Terminal::sendString("SH");

		switch(subHeader.type){
			case VEX_BLOCK_TYPE_CODE_PART:
				for (int i = 0; i < subHeader.size; i++){
					memcpy(&codeSliceHeader, reader, sizeof(VexCodeSliceHeader));
					reader += sizeof(VexCodeSliceHeader);
					totalBlockSize = codeSliceHeader.codeLength + codeSliceHeader.symNameTableLength + (codeSliceHeader.relocationsCount*sizeof(VexCodeRelocation));
					relCount = codeSliceHeader.relocationsCount;
					
					//Terminal::sendString("SLICE\n");
					//Terminal::sendNumber(i);
					//Terminal::sendNumber(codeSliceHeader.globalShift);
					//Terminal::sendNumber(codeSliceHeader.codeLength);
					//Terminal::sendNumber(codeSliceHeader.symNameTableLength);
					//Terminal::sendNumber(codeSliceHeader.relocationsCount);
					//Terminal::sendNumber(totalBlockSize);
					
					memcpy(partData, reader, totalBlockSize);
					reader += totalBlockSize;
					
					for (int rel = 0; rel < relCount; rel++){
						VexCodeRelocation *verRel = (VexCodeRelocation *)(partData + codeSliceHeader.codeLength + codeSliceHeader.symNameTableLength + rel*sizeof(VexCodeRelocation));
						Terminal::sendString("RL\n");

						switch(verRel->type){
							case VEX_REL_TYPE_CODE:
								//Terminal::sendNumber(verRel->shift);
								//Terminal::sendNumber(verRel->targetShift);
								//Terminal::sendNumber(verRel->bind);
								//Terminal::sendNumber(verRel->source);
								//Terminal::sendNumber(*((int*)(&partData[verRel->shift])), true);
							
								if (verRel->source != 1){
									Terminal::sendString("Wr bnd src");
									return false;
								}
								
								if (verRel->bind == 1){
									name = (char*)partData + codeSliceHeader.codeLength + verRel->nameShift;
									cmd = (unsigned int)getCmd(name);
									//Terminal::sendString(name);
									
									if (!cmd){
										Terminal::sendString("Unk cmd");
										return false;
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
								Terminal::sendString("ROM\n");
								Terminal::sendNumber(verRel->shift);
								Terminal::sendNumber(verRel->targetShift);
								//Terminal::sendNumber(verRel->bind);
								//Terminal::sendNumber(verRel->source);

								*((int*)(&partData[verRel->shift])) = (int)(romRomShift + verRel->targetShift);
							break;

							case VEX_REL_TYPE_RAM:
								//Terminal::sendString("RAM\n");
								//Terminal::sendNumber(verRel->shift);
								//Terminal::sendNumber(verRel->targetShift);
							
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
				return true;

			default:
				Terminal::sendString("Unk bl tp\n");
				Terminal::sendNumber(subHeader.type);
				end = true;
		}
	}
	return false;
	return true;
}

int runGame(){
	const unsigned char *prog = gameRom + 1;
	typedef int func(void);
	func* f = (func*)(prog);
	int b = f();
	Terminal::sendNumber(b);
	return 0;
}
