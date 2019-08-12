#pragma once

enum FileTypes{
	FILE_TYPE_UNKNOWN = 0x00,
	FILE_TYPE_RUNNABLE = 0x01,
	FILE_TYPE_DIRECTORY = 0x02
};

enum FileFlags{
	FILE_FLAGS_NONE = 0x00,
	FILE_FLAGS_HIDDEN = 0x01,
	FILE_FLAGS_SYSTEM = 0x02
};

// 28 Bytes
struct FileInfo{
	unsigned int fileSize;
	
	unsigned char modifiedSeconds;
	unsigned char modifiedMinutes;
	unsigned char modifiedHours;
	unsigned char modifiedMonth;
		
	unsigned char modifiedDay;
	unsigned short modifiedYear;
	unsigned char createSeconds;
	
	unsigned char createMinutes;
	unsigned char createHours;
	unsigned char createMonth;
	unsigned char flags;
	
	unsigned char fileType;
	unsigned char r1, r2, r3;	
	
	unsigned short createYear;
	unsigned short permissions;
	
	char *fileName;
};

// 8 Bytes + 28 Bytes
struct DirectoryReader{
	int currentCluster;
	unsigned short sectorShift;
	unsigned short inSectorShift;
	FileInfo fileInfo;
};

// 32 Bytes
struct FileWorker{
	int startCluster;
	
	int currentCluster;
	
	unsigned short sectorShift;
	unsigned short inSectorShift;
	
	unsigned int fileSector;
	
	unsigned short fileInSectorShift;
	unsigned char mode;
	unsigned char r1;
	
	unsigned int fileSize;
	
	unsigned int readed;
	
	unsigned int reserved;
};

// SD card
bool SDControlBuffer();
void SDClearBuffer();

bool SDReadSector(unsigned int sector);
bool SDWriteSector(unsigned int sector);
bool SDEnable();
void SDDisable();
bool SDIsHC();

unsigned char SDGetByte(unsigned int address);
unsigned short SDGetWord(unsigned int address);
unsigned int SDGetDWord(unsigned int address);
void SDSetByte(unsigned int address, char byte);
void SDSetWord(unsigned int address, short word);
void SDSetDWord(unsigned int address, int dword);

// Read Dir
DirectoryReader *FSReadDir(char *path);
FileInfo *FSReadNextFile(DirectoryReader *dirReader);
void FSCloseDir(DirectoryReader *dirReader);

// Files
FileWorker *FSReadFile(char *filePath);
FileWorker *FSWriteFile(const char *filePath);
unsigned int FSRead(FileWorker *fileWorker, void *dst, unsigned int length);
unsigned int FSWrite(FileWorker *fileWorker, const void *src, int length);
bool FSSeek(unsigned int shift, FileWorker *fileWorker);
void FSClose(FileWorker *fileWorker);

// File System
void FSGetNameFromFileStruct(unsigned char* fStruct, char *fileName);
int FSGetCluster(const char *filePath, FileInfo *fileInfo = 0, int *targetSector = 0, int *targetSectorShift = 0);
unsigned int FSGetStartSector(int cluster);
int FSMakeNewCluster();
void FSRemoveClusterChain(int cluster);
void FSSetClusterValue(int cluster, int value);
int FSGetClusterValue(int cluster);
void FSClearCluster(int cluster);
void FSGetEmptyFilePlace(int cluster, int *targetSector, int *targetSectorShift);
bool FSIsFree(char b);
