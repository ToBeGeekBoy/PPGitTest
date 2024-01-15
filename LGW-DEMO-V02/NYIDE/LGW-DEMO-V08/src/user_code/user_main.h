/*******************************************************************************
* File Name     		:	main.h
* Copyright				:	2017 LGW Corporation, All Rights Reserved.
* Module Name			:	main
* Author	    		:	Pillar Peng
* Date of Creat 		:	2017-07-26 17:39:29 Wed
* Date of Last Edit		:	2017-07-26 17:39:29 Wed
*==============================================================================
* Log    (Detail of Modification)
*===============================================================================
* 2017-07-26 17:39:29 Wed
*		1. modification
*******************************************************************************/

//============================================================================//
// Section for Multi-Include-Prevent
//============================================================================//
#ifndef _USER_MAIN_H_
#define _USER_MAIN_H_

//============================================================================//
// Section for Debug Switch
//============================================================================//


//============================================================================//
// Section for Include File
//============================================================================//
#include "include.h"
#include "nx1_lib.h"
#include "nx1_smu.h"
#include "nx1_gpio.h"
#include "nx1_timer.h"
#include "nx1_rtc.h"
#include "nx1_dac.h"

#include "user_play.h"
#include "user_key.h"
#include "user_record.h"
#include "user_talkback_adpcm.h"
#include "user_led.h"
#include "user_motion.h"

#include "user_storage_break_point.h"

#if _UART_MODULE && _DEBUG_MODE
    #include "debug.h"
#endif

#if _VR_FUNC
    #include "user_vr.h"
#endif

#if USER_CHECK_IC_STATUS_EN
    #include "user_check_ic_status.h"
#endif

//----------------------------------------------------------------------------//
// 常用算数
//----------------------------------------------------------------------------//
#define FUNC_ABSOLUTE(a,b)         (((a)>(b))?((a)-(b)):((b)-(a)))                  // 绝对值
#define FUNC_ARRAY_MEMS(array)     (sizeof(array)/sizeof(array[0]))

//----------------------------------------------------------------------------//
// 状态相关
//----------------------------------------------------------------------------//
#define UNDONE			(0)
#define DONE			(1)

#define STEP_OVER  		(0xff)                                                  // 结束步骤

#define TIME_CNT_BASE        (1)                                                //时基，xms
#define TIME_CNT_MS(x)       ((x) / TIME_CNT_BASE)                              // xms
#define TIME_CNT_SECOND(x)   ((x) * TIME_CNT_MS(1000))                          // xs
#define TIME_CNT_MINUTE(x)   ((x) * 60 * TIME_CNT_SECOND(1))                    // xmin
#define TIME_CNT_HOUR(x)     ((x) * 60 * TIME_CNT_MINUTE(1))                    // xh

#define STATUS_IDLE     (0)
#define STATUS_WORKING  (1)

#define STATUS_UNDONE   (0)
#define STATUS_DONE     (1)
#define STATUS_ERROR    (2)

//----------------------------------------------------------------------------//
// 字节 相关
//----------------------------------------------------------------------------//
#define SIZE_OF_1M_BYTE(x) 				(u32)((x)*0x100000)						// x * 1M byte
#define SIZE_OF_1K_BYTE(x) 				(u32)((x)*0x400)						// x * 1K byte

#define SIZE_OF_1M_BIT(x) 				(u32)((x)*(0x100000/8))					// x * 1M bit
#define SIZE_OF_1K_BIT(x) 				(u32)((x)*(0x400/8))					// x * 1K bit

//----------------------------------------------------------------------------//
// 系统睡眠相关
//----------------------------------------------------------------------------//
#define SYSTEM_IDLE_TIME        TIME_CNT_MINUTE(1)    // 单位：1ms, 系统工作状态时：所有模块空闲的一段时间后，进入系统空闲状态
#define SYSTEM_SLEEP_TIME       TIME_CNT_MS(100)      // 单位：1ms, 系统工作空闲时：所有模块空闲的一段时间后，进入 IC 睡眠状态

extern u8 gu8_flag_10ms;                  	                                    // 10ms 标志位 按鍵扫描 使用         置位的操作在 TMR1_ISR() 函数中，该函数在 isr.c 文件中

#if _VR_FUNC
    extern u8 gu8_vr_flag_10ms;
#endif

#if _STORAGE_MODULE && !USER_STORAGE_BREAK_POINT_MOUDLE
    //需要存储变量数组
    extern u8 gu8_sto_buf[_STORAGE_VAR_CNT];
#endif

void user_timebase_from_rtc_isr(void);
void user_timebase_from_rtc_isr_1ms_iram(void);
void user_timer1_init(void);

u8 user_system_get_status_for_sleep(void);
void user_system_sleep_check(void);
void user_system_off_process(void);
void user_system_wakeup_process(void);

void user_system_stop_all_user_module(void);
void user_system_stop_all_audio_module(void);

void user_system_idle_check(void);
void user_system_set_status(u8 status);
u8 user_system_get_status(void);

void user_system_wakeup_pin_init(void);
void user_init(void);
void user_main_service_loop(void);

//----------------------------------------------------------------------------//
// 输出模块 相关
//----------------------------------------------------------------------------//
#define USER_OUTPUT_MODULE_EN           ENABLE

#if USER_OUTPUT_MODULE_EN
    #define USER_OUPUT_DELAY_TIME       TIME_CNT_MS(200)          // 单位 1 ms

    void user_output_process(void);
    u8 user_output_get_status(void);
    void user_output_check_status(void);
#endif // endof #if USER_OUTPUT_MODULE_EN


//----------------------------------------------------------------------------//
// GPIO 相关
//----------------------------------------------------------------------------//
typedef struct user_gpio_
{
    GPIO_TypeDef* GPIOx;
    u8 Pin;
}user_gpio_type;

void user_systick_clear(u32 *timer_cnt);
u32 user_systick_get_time(u32 *timer_cnt);

#endif //end of _USER_MIAN_H_
