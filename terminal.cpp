#include <string.h>

#include "config.h"
#include "terminal.h"
#include "text.h"
#include "helpers.h"
#include "engine.h"
#include "display_ili9341.h"
#include "mem.h"

extern Display_ILI9341 display;

//terminal need 1600 bytes + sizeof(TerminalInfo)
char *termAddress = 0;
const char conv[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

void Terminal::initTerminal(){
	termAddress = malloc(1600 + sizeof(TerminalInfo));
	clearMemory();
}

void Terminal::disableTerminal(){
	if (termAddress){
		free(termAddress);
		termAddress = 0;
	}
}

void Terminal::clearMemory(){
	TerminalInfo *termInfo = (TerminalInfo*)termAddress;
	termInfo->currentLine = 0;
	for (int i = 0; i < 40*20; i++){
		termAddress[i+sizeof(TerminalInfo)] = ' ';
	}
}

void Terminal::scroll(){
	for (int i = 0; i < 19; i++){
		memcpy(termAddress+sizeof(TerminalInfo)+i*40, termAddress+sizeof(TerminalInfo)+(i+1)*40, 40);
	}
	memset(termAddress+sizeof(TerminalInfo)+19*40, ' ', 40);
}

void Terminal::draw(){
	TerminalInfo *termInfo = (TerminalInfo*)termAddress;
	for (int i = termInfo->currentLine; i >= 0; i--){
		char string[41];
		memcpy(string, termAddress+sizeof(TerminalInfo)+i*40, 40);
		string[40] = 0;
		Text::displayString(string, 4, 0, i*12, false);
	}
}

void Terminal::sendString(const char *string, bool update){
	if (termAddress){
		int stringSize = strlen(string) < 40 ? strlen(string) : 40;
		TerminalInfo *termInfo = (TerminalInfo*)termAddress;
		
		if (termInfo->currentLine == 19){
			for (int i = 0; i < 40; i++){
				if ((termAddress+sizeof(TerminalInfo)+19*40)[i] != ' '){
					scroll();
					break;
				}
			}
		}
		
		memcpy(termAddress+sizeof(TerminalInfo)+termInfo->currentLine*40, string, stringSize);
		if (termInfo->currentLine < 19){
			termInfo->currentLine++;
		}
		
		if (update){
			Terminal::draw();
			display.draw();
		}
	}
}

void Terminal::sendNumber(int number, bool asHex, bool update){
	if (termAddress){
		char string[40];
		if (asHex){
			for(int i = 7; i >= 0; i--){
				char c = number&0x0f;
				number = number >> 4;
				string[i] = conv[c];
			}
			string[8] = 0;
		}else{
			itoa(number, string);
		}
		sendString(string);
		
		if (update){
			Terminal::draw();
			display.draw();
		}
	}
}
