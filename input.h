#pragma once

#include<stm32f10x.h>
#include<stm32f10x_gpio.h>

#include "display.h"

#define INPUT_PORT GPIOB

#define INPUT_SELECT GPIO_Pin_4
#define INPUT_START GPIO_Pin_5
#define INPUT_A GPIO_Pin_6
#define INPUT_X GPIO_Pin_7
#define INPUT_B GPIO_Pin_8
#define INPUT_Y GPIO_Pin_9

class Input{
	public:
		static bool getState(uint16_t button);
		static short int getXAxis();
		static short int getYAxis();
};
