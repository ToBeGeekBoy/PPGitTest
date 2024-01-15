//****************************************************************
//****************************************************************
#include "include.h"
#include "nx1_lib.h"
#include "nx1_gpio.h"
//****************************************************************
//****************************************************************

#if _GPIO_I2C_MODULE

#define C_ShiftCnt		0x07

#if		OPTION_HIGH_FREQ_CLK==32000000
#define GPIO_I2C_DELAY_COUNT 10
#elif	OPTION_HIGH_FREQ_CLK==24000000
#define GPIO_I2C_DELAY_COUNT 8
#elif	OPTION_HIGH_FREQ_CLK==16000000
#define GPIO_I2C_DELAY_COUNT 4
#elif	OPTION_HIGH_FREQ_CLK==12000000
#define GPIO_I2C_DELAY_COUNT 3
#else
#define GPIO_I2C_DELAY_COUNT 10
#endif

#define GPIO_I2C_SCL_OUT_LO		(_GPIO_I2C_IO->DIR&=(~(1<<_GPIO_I2C_SCL)))
#define GPIO_I2C_SCL_OUT_HI		(_GPIO_I2C_IO->DIR|=(1<<_GPIO_I2C_SCL))
#define GPIO_I2C_SDA_OUT_LO		(_GPIO_I2C_IO->DIR&=(~(1<<_GPIO_I2C_SDA)))
#define GPIO_I2C_SDA_OUT_HI		(_GPIO_I2C_IO->DIR|=(1<<_GPIO_I2C_SDA))
#define GPIO_I2C_SCL_IN			((_GPIO_I2C_IO->PAD>>_GPIO_I2C_SCL)&0x01)
#define GPIO_I2C_SDA_IN			((_GPIO_I2C_IO->PAD>>_GPIO_I2C_SDA)&0x01)


void GPIO_I2C_Init();
void GPIO_I2C_Start();
void GPIO_I2C_RepeatStart();
void GPIO_I2C_Stop();
void GPIO_I2C_Send_NoACK();
void GPIO_I2C_Send_ACK();
void GPIO_I2C_Write(U8 U8_write_data);
U8 GPIO_I2C_Read();
U8 GPIO_I2C_Check_ACK();

//------------------------------------------------------------------//
// GPIO_I2C Delay
// Parameter: 
//			U32_GPIO_I2C_User_Delay:for loop cycle.
// return value:
//          None
//------------------------------------------------------------------//
void GPIO_I2C_User_Delay(U32 U32_GPIO_I2C_User_Delay)
{
	volatile U32 i;
	for(i=0; i<U32_GPIO_I2C_User_Delay; i++);
}


//------------------------------------------------------------------//
// GPIO_I2C Initial
// Parameter: 
//			None
// return value:
//          None
//------------------------------------------------------------------//
void GPIO_I2C_Init()
{
	GPIO_Init(_GPIO_I2C_IO, _GPIO_I2C_SCL, GPIO_MODE_IN_FLOAT);
	GPIO_Init(_GPIO_I2C_IO, _GPIO_I2C_SDA, GPIO_MODE_IN_FLOAT);

	_GPIO_I2C_IO->Data &= ~(1<<_GPIO_I2C_SDA);
	_GPIO_I2C_IO->Data &= ~(1<<_GPIO_I2C_SCL);
	GPIO_I2C_User_Delay(GPIO_I2C_DELAY_COUNT);
}



//------------------------------------------------------------------//
// GPIO_I2C_Start
// Parameter: 
//			None
// return value:
//          None
//			_______
// SCL ____/	   \_____
//     ________
// SDA		   \_________
//------------------------------------------------------------------//
void GPIO_I2C_Start()
{
	GPIO_I2C_SCL_OUT_HI;
	GPIO_I2C_SDA_OUT_HI;
	GPIO_I2C_User_Delay(GPIO_I2C_DELAY_COUNT);
	GPIO_I2C_SDA_OUT_LO;
	GPIO_I2C_User_Delay(GPIO_I2C_DELAY_COUNT);
	GPIO_I2C_SCL_OUT_LO;
	GPIO_I2C_User_Delay(GPIO_I2C_DELAY_COUNT);
}



//------------------------------------------------------------------//
// GPIO_I2C_RepetStart
// Parameter: 
//			None
// return value:
//          None
//			 _______
// SCL _____/	    \_____
//       ________
// SDA _/        \_________
//------------------------------------------------------------------//
void GPIO_I2C_RepeatStart()
{
	GPIO_I2C_SDA_OUT_HI;
	GPIO_I2C_User_Delay(GPIO_I2C_DELAY_COUNT);
	GPIO_I2C_SCL_OUT_HI;
	GPIO_I2C_User_Delay(GPIO_I2C_DELAY_COUNT);
	GPIO_I2C_SDA_OUT_LO;
	GPIO_I2C_User_Delay(GPIO_I2C_DELAY_COUNT);
	GPIO_I2C_SCL_OUT_LO;
	GPIO_I2C_User_Delay(GPIO_I2C_DELAY_COUNT);
}


//------------------------------------------------------------------//
// GPIO_I2C_Stop
// Parameter: 
//			None
// return value:
//          None
//			_______
// SCL ____/	   \_____
//  		 	_________
// SDA ________/
//------------------------------------------------------------------//
void GPIO_I2C_Stop()
{
	GPIO_I2C_SDA_OUT_LO;
	GPIO_I2C_SCL_OUT_LO;
	GPIO_I2C_User_Delay(GPIO_I2C_DELAY_COUNT);
	GPIO_I2C_SCL_OUT_HI;
	GPIO_I2C_User_Delay(GPIO_I2C_DELAY_COUNT);
	GPIO_I2C_SDA_OUT_HI;		
}


//------------------------------------------------------------------//
// GPIO_I2C_Send_NoACK
// Parameter: 
//			None
// return value:
//          None
//------------------------------------------------------------------//
void GPIO_I2C_Send_NoACK()
{
	GPIO_I2C_SDA_OUT_HI;
	GPIO_I2C_User_Delay(GPIO_I2C_DELAY_COUNT);
	GPIO_I2C_SCL_OUT_HI;
	GPIO_I2C_User_Delay(GPIO_I2C_DELAY_COUNT);
	GPIO_I2C_SCL_OUT_LO;	
	GPIO_I2C_User_Delay(GPIO_I2C_DELAY_COUNT);	
}


//------------------------------------------------------------------//
// GPIO_I2C_Send_ACK
// Parameter: 
//			None
// return value:
//          None
//------------------------------------------------------------------//
void GPIO_I2C_Send_ACK()
{
	GPIO_I2C_SDA_OUT_LO;
	GPIO_I2C_User_Delay(GPIO_I2C_DELAY_COUNT);
	GPIO_I2C_SCL_OUT_HI;
	GPIO_I2C_User_Delay(GPIO_I2C_DELAY_COUNT);
	GPIO_I2C_SCL_OUT_LO;
	GPIO_I2C_User_Delay(GPIO_I2C_DELAY_COUNT);
}


//------------------------------------------------------------------//
// GPIO_I2C_Write
// Parameter: 
//			U8_write_data:data(8bits)
// return value:
//          None
//------------------------------------------------------------------//
void GPIO_I2C_Write(U8 U8_write_data)
{
	U8 U8_User_ShiftCnt = C_ShiftCnt;//8bit Offset
	GPIO_I2C_SCL_OUT_LO;
	while(U8_User_ShiftCnt!=0xFF)
	{
		if(U8_write_data&(1<<U8_User_ShiftCnt))
		{
			GPIO_I2C_SDA_OUT_HI;
		}
		else
		{
			GPIO_I2C_SDA_OUT_LO;
		}
		GPIO_I2C_User_Delay(GPIO_I2C_DELAY_COUNT);
		GPIO_I2C_SCL_OUT_HI;
		GPIO_I2C_User_Delay(GPIO_I2C_DELAY_COUNT);
		GPIO_I2C_SCL_OUT_LO;
		GPIO_I2C_User_Delay(GPIO_I2C_DELAY_COUNT);

		U8_User_ShiftCnt--;		
	}
}


//------------------------------------------------------------------//
// GPIO_I2C_Read
// Parameter: 
//			None
// return value:
//          Data from slave(8 bits)
//------------------------------------------------------------------//
U8 GPIO_I2C_Read()
{
	U8 U8_User_ReadBuf = 0;
	U8 U8_User_ShiftCnt = C_ShiftCnt;//8bit Offset
	GPIO_I2C_SDA_OUT_HI;
	GPIO_I2C_User_Delay(GPIO_I2C_DELAY_COUNT);
	
	while(U8_User_ShiftCnt!=0xFF)
	{
		GPIO_I2C_SCL_OUT_HI;
		GPIO_I2C_User_Delay(GPIO_I2C_DELAY_COUNT);
		U8_User_ReadBuf<<=1;		

		if(GPIO_I2C_SDA_IN)
		{
			U8_User_ReadBuf |= 0x01;
		}
		GPIO_I2C_SCL_OUT_LO;
		GPIO_I2C_User_Delay(GPIO_I2C_DELAY_COUNT<<1);
		
		U8_User_ShiftCnt--;
	}
	return U8_User_ReadBuf;	
}


//------------------------------------------------------------------//
// GPIO_I2C_Check_ACK
// Parameter: 
//			None
// return value:
//          0:ACK(from slave)
//			1:nonACK(from slave)
//------------------------------------------------------------------//
U8 GPIO_I2C_Check_ACK()
{
	U8 ACK_Flag;
	
	GPIO_I2C_SDA_OUT_HI;
	GPIO_I2C_User_Delay(GPIO_I2C_DELAY_COUNT);
	GPIO_I2C_SCL_OUT_HI;
	GPIO_I2C_User_Delay(GPIO_I2C_DELAY_COUNT);
	
	if(GPIO_I2C_SDA_IN)
	{
		ACK_Flag = 1;	//1: error
	}
	else
	{
		ACK_Flag = 0;	//0: ok
	}
	GPIO_I2C_SCL_OUT_LO;
	GPIO_I2C_User_Delay(GPIO_I2C_DELAY_COUNT);
	return ACK_Flag;
}
//==================================================================================================
#endif//_GPIO_I2C_MODULE