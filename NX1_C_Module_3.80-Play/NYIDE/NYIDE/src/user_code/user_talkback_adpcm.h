/*******************************************************************************
* File Name        : user_talkback_adpcm.h
* Copyright        : 2018 LGW Corporation, All Rights Reserved.
* Module Name      : talk back
* Author           : Pillar Peng
*******************************************************************************/

#ifndef _USER_TALKBACK_ADPCM_H_
#define _USER_TALKBACK_ADPCM_H_

#include "user_main.h"

//----------------------------------------------------------------------------//
// 模块使能宏定义
//----------------------------------------------------------------------------//
#define USER_TALKBACK_MODULE					(DISABLE)						// 模块使能

#if USER_TALKBACK_MODULE

//----------------------------------------------------------------------------//
// 时间基准
//----------------------------------------------------------------------------//
#define TALKBACK_TIMER_BASE                      (1)                            // MS
#define TALKBACK_TIMER_CNT_MS(x)                 ((x)/TALKBACK_TIMER_BASE)      // MS
#define TALKBACK_TIMER_CNT_S(x)                  ((x)*1000/TALKBACK_TIMER_BASE) // S


typedef struct TALK_BACK_
{
	u8 cmd;
	u8 update_flag;
	u8 step;
	u32 timer_cnt;
	u8 detect_sound_flag;
}TALK_BACK_Type;


#define USER_TALKBACK_STORE_MODE                (_SPI_MODULE?SPI_MODE:EF_MODE)

#define USER_TALK_BACK_DELAY_TIME               (TALKBACK_TIMER_CNT_MS(100))    // 录音前的延时时间
#define USER_TALK_BACK_MIN_TIME                 (TALKBACK_TIMER_CNT_MS(200))    // 录音最小时间长度
#define USER_TALK_BACK_MAX_TIME                 (TALKBACK_TIMER_CNT_MS(5000))   // 录音最大时间长度
#define USER_TALK_BACK_MAX_LENGTH               (SPI_BLOCK_SIZE)                // 录音最大长度

#if USER_TALKBACK_STORE_MODE == SPI_MODE
    #define USER_TALK_BACK_START_ADDRESS        (RESOURCE_GetAddress(1))	    // 录音起始地址
#else
    #define USER_TALK_BACK_START_ADDRESS        (RESOURCE_EF_GetAddress(1))	    // 录音起始地址
#endif

#define USER_TALK_BACK_SKIP_TAIL_TIME           (100)                           // MS   去除尾音时长

#define	USER_USER_TALK_BACK_PITCH		        (12)                            // 变调比例，0:为不变调，>0:音调变高，<0:音调变低，声音更沉
#define	USER_USER_TALK_BACK_PP_GAIN		        (_PP_GAIN)                      // 播放 talk back 时的 PP gain 值，也就是音量。 只在 PP 模式下有效

//----------------------------------------------------------------------------//
// 指示灯
//----------------------------------------------------------------------------//
#define USER_TALK_BACK_HINT_EN                  (DISABLE)

#if USER_TALK_BACK_HINT_EN
    #define USER_TALK_BACK_LED_PORT             (GPIOB)
    #define USER_TALK_BACK_LED_PIN              (15)
    #define USER_TALK_BACK_LED_ON()             GPIO_Init(USER_TALK_BACK_LED_PORT, (USER_TALK_BACK_LED_PIN), GPIO_MODE_OUT_HI)
    #define USER_TALK_BACK_LED_OFF()            GPIO_Init(USER_TALK_BACK_LED_PORT, (USER_TALK_BACK_LED_PIN), GPIO_MODE_OUT_LOW)
#endif  // endof #if USER_TALK_BACK_HINT_EN

//----------------------------------------------------------------------------//
// 提示音
//----------------------------------------------------------------------------//
#define USER_TALK_BACK_HINT_START               (AUD_DI)
#define USER_TALK_BACK_HINT_END                 (AUD_DI)

extern TALK_BACK_Type gst_user_talk_back;

u8 user_talk_back_get_cmd(void);
void user_talk_back_set_cmd(u8 cmd);
void ADPCM_StopRecordErase(void);
void user_talk_back_start(void);
void user_talk_back_end(void);
u8 user_talk_back_get_status(void);

void user_talk_back_process(void);
void user_play_stop_all(void);

#endif // endof #if USER_TALKBACK_MODULE

#endif //end of _USER_TALKBACK_ADPCM_H_