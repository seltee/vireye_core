#include "display.h"

unsigned short int Display::getWidth(){
	return 0;
}

unsigned short int Display::getHeight(){
	return 0;
}

unsigned short int Display::getNativeWidth(){
	return 0;
}

unsigned short int Display::getNativeHeight(){
	return 0;
}

bool Display::setDimentions(){
	return false;
}

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