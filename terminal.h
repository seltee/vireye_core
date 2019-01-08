#pragma once
#include "engine.h"

struct TerminalInfo{
	unsigned int currentLine;
};

class Terminal{
	public:
	static void setMemory(unsigned char *address);
	static void scroll();
	static void draw();
	static void sendString(char *string);
	static void sendNumber(int number, bool asHex = false);
};

