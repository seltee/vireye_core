#include "hardware.h"
#include "input.h"
#include "sdcard.h"

unsigned int tmrCounter = 0;

bool initHardware(){
	hInitRCC();
	hInitGPIO();
	hInitADC();
	hEnableTimer();
	hEnableInterrupts();	

	hInitSpi(H_SPI1);
	hInitSpi(H_SPI2);
	return true;
}

short int getAdcValueForXAxis(){
	return ((uint16_t)(*(__IO uint32_t*) ((uint32_t)H_ADC1 + ADC_InjectedChannel_1 + (uint8_t)0x28))) - 2048;
}

short int getAdcValueForYAxis(){
	return ((uint16_t)(*(__IO uint32_t*) ((uint32_t)H_ADC1 + ADC_InjectedChannel_2 + (uint8_t)0x28))) - 2048;
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

void hInitRCC(){
	H_RCC->APB2ENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1 | RCC_APB2Periph_ADC1;
	H_RCC->APB1ENR |= RCC_APB1Periph_SPI2 | RCC_APB1Periph_TIM2;
	H_RCC->AHBENR |= RCC_AHBPeriph_DMA1;
}

void hInitGPIO(){
	// Spi1 - Display
  H_GPIOA->CRL &= ~(uint32_t)((uint32_t)0x0f << GPIO_PIN_5_Pos | (uint32_t)0x0f << GPIO_PIN_6_Pos | (uint32_t)0x0f << GPIO_PIN_7_Pos);
  H_GPIOA->CRL |=  (uint32_t)((uint32_t)0x0b << GPIO_PIN_5_Pos | (uint32_t)0x0b << GPIO_PIN_6_Pos | (uint32_t)0x0b << GPIO_PIN_7_Pos);
	
	// Spi2 - Flash
	H_GPIOB->CRH &= ~(uint32_t)((uint32_t)0x0f << GPIO_PIN_13_Pos | (uint32_t)0x0f << GPIO_PIN_15_Pos);
  H_GPIOB->CRH |=  (uint32_t)((uint32_t)0x0b << GPIO_PIN_13_Pos | (uint32_t)0x0b << GPIO_PIN_15_Pos);
	
	H_GPIOB->CRH &= ~(uint32_t)((uint32_t)0x0f << GPIO_PIN_14_Pos);
  H_GPIOB->CRH |=  (uint32_t)((uint32_t)0x08 << GPIO_PIN_14_Pos);
	
	// Buttons
	H_GPIOA->CRL &= ~(uint32_t)((uint32_t)0x0f << GPIO_PIN_0_Pos);
	H_GPIOA->CRL |=  (uint32_t)((uint32_t)0x08 << GPIO_PIN_0_Pos);
	
	H_GPIOA->CRH &= ~(uint32_t)((uint32_t)0x0f << GPIO_PIN_8_Pos | (uint32_t)0x0f << GPIO_PIN_12_Pos);
	H_GPIOA->CRH |=  (uint32_t)((uint32_t)0x08 << GPIO_PIN_8_Pos | (uint32_t)0x08 << GPIO_PIN_12_Pos);
	
	H_GPIOC->CRH &= ~(uint32_t)((uint32_t)0x0f << GPIO_PIN_13_Pos | (uint32_t)0x0f << GPIO_PIN_14_Pos | (uint32_t)0x0f << GPIO_PIN_15_Pos);
	H_GPIOC->CRH |=  (uint32_t)((uint32_t)0x08 << GPIO_PIN_13_Pos | (uint32_t)0x08 << GPIO_PIN_14_Pos | (uint32_t)0x08 << GPIO_PIN_15_Pos);
	
	H_GPIOA->BSRR = (0x01 << 0 | 0x01 << 8 | 0x01 << 12);
	H_GPIOC->BSRR = (0x01 << 13 | 0x01 << 14 | 0x01 << 15);
	
	// Sound
	H_GPIOB->CRL &= ~(uint32_t)((uint32_t)0x0f << GPIO_PIN_5_Pos | (uint32_t)0x0f << GPIO_PIN_6_Pos | (uint32_t)0x0f << GPIO_PIN_7_Pos);
  H_GPIOB->CRL |=  (uint32_t)((uint32_t)0x02 << GPIO_PIN_5_Pos | (uint32_t)0x02 << GPIO_PIN_6_Pos | (uint32_t)0x02 << GPIO_PIN_7_Pos);
	
	H_GPIOB->CRH &= ~(uint32_t)((uint32_t)0x0f << GPIO_PIN_8_Pos | (uint32_t)0x0f << GPIO_PIN_9_Pos | (uint32_t)0x0f << GPIO_PIN_10_Pos | (uint32_t)0x0f << GPIO_PIN_11_Pos | (uint32_t)0x0f << GPIO_PIN_12_Pos);
  H_GPIOB->CRH |=  (uint32_t)((uint32_t)0x02 << GPIO_PIN_8_Pos | (uint32_t)0x02 << GPIO_PIN_9_Pos | (uint32_t)0x02 << GPIO_PIN_10_Pos | (uint32_t)0x02 << GPIO_PIN_11_Pos | (uint32_t)0x02 << GPIO_PIN_12_Pos);
}

void hInitADC(){
	unsigned int tmpreg1 = 0, tmpreg2 = 0, tmpreg3 = 0;
	
	H_RCC->CFGR &= ~RCC_CFGR_ADCPRE;
	H_RCC->CFGR |= RCC_CFGR_ADCPRE_DIV8;
	
	H_ADC1->CR1 = (0x01 << 8);
	H_ADC1->CR2 = 0x000E0000 | 0x01 << 1;

  H_ADC1->JSQR = (2 - 1) << 20;
		
	//Channel 8
  tmpreg1 = H_ADC1->SMPR2;
  tmpreg2 = 0x00000007 << (3 * 0x08);
  tmpreg1 &= ~tmpreg2;
  tmpreg2 = (uint32_t)ADC_SampleTime_7Cycles5 << (3 * 0x08);
  tmpreg1 |= tmpreg2;
  H_ADC1->SMPR2 = tmpreg1;
	
	tmpreg1 = H_ADC1->JSQR;
  tmpreg3 =  (tmpreg1 & (uint32_t)0x00300000)>> 20;
  tmpreg2 = (uint32_t)0x0000001F << (5 * (uint8_t)((1 + 3) - (tmpreg3 + 1)));
  tmpreg1 &= ~tmpreg2;
  tmpreg2 = (uint32_t)0x08 << (5 * (uint8_t)((1 + 3) - (tmpreg3 + 1)));
  tmpreg1 |= tmpreg2;
  H_ADC1->JSQR = tmpreg1;
	
	//Channel 9
  tmpreg1 = H_ADC1->SMPR2;
  tmpreg2 = 0x00000007 << (3 * 0x09);
  tmpreg1 &= ~tmpreg2;
  tmpreg2 = (uint32_t)ADC_SampleTime_7Cycles5 << (3 * 0x09);
  tmpreg1 |= tmpreg2;
  H_ADC1->SMPR2 = tmpreg1;
	
	tmpreg1 = H_ADC1->JSQR;
  tmpreg3 =  (tmpreg1 & (uint32_t)0x00300000)>> 20;
  tmpreg2 = (uint32_t)0x0000001F << (5 * (uint8_t)((2 + 3) - (tmpreg3 + 1)));
  tmpreg1 &= ~tmpreg2;
  tmpreg2 = (uint32_t)0x09 << (5 * (uint8_t)((2 + 3) - (tmpreg3 + 1)));
  tmpreg1 |= tmpreg2;
  H_ADC1->JSQR = tmpreg1;
	
  H_ADC1->CR2 |= ADC_ExternalTrigInjecConv_None;
	H_ADC1->CR2 |= ADC_CR2_ADON;

	// Wait for init
	H_ADC1->CR2 |= ADC_CR2_RSTCAL;
	while ((H_ADC1->CR2 & ADC_CR2_RSTCAL) == ADC_CR2_RSTCAL);
	H_ADC1->CR2 |= ADC_CR2_CAL;
	while ((H_ADC1->CR2 & ADC_CR2_RSTCAL) == ADC_CR2_CAL);

	// Adc start
	H_ADC1->CR1 |= (uint32_t)ADC_CR1_JAUTO;
	H_ADC1->CR2 |= 0x00208000;
}

void hInitSpi(SPI_Def *spi){
	spi->CR1 = 0<<SPI_CR1_DFF_Pos  	//8 bit
    | 0<<SPI_CR1_LSBFIRST_Pos     //MSB first
    | 1<<SPI_CR1_SSM_Pos          //program SS
    | 1<<SPI_CR1_SSI_Pos          //SS to 1
    | 0x00<<SPI_CR1_BR_Pos        //Rate/2
    | 1<<SPI_CR1_MSTR_Pos         //Master
    | 0<<SPI_CR1_CPOL_Pos 
		| 0<<SPI_CR1_CPHA_Pos
		| 1<<SPI_CR1_SPE_Pos;
}

void hEnableTimer(){
	// Setting up timer for 1000 ms
	H_TIM2->PSC = hGetClock() / 10000 - 1;
	H_TIM2->ARR = 10;
	H_TIM2->DIER |= TIM_DIER_UIE;
	H_TIM2->CR1 |= TIM_CR1_CEN;
}

void hEnableInterrupts(){
	H_NVIC->ISER[0U] = (uint32_t)(1UL << (TIM2_IRQn & 0x1FUL));
}

unsigned int hGetClock(){
	return 128000000;
}

extern "C" void TIM2_IRQHandler(void)
{
  H_TIM2->SR &= ~TIM_SR_UIF;
	tmrCounter++;
}








