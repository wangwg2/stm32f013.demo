/**
  ******************************************************************************
  * @file    sht31.c
  * @brief   SHT31 传感器 (LPC845模拟SHT31温度传感器)
  ******************************************************************************
  */
#include "stm32f1xx_hal.h"
#include "i2c.h"
#include "sht31.h"
#include <stdio.h>

/**
 * @brief 获取 SHT31 传感器数据
 * 
 * @param sht31Data 获取的数据
 * @return int8_t  0 OK, -1 error
 */
int8_t sht31_get_data(SHT31_DATA *sht31_data)
{
  uint8_t data[6];

  HAL_StatusTypeDef status = HAL_OK;
  status = sht31_send_cmd(HIGH_DISABLED_CMD);
  if (status != HAL_OK)
  {
    HAL_I2C_StateTypeDef error_status = HAL_I2C_GetState(&hi2c1);
    printf("sht31_send_cmd err1:%02x -- err2:%02x\r\n", status, error_status);
    // return -1;
  }

  status = HAL_I2C_Master_Receive(&hi2c1, SHT31_ADDR_READ, data, 6, 0xffff);
  if (status != HAL_OK)
  {
    HAL_I2C_StateTypeDef error_status = HAL_I2C_GetState(&hi2c1);
    printf("HAL_I2C_Master_Receive err1:%02x -- err2:%02x\r\n", status, error_status);
    return -1;
  }

  sht31_parse_data(sht31_data, data);
  return 0;
}

/**
 * @brief 解析 SHT31 数据
 * 
 * @param sht31_data 解析结果数据
 * @param data 原始测量数据
 */
void sht31_parse_data(SHT31_DATA *sht31_data, const uint8_t *data)
{
  uint16_t t;
  t = ((uint16_t)data[0] << 8) + data[1];
  sht31_data->temperature = SHT3X_TC(t);
  t = ((uint16_t)data[3] << 8) + data[4];
  sht31_data->humidity = SHT3X_RH(t);
}

/**
 * @brief 向SHT31发送一条指令(16bit)
 * 
 * @param cmd SHT31指令（SHT31_CMD枚举定义）
 * @return uint8_t 成功返回HAL_OK
 */
uint8_t sht31_send_cmd(SHT31_CMD cmd)
{
  uint8_t cmd_buffer[2];
  cmd_buffer[0] = cmd >> 8;
  cmd_buffer[1] = cmd;
  return HAL_I2C_Master_Transmit(&hi2c1, SHT31_ADDR_WRITE, (uint8_t *)cmd_buffer, 2, 0xFFFF);
}

/**
 * @brief 复位SHT31
 * 
 */
void sht31_reset(void)
{
  sht31_send_cmd(SOFT_RESET_CMD);
  HAL_Delay(20);
}

/**
 * @brief 初始化SHT31
 * @note 周期测量模式
 * 
 * @return uint8_t 成功返回HAL_OK
 */
uint8_t sht31_init(void)
{
  return sht31_send_cmd(MEDIUM_2_CMD);
}

/**
 * @brief 从SHT31读取一次数据
 * 
 * @param data 存储读取数据的地址（6个字节数组）
 * @return uint8_t 成功返回HAL_OK
 */
uint8_t sht31_read_data(uint8_t *data)
{
  sht31_send_cmd(READOUT_FOR_PERIODIC_MODE);
  return HAL_I2C_Master_Receive(&hi2c1, SHT31_ADDR_READ, data, 6, 0xFFFF);
}