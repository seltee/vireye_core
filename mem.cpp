#include "mem.h"


unsigned char ram[RAM_SIZE];
const unsigned char *fileRom = (unsigned char *)(0x08000000 + 16*1024);
const unsigned char *gameRom = (unsigned char *)(0x08000000 + 40*1024);
