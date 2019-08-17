#include "sound.h"
#include "hardware.h"

uint16_t (*_callback)(void);
uint8_t sample = 0;
int8_t shLeft;
int8_t shRight;

bool enableSoundMono(unsigned short freq, unsigned char bitPerSample, uint16_t (*callback)(void)){
	if (freq < 1000 || freq > 44100){
		return false;
	}

	if (bitPerSample < 8){
		shLeft= 8 - bitPerSample;
		shRight = 0;
	} else {
		shRight = bitPerSample - 8;
		shLeft = 0;
	}

	_callback = callback;

	hEnableSoundTim(1000000 / freq - 1, hGetClock() / 1000000);
	return true;
}

void disableSound(){
	hDisableSoundTim();
}

extern "C" void TIM3_IRQHandler(void)
{
	H_GPIOB->ODR = (H_GPIOB->ODR & 0xffffe01f) | (sample << 5);
	sample = ((_callback() >> shRight) << shLeft);
  H_TIM3->SR &= ~TIM_SR_UIF;
}

