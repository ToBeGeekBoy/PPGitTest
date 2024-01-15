/* Includes ------------------------------------------------------------------*/

#include "include.h"
#include "nx1_spi.h"
#include "nx1_fmc.h"
#include "VoiceTab.h"
#include <string.h>
#if _AUDIO_MODULE==1 || _STORAGE_MODULE==1

//------------------------------------------------------------------//
// Read data from storage (called by lib)
// Parameter: 
//          mode     : storage mode
//          *cur_addr: current address
//          *buf     : data buffer
//          size     : data size to read(bytes)
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_ReadData(U8 mode, U32 *cur_addr, U8 *buf,U16 size){
    switch(mode) {
        case    OTP_MODE:
#if _EF_SERIES
        case    EF_UD_MODE:
#endif
            memcpy(buf, (void*)(*cur_addr), size);
            break;
#if _SPI_MODULE
        case    SPI_MODE:
            SPI_BurstRead(buf,*cur_addr,size);
            break;
#endif
#if _SPI1_MODULE && _SPI1_USE_FLASH
        case    SPI1_MODE:
            SPI1_BurstRead(buf,*cur_addr,size);
            break;
#endif
        default:
            break;
    }
    *cur_addr+=size;
}    
//------------------------------------------------------------------//
// Erase data to storage
// Parameter: 
//          mode    : storage mode
//          addr    : current address
//          size    : align 4k/64k/512bytes.
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_EraseData(U8 mode, U32 addr,U32 size){
    
#if _SPI_MODULE    
    if(mode==SPI_MODE){
        SPI_BurstErase_Sector(addr,size);
    }        
#endif
#if _SPI1_MODULE && _SPI1_USE_FLASH
	if(mode==SPI1_MODE){
        SPI1_BurstErase_Sector(addr,size);
	}		
#endif
#if _EF_SERIES
	if(mode==EF_MODE){
        FMC_BurstErase_Sector(addr,size);
	}
#endif
}
//------------------------------------------------------------------//
// Write data to storage (called by lib)
// Parameter: 
//          mode     : storage mode
//          *cur_addr: current address
//          *buf     : data buffer
//          size     : data size to read(bytes)
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_WriteData(U8 mode, U32 *cur_addr,U8 *buf,U16 size){
    
    if(mode==SPI_MODE){
  
        SPI_BurstWrite(buf,*cur_addr,size);

    }        
#if _SPI1_MODULE && _SPI1_USE_FLASH
	else if(mode==SPI1_MODE){
		SPI1_BurstWrite(buf,*cur_addr,size);
	}		
#endif
#if _EF_SERIES
	else if(mode==EF_MODE){
		FMC_BurstWrite(buf,*cur_addr,size);
	}
#endif
    *cur_addr+=size;
}    
//------------------------------------------------------------------//
// Write header to storage (called by lib)
// Parameter: 
//          mode     : storage mode
//          *cur_addr: current address
//          *buf     : data buffer
//          size     : data size to read(bytes)
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_WriteHeader(U8 mode, U32 *cur_addr,U8 *buf,U16 size){
    
    if(mode==SPI_MODE){
        SPI_BurstWrite(buf,*cur_addr,size); 
    }     
#if _SPI1_MODULE && _SPI1_USE_FLASH
	else if(mode==SPI1_MODE){
		SPI1_BurstWrite(buf,*cur_addr,size);
	}		
#endif
#if _EF_SERIES
	else if(mode==EF_MODE){
		FMC_BurstWrite(buf,*cur_addr,size);
	}
#endif
}    
#if _AUDIO_MODULE==1
//------------------------------------------------------------------//
// Get file addr from index
// Parameter: 
//          index: file index or absolute address
//          mode:OTP_MODE,EF_MODE,SPI_MODE,SPI1_MODE
// return value:
//          address
//------------------------------------------------------------------// 
static U32 GetStartAddr(U32 index,U8 mode){
    U32 addr=0;
    switch(mode) {
        case    OTP_MODE:
            if(index&ADDRESS_MODE) {
                addr=index&(~ADDRESS_MODE);
            } else {
                addr=PlayList[index];
            }
            break;
#if _EF_SERIES
        case    EF_UD_MODE:
            if(index&ADDRESS_MODE) {
                addr=index&(~ADDRESS_MODE);
            } else {
                addr=UserDefinedData_GetAddressUseType(index,kUD_VOICE,EF_UD_MODE);
            }
            break;
#endif
#if _SPI_MODULE
        case    SPI_MODE:
            if(index&ADDRESS_MODE) {
                addr=index&(~ADDRESS_MODE);
            } else {
                U8 tempbuf[SPI_TAB_INFO_ENTRY_SIZE];
                addr=SpiOffset();
                SPI_BurstRead(tempbuf,addr+SPI_BASE_INFO_SIZE+(SPI_TAB_INFO_ENTRY_SIZE*index),SPI_TAB_INFO_ENTRY_SIZE);
                addr=addr+(tempbuf[0]|(tempbuf[1]<<8)|(tempbuf[2]<<16)|(tempbuf[3]<<24));
                addr&=(~SPI_DATA_TYPE_MASK);
            }
            break;
#endif
#if _SPI1_MODULE && _SPI1_USE_FLASH
        case    SPI1_MODE:
            if(index&ADDRESS_MODE) {
                addr=index&(~ADDRESS_MODE);
            } else {
                U8 tempbuf[SPI_TAB_INFO_ENTRY_SIZE];
                addr=Spi1_Offset();
                SPI1_BurstRead(tempbuf,addr+SPI_BASE_INFO_SIZE+(SPI_TAB_INFO_ENTRY_SIZE*index),SPI_TAB_INFO_ENTRY_SIZE);
                addr=addr+(tempbuf[0]|(tempbuf[1]<<8)|(tempbuf[2]<<16)|(tempbuf[3]<<24));
                addr&=(~SPI_DATA_TYPE_MASK);
            }
            break;
#endif
        default:
            break;
    }
    return addr;
}  
//------------------------------------------------------------------//
// Set start address of file(called by lib)
// Parameter: 
//          addr       : file index or absolute address
//          mode       : OTP_MODE,EF_MODE,SPI_MODE,SPI1_MODE,EF_UD_MODE
//          rw         : 1:write, 0:read
//          *start_addr: current address
// return value:
//          0: file index ok
//          others: file index fail
//------------------------------------------------------------------// 
U8 CB_SetStartAddr(U32 addr,U8 mode,U8 rw,U32 *start_addr){
    if(!(addr&ADDRESS_MODE)) {  //index mode check
        switch(mode) {
            case    OTP_MODE:
                if(addr >= VOICE_FILE_NUMBER) return 1;
                break;
#if _EF_SERIES
            case    EF_UD_MODE:
                break;//no check
#endif
#if _SPI_MODULE
            case    SPI_MODE:
                if(addr >= AUDIO_GetSpiFileCount()) return 1;
                break;
#endif
#if _SPI1_MODULE && _SPI1_USE_FLASH
            case    SPI1_MODE:
                if(addr >= AUDIO_GetSpi1FileCount()) return 1;
                break;
#endif
            default:
                return 1;//unknown storage
        }
    }
    *start_addr = GetStartAddr(addr,mode); 
	return (!(*start_addr))?1:0;
}     
//------------------------------------------------------------------//
// Get start address
// Parameter: 
//          *index:midi file index
//          *addr:Timbre start address
//          mode:SPI_MODE,SPI1_MODE
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_Midi_GetStartAddr(U32 *index,U32 *addr,U8 mode){
    U8 temp_buf[SPI_TAB_INFO_ENTRY_SIZE];
    U32 offset=0;

    if(mode==SPI_MODE){
		offset=SpiOffset();
		
		if((*index)&ADDRESS_MODE){
			(*index)=(*index)&(~ADDRESS_MODE);
		}else{
			SPI_BurstRead(temp_buf,offset+SPI_BASE_INFO_SIZE+(SPI_TAB_INFO_ENTRY_SIZE*(*index)),SPI_TAB_INFO_ENTRY_SIZE);   
			*index=offset+(temp_buf[0]|(temp_buf[1]<<8)|(temp_buf[2]<<16)|(temp_buf[3]<<24));
			(*index)&=(~SPI_DATA_TYPE_MASK);
		}	
		
		SPI_BurstRead(temp_buf,offset+SPI_TIMBRE_ADDR_OFFSET,4);
	}		
 #if _SPI1_MODULE && _SPI1_USE_FLASH
    else if(mode==SPI1_MODE){
	    offset=Spi1_Offset();
	
        if((*index)&ADDRESS_MODE){
            (*index)=(*index)&(~ADDRESS_MODE);
        }else{
            SPI1_BurstRead(temp_buf,offset+SPI_BASE_INFO_SIZE+(SPI_TAB_INFO_ENTRY_SIZE*(*index)),SPI_TAB_INFO_ENTRY_SIZE);   
            *index=offset+(temp_buf[0]|(temp_buf[1]<<8)|(temp_buf[2]<<16)|(temp_buf[3]<<24));
            (*index)&=(~SPI_DATA_TYPE_MASK);
        }		
	    SPI1_BurstRead(temp_buf,offset+SPI_TIMBRE_ADDR_OFFSET,4);        
    }
 #endif	
	
	*addr=offset+(temp_buf[0]|(temp_buf[1]<<8)|(temp_buf[2]<<16)|(temp_buf[3]<<24));	
}   
#endif
//------------------------------------------------------------------//
// Get User-Defined Section Address by storage mode
// Parameter: 
//          mode : EF_MODE,SPI_MODE,SPI1_MODE
//          idx: resource index, it is from 0.
// return value:
//          return: the address of resource count
//------------------------------------------------------------------//
U32 CB_GetUserDefinedAddress(U8 mode, U16 idx)
{
    U32 addr=0;
    
#if _SPI_MODULE    
    if(mode==SPI_MODE) {
        addr = RESOURCE_GetAddress(idx);
    }
#endif
#if _SPI1_USE_FLASH && _SPI1_MODULE
    if(mode==SPI1_MODE) {
        addr = RESOURCE1_GetAddress(idx);
    }
#endif
#if _EF_SERIES
    if(mode==EF_MODE) {
        addr = RESOURCE_EF_GetAddress(idx);
    }
#endif
    return addr;
}
//------------------------------------------------------------------//
// Seek from start address(called by lib)
// Parameter: 
//          offset: seek offset
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_Seek(U32 offset){  
   
}
#endif
//------------------------------------------------------------------//
// SPI_Encoder MARK CallBack function
// Parameter: 
//          ch: active channel
//			markNum: mark number 1~255 that is set up by SPI_Encoder
//			index: SPI_Encoder file index / sequence of the MARK on the sentence order
// return value:
//          NONE
//------------------------------------------------------------------//  
void CB_SPI_MARK(U8 ch, U8 markNum, U8 index){
#if _AUDIO_SENTENCE   	
	SentenceCB[ch] = 0;
#endif

}
