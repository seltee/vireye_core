#pragma once
#include "engine.h"

struct TerminalInfo{
	unsigned int currentLine;
};

class Terminal{
	public:
	static void setMemory(unsigned char *address);
	static void clearMemory();
	static void scroll();
	static void draw();
	static void sendString(const char *string, bool update = false);
	static void sendNumber(int number, bool asHex = false, bool update = false);
};

