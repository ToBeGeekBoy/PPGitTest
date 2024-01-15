/*******************************************************************************
* File Name     		:	user_key.h
* Copyright				:	2017 LGW Corporation, All Rights Reserved.
* Module Name			:	voice recognition
* Author	    		:	Pillar Peng
* Date of Creat 		:	2017.6.7
* Date of Last Edit		:	2017.6.7
*==============================================================================
* Log    (Detail of Modification)
*===============================================================================
* 2017.6.7
*		1. modification
*******************************************************************************/
#ifndef _USER_KEY_H_
#define _USER_KEY_H_

#include "user_main.h"

#define USER_KEY_MODULE					ENABLE									// 按键模块使能
#if USER_KEY_MODULE

//----------------------------------------------------------------------------//
// 选择按键是对GND触发还是对VDD触发
//----------------------------------------------------------------------------//
#define KEY_TRIGGER_GND                 (0)                 //对GND触发
#define KEY_TRIGGER_VDD                 (1)                 //对VDD触发
#define KEY_TRIGGER_CONFIG              (KEY_TRIGGER_GND)   //触发方式设置
//----------------------------------------------------------------------------//
// 时间基准
//----------------------------------------------------------------------------//
#define KEY_TIMER_BASE                  10                                      // MS
#define KEY_TIMER_CNT_MS(time_cnt)      ((time_cnt)/KEY_TIMER_BASE)             // MS


//----------------------------------------------------------------------------//
// 最大 按键数量 （1~16）
// 注意：
// 		1. 没有多功能按键时， 基本 ROM 占用： 180 byte
// 		2. 每增加一个多功能按键 RAM 增加 8 byte;
// 		3. 每增加一个多功能按键 ROM 增加 32 byte(大约);
//----------------------------------------------------------------------------//
#define USER_KEY_MAX_NUM			    (sizeof(lcst_user_key_gpio_a)/sizeof(UserKey_Type))		// 按键最大数量
#define KEY_PIN_MASK				    (Key_GetKeyMask(USER_KEY_MAX_NUM))                      // KEY mask

//----------------------------------------------------------------------------//
// 各种 debounce
//----------------------------------------------------------------------------//
#define SINGLE_KEY_TIME_DEFAULT     	KEY_TIMER_CNT_MS(20)       				// 判定 单击的时间长度，软件消抖
#define	MULTI_KEY_TIME_DEFAULT			KEY_TIMER_CNT_MS(300)	    			// 判定 多击时的时间间隔
#define LONG_KEY_TIME_DEFAULT  			KEY_TIMER_CNT_MS(1000)    				// 判定 长按的时间长度
#define KEY_DEBOUNCE					SINGLE_KEY_TIME_DEFAULT            		// 判定 单击的时间长度，软件消抖

//----------------------------------------------------------------------------//
// 普通按键
//----------------------------------------------------------------------------//
#define NORMAL_FUNC_KEY					DISABLE								    // 多功能按键 选项, 1:使能 0:失能

//----------------------------------------------------------------------------//
// 多功能
// - 长按
// - 多击 （双击，三击...）
//----------------------------------------------------------------------------//
#define MULTI_FUNC_KEY					ENABLE								    // 多功能按键 选项, 1:使能 0:失能

//----------------------------------------------------------------------------//
// 长按使能
//----------------------------------------------------------------------------//
#define LONG_FUNC_KEY					ENABLE  								// 长按 选项, 1:使能 0:失能

//----------------------------------------------------------------------------//
// 最大多击数 选择 (1~...)
// 如：
// - 1 : 只有单击
// - 2 : 单击，双击
// - 3 : 单击，双击， 三击
// - 4 : 单击，双击， 三击，四击
//----------------------------------------------------------------------------//
#define KEY_CLICK_MAX_CHANGE_EN       	DISABLE									// 各个按键的多击次数不一样使能选项   1：使能   0 失能

#if KEY_CLICK_MAX_CHANGE_EN
    #define KEY_CLICK_MAX      	        gst_multi_func_key_max_click_a[st_mkey->KeyNum]   // 多击次数
#else
    #define KEY_CLICK_MAX				1								        // 多功能按键时，最大的多击次数 （如：3, 就多就是三击）
#endif

#define SINGLE_CLICK       				1    									// 单击按键
#define DOUBLE_CLICK       				2    									// 双击按键
// #define TRIPLE_CLICK       				3    									// 三击按键
// #define QUADRUPLE_CLICK       			4    									// 四击按键

//----------------------------------------------------------------------------//
// 单击 消抖时间
//----------------------------------------------------------------------------//
#define SINGLE_KEY_TIME_CHANGE_EN      	DISABLE									// 各个按键的长按时间不一样使能选项   1：使能   0 失能
#if SINGLE_KEY_TIME_CHANGE_EN
    #define SINGLE_KEY_TIME_CNT      	gcu32_key_deboucne_a[i]					// 判定 长按的时间长度
#else
    #define SINGLE_KEY_TIME_CNT      	SINGLE_KEY_TIME_DEFAULT					// 判定 长按的时间长度
#endif

//----------------------------------------------------------------------------//
// 多击时间间隔
//----------------------------------------------------------------------------//
#define MULTI_KEY_TIME_CHANGE_EN       	DISABLE									// 各个按键的长按时间不一样使能选项   1：使能   0 失能
#if MULTI_KEY_TIME_CHANGE_EN
    #define MULTI_KEY_TIME_CNT      	gst_multi_func_key_multi_key_time_a[st_mkey->KeyNum]									// 判定 长按的时间长度
#else
    #define MULTI_KEY_TIME_CNT      	MULTI_KEY_TIME_DEFAULT					// 判定 长按的时间长度
#endif

#define KEY(x)                          (1UL<<(x))

//----------------------------------------------------------------------------//
// 长按 消抖时间
//----------------------------------------------------------------------------//
#define LONG_KEY_TIME_CHANGE_EN       	DISABLE									// 各个按键的长按时间不一样使能选项   1：使能   0 失能
#if LONG_KEY_TIME_CHANGE_EN
    #define LONG_KEY_TIME_CNT      	    gc32_multi_func_key_long_key_time_a[st_mkey->KeyNum]									// 判定 长按的时间长度
#else
    #define LONG_KEY_TIME_CNT      	    LONG_KEY_TIME_DEFAULT					// 判定 长按的时间长度
#endif


//----------------------------------------------------------------------------//
// 按键状态 相关
//----------------------------------------------------------------------------//
extern u8 gu8_key_state_a[];
#define key_state_set(num,sta)       (gu8_key_state_a[(num)] |= (sta))
#define key_state_clear(num,sta)     (gu8_key_state_a[(num)] &= ~(sta))
u8 key_state_check(u8 num, u8 sta);

// 注意：如果状态的个数超过 8 个；则需要将 状态的变量(gu8_key_state_a)改为 U16/U32 ，否则会溢出，出错。
#define KEY_STATE_MASK                   (-1)
#define KEY_STATE_NONE                   (0)
#define KEY_STATE_PRESS                  (1<<0)
#define KEY_STATE_RELEASE                (1<<1)
#define KEY_STATE_SHORT                  (1<<2)
#define KEY_STATE_LONG_PRESS             (1<<3)
#define KEY_STATE_LONG_RELEASE           (1<<4)
#define KEY_STATE_DOUBLE_CLICK           (1<<5)
#define KEY_STATE_MAX_CLICK_RELEASE      (1<<6)

typedef struct UserKey_
{
    GPIO_TypeDef* GPIOx;
    U8 Pin;
}UserKey_Type;


u32 user_key_get_key_mask(u8 key_max_num);
u32 user_key_get_key_value(void);

void user_key_init(void);
void user_key_scan(void);
u8 user_key_check_status(void);
void user_key_sleep_process(void);
u8 user_key_get_key_max_num(void);

void user_key_process(void);

//----------------------------------------------------------------------------//
// 普通按键 按下和释放 处理函数
//----------------------------------------------------------------------------//
#if NORMAL_FUNC_KEY
    void key_press_process(u8 key_num);
    void key_release_process(u8 key_num);
#endif // end of #if NORMAL_FUNC_KEY

#if MULTI_FUNC_KEY
typedef struct MuiltFuncKey_
{
    u8 KeyNum;																// 按键 值（5 bits = 0~31）
    u8 MultiClick_TimeCntFlag;												// 多击计时 标志位 （0 : 停止计时 - 1 : 开始计时）
    u8 LongPress_TimeCntFlag;												// 长按计时 标志位 （0 : 停止计时 - 1 : 开始计时）
    u8 KeyClick_Count;														// 按键 按下 的次数
    u32 LongPress_TimeCount;												// 长按 的判定时间计数
    // const u32 LongPress_TargetTimeCount;									// 长按 的判定时间计数.  为什么不把长按的时间放在 结构体 里面呢？ 因为放在 结构体 里面，会占有 ROM 和 RAM. 而拎出来 作为 const 变量，则只消耗 ROM 而且比较少, RAM 不变。
    u32 MultiClick_TimeCount;												// 双击/多击 按键的间隔时间计数
}MuiltFuncKey_Type;

//----------------------------------------------------------------------------//
// 多功能按键 相关
//----------------------------------------------------------------------------//
void multi_func_key_timer_count_func(MuiltFuncKey_Type *st_mkey);
void multi_func_key_release_process(MuiltFuncKey_Type *st_mkey);
void multi_func_key_press_process(MuiltFuncKey_Type *st_mkey);

//----------------------------------------------------------------------------//
// 多功能按键 函数
//----------------------------------------------------------------------------//
void key_short_process(u8 key_num);
void key_long_press_process(u8 key_num);
void key_long_release_process(u8 key_num);
void key_double_click_process(u8 key_num);
void key_max_click_release_process(u8 key_num);

void multi_func_key_multi_click_process(MuiltFuncKey_Type* st_mkey);
#endif // end of MULTI_FUNC_KEY

#endif // end of #if USER_KEY_MODULE

#endif // end of _USER_KEY_H_