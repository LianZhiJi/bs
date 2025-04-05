// st7735.c
#include "st7735.h"
#include "font.h" // �����ֿ�����

// ˽�к�������
static void ST7735_WriteCommand(uint8_t cmd);
static void ST7735_WriteData(uint8_t data);
static void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

// ��ʼ���������У�������ST7735S��
static const uint8_t init_cmds[] = {
    // �����λ
    0x01, 0x80, 150,
    // �˳�˯��ģʽ
    0x11, 0x80, 255,
    // ֡�ʿ���
    0xB1, 3, 0x01, 0x2C, 0x2D,
    // ֡�ʿ���
    0xB2, 3, 0x01, 0x2C, 0x2D,
    // ֡�ʿ���
    0xB3, 6, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D,
    // �з�ת
    0xB4, 1, 0x07,
    // ��Դ����1
    0xC0, 3, 0xA2, 0x02, 0x84,
    // ��Դ����2
    0xC1, 1, 0xC5,
    // ��Դ����3
    0xC2, 2, 0x0A, 0x00,
    // ��Դ����4
    0xC3, 2, 0x8A, 0x2A,
    // ��Դ����5
    0xC4, 2, 0x8A, 0xEE,
    // VCOM����
    0xC5, 1, 0x0E,
    // �ڴ���ʿ��ƣ�MX/MY/MV RGB˳��
    0x36, 1, 0xC8,
    // ���ظ�ʽ��16λ��
    0x3A, 1, 0x05,
    // ��ʾ��
    0x29, 0x80, 255
};

// ��������
static void ST7735_WriteCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(ST7735_DC_PORT, ST7735_DC_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_SET);
}

// ��������
static void ST7735_WriteData(uint8_t data) {
    HAL_GPIO_WritePin(ST7735_DC_PORT, ST7735_DC_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_SET);
}

// Ӳ����λ
static void ST7735_Reset(void) {
    HAL_GPIO_WritePin(ST7735_RST_PORT, ST7735_RST_PIN, GPIO_PIN_RESET);
    HAL_Delay(50);
    HAL_GPIO_WritePin(ST7735_RST_PORT, ST7735_RST_PIN, GPIO_PIN_SET);
    HAL_Delay(150);
}

// ��ʼ������
void ST7735_Init(void) {
    // ��ʼ��GPIO
    HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ST7735_DC_PORT, ST7735_DC_PIN, GPIO_PIN_SET);
    
    // Ӳ����λ
    ST7735_Reset();
    
    // ������ʼ����������
    const uint8_t *cmd = init_cmds;
    while (cmd < init_cmds + sizeof(init_cmds)) {
        uint8_t command = *cmd++;   // ��ȡ�����ֽ�
        uint8_t delay = 0;
        uint8_t param_len = *cmd++; // ��ȡ��������
        
        // ���������ǣ����ӳ٣�
        if (param_len == 0x80) {    // 0x80 ��ʾ����Ϊ�ӳ�ʱ��
            delay = *cmd++;        // ��ȡ�ӳ�ʱ�䣨��λ��ms��
            param_len = *cmd++;    // ʵ�ʲ�������
        }
        
        // ��������
        ST7735_WriteCommand(command);
        
        // ���Ͳ���
        for (uint8_t i = 0; i < param_len; i++) {
            ST7735_WriteData(*cmd++);
        }
        
        // �����ӳ�
        if (delay) {
            HAL_Delay(delay);
        }
    }
    
    // ����Ĭ����ʾ���򣨿�ѡ��� init_cmds �������ÿ�ɾ����
    ST7735_WriteCommand(0x36);
    ST7735_WriteData(0xC8); // RGB˳�� + ������ʾ
    
    // ������ʾ
    ST7735_WriteCommand(0x29);
    
    // ��������
    ST7735_Backlight_On();
}

// ���⿪��
void ST7735_Backlight_On(void) {
    HAL_GPIO_WritePin(ST7735_BLK_PORT, ST7735_BLK_PIN, GPIO_PIN_SET);
}

// ����ر�
void ST7735_Backlight_Off(void) {
    HAL_GPIO_WritePin(ST7735_BLK_PORT, ST7735_BLK_PIN, GPIO_PIN_RESET);
}

// ������ʾ����
static void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    ST7735_WriteCommand(0x2A); // �е�ַ����
    ST7735_WriteData(0x00);
    ST7735_WriteData(x0 + 2);  // Xƫ�ƣ�������Ļ�ͺŵ�����
    ST7735_WriteData(0x00);
    ST7735_WriteData(x1 + 2);

    ST7735_WriteCommand(0x2B); // �е�ַ����
    ST7735_WriteData(0x00);
    ST7735_WriteData(y0 + 3);  // Yƫ��
    ST7735_WriteData(0x00);
    ST7735_WriteData(y1 + 3);

    ST7735_WriteCommand(0x2C); // �ڴ�д��
}

// ��������
void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if(x >= ST7735_WIDTH || y >= ST7735_HEIGHT) return;
    
    ST7735_SetAddressWindow(x, y, x, y);
    
    uint8_t data[2] = {color >> 8, color & 0xFF};
    HAL_GPIO_WritePin(ST7735_DC_PORT, ST7735_DC_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, data, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_SET);
}

// �����Ļ
void ST7735_FillScreen(uint16_t color) {
    uint8_t data[2] = {color >> 8, color & 0xFF};
    
    ST7735_SetAddressWindow(0, 0, ST7735_WIDTH-1, ST7735_HEIGHT-1);
    
    HAL_GPIO_WritePin(ST7735_DC_PORT, ST7735_DC_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_RESET);
    
    for(uint32_t i = 0; i < ST7735_WIDTH * ST7735_HEIGHT; i++) {
        HAL_SPI_Transmit(&hspi1, data, 2, HAL_MAX_DELAY);
    }
    
    HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_SET);
}

// ��ʾ�ַ�������Ҫ�ֿ�֧�֣�
void ST7735_DrawString(uint8_t x, uint8_t y, const char *str, uint16_t color) {
    while (*str) {
        // ASCII�ַ������ֽڣ�
        if ((*str & 0x80) == 0) { 
            // ԭ��ASCII�����߼����ԣ�
        } 
        // ���֣�˫�ֽ�GB2312���룩
        else {
            if (x + 12 >= ST7735_WIDTH) { // ����
                x = 0;
                y += 12;
                if (y + 12 >= ST7735_HEIGHT) break;
            }
            
            // ���Һ������ֿ��е�����
            uint8_t found = 0;
            for (uint8_t i = 0; i < sizeof(tfont12)/sizeof(typFNT_GB12); i++) {
                if (str[0] == tfont12[i].Index[0] && str[1] == tfont12[i].Index[1]) {
                    // ����12x12����
                    const uint8_t *data = tfont12[i].Msk;
                    for (uint8_t row = 0; row < 12; row++) { // 12��
                        uint16_t line = (data[row*2] << 8) | data[row*2+1]; // �ϲ������ֽ�
                        for (uint8_t col = 0; col < 12; col++) { // 12��
                            if (line & (0x8000 >> col)) { // �Ӹ�λ����λɨ��
                                ST7735_DrawPixel(x + col, y + row, color);
                            }
                        }
                    }
                    found = 1;
                    break;
                }
            }
            
            if (found) {
                x += 12 + 2; // ���ֿ�� + ���
                str += 2;    // ����˫�ֽ�
            } else {
                str += 2;    // δ�ҵ��ֿ⣬����
            }
        }
    }
}