#include<stm32f10x_adc.h>

#include "input.h"
#include "hardware.h"
#include "sound.h"

const uint16_t butEqPin[] =  { GPIO_Pin_8, GPIO_Pin_12, GPIO_Pin_14, GPIO_Pin_15, GPIO_Pin_13, GPIO_Pin_0 };
const GPIO_TypeDef * butEqPort[] = { GPIOA, GPIOA, GPIOC, GPIOC, GPIOC, GPIOA };

bool Input::getState(uint8_t button){
	if (button >= 0 && button <= 5){
		return (butEqPort[button]->IDR & butEqPin[button]) == 0;
	}
	return false;
}

short int Input::getXAxis(){
	int counter = 0;
	for (int i = 0; i < 8; i++){
		counter += getAdcValueForXAxis();
	}
	return counter >> 3;
}
short int Input::getYAxis(){
	int counter = 0;
	for (int i = 0; i < 8; i++){
		counter += getAdcValueForYAxis();
	}
	return counter >> 3;
}
