/*******************************************************************************
* File Name     		:	key_scan.c
* Copyright				:	2017 LGW Corporation, All Rights Reserved.
* Module Name			:	keyscan
* Author	    		:	Pillar Peng
* Date of Creat 		:	2017.6.8
* Date of Last Edit		:	2017.6.8
*===============================================================================
* Log    (Detail of Modification)
*===============================================================================
* Code Version    : V01
*   -> Data 	  : 2017.6.8
*   -> History    :
*       1. Initial
*-------------------------------------------------------------------------------
* Code Version    : V02
*   -> Data 	  : 2019-08-30 10:49:29 Fri
*   -> History    :
*       1. 增加了 可以 任意选择 IO 口，做为 按键。
*       2. 增加了 按键睡眠前处理，为了 在 按键不管是按下 还是 松开 都可以睡眠。但是需要在 睡眠前 调用该函数。 void user_key_sleep_process(void)
*       3. 增加了 按键最多只能是 32 个按键。之前是 16 个。
*-------------------------------------------------------------------------------
* Code Version    : V03
*   -> Data 	  : 2019-08-30 11:38:18 Fri
*   -> History    :
*       1. 增加了 LONG_KEY_TIME_CHANGE_EN 选项：就是 每个按键的长按时间可以自定义。比如 KEY1 的长按时间是 1s, KEY2 的可以是 2S。
*-------------------------------------------------------------------------------
* Code Version    : V04
*   -> Data 	  : 2021-12-07 15:01:04 Tue
*   -> History    :
*       1. 修改 所有按键可以单独设置 以下功能：
*          - SINGLE_KEY_TIME_CHANGE_EN      // 单击消抖时间
*          - MULTI_KEY_TIME_CHANGE_EN       // 多击时间间隔
*          - KEY_CLICK_MAX_CHANGE_EN        // 多击次数
*-------------------------------------------------------------------------------
* Code Version    : V05
*   -> Data 	  : 2022-01-18 14:35:33 Tue
*   -> History    :
*       1. 将 按键扫描函数(user_key_scan) 和 按键处理函数(user_key_process) 分开了。
*       - 按键扫描函数(user_key_scan) 放在 RTC 10ms 路径中。 按键扫描的时间更精准了。
*       - 按键处理函数(user_key_process) 放在 主循环中。
*-------------------------------------------------------------------------------
* Code Version    : V06
*   -> Data 	  : 2022-12-13 14:35:33 Tue
*   -> History    :
*       1. 增加了宏KEY_TRIGGER_CONFIG，可选择按键是对地触发还是对VDD触发
*-------------------------------------------------------------------------------
*===============================================================================
* Program Description
*===============================================================================
* Function:
*		1.  按键功能（ 普通按键 / 多功能按键）
*			--> 普通按键
*				- 单击 按下
*				- 单击 释放
*			--> 多功能按键 （多击功能 可扩展(三击，四击都可以的，程序中 是双击)）
*				- 单击 按下
*				- 双击 按下
*				- 双击 释放
*				- 长按 按下
*				- 长按 释放
*
* 	->使用说明
* 		1. 使用前 初始化。 user_key_init();
* 		2. 在调用扫描函数 user_key_scan(); 将其放在 每 10 ms 执行一次的地方。
* 		3. 在 user_key.c 中的 有相应的按键处理程序。下面以 非多功能按键为例：按键按下，和 按键松开。
*       - 按键按下：
*           ```
*           void key_press_process(u16 key_num)
*           {
*           	switch(key_num)
*           	{
*                   // KEY1 处理程序
*           		case 0:
*           			break;
*
*                   // KEY2 处理程序
*           		case 1:
*           			break;
*           	}
*           }
*           ```
*
*       - 按键松开：
*           ```
*           void key_release_process(u16 key_num)
*           {
*           	switch(key_num)
*           	{
*                   // KEY1 处理程序
*           		case 0:
*           			break;
*
*                   // KEY2 处理程序
*           		case 1:
*           			break;
*           	}
*           }
*           ```
*******************************************************************************/
#include "user_key.h"

#if USER_KEY_MODULE

const UserKey_Type lcst_user_key_gpio_a[]=
{
    {.GPIOx = GPIOA, .Pin = 3},
    {.GPIOx = GPIOA, .Pin = 4},
    {.GPIOx = GPIOA, .Pin = 5},
    // {.GPIOx = GPIOA, .Pin = 6},
    // {.GPIOx = GPIOA, .Pin = 7},
    // {.GPIOx = GPIOA, .Pin = 8},
    // {.GPIOx = GPIOA, .Pin = 9},
    // {.GPIOx = GPIOA, .Pin = 5},
    // {.GPIOx = GPIOB, .Pin = 6},
    // {.GPIOx = GPIOB, .Pin = 7},
};

//----------------------------------------------------------------------------//
// 单击的消抖 计数
//----------------------------------------------------------------------------//
u32 gu32_key_deboucne_cnt_a[USER_KEY_MAX_NUM] =
{
    KEY_DEBOUNCE,
};

//----------------------------------------------------------------------------//
// 按键状态变量
// - KEY_STATE_NONE
// - KEY_STATE_PRESS
// - KEY_STATE_RELEASE
// - KEY_STATE_SHORT
// - KEY_STATE_LONG_PRESS
// - KEY_STATE_LONG_RELEASE
// - KEY_STATE_DOUBLE_CLICK
// - KEY_STATE_MAX_CLICK_RELEASE
//----------------------------------------------------------------------------//
u8 gu8_key_state_a[USER_KEY_MAX_NUM] = {0};

#if MULTI_FUNC_KEY
    MuiltFuncKey_Type gst_multi_func_key_a[USER_KEY_MAX_NUM] = {{0},};

    // 多击次数 选择
    #if KEY_CLICK_MAX_CHANGE_EN
    const u32 gst_multi_func_key_max_click_a[USER_KEY_MAX_NUM] =
    {
        1,
        2,
        // 3,
        // 1,
        // KEY_CLICK_MAX,
        // KEY_CLICK_MAX,
        // KEY_CLICK_MAX,
        // KEY_CLICK_MAX,
    };
    #endif

    // 单击消抖时间
    #if SINGLE_KEY_TIME_CHANGE_EN
        const u32 gcu32_key_deboucne_a[USER_KEY_MAX_NUM] =
        {
            KEY_TIMER_CNT_MS(20),
            KEY_TIMER_CNT_MS(500),
            // KEY_TIMER_CNT_MS(1000),
            // KEY_TIMER_CNT_MS(2000),
        };
    #endif

    // 多击时间间隔 选择
    #if MULTI_KEY_TIME_CHANGE_EN
    const u32 gst_multi_func_key_multi_key_time_a[USER_KEY_MAX_NUM] =
    {
        KEY_TIMER_CNT_MS(300),
        KEY_TIMER_CNT_MS(1000),
        // KEY_TIMER_CNT_MS(2000),
        // KEY_TIMER_CNT_MS(4000),
        // LONG_KEY_TIME_DEFAULT,
        // LONG_KEY_TIME_DEFAULT,
        // LONG_KEY_TIME_DEFAULT,
    };
    #endif

    // 长按时间 选择
    // 为什么不把长按的时间放在 结构体 里面呢？ 因为放在 结构体 里面，会占有 ROM 和 RAM. 而拎出来 作为 const 变量，则只消耗 ROM 而且比较少, RAM 不变。
    #if LONG_KEY_TIME_CHANGE_EN
    const u32 gc32_multi_func_key_long_key_time_a[USER_KEY_MAX_NUM] =
    {
        KEY_TIMER_CNT_MS(1000),
        KEY_TIMER_CNT_MS(2000),
        // KEY_TIMER_CNT_MS(3000),
        // KEY_TIMER_CNT_MS(4000),
        // LONG_KEY_TIME_DEFAULT,
        // LONG_KEY_TIME_DEFAULT,
        // LONG_KEY_TIME_DEFAULT,
    };
    #endif
#endif

//----------------------------------------------------------------------------//
// Function Name :	user_key_init
// Description	 :	按键 初始化
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
void user_key_init(void)
{
    int i = 0;

    //----------------------------------------------------------------------------//
    // 按键引脚的初始化
    //----------------------------------------------------------------------------//
    for(i = 0; i < USER_KEY_MAX_NUM; i++)
    {
        #if KEY_TRIGGER_CONFIG == KEY_TRIGGER_GND
            // IO 初始化
            GPIO_Init(lcst_user_key_gpio_a[i].GPIOx, lcst_user_key_gpio_a[i].Pin, GPIO_INPUT_PULLHIGH);
        #elif KEY_TRIGGER_CONFIG == KEY_TRIGGER_VDD
            // IO 初始化
            GPIO_Init(lcst_user_key_gpio_a[i].GPIOx, lcst_user_key_gpio_a[i].Pin, GPIO_INPUT_FLOAT);
        #endif

        // 多功能按键时，初始化变量
        #if MULTI_FUNC_KEY
            gst_multi_func_key_a[i].KeyNum = i;
        #endif

        // debounce 初始化
        gu32_key_deboucne_cnt_a[i] = SINGLE_KEY_TIME_CNT;
    }
}
//----------------------------------------------------------------------------//
// Function Name :	user_key_get_key_mask
// Description	 :	得到按键掩码
// Parameters	 :	@u8 key_max_num：按键数量
// Return		 :	掩码值
//----------------------------------------------------------------------------//
u32 user_key_get_key_mask(u8 key_max_num)
{
    u32 key_mask = 0;
    int i;

    for(i = 0; i < key_max_num; ++i)
    {
        key_mask |= (1UL<<i);
    }

    return key_mask;
}
//----------------------------------------------------------------------------//
// Function Name :	user_key_get_key_value
// Description	 :	得到按键值
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
u32 user_key_get_key_value(void)
{
    u32 key_value = 0;
    u8 bit_value = 0;
    int i;

    for(i = 0; i < USER_KEY_MAX_NUM; ++i)
    {
        bit_value = !GPIO_Read(lcst_user_key_gpio_a[i].GPIOx, lcst_user_key_gpio_a[i].Pin);

        key_value |= bit_value<<i;
    }

    return key_value;
}
//----------------------------------------------------------------------------//
// Function Name :	user_key_get_key_max_num
// Description	 :	得到按键数量
// Parameters	 :	NONE
// Return		 :	按键数量
//----------------------------------------------------------------------------//
u8 user_key_get_key_max_num(void)
{
    return USER_KEY_MAX_NUM;
}
//----------------------------------------------------------------------------//
// Function Name : user_key_sleep_process
// Description   : 睡眠前的按键处理：可以使得 按键在 按下或者松开的情况下，都可以睡眠
// Parameters    : NONE
// Returns       : NONE
//----------------------------------------------------------------------------//
void user_key_sleep_process(void)
{
    u8 bit_value=0;
    int i;

    for(i = 0; i < USER_KEY_MAX_NUM; ++i)
    {
        GPIO_WakeUpCmd(lcst_user_key_gpio_a[i].GPIOx, lcst_user_key_gpio_a[i].Pin, ENABLE);   // PA0

        bit_value = GPIO_Read(lcst_user_key_gpio_a[i].GPIOx, lcst_user_key_gpio_a[i].Pin);
        GPIO_Write(lcst_user_key_gpio_a[i].GPIOx, lcst_user_key_gpio_a[i].Pin, bit_value);
    }
}
//----------------------------------------------------------------------------//
// Function Name :	user_key_scan
// Description	 :	按键 扫描
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
void user_key_scan(void)
{
    int i = 0;

    // volatile u32 current_key=0;

    volatile u8 current_key_a[USER_KEY_MAX_NUM] = {0};
    static u8 last_key_a[USER_KEY_MAX_NUM] = {0};
    static u8 last_valid_key_a[USER_KEY_MAX_NUM] = {0};
    static u8 fisrt_flag_a[USER_KEY_MAX_NUM] = {0};

    // 获取当前键值
    for(i = 0; i < USER_KEY_MAX_NUM; ++i)
    {
        #if KEY_TRIGGER_CONFIG == KEY_TRIGGER_GND
            current_key_a[i] = !GPIO_Read(lcst_user_key_gpio_a[i].GPIOx, lcst_user_key_gpio_a[i].Pin);
        #elif KEY_TRIGGER_CONFIG == KEY_TRIGGER_VDD
            current_key_a[i] = GPIO_Read(lcst_user_key_gpio_a[i].GPIOx, lcst_user_key_gpio_a[i].Pin);
        #endif
    }

    // 判断键值
    for(i = 0; i < USER_KEY_MAX_NUM; ++i)
    {
        // 获取键值
        // current_key_a[i] = !GPIO_Init(lcst_user_key_gpio_a[i].GPIOx, lcst_user_key_gpio_a[i].Pin);

        // 比较键值
        if(last_key_a[i] != current_key_a[i]) // 读取当前键值
        {
            last_key_a[i] = current_key_a[i];

            gu32_key_deboucne_cnt_a[i] = SINGLE_KEY_TIME_CNT;

        }

        // 消抖处理
        if(gu32_key_deboucne_cnt_a[i])
        {
            if(0 == --gu32_key_deboucne_cnt_a[i])
            {
                if(
                    (current_key_a[i] ^ last_valid_key_a[i])                    // 说明 i 键值 有变化
                    || 0 == fisrt_flag_a[i]                                     // 如果是上电第一次则相应
                )
                {
                    fisrt_flag_a[i] = 1;                                        // 设置第一次响应标志

                    // 有键按下
                    if(current_key_a[i])
                    {
                        #if NORMAL_FUNC_KEY
                            // key_press_process(i);
                            key_state_set(i, KEY_STATE_PRESS);
                        #endif

                        #if MULTI_FUNC_KEY
                            multi_func_key_press_process(&gst_multi_func_key_a[i]);
                        #endif

                    }
                    // 按键释放
                    else
                    {
                        #if NORMAL_FUNC_KEY
                            // key_release_process(i);
                            key_state_set(i, KEY_STATE_RELEASE);
                        #endif

                        #if MULTI_FUNC_KEY
                            multi_func_key_release_process(&gst_multi_func_key_a[i]);
                        #endif

                    }
                }

                last_valid_key_a[i] = current_key_a[i]; // 记录按键状态
            }
        }

    #if MULTI_FUNC_KEY
        multi_func_key_timer_count_func(&gst_multi_func_key_a[i]);
    #endif
    }
}
//----------------------------------------------------------------------------//
// Function Name :	user_key_check_status
// Description	 :	检查 按键是否稳定
// 					包括 debounce 是否为零； 长按/多击 是否还在击数
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
u8 user_key_check_status(void)
{
    int i = 0;

	u8 flag = 0;

	#if MULTI_FUNC_KEY
		for(i = 0; i < USER_KEY_MAX_NUM; ++i)
		{
			if(
				   gst_multi_func_key_a[i].LongPress_TimeCntFlag 					// 当 长按计时 时， 说明按键正在工作
				|| gst_multi_func_key_a[i].MultiClick_TimeCntFlag					// 当 多击计时 时， 说明按键正在工作

                // || gst_multi_func_key_a[i].KeyClick_Count                         // 只有 按键没有松开，则算正在工作
                // || gst_multi_func_key_a[i].LongPress_TimeCount                    // 只有 按键没有松开，则算正在工作
                // || gst_multi_func_key_a[i].LongPress_TimeCount                    // 只有 按键没有松开，则算正在工作
            )
			{																		 // 按键 正在工作时 flag = 1
				flag = 1;

                return STATUS_WORKING;
			}
		}
	#endif

    for(i = 0; i < USER_KEY_MAX_NUM; ++i)
    {
        if(
               0 != gu32_key_deboucne_cnt_a[i]									    // 当 debounce 不为零时，说明 按键不状态稳定
            || 0 != flag															// 当 长按标志·多击标志 不为零时，说明 按键不稳定
        )
        {
            return STATUS_WORKING;
        }
    }

    return STATUS_IDLE;
}

//----------------------------------------------------------------------------//
// Function Name : key_state_check
// Description   : 按键状态检测，然后清除该按键的状态
// Parameters    :
//                 @u8 num： 按键键值
//                 @u8 sta： 按键状态
//                      - KEY_STATE_NONE
//                      - KEY_STATE_PRESS
//                      - KEY_STATE_RELEASE
//                      - KEY_STATE_SHORT
//                      - KEY_STATE_LONG_PRESS
//                      - KEY_STATE_LONG_RELEASE
//                      - KEY_STATE_DOUBLE_CLICK
//                      - KEY_STATE_MAX_CLICK_RELEASE
// Returns       :
//                  @u8 ：是否有该按键状态 1：有； 0：无
//----------------------------------------------------------------------------//
u8 key_state_check(u8 num, u8 sta)
{
    u8 flag;

    flag = gu8_key_state_a[num]&sta;    // 检测 是否有这个状态

    flag?key_state_clear(num, sta):0;   // 有，则清除； 无，不处理

    return flag;                        // 返回 是否有该状态 1：有； 0：无
}
//----------------------------------------------------------------------------//
// Function Name :	user_key_process
// Description	 :	按键触发处理
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
void user_key_process(void)
{
    int i;

    for(i = 0; i < USER_KEY_MAX_NUM; i++)
    {
        // 如果按键没有任何的状态变化， 则退出当前循环
        if(0 == gu8_key_state_a[i])
        {
            continue;
        }

    #if NORMAL_FUNC_KEY
        // 短按
        if(key_state_check(i, KEY_STATE_PRESS))
        {
            key_press_process(i);
        }

        // 短按释放
        if(key_state_check(i, KEY_STATE_RELEASE))
        {
            key_release_process(i);
        }
    #endif

    #if MULTI_FUNC_KEY
        // 多功能按键
        // 单击
        if(key_state_check(i, KEY_STATE_SHORT))
        {
            key_short_process(i);
        }

        // 长按
        if(key_state_check(i, KEY_STATE_LONG_PRESS))
        {
            key_long_press_process(i);
        }
        // 长按释放
        if(key_state_check(i, KEY_STATE_LONG_RELEASE))
        {
            key_long_release_process(i);
        }

        // 双击
        if(key_state_check(i, KEY_STATE_DOUBLE_CLICK))
        {
            key_double_click_process(i);
        }
        // // 最大击数释放
        // if(key_state_check(i, KEY_STATE_MAX_CLICK_RELEASE))
        // {
        //     key_max_click_release_process(i);
        // }
    #endif
    }
}

#if NORMAL_FUNC_KEY
//----------------------------------------------------------------------------//
// 普通按键 按下 处理
//----------------------------------------------------------------------------//
void key_press_process(u8 key_num)
{
    switch(key_num)
    {
        case 0:
            #if USER_PLAY_MODULE
                user_play_audio(1, 0);
            #endif
            break;

        case 1:
            #if USER_PLAY_MODULE
                user_play_audio(1, 1);
            #endif
            break;
    }
}
//----------------------------------------------------------------------------//
// 普通按键 释放 处理
//----------------------------------------------------------------------------//
void key_release_process(u8 key_num)
{
    switch (key_num)
    {
        case 0:
            break;

        case 1:
            break;
    }
}
#endif

#if MULTI_FUNC_KEY
//----------------------------------------------------------------------------//
// 短按/单击 处理
//----------------------------------------------------------------------------//
void key_short_process(u8 key_num)
{
    switch (key_num)
    {
		case 0:
            user_system_set_status(STATUS_WORKING);

            // 语音识别
            #if _VR_FUNC
                if(VR_CMD_OFF == user_vr_get_cmd())
                {
                    user_vr_set_cmd(VR_CMD_GRP1);
                }
            #endif

            // 播放0~10
            #if USER_PLAY_MODULE
                user_set_sound_type(SOUND_TYPE_SONG);
                user_play_mode_set_cmd(USER_PLAY_MODE_ON_OFF_NEXT);
            #endif

            user_led_set_cmd(LED_CMD_ALL_OFF);
			break;

		case 1:
            user_system_set_status(STATUS_WORKING);
            user_system_stop_all_audio_module();
            #if AUDIO_PLAY_SOUND
                user_play_mode_set_cmd(USER_PLAY_MODE_SINGLE_NO_PLAY);
            #endif

            // 语音识别
            #if _VR_FUNC
                if(VR_CMD_OFF == user_vr_get_cmd())
                {
                    user_vr_set_cmd(VR_CMD_GRP1);
                }
            #endif

            // 播放一首音乐
            #if USER_PLAY_MODULE
                user_play_audio(1, 15);
            #endif
			break;
    }
}

//----------------------------------------------------------------------------//
// 长按 处理
//----------------------------------------------------------------------------//
void key_long_press_process(u8 key_num)
{
    switch (key_num)
    {
		case 0:
            // 关机
            user_system_set_status(STATUS_IDLE);

            #if USER_PLAY_MODULE
                user_play_audio(1, AUD_POWER_OFF);
            #endif
			break;

		case 1:

            break;
    }
}
//----------------------------------------------------------------------------//
// 长按释放 处理
//----------------------------------------------------------------------------//
void key_long_release_process(u8 key_num)
{
    switch (key_num)
    {
		case 0:
			break;

		case 1:

			break;
    }
}
//----------------------------------------------------------------------------//
// 双击 处理
//----------------------------------------------------------------------------//
void key_double_click_process(u8 key_num)
{
    // switch (key_num)
    // {
    //     case 0:
    //         break;
    // }
}
//----------------------------------------------------------------------------//
// 做大多击数后的释放 处理
//----------------------------------------------------------------------------//
void key_max_click_release_process(u8 key_num)
{
    // switch (key_num)
    // {
    //     case 0:
    //         break;
    // }
}
#endif

#if MULTI_FUNC_KEY
//----------------------------------------------------------------------------//
// 多功能 按键 相关函数
//----------------------------------------------------------------------------//
//----------------------------------------------------------------------------//
// Function Name :	multi_func_key_clear_key_parameter
// Description	 :	清楚相应按键的参数
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
void multi_func_key_clear_key_parameter(MuiltFuncKey_Type* st_mkey)
{
	// 生成有效值之后，清零所有（除了键值）
	st_mkey->KeyClick_Count = 0;

	st_mkey->LongPress_TimeCntFlag = 0;
	st_mkey->MultiClick_TimeCntFlag = 0;

	st_mkey->MultiClick_TimeCount = 0;
	st_mkey->LongPress_TimeCount = 0;
}

//----------------------------------------------------------------------------//
// Function Name :	multi_func_key_press_process
// Description	 :	多功能按键， 有键按下时的处理
// Parameters	 :
// 					MuiltFuncKey_Type *st_mkey
// Return		 :	NONE
//----------------------------------------------------------------------------//
void multi_func_key_press_process(MuiltFuncKey_Type *st_mkey)
{
	st_mkey->MultiClick_TimeCntFlag = 0;										// 单击时，清除 双击间隔时间 计时标志，停止计时
	st_mkey->MultiClick_TimeCount = 0;											// 清零 间隔计时

	++st_mkey->KeyClick_Count;													// 按键计数

	if(1 == st_mkey->KeyClick_Count)											// 第一次按键
	{
        #if LONG_FUNC_KEY
            // 长按
            st_mkey->LongPress_TimeCntFlag = 1;										// 置位 长按计时标志位
            st_mkey->LongPress_TimeCount = 0;										// 清零 长按计数
        #endif
	}
	else if(KEY_CLICK_MAX == st_mkey->KeyClick_Count)							// 按键最大的多击数
	{
		multi_func_key_multi_click_process(st_mkey);
	}
}
//----------------------------------------------------------------------------//
// Function Name :	multi_func_key_release_process
// Description	 :	多功能按键， 有键释放时的处理
// Parameters	 :
// 					MuiltFuncKey_Type *st_mkey
// Return		 :	NONE
//----------------------------------------------------------------------------//
void multi_func_key_release_process(MuiltFuncKey_Type *st_mkey)
{
	if(1 == KEY_CLICK_MAX)														// 如果最大击数是 1， 则
	{
		if(1 == st_mkey->KeyClick_Count)										//
		{
            #if LONG_FUNC_KEY

			// 长按
			st_mkey->LongPress_TimeCntFlag = 0;									// 释放按键后，停止长按计数

			if(st_mkey->LongPress_TimeCount >= LONG_KEY_TIME_CNT)					// 如果 按键时间超过长按时间，则属于 长按，这里是释放，所以是长按释放
			{
				// multi_func_key_long_release_process(st_mkey);				// 长按 释放 处理函数
                key_state_set(st_mkey->KeyNum, KEY_STATE_LONG_RELEASE);         // 长按 释放 设置标志位
			}
			else																// 如果不是长按，则是单击
            #endif
			{
				multi_func_key_multi_click_process(st_mkey);
			}

			multi_func_key_clear_key_parameter(st_mkey);							// 清除 按键的部分参数
		}
	}
	else
	{
		if(1 == st_mkey->KeyClick_Count)										// 多击 按键释放时，开始计数	// 只有当 是第一次按键时才有效
		{
            #if LONG_FUNC_KEY
			// 长按
			st_mkey->LongPress_TimeCntFlag = 0;									// 释放按键后，停止长按计数

			if(st_mkey->LongPress_TimeCount >= LONG_KEY_TIME_CNT)					// 如果 按键时间超过长按时间，则属于 长按，这里是释放，所以是长按释放
			{
				// multi_func_key_long_release_process(st_mkey);				// 长按 释放 处理函数
                key_state_set(st_mkey->KeyNum, KEY_STATE_LONG_RELEASE);         // 长按 释放 设置标志位

				multi_func_key_clear_key_parameter(st_mkey);						// 清除 按键的部分参数
			}
			else																// 如果不是长按，则是单击
            #endif
			{
				st_mkey->MultiClick_TimeCntFlag = 1;							// 释放按键后，开始多击间隔延时计数
				st_mkey->MultiClick_TimeCount = 0;								// 清零 间隔计数
			}
		}
		else if(KEY_CLICK_MAX != st_mkey->KeyClick_Count)						// 如果不是 最大多击数 则
		{
			st_mkey->MultiClick_TimeCntFlag = 1;								// 释放按键后，开始多击间隔延时计数
			st_mkey->MultiClick_TimeCount = 0;									// 清零 间隔计数
		}
		else if (KEY_CLICK_MAX == st_mkey->KeyClick_Count)						// 如果是 最大多击数 则
		{
			// multi_func_key_max_click_release_process(st_mkey);                  // 进入 最大击数 release 处理
            key_state_set(st_mkey->KeyNum, KEY_STATE_MAX_CLICK_RELEASE);                // 进入 最大击数 release 设置标志位
			multi_func_key_clear_key_parameter(st_mkey);						// 清除 按键的部分参数
		}
	}
}
//----------------------------------------------------------------------------//
// Function Name :	multi_func_key_timer_count_func
// Description	 :	多功能按键， 长按和多击时的计时处理
// Parameters	 :
// 					MuiltFuncKey_Type *st_mkey
// Return		 :	NONE
//----------------------------------------------------------------------------//
void multi_func_key_timer_count_func(MuiltFuncKey_Type *st_mkey)
{
    #if LONG_FUNC_KEY
        // 只有当第一次按键时，才判断 长按
        if(1 == st_mkey->KeyClick_Count)
        {
            // 长按判断
            if(st_mkey->LongPress_TimeCntFlag)										// 长按计时开始
            {
                if(++st_mkey->LongPress_TimeCount >= LONG_KEY_TIME_CNT)					// 长按计时, 时间超过 长按时间 ，则为长按
                {
                    st_mkey->LongPress_TimeCntFlag = 0;								// 清除 长按计时标志

                    st_mkey->LongPress_TimeCount = LONG_KEY_TIME_CNT;					// 如果长按

                    // 有效长按 处理
                    // multi_func_key_long_press_process(st_mkey);						// 长按 按下 处理函数
                    key_state_set(st_mkey->KeyNum, KEY_STATE_LONG_PRESS);
                }
            }
        }
    #endif

	// 多击判断
	if(st_mkey->MultiClick_TimeCntFlag)											// 间隔时间 计时标志
	{
		if(++st_mkey->MultiClick_TimeCount >= MULTI_KEY_TIME_CNT)					// 超过 interval 时间， 没有按键 则是短按
		{
			multi_func_key_multi_click_process(st_mkey);

			multi_func_key_clear_key_parameter(st_mkey);							// 清除 按键的部分参数
		}
	}
}

//----------------------------------------------------------------------------//
// Function Name :	multi_func_key_multi_click_process
// Description	 :	多功能按键 多击处理 (按下)
// Parameters	 :
// 					MuiltFuncKey_Type* st_mkey
// 					主要使用两个参数
// 					 - st_mkey.KeyNum
// 					 - st_mkey.KeyClick_Count
// Return		 :	NONE
//----------------------------------------------------------------------------//
void multi_func_key_multi_click_process(MuiltFuncKey_Type* st_mkey)
{
	switch(st_mkey->KeyClick_Count)
	{
		// 单击
		case SINGLE_CLICK:
            key_state_set(st_mkey->KeyNum, KEY_STATE_SHORT);
			break;

		// 双击
		case DOUBLE_CLICK:
            key_state_set(st_mkey->KeyNum, KEY_STATE_DOUBLE_CLICK);
			break;

		// // 三击
		// case TRIPLE_CLICK:
        //     key_state_set(st_mkey->KeyNum, KEY_STATE_TRIPLE_CLICK);
		// 	break;
	}
}
#endif // endif #if MULTI_FUNC_KEY

#endif // end of #if USER_KEY_MODULE