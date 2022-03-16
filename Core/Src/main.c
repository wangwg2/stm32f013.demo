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
#include "sht31.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/** @name SHT31 (I2C) */
#define SHT31_I2C_ENABLE 1
// #define SENDBACK_I2C_ENABLE 1

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t key_toggle = 0;
char startMessage[] = "** UART communication based on IT **\r\n";
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
#ifdef SENDBACK_I2C_ENABLE
  uint8_t sendback_i2c_on = 0;
  HAL_StatusTypeDef status;
  // 使能主机中断接收
  status = HAL_I2C_Master_Receive_IT(&hi2c1, SENDBACK_I2C_ADDR_READ, I2C_recvBuf, sizeof(I2C_recvBuf));
  if (status != HAL_OK)
  {
    HAL_I2C_StateTypeDef error_status = HAL_I2C_GetState(&hi2c1);
    printf("HAL_I2C_Master_Receive_IT err1:%02x -- err2:%02x\r\n", status, error_status);
    return -1;
  }
#endif // SENDBACK_I2C_ENABLE
#ifdef SHT31_I2C_ENABLE
  uint8_t sht31_i2c_on = 0;
  HAL_StatusTypeDef status;
  SHT31_DATA sht31_data;
  uint8_t I2C_sendBuf[2] = {0x24, 0x00};
  uint8_t I2C_recvdBuf[6];
  HAL_Delay(200);

#if 0
  sht31_get_data(&sht31_data);
  printf("SHT31 temperature: %d, humidity: %d\r\n", sht31_data.temperature, sht31_data.humidity);
  HAL_Delay(200);

  sht31_reset();
  if (sht31_init() == HAL_OK)
    printf("SHT31 init OK\n");
  else
    printf("SHT31 init FAIL\n");
  HAL_Delay(2000);
#endif // 0
#endif // SHT31_I2C_ENABLE
  
  Uart1_printf(startMessage);
  Uart2_printf(startMessage);
  HAL_UART_Receive_IT(&huart1, (uint8_t *)rxBuffer, 15);
  HAL_UART_Receive_IT(&huart2, (uint8_t *)rxBuffer2, 15);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HAL_Delay(200);
    if (Key_Scan(KEY1_GPIO_Port, KEY1_Pin) == KEY_ON)
    {
      key_toggle = 1;
      // D2 反转
      HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
#ifdef SENDBACK_I2C_ENABLE
      sendback_i2c_on = !sendback_i2c_on;
#endif // SENDBACK_I2C_ENABLE
#ifdef SHT31_I2C_ENABLE
      sht31_i2c_on = !sht31_i2c_on;
#endif // SHT31_I2C_ENABLE
    }
#ifdef SENDBACK_I2C_ENABLE
    if (sendback_i2c_on)
    {
      i2c_send_data();
    }
#endif // SENDBACK_I2C_ENABLE
#ifdef SHT31_I2C_ENABLE
    if (sht31_i2c_on)
    {
      // status = HAL_I2C_Master_Transmit(&hi2c1, SHT31_ADDR_WRITE, I2C_sendBuf, sizeof(I2C_sendBuf), 0x50);
      // if (status != HAL_OK)
      // {
      //   HAL_I2C_StateTypeDef error_status = HAL_I2C_GetState(&hi2c1);
      //   printf("HAL_I2C_Master_Transmit err1:%02x -- err2:%02x\r\n", status, error_status);
      // }
      status = HAL_I2C_Master_Receive(&hi2c1, SHT31_ADDR_READ, I2C_recvdBuf, sizeof(I2C_recvdBuf), 0x50);
      if (status != HAL_OK)
      {
        HAL_I2C_StateTypeDef error_status = HAL_I2C_GetState(&hi2c1);
        printf("HAL_I2C_Master_Receive err1:%02x -- err2:%02x\r\n", status, error_status);
      }
      sht31_parse_data(&sht31_data, I2C_recvdBuf);
#if 0      
      sht31_reset();
      if (sht31_init() == HAL_OK)
        printf("SHT31 init OK\n");
      else
        printf("SHT31 init FAIL\n");
      HAL_Delay(20);
      if (sht31_get_data(&sht31_data) == HAL_OK)
      {
        printf("SHT31 temperature: %d, humidity: %d\r\n", sht31_data.temperature, sht31_data.humidity);
      }
      else
      {
        printf("SHT31 error!\r\n");
      }
#endif // 0
    }
#endif // SHT31_I2C_ENABLE
    key_toggle = 0;
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

