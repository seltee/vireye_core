#pragma once
#include <stdint.h>
#include "hardware_f103.h"

bool initHardware();
short int getAdcValueForXAxis();
short int getAdcValueForYAxis();

unsigned int getTimer();
unsigned int getTimerWithClear();
void clearTimer();

// Initialization
void hInitRCC();
void hInitGPIO();
void hInitADC();
void hInitSpi(SPI_Def *spi);
void hEnableTimer();
void hEnableInterrupts();
void hEnableSoundTim(uint16_t psc, uint16_t arr);
void hDisableSoundTim();
unsigned int hGetClock();

