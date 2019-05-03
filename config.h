#pragma once

#define CFG_FILE "/core.cfg"

struct CoreConfigHeader{
	short int coreConfigVersion;
	short int size;
};

struct CoreConfig{
	unsigned char invertAxisX;
	unsigned char invertAxisY;
	unsigned char u[3];
};

bool readConfig(CoreConfig *config, unsigned int cfgSize);
void configInit();

bool getConfig(CoreConfig *config, unsigned int cfgSize);
bool applyConfig(CoreConfig *config, unsigned int cfgSize);
bool saveConfig();

extern CoreConfig sysConfig;
