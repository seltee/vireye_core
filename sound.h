#pragma once
#include <stm32f10x.h>

extern bool playSounds;

bool enableSoundMono(unsigned short freq, unsigned char bitPerSample, uint16_t (*callback)(void));
void disableSound();
