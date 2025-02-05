


#include "KTH57XXIIC.h"
#include "MyI2C.h"

#include "main.h"
#include "string.h"
#include "sgd.h"
#include "SEGGER_RTT.h"
//#include "py32f0xx_hal.h"
//#include "py32f0xx_hal_adc.h"
#include "py32f072xx_Start_Kit.h" 
 
TIM_HandleTypeDef    TimHandle3;
TIM_HandleTypeDef    TimHandle14;
TIM_OC_InitTypeDef sConfig;
void sgdSetBatteryLedPluse(int pluseRate){
sConfig.Pulse = pluseRate;												 /* TIM1_CCR2 value:20,duty cycle:20/50=40% */
 /* Initializes the TIM PWM channel 2 */
 if (HAL_TIM_PWM_ConfigChannel(&TimHandle3, &sConfig, TIM_CHANNEL_1) != HAL_OK)
 {
   APP_ErrorHandler();
 }
	
//	if (HAL_TIM_PWM_Start(&TimHandle3, TIM_CHANNEL_2) != HAL_OK)
//	{
//	  APP_ErrorHandler();
//	}
	
	if (HAL_TIMEx_PWMN_Start(&TimHandle3, TIM_CHANNEL_1) != HAL_OK)
	{
	  APP_ErrorHandler();
	}


}


void sgdBatteryLedConfige(int pluseRate){

	
	
  TimHandle3.Instance = TIM17;                                                  /* Select TIM1 */
  TimHandle3.Init.Period            = 1000;                                      /* Auto reload value： */
  TimHandle3.Init.Prescaler         = 4;                                 /* Prescaler:800-1 */
  TimHandle3.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;                  /* Clock division: tDTS=tCK_INT */
  TimHandle3.Init.CounterMode       = TIM_COUNTERMODE_UP;                      /* CounterMode:Up */
  TimHandle3.Init.RepetitionCounter = 1000;                                   /* repetition counter value:1-1 */
  TimHandle3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;          /* TIM1_ARR register is not buffered */
  /* Initializes the TIM PWM Time Base */
  if (HAL_TIM_PWM_Init(&TimHandle3) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  
  sConfig.OCMode       = TIM_OCMODE_PWM1;                                     /* Set as PWM1 mode */
  sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;                                 /* OC channel active high */
  sConfig.OCFastMode   = TIM_OCFAST_DISABLE;                                  /* Output Compare fast disable */
  sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;                                /* OCN channel active high */
  sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;                              /* OC1N channel idle state is low level */
  sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;                               /* OC1 channel idle state is low level */

//  sConfig.Pulse = PULSE1_VALUE;                                               /* TIM1_CCR1 value:10，duty cycle:10/50=20% */
//  /* Initializes the TIM PWM channel 1 */
//  if (HAL_TIM_PWM_ConfigChannel(&TimHandle3, &sConfig, TIM_CHANNEL_1) != HAL_OK)
//  {
//    APP_ErrorHandler();
//  }
  sConfig.Pulse = pluseRate;                                               /* TIM1_CCR2 value:20,duty cycle:20/50=40% */
  /* Initializes the TIM PWM channel 2 */
  if (HAL_TIM_PWM_ConfigChannel(&TimHandle3, &sConfig, TIM_CHANNEL_1) != HAL_OK)
  {
    APP_ErrorHandler();
  }
//  sConfig.Pulse = PULSE3_VALUE;                                               /* TIM1_CCR3 value:30,duty cycle:30/50=60% */
//  /* Initializes the TIM PWM channel 3 */
//  if (HAL_TIM_PWM_ConfigChannel(&TimHandle3, &sConfig, TIM_CHANNEL_3) != HAL_OK)
//  {
//    /* Configuration Error */
//    APP_ErrorHandler();
//  }
//  sConfig.Pulse = PULSE4_VALUE;                                               /* TIM1_CCR4 value:40,duty cycle:40/50=80% */
//  /* Initializes the TIM PWM channel 4 */
//  if (HAL_TIM_PWM_ConfigChannel(&TimHandle3, &sConfig, TIM_CHANNEL_4) != HAL_OK)
//  {
//    APP_ErrorHandler();
//  }
  
  /* Starts the all channel PWM signal generation. */
//  if (HAL_TIM_PWM_Start(&TimHandle3, TIM_CHANNEL_1) != HAL_OK)
//  {
//    APP_ErrorHandler();
//  }
//  if (HAL_TIM_PWM_Start(&TimHandle3, TIM_CHANNEL_2) != HAL_OK)
//  {
//    APP_ErrorHandler();
//  }
//  if (HAL_TIM_PWM_Start(&TimHandle3, TIM_CHANNEL_3) != HAL_OK)
//  {
//    APP_ErrorHandler();
//  }
//  if (HAL_TIM_PWM_Start(&TimHandle3, TIM_CHANNEL_4) != HAL_OK)
//  {
//    APP_ErrorHandler();
//  }
//	
  if (HAL_TIMEx_PWMN_Start(&TimHandle3, TIM_CHANNEL_1) != HAL_OK)
  {
    APP_ErrorHandler();
  }


}
int ip5306State = 0;
int sgdIsIp5306TimerStop(){
	return ip5306State == 1;
}
void sgdStop5306KeyTimer(){
	ip5306State = 0;

	HAL_TIM_Base_Stop(&TimHandle14);
}
void sgdIp5306KeyConfige(){
  ip5306State = 1;

	
	
  TimHandle14.Instance = TIM14;                                                  /* Select TIM1 */
  TimHandle14.Init.Period            = 1500;                                      /* Auto reload value： */
  TimHandle14.Init.Prescaler         = 48000;                                 /* Prescaler:800-1 */
  TimHandle14.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;                  /* Clock division: tDTS=tCK_INT */
  TimHandle14.Init.CounterMode       = TIM_COUNTERMODE_UP;                      /* CounterMode:Up */
  TimHandle14.Init.RepetitionCounter = 1000;                                   /* repetition counter value:1-1 */
  TimHandle14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;          /* TIM1_ARR register is not buffered */
  /* Initializes the TIM PWM Time Base */
  if (HAL_TIM_PWM_Init(&TimHandle14) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  
  sConfig.OCMode       = TIM_OCMODE_PWM1;                                     /* Set as PWM1 mode */
  sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;                                 /* OC channel active high */
  sConfig.OCFastMode   = TIM_OCFAST_DISABLE;                                  /* Output Compare fast disable */
  sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;                                /* OCN channel active high */
  sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;                              /* OC1N channel idle state is low level */
  sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;                               /* OC1 channel idle state is low level */

//  sConfig.Pulse = PULSE1_VALUE;                                               /* TIM1_CCR1 value:10，duty cycle:10/50=20% */
//  /* Initializes the TIM PWM channel 1 */
//  if (HAL_TIM_PWM_ConfigChannel(&TimHandle3, &sConfig, TIM_CHANNEL_1) != HAL_OK)
//  {
//    APP_ErrorHandler();
//  }
  sConfig.Pulse = 200;                                               /* TIM1_CCR2 value:20,duty cycle:20/50=40% */
  /* Initializes the TIM PWM channel 2 */
  if (HAL_TIM_PWM_ConfigChannel(&TimHandle14, &sConfig, TIM_CHANNEL_1) != HAL_OK)
  {
    APP_ErrorHandler();
  }
//  sConfig.Pulse = PULSE3_VALUE;                                               /* TIM1_CCR3 value:30,duty cycle:30/50=60% */
//  /* Initializes the TIM PWM channel 3 */
//  if (HAL_TIM_PWM_ConfigChannel(&TimHandle3, &sConfig, TIM_CHANNEL_3) != HAL_OK)
//  {
//    /* Configuration Error */
//    APP_ErrorHandler();
//  }
//  sConfig.Pulse = PULSE4_VALUE;                                               /* TIM1_CCR4 value:40,duty cycle:40/50=80% */
//  /* Initializes the TIM PWM channel 4 */
//  if (HAL_TIM_PWM_ConfigChannel(&TimHandle3, &sConfig, TIM_CHANNEL_4) != HAL_OK)
//  {
//    APP_ErrorHandler();
//  }
  
  /* Starts the all channel PWM signal generation. */
//  if (HAL_TIM_PWM_Start(&TimHandle3, TIM_CHANNEL_1) != HAL_OK)
//  {
//    APP_ErrorHandler();
//  }
//  if (HAL_TIM_PWM_Start(&TimHandle3, TIM_CHANNEL_2) != HAL_OK)
//  {
//    APP_ErrorHandler();
//  }
//  if (HAL_TIM_PWM_Start(&TimHandle3, TIM_CHANNEL_3) != HAL_OK)
//  {
//    APP_ErrorHandler();
//  }
//  if (HAL_TIM_PWM_Start(&TimHandle3, TIM_CHANNEL_4) != HAL_OK)
//  {
//    APP_ErrorHandler();
//  }
//	
  if (HAL_TIM_PWM_Start(&TimHandle14, TIM_CHANNEL_1) != HAL_OK)
  {
    APP_ErrorHandler();
  }


}


