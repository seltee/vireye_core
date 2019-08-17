#pragma once
#include <stdint.h>

extern bool playSounds;

bool enableSoundMono(unsigned short freq, unsigned char bitPerSample, uint16_t (*callback)(void));
void disableSound();
