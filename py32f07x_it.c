/**
  ******************************************************************************
  * @file    py32f07x_it.c
  * @author  MCU Application Team
  * @brief   Interrupt Service Routines.
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
#include "py32f07x_it.h"
#include "sgd.h"

/* Private includes ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private user code ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
extern TIM_HandleTypeDef    TimHandle1;

/******************************************************************************/
/*          Cortex-M0+ Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
}

/**
  * @brief This function handles Hard fault interrupt.
  */


void HardFault_Handler(void) {
    while(1)
		{
			;
		}
	}


/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
  
	//SEGGER_RTT_printf(0, "r  \r\n");
 			// sgdReadAdc( );
	if(getTick() % 10  == 0){
			//sgdIP5036Ctrl();
			sgdReadAdc();
		}
}


/**
  * @brief This function handles EXTI4_15 interrupt.
  */
void EXTI4_15_IRQHandler(void)
{
//LL_GPIO_TogglePin(PORT_FULL_LED, PIN_FULL_LED);
  /* Handle EXTI interrupt request */
//	 LL_GPIO_TogglePin(PORT_CHG_LED, PIN_CHG_LED);

  // SEGGER_RTT_printf(0, "EXTI4_15_IRQHandler %x  %d\r\n",READ_REG(EXTI->PR),getTick());
//  if(LL_EXTI_IsActiveFlag(LL_EXTI_LINE_1))
//  { 
//  	sgdStartFlash();
//	 SEGGER_RTT_printf(0, "EXTI4_15_IRQHandler %d  %d\r\n",LL_EXTI_LINE_1,getTick());
//    LL_EXTI_ClearFlag(LL_EXTI_LINE_1);
//  }
//  else if(LL_EXTI_IsActiveFlag(LL_EXTI_LINE_9))
//  {
//  	sgdStartFlash();
//	 SEGGER_RTT_printf(0, "EXTI4_15_IRQHandler  5- %d  %d\r\n",LL_EXTI_LINE_9,getTick());
//    LL_EXTI_ClearFlag((LL_EXTI_LINE_9));
//  }
//  else if(LL_EXTI_IsActiveFlag(LL_EXTI_LINE_10))
//  {
//  	//sgdStartFlash();
//  	sgdStartSFlash();
//	 SEGGER_RTT_printf(0, "EXTI4_15_IRQHandler 7- %d  %d\r\n",LL_EXTI_LINE_10,getTick());
//    LL_EXTI_ClearFlag(LL_EXTI_LINE_10);
//  }
//  HAL_EXTI_ClearConfigLine(EXTI_HandleTypeDef * hexti)
//  HAL_GPIO_EXTI_IRQHandler(USER_BUTTON_PIN);
//  
//    HAL_GPIO_EXTI_Callback(GPIO_Pin);

 	HAL_GPIO_EXTI_IRQHandler(PIN_CAMERA);
	HAL_GPIO_EXTI_IRQHandler(PIN_FLASH_IN);
	HAL_GPIO_EXTI_IRQHandler(PIN_SW_DEC);
	//HAL_GPIO_EXTI_IRQHandler(PIN_XFULL);
	HAL_GPIO_EXTI_IRQHandler(PIN_IGBT_G);

  
}

void EXTI2_3_IRQHandler(void)
{
	
	
	
	
  HAL_GPIO_EXTI_IRQHandler(PIN_FLASH_BT);
}

void EXTI0_1_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(PIN_CAMERA);
}



/******************************************************************************/
/* PY32F071 Peripheral Interrupt Handlers                                     */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file.                                          */
/******************************************************************************/
void USB_IRQHandler(void)
{
  USBD_IRQHandler();
}

extern COMP_HandleTypeDef  hcomp1;
extern  ADC_HandleTypeDef        AdcHandle;
void ADC_COMP_IRQHandler(void)
{
  HAL_ADC_IRQHandler(&AdcHandle);
  HAL_COMP_IRQHandler(&hcomp1);
}

void HAL_TIMEx_BreakCallback(TIM_HandleTypeDef *htim)
{
  /* Prevent unused argument(s) compilation warning */
   
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_TIMEx_BreakCallback could be implemented in the user file
   */
}
void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
   HAL_TIM_IRQHandler(&TimHandle1);
}
extern RTC_HandleTypeDef RtcHandle;
void RTC_IRQHandler(void)
{
  HAL_RTCEx_RTCIRQHandler(&RtcHandle);
		SEGGER_RTT_printf(0, "RTC_IRQHandler   \r\n");
}

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
