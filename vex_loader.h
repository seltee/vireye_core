#pragma once

struct VexMainHeader{
	unsigned char mark[4];
	unsigned char version;
	unsigned char subVersion;
	unsigned char architecture;
	unsigned char r1;
	unsigned int maxCodeBlockSize;
	unsigned int ramSize;
	unsigned int codeSize;
	unsigned int romSize;
	unsigned int entry;
};

struct VexSubHeader {
	unsigned char type;
	unsigned char version;
	unsigned short headerSize;
	unsigned int size;
};

struct VexCodeSliceHeader {
	unsigned int globalShift;
	unsigned int codeLength;
	unsigned int symNameTableLength;
	unsigned int relocationsCount;
};

struct VexCodeRelocation {
	unsigned int shift;
	unsigned short type;
	unsigned char bind;
	unsigned char source;
	unsigned int nameShift;
	unsigned int targetShift;
};

#define VEX_BLOCK_TYPE_END 0
#define VEX_BLOCK_TYPE_MAP 1
#define VEX_BLOCK_TYPE_CODE 2
#define VEX_BLOCK_TYPE_RAM 3
#define VEX_BLOCK_TYPE_RODATA 4
#define VEX_BLOCK_TYPE_SYMTABLE 5
#define VEX_BLOCK_TYPE_CODE_PART 6
#define VEX_BLOCK_TYPE_RAM_RELOCATION 7

#define VEX_REL_TYPE_UNKNOWN 0
#define VEX_REL_TYPE_ROM 2
#define VEX_REL_TYPE_RAM 3
#define VEX_REL_TYPE_CODE 4

bool loadGameInternal();
bool loadGame(char *path, unsigned char *ramBuffer);
int runGame();





