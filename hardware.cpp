#include<stm32f10x.h>
#include<stm32f10x_rcc.h>
#include<stm32f10x_gpio.h>
#include<stm32f10x_dma.h>
#include<stm32f10x_adc.h>

#include "hardware.h"
#include "input.h"
#include "sdcard.h"

unsigned int tmrCounter = 0;

bool initHardware(){
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	// Clock all the peripherals
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	// Enable ADC system clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
	
	// Clock for ADC
  RCC_ADCCLKConfig (RCC_PCLK2_Div8);
	
	// Clock for DMA
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	// Remap config pf PB3
	//GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);
	
	// GPIO for input
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin_8 |  GPIO_Pin_12 |  GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin_13 |  GPIO_Pin_14 |  GPIO_Pin_15;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//GPIO for sound
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
		
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
			
	//MS timer
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	
	// Setting up timer for 1000 ms
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM2->PSC = RCC_Clocks.SYSCLK_Frequency / 10000 - 1;
	TIM2->ARR = 10;
	TIM2->DIER |= TIM_DIER_UIE;
	TIM2->CR1 |= TIM_CR1_CEN;
	NVIC_EnableIRQ(TIM2_IRQn);
	
	// SPI initializing
	SPI_InitTypeDef SPI_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	
	// Spi1 - Display
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_5;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
		
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; 
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; 
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
	
	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1, ENABLE);
			
	// Spi2	- Flash
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, ENABLE);
	
	
	return true;
}

short int getAdcValueForXAxis(){
	return ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1) - 2048;
}

short int getAdcValueForYAxis(){
	return ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_2) - 2048;
}

unsigned int getTimer(){
	return tmrCounter;
}

unsigned int getTimerWithClear(){
	unsigned int ret = tmrCounter;
	tmrCounter = 0;
	return ret;
}

void clearTimer(){
	tmrCounter = 0;
}

extern "C" void TIM2_IRQHandler(void)
{
  TIM2->SR &= ~TIM_SR_UIF;
	tmrCounter++;
}








