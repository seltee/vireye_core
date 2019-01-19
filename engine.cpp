#include "engine.h"
#include "string.h"
#include "helpers.h"
#include "terminal.h"

struct SpriteCash *spriteCash = 0;
unsigned char spriteCount = 0;
unsigned char fillColor = 0x00;
unsigned short maxSprites;
bool lineClear = true;

unsigned short *colorPallete = 0;

#define RED             0x07e0   
#define GREEN           0x001f   
#define BLUE            0xf800
#define GREY						0x518C
#define YELLOW					0xe0ff
#define CIAN            0x975E
#define PURPLE					0x9093


const unsigned short defColorPallete[] = {
	0x0000, 0x0000, 0xffff, RED, GREEN, BLUE, GREY, YELLOW, CIAN, PURPLE
};

void Engine::setPalette(const unsigned short *colors){
	colorPallete = (unsigned short *)colors;
}	

void Engine::parseLine(int16_t lineNum, uint16_t* cLine){
	SpriteCash *sprTarget;
	Matrix *matrix;
	const uint8_t *spriteBytes;
	uint16_t actualWidth, displayWidth;
	uint16_t actualMatrixWidth, actualMatrixLine;
	uint16_t actualLine;
	int32_t targetPixel;
	int16_t widthIterator, pixelIterator, spriteIterator;
	uint16_t color;
	int32_t spriteLineShift, matrixLineSprite;
	uint8_t byte;
	int8_t iterator;

	const uint8_t *data;
	bool upScale;
	
	if (lineClear){
		memset(cLine, fillColor, 640);
	}
	
	for (uint16_t s = 0; s < spriteCount; s++){
		sprTarget = &spriteCash[s];
		if (lineNum >= sprTarget->y && lineNum < sprTarget->y + sprTarget->height){	
			upScale = sprTarget->upScale;
			actualLine = (sprTarget->flags & SPRITE_V_MIRROR) 
			? (upScale ? ((sprTarget->height - (lineNum - sprTarget->y) - 1)>>1) : (sprTarget->height - (lineNum - sprTarget->y) - 1)) 
			: (upScale ? ((lineNum - sprTarget->y)>>1) : (lineNum - sprTarget->y));
			iterator = (sprTarget->flags & SPRITE_H_MIRROR) ? -1 : 1;
			
			targetPixel = sprTarget->x;
			
			switch(sprTarget->type){
				case SPRITE_TYPE_PALLETE:
					// Sprite with in memory
					actualWidth = upScale ? (sprTarget->width >> 1) : sprTarget->width;
				
					// Sprite data
					data = sprTarget->sprite;
				
					// Data to draw pointer
					spriteBytes = (sprTarget->flags & SPRITE_H_MIRROR) ? &data[actualLine*actualWidth + actualWidth-1] : &data[actualLine*actualWidth];
				
					if (upScale){
						// Drawing sprite with upsacle
						if (targetPixel >= 0 && targetPixel < 320-sprTarget->width){
							for (widthIterator = 0; widthIterator < actualWidth; widthIterator++){
								if (*spriteBytes){
									*(uint32_t*)(&cLine[targetPixel]) = colorPallete[*spriteBytes] + (colorPallete[*spriteBytes] << 16);
								}
								targetPixel+=2;
								spriteBytes+=iterator;
							}
						}else{
							for (widthIterator = 0; widthIterator < actualWidth; widthIterator++){
								if (*spriteBytes && targetPixel >= 0 && targetPixel < 320){
									*(uint32_t*)(&cLine[targetPixel]) = colorPallete[*spriteBytes] + (colorPallete[*spriteBytes] << 16);
								}
								targetPixel+=2;
								spriteBytes+=iterator;
							}
						}
					} else {
						// Without upscale
						if (targetPixel >= 0 && targetPixel < 320-sprTarget->width){
							for (widthIterator = 0; widthIterator < actualWidth; widthIterator++){
								if (*spriteBytes){
									*(uint16_t*)(&cLine[targetPixel]) = colorPallete[*spriteBytes];
								}
								targetPixel++;
								spriteBytes+=iterator;
							}
						}else{
							for (widthIterator = 0; widthIterator < actualWidth; widthIterator++){
								if (*spriteBytes && targetPixel >= 0 && targetPixel < 320){
									*(uint16_t*)(&cLine[targetPixel]) = colorPallete[*spriteBytes];
								}
								targetPixel++;
								spriteBytes+=iterator;
							}
						}
					}
				break;
					
				case SPRITE_TYPE_BIT_MASK:
					color = colorPallete[sprTarget->color];
					data = sprTarget->sprite;
				
					if (upScale){
						actualWidth = sprTarget->width >> 4;
						spriteLineShift = actualLine*actualWidth;

						if (targetPixel >= 0 && targetPixel < 320 - sprTarget->width){
							for (widthIterator = 0; widthIterator < actualWidth; widthIterator++){
								byte = data[spriteLineShift + widthIterator];
								for (pixelIterator = 0; pixelIterator < 8; pixelIterator++){
									if ((0x80 >> pixelIterator) & byte){
										*(uint32_t*)(&cLine[targetPixel]) = color + (color << 16);
									}
									targetPixel+=2;
								}
							}	
						} else {
							for (widthIterator = 0; widthIterator < actualWidth; widthIterator++){
								byte = data[spriteLineShift + widthIterator];
								for (pixelIterator = 0; pixelIterator < 8; pixelIterator++){
									if (targetPixel >= 0 && targetPixel < 320){
										if ((0x80 >> pixelIterator) & byte){
											*(uint32_t*)(&cLine[targetPixel]) = color + (color << 16);
										}
									}
									targetPixel+=2;
								}
							}	
						}
					} else {
						actualWidth = sprTarget->width >> 3;
						spriteLineShift = actualLine*actualWidth;

						if (targetPixel >= 0 && targetPixel < 320 - sprTarget->width){
							for (widthIterator = 0; widthIterator < actualWidth; widthIterator++){
								byte = data[spriteLineShift + widthIterator];
								for (pixelIterator = 0; pixelIterator < 8; pixelIterator++){
									if ((0x80 >> pixelIterator) & byte){
										cLine[targetPixel] = color;
									}
									targetPixel+=1;
								}
							}	
						} else {
							for (widthIterator = 0; widthIterator < actualWidth; widthIterator++){
								byte = data[spriteLineShift + widthIterator];
								for (pixelIterator = 0; pixelIterator < 8; pixelIterator++){
									if (targetPixel >= 0 && targetPixel < 320){
										if ((0x80 >> pixelIterator) & byte){
											cLine[targetPixel] = color;
										}
									}
									targetPixel+=1;
								}
							}	
						}
					}
				break;
					
				case SPRITE_TYPE_BYTE_MASK:
					// Sprite with in memory
					actualWidth = upScale ? (sprTarget->width >> 1) : sprTarget->width;
				
					// Sprite data
					data = sprTarget->sprite;
				
					// Data to draw pointer
					spriteBytes = (sprTarget->flags & SPRITE_H_MIRROR) ? &data[actualLine*actualWidth + actualWidth-1] : &data[actualLine*actualWidth];
				
					// Color of sprite
					color = colorPallete[sprTarget->color];
				
					if (upScale){
						// Drawing sprite with upsacle
						if (targetPixel >= 0 && targetPixel < 320-sprTarget->width){
							for (widthIterator = 0; widthIterator < actualWidth; widthIterator++){
								if (*spriteBytes){
									*(uint32_t*)(&cLine[targetPixel]) = color + (color << 16);
								}
								targetPixel+=2;
								spriteBytes+=iterator;
							}
						}else{
							for (widthIterator = 0; widthIterator < actualWidth; widthIterator++){
								if (*spriteBytes && targetPixel >= 0 && targetPixel < 320){
									*(uint32_t*)(&cLine[targetPixel]) = color + (color << 16);
								}
								targetPixel+=2;
								spriteBytes+=iterator;
								}
							}
						} else {
						// Without upscale
						if (targetPixel >= 0 && targetPixel < 320-sprTarget->width){
							for (widthIterator = 0; widthIterator < actualWidth; widthIterator++){
								if (*spriteBytes){
									*(uint16_t*)(&cLine[targetPixel]) = color;
								}
								targetPixel++;
								spriteBytes+=iterator;
							}
						}else{
							for (widthIterator = 0; widthIterator < actualWidth; widthIterator++){
								if (*spriteBytes && targetPixel >= 0 && targetPixel < 320){
									*(uint16_t*)(&cLine[targetPixel]) = color;
								}
								targetPixel++;
								spriteBytes+=iterator;
							}
						}
					}
					break;
					
				case SPRITE_TYPE_MATRIX:
					// sprites in row. Color here stores shift equals to size of each sprite
					actualMatrixWidth = upScale ? ((sprTarget->width >> sprTarget->color) >> 1) : (sprTarget->width >> sprTarget->color);
					// real width and height of sprite
					actualWidth = 1 << sprTarget->color;
					// line in matrix
					actualMatrixLine = actualLine / actualWidth;
					// line of sprite
					actualLine = actualLine % actualWidth;
					// line in sprite
					spriteLineShift = actualLine*actualWidth;
					// shift in matrix
					matrixLineSprite = actualMatrixLine*actualMatrixWidth;
					// real width on display
					displayWidth = actualWidth<<1;
					// our matrix
					matrix = (Matrix*)sprTarget->sprite;
				
				
					if (upScale){
						// iterating sprites in matrix
						for (spriteIterator = 0; spriteIterator < actualMatrixWidth; spriteIterator++){
							// drawing each sprite
							spriteBytes = matrix->sprites[matrix->matrix[matrixLineSprite]] + spriteLineShift;
							
							// If sprite fits in boundings - we don't need to check every pixel position
							if (targetPixel >= 0 && targetPixel < 320-displayWidth){
								for (widthIterator = 0; widthIterator < actualWidth; widthIterator++){
									if (*spriteBytes){
										color = colorPallete[*spriteBytes];
										*(uint32_t*)(&cLine[targetPixel]) = color + (color << 16);
									}
									spriteBytes++;
									targetPixel+=2;
								}
							}else{
								for (widthIterator = 0; widthIterator < actualWidth; widthIterator++){
									if (*spriteBytes && targetPixel >= 0 && targetPixel < 320){
										color = colorPallete[*spriteBytes];
										*(uint32_t*)(&cLine[targetPixel]) = color + (color << 16);
									}
									spriteBytes++;
									targetPixel+=2;
								}
							}
							
							matrixLineSprite++;
						}
					}else{
						// iterating sprites in matrix
						for (spriteIterator = 0; spriteIterator < actualMatrixWidth; spriteIterator++){
							// drawing each sprite
							spriteBytes = ((uint8_t**)sprTarget->sprite)[matrixLineSprite] + spriteLineShift;
							
							// If sprite fits in boundings - we don't need to check every pixel position
							if (targetPixel >= 0 && targetPixel < 320-displayWidth){
								for (widthIterator = 0; widthIterator < actualWidth; widthIterator++){
									if (*spriteBytes){
										*(uint16_t*)(&cLine[targetPixel]) = colorPallete[*spriteBytes];
									}
									spriteBytes++;
									targetPixel++;
								}
							}else{
								for (widthIterator = 0; widthIterator < actualWidth; widthIterator++){
									if (*spriteBytes && targetPixel >= 0 && targetPixel < 320){
										*(uint16_t*)(&cLine[targetPixel]) = colorPallete[*spriteBytes];
									}
									spriteBytes++;
									targetPixel++;
								}
							}
							matrixLineSprite++;
						}
					}
				break;
			}
		}
	}
}

void Engine::clear(){
	spriteCount = 0;
}

void Engine::setLineClear(bool state){
	lineClear = state;
}

void Engine::setFillColor(unsigned char fillNum){
	switch(fillNum){
		case FILL_COLOR_WHITE:
			fillColor = 0xff;
			break;
		default:
			fillColor = 0x00;
			break;
	}
}

void Engine::setSpriteMemory(unsigned char *address, unsigned int size){
	maxSprites = size / sizeof(SpriteCash);
	spriteCash = (SpriteCash*)address;
	spriteCount = 0;
	colorPallete = (unsigned short *)defColorPallete;
}

void Engine::displaySprite(const uint8_t *sprite, int16_t x, int16_t y, int16_t width, int16_t height, uint8_t flags, bool upScale){
	if (spriteCount < maxSprites){
		spriteCash[spriteCount].sprite = sprite;
		spriteCash[spriteCount].type = SPRITE_TYPE_PALLETE;
		spriteCash[spriteCount].x = x;
		spriteCash[spriteCount].y = y;
		spriteCash[spriteCount].width = upScale ? (width<<1) : width;
		spriteCash[spriteCount].height = upScale ? (height<<1) : height;
		spriteCash[spriteCount].upScale = upScale;
		spriteCash[spriteCount].flags = flags;
		spriteCash[spriteCount].color = 0;
		spriteCount++;
	}
}

void Engine::displaySpriteBitMask(const uint8_t *sprite, uint8_t color, int16_t x, int16_t y, uint8_t bytesPerWidth, int16_t height, uint8_t flags, bool upScale){
	if (spriteCount < maxSprites){
		spriteCash[spriteCount].sprite = sprite;
		spriteCash[spriteCount].type = SPRITE_TYPE_BIT_MASK;
		spriteCash[spriteCount].x = x;
		spriteCash[spriteCount].y = y;
		spriteCash[spriteCount].width = upScale ? (bytesPerWidth<<4) : (bytesPerWidth<<3);
		spriteCash[spriteCount].height = upScale ? (height<<1) : (height);
		spriteCash[spriteCount].color = color;
		spriteCash[spriteCount].upScale = upScale;
		spriteCash[spriteCount].flags = flags;
		spriteCount++;
	}
}


void Engine::displaySpriteByteMask(const uint8_t *sprite, uint8_t color, int16_t x, int16_t y, int16_t width, int16_t height, uint8_t flags, bool upScale){
	if (spriteCount < maxSprites){
		spriteCash[spriteCount].sprite = sprite;
		spriteCash[spriteCount].type = SPRITE_TYPE_BYTE_MASK;
		spriteCash[spriteCount].x = x;
		spriteCash[spriteCount].y = y;
		spriteCash[spriteCount].width = upScale ? (width<<1) : width;
		spriteCash[spriteCount].height = upScale ? (height<<1) : height;
		spriteCash[spriteCount].upScale = upScale;
		spriteCash[spriteCount].flags = flags;
		spriteCash[spriteCount].color = color;
		spriteCount++;
	}
}
	

void Engine::displaySpriteMatrix(const Matrix *matrix, int8_t size, int16_t x, int16_t y, int16_t width, int16_t height, uint8_t flags, bool upScale){
	unsigned char shift = 0;
	if (spriteCount < maxSprites){
		switch(size){
			case 1:
				shift = 0;
			break;
			case 2:
				shift = 1;
			break;		
			case 4:
				shift = 2;
			break;		
			case 8:
				shift = 3;
			break;		
			case 16:
				shift = 4;
			break;	
			case 32:
				shift = 5;
			break;
			case 64:
				shift = 6;
			break;					
			default:
				return;
		}
		
		spriteCash[spriteCount].sprite = (const uint8_t*)matrix;
		spriteCash[spriteCount].type = SPRITE_TYPE_MATRIX;
		spriteCash[spriteCount].x = x;
		spriteCash[spriteCount].y = y;
		spriteCash[spriteCount].width = upScale ? ((width << shift) << 1) : (width << shift);
		spriteCash[spriteCount].height = upScale ? ((height<< shift) << 1) : (height << shift);
		spriteCash[spriteCount].upScale = upScale;
		spriteCash[spriteCount].flags = flags;
		spriteCash[spriteCount].color = shift;
		spriteCount++;
	}
}








