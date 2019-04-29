#include "sound.h"

uint16_t (*_callback)(void);
uint8_t sample = 0;
int8_t shLeft;
int8_t shRight;

bool enableSoundMono(unsigned short freq, unsigned char bitPerSample, uint16_t (*callback)(void)){
	if (freq < 1000 || freq > 44100){
		return false;
	}

	if (bitPerSample < 8){
		shRight = 8 - bitPerSample;
		shLeft = 0;
	} else {
		shLeft = bitPerSample - 8;
		shRight = 0;
	}

	_callback = callback;
	
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM3->PSC = 1000000 / freq - 1;
	TIM3->ARR = RCC_Clocks.SYSCLK_Frequency / 1000000;
	TIM3->DIER |= TIM_DIER_UIE;
	TIM3->CR1 |= TIM_CR1_CEN;
	NVIC_EnableIRQ(TIM3_IRQn);
	
	return true;
}

extern "C" void TIM3_IRQHandler(void)
{
	GPIOB->ODR = (GPIOB->ODR & 0xfffff00f) | (sample << 4);
	sample = _callback() >> shRight << shLeft;
  TIM3->SR &= ~TIM_SR_UIF;
}

void disableSound(){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, DISABLE);
	NVIC_DisableIRQ(TIM3_IRQn);
}
