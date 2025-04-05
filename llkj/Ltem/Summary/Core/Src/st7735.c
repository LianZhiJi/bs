#include "st7735.h"
#include "spi.h" // Assuming there's an SPI driver

void ST7735_Init(void) {
    // Initialization sequence for the ST7735
    ST7735_SendCommand(ST7735_SWRESET); // Software reset
    HAL_Delay(150); // Delay 150 ms
    ST7735_SendCommand(ST7735_SLPOUT);  // Sleep out
    HAL_Delay(500); // Delay 500 ms
    // More initialization commands...
}

void ST7735_SendCommand(uint8_t cmd) {
    // Function to send a command to the ST7735
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); // Set DC pin to low for command
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); // Set DC pin to high for data
}

void ST7735_SendData(uint8_t data) {
    // Function to send data to the ST7735
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); // Set DC pin to high for data
    HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
}

void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    // Function to set the address window
    ST7735_SendCommand(ST7735_CASET); // Column address set
    uint8_t data[] = {0x00, x0, 0x00, x1};
    ST7735_SendData(data, sizeof(data));
    ST7735_SendCommand(ST7735_RASET); // Row address set
    data[1] = y0;
    data[3] = y1;
    ST7735_SendData(data, sizeof(data));
    ST7735_SendCommand(ST7735_RAMWR); // Write to RAM
}

void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    // Function to draw a pixel
    ST7735_SetAddressWindow(x, y, x+1, y+1);
    uint8_t data[] = {color >> 8, color & 0xFF};
    ST7735_SendData(data, sizeof(data));
}

void ST7735_FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    // Function to fill a rectangle
    ST7735_SetAddressWindow(x, y, x+w-1, y+h-1);
    uint8_t data[] = {color >> 8, color & 0xFF};
    for (uint32_t i = 0; i < w*h; i++) {
        ST7735_SendData(data, sizeof(data));
    }
}