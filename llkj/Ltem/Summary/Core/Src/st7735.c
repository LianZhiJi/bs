// st7735.c
#include "st7735.h"
#include "font.h" // 包含字库数据

// 私有函数声明
static void ST7735_WriteCommand(uint8_t cmd);
static void ST7735_WriteData(uint8_t data);
static void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

// 初始化命令序列（适用于ST7735S）
static const uint8_t init_cmds[] = {
    // 软件复位
    0x01, 0x80, 150,
    // 退出睡眠模式
    0x11, 0x80, 255,
    // 帧率控制
    0xB1, 3, 0x01, 0x2C, 0x2D,
    // 帧率控制
    0xB2, 3, 0x01, 0x2C, 0x2D,
    // 帧率控制
    0xB3, 6, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D,
    // 列反转
    0xB4, 1, 0x07,
    // 电源控制1
    0xC0, 3, 0xA2, 0x02, 0x84,
    // 电源控制2
    0xC1, 1, 0xC5,
    // 电源控制3
    0xC2, 2, 0x0A, 0x00,
    // 电源控制4
    0xC3, 2, 0x8A, 0x2A,
    // 电源控制5
    0xC4, 2, 0x8A, 0xEE,
    // VCOM控制
    0xC5, 1, 0x0E,
    // 内存访问控制（MX/MY/MV RGB顺序）
    0x36, 1, 0xC8,
    // 像素格式（16位）
    0x3A, 1, 0x05,
    // 显示开
    0x29, 0x80, 255
};

// 发送命令
static void ST7735_WriteCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(ST7735_DC_PORT, ST7735_DC_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_SET);
}

// 发送数据
static void ST7735_WriteData(uint8_t data) {
    HAL_GPIO_WritePin(ST7735_DC_PORT, ST7735_DC_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_SET);
}

// 硬件复位
static void ST7735_Reset(void) {
    HAL_GPIO_WritePin(ST7735_RST_PORT, ST7735_RST_PIN, GPIO_PIN_RESET);
    HAL_Delay(50);
    HAL_GPIO_WritePin(ST7735_RST_PORT, ST7735_RST_PIN, GPIO_PIN_SET);
    HAL_Delay(150);
}

// 初始化函数
void ST7735_Init(void) {
    // 初始化GPIO
    HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ST7735_DC_PORT, ST7735_DC_PIN, GPIO_PIN_SET);
    
    // 硬件复位
    ST7735_Reset();
    
    // 解析初始化命令序列
    const uint8_t *cmd = init_cmds;
    while (cmd < init_cmds + sizeof(init_cmds)) {
        uint8_t command = *cmd++;   // 读取命令字节
        uint8_t delay = 0;
        uint8_t param_len = *cmd++; // 读取参数长度
        
        // 处理特殊标记（如延迟）
        if (param_len == 0x80) {    // 0x80 表示后续为延迟时间
            delay = *cmd++;        // 读取延迟时间（单位：ms）
            param_len = *cmd++;    // 实际参数长度
        }
        
        // 发送命令
        ST7735_WriteCommand(command);
        
        // 发送参数
        for (uint8_t i = 0; i < param_len; i++) {
            ST7735_WriteData(*cmd++);
        }
        
        // 处理延迟
        if (delay) {
            HAL_Delay(delay);
        }
    }
    
    // 设置默认显示方向（可选项，若 init_cmds 中已配置可删除）
    ST7735_WriteCommand(0x36);
    ST7735_WriteData(0xC8); // RGB顺序 + 横向显示
    
    // 开启显示
    ST7735_WriteCommand(0x29);
    
    // 开启背光
    ST7735_Backlight_On();
}

// 背光开启
void ST7735_Backlight_On(void) {
    HAL_GPIO_WritePin(ST7735_BLK_PORT, ST7735_BLK_PIN, GPIO_PIN_SET);
}

// 背光关闭
void ST7735_Backlight_Off(void) {
    HAL_GPIO_WritePin(ST7735_BLK_PORT, ST7735_BLK_PIN, GPIO_PIN_RESET);
}

// 设置显示窗口
static void ST7735_SetAddressWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    ST7735_WriteCommand(0x2A); // 列地址设置
    ST7735_WriteData(0x00);
    ST7735_WriteData(x0 + 2);  // X偏移（根据屏幕型号调整）
    ST7735_WriteData(0x00);
    ST7735_WriteData(x1 + 2);

    ST7735_WriteCommand(0x2B); // 行地址设置
    ST7735_WriteData(0x00);
    ST7735_WriteData(y0 + 3);  // Y偏移
    ST7735_WriteData(0x00);
    ST7735_WriteData(y1 + 3);

    ST7735_WriteCommand(0x2C); // 内存写入
}

// 绘制像素
void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color) {
    if(x >= ST7735_WIDTH || y >= ST7735_HEIGHT) return;
    
    ST7735_SetAddressWindow(x, y, x, y);
    
    uint8_t data[2] = {color >> 8, color & 0xFF};
    HAL_GPIO_WritePin(ST7735_DC_PORT, ST7735_DC_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, data, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_SET);
}

// 填充屏幕
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

// 显示字符串（需要字库支持）
void ST7735_DrawString(uint8_t x, uint8_t y, const char *str, uint16_t color) {
    while (*str) {
        // ASCII字符（单字节）
        if ((*str & 0x80) == 0) { 
            // 原有ASCII绘制逻辑（略）
        } 
        // 汉字（双字节GB2312编码）
        else {
            if (x + 12 >= ST7735_WIDTH) { // 换行
                x = 0;
                y += 12;
                if (y + 12 >= ST7735_HEIGHT) break;
            }
            
            // 查找汉字在字库中的索引
            uint8_t found = 0;
            for (uint8_t i = 0; i < sizeof(tfont12)/sizeof(typFNT_GB12); i++) {
                if (str[0] == tfont12[i].Index[0] && str[1] == tfont12[i].Index[1]) {
                    // 绘制12x12汉字
                    const uint8_t *data = tfont12[i].Msk;
                    for (uint8_t row = 0; row < 12; row++) { // 12行
                        uint16_t line = (data[row*2] << 8) | data[row*2+1]; // 合并两个字节
                        for (uint8_t col = 0; col < 12; col++) { // 12列
                            if (line & (0x8000 >> col)) { // 从高位到低位扫描
                                ST7735_DrawPixel(x + col, y + row, color);
                            }
                        }
                    }
                    found = 1;
                    break;
                }
            }
            
            if (found) {
                x += 12 + 2; // 汉字宽度 + 间距
                str += 2;    // 跳过双字节
            } else {
                str += 2;    // 未找到字库，跳过
            }
        }
    }
}