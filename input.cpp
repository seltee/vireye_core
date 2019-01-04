#include<stm32f10x_adc.h>

#include "input.h"
#include "hardware.h"

bool Input::getState(uint16_t button){
	return GPIO_ReadInputDataBit(INPUT_PORT, button) ? false : true;
}

short int Input::getXAxis(){
	return (getAdcValueForXAxis() + getAdcValueForXAxis() + getAdcValueForXAxis() + getAdcValueForXAxis()
					+ getAdcValueForXAxis() + getAdcValueForXAxis() + getAdcValueForXAxis() + getAdcValueForXAxis()) >> 3;
}
short int Input::getYAxis(){
	return (getAdcValueForYAxis() + getAdcValueForYAxis() + getAdcValueForYAxis() + getAdcValueForYAxis()
					+ getAdcValueForYAxis() + getAdcValueForYAxis() + getAdcValueForYAxis() + getAdcValueForYAxis()) >> 3;
}
