#include "include.h"
#include "nx1_lib.h"

//EEPROM Memory Address from 0~7FFF(A0~A14 define)

#if (_GPIO_I2C_MODULE && _EEPROM_24LC256_MODULE)

U8 User_24LC256_Write(U8 I2C_Addr, U16 EEPROM_Addr, U8 *Buf);
U8 User_24LC256_Read(U8 I2C_Addr, U16 EEPROM_Addr, U8 *Buf);
U8 User_24LC256_Write_nBytes(U8 I2C_Addr, U16 EEPROM_Addr, U8 Len, U8 *Buf);
U8 User_24LC256_Read_nBytes(U8 I2C_Addr, U16 EEPROM_Addr, U8 Len, U8 *Buf);

//**************************************************************************************************
//I2C Write a Byte Data
//I2C_Addr:		24LC256 Device Address(7 bits left shift 1)
//EEPROM_Addr:	Memory address from 0~7FFF
//Buf:			write data
//return:		1,nonACK Error
//       		0,good work
//**************************************************************************************************
U8 User_24LC256_Write(U8 I2C_Addr, U16 EEPROM_Addr, U8 *Buf)
{
	GPIO_I2C_Start();//I2C start
	GPIO_I2C_Write((I2C_Addr<<1)|GPIO_I2C_WRITE_MODE);//write device address with write bit
	if(GPIO_I2C_Check_ACK()==1)
	{
		return 1;//nonACK
	}
	
	GPIO_I2C_Write((U8)(EEPROM_Addr>>8));//write memory address HI Byte
	if(GPIO_I2C_Check_ACK()==1)
	{
		return 1;//nonACK
	}
	
	GPIO_I2C_Write((U8)(EEPROM_Addr&0xFF));//write memory address LO Byte
	if(GPIO_I2C_Check_ACK()==1)
	{
		return 1;//nonACK
	}
	
	GPIO_I2C_Write(*Buf);//write Data
	if(GPIO_I2C_Check_ACK()==1)
	{
		return 1;//nonACK
	}
	
	GPIO_I2C_Stop();
	return 0;
}

//**************************************************************************************************
//I2C Read a Byte Data
//I2C_Addr:		24LC256 Device Address(7 bits left shift 1)
//EEPROM_Addr:	Memory address from 0~7FFF
//EEPROM_24LC256_Data:Data buffer
//Buf:			Data buffer
//return:		1,nonACK Error
//       		0,good work
//**************************************************************************************************
U8 User_24LC256_Read(U8 I2C_Addr, U16 EEPROM_Addr, U8 *Buf)
{
	GPIO_I2C_Start();
	GPIO_I2C_Write((I2C_Addr<<1)|GPIO_I2C_WRITE_MODE);//write device address with write bit
	if(GPIO_I2C_Check_ACK()==1)
	{
		return 1;//nonACK
	}
	
	GPIO_I2C_Write((U8)(EEPROM_Addr>>8));//write memory address HI Byte
	if(GPIO_I2C_Check_ACK()==1)
	{
		return 1;//nonACK
	}
	
	GPIO_I2C_Write((U8)(EEPROM_Addr&0xFF));//write memory address LO Byte
	if(GPIO_I2C_Check_ACK()==1)
	{
		return 1;//nonACK
	}
	
	GPIO_I2C_RepeatStart();
	
	GPIO_I2C_Write((I2C_Addr<<1)|GPIO_I2C_READ_MODE);//write device address with read bit
	if(GPIO_I2C_Check_ACK()==1)
	{
		return 1;//nonACK
	}
	*Buf = GPIO_I2C_Read();//read a byte data
	GPIO_I2C_Send_NoACK();//receive end, give nonACK bit
	GPIO_I2C_Stop();
	return 0;
}

//**************************************************************************************************
//24LC256 Read continue data
//I2C_addr:  24LC256 device address
//EEPROM_Addr:Memory address
//len:Read Data Length
//buff:Data buffer
//return:1,nonACK error
//       0,good Work
//**************************************************************************************************
U8 User_24LC256_Read_nBytes(U8 I2C_Addr, U16 EEPROM_Addr, U8 Len, U8 *Buf)
{
	GPIO_I2C_Start();
	
	GPIO_I2C_Write((I2C_Addr<<1)|GPIO_I2C_WRITE_MODE);//write device address with write bit
	if(GPIO_I2C_Check_ACK()==1)
	{
		return 1;//nonACK
	}
	
	GPIO_I2C_Write((U8)(EEPROM_Addr>>8));//write memory address HI Byte
	if(GPIO_I2C_Check_ACK()==1)
	{
		return 1;//nonACK
	}
	
	GPIO_I2C_Write((U8)(EEPROM_Addr&0xFF));//write memory address LO Byte
	if(GPIO_I2C_Check_ACK()==1)
	{
		return 1;//nonACK
	}

	GPIO_I2C_RepeatStart();
	
	GPIO_I2C_Write((I2C_Addr<<1)|GPIO_I2C_READ_MODE);//write device address with read bit
	if(GPIO_I2C_Check_ACK()==1)
	{
		return 1;//nonACK
	}
	while(Len)
	{
		*Buf = GPIO_I2C_Read();//read a byte data
		if(Len==1)
		{
			GPIO_I2C_Send_NoACK();		//send NoACK
		}
		else
		{
			GPIO_I2C_Send_ACK();		//send ACK
		}
		Len--;
		Buf++;
	}
	GPIO_I2C_Stop();
	return 0;
}

//**************************************************************************************************
//24LC256 Write continue data
//I2C_addr:  24LC256 device address
//EEPROM_Addr:Memory address
//len:Write Data Length
//buff:Data buffer
//return:1,nonACK error
//       0,good Work
//**************************************************************************************************
U8 User_24LC256_Write_nBytes(U8 I2C_Addr, U16 EEPROM_Addr, U8 Len, U8 *Buf)
{
	GPIO_I2C_Start();//I2C start
	GPIO_I2C_Write((I2C_Addr<<1)|GPIO_I2C_WRITE_MODE);//write device address with write bit
	if(GPIO_I2C_Check_ACK()==1)
	{
		return 1;//nonACK
	}
	
	GPIO_I2C_Write((U8)(EEPROM_Addr>>8));//write memory address HI Byte
	if(GPIO_I2C_Check_ACK()==1)
	{
		return 1;//nonACK
	}
	
	GPIO_I2C_Write((U8)(EEPROM_Addr&0xFF));//write memory address LO Byte
	if(GPIO_I2C_Check_ACK()==1)
	{
		return 1;//nonACK
	}
	
	while(Len)
	{
		GPIO_I2C_Write(*Buf);
		if(GPIO_I2C_Check_ACK()==1)
		{
			return 1;//nonACK
		}
		Len--;
		Buf++;
	}
	
	GPIO_I2C_Stop();
	return 0;
}

#endif//(_GPIO_I2C_MODULE && _EEPROM_24LC256_MODULE)