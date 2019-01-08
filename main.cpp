#include "display_ili9341.h"
#include "engine.h"
#include "input.h"
#include "text.h"
#include "helpers.h"
#include "hardware.h"
#include "mem.h"
#include "vex_loader.h"
#include "terminal.h"
#include <string.h>

Display_ILI9341 display;

int main(void)
{
	// Init all of the cpu hardware. Note: display has it's own hardware initialization
	if (initHardware()){
		// Initialization
		display.init();
		Terminal::setMemory(ram);
		Engine::setSpriteMemory(ram+(4*1024), (4*1024));

		// Check if there is program in memory by file id
		if (fileRom[0] == 'V' && fileRom[1] == 'E' && fileRom[2] == 'E' && fileRom[3] == 'X'){
			if (loadGame()){
				runGame();
			} else {
				Terminal::sendString("Unable to load\n");
			}
			// Draw terminal infinetelly if error
			while(1){
				Terminal::draw();
				display.draw();
			}
		} else {
			short c = 0, dir = 1;
			while(1){
				c+=dir;
				if (c > 200) dir = -1;
				if (c < 0) dir = 1;
				Text::displayString("No program in memory", 4, 10, 10 + c, false);
				Text::displayString("Use patcher to link program", 4, 10, 26 + c, false);
				display.draw();
			}
		}
	}
	while(1);
}
