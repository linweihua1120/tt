/**
  ******************************************************************************
  * @file    main.c
  * @author  MCU Application Team
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 Puya Semiconductor Co.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by Puya under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  * @attention
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
#include "sb.h"
#include "sgd.h"


#include "SEGGER_RTT.h"

/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef    TimHandle1,TimHandle2, TimHandle15;
TIM_SlaveConfigTypeDef   sSlaveConfig;
TIM_MasterConfigTypeDef sMasterConfig;
//TIM_OC_InitTypeDef sConfig;
TIM_BreakDeadTimeConfigTypeDef  sBreakConfig;

TIM_OC_InitTypeDef              sPWMConfig;
COMP_HandleTypeDef  hcomp1;
int compTick = 0 ;
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
#define PERIOD (400-1)
#define PERIOD15 360
#define PLUSE (PERIOD*30 / 100)
#define PRESCALER 0
int timerState = 1 ;
int sgdGetPsfbTimerState (){
	return timerState;
}
void sgdSetPsfbTimerState (int _state){
	  timerState = _state;
}
 void initExitIO(){
// 
//  GPIO_InitTypeDef  GPIO_InitStruct = {0};
//  USER_BUTTON_GPIO_CLK_ENABLE();                 /* Enable the GPIO clock corresponding to the button */
//  GPIO_InitStruct.Mode  = GPIO_MODE_IT_FALLING;  /* GPIO mode is a falling edge interrupt */
//  GPIO_InitStruct.Pull  = GPIO_PULLUP;           /* pull up */
//  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  /* The speed is high */
//  GPIO_InitStruct.Pin = GPIO_PIN_0;
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

//  HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
//  HAL_NVIC_SetPriority(EXTI0_1_IRQn, 0, 0);



 	}

/**
  * @brief  Main program.
  * @retval int
  */

void setShipParse(int shift){
	HAL_TIM_OC_Stop(&TimHandle1,TIM_CHANNEL_1);
	HAL_TIM_OC_Stop(&TimHandle15,TIM_CHANNEL_1);
	HAL_TIMEx_OCN_Stop(&TimHandle1,TIM_CHANNEL_1);
	HAL_TIMEx_OCN_Stop(&TimHandle15,TIM_CHANNEL_1);
	HAL_TIM_Base_Stop(&TimHandle1);
	HAL_TIM_Base_Stop(&TimHandle15);
	__HAL_TIM_SET_COUNTER(&TimHandle1,shift * PERIOD / 100);
	
	HAL_TIM_Base_Start(&TimHandle15);
	HAL_TIM_OC_Start(&TimHandle1,TIM_CHANNEL_1);
	HAL_TIM_OC_Start(&TimHandle15,TIM_CHANNEL_1);
	HAL_TIMEx_OCN_Start(&TimHandle1,TIM_CHANNEL_1);
	HAL_TIMEx_OCN_Start(&TimHandle15,TIM_CHANNEL_1); 
	 if (HAL_TIM_OC_Start(&TimHandle15, TIM_CHANNEL_1) != HAL_OK)
	 {
	   APP_ErrorHandler();
	 }
	
} 

int state = 0 ;

void setGPIOA6BrakeLow(){
//	SEGGER_RTT_printf(0, "setGPIOA6BrakeLow \r\n");
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10);
  GPIO_InitTypeDef   GPIO_InitStruct = {0};
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  HAL_GPIO_Init(GPIOB,  &GPIO_InitStruct);   
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,0);

}
void setCompTick(int tick){
	if(tick > 10 ){
	compTick  = tick;
		}
}
void setGPIOA6CompOut(){
//	SEGGER_RTT_printf(0, "setGPIOA6CompOut \r\n");
  GPIO_InitTypeDef   GPIO_InitStruct = {0};
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	
	
  GPIO_InitStruct.Alternate = GPIO_AF7_COMP1;            /* analog Mode */
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;            /* analog Mode */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  HAL_GPIO_Init(GPIOB,  &GPIO_InitStruct);   

}
//
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
////	SEGGER_RTT_printf(0, "HAL_GPIO_EXTI_Callback \r\n");
//	if(state == 0 ){
//		state = 1; 
//		}
//	else{
//		state =  0;
//		}
////		setGPIOA6BrakeLow();
//////	setShipParse(50);
////		 setGPIOA6CompOut();
//
//}

int prePluse = 0;

// void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//  /* Prevent unused argument(s) compilation warning */
// // UNUSED(htim);
//
//  /* NOTE : This function should not be modified, when the callback is needed,
//            the HAL_TIM_PeriodElapsedCallback could be implemented in the user file
//   */
//	//SEGGER_RTT_printf(0, "1\r\n");
//	if(prePluse !=  sgdGetPwmPluse()){
//
//	prePluse = sgdGetPwmPluse();
//	
//			SEGGER_RTT_printf(0, "prePluse !=  sgdGetPwmPluse() \r\n");
//	HAL_TIM_OC_Stop(&TimHandle1,TIM_CHANNEL_1);
//	HAL_TIM_OC_Stop(&TimHandle1,TIM_CHANNEL_2);
//	HAL_TIM_Base_Stop(&TimHandle1);
//	timerState = 0;
//  /// 	setPluse( prePluse);
//		}
//}
void initTim15(int setPluseRate){
	
	 TimHandle15.Instance = TIM15;												 /* Select TIM3 */
	 TimHandle15.Init.Prescaler = PRESCALER;									 /* Prescaler:1000-1 */
	 TimHandle15.Init.CounterMode	   = TIM_COUNTERMODE_UP;//TIM_COUNTERMODE_UP;			   /* CounterMode:Up */
	 TimHandle15.Init.Period = PERIOD15;										  /* Auto reload value： */
	 TimHandle15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;					/* Clock division: tDTS=tCK_INT */
	 TimHandle15.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;		/* TIM1_ARR register is not buffered */
	 /* Initializes the TIM Output Compare */
	 if (HAL_TIM_OC_Init(&TimHandle15) != HAL_OK)
	 {
	   APP_ErrorHandler();
	 }
//	 sBreakConfig.BreakState	   = TIM_BREAK_DISABLE;// TIM_BREAK_ENABLE; 					 /* Break input BRK is enabled */
//	  //sBreakConfig.BreakState	  =  TIM_BREAK_ENABLE;						/* Break input BRK is enabled */
//	 sBreakConfig.DeadTime		   = 25; 								  /* Set the dead time */
//	 sBreakConfig.OffStateRunMode  = TIM_OSSR_ENABLE;						/* Off state in run mode: output enable */
//	 sBreakConfig.OffStateIDLEMode = TIM_OSSI_ENABLE;						/* IDLE state in run mode: output enable */
//	 sBreakConfig.LockLevel 	   = TIM_LOCKLEVEL_OFF; 					/* LOCK off */
//	 sBreakConfig.BreakPolarity    = TIM_BREAKPOLARITY_LOW; 				/* Break input BRK is active LOW */
//	 sBreakConfig.AutomaticOutput  = TIM_AUTOMATICOUTPUT_ENABLE;			/* Automatic output enable */
//	 /* Configures the Break feature, dead time */
//	 if (HAL_TIMEx_ConfigBreakDeadTime(&TimHandle15, &sBreakConfig) != HAL_OK)
//	 {
//	   APP_ErrorHandler();
//	 }
	 sPWMConfig.OCMode		 = TIM_OCMODE_PWM2; 									 /* Set as PWM1 mode */
	 sPWMConfig.OCPolarity	 = TIM_OCPOLARITY_HIGH; 								 /* Compare output polarity: HIGH */
	 sPWMConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;								 /* Compare complementary output polarity: HIGH */
	 sPWMConfig.OCIdleState  = TIM_OCNIDLESTATE_RESET; 								 /* Output Idle state: HIGH */
	 sPWMConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;								 /* Complementary output Idle state: LOW */
	 sPWMConfig.OCFastMode	 = TIM_OCFAST_DISABLE;									 /* Output Compare fast disable */
	 sPWMConfig.Pulse = PERIOD15 /2;												  /* Duty cycle of channel 1 */ 										/* Duty cycle of channel 1 */
	 if (HAL_TIM_PWM_ConfigChannel(&TimHandle15, &sPWMConfig, TIM_CHANNEL_1) != HAL_OK)/* Configure Channel 1 */
	 {
	   APP_ErrorHandler();
	 } 
	 sPWMConfig.Pulse = PERIOD15 /4;												  /* Duty cycle of channel 1 */ 										/* Duty cycle of channel 1 */
	 if (HAL_TIM_PWM_ConfigChannel(&TimHandle15, &sPWMConfig, TIM_CHANNEL_2) != HAL_OK)/* Configure Channel 1 */
	 {
	   APP_ErrorHandler();
	 } 
	 
	 /* Starts the TIM3 Output Compare signal generation. */
	 if (HAL_TIM_OC_Start(&TimHandle15, TIM_CHANNEL_1) != HAL_OK)
	 {
	   APP_ErrorHandler();
	 }
	 
	 /* Channel 1 outputs PWM */
	 if (HAL_TIM_PWM_Start(&TimHandle15, TIM_CHANNEL_1) != HAL_OK)
	 {
	   APP_ErrorHandler();
	 }
	 /* Channel 1N outputs PWM */
	 if (HAL_TIMEx_PWMN_Start(&TimHandle15, TIM_CHANNEL_1) != HAL_OK)
	 {
	   APP_ErrorHandler();
	 }
	 /* Channel 2 outputs PWM */
	 if (HAL_TIM_PWM_Start(&TimHandle15, TIM_CHANNEL_2) != HAL_OK)
	 {
	   APP_ErrorHandler();
	 }
	  

}

void initTim(int setPluseRate){
	
	 TimHandle1.Instance = TIM1;										   /* Select TIM1 */
//	 HAL_TIM_PWM_Stop(&TimHandle1, TIM_CHANNEL_1) ;
//	 HAL_TIM_PWM_Stop(&TimHandle1, TIM_CHANNEL_2) ;
	 /* Channel 1 outputs PWM */
//	 if (HAL_TIM_PWM_Stop(&TimHandle1, TIM_CHANNEL_1) != HAL_OK)
//	 {
//	   APP_ErrorHandler();
//	 }
//	 /* Channel 1 outputs PWM */
//	 if (HAL_TIM_PWM_Stop(&TimHandle1, TIM_CHANNEL_2) != HAL_OK)
//	 {
//	   APP_ErrorHandler();
//	 }
	//HAL_TIM_Base_DeInit(&TimHandle1);
	 TimHandle1.Init.Period 		   = PERIOD;						 /* Auto reload value： */
	 TimHandle1.Init.Prescaler		   = PRESCALER; 						 /* Prescaler:100-1 */
	 TimHandle1.Init.ClockDivision	   = TIM_CLOCKDIVISION_DIV1;		   /* Clock division: tDTS=tCK_INT */
	 TimHandle1.Init.CounterMode	   = TIM_COUNTERMODE_CENTERALIGNED3;//TIM_COUNTERMODE_UP;			   /* CounterMode:Up */
	 TimHandle1.Init.RepetitionCounter = 1 - 1; 						   /* repetition counter value:1-1 */
	 TimHandle1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;   /* TIM1_ARR register is not buffered */
	 /* Initialize TIM1 */
	 if (HAL_TIM_Base_Init(&TimHandle1) != HAL_OK)
	 {
	   APP_ErrorHandler();
	 }
	
	 // sBreakConfig.BreakState	   = TIM_BREAK_DISABLE;// TIM_BREAK_ENABLE; 					 /* Break input BRK is enabled */
	 sBreakConfig.BreakState	  =  TIM_BREAK_ENABLE;						/* Break input BRK is enabled */
	 sBreakConfig.DeadTime		   = 0; 								  /* Set the dead time */
	 sBreakConfig.OffStateRunMode  = TIM_OSSR_ENABLE;						/* Off state in run mode: output enable */
	 sBreakConfig.OffStateIDLEMode = TIM_OSSI_ENABLE;						/* IDLE state in run mode: output enable */
	 sBreakConfig.LockLevel 	   = TIM_LOCKLEVEL_OFF; 					/* LOCK off */
	 sBreakConfig.BreakPolarity    = TIM_BREAKPOLARITY_LOW; 				/* Break input BRK is active LOW */
	 sBreakConfig.AutomaticOutput  = TIM_AUTOMATICOUTPUT_ENABLE;			/* Automatic output enable */
	 /* Configures the Break feature, dead time */
	  if (HAL_TIMEx_ConfigBreakDeadTime(&TimHandle1, &sBreakConfig) != HAL_OK)
	 {
	   APP_ErrorHandler();
	 }
	 sPWMConfig.OCMode		 = TIM_OCMODE_PWM1; 									 /* Set as PWM1 mode */
	 sPWMConfig.OCPolarity	 = TIM_OCPOLARITY_HIGH; 								 /* Compare output polarity: HIGH */
	 sPWMConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;								 /* Compare complementary output polarity: HIGH */
	 sPWMConfig.OCIdleState  = TIM_OCNIDLESTATE_SET; 								 /* Output Idle state: HIGH */
	 sPWMConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;								 /* Complementary output Idle state: LOW */
	 sPWMConfig.OCFastMode	 = TIM_OCFAST_DISABLE;									 /* Output Compare fast disable */
//	 sPWMConfig.Pulse = PLUSE;												  /* Duty cycle of channel 1 */ 										/* Duty cycle of channel 1 */

	 sPWMConfig.Pulse = setPluseRate * PERIOD / 100;			
	 if (HAL_TIM_PWM_ConfigChannel(&TimHandle1, &sPWMConfig, TIM_CHANNEL_1) != HAL_OK)/* Configure Channel 1 */
	 {
	   APP_ErrorHandler();
	 }	
	 sPWMConfig.OCIdleState  = TIM_OCNIDLESTATE_RESET; 								 /* Output Idle state: HIGH */
	 sPWMConfig.OCPolarity	 = TIM_OCPOLARITY_LOW; 								 /* Compare output polarity: HIGH */
	 sPWMConfig.Pulse = PERIOD - ( setPluseRate * PERIOD / 100); 									/* Duty cycle of channel 1 */										  /* Duty cycle of channel 1 */
	 if (HAL_TIM_PWM_ConfigChannel(&TimHandle1, &sPWMConfig, TIM_CHANNEL_2) != HAL_OK)/* Configure Channel 1 */
	 {
	   APP_ErrorHandler();
	 } 
	   
	   __HAL_TIM_SET_COUNTER(&TimHandle1,0);

	 /* Enable TIM1 update interrupt */
	// __HAL_TIM_ENABLE_IT(&TimHandle1, TIM_IT_UPDATE);
	 
	 /* Channel 1 outputs PWM */
	 if (HAL_TIM_PWM_Start(&TimHandle1, TIM_CHANNEL_1) != HAL_OK)
	 {
	   APP_ErrorHandler();
	 }
	 /* Channel 1 outputs PWM */
	 if (HAL_TIM_PWM_Start(&TimHandle1, TIM_CHANNEL_2) != HAL_OK)
	 {
	   APP_ErrorHandler();
	 }
	  
	 /* Starts the TIM3 Output Compare signal generation. */
	 if (HAL_TIM_OC_Start(&TimHandle1, TIM_CHANNEL_1) != HAL_OK)
	 {
	   APP_ErrorHandler();
	 }
	 

}
 
void initTim2(int setPluseRate){
	
	 TimHandle2.Instance = TIM2;										   /* Select TIM1 */
//	 HAL_TIM_PWM_Stop(&TimHandle1, TIM_CHANNEL_1) ;
//	 HAL_TIM_PWM_Stop(&TimHandle1, TIM_CHANNEL_2) ;
	 /* Channel 1 outputs PWM */
//	 if (HAL_TIM_PWM_Stop(&TimHandle1, TIM_CHANNEL_1) != HAL_OK)
//	 {
//	   APP_ErrorHandler();
//	 }
//	 /* Channel 1 outputs PWM */
//	 if (HAL_TIM_PWM_Stop(&TimHandle1, TIM_CHANNEL_2) != HAL_OK)
//	 {
//	   APP_ErrorHandler();
//	 }
	//HAL_TIM_Base_DeInit(&TimHandle1);
	 TimHandle2.Init.Period 		   = PERIOD;						 /* Auto reload value： */
	 TimHandle2.Init.Prescaler		   = PRESCALER; 						 /* Prescaler:100-1 */
	 TimHandle2.Init.ClockDivision	   = TIM_CLOCKDIVISION_DIV1;		   /* Clock division: tDTS=tCK_INT */
	 TimHandle2.Init.CounterMode	   = TIM_COUNTERMODE_CENTERALIGNED3;//TIM_COUNTERMODE_UP;			   /* CounterMode:Up */
	 TimHandle2.Init.RepetitionCounter = 1 - 1; 						   /* repetition counter value:1-1 */
	 TimHandle2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;   /* TIM1_ARR register is not buffered */
	 /* Initialize TIM1 */
	 if (HAL_TIM_Base_Init(&TimHandle2) != HAL_OK)
	 {
	   APP_ErrorHandler();
	 }
	 
  	 uint32_t  cr1 =  TIM2->CR1;
	   TIM2->CR1 =   TIM2->CR1 | (0x3<<5);
//	
//	 // sBreakConfig.BreakState	   = TIM_BREAK_DISABLE;// TIM_BREAK_ENABLE; 					 /* Break input BRK is enabled */
//	 sBreakConfig.BreakState	  =  TIM_BREAK_ENABLE;						/* Break input BRK is enabled */
//	 sBreakConfig.DeadTime		   = 0; 								  /* Set the dead time */
//	 sBreakConfig.OffStateRunMode  = TIM_OSSR_ENABLE;						/* Off state in run mode: output enable */
//	 sBreakConfig.OffStateIDLEMode = TIM_OSSI_ENABLE;						/* IDLE state in run mode: output enable */
//	 sBreakConfig.LockLevel 	   = TIM_LOCKLEVEL_OFF; 					/* LOCK off */
//	 sBreakConfig.BreakPolarity    = TIM_BREAKPOLARITY_LOW; 				/* Break input BRK is active LOW */
//	 sBreakConfig.AutomaticOutput  = TIM_AUTOMATICOUTPUT_ENABLE;			/* Automatic output enable */
//	 /* Configures the Break feature, dead time */
//	  if (HAL_TIMEx_ConfigBreakDeadTime(&TimHandle2, &sBreakConfig) != HAL_OK)
//	 {
//	   APP_ErrorHandler();
//	 }
	 sPWMConfig.OCMode		 = TIM_OCMODE_PWM1; 									 /* Set as PWM1 mode */
	 sPWMConfig.OCPolarity	 = TIM_OCPOLARITY_HIGH; 								 /* Compare output polarity: HIGH */
	 sPWMConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;								 /* Compare complementary output polarity: HIGH */
	 sPWMConfig.OCIdleState  = TIM_OCNIDLESTATE_RESET; 								 /* Output Idle state: HIGH */
	 sPWMConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;								 /* Complementary output Idle state: LOW */
	 sPWMConfig.OCFastMode	 = TIM_OCFAST_DISABLE;									 /* Output Compare fast disable */
//	 sPWMConfig.Pulse = PLUSE;												  /* Duty cycle of channel 1 */ 										/* Duty cycle of channel 1 */

	 sPWMConfig.Pulse = setPluseRate * PERIOD / 100;			
	 if (HAL_TIM_PWM_ConfigChannel(&TimHandle2, &sPWMConfig, TIM_CHANNEL_3) != HAL_OK)/* Configure Channel 1 */
	 {
	   APP_ErrorHandler();
	 }	
	 sPWMConfig.Pulse = setPluseRate * PERIOD / 200;
//	 sPWMConfig.OCIdleState  = TIM_OCNIDLESTATE_RESET; 								 /* Output Idle state: HIGH */
//	 sPWMConfig.OCPolarity	 = TIM_OCPOLARITY_LOW; 								 /* Compare output polarity: HIGH */
//	 sPWMConfig.Pulse = PERIOD - ( setPluseRate * PERIOD / 100); 									/* Duty cycle of channel 1 */										  /* Duty cycle of channel 1 */
	 if (HAL_TIM_PWM_ConfigChannel(&TimHandle2, &sPWMConfig, TIM_CHANNEL_4) != HAL_OK)/* Configure Channel 1 */
	 {
	   APP_ErrorHandler();
	 } 
	   
	   __HAL_TIM_SET_COUNTER(&TimHandle1,0);

	 /* Enable TIM1 update interrupt */
	// __HAL_TIM_ENABLE_IT(&TimHandle1, TIM_IT_UPDATE);
	 
	 /* Channel 1 outputs PWM */
	 if (HAL_TIM_PWM_Start(&TimHandle2, TIM_CHANNEL_3) != HAL_OK)
	 {
	   APP_ErrorHandler();
	 }
	 /* Channel 1 outputs PWM */
	 if (HAL_TIM_PWM_Start(&TimHandle2, TIM_CHANNEL_4) != HAL_OK)
	 {
	   APP_ErrorHandler();
	 }
	  
	 /* Starts the TIM3 Output Compare signal generation. */
	 if (HAL_TIM_OC_Start(&TimHandle2, TIM_CHANNEL_3) != HAL_OK)
	 {
	   APP_ErrorHandler();
	 }
	 

}
 
 void set15Pluse(int pluseRate){
 	if(pluseRate < 95)
   	{ sPWMConfig.Pulse =  pluseRate * PERIOD15 /100;	}
	else
   	{ sPWMConfig.Pulse =  95 * PERIOD15 /100;	}
  	 if (HAL_TIM_PWM_ConfigChannel(&TimHandle15, &sPWMConfig, TIM_CHANNEL_1) != HAL_OK)/* Configure Channel 1 */
  	 {
  	   APP_ErrorHandler();
  	 }	
	 
	 /* Starts the TIM3 Output Compare signal generation. */
	 if (HAL_TIM_OC_Start(&TimHandle15, TIM_CHANNEL_1) != HAL_OK)
	 {
	   APP_ErrorHandler();
	 }
	 
	 /* Channel 1 outputs PWM */
	 if (HAL_TIM_PWM_Start(&TimHandle15, TIM_CHANNEL_1) != HAL_OK)
	 {
	   APP_ErrorHandler();
	 }
	 /* Channel 1N outputs PWM */
	 if (HAL_TIMEx_PWMN_Start(&TimHandle15, TIM_CHANNEL_1) != HAL_OK)
	 {
	   APP_ErrorHandler();
	 }
 	}
 void setPluse(int pluseRate){
 	
 // SEGGER_RTT_printf(0, "adc_value = %u calculate_current	   %u	pluseRate %d\r\n",adc_value, (int)(current * 1000),(int)(pluseRate));
 // 						 SEGGER_RTT_printf(0, "pluseRate = %d	= %d \r\n",pluseRate,pluseRate * PERIOD /100);
 
 //  
 // 	 HAL_TIM_OC_Stop(&TimHandle1,TIM_CHANNEL_1);
 // 	 HAL_TIM_OC_Stop(&TimHandle1,TIM_CHANNEL_2);
 // 	 HAL_TIM_Base_Stop(&TimHandle1);
 // 	 __HAL_TIM_SET_COUNTER(&TimHandle1,0);
//		 if(prePluse !=  sgdGetPwmPluse()){
	 
//	 uint16_t adc_value = read_adc_value();   // 读取ADC值
//	 float current = calculate_current(adc_value); // 计算实际电流
//						   SEGGER_RTT_printf(0, "pluseRate = %d calculate_current = %d	 \r\n",pluseRate * PERIOD /100,(int)(current * 1000));
//		 
//				 prePluse = sgdGetPwmPluse();
// 
//		 setGPIOA6BrakeLow();
//		 
//		initTim(prePluse);
//		setGPIOA6CompOut();
 // 	 sPWMConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;								  /* Output Idle state: HIGH */
 // 	 sPWMConfig.OCPolarity	  = TIM_OCPOLARITY_LOW;  
 // 	 sPWMConfig.Pulse = PERIOD - pluseRate * PERIOD /100;												  /* Duty cycle of channel 1 */  
 //
 // 	 if (HAL_TIM_PWM_ConfigChannel(&TimHandle1, &sPWMConfig, TIM_CHANNEL_1) != HAL_OK)/* Configure Channel 1 */
 // 	 {
 // 	   APP_ErrorHandler();
 // 	 }	
 //
 // 	 sPWMConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;								  /* Output Idle state: HIGH */
 // 	 sPWMConfig.OCPolarity	  = TIM_OCPOLARITY_HIGH ;	 
//  	 sPWMConfig.Pulse =  pluseRate * PERIOD /100;		 
//  	 if (HAL_TIM_PWM_ConfigChannel(&TimHandle15, &sPWMConfig, TIM_CHANNEL_1) != HAL_OK)/* Configure Channel 1 */
//  	 {
//  	   APP_ErrorHandler();
//  	 }	
//	 
//	 /* Starts the TIM3 Output Compare signal generation. */
//	 if (HAL_TIM_OC_Start(&TimHandle15, TIM_CHANNEL_1) != HAL_OK)
//	 {
//	   APP_ErrorHandler();
//	 }
//	 
//	 /* Channel 1 outputs PWM */
//	 if (HAL_TIM_PWM_Start(&TimHandle15, TIM_CHANNEL_1) != HAL_OK)
//	 {
//	   APP_ErrorHandler();
//	 }
//	 /* Channel 1N outputs PWM */
//	 if (HAL_TIMEx_PWMN_Start(&TimHandle15, TIM_CHANNEL_1) != HAL_OK)
//	 {
//	   APP_ErrorHandler();
//	 }
//			 }
// 
//  	 HAL_TIM_OC_Start(&TimHandle15,TIM_CHANNEL_1);
//  	 if (HAL_TIM_OC_Start(&TimHandle15, TIM_CHANNEL_1) != HAL_OK)
//  	 {
//  	 APP_ErrorHandler();
//  	 }
 // 	 HAL_TIM_OC_Start(&TimHandle1,TIM_CHANNEL_2);
 // 	 if (HAL_TIM_OC_Start(&TimHandle1, TIM_CHANNEL_2) != HAL_OK)
 // 	 {
 // 	 APP_ErrorHandler();
 // 	 }
 }

 void readyToFlash(COMP_HandleTypeDef *hcomp)
{
	SEGGER_RTT_printf(0, "readyToFlash \r\n");
 	compTick = HAL_GetTick( )+1;
	 setGPIOA6BrakeLow();
}

 void HAL_COMP_TriggerCallback(COMP_HandleTypeDef *hcomp)
{
	if(compTick == 0 ){
 	compTick = HAL_GetTick( )+1;
	 setGPIOA6BrakeLow();
//	SEGGER_RTT_printf(0, "HAL_COMP_TriggerCallback \r\n");
		}
}
void initCOMP1(){

	//   __HAL_SYSCFG_COMP1_BREAK_TIM1() ;
	//   __HAL_SYSCFG_COMP1_BREAK_TIM15() ;
	hcomp1.Instance = COMP1; 											 /* COMP1 */
	hcomp1.Init.InputMinus	   = COMP_INPUT_MINUS_IO11; 				 /* Negative input is VREF (1.2V) */
	hcomp1.Init.InputPlus	   = COMP_INPUT_PLUS_IO9;					 /* Positive input selection is PA0 */
	hcomp1.Init.OutputPol	   = COMP_OUTPUTPOL_INVERTED;			  /* COMP1 polarity Sexual selection is not reverse */
	hcomp1.Init.Mode 		   = COMP_POWERMODE_HIGHSPEED;				 /* COMP1 power consumption mode is selected as High speed mode */
	hcomp1.Init.Hysteresis	   = COMP_HYSTERESIS_ENABLE;				 /* Hysteresis function off */
	hcomp1.Init.WindowMode	   = COMP_WINDOWMODE_DISABLE;				 /* Window Mode Off */
	hcomp1.Init.TriggerMode	   = COMP_TRIGGERMODE_IT_RISING_FALLING;	 /* COMP1 rising/falling edge triggering */
	hcomp1.Init.DigitalFilter   = 200;										/* Disable DigitalFilter */
	//hcomp1.Init.VrefDiv		   = COMP_VREF_DIV_32_64VREF;				 /* Vrefcmp 32/64 */
		//hcomp1.Init.VrefDiv		   = COMP_VREF_DIV_19_64VREF;				 /* Vrefcmp 32/64 */
hcomp1.Init.VrefDiv		   = COMP_VREF_DIV_21_64VREF;				 /* Vrefcmp 32/64 */
	//hcomp1.Init.VrefDiv		   = COMP_VREF_DIV_42_64VREF;				 /* Vrefcmp 32/64 */
	hcomp1.Init.VrefSrc		   = COMP_VREF_SRC_VCCA;					 /* Vref select VCCA */  

	/* COMP1 initialization */
	if (HAL_COMP_Init(&hcomp1) != HAL_OK)								 
	{
	 APP_ErrorHandler();
	}
	SEGGER_RTT_printf(0, "HAL_COMP_Init \r\n");
	/* COMP1 Start */
	HAL_COMP_Start(&hcomp1); 
	SEGGER_RTT_printf(0, "HAL_COMP_Start \r\n");

}
void initPSFB(void)
{
  /* Reset of all peripherals, Initializes the Systick */

	SEGGER_RTT_printf(0, "initPSFB \r\n");
	//initExitIO();
	//SEGGER_RTT_printf(0, "initExitIO end \r\n");
	initTim(40);
	initTim15(30);
	initTim2(80);
	SEGGER_RTT_printf(0, "initTim end \r\n");
	//setShipParse(50);
	
	SEGGER_RTT_printf(0, "setShipParse end \r\n");

	initCOMP1();

	SEGGER_RTT_printf(0, "initCOMP1 end \r\n");

	 
//	/* Infinite loop*/
//	while (1)
//	{
//		if(compTick != 0 && ((HAL_GetTick( ) - compTick) > 500)){
//			compTick = 0 ;
//			setGPIOA6CompOut();
//			SEGGER_RTT_printf(0, "setGPIOA6CompOut \r\n");
//			}
//		 // SEGGER_RTT_printf(0, "\r compTick  = %d  HAL_GetTick( ) =%d   diff =%d  \r\n",compTick,HAL_GetTick( ),HAL_GetTick( )-compTick );
//			
//	}

}
void psfbcheck()
{
	if(compTick != 0 && ((HAL_GetTick( ) - compTick) > 1000)){
				compTick = 0 ;
				setGPIOA6CompOut();
				//SEGGER_RTT_printf(0, "setGPIOA6CompOut \r\n");
				}

}
/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
