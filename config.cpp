#include <string.h>
#include "config.h"
#include "sdcard.h"

CoreConfig sysConfig;

bool readConfig(CoreConfig *config, unsigned int cfgSize){
	CoreConfigHeader header;
	FileWorker worker;
	if (FSReadFile(CFG_FILE, &worker)){
		FSRead(&worker, &header, sizeof(CoreConfigHeader));
		FSRead(&worker, config, header.size > cfgSize ? cfgSize : header.size);	
		return true;
	}
	return false;
}

void configInit(){
	memset(&sysConfig, 0, sizeof(CoreConfig));
	readConfig(&sysConfig, sizeof(CoreConfig));
}

bool getConfig(CoreConfig *config, unsigned int cfgSize){
	memcpy(config, &sysConfig, cfgSize < sizeof(CoreConfig) ? cfgSize : sizeof(CoreConfig));
	return true;
}

bool applyConfig(CoreConfig *config, unsigned int cfgSize){
	memcpy(&sysConfig, config, cfgSize < sizeof(CoreConfig) ? cfgSize : sizeof(CoreConfig));
	return true;
}

bool saveConfig(){
	CoreConfigHeader header;
	FileWorker worker;
	if (FSWriteFile(CFG_FILE, &worker)){
		header.coreConfigVersion = 1;
		header.size = sizeof(CoreConfig);
		FSWrite(&worker, &header, sizeof(CoreConfigHeader));
		FSWrite(&worker, &sysConfig, sizeof(CoreConfig));
		FSClose(&worker);
		return true;
	}
	return false;
}
