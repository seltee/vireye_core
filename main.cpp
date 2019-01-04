#include "display_ili9341.h"
#include "engine.h"
#include "input.h"
#include "text.h"
#include "helpers.h"
#include "hardware.h"
#include "sprites.h"

int main(void)
{
	// Initial position of player
	int positionX = 160-8;
	int positionY = 120-8;
	
	// Other parameters
	int counter = 0;
	int spaceMove = 0;
	short int bulletX = 0;
	short int bulletY = 0;
	bool bulletAlive = false;

	// Our space (space is a matrix of sprites)
	const unsigned char *spaceMatrix[90];
	for (int i = 0; i < 90; i++){
		spaceMatrix[i] = space[i % 2];
	}
	
	// Init all of the cpu hardware. Note: display has it's own hardware initialization
	if (initHardware()){	
		// initialization
		Engine engine;
		Display_ILI9341 display;
		engine.init();
		display.init(&engine);

		// Game cycle
		while(1)
    {			
			// Display space
			// See engine.h for all available drawing functions
			spaceMove++;
			engine.displaySpriteMatrix(spaceMatrix, 16, 0, (spaceMove % 32) - 32, 10, 9);
		
			// Update player position
			positionX += Input::getXAxis()>>8;
			positionY -= Input::getYAxis()>>8;
						
			// Shoot bullet
			if (Input::getState(INPUT_A) && !bulletAlive){
				bulletX = positionX + 3;
				bulletY = positionY;
				bulletAlive = true;
			}

			// Draw player
			counter++;
			engine.displaySprite(shipSprite[counter%2], positionX, positionY, 8, 8);
						
			// Draw bullet
			if (bulletAlive){
				bulletY -= 6;
				bulletAlive = bulletY < 0 ? false : true;
				engine.displaySprite(bulletSprite, bulletX, bulletY, 4, 4);
			}
			
			// Draw axis information
			char string[16];
			itoa(Input::getXAxis(), string);
			Text::displayString(&engine, string, 4, 0, 0);
			itoa(Input::getYAxis(), string);
			Text::displayString(&engine, string, 4, 0, 24);
			itoa(display.getFPS(), string);
			Text::displayString(&engine, string, 4, 0, 48);
			
			// Show current cpu frequency
			RCC_ClocksTypeDef RCC_Clocks;
			RCC_GetClocksFreq(&RCC_Clocks);	
			itoa(RCC_Clocks.SYSCLK_Frequency, string);
			Text::displayString(&engine, "SYSCLK", 4, 0, 220);
			Text::displayString(&engine, string, 4, 100, 220);
			
			// Draw sprites on real screen
			display.draw();
    }
	}
}
