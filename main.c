/**
  ******************************************************************************
  * @file    main.c
  * @author  MCU Application Team
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) Puya Semiconductor Co.
  * All rights reserved.</center></h2>
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_SystemClockConfig(void);
static void APP_ConfigureExti(void);

/* Private variables ---------------------------------------------------------*/

/**
  * @brief  USB peripheral initialization function
  * @param  None
  * @retval None
  */
static void APP_USBInit(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE();

  __HAL_RCC_USB_CLK_ENABLE();

  cdc_acm_init();

  /* Enable USB interrupt */
  NVIC_EnableIRQ(USBD_IRQn);
}


// Function to print an array of integers
void print_int_array(unsigned BufferIndex, const char *prefix, const uint8_t *array, size_t size) {
    // Print the prefix (optional)
    if (prefix != NULL) {
        SEGGER_RTT_printf(BufferIndex, "%s\n", prefix);
    }

    // Print the array elements
    for (size_t i = 0; i < size; i++) {
        SEGGER_RTT_printf(BufferIndex, "Element %zu: %d\n", i, array[i]);
    }
}

 
/**
  * @brief  应用程序入口函数.
  * @retval int
  */
int main(void)
{
  HAL_Init();
  /* 系统时钟配置 */
		APP_SystemClockConfig();
  /* 初始化所有外设，Flash接口，SysTick */
  HAL_Init();
  
//  /* 初始化LED GPIO */
//  BSP_LED_Init(LED_GREEN);
//  
//  SEGGER_RTT_printf(0, "main start\r\n" );
//  /* Initialize USB peripheral */
//  //APP_USBInit();
//  for (uint32_t i=0; i<2; i++)
//  {
//    BSP_LED_Toggle(LED_GREEN);
//    HAL_Delay(100);
//  }
//  
  /* 配置外部中断 */
////  APP_ConfigureExti();
// sgdOnePulseConfig(3000 );
//  
// 		sgdStartOnePluse( );
  
//		HAL_TIM_Encoder_Start(&TimHandleM, TIM_CHANNEL_1);
//192.168.0.151 19020
	
 //   cdc_acm_data_send_with_dtr_test();
  sgdInit();
  sgdLoop();
  while (1)
  {
    /* Delay for 1s */
    HAL_Delay(1); 
		//HAL_TIM_Encoder_Start(&TimHandleM, TIM_CHANNEL_1);
    /* Call the test function to send data to the USB host */
    //cdc_acm_data_send_with_dtr_test();
 //	HAL_TIM_Encoder_Start(&TimHandle,TIM_CHANNEL_1);
  }
}

/**
  * @brief  系统时钟配置函数
  * @param  无
  * @retval 无
  */
static void APP_SystemClockConfig(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /* Oscillator configuration */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE; /* Select oscillators HSE, HSI, LSI, LSE */
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;                           /* Enable HSI */
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;                           /* HSI not divided */
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_16MHz;  /* Configure HSI clock as 16MHz */
  RCC_OscInitStruct.HSEState = RCC_HSE_OFF;                          /* Disable HSE */
  /*RCC_OscInitStruct.HSEFreq = RCC_HSE_16_32MHz;*/
  RCC_OscInitStruct.LSIState = RCC_LSI_OFF;                          /* Disable LSI */
  RCC_OscInitStruct.LSEState = RCC_LSE_OFF;                          /* Disable LSE */
  /*RCC_OscInitStruct.LSEDriver = RCC_LSEDRIVE_MEDIUM;*/
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;                       /* Enable PLL */
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;               /* PLL source is HSI */
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL3;                       /* PLL multiplication factor is 3 */
  /* Configure oscillator */
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    APP_ErrorHandler();
  }

  /* Clock source configuration */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1; /* Configure clock types HCLK, SYSCLK, PCLK1 */
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; /* Select PLL as system clock */
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;        /* AHB clock not divided */
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;         /* APB clock not divided */
  /* Configure clock source */
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    APP_ErrorHandler();
  }
}


/**
  * @brief  配置EXTI
  * @param  无
  * @retval 无
  */
static void APP_ConfigureExti(void)
{
  /* 配置引脚 */
  GPIO_InitTypeDef  GPIO_InitStruct;
  USER_BUTTON_GPIO_CLK_ENABLE();                 /* 使能按键 GPIO时钟 */
  GPIO_InitStruct.Mode  = GPIO_MODE_IT_FALLING;  /* GPIO模式为下降沿中断 */
  GPIO_InitStruct.Pull  = GPIO_PULLUP;           /* 上拉 */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  /* 速度为高速 */
  GPIO_InitStruct.Pin = USER_BUTTON_PIN;
  HAL_GPIO_Init(USER_BUTTON_GPIO_PORT, &GPIO_InitStruct);

  /* 使能EXTI中断 */
  HAL_NVIC_EnableIRQ(USER_BUTTON_EXTI_IRQn);
  /* 配置中断优先级 */
  HAL_NVIC_SetPriority(USER_BUTTON_EXTI_IRQn, 0, 0);
}

/**
  * @brief  错误执行函数
  * @param  无
  * @retval 无
  */
void APP_ErrorHandler(void)
{
  /* 无限循环 */
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  输出产生断言错误的源文件名及行号
  * @param  file：源文件名指针
  * @param  line：发生断言错误的行号
  * @retval 无
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* 用户可以根据需要添加自己的打印信息,
     例如: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* 无限循环 */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
