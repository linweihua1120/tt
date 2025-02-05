


#include "KTH57XXIIC.h"
#include "MyI2C.h"

#include "main.h"
#include "string.h"
#include "sgd.h"
#include "SEGGER_RTT.h"
//#include "py32f0xx_hal.h"
//#include "py32f0xx_hal_adc.h"
#include "py32f072xx_Start_Kit.h" 

 
ADC_HandleTypeDef        AdcHandle;
uint16_t                 aADCxConvertedData;
uint16_t                 aADC_CURRENT;
uint16_t                 aADC_HV;
uint16_t                 aADC_BAT;
uint16_t                 aADC_TMP;
#define NUM_SAMPLES 5 

int currentCH = 0 ; //0: VBAT  1:V1V5
// 数据缓冲区
static uint16_t adcBuffer[NUM_SAMPLES] = {0}; // 存储最近的采样值
static uint8_t index = 0;                    // 当前索引位置
static uint8_t count = 0;                    // 已采集样本数

void sgdAdcSetChannle(int ch);
/**
 * 更新 ADC 缓冲区并计算最近5次采样的平均值
 * @param newAdcValue 新采样值
 * @return 最近5次采样的平均值
 */
uint16_t updateAdcAndGetAverage(uint16_t newAdcValue) {
    // 将新采样值存入缓冲区
    adcBuffer[index] = newAdcValue;
    
    // 更新索引，确保循环覆盖缓冲区
    index = (index + 1) % NUM_SAMPLES;
    
    // 增加采样计数，但不能超过缓冲区大小
    if (count < NUM_SAMPLES) {
        count++;
    }

    // 计算平均值
    uint32_t sum = 0;
    for (uint8_t i = 0; i < count; i++) {
        sum += adcBuffer[i];
    }

    return (uint16_t)(sum / count);
}

/**
  * @brief  ADC Conversion Callback.
  * @param  hadc：ADC handle
  * @retval None
  */
void APP_ErrorHandler()
{
	
}
//void sgdSetHV(){
//	aADC_HV = 100;
//
//}
int sgdGetADCCurrentres(void )
{
//		SEGGER_RTT_printf(0, "Channel4 : %u\r\n", (unsigned int)aADCxConvertedData);
	return  (unsigned int)(aADC_CURRENT ) ;
}
int sgdGetADCCurrentresmA(void )
{
//		SEGGER_RTT_printf(0, "Channel4 : %u\r\n", (unsigned int)aADCxConvertedData);
	return  (unsigned int)((aADC_CURRENT  * 5000 *4) /4096) ;
}
int sgdGetADCres(void )
{
		//SEGGER_RTT_printf(0, "vbat : %u\r\n", (unsigned int)((aADCxConvertedData * 4700) /4096));
	return  (unsigned int)((aADCxConvertedData * 4700) /4096) ;
}
int sgdGetADCTempresINT()
{
		//SEGGER_RTT_printf(0, "vbat : %u\r\n", (unsigned int)((aADCxConvertedData * 4700) /4096));
	return  (unsigned int)(aADCxConvertedData) ;
}

int sgdGetADCTempres(void )
{
		//SEGGER_RTT_printf(0, "vbat : %u\r\n", (unsigned int)((aADCxConvertedData * 4700) /4096));
	return  (unsigned int)((aADCxConvertedData * 5000) /4096) ;
}
int sgdGetADCBat(void )
{
		//SEGGER_RTT_printf(0, "vbat : %u\r\n", (unsigned int)((aADCxConvertedData * 4700) /4096));
	return  (unsigned int)((aADC_BAT * 5000 *2) /4096) ;
}


int sgdGetADCHV(void )
{
		//SEGGER_RTT_printf(0, "vbat : %u\r\n", (unsign ed int)((aADCxConvertedData * 4700) /4096));
	return  (unsigned int)((aADC_HV * 5000) /4096) ;
}
int sgdGetADCHVmV(void )
{
		//SEGGER_RTT_printf(0, "vbat : %u\r\n", (unsign ed int)((aADCxConvertedData * 4700) /4096));
	return  (unsigned int)((aADC_HV * 5 * 200) /4096 ) ;
}
int sgdGetADC1v5res(void )
{
return 1500;
//		SEGGER_RTT_printf(0, "Channel4 : %u\r\n", (unsigned int)aADCxConvertedData);
	return    (unsigned int)((aADC_HV * 4700) /4096) ;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
  int now  = getTick()/10;
  
  aADCxConvertedData =  ( HAL_ADC_GetValue(hadc));
//		SEGGER_RTT_printf(0, "adc %d  : %u  tick = %d ,isCharging() = %d\r\n",currentCH, (unsigned int)aADC_CURRENT,getTick(),isCharging());
  if((now / 10)%10 == 9 ){}
 	if(currentCH == 1){
		aADC_HV = aADCxConvertedData;
 		}
 	else if(currentCH == 0){
		aADC_CURRENT = aADCxConvertedData;
 		}
 	else if(currentCH == 4){
		aADC_BAT = aADCxConvertedData;
 		}
 	else{
		aADC_TMP = aADCxConvertedData;
 		}
  	
 // regulate_current_no_integral( ) ;
 	//SEGGER_RTT_printf(0, "index =\t %d\t currentCH =\t%d\t  HV = %d \t  BAT = %d \t TMP = %d \t aADCxConvertedData : %u  \t tick =%u\r\n",now % 30	 ,currentCH,sgdGetADCHV( ),sgdGetADCBat( ),sgdGetADCTemp( ), (unsigned int)aADCxConvertedData,getTick());
//	if(isCharging()){
//		currentCH = 0;
//		}
// 	else
		if(currentCH == 9){
		currentCH = 1;
	sgdAdcSetChannle(currentCH);
    HAL_ADC_Start_IT(&AdcHandle);

  	}
	else if(currentCH == 1 ){
		currentCH = 4;
	sgdAdcSetChannle(currentCH);
    HAL_ADC_Start_IT(&AdcHandle);
		}
	else if(currentCH == 4){
		currentCH = 0;
	sgdAdcSetChannle(currentCH);
    HAL_ADC_Start_IT(&AdcHandle);
		}
	else{
		currentCH = 9;
	sgdAdcSetChannle(currentCH);
		}
//	if(currentCH == 0 ){
//	
//  aADCxConvertedData = HAL_ADC_GetValue(hadc);
//  regulate_current( ) ;
//		}
//	else{
//  aADC_HV = HAL_ADC_GetValue(hadc);
//		}
}

void sgdAdcSetChannle(int ch){
		ADC_ChannelConfTypeDef   sConfig={0};  
		sConfig.Channel      = ch;
		sConfig.Rank         = ADC_REGULAR_RANK_1;
		sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;
		/* ADC channel configuration */
		if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
		{
		APP_ErrorHandler();
		}
}

/**
  * @brief  ADC configuration function.
  * @param  None
  * @retval None
  */
  void sgdADCInit(void)
{   
  ADC_ChannelConfTypeDef   sConfig={0};  
  RCC_PeriphCLKInitTypeDef RCC_PeriphCLKInit={0};
  
    /* Enable ADC clock */
  __HAL_RCC_ADC_CLK_ENABLE();
	
  GPIO_InitTypeDef GPIO_InitStruct={0};
  __HAL_RCC_GPIOB_CLK_ENABLE();                               /* GPIOA clock enable */
 
//  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = PIN_ADC_BAT;
  HAL_GPIO_Init(PORT_ADC_BAT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = PIN_ADC_HV;
  HAL_GPIO_Init(PORT_ADC_HV, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = PIN_ADC_TMP;
  HAL_GPIO_Init(PORT_ADC_TMP, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = PIN_ADC_CURRENT;
  HAL_GPIO_Init(PORT_ADC_CURRENT, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(ADC_COMP_IRQn, 0, 0); /* Interrupt priority setting */
  HAL_NVIC_EnableIRQ(ADC_COMP_IRQn);         /* ADC interrupt enable */
  
  RCC_PeriphCLKInit.PeriphClockSelection= RCC_PERIPHCLK_ADC;
  RCC_PeriphCLKInit.ADCClockSelection   = RCC_ADCCLKSOURCE_PCLK_DIV4;
  HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInit);
  
  AdcHandle.Instance = ADC1;
  
  AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;             /* 12-bit resolution for converted data */
  AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;            /* Right-alignment for converted data */
  AdcHandle.Init.ScanConvMode          = ADC_SCAN_DISABLE;               /* Scan mode off */
  AdcHandle.Init.ContinuousConvMode    = DISABLE;                        /* Single mode */
  AdcHandle.Init.NbrOfConversion       = 1;                              /* Number of conversion channels 1 */
  AdcHandle.Init.DiscontinuousConvMode = DISABLE;                        /* Discontinuous mode not enabled */
  AdcHandle.Init.NbrOfDiscConversion   = 1;                              /* Discontinuous mode short sequence length is 1 */
  AdcHandle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;             /* software trigger */
  /* ADC initialization */
  if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
  {
    APP_ErrorHandler();
  }
 
  sConfig.Channel      = CHANNEL_ADC_BAT;
  sConfig.Rank         = ADC_REGULAR_RANK_5;
  sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;
  /* ADC channel configuration */
  if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  sConfig.Channel      = CHANNEL_ADC_HV;
  sConfig.Rank         = ADC_REGULAR_RANK_2;
  sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;
  /* ADC channel configuration */
  if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  sConfig.Channel      = CHANNEL_ADC_CURRENT;
  sConfig.Rank         = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;
  /* ADC channel configuration */
  if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  sConfig.Channel      = CHANNEL_ADC_TMP;
  sConfig.Rank         = ADC_REGULAR_RANK_10;
  sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES_5;
  /* ADC channel configuration */
  if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
  {
    APP_ErrorHandler();
  }
  /* ADC calibration */
  if(HAL_ADCEx_Calibration_Start(&AdcHandle) != HAL_OK)
  {
    APP_ErrorHandler();
  }

  sgdAdcSetChannle(1);
}
void sgdReadAdc(void)
{
	//SEGGER_RTT_printf(0, "sgdReadAdc\r\n");
//currentCH = 1;
    HAL_ADC_Start_IT(&AdcHandle);

}

int sgdGetBatteryV(void )
{
	 return 3800;
		//SEGGER_RTT_printf(0, "sgdGetBatteryV : %u   vol =  %d mv\r\n", (unsigned int)aADCxConvertedData,(unsigned int)((aADCxConvertedData * 4600) /4096));
	return (unsigned int) ((aADCxConvertedData * 4860) /4096 );
}


