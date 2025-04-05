#ifndef ST7735_H
#define ST7735_H

#include <stdint.h>
#include <stdbool.h>

// ST7735 Commands
#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

// More ST7735 command definitions...

// ST7735 Initialization sequence
void ST7735_Init(void);

// Function to send a command to the ST7735
void ST7735_SendCommand(uint8_t cmd);

// Function to send data to the ST7735
void ST7735_SendData(uint8_t data);

// Function to set the address window
void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

// Function to draw a pixel
void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color);

// Function to fill a rectangle
void ST7735_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

#endif // ST7735_H