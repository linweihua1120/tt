#include "MyI2C.h"
#include "KTH57XXIIC.h"
#include "sgd.h"
#include "py32f072xx_Start_Kit.h"

#include "SEGGER_RTT.h"

#include "sgdFlash.h"


uint8_t RMData[9];
uint8_t RRData[3] = {0};

uint8_t I2C_ADDR = 0xF0;
uint32_t zeroAngle = 0 ;

/*
void KTH57XXIICADDRInit(uint8_t A1,uint8_t A0)
{
	IIC_ADDR = I2C_BASE_ADDR | (A1 ? 2:0) | (A0 ? 1:0);
	IIC_ADDR = IIC_ADDR << 1;
	

}
*/

uint8_t ExitMode(void)
{
	uint8_t dataStore;
	
	MyI2C_Start(); 
	MyI2C_SendByte(I2C_ADDR);	          //����������ַ+д����
	IIC_wait_Ack();					            //�ȴ�Ӧ��
	
/*	*/	if(IIC_wait_Ack())				          //�ȴ�Ӧ��
	{
			MyI2C_Stop();	
			return 1;		
	}	
	

	MyI2C_SendByte(CMD_EXIT_MODE);			//д����CMD_EXIT_MODE
	IIC_wait_Ack();					            //�ȴ�Ӧ��
	MyI2C_Start();
	MyI2C_SendByte(I2C_ADDR | 1);	        //����������ַ+������
	IIC_wait_Ack();					            //�ȴ�Ӧ��
	dataStore=MyI2C_ReceiveByte();	        //����state
	MyI2C_SendAck(1);
	MyI2C_Stop();						              //����һ��ֹͣ����
	return dataStore;
	
}


uint8_t reset(void)
{
	MyI2C_Start();
	MyI2C_SendByte(I2C_ADDR);	          //����������ַ+д����
	if(IIC_wait_Ack())				          //�ȴ�Ӧ��
	{
			MyI2C_Stop();	
			return 1;		
	}	
	MyI2C_SendByte(CMD_RESET);			//д����CMD_RESET
	IIC_wait_Ack();					     //�ȴ�Ӧ��
	MyI2C_Start();
	MyI2C_Stop();						 //����һ��ֹͣ����
			return 0;		
}

uint8_t singleMeasurementMode(uint8_t axis)
{
	uint8_t dataStore;
	MyI2C_Start();
	MyI2C_SendByte(I2C_ADDR);	          //����������ַ+д����
	if(IIC_wait_Ack())				          //�ȴ�Ӧ��
	{
			MyI2C_Stop();	
			return 1;		
	}	
	MyI2C_SendByte(CMD_SINGLE_MEASUREMENT | axis);			//д����CMD_SINGLE_MEASUREMENT
	IIC_wait_Ack();					     //�ȴ�Ӧ��
	MyI2C_Start();
	MyI2C_SendByte(I2C_ADDR | 1);	          //����������ַ+������
	IIC_wait_Ack();					     //�ȴ�Ӧ��
	dataStore=MyI2C_ReceiveByte();	        //����state
	MyI2C_SendAck(1);
	MyI2C_Stop();						 //����һ��ֹͣ����
	
	return dataStore;
	
	
}

uint8_t DutyCycleMode(uint8_t axis)
{
	uint8_t dataStore;
	MyI2C_Start();
	MyI2C_SendByte(I2C_ADDR);	          //����������ַ+д����
	if(IIC_wait_Ack())				          //�ȴ�Ӧ��
	{
			MyI2C_Stop();	
			return 1;		
	}	
	MyI2C_SendByte(CMD_DutyCycleMode | axis);			//д����CMD_DutyCycleMode
	IIC_wait_Ack();					     //�ȴ�Ӧ��
	MyI2C_Start();
	MyI2C_SendByte(I2C_ADDR | 1);	          //����������ַ+������
	IIC_wait_Ack();					     //�ȴ�Ӧ��
	dataStore=MyI2C_ReceiveByte();	        //����state
	MyI2C_SendAck(1);
	MyI2C_Stop();						 //����һ��ֹͣ����
	
	return dataStore;


}

uint8_t writeRegister(uint8_t address, uint16_t data)
{
	uint8_t i ,cmd_buf[4];
	uint8_t dataStore;
	cmd_buf[0] = CMD_WRITE_REGISTER;
	cmd_buf[1] = data >> 8;
	cmd_buf[2] = data &0xFF;
	cmd_buf[3] = address << 2;
	
	
	MyI2C_Start();
	MyI2C_SendByte(I2C_ADDR);	          //����������ַ+д����
	if(IIC_wait_Ack())				          //�ȴ�Ӧ��
	{
			MyI2C_Stop();	
			return 1;		
	}	
	for(i = 0; i < 4; i++)
	{
		MyI2C_SendByte(cmd_buf[i]);
		IIC_wait_Ack();					     //�ȴ�Ӧ��
	
	}
	
	MyI2C_Start();
	MyI2C_SendByte(I2C_ADDR | 0x01);	          //����������ַ+д����
	IIC_wait_Ack();					     //�ȴ�Ӧ��
	dataStore=MyI2C_ReceiveByte();	        //����state
	MyI2C_SendAck(1);
	MyI2C_Stop();						 //����һ��ֹͣ����
	return dataStore;

}


uint8_t readRegister(uint8_t address)
{
	uint8_t i ,cmd_buf[2];
	
	cmd_buf[0] = CMD_READ_REGISTER;
	cmd_buf[1] = address << 2;
	
	
	MyI2C_Start();
	MyI2C_SendByte(I2C_ADDR);	          //����������ַ+д����
	IIC_wait_Ack();					     //�ȴ�Ӧ��

/*	
	if(IIC_wait_Ack())				          //�ȴ�Ӧ��
	{
			MyI2C_Stop();	
			return 1;		
	}	
*/	
	for(i = 0; i < 2; i++)
	{
		MyI2C_SendByte(cmd_buf[i]);
		IIC_wait_Ack();					     //�ȴ�Ӧ��
	
	}
	MyI2C_Start();
	MyI2C_SendByte(I2C_ADDR | 0x01);	          //����������ַ+������
	IIC_wait_Ack();					     //�ȴ�Ӧ��

		RRData[0] = MyI2C_ReceiveByte();
		MyI2C_SendAck(0);
		RRData[1] = MyI2C_ReceiveByte();
		MyI2C_SendAck(0);
		RRData[2] = MyI2C_ReceiveByte();
		MyI2C_SendAck(1);
	
/*	for(i = 0; i < 3; i++)
	{
		if (i == 2)
		{
			RRData[i] = MyI2C_ReceiveByte();
			MyI2C_SendAck(1);
		}

		else
		{
			RRData[i] = MyI2C_ReceiveByte();
			MyI2C_SendAck(0);
	
		}
	
	
	}
*/	
	MyI2C_Stop();						 //����һ��ֹͣ����
	return 0;

}
uint8_t readMeasurement(uint8_t axis)
{
	uint8_t i;
	uint8_t com;
	uint8_t counter = 1;
	
	com = CMD_READ_MEASUREMENT + axis;
	
	for(i = 0; i < 4; i++)
	{
		counter = counter + (axis & 0x01) * 2;
		axis = axis >> 1;
	}
	MyI2C_Start();
	MyI2C_SendByte(I2C_ADDR);	          //����������ַ+д����
	if(IIC_wait_Ack())				          //�ȴ�Ӧ��
	{
			MyI2C_Stop();	
			return 1;		
	}	
	MyI2C_SendByte(com);
	IIC_wait_Ack();					     //�ȴ�Ӧ��
	
	MyI2C_Start();
	MyI2C_SendByte(I2C_ADDR | 0x01);	          //����������ַ+������
	if(IIC_wait_Ack())				          //�ȴ�Ӧ��
	{
			MyI2C_Stop();	
			return 1;		
	}	
	for(i = 0; i < counter; i ++)
	{
		if (i == counter - 1)
		{
			RMData[i] = MyI2C_ReceiveByte();
			MyI2C_SendAck(1);
		}

		else
		{
			RMData[i] = MyI2C_ReceiveByte();
			MyI2C_SendAck(0);
	
		}
	
	
	}
	
	MyI2C_Stop();						 //����һ��ֹͣ����
	return RMData[0];

}
void    RegInitial(void)
{
	writeRegister(0x19,0x0000);
	writeRegister(0x1B,0x0403);
	writeRegister(0x1C,0x0F2F);
	writeRegister(0x1D,0x0302);
	writeRegister(0x1E,0x8000);
	
}


uint16_t A_Data;             //½Ç¶È - ²âÊÔÖµ
uint16_t B_Data;             //B - ²âÊÔÖµ

uint16_t DID;
float AngleOutput;


//从flash中读取初始角度
void sgdZeroAngleInit(){
 	zeroAngle = HW32_REG(ZERO_ANGLE_FLASH_ADDR );
	SEGGER_RTT_printf(0, "\nsgdZeroAngleInit zeroAngle %d	\n",zeroAngle );

	
}
int sgdIsKT5763Online(){
	return (  DID!=0 );
}
void sgdI2C_Init()
{
	MyI2C_Init();
}
void sgdKT5763Init(){
	sgdZeroAngleInit();

	MyI2C_Init();
	
	Delay_ms(10);
	ExitMode();
	Delay_ms(10);
	readRegister(0x0D);
	DID = (RRData[1] << 8) + RRData[2];
	SEGGER_RTT_printf(0, "\n DID %x	\n",DID );
	RegInitial();
    DutyCycleMode(0x06);                             
	Delay_ms(10);
	
	
}
void sgdReadAngle(){
			A_Data = 0;

			readMeasurement(0x06);							  // µÚËÄ²½ £º¶ÁÈ¡½Ç¶ÈA ºÍ´Å³¡BÖµ 
			A_Data = RMData[1];
			AngleOutput = (RMData[1]& 0xFF)  * 360.0/ 256 ;		//Ôò angle = RMData[1] / 256 * 360  £¨KTH5763ÊÇ8Î»¾ø¶Ô½Ç¶ÈÊä³ö£©
			B_Data = (RMData[3] << 8) + RMData[4];			  //¶ÁÈ¡Æ½Ãæ´Å³¡Êä³öÖµ
			if(A_Data == 0xff){
				sgdKT5763Init();

				}
  	 	// SEGGER_RTT_printf(0, "\n A_Data is %d	 B_Data = %d  AngleOutput= %d\n",A_Data,B_Data,(int)AngleOutput);
}


int sgdGetAngle(){

			return (int)AngleOutput;
}

int sgdGetPowerLevel(){
// 		SEGGER_RTT_printf(0, "\n AngleOutput = %d zeroAngle = %d sum = %d  %d  %X  \n",(int)AngleOutput, (int)zeroAngle,(int)(AngleOutput-zeroAngle + 360 +11),(int)(AngleOutput-zeroAngle + 360 +11  )/23 ,zeroAngle );
	return	((int)(AngleOutput + 360 - zeroAngle  + 11) % 360) / 23 ;
}


void tempZeroAngleIncrease(){
	
	zeroAngle++;
  HAL_FLASH_Unlock();
		SEGGER_RTT_printf(0, "\n tempZeroAngleIncrease  zeroAngle %d	\n",HW32_REG(ZERO_ANGLE_FLASH_ADDR ) );
		
	APP_FlashErase(  ZERO_ANGLE_FLASH_ADDR , 1);
	if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_PAGE, ZERO_ANGLE_FLASH_ADDR, &zeroAngle) == HAL_OK)/* Execute programming */
    {
		SEGGER_RTT_printf(0, "\n HAL_FLASH_Program  zeroAngle %d	\n",zeroAngle );
  HAL_FLASH_Lock();
		return ;
    }
		SEGGER_RTT_printf(0, "\n HAL_FLASH_Program  fail  \n" );
  HAL_FLASH_Lock();
}


int sgdGetZeroAngle(){

			return (int)zeroAngle;
}

void sgdResetZeroAngle(){
	 sgdSaveZeroAngle(0xffffffff);
//	APP_FlashErase(ZERO_ANGLE_FLASH_ADDR,1);

}

int sgdSaveZeroAngle(int min){
//	sgdReadAngle();
	zeroAngle = (int)min;
 	HAL_FLASH_Unlock();
	APP_FlashErase(  ZERO_ANGLE_FLASH_ADDR , 1);
	int res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_PAGE, ZERO_ANGLE_FLASH_ADDR, &zeroAngle);
  	HAL_FLASH_Lock();
	return (res == HAL_OK);
	

}



int sgdSetZeroAngle(int min  )
{
	if(sgdIsKT5763Online()){
			return sgdSaveZeroAngle(min);
		}
	return 0 ;
}

























