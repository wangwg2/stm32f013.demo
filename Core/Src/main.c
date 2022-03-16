/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "key.h"
#include "uart.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
char startMessage[] = "** UART communication based on IT **\r\n";
extern void Uart1_printf(const char *format,...);
extern void Uart2_printf(const char *format,...);
unsigned char rx_data[32] = {0};
unsigned char tx_data[32] = {0};
typedef struct
{
  uint16_t dig_T1;
  int16_t  dig_T2;
  int16_t  dig_T3;

  uint16_t dig_P1;
  int16_t  dig_P2;
  int16_t  dig_P3;
  int16_t  dig_P4;
  int16_t  dig_P5;
  int16_t  dig_P6;
  int16_t  dig_P7;
  int16_t  dig_P8;
  int16_t  dig_P9;

  uint8_t  dig_H1;
  int16_t  dig_H2;
  uint8_t  dig_H3;
  int16_t  dig_H4;
  int16_t  dig_H5;
  int8_t   dig_H6;
} bmp280_calib_data;
		
static bmp280_calib_data bmp280_calib; 
long t_fine;
enum
{
  BMP280_REGISTER_DIG_T1              = 0x88,
  BMP280_REGISTER_DIG_T2              = 0x8A,
  BMP280_REGISTER_DIG_T3              = 0x8C,

  BMP280_REGISTER_DIG_P1              = 0x8E,
  BMP280_REGISTER_DIG_P2              = 0x90,
  BMP280_REGISTER_DIG_P3              = 0x92,
  BMP280_REGISTER_DIG_P4              = 0x94,
  BMP280_REGISTER_DIG_P5              = 0x96,
  BMP280_REGISTER_DIG_P6              = 0x98,
  BMP280_REGISTER_DIG_P7              = 0x9A,
  BMP280_REGISTER_DIG_P8              = 0x9C,
  BMP280_REGISTER_DIG_P9              = 0x9E,

  BMP280_REGISTER_CHIPID             = 0xD0,
  BMP280_REGISTER_VERSION            = 0xD1,
  BMP280_REGISTER_SOFTRESET          = 0xE0,

  BMP280_REGISTER_CAL26              = 0xE1,  // R calibration stored in 0xE1-0xF0

  BMP280_REGISTER_CONTROL            = 0xF4,
  BMP280_REGISTER_CONFIG             = 0xF5,
  BMP280_REGISTER_PRESSUREDATA       = 0xF7,
  BMP280_REGISTER_TEMPDATA           = 0xFA,
};

uint8_t read8(unsigned char reg)
{
  uint8_t value;
  tx_data[0] = reg;
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, 2, 0x10);
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
  value = rx_data[1];
  return value;
}

uint16_t read16(unsigned char reg)
{
  uint16_t value;
  tx_data[0] = reg;
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, 3, 0x10);
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
  value = (rx_data[1]<<8) + rx_data[2];
  return value;
}

uint16_t read16_LE(unsigned char reg) 
{
  uint16_t temp = read16(reg);
  return (temp >> 8) | (temp << 8);  
}

int16_t readS16(unsigned char reg)
{
  return (int16_t)read16(reg);
}

int16_t readS16_LE(unsigned char reg)
{
  return (int16_t)read16_LE(reg);
}

float readTemperature(void)
{
  int32_t var1, var2;

  int32_t adc_T = read16(BMP280_REGISTER_TEMPDATA);
  adc_T <<= 8;
  adc_T |= read8(BMP280_REGISTER_TEMPDATA+2);
  adc_T >>= 4;

  var1  = ((((adc_T>>3) - ((int32_t)bmp280_calib.dig_T1 <<1))) * 
      ((int32_t)bmp280_calib.dig_T2)) >> 11;

  var2  = (((((adc_T>>4) - ((int32_t)bmp280_calib.dig_T1)) * 
        ((adc_T>>4) - ((int32_t)bmp280_calib.dig_T1))) >> 12) * 
      ((int32_t)bmp280_calib.dig_T3)) >> 14;

  t_fine = var1 + var2;

  float T  = (t_fine * 5 + 128) >> 8;
  return T/100;
}

float readPressure(void)
{
  int64_t var1, var2, p;

  int32_t adc_P = read16(BMP280_REGISTER_PRESSUREDATA);
  adc_P <<= 8;
  adc_P |= read8(BMP280_REGISTER_PRESSUREDATA+2);
  adc_P >>= 4;

  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)bmp280_calib.dig_P6;
  var2 = var2 + ((var1*(int64_t)bmp280_calib.dig_P5)<<17);
  var2 = var2 + (((int64_t)bmp280_calib.dig_P4)<<35);
  var1 = ((var1 * var1 * (int64_t)bmp280_calib.dig_P3)>>8) +
    ((var1 * (int64_t)bmp280_calib.dig_P2)<<12);
  var1 = (((((int64_t)1)<<47)+var1))*((int64_t)bmp280_calib.dig_P1)>>33;

  if (var1 == 0) {
    return 0;  // avoid exception caused by division by zero
  }
  p = 1048576 - adc_P;
  p = (((p<<31) - var2)*3125) / var1;
  var1 = (((int64_t)bmp280_calib.dig_P9) * (p>>13) * (p>>13)) >> 25;
  var2 = (((int64_t)bmp280_calib.dig_P8) * p) >> 19;

  p = ((p + var1 + var2) >> 8) + (((int64_t)bmp280_calib.dig_P7)<<4);
  return (float)p/256;
}

float T,P;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
  // Uart1_printf(startMessage);
  // Uart2_printf(startMessage);
  // HAL_UART_Receive_IT(&huart1, (uint8_t *)rxBuffer, 15);
  // HAL_UART_Receive_IT(&huart2, (uint8_t *)rxBuffer2, 15);

  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);

  tx_data[0] = 0xD0;
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, 2, 0x10);
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);
  Uart1_printf("spi: 0x%x\r\n",rx_data[1]);
  HAL_Delay(100);

  bmp280_calib.dig_T1 = read16_LE(BMP280_REGISTER_DIG_T1);
  bmp280_calib.dig_T2 = readS16_LE(BMP280_REGISTER_DIG_T2);
  bmp280_calib.dig_T3 = readS16_LE(BMP280_REGISTER_DIG_T3);

  bmp280_calib.dig_P1 = read16_LE(BMP280_REGISTER_DIG_P1);
  bmp280_calib.dig_P2 = readS16_LE(BMP280_REGISTER_DIG_P2);
  bmp280_calib.dig_P3 = readS16_LE(BMP280_REGISTER_DIG_P3);
  bmp280_calib.dig_P4 = readS16_LE(BMP280_REGISTER_DIG_P4);
  bmp280_calib.dig_P5 = readS16_LE(BMP280_REGISTER_DIG_P5);
  bmp280_calib.dig_P6 = readS16_LE(BMP280_REGISTER_DIG_P6);
  bmp280_calib.dig_P7 = readS16_LE(BMP280_REGISTER_DIG_P7);
  bmp280_calib.dig_P8 = readS16_LE(BMP280_REGISTER_DIG_P8);
  bmp280_calib.dig_P9 = readS16_LE(BMP280_REGISTER_DIG_P9);

  tx_data[0] = 0x74;
  tx_data[1] = 0xff;
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET);
  HAL_SPI_TransmitReceive(&hspi1, tx_data, rx_data, 2, 0x10);
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
#if 0
    // HAL_Delay(500);
    if (Key_Scan(KEY1_GPIO_Port, KEY1_Pin) == KEY_ON)
    {
      // D2 反转
      HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    }
#endif
    T = readTemperature();
    P = readPressure();

    //printf("T = %f; P = %f\r\n", 23.3, 45.1);
    Uart1_printf("T = %fC; P = %fPa\r\n",T,P);
    HAL_Delay(500);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

