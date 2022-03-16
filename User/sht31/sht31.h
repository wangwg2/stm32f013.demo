/**
  ******************************************************************************
  * @file    sht31.h
  * @brief   SHT31 传感器 (LPC845模拟SHT31温度传感器)
  ******************************************************************************
  * @attention
  *
  * 请确保接线正确
  ******************************************************************************
  */
#ifndef __SHT31_H_
#define __SHT31_H_

/*! CPP guard */
#ifdef __cplusplus
extern "C"
{
#endif

// SHT31 ADDRESS
#define SHT31_ADDRESS 0x44
#define SHT31_ADDR_WRITE (SHT31_ADDRESS << 1) | 0x00
#define SHT31_ADDR_READ ((SHT31_ADDRESS << 1) + 1) 

typedef enum
{
  // 软件复位命令
  SOFT_RESET_CMD = 0x30A2,
  // Break周期性采集模式
  BREAK_CMD = 0x3093,
  // 读/清除 状态寄存器
  READ_REG_CMD = 0xF32D,
  CLEAN_REG_CMD = 0x3041,

  // 单次测量模式
  // 命名格式：Repeatability_CS_CMD (CS:Clock stretching)
  HIGH_ENABLED_CMD = 0x2C06,
  MEDIUM_ENABLED_CMD = 0x2C0D,
  LOW_ENABLED_CMD = 0x2C10,
  HIGH_DISABLED_CMD = 0x2400,
  MEDIUM_DISABLED_CMD = 0x240B,
  LOW_DISABLED_CMD = 0x2416,

  // 周期测量模式
  // 命名格式：Repeatability_MPS_CMD (MPS:次数/秒)
  HIGH_0_5_CMD = 0x2032,
  MEDIUM_0_5_CMD = 0x2024,
  LOW_0_5_CMD = 0x202F,
  HIGH_1_CMD = 0x2130,
  MEDIUM_1_CMD = 0x2126,
  LOW_1_CMD = 0x212D,
  HIGH_2_CMD = 0x2236,
  MEDIUM_2_CMD = 0x2220,
  LOW_2_CMD = 0x222B,
  HIGH_4_CMD = 0x2334,
  MEDIUM_4_CMD = 0x2322,
  LOW_4_CMD = 0x2329,
  HIGH_10_CMD = 0x2737,
  MEDIUM_10_CMD = 0x2721,
  LOW_10_CMD = 0x272A,
  // 周期测量模式读取数据命令
  READOUT_FOR_PERIODIC_MODE = 0xE000,
} SHT31_CMD;

typedef struct __SHT31_DATA
{
  int16_t temperature;
  uint16_t humidity;
} SHT31_DATA;

// DATE PROCESS 将原始量转化为物理量
#define SHT3X_TC(data)	(175 * (float)data / 65535 - 45)  // 摄氏度处理
#define SHT3X_TF(data)  (315 * (float)data / 65535 - 49)  // 华氏度处理
#define SHT3X_RH(data)  (100 * (float)data / 65535)       // 相对湿度处理

// SHT31 控制
uint8_t sht31_send_cmd(SHT31_CMD cmd);
void sht31_reset(void);
uint8_t sht31_init(void);
uint8_t sht31_read_data(uint8_t *data);
void sht31_parse_data(SHT31_DATA *sht31_data, const uint8_t *data);

/**
 * @brief 获取 SHT31 传感器数据
 * 
 * @param data 获取的数据
 * @return int8_t  0 OK, -1 error
 */
int8_t sht31_get_data(SHT31_DATA *sht31_data);

#ifdef __cplusplus
}
#endif
#endif /* __SHT31_H_ */