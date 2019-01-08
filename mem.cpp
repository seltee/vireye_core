#include "mem.h"


unsigned char ram[RAM_SIZE];
unsigned const char *fileRom = (unsigned char *)(0x08000000 + 16*1024);
unsigned const char *gameRom = (unsigned char *)(0x08000000 + 40*1024);
