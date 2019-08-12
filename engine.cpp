#include "engine.h"
#include "string.h"
#include "helpers.h"
#include "terminal.h"
#include "mem.h"

struct SpriteCash *spriteCash = 0;
unsigned char fillColor = 0x00;
unsigned short maxSprites = 0;
unsigned short spriteCount = 0;
bool lineClear = true;

const unsigned short *colorPallete = 0;

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

unsigned short Engine::getSpriteLimit(){
	return maxSprites;
}
unsigned short Engine::getSpriteCash(){
	return spriteCount;
}

void Engine::setPalette(const unsigned short *colors){
	if (colors){
		colorPallete = (unsigned short *)colors;
	}else{
		colorPallete = defColorPallete;
	}
}	

void Engine::parseLine(int16_t lineNum, uint16_t* cLine){
	SpriteCash *sprTarget;
	Matrix *matrix;
	const uint8_t *spriteBytes;
	int actualWidth, displayWidth;
	int actualMatrixWidth, actualMatrixLine;
	int actualLine;
	int targetPixel;
	int widthIterator, pixelIterator, spriteIterator;
	int spriteLineShift, matrixLineSprite;
	unsigned short color;
	char iterator;
	unsigned char byte;
	

	const unsigned char *data;
	bool upScale;
	bool hMirror;
	
	if (lineClear){
		memset(cLine, fillColor, 640);
	}
	
	for (unsigned short s = 0; s < spriteCount; s++){
		sprTarget = &spriteCash[s];
		if (lineNum >= sprTarget->y && lineNum < sprTarget->y + sprTarget->height){	
			hMirror = (sprTarget->flags & SPRITE_H_MIRROR);
			
			upScale = sprTarget->upScale;
			actualLine = (sprTarget->flags & SPRITE_V_MIRROR) 
			? (upScale ? ((sprTarget->height - (lineNum - sprTarget->y) - 1)>>1) : (sprTarget->height - (lineNum - sprTarget->y) - 1)) 
			: (upScale ? ((lineNum - sprTarget->y)>>1) : (lineNum - sprTarget->y));
			iterator = hMirror ? -1 : 1;
			
			targetPixel = sprTarget->x;
			
			switch(sprTarget->type){
				case SPRITE_TYPE_PALLETE:
					// Sprite with in memory
					actualWidth = upScale ? (sprTarget->width >> 1) : sprTarget->width;
				
					// Sprite data
					data = sprTarget->sprite;
				
					// Data to draw pointer
					spriteBytes = hMirror ? &data[actualLine*actualWidth + actualWidth-1] : &data[actualLine*actualWidth];
				
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
								byte = hMirror ? data[spriteLineShift + actualWidth - widthIterator - 1] : data[spriteLineShift + widthIterator];
								if (hMirror){
									for (pixelIterator = 7; pixelIterator >= 0; pixelIterator--){
										if ((0x80 >> pixelIterator) & byte){
											*(uint32_t*)(&cLine[targetPixel]) = color + (color << 16);
										}
										targetPixel+=2;
									}
								}else{
									for (pixelIterator = 0; pixelIterator < 8; pixelIterator++){
										if ((0x80 >> pixelIterator) & byte){
											*(uint32_t*)(&cLine[targetPixel]) = color + (color << 16);
										}
										targetPixel+=2;
									}
								}
							}	
						} else {
							for (widthIterator = 0; widthIterator < actualWidth; widthIterator++){
								byte = hMirror ? data[spriteLineShift + actualWidth - widthIterator - 1] : data[spriteLineShift + widthIterator];
								if (hMirror){
									for (pixelIterator = 7; pixelIterator >= 0; pixelIterator--){
										if (targetPixel >= 0 && targetPixel < 320){
											if ((0x80 >> pixelIterator) & byte){
												*(uint32_t*)(&cLine[targetPixel]) = color + (color << 16);
											}
										}
										targetPixel+=2;
									}
								}else{
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
						}
					} else {
						actualWidth = sprTarget->width >> 3;
						spriteLineShift = actualLine*actualWidth;

						if (targetPixel >= 0 && targetPixel < 320 - sprTarget->width){
							for (widthIterator = 0; widthIterator < actualWidth; widthIterator++){
								byte = hMirror ? data[spriteLineShift + actualWidth - widthIterator - 1] : data[spriteLineShift + widthIterator];
								if (hMirror){
									for (pixelIterator = 7; pixelIterator >= 0; pixelIterator--){
										if ((0x80 >> pixelIterator) & byte){
											cLine[targetPixel] = color;
										}
										targetPixel+=1;
									}
								}else{
									for (pixelIterator = 0; pixelIterator < 8; pixelIterator++){
										if ((0x80 >> pixelIterator) & byte){
											cLine[targetPixel] = color;
										}
										targetPixel+=1;
									}
								}
							}	
						} else {
							for (widthIterator = 0; widthIterator < actualWidth; widthIterator++){
								byte = hMirror ? data[spriteLineShift + actualWidth - widthIterator - 1] : data[spriteLineShift + widthIterator];
								if (hMirror){
									for (pixelIterator = 7; pixelIterator >= 0; pixelIterator--){
										if (targetPixel >= 0 && targetPixel < 320){
											if ((0x80 >> pixelIterator) & byte){
												cLine[targetPixel] = color;
											}
										}
										targetPixel+=1;
									}
								}else{
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
					}
				break;
					
				case SPRITE_TYPE_BYTE_MASK:
					// Sprite with in memory
					actualWidth = upScale ? (sprTarget->width >> 1) : sprTarget->width;
				
					// Sprite data
					data = sprTarget->sprite;
				
					// Data to draw pointer
					spriteBytes = hMirror ? &data[actualLine*actualWidth + actualWidth-1] : &data[actualLine*actualWidth];
				
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
					
				case SPRITE_TYPE_FILLED_RECT:
					targetPixel = sprTarget->x;
					spriteIterator = sprTarget->x + sprTarget->width;
				
					if (targetPixel < 0) targetPixel = 0;
					if (targetPixel >= 320) break;
					if (spriteIterator < 0) break;
					if (spriteIterator >= 320) spriteIterator = 320;
				
					color = colorPallete[sprTarget->color];
					
					for (; targetPixel < spriteIterator; targetPixel++){
						*(uint16_t*)(&cLine[targetPixel]) = color;
					}
					break;
				
				case SPRITE_TYPE_RECT:
					color = colorPallete[sprTarget->color];
					
					targetPixel = sprTarget->x;
					spriteIterator = sprTarget->x + sprTarget->width;

					if (upScale){
						if (lineNum == sprTarget->y || lineNum == sprTarget->y + 1 || lineNum == sprTarget->y + sprTarget->height - 1 || lineNum == sprTarget->y + sprTarget->height - 2){
							if (targetPixel < 0) targetPixel = 0;
							if (targetPixel >= 320) break;
							if (spriteIterator < 0) break;
							if (spriteIterator >= 320) spriteIterator = 320;
							
							for (; targetPixel < spriteIterator; targetPixel++){
								*(uint16_t*)(&cLine[targetPixel]) = color;
							}
						} else {
							targetPixel = sprTarget->x;
							spriteIterator = sprTarget->x + sprTarget->width - 1;
							
							if (targetPixel >= 0 && targetPixel < 320) *(uint16_t*)(&cLine[targetPixel]) = color;
							targetPixel++;
							if (targetPixel >= 0 && targetPixel < 320) *(uint16_t*)(&cLine[targetPixel]) = color;
							
							if (spriteIterator >= 0 && spriteIterator < 320) *(uint16_t*)(&cLine[spriteIterator]) = color;
							spriteIterator--;
							if (spriteIterator >= 0 && spriteIterator < 320) *(uint16_t*)(&cLine[spriteIterator]) = color;
						}
					} else {
						if (lineNum == sprTarget->y || lineNum == sprTarget->y + sprTarget->height - 1){
							if (targetPixel < 0) targetPixel = 0;
							if (targetPixel >= 320) break;
							if (spriteIterator < 0) break;
							if (spriteIterator >= 320) spriteIterator = 320;
							
							for (; targetPixel < spriteIterator; targetPixel++){
								*(uint16_t*)(&cLine[targetPixel]) = color;
							}
						} else {
							targetPixel = sprTarget->x;
							spriteIterator = sprTarget->x + sprTarget->width - 1;
							if (targetPixel >= 0 && targetPixel < 320) *(uint16_t*)(&cLine[targetPixel]) = color;
							if (spriteIterator >= 0 && spriteIterator < 320) *(uint16_t*)(&cLine[spriteIterator]) = color;
						}
						break;
					}
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

bool Engine::setSpriteLimit(unsigned short newSpriteCount){
	disableGraphics();
	if (newSpriteCount){
		spriteCash = (SpriteCash *)malloc(newSpriteCount * sizeof(SpriteCash));
		if (spriteCash){
			maxSprites = newSpriteCount;
			spriteCount = 0;
			return true;
		}
	}
	return false;
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
	if (spriteCount < maxSprites){
		unsigned char shift = 0;
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


void Engine::displayFilledRect(uint8_t color, int16_t x, int16_t y, int16_t width, int16_t height, bool upScale){
	if (spriteCount < maxSprites){
		spriteCash[spriteCount].type = SPRITE_TYPE_FILLED_RECT;
		spriteCash[spriteCount].x = x;
		spriteCash[spriteCount].y = y;
		spriteCash[spriteCount].width = upScale ? width * 2 : width;
		spriteCash[spriteCount].height = upScale ? height * 2 : height;
		spriteCash[spriteCount].upScale = upScale;
		spriteCash[spriteCount].color = color;
		spriteCash[spriteCount].flags = 0;
		spriteCount++;
	}
}

void Engine::displayRect(uint8_t color, int16_t x, int16_t y, int16_t width, int16_t height, bool upScale){
	if (spriteCount < maxSprites){
		spriteCash[spriteCount].type = SPRITE_TYPE_RECT;
		spriteCash[spriteCount].x = x;
		spriteCash[spriteCount].y = y;
		spriteCash[spriteCount].width = upScale ? width * 2 : width;
		spriteCash[spriteCount].height = upScale ? height * 2 : height;
		spriteCash[spriteCount].upScale = upScale;
		spriteCash[spriteCount].color = color;
		spriteCash[spriteCount].flags = 0;
		spriteCount++;
	}
}

void Engine::disableGraphics(){
	if (spriteCash){
		maxSprites = 0;
		spriteCount = 0;
		free(spriteCash);
		spriteCash = 0;
	}
}
