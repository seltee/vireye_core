#include<stm32f10x_adc.h>

#include "input.h"
#include "hardware.h"
#include "sound.h"

const uint16_t butEqPin[] =  { GPIO_Pin_4, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15, GPIO_Pin_8, GPIO_Pin_9 };
const GPIO_TypeDef * butEqPort[] = { GPIOB, GPIOC, GPIOC, GPIOC, GPIOB, GPIOB };

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
