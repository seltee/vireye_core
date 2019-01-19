#include <string.h>
#include "display_ili9341.h"
#include "engine.h"
#include "input.h"
#include "text.h"
#include "helpers.h"
#include "hardware.h"
#include "mem.h"
#include "vex_loader.h"
#include "terminal.h"
#include "sdcard.h"

Display_ILI9341 display;

void endlessRun(){
	while(1){
		Terminal::draw();
		display.draw();
	}
}

int main(void)
{
	// Init all of the cpu hardware. Note: display has it's own hardware initialization
	if (initHardware()){
		// Initialization
		display.init();
		Terminal::setMemory(ram);
		Engine::setSpriteMemory(ram+(2*1024), (4*1024));

		// Enable SD
		bool SDStatus = SDEnable(ram+(8*1024));

		// Try to run from internal memory
		if (loadGameInternal()){
			runGame();
		} else {
			// Try to run from SD Card
			if (SDStatus && loadGame("/autorun.vex", ram+(9*1024))){
				runGame();
			} else {
				display.setFPS(30);
				short c = 0, dir = 1;
				while(1){
					c+=dir;
					if (c > 160) dir = -1;
					if (c < 0) dir = 1;
					Text::displayString("No program in memory", 4, 10, 10 + c, false);
					Text::displayString("Use patcher to link program", 4, 10, 26 + c, false);
					Text::displayString("Or put autorun.vex on SD card", 4, 10, 42 + c, false);
					Text::displayString("Thank you for using Vireye", 4, 10, 58 + c, false);
					display.draw();
				}
			}
			Terminal::sendString("Unable to load");
		}
	}
	endlessRun();
}
