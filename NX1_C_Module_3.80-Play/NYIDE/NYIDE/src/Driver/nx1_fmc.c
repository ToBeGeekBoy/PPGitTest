#include "nx1_fmc.h"
#include "nx1_wdt.h"
#include "include.h"
#include <string.h>
#include "debug.h"
#include "nx1_gpio.h"

#if     defined(P_FMC)
#include "UserDefinedTab.h"
//------------------------------------------------------------------//
// FMC Init
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void FMC_Init(viud)
{
    P_SMU->FUNC_Ctrl |= C_Func_FMC_En;
}
//------------------------------------------------------------------//
// Erase FMC, erase unit is 512 bytes, sector erase
// Parameter: 
//          addr:start address
//          len:legth to erase(bytes)
// return value:
//          NONE
//------------------------------------------------------------------//    
void FMC_BurstErase_Sector(U32 addr, U32 len)
{
    if(!len) return;
    if((addr&(~0x1FF)) <= (U32)UPDATEABLE_RESOURCES_DATA) return;
    FMC_Init();
    P_FMC->Ctrl = MAIN_FLASH_SECTORERASE;
    
    do {
        U32 erase_len = (len > MAIN_FLASH_SECTORSIZE)?MAIN_FLASH_SECTORSIZE:len;
        
        P_FMC->Addr = addr;
        P_FMC->Data = DATA_TRIGGER; //  TRIGGER
        while (!(P_FMC->Flag & C_FMC_Finish_Flag));
        P_FMC->Flag = 1;
        WDT_Clear();
        
        len -= erase_len;
        addr += erase_len;
    } while(len>0);
}
//------------------------------------------------------------------//
// Burst read from FMC
// Parameter: 
//          buf  : data buffer
//          addr : start address
//          len  : length to read(bytes)
// return value:
//          NONE
//------------------------------------------------------------------//    
void FMC_BurstRead(U8 *buf, U32 addr, U16 len)
{
    FMC_Init();
    memcpy(buf,(void*)addr,len);
}
//------------------------------------------------------------------//
// Burst write to FMC
// Parameter: 
//          buf:data buffer
//          addr:start address
//          len:legth to write(bytes)
// return value:
//          NONE
//------------------------------------------------------------------//    
void FMC_BurstWrite(U8 *buf, U32 addr, U16 len)
{
    if(!buf || !len) return;
    if(addr <= (U32)UPDATEABLE_RESOURCES_DATA) return;
    FMC_Init();
    P_FMC->Ctrl = MAIN_FLASH_WORDPROGRAM;
    
    U8 base = addr&(DATA_ALIGN-1); //0~3
    U8 copyLen = DATA_ALIGN-base; //4~1
    copyLen = copyLen>len?len:copyLen;
    U32 temp = -1;
    U8* pTemp = ((U8*)&temp)+base;
    do {
        U8 i = copyLen;
        do {
            *pTemp++ = *buf++;
        } while(--i);
        
        P_FMC->Addr = (addr);
        P_FMC->Data = (temp);
        while (!(P_FMC->Flag & C_FMC_Finish_Flag));
        P_FMC->Flag = C_FMC_Finish_Flag; //W1C
        
        addr += copyLen;
        len -= copyLen;
        
        copyLen = len>DATA_ALIGN?DATA_ALIGN:len;
        temp = -1;
        pTemp = ((U8*)&temp);
    } while(len>0);
}
#endif	//#if	defined(P_FMC)