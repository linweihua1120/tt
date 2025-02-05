	#ifndef _SGD
#define _SGD
//10-25
//添加用户操作 tick
//用户操作 10 秒内  电池指示灯闪烁
//如果没有插充电器 用户15分钟不操作 进入休眠
//没有挡位的编码器 默认最大亮度闪
 
//充电 问题  应该 是 usbhost 变量没设dui

//10-30
//添加 赛博功能 
#include "py32f072xx_Start_Kit.h"

//11-8
//添加闪光zhi之前验证电压是否满足  否则不闪
//关闭开机的灯光流程 

//1230 添加电压2.8V限制 ，低于2.8V 无法边冲边用
   #define BOOTLOAD_MODE 1

//#define VECT_TAB_OFFSET  0x000 /*!< Vector Table base offset field.
//                                   This value must be a multiple of 0x100. */

#define VECT_TAB_OFFSET  0x00000 /*!< Vector Table base offset field.
                                   This value must be a multiple of 0x100. */

///************************* Miscellaneous Configuration ************************/
//#ifdef BOOTLOAD_MODE 
//#else
//#endif

#define FALSE 0
#define TRUE   (!FALSE) 

#define MIN_WORK_BATTERY_VOL 3000


#define getTick()  HAL_GetTick()

#define CHANNEL_ADC_BAT				ADC_CHANNEL_4
#define PORT_ADC_BAT				GPIOA
#define PIN_ADC_BAT					GPIO_PIN_4

#define CHANNEL_ADC_HV				ADC_CHANNEL_1
#define PORT_ADC_HV					GPIOA
#define PIN_ADC_HV					GPIO_PIN_1

#define CHANNEL_ADC_TMP				ADC_CHANNEL_9
#define PORT_ADC_TMP				GPIOB
#define PIN_ADC_TMP					GPIO_PIN_1

#define CHANNEL_ADC_CURRENT			ADC_CHANNEL_0
#define PORT_ADC_CURRENT			GPIOA
#define PIN_ADC_CURRENT				GPIO_PIN_0




#define PORT_FLASH_IN				GPIOC
#define PIN_FLASH_IN				GPIO_PIN_13

#define PORT_USB_DEC				GPIOF
#define PIN_USB_DEC					GPIO_PIN_9
#define PORT_RX_C1					GPIOF
#define PIN_RX_C1					GPIO_PIN_1 

#define PORT_ADB					GPIOA
#define PIN_ADB						GPIO_PIN_4


#define PORT_AD_1V5					GPIOA
#define PIN_AD_1V5					GPIO_PIN_1
#define PORT_CANON_D_OUT			GPIOF
#define PIN_CANON_D_OUT				GPIO_PIN_1

#define PORT_CANON_DATA				GPIOA
#define PIN_CANON_DATA				GPIO_PIN_2
//#define PORT_CANON_CLK				GPIOA
//#define PIN_CANON_CLK				GPIO_PIN_3
#define PORT_CAMERA					GPIOB
#define PIN_CAMERA					GPIO_PIN_0



//#define PORT_RX_C					GPIOA
//#define PIN_RX_C					GPIO_PIN_5
#define PORT_COMP1OUT_DEC					GPIOA
#define PIN_COMP1OUT_DEC					GPIO_PIN_6

#define PORT_RX_A					GPIOA
#define PIN_RX_A					GPIO_PIN_15
//#define PORT_RX_B					GPIOB
//#define PIN_RX_B					GPIO_PIN_0

#define PORT_RX_E					GPIOB
#define PIN_RX_E					GPIO_PIN_2
#define PORT_CANON_C_COMP			GPIOB
#define PIN_CANON_C_COMP			GPIO_PIN_10
#define PORT_CANON_D_COMP			GPIOB
#define PIN_CANON_D_COMP			GPIO_PIN_11


#define PORT_SW_DEC					GPIOA
#define PIN_SW_DEC					GPIO_PIN_11



//#define PORT_XFULL					GPIOB
//#define PIN_XFULL					GPIO_PIN_13
#define PORT_F_EN					GPIOB
//#define PIN_F_EN					GPIO_PIN_14
#define PORT_IGBT_G					GPIOB
#define PIN_IGBT_G					GPIO_PIN_9
#define PORT_F_ON					GPIOA
#define PIN_F_ON					GPIO_PIN_8
#define PORT_POWER_5VCTRL			GPIOF
#define PIN_POWER_5VCTRL			GPIO_PIN_0
#define PORT_CHG					GPIOA
#define PIN_CHG						GPIO_PIN_10
#define PORT_IP5306_KEY				GPIOB
#define PIN_IP5306_KEY				GPIO_PIN_2
#define PORT_SWITCH_DEC				GPIOF
#define PIN_SWITCH_DEC				GPIO_PIN_5

#define PORT_CHG_LED				GPIOF
#define PIN_CHG_LED					GPIO_PIN_6


#define PORT_FULL_LED				GPIOA
#define PIN_FULL_LED				GPIO_PIN_15

#define PORT_FLASH_BT				GPIOB
#define PIN_FLASH_BT				GPIO_PIN_3

#define PORT_CLK					GPIOB
#define PIN_CLK						GPIO_PIN_4

#define PORT_CSDA					GPIOB
#define PIN_CSDA					GPIO_PIN_5

#define PORT_BAT_LED				GPIOB
#define PIN_BAT_LED					GPIO_PIN_7
#define PORT_TTL_M					GPIOB
#define PIN_TTL_M					GPIO_PIN_6



#define PORT_S1						GPIOB
#define PIN_S1						GPIO_PIN_13 
#define PORT_S2						GPIOA
#define PIN_S2						GPIO_PIN_10

//Ｕ２
//#define PORT_IP5306_KEY				GPIOB
//#define PIN_IP5306_KEY				GPIO_PIN_0
//
//#define PORT_XFULL					GPIOA
//#define PIN_XFULL					GPIO_PIN_0
//#define PORT_CHG					GPIOA
//#define PIN_CHG						GPIO_PIN_1
//#define PORT_POWER_5VCTRL			GPIOA
//#define PIN_POWER_5VCTRL			GPIO_PIN_2
////#define PORT_IP5306_KEY				GPIOA
////#define PIN_IP5306_KEY				GPIO_PIN_3
//#define PORT_NSS					GPIOA
//#define PIN_NSS						GPIO_PIN_4
//#define PORT_SCK					GPIOA
//#define PIN_SCK						GPIO_PIN_5
//#define PORT_MISO					GPIOA
//#define PIN_MISO					GPIO_PIN_6
//#define PORT_MOSI					GPIOA
//#define PIN_MOSI					GPIO_PIN_7
//#define PORT_ADB					GPIOB
//#define PIN_ADB						GPIO_PIN_0
//#define PORT_FLASH_BT				GPIOB
//#define PIN_FLASH_BT				GPIO_PIN_1
//#define PORT_F_ON					GPIOA
//#define PIN_F_ON					GPIO_PIN_8
//#define PORT_SWITCH_DEC				GPIOA
//#define PIN_SWITCH_DEC				GPIO_PIN_9
//#define PORT_FLASH_IN				GPIOA
//#define PIN_FLASH_IN				GPIO_PIN_10
//#define PORT_FULL_LED					GPIOF
//#define PIN_FULL_LED					GPIO_PIN_5
//#define PORT_S1						GPIOB
//#define PIN_S1						GPIO_PIN_4 
//#define PORT_BAT_LED				GPIOB
//#define PIN_BAT_LED					GPIO_PIN_5
//#define PORT_S2						GPIOB
//#define PIN_S2						GPIO_PIN_6
//#define PORT_CSDA					GPIOB
//#define PIN_CSDA						GPIO_PIN_7
//#define PORT_CHG_LED				GPIOF
//#define PIN_CHG_LED					GPIO_PIN_8
//#define PORT_CLK					GPIOB
//#define PIN_CLK						GPIO_PIN_8
//#define PORT_SW_DEC					GPIOB
//#define PIN_SW_DEC					GPIO_PIN_9


//
//#define PORT_IP5306_L1				GPIOA
//#define PIN_IP5306_L1				GPIO_PIN_7
//#define PORT_ANG_SET				GPIOB
//#define PIN_ANG_SET					GPIO_PIN_0
//#define PORT_USB_DEC				GPIOB
//#define PIN_USB_DEC					GPIO_PIN_6


#define INIT_INPUT_PB12			sgdIOConfig(GPIOB, GPIO_PIN_12,					GPIO_MODE_INPUT,GPIO_PULLUP)


#define INIT_INPUT_TTL_M				sgdIOConfig(PORT_TTL_M,PIN_TTL_M,		GPIO_MODE_INPUT,GPIO_PULLDOWN)
//#define INIT_INPUT_F_EN				sgdIOConfig(PORT_F_EN,PIN_F_EN,		GPIO_MODE_INPUT,GPIO_PULLDOWN)

#define INIT_INPUT_USB_DEC			sgdIOConfig(PORT_USB_DEC,PIN_USB_DEC,		GPIO_MODE_INPUT,GPIO_PULLUP)

#define INIT_INPUT_SWITCH_DEC		sgdIOConfig(PORT_SWITCH_DEC,PIN_SWITCH_DEC,		GPIO_MODE_INPUT,GPIO_PULLUP)

#define INIT_INPUT_UP_S1			sgdIOConfig(PORT_S1, PIN_S1,					GPIO_MODE_INPUT,GPIO_PULLUP)
#define INIT_INPUT_UP_S2			sgdIOConfig(PORT_S2, PIN_S2,					GPIO_MODE_INPUT,GPIO_PULLUP)
#define INIT_INPUT_UP_XFULL			sgdIOConfig(PORT_XFULL, PIN_XFULL,				GPIO_MODE_INPUT,GPIO_PULLUP)

#define INIT_INPUT_CAMERA			sgdIOConfig(PORT_CAMERA, PIN_CAMERA,					GPIO_MODE_INPUT,GPIO_PULLUP)
#define INIT_INPUT_FLASH_BT			sgdIOConfig(PORT_FLASH_BT, PIN_FLASH_BT,					GPIO_MODE_INPUT,GPIO_PULLUP)
#define INIT_INPUT_FLASH_IN		sgdIOConfig(PORT_FLASH_IN, PIN_FLASH_IN,				GPIO_MODE_INPUT,GPIO_PULLUP)

#define INIT_OUTPUT_CHG				sgdIOConfig(PORT_CHG, PIN_CHG,					GPIO_MODE_OUTPUT_PP,GPIO_PULLUP)
#define INIT_OUTPUT_POWER_5VCTRL	sgdIOConfig(PORT_POWER_5VCTRL, PIN_POWER_5VCTRL,GPIO_MODE_OUTPUT_PP,GPIO_PULLUP)
#define INIT_OUTPUT_IP5306_KEY		sgdIOConfig(PORT_IP5306_KEY, PIN_IP5306_KEY,	GPIO_MODE_OUTPUT_PP,GPIO_PULLUP)
#define INIT_OUTPUT_CHG_LED			sgdIOConfig(PORT_CHG_LED, PIN_CHG_LED,			GPIO_MODE_OUTPUT_PP,GPIO_PULLUP)
#define INIT_OUTPUT_FULL_LED		sgdIOConfig(PORT_FULL_LED, PIN_FULL_LED,				GPIO_MODE_OUTPUT_PP,GPIO_PULLUP)
#define INIT_OUTPUT_BOOT			sgdIOConfig(GPIOF, GPIO_PIN_4,					GPIO_MODE_OUTPUT_PP,GPIO_PULLUP)
#define INIT_OUTPUT_CSDA				sgdIOConfig(PORT_CSDA, PIN_CSDA,			GPIO_MODE_OUTPUT_OD,GPIO_PULLUP)
#define INIT_OUTPUT_CLK				sgdIOConfig(PORT_CLK, PIN_CLK,					GPIO_MODE_OUTPUT_OD,GPIO_PULLUP)
#define INIT_OUTPUT_F_EN			sgdIOConfig(PORT_F_EN, PIN_F_EN,					GPIO_MODE_OUTPUT_PP,GPIO_PULLUP)

#define INIT_INPUT_CHG				sgdIOConfig(PORT_CHG, PIN_CHG,					GPIO_MODE_INPUT,GPIO_PULLUP)
#define INIT_INPUT_POWER_5VCTRL		sgdIOConfig(PORT_POWER_5VCTRL, PIN_POWER_5VCTRL,GPIO_MODE_INPUT,GPIO_PULLUP)
#define INIT_INPUT_IP5306_KEY		sgdIOConfig(PORT_IP5306_KEY, PIN_IP5306_KEY,	GPIO_MODE_INPUT,GPIO_PULLUP)
#define INIT_INPUT_CHG_LED			sgdIOConfig(PORT_CHG_LED, PIN_CHG_LED,			GPIO_MODE_INPUT,GPIO_PULLUP)
#define INIT_INPUT_FULL_LED			sgdIOConfig(PORT_FULL_LED, PIN_FULL_LED,				GPIO_MODE_INPUT,GPIO_PULLUP)
#define INIT_INPUT_BAT_LED			sgdIOConfig(PORT_BAT_LED, PIN_BAT_LED,			GPIO_MODE_INPUT,GPIO_PULLUP)

#define INIT_INPUT_FLASH_BT			sgdIOConfig(PORT_FLASH_BT, PIN_FLASH_BT,		GPIO_MODE_INPUT,GPIO_PULLUP)
#define INIT_INPUT_FLASH_IN			sgdIOConfig(PORT_FLASH_IN, PIN_FLASH_IN,		GPIO_MODE_INPUT,GPIO_PULLUP)

#define INIT_INPUT_S1				sgdIOConfig(PORT_S1, PIN_S1,						GPIO_MODE_INPUT,GPIO_PULLUP)
#define INIT_INPUT_S2				sgdIOConfig(PORT_S2, PIN_S2,						GPIO_MODE_INPUT,GPIO_PULLUP)
#define INIT_INPUT_XFULL			sgdIOConfig(PORT_XFULL, PIN_XFULL,				GPIO_MODE_INPUT,GPIO_PULLUP)


#define INIT_OUTPUT_IGBT_G			sgdIOConfig(PORT_IGBT_G, PIN_IGBT_G,					GPIO_MODE_OUTPUT_PP,GPIO_PULLDOWN)

//#define INIT_INPUT_DOWN_ADB			sgdIOConfig(PORT_ADB, PIN_ADB,GPIO_MODE_ANALOG,GPIO_PULLUP)
//#define INIT_INPUT_DOWN_IP5306_L1	sgdIOConfig(PORT_IP5306_L1, PIN_IP5306_L1,GPIO_MODE_INPUT,GPIO_PUUP)
//#define INIT_OUTPUT_BAT_LED			sgdIOConfig(PORT_BAT_LED, PIN_BAT_LED,GPIO_MODE_OUTPUT_PP,GPIO_PULLUP)
//#define INIT_OUTPUT_F_EN			sgdIOConfig(PORT_F_EN, PIN_F_EN,GPIO_MODE_OUTPUT_PP,GPIO_PULLUP)
//#define INIT_OUTPUT_F_ON			sgdIOConfig(PORT_F_ON, PIN_F_ON,GPIO_MODE_OUTPUT_PP,GPIO_PULLUP)










#define IS_HIGH_PB12					HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12)

#define IS_HIGH_TTL_M					HAL_GPIO_ReadPin(PORT_TTL_M,PIN_TTL_M )



#define IS_HIGH_USB_DEC					HAL_GPIO_ReadPin(PORT_USB_DEC, PIN_USB_DEC )

#define IS_HIGH_CSDA					HAL_GPIO_ReadPin(PORT_CSDA, PIN_CSDA )



#define IS_HIGH_COMP1OUT				 	HAL_GPIO_ReadPin(PORT_COMP1OUT,PIN_COMP1OUT)

#define IS_HIGH_CAMERA				HAL_GPIO_ReadPin(PORT_CAMERA, PIN_CAMERA )
#define IS_HIGH_FLASH_BT			HAL_GPIO_ReadPin(PORT_FLASH_BT,PIN_FLASH_BT )
#define IS_HIGH_FLASH_IN			HAL_GPIO_ReadPin(PORT_FLASH_IN,PIN_FLASH_IN )

#define IS_HIGH_ADB				 	HAL_GPIO_ReadPin(PORT_ADB,PIN_ADB)
#define IS_HIGH_S2					HAL_GPIO_ReadPin(PORT_S2,PIN_S2)
#define IS_HIGH_S1					HAL_GPIO_ReadPin(PORT_S1,PIN_S1)
#define IS_HIGH_XFULL				HAL_GPIO_ReadPin(PORT_XFULL,PIN_XFULL)
#define IS_HIGH_SWITCH_DEC			0//HAL_GPIO_ReadPin(PORT_SWITCH_DEC,PIN_SWITCH_DEC)
#define IS_HIGH_CHG					HAL_GPIO_ReadPin(PORT_CHG, PIN_CHG )
#define IS_HIGH_IP5306_KEY			HAL_GPIO_ReadPin(PORT_IP5306_KEY, PIN_IP5306_KEY )
#define IS_HIGH_IGBT_G 			HAL_GPIO_ReadPin(PORT_IGBT_G, PIN_IGBT_G )




#define SET_CSDA						HAL_GPIO_WritePin(PORT_CSDA, PIN_CSDA,GPIO_PIN_SET)
#define SET_SCK						HAL_GPIO_WritePin(PORT_CLK, PIN_CLK,GPIO_PIN_SET)
#define SET_F_ON					HAL_GPIO_WritePin(PORT_F_ON, PIN_F_ON,GPIO_PIN_SET)
//#define SET_F_EN					 HAL_GPIO_WritePin(PORT_F_EN, PIN_F_EN,GPIO_PIN_SET)
#define SET_CHG						HAL_GPIO_WritePin(PORT_CHG, PIN_CHG,GPIO_PIN_SET )
#define SET_CHG_LED					HAL_GPIO_WritePin(PORT_CHG_LED, PIN_CHG_LED ,GPIO_PIN_SET)
#define SET_FULL_LED				HAL_GPIO_WritePin(PORT_FULL_LED, PIN_FULL_LED,GPIO_PIN_SET )
#define SET_BAT_LED					HAL_GPIO_WritePin(PORT_BAT_LED, PIN_BAT_LED ,GPIO_PIN_SET)
#define SET_IP5306_KEY				HAL_GPIO_WritePin(PORT_IP5306_KEY, PIN_IP5306_KEY ,GPIO_PIN_SET)
#define SET_IGBT_G					HAL_GPIO_WritePin(PORT_IGBT_G, PIN_IGBT_G ,GPIO_PIN_SET)
#define TOGGLE_CHG_LED				HAL_GPIO_TogglePin(PORT_CHG_LED, PIN_CHG_LED )

#define RESET_CSDA						HAL_GPIO_WritePin(PORT_CSDA, PIN_CSDA,GPIO_PIN_RESET)
#define RESET_SCK						HAL_GPIO_WritePin(PORT_CLK, PIN_CLK,GPIO_PIN_RESET)
#define RESET_F_ON					HAL_GPIO_WritePin(PORT_F_ON, PIN_F_ON ,GPIO_PIN_RESET)
//#define RESET_F_EN				// HAL_GPIO_WritePin(PORT_F_EN, PIN_F_EN ,GPIO_PIN_RESET)
#define RESET_CHG					HAL_GPIO_WritePin(PORT_CHG, PIN_CHG ,GPIO_PIN_RESET)
#define RESET_CHG_LED				HAL_GPIO_WritePin(PORT_CHG_LED, PIN_CHG_LED ,GPIO_PIN_RESET)
#define RESET_FULL_LED				HAL_GPIO_WritePin(PORT_FULL_LED, PIN_FULL_LED ,GPIO_PIN_RESET)
#define RESET_BAT_LED				HAL_GPIO_WritePin(PORT_BAT_LED, PIN_BAT_LED,GPIO_PIN_RESET )
#define RESET_IP5306_KEY			HAL_GPIO_WritePin(PORT_IP5306_KEY, PIN_IP5306_KEY,GPIO_PIN_RESET)
#define RESET_IGBT_G					HAL_GPIO_WritePin(PORT_IGBT_G, PIN_IGBT_G ,GPIO_PIN_RESET)
#ifdef FUGU

#define SET_POWER_5VCTRL			HAL_GPIO_WritePin(PORT_POWER_5VCTRL, PIN_POWER_5VCTRL,GPIO_PIN_SET)
#define RESET_POWER_5VCTRL			HAL_GPIO_WritePin(PORT_POWER_5VCTRL, PIN_POWER_5VCTRL,GPIO_PIN_RESET)
#define IS_HIGH_POWER_5VCTRL		HAL_GPIO_ReadPin(PORT_POWER_5VCTRL, PIN_POWER_5VCTRL )
#endif
#ifndef FUGU

#define RESET_POWER_5VCTRL			HAL_GPIO_WritePin(PORT_POWER_5VCTRL, PIN_POWER_5VCTRL,GPIO_PIN_SET)
#define SET_POWER_5VCTRL			HAL_GPIO_WritePin(PORT_POWER_5VCTRL, PIN_POWER_5VCTRL,GPIO_PIN_RESET)
#define IS_HIGH_POWER_5VCTRL		(!(HAL_GPIO_ReadPin(PORT_POWER_5VCTRL, PIN_POWER_5VCTRL )))
#endif
//
//#define SET_POWER_5VCTRL			HAL_GPIO_WritePin(PORT_POWER_5VCTRL, PIN_POWER_5VCTRL,GPIO_PIN_SET)
//#define RESET_POWER_5VCTRL			HAL_GPIO_WritePin(PORT_POWER_5VCTRL, PIN_POWER_5VCTRL,GPIO_PIN_RESET)
//#define IS_HIGH_POWER_5VCTRL		HAL_GPIO_ReadPin(PORT_POWER_5VCTRL, PIN_POWER_5VCTRL )

 
	

enum SwitchState{
	SwitchState_Off=0,
	SwitchState_On,
};

enum SMode{
	SMode_S1 = 1,
	SMode_S2,
	SMode_Off
};

enum PowerLevel{
	PowerLevel_1_1=0,
	PowerLevel_1_2,
	PowerLevel_1_4,
	PowerLevel_1_8,
	PowerLevel_1_16,
	PowerLevel_1_32,
	PowerLevel_1_64,
};


enum BatteryState{
	BatteryState_25=0,
	BatteryState_50,
	BatteryState_75,
	BatteryState_100,
	BatteryState_Chargineg,
	BatteryState_ChargeFull,
};


void sgdStopFON(void);
void sgdStartFON(void);


	
int timeGone(unsigned int rtcPre);
void sgdInit(void);
void sgdLoop(void);
void setChhgTick(void);
void sgdStartFlash(void);
void sgdStartSFlash(void);
void Delay_ms(int delayMS);	
 void sgdIOConfig(GPIO_TypeDef 			   *GPIOx, uint32_t pin,uint32_t mode, uint32_t pull  );
int getSMode(void);


void sgdDmaChannel1IRQCallback(USART_TypeDef *USARTx);
void sgdDmaChannel2_3_IRQCallback(USART_TypeDef *USARTx);
void sgdUsartIRQCallback(USART_TypeDef *USARTx);

#endif

