#pragma once

#include "display.h"

#define PIN_LCD_LED GPIO_Pin_1
#define PIN_LCD_DC GPIO_Pin_2
#define PIN_LCD_RESET GPIO_Pin_3
#define PIN_LCD_CS GPIO_Pin_4

class Display_ILI9341 : public Display{
public:
	virtual unsigned short int getWidth();
	virtual unsigned short int getHeight();
	virtual unsigned short int getNativeWidth();
	virtual unsigned short int getNativeHeight();
	virtual bool setDimentions();
	virtual unsigned char getFPS();

	virtual void init(Engine *engine);
	virtual void draw();
private:
	void sendCMD(uint8_t cmd);
	void writeData8(uint8_t data);
	void writeData16(uint16_t data);
	
	void displayClear(uint8_t color);

	void initDMA(uint16_t *cLine);
	void initTimer();

	void setCol(uint16_t startCol, uint16_t endCol);
	void setPage(uint16_t startPage, uint16_t endPage);
	void setXY(uint16_t positionX, uint16_t positionY);
	void setPixel(uint16_t positionX, uint16_t positionY, uint8_t color);
	void setOrientation(uint8_t orient);
};
