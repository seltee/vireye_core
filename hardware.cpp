#include<stm32f10x.h>
#include<stm32f10x_rcc.h>
#include<stm32f10x_gpio.h>
#include<stm32f10x_dma.h>
#include<stm32f10x_adc.h>

#include "hardware.h"
#include "input.h"

bool initHardware(){
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	//clock all the peripherals
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	//enable ADC system clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
	//clock for ADC
  RCC_ADCCLKConfig (RCC_PCLK2_Div8);
	
	//port C
	//why actually we need this?
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_9 ;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//GPIO for input
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin   = INPUT_START | INPUT_A | INPUT_B | INPUT_X | INPUT_Y;
	GPIO_Init(INPUT_PORT, &GPIO_InitStructure);
		
	//ADC for analog sticks
  //Config
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;  // we work in continuous sampling mode
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 2;
	ADC_Init ( ADC1, &ADC_InitStructure);
	
	ADC_InjectedSequencerLengthConfig(ADC1, 2);
  ADC_InjectedChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_7Cycles5);
  ADC_InjectedChannelConfig(ADC1, ADC_Channel_9, 2, ADC_SampleTime_7Cycles5);
 
  ADC_ExternalTrigInjectedConvConfig( ADC1, ADC_ExternalTrigInjecConv_None );
	ADC_Cmd(ADC1 , ENABLE ) ;
 
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
  
	ADC_AutoInjectedConvCmd( ADC1, ENABLE );
	ADC_SoftwareStartInjectedConvCmd ( ADC1 , ENABLE ) ;
	
	return true;
}

short int getAdcValueForXAxis(){
	return ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1) - 2048;
}

short int getAdcValueForYAxis(){
	return ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_2) - 2048;
}










