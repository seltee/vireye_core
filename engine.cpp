#include "engine.h"
#include "string.h"
#include "helpers.h"

struct SpriteCash spriteCash[MAX_SPRITES];
uint8_t spriteCount;
unsigned char fillColor = 0x00;

#define RED             0x07e0   
#define GREEN           0x001f   
#define BLUE            0xf800
#define CIAN            0x975E
#define PURPLE					0x9093

const uint16_t colorPallet[] = {
	0x0000, 0x0000, 0xffff, RED, GREEN, BLUE, CIAN, PURPLE
};

bool lineClear = true;

void Engine::init(){
	spriteCount = 0;
	for (int i = 0; i < MAX_SPRITES; i++){
		spriteCash[i].type = 0;
	}
}

void Engine::parseLine(int16_t lineNum, uint16_t* cLine){
	SpriteCash *sprTarget;
	const uint8_t *spriteBytes;
	uint16_t actualWidth, displayWidth;
	uint16_t actualMatrixWidth, actualMatrixLine;
	uint16_t actualLine;
	int32_t targetPixel;
	int16_t widthIterator, pixelIterator, spriteIterator;
	uint16_t color;
	int32_t spriteLineShift, matrixLineSprite;
	uint8_t byte;
	const uint8_t *data;
	bool upScale;
	
	if (lineClear){
		memset(cLine, fillColor, 640);
	}
	
	for (uint16_t s = 0; s < spriteCount; s++){
		sprTarget = &spriteCash[s];
		if (lineNum >= sprTarget->y && lineNum < sprTarget->y + sprTarget->height){	
			upScale = sprTarget->upScale;
			actualLine = upScale ? ((lineNum - sprTarget->y)>>1) : (lineNum - sprTarget->y);
			targetPixel = sprTarget->x;
			
			switch(sprTarget->type){
				case SPRITE_TYPE_PALLETE:
					// Sprite with in memory
					actualWidth = upScale ? (sprTarget->width >> 1) : sprTarget->width;
					// Sprite data
					data = sprTarget->sprite;
					// Data to draw pointer
					spriteBytes = &data[actualLine*actualWidth];
				
					if (upScale){
						// Drawing sprite with upsacle
						if (targetPixel >= 0 && targetPixel < 320-sprTarget->width){
							for (widthIterator = 0; widthIterator < actualWidth; widthIterator++){
								if (*spriteBytes){
									color = colorPallet[*spriteBytes];
									*(uint32_t*)(&cLine[targetPixel]) = color + (color << 16);
								}
								targetPixel+=2;
								spriteBytes++;
							}
						}else{
							for (widthIterator = 0; widthIterator < actualWidth; widthIterator++){
								if (*spriteBytes && targetPixel >= 0 && targetPixel < 320){
									color = colorPallet[*spriteBytes];
									*(uint32_t*)(&cLine[targetPixel]) = color + (color << 16);
								}
								targetPixel+=2;
								spriteBytes++;
							}
						}
					} else {
						// Without upscale
						if (targetPixel >= 0 && targetPixel < 320-sprTarget->width){
							for (widthIterator = 0; widthIterator < actualWidth; widthIterator++){
								if (*spriteBytes){
									*(uint16_t*)(&cLine[targetPixel]) = colorPallet[*spriteBytes];
								}
								targetPixel++;
								spriteBytes++;
							}
						}else{
							for (widthIterator = 0; widthIterator < actualWidth; widthIterator++){
								if (*spriteBytes && targetPixel >= 0 && targetPixel < 320){
									*(uint16_t*)(&cLine[targetPixel]) = colorPallet[*spriteBytes];
								}
								targetPixel++;
								spriteBytes++;
							}
						}
					}
					
				break;
					
				case SPRITE_TYPE_MASK:
					color = colorPallet[sprTarget->color];
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
				
				
					if (upScale){
						// iterating sprites in matrix
						for (spriteIterator = 0; spriteIterator < actualMatrixWidth; spriteIterator++){
							// drawing each sprite
							spriteBytes = ((uint8_t**)sprTarget->sprite)[matrixLineSprite] + spriteLineShift;
							
							// If sprite fits in boundings - we don't need to check every pixel position
							if (targetPixel >= 0 && targetPixel < 320-displayWidth){
								for (widthIterator = 0; widthIterator < actualWidth; widthIterator++){
									if (*spriteBytes){
										color = colorPallet[*spriteBytes];
										*(uint32_t*)(&cLine[targetPixel]) = color + (color << 16);
									}
									spriteBytes++;
									targetPixel+=2;
								}
							}else{
								for (widthIterator = 0; widthIterator < actualWidth; widthIterator++){
									if (*spriteBytes && targetPixel >= 0 && targetPixel < 320){
										color = colorPallet[*spriteBytes];
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
										*(uint16_t*)(&cLine[targetPixel]) = colorPallet[*spriteBytes];
									}
									spriteBytes++;
									targetPixel++;
								}
							}else{
								for (widthIterator = 0; widthIterator < actualWidth; widthIterator++){
									if (*spriteBytes && targetPixel >= 0 && targetPixel < 320){
										*(uint16_t*)(&cLine[targetPixel]) = colorPallet[*spriteBytes];
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

void Engine::displaySprite(const uint8_t *sprite, int16_t x, int16_t y, int16_t width, int16_t height, bool upScale){
	if (spriteCount < MAX_SPRITES){
		spriteCash[spriteCount].sprite = sprite;
		spriteCash[spriteCount].type = SPRITE_TYPE_PALLETE;
		spriteCash[spriteCount].x = x;
		spriteCash[spriteCount].y = y;
		spriteCash[spriteCount].width = upScale ? (width<<1) : width;
		spriteCash[spriteCount].height = upScale ? (height<<1) : height;
		spriteCash[spriteCount].upScale = upScale;
		spriteCount++;
	}
}

void Engine::displaySpriteMask(const uint8_t *sprite, uint8_t color, int16_t x, int16_t y, uint8_t bytesPerWidth, int16_t height, bool upScale){
	if (spriteCount < MAX_SPRITES){
		spriteCash[spriteCount].sprite = sprite;
		spriteCash[spriteCount].type = SPRITE_TYPE_MASK;
		spriteCash[spriteCount].x = x;
		spriteCash[spriteCount].y = y;
		spriteCash[spriteCount].width = upScale ? (bytesPerWidth<<4) : (bytesPerWidth<<3);
		spriteCash[spriteCount].height = upScale ? (height<<1) : (height);
		spriteCash[spriteCount].color = color;
		spriteCash[spriteCount].upScale = upScale;
		spriteCount++;
	}
}

void Engine::displaySpriteMatrix(const uint8_t **data, int8_t size, int16_t x, int16_t y, int16_t width, int16_t height, bool upScale){
	unsigned char shift = 0;
	if (spriteCount < MAX_SPRITES){
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
		
		spriteCash[spriteCount].sprite = (const uint8_t*)data;
		spriteCash[spriteCount].type = SPRITE_TYPE_MATRIX;
		spriteCash[spriteCount].x = x;
		spriteCash[spriteCount].y = y;
		spriteCash[spriteCount].width = upScale ? ((width << shift) << 1) : (width << shift);
		spriteCash[spriteCount].height = upScale ? ((height<< shift) << 1) : (height << shift);
		spriteCash[spriteCount].upScale = upScale;
		
		spriteCash[spriteCount].color = shift;
		spriteCount++;
	}
}








