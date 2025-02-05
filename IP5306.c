#include "MyI2C.h"
#include "KTH57XXIIC.h"
#include "sgd.h"
#include "py32f072xx_Start_Kit.h"

#include "SEGGER_RTT.h"

#include "sgdFlash.h"


uint8_t I2C_ADDR_IP5306 = 0xEA;
int ip5306ReadRes = 0;



uint8_t ip5306Test(void)
{
//	readIp5306Register(0);
			modifySetIp5306Register(0,0x02);
	
//	MyI2C_Start();
//	MyI2C_SendByte(I2C_ADDR_IP5306);	          //发送器件地址+写命令
//	if(IIC_wait_Ack())				          //等待应答
//	{
//			MyI2C_Stop();	
//			return 1;		
//	}	
//	MyI2C_SendByte(0X05);			//写命令CMD_RESET
//	IIC_wait_Ack();					     //等待应答
//	MyI2C_SendByte(0X5A);			//写命令CMD_RESET
//	IIC_wait_Ack();					     //等待应答
//	MyI2C_Stop();						 //产生一个停止条件
//			return 0;		
}

uint8_t writeIp5306Cmd(uint8_t address)
{
	
	MyI2C_Start();
	MyI2C_SendByte(I2C_ADDR_IP5306);	          //???????????+д????
	if(IIC_wait_Ack())				          //??????
	{
			MyI2C_Stop();	
			return 1;		
	}	
	
	MyI2C_SendByte(address);
	IIC_wait_Ack();						 //?????????????
	return 0;

}
uint8_t writeIp5306Register(uint8_t address, uint16_t data)
{
	
	
	MyI2C_Start();
	MyI2C_SendByte(I2C_ADDR_IP5306);	          //???????????+д????
	if(IIC_wait_Ack())				          //??????
	{
			MyI2C_Stop();	
			return 1;		
	}	
	
	MyI2C_SendByte(address);
	IIC_wait_Ack();						 //?????????????
	
	MyI2C_SendByte(data);
	IIC_wait_Ack();						 //?????????????
	return 0;

}


int modifySetIp5306Register(uint8_t address , uint8_t data)
{
if (readIp5306Register(address) == 0)
	{
//	ip5306ReadRes = 0x35;
//	SEGGER_RTT_printf(0, "modifySetIp5306Register: %x  \r\n", ip5306ReadRes );
		if((ip5306ReadRes & data) != data)
		{	
			ip5306ReadRes = ip5306ReadRes | data;
//	SEGGER_RTT_printf(0, "modifySetIp5306Register2: %x  \r\n", ip5306ReadRes );	
			writeIp5306Register(  address,   ip5306ReadRes);
		}
			
	}
}



int readIp5306Register(uint8_t address)

{
	writeIp5306Cmd( address);
		
	MyI2C_Start();
	MyI2C_SendByte(I2C_ADDR_IP5306 | 0x01);	          //???????????+??????
	if(IIC_wait_Ack())				          //??????
	{
			MyI2C_Stop();
 	SEGGER_RTT_printf(0, "readIp5306Register:error \r\n");	
			ip5306ReadRes = -1;
			return -1;
	}	
	
	ip5306ReadRes = (uint8_t)MyI2C_ReceiveByte();
	
	MyI2C_SendAck(1);
 	MyI2C_Stop();	
//      	SEGGER_RTT_printf(0, "readIp5306Register: %x  addr %x\r\n", ip5306ReadRes ,address);	
							 //?????????????
	return 0;

}


int setIp5306Register(uint8_t address,uint8_t value)
{
		SEGGER_RTT_printf(0, "setIp5306Register add=%x  value = %x \r\n",address,value);

//	if(0==readIp5306Register(address))
//		{
//		SEGGER_RTT_printf(0, "return 0\r\n");
//		return 0;
//
// 		}
// 	 
//	writeIp5306Cmd( address);
		
	MyI2C_Start();
	MyI2C_SendByte(I2C_ADDR_IP5306  );	          //???????????+??????
	if(IIC_wait_Ack())				          //??????
	{
			MyI2C_Stop();
			ip5306ReadRes = -1;
		SEGGER_RTT_printf(0, "return -1\r\n");
			return -1;
	}	
	
	MyI2C_SendByte(0);	          //???????????+??????
	if(IIC_wait_Ack())				          //??????
	{
			MyI2C_Stop();
			ip5306ReadRes = -1;
		SEGGER_RTT_printf(0, "return -1\r\n");
			return -1;
	}	
	
	MyI2C_SendByte(value);	          //???????????+??????
	if(IIC_wait_Ack())				          //??????
	{
			MyI2C_Stop();
			ip5306ReadRes = -1;
		SEGGER_RTT_printf(0, "return -1\r\n");
			return -1;
	}	
	
//	ip5306ReadRes = (uint8_t)MyI2C_ReceiveByte();
		
	MyI2C_Stop();						 //?????????????
	return 0;

}

int sgdGetIp5306State(){
	int state  = 0 ;
	readIp5306Register(0);
    // 	SEGGER_RTT_printf(0, "ip5306ReadRes:= %x  sgdGetUsbConnected() = %x eee =%d\r\n", ip5306ReadRes ,sgdGetUsbConnected(),ip5306ReadRes & 0x02 );
 	//		SEGGER_RTT_printf(0, "sgdGetUsbConnected()  false ip5306ReadRes & 0x02 = %d\r\n",(ip5306ReadRes & 0x02));
//			if((ip5306ReadRes & 0x02) == 0)
//			{
//				SEGGER_RTT_printf(0, "ip5306ReadRes & 0x02 == 0 \r\n");
//				ip5306ReadRes = ip5306ReadRes|0x37;
//				setIp5306Register(0,ip5306ReadRes);
//			}
		
			if(((ip5306ReadRes & 0x10) == 0 ) || ((ip5306ReadRes & 0xe47) != 0 ))
				{
					
					ip5306ReadRes = ip5306ReadRes & 0xB8;
					ip5306ReadRes = ip5306ReadRes|0x10;
					setIp5306Register(0,ip5306ReadRes);
				}
//	if ( sgdGetUsbConnected() )
// 		{
// 			if((ip5306ReadRes & 0x10) != 0)
// 				{
//					ip5306ReadRes = ip5306ReadRes & 0xef;
//    				setIp5306Register(0,ip5306ReadRes);
// 				}
//		}
//	else{
//
//		}
	readIp5306Register(0x70);
	if((ip5306ReadRes & 0x08)  !=  0){
		state  = 1;
		}
	readIp5306Register(0x71);
	if((ip5306ReadRes & 0x08)  !=  0){
		state  += 2;
		}
	return state;
}

void sgdEnableCharge(int state)
{
	
	readIp5306Register(0);
	ip5306ReadRes = ip5306ReadRes|0x37;
	if(state==0){
		ip5306ReadRes = ip5306ReadRes & 0xdf;
		}
    setIp5306Register(0,ip5306ReadRes);
	
}


