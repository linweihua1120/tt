#include "sgd.h"
#include "py32f072xx_Start_Kit.h"  

//#define PORT_SDA						GPIOA
//#define PIN_SDA							LL_GPIO_PIN_12
//#define PORT_SCLK						GPIOA
//#define PIN_SCLK						LL_GPIO_PIN_11
//#define PORT_SDA						GPIOA 
//#define PIN_SDA							LL_GPIO_PIN_7
//#define PORT_SCLK						GPIOB
//#define PIN_SCLK						LL_GPIO_PIN_0

//#define SET_SDA							LL_GPIO_SetOutputPin(PORT_SDA, PIN_SDA)
//#define SET_SCLK						LL_GPIO_SetOutputPin(PORT_CLK, PIN_CLK)
//#define RESET_SDA						LL_GPIO_ResetOutputPin(PORT_SDA, PIN_SDA)
//#define RESET_SCLK					LL_GPIO_ResetOutputPin(PORT_CLK, PIN_CLK)
//#define IS_HIGH_SDA					LL_GPIO_IsInputPinSet(PORT_SDA, PIN_SDA )
//#define INIT_OUTPUT_SCLK		sgdIOConfig(PORT_CLK, PIN_CLK,LL_GPIO_MODE_OUTPUT,LL_GPIO_PULL_NO,LL_GPIO_OUTPUT_OPENDRAIN)
//#define INIT_OUTPUT_SDA			sgdIOConfig(PORT_SDA, PIN_SDA,LL_GPIO_MODE_OUTPUT,LL_GPIO_PULL_NO,LL_GPIO_OUTPUT_OPENDRAIN)
//#define INIT_INPUT_SDA			sgdIOConfig(PORT_SDA, PIN_SDA,LL_GPIO_MODE_INPUT,LL_GPIO_PULL_UP,LL_GPIO_OUTPUT_PUSHPULL)

//
//#define SET_SDA						1	 
//#define SET_SCLK					0	 
//#define RESET_SDA					1	 
//#define RESET_SCLK				0	 
//#define IS_HIGH_SDA					1 
//#define INIT_OUTPUT_SCLK		 
//#define INIT_OUTPUT_SDA			 
#define INIT_INPUT_SDA			 


uint8_t MyI2C_ReceiveAck(void);
void Delay_us(int delayUS){
	int count = delayUS *6;
	while (count-->0)
		{
		;
		}
	
}
void MyCLK(){
	SET_SCK;
	Delay_ms(10);
	RESET_SCK;
	Delay_ms(10);

}
void MyI2C_W_SCL(uint8_t BitValue)
{
	(BitValue == 0)? RESET_SCK:SET_SCK;
	//GPIO_WriteBit(GPIOB, GPIO_Pin_6, (BitAction)BitValue);
	 Delay_us(10);
}

void MyI2C_W_SDA(uint8_t BitValue)
{
	(BitValue == 0)? RESET_CSDA:SET_CSDA;
	 Delay_us(10);
//	GPIO_WriteBit(GPIOB, GPIO_Pin_7, (BitAction)BitValue);
	 //Delay_us(10);
}

uint8_t MyI2C_R_SDA(void)
 {
	 uint8_t BitValue;
	 BitValue = IS_HIGH_CSDA;// GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7);
	 //Delay_us(10);
	 return BitValue;
	 
 }

void MyI2C_Init(void)
{
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//	
//    GPIO_InitTypeDef GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);
//	
//	GPIO_SetBits(GPIOB, GPIO_Pin_6 | GPIO_Pin_7);
//	Delay_ms(10);

 INIT_OUTPUT_CLK	;
 INIT_OUTPUT_CSDA		;
	
}

void MyI2C_in_Init(void)
{
 INIT_OUTPUT_CLK	;
 INIT_INPUT_SDA		;
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//	
//    GPIO_InitTypeDef GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);

	
//	GPIO_SetBits(GPIOB, GPIO_Pin_6);

	
}

void MyI2C_Start(void)
{
 	//SEGGER_RTT_printf(0, " \r\nMyI2C_Start TICK = %d \r\n"  ,getTick() );
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	Delay_us(4);
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(0);
	Delay_us(40);

}

void MyI2C_Stop(void)
{
	
 //	SEGGER_RTT_printf(0, "MyI2C_Stop TICK = %d \r\n"  ,getTick() );
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(0);
	Delay_us(4);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(1);
	Delay_us(40);
}

void MyI2C_SendByte(uint8_t Byte)
{
 	uint8_t i;
 	//SEGGER_RTT_printf(0, "MyI2C_SendByte:   %X  \r\n",  Byte  );
	MyI2C_W_SCL(0);
	for(i = 0;i < 8; i++)
	{
		MyI2C_W_SDA(Byte & (0x80 >> i));
		MyI2C_W_SCL(1);
	//Delay_us(2);
		MyI2C_W_SCL(0);
	//Delay_us(10);
	}	

}

uint8_t MyI2C_ReceiveByte(void)
{
	uint8_t i ,Byte = 0x00;
	MyI2C_W_SDA(1);
	
	for(i = 0;i < 8; i++)
	{	
		MyI2C_W_SCL(1);
		if (MyI2C_R_SDA() ==1)
		{
			Byte |= (0x80 >> i);
		}
				//	SEGGER_RTT_printf(0, "readIp5306Register: %d  %d  \r\n", i, Byte  );	
		    MyI2C_W_SCL(0);
	        //Delay_us(2);
	
	}	
	//MyI2C_ReceiveAck();
	return Byte;

}

void MyI2C_SendAck(uint8_t AckBit)
{
		MyI2C_W_SDA(AckBit);
		MyI2C_W_SCL(1);
		Delay_us(40);
		MyI2C_W_SCL(0);
}

uint8_t MyI2C_ReceiveAck(void)
{
	uint8_t AckBit;
	MyI2C_W_SDA(1);
//	MyI2C_W_SCL(1);
	AckBit = MyI2C_R_SDA();
//	MyI2C_W_SCL(0);
	
	return AckBit;

}

uint8_t IIC_wait_Ack(void)
{
    uint16_t ucErrTime=0;
	
	MyI2C_W_SCL(1);
	//Delay_us(4);
//   usleep(1);
	
    while(MyI2C_ReceiveAck())
    {
        ucErrTime++;
        if(ucErrTime > 5)
        {
          //  MyI2C_Stop();
	// SEGGER_RTT_printf(0, "IIC_wait_Ack  error\r\n");
            return 1;
        }
    }
	MyI2C_W_SCL(0);
//	 SEGGER_RTT_printf(0, "IIC_wait_Ack  \r\n");
    return 0;
}
