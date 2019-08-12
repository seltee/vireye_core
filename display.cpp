#include "display.h"

void Display::init(){
	fpsLimit = 0;
}

void Display::draw(){
	
}

unsigned short Display::getFPS(){
	return 0;
}

void Display::setFPS(unsigned short limit){
	fpsLimit = limit;
}
