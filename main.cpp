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
#include "sound.h"
#include "config.h"

Display_ILI9341 display;

void message(char *msg){
	Text::displayString(msg, 4, 10, 10, false);
	display.draw();
}

int main(void)
{
	// Init all of the cpu hardware. Note: display has it's own hardware initialization
	if (initHardware()){
		enableMemory((char*)(ram+(18*1024)), 1024);
		
		// Initialization
		display.init();

		// Sprites
		Engine::setSpriteLimit(16);
		Engine::setPalette(0);

		// Enable SD
		bool SDStatus = SDEnable();
		
		if (SDStatus){
			message("Init ...");
			// Load configuration
			configInit();
			
			// Try to run from internal memory
			if (SDStatus && debugInternal()){
				message("Loading ...");
				if (loadGame("/debug.vex", ram+(5*1024))){
					message("Starting ...");
					runGame();
				}else{
					message("Debug failed");
				}
			} else {
				message("Running SD ...");
				// Try to run from SD Card
				if (SDStatus && loadGame("/autorun.vex", ram+(5*1024))){
					message("Autorun ...");
					runGame();
				} else {
					display.setFPS(30);
					short c = 0, dir = 1;
					while(1){
						c+=dir;
						if (c > 160) dir = -1;
						if (c < 0) dir = 1;
						Text::displayString("No program", 4, 10, 10 + c, false);
						display.draw();
					}
				}
			}
		}else{
			message("No SD card");
		}
	}
	while(1);
}
