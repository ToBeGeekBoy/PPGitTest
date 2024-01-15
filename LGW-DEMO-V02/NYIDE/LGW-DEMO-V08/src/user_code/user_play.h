/*******************************************************************************
* File Name     		:	audio_play.h
* Copyright				:	2017 company name Corporation, All Rights Reserved.
* Module Name			:	Module Name
* Author	    		:	Pillar Peng
* Date of Creat 		:	2017/2/24
* Date of Last Edit		:	2018-06-19 11:45:47 Tue
*==============================================================================
* Log    (Detail of Modification)
*===============================================================================
* 2018-06-19 11:45:50 Tue
*		1. modification
*******************************************************************************/

//============================================================================//
// Section for Multi-Include-Prevent
//============================================================================//
#ifndef _AUDIO_PLAY_H_
#define _AUDIO_PLAY_H_

//============================================================================//
// Section for Debug Switch
//============================================================================//


//============================================================================//
// Section for Include File
//============================================================================//
#include "user_main.h"
#include "nx1_lib.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

//============================================================================//
// Section for Definition of Macro
//============================================================================//
// 模块使能
#define USER_PLAY_MODULE				    (ENABLE)
#define AUDIO_PLAY_SOUND                    (ENABLE)

//----------------------------------------------------------------------------//
// 播放
//----------------------------------------------------------------------------//
#define PLAY_STEP_OVER			            (0xffff)
#define USER_PLAY_MAX_CNT   	            (20)                                // 播放函数一次性连续播放 最大数量 void user_play_audio(int num, ...);

//----------------------------------------------------------------------------//
// 声音
//----------------------------------------------------------------------------//
// 错误索引/无效索引
#define USER_PLAY_ERROR_INDEX               (0xffff)

// 不同类型声音的 语音索引起始值
// 歌曲
#define SOUND0_START_AUD_IDX            (0)
#define SOUND0_MAX_AUD_CNT              (10)

// 抖音
#define SOUND1_START_AUD_IDX            (SOUND0_START_AUD_IDX + SOUND0_MAX_AUD_CNT)
#define SOUND1_MAX_AUD_CNT              (1)

// 古诗
#define SOUND2_START_AUD_IDX            (SOUND1_START_AUD_IDX + SOUND1_MAX_AUD_CNT)
#define SOUND2_MAX_AUD_CNT              (12)

// 英语
#define SOUND3_START_AUD_IDX            (SOUND2_START_AUD_IDX + SOUND2_MAX_AUD_CNT)
#define SOUND3_MAX_AUD_CNT              (3)

// 故事
#define SOUND4_START_AUD_IDX            (SOUND3_START_AUD_IDX + SOUND3_MAX_AUD_CNT)
#define SOUND4_MAX_AUD_CNT              (8)

// 睡眠曲
#define SOUND5_START_AUD_IDX            (SOUND4_START_AUD_IDX + SOUND4_MAX_AUD_CNT)
#define SOUND5_MAX_AUD_CNT              (12)

// 好习惯
#define SOUND6_START_AUD_IDX            (SOUND5_START_AUD_IDX + SOUND5_MAX_AUD_CNT)
#define SOUND6_MAX_AUD_CNT              (6)

// 国学
#define SOUND7_START_AUD_IDX            (SOUND6_START_AUD_IDX + SOUND6_MAX_AUD_CNT)
#define SOUND7_MAX_AUD_CNT              (12)

// 提示音
#define SOUND8_START_AUD_IDX            (SOUND7_START_AUD_IDX + SOUND7_MAX_AUD_CNT)
#define SOUND8_MAX_AUD_CNT              (11)

// DI 声
#define SOUND9_START_AUD_IDX            (SOUND8_START_AUD_IDX + SOUND8_MAX_AUD_CNT)
#define SOUND9_MAX_AUD_CNT              (1)

// 结束
#define SOUND10_START_AUD_IDX           (SOUND9_START_AUD_IDX + SOUND9_MAX_AUD_CNT)
#define SOUND10_MAX_AUD_CNT             (0)

// 歌曲故事等总声音数量不包括音效
#define SOUND_MAX_NUM                   (SOUND10_START_AUD_IDX)

// 提示音 - 类型
#define AUD_HINT_TYPE(x)                 SOUND1_START_AUD_IDX+(x)
#define AUD_POWER_OFF                    AUD_HINT_TYPE(0)
#define AUD_POWER_ON                     AUD_HINT_TYPE(0)
#define AUD_DI                           AUD_HINT_TYPE(0)

#define AUD_LVD_WRRNING                  AUD_DI
#define AUD_LVD_SLEEP                    AUD_POWER_OFF

// 声音类型
#define SOUND_TYPE_MAX_NUM 				 (1)

#define	SOUND_TYPE_SONG				     (0)     // 歌曲
#define	SOUND_TYPE_POETRY				 (1)     // 古诗
#define	SOUND_TYPE_ENGLISH				 (2)     // 英语
#define	SOUND_TYPE_STORY				 (3)     // 故事
#define	SOUND_TYPE_MIDI				     (4)     // 睡眠曲
#define	SOUND_TYPE_HABIT			     (5)     // 好习惯
#define	SOUND_TYPE_CHINESE				 (6)     // 国学

//----------------------------------------------------------------------------//
// 随机播放 相关
//----------------------------------------------------------------------------//
#define	USER_RANDOM_PLAY_EN	             (DISABLE)
#define	SOUND_TYPE_SONG_FIXED_SEQUENCE	 (5)     // 歌曲固定数量

typedef struct AudioPlay_
{
	u8  PlayFlag; 			        // 播放标志
	u8  PlayStatus; 			    // 播放状态
	u16 PlayCnt; 		            // 播放序号
	u16 MaxPlayCnt;			        // 最大播放数量
	u16 AudIdx[USER_PLAY_MAX_CNT];  // 播放索引数组
	// u16 *AudIdx;				    // 播放索引数组
} AudioPlay_type;

typedef struct Sound_
{
	u8	Type;					    // 声音的类型 		比如：歌曲，诗歌，故事 等
	u16 StartIdx;				    // 声音的起始索引
	u16 MaxCnt; 				    // 声音的最大数量
	u16 Cnt; 					    // 声音的计数
}Sound_Type;

extern AudioPlay_type gst_audio_play;
extern u8 gu8_sound_type;
extern u16 gu16_current_audio_index;

u8 user_play_get_status(void);
u8 user_play_get_all_status(void);
void user_play_stop(void);
void user_play_stop_all(void);
void user_play_audio(int num, ...);
void user_play_pause(void);
void user_play_resume(void);
u16 user_play_get_current_index(void);

void user_play_service_loop(void);


#if _ACTION_MODULE
    void user_action_play(u16 aud_idx);
    void user_action_stop(void);
    u8 user_action_get_status(void);
#endif

#if AUDIO_PLAY_SOUND
    extern u8 gu8_play_startover_flag;

    void user_set_sound_type(u8 type);
    u16 user_get_sound_type(void);
    void user_change_sound_type(void);

    void user_play_prev_sound(u8 type);
    void user_play_next_sound(u8 type);
    void user_play_next_sound_on_off(u8 type);

    #if _RANDOM_GEN
        void user_play_random_sound(u8 type);
    #endif

    //----------------------------------------------------------------------------//
    // play mode
    //----------------------------------------------------------------------------//
    #define USER_PLAY_MODE_SINGLE_NO_PLAY   (0)
    #define USER_PLAY_MODE_ON_OFF_NEXT      (1)
    #define USER_PLAY_MODE_LOOP             (2)
    #define USER_PLAY_MODE_PLAY_STOP        (3)
    #define USER_PLAY_MODE_PREV             (4)
    #define USER_PLAY_MODE_NEXT             (5)
    #define USER_PLAY_MODE_OTHER            (6)

    void user_play_mode_set_cmd(u8 cmd);
    u8 user_play_mode_get_cmd(void);
    void user_play_mode_process(void);
    //----------------------------------------------------------------------------//
#endif  // endof AUDIO_PLAY_SOUND



#endif //end of _AUDIO_PLAY_H_

