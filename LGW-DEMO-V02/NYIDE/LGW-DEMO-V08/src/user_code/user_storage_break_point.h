/*******************************************************************************
* File Name     		:	user_storage_break_point.h
* Copyright				:	2023 LGW Corporation, All Rights Reserved.
* Module Name			:	bma220
* Author	    		:	songuo
* Date of Creat 		:	2023-01-09 16:21:13 Tue
* Date of Last Edit		:	2023-01-09 16:21:13 Tue
*==============================================================================
* Log    (Detail of Modification)
*===============================================================================
* 2023-01-11 16:21:16 Tue
*		1. modification
*******************************************************************************/

#ifndef _USER_STORAGE_BREAK_POINT_H_
#define _USER_STORAGE_BREAK_POINT_H_

#include "nx1_lib.h"

#define USER_STORAGE_BREAK_POINT_MOUDLE             (DISABLE)

#if USER_STORAGE_BREAK_POINT_MOUDLE

//----------------------------------------------------------------------------//
// 自定义变量功能开关
//----------------------------------------------------------------------------//
#define USER_CUSTOMIZE_VAR_CMD               (ENABLE)

#if USER_CUSTOMIZE_VAR_CMD
    //----------------------------------------------------------------------------//
    // 自定义的变量结构体，根据需求添加变量即可
    //----------------------------------------------------------------------------//
    typedef struct
    {
        u8 volume;                // 音量值
        u16 song_index;           // 歌曲下标
        // u8 var_3;              // 自定义存储变量3
        // u8 var_4;              // 自定义存储变量4
        // u8 var_5;              // 自定义存储变量5
    }user_customize_var_t;
    //----------------------------------------------------------------------------//
    // 自定义变量有效范围值
    //----------------------------------------------------------------------------//
    #define USER_VOLUME_MIN                             (0)     //音量值最小值
    #define USER_VOLUME_MAX                             (15)    //音量值最大值
    #define USER_SONG_STRAT_INDEX                       (0)     //歌曲下标开始值
    #define USER_SONG_END_INDEX                         (1)     //歌曲下标结束值
    //----------------------------------------------------------------------------//
    // 自定义变量初始值
    //----------------------------------------------------------------------------//
    #define USER_VAR_1_INIT_VALUE                       (USER_VOLUME_MAX)           // 音量初始值
    #define USER_VAR_2_INIT_VALUE                       (USER_SONG_STRAT_INDEX)     // 歌曲下标初始值
    // #define USER_VAR_3_INIT_VALUE                    (0)                         // 变量3初始值
    // #define USER_VAR_4_INIT_VALUE                    (0)                         // 变量4初始值
    // #define USER_VAR_5_INIT_VALUE                    (0)                         // 变量5初始值

    //----------------------------------------------------------------------------//
    // 自定义变量大小
    //----------------------------------------------------------------------------//
    #define USER_CUSTOMIZE_VAR_SIZE                     (sizeof(gst_user_customize_var)) //byte

    extern user_customize_var_t gst_user_customize_var;

    // 用户调用的变量值
    extern u8 gu8_volume;               // 音量值
    extern u16 gu16_song_index;         // 歌曲下标值

    void user_storage_set_customize_var(void);
    void user_storage_get_all_customize_var_value(void);
    void user_storage_save_customize_var(void);
#endif

//----------------------------------------------------------------------------//
// 自定义断点功能开关
//----------------------------------------------------------------------------//
#define USER_CUSTOMIZE_BREAK_POINT_CMD               (ENABLE)

#if USER_CUSTOMIZE_BREAK_POINT_CMD
    //----------------------------------------------------------------------------//
    // 自定义需要记录的断点信息数量
    //----------------------------------------------------------------------------//
    #define USER_SBC_AUD_BREAK_POINT_NUM              (1)
    //----------------------------------------------------------------------------//
    // 自定义断点信息大小
    //----------------------------------------------------------------------------//
    #define USER_CUSTOMIZE_BREAK_POINT_SIZE           (sizeof(gst_user_customize_audioplay_breakpoint))    //byte
    //----------------------------------------------------------------------------//
    // 自定义断点信息初始值
    //----------------------------------------------------------------------------//
    #define USER_CUSTOMIZE_BREAK_POINT_CURADDR_VAR_INIT_VALUE            (0)
    #define USER_CUSTOMIZE_BREAK_POINT_PROCESS_OUNT_VAR_INIT_VALUE       (0)
    #define USER_CUSTOMIZE_BREAK_POINT_RAND_0_VAR_INIT_VALUE             (0)
    #define USER_CUSTOMIZE_BREAK_POINT_RAND_1_VAR_INIT_VALUE             (0)
    //----------------------------------------------------------------------------//
    // 自定义断点下标
    //----------------------------------------------------------------------------//
    #define USER_CUSTOMIZE_BREAK_POINT_1_INDEX            (0)       //自定义断点下标1
    #define USER_CUSTOMIZE_BREAK_POINT_2_INDEX            (1)       //自定义断点下标2
    #define USER_CUSTOMIZE_BREAK_POINT_3_INDEX            (2)       //自定义断点下标3
    #define USER_CUSTOMIZE_BREAK_POINT_4_INDEX            (3)       //自定义断点下标4
    #define USER_CUSTOMIZE_BREAK_POINT_5_INDEX            (4)       //自定义断点下标5
    //----------------------------------------------------------------------------//
    // 自定义播放断点信息结构体
    //----------------------------------------------------------------------------//
    typedef struct
    {
        u32 curaddr;
        u32 process_ount;
        u16 rand[2];
    }user_customize_audioplay_breakpoint_t;

    // 存储的断点
    extern user_customize_audioplay_breakpoint_t gst_user_customize_audioplay_breakpoint[USER_SBC_AUD_BREAK_POINT_NUM];
    // 用户断点
    extern user_customize_audioplay_breakpoint_t gst_user_customize_play_breakpoint[USER_SBC_AUD_BREAK_POINT_NUM];

    //----------------------------------------------------------------------------//
    // 保存sbc_audioapi.c文件里关于断点信息的指针
    //----------------------------------------------------------------------------//
    extern u32 *gu32_p_SbcSeekCtl_CurAddr;
    extern u32 *gu32_p_SbcSeekCtl_process_count;
    extern u16 *gu16_p_SbcSeekCtl_rand;
    extern u32 *gu32_p_CurAddr;
    extern u16 *gu16_p_SBCRam;
    extern volatile s32 *gs32_p_AudioBufCtl_process_count;

    extern void user_storage_get_sbc_breakpoint_val(void);
    extern S8 SbcRamEBufCtrl_Stop(U8 mode);

    void user_storage_save_breakpoint(u32 p_index);
    void user_storage_clean_breakpoint(u32 p_index);
    void user_storage_restore_breakpoint(u32 p_index);
    void user_storage_clean_all_breakpoint(void);
    void user_storage_clean_sbc_self_contained_breakpoint(void);

    void user_storage_set_customize_audioplay_breakpoint(void);
    void user_storage_get_customize_play_break_point(u32 p_index);
    void user_storage_get_all_customize_play_break_point(void);
#endif

//----------------------------------------------------------------------------//
// 第一次存储标志变量,目的是为了第一次存储时初始化变量值
//----------------------------------------------------------------------------//
#if USER_CUSTOMIZE_VAR_CMD || USER_CUSTOMIZE_BREAK_POINT_CMD
    extern u32 gu32_user_customize_first_storage_flag;
    void user_storage_set_customize_first_storage_flag(void);

    //第一次存储标志变量大小
    #define USER_FIRST_STORAGE_FLAG_SIZE            (sizeof(gu32_user_customize_first_storage_flag))         //byte
#endif

//----------------------------------------------------------------------------//
// 需要存储的变量总个数
// 1:自定义变量功能和自定义断点功能都关闭，只存储自带的断点信息
// 2:自定义变量功能或自定义断点功能两者之一打开，存储自带的断点信息和打开的自定义变量
// 3:自定义变量功能和自定义断点功能都打开，存储自带的断点信息、自定义变量和自定义断点
//----------------------------------------------------------------------------//
#if !USER_CUSTOMIZE_VAR_CMD && !USER_CUSTOMIZE_BREAK_POINT_CMD
    #define USER_STORAGE_VAR_CNT                      (1)
#elif USER_CUSTOMIZE_VAR_CMD && !USER_CUSTOMIZE_BREAK_POINT_CMD
    #define USER_STORAGE_VAR_CNT                      (3)
#elif !USER_CUSTOMIZE_VAR_CMD && USER_CUSTOMIZE_BREAK_POINT_CMD
    #define USER_STORAGE_VAR_CNT                      (3)
#elif USER_CUSTOMIZE_VAR_CMD && USER_CUSTOMIZE_BREAK_POINT_CMD
    #define USER_STORAGE_VAR_CNT                      (4)
#endif
//----------------------------------------------------------------------------//
// 自带的断点信息大小
//----------------------------------------------------------------------------//
#define USER_BREAK_POINT_SIZE                       (12)        //byte
//----------------------------------------------------------------------------//
// 需要存储的变量总大小
//----------------------------------------------------------------------------//
#if !USER_CUSTOMIZE_VAR_CMD && !USER_CUSTOMIZE_BREAK_POINT_CMD
    #define USER_STORAGE_VAR_SIZE                     (USER_BREAK_POINT_SIZE)                                                                //byte
#elif USER_CUSTOMIZE_VAR_CMD && !USER_CUSTOMIZE_BREAK_POINT_CMD
    #define USER_STORAGE_VAR_SIZE                     (USER_BREAK_POINT_SIZE + USER_CUSTOMIZE_VAR_SIZE + USER_FIRST_STORAGE_FLAG_SIZE)       //byte
#elif !USER_CUSTOMIZE_VAR_CMD && USER_CUSTOMIZE_BREAK_POINT_CMD
    #define USER_STORAGE_VAR_SIZE                     (USER_BREAK_POINT_SIZE + USER_CUSTOMIZE_BREAK_POINT_SIZE + USER_FIRST_STORAGE_FLAG_SIZE)  //byte
#elif USER_CUSTOMIZE_VAR_CMD && USER_CUSTOMIZE_BREAK_POINT_CMD
    #define USER_STORAGE_VAR_SIZE                     (USER_BREAK_POINT_SIZE + USER_CUSTOMIZE_VAR_SIZE + USER_CUSTOMIZE_BREAK_POINT_SIZE + USER_FIRST_STORAGE_FLAG_SIZE)   //byte
#endif
//----------------------------------------------------------------------------//
// 是否在执行暂停播放、执行停止播放和声音播放停止时清除断点信息
//----------------------------------------------------------------------------//
#if USER_CUSTOMIZE_BREAK_POINT_CMD
    #define USER_SBC_PAUSE_CLEAN_BREAK_POINT            (DISABLE)
    #define USER_SBC_STOP_CLEAN_BREAK_POINT             (DISABLE)
    #define USER_SBC_PLAY_END_CLEAN_BREAK_POINT         (DISABLE)
#else
    #define USER_SBC_PAUSE_CLEAN_BREAK_POINT            (ENABLE)
    #define USER_SBC_STOP_CLEAN_BREAK_POINT             (ENABLE)
    #define USER_SBC_PLAY_END_CLEAN_BREAK_POINT         (ENABLE)
#endif
//----------------------------------------------------------------------------//
// 存储状态
//----------------------------------------------------------------------------//
#define USER_STORAGE_BREAK_POINT_STATUS_ERROR           (-1)    // 存储断点信息错误
#define USER_STORAGE_BREAK_POINT_STATUS_SUCCESS         (0)     // 存储断点信息成功

#define USER_STORAGE_BREAK_POINT_STATUS_UNDONE          (1)     // 存储断点信息未完成
#define USER_STORAGE_BREAK_POINT_STATUS_DONE            (2)     // 存储断点信息完成

#define STATUS_IDLE                                     (0)     //空闲
#define STATUS_WORKING                                  (1)     //工作

void user_storage_init(void);
u8 user_storage_get_status(void);
u8 user_storage_get_work_status(void);
void user_storage_cmd(void);
void user_storage_cmd_process(void);

#endif

#endif
