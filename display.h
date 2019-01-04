#pragma once
#include <stm32f10x.h>
#include "engine.h"

class Display{
	public:
		virtual unsigned short int getWidth();
		virtual unsigned short int getHeight();
		virtual unsigned short int getNativeWidth();
		virtual unsigned short int getNativeHeight();
		virtual bool setDimentions();
		virtual unsigned char getFPS();
		virtual void init(Engine *engine);
		virtual void draw();
		
	protected:
		Engine *engine;
};
