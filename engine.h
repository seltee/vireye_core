#pragma once
#include <stm32f10x.h>

#define SPRITE_TYPE_NONE 0
#define SPRITE_TYPE_PALLETE 1
#define SPRITE_TYPE_MASK 2
#define SPRITE_TYPE_MATRIX 3

#define FILL_COLOR_BLACK 0
#define FILL_COLOR_WHITE 1

enum{
	COLOR_TYPE_UNKNOWN,
	COLOR_TYPE_RGB565 = 1
};

struct SpriteCash{
	const uint8_t *sprite;
	int16_t x;
	int16_t y;
	uint16_t width;
	uint16_t height;
	uint8_t type;
	uint8_t color;
	uint8_t upScale;
	uint8_t unused2;
};

struct Matrix{
	const unsigned char **sprites;
	unsigned char *matrix;
};

class Engine{
	public:
		static void parseLine(int16_t lineNum, uint16_t* cLine);
		static void clear();
		static void setLineClear(bool state);
		static void setFillColor(unsigned char fillNum);
		static void setSpriteMemory(unsigned char *address, unsigned int size);
		static void setPalette(const unsigned short *colors, unsigned char format, unsigned char colorCount);
			
		// this function draws single sprite
		// colors array (from pallete), sprite x position, sprite y position, sprite width, sprite height (must be exectly the same, as it's exist in colors array)
		static void displaySprite(const uint8_t *sprite, int16_t x, int16_t y, int16_t width, int16_t height, bool upScale = true);
	
		// this function draws sprite by mask with selected color
		// mask data, color for each 1 bit in data, sprite x position, sprite y position, width byte count (each byte is 8 pixels), sprite height (same as in display sprite), upscale *2
		static void displaySpriteMask(const uint8_t *sprite, uint8_t color, int16_t x, int16_t y, uint8_t bytesPerWidth, int16_t height, bool upScale = true);
	
		// this function draws matrix of sprites
		// sprites data (each value is a pointer to sprites colors array, size of each sprite(width and height must be same), x position, y position, amount of sprites in row, amount of sprites in coll	
		static void displaySpriteMatrix(const Matrix *matrix, int8_t size, int16_t x, int16_t y, int16_t width, int16_t height, bool upScale = true);
};
