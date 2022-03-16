/**
  ******************************************************************************
  * @file    oled.c
  * @brief   0.96英寸七针OLED显示 (4线SPI接口)
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */
#include "oled.h"

/**
 * @brief OLED端口定义
 *
 * ----------------------------------------------------------------------------
 * GND  电源地
 * VCC  接3.3v电源
 * D0   SPI2_SCK    接PB13 串行时钟线
 * D1   SPI2_MOSI   接PB15 串行数据线
 * RES  硬复位 OLED  接PB1
 * DC   命令/数据标志 接PB14 （写0—命令/写1—数据）
 * CS   片选信号     接PB12
 */

uint8_t OLED_GRAM[128][8];

/**
 * @brief SSD1306 Command Table
 *******************************************************************************
 */
/**
 * Fundamental Command Table
 */
// set contrast control register +A[7:0]; [0x00~0xff]
#define SSD1306_SETCONTRAST 0x81
// Entire Display On [RESUME - Output follows RAM content], [Output ignores RAM content]
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
// Set Normal / Inverse Display
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
// Set Display ON/OFF
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

/**
 * Hardware Configuration Command Table
 */
// Set Display Start Line [0x40~0x7F]. Line 0 - 63 using X5X3X2X1X0
#define SSD1306_SETSTARTLINE 0x40
// Set Segment re-map [column address 0 is mapped to SEG0 (RESET)]
#define SSD1306_SEGREMAP 0xA0
// Set Segment re-map [column address 127 is mapped to SEG0]
#define SSD1306_SEGREMAP_INVERT 0xA1
// Set MUX ratio to N+1 MUX +A[5:0]; N=A[5:0]: from 16MUX to 64MUX
#define SSD1306_SETMULTIPLEX 0xA8
// Set COM Output Scan Direction [normal mode]   COM0 -> COM[N-1] (RESET)
#define SSD1306_COMSCANINC 0xC0
// Set COM Output Scan Direction [remapped mode] COM[N-1] -> COM0
#define SSD1306_COMSCANDEC 0xC8
// set display offset +A[5:0]; [0x00-0x3F]
#define SSD1306_SETDISPLAYOFFSET 0xD3
// set com pins hardware configuration A[5:4]
#define SSD1306_SETCOMPINS 0xDA
// set display clock divide ratio/oscillator frequency +A[7:0]
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
// set pre-charge period +A[7:0]
#define SSD1306_SETPRECHARGE 0xD9
// Set VCOMH Deselect Level +A[6:4]
#define SSD1306_SETVCOMDETECT 0xDB

/**
 * Addressing Setting Command Table
 */
// [00~0F] set low column address
#define SSD1306_SETLOWCOLUMN 0x00
// [10~1F] set high column address
#define SSD1306_SETHIGHCOLUMN 0x10
// Set Page Addressing Mode +A[1:0] (0x00/0x01/0x02)
//   0x00 - Horizontal Addressing Mode;
//   0x01 - Vertical Addressing Mode;
//   0x02 - Vertical Addressing Mode;
#define SSD1306_MEMORYMODE 0x20
// Set Column Address, +A[6:0]B[6:0] Setup column start and end address
#define SSD1306_COLUMNADDR 0x21
// Set Page Address, +A[2:0]B[2:0] Setup page start and end address
#define SSD1306_PAGEADDR 0x22

/**
 * Scrolling Command Table
 */
// Continuous Horizontal Scroll Setup
//    26/27 A[7:0] B[2:0] C[2:0] D[2:0] E[7:0] F[7:0]
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
// Continuous Vertical and Horizontal Scroll Setup
//    29/2A A[2:0] B[2:0] C[2:0] D[2:0] E[5:0]
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A
// Deactivate scroll
#define SSD1306_DEACTIVATE_SCROLL 0x2E
// Activate scroll
#define SSD1306_ACTIVATE_SCROLL 0x2F
// Set Vertical Scroll Area
//    A3 A[5:0] B[6:0]
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3

/**
 * Command Table for Charge Bump Setting 
 */
// Charge Pump Setting +A[7:0]
//   0x10 - Disable charge pump(RESET)
//   0x14 - Enable Charge Pump
#define SSD1306_CHARGEPUMP 0x8D

// #define SSD1306_EXTERNALVCC  0x01
// #define SSD1306_SWITCHCAPVCC 0x02
//------ SSD1306 Command Table [END] ------------------------------------------


// 写数据/命令(寄存器)
#define OLED_CMD 0
#define OLED_DATA 1

// 根据数据手册提供对应的宏定义
//-----------------------------------------------------------------------------
// 亮度
#define BRIGHTNESS 0xCF
// 宽度与长度
#define MAX_COLUMN 128
#define MAX_ROW 64
// #define PAGE 8                    // 页数
// #define MAX_CHAR_POSX X_WIDTH - 6 // 字符宽度需要-6
// #define MAX_CHAR_POSY Y_WIDTH - 6 // 字符长度需要-6


/**
 * @brief 字符表定义
 *******************************************************************************
 */
// ASCII编码6*8英文字符
const uint8_t F6x8[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // sp
  0x00, 0x00, 0x00, 0x2f, 0x00, 0x00, // !
  0x00, 0x00, 0x07, 0x00, 0x07, 0x00, // "
  0x00, 0x14, 0x7f, 0x14, 0x7f, 0x14, // #
  0x00, 0x24, 0x2a, 0x7f, 0x2a, 0x12, // $
  0x00, 0x62, 0x64, 0x08, 0x13, 0x23, // %
  0x00, 0x36, 0x49, 0x55, 0x22, 0x50, // &
  0x00, 0x00, 0x05, 0x03, 0x00, 0x00, // '
  0x00, 0x00, 0x1c, 0x22, 0x41, 0x00, // (
  0x00, 0x00, 0x41, 0x22, 0x1c, 0x00, // )
  0x00, 0x14, 0x08, 0x3E, 0x08, 0x14, // *
  0x00, 0x08, 0x08, 0x3E, 0x08, 0x08, // +
  0x00, 0x00, 0x00, 0xA0, 0x60, 0x00, // ,
  0x00, 0x08, 0x08, 0x08, 0x08, 0x08, // -
  0x00, 0x00, 0x60, 0x60, 0x00, 0x00, // .
  0x00, 0x20, 0x10, 0x08, 0x04, 0x02, // /
  0x00, 0x3E, 0x51, 0x49, 0x45, 0x3E, // 0
  0x00, 0x00, 0x42, 0x7F, 0x40, 0x00, // 1
  0x00, 0x42, 0x61, 0x51, 0x49, 0x46, // 2
  0x00, 0x21, 0x41, 0x45, 0x4B, 0x31, // 3
  0x00, 0x18, 0x14, 0x12, 0x7F, 0x10, // 4
  0x00, 0x27, 0x45, 0x45, 0x45, 0x39, // 5
  0x00, 0x3C, 0x4A, 0x49, 0x49, 0x30, // 6
  0x00, 0x01, 0x71, 0x09, 0x05, 0x03, // 7
  0x00, 0x36, 0x49, 0x49, 0x49, 0x36, // 8
  0x00, 0x06, 0x49, 0x49, 0x29, 0x1E, // 9
  0x00, 0x00, 0x36, 0x36, 0x00, 0x00, // :
  0x00, 0x00, 0x56, 0x36, 0x00, 0x00, // ;
  0x00, 0x08, 0x14, 0x22, 0x41, 0x00, // <
  0x00, 0x14, 0x14, 0x14, 0x14, 0x14, // =
  0x00, 0x00, 0x41, 0x22, 0x14, 0x08, // >
  0x00, 0x02, 0x01, 0x51, 0x09, 0x06, // ?
  0x00, 0x32, 0x49, 0x59, 0x51, 0x3E, // @
  0x00, 0x7C, 0x12, 0x11, 0x12, 0x7C, // A
  0x00, 0x7F, 0x49, 0x49, 0x49, 0x36, // B
  0x00, 0x3E, 0x41, 0x41, 0x41, 0x22, // C
  0x00, 0x7F, 0x41, 0x41, 0x22, 0x1C, // D
  0x00, 0x7F, 0x49, 0x49, 0x49, 0x41, // E
  0x00, 0x7F, 0x09, 0x09, 0x09, 0x01, // F
  0x00, 0x3E, 0x41, 0x49, 0x49, 0x7A, // G
  0x00, 0x7F, 0x08, 0x08, 0x08, 0x7F, // H
  0x00, 0x00, 0x41, 0x7F, 0x41, 0x00, // I
  0x00, 0x20, 0x40, 0x41, 0x3F, 0x01, // J
  0x00, 0x7F, 0x08, 0x14, 0x22, 0x41, // K
  0x00, 0x7F, 0x40, 0x40, 0x40, 0x40, // L
  0x00, 0x7F, 0x02, 0x0C, 0x02, 0x7F, // M
  0x00, 0x7F, 0x04, 0x08, 0x10, 0x7F, // N
  0x00, 0x3E, 0x41, 0x41, 0x41, 0x3E, // O
  0x00, 0x7F, 0x09, 0x09, 0x09, 0x06, // P
  0x00, 0x3E, 0x41, 0x51, 0x21, 0x5E, // Q
  0x00, 0x7F, 0x09, 0x19, 0x29, 0x46, // R
  0x00, 0x46, 0x49, 0x49, 0x49, 0x31, // S
  0x00, 0x01, 0x01, 0x7F, 0x01, 0x01, // T
  0x00, 0x3F, 0x40, 0x40, 0x40, 0x3F, // U
  0x00, 0x1F, 0x20, 0x40, 0x20, 0x1F, // V
  0x00, 0x3F, 0x40, 0x38, 0x40, 0x3F, // W
  0x00, 0x63, 0x14, 0x08, 0x14, 0x63, // X
  0x00, 0x07, 0x08, 0x70, 0x08, 0x07, // Y
  0x00, 0x61, 0x51, 0x49, 0x45, 0x43, // Z
  0x00, 0x00, 0x7F, 0x41, 0x41, 0x00, // [
  0x00, 0x55, 0x2A, 0x55, 0x2A, 0x55, // 55
  0x00, 0x00, 0x41, 0x41, 0x7F, 0x00, // ]
  0x00, 0x04, 0x02, 0x01, 0x02, 0x04, // ^
  0x00, 0x40, 0x40, 0x40, 0x40, 0x40, // _
  0x00, 0x00, 0x01, 0x02, 0x04, 0x00, // '
  0x00, 0x20, 0x54, 0x54, 0x54, 0x78, // a
  0x00, 0x7F, 0x48, 0x44, 0x44, 0x38, // b
  0x00, 0x38, 0x44, 0x44, 0x44, 0x20, // c
  0x00, 0x38, 0x44, 0x44, 0x48, 0x7F, // d
  0x00, 0x38, 0x54, 0x54, 0x54, 0x18, // e
  0x00, 0x08, 0x7E, 0x09, 0x01, 0x02, // f
  0x00, 0x18, 0xA4, 0xA4, 0xA4, 0x7C, // g
  0x00, 0x7F, 0x08, 0x04, 0x04, 0x78, // h
  0x00, 0x00, 0x44, 0x7D, 0x40, 0x00, // i
  0x00, 0x40, 0x80, 0x84, 0x7D, 0x00, // j
  0x00, 0x7F, 0x10, 0x28, 0x44, 0x00, // k
  0x00, 0x00, 0x41, 0x7F, 0x40, 0x00, // l
  0x00, 0x7C, 0x04, 0x18, 0x04, 0x78, // m
  0x00, 0x7C, 0x08, 0x04, 0x04, 0x78, // n
  0x00, 0x38, 0x44, 0x44, 0x44, 0x38, // o
  0x00, 0xFC, 0x24, 0x24, 0x24, 0x18, // p
  0x00, 0x18, 0x24, 0x24, 0x18, 0xFC, // q
  0x00, 0x7C, 0x08, 0x04, 0x04, 0x08, // r
  0x00, 0x48, 0x54, 0x54, 0x54, 0x20, // s
  0x00, 0x04, 0x3F, 0x44, 0x40, 0x20, // t
  0x00, 0x3C, 0x40, 0x40, 0x20, 0x7C, // u
  0x00, 0x1C, 0x20, 0x40, 0x20, 0x1C, // v
  0x00, 0x3C, 0x40, 0x30, 0x40, 0x3C, // w
  0x00, 0x44, 0x28, 0x10, 0x28, 0x44, // x
  0x00, 0x1C, 0xA0, 0xA0, 0xA0, 0x7C, // y
  0x00, 0x44, 0x64, 0x54, 0x4C, 0x44, // z
  0x14, 0x14, 0x14, 0x14, 0x14, 0x14  // horiz lines
};
//------ 字符表定义 [END] -------------------------------------------------------


// Hardware reset
void OLED_Reset()
{
  OLED_RST_Set();
  HAL_Delay(100);
  OLED_RST_Clr();
  HAL_Delay(100);
  OLED_RST_Set();
  HAL_Delay(100);
}

// 一个字节数据写入
void OLED_WrDat(uint8_t data)
{
  OLED_DC_Set();
  OLED_CS_Clr();
  HAL_SPI_Transmit(&hspi2, &data, 1, 1000);
  OLED_CS_Set();
}

// 一条指令写入
void OLED_WrCmd(uint8_t cmd)
{
  OLED_DC_Clr();
  OLED_CS_Clr();
  HAL_SPI_Transmit(&hspi2, &cmd, 1, 1000);
  OLED_CS_Set();
}

// 更新显存到OLED
void OLED_Refresh_Gram(void)
{
  uint8_t i, n;
  for (i = 0; i < 8; i++)
  {
    OLED_WrCmd(0xb0 + i); // 设置页地址（0~7）
    OLED_WrCmd(SSD1306_SETLOWCOLUMN);
    OLED_WrCmd(SSD1306_SETHIGHCOLUMN);
    for (n = 0; n < 128; n++)
      OLED_WrDat(OLED_GRAM[n][i]);
  }
}

// 开启OLED显示
void OLED_On(void)
{
  OLED_WrCmd(SSD1306_CHARGEPUMP); // 设置电荷泵命令字
  OLED_WrCmd(0X14);               // Enable Charge Pump
  OLED_WrCmd(SSD1306_DISPLAYON);  // DISPLAY ON
}

// 关闭OLED显示
void OLED_Off(void)
{
  OLED_WrCmd(SSD1306_CHARGEPUMP); // 设置电荷泵命令字
  OLED_WrCmd(0X10);               // Disable charge pump(RESET)
  OLED_WrCmd(SSD1306_DISPLAYOFF); // DISPLAY OFF
}


/**
 * @brief OLED 显示坐标设置
 * 
 * @param x 横坐标
 * @param y 纵坐标
 */
void OLED_SetPos(uint8_t x, uint8_t y)
{
  OLED_WrCmd(0xb0 + y);                   // 设置页(page)地址
  OLED_WrCmd(((x & 0xf0) >> 4) | 0x10);   // 列高四位地址
  OLED_WrCmd((x & 0x0f) | 0x01);          // 列低四位地址
}


/**
 * @brief 在指定位置显示一个字符，包括部分字符
 * 
 * @param x 0~127
 * @param y 0~6
 * @param chr 显示字符
 */
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr)
{
  uint8_t c = 0, i = 0;
  c = chr - ' ';              // 得到偏移后的值
  if (x > MAX_COLUMN - 1)
  {
    x = 0;
    y = y + 1;
  }
  OLED_SetPos(x, y + 1);
  for (i = 0; i < 6; i++)
    OLED_WrDat(*(F6x8 + c * 6 + i));
}

/**
 * @brief 在指定位置显示一个字符串
 * 
 * @param x 0~127
 * @param y 0~6
 * @param str 
 */
void OLED_ShowStr(uint8_t x, uint8_t y, char *str)
{
  uint8_t c = 0, i = 0, j = 0;

  while (*(str + j) != '\0')
  {
    c = *(str + j) - 32;
    if (x > 126)
    {
      x = 0;
      y++;
    }
    OLED_SetPos(x, y);
    for (i = 0; i < 6; i++)
    {
      OLED_WrDat(*(F6x8 + c * 6 + i));
    }
    x += 6;
    j++;
  }
}


/**
 * @brief OLED画点
 * 
 * @param x 0~127
 * @param y 0~63
 * @param t 0-清空, 1-填充 
 */
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t)
{
  uint8_t pos, bx, temp = 0;
  if (x > 127 || y > 63)
    return;
  pos = 7 - y / 8;
  bx = y % 8;
  temp = 1 << (7 - bx);
  if (t)
    OLED_GRAM[x][pos] |= temp;
  else
    OLED_GRAM[x][pos] &= ~temp;
}

/**
 * @brief OLED填充
 * 
 * @param x1 填充区域的对角坐标
 * @param y1 填充区域的对角坐标
 * @param x2 填充区域的对角坐标
 * @param y2 填充区域的对角坐标
 * @param dot 0-清空;1-填充
 * 
 * @note 确保 x1<=x2, y1<=y2, 0<=x1<=127, 0<=y1<=63
 */
void OLED_Fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot)
{
  uint8_t x, y;
  for (x = x1; x <= x2; x++)
  {
    for (y = y1; y <= y2; y++)
      OLED_DrawPoint(x, y, dot);
  }
  OLED_Refresh_Gram();
}


/**
 * @brief 清屏函数. 清完屏,整个屏幕是黑色的!和没点亮一样!!!
 * 
 */
void OLED_Clear(void)
{
  OLED_Fill(0, 0, 127, 63, 0x00);
  HAL_Delay(200);
}


/**
 * @brief OLED 初始化
 * 
 */
void OLED_Init(void)
{
  OLED_Reset();

  OLED_WrCmd(SSD1306_DISPLAYOFF);
  OLED_WrCmd(SSD1306_SETLOWCOLUMN);
  OLED_WrCmd(SSD1306_SETHIGHCOLUMN);
  OLED_WrCmd(SSD1306_SETSTARTLINE);
  OLED_WrCmd(SSD1306_SETCONTRAST);
  OLED_WrCmd(BRIGHTNESS);
  // OLED_WrCmd(SSD1306_SEGREMAP);
  // OLED_WrCmd(SSD1306_COMSCANINC);
  OLED_WrCmd(SSD1306_SEGREMAP_INVERT); // 左右反置
  OLED_WrCmd(SSD1306_COMSCANDEC);      // 上下反置
  OLED_WrCmd(SSD1306_NORMALDISPLAY);
  OLED_WrCmd(SSD1306_SETMULTIPLEX);
  OLED_WrCmd(0x3f); // 1/64 duty
  OLED_WrCmd(SSD1306_SETDISPLAYOFFSET);
  OLED_WrCmd(0x00); // not offset
  OLED_WrCmd(SSD1306_SETDISPLAYCLOCKDIV);
  OLED_WrCmd(0x80);
  OLED_WrCmd(SSD1306_SETPRECHARGE);
  OLED_WrCmd(0xf1);
  OLED_WrCmd(SSD1306_SETCOMPINS);
  OLED_WrCmd(0x12);
  OLED_WrCmd(SSD1306_SETVCOMDETECT);
  OLED_WrCmd(0x40);
  OLED_WrCmd(SSD1306_MEMORYMODE);
  OLED_WrCmd(0x02);
  OLED_WrCmd(SSD1306_CHARGEPUMP);
  OLED_WrCmd(0x14); // enable
  OLED_WrCmd(SSD1306_DISPLAYALLON_RESUME);
  OLED_WrCmd(SSD1306_NORMALDISPLAY);
  OLED_WrCmd(SSD1306_DISPLAYON);

  OLED_Clear();
}

