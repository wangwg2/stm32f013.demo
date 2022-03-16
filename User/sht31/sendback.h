/**
  ******************************************************************************
  * @file    sendback.h
  * @brief   I2C Send Back (LPC845模拟I2C)
  ******************************************************************************
  * @attention
  *
  * 请确保接线正确
  ******************************************************************************
  */
#ifndef __SENDBACK_H_
#define __SENDBACK_H_

/*! CPP guard */
#ifdef __cplusplus
extern "C"
{
#endif

// I2C SENDBACK Device ADDRESS
#define SENDBACK_I2C_ADDR 0x7E
#define SENDBACK_I2C_ADDR_WRITE (SENDBACK_I2C_ADDR << 1) | 0x00
#define SENDBACK_I2C_ADDR_READ  (SENDBACK_I2C_ADDR << 1) + 1

// 收发缓存
extern uint8_t I2C_sendBuf[10];
extern uint8_t I2C_recvBuf[10];

int8_t i2c_send_data();

#ifdef __cplusplus
}
#endif
#endif /* __SENDBACK_H_ */