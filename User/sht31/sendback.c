/**
  ******************************************************************************
  * @file    sendback.c
  * @brief   I2C Send Back (LPC845模拟I2C)
  ******************************************************************************
  */
#include "stm32f1xx_hal.h"
#include "i2c.h"
#include "sendback.h"
#include "util.h"
#include <stdio.h>

// 接收缓存
uint8_t I2C_sendBuf[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
uint8_t I2C_recvBuf[10] = {0};

/**
 * @brief 获取 SHT31 传感器数据
 * 
 * @param sht31Data 获取的数据
 * @return int8_t  0: OK, other: error
 */
int8_t i2c_send_data()
{
  HAL_StatusTypeDef status = HAL_OK;
  status = HAL_I2C_Master_Transmit(&hi2c1, SENDBACK_I2C_ADDR_WRITE, I2C_sendBuf, sizeof(I2C_sendBuf), 0xFFFF);
  if (status != HAL_OK)
  {
    HAL_I2C_StateTypeDef error_status = HAL_I2C_GetState(&hi2c1);
    printf("i2c_send_data err1:%02x -- err2:%02x\r\n", status, error_status);
    return 1;
  }
  else
  {
    printf("i2c_send_data ok\r\n");
  }

  return 0;
}


// 主机接收回调函数
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  int n = sizeof I2C_recvBuf << 1;
  char hexstr[n + 1];
  btox(hexstr, I2C_recvBuf, n);
  hexstr[n] = 0;
  printf("I2C Master Receive: %s\r\n", hexstr);

  // 再次打开 I2C 中断接收
  HAL_I2C_Master_Receive_IT(&hi2c1, SENDBACK_I2C_ADDR_READ, I2C_recvBuf, sizeof(I2C_recvBuf));
}