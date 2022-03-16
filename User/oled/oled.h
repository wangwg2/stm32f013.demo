/**
  ******************************************************************************
  * @file    oled.h
  * @brief   0.96英寸七针OLED显示 (4线SPI接口)
  ******************************************************************************
  * @attention
  *
  * 请确保接线正确
  ******************************************************************************
  */
#ifndef __OLED_H__
#define __OLED_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "spi.h"

/**
 * @brief OLED 控制函数
 * 
 */
#define OLED_CS_Set() HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);
#define OLED_CS_Clr() HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET);

#define OLED_DC_Set() HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET);
#define OLED_DC_Clr() HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_RESET);

#define OLED_RST_Set() HAL_GPIO_WritePin(OLED_RES_GPIO_Port, OLED_RES_Pin, GPIO_PIN_SET);
#define OLED_RST_Clr() HAL_GPIO_WritePin(OLED_RES_GPIO_Port, OLED_RES_Pin, GPIO_PIN_RESET);


void OLED_Init(void);   // OLED初始化
void OLED_On(void);     // 开启OLED显示
void OLED_Off(void);    // 关闭OLED显示
void OLED_Clear(void);  // OLED清屏
void OLED_ShowStr(uint8_t x, uint8_t y, char * str);                          // 显示字符串
void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t);                         // OLED画点
void OLED_Fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot);  // OLED填充

#ifdef __cplusplus
}
#endif

#endif /* __OLED_H__ */