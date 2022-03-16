/**
  ******************************************************************************
  * @file    key.c
  * @brief   按键处理
  ******************************************************************************
  */
#include "key.h"


/**
 * @brief 按键扫描
 * 
 * @param GPIOx 
 * @param GPIO_Pin 
 * @return uint8_t 按键按下或无按键
 */
uint8_t Key_Scan(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
  // 检测是否有按键按下
  if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_RESET)
  {
    // 等待按键释放
    while (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_SET)
      ;
    return KEY_ON;
  }
  else
    return KEY_OFF;
}
