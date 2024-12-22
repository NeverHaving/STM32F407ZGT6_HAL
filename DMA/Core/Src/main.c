/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "ssd1306.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DATA_SIZE 4

uint8_t DataA[] = {0x01, 0x02, 0x03, 0x04}; // 源数据
uint8_t DataB[DATA_SIZE];                    // 目的数据

DMA_HandleTypeDef hdma;


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static void MX_DMA_Init(void) {
    __HAL_RCC_DMA2_CLK_ENABLE(); // 启用 DMA 时钟

    // 配置 DMA
    hdma.Instance = DMA2_Stream0; // 选择 DMA 流
    hdma.Init.Channel = DMA_CHANNEL_0; // 选择通道
    hdma.Init.Direction = DMA_MEMORY_TO_MEMORY; // 内存到内存传输
    hdma.Init.PeriphInc = DMA_PINC_ENABLE; // 外设地址递增
    hdma.Init.MemInc = DMA_MINC_ENABLE; // 内存地址递增
    hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE; // 外设数据对齐
    hdma.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE; // 内存数据对齐
    hdma.Init.Mode = DMA_NORMAL; // 普通模式
    hdma.Init.Priority = DMA_PRIORITY_LOW; // 优先级
    hdma.Init.FIFOMode = DMA_FIFOMODE_DISABLE; // 禁用 FIFO
    HAL_DMA_Init(&hdma);

    // 关联 DMA 和外设
    // __HAL_LINKDMA(&hdma, hdma, &hdma); // 这一步可以根据具体外设进行调整
}


void displayData(uint8_t *data, uint8_t startY, const char *label) {
    char strTmp[20]; // 用于存放转换后的字符串
    char dataStr[100] = {0}; // 用于存放所有数据的字符串
    uint8_t offset = 0; // 用于计算字符串位置

    // 显示标签
    OLED_DrawStr(1, startY, (uint8_t *)label); // 显示标签

    // 构建数据字符串
    for (uint8_t i = 0; i < DATA_SIZE; i++) {
        snprintf(strTmp, sizeof(strTmp), "%02X", data[i]); // 转换为十六进制字符串
        if (i > 0) {
            offset += snprintf(dataStr + offset, sizeof(dataStr) - offset, " "); // 添加空格
        }
        offset += snprintf(dataStr + offset, sizeof(dataStr) - offset, "%s", strTmp); // 添加数据
    }

    // 显示数据字符串
    OLED_DrawStr(1, startY + 8, (uint8_t *)dataStr); // 显示所有数据
}

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
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
	OLED_Init(hi2c1);
  MX_DMA_Init();
	
  displayData(DataA, 1, "DataA:");
  displayData(DataB, 1 + (DATA_SIZE + 1) * 8, "DataB:");
  OLED_Refresh(); //
	
  // 启动 DMA 传输
  if (HAL_DMA_Start(&hdma, (uint32_t)DataA, (uint32_t)DataB, DATA_SIZE) != HAL_OK) {
      // 错误处理
      while (1);
  }

    // 等待 DMA 传输完成
  HAL_DMA_PollForTransfer(&hdma, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);

  // 传输完成后，显示 DataA 和 DataB 的内容
  displayData(DataA, 1, "DataA:");
  displayData(DataB, 1 + (DATA_SIZE + 1) * 8, "DataB:");
  OLED_Refresh(); //

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

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
