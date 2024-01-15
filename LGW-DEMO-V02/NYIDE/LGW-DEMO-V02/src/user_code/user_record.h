/*******************************************************************************
* File Name     		:	user_record.h
* Copyright				:	2017 LGW Corporation, All Rights Reserved.
* Module Name			:	record
* Author	    		:	Pillar Peng
*******************************************************************************/
#ifndef _USER_RECORD_H_
#define _USER_RECORD_H_

#include "user_main.h"
#include "nx1_spi.h"

//----------------------------------------------------------------------------//
// 模块使能宏定义
//----------------------------------------------------------------------------//
#define USER_RECORD_MODULE					(DISABLE)				// 模块使能

#if USER_RECORD_MODULE

//----------------------------------------------------------------------------//
// 时间基准
//----------------------------------------------------------------------------//
#define USER_RECORD_TIMER_BASE              (1)                                 // MS
#define USER_RECORD_TIMER_CNT_MS(x)         ((x)/USER_RECORD_TIMER_BASE)        // MS
#define USER_RECORD_TIMER_CNT_S(x)          ((x)*1000/USER_RECORD_TIMER_BASE)   // S

// 命令
#define USER_RECORD_CMD_DISABLE	            (0)
#define USER_RECORD_CMD_ENABLE	            (1)
#define USER_RECORD_CMD_PLAY		        (2)

#define USER_RECORD_STORE_MODE              (_SPI_MODULE?SPI_MODE:EF_MODE)

#define USER_RECORD_SIZE         		    (SPI_BLOCK_SIZE)                    // 录音大小
#if _SPI_MODULE
    #define USER_RECORD_START_ADDRESS	    ((u32)RESOURCE_GetAddress((lst_user_record.record_num)))
    #define USER_RECORD_START_ADDRESS_0		((u32)RESOURCE_GetAddress(0))
#else
    #define USER_RECORD_START_ADDRESS	    ((u32)RESOURCE_EF_GetAddress((lst_user_record.record_num)))
    #define USER_RECORD_START_ADDRESS_0		((u32)RESOURCE_EF_GetAddress(0))
#endif

#define USER_RECORD_MAX_TIME                (USER_RECORD_TIMER_CNT_S(4))
#define USER_RECORD_SKIP_TAIL_TIME          (100)                               // MS   去除尾音时长

// 录音段落数量
#define USER_RECORD_MAX_SEGMENT_NUM         (1)

#define	USER_RECORD_PITCH		            (0)                                 // 变调比例，0:为不变调，>0:音调变高，<0:音调变低，声音更沉
#define	USER_RECORD_PP_GAIN		            (_PP_GAIN)                          // 播放 录音 时的 PP gain 值，也就是音量。 只在 PP 模式下有效

//----------------------------------------------------------------------------//
// 提示音
//----------------------------------------------------------------------------//
#define USER_RECORD_HINT_START              (AUD_DI)
#define USER_RECORD_HINT_END                (AUD_DI)

extern void ADPCM_StartRecordErase(U32 address, U32 length, U16 chan);
extern void ADPCM_StopRecordErase(void);

void user_record(void);
void user_record_set_cmd(u8 cmd, u8 num);
u8 user_record_get_cmd(void);
void user_record_process(void);

u32 user_record_get_start_addr(u8 num);
u8 user_record_get_current_num(void);

#endif //end of #if USER_RECORD_MODULE

#endif //end of _USER_RECORD_H_
