
#include "KTH57XXIIC.h"
#include "MyI2C.h"
#include "ADC.h"
#include "OnePluseFlash.h"
#include "main.h"
#include "BatteryLed.h"
#include "string.h"
#include "sgd.h"
#include "SEGGER_RTT.h"
#include "py32f072xx_Start_Kit.h" 

// 控制电池LED灯的函数
#define BATTERY_LED_TIME 500  // LED闪烁时间间隔

// 电压检测相关常量定义
#define SAMPLE_COUNT 7             // 保存最近的样本数
#define LOW_THRESHOLD 2900         // 低电压的阈值
#define HIGH_THRESHOLD 4400        // 高电压的阈值
#define MIN_LOW_COUNT 1            // 前后低电压的最少连续个数
#define MIN_HIGH_COUNT 2           // 中间高电压的最少连续个数

// 输入检测时间戳
int _inputTick = 1;


// 保存最近的电压值的缓冲区
static int voltageBuffer[SAMPLE_COUNT] = {0};
// 当前写入位置
static int index = 0;
// 标记缓冲区是否填满
static int bufferFull = 0;

//#define demoDeug 1 

// S1 S2 选择模式，1: S1，2: S2，3: OFF
uint8_t sMode  = 0;

// 闪光灯手动模式功率等级，范围 0 - 6
uint8_t powerLevel = 0;

// 转轮顺序数组
///uint8_t flashPowerArrayOrder[8] = {3, 4, 1, 5, 2, 0, 2, 6};

// 每个功率等级 IGBT 门级脉冲持续的时间，单位 us
uint32_t flashPowerArray[8] = {3111, 284, 144, 83, 49, 32, 22, 5000 };
// uint32_t flashPowerArray[8] = {3111, 3111, 3111, 3111, 3111, 3111, 3111, 5000 };
// 1 - 14 低电压  3250 -> 3050
// 1 - 17 添加电压低于 3000 进入红灯闪烁

// 设备状态相关变量
int switchState = 0;  // 0 - 关机，1 - 手动，2 - ttl
//int ip5306L1State = 0;  // 0 - 灭，1 - 亮，2 - 闪烁

// 时间戳相关变量
int flashTick  = 10;  // 最后一次闪光的时间戳
int userOperateTick  = 10;  // 用户最后一次操作的时间戳
int _chgTick = 1;  // 电容上一次充电的时间戳
int _chgFullTick = 1;  // 电容上一次充满的时间戳
//int _chgTotalTime = 1;  // 电容上一次充满的时间戳
int _swTick = 1;  // 最后一次检测到 sw 的时间戳
//int _batteryLowTick = 1;  // 最后电压高于 3600mv 的时间戳

// USB 相关状态变量
int usbHostConnected = 0;
int usbInsertion = 0;

// 电池电压低状态标志
int lowBatteryState = 0;

// 时间计数相关变量
uint32_t _oneFlashTick = 2;
uint32_t _igbtGateTick = 1;
uint32_t _flashInTick = 1;
uint32_t _oneFlashCount = 1;
uint32_t _igbtGateCount = 1;
uint32_t _flashInCount = 1;
uint32_t _currentTemp = 1;

// 电容是否充满标志
_Bool isCapFull = FALSE;

// IP5306 高低电平计数变量
//int ip5306HighLevelCount = 0;
//int ip5306LowLevelCount = 0;

// 电池状态，初始为 25%
int batteryState = BatteryState_25;

// 电池 LED 配置函数声明
void sgdBatteryLedConfige(int pluseRate);

// 函数声明
void sgdOnePulseConfig(int pluseUS);

/**
 * @brief 睡眠检查函数，若未插入 USB 且用户操作超时则进行系统复位
 */
void sgdSleepCheck() {
    if (((IS_HIGH_USB_DEC)) && (timeGone(userOperateTick) > 900000)) {
        SEGGER_RTT_printf(0, "userOperateTick  sgdEnterStop\r\n");
        // 未插入 USB 且超时进入休眠
        HAL_NVIC_SystemReset();
        // Delay_ms(1000);
        // sgdEnterStop();
    }
}

/**
 * @brief 获取 S 模式的值
 * @return 当前的 S 模式值
 */
int getSMode() {
    return sMode;
}

/**
 * @brief 使电容充电上升操作
 */
void sgdMakeCHGRise() {
    // SEGGER_RTT_printf(0, "sgdMakeCHGRise	 =	%d\r\n", (getTick()));
    sgdEnableSwInt();
    _swTick = getTick();
    _chgTick = getTick();
    RESET_CHG;
    SET_CHG;
}


/**
 * @brief 读取 IP5306 状态并更新电池状态
 */



/**
 * @brief 检查最后 7 个数值是否符合“前低、中高、后低”模式
 * @param voltage 当前电池电压
 * @param realVol 实际电池电压
 * @return 如果匹配返回 1，否则返回 0
 */
int checkBatteryLow(int voltage, int realVol) {
    // 将当前电压值存入循环缓冲区
    voltageBuffer[index] = voltage;
    index = (index + 1) % SAMPLE_COUNT;

    // 如果缓冲区已满，设置标志
    if (index == 0) {
        bufferFull = 1;
    }

    // 如果缓冲区未填满，直接返回
    if (!bufferFull) {
        return 0;
    }

    // 若实际电压在 2200 - 3000 之间，返回 1
    if (realVol < 3000 && realVol > 2200) {
        SEGGER_RTT_printf(0, "realVol = %d\r\n", realVol);
        return 1;
    }

    // 检查前段是否连续低电压
    int lowStartCount = 0;
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        if (voltageBuffer[i] < LOW_THRESHOLD) {
            lowStartCount++;
        } else {
            break;  // 非低电压时中断计数
        }
    }

    // 检查中间是否连续高电压
    int highMiddleCount = 0;
    int isMiddle = 0;
    for (int i = lowStartCount; i < SAMPLE_COUNT; i++) {
        if (voltageBuffer[i] > HIGH_THRESHOLD) {
            highMiddleCount++;
            isMiddle = 1;
        } else if (isMiddle) {
            break;  // 高电压结束后中断计数
        }
    }

    // 检查后段是否连续低电压
    int lowEndCount = 0;
    for (int i = SAMPLE_COUNT - 1; i >= 0; i--) {
        if (voltageBuffer[i] < LOW_THRESHOLD) {
            lowEndCount++;
        } else {
            break;  // 非低电压时中断计数
        }
    }

    // 判断是否符合“前低、中高、后低”的模式
    if (lowStartCount >= MIN_LOW_COUNT &&
        highMiddleCount >= MIN_HIGH_COUNT &&
        lowEndCount >= MIN_LOW_COUNT &&
        (lowStartCount + highMiddleCount + lowEndCount == SAMPLE_COUNT)) {
        SEGGER_RTT_printf(0, "low ======\r\n");
        return 1;
    }

    return 0;
}

/**
 * @brief 获取输入并处理相关逻辑
 */
void sgdGetInput() {
    // 保存之前的功率等级、模式和 USB 状态
    static int preFlashPower = 1;
    static int presMode = 1;
    static int preUSBDec = 1;

    // 每 50ms 检查一次
    if (timeGone(_inputTick) > 50) {
        _inputTick = getTick();

        // 电容没充满，不检查
        // if (!(IS_HIGH_XFULL)) {}

        // 读取 ADC、角度等信息
        sgdReadAdc();
        sgdGetADCres();
        sgdReadAngle();

        // 读取 IP5306 状态
      //  sgdReadIP5306();
		

        // 检查电池是否低电压
        if (checkBatteryLow(sgdGetADCres(),getFilterBat_mV())) {
            lowBatteryState = 1;
        }

        // 根据开关状态更新 switchState
        switchState = IS_HIGH_SWITCH_DEC ? SwitchState_Off : SwitchState_On;
        // 获取当前功率等级
        powerLevel = sgdGetPowerLevel();
        // 获取当前温度
        _currentTemp = sgdGetADCTempres();
        // 更新 USB 插入状态
        usbInsertion = IS_HIGH_USB_DEC ? 1 : 0;
        if (usbInsertion == 0) {
            usbHostConnected = 0;
        }

        // 根据 S1、S2 状态更新 sMode
        if (!IS_HIGH_S2) {
            sMode = SMode_S1;
        } else if (!IS_HIGH_S1) {
            sMode = SMode_S2;
            // sgdEnterStop();
        } else {
            sMode = SMode_Off;
        }

        // 如果功率等级、USB 状态或模式发生变化
        if (preFlashPower != powerLevel || usbInsertion != preUSBDec || sMode != presMode) {
//            SEGGER_RTT_printf(0, "\r\nsgdGetInput:powerLevel =%d 5306state = %d vbat = %dmv v1_5 = %dmv batState = %d \r\n operateTick = %d s1 = %d s2 = %d xfull =  %d switch = %d usb = %d \r\n",
//                              powerLevel, 1, sgdGetADCres(), sgdGetADC1v5res(), batteryState, timeGone(userOperateTick), IS_HIGH_S1, IS_HIGH_S2, IS_HIGH_XFULL, IS_HIGH_SWITCH_DEC, usbHostConnected);
            // 更新用户操作时间戳
            userOperateTick = getTick();
            // 更新保存的功率等级、USB 状态和模式
            preFlashPower = powerLevel;
            preUSBDec = usbInsertion;
            presMode = sMode;

            // 打印日志
            printFlashLog();

            // 根据功率等级配置单脉冲
            if (powerLevel >= 0 && powerLevel < 7) {
                sgdOnePulseConfig(flashPowerArray[powerLevel]);
            } else {
                sgdOnePulseConfig(100);
            }
        }
    }
}

/**
 * @brief 初始化退出相关的 IO
 */
void sgdInitExitIO() {

 
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
  
  GPIO_InitStruct.Mode  = GPIO_MODE_IT_RISING;  /* GPIO mode is a falling edge interrupt */
  GPIO_InitStruct.Pull  = GPIO_PULLDOWN;           /* pull up */
  GPIO_InitStruct.Pin = PIN_IGBT_G;
  HAL_GPIO_Init(PORT_IGBT_G, &GPIO_InitStruct);
  

  /* Enable EXTI interrupt */

  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
  /* Configure interrupt priority */
  
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);

  HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);
  /* Configure interrupt priority */
  
  HAL_NVIC_SetPriority(EXTI2_3_IRQn, 0, 0);



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
// void sgdIOInit(){
//	 // 初始化输入输出端口，配置硬件相关功能
//	 INIT_OUTPUT_FULL_LED;			// 初始化全亮LED输出
//	 INIT_OUTPUT_CHG_LED;			// 初始化充电LED输出
//	 
//	 INIT_INPUT_TTL_M;				// 初始化TTL输入（M）
//	 INIT_INPUT_CAMERA; 			// 初始化摄像头输入
//	 INIT_INPUT_USB_DEC;			// 初始化USB解码输入
//	 INIT_INPUT_FLASH_BT;			// 初始化Flash按钮输入
//	 INIT_INPUT_FLASH_IN;			// 初始化Flash输入
//	 INIT_INPUT_USB_DEC;			// 再次初始化USB解码输入
//	 INIT_INPUT_SWITCH_DEC; 		// 初始化开关解码输入
//	 INIT_INPUT_UP_S1;				// 初始化上按键S1输入
//	 INIT_INPUT_UP_S2;				// 初始化上按键S2输入
//	 INIT_INPUT_UP_XFULL;			// 初始化上按键XFULL输入
// 
//	 SEGGER_RTT_printf(0, "  \r\n");
// 
//	 INIT_OUTPUT_F_EN;				// 初始化F_EN输出
//	 RESET_F_EN;					// 复位F_EN
// 
//	 INIT_OUTPUT_CHG;				// 初始化充电输出
//	 INIT_OUTPUT_POWER_5VCTRL;		// 初始化5V控制电源输出
// }
// 
 void sgdOpen5VPower(){
	 SET_POWER_5VCTRL;				// 开启5V电源控制
 }
 
 void varInit(){
	 // 初始化变量
	 _chgTick = 1;					// 设置充电计时器的初始值
	 flashTick = 1; 				// 设置闪光计时器的初始值
	 userOperateTick = 0;			// 用户操作计时器初始化
 }
 
 void sgdChargeCtrl(){
	 // 控制充电过程
	 if(IS_HIGH_SWITCH_DEC){
		 RESET_FULL_LED;			// 复位全亮LED
		 RESET_CHG_LED; 			// 复位充电LED
		 SEGGER_RTT_printf(0, "IS_HIGH_SWITCH_DEC  == 1 \r\n");
		 return;
	 }
 
	 if(_chgFullTick < _chgTick){
//		 if(!(IS_HIGH_XFULL)){
//			 chargingComplete(getTick());  // 完成充电
//			 _chgFullTick = getTick();	  // 更新充电完成时间
//			 isCapFull = TRUE;			  // 设置电容已充满标志
//		 }
	 }
 
	 if(getMissState() == 0){
		 SEGGER_RTT_printf(0, "getMissState \r\n");
		 SET_FULL_LED;					// 设置全亮LED
		 RESET_CHG_LED; 				// 复位充电LED
		 for(int i = 0; i < 5; i++){
			 Delay_ms(100);
			 SET_FULL_LED;
			 Delay_ms(100);
			 RESET_FULL_LED;
		 }
		 resetMissState();			   // 重置丢失状态
		 SEGGER_RTT_printf(0, "getMissState  end\r\n");
	 }
 
	 if(getIgbtState() == 0){
		 SET_FULL_LED;					// 设置全亮LED
		 SET_CHG_LED;					// 设置充电LED
		 for(int i = 0; i < 50; i++){
			 Delay_ms(100);
			 SET_FULL_LED;
			 Delay_ms(100);
			 RESET_FULL_LED;
		 }
	 }
 
	 if(lowBatteryState == 1){	
		 // 低电池状态
		 SEGGER_RTT_printf(0, "111low Battery =%d MIN_WORK_BATTERY_VOL= %d\r\n", sgdGetBatteryV(), MIN_WORK_BATTERY_VOL);
	// RESET_F_EN;					// 复位F_EN
		 RESET_CHG; 					// 复位充电
		 while(IS_HIGH_USB_DEC || (!IS_HIGH_USB_DEC && (sgdGetBatteryV() < (500 + MIN_WORK_BATTERY_VOL)))){
			 SET_CHG_LED;				// 设置充电LED
			 RESET_FULL_LED;			// 复位全亮LED
			 Delay_ms(500);
			 RESET_CHG_LED; 			// 复位充电LED
			 RESET_FULL_LED;			// 复位全亮LED
			 sgdGetInput(); 			// 获取输入
			 Delay_ms(500);
			 SEGGER_RTT_printf(0, "low Battery =%d\r\n", sgdGetBatteryV());
 
			 if(IS_HIGH_SWITCH_DEC) break;	// 检查是否为高电平开关
		 }
	 } else {
		 // 正常工作流程
		 if(IS_HIGH_POWER_5VCTRL && IS_HIGH_CHG){
			 if (!isCapFull){
				 SET_CHG_LED;			// 设置充电LED
				 RESET_FULL_LED;		// 复位全亮LED
			 } else {
				 int _preiod = (sgdGetADCTempres() > 90) ? 200 : 400;
				 int _fullPluseTick  = (getTick() % _preiod);
				 if(_fullPluseTick > _preiod / 2){
					 SET_FULL_LED;		// 设置全亮LED
				 } else {
					 if(isTempCanFlash() == 0){
						 RESET_FULL_LED;  // 复位全亮LED
					 }
				 }
				 RESET_CHG_LED; 		// 复位充电LED
			 }
		 } else {
			 SET_FULL_LED;				// 设置全亮LED
			 SET_CHG_LED;				// 设置充电LED
		 }
	 }
 
	 if(IS_HIGH_POWER_5VCTRL){
		 if(timeGone(_chgTick) > 2000 && (_chgFullTick > _chgTick)){
			 sgdMakeCHGRise();			// 使充电升高
			 startCharging(getTick(), FALSE); // 开始充电
		 } else if((!IS_HIGH_CHG) && (timeGone(_chgTick) > 5)){
			 sgdMakeCHGRise();			// 使充电升高
			 startCharging(getTick(), TRUE);  // 开始充电
		 } else if(timeGone(_chgTick) > 5000){
			 sgdMakeCHGRise();			// 使充电升高
		 }
	 }
 }
 
 void sgdStartFlash(){
	 userOperateTick = getTick();	// 更新用户操作计时器
	 
//	 if((IS_HIGH_SWITCH_DEC) || (!isCapFull)){
//		 // 电容未充满，跳过闪光
//		 return;
//	 }
//	 if(isTempCanFlash() == 0){
//		 // 如果温度过高，不能闪光
//		 return;
//	 }
//	 
	 RESET_CHG; 					// 复位充电
	 _oneFlashTick = getTick(); 	 // 设置闪光时间
	 isCapFull = FALSE; 			 // 设置电容未充满标志
	 sgdStartOnePluse();			 // 启动一次闪光
 
	 flashTriggered(getTick(), createLogEntry(0, powerLevel, IS_HIGH_SWITCH_DEC, IS_HIGH_USB_DEC)); 
 }
 
 void sgdSetAndFlash(int plus){
	 SEGGER_RTT_printf(0, "sgdSetAndFlash------------------ plus  = %d \r\n", plus);
	 sgdOnePulseConfig(plus);		 // 配置闪光脉冲
	 sgdStartFlash();				 // 启动闪光
 }
 
 int isTempCanFlash(){
	 // 判断温度是否适合闪光
	 if(_currentTemp > 90){
		 return 0;	 // 温度过高，不能闪光
	 }
	 else if((_currentTemp > 80) && (_currentTemp < 90) && (timeGone(_oneFlashTick) < 10000)){
		 return 0;	 // 温度适中但间隔小于10秒，不能闪光
	 }
	 else {
		 return 1;	 // 可以闪光
	 }
 }
 
 void setIgbtGateTick(){
	 _igbtGateTick = getTick(); 	 // 设置IGBT门控信号的时间
 }
 
 void sgdStartSFlash(){
	 SEGGER_RTT_printf(0, "sgdStartSFlash------------------ sMode  = %d  %d %d\r\n", sMode, flashTick, getTick());
	 
	 if(sMode == SMode_S1){
		 sgdStartFlash();			   // 如果是S1模式，启动闪光
	 } else if(sMode == SMode_S2 && timeGone(flashTick) < 2000){
		 SEGGER_RTT_printf(0, "sgdStartSFlash ************************	flashTick = %d	%d\r\n", flashTick, getTick());
		 sgdStartFlash();			   // 如果是S2模式并且时间间隔小于2秒，启动闪光
	 }
 
	 flashTick = getTick(); 		  // 更新闪光计时器
 }
 
 void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	 UNUSED(GPIO_Pin);				  // 防止未使用参数的编译警告
 
	 if(GPIO_Pin == PIN_SW_DEC){	  // 开关解码按键中断
		 sgdDisableSwInt(); 		  // 禁用开关中断
		 _swTick = getTick();		  // 更新开关按键计时器
	 }
	 
	 else if(GPIO_Pin == PIN_CAMERA){ // 摄像头按键中断
		 sgdStartFlash();			   // 启动闪光
		 _oneFlashCount++;			   // 增加闪光计数
		 SEGGER_RTT_printf(0, "PIN_CAMERA	 %d  %d  count	= %d \r\n", PIN_CAMERA, getTick(), _oneFlashCount);
	 }
	 else if(GPIO_Pin == PIN_FLASH_BT){ // Flash按钮按键中断
		 sgdStartFlash();			   // 启动闪光
		 _oneFlashCount++;			   // 增加闪光计数
		 SEGGER_RTT_printf(0, "PIN_FLASH_BT    %d  %d  count  = %d	 bat = %d \r\n", PIN_FLASH_BT, getTick(), _oneFlashCount, sgdGetBatteryV());
	 }
	 else if(GPIO_Pin == PIN_FLASH_IN){ // Flash输入按键中断
		 sgdStartSFlash();			   // 启动S模式闪光
		 _flashInCount++;			   // 增加Flash输入计数
		 _flashInTick = getTick();	   // 更新Flash输入时间
		 SEGGER_RTT_printf(0, "PIN_FLASH_IN  s %d  %d  count  = %d \r\n", PIN_FLASH_IN, getTick(), _flashInCount);
	 }
	 else if(GPIO_Pin == PIN_IGBT_G){  // IGBT门控按键中断
		 SEGGER_RTT_printf(0, "----IGBT_G plus = %d  igbt =  %d  gone = %d	last = %d  count  = %d	   igbt lou = %d  in lou = %d  ilou = %d\r\n", _oneFlashTick, _igbtGateTick, timeGone(_oneFlashTick), timeGone(_igbtGateTick), _igbtGateCount, (_oneFlashCount - _igbtGateCount), (_oneFlashCount - _flashInCount), (_igbtGateCount - _flashInCount));
		 _igbtGateTick = getTick();    // 更新IGBT门控时间
		 _igbtGateCount++;			   // 增加IGBT门控计数
	 }
	
 }


// 控制IP5306的函数
void sgdIP5036Ctrl() {
// 定义全局变量
    static int _ip5306Tick = 1;  // 用于存储IP5306计时器的计时值
    if (timeGone(_ip5306Tick) > 100) {  // 每100ms检查一次
        _ip5306Tick = getTick();  // 更新计时器
        // 如果ADC值在1300到1700之间，停止IP5306定时器
        if (sgdGetADC1v5res() > 1300 && sgdGetADC1v5res() < 1700) {
            sgdStop5306KeyTimer();
        } else {
            if (sgdIsIp5306TimerStop()) {
                // 如果定时器已经停止，配置IP5306
                SEGGER_RTT_printf(0, "sgdIsIp5306TimerStop+++++++++++++++++++++++++++++++++++++++------------------  =  %d\r\n", sgdGetADC1v5res());
                sgdIp5306KeyConfige();
            }
        }
    }
}
void sgdBatteryLedCtrl() {
    static int _batteryTick = 10;  // 电池计时器
    static int _FEN_IN = 0;  // 控制电池LED状态的变量
    int now = getTick();  // 获取当前时间
    int pluse = now % 1000;  // 获取当前时间的脉冲值

    // 每5ms执行一次
    if (timeGone(_batteryTick) > 5 && now % 10 == 0) {
        _batteryTick = getTick();

        // 根据电池状态控制LED的闪烁模式
        switch (batteryState) {
            case BatteryState_100:
            case BatteryState_75:
            case BatteryState_50:
                // 当电池状态为100%、75%、50%时，控制LED闪烁
                if (timeGone(1) > 10000) {
                    sgdSetBatteryLedPluse(0);  // 关闭LED闪烁
                    break;
                }
            case BatteryState_25:
                // 电池电量为25%时，控制LED闪烁的频率
                if (now % (BATTERY_LED_TIME * 10) < (1 + batteryState) * BATTERY_LED_TIME) {
                    if (now % BATTERY_LED_TIME < BATTERY_LED_TIME / 2) {
                        sgdSetBatteryLedPluse(0);  // LED熄灭
                    } else {
                        sgdSetBatteryLedPluse(1000);  // LED亮
                    }
                } else {
                    sgdSetBatteryLedPluse(0);  // LED熄灭
                }
                break;

            case BatteryState_ChargeFull:
                // 电池充满时，LED常亮
                sgdSetBatteryLedPluse(1000);
                break;

            case BatteryState_Chargineg:
                // 充电状态下，LED闪烁
                if (now % 2000 >= 1000) {
                    pluse = 1000 - now % 1000;
                }
                sgdSetBatteryLedPluse(pluse + 10);  // 设置LED的亮度
                break;
        }
    }
}

// 等待5V电压准备好
void sgdWait5VReady() {
    while (!(sgdGetADC1v5res() > 1300 && sgdGetADC1v5res() < 1700)) {
        // 等待5V电源电压稳定
        sgdReadAdc();
        SEGGER_RTT_printf(0, "wait 5v vbat = %d  ick= %d\r\n", sgdGetADC1v5res(), getTick());
        Delay_ms(20);  // 延时20ms
        if (IS_HIGH_SWITCH_DEC) {
            RESET_POWER_5VCTRL;  // 关闭5V电源
        }
    }
    SEGGER_RTT_printf(0, "5v ready tick = %d\r\n", getTick());  // 5V电源准备好
}

// 自动闪光控制
void sgdAutoFlash() {
	static int t = 1;
    if (sMode == 2) {
        if (timeGone(t) > 10000) {  // 每10秒执行一次
            t = getTick();
            sgdStartFlash();  // 启动闪光
        }
    }
}
//
//// 系统初始化函数
//void sgdInit() {
//    SEGGER_RTT_printf(0, "sgdInit  1123   start\r\n");
//    sgdClockEnable();  // 启用时钟
//
//    SEGGER_RTT_printf(0, "sgdIOInit   start\r\n");
//    sgdIOInit();  // 初始化IO
//    Delay_ms(100);
//
//    SEGGER_RTT_printf(0, "sgdBatteryLedConfige   start\r\n");
//    sgdBatteryLedConfige(800);  // 配置电池LED
//    SEGGER_RTT_printf(0, "sgdADCInit   start\r\n");
//    sgdADCInit();  // 初始化ADC
//
//    SEGGER_RTT_printf(0, "5306Key   start\r\n");
//    sgdIp5306KeyConfige();  // 配置IP5306按键
//    SEGGER_RTT_printf(0, "I2C   start\r\n");
//    sgdI2C_Init();  // 初始化I2C
//
//    SEGGER_RTT_printf(0, "ifdef FUGU \r\n");
//    sgdIp5306KeyConfige();  // 配置IP5306按键
//
//    Delay_ms(10);
//    SEGGER_RTT_printf(0, "IS_HIGH_SWITCH_DEC = %d    \r\n", IS_HIGH_SWITCH_DEC);
//
//    // 检查开关状态并进行相应操作  插入usb的时候停留在这里
//    while (IS_HIGH_SWITCH_DEC) {
//        if (timeGone(userOperateTick) > 10) {
//            userOperateTick = getTick();
//          //  sgdReadIP5306();
//            SEGGER_RTT_printf(0, "batteryState = %d    \r\n", batteryState);
//            sgdBatteryLedCtrl();
//        }
//    }
//
//    SEGGER_RTT_printf(0, "sgdOpen5VPower   start\r\n");
//    sgdOpen5VPower();  // 打开5V电源
//
//    SEGGER_RTT_printf(0, "sgdWait5VReady   start\r\n");
//    sgdWait5VReady();  // 等待5V电源准备好
//
//    sgdReadLog();  // 读取日志
//    SEGGER_RTT_printf(0, "sgdKT5763Init   start\r\n");
//    sgdKT5763Init();  // 初始化KT5763
//    SEGGER_RTT_printf(0, "sgdOnePulseConfig   start\r\n");
//    sgdOnePulseConfig(3000);  // 配置单脉冲模式
//
////    SEGGER_RTT_printf(0, "sgdFactoryInit   start\r\n");
////    sgdFactoryInit();  // 初始化工厂设置
////
////    SEGGER_RTT_printf(0, "sgdNoAngleInfo   start\r\n");
////    sgdNoAngleInfo();  // 设置无角度信息
//
//    Delay_ms(10);
//    sgdMakeCHGRise();  // 使充电上升
//    sgdInitExitIO();  // 初始化退出IO
//
//    SEGGER_RTT_printf(0, "sgdInit end \r\n");
//    //INIT_OUTPUT_F_EN;
//    userOperateTick = 100;
//
//    SEGGER_RTT_printf(0, "sgdInit   end\r\n");
//}


// 系统主循环
//void sgdLoop() {
//    if (IS_HIGH_SWITCH_DEC) {
//        // 关机操作
//       // RESET_F_EN;
//        RESET_POWER_5VCTRL;
//        RESET_FULL_LED;
//        RESET_CHG_LED;
//       // RESET_CHG;
//
//        if (!(IS_HIGH_USB_DEC)) {
//            if (timeGone(userOperateTick) > 10) {
//                userOperateTick = getTick();
//              //  sgdReadIP5306();
//                SEGGER_RTT_printf(0, "batteryState = %d    \r\n", batteryState);
//            }
//            sgdBatteryLedCtrl();  // 控制电池LED
//        }
//    } else {
//        // 开机操作
//        sgdGetInput();  // 获取输入
//        //SET_F_EN;
//        SET_POWER_5VCTRL;
//
//        sgdIP5036Ctrl();  // 控制IP5036
//        sgdChargeCtrl();  // 控制充电
//        sgdBatteryLedCtrl();  // 控制电池LED
//        sgdSleepCheck();  // 检查睡眠状态
//
//        // 检查闪光相关的时间差异
//        if (_oneFlashTick > _igbtGateTick && _igbtGateTick > 20) {
//            _igbtGateTick = _oneFlashTick;
//            SEGGER_RTT_printf(0, "----------------------_oneFlashTick - _igbtGateTick  > 10-----------------------sp\r\n");
//        }
//
//        if (_oneFlashTick > _flashInTick && _flashInTick > 20) {
//            _flashInTick = _oneFlashTick;
//            SEGGER_RTT_printf(0, "----------------------_oneFlashTick - _flashInTick  > 10-----------------------sp\r\n");
//        }
//
//        // sgdAutoFlash();  // 自动闪光
//    }
//}

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

		
		SEGGER_RTT_printf(0, "sgd1msConfig   start\r\n");
		sgd1msConfig( );
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
//		#endif
		
		sgdKT5763Init();
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
//int autoFLashTick  =1;
//
//void sgdSetUsbConnected(int connectState){
//	 usbHostConnected = connectState;
//	 //userOperateTick = getTick();
//}
//int sgdGetUsbConnected( ){
//	return  usbHostConnected  ;
//} 
//
//void sgdSleepRun(){
//	return;
//							SEGGER_RTT_printf(0, "sgdSleepRun\r\n ");
// 	__HAL_RCC_GPIOA_FORCE_RESET();
//	__HAL_RCC_GPIOB_FORCE_RESET();
//	__HAL_RCC_GPIOC_FORCE_RESET();
//	__HAL_RCC_GPIOF_FORCE_RESET();
//	__HAL_RCC_FLASH_CLK_DISABLE();
//	__HAL_RCC_TIM3_FORCE_RESET() ;
//	__HAL_RCC_TIM1_FORCE_RESET() ;
//	__HAL_RCC_TIM14_FORCE_RESET() ;
//	__HAL_RCC_TIM17_FORCE_RESET() ;
//	__HAL_RCC_SPI1_FORCE_RESET();
//	__HAL_RCC_ADC_FORCE_RESET();
//	
//	  HAL_NVIC_DisableIRQ(EXTI4_15_IRQn);
//	  /* Configure interrupt priority */
//	
//	  HAL_NVIC_DisableIRQ(EXTI2_3_IRQn);
//	
//	SEGGER_RTT_printf(0, "HAL_PWR_EnterSLEEPMode\r\n ");
//
//	HAL_PWR_EnterSTOPMode(0,0);
//
//	HAL_SuspendTick();
//
//	/* Enter STOP mode */
//	//HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
//	while(1){
//							SEGGER_RTT_printf(0, "while(1) \r\n" );
//	__HAL_RCC_GPIOB_CLK_ENABLE();
//	__HAL_RCC_GPIOC_CLK_ENABLE();
//	INIT_INPUT_FLASH_BT;
//	INIT_INPUT_USB_DEC;
//	int i =10;
//	while(i-->0){
//							SEGGER_RTT_printf(0, "while( i = %d) \r\n",i );
//							SEGGER_RTT_printf(0, "IS_HIGH_USB_DEC = %d   IS_HIGH_FLASH_BT=%d \r\n",IS_HIGH_USB_DEC,IS_HIGH_FLASH_BT);
//		;}
//	if(!IS_HIGH_USB_DEC  || !IS_HIGH_FLASH_BT)
//		{
//							SEGGER_RTT_printf(0, "break if(!IS_HIGH_USB_DEC  || !IS_HIGH_FLASH_BT)\r\n ");
//		
//			break;
//		}
////	__HAL_RCC_GPIOB_FORCE_RESET();
////	__HAL_RCC_GPIOC_FORCE_RESET();
//	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
//	}
//	/* Resume the SysTick interrupt */
//	HAL_ResumeTick();
//	sgdInit();
//	userOperateTick = getTick();
// 
//}
//void sgdOnSleepStart(){
//	  __HAL_RCC_GPIOB_CLK_ENABLE();
//	   __HAL_RCC_GPIOC_CLK_ENABLE();
//	   GPIO_InitTypeDef  GPIO_InitStruct = {0};
//	   GPIO_InitStruct.Mode  = GPIO_MODE_IT_FALLING;  /* GPIO mode is a falling edge interrupt */
//	   GPIO_InitStruct.Pull  = GPIO_PULLUP; 		  /* pull up */
//	   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  /* The speed is high */
//	   GPIO_InitStruct.Pin = PIN_USB_DEC;
//	   HAL_GPIO_Init(PORT_USB_DEC, &GPIO_InitStruct);
////	   GPIO_InitStruct.Pin = PIN_FLASH_BT;
////	   HAL_GPIO_Init(PORT_FLASH_BT, &GPIO_InitStruct);
//	 //  GPIO_InitStruct.Pull  = GPIO_PULLDOWN; 		  /* pull up */
//	 //  GPIO_InitStruct.Pin = PIN_BAT_LED;
//	 //  HAL_GPIO_Init(PORT_BAT_LED, &GPIO_InitStruct);
//	 //  GPIO_InitStruct.Pin = PIN_XFULL;
//	 //  HAL_GPIO_Init(PORT_XFULL, &GPIO_InitStruct);
//	 //  GPIO_InitStruct.Mode  = GPIO_MODE_IT_RISING;  /* GPIO mode is a falling edge interrupt */
//	 //  GPIO_InitStruct.Pull  = GPIO_PULLDOWN; 		  /* pull up */
//	 //  GPIO_InitStruct.Pin = PIN_SW_DEC;
//	 //  HAL_GPIO_Init(PORT_SW_DEC, &GPIO_InitStruct);
//	 
//	   
//	   HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
//	   /* Configure interrupt priority */
//	   HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
//	 HAL_PWR_EnterSTOPMode(0,0);
////HAL_GPIO_DeInit(PORT_FLASH_BT, PIN_FLASH_BT);
////	  __HAL_RCC_GPIOB_FORCE_RESET();
//
//	 int now = getTick();
//	while(getTick()< now+300){
//				;}
//}
int  _pwmTick1 = 1;
void sgdLoop(){
	sgdGetInput();
	psfbcheck();
	sgdChargeCtrl();
	if(timeGone( _pwmTick1 ) > 9){
		_pwmTick1 = getTick();
		sgdFilterBat( sgdGetADCBat( ));	
		set15Pluse(calPluse(getFilterBat_mV(),sgdGetADCHV( )));
		//SEGGER_RTT_printf(0, "Average HV = %03d, Average Current = %04d  	vbat = %04d  tick = %d   pluse = %d  \r\n",sgdGetADCHV( ), 		sgdGetADCBat() ,		getFilterBat_mV( ) ,		getTick(),calPluse(getFilterBat_mV(),sgdGetADCHV( )));
					}
	 
	
}

