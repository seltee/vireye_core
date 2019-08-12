#include "mem.h"
#include "string.h"
#include "sdcard.h"
#include "engine.h"

#define BLOCK_SIZE 16
#define PARTITION_SIZE (BLOCK_SIZE*4 + 1)

char ram[RAM_SIZE];
char *mem = 0;
unsigned short blocksNum;
	
void enableMemory(char *memory, unsigned short size){
	Engine::disableGraphics();
	mem = memory;
	blocksNum = size / PARTITION_SIZE;
	memset(mem, 0, blocksNum * PARTITION_SIZE);
	SDClearBuffer();
}

void disableMemory(){
	mem = 0;
}

char *malloc(unsigned int size){
	unsigned short inBlocksSize = size / BLOCK_SIZE + ((size % BLOCK_SIZE) != 0 ? 1 : 0);
	unsigned short counter = 0, i, j, n;

	if (mem && size > 0){
		for (i = 0; i < blocksNum; i++){
			if ((mem[i] & 0x55) != 0x55){
				for (j = 0; j < 4; j++){
					if ((mem[i] & (0x01 << (j * 2))) == 0){
						counter++;
						if (counter == inBlocksSize){
							for (n = 0; n < inBlocksSize; n++){
								mem[i] |= ((n == 0) ? 0x03 : 0x01) << (j * 2);
								if (n != inBlocksSize-1){
									j--;
									if (j > 4){
										j = 3;
										i--;
									}
								}
							}
							return mem + blocksNum + (i * 4 + j) * BLOCK_SIZE;
						}
					} else {
						counter = 0;
					}
				}
			} else {
				counter = 0;
			}
		}
	}
	return 0;
}

void free(void *memory){
	unsigned short block, subBlock, subShift;
	char bc;
	if (mem && memory){
		block = ((int)memory - (int)mem - blocksNum) / BLOCK_SIZE;
		subBlock = block / 4;
		subShift = (block % 4) * 2;
		while(true){
			bc = (mem[subBlock] >> subShift) & 0x03;
			if (bc == 0) break;
			mem[subBlock] &= ~(0x03 << subShift);
			if (bc == 0x03) break;
			subShift+=2;
			if (subShift == 8){
				subShift = 0;
				subBlock++;
			}
		}
	}
}

unsigned int getFreeMem(){
	unsigned short i, j, counter = 0;
	if (mem){
		for (i = 0; i < blocksNum; i++){
			for (j = 0; j < 4; j++){
				if ((mem[i] & (0x01 << (j << 1))) == 0){
					counter += BLOCK_SIZE;
				}
			}
		}
	}
	return counter;
}






