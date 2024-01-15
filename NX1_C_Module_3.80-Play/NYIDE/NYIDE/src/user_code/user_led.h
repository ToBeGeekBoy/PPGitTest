#ifndef _USER_LED_H_
#define _USER_LED_H_

#include "user_main.h"


//----------------------------------------------------------------------------//
// 模块使能宏定义
//----------------------------------------------------------------------------//
#define USER_LED_MODULE						(DISABLE)			// 模块使能

#if USER_LED_MODULE

//----------------------------------------------------------------------------//
// 时间基准
//----------------------------------------------------------------------------//
#define LED_TIMER_BASE                      (1)                   // MS
#define LED_TIMER_CNT_MS(x)                 ((x)/LED_TIMER_BASE)  // MS
//----------------------------------------------------------------------------//
// LED 模式
//----------------------------------------------------------------------------//
#define LED_ON								(1)
#define LED_OFF								(LED_ON?0:1)
#define LED_TOGGLE							(2)

#define LED_STATUS_OFF						(0)			// LED 状态 OFF
#define LED_STATUS_ON						(1)			// LED 状态 ON

//----------------------------------------------------------------------------//
// LED 数量 和 IO 定义
//----------------------------------------------------------------------------//
#define LED_MAX_NUM							(sizeof(lcst_user_led_gpio_a) / sizeof(user_gpio_type))  // led最大数量
//----------------------------------------------------------------------------//
// LED 闪法 种类
//----------------------------------------------------------------------------//
#define LED_CMD_MAX_NUM   					(8)

#define LED_CMD_0   						(0)
#define LED_CMD_1   						(1)
#define LED_CMD_2   						(2)
#define LED_CMD_3   						(3)
#define LED_CMD_4   						(4)
#define LED_CMD_5   						(5)
#define LED_CMD_6   						(6)
#define LED_CMD_7   						(7)
#define LED_CMD_8   						(8)
#define LED_CMD_9   						(9)
#define LED_CMD_10  						(10)

#define LED_CMD_ALL_OFF  	    			(0)
#define LED_CMD_ALL_ON  	    			(1)
#define LED_CMD_ALL_FLASH_3HZ  				(2)

#define LED_CMD_SONG  	        			(LED_CMD_ALL_FLASH_3HZ)
#define LED_CMD_RECORD 	        			(LED_CMD_ALL_ON)

typedef struct
{
	u8 cmd;								     // LED 的指令
	u8 update_flag;						     // LED 更新标志位
	u8 process_step;
	u32 timer_cnt;
} user_led_t;

//----------------------------------------------------------------------------//
// 基础 驱动
//----------------------------------------------------------------------------//
void user_led_init(void);
void led_set_status(u8 led_num, u8 sta);
void user_led_all_off(void);
void user_led_all_on(void);

//----------------------------------------------------------------------------//
// led 的应用
//----------------------------------------------------------------------------//
void user_led_set_cmd(u8 cmd);
u8 user_led_get_cmd(void);
void user_led_process(void);

#endif //end of #if USER_LED_MODULE

#endif //end of _USER_LED_H_
