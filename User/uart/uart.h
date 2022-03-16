/**
  ******************************************************************************
  * @file    uart.h
  * @brief   串口通讯
  ******************************************************************************
  */
#ifndef __UART_H__
#define __UART_H__

#include "usart.h"

#ifdef __cplusplus
extern "C"
{
#endif

// 接收缓存
extern uint8_t rxBuffer[20];
extern uint8_t rxBuffer2[20];

#ifdef __cplusplus
}
#endif

#endif /* __UART_H__ */