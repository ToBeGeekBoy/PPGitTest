#include "nx1_spi.h"
#include "nx1_gpio.h"
#include "nx1_wdt.h"
#include "include.h"

/********************************************************************/
// SPI1 Remark on NX1 OTP series
//  1.	CS, CLK, MOSI, MISO mapping to PB8~11.
//  2.	CS pin is determined by user, but PB8 is recommended base on -
//     	the power domain relationship.
//	3. 	Access mode only supports 1-1-1 mode.
//
// SPI1 Remark on NX1 EF series
//  1.	CS, CLK, MOSI, MISO mapping to PA12~15.
//	2. 	Access mode supports 1-1-1 mode, 1-1-1 3 wire mode, 1-2-2 mode and 1-1-2 mode.
/********************************************************************/
#if _SPI1_MODULE 
#if	defined(P_SPI1)
//////////////////////////////////////////////////////////////////////
//  Static Variables
//////////////////////////////////////////////////////////////////////
#if _EF_SERIES
static U8 SpiCmdBusMode, SpiAddrBusMode, SpiDataBusMode;
#endif

//////////////////////////////////////////////////////////////////////
//  Extern Functions
//////////////////////////////////////////////////////////////////////
extern void CB_SPI1_BlockErase_Timeout(void);
extern void CB_SPI1_SectorErase_Timeout(void);


//------------------------------------------------------------------//
// SPI1 Init
// Parameter: 
//          divide	: SPI_CLKDIV_1,SPI_CLKDIV_2...
//			pol 	: SPI_Pol_Low, SPI_Pol_High
//			pha		: SPI_Phase_1st_Edge, SPI_Phase_2nd_Edge
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI1_Init(U32 divide,U8 pol, U8 pha)
{
    SPI1_SetClockDivisor(divide);
    SPI1_SetClockPolarity(pol);
    SPI1_SetClockPhase(pha);
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,1);
}
//------------------------------------------------------------------//
// SPI1 set clock divisor 
// Parameter: 
//          divide	: SPI_CLKDIV_1,SPI_CLKDIV_2...SPI_CLKDIV_128
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI1_SetClockDivisor(U32 divide)
{
    SPI_TypeDef *SPI = P_SPI1;
    SPI->Ctrl = (SPI->Ctrl & ~(C_SPI_CLK_Div)) | divide;
}
//------------------------------------------------------------------//
// SPI1 set clock polarity
// Parameter: 
//			pol 	: SPI_Pol_Low, SPI_Pol_High
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI1_SetClockPolarity(U32 pol)
{
    SPI_TypeDef *SPI = P_SPI1;
    SPI->Ctrl = (SPI->Ctrl & ~(C_SPI_Pol)) | pol;
}
//------------------------------------------------------------------//
// SPI1 set clock phase
// Parameter: 
//			pha		: SPI_Phase_1st_Edge, SPI_Phase_2nd_Edge
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI1_SetClockPhase(U32 pha)
{
    SPI_TypeDef *SPI = P_SPI1;
    SPI->Ctrl = (SPI->Ctrl & ~(C_SPI_Phase)) | pha;
}
#if _EF_SERIES
//------------------------------------------------------------------//
// Set SPI bus mode
// Parameter: 
//          cmd_bus_mode  : SPI1_DATA_1,SPI1_DATA_2
//          addr_bus_mode : SPI1_DATA_1,SPI1_DATA_2
//          data_bus_mode : SPI1_DATA_1,SPI1_DATA_2
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI1_SetBusMode(U8 cmd_bus_mode,U8 addr_bus_mode,U8 data_bus_mode)
{
    SpiCmdBusMode = cmd_bus_mode;
    SpiAddrBusMode= addr_bus_mode;
    SpiDataBusMode= data_bus_mode;
}
#endif
//------------------------------------------------------------------//
// Send data 
//			1. CS pin is controlled by user when _SPI1_MODULE = enable 
//			   and _SPI1_USE_FLASH = disable.
//			2. CS pin is recommended to use PB8. (Power Domain relationship)
// Parameter: 
//          *data : data buffer
//          len   : length to write(bytes)
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI1_SendData(U32 *data,U8 len)
{
    U8 i,j,k;
    SPI_TypeDef *SPI = P_SPI1;
	
    do{
        if(len>SPI1_FIFO_SIZE)
        {
            k=SPI1_FIFO_SIZE;
        }
        else
        {    
            k=len;
        }
        j=(k+3)>>2;
        for(i=0;i<j;i++)
        {
            SPI->Data = *data++;    
        }    
    
        SPI->Ctrl &=~(C_SPI_FIFO_Length);
        SPI->Ctrl |= (C_SPI_Tx_Start | ((k-1)<<SPI_DATA_NUM_OFFSET));        
        do{
        }while(SPI->Ctrl&C_SPI_Tx_Busy); 

        len-=k;
    }while(len);
} 
//------------------------------------------------------------------//
// Receive data
//			1. CS pin is controlled by user when _SPI1_MODULE = enable 
//			   and _SPI1_USE_FLASH = disable.
//			2. CS pin is recommended to use PB8. (Power Domain relationship)
// Parameter: 
//          *data : data buffer
//          len	  : length to read(bytes), multiple of 4
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI1_ReceiveData(void *buf,U16 len)
{
    SPI_TypeDef *SPI = P_SPI1;
    U32 base = (SPI->Ctrl & ~(C_SPI_FIFO_Length)) | C_SPI_Rx_Start;
    U32 temp;
    U32 copy_len=0;
    U32 read_len = (U32)buf;
    if((read_len&0x3)==0) {
        read_len = (len>SPI1_FIFO_SIZE)?SPI1_FIFO_SIZE:len;
    } else { //have lead fragment
        read_len = (-read_len)&0x3; //4's complement
        read_len = (len<sizeof(U32))?len:read_len;//1~3
    }
    U32 ctrl = base | ((read_len-1)<<SPI_DATA_NUM_OFFSET);
    while(len) {
        SPI->Ctrl = ctrl; //(T)Trig
        
        U8 *pTemp = (U8*)&temp;
        U8 **p8buf = (U8**)&buf;
        while(copy_len--) {//(C)Copy for lead fragment
            *(*p8buf) = *pTemp;
            (*p8buf)++;
            pTemp++;
        }
        
        copy_len = read_len;
        len -= read_len;
        read_len = (len>SPI1_FIFO_SIZE)?SPI1_FIFO_SIZE:len;
        ctrl = base | ((read_len-1)<<SPI_DATA_NUM_OFFSET);
        //(void)SPI->Ctrl;
        while(SPI->Ctrl&C_SPI_Rx_Busy); //(W)Wait
        
        U32 **p32buf = (U32**)&buf;
        while(copy_len>=sizeof(U32)) {
            copy_len-=sizeof(U32);
            *(*p32buf)=SPI->Data;//(C)direct word copy
            (*p32buf)++;
        }
        if(copy_len>0) {
            temp = SPI->Data;//tail fragment
        }
    }
    U8 *pTemp = (U8*)&temp;
    U8 **p8buf = (U8**)&buf;
    while(copy_len--) {//(C)Copy for tail fragment
        *(*p8buf) = *pTemp;
        (*p8buf)++;
        pTemp++;
    }
} 
#if _SPI1_USE_FLASH
//------------------------------------------------------------------//
// Send command+address
// Parameter: 
//          cmd  : command code
//          addr : address
//          dummy: dummy byte 
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI1_SendCmdAddr(U8 cmd,U32 addr,U8 dummy)
{
    SPI_TypeDef *SPI = P_SPI1;
    U32 base = SPI->Ctrl & ~(C_SPI_FIFO_Length);
#if _EF_SERIES
    base = base & ~(C_SPI_DataMode);
#endif
    U32 ctrl = base | (C_SPI_Tx_Start | ((sizeof(cmd)-1)<<SPI_DATA_NUM_OFFSET));//(D)cmd
#if _EF_SERIES
    ctrl = ctrl | SpiCmdBusMode;
#endif
    SPI->Data=cmd;
    SPI->Ctrl = ctrl; //(T)cmd
    
#if     _SPI1_ADDR_BYTE==3
    U32 temp = __builtin_bswap32(addr)>>8;
#elif   _SPI1_ADDR_BYTE==4
    U32 temp = __builtin_bswap32(addr);
#else
#error  "_SPI1_ADDR_BYTE setting error"
#endif
    
    U8 len = _SPI1_ADDR_BYTE+dummy; //len must <=8
    do {
        U8 write_len = (len>SPI1_FIFO_SIZE)?SPI1_FIFO_SIZE:len;
        U8 writeLenInWord = (write_len+3)/4;
        ctrl = base | (C_SPI_Tx_Start | ((write_len-1)<<SPI_DATA_NUM_OFFSET));//(D)addr
#if _EF_SERIES
        ctrl = ctrl | SpiAddrBusMode;
#endif
        len-=write_len;
        while(SPI->Ctrl&C_SPI_Tx_Busy); //(W)
        do {
            SPI->Data = temp;
            temp = 0; //dummy section force 0
        } while(--writeLenInWord);
        SPI->Ctrl = ctrl;//(T)
    } while(len>0);
#if _EF_SERIES
    base |= SpiDataBusMode;
#endif
    while(SPI->Ctrl&C_SPI_Tx_Busy); //(W)
    SPI->Ctrl = base;
}  
//------------------------------------------------------------------//
// Send command
// Parameter: 
//          cmd:command code
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI1_SendCmd(U8 cmd)
{
    SPI_TypeDef *SPI = P_SPI1;
    
    SPI->Data  = cmd;    
#if _EF_SERIES
    SPI->Ctrl &= (~(C_SPI_FIFO_Length|C_SPI_DataMode));
    SPI->Ctrl |= C_SPI_Tx_Start| SpiCmdBusMode;
#else
    SPI->Ctrl &= ~(C_SPI_FIFO_Length);
    SPI->Ctrl |= C_SPI_Tx_Start;    
#endif
    do{
    }while(SPI->Ctrl&C_SPI_Tx_Busy); 
	
#if _EF_SERIES
    SPI->Ctrl &= (~(C_SPI_DataMode));
    SPI->Ctrl |= SpiDataBusMode;
#endif
}
//------------------------------------------------------------------//
// Send command with CS down and High 
// Parameter: 
//          cmd:command code
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI1_ByteCmd(U8 cmd)
{
	GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,0);
    SPI1_SendCmd(cmd);
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,1);
}
//------------------------------------------------------------------//
// Write data to Flash setting with CS down and High 
// Parameter: 
//          cmd		:	Command code
//			p_Data	:	Data to write
//          len  	: 	Data length to write(bytes)
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI1_WriteFlashSet(U8 cmd, U32* p_Data, U8 len)
{
#if _EF_SERIES
	SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);
#endif
	SPI1_WRENCmd(ENABLE);
	GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,0);
    SPI1_SendCmd(cmd);
    SPI1_SendData(p_Data,len);
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,1);
}
//------------------------------------------------------------------//
// Read data from Flash setting with CS down and High 
// Parameter: 
//          cmd		:	Command code
//			p_Data	:	Data to read
//          len  	: 	Data length to read(bytes)
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI1_ReadFlashSet(U8 cmd, U32* p_Data, U8 len)
{
#if _EF_SERIES
	SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);
#endif
	GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,0);
    SPI1_SendCmd(cmd);
    SPI1_ReceiveData(p_Data,len);
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,1);
}
//------------------------------------------------------------------//
// Enable or Disable WREN
// Parameter: 
//          cmd : ENABLE or DISABLE
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI1_WRENCmd(U8 cmd)
{
    U8 wren;
    
    if(cmd==ENABLE)
    {
        wren=SPI_WREN_CMD;
    }
    else
    {
        wren=SPI_WRDI_CMD;
    }
    
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,0);
    SPI1_SendCmd(wren);
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,1);
}
//------------------------------------------------------------------//
// Sector erase
// Parameter: 
//          addr:sector address
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI1_SectorErase(U32 addr)
{
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,0);    
    SPI1_SendCmdAddr(SPI_SECTOR_ERASE_CMD,addr,0);    
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,1);
} 
//------------------------------------------------------------------//
// Block erase
// Parameter: 
//          addr:block address
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI1_BlockErase(U32 addr)
{
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,0);    
    SPI1_SendCmdAddr(SPI_BLOCK_ERASE_CMD,addr,0);    
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,1);
}
//------------------------------------------------------------------//
// Chip erase
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI1_ChipErase(void)
{
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,0);
    SPI1_SendCmd(SPI_CHIP_ERASE_CMD);
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,1);
}
//------------------------------------------------------------------//
// Reset SPI1 Flash 
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI1_Reset(void)
{
#if _EF_SERIES
	SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);
#endif
	
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,0); 
    SPI1_SendCmd(SPI_RESETEN_CMD);
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,1);
    
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,0);
    SPI1_SendCmd(SPI_RESET_CMD);
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,1);
}
//------------------------------------------------------------------//
// Enable or Disable 4-byte address mode  
// Parameter: 
//          cmd:ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI1_4ByteAddrCmd(U8 cmd)
{ 
#if _EF_SERIES
	SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);
#endif

    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,0);
   
    if(cmd==ENABLE)
    {
        SPI1_SendCmd(SPI_ENTER_4BYTE_ADDR_CMD);
    }else
    {
        SPI1_SendCmd(SPI_EXIT_4BYTE_ADDR_CMD);
    }            
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,1);
}
//------------------------------------------------------------------//
// Burst read from SPI1
// Parameter: 
//          buf  : data buffer
//          addr : start address
//          len  : length to read(bytes)
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI1_BurstRead(U8 *buf,U32 addr,U16 len)
{
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,0);
#if _EF_SERIES
#if (_SPI1_ACCESS_MODE==SPI_1_1_1_MODE) || (_SPI1_ACCESS_MODE==SPI_1_1_1_MODE_3WIRE)
    SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);  
    SPI1_SendCmdAddr(SPI_READ_CMD,addr,0);  
#elif _SPI1_ACCESS_MODE==SPI_1_2_2_MODE    
    SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_2,SPI1_DATA_2);  
    SPI1_SendCmdAddr(SPI_2READ_CMD,addr,1);  
#elif _SPI1_ACCESS_MODE==SPI_1_1_2_MODE
    SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_2);  
    SPI1_SendCmdAddr(SPI_DUAL_READ_CMD,addr,1);    
#endif
#else
    SPI1_SendCmdAddr(SPI_READ_CMD,addr,0);
#endif
    SPI1_ReceiveData(buf,len);
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,1);
}
//------------------------------------------------------------------//
// Set start address
// Parameter: 
//          addr:start address
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI1_FastReadStart(U32 addr)
{
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,0);
#if _EF_SERIES
#if (_SPI1_ACCESS_MODE==SPI_1_1_1_MODE) || (_SPI1_ACCESS_MODE==SPI_1_1_1_MODE_3WIRE)
    SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);
    SPI1_SendCmdAddr(SPI_READ_CMD,addr,0);
#elif _SPI1_ACCESS_MODE==SPI_1_2_2_MODE
    SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_2,SPI1_DATA_2);
    SPI1_SendCmdAddr(SPI_2READ_CMD,addr,1);
#elif _SPI1_ACCESS_MODE==SPI_1_1_2_MODE
    SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_2);
    SPI1_SendCmdAddr(SPI_DUAL_READ_CMD,addr,1);
#endif
#else
    SPI1_SendCmdAddr(SPI_READ_CMD,addr,0);
#endif
}
//------------------------------------------------------------------//
// Stop read
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI1_FastReadStop(void)
{
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,1);
}
//------------------------------------------------------------------//
// Erase SPI1, erase unit is 64K bytes, block erase
// Parameter: 
//          addr:start address
//          len:legth to erase(bytes)
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI1_BurstErase(U32 addr,U32 len)
{
    U32 erase_len;
    U32 Data;
    
#if _EF_SERIES
	SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);
#endif

    while(len)
    {
        U16 eraseTimeoutCount=((_SPI1_BLOCK_ERASE_MAX_TIME*215)>>11)+1;    // 1 count = 10msec, and compensating ILRC +5% deviation.
        U32 eraseSysTick=SysTick;
        /*if(len>SPI_SECTOR_SIZE){
            erase_len=SPI_SECTOR_SIZE;
        }else{
            erase_len=len;
        }*/             
        if(len>SPI_BLOCK_SIZE)
        {
            erase_len=SPI_BLOCK_SIZE;
        }
        else
        {
            erase_len=len;
        }       
        SPI1_WRENCmd(ENABLE);
        
        //SPI1_SectorErase(addr);
        SPI1_BlockErase(addr);
        
        GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,0);
        SPI1_SendCmd(SPI_READ_STS_REG1_CMD);
        do{
            if(((SysTick%10)==0)&&(SysTick!=eraseSysTick)){
                WDT_Clear();
                eraseTimeoutCount--;
                eraseSysTick=SysTick;
            }
            SPI1_ReceiveData(&Data,1);        
        }while((Data&0x1)&&(eraseTimeoutCount));
        
        if(((Data&0x1)==1) && (eraseTimeoutCount==0)){
            CB_SPI1_BlockErase_Timeout();
        }

        GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,1);
    
        len-=erase_len;
        addr+=erase_len;
    }    
}
//------------------------------------------------------------------//
// Erase SPI1, erase unit is 4K bytes, sector erase
// Parameter: 
//          addr:start address
//          len:legth to erase(bytes)
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI1_BurstErase_Sector(U32 addr,U32 len)
{
    U32 erase_len;
    U32 Data;
    
#if _EF_SERIES
    SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);
#endif
    
    while(len)
    {
        U16 eraseTimeoutCount=((_SPI1_SECTOR_ERASE_MAX_TIME*215)>>11)+1;    // 1 count = 10msec, and compensating ILRC +5% deviation.
        U32 eraseSysTick=SysTick;
        if(len>SPI_SECTOR_SIZE){
            erase_len=SPI_SECTOR_SIZE;
        }else{
            erase_len=len;
        }                
/*        if(len>SPI_BLOCK_SIZE)
        {
            erase_len=SPI_BLOCK_SIZE;
        }
        else
        {
            erase_len=len;
        } */       
        SPI1_WRENCmd(ENABLE);
        
        SPI1_SectorErase(addr);
        //SPI1_BlockErase(addr);
        
        GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,0);
        SPI1_SendCmd(SPI_READ_STS_REG1_CMD);
        do{
            if(((SysTick%10)==0)&&(SysTick!=eraseSysTick)){
                WDT_Clear();
                eraseTimeoutCount--;
                eraseSysTick=SysTick;
            }
            SPI1_ReceiveData(&Data,1);        
        }while((Data&0x1)&&(eraseTimeoutCount));
        
        if(((Data&0x1)==1) && (eraseTimeoutCount==0)){
            CB_SPI1_SectorErase_Timeout();
        }
        
        GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,1);
    
        len-=erase_len;
        addr+=erase_len;
    }    
} 
//------------------------------------------------------------------//
// Erase SPI1, erase unit is 4k bytes, sector erase
// Parameter: 
//          addr: Erase start address of the sector
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI1_StartSectorErase(U32 addr)
{
#if _EF_SERIES
	SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);
#endif
    SPI1_WRENCmd(ENABLE);
    SPI1_SectorErase(addr);  
}
//------------------------------------------------------------------//
// Erase SPI1, erase unit is 64k bytes, block erase
// Parameter: 
//          addr: Erase start address of the block
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI1_StartBlockErase(U32 addr)    //C_MODULE need add.
{
#if _EF_SERIES
    SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);
#endif
    SPI1_WRENCmd(ENABLE);
    SPI1_BlockErase(addr);
} 
//------------------------------------------------------------------//
// Erase SPI1 whole chip
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI1_StartChipErase(void)
{
#if _EF_SERIES
    SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);
#endif
    SPI1_WRENCmd(ENABLE);
    SPI1_ChipErase();
} 
//------------------------------------------------------------------//
// Check Flash Busy Flag
// Parameter: 
//          NONE 
// return value:
//          return: 1: busy, 0: non-busy
//------------------------------------------------------------------//    
U8 SPI1_CheckBusyFlag(void)
{
    U32 Data;
    
#if _EF_SERIES
	SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);
#endif
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,0);
    SPI1_SendCmd(SPI_READ_STS_REG1_CMD);
    SPI1_ReceiveData(&Data,1);
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,1);
    Data &= 0x1;

    return Data;
}
//------------------------------------------------------------------//
// Burst write to SPI1
// Parameter: 
//          buf:data buffer
//          addr:start address
//          len:legth to write(bytes)
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI1_BurstWrite(U8 *buf,U32 addr,U16 len)
{
    U8 write_len,i;
    U16 page_size;
    U16 offset=0;
    U32 Data;
    U8 temp[SPI1_FIFO_SIZE];
    
    while(len)
    {
        SPI1_WRENCmd(ENABLE);       
	#if _EF_SERIES
		SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);
	#endif
            
        GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,0);
        SPI1_SendCmdAddr(SPI_PAGE_PRG_CMD,addr+offset,0);
        
        page_size=SPI_PAGE_SIZE-((addr+offset)&(SPI_PAGE_SIZE-1));
        
        if(page_size>len)
        {
            page_size=len;    
        }    
        
        while(page_size)
        {           
            if(page_size>SPI1_FIFO_SIZE)
            {
                write_len=SPI1_FIFO_SIZE;
            }
            else
            {
                write_len=page_size;
            }        
            for(i=0;i<write_len;i++)
            {
                temp[i]=buf[offset+i];
            }    
            SPI1_SendData((U32 *)temp,write_len);
            
            offset+=write_len;
            page_size-=write_len;
            len-=write_len;
        }            
        GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,1);
        
	#if _EF_SERIES
		SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);
	#endif
        GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,0);
        SPI1_SendCmd(SPI_READ_STS_REG1_CMD);
        do{
            SPI1_ReceiveData(&Data,1);
        
        }while(Data&0x1);        
        GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,1);
    
    }
} 
//------------------------------------------------------------------//
// Write status register(1 byte)
// Parameter: 
//          reg : 1,2,3
//          data:data to write
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI1_WriteStsReg(U8 reg,U32 data)
{
    U32 buf;

#if _EF_SERIES
	SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);
#endif
	
    SPI1_WRENCmd(ENABLE);
    
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,0);     
    if(reg==1) 
    {
        SPI1_SendCmd(SPI_WRITE_STS_REG1_CMD);
    }else if(reg==2) 
    {
        SPI1_SendCmd(SPI_WRITE_STS_REG2_CMD);
    }else if(reg==3) 
    {
        SPI1_SendCmd(SPI_WRITE_STS_REG3_CMD);
    }            
    SPI1_SendData(&data,1);
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,1);
//    
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,0);
    SPI1_SendCmd(SPI_READ_STS_REG1_CMD);
    do{
        SPI1_ReceiveData(&buf,1);
    }while(buf&0x1);
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,1);
}    
//------------------------------------------------------------------//
// Write status register(2 byte)
// Parameter: 
//          data:data to write
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI1_WriteStsReg2(U32 data)
{
    U32 buf;

#if _EF_SERIES
	SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);
#endif
    
    SPI1_WRENCmd(ENABLE);
    
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,0);     
    SPI1_SendCmd(SPI_WRITE_STS_REG1_CMD);
    SPI1_SendData(&data,2);
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,1);
//    
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,0);       
    SPI1_SendCmd(SPI_READ_STS_REG1_CMD);
    do{
        SPI1_ReceiveData(&buf,1);
    }while(buf&0x1);    
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,1);
}
//------------------------------------------------------------------//
// read status register
// Parameter: 
//          reg : 1,2,3
// return value:
//          Status data
//------------------------------------------------------------------//      
U32 SPI1_ReadStsReg(U8 reg)
{
    U32 Data;

#if _EF_SERIES
	SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);
#endif
    
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,0);
    if(reg==1) 
    {
        SPI1_SendCmd(SPI_READ_STS_REG1_CMD);
    }else if(reg==2) 
    {
        SPI1_SendCmd(SPI_READ_STS_REG2_CMD);    
    }else if(reg==3) 
    {
        SPI1_SendCmd(SPI_READ_STS_REG3_CMD);    
    }
    
    Data=0;        
    SPI1_ReceiveData(&Data,1);        
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,1);
    
    return Data;
}
//------------------------------------------------------------------//
// Get Vendor ID
// Parameter: 
//          NONE
// return value:
//          Manufacture ID
//------------------------------------------------------------------//      
U32 SPI1_GetVendorID(void)
{
    U32 _Data=0;
    SPI1_ReadFlashSet(SPI_READ_MFID_DEVID_CMD,&_Data,3);//Read JEDEC_ID, CMD=0x9F
    return _Data;
}               
//------------------------------------------------------------------//
// Enable 4Byte Address (volatile)
// Parameter: 
//          _JEDEC_ID(Big endian):
//					[0:7]:MF0-MF7(Manufacturer ID)
//					[8:15]:ID8-ID15(Memory Type ID)
//					[16:23]:ID0-ID7(Capacity ID)
// return value:
//          EXIT_SUCCESS:Setting success
//          EXIT_FAILURE:Setting failure
//------------------------------------------------------------------// 
U8	SPI1_4ByteAddressEnable(const U32 _JEDEC_ID)
{
	switch(_JEDEC_ID)
	{
		case	(SPI_VENDOR_XMC		|(0x1960<<8))://(XMC)xm25QH256b
		case	(SPI_VENDOR_WINBOND	|(0x1940<<8))://(winbond)w25q256fv
		#if _EF_SERIES
			SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);
		#endif
			SPI1_ByteCmd(0xB7);//4Byte Address Enable CMD=0xB7
			return (SPI1_Is4ByteAddressSet(_JEDEC_ID)==true)?EXIT_SUCCESS:EXIT_FAILURE;
			break;
		default:
			return EXIT_FAILURE;
			break;
	}
}
//------------------------------------------------------------------//
// Disable 4Byte Address (volatile)
// Parameter: 
//          _JEDEC_ID(Big endian):
//					[0:7]:MF0-MF7(Manufacturer ID)
//					[8:15]:ID8-ID15(Memory Type ID)
//					[16:23]:ID0-ID7(Capacity ID)
// return value:
//          EXIT_SUCCESS:Setting success
//          EXIT_FAILURE:Setting failure
//------------------------------------------------------------------// 
U8	SPI1_4ByteAddressDisable(const U32 _JEDEC_ID)
{
	switch(_JEDEC_ID)
	{
		case	(SPI_VENDOR_XMC		|(0x1960<<8))://(XMC)xm25QH256b
		#if _EF_SERIES
			SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);
		#endif
			SPI1_ByteCmd(0x29);//4Byte Address Disable CMD=0x29
			return (SPI1_Is4ByteAddressSet(_JEDEC_ID)==false)?EXIT_SUCCESS:EXIT_FAILURE;
			break;
		case	(SPI_VENDOR_WINBOND	|(0x1940<<8))://(winbond)w25q256fv
		#if _EF_SERIES
			SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);
		#endif
			SPI1_ByteCmd(0xE9);//4Byte Address Disable CMD=0xE9
			return (SPI1_Is4ByteAddressSet(_JEDEC_ID)==false)?EXIT_SUCCESS:EXIT_FAILURE;
			break;
		default:
			return EXIT_FAILURE;
			break;
	}
}
//------------------------------------------------------------------//
// Is 4Byte Address Set?
// Parameter: 
//          _JEDEC_ID(Big endian):
//					[0:7]:MF0-MF7(Manufacturer ID)
//					[8:15]:ID8-ID15(Memory Type ID)
//					[16:23]:ID0-ID7(Capacity ID)
// return value:
//          true:  Is setting
//          false: Not set, or ID not exit
//------------------------------------------------------------------// 
bool SPI1_Is4ByteAddressSet(const U32 _JEDEC_ID)
{
	U32 _Data=0;
	switch(_JEDEC_ID)
	{
		case	(SPI_VENDOR_XMC		|(0x1960<<8))://(XMC)xm25QH256b
			SPI1_ReadFlashSet(0x16,&_Data,1);//Read BankAddress REG, CMD=0x16
			return ((_Data&(0x1<<7))!=0)?true:false;//check EXTADD status(Bank Addr REG, bit7)
			break;
		case	(SPI_VENDOR_WINBOND	|(0x1940<<8))://(winbond)w25q256fv
			SPI1_ReadFlashSet(0x15,&_Data,1);//Read REG-3, CMD=0x15
			return ((_Data&(0x1<<0))!=0)?true:false;//check ADS status(REG-3, bit0)
			break;
		default:
			return false;
			break;
	}
}
#endif	//_SPI1_USE_FLASH

//------------------------------------------------------------------//
// Set CS pin Lo
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI1_CS_On(void)
{
#if _EF_SERIES
    SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);
#endif
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,0);
}
//------------------------------------------------------------------//
// Set CS pin Hi
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI1_CS_Off(void)
{
    GPIO_Write(SPI1_CS_PORT,SPI1_CS_PIN,1);
}
//------------------------------------------------------------------//
// Is SPI Bus Busy(Check CS Pin)
// Parameter: 
//          NONE
// return value:
//          true / false
//------------------------------------------------------------------//
bool SPI1_isBusBusy(void)
{
    return ((SPI1_CS_PORT->Data>>SPI1_CS_PIN)&0x1)==0;
}
//------------------------------------------------------------------//
// Send Data, Set 1 Byte.
// Parameter: 
//          _data: 1 Byte
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI1_TX(U32 _data)
{
    SPI1_SendData(&_data,1);
}
//------------------------------------------------------------------//
// Read Data, Get 1 Byte.
// Parameter: 
//          NONE
// return value:
//          Data(1 Byte)
//------------------------------------------------------------------//
U32  SPI1_RX(void)
{
    U32 _data=0;
    SPI1_ReceiveData(&_data,1);
    return _data;
}
#if _SPI1_USE_FLASH
//------------------------------------------------------------------//
// Write status register for Q-Code
// Parameter:
//          reg : 0x1,0x31,0x11
//          data:data to write
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI1_WRSR(U8 reg,U32 data, U8 len)
{
    SPI1_WriteFlashSet(reg,&data,len);
    while(SPI1_CheckBusyFlag());
}
//------------------------------------------------------------------//
// read status register for Q-Code
// Parameter:
//          reg : 0x5,0x35,0x15
// return value:
//          Status data
//------------------------------------------------------------------//
U32 SPI1_RDSR(U8 reg)
{
    U32 Data=0;
    SPI1_ReadFlashSet(reg, &Data, 1);
    return Data;
}
//------------------------------------------------------------------//
// Start Page Program (cmd = 0x02)
// Parameter: 
//          _addr:Program Address
//          _data:1st Byte Data.
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI1_WRD(U32 _addr, U32 _data)
{
#if _EF_SERIES
    SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);
#endif
    SPI1_WRENCmd(ENABLE);
    SPI1_CS_On();
    SPI1_SendCmdAddr(SPI_PAGE_PRG_CMD, _addr, 0);
    SPI1_TX(_data);
}
//------------------------------------------------------------------//
// Start Read Data (cmd = 0x03)
// Parameter: 
//          _addr:Read Address
// return value:
//          1st Byte Data.
//------------------------------------------------------------------//
U32  SPI1_RDD(U32 _addr)
{
#if _EF_SERIES
    SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);
#endif
    SPI1_CS_On();
    SPI1_SendCmdAddr(SPI_READ_CMD, _addr, 0);
    return SPI1_RX();
}
//------------------------------------------------------------------//
// Write Enable (cmd = 0x06)
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI1_WREN(void)
{
#if _EF_SERIES
    SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);
#endif
    SPI1_WRENCmd(ENABLE);
}
//------------------------------------------------------------------//
// Write Disable (cmd = 0x04)
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI1_WRDIS(void)
{
#if _EF_SERIES
    SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);
#endif
    SPI1_WRENCmd(DISABLE);
}
//------------------------------------------------------------------//
// Read JEDEC ID (cmd = 0x9F)
// Parameter: 
//          NONE
// return value:
//          JEDEC_ID:
//              Manufacture ID[23:16]
//              Memory Type[15:8]
//              Capacity[7:0]
//------------------------------------------------------------------//
U32  SPI1_RDID(void)
{
    U32 _data = SPI1_GetVendorID();
    return __builtin_bswap32(_data)>>8;
}
//------------------------------------------------------------------//
// Chip Erase (cmd = 0xC7)
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI1_CE(void)
{
    SPI1_StartChipErase();
}
//------------------------------------------------------------------//
// Sector Erase (cmd = 0x20)
// Parameter: 
//          _addr: Sector Erase start address
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI1_SE(U32 _addr)
{
    SPI1_StartSectorErase(_addr);
}
//------------------------------------------------------------------//
// Block Erase (cmd = 0xD8)
// Parameter: 
//          _addr: Block Erase start address
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI1_BE(U32 _addr)
{
    SPI1_StartBlockErase(_addr);
}
//------------------------------------------------------------------//
// Deep Power-Down (cmd = 0xB9)
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI1_DP(void)
{
#if _EF_SERIES
    SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);
#endif
    SPI1_ByteCmd(SPI_POWER_DOWN_CMD);
}
//------------------------------------------------------------------//
// Release Deep Power-down / Get ID(cmd = 0xAB)
// Parameter: 
//          NONE
// return value:
//          Flash chip ID(8-Bits)
//------------------------------------------------------------------//
U32  SPI1_RDP(void)
{
#if _EF_SERIES
    SPI1_SetBusMode(SPI1_DATA_1,SPI1_DATA_1,SPI1_DATA_1);
#endif
    SPI1_CS_On();
    SPI1_TX(SPI_RELEASE_POWER_DOWN_CMD);
    SPI1_RX(); //dummy
    SPI1_RX(); //dummy
    SPI1_RX(); //dummy
    U32 _data = SPI1_RX();
    SPI1_CS_Off();
    return _data;
}
//------------------------------------------------------------------//
// Get User Defined Section address (support by SPI Encoder)
// Parameter: 
//          _index:0~65535
// return value:
//          address of UDR
//------------------------------------------------------------------//
U32  SPI1_GetAddr(U16 _index)
{
    return UserDefinedData_GetAddressUseType(_index, kUD_USER_DEFINED, SPI1_MODE);
}
#endif	//_SPI1_USE_FLASH
#endif	//#if	defined(P_SPI1)

#endif	//_SPI1_MODULE