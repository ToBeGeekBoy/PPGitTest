/*******************************************************************************
* File Name     		:	file name
* Copyright				:	2017 company name Corporation, All Rights Reserved.
* Module Name			:	module name
* Project number		:	project number
* IC body       		:	IC body
* Compiler Version		:	Compiler Version
* Asm Version	  		:	Asm Version
* Author	    		:	Pillar Peng
* Date of Creat 		:	2017.2.24
* Date of Last Edit		:	2018-06-19 11:45:56 Tue
*===============================================================================
* Log    (Detail of Modification)
*===============================================================================
* 2017.2.24
*===============================================================================
* Program Description
*===============================================================================
* Function:
*			1.  Describe the function of program briefly
*-------------------------------------------------------------------------------
* Code Version    : V01
*   -> Data 	  : 2018-06-19 11:45:56 Tue
*   -> History    :
*       1. Initial
*-------------------------------------------------------------------------------
* Code Version    : V02
*   -> Data 	  : XXX
*   -> History    :
*       1. Initial
*-------------------------------------------------------------------------------
* Code Version    : V03
*   -> Data 	  : 2020-07-29 14:57:48 Wed
*   -> History    :
*       1. 添加了 随机播放函数
*       2. 添加了 pause resume 函数，还有 状态获取的函数
*       3. 修改了 播放函数。   void user_play_audio(int num, ...);   user_play_audio(3, index1, index2, index3);
*       4. 修改了 文件名。  audio_play --> user_play
*-------------------------------------------------------------------------------
* Code Version    : V04
*   -> Data 	  : 2020-09-29 10:36:06 Tue
*   -> History    :
*       1. 添加了 USER_PLAY_MODULE 宏定义
*       1. 修改了函数名 把所有的 AudioPlay_xxx 函数都改为了 UserPlay_xx 函数
*-------------------------------------------------------------------------------
* Code Version    : V05
*   -> Data 	  : 2022-01-18 14:44:00 Tue
*   -> History    :
*       1. 增加 play mode
*       2. 将 命名的字母大写 全部改为 小写
*-------------------------------------------------------------------------------
*******************************************************************************/

#include "user_play.h"

#if USER_PLAY_MODULE

u8 gu8_sound_type = 0;															// 当前声音类型
u8 gu8_sound2_type = 0;															// 当前声音类型

u16 gu16_current_audio_index = 0;
u8 gu8_play_startover_flag = 1;

AudioPlay_type gst_audio_play;
// {
//     .PlayFlag   = 0, 			    // 播放标志
// 	.PlayStatus = 0, 			    // 播放状态
// 	.PlayCnt    = 0, 		        // 播放序号
// 	.MaxPlayCnt = 0,			    // 最大播放数量
// //	.AudIdx     = NULL,				// 播放索引数组
// };


typedef struct user_play_
{
    u8 cmd;
    u8 update_flag;
    u8 step;
} usert_play_t;

usert_play_t gst_user_play=
{
    0,
};


#if AUDIO_PLAY_SOUND
    // sound key 的定义和初始化 - KEY1-KEY10
    Sound_Type gst_sound_a[SOUND_TYPE_MAX_NUM]=
    {
        // type     StartIdx, 				MaxCnt, 				Cnt,
        {0, 	    SOUND0_START_AUD_IDX, 	SOUND0_MAX_AUD_CNT, 	SOUND0_MAX_AUD_CNT},			// KEY1    播放声音的初始化
        // {1, 	    SOUND1_START_AUD_IDX, 	SOUND1_MAX_AUD_CNT, 	SOUND1_MAX_AUD_CNT},			// KEY2    播放声音的初始化
        // {2, 	    SOUND2_START_AUD_IDX, 	SOUND2_MAX_AUD_CNT, 	SOUND2_MAX_AUD_CNT},			// KEY3    播放声音的初始化
        // {3, 	    SOUND3_START_AUD_IDX, 	SOUND3_MAX_AUD_CNT, 	SOUND3_MAX_AUD_CNT},			// KEY4    播放声音的初始化
    };
#endif
//----------------------------------------------------------------------//
// Function Name : user_play_stop
// Description	 : 停止播放
// Parameters	 : NONE
// Returns		 : NONE
//----------------------------------------------------------------------//
void user_play_stop(void)
{
	AUDIO_Stop(AUDIO_CH0);

    #if _ACTION_MODULE
        user_action_stop();
    #endif

	gst_audio_play.PlayCnt = PLAY_STEP_OVER;									// 清零 步骤
	gst_audio_play.PlayStatus = STATUS_STOP;
}
//----------------------------------------------------------------------//
// Function Name : user_play_stop_all
// Description	 : 停止播放所有
// Parameters	 : NONE
// Returns		 : NONE
//----------------------------------------------------------------------//
void user_play_stop_all(void)
{
    user_play_stop();														    // 停止播放

    #if _SBC_PLAY
        SBC_Stop();
    #endif

    #if _SBC_CH2_PLAY
        SBC_CH2_Stop();
    #endif

    #if _SBC2_PLAY
        SBC2_Stop();
    #endif

    #if _SBC2_CH2_PLAY
        SBC2_CH2_Stop();
    #endif

    #if _ADPCM_PLAY
        ADPCM_Stop();
    #endif

    #if _ADPCM_CH2_PLAY
        ADPCM_CH2_Stop();
    #endif

    #if _ADPCM_CH3_PLAY
        ADPCM_CH3_Stop();
    #endif

    #if _ADPCM_CH4_PLAY
        ADPCM_CH4_Stop();
    #endif

    #if _ADPCM_CH5_PLAY
        ADPCM_CH5_Stop();
    #endif

    #if _ADPCM_CH6_PLAY
        ADPCM_CH6_Stop();
    #endif

    #if _ADPCM_CH7_PLAY
        ADPCM_CH7_Stop();
    #endif

    #if _ADPCM_CH8_PLAY
        ADPCM_CH8_Stop();
    #endif

    #if _CELP_PLAY
        CELP_Stop();
    #endif

    #if _PCM_PLAY
        PCM_Stop();
    #endif

    #if _PCM_CH2_PLAY
        PCM_CH2_Stop();
    #endif

    #if _PCM_CH3_PLAY
        PCM_CH3_Stop();
    #endif

    #if _MIDI_PLAY
        MIDI_Stop();
    #endif
}
//----------------------------------------------------------------------//
// Function Name : user_play_pause
// Description	 : 暂停播放
// Parameters	 : NONE
// Returns		 : NONE
//----------------------------------------------------------------------//
void user_play_pause(void)
{
	AUDIO_Pause(AUDIO_CH0);

	gst_audio_play.PlayStatus = STATUS_PAUSE;
}
//----------------------------------------------------------------------//
// Function Name : user_play_resume
// Description	 : 继续播放
// Parameters	 : NONE
// Returns		 : NONE
//----------------------------------------------------------------------//
void user_play_resume(void)
{
	AUDIO_Resume(AUDIO_CH0);

	gst_audio_play.PlayStatus = STATUS_PLAYING;
}
//----------------------------------------------------------------------//
// Function Name : user_play_get_status
// Description	 : 获取播放状态
// Parameters	 : NONE
// Returns		 : @u8 status 播放状态
// 					- 0: STATUS_STOP
// 					- 1: STATUS_PLAYING
//----------------------------------------------------------------------//
u8 user_play_get_status(void)
{
	return gst_audio_play.PlayStatus;
}
//----------------------------------------------------------------------------//
// 获取全部语音播放状态
// 只检测三种状态：
// - 全部 停止
// - 有 播放存放
// - 其他 (录音状态 等)
//----------------------------------------------------------------------------//
u8 user_play_get_all_status(void)
{
    u8 sta = -1;

    if(
        STATUS_STOP == user_play_get_status()

        && STATUS_STOP == AUDIO_GetStatus(AUDIO_CH0)

        #if _SBC_PLAY
            && STATUS_STOP == SBC_GetStatus()
        #endif

        #if _SBC_CH2_PLAY
            && STATUS_STOP == SBC_CH2_GetStatus()
        #endif

        #if _SBC2_PLAY
            && STATUS_STOP == SBC2_GetStatus()
        #endif

        #if _SBC2_CH2_PLAY
            && STATUS_STOP == SBC2_CH2_GetStatus()
        #endif

        #if _ADPCM_PLAY
            && STATUS_STOP == ADPCM_GetStatus()
        #endif

        #if _ADPCM_CH2_PLAY
            && STATUS_STOP == ADPCM_CH2_GetStatus()
        #endif

        #if _ADPCM_CH3_PLAY
            && STATUS_STOP == ADPCM_CH3_GetStatus()
        #endif

        #if _ADPCM_CH4_PLAY
            && STATUS_STOP == ADPCM_CH4_GetStatus()
        #endif

        #if _ADPCM_CH5_PLAY
            && STATUS_STOP == ADPCM_CH5_GetStatus()
        #endif

        #if _ADPCM_CH6_PLAY
            && STATUS_STOP == ADPCM_CH6_GetStatus()
        #endif

        #if _ADPCM_CH7_PLAY
            && STATUS_STOP == ADPCM_CH7_GetStatus()
        #endif

        #if _ADPCM_CH8_PLAY
            && STATUS_STOP == ADPCM_CH8_GetStatus()
        #endif

        #if _CELP_PLAY
            && STATUS_STOP == CELP_GetStatus()
        #endif

        #if _PCM_PLAY
            && STATUS_STOP == PCM_GetStatus()
        #endif

        #if _PCM_CH2_PLAY
            && STATUS_STOP == PCM_CH2_GetStatus()
        #endif

        #if _PCM_CH3_PLAY
            && STATUS_STOP == PCM_CH3_GetStatus()
        #endif

        #if _MIDI_PLAY
            && STATUS_STOP == MIDI_GetStatus()
        #endif
        )
    {
        sta = STATUS_STOP;
    }
    else if(
        STATUS_PLAYING == user_play_get_status()

        || STATUS_PLAYING == AUDIO_GetStatus(AUDIO_CH0)

        #if _SBC_PLAY
            || STATUS_PLAYING == SBC_GetStatus()
        #endif

        #if _SBC_CH2_PLAY
            || STATUS_PLAYING == SBC_CH2_GetStatus()
        #endif

        #if _SBC2_PLAY
            || STATUS_PLAYING == SBC2_GetStatus()
        #endif

        #if _SBC2_CH2_PLAY
            || STATUS_PLAYING == SBC2_CH2_GetStatus()
        #endif

        #if _ADPCM_PLAY
            || STATUS_PLAYING == ADPCM_GetStatus()
        #endif

        #if _ADPCM_CH2_PLAY
            || STATUS_PLAYING == ADPCM_CH2_GetStatus()
        #endif

        #if _ADPCM_CH3_PLAY
            || STATUS_PLAYING == ADPCM_CH3_GetStatus()
        #endif

        #if _ADPCM_CH4_PLAY
            || STATUS_PLAYING == ADPCM_CH4_GetStatus()
        #endif

        #if _ADPCM_CH5_PLAY
            || STATUS_PLAYING == ADPCM_CH5_GetStatus()
        #endif

        #if _ADPCM_CH6_PLAY
            || STATUS_PLAYING == ADPCM_CH6_GetStatus()
        #endif

        #if _ADPCM_CH7_PLAY
            || STATUS_PLAYING == ADPCM_CH7_GetStatus()
        #endif

        #if _ADPCM_CH8_PLAY
            || STATUS_PLAYING == ADPCM_CH8_GetStatus()
        #endif

        #if _CELP_PLAY
            || STATUS_PLAYING == CELP_GetStatus()
        #endif

        #if _PCM_PLAY
            || STATUS_PLAYING == PCM_GetStatus()
        #endif

        #if _PCM_CH2_PLAY
            || STATUS_PLAYING == PCM_CH2_GetStatus()
        #endif

        #if _PCM_CH3_PLAY
            || STATUS_PLAYING == PCM_CH3_GetStatus()
        #endif

        #if _MIDI_PLAY
            || STATUS_PLAYING == MIDI_GetStatus()
        #endif
        )
    {
        sta = STATUS_PLAYING;
    }

	return sta;
}
//----------------------------------------------------------------------------//
// Function Name : user_play_audio
// Description	 : 播放 声音。 数量 根据 参数数量而定
//                 我要连续播放 3 个声音
//                 Ex: user_play_audio(3, index1, index2, index3)
// Parameters	 :
// 					@u16 num 待播放的语音数量
// 					@... 待播放的语音索引
// 					    - 表示可变的参数
// Returns		 : NONE
//----------------------------------------------------------------------------//
void user_play_audio(int num,...)
{
    va_list valist;

    int i;

    #if 0 //_USE_HEAP_RAM
        //----------------------------------------------------------------------------//
        // 获取播放索引
        //----------------------------------------------------------------------------//
        if(gst_audio_play.AudIdx != NULL)
        {
            MemFree(gst_audio_play.AudIdx);
        }

        gst_audio_play.AudIdx = NULL;

        // 分配空间
        gst_audio_play.AudIdx = (u16*)MemAlloc(num*sizeof(u16));

        // 分配失败，退出
        if(NULL == gst_audio_play.AudIdx)
        {
            gst_audio_play.PlayCnt = PLAY_STEP_OVER;									// 播放步骤 1
            // gst_audio_play.PlayStatus = STATUS_STOP;
            return;
        }
        // 清零 数组
        // memset(gst_audio_play.AudIdx, 0, num*sizeof(u16));
    #else
        // 数量不超过最大值
        if(num >= USER_PLAY_MAX_CNT)
        {
            num = USER_PLAY_MAX_CNT;
        }
    #endif

    // 为 num 个参数初始化 valist， 将参数装入 valist 中
    va_start(valist, num);

    // 访问所有赋给 valist 的参数， va_arg(valist, int) 以 int 为单位取出 valist 中的值
    for (i = 0; i < num; i++)
    {
       gst_audio_play.AudIdx[i] = va_arg(valist, int);
    }

    //  清理为 valist 保留的内存
    va_end(valist);

    //----------------------------------------------------------------------------//
    // 播放
    //----------------------------------------------------------------------------//
    gst_audio_play.MaxPlayCnt = num;												// 播放步骤 1

	gst_audio_play.PlayCnt = 0;
	gst_audio_play.PlayStatus = STATUS_PLAYING;

    gu16_current_audio_index = gst_audio_play.AudIdx[0];                        // 设置为当前声音索引
}
//----------------------------------------------------------------------//
// Function Name : user_play_service_loop
// Description	 : 播放函数
// Parameters	 : NONE
// Returns		 : 播放状态
//----------------------------------------------------------------------//
void user_play_service_loop(void)
{
	// 检测 播放步骤是否超过最大步骤，即 播放步骤完成，可能是正在播放最后一个语音，也可能是播放完成
	if(gst_audio_play.PlayCnt >= gst_audio_play.MaxPlayCnt)
	{
		gst_audio_play.PlayCnt = PLAY_STEP_OVER;
	}

	// 如果是步骤1 则先停止之前的播放
	if(gst_audio_play.PlayCnt == 0)													// 如果是 第一步，则停止 之前的语音播放
	{
		AUDIO_Stop(AUDIO_CH0);
	}

	// 当 不是结束步骤时, 和 没有在播放时 才播放
	if(gst_audio_play.PlayCnt != PLAY_STEP_OVER && STATUS_STOP == AUDIO_GetStatus(AUDIO_CH0))		// 播放完成猜播放
	{
        gst_audio_play.PlayFlag = 1;

        gu16_current_audio_index = gst_audio_play.AudIdx[gst_audio_play.PlayCnt];

        #if _ACTION_MODULE
        // play Action
        user_action_play(gu16_current_audio_index);
        #endif

        // GPIO_Toggle(GPIOA,8);

    	#if _SPI_MODULE
		    AUDIO_Play(AUDIO_CH0, gu16_current_audio_index, SPI_MODE);						// 播放 当前句
        #else
            AUDIO_Play(AUDIO_CH0, gu16_current_audio_index, OTP_MODE);						// 播放 当前句
        #endif

		gst_audio_play.PlayCnt++;																	// 播放完，继续下一步

	}

	// 播放步骤是在最后一步，并且播放也停止了，说明没有声音在播放
	if(PLAY_STEP_OVER == gst_audio_play.PlayCnt && STATUS_STOP == AUDIO_GetStatus(AUDIO_CH0))
	{
        // if(gst_audio_play.PlayFlag)                                                        // 由播放第一次变为停止，则是第一次停止
        // {
        //     gst_audio_play.PlayFlag  = 0;                                                  // 清楚播放标志位

        //     #if _ACTION_MODULE
        //     user_action_stop();
        //     #endif
        // }

		gst_audio_play.PlayStatus = STATUS_STOP;																//
	}
    // 除了停止之外的其他状态， gst_audio_play.PlayStatus 的状态 和 AUDIO_GetStatus(AUDIO_CH0) 一样
	else if(STATUS_STOP != AUDIO_GetStatus(AUDIO_CH0))
	{
		gst_audio_play.PlayStatus = AUDIO_GetStatus(AUDIO_CH0);
	}

    //----------------------------------------------------------------------------//
    // 播放模式
    //----------------------------------------------------------------------------//
    #if AUDIO_PLAY_SOUND
        user_play_mode_process();
    #endif
}
//----------------------------------------------------------------------//
// Function Name : user_play_get_current_index
// Description	 : 得到当前播放声音下标
// Parameters	 : NONE
// Returns		 : 当前播放声音下标
//----------------------------------------------------------------------//
u16 user_play_get_current_index(void)
{
    // 如果不是播放状态 则 返回 错误索引值
    if(STATUS_PLAYING != user_play_get_status())
    {
        return USER_PLAY_ERROR_INDEX;
    }

    return gu16_current_audio_index;
}

#if _ACTION_MODULE
    //----------------------------------------------------------------------//
    // Function Name : user_action_stop
    // Description	 : 停止播放action
    // Parameters	 : NONE
    // Returns		 : NONE
    //----------------------------------------------------------------------//
    void user_action_stop(void)
    {
        s8 i = 0;

        for(i = 0;i < _VIO_CH_NUM;++i)
        {
            ACTION_StopIO(ACTION_CH0 + i);
        }
    }
    //----------------------------------------------------------------------//
    // Function Name : user_action_get_status
    // Description	 : 得到action状态
    // Parameters	 : NONE
    // Returns		 : NONE
    //----------------------------------------------------------------------//
    u8 user_action_get_status(void)
    {
        s8 i=0;

        u8 sta = STATUS_STOP;

        for(i=0;i<_VIO_CH_NUM;++i)                                                  // 只有有一个在播放就 是播放状态
        {
            if(STATUS_PLAYING == ACTION_GetIOStatus(ACTION_CH0+i))
            {
                sta = STATUS_PLAYING;
                break;
            }
        }
        return sta;
    }
    //----------------------------------------------------------------------//
    // Function Name : user_action_play
    // Description	 : action 播放
    // Parameters	 : @u16 aud_idx：播放下标
    // Returns		 : NONE
    //----------------------------------------------------------------------//
    void user_action_play(u16 aud_idx)
    {
        u8 type = 0;
        u16 action_index = 0;

        user_action_stop();

        action_index = 0;

        ACTION_PlayIO(ACTION_CH0, action_index+0, ACTION_PORTA | 8,  ACTION_NO_REPEAT, ACTION_BUSY_HIGH, SPI_MODE);
        ACTION_PlayIO(ACTION_CH1, action_index+1, ACTION_PORTA | 9,  ACTION_NO_REPEAT, ACTION_BUSY_HIGH, SPI_MODE);
        ACTION_PlayIO(ACTION_CH2, action_index+2, ACTION_PORTA | 10,  ACTION_NO_REPEAT, ACTION_BUSY_HIGH, SPI_MODE);
    }
    //----------------------------------------------------------------------//
    // Function Name : user_action_play_end
    // Description	 : end action 播放
    // Parameters	 : NONE
    // Returns		 : NONE
    //----------------------------------------------------------------------//
    void user_action_play_end(void)
    {
        gu8_SoundTpyeForAction = SOUND_TYPE_PLAY_END;

        ACTION_PlayIO(ACTION_CH0, PLAY_END_ACTION_IDX+0, ACTION_PORTA | 8,  ACTION_NO_REPEAT, ACTION_BUSY_HIGH, SPI_MODE);
        ACTION_PlayIO(ACTION_CH1, PLAY_END_ACTION_IDX+1, ACTION_PORTA | 9,  ACTION_NO_REPEAT, ACTION_BUSY_HIGH, SPI_MODE);
        // ACTION_PlayIO(ACTION_CH2, gst_Action.Index+2, ACTION_PORTA | 5,  ACTION_NO_REPEAT, ACTION_BUSY_HIGH, SPI_MODE);
    }
#endif

#if AUDIO_PLAY_SOUND
//============================================================================//
// Section for sound
//============================================================================//
//----------------------------------------------------------------------//
// Function Name : user_set_sound_type
// Description	 : 设置声音类型
// Parameters	 : @u8 type：类型
// Returns		 : NONE
//----------------------------------------------------------------------//
void user_set_sound_type(u8 type)
{
    gu8_sound_type = type;
}
//----------------------------------------------------------------------//
// Function Name : user_get_sound_type
// Description	 : 得到声音类型
// Parameters	 : NONE
// Returns		 : 声音类型
//----------------------------------------------------------------------//
u16 user_get_sound_type(void)
{
    return gu8_sound_type;
}
//----------------------------------------------------------------------//
// Function Name : user_change_sound_type
// Description	 : 切换声音类型
// Parameters	 : NONE
// Returns		 : NONE
//----------------------------------------------------------------------//
void user_change_sound_type(void)
{
    if(++gu8_sound2_type >= SOUND_TYPE_MAX_NUM)
    {
        gu8_sound2_type = 0;
    }

    gu8_sound_type = gu8_sound2_type;

    gu8_play_startover_flag = 1;

    user_system_stop_all_audio_module();
    user_set_sound_type(gu8_sound_type);
    user_play_mode_set_cmd(USER_PLAY_MODE_ON_OFF_NEXT);
}
//----------------------------------------------------------------------//
// Function Name : user_play_current_sound
// Description	 : 播放 当前 声音，这里并没有真正的播放，只是设置标志
// Parameters	 : NONE
// Returns		 : NONE
//----------------------------------------------------------------------//
void user_play_current_sound(u8 type)
{
    Sound_Type *st_sound;

	u16 audio_idx = 0;

    gu8_sound_type = type;       // 记录 声音类型

    st_sound = &gst_sound_a[type];

    if(st_sound->Cnt >= st_sound->MaxCnt)
    {
        st_sound->Cnt = 0;
    }

    audio_idx = (st_sound->StartIdx + st_sound->Cnt);	// 获取播放索引

    user_play_audio(1, audio_idx);
}
//----------------------------------------------------------------------//
// Function Name : user_play_prev_sound
// Description	 : 播放 上一个 声音，这里并没有真正的播放，只是设置标志
// Parameters	 : NONE
// Returns		 : NONE
//----------------------------------------------------------------------//
void user_play_prev_sound(u8 type)
{
    Sound_Type *st_sound;

	u16 audio_idx = 0;

    gu8_sound_type = type;       // 记录 声音类型

    st_sound = &gst_sound_a[type];

    // 计数
    if(0 == st_sound->Cnt)
    {
        st_sound->Cnt = st_sound->MaxCnt-1;
    }
    else
    {
        --st_sound->Cnt;
    }

    audio_idx = (st_sound->StartIdx + st_sound->Cnt);	// 获取播放索引

    user_play_audio(1, audio_idx);
}
//----------------------------------------------------------------------//
// Function Name : user_play_next_sound
// Description	 : 播放 下一个 声音，这里并没有真正的播放，只是设置标志
// Parameters	 : NONE
// Returns		 : NONE
//----------------------------------------------------------------------//
void user_play_next_sound(u8 type)
{
    Sound_Type *st_sound;

    #if USER_RANDOM_PLAY_EN
        static u8 random_flag = 0;
    #endif

	u16 audio_idx = 0;

    gu8_sound_type = type;       // 记录 声音类型

    st_sound = &gst_sound_a[type];

    // 计数
    if(++st_sound->Cnt >= st_sound->MaxCnt || gu8_play_startover_flag)
    {
        #if USER_RANDOM_PLAY_EN
            if(gu8_play_startover_flag)
            {
                random_flag = 0;
            }
        #endif

        st_sound->Cnt = 0;
    }

    audio_idx = (st_sound->StartIdx + st_sound->Cnt);	                        // 获取播放索引

    #if USER_RANDOM_PLAY_EN       // 是否需要随机
    // 前 5 首音乐固定
    if(st_sound->Cnt < SOUND_TYPE_SONG_FIXED_SEQUENCE)
    {
        if(0 == random_flag)
        {
            audio_idx = (st_sound->StartIdx + st_sound->Cnt);	// 获取播放索引
        }
        else
        {
            audio_idx = st_sound->StartIdx + SOUND_TYPE_SONG_FIXED_SEQUENCE + Random(st_sound->MaxCnt - SOUND_TYPE_SONG_FIXED_SEQUENCE);
        }
    }
    // 后面的真随机
    else
    {
        random_flag = 1;
        audio_idx = st_sound->StartIdx + SOUND_TYPE_SONG_FIXED_SEQUENCE + Random(st_sound->MaxCnt - SOUND_TYPE_SONG_FIXED_SEQUENCE);
    }
    #endif // end of 0

    user_play_audio(1, audio_idx);

    gu8_play_startover_flag = 0;
}
//----------------------------------------------------------------------------//
// Function Name : User_PlayOnOff
// Description	 : 播放 On/Off 功能
// Parameters	 :
// 					@Sound_Type *st_sound	这是 sound 的结构体变量
// Returns		 : NONE
//----------------------------------------------------------------------------//
void user_play_next_sound_on_off(u8 type)
{
    if(STATUS_PLAYING == user_play_get_status())							    // 当 正在播放时，停止播放
    {
        user_play_stop_all();
    }
    else
    {
        user_play_stop_all();
		user_play_next_sound(type);											    // 获取播放语音索引
    }
}

#if USER_RANDOM_PLAY_EN
//----------------------------------------------------------------------//
// Function Name : User_PlayRandomSound
// Description	 : 播放 随机 声音，这里并没有真正的播放，只是设置标志
// Parameters	 : NONE
// Returns		 : NONE
//----------------------------------------------------------------------//
void user_play_random_sound(u8 type)
{
    Sound_Type *st_sound;

	u16 audio_idx = 0;

    gu8_sound_type = type;       // 记录 声音类型

    st_sound = &gst_sound_a[type];

    // // 计数
    // if(++st_sound->Cnt >= st_sound->MaxCnt)
    // {
    //     st_sound->Cnt = 0;
    // }

    st_sound->Cnt = Random(st_sound->MaxCnt);

    audio_idx = st_sound->StartIdx + st_sound->Cnt;	// 获取播放索引

    user_play_audio(1, audio_idx);
}
#endif      // endof #if USER_RANDOM_PLAY_EN

//----------------------------------------------------------------------------//
// play mode
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------//
// Function Name : user_play_mode_set_cmd
// Description	 : 设置播放命令
// Parameters	 : @u8 cmd：命令
// Returns		 : NONE
//----------------------------------------------------------------------//
void user_play_mode_set_cmd(u8 cmd)
{
    // 关闭录音
    #if USER_RECORD_MODULE
        user_record_set_cmd(USER_RECORD_CMD_DISABLE, 0);
    #endif

    // 关闭录变音
    #if USER_TALKBACK_MODULE
        user_talk_back_set_cmd(DISABLE);
    #endif

    // 单曲模式不播放 模式
    if(USER_PLAY_MODE_SINGLE_NO_PLAY == cmd)
    {
        cmd = USER_PLAY_MODE_ON_OFF_NEXT;
        gst_user_play.update_flag = 0;
    }
    else
    {
        gst_user_play.update_flag = 1;
    }

    if(USER_PLAY_MODE_ON_OFF_NEXT == cmd)
    {
        // 如果上次是 loop mode, 则从头开始播放
        if(USER_PLAY_MODE_LOOP == gst_user_play.cmd)
        {
            gu8_play_startover_flag = 1;
        }
    }
    else if(USER_PLAY_MODE_LOOP == cmd)
    {
        gu8_play_startover_flag = 1;
    }

    gst_user_play.cmd = cmd;
    gst_user_play.step = 0;
}
//----------------------------------------------------------------------//
// Function Name : user_play_mode_get_cmd
// Description	 : 得到播放命令
// Parameters	 : NONE
// Returns		 : 播放命令
//----------------------------------------------------------------------//
u8 user_play_mode_get_cmd(void)
{
    return gst_user_play.cmd;
}
//----------------------------------------------------------------------//
// Function Name : user_play_mode_process_1
// Description	 : 播放命令1处理
// Parameters	 : NONE
// Returns		 : NONE
//----------------------------------------------------------------------//
void user_play_mode_process_1(void)
{
    if(gst_user_play.update_flag)
    {
        gst_user_play.update_flag = 0;
        gst_user_play.step = 0;

        user_play_next_sound_on_off(gu8_sound_type);
    }
}
//----------------------------------------------------------------------//
// Function Name : user_play_mode_process_2
// Description	 : 播放命令2处理
// Parameters	 : NONE
// Returns		 : NONE
//----------------------------------------------------------------------//
void user_play_mode_process_2(void)
{
    if(gst_user_play.update_flag)
    {
        gst_user_play.step = 0;

        user_play_stop_all();

        if(STATUS_STOP == user_play_get_status())
        {
            gst_user_play.update_flag = 0;
            user_play_next_sound(gu8_sound_type);
        }
    }
}
//----------------------------------------------------------------------//
// Function Name : user_play_mode_process_3
// Description	 : 播放命令3处理
// Parameters	 : NONE
// Returns		 : NONE
//----------------------------------------------------------------------//
void user_play_mode_process_3(void)
{
    if(gst_user_play.update_flag)
    {
        gst_user_play.update_flag = 0;
        gst_user_play.step = 0;

        user_play_stop_all();
        user_play_prev_sound(gu8_sound_type);
    }
}
//----------------------------------------------------------------------//
// Function Name : user_play_mode_process_4
// Description	 : 播放命令4处理
// Parameters	 : NONE
// Returns		 : NONE
//----------------------------------------------------------------------//
void user_play_mode_process_4(void)
{
    if(gst_user_play.update_flag)
    {
        gst_user_play.update_flag = 0;
        gst_user_play.step = 0;

        user_play_stop_all();
        user_play_next_sound(gu8_sound_type);
    }
}
//----------------------------------------------------------------------//
// Function Name : user_play_mode_process_5
// Description	 : 播放命令5处理
// Parameters	 : NONE
// Returns		 : NONE
//----------------------------------------------------------------------//
void user_play_mode_process_5(void)
{
    if(gst_user_play.update_flag)
    {
        gst_user_play.update_flag = 0;
        gst_user_play.step = 0;

        if(STATUS_PLAYING == user_play_get_status())							// 当 正在播放时，停止播放
        {
            user_play_stop_all();
        }
        else
        {
            user_play_stop_all();
            user_play_current_sound(gu8_sound_type);							// 获取播放语音索引
        }
    }
}
//----------------------------------------------------------------------//
// Function Name : user_play_mode_process
// Description	 : 播放命令处理
// Parameters	 : NONE
// Returns		 : NONE
//----------------------------------------------------------------------//
void user_play_mode_process(void)
{
    switch (gst_user_play.cmd)
    {
        case USER_PLAY_MODE_ON_OFF_NEXT:
            user_play_mode_process_1();
        break;

        case USER_PLAY_MODE_LOOP:
            user_play_mode_process_2();
        break;

        case USER_PLAY_MODE_PREV:
            user_play_mode_process_3();
        break;

        case USER_PLAY_MODE_NEXT:
            user_play_mode_process_4();
        break;

        case USER_PLAY_MODE_PLAY_STOP:
            user_play_mode_process_5();
        break;

        default:
        break;
    }
}
//----------------------------------------------------------------------------//

#endif // endof #if AUDIO_PLAY_SOUND

#endif  // endof #if USER_PLAY_MODULE