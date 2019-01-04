#pragma once
#include "engine.h"

class Text{
	public:
		static void displayString(Engine *engine, char *string, uint8_t color, uint16_t x, uint16_t y, bool upscale = true);
};
