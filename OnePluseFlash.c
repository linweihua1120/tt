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
/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef    TimHandle;
TIM_OnePulse_InitTypeDef sConfig2;

TIM_HandleTypeDef    TimHandleM;
TIM_OnePulse_InitTypeDef sConfigM;

void sgdStartOnePluse(void)
{ 
  /* Starts the TIM1 One Pulse signal generation. */
  if (HAL_TIM_OnePulse_Start(&TimHandleM, TIM_CHANNEL_1) != HAL_OK)
  {
    APP_ErrorHandler();
   }
	HAL_TIM_Encoder_Start(&TimHandleM,TIM_CHANNEL_1);
					SEGGER_RTT_printf(0, "sgdStartOnePluse Period = %d\r\n ",TimHandleM.Init.Period);
}



void startPluse(){

}
void sgdOnePulseConfig(int pluseUS)
{

  TimHandleM.Instance = TIM3;                                           /* Select TIM1 */
 // TimHandleM.Init.Period            = 3000;                           /* Auto reload value： */
  TimHandleM.Init.Period            = pluseUS;                           /* Auto reload value： */
  TimHandleM.Init.Prescaler         = 23;                            /* Prescaler:1-1 */
  TimHandleM.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;           /* Clock division: tDTS=tCK_INT */
  TimHandleM.Init.CounterMode       = TIM_COUNTERMODE_UP;               /* CounterMode:Up */
  TimHandleM.Init.RepetitionCounter = 1 - 1;                            /* repetition counter value:1-1 */
  TimHandleM.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;   /* TIM1_ARR register is not buffered */
  /* Initializes the TIM1 One Pulse mode */
  if (HAL_TIM_OnePulse_Init(&TimHandleM, TIM_OPMODE_SINGLE) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  sConfig2.OCMode       = TIM_OCMODE_PWM2;                              /* Set as PWM2 mode  */
  sConfig2.OCPolarity   = TIM_OCPOLARITY_HIGH;                          /* OC channel active high */
  sConfig2.Pulse        = 1;                                        /* Pulse:16383 */
  sConfig2.ICPolarity   = TIM_ICPOLARITY_RISING;                        /* Capture triggered by rising edge on timer input */
  sConfig2.ICSelection  = TIM_ICSELECTION_DIRECTTI;                     /* CC2 channel connect to TI2 */
  sConfig2.ICFilter     = 0;                                            /* No filter */
  sConfig2.OCNPolarity  = TIM_OCNPOLARITY_HIGH;                         /* OCN channel active high */
  sConfig2.OCIdleState  = TIM_OCIDLESTATE_RESET;                        /* OC1 channel idle state is low level */
  sConfig2.OCNIdleState = TIM_OCNIDLESTATE_RESET;                       /* OC1N channel idle state is low level */
  /* Initializes the TIM One Pulse Channels */
  if (HAL_TIM_OnePulse_ConfigChannel(&TimHandleM, &sConfig2, TIM_CHANNEL_2, TIM_CHANNEL_1) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  
//
//  TimHandleM.Instance = TIM2;                                           /* Select TIM1 */
//  TimHandleM.Init.Period            = 300;                           /* Auto reload value： */
// // TimHandleM.Init.Period            = pluseUS;                           /* Auto reload value： */
//  TimHandleM.Init.Prescaler         = 23;                            /* Prescaler:1-1 */
//  TimHandleM.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;           /* Clock division: tDTS=tCK_INT */
//  TimHandleM.Init.CounterMode       = TIM_COUNTERMODE_UP;               /* CounterMode:Up */
//  TimHandleM.Init.RepetitionCounter = 1 - 1;                            /* repetition counter value:1-1 */
//  TimHandleM.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;   /* TIM1_ARR register is not buffered */
//  /* Initializes the TIM1 One Pulse mode */
//  if (HAL_TIM_OnePulse_Init(&TimHandleM, TIM_OPMODE_SINGLE) != HAL_OK)
//  {
//    APP_ErrorHandler();
//  }
//  sConfigM.OCMode       = TIM_OCMODE_PWM2;                              /* Set as PWM2 mode  */
//  sConfigM.OCPolarity   = TIM_OCPOLARITY_HIGH;                          /* OC channel active high */
//  sConfigM.Pulse        = 1;                                        /* Pulse:16383 */
//  sConfigM.ICPolarity   = TIM_ICPOLARITY_RISING;                        /* Capture triggered by rising edge on timer input */
//  sConfigM.ICSelection  = TIM_ICSELECTION_DIRECTTI;                     /* CC2 channel connect to TI2 */
//  sConfigM.ICFilter     = 0;                                            /* No filter */
//  sConfigM.OCNPolarity  = TIM_OCNPOLARITY_HIGH;                         /* OCN channel active high */
//  sConfigM.OCIdleState  = TIM_OCIDLESTATE_RESET;                        /* OC1 channel idle state is low level */
//  sConfigM.OCNIdleState = TIM_OCNIDLESTATE_RESET;                       /* OC1N channel idle state is low level */
//  /* Initializes the TIM One Pulse Channels */
//  if (HAL_TIM_OnePulse_ConfigChannel(&TimHandleM, &sConfigM, TIM_CHANNEL_1, TIM_CHANNEL_2) != HAL_OK)
//  {
//    APP_ErrorHandler();
//  }
  
					SEGGER_RTT_printf(0, "sgdOnePulseConfig Period = %d\r\n ",TimHandleM.Init.Period);
//	
//  TimHandle.Instance = TIM1;                                           /* Select TIM1 */
//  TimHandle.Init.Period            = pluseUS;                           /* Auto reload value： */
//  TimHandle.Init.Prescaler         = 47;                            /* Prescaler:1-1 */
//  TimHandle.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;           /* Clock division: tDTS=tCK_INT */
//  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;               /* CounterMode:Up */
//  TimHandle.Init.RepetitionCounter = 0;                            /* repetition counter value:1-1 */
//  TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;   /* TIM1_ARR register is not buffered */
//  /* Initializes the TIM1 One Pulse mode */
//  if (HAL_TIM_OnePulse_Init(&TimHandle, TIM_OPMODE_SINGLE) != HAL_OK)
//  {
//    APP_ErrorHandler();
//  }
//  sConfig2.OCMode       = TIM_OCMODE_PWM2;                              /* Set as PWM2 mode  */
//  sConfig2.OCPolarity   = TIM_OCPOLARITY_HIGH;                          /* OC channel active high */
//  sConfig2.Pulse        = 1;                                        /* Pulse:16383 */
//  sConfig2.ICPolarity   = TIM_ICPOLARITY_RISING;                        /* Capture triggered by rising edge on timer input */
//  sConfig2.ICSelection  = TIM_ICSELECTION_DIRECTTI;                     /* CC2 channel connect to TI2 */
//  sConfig2.ICFilter     = 0;                                            /* No filter */
//  sConfig2.OCNPolarity  = TIM_OCNPOLARITY_HIGH;                         /* OCN channel active high */
//  sConfig2.OCIdleState  = TIM_OCIDLESTATE_RESET;                        /* OC1 channel idle state is low level */
//  sConfig2.OCNIdleState = TIM_OCNIDLESTATE_RESET;                       /* OC1N channel idle state is low level */
//  /* Initializes the TIM One Pulse Channels */
//  if (HAL_TIM_OnePulse_ConfigChannel(&TimHandle, &sConfig2, TIM_CHANNEL_1, TIM_CHANNEL_2) != HAL_OK)
//  {
//    APP_ErrorHandler();
//  }
//  

}


/************************ (C) COPYRIGHT Puya *****END OF FILE******************/

