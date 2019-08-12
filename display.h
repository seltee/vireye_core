#pragma once
#include <stm32f10x.h>
#include "engine.h"

class Display{
	public:
		virtual unsigned short getFPS();
		virtual void setFPS(unsigned short limit);
		virtual void init();
		virtual void draw();
	
	protected:
		unsigned short fpsLimit;
};
