// st7735.h
#ifndef ST7735_H
#define ST7735_H

#include "main.h"
#include "spi.h"

// 引脚定义
#define ST7735_CS_PIN   GPIO_PIN_3
#define ST7735_CS_PORT  GPIOA
#define ST7735_DC_PIN   GPIO_PIN_2
#define ST7735_DC_PORT  GPIOA
#define ST7735_RST_PIN  GPIO_PIN_1
#define ST7735_RST_PORT GPIOA
#define ST7735_BLK_PIN  GPIO_PIN_4
#define ST7735_BLK_PORT GPIOA

// 屏幕尺寸（根据实际型号修改）
#define ST7735_WIDTH  128
#define ST7735_HEIGHT 160

// 颜色定义（RGB565）
#define ST7735_BLACK   0x0000
#define ST7735_WHITE   0xFFFF
#define ST7735_RED     0xF800
#define ST7735_GREEN   0x07E0
#define ST7735_BLUE    0x001F

void ST7735_Init(void);
void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void ST7735_FillScreen(uint16_t color);
void ST7735_DrawString(uint8_t x, uint8_t y, const char *str, uint16_t color);
void ST7735_Backlight_On(void);
void ST7735_Backlight_Off(void);

#endif