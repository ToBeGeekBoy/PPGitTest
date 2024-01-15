#include <string.h>
#include "include.h"
#include "nx1_spi.h"
#include "debug.h"
#if _STORAGE_MODULE

#if _STORAGE_USER_DEF_AMOUNT < 2
#error "_STORAGE_USER_DEF_AMOUNT must larger or equal to 2!"
#endif

//#if _STORAGE_NONBLOCK_ERASE && (_STORAGE_MODE!=SPI_MODE)
#if _SBC_AUTO_SAVE_TIME && (_STORAGE_MODE!=SPI_MODE)
#error "_SBC_AUTO_SAVE_TIME only support SPI_MODE!"
#endif

#define _STORAGE_BLOCK_SIZE (_STORAGE_BUF_SIZE + _STORAGE_VAR_CNT*4+4)

#define C_STORAGE_NONBLOCK_IDLE      0
#define C_STORAGE_NONBLOCK_ERASE     1

#define Dprintf(...)// dprintf
//#define Dprintf     dprintf

static U32 var_addr[_STORAGE_VAR_CNT];
static STORAGE_T    Storage_1;
static STORAGE_NONBLOCK_ERASE_T StorageNonBlockErase_1;

//----------------------------------------------------------------------------//
// Storage Module init
// Parameter: 
//          mode        : EF_MODE,SPI_MODE,SPI1_MODE
//          index       : file index or absolute address
// return value:
//			None
//----------------------------------------------------------------------------//
void STORAGE_Init(U8 mode, U32 index)
{
    Storage_1.var_cnt    = _STORAGE_VAR_CNT;
    Storage_1.block_size = _STORAGE_BLOCK_SIZE;

    if(mode==SPI_MODE || mode== SPI1_MODE)
    {
        Storage_1.erase_size = 4096;
    }
    else if(mode==EF_MODE) 
    {
        Storage_1.erase_size = 512; 
    }
    Storage_1.pVar_table = var_addr;
    STORAGE_LibRecovery(&Storage_1, index, _STORAGE_USER_DEF_AMOUNT, mode);
#if _SBC_AUTO_SAVE_TIME//_STORAGE_NONBLOCK_ERASE
    StorageNonBlockErase_1.state = C_STORAGE_NONBLOCK_IDLE;
    Dprintf("\r\n\r\nRecovery cur 0x%x bsize %d\r\n",Storage_1.cur_addr,Storage_1.block_size);
#endif
}
//----------------------------------------------------------------------------//
// Storage set variables
// Parameter: 
//          var : the address of var
//          len : the length of variable. The unit is bytes.
// return value:
//          0		: access pass
//			-1		: access fail
//----------------------------------------------------------------------------//
S8 STORAGE_SetVar(U8 *var, U8 len)
{
    return(STORAGE_LibSetVar(&Storage_1, var, len));
}
//----------------------------------------------------------------------------//
// Storage Module refresh
// Parameter: 
//          mode        : EF_MODE,SPI_MODE,SPI1_MODE
// return value:
//			ret: -1 is fail. 0 is pass
//----------------------------------------------------------------------------//
S8 STORAGE_Save(U8 mode)
{
#if _SBC_AUTO_SAVE_TIME//_STORAGE_NONBLOCK_ERASE
    U32 cur_sector, end_sector, next_sector, old_erase_addr;

    if(StorageNonBlockErase_1.state == C_STORAGE_NONBLOCK_ERASE)
    {
        // Erasing is doing.
        Dprintf("Erasing is doing\r\n");
        return -1;
    }

    STORAGE_LibSave2(&Storage_1, mode);

    // Non Block Erasing Flow
    cur_sector = Storage_1.cur_addr & (~(U32)(Storage_1.erase_size-1));
    end_sector = (Storage_1.cur_addr+Storage_1.block_size) & (~(U32)(Storage_1.erase_size-1));

    if(cur_sector != end_sector)
    {
        // Don't erase
        return 0;
    }

    next_sector = cur_sector+Storage_1.erase_size;
    if( next_sector >= Storage_1.end_addr )
    {
       next_sector = Storage_1.start_addr;
    }

    if(StorageNonBlockErase_1.erased_addr != next_sector)
    {
        // erase next sector
        old_erase_addr = StorageNonBlockErase_1.erased_addr;
        StorageNonBlockErase_1.erased_addr = next_sector;
        StorageNonBlockErase_1.state = C_STORAGE_NONBLOCK_ERASE;
        // start erase
        Dprintf("Storage Erase addr 0x%x\r\n",StorageNonBlockErase_1.erased_addr);
        Dprintf("cur 0x%x bsize %d\r\n",Storage_1.cur_addr,Storage_1.block_size);

        if(SBC_GetStatus()==STATUS_PLAYING)
        {
            if(SbcRamEBufCtrl_SetEraseSt()) //C_FLASH_ERASE_START      1
            {
                Dprintf("SbcRamEBufCtrl_SetEraseSt fail !!\r\n");
                StorageNonBlockErase_1.state = C_STORAGE_NONBLOCK_IDLE;
                StorageNonBlockErase_1.erased_addr = old_erase_addr;
            }
        }
        else
        {
            //Burst Erase 
            Dprintf("Block Erase\r\n");
            SPI_BurstErase_Sector(StorageNonBlockErase_1.erased_addr, Storage_1.erase_size);
            StorageNonBlockErase_1.state = C_STORAGE_NONBLOCK_IDLE;
        }
        
    }
    return 0;
#else
    STORAGE_LibSave(&Storage_1, mode);
    return 0;
#endif

}
//----------------------------------------------------------------------------//
// Set non block erasing done by erasing engine
// Parameter: 
//			None
// return value:
//			None
//----------------------------------------------------------------------------//
void STORAGE_NonBlockSectorErase_Init(void)
{
    memset((void *)&StorageNonBlockErase_1,0,sizeof(StorageNonBlockErase_1));
}
//----------------------------------------------------------------------------//
// Get storage non blocking erasing address, when erase will be executed.
// Parameter: 
//			None
// return value:
//			-1 : fail
//----------------------------------------------------------------------------//
S32 STORAGE_GetNonBlockSectorAddr(void)
{
    if(StorageNonBlockErase_1.state == C_STORAGE_NONBLOCK_ERASE)
    {
        return StorageNonBlockErase_1.erased_addr;
    }
    else
    {
        return -1;
    }
}
//----------------------------------------------------------------------------//
// Set non block erasing done by erasing engine
// Parameter: 
//			None
// return value:
//			None
//----------------------------------------------------------------------------//
void STORAGE_NonBlockSectorEraseDone(void)
{
    StorageNonBlockErase_1.state = C_STORAGE_NONBLOCK_IDLE;
}

#endif
