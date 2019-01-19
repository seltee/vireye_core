#include<stm32f10x.h>
#include<stm32f10x_rcc.h>
#include<stm32f10x_gpio.h>
#include<stm32f10x_spi.h>
#include<stm32f10x_dma.h>

#include "display_ili9341.h"
#include "helpers.h"

unsigned short int line1[321];
unsigned short int line2[321];
bool sendLineDone;
unsigned short frameCounter = 0;
unsigned short frameRate = 0;
bool allowFrame = false;

bool Display_ILI9341::setDimentions(){
	return true;
}

unsigned short Display_ILI9341::getFPS(){
	return frameRate;
}

void Display_ILI9341::setFPS(unsigned short limit){
	fpsLimit = limit;
	initFPSTimer(limit);
}

void Display_ILI9341::init(){
	sendLineDone = false;
	fpsLimit = 0;

	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	

	// GPIO
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin   = PIN_LCD_LED | PIN_LCD_DC | PIN_LCD_RESET | PIN_LCD_CS;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
		
	// Spi1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_5;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
		
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; 
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; 
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
	
	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1, ENABLE);
	
	// Display comands
	GPIO_WriteBit(GPIOA,PIN_LCD_CS,Bit_SET);		
	GPIO_WriteBit(GPIOA,PIN_LCD_RESET,Bit_RESET);			
	GPIO_WriteBit(GPIOA,PIN_LCD_RESET,Bit_SET);	
	
	delayByLoop(30000);
	sendCMD(0x01);        // Software Reset
	delayByLoop(30000);
	
	sendCMD(0xCB);
	writeData8(0x39);
	writeData8(0x2C);
	writeData8(0x00);
	writeData8(0x34);
	writeData8(0x02);

	sendCMD(0xCF);
	writeData8(0x00);
	writeData8(0xC1);
	writeData8(0x30);

	sendCMD(0xE8);
	writeData8(0x85);
	writeData8(0x00);
	writeData8(0x78);

	sendCMD(0xEA);
	writeData8(0x00);
	writeData8(0x00);

	sendCMD(0xED);
	writeData8(0x64);
	writeData8(0x03);
	writeData8(0x12);
	writeData8(0x81);

	sendCMD(0xF7);
	writeData8(0x20);

	sendCMD(0xC0);      // Power control
	writeData8(0x25);     // VRH[5:0]

	sendCMD(0xC1);      // Power control
	writeData8(0x10);     // SAP[2:0];BT[3:0]

	sendCMD(0xC5);      			// VCM control
	writeData8(0x3e);     // Contrast
	writeData8(0x28);

	sendCMD(0xC7);      // VCM control2
	writeData8(0x86);

	sendCMD(0x36);      // Memory Access Control
	writeData8(0x48);

	sendCMD(0x3A); 
	writeData8(0x55); 

	sendCMD(0xB1); 
	writeData8(0x00); 
	writeData8(0x18); 

	sendCMD(0xB6);      // Display Function Control
	writeData8(0x08); 
	writeData8(0x82); 
	writeData8(0x27); 
	
	sendCMD(0x11);      // Exit Sleep
	sendCMD(0x29);    // Display on

	//3gamma function disable
	sendCMD(0xF2);
	writeData8(0x00);

	//gamma curve selected
	sendCMD(0x26);
	writeData8(0x01);

	//set positive gamma correction
	sendCMD(0xE0);
	writeData8(0x0F);
	writeData8(0x31);
	writeData8(0x2B);
	writeData8(0x0C);
	writeData8(0x0E);
	writeData8(0x08);
	writeData8(0x4E);
	writeData8(0xF1);
	writeData8(0x37);
	writeData8(0x07);
	writeData8(0x10);
	writeData8(0x03);
	writeData8(0x0E);
	writeData8(0x09);
	writeData8(0x00);

	//set negative gamma correction
	sendCMD(0xE1);
	writeData8(0x00);
	writeData8(0x0E);
	writeData8(0x14);
	writeData8(0x03);
	writeData8(0x11);
	writeData8(0x07);
	writeData8(0x31);
	writeData8(0xC1);
	writeData8(0x48);
	writeData8(0x08);
	writeData8(0x0F);
	writeData8(0x0C);
	writeData8(0x31);
	writeData8(0x36);
	writeData8(0x0F);

	setOrientation(3);
	displayClear(0);
	
	initTimer();
}

void Display_ILI9341::draw(){
	uint16_t *cLine;
	
	sendCMD(0x2c);

	GPIO_WriteBit(GPIOA,PIN_LCD_DC,Bit_SET);
	GPIO_WriteBit(GPIOA,PIN_LCD_CS,Bit_RESET);
	cLine = line1;

	Engine::parseLine(0, cLine);
	for(uint16_t lineNum=1; lineNum<=240; lineNum++)
	{
		sendLineDone = false;
		initDMA(cLine);
		DMA_Cmd(DMA1_Channel3, ENABLE);
		cLine = (cLine == line1 ? line2 : line1);
		Engine::parseLine(lineNum, cLine);
		while(!sendLineDone);
	}
	
	Engine::clear();
	frameCounter++;
	while(fpsLimit && !allowFrame);
	allowFrame = false;
}

void Display_ILI9341::sendCMD(uint8_t cmd){
	GPIO_WriteBit(GPIOA,PIN_LCD_CS,Bit_SET);
	GPIO_WriteBit(GPIOA,PIN_LCD_DC,Bit_RESET);
	GPIO_WriteBit(GPIOA,PIN_LCD_CS,Bit_RESET);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI1->DR = cmd;
}

void Display_ILI9341::writeData8(uint8_t data){
	GPIO_WriteBit(GPIOA,PIN_LCD_DC,Bit_SET);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI1->DR = data;
}

void Display_ILI9341::writeData16(uint16_t data){
	GPIO_WriteBit(GPIOA,PIN_LCD_DC,Bit_SET);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI1->DR = data >> 8;
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI1->DR = data & 0x00ff;
}

void Display_ILI9341::displayClear(uint8_t color){
	setCol(0, 320);
	setPage(0, 240);
	sendCMD(0x2c);
	GPIO_WriteBit(GPIOA,PIN_LCD_DC,Bit_SET);
	GPIO_WriteBit(GPIOA,PIN_LCD_CS,Bit_RESET);

	for (uint16_t i=0; i<320; i++){
		line1[i] = 0x0000;
	}
	
	for(uint16_t i=0; i<240; i++)
	{
		sendLineDone = false;
		initDMA(line1);
		DMA_Cmd(DMA1_Channel3, ENABLE);
		while(!sendLineDone);
	}
}

void Display_ILI9341::setCol(uint16_t startCol, uint16_t endCol){
	sendCMD(0x2A);
	writeData16(startCol);
	writeData16(endCol);
}

void Display_ILI9341::setPage(uint16_t startPage, uint16_t endPage){
	sendCMD(0x2B);
	writeData16(startPage);
	writeData16(endPage);
}

void Display_ILI9341::setXY(uint16_t positionX, uint16_t positionY){
	setCol(positionX, positionX);
	setPage(positionY, positionY);
	sendCMD(0x2c);
}

void Display_ILI9341::setPixel(uint16_t positionX, uint16_t positionY, uint8_t color){
	setXY(positionX, positionY);
	writeData16(color);
}

void Display_ILI9341::setOrientation(uint8_t orient)
{
	sendCMD(0x36);
	switch (orient)
	{
		case 0: writeData8(0x48);
				break;
		case 1: writeData8(0x28);
				break;
		case 2: writeData8(0x88);
				break;
		case 3: writeData8(0xE8);
				break;
	}
}

void Display_ILI9341::initDMA(uint16_t *cLine){
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	//DMA
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(SPI1->DR));
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)cLine;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = 640;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
  DMA_Init(DMA1_Channel3, &DMA_InitStructure);
	
  SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
	
  DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);
  NVIC_EnableIRQ(DMA1_Channel3_IRQn);	
}

void Display_ILI9341::initTimer(){
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	
	// Setting up 1 second timer
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	TIM3->PSC = RCC_Clocks.SYSCLK_Frequency / 10000 - 1;
	TIM3->ARR = 10000 ;
	TIM3->DIER |= TIM_DIER_UIE;
	TIM3->CR1 |= TIM_CR1_CEN;
	NVIC_EnableIRQ(TIM3_IRQn);
}

void Display_ILI9341::initFPSTimer(unsigned short limit){
	RCC_ClocksTypeDef RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	
	// Setting up timer for 1 sec / limit
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	TIM4->PSC = RCC_Clocks.SYSCLK_Frequency / 10000 - 1;
	TIM4->ARR = 10000/limit;
	TIM4->DIER |= TIM_DIER_UIE;
	TIM4->CR1 |= TIM_CR1_CEN;
	NVIC_EnableIRQ(TIM4_IRQn);
}

extern "C" void DMA1_Channel3_IRQHandler(void) {
	GPIO_WriteBit(GPIOA,PIN_LCD_LED,Bit_SET);
	DMA_Cmd(DMA1_Channel3, DISABLE);
	DMA_ClearITPendingBit(DMA1_IT_TC3);
	sendLineDone = true;
}

extern "C" void TIM3_IRQHandler(void)
{
  TIM3->SR &= ~TIM_SR_UIF;
	frameRate = frameCounter;
	frameCounter = 0;
}

extern "C" void TIM4_IRQHandler(void)
{
  TIM4->SR &= ~TIM_SR_UIF;
	allowFrame = true;
}









