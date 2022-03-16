/**
  ******************************************************************************
  * @file    key.h
  * @brief   按键处理
  ******************************************************************************
  */
#ifndef __KEY_H__
#define __KEY_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "gpio.h"

#define KEY_ON 0
#define KEY_OFF 1

uint8_t Key_Scan(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

#ifdef __cplusplus
}
#endif

#endif /* __KEY_H__ */