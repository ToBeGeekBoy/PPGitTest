/*******************************************************************************
* File Name     		:	user_record.c
* Copyright				:	2017 LGW Corporation, All Rights Reserved.
* Module Name			:	record
* Author	    		:	Pillar Peng
* Date of Creat 		:	2017-12-05 14:15:12 Tue
* Date of Last Edit		:	2022-05-09 16:30:29 Mon
*==============================================================================
* Log    (Detail of Modification)
*===============================================================================
* Function:
*			1.  record func
*-------------------------------------------------------------------------------
* Code Version    : V01
*   -> Data 	  : 2020-07-17 17:02:06 Fri
*   -> History    :
*       1. Initial
*-------------------------------------------------------------------------------
* Code Version    : V06
*   -> Data 	  : 2022-05-09 16:30:25 Mon
*   -> History    :
*       1. 将
*
*            #if _ADPCM_RECORD_SOUND_TRIG
*                ADPCM_SaveEndFifoCnt();
*            #endif
*
*            改为：
*
*                ADPCM_Stop();
*            。
*        不然 播放时，会出现播放声音不正常。
*       2. 去掉 // ADPCM_StopRecordErase(); ； 因为用调用 ADPCM_Stop(); 时, 会自动调用 ADPCM_StopRecordErase() 的功能。
*-------------------------------------------------------------------------------
*******************************************************************************/

#include "user_record.h"

#if USER_RECORD_MODULE

typedef struct USER_RECORD_
{
    u8 cmd;
    u8 step;
    u32 timer_cnt;
    u32 start_addr;          // 录音起始地址
    u8 record_num;           // 录音段的序号
}USER_RECORD_Type;

static USER_RECORD_Type lst_user_record =
{
    .cmd = 0,
    .step = 0,
    .timer_cnt = 0,
    .record_num = 0,
};

//----------------------------------------------------------------------//
// Function Name : user_record
// Description   : 录音
// Parameters    : NONE
// Returns       : NONE
//----------------------------------------------------------------------//
void user_record(void)
{
	user_play_stop();
    ADPCM_Stop();

    // 去除尾巴声音 比如停止录音时的按键音   1000 ms
    ADPCM_SkipTailSetting(USER_RECORD_SKIP_TAIL_TIME);

    // 擦除 空间
    #if _ADPCM_RECORD_ERASING
        // 实现一边录音 一遍擦除， 不用等待 擦除的时间。
        ADPCM_StartRecordErase(lst_user_record.start_addr, USER_RECORD_SIZE, C_RECORD_RECORD_ADPCM);
    #else
        // 单次全部擦除
        #if USER_RECORD_STORE_MODE==SPI_MODE
            SPI_BurstErase(lst_user_record.start_addr, USER_RECORD_SIZE);
        #else
            FMC_BurstErase_Sector(lst_user_record.start_addr, USER_RECORD_SIZE);
        #endif
    #endif

    ADPCM_Record(ADDRESS_MODE|lst_user_record.start_addr, USER_RECORD_STORE_MODE, _ADPCM_RECORD_SAMPLE_RATE, USER_RECORD_SIZE);      // 开始 录音;

    // 打开了 sound-trigger 功能，就需要添加这个语句
    #if _ADPCM_RECORD_SOUND_TRIG
        ADPCM_SaveStartFifoCnt();
    #endif
}
//----------------------------------------------------------------------//
// Function Name : user_record_get_current_num
// Description   : 得到当前录音段编号
// Parameters    : NONE
// Returns       : 当前录音段编号
//----------------------------------------------------------------------//
u8 user_record_get_current_num(void)
{
    return lst_user_record.record_num;
}
//----------------------------------------------------------------------//
// Function Name : user_record_get_start_addr
// Description   : 得到录音开始地址
// Parameters    : @u8 num：录音段编号
// Returns       : 录音开始地址
//----------------------------------------------------------------------//
u32 user_record_get_start_addr(u8 num)
{
    if(num >= USER_RECORD_MAX_SEGMENT_NUM)
    {
        num = 0;
    }

    #if USER_RECORD_STORE_MODE==SPI_MODE
        return (u32)RESOURCE_GetAddress(num);
    #elif USER_RECORD_STORE_MODE==EF_MODE
        return (u32)RESOURCE_EF_GetAddress(num);
    #endif
}
//----------------------------------------------------------------------------//
// Function Name : user_record_set_cmd
// Description   : 设置录音命令
// Parameters    :
// 					@u8 cmd
// 					- 0: DISABLE
// 					- 1: ENABLE
// 					- 2: USER_RECORD_CMD_PLAY
//
// 					@u8 num : 录音段 序号
// Returns       : NONE
//----------------------------------------------------------------------------//
void user_record_set_cmd(u8 cmd, u8 num)
{
    // 超过最大段落数
    if(num >= USER_RECORD_MAX_SEGMENT_NUM)
    {
        num = 0;
    }

    lst_user_record.record_num = num;
    lst_user_record.start_addr = USER_RECORD_START_ADDRESS_0;   // USER_RECORD_START_ADDRESS;

	if(DISABLE == cmd)
	{
        // if(USER_RECORD_CMD_ENABLE == lst_user_record.cmd)
        // {
        //     // #if _ADPCM_RECORD_SOUND_TRIG
        //     //     // ADPCM_SaveEndFifoCnt();
        //     //     ADPCM_Stop();
        //     // #endif
        // }

		lst_user_record.step = STEP_OVER;
	}
	else if(ENABLE == cmd)
	{
		lst_user_record.step = 0;
	}
	else if(USER_RECORD_CMD_PLAY == cmd)
	{
        // if(USER_RECORD_CMD_ENABLE == lst_user_record.cmd)
        // {
        //     // #if _ADPCM_RECORD_SOUND_TRIG
        //     //     // ADPCM_SaveEndFifoCnt();
        //     //     ADPCM_Stop();
        //     // #endif
        // }

		lst_user_record.step = 0;

		// ADPCM_Play(ADDRESS_MODE|lst_user_record.start_addr, SPI_MODE);             // 播放 录音
	}

    // 停止 播放
    // ADPCM_StopRecordErase();
    ADPCM_Stop();
	user_play_stop();														    // 停止 播放

    SetPPGain(_PP_GAIN);

    #if _ADPCM_PITCH_CHANGE
        ADPCM_SetPitch(0);
    #endif
    #if _ADPCM_SPEED_CHANGE
        // ADPCM_SetSpeed(0);
        // ADPCM_SetSpeedPitch(0, 0);
    #endif

	lst_user_record.cmd = cmd;
}
//----------------------------------------------------------------------------//
// Function Name : user_record_get_cmd
// Description   : 获取录音状态
// Parameters    : NONE
// Returns       :
// 					@u8 cmd
// 					- 0: DISABLE
// 					- 1: ENABLE
// 					- 2: USER_RECORD_CMD_PLAY
//----------------------------------------------------------------------------//
u8 user_record_get_cmd(void)
{
	return lst_user_record.cmd;
}
//----------------------------------------------------------------------//
// Function Name : user_record_process
// Description   : 录音 处理
// Parameters    : NONE
// Returns       : NONE
//----------------------------------------------------------------------//
void user_record_process(void)
{
    if(USER_RECORD_CMD_ENABLE == lst_user_record.cmd)
    {
        switch(lst_user_record.step)
        {
            // 播放 开始录音 提示音
            case 0:
                user_play_audio(1, USER_RECORD_HINT_START);
                ++lst_user_record.step;
                break;

            // 等待播放完成
            case 1:
                if(STATUS_STOP == user_play_get_status())
                {
                    ++lst_user_record.step;                                     // 系统关闭 步骤 加加
                }
                break;

            // 开始 录音
            case 2:
                user_record();

                // lst_user_record.timer_cnt = 0;                               // 清零 录音计数
                user_systick_clear(&lst_user_record.timer_cnt);
                ++lst_user_record.step;                                         // 步骤 加加
                break;

            // 录音最大的时间长度为, 超过之后
            case 3:
                if(
                    // STATUS_RECORDING == AUDIO_GetStatus(AUDIO_CH0)
                    STATUS_RECORDING == ADPCM_GetStatus()
                )
                {
                    if(user_systick_get_time(&lst_user_record.timer_cnt) >= USER_RECORD_MAX_TIME)
                    {
                        user_record_set_cmd(USER_RECORD_CMD_DISABLE, user_record_get_current_num());                            // 停止 SBC 的播放，播放前，记得先执行停止

                        user_play_audio(2, USER_RECORD_HINT_END, USER_RECORD_HINT_END);

                        lst_user_record.step = STEP_OVER;                            // 步骤 加加
                    }
                }
                else
                {
                    user_record_set_cmd(USER_RECORD_CMD_DISABLE, user_record_get_current_num());                                // 停止 SBC 的播放，播放前，记得先执行停止
                    user_play_audio(2, USER_RECORD_HINT_END, USER_RECORD_HINT_END);
                    lst_user_record.step = STEP_OVER;                                // 步骤 加加
                }
                break;
        }
    }
    else if(USER_RECORD_CMD_PLAY == lst_user_record.cmd)
    {
        switch(lst_user_record.step)
        {
            // 播放 录音
            case 0:
                #if _ADPCM_PITCH_CHANGE
                    ADPCM_SetPitch(USER_RECORD_PITCH);
                #endif

                SetPPGain(USER_RECORD_PP_GAIN);

                ADPCM_Play(ADDRESS_MODE|lst_user_record.start_addr, USER_RECORD_STORE_MODE);                      // 播放 录音

                ++lst_user_record.step;
                break;

            // 等待播放完成
            case 1:
                if(STATUS_STOP == user_play_get_all_status())
                {
                    user_record_set_cmd(USER_RECORD_CMD_DISABLE, user_record_get_current_num());    // 停止 SBC 的播放，播放前，记得先执行停止
                    lst_user_record.step = STEP_OVER;
                }
                break;
        }
    }
}

#endif //end of #if USER_RECORD_MODULE