#include "nx1_spi.h"
#include "nx1_gpio.h"
#include "nx1_wdt.h"
#include "include.h"
#include <string.h>


//////////////////////////////////////////////////////////////////////
//  Static Variables
//////////////////////////////////////////////////////////////////////
static U8 SpiCmdBusMode, SpiAddrBusMode, SpiDataBusMode;

//////////////////////////////////////////////////////////////////////
//  Extern Functions
//////////////////////////////////////////////////////////////////////
extern void CB_SPI0_BlockErase_Timeout(void);
extern void CB_SPI0_SectorErase_Timeout(void);


//------------------------------------------------------------------//
// SPI Init
// Parameter: 
//          divide  : SPI_CLKDIV_1,SPI_CLKDIV_2...
//          pol 	: SPI_Pol_Low, SPI_Pol_High
//          pha     : SPI_Phase_1st_Edge, SPI_Phase_2nd_Edge
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI_Init(U32 divide,U8 pol, U8 pha)
{
    SPI_SetClockDivisor(divide);
    SPI_SetClockPolarity(pol);
    SPI_SetClockPhase(pha);
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,1);
}
//------------------------------------------------------------------//
// SPI set clock divisor 
// Parameter: 
//          divide  : SPI_CLKDIV_1,SPI_CLKDIV_2...SPI_CLKDIV_128
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI_SetClockDivisor(U32 divide)
{
    SPI_TypeDef *SPI = P_SPI0;
    SPI->Ctrl = (SPI->Ctrl & ~(C_SPI_CLK_Div)) | divide;
}
//------------------------------------------------------------------//
// SPI set clock polarity
// Parameter: 
//          pol     : SPI_Pol_Low, SPI_Pol_High
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI_SetClockPolarity(U32 pol)
{
    SPI_TypeDef *SPI = P_SPI0;
    SPI->Ctrl = (SPI->Ctrl & ~(C_SPI_Pol)) | pol;
}
//------------------------------------------------------------------//
// SPI set clock phase
// Parameter: 
//          pha     : SPI_Phase_1st_Edge, SPI_Phase_2nd_Edge
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI_SetClockPhase(U32 pha)
{
    SPI_TypeDef *SPI = P_SPI0;
    SPI->Ctrl = (SPI->Ctrl & ~(C_SPI_Phase)) | pha;
}
//------------------------------------------------------------------//
// Set SPI bus mode
// Parameter: 
//          cmd_bus_mode  : SPI_DATA_1,SPI_DATA_2,SPI_DATA_4
//          addr_bus_mode : SPI_DATA_1,SPI_DATA_2,SPI_DATA_4
//          data_bus_mode : SPI_DATA_1,SPI_DATA_2,SPI_DATA_4
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI_SetBusMode(U8 cmd_bus_mode,U8 addr_bus_mode,U8 data_bus_mode)
{
    SpiCmdBusMode = cmd_bus_mode;
    SpiAddrBusMode= addr_bus_mode;
    SpiDataBusMode= data_bus_mode;
}
//------------------------------------------------------------------//
// Send command+address
// Parameter: 
//          cmd  : command code
//          addr : address
//          dummy: dummy byte 
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI_SendCmdAddr(U8 cmd,U32 addr,U8 dummy)
{
    SPI_TypeDef *SPI = P_SPI0;
    U32 base = SPI->Ctrl & ~(C_SPI_FIFO_Length | C_SPI_DataMode);
    U32 ctrl = base | (C_SPI_Tx_Start | ((sizeof(cmd)-1)<<SPI_DATA_NUM_OFFSET) | SpiCmdBusMode);//(D)cmd
    SPI->Data = cmd;
    SPI->Ctrl = ctrl; //(T)cmd
    
#if     _SPI_ADDR_BYTE==3
    U32 temp = __builtin_bswap32(addr)>>8;
#elif   _SPI_ADDR_BYTE==4
    U32 temp = __builtin_bswap32(addr);
#else
#error  "_SPI_ADDR_BYTE setting error"
#endif
    
    U8 len = _SPI_ADDR_BYTE+dummy;
    do {
        U8 write_len = (len>SPI_FIFO_SIZE)?SPI_FIFO_SIZE:len;
        U8 writeLenInWord = (write_len+3)/4;
        ctrl = base | (C_SPI_Tx_Start | ((write_len-1)<<SPI_DATA_NUM_OFFSET) | SpiAddrBusMode);//(D)addr
        len-=write_len;
        while(SPI->Ctrl&C_SPI_Tx_Busy); //(W)
        do {
            SPI->Data = temp;
            temp = 0; //dummy section force 0
        } while(--writeLenInWord);
        SPI->Ctrl = ctrl;//(T)
    } while(len>0);
    base |= SpiDataBusMode;
    while(SPI->Ctrl&C_SPI_Tx_Busy); //(W)
    SPI->Ctrl = base;
}
//------------------------------------------------------------------//
// Send command
// Parameter: 
//          cmd : command code
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI_SendCmd(U8 cmd)
{
    SPI_TypeDef *SPI = P_SPI0;
       
    SPI->Data=cmd;
       
    SPI->Ctrl &= (~(C_SPI_FIFO_Length|C_SPI_DataMode));
    SPI->Ctrl |= C_SPI_Tx_Start| SpiCmdBusMode;
    
    do{
    }while(SPI->Ctrl&C_SPI_Tx_Busy); 
    
    
    SPI->Ctrl &= (~C_SPI_DataMode);
    SPI->Ctrl |= SpiDataBusMode;

} 
//------------------------------------------------------------------//
// Send command with CS down and High 
// Parameter: 
//          cmd:command code
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI_ByteCmd(U8 cmd)
{
	GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,0);
    SPI_SendCmd(cmd);
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,1);
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
void SPI_WriteFlashSet(U8 cmd, U32* p_Data, U8 len)
{
	SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);
	SPI_WRENCmd(ENABLE);
	GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,0);
    SPI_SendCmd(cmd);
    SPI_SendData(p_Data,len);
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,1);
}
//------------------------------------------------------------------//
// Read data from Flash setting with CS down and High 
// Parameter: 
//          cmd:command code
//			p_Data:Data to read
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI_ReadFlashSet(U8 cmd, U32* p_Data, U8 len)
{
	SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);
	GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,0);
    SPI_SendCmd(cmd);
    SPI_ReceiveData(p_Data,len);
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,1);
}
//------------------------------------------------------------------//
// Send data
// Parameter: 
//          data : data buffer
//          len  : length to write(bytes)
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI_SendData(U32 *data,U8 len)
{
    U8 i,j,k;
    SPI_TypeDef *SPI = P_SPI0;

    do{
        if(len>SPI_FIFO_SIZE)
        {
            k=SPI_FIFO_SIZE;
        }
        else
        {    
            k=len;
        }
        j=(k+3)>>2;
        for(i=0;i<j;i++)
        {
            SPI->Data=*data++;    
        }    
        SPI->Ctrl &= (~C_SPI_FIFO_Length);
        SPI->Ctrl |= (C_SPI_Tx_Start | ((k-1)<<SPI_DATA_NUM_OFFSET));
        
        do{
        }while(SPI->Ctrl&C_SPI_Tx_Busy);

        len-=k;
    }while(len);
}     
//------------------------------------------------------------------//
// Receive data
// Parameter: 
//          data : data buffer
//          len  : length to read(bytes)
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI_ReceiveData(void *buf,U16 len)
{
    SPI_TypeDef *SPI = P_SPI0;
    U32 base = (SPI->Ctrl & ~(C_SPI_FIFO_Length)) | C_SPI_Rx_Start;
    U32 temp;
    U32 copy_len=0;
    U32 read_len = (U32)buf;
    if((read_len&0x3)==0) {
        read_len = (len>SPI_FIFO_SIZE)?SPI_FIFO_SIZE:len;
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
        read_len = (len>SPI_FIFO_SIZE)?SPI_FIFO_SIZE:len;
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
//------------------------------------------------------------------//
// Enable or Disable WREN
// Parameter: 
//          cmd : ENABLE or DISABLE
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI_WRENCmd(U8 cmd)
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
    
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,0);
    
    SPI_SendCmd(wren);
    
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,1);
    
}    
//------------------------------------------------------------------//
// Sector erase
// Parameter: 
//          addr:sector address
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI_SectorErase(U32 addr)
{
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,0);
    
    SPI_SendCmdAddr(SPI_SECTOR_ERASE_CMD,addr,0);
    
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,1);
}    
//------------------------------------------------------------------//
// Block erase
// Parameter: 
//          addr:block address
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI_BlockErase(U32 addr)
{
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,0);
    
    SPI_SendCmdAddr(SPI_BLOCK_ERASE_CMD,addr,0);
    
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,1);
}   
//------------------------------------------------------------------//
// Chip erase
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI_ChipErase(void)
{
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,0);
    SPI_SendCmd(SPI_CHIP_ERASE_CMD);
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,1);
}
//------------------------------------------------------------------//
// Reset SPI 
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI_Reset(void)
{
    SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);
    
    
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,0);
      
    SPI_SendCmd(SPI_RESETEN_CMD);
   
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,1);
    
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,0);
    
    SPI_SendCmd(SPI_RESET_CMD);
   
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,1);
}         
//------------------------------------------------------------------//
// Enable or Disable QE  
// Parameter: 
//          cmd:ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI_QECmd(U8 cmd)
{
    if(cmd==ENABLE)
    {
        SPI_WriteStsReg(2,SPI_QE_BIT);
    }else
    {
        SPI_WriteStsReg(2,0);
    }            
}     
//------------------------------------------------------------------//
// Enable or Disable 4-byte address mode  
// Parameter: 
//          cmd:ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI_4ByteAddrCmd(U8 cmd)
{
    SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);
     
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,0);
      
    if(cmd==ENABLE)
    {
        SPI_SendCmd(SPI_ENTER_4BYTE_ADDR_CMD);
    }else
    {
        SPI_SendCmd(SPI_EXIT_4BYTE_ADDR_CMD);
    }            
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,1);
}       

//------------------------------------------------------------------//
// Burst read from SPI
// Parameter: 
//          buf  : data buffer
//          addr : start address
//          len  : length to read(bytes)
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI_BurstRead(U8 *buf,U32 addr,U16 len)
{
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,0);
#if (_SPI_ACCESS_MODE==SPI_1_1_1_MODE) || (_EF_SERIES && (_SPI_ACCESS_MODE==SPI_1_1_1_MODE_3WIRE))
    SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);  
    SPI_SendCmdAddr(SPI_READ_CMD,addr,0);  
#elif _SPI_ACCESS_MODE==SPI_1_2_2_MODE    
    SPI_SetBusMode(SPI_DATA_1,SPI_DATA_2,SPI_DATA_2);  
    SPI_SendCmdAddr(SPI_2READ_CMD,addr,1);  
#elif _SPI_ACCESS_MODE==SPI_1_1_2_MODE
    SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_2);  
    SPI_SendCmdAddr(SPI_DUAL_READ_CMD,addr,1);    
#elif _SPI_ACCESS_MODE==SPI_1_4_4_MODE
    SPI_SetBusMode(SPI_DATA_1,SPI_DATA_4,SPI_DATA_4); 
    SPI_SendCmdAddr(SPI_4READ_CMD,addr,3);
#endif
    SPI_ReceiveData(buf,len);
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,1);
}
//------------------------------------------------------------------//
// Set start address
// Parameter: 
//          addr:start address
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI_FastReadStart(U32 addr)
{   
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,0);
    
#if (_SPI_ACCESS_MODE==SPI_1_1_1_MODE) || (_EF_SERIES && (_SPI_ACCESS_MODE==SPI_1_1_1_MODE_3WIRE))
    SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);  
    SPI_SendCmdAddr(SPI_READ_CMD,addr,0);  
#elif _SPI_ACCESS_MODE==SPI_1_2_2_MODE    
    SPI_SetBusMode(SPI_DATA_1,SPI_DATA_2,SPI_DATA_2);  
    SPI_SendCmdAddr(SPI_2READ_CMD,addr,1);  
#elif _SPI_ACCESS_MODE==SPI_1_1_2_MODE
    SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_2);  
    SPI_SendCmdAddr(SPI_DUAL_READ_CMD,addr,1);      
#elif _SPI_ACCESS_MODE==SPI_1_4_4_MODE
    SPI_SetBusMode(SPI_DATA_1,SPI_DATA_4,SPI_DATA_4); 
    SPI_SendCmdAddr(SPI_4READ_CMD,addr,3);
#endif    
    
//    SPI0->Ctl&=(~SPI_DATA_NUM_SEL);
//        
//    SPI0->Ctl|=((SPI_FIFO_SIZE-1)<<SPI_DATA_NUM_OFFSET);
}    
//------------------------------------------------------------------//
// Read data
// Parameter: 
//          *buf:data buffer,alignment of 4
//          len:legth to read(bytes)
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI_FastRead(U32 *buf,U32 len)
{
    U8 last_bytes,i;
    
    last_bytes= len&(SPI_FIFO_SIZE-1);
    len-=last_bytes;
    
    if(len){
        P_SPI0->Ctrl&=(~C_SPI_FIFO_Length);
        P_SPI0->Ctrl|=((SPI_FIFO_SIZE-1)<<SPI_DATA_NUM_OFFSET);   
        while(len){
            //GPIOB->Data&=(~(1<<10));
            P_SPI0->Ctrl|=C_SPI_Rx_Start;    
            do{
            }while(P_SPI0->Ctrl&C_SPI_Rx_Busy); 
            
            *buf++=P_SPI0->Data;
            *buf++=P_SPI0->Data;
            *buf++=P_SPI0->Data;
            *buf++=P_SPI0->Data;
            // GPIOB->Data|=(1<<10);
            len-=SPI_FIFO_SIZE;
        };  
    }
  
    if(last_bytes){
        P_SPI0->Ctrl&=(~C_SPI_FIFO_Length);
        P_SPI0->Ctrl|=C_SPI_Rx_Start|((last_bytes-1)<<SPI_DATA_NUM_OFFSET); 
         
        do{
        }while(P_SPI0->Ctrl&C_SPI_Rx_Busy);       
        
        last_bytes=last_bytes>>2;
        
        for(i=0;i<last_bytes;i++){
            *buf++=P_SPI0->Data;
        }    
    }    
}    
//------------------------------------------------------------------//
// Stop read
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI_FastReadStop(void)
{
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,1);
} 
//------------------------------------------------------------------//
// Erase Spi, erase unit is 64K bytes, block erase
// Parameter: 
//          addr:start address
//          len:legth to erase(bytes)
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI_BurstErase(U32 addr,U32 len)
{
    U32 erase_len;
    U32 Data;
    
    SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);
    
    while(len)
    {
        U16 eraseTimeoutCount=((_SPI_BLOCK_ERASE_MAX_TIME*215)>>11)+1;    // 1 count = 10msec, and compensating ILRC +5% deviation.
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
        SPI_WRENCmd(ENABLE);
        
        //SPI_SectorErase(addr);
        SPI_BlockErase(addr);
        
        GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,0);
        SPI_SendCmd(SPI_READ_STS_REG1_CMD);
        do{
            if(((SysTick%10)==0)&&(SysTick!=eraseSysTick)){
                WDT_Clear();
                eraseTimeoutCount--;
                eraseSysTick=SysTick;
            }
            SPI_ReceiveData(&Data,1);        
        }while((Data&0x1)&&(eraseTimeoutCount));
        
        if(((Data&0x1)==1) && (eraseTimeoutCount==0)){
            CB_SPI0_BlockErase_Timeout();
        }

        GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,1);
    
        len-=erase_len;
        addr+=erase_len;
    }    
}   
//------------------------------------------------------------------//
// Erase Spi, erase unit is 4K bytes, sector erase
// Parameter: 
//          addr:start address
//          len:legth to erase(bytes)
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI_BurstErase_Sector(U32 addr,U32 len)
{
    U32 erase_len;
    U32 Data;
    
    SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);
    
    while(len)
    {
        U16 eraseTimeoutCount=((_SPI_SECTOR_ERASE_MAX_TIME*215)>>11)+1;    // 1 count = 10msec, and compensating ILRC +5% deviation.
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
        SPI_WRENCmd(ENABLE);
        
        SPI_SectorErase(addr);
        //SPI_BlockErase(addr);
        
        GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,0);
        SPI_SendCmd(SPI_READ_STS_REG1_CMD);
        do{
            if(((SysTick%10)==0)&&(SysTick!=eraseSysTick)){
                WDT_Clear();
                eraseTimeoutCount--;
                eraseSysTick=SysTick;
            }
            SPI_ReceiveData(&Data,1);        
        }while((Data&0x1)&&(eraseTimeoutCount));

        if(((Data&0x1)==1) && (eraseTimeoutCount==0)){
            CB_SPI0_SectorErase_Timeout();
        }
        
        GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,1);
    
        len-=erase_len;
        addr+=erase_len;
    }    
} 
//------------------------------------------------------------------//
// Erase Spi, erase unit is 4k bytes, sector erase
// Parameter: 
//          addr: Erase start address of the sector
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI_StartSectorErase(U32 addr)
{
    SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);
    SPI_WRENCmd(ENABLE);
    SPI_SectorErase(addr);
    
} 
//------------------------------------------------------------------//
// Erase Spi, erase unit is 64k bytes, block erase
// Parameter: 
//          addr: Erase start address of the block
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI_StartBlockErase(U32 addr)    //C_MODULE need add.
{
    SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);
    SPI_WRENCmd(ENABLE);
    SPI_BlockErase(addr);
} 
//------------------------------------------------------------------//
// Erase SPI whole chip
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI_StartChipErase(void)
{
    SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);
    SPI_WRENCmd(ENABLE);
    SPI_ChipErase();
} 
//------------------------------------------------------------------//
// Check Flash Busy Flag
// Parameter: 
//          NONE 
// return value:
//          return: 1: busy, 0: non-busy
//------------------------------------------------------------------//    
U8 SPI_CheckBusyFlag(void)
{
    U32 Data;
    
    SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,0);
    SPI_SendCmd(SPI_READ_STS_REG1_CMD);
    SPI_ReceiveData(&Data,1);
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,1);
    Data &= 0x1;

    return Data;
}
//------------------------------------------------------------------//
// Wait erase done
// Parameter: 
//          NONE 
// return value:
//          0 : pass
//          -1 : fail, timeout
//------------------------------------------------------------------//
S8 SPI_WaitEraseDone(void)
{
#define SPI_WAIT_TIMEOUT_ms  (_SPI_SECTOR_ERASE_MAX_TIME + (_SPI_SECTOR_ERASE_MAX_TIME/20) )  // *1.05
    U32 OldSysTick;

    OldSysTick = SysTick;
    while(SPI_CheckBusyFlag())
    {
        U32 difTick;
        
        difTick = SysTick-OldSysTick;
        if( difTick > SPI_WAIT_TIMEOUT_ms )
        {
            CB_SPI0_SectorErase_Timeout();
            //dprintf("erase time out\r\n");
            return -1;
        }
        if( (difTick%10) ==0 )
        {
            AUDIO_ServiceLoop();
            WDT_Clear();
        }
    }
    return 0;
}
//------------------------------------------------------------------//
// Burst write to SPI
// Parameter: 
//          buf:data buffer
//          addr:start address
//          len:legth to write(bytes)
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI_BurstWrite(U8 *buf,U32 addr,U16 len)
{
    U8 write_len,i;
    U16 page_size;
    U16 offset=0;
    U32 Data;
    U8 temp[SPI_FIFO_SIZE];
    
    while(len)
    {
        SPI_WRENCmd(ENABLE);       
        SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);
        
        
        GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,0);
        SPI_SendCmdAddr(SPI_PAGE_PRG_CMD,addr+offset,0);
        
        page_size=SPI_PAGE_SIZE-((addr+offset)&(SPI_PAGE_SIZE-1));
        
        if(page_size>len)
        {
            page_size=len;    
        }    
        
        while(page_size)
        {           
            if(page_size>SPI_FIFO_SIZE)
            {
                write_len=SPI_FIFO_SIZE;
            }
            else
            {
                write_len=page_size;
            }        
            for(i=0;i<write_len;i++)
            {
                temp[i]=buf[offset+i];
            }    
            SPI_SendData((U32 *)temp,write_len);
            
            offset+=write_len;
            page_size-=write_len;
            len-=write_len;
        }            
        GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,1);
        
        SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);
        GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,0);
        SPI_SendCmd(SPI_READ_STS_REG1_CMD);
        do{
            SPI_ReceiveData(&Data,1);
        
        }while(Data&0x1);        
        GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,1);
    
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
void SPI_WriteStsReg(U8 reg,U32 data)
{
    U32 buf;
    
    SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);
    
    SPI_WRENCmd(ENABLE);
    
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,0);     
    if(reg==1) 
    {
        SPI_SendCmd(SPI_WRITE_STS_REG1_CMD);
    }else if(reg==2) 
    {
        SPI_SendCmd(SPI_WRITE_STS_REG2_CMD);
    }else if(reg==3) 
    {
        SPI_SendCmd(SPI_WRITE_STS_REG3_CMD);
    }            
    SPI_SendData(&data,1);
    
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,1);
    
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,0);
        
    SPI_SendCmd(SPI_READ_STS_REG1_CMD);
    do{
        SPI_ReceiveData(&buf,1);
    }while(buf&0x1);
    
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,1);
}   
//------------------------------------------------------------------//
// Write status register(2 byte)
// Parameter: 
//          data:data to write
// return value:
//          NONE
//------------------------------------------------------------------//    
void SPI_WriteStsReg2(U32 data)
{
    U32 buf;
    
    SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);
    
    SPI_WRENCmd(ENABLE);
    
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,0);     
   
    SPI_SendCmd(SPI_WRITE_STS_REG1_CMD);
      
    SPI_SendData(&data,2);
    
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,1);
    
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,0);
        
    SPI_SendCmd(SPI_READ_STS_REG1_CMD);
    do{
        SPI_ReceiveData(&buf,1);
    }while(buf&0x1);
    
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,1);
} 
//------------------------------------------------------------------//
// read status register
// Parameter: 
//          reg : 1,2,3
// return value:
//          Status data
//------------------------------------------------------------------//      
U32 SPI_ReadStsReg(U8 reg)
{
    U32 Data;
    
    SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);
    
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,0);
    
    if(reg==1) 
    {
        SPI_SendCmd(SPI_READ_STS_REG1_CMD);
    }else if(reg==2) 
    {
        SPI_SendCmd(SPI_READ_STS_REG2_CMD);    
    }else if(reg==3) 
    {
        SPI_SendCmd(SPI_READ_STS_REG3_CMD);    
    }
    
    Data=0;
        
    SPI_ReceiveData(&Data,1);
        
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,1);
    
    return Data;
} 
//------------------------------------------------------------------//
// Set Addr Mode of XIP
// Parameter: 
//          addr_mode: C_SPI_Addr_Mode_3Byte, C_SPI_Addr_Mode_4Byte
// return value:
//          NONE
//------------------------------------------------------------------// 
void SPI_XipAddrMode(U8 addr_mode)
{
    P_SPI0->Addr_Mode = ((P_SPI0->Addr_Mode&(~C_SPI_Addr_Mode))|addr_mode);
}   
//------------------------------------------------------------------//
// Set IO Mode of XIP
// Parameter: 
//          io_mode: C_SPI_CMD_1IO, C_SPI_CMD_2IO, C_SPI_CMD_4IO
// return value:
//          NONE
//------------------------------------------------------------------// 
void SPI_XipIoMode(U8 io_mode)
{
    P_SPI0->Addr_Mode &= (~C_SPI_CMD); //3Byte Addr only
    P_SPI0->Addr_Mode |= io_mode;
}
//------------------------------------------------------------------//
// Set Bank of XIP ROM2
// Parameter: 
//          bank: xip rom2 bank, 0~15, only 3byte Address mode, IC can support more.
// return value:
//          NONE
//------------------------------------------------------------------// 
void SPI_XipRom2Bank(U8 bank)
{
    P_SPI0->Addr_Mode &= (~C_SPI_DataFlash_Bank); 
    P_SPI0->Addr_Mode |= (bank<<8);
}
//------------------------------------------------------------------//
// Get Vendor ID
// Parameter: 
//          NONE
// return value:
//          Manufacture ID
//------------------------------------------------------------------//      
U32 SPI_GetVendorID(void)
{
    U32 _Data=0;
    SPI_ReadFlashSet(SPI_READ_MFID_DEVID_CMD,&_Data,3);//Read JEDEC_ID, CMD=0x9F
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
U8	SPI_4ByteAddressEnable(const U32 _JEDEC_ID)
{
	switch(_JEDEC_ID)
	{
		case	(SPI_VENDOR_XMC		|(0x1960<<8))://(XMC)xm25QH256b
		case	(SPI_VENDOR_WINBOND	|(0x1940<<8))://(winbond)w25q256fv
			SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);
			SPI_ByteCmd(0xB7);//4Byte Address Enable CMD=0xB7
			return (SPI_Is4ByteAddressSet(_JEDEC_ID)==true)?EXIT_SUCCESS:EXIT_FAILURE;
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
U8	SPI_4ByteAddressDisable(const U32 _JEDEC_ID)
{
	switch(_JEDEC_ID)
	{
		case	(SPI_VENDOR_XMC		|(0x1960<<8))://(XMC)xm25QH256b
			SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);
			SPI_ByteCmd(0x29);//4Byte Address Disable CMD=0x29
			return (SPI_Is4ByteAddressSet(_JEDEC_ID)==false)?EXIT_SUCCESS:EXIT_FAILURE;
			break;
		case	(SPI_VENDOR_WINBOND	|(0x1940<<8))://(winbond)w25q256fv
			SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);
			SPI_ByteCmd(0xE9);//4Byte Address Disable CMD=0xE9
			return (SPI_Is4ByteAddressSet(_JEDEC_ID)==false)?EXIT_SUCCESS:EXIT_FAILURE;
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
//          true:  Is set
//          false: Not set, or ID not exit
//------------------------------------------------------------------// 
bool SPI_Is4ByteAddressSet(const U32 _JEDEC_ID)
{
	U32 _Data=0;
	switch(_JEDEC_ID)
	{
		case	(SPI_VENDOR_XMC		|(0x1960<<8))://(XMC)xm25QH256b
			SPI_ReadFlashSet(0x16,&_Data,1);//Read BankAddress REG, CMD=0x16
			return ((_Data&(0x1<<7))!=0)?true:false;//check EXTADD status(Bank Addr REG, bit7)
			break;
		case	(SPI_VENDOR_WINBOND	|(0x1940<<8))://(winbond)w25q256fv
			SPI_ReadFlashSet(0x15,&_Data,1);//Read REG-3, CMD=0x15
			return ((_Data&(0x1<<0))!=0)?true:false;//check ADS status(REG-3, bit0)
			break;
		default:
			return false;
			break;
	}
}
//------------------------------------------------------------------//
// Enable QE Bit (non-volatile)
// Parameter: 
//          _JEDEC_ID(Big endian):
//					[0:7]:MF0-MF7(Manufacturer ID)
//					[8:15]:ID8-ID15(Memory Type ID)
//					[16:23]:ID0-ID7(Capacity ID)
// return value:
//          EXIT_SUCCESS:Setting success
//          EXIT_FAILURE:Setting failure
//------------------------------------------------------------------// 
U8	SPI_QEBitEnable(const U32 _JEDEC_ID)
{
	U32 _Data=0, _Data1=0;
	switch(_JEDEC_ID)
	{
		case	(SPI_VENDOR_XMC		|(0x1960<<8))://(XMC)xm25QH256b
			SPI_ReadFlashSet(0x05,&_Data,1);//Read Status REG, CMD=0x05			
			_Data|=(0x1<<6);//Set QE bit(Status REG, bit6)
			SPI_WriteFlashSet(0x01,&_Data,1);//Write Back Status REG, CMD=0x01
		    do {
				SPI_ReadFlashSet(0x05,&_Data,1);//Read Status REG, CMD=0x05
			} while(_Data&(0x1<<0));//wait WIP bit unset(Status REG, bit0)
			return (SPI_IsQEBitSet(_JEDEC_ID)==true)?EXIT_SUCCESS:EXIT_FAILURE;
			break;
        case	(SPI_VENDOR_WINBOND	|(0x1440<<8)):
		case	(SPI_VENDOR_WINBOND	|(0x1740<<8))://(winbond)w25q64fv, w25q64jv
		case	(SPI_VENDOR_WINBOND	|(0x1840<<8))://(winbond)w25q128fv
		case	(SPI_VENDOR_WINBOND	|(0x1940<<8))://(winbond)w25q256fv
			/***************************************************************************
			***Winbond's previous generation need continue write REG-1 & REG-2 by 01h***
			***************************************************************************/
			SPI_ReadFlashSet(0x05,&_Data,1);//Read REG-1, CMD=0x05
			SPI_ReadFlashSet(0x35,&_Data1,1);//Read REG-2, CMD=0x35
			_Data=_Data+(_Data1<<8);//Combine REG-1 and REG-2 to 16bit
			_Data|=(0x1<<9);//Set QE bit(REG-S9, bit9)
			SPI_WriteFlashSet(0x01,&_Data,2);//Write Back REG-1&REG-2, CMD=0x01
			do {
				SPI_ReadFlashSet(0x05,&_Data,1);//Read REG-1, CMD=0x05
			} while(_Data&(0x1<<0));//wait BUSY bit unset(REG-1, bit0)
			return (SPI_IsQEBitSet(_JEDEC_ID)==true)?EXIT_SUCCESS:EXIT_FAILURE;
			break;
		default:
			return EXIT_FAILURE;
			break;
	}
}
//------------------------------------------------------------------//
// Disable QE Bit (non-volatile)
// Parameter: 
//          _JEDEC_ID(Big endian):
//					[0:7]:MF0-MF7(Manufacturer ID)
//					[8:15]:ID8-ID15(Memory Type ID)
//					[16:23]:ID0-ID7(Capacity ID)
// return value:
//          EXIT_SUCCESS:Setting success
//          EXIT_FAILURE:Setting failure
//------------------------------------------------------------------// 
U8	SPI_QEBitDisable(const U32 _JEDEC_ID)
{
	U32 _Data=0, _Data1=0;
	switch(_JEDEC_ID)
	{
		case	(SPI_VENDOR_XMC		|(0x1960<<8))://(XMC)xm25QH256b
			SPI_ReadFlashSet(0x05,&_Data,1);//Read Status REG, CMD=0x05
			_Data&=~(0x1<<6);//Unset QE bit(Status REG, bit6)
			SPI_WriteFlashSet(0x01,&_Data,1);//Write Back Status REG, CMD=0x01
			do {
				SPI_ReadFlashSet(0x05,&_Data,1);//Read Status REG, CMD=0x05
			} while(_Data&(0x1<<0));//wait WIP bit unset(Status REG, bit0)
			return (SPI_IsQEBitSet(_JEDEC_ID)==false)?EXIT_SUCCESS:EXIT_FAILURE;
			break;
		case	(SPI_VENDOR_WINBOND	|(0x1440<<8)):
        case	(SPI_VENDOR_WINBOND	|(0x1740<<8))://(winbond)w25q64fv, w25q64jv
		case	(SPI_VENDOR_WINBOND	|(0x1840<<8))://(winbond)w25q128fv
		case	(SPI_VENDOR_WINBOND	|(0x1940<<8))://(winbond)w25q256fv
			/***************************************************************************
			***Winbond's previous generation need continue write REG-1 & REG-2 by 01h***
			***************************************************************************/
			SPI_ReadFlashSet(0x05,&_Data,1);//Read REG-1, CMD=0x05
			SPI_ReadFlashSet(0x35,&_Data1,1);//Read REG-2, CMD=0x35
			_Data=_Data+(_Data1<<8);//Combine REG-1 and REG-2 to 16bit
			_Data&=~(0x1<<9);//Unset QE bit(REG-S9, bit9)
			SPI_WriteFlashSet(0x01,&_Data,2);//Write Back REG-1&REG-2, CMD=0x01
			do {
				SPI_ReadFlashSet(0x05,&_Data,1);//Read REG-1, CMD=0x05
			} while(_Data&(0x1<<0));//wait BUSY bit unset(REG-1, bit0)
			return (SPI_IsQEBitSet(_JEDEC_ID)==false)?EXIT_SUCCESS:EXIT_FAILURE;
			break;
		default:
			return EXIT_FAILURE;
			break;
	}
}
//------------------------------------------------------------------//
// Is QE Bit Set?
// Parameter: 
//          _JEDEC_ID(Big endian):
//					[0:7]:MF0-MF7(Manufacturer ID)
//					[8:15]:ID8-ID15(Memory Type ID)
//					[16:23]:ID0-ID7(Capacity ID)
// return value:
//          true:  Is set
//          false: Not set, or ID not exit
//------------------------------------------------------------------// 
bool SPI_IsQEBitSet(const U32 _JEDEC_ID)
{
	U32 _Data=0;
	switch(_JEDEC_ID)
	{
		case	(SPI_VENDOR_XMC		|(0x1960<<8))://(XMC)xm25QH256b
			SPI_ReadFlashSet(0x05,&_Data,1);//Read Status REG, CMD=0x05
			return ((_Data&(0x1<<6))!=0)?true:false;//check QE status(Status REG, bit6)
			break;
		case	(SPI_VENDOR_WINBOND	|(0x1440<<8)):
        case	(SPI_VENDOR_WINBOND	|(0x1740<<8))://(winbond)w25q64fv, w25q64jv
		case	(SPI_VENDOR_WINBOND	|(0x1840<<8))://(winbond)w25q128fv
		case	(SPI_VENDOR_WINBOND	|(0x1940<<8))://(winbond)w25q256fv
			SPI_ReadFlashSet(0x35,&_Data,1);//Read REG-2, CMD=0x35
			return ((_Data&(0x1<<1))!=0)?true:false;//check QE status(REG-2, bit1)
			break;
		default:
			return false;
			break;
	}
}

//------------------------------------------------------------------//
// Write status register for Q-Code
// Parameter:
//          reg : 0x1,0x31,0x11
//          data:data to write
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI_WRSR(U8 reg,U32 data, U8 len)
{
    SPI_WriteFlashSet(reg,&data,len);
    while(SPI_CheckBusyFlag());
}
//------------------------------------------------------------------//
// read status register for Q-Code
// Parameter:
//          reg : 0x5,0x35,0x15
// return value:
//          Status data
//------------------------------------------------------------------//
U32 SPI_RDSR(U8 reg)
{
    U32 Data=0;
    SPI_ReadFlashSet(reg, &Data, 1);
    return Data;
}
#if _SPI_MODULE
//------------------------------------------------------------------//
// Set CS pin Lo
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI_CS_On(void)
{
    SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,0);
}
//------------------------------------------------------------------//
// Set CS pin Hi
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI_CS_Off(void)
{
    GPIO_Write(SPI0_CS_PORT,SPI0_CS_PIN,1);
}
//------------------------------------------------------------------//
// Is SPI Bus Busy(Check CS Pin)
// Parameter: 
//          NONE
// return value:
//          true / false
//------------------------------------------------------------------//
bool SPI_isBusBusy(void)
{
    return ((SPI0_CS_PORT->Data>>SPI0_CS_PIN)&0x1)==0;
}
//------------------------------------------------------------------//
// Send Data, Set 1 Byte.
// Parameter: 
//          _data: 1 Byte
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI_TX(U32 _data)
{
    SPI_SendData(&_data,1);
}
//------------------------------------------------------------------//
// Read Data, Get 1 Byte.
// Parameter: 
//          NONE
// return value:
//          Data(1 Byte)
//------------------------------------------------------------------//
U32  SPI_RX(void)
{
    U32 _data=0;
    SPI_ReceiveData(&_data,1);
    return _data;
}
//------------------------------------------------------------------//
// Start Page Program (cmd = 0x02)
// Parameter: 
//          _addr:Program Address
//          _data:1st Byte Data.
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI_WRD(U32 _addr, U32 _data)
{
    SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);
    SPI_WRENCmd(ENABLE);
    SPI_CS_On();
    SPI_SendCmdAddr(SPI_PAGE_PRG_CMD, _addr, 0);
    SPI_TX(_data);
}
//------------------------------------------------------------------//
// Start Read Data (cmd = 0x03)
// Parameter: 
//          _addr:Read Address
// return value:
//          1st Byte Data.
//------------------------------------------------------------------//
U32  SPI_RDD(U32 _addr)
{
    SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);
    SPI_CS_On();
    SPI_SendCmdAddr(SPI_READ_CMD, _addr, 0);
    return SPI_RX();
}
//------------------------------------------------------------------//
// Write Enable (cmd = 0x06)
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI_WREN(void)
{
    SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);
    SPI_WRENCmd(ENABLE);
}
//------------------------------------------------------------------//
// Write Disable (cmd = 0x04)
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI_WRDIS(void)
{
    SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);
    SPI_WRENCmd(DISABLE);
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
U32  SPI_RDID(void)
{
    U32 _data = SPI_GetVendorID();
    return __builtin_bswap32(_data)>>8;
}
//------------------------------------------------------------------//
// Chip Erase (cmd = 0xC7)
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI_CE(void)
{
    SPI_StartChipErase();
}
//------------------------------------------------------------------//
// Sector Erase (cmd = 0x20)
// Parameter: 
//          _addr: Sector Erase start address
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI_SE(U32 _addr)
{
    SPI_StartSectorErase(_addr);
}
//------------------------------------------------------------------//
// Block Erase (cmd = 0xD8)
// Parameter: 
//          _addr: Block Erase start address
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI_BE(U32 _addr)
{
    SPI_StartBlockErase(_addr);
}
//------------------------------------------------------------------//
// Deep Power-Down (cmd = 0xB9)
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI_DP(void)
{
    SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);
    SPI_ByteCmd(SPI_POWER_DOWN_CMD);
}
//------------------------------------------------------------------//
// Release Deep Power-down / Get ID(cmd = 0xAB)
// Parameter: 
//          NONE
// return value:
//          Flash chip ID(8-Bits)
//------------------------------------------------------------------//
U32  SPI_RDP(void)
{
    SPI_SetBusMode(SPI_DATA_1,SPI_DATA_1,SPI_DATA_1);
    SPI_CS_On();
    SPI_TX(SPI_RELEASE_POWER_DOWN_CMD);
    SPI_RX(); //dummy
    SPI_RX(); //dummy
    SPI_RX(); //dummy
    U32 _data = SPI_RX();
    SPI_CS_Off();
    return _data;
}
//------------------------------------------------------------------//
// Get User Defined Section address (support by SPI Encoder)
// Parameter: 
//          _index:0~65535
// return value:
//          address of UDR
//------------------------------------------------------------------//
U32  SPI_GetAddr(U16 _index)
{
    return UserDefinedData_GetAddressUseType(_index, kUD_USER_DEFINED, SPI_MODE);
}
#endif

