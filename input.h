#pragma once

#include<stm32f10x.h>
#include<stm32f10x_gpio.h>

#include "display.h"

enum ButtonId {
	INPUT_SELECT = 0,
	INPUT_START,
	INPUT_A,
	INPUT_X,
	INPUT_B,
	INPUT_Y
};

class Input{
	public:
		static bool getState(uint8_t button);
		static short int getXAxis();
		static short int getYAxis();
};
