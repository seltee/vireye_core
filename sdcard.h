#pragma once

#define FS_FILE_MODE_R 0
#define FS_FILE_MODE_W 1

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

struct DirectoryReader{
	int currentCluster;
	unsigned short sectorShift;
	unsigned short inSectorShift;
};

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
};

struct FileWorker{
	int startCluster;
	int currentCluster;
	unsigned short sectorShift;
	unsigned short inSectorShift;
	unsigned int fileSize;
	char *buffer;
	unsigned int readed;
	unsigned char mode;
	unsigned char r1;
	unsigned char r2;
	unsigned char r3;
	unsigned int reserved;
};

bool SDReadSector(unsigned int sector);
bool SDEnable(unsigned char *newBuffer); //1024 byte
void SDDisable();
bool SDIsHC();

unsigned char SDGetByte(unsigned int address);
unsigned short SDGetWord(unsigned int address);
unsigned int SDGetDWord(unsigned int address);

void FSGetNameFromFileStruct(unsigned char* fStruct, char *fileName);
int FSGetCluster(char *filePath, FileInfo *fileInfo = 0);
int FSGetNextCluster(int cluster);
unsigned int FSGetStartSector(int cluster);
bool FSReadDir(char *path, DirectoryReader *dirReader);
bool FSReadNextFile(char *fileName, int fileNameLength, DirectoryReader *dirReader, FileInfo *fileInfo);
bool FSReadFile(char *filePath, FileWorker *fileWorker);
unsigned int FSRead(FileWorker *fileWorker, void *target, unsigned int length);
//unsigned int FSWrite(FileWorker *fileWorker, unsigned char *buffer, int length);
bool FSSeek(unsigned int shift, FileWorker *fileWorker);
bool FSClose(FileWorker *fileWorker);


