/**
  ******************************************************************************
  * @file    uart.c
  * @brief   串口通讯
  ******************************************************************************
  */
#include "usart.h"
#include "uart.h"
#include <stdio.h>

// 接收缓存
uint8_t rxBuffer[20];
uint8_t rxBuffer2[20];

/**
  * @brief Rx Transfer completed callbacks
  * @param uart handle
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);

  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UART_RxCpltCallback can be implemented in the user file
   */
  if (huart->Instance == USART1)
  {
    // printf("UART1 received message!\r\n");
    HAL_UART_Transmit_IT(&huart1, rxBuffer, 15);
    HAL_UART_Receive_IT(&huart1, (uint8_t *)rxBuffer, 15);
  }
  if (huart->Instance == USART2)
  {
    HAL_UART_Transmit_IT(&huart2, rxBuffer, 15);
    HAL_UART_Receive_IT(&huart1, (uint8_t *)rxBuffer, 15);
  }
}