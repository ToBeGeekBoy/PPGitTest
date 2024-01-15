/*******************************************************************************
* File Name         : user_talkback_adpcm.c
* Author            : Pillar Peng
* Date of Creat     : 2018/12/19
* Date of Last Edit : 2018/12/19
*==============================================================================
* Log    (Detail of Modification)
*===============================================================================
* Program Description
*===============================================================================
* Function:
*                           talk back 功能
*
*        使用方法：
*        1.在 nx1_config.h 中，使能以下宏定义：
*
*        #define _ADPCM_RECORD               ENABLE             // Enable or Disable recorder
*
*        #define _ADPCM_RECORD_SAMPLE_RATE   SAMPLE_RATE_8000	// Set sample rate (support 8000/10000/12000/16000Hz)
*        #define _ADPCM_RECORD_ERASING       ENABLE             // Enable or Disable real-time erasing function during Recording
*        #define _ADPCM_RECORD_SOUND_TRIG    ENABLE             // Enable or Disable sound trig record function
*        #define _ADPCM_PLAY                 ENABLE             // Enable or Disable playback
*
*        2. 将 这个函数放在主循环中，void user_talk_back_process(void)；
*
*        3. 使用：
*        - 打开该功能：User_TalkBack_SetCmd(ENABLE);
*        - 关闭该功能：User_TalkBack_SetCmd(DISABLE);
*
*       4.修改声音触发的参数如下：（utility_define.h）
*       ```
*       // Sound Detection setting for user
*       #define SD_HIGH_THRESHOLD		300
*       #define SD_LOW_THRESHOLD		300
*       #define SD_ACTIVE_TIME			80		// unit: ms
*       #define SD_MUTE_TIME			650		// unit: ms
*       #define SD_TIMEOUT_TIME			5000	// unit: ms
*       ```
*       5.若PP_GAIN已经调至最大，录音音量还是不能达到要求，应修改reg_callback.c文件中宏PGA_GAIN（ADC增益值）
*===============================================================================
* Code Version    : V01
*   -> Data 	  : 2020-07-17 17:02:06 Fri
*   -> History    :
*       1. Initial
*-------------------------------------------------------------------------------
* Code Version    : V08
*   -> Data 	  : 2022-05-09 16:36:25 Mon
*   -> History    :
*       0. 修改以下两点 - 合适于 NX1_CM_3.2
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

#include "user_talkback_adpcm.h"

#if USER_TALKBACK_MODULE

TALK_BACK_Type gst_user_talk_back =
{
    .cmd = 0,
    .update_flag = 0,
    .step = 0,
    .timer_cnt = 0,
    .detect_sound_flag = 0,
};
//----------------------------------------------------------------------------//
// Function Name :	user_talk_back_set_cmd
// Description	 :	设置 talk back 命令
// Parameters	 :	@u8 cmd：命令
// Return		 :	NONE
//----------------------------------------------------------------------------//
void user_talk_back_set_cmd(u8 cmd)
{
    gst_user_talk_back.cmd = cmd;
    gst_user_talk_back.update_flag = 0;
    gst_user_talk_back.timer_cnt = 0;
    gst_user_talk_back.detect_sound_flag = 0;

    #if USER_TALK_BACK_HINT_EN
        USER_TALK_BACK_LED_OFF();
    #endif  // endof #if USER_TALK_BACK_HINT_EN

    // 恢复 PP-GAIN/变声等 的默认值
    SetPPGain(_PP_GAIN);

    #if _ADPCM_PITCH_CHANGE
        ADPCM_SetPitch(0);
    #endif
    #if _ADPCM_SPEED_CHANGE
        // ADPCM_SetSpeed(0);
        // ADPCM_SetSpeedPitch(0, 0);
    #endif

    // 停止 所有播放-包括 talkback
    user_talk_back_end();

    if(ENABLE == cmd)
    {
        // 语音识别 - 打开录音时， VR 需要关闭
        #if _VR_FUNC
            user_vr_set_cmd(VR_CMD_OFF);
        #endif

        // 更新 录音工作标志位
        gst_user_talk_back.update_flag = 1;

        // 播放起始语
        // user_play_audio(1, USER_TALK_BACK_HINT_START);
    }
    else
    {
        // 播放关闭语
        // user_play_audio(2, USER_TALK_BACK_HINT_END, USER_TALK_BACK_HINT_END);
    }
}
//----------------------------------------------------------------------------//
// Function Name :	user_talk_back_get_cmd
// Description	 :	得到 talk back 命令
// Parameters	 :	NONE
// Return		 :	DISABLE ENABLE
//----------------------------------------------------------------------------//
u8 user_talk_back_get_cmd(void)
{
    return gst_user_talk_back.cmd;
}
//----------------------------------------------------------------------------//
// Function Name :	user_talk_back_get_status
// Description	 :	获取 talk back 的工作状态
// Parameters	 :	NONE
// Return		 :	STATUS_IDLE STATUS_WORKING
//----------------------------------------------------------------------------//
u8 user_talk_back_get_status(void)
{
    if(
        DISABLE == gst_user_talk_back.cmd
        ||
        // 打开时 并且 静音 则算是空闲
        (
               ENABLE == gst_user_talk_back.cmd                  // 打开时
            && 0 == gst_user_talk_back.detect_sound_flag         // 静音
        )
    )
    {
        return STATUS_IDLE;
    }

    return STATUS_WORKING;
}
//----------------------------------------------------------------------------//
// Function Name :	user_talk_back_start
// Description	 :	talk back 开始工作时的处理
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
void user_talk_back_start(void)
{
    user_play_stop_all();

    ADPCM_SkipTailSetting(USER_TALK_BACK_SKIP_TAIL_TIME);                       // 减去尾部声音的时长

        // 擦除 空间
    #if _ADPCM_RECORD_ERASING
        // 实现一边录音 一遍擦除， 不用等待 擦除的时间。
        ADPCM_StartRecordErase(USER_TALK_BACK_START_ADDRESS, USER_TALK_BACK_MAX_LENGTH, C_RECORD_RECORD_ADPCM);
    #else
        // 单次全部擦除
        #if USER_RECORD_STORE_MODE==SPI_MODE
            SPI_BurstErase(USER_TALK_BACK_START_ADDRESS, USER_TALK_BACK_MAX_LENGTH);
        #else
            FMC_BurstErase_Sector(USER_TALK_BACK_START_ADDRESS, USER_TALK_BACK_MAX_LENGTH);
        #endif
    #endif

    ADPCM_Record(ADDRESS_MODE|USER_TALK_BACK_START_ADDRESS, USER_TALKBACK_STORE_MODE, _ADPCM_RECORD_SAMPLE_RATE, USER_TALK_BACK_MAX_LENGTH);      // 开始 录音;
}
//----------------------------------------------------------------------------//
// Function Name :	user_talk_back_play
// Description	 :	talk back 播放录音
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
void user_talk_back_play(void)
{
    #if _ADPCM_PLAY
        ADPCM_Stop();
    #endif

    // ADPCM_StopRecordErase();
    user_play_stop_all();

    #if _ADPCM_PITCH_CHANGE
        ADPCM_SetPitch(USER_USER_TALK_BACK_PITCH);
    #endif

    #if _ADPCM_SPEED_CHANGE
        // ADPCM_SetSpeed(-12);
        // ADPCM_SetSpeedPitch(2, 2);
    #endif

    SetPPGain(USER_USER_TALK_BACK_PP_GAIN);

    ADPCM_Play(ADDRESS_MODE|USER_TALK_BACK_START_ADDRESS, USER_TALKBACK_STORE_MODE);
}
//----------------------------------------------------------------------------//
// Function Name :	user_talk_back_end
// Description	 :	talk back 结束时处理
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
void user_talk_back_end(void)
{
    #if _ADPCM_PLAY
        ADPCM_Stop();
    #endif

    // ADPCM_StopRecordErase();
    user_play_stop_all();
}
//----------------------------------------------------------------------------//
// Function Name :	user_talk_back_process
// Description	 :	talk back 处理
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
void user_talk_back_process(void)
{
    if(ENABLE == user_talk_back_get_cmd())
    {
        if(gst_user_talk_back.update_flag)
        {
            gst_user_talk_back.update_flag = 0;
            gst_user_talk_back.step = 0;
            gst_user_talk_back.detect_sound_flag = 0;

            user_systick_clear(&gst_user_talk_back.timer_cnt);
        }

        switch(gst_user_talk_back.step)
        {
            // 等待播放和输出空闲
            case 0:
                if(
                    1
                    #if USER_PLAY_MODULE
                        && STATUS_STOP == user_play_get_all_status()                    // 声音停止
                    #else
                        && STATUS_STOP == AUDIO_GetStatus_All()                         // 声音停止
                    #endif

                    // && user_systick_get_time(&gst_user_talk_back.timer_cnt) >= USER_TALK_BACK_DELAY_TIME
                    )
                {
                    user_talk_back_start();
                    gst_user_talk_back.detect_sound_flag = 0;

                    user_systick_clear(&gst_user_talk_back.timer_cnt);

                    ++gst_user_talk_back.step;
                }
                break;

            // 等待有声音触发
            case 1:
                if(gst_user_talk_back.detect_sound_flag)
                {
                    user_systick_clear(&gst_user_talk_back.timer_cnt);
                    ++gst_user_talk_back.step;
                }
                break;

            // 判断是否超过最小时间
            case 2:
                if(gst_user_talk_back.detect_sound_flag)
                {
                    // 超过 最小时间 则有效，进入下一阶段
                    // if(user_systick_get_time(&gst_user_talk_back.timer_cnt) >= USER_TALK_BACK_MIN_TIME)
                    {
                        user_system_set_status(STATUS_WORKING);

                        ++gst_user_talk_back.step;
                    }
                }
                // 没有超过最小时间，则重新开始
                else
                {
                    user_talk_back_end();
                    gst_user_talk_back.update_flag = 1;  // 重新开始
                }
                break;

            // 等待静音 或者 录音到最长时间， 然后播放录音
            case 3:
                if(
                    // STATUS_STOP == ADPCM_GetStatus()                         // 当 静音时 也就是 停止录音播放时，
                    0 == gst_user_talk_back.detect_sound_flag                   // 静音
                    || (user_systick_get_time(&gst_user_talk_back.timer_cnt) >= USER_TALK_BACK_MAX_TIME)                 // 当 超过 设定时间时
                    )
                {
                    // 提示灯
                    #if USER_TALK_BACK_HINT_EN
                        USER_TALK_BACK_LED_OFF();
                    #endif  // endof #if USER_TALK_BACK_HINT_EN

                    // 先停止录音 - 不能省略
                    #if _ADPCM_RECORD_SOUND_TRIG
                        // ADPCM_SaveEndFifoCnt();
                        ADPCM_Stop();
                    #endif

                    // 播放录音内容
                    user_talk_back_play();

                    user_systick_clear(&gst_user_talk_back.timer_cnt);
                    ++gst_user_talk_back.step;
                }
                break;

            // 等待播放完成后，再次进入 talk back
            case 4:
                if(STATUS_STOP == ADPCM_GetStatus())
                {
                    gst_user_talk_back.update_flag = 1;  // 重新开始
                }
                break;
        }
    }
    // 自动 打开 talkback 功能
    else
    {
        // 当系统工作模式下，当无声音和无其他动作时，重新进入 talk back 功能
        if(STATUS_WORKING == user_system_get_status())
        {
            if(
                DISABLE == user_talk_back_get_cmd()                             // talkback 关闭

                #if USER_RECORD_MODULE
                    && USER_RECORD_CMD_DISABLE == user_record_get_cmd()         // 录音关闭
                #endif
                #if USER_OUTPUT_MODULE_EN
                    && STATUS_IDLE == user_output_get_status()                      // 输出稳定
                #endif
                #if USER_PLAY_MODULE && AUDIO_PLAY_SOUND
                    && USER_PLAY_MODE_LOOP != user_play_mode_get_cmd()              // 不是循环模式
                #endif

                #if USER_PLAY_MODULE
                    && STATUS_STOP == user_play_get_all_status()                    // 声音停止
                #else
                    && STATUS_STOP == AUDIO_GetStatus_All()                         // 声音停止
                #endif

                #if _VR_FUNC
                    && VR_CMD_OFF == user_vr_get_cmd()                          // VR 是关闭的状态
                #endif
                )
            {
                user_talk_back_set_cmd(ENABLE);
            }
        }
    }
}
#endif // endof #if USER_TALKBACK_MODULE