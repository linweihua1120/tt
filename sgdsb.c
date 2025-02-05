
#include "KTH57XXIIC.h"
#include "MyI2C.h"
#include "ADC.h"

#include "OnePluseFlash.h"
#include "main.h"
#include "BatteryLed.h"

#include "string.h"
#include "sgd.h"
#include "SEGGER_RTT.h"
//#include "py32f0xx_hal.h"
//#include "py32f0xx_hal_adc.h"
#include "py32f072xx_Start_Kit.h" 

//#define demoDeug 1 
uint8_t sMode  = 0;//S1 S2 选择  1:S1 2:S2  3:OFF

uint8_t powerLevel = 0;//闪光灯手动模式功率等级  0 - 6
// 转轮顺序 

uint8_t flashPowerArrayOrder[8] = {3,4,1,5,2,0,2,6}  ;//{0,4,6,7,3,5,2};

uint32_t flashPowerArray[8] = {3111,284,144,83,49,32,22,5000 };//每个功率等级IGBT门级脉冲持续的时间 单位 us

//uint32_t flashPowerArray[8] = {3111,3111,3111,3111,3111,3111,3111,5000 };

int switchState = 0 ; //0-关机  1-手动  2-ttl
int ip5306L1State = 0 ; //0-灭  1-亮  2-闪烁

int flashTick  = 10;//最后一次闪光的时间戳
int userOperateTick  = 10;//用户最后一次操作的时间戳
 int _chgTick = 1;//电容上一次充电的时间戳
 int _swTick = 1;//最后yi检测到sw的时间戳
 int _batteryLowTick = 1;//最后电压高于3600mv的时间戳
 
int usbHostConnected = 0;
int usbInsertion = 0;
int lowBatteryState = 0;//电池电压低 



  void sgdOnePulseConfig(int pluseUS);
//UART  
//uint8_t aTxBuffer[] = "UART Test1111111111111111111111111111111111111111111111111111111";
//uint8_t aRxBuffer[30];

//__IO ITStatus UartReady = RESET;

//int _l1RiseTick = 1;
//int _l1FallTick = 2;
//
//int _keyRiseTick = 1;
//int _keyFallTick = 1;
uint32_t _oneFlashTick = 1;
uint32_t _igbtGateTick = 1;
uint32_t _flashInTick = 1;
uint32_t _oneFlashCount = 1;
uint32_t _igbtGateCount = 1;
uint32_t _flashInCount = 1;




void sgdBatteryLedConfige(int pluseRate);
int timeGone(unsigned int rtcPre)
{
	unsigned int tick = getTick();
	if(rtcPre==0)
		return 0;
	if(tick < rtcPre)
		{
			return 0xFFFFFFFF- rtcPre + tick;
		}
	else 
		return tick-rtcPre;
}

void sgdSleepCheck(){
	
			if (((IS_HIGH_USB_DEC))&&(timeGone(userOperateTick ) > 900000))
				{
	 				SEGGER_RTT_printf(0, "userOperateTick  sgdEnterStop\r\n");
					//未插入USB 且超时进入休眠
				HAL_NVIC_SystemReset( );
//					Delay_ms(1000);
					//sgdEnterStop( );
					
				}
}


int getSMode(){
	return sMode;
};
void Delay_ms(int delayMS){

	int now = 1 ;
	now = getTick();
	if(now == 0 )
	{now = 1;}
	while (timeGone(now) < delayMS)
		{
		;
		}

}

  void sgdDisableSwInt(){
  uint32_t position = 12;
  uint32_t iocurrent;
  uint32_t tmp;
      tmp = EXTI->EXTICR[position >> 2u];
      tmp &= (0x0FuL << (8u * (position & 0x03u)));
//      if (tmp == (GPIO_GET_INDEX(PIN_SW_DEC) << (8u * (position & 0x03u))))
//      {	}
        /* Clear EXTI line configuration */
        EXTI->IMR &= ~(iocurrent);
        EXTI->EMR &= ~(iocurrent);

        /* Clear Rising Falling edge configuration */
        EXTI->RTSR &= ~(iocurrent);
        EXTI->FTSR &= ~(iocurrent);

        tmp = 0x0FuL << (8u * (position & 0x03u));
        EXTI->EXTICR[position >> 2u] &= ~tmp;
      
}
void sgdEnableSwInt(){
  GPIO_InitTypeDef  GPIO_InitStruct = {0};         
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  /* The speed is high */
  GPIO_InitStruct.Mode  = GPIO_MODE_IT_RISING;  /* GPIO mode is a falling edge interrupt */
  GPIO_InitStruct.Pull  = GPIO_PULLUP;           /* pull up */
  GPIO_InitStruct.Pin = PIN_SW_DEC;
  HAL_GPIO_Init(PORT_SW_DEC, &GPIO_InitStruct);

}

void sgdMakeCHGRise(){

  	sgdEnableSwInt();
	_swTick = getTick();
	RESET_CHG;
	SET_CHG;

}

int _pwmTick = 1;
int  pwm15Rate = 0 ;

int ip5306HighLevelCount = 0;
int ip5306LowLevelCount = 0;
int batteryState = BatteryState_25;
void sgdReadIP5306(){
	static int _ip5306Tick2S = 1;
	static int _ip5306Tick10ms = 1;
	static int _vol_battery = 1;
//	if((!(IS_HIGH_CHG)) == 1  || 1 == (!(IS_HIGH_XFULL)) ){
//		_vol_battery  =  sgdGetBatteryV();
//		}
//		//SEGGER_RTT_printf(0, "\r\sgdReadIP5306   \r\n"  );
//	if(timeGone(_ip5306Tick10ms) > 10 ){
//		_ip5306Tick10ms = getTick();
//		if(timeGone(_ip5306Tick2S) > 500 ){
//			_ip5306Tick2S = getTick();
//			 
//		int ip5306State  = sgdGetIp5306State() ;
//		
//		if(ip5306State == 3){
//			batteryState = BatteryState_ChargeFull;
//			}
//		else if(ip5306State == 1){
//			batteryState = BatteryState_Chargineg;
//			}
//		else	{
//				//	放电
//				if(_vol_battery > 4100){
//					batteryState = BatteryState_100;
//					}
//				else if(_vol_battery > 4000){
//					batteryState = BatteryState_75;
//					}
//				else if(_vol_battery> 3800){
//					batteryState = BatteryState_50;
//					}
//				else{
//					batteryState = BatteryState_25;
//					}
//			}
//		
//		}
//	}
}

static void sgdEnterStop(void)
{
return ;
		SEGGER_RTT_printf(0, "\r\sgdEnterStop   \r\n"  );
			
 	__HAL_RCC_GPIOA_FORCE_RESET();
	__HAL_RCC_GPIOB_FORCE_RESET();
	__HAL_RCC_GPIOC_FORCE_RESET();
	__HAL_RCC_GPIOF_FORCE_RESET();
	__HAL_RCC_FLASH_CLK_DISABLE();
	__HAL_RCC_TIM3_FORCE_RESET() ;
	__HAL_RCC_TIM1_FORCE_RESET() ;
	__HAL_RCC_TIM14_FORCE_RESET() ;
	__HAL_RCC_TIM17_FORCE_RESET() ;
	__HAL_RCC_SPI1_FORCE_RESET();
	__HAL_RCC_ADC_FORCE_RESET();

	HAL_PWR_EnterSLEEPMode(PWR_SLEEPENTRY_WFI);
 
//	HAL_PWR_EnterSTOPMode(0, 0);

// while (1)
//	 {
//	 ;
//	 }
  /* Request Wait For Event */
//   __SEV();
//   __WFE();
//   __WFE();

}
int _inputTick = 1;
int _inputADCTick = 1;
void sgdGetInput(){
	static int preFlashPower = 1;
	static int presMode = 1;
	static int preUSBDec = 1;
	
	
	if(timeGone(_inputTick) > 50){
//		 		SEGGER_RTT_printf(0, "_inputTick = %d  tick  = %d gone = %d\r\n",_inputTick,getTick(),timeGone(_inputTick)  );
		_inputTick = getTick();
		
	//电容没充满 不检查
//	if(!(IS_HIGH_XFULL)   )
//		{}
	sgdGetADCres();
	sgdReadAngle(  );

	if(sgdGetADCres( ) > MIN_WORK_BATTERY_VOL){
		_batteryLowTick = getTick();
			lowBatteryState = 0;
		}
	if(timeGone(_batteryLowTick)  > 4000){
			lowBatteryState = 1;
		}
	switchState  = IS_HIGH_SWITCH_DEC ? SwitchState_Off :SwitchState_On;
	powerLevel = sgdGetPowerLevel() ;
		
	usbInsertion = IS_HIGH_USB_DEC ? 1 : 0 ;
	if(usbInsertion == 0){
		usbHostConnected = 0 ;
		}
//		SEGGER_RTT_printf(0, "sgdGetInput: %d %d  %d  \r\n",   powerLevel ,presMode,flashPowerArray[flashPowerArrayOrder[powerLevel]] );
	if(!IS_HIGH_S2){
		sMode = SMode_S1;
		}
	else if(!IS_HIGH_S1){
		sMode = SMode_S2;
//		sgdEnterStop( );
		}
	else{
		sMode = SMode_Off;
		}
	
//	INIT_INPUT_TTL_M;	
//	INIT_INPUT_CAMERA;	
//	INIT_INPUT_USB_DEC;
//	INIT_INPUT_SWITCH_DEC;		
//	INIT_INPUT_UP_S1;			
//	INIT_INPUT_UP_S2;	
	//SEGGER_RTT_printf(0, "sgdGetInput: S1 = %d S2= %d   SWITCH_DEC = %d  USB_DEC = %d IS_HIGH_TTL_M = %d IS_HIGH_CAMERA = %d \r\n", IS_HIGH_S1,IS_HIGH_S2, IS_HIGH_SWITCH_DEC,IS_HIGH_USB_DEC,IS_HIGH_TTL_M ,IS_HIGH_CAMERA );
	if(preFlashPower != powerLevel || usbInsertion != preUSBDec || sMode != presMode){
				SEGGER_RTT_printf(0, "\r\nsgdGetInput:powerLevel =%d 5306state = %d vbat = %dmv v1_5 = %dmv batState = %d \r\n operateTick = %d s1 = %d s2 = %d xfull =  %d switch = %d usb = %d \r\n",powerLevel,1,sgdGetADCres( ),sgdGetADC1v5res( ),batteryState,timeGone( userOperateTick)  , IS_HIGH_S1,IS_HIGH_S2,1 , IS_HIGH_SWITCH_DEC ,usbHostConnected );
				userOperateTick = getTick();
				preFlashPower = powerLevel;
				preUSBDec = usbInsertion;
				presMode = sMode;
				if(powerLevel >=0 && powerLevel < 7)
					{
						//setShipParse(powerLevel * 100 / 6);
  					sgdOnePulseConfig(flashPowerArray[powerLevel]);
					}
				else{
						setShipParse(0);
  					sgdOnePulseConfig(100);

					}
						}
		}
}

//void release(){
//	SEGGER_RTT_printf(0, "release");

////	RESET_CHG;
////	SET_F_EN;
////	SET_F_ON;
//}

//void sgdChargeLogic(){
////			SET_F_EN;
////			if(timeGone(chgTick)> 5000){
////										chgTick = getTick();
////										sgdMakeCHGRise();
////									}
////								if(!IS_HIGH_XFULL){
////										//		充满了
////										SET_FULL_LED;
////										RESET_CHG_LED
////								
////									}
////								else {
////										//		充电中
////										SET_CHG_LED;
////										RESET_FULL_LED;
////									}

////			if(flashPowerArrayOrder[powerLevel] == 9){
////					release();
////				}
////			else{
////					
////				}

//}
//void enterSleepMode(){
////1秒内2次短按 IP5306_KEY键
//	  int tick = getTick()%400;
//// 		SEGGER_RTT_printf(0, "enterSleepMode  %d\r\n",tick);
////	if(tick >100) 
////		{
////			SET_IP5306_KEY;
////		}
////	else{
////			RESET_IP5306_KEY;
////		}
////	SEGGER_RTT_printf(0, " enterSleepMode \r\n" );

//	//RESET_POWER_SWITCH;
////	RESET_F_EN;
//}
//void exitSleepMode(){
//// IP5306_KEY键   按键持续时间长于 50ms，但小于 2s， 即为短按动作，短按会打开电量显示灯和升压输出。
////开1.5秒 关1.5妙
////			RESET_IP5306_KEY;
//	  int tick = getTick()%3000;
//// 		SEGGER_RTT_printf(0, "exitSleepMode  %d\r\n",tick);
////	if( tick > 2500  ) 
////		{
////			RESET_IP5306_KEY;
////// 		SEGGER_RTT_printf(0, "exitSleepMode SET_IP5306_KEY %d\r\n",tick);
////		}
////	else{
////			SET_IP5306_KEY;
////// 		SEGGER_RTT_printf(0, "exitSleepMode RESET_IP5306_KEY %d\r\n",tick);
////		}
////	SEGGER_RTT_printf(0, " exitSleepMode \r\n" );
//	//SET_POWER_SWITCH;
////	SET_F_EN;
//}
//void sgdPowerControl(){
//		if(getTick()%100 ==0){
////		SEGGER_RTT_printf(0, "IS_HIGH_ADB  %d\r\n",IS_HIGH_ADB);
// 					  exitSleepMode();
////				if( isSleep == 0){
//// 					  exitSleepMode();
////					}
////				else{
////					enterSleepMode();
////					}
//			}
//}
//void sgdFONTrigCtrl(){

////	if(timeGone(flashTick) > 50  && !(IS_HIGH_FLASHH_BT)){
//////	 SEGGER_RTT_printf(0, "sgdFONTrigCtrl  %d\r\n",getTick());
////			SET_F_ON_TRIG;
////			SET_CHG;
////		}
//}
//void sgdLoop(){
//	while(1){
//			sgdChargeLogic();
//			sgdFONTrigCtrl();

//			sgdGetInput();
// 			sgdPowerControl();
//			if(_tick>5000){

//				}
//		if(_tick % 1000 == 0){
////			SEGGER_RTT_printf(0, "sgdLoop\r\n");
////				SEGGER_RTT_printf(0, "HAL_GPIO_ReadPin: %d %d %d %d %d %d %d %d\r\n", IS_HIGH_BAT_FULL,IS_HIGH_POWERA,IS_HIGH_POWERB,IS_HIGH_POWERC,IS_HIGH_S1,IS_HIGH_S2,IS_HIGH_SWITCH_DEC,IS_HIGH_XFULL); 
//				if((_tick & 1000 ) > flashPowerArrayOrder[powerLevel]*100){
////					SEGGER_RTT_printf(0, "GPIO_PIN_RESET\r\n");
////					 SET_F_EN;				
////					 SET_F_ON;				
////					 SET_CHG;						
////					 SET_CHG_LED;					
////					 SET_FULL_LED;				
////					 SET_BAT_LED;					
////					 SET_POWER_SWITCH;	
//					 
////					 SET_LED;		
//					}
//				else{
////					SEGGER_RTT_printf(0, "GPIO_PIN_SET\r\n");
////					 RESET_F_EN;				
////					 RESET_F_ON;				
////					 RESET_CHG;						
////					 RESET_CHG_LED;					
////					 RESET_FULL_LED;				
////					 RESET_BAT_LED;					
////					 RESET_POWER_SWITCH;	
//					 
////					 RESET_LED;	
//					}
//			}
//		
//		//SEGGER_RTT_printf(0, "SystemCoreClock: %ld\r\n", 33);
//	}
//}


 void sgd1msConfig(void)
{
	NVIC_SetPriority(SysTick_IRQn, 0);
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}
void sgdIOConfig(GPIO_TypeDef 			*GPIOx, uint32_t pin,uint32_t mode, uint32_t pull  )
 {
	 
  GPIO_InitTypeDef  GPIO_InitStruct;

  /* Enable the GPIO_LED Clock */

  /* Configure the GPIO_LED pin */
  GPIO_InitStruct.Pin = pin;
  GPIO_InitStruct.Mode = mode;
  GPIO_InitStruct.Pull = pull;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

  HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);

   /* Enable clock */
 
//   /* Configure PA11 pin as output */
//   LL_GPIO_SetPinMode(GPIOx, pin, mode);
//   /* Default (after reset) is push-pull output */
//	LL_GPIO_SetPinOutputType(GPIOx, pin, type);  
//   /* Configure PA11 output speed as very high */
//   LL_GPIO_SetPinSpeed(GPIOx, pin, LL_GPIO_SPEED_FREQ_VERY_HIGH);
//   /* Default (after reset) is no pull-up or pull-down */
//   LL_GPIO_SetPinPull(GPIOx, pin, pull); /**/
 }
 void sgdClockEnable(){
	__HAL_RCC_COMP1_CLK_ENABLE();
	__HAL_RCC_COMP2_CLK_ENABLE();
	__HAL_RCC_TIM1_CLK_ENABLE();
	__HAL_RCC_TIM3_CLK_ENABLE();
	__HAL_RCC_TIM15_CLK_ENABLE();
	__HAL_RCC_TIM16_CLK_ENABLE();
 	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_FLASH_CLK_ENABLE();
	__HAL_RCC_TIM3_CLK_ENABLE() ;
	__HAL_RCC_TIM1_CLK_ENABLE() ;
	__HAL_RCC_TIM2_CLK_ENABLE() ;
	__HAL_RCC_TIM14_CLK_ENABLE() ;
	__HAL_RCC_TIM17_CLK_ENABLE() ;
	__HAL_RCC_SPI1_CLK_ENABLE();
	__HAL_RCC_ADC_CLK_ENABLE();
 
 	}

 void sgdInitExitIO(){
 
  GPIO_InitTypeDef  GPIO_InitStruct = {0};
  USER_BUTTON_GPIO_CLK_ENABLE();                 /* Enable the GPIO clock corresponding to the button */
  GPIO_InitStruct.Mode  = GPIO_MODE_IT_FALLING;  /* GPIO mode is a falling edge interrupt */
  GPIO_InitStruct.Pull  = GPIO_PULLUP;           /* pull up */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  /* The speed is high */
  GPIO_InitStruct.Pin = PIN_FLASH_BT;
  HAL_GPIO_Init(PORT_FLASH_BT, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = PIN_FLASH_IN;
  HAL_GPIO_Init(PORT_FLASH_IN, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = PIN_CAMERA;
  HAL_GPIO_Init(PORT_CAMERA, &GPIO_InitStruct);
//  GPIO_InitStruct.Pin = PIN_XFULL;
//  HAL_GPIO_Init(PORT_XFULL, &GPIO_InitStruct);
//  GPIO_InitStruct.Mode  = GPIO_MODE_IT_RISING;  /* GPIO mode is a falling edge interrupt */
//  GPIO_InitStruct.Pull  = GPIO_PULLDOWN;           /* pull up */
//  GPIO_InitStruct.Pin = PIN_SW_DEC;
//  HAL_GPIO_Init(PORT_SW_DEC, &GPIO_InitStruct);

  
//  GPIO_InitStruct.Mode  = GPIO_MODE_IT_RISING;  /* GPIO mode is a falling edge interrupt */
//  GPIO_InitStruct.Pull  = GPIO_PULLDOWN;           /* pull up */
//  GPIO_InitStruct.Pin = PIN_IGBT_G;
//  HAL_GPIO_Init(PORT_IGBT_G, &GPIO_InitStruct);
  
//  GPIO_InitStruct.Mode  = GPIO_MODE_IT_RISING_FALLING;  /* GPIO mode is a falling edge interrupt */
//  GPIO_InitStruct.Pull  = GPIO_PULLDOWN;           /* pull up */
//  GPIO_InitStruct.Pin = PIN_F_EN;
//  HAL_GPIO_Init(PORT_F_EN, &GPIO_InitStruct);


//  HAL_NVIC_DisableIRQ(EXTI0_1_IRQn);
  /* Enable EXTI interrupt */
//  HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
  /* Configure interrupt priority */
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);

  HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);
  HAL_NVIC_SetPriority(EXTI2_3_IRQn, 0, 0);
  /* Configure interrupt priority */
  HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
  HAL_NVIC_SetPriority(EXTI0_1_IRQn, 0, 0);



 	}
 void sgdIOInit(){
// 
//	
//  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_ALL);

	INIT_OUTPUT_FULL_LED;		
	INIT_OUTPUT_CHG_LED;	
//	 SET_FULL_LED;
//	 SET_CHG_LED;
	INIT_INPUT_TTL_M;	
	INIT_INPUT_CAMERA;	
	INIT_INPUT_USB_DEC;
	INIT_INPUT_FLASH_BT	;	
	INIT_INPUT_FLASH_IN;
	INIT_INPUT_SWITCH_DEC;		
	INIT_INPUT_UP_S1;			
	INIT_INPUT_UP_S2;			
	//INIT_INPUT_UP_XFULL;	
	
	;
	
	SEGGER_RTT_printf(0, "  \r\n" );
	INIT_INPUT_PB12;

}
 void sgdOpen5VPower(){
	SET_POWER_5VCTRL;

 	}
 void varInit(){
	_chgTick = 1;
	flashTick = 1;
	userOperateTick = 0;
 	}
 //按住预闪按钮开机，红灯开始1秒闪烁5次，持续按住预闪按钮3秒，则将当前编码器位置当做初始位置
// void sgdFactoryInit(){
//		uint32_t flashAngle = HW32_REG(ZERO_ANGLE_FLASH_ADDR );
//			 SEGGER_RTT_printf(0, "sgdFactoryInit  %d  %d\r\n",flashAngle ,getTick());
// 		if(flashAngle == 0xFFFFFFFF)
// 			{
//			
//					while (1){
//						Delay_ms(100);
//						TOGGLE_CHG_LED;
//						if (!IS_HIGH_FLASH_BT)
//							{	
//								sgdSetZeroAngle();
//							}
//						}

// 			}
//		while (!IS_HIGH_FLASH_BT)
//			{
//				int t = getTick()%200;
//				if(timeGone(1)>3000){
//					sgdSetZeroAngle();
//					RESET_CHG_LED;
//					SET_FULL_LED;
//					while(1){
//						;
//						}
//					}
//				if(t<100){
//					SET_CHG_LED;
//					}
//				else{
//					RESET_CHG_LED;
//					}
//			}
// 	}
int isCharging(){
	 return ( timeGone(_oneFlashTick) < 3000  ); 
}
void sgdChargeCtrl(){
					//SEGGER_RTT_printf(0, "sgdChargeCtrl\r\n");
			if(IS_HIGH_SWITCH_DEC)
				{
							RESET_FULL_LED;
							RESET_CHG_LED;
			 				SEGGER_RTT_printf(0, "IS_HIGH_SWITCH_DEC  == 1 \r\n" );
							return ;
				}
			
					if(1500 > sgdGetADCHV( )){
					// SEGGER_RTT_printf(0, "  Battery	 charging  HV = %d\r\n" ,sgdGetADCHV( ));
							SET_CHG_LED;
							RESET_FULL_LED;
						}
					else{
					//SEGGER_RTT_printf(0, "  Battery	charged\r\n");
							RESET_CHG_LED;
							SET_FULL_LED;
						}
//			if (sgdGetBatteryV( )  < 3050)
//				{
//					SEGGER_RTT_printf(0, "low Battery	sgdEnterStop\r\n");
//					//RESET_F_EN;
//					RESET_POWER_5VCTRL;
//					while(IS_HIGH_USB_DEC){
//						int tick1000 = getTick() % 1000 ;
//						sgdSleepCheck();
//						 if(tick1000 < 500){
//								SET_CHG_LED;
//								RESET_FULL_LED;
//						}else
//							{
//							RESET_CHG_LED;
//							RESET_FULL_LED;
//							}
//					
//						}
//					
//				}
//			else{
//				
//				}
				
}
int currenBatV = 0 ;
int flashCount = 0 ;
void sgdStartFlash(){
//return;
	 //SEGGER_RTT_printf(0, "sgdStartFlash------------------ sMode  = %d  %d  %d  flashCount= %d\r\n",sMode,flashTick ,getTick(),flashCount++);
   //modifySetIp5306Register(0,0x02);
//	#ifdef SAIBO
//   if((IS_HIGH_SWITCH_DEC) ||(IS_HIGH_XFULL)){
//			 SEGGER_RTT_printf(0, "IS_HIGH_SWITCH_DEC %d ---------IS_HIGH_F_EN = %d IS_HIGH_XFULL =  %d\r\n",IS_HIGH_SWITCH_DEC,IS_HIGH_F_EN ,IS_HIGH_XFULL);
//   	return ;
//   	}
//	#endif
	// ||(!IS_HIGH_F_EN)
//	#ifdef FUGU
//			// SEGGER_RTT_printf(0, "vol too low v =  %d\r\n", sgdGetADC1v5res( ) );
////	if( (!(sgdGetADC1v5res( )  > 1300  && sgdGetADC1v5res( )  < 1600)))
////		{
////			 SEGGER_RTT_printf(0, "vol too low v =  %d\r\n", sgdGetADC1v5res( ) );
////   	return ;
////		}
////	if( ((sgdGetADCres( ) < 3600)))
////		{
////			 SEGGER_RTT_printf(0, "vol too bat slow v =  %d\r\n", sgdGetADC1v5res( ) );
////   	return ;
//////		}
//   if((IS_HIGH_SWITCH_DEC)||(IS_HIGH_XFULL)){
//			 SEGGER_RTT_printf(0, "IS_HIGH_SWITCH_DEC %d ---------IS_HIGH_F_EN = %d IS_HIGH_XFULL =  %d\r\n",IS_HIGH_SWITCH_DEC,IS_HIGH_F_EN ,IS_HIGH_XFULL);
//   	return ;
//
//	 }
//	#endif
//	   SET_IGBT_G;

sgdStartOnePluse();
   userOperateTick = getTick();
   _oneFlashTick = getTick();
_oneFlashCount++;

//去除高电流
currenBatV = getFilterBat_mV( );
setGPIOA6BrakeLow();
setCompTick(getTick() - 700);

   //set15Pluse( 40);
	_pwmTick = getTick();

   // LL_TIM_EnableCounter(TIM_F_ON);
	//_chgTick = getTick();
	//开启电容充电
	///RESET_CHG;
}
int getOneFlashCount(){
	return _oneFlashCount;
}	

void sgdSetAndFlash(int plus){
	SEGGER_RTT_printf(0, "sgdSetAndFlash------------------ plus  = %d \r\n",plus);
	sgdOnePulseConfig(plus);
	sgdStartFlash();

}

void setIgbtGateTick( ){
	_igbtGateTick = getTick();
}
void sgdStartSFlash(){
//			 sgdStartFlash();
//
//			 SEGGER_RTT_printf(0, "sgdStartSFlash------------------ sMode  = %  %d  %d\r\n",sMode,flashTick ,getTick());
	if(sMode == SMode_S1){
		sgdStartFlash();
		}
	else if(sMode == SMode_S2  && timeGone(flashTick)<2000){
			 SEGGER_RTT_printf(0, "sgdStartSFlash ************************  flashTick = %d  %d\r\n",flashTick ,getTick());
		sgdStartFlash();
		}
	flashTick = getTick();
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);
//	SEGGER_RTT_printf(0, "HAL_GPIO_EXTI_Callback－－－－－－－－－－－－ %d  %d\r\n",GPIO_Pin,getTick());

  if(GPIO_Pin ==  PIN_CAMERA)
  {
  	sgdStartFlash();
	_oneFlashCount++;
	 SEGGER_RTT_printf(0, "PIN_CAMERA    %d  %d  count  = %d \r\n",PIN_CAMERA,getTick(),_oneFlashCount);
  }
  else if(GPIO_Pin ==  PIN_FLASH_BT)
  {
  	sgdStartFlash();
	_oneFlashCount++;
	 SEGGER_RTT_printf(0, "PIN_FLASH_BT    %d  %d  count  = %d \r\n",PIN_FLASH_BT,getTick(),_oneFlashCount);
  }
  else if(GPIO_Pin ==  PIN_FLASH_IN)
  {
  	sgdStartSFlash();
	_flashInCount++;
	_flashInTick = getTick();
	 SEGGER_RTT_printf(0, "PIN_FLASH_IN  s %d  %d  count  = %d \r\n",PIN_FLASH_IN,getTick(),_flashInCount);
  }
  else if(GPIO_Pin ==  PIN_IGBT_G)
  {
  
	SEGGER_RTT_printf(0, "----IGBT_G plus = %d	igbt =	%d	gone = %d  last = %d  count  = %d	  igbt lou = %d  in lou = %d  ilou = %d\r\n",_oneFlashTick, _igbtGateTick,(timeGone(_oneFlashTick)),(timeGone(_igbtGateTick)),_igbtGateCount,(_oneFlashCount - _igbtGateCount),(_oneFlashCount - _flashInCount),(_igbtGateCount - _flashInCount));
  	_igbtGateTick = getTick();
	_igbtGateCount++;
	  }
}

//static void APP_ConfigTIM1ExternalClock(void)
//{  
//  /* Configure PA8 pin as TI1 input */
//  LL_GPIO_InitTypeDef ETRGPIOinit={0};
//  
//  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
//  
//  ETRGPIOinit.Pin        = LL_GPIO_PIN_8;
//  ETRGPIOinit.Mode       = LL_GPIO_MODE_ALTERNATE;
//  ETRGPIOinit.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
//  ETRGPIOinit.Pull       = LL_GPIO_PULL_UP;
//  ETRGPIOinit.Alternate  = LL_GPIO_AF_2;
//  ETRGPIOinit.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
//  
//  LL_GPIO_Init(GPIOA,&ETRGPIOinit);
//  
//  /* Configure TIM1 external clock source mode 1 */
//  LL_TIM_SetClockSource(TIM1,LL_TIM_CLOCKSOURCE_EXT_MODE1);
//  
//  /* Configure trigger selection as TI1F_ED */
//  LL_TIM_SetTriggerInput(TIM1,LL_TIM_TS_TI1F_ED);
//  
//}

int s_ip5306Tick = 1;


//void sgdAutoFlash(){
//	if(timeGone(flashTick) > 6000){
//			 SEGGER_RTT_printf(0, "sgdAutoFlash------------------ sMode  = %  %d  %d\r\n",sMode,flashTick ,getTick());
//		flashTick = getTick();
//  			sgdStartFlash();
//			memset(aTxBuffer,0,sizeof(aTxBuffer)-1);
//			sprintf((char*)aTxBuffer,"count = %d v= %d mV adc = %d \r\n\0" ,flashCount++,uhADCxConvertedData_Voltage_mVolt,uhADCxConvertedData);
// 			sgdUsartTransmit_DMA(USART2, (uint8_t*)aTxBuffer, strlen((char*)aTxBuffer));
//		}

//}
#define BATTERY_LED_TIME 500
void sgdBatteryLedCtrl(){
 	static int _batteryTick = 10;
 	static int _FEN_IN = 0;
	int now = getTick();
	int pluse = now%1000;
//	#ifdef FUGU
//				INIT_OUTPUT_F_EN;
//				SET_F_EN;
////	if(batteryState == BatteryState_Chargineg || BatteryState_ChargeFull == batteryState){
////			if(_FEN_IN == 1){
////				_FEN_IN = 0;
////				SET_F_EN;
////			}
////		}
////	if(batteryState != BatteryState_Chargineg && BatteryState_ChargeFull != batteryState){
////			if(_FEN_IN == 0){
////				_FEN_IN = 1;
////				INIT_INPUT_F_EN;
////			}
////		}
//
//	#endif
	

	if(timeGone(_batteryTick) >5 && now%10 == 0 )
		{
			_batteryTick = getTick();
			
//					if(now % 2000 >= 1000){
//					pluse = 1000-now%1000;
//						}
//					sgdSetBatteryLedPluse(pluse+10);
//					return;
			switch (batteryState)
				{
					case BatteryState_100:
					case BatteryState_75:
					case BatteryState_50:
						
						if(timeGone(1) > 10000)
							{
								sgdSetBatteryLedPluse(0);
								break;
							}
					case BatteryState_25:
						
									
							if(now % (BATTERY_LED_TIME * 10) < (1+batteryState)*BATTERY_LED_TIME){
									if(now % BATTERY_LED_TIME < BATTERY_LED_TIME/2){
											sgdSetBatteryLedPluse(0);
										}
									else{
											sgdSetBatteryLedPluse(1000);
										}
								}
							else{
											sgdSetBatteryLedPluse(0);
								}
							break;
						
					case BatteryState_ChargeFull:
						sgdSetBatteryLedPluse(1000);
						break;
						
					case BatteryState_Chargineg:
					if(now % 2000 >= 1000){
					pluse = 1000-now%1000;
						}
					sgdSetBatteryLedPluse(pluse+10);
						break;
						
				}
			
			//SEGGER_RTT_printf(0, "batteryState = %d  pluse =%d\r\n",batteryState,pluse );
			
		}
	}
// void TIM1Channel(void)
//{
//  
//}

/**
  * @brief  Configure TIM base
  * @param  None
  * @retval None
  */
//  void sgdOnePulseConfig(int pluseUS)
//{
//  /* Configure TIM1 */
//  
//}
// static void TIM1Channel(void)
// {


//   
//   /* 通道2配置 */
//   /* 配置输出极性为高有效 */
//   LL_TIM_OC_SetPolarity(TIM1,LL_TIM_CHANNEL_CH3,LL_TIM_OCPOLARITY_HIGH);
//   /* 配置空闲极性为低 */
//   LL_TIM_OC_SetIdleState(TIM1,LL_TIM_CHANNEL_CH3,LL_TIM_OCIDLESTATE_LOW);
//   /* 设置比较值：200 */
//   LL_TIM_OC_SetCompareCH3(TIM1,1);
//   /* 配置通道为PWM2模式 */
//   LL_TIM_OC_SetMode(TIM1,LL_TIM_CHANNEL_CH3,LL_TIM_OCMODE_PWM2);
//   
//   /* 输出映射PA3 */
//   /* 输出映射PA3 */
//   LL_GPIO_SetPinMode(GPIOB,LL_GPIO_PIN_6,LL_GPIO_MODE_ALTERNATE);
//   LL_GPIO_SetAFPin_0_7(GPIOB,LL_GPIO_PIN_6,LL_GPIO_AF_1);
////   LL_GPIO_SetPinMode(GPIOA,LL_GPIO_PIN_0,LL_GPIO_MODE_ALTERNATE);
////   LL_GPIO_SetAFPin_8_15(GPIOA,LL_GPIO_PIN_0,LL_GPIO_AF_13);
//   
//   /* 使能通道1 */
//   LL_TIM_CC_EnableChannel(TIM1,LL_TIM_CHANNEL_CH3);
// }
// 
// /**
//   * @brief  TIM1单脉冲模式配置函数
//   * @param  无
//   * @retval 无
//   */
// static void sgdOnePulseConfig(int pluseUS)
// {
////   /* CK_INT 1分频 */
////   LL_TIM_SetClockDivision(TIM3,LL_TIM_CLOCKDIVISION_DIV1);
////   /* 向上计数模式 */
////   LL_TIM_SetCounterMode(TIM3,LL_TIM_COUNTERMODE_UP);
////   /* 重装载值1000 */
////	LL_TIM_SetAutoReload(TIM3,pluseUS );
////	/* CK_CNT 预分频值：800 */
////	LL_TIM_SetPrescaler(TIM3,47);
//// 
//// //  /* 设置TIM1为从模式触发模式 */
//// //  LL_TIM_SetSlaveMode(TIM1,LL_TIM_SLAVEMODE_TRIGGER);
//// //  
//// //  /* 设置输入触发源为TI2FP2 */
//// //  LL_TIM_SetTriggerInput(TIM1,LL_TIM_TS_TI2FP2);
////   
////   /* 触发/输出通道配置 */
////   TIM3Channel();
////   
////   /* 使能单脉冲模式 */
////   LL_TIM_SetOnePulseMode(TIM3,LL_TIM_ONEPULSEMODE_SINGLE);
////   
////   /* 关闭自动输出 */
////   LL_TIM_DisableAutomaticOutput(TIM3);
////   /* 主输出使能 */
////   LL_TIM_EnableAllOutputs(TIM3);
////   
//// //  /* 开启TIM1计数器 */
//// //  LL_TIM_EnableCounter(TIM1);
//// 
//// 
//// 
//// 
// 
// 
// 
// 
// 
//    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM1);
// 
//   /* CK_INT 1分频 */
//   LL_TIM_SetClockDivision(TIM1,LL_TIM_CLOCKDIVISION_DIV1);
//   /* 向上计数模式 */
//   LL_TIM_SetCounterMode(TIM1,LL_TIM_COUNTERMODE_UP);
//   /* 重装载值1000 */
//	LL_TIM_SetAutoReload(TIM1,pluseUS );
//	/* CK_CNT 预分频值：800 */
//	LL_TIM_SetPrescaler(TIM1,47);
// 
// //  /* 设置TIM1为从模式触发模式 */
// //  LL_TIM_SetSlaveMode(TIM1,LL_TIM_SLAVEMODE_TRIGGER);
// //  
// //  /* 设置输入触发源为TI2FP2 */
// //  LL_TIM_SetTriggerInput(TIM1,LL_TIM_TS_TI2FP2);
//   
//   /* 触发/输出通道配置 */
//   TIM1Channel();
//   
//   /* 使能单脉冲模式 */
//   LL_TIM_SetOnePulseMode(TIM1,LL_TIM_ONEPULSEMODE_SINGLE);
//   
//   /* 关闭自动输出 */
//   LL_TIM_DisableAutomaticOutput(TIM1);
//   /* 主输出使能 */
//   LL_TIM_EnableAllOutputs(TIM1);
// }

void sgdWait5VReady()
{
	while(!(sgdGetADC1v5res( )  > 1300  && sgdGetADC1v5res( )  < 1700)){
//		SET_CHG_LED;
//		SET_FULL_LED;
		sgdReadAdc( );
	SEGGER_RTT_printf(0, "wait 5v vbat = %d  ick= %d\r\n",sgdGetADC1v5res( ),getTick());
		Delay_ms(20);
		if(IS_HIGH_SWITCH_DEC){
			RESET_POWER_5VCTRL;
			}
		}
//	RESET_CHG_LED;
//	RESET_FULL_LED;
	SEGGER_RTT_printf(0, "5v ready tick = %d\r\n",getTick());
	
}

//当检测到没有 角度信号的时候，红灯闪烁，等待用户旋转编码器，编码器覆盖到所有角度 将角度信息写入flash。
void sgdNoAngleInfo(){
int _tick = 1;
int _tickGetInput = 1;
int state = 0 ;
static int min1 =361;
static int min2 =361;
static int max1 =0;
static int max2 =0;
int angle = 0 ;
	sgdZeroAngleInit();
	if(  sgdGetZeroAngle()  == -1) 
		{
			while(state == 0){
					_tick = getTick()%200;
					
					if(IS_HIGH_SWITCH_DEC){
						RESET_POWER_5VCTRL;
						}
					if(_tick > 100){
						SET_CHG_LED;
						}
					else
						{
						RESET_CHG_LED;
						}
					
					if(timeGone(_tickGetInput) > 100){
//								SEGGER_RTT_printf(0, "_tickGetInput = %d  timeGone(_tickGetInput) =%d\r\n ",_tickGetInput,timeGone(_tickGetInput) );
							_tickGetInput = getTick();
 							sgdReadAngle(  );
							angle =  sgdGetAngle( ) ;
							if(angle == 358 || angle == 0){
								SEGGER_RTT_printf(0, "continue angle =%d tick = %d\r\n",angle,getTick());
								continue;
								}
							angle = (  sgdGetAngle( )  )% 360;
 							SEGGER_RTT_printf(0, "angel = %d  min = %d  max = %d rang1= %d  min2 = %d  max2 = %d  rang2 = %d  tick  = %d = %d\r\n",angle,min1,max1,(max1-min1),min2,max2,(max2+360-min2),getTick(),_tickGetInput);
							if(angle > max1){
								max1 = angle;
							}
							if(angle < min1){
								min1 = angle;
							}
							if (angle > 220 && angle  < min2)
								{
									min2  = angle ;
								}
							if (angle < 220 && angle  > max2)
								{
									max2  = angle ;
								}
							if((max1-min1) < 150 && (max1-min1)>136 &&(min1 != 361)){
								state =1;
 							SEGGER_RTT_printf(0, "1 angel = %d  min = %d  max = %d rang1= %d  min2 = %d  max2 = %d  rang2 = %d  tick  = %d = %d\r\n",angle,min1,max1,(max1-min1),min2,max2,(max2+360-min2),getTick(),_tickGetInput);
							
								sgdSaveZeroAngle(min1);
								}
							
							if( (max2+360-min2) < 150 && max2 != 0 && (max2+360-min2)>136   && (min2 != 361)){
								state =1;
 							SEGGER_RTT_printf(0, "2 angel = %d  min = %d  max = %d rang1= %d  min2 = %d  max2 = %d  rang2 = %d  tick  = %d = %d\r\n",angle,min1,max1,(max1-min1),min2,max2,(max2+360-min2),getTick(),_tickGetInput);
							
								sgdSaveZeroAngle(min2);
								}
							
						}
				}
//			while(state == 1){if(timeGone(_tickGetInput) > 100){
//							_tickGetInput = getTick();
//							sgdReadAngle(  );
//							angle =  sgdGetAngle( ) ;
//							if(angle == 358 || angle == 0){
//								continue;
//								}
//							angle = (  sgdGetAngle( ) + 126 )% 360;
//							SEGGER_RTT_printf(0, "angel = %d  min = %d  max = %d rang1= %d  min2 = %d  max2 = %d  rang2 = %d\r\n",angle,min1,max1,(max1-min1),min2,max2,(max2+360-min2));
//							
//							
//						}
//
//				}
		}

}
int isGoBootload(){
  __HAL_RCC_GPIOB_CLK_ENABLE();
  INIT_OUTPUT_FULL_LED;
  SET_FULL_LED;
	INIT_INPUT_S2;
	INIT_INPUT_FLASH_BT;
//	Delay_ms(100);
				SEGGER_RTT_printf(0, "IS_HIGH_FLASH_BT = %d   IS_HIGH_S2  = %d\r\n",IS_HIGH_FLASH_BT,IS_HIGH_S2);
	if((IS_HIGH_FLASH_BT == GPIO_PIN_RESET) && (IS_HIGH_S2  == GPIO_PIN_RESET)){
				SEGGER_RTT_printf(0, "isGoBootload TRUE\r\n",getTick ());
		return TRUE;
		}
				SEGGER_RTT_printf(0, "isGoBootload FALSE\r\n",getTick ());
	return FALSE;


}
void sgdFactoryInit(){
	int _tick = 2;
	if((IS_HIGH_FLASH_BT == GPIO_PIN_RESET) && (IS_HIGH_S1  == GPIO_PIN_RESET))
		{
				SEGGER_RTT_printf(0, "sgdFactoryInit   tick  = %d\r\n",getTick ());
			while((IS_HIGH_FLASH_BT == GPIO_PIN_RESET) && (IS_HIGH_S1  == GPIO_PIN_RESET)){
				
						_tick = getTick()%200;
						
						if(_tick > 100){
							SET_FULL_LED;
							}
						else
							{
							RESET_FULL_LED;
							}
				}
			if(getTick () > 3000){
				sgdResetZeroAngle();
				RESET_FULL_LED;
				SEGGER_RTT_printf(0, "sgdResetZeroAngle   tick  = %d\r\n",getTick ());
				sgdZeroAngleInit();
				}
		}
}
RTC_HandleTypeDef RtcHandle;
void sgdInitRTC()
{
	
	/* RTC initialization */
	RtcHandle.Instance = RTC;						  /* Select RTC */
	RtcHandle.Init.AsynchPrediv = RTC_AUTO_1_SECOND;  /* RTC asynchronous prescaler calculated automatically for one second time base */
	RtcHandle.Init.OutPut = RTC_OUTPUTSOURCE_NONE;	  /* No output on the TAMPER pin */
	if (HAL_RTC_Init(&RtcHandle) != HAL_OK)
	{
	}
	

}
void sgdInit(){
//#ifdef  demoDeug
//	return;
//#endif
//while(1){;}
//LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM1|LL_APB1_GRP2_PERIPH_TIM14|LL_APB1_GRP2_PERIPH_ADC1);
//LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

//  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_ALL);
		SEGGER_RTT_printf(0, "sgdInit  1123   start\r\n");
		sgdClockEnable();
 	//	Delay_ms(500);

		
//		SEGGER_RTT_printf(0, "sgd1msConfig   start\r\n");
//		sgd1msConfig( );
// 		Delay_ms(10);
		
		
		SEGGER_RTT_printf(0, "sgdIOInit   start\r\n");
		sgdIOInit();
 		Delay_ms(100);
		SEGGER_RTT_printf(0, "sgdBatteryLedConfige   start\r\n");
 		sgdBatteryLedConfige(800);
		SEGGER_RTT_printf(0, "sgdADCInit   start\r\n");
		sgdADCInit();
		
		
		SEGGER_RTT_printf(0, "5306Key   start\r\n");
   		sgdIp5306KeyConfige ( );
		
		SEGGER_RTT_printf(0, "I2C   start\r\n");
		sgdI2C_Init();
//		#ifdef FUGU
//		SEGGER_RTT_printf(0, "ifdef FUGU \r\n");
//   		sgdIp5306KeyConfige ( );
// 		Delay_ms(10);
//		sgdKT5763Init();
//		#endif
		
 		Delay_ms(10);
		
		SEGGER_RTT_printf(0, "IS_HIGH_SWITCH_DEC = %d    \r\n",IS_HIGH_SWITCH_DEC);
		
				while(IS_HIGH_SWITCH_DEC)
				{
					if(timeGone	(userOperateTick) > 10){
							userOperateTick = getTick();
							sgdReadIP5306();
							SEGGER_RTT_printf(0, "batteryState = %d    \r\n",batteryState);
							sgdBatteryLedCtrl();
						}
				}
// 			while(1){
// 				SEGGER_RTT_printf(0, "while\r\n");
// 				;}

		SEGGER_RTT_printf(0, "sgdOpen5VPower   start\r\n");
 		sgdOpen5VPower();
//		
//		SEGGER_RTT_printf(0, "sgdWait5VReady   start\r\n");
//	 	sgdWait5VReady();
//		
//		
//		SEGGER_RTT_printf(0, "sgdKT5763Init   start\r\n"); 
//		sgdKT5763Init();
		SEGGER_RTT_printf(0, "sgdOnePulseConfig   start\r\n");
 		sgdOnePulseConfig(3000 );
//		
//		SEGGER_RTT_printf(0, "sgdFactoryInit   start\r\n");
//		sgdFactoryInit();
//		
//		SEGGER_RTT_printf(0, "sgdNoAngleInfo   start\r\n");
//		sgdNoAngleInfo();
	
//	HAL_NVIC_SystemReset( );


	//	sgdInitRTC();

		 
		Delay_ms(10);
		sgdMakeCHGRise();
 	 	sgdInitExitIO();
	  // INIT_OUTPUT_F_EN;
	   userOperateTick =100;
	    initPSFB ();
		SEGGER_RTT_printf(0, "sgdInit end \r\n");
//	
	/* Configure USART */
//	sgdConfigUsart(USART2);
//	
//	/* Send "UART Test" using DMA and wait for transmission to complete */
//	sgdUsartTransmit_DMA(USART2, (uint8_t*)aTxBuffer, sizeof(aTxBuffer)-1);
	 SEGGER_RTT_printf(0, "sgdInit   end\r\n");
	 
	
}
int autoFLashTick  =1;
void sgdAutoFlash()
{
		if(timeGone(autoFLashTick)>6000){
			autoFLashTick = getTick();
			sgdStartFlash();
		}
	if(sMode == 2){
	}
}
void sgdSetUsbConnected(int connectState){
	 usbHostConnected = connectState;
	 //userOperateTick = getTick();
}
int sgdGetUsbConnected( ){
	return  usbHostConnected  ;
} 

void sgdSleepRun(){
	return;
							SEGGER_RTT_printf(0, "sgdSleepRun\r\n ");
 	__HAL_RCC_GPIOA_FORCE_RESET();
	__HAL_RCC_GPIOB_FORCE_RESET();
	__HAL_RCC_GPIOC_FORCE_RESET();
	__HAL_RCC_GPIOF_FORCE_RESET();
	__HAL_RCC_FLASH_CLK_DISABLE();
	__HAL_RCC_TIM3_FORCE_RESET() ;
	__HAL_RCC_TIM1_FORCE_RESET() ;
	__HAL_RCC_TIM14_FORCE_RESET() ;
	__HAL_RCC_TIM17_FORCE_RESET() ;
	__HAL_RCC_SPI1_FORCE_RESET();
	__HAL_RCC_ADC_FORCE_RESET();
	
	  HAL_NVIC_DisableIRQ(EXTI4_15_IRQn);
	  /* Configure interrupt priority */
	
	  HAL_NVIC_DisableIRQ(EXTI2_3_IRQn);
	
	SEGGER_RTT_printf(0, "HAL_PWR_EnterSLEEPMode\r\n ");

	HAL_PWR_EnterSTOPMode(0,0);

	HAL_SuspendTick();

	/* Enter STOP mode */
	//HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
	while(1){
							SEGGER_RTT_printf(0, "while(1) \r\n" );
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	INIT_INPUT_FLASH_BT;
	INIT_INPUT_USB_DEC;
	int i =10;
	while(i-->0){
							SEGGER_RTT_printf(0, "while( i = %d) \r\n",i );
							SEGGER_RTT_printf(0, "IS_HIGH_USB_DEC = %d   IS_HIGH_FLASH_BT=%d \r\n",IS_HIGH_USB_DEC,IS_HIGH_FLASH_BT);
		;}
	if(!IS_HIGH_USB_DEC  || !IS_HIGH_FLASH_BT)
		{
							SEGGER_RTT_printf(0, "break if(!IS_HIGH_USB_DEC  || !IS_HIGH_FLASH_BT)\r\n ");
		
			break;
		}
//	__HAL_RCC_GPIOB_FORCE_RESET();
//	__HAL_RCC_GPIOC_FORCE_RESET();
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
	}
	/* Resume the SysTick interrupt */
	HAL_ResumeTick();
	sgdInit();
	userOperateTick = getTick();
 
}
void sgdOnSleepStart(){
	  __HAL_RCC_GPIOB_CLK_ENABLE();
	   __HAL_RCC_GPIOC_CLK_ENABLE();
	   GPIO_InitTypeDef  GPIO_InitStruct = {0};
	   GPIO_InitStruct.Mode  = GPIO_MODE_IT_FALLING;  /* GPIO mode is a falling edge interrupt */
	   GPIO_InitStruct.Pull  = GPIO_PULLUP; 		  /* pull up */
	   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  /* The speed is high */
	   GPIO_InitStruct.Pin = PIN_USB_DEC;
	   HAL_GPIO_Init(PORT_USB_DEC, &GPIO_InitStruct);
//	   GPIO_InitStruct.Pin = PIN_FLASH_BT;
//	   HAL_GPIO_Init(PORT_FLASH_BT, &GPIO_InitStruct);
	 //  GPIO_InitStruct.Pull  = GPIO_PULLDOWN; 		  /* pull up */
	 //  GPIO_InitStruct.Pin = PIN_BAT_LED;
	 //  HAL_GPIO_Init(PORT_BAT_LED, &GPIO_InitStruct);
	 //  GPIO_InitStruct.Pin = PIN_XFULL;
	 //  HAL_GPIO_Init(PORT_XFULL, &GPIO_InitStruct);
	 //  GPIO_InitStruct.Mode  = GPIO_MODE_IT_RISING;  /* GPIO mode is a falling edge interrupt */
	 //  GPIO_InitStruct.Pull  = GPIO_PULLDOWN; 		  /* pull up */
	 //  GPIO_InitStruct.Pin = PIN_SW_DEC;
	 //  HAL_GPIO_Init(PORT_SW_DEC, &GPIO_InitStruct);
	 
	   
	   HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
	   /* Configure interrupt priority */
	   HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
	 HAL_PWR_EnterSTOPMode(0,0);
//HAL_GPIO_DeInit(PORT_FLASH_BT, PIN_FLASH_BT);
//	  __HAL_RCC_GPIOB_FORCE_RESET();

	 int now = getTick();
	while(getTick()< now+300){
				;}
}
int calPluse(int V_bat1, int sgdGetADCHV) {
    // 定义低电压和高电压点的参数
    int V_bat  = V_bat1 ;
    int a = 0 ;
		int b =0;
		if(V_bat > 4100){
  a = 43;
  b = 36;
  }
// else if(V_bat > 4050){
//  a = 34;
//  b = 38;
//  }
 else if(V_bat > 4000){
  a = 45;
  b = 38;
  }
 else if(V_bat > 3900){
  a = 45;
  b = 40;
  }
 else if(V_bat > 3800){
  a = 46;
  b = 40;
  }
 else if(V_bat > 3700){
  a = 47;
  b = 40;
  }
 else if(V_bat > 3600){
  a = 47;
  b = 42;
  }
 else if(V_bat > 3500){
  a = 47;
  b = 42;
  }
 else if(V_bat > 3400){
  a = 47;
  b = 45;
  }
 else if(V_bat > 3300){
  a = 47;
  b = 47;
  }
 else if(V_bat > 3200){
  a = 47;
  b = 49;
  }
 else if(V_bat > 3100){
  a = 48;
  b = 52;
  }
 else if(V_bat > 3000){
  a = 48;
  b = 54;
  }
 else if(V_bat > 2900){
  a = 50;
  b = 55;
  }
 else if(V_bat > 2800){
  a = 52;
  b = 56;
  }
 else if(V_bat > 2600){
  a = 54;
  b = 58;
  }
 else if(V_bat > 2400){
  a = 54;
  b = 70;
  }
// else if(V_bat > 3400){
//  a = 43;
//  b = 54;
//  }
// else if(V_bat > 3300){
//  a = 44;
//  b = 58;
//  }
// else if(V_bat > 3200){
//  a = 44;
//  b = 58;
//  }
// else if(V_bat > 3100){
//  a = 45;
//  b = 58;
//  }
// else if(V_bat > 3000){
//  a = 80;
//  b = 80;
//  }
// else if(V_bat > 2900){
//  a = 92;
//  b = 75;
//  }
// else if(V_bat > 2800){
//  a = 90;
//  b = 85;
//  }
// else if(V_bat > 2700){
//  a = 99;
//  b = 80;
//  }
 else{
  a = 35;
  b = 39;
  }
// 
//  a = 33;
//  b = 39;
int pluse = ( (a + b*sgdGetADCHV / 1300)*4/5);
if(pluse > 99)
{
 pluse = 99 ;
}
 pluse =  100 -pluse;
//SEGGER_RTT_printf(0, "(calPluse()  V_bat= %d   a = %d  b = %d  pluse = %d)\r\n" ,V_bat ,a,b ,pluse); 
	setPluseInfo(V_bat1,  a,  b,  pluse);
 return pluse ;

 SEGGER_RTT_printf(0, "(calPluse()  V_bat= %d   a = %d  b = %d HV =%d \t pluse = %d)\r\n" ,V_bat ,a,b,sgdGetADCHV ,( (a + b*sgdGetADCHV / 1300)*4/5)); 

	return ( (a + b*sgdGetADCHV / 1300)*4/5);
	
//		set15Pluse( (45+40*sgdGetADCHV( ) / 1300)*4/5); //4.1
//		set15Pluse( (47+40*sgdGetADCHV( ) / 1300)*4/5); //4.0
	//	set15Pluse( (49+42*sgdGetADCHV( ) / 1300)*4/5); //3.8
//		set15Pluse( (50+45*sgdGetADCHV( ) / 1300)*4/5); //3.6
//		set15Pluse( (55	+49*sgdGetADCHV( ) / 1300)*4/5); //3.4
//		set15Pluse( (59	+51*sgdGetADCHV( ) / 1300)*4/5); //3.2
	//	set15Pluse( (79		+51*sgdGetADCHV( ) / 1300)*4/5); //3.0
	//	set15Pluse( (70+50*sgdGetADCHV( ) / 1300)*4/5);
}

void sgdLoop(){
//#ifdef demoDeug
//	return;
//#endif
	 sgdGetInput();
	//setPluse( sgdGetPwmPluse());

	psfbcheck();
	sgdChargeCtrl();
	if(timeGone( _pwmTick ) > 9){
				_pwmTick = getTick();
				
			//SEGGER_RTT_printf(0, "_pwmTick = %d \r\n",_pwmTick);
	 	set15Pluse(calPluse(currenBatV,sgdGetADCHV( )));
//			 	set15Pluse(45);
			charge_control( );
					}
	  // sgdAutoFlash();
//			SEGGER_RTT_printf(0, "(sgdGetPsfbTimerState()== %d )\r\n" ,sgdGetPsfbTimerState() );
//if(sgdGetPsfbTimerState()==0)
//	{
//			SEGGER_RTT_printf(0, "(sgdGetPsfbTimerState()==0)\r\n");
//		
//	}
//	   if(isCharging())
//	   	{
//			
//	   	}
//	if(IS_HIGH_SWITCH_DEC)
////	if(0)
//
//		{
//	//	关机
//			//RESET_F_EN;
//			RESET_POWER_5VCTRL;
//			RESET_FULL_LED;
//			RESET_CHG_LED;
//			RESET_CHG;
//			if(!(IS_HIGH_USB_DEC))
//				{
//					if(timeGone	(userOperateTick) > 10){
//							userOperateTick = getTick();
//							//sgdReadIP5306();
//							SEGGER_RTT_printf(0, "22 batteryState = %d    \r\n",batteryState);
//						}
//					sgdBatteryLedCtrl();
//				}
//			else
//				{
//	//				sgdSetUsbConnected(FALSE);
//				}
//		}
//	else{
//  			sgdGetInput();
//			//SET_F_EN;
// 			SET_POWER_5VCTRL;
////			if(lowBatteryState == 1)
////				{
////				//低电量关闭5V 和闪光使能
////					RESET_F_EN;
////					RESET_POWER_5VCTRL;
////				}
////			else{
////				}
//			#ifdef FUGU
//				sgdIP5036Ctrl();
//			#endif
//			sgdChargeCtrl(); 
//			sgdBatteryLedCtrl();
//			//sgdSleepCheck();
//			if(_oneFlashTick > _igbtGateTick  > 20){
//				_igbtGateTick = _oneFlashTick;
//				SEGGER_RTT_printf(0, "----------------------_oneFlashTick - _igbtGateTick  > 10-----------------------sp\r\n");
//
//				}
//			if(_oneFlashTick > _flashInTick  > 20){
//				_flashInTick = _oneFlashTick;
//				SEGGER_RTT_printf(0, "----------------------_oneFlashTick - _flashInTick  > 10-----------------------sp\r\n");
//
//				}
//	    sgdAutoFlash();
//				
// 			if(timeGone(_oneFlashTick) > 3  && IS_HIGH_IGBT_G){
//				SEGGER_RTT_printf(0, "----------------------RESET_IGBT_G-------------------- \r\n");
//				RESET_IGBT_G;
// 				}
//				
//		}
	
	
}
