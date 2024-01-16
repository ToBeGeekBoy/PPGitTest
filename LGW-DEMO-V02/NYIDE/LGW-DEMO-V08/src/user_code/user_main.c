/*******************************************************************************
* File Name     		:	main.c
* Copyright				:	2020 LGW Corporation, All Rights Reserved.
* Update Disctription   :
*                           1. 10 ms 定时路径的修改
*                              1) 将 user_main.c 中的 函数 user_timebase_from_rtc_isr(SYS_TICK_COUNT) 在 void TIMEBASE_ServiceLoop(void) 函数中调用， 文件是 isr.c
*                              2) user_timebase_from_rtc_isr(SYS_TICK_COUNT) 需要放在中断是 1ms 的 中断处理中。
*                           2. IC body 是 NX11P22 以及 NX12P44时，需要添加 _PP_IDLE_INTO_HALT 的引脚设置 防止大电流， 就是在 IC 睡眠前，输出一个20MS 左右的高脉冲到 PP1引脚，参考上一个版本。
*                              - 特别注意：一定全局搜索上一个版本的 _PP_IDLE_INTO_HALT 宏定义，以此来修改。偶尔会有一些东西增加或者删除。
*                           3. IC 睡眠功能，使用自定义睡眠功能。System_CheckPowerOff()。
*                               需要修改：
*                                -1) _AUDIO_IDLE_INTO_HALT 为 DISABLE, 在 nx1_config.h 中
*                                -2) 在 sys.c 文件 的 void SYS_ServiceLoop(void) 函数里面修改。 Line 667
*                                    原代码：#if _AUDIO_IDLE_INTO_HALT || (_TOUCHKEY_MODULE && _TOUCHKEY_INTO_STANDBY)
*                                    修改为：#if _AUDIO_IDLE_INTO_HALT
*                           4. 在 进入睡眠(halt/slow-mode)前，再做一次睡眠状态检测。 在 sys.c 中 的 SYS_Sleep() 函数中修改。如下：
*                                   ```
*                                   //----------------------------------------------------------------------------//
*                                   // 再次睡眠的状态检测，看是否满足睡眠条件  -  add by pp
*                                   // 注：
*                                   //    为了防止：通信时，刚进入刚函数，然后接收到了一个通信码(中断接收)，没有被处理，然后就睡眠了。
*                                   //    所以，通信时，需要添加，一定是通信码被处理后并且没有正在通信，才可以睡眠。
*                                   //----------------------------------------------------------------------------//
*                                   if(STATUS_IDLE == user_system_get_status_for_sleep())
*                                   {
*                                   ///////////////////////////////////////////////////////////////////////////////////////
*                                       // standby or halt mode
*                                   #if	(_TOUCHKEY_MODULE && _TOUCHKEY_INTO_STANDBY)
*                                       //into touch Scan at standby mode
*                                       Touch_IntoStandBy();
*                                   #else
*                                   #if body_MultiTrgLdo
*                                       SYS_MULTI_TRG_LDO();
*                                   #else
*                                       //into halt mode
*                                       SMU_Sleep(C_SLP_Halt_Mode);
*                                   #endif
*                                   #endif
*                                   ///////////////////////////////////////////////////////////////////////////////////////
*                                   }
*                                   ```
*                           5. touch 模块的长按功能的添加 以及 睡眠处理修改 - 看情况修改
*                               -1) touch 长按功能的模块使能宏定义。 在 nx1_config.h 的 _TOUCHKEY_MODULE 下面，具体如下：
*                                   #define TOUCHKEY_LONGKEY            ENABLE                  // 是否 开启触摸长按功能  disable or enable
*                                   #define TOUCH_LONG_PRESS_TIME_CNT   (100)                   // 触摸长按的时间 单位是 0.01 秒就是 10 ms. 100*10ms = 1s
*                               -2) 长按代码的添加 在 touch_callback.c, 详情看文件。
*                               -3) 将 长按计时的 函数，放在 user_main.c 的 user_main_service_loop() 函数中调用。
*                                   ```
*                                   // 触摸长按 计时
*                                   #if (_TOUCHKEY_MODULE && TOUCHKEY_LONGKEY)
*                                       CB_Touch_LongPress_Timer();
*                                   #endif
*                                   ```
*                               -4) 长按处理的函数，在 touch_callback.c 文件中。
*                               -5) 睡眠处理注意：
*                                   1.在使用九齐自带触摸按键的睡眠函数， 语音识别必须是 停止的，才可以进去睡眠。
*                                   2.使用 LGW 自定义的睡眠函数，与语音识别的状态没有关系。
*                                   3.本源码是使用 LGW 自定义的睡眠函数
*                               -6) 触摸睡眠时，作为非触摸 IO 的唤醒问题处理。
*                                   1.触摸睡眠中，非触摸 IO 口，是不能直接跳出 触摸的睡眠函数的。
*                                   2.该问题的处理函数在 ：CB_StandByScan() 函数中的 u8 User_Touch_GetWakeUpFlag_FromGPIO(void)
*                       6. 为了解决打开DAC模式时,睡眠不能正常Ramp_Down,唤醒不能正常Ramp_up,开启多个通道使用DAC输出时会出现的死机问题,
*                          把进入睡眠的处理放在sys.c文件的SYS_ServiceLoop()里
*                       7. 修改user_main.c文件中user_timebase_from_rtc_isr（）函数，解决定时不准问题
*                       8. 修改user_vr.c文件中user_vr_service_loop（）函数。选择VR触发模式，并处于组2状态时，使用gu8_vr_flag_10ms定时，确保是声音播放完之后再进行计时回到组1。
*                       9. 修改user_timebase_from_rtc_isr()函数，母体不是NX12FS时，且没有录音和录变音动作时，在 isr.c 中的 TIMEBASE_ServiceLoop()函数中1ms处调用。
*                          母体为NX12/13FS时且打开IRAM和nx1_config.h文件里的宏_SBC_RECORD_ERASING、_ADPCM_RECORD_ERASING时，且有进行录音和录变音动作时，不会调用此函数。
*                       10. 增加user_timebase_from_rtc_isr_1ms_iram()函数，功能与user_timebase_from_rtc_isr()函数相同，母体为NX12/13FS时且打开IRAM和nx1_config.h文件里
*                           的宏_SBC_RECORD_ERASING、_ADPCM_RECORD_ERASING时，且有进行录音和录变音动作时，在record_erasing_iram1.c 中的 TIMEBASE_IRAM_ServiceLoop()函数中1ms处调用，
*                           其他情况下不会调用此函数。
*                       11.需要将此程序修改成母体为NX12FS，且nx1_config.h文件里的宏_SBC_RECORD_ERASING、_ADPCM_RECORD_ERASING时，需要将NYIDE的congfig选项中需要将IRAM打开
*                       12.将改好的母体为NX12FS的程序烧录FDB板上时，进入睡眠时静态电流有11uA，这是由于板子上接着flash有关，把flash卸掉，静态电流就是4uA
*                       13.增加USER_STORAGE_BREAK_POINT_MOUDLE模块，可以播放自定义断点和存储自定义变量，目前只支持SBC算法的断点续播。
*                       14.取消user_system_get_status_for_sleep()函数中判断触摸按键是否进入睡眠的语句，解决触摸按键空闲时计时不能进入睡眠的问题
* Author	    		:	Pillar Peng
*===============================================================================
* Log    (Detail of Modification)
*===============================================================================
* Code Version    : V01
*   -> Data 	  : 2024-01-15 10:33:37 Mon
*   -> History    :
*       1. Initial
*-------------------------------------------------------------------------------
* Code Version    : V02
*   -> Data 	  : 2024-01-15 11:20:32 Mon
*   -> History    :
*       1. change prj name
*-------------------------------------------------------------------------------
* Code Version    : V03
*   -> Data 	  : 2024-01-15 11:20:32 Mon
*   -> History    :
*       1. change prj name
*-------------------------------------------------------------------------------
* Code Version    : V04
*   -> Data 	  : 2024-01-15 15:46:34 Mon
*   -> History    :
*       1. BR1-1
*-------------------------------------------------------------------------------
* Code Version    : V04
*   -> Data 	  : 2024-01-15 15:47:51 Mon
*   -> History    :
*       1. BR1-2
*-------------------------------------------------------------------------------
* Code Version    : V04
*   -> Data 	  : 2024-01-15 15:47:51 Mon
*   -> History    :
*       1. BR1-3
*-------------------------------------------------------------------------------
* Code Version    : V05
*   -> Data 	  : 2024-01-15 15:54:23 Mon
*   -> History    :
*       1. BR1-3 merge with br1
*-------------------------------------------------------------------------------
* Code Version    : V06
*   -> Data 	  : 2024-01-15 15:58:19 Mon
*   -> History    :
*       1. v06
*-------------------------------------------------------------------------------
* Code Version    : V07
*   -> Data 	  : 2024-01-15 17:36:21 Mon
*   -> History    :
*       1. v07
*-------------------------------------------------------------------------------
* Code Version    : V07 merge with V06-MOTION-2
*   -> Data 	  : 2024-01-15 15:58:19 Mon
*   -> History    :
*       1. 增加 MOTION 模块
*       2. 按键 增加 motion 动作
*       3. 修改动作
*-------------------------------------------------------------------------------
* Code Version    : Merge branch 'V06-LED' into user-main
*   -> Data 	  : 2024-01-15 17:50:41 Mon
*   -> History    :
*       1. 增加 LED 模块
*       2. 按键处，添加 LED 处理
*       3. 更改 LED 引脚
*-------------------------------------------------------------------------------
* Code Version    : V07-1
*   -> Data 	  : 2024-01-15 17:54:15 Mon
*   -> History    :
*       1. 编译
*-------------------------------------------------------------------------------
* Code Version    : V08
*   -> Data 	  : 2024-01-15 17:55:24 Mon
*   -> History    :
*       1. v08
*-------------------------------------------------------------------------------
* Code Version    : V06-MOTION-3
*   -> Data 	  : 2024-01-15 18:02:47 Mon
*   -> History    :
*       1. 修改动作
*-------------------------------------------------------------------------------
* Code Version    : V09
*   -> Data 	  : 2024-01-16 11:39:38 Tue
*   -> History    :
*       1. new
*-------------------------------------------------------------------------------
*===============================================================================
* Program Description
*===============================================================================
* Function:
*
* Project NO. : LGW-demo-v02
* IC BODY     : NX11P21A + Flash(8 Mbit)
*
* I/O :
*       ----------------------   KEY   ------------------------------
*     ==>PA3 : 输入上拉 对地触发 按键 短按：ON/OFF 下一曲     长按：睡眠
*     ==>PA4 : 输入上拉 对地触发 按键 短按：播放歌曲          长按：    松开：
*       ----------------------   SPI0(NX1-OTP) - SPI_1_1_1_MODE   -----------------
*     ==>PB0  : SPI0 CS
*     ==>PB1  : SPI0 CLK
*     ==>PB2  : SPI0 MOSI/IO0
*     ==>PB3  : SPI0 MISO/IO1
*     ==>PB4  : SPI0 WP/IO2
*     ==>PB5  : SPI0 HOLE/IO3
*
*******************************************************************************/

#include "user_main.h"

u8 gu8_flag_10ms    = 0;                                                        // 10ms 标志位 按鍵扫描 使用         置位的操作在 RTC_ISR() 函数中，该函数在 isr.c 文件中

static u8 lu8_work_status = STATUS_IDLE;                                        // 工作状态
static u32 lu32_work_idle_cnt = 0;                                              // 空闲计数

#if _STORAGE_MODULE && !USER_STORAGE_BREAK_POINT_MOUDLE
    //需要存储变量数组
    u8 gu8_sto_buf[_STORAGE_VAR_CNT];
#endif

//----------------------------------------------------------------------------//
// 用户计时相关 - user systick
//----------------------------------------------------------------------------//
volatile u32 gu32_user_systick_ms=0;
//----------------------------------------------------------------------------//
// 10ms定时计数
//----------------------------------------------------------------------------//
u32 gu32_user_10ms_count = 0;
//----------------------------------------------------------------------------//
// Function Name : user_systick_clear
// Description   : 用户 计时 清除
// Parameters    :
//                  @u32 *timer_cnt : 计时变量的地址
// Returns       : NONE
//----------------------------------------------------------------------------//
void user_systick_clear(u32 *timer_cnt)
{
    *timer_cnt = gu32_user_systick_ms;
}
//----------------------------------------------------------------------------//
// Function Name : user_systick_get_time
// Description   : 获取 用户计时的时长。
// Parameters    :
//                  @u32 *timer_cnt : 计时变量的地址
// Returns       :
//                  @u32 : 返回 用户计时的时长 - 单位： ms
//----------------------------------------------------------------------------//
u32 user_systick_get_time(u32 *timer_cnt)
{
    return (u32)(gu32_user_systick_ms-*timer_cnt);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Function Name : user_timebase_from_rtc_isr
// Description   : 母体不是NX12FS时，没有进行录音和录变音动作时，在 isr.c 中的 TIMEBASE_ServiceLoop()函数中1ms处调用。
//                 母体为NX12/13FS时且打开IRAM和nx1_config.h文件里的宏_SBC_RECORD_ERASING、_ADPCM_RECORD_ERASING时，
//                 且有进行录音和录变音动作时，不会调用此函数
// Parameters    : NONE
// Returns       : NONE
//----------------------------------------------------------------------------//
void user_timebase_from_rtc_isr(void)
{
    // user systick - 用户 1ms 时基
    ++gu32_user_systick_ms;

    if(++gu32_user_10ms_count >= 10)
    {
        gu32_user_10ms_count = 0;

        gu8_flag_10ms = 1; // 10ms 标志位 按鍵扫描 使用. 置位的操作在 RTC_ISR() 函数中，该函数在 isr.c 文件中

        #if _VR_FUNC
            gu8_vr_flag_10ms = 1;
        #endif

        // 按键扫描，当按键需要精准定时时才放在这里
        // #if USER_KEY_MODULE
            // user_key_scan();
        // #endif
    }
}
//----------------------------------------------------------------------------//
// Function Name : user_timebase_from_rtc_isr_1ms_iram
// Description   : 2023-07-12修改，母体不是NX12FS时，没有进行录音和录变音动作时，不会调用此函数
//                 母体为NX12/13FS时且打开IRAM和nx1_config.h文件里的宏_SBC_RECORD_ERASING、_ADPCM_RECORD_ERASING时，
//                 且有进行录音和录变音动作时，在record_erasing_iram1.c 中的 TIMEBASE_IRAM_ServiceLoop()函数中1ms处直接处理。
// Parameters    : NONE
// Returns       : NONE
//----------------------------------------------------------------------------//
// void user_timebase_from_rtc_isr_1ms_iram(void)
// {
//     // user systick - 用户 1ms 时基
//     ++gu32_user_systick_ms;

//     if(++gu32_user_10ms_count >= 10)
//     {
//         gu32_user_10ms_count = 0;

//         gu8_flag_10ms = 1; // 10ms 标志位 按鍵扫描 使用. 置位的操作在 RTC_ISR() 函数中，该函数在 isr.c 文件中

//         #if _VR_FUNC
//             gu8_vr_flag_10ms = 1;
//         #endif

//         // 按键扫描，当按键需要精准定时时才放在这里
//         // #if USER_KEY_MODULE
//             // user_key_scan();
//         // #endif
//     }
// }
//----------------------------------------------------------------------//
// Function Name :
// Description   : timer1 定时 10 ms TIMER0 用于播放声音了
//                 由于 在 NYIDE | Project Information | Config Block 中设置了 电源电压为3.0，
//                 所以 主频最高由 32M 变成了 16M。所以 定时器的 分屏要编程原来的一半
//
// Parameters    : NONE
// Returns       : NONE
//----------------------------------------------------------------------//
void user_timer1_init(void)
{
    #if _EF_SERIES
        TIMER_Init(TIMER1, 10000-1);                                            // TIMER2 初始化， 分频32， 向下计数：9999+1  ，内部hiclk (1/(32M/32))*10000 = 10ms
    #else
        TIMER_Init(TIMER1, TIMER_CLKDIV_32, 10000-1);                           // TIMER2 初始化， 分频32， 向下计数：9999+1  ，内部hiclk (1/(32M/32))*10000 = 10ms
    #endif

    TIMER_IntCmd(TIMER1, ENABLE);                                               // 开启定时器中断
    TIMER_Cmd(TIMER1, ENABLE);                                                  // 打开定时器1
}
//----------------------------------------------------------------------------//
// Function Name : user_system_get_status_for_sleep
// Description   : 获取是否可以睡眠的状态 - 检测睡眠前的各个模块状态，
// Parameters    : NONE
// Returns       :
//                  @u8
//                   - STATUS_IDLE    - 可以睡眠了
//                   - STATUS_WORKING - 还不能睡眠
//----------------------------------------------------------------------------//
u8 user_system_get_status_for_sleep(void)
{
    if(
        1
        && STATUS_IDLE == user_system_get_status()

        #if USER_PLAY_MODULE
            &&
            (
                STATUS_STOP == user_play_get_all_status()                            // 当 声音停止
                // || STATUS_PAUSE == user_play_get_status()                            // 当 声音暂停
            )
        #endif

        #if USER_KEY_MODULE
            && STATUS_IDLE == user_key_check_status()                                // 当 按键稳定时
        #endif

        //----------------------------------------------------------------------------//
        // 添加以下语句会导致触摸空闲时不能计时进入睡眠的问题 2023-09-14
        //----------------------------------------------------------------------------//
        // #if (_TOUCHKEY_MODULE && _TOUCHKEY_INTO_STANDBY)
            // && (!Touch_GetPADStatus())
            // && (Touch_GetSleepFlag())
        // #endif

        #if USER_TALKBACK_MODULE
            && DISABLE == user_talk_back_get_cmd()                              // 即录即放停止状态
        #endif

        #if USER_RECORD_MODULE
            && USER_RECORD_CMD_DISABLE == user_record_get_cmd()                 // 当 录音 稳定时
        #endif

        #if USER_LED_MODULE
            && LED_CMD_ALL_OFF == user_led_get_cmd()                            // 当 LED 关闭
        #endif

        #if USER_MOTION_MODULE
            && MOTION_STOP == user_motion_get_cmd()                             // 当 动作 停止
        #endif

        #if USER_OUTPUT_MODULE_EN
            && STATUS_IDLE == user_output_get_status()                          // 当 输出模块 稳定
        #endif

        #if _VR_FUNC
            && VR_CMD_OFF == user_vr_get_cmd()                                  // 当 VR 关闭
        #endif

        #if USER_STORAGE_BREAK_POINT_MOUDLE
            && STATUS_IDLE == user_storage_get_work_status()
        #endif
    )
    {
        return STATUS_IDLE;
    }

    return STATUS_WORKING;
}
//----------------------------------------------------------------------//
// Function Name : user_system_sleep_check
// Description   :
//                  判断系统的各个模块是否全部都是空闲状态
//                  只要有一个 不是空闲状态，则系统都不是 空闲状态，属于工作状态
// Parameters    : NONE
// Returns       : NONE
//----------------------------------------------------------------------//
void user_system_sleep_check(void)
{

    if(STATUS_WORKING == user_system_get_status())                               // 当 系统工作时， 退出
    {
        return;
    }
    if(STATUS_IDLE == user_system_get_status_for_sleep())
    {
        // 无动作时间 超过 scantime*TIME_POWER_DOWN = 10ms*(10) = 100ms
        if(user_systick_get_time(&lu32_work_idle_cnt) >= SYSTEM_SLEEP_TIME)
        {
            user_systick_clear(&lu32_work_idle_cnt);                            // 清零 时间计数
            user_system_off_process();
        }
    }
    else                                                                        // 只要 一有动作，马上清零之前的 计数
    {
        user_systick_clear(&lu32_work_idle_cnt);
    }
}
//----------------------------------------------------------------------------//
// Function Name : user_system_idle_check
// Description   : 系统空闲检测
// Parameters    : NONE
// Returns       : NONE
//----------------------------------------------------------------------------//
void user_system_idle_check(void)
{
    if(STATUS_IDLE == user_system_get_status())                                 // 当 系统空闲时， 退出
    {
        return;
    }

    if(
        1

        #if USER_PLAY_MODULE
            &&
            (
                STATUS_STOP == user_play_get_all_status()                            // 当 声音停止
                // || STATUS_PAUSE == user_play_get_status()                     // 当 声音暂停
            )
        #endif

        #if USER_KEY_MODULE
            && STATUS_IDLE == user_key_check_status()                           // 当 按键稳定时
        #endif

        #if USER_TALKBACK_MODULE
            && STATUS_IDLE == user_talk_back_get_status()                       // 当 回话 稳定时
        #endif

        #if USER_RECORD_MODULE
            && USER_RECORD_CMD_DISABLE == user_record_get_cmd()                 // 当 录音 稳定时
        #endif

        #if USER_LED_MODULE
            && LED_CMD_ALL_OFF == user_led_get_cmd()                            // 当 LED 关闭
        #endif

        #if USER_MOTION_MODULE
            && MOTION_STOP == user_motion_get_cmd()                             // 当 动作 停止
        #endif

        #if USER_OUTPUT_MODULE_EN
            && STATUS_IDLE == user_output_get_status()                          // 当 输出模块 稳定
        #endif
    )
    {
        if(user_systick_get_time(&lu32_work_idle_cnt) >= SYSTEM_IDLE_TIME)                              // 1min
        {
            user_systick_clear(&lu32_work_idle_cnt);

            user_system_set_status(STATUS_IDLE);

        #if USER_PLAY_MODULE
            user_play_audio(1, AUD_POWER_OFF);
        #endif
        }
    }
    else
    {
        user_systick_clear(&lu32_work_idle_cnt);
    }
}
//----------------------------------------------------------------------//
// Function Name : user_system_off_process
// Description   : 系统关闭处理函数
// Parameters    : NONE
// Returns       : NONE
//----------------------------------------------------------------------//
void user_system_off_process(void)
{
    //--------------------------------------------------------------------------
    // before sleep process - 睡眠的处理
    //--------------------------------------------------------------------------
    // Stop all user modules
    user_system_stop_all_user_module();

    // wakeup pin process - 需要在 nx1_config.h 的 _GPIO_WAKEUP 模块设置
    // GPIO_WritePort(GPIOA, GPIO_ReadPort(GPIOA));
    // GPIO_Write(GPIOA, 3, GPIO_Read(GPIOA, 3));
    // GPIO_Write(GPIOA, 4, GPIO_Read(GPIOA, 4));

    // User_Key_SleepProcess();
}
//----------------------------------------------------------------------//
// Function Name : user_system_wakeup_process
// Description   : 唤醒时处理函数
// Parameters    : NONE
// Returns       : NONE
//----------------------------------------------------------------------//
void user_system_wakeup_process(void)
{
    //--------------------------------------------------------------------------
    // after sleep process - 唤醒时的处理
    //--------------------------------------------------------------------------

}
//----------------------------------------------------------------------------//
// Function Name :	user_system_stop_all_user_module
// Description	 :	停止 所有的用户模块
// 					1.播放声音模块
// 					2.LED 模块
// 					...
//
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
void user_system_stop_all_user_module(void)
{
    // 停止所有与 声音相关的模块
    user_system_stop_all_audio_module();

    #if USER_LED_MODULE
        user_led_set_cmd(LED_CMD_ALL_OFF);
    #endif

    #if USER_MOTION_MODULE
        user_motion_set_cmd(MOTION_STOP);
    #endif

    // 停止 语音识别
    #if _VR_FUNC
        user_vr_set_cmd(VR_CMD_OFF);
    #endif
}

//----------------------------------------------------------------------------//
// 停止所有与 声音相关的模块
//----------------------------------------------------------------------------//
void user_system_stop_all_audio_module(void)
{
    // 停止 播放
    #if USER_PLAY_MODULE
        #if AUDIO_PLAY_SOUND
            // user_play_mode_set_cmd(USER_PLAY_MODE_SINGLE_NO_PLAY);
        #endif
        user_play_stop_all();
    #endif

    #if USER_RECORD_MODULE
        user_record_set_cmd(USER_RECORD_CMD_DISABLE, 0);
    #endif

    #if USER_TALKBACK_MODULE
        user_talk_back_set_cmd(DISABLE);
    #endif
}

//----------------------------------------------------------------------------//
// Function Name :	user_system_wakeup_pin_init
// Description	 :	唤醒脚 初始化
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
void user_system_wakeup_pin_init(void)
{
    // GPIO_WakeUpCmd(GPIOA, 3, ENABLE);                                           // 为唤醒引脚
    // GPIO_WakeUpCmd(GPIOA, 4, ENABLE);                                           // 为唤醒引脚
}

//status : STATUS_IDLE/ STATUS_WORKING
void user_system_set_status(u8 status)
{
    lu8_work_status = status;

    // lu32_work_idle_cnt = 0;                                                      // 清零 空闲计数
    user_systick_clear(&lu32_work_idle_cnt);

    if(STATUS_IDLE == status)
    {
        user_system_stop_all_user_module();
    }
}

u8 user_system_get_status(void)
{
    return lu8_work_status;
}

//----------------------------------------------------------------------------//
// 输出模块 相关
//----------------------------------------------------------------------------//
#if USER_OUTPUT_MODULE_EN

u8 gu8_output_status = STATUS_WORKING;

u8 user_output_get_status(void)
{
    return gu8_output_status;
}

void user_output_check_status(void)
{
    static u32 time_cnt = 0;

    if(
        1
        #if USER_PLAY_MODULE
            && STATUS_PLAYING != user_play_get_all_status()                     // 无声音播放, 有可能是正在录音等其他状态
        #endif
        #if USER_MOTION_MODULE
            && MOTION_STOP == user_motion_get_cmd()                             // 无动作
        #endif
        #if USER_LED_MODULE
            && LED_CMD_ALL_OFF == user_led_get_cmd()                             // 无动作
        #endif
        )
    {
        if(STATUS_IDLE != gu8_output_status)
        {
            // 空闲 一段时间 算真空闲
            if(user_systick_get_time(&time_cnt) >= USER_OUPUT_DELAY_TIME)
            {
                user_systick_clear(&time_cnt);                                  // 清零计数
                gu8_output_status = STATUS_IDLE;
            }
        }
    }
    else
    {
        user_systick_clear(&time_cnt);                                          // 清零计数
        gu8_output_status = STATUS_WORKING;
    }
}

void user_output_process(void)
{
    // 输出 状态 检测
    user_output_check_status();

    if(
        1
        #if USER_PLAY_MODULE
            && STATUS_PLAYING == user_play_get_all_status()                     // 有声音在播放
            && AUD_DI != user_play_get_current_index()                          // 当前播放的不是 AUD_DI， 该声不需要动作
        #else
            && STATUS_PLAYING == AUDIO_GetStatus_All()
        #endif
    )
    {
        #if USER_PLAY_MODULE
            // 播放歌曲时
            if(STATUS_PLAYING == user_play_get_status())
        #else
            if(STATUS_PLAYING == AUDIO_GetStatus(AUDIO_CH0))
        #endif  // endof #if USER_PLAY_MODULE
            {
                #if USER_LED_MODULE
                    if(LED_CMD_SONG != user_led_get_cmd())
                    {
                        user_led_set_cmd(LED_CMD_SONG);
                    }
                #endif

                #if USER_MOTION_MODULE
                    // 停止原来的动作
                    if(MOTION_SONG != user_motion_get_cmd())
                    {
                        user_motion_set_cmd(MOTION_SONG);
                    }
                #endif

                // visio 动作
                // ...
            }

        #if USER_RECORD_MODULE
            // 播放录音时
            else if(STATUS_PLAYING == ADPCM_GetStatus())
            {
                #if USER_LED_MODULE
                    if(LED_CMD_RECORD != user_led_get_cmd())
                    {
                        user_led_set_cmd(LED_CMD_RECORD);
                    }
                #endif

                #if USER_MOTION_MODULE
                    if(MOTION_RECORD != user_motion_get_cmd())
                    {
                        user_motion_set_cmd(MOTION_RECORD);
                    }
                #endif
            }
        #endif
    }
    else
    {
        #if USER_LED_MODULE
            if(LED_CMD_ALL_OFF != user_led_get_cmd())
            {
                user_led_set_cmd(LED_CMD_ALL_OFF);
            }
        #endif

        #if USER_MOTION_MODULE
            if(MOTION_STOP != user_motion_get_cmd())
            {
                user_motion_set_cmd(MOTION_STOP);
            }
        #endif
    }
}
#endif // end of #if USER_OUTPUT_MODULE_EN
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Function Name : user_init
// Description	 : 系统上电初始化
// Parameters	 : NONE
// Returns		 : NONE
//----------------------------------------------------------------------------//
void user_init(void)
{
    //----------------------------------------------------------------------------//
    // 用户初始化
    //----------------------------------------------------------------------------//
    // 变量初始化
    gu8_flag_10ms       	= 0;                                            	// 10ms 标志位 按鍵扫描 使用

    // 按键初始化
    #if USER_KEY_MODULE
        user_key_init();
    #endif

    #if USER_LED_MODULE
        user_led_init();
    #endif

    #if USER_MOTION_MODULE
        user_motion_init();
    #endif

    // 语音识别
    #if _VR_FUNC
        user_vr_init();
    #endif

    user_system_wakeup_pin_init();

    // 延时
    // Delayms(20);																// 延时 200ms, 等待一切稳定

    //----------------------------------------------------------------------------//
    // 上电之后的处理
    //----------------------------------------------------------------------------//
    // 系统工作
    user_system_set_status(STATUS_IDLE);

    #if _VR_FUNC
        user_vr_set_cmd(VR_CMD_GRP1);
    #endif

    // 播放上电音
    #if USER_PLAY_MODULE
        user_play_audio(1, AUD_DI);
    #endif

    #if USER_STORAGE_BREAK_POINT_MOUDLE && _STORAGE_MODULE
    //----------------------------------------------------------------------------//
    // 恢复存储的变量值
    //----------------------------------------------------------------------------//
        // 存储变量相关初始化
        user_storage_init();
    //----------------------------------------------------------------------------//
    // 恢复存储的变量值后的处理
    //----------------------------------------------------------------------------//
        // 恢复音量
        AUDIO_SetChVolume(AUDIO_CH0,gu8_volume);
    #endif
}

void user_main_service_loop(void)
{
    // 语音识别
    #if _VR_FUNC
        user_vr_service_loop();
    #endif

    //----------------------------------------------------------------------------//
    // 定时时基为10ms
    //----------------------------------------------------------------------------//
    if(gu8_flag_10ms)
    {
        gu8_flag_10ms = 0;

        #if USER_KEY_MODULE
            user_key_scan();
        #endif
    }

    #if USER_KEY_MODULE
        user_key_process();
    #endif

    #if USER_STORAGE_BREAK_POINT_MOUDLE && _STORAGE_MODULE
        //存储断点信息相关处理
        user_storage_cmd_process();
    #endif

    #if USER_LED_MODULE
        user_led_process();
    #endif

    #if USER_MOTION_MODULE
        user_motion_process();
    #endif

    #if USER_TALKBACK_MODULE
        user_talk_back_process();
    #endif

    #if USER_RECORD_MODULE
        user_record_process();
    #endif

    // 输出处理
    #if USER_OUTPUT_MODULE_EN
        user_output_process();
    #endif

    // 播放函数
    #if USER_PLAY_MODULE
        user_play_service_loop();
    #endif

    // 空闲计数
    user_system_idle_check();

    // 系统 低功耗检测
    // user_system_sleep_check();
}