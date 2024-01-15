/*******************************************************************************
* File Name     		:	user_motion.h
* Copyright				:	2017 LGW Corporation, All Rights Reserved.
* Module Name			:	action
* Author	    		:	Pillar Peng
* Date of Creat 		:	2017-06-19 11:34:27 Mon
* Date of Last Edit		:	2017-06-19 11:34:27 Mon
*==============================================================================
* Log    (Detail of Modification)
*===============================================================================
* 2017-06-19 11:34:27 Mon
*		1. modification
*******************************************************************************/

//============================================================================//
// Multi-Include-Prevent
//============================================================================//
#ifndef _USER_MOTION_H_
#define _USER_MOTION_H_

//============================================================================//
// Debug Switch
//============================================================================//


//============================================================================//
// Include File
//============================================================================//
#include "user_main.h"

//----------------------------------------------------------------------------//
// 模块使能宏定义
//----------------------------------------------------------------------------//
#define USER_MOTION_MODULE					(ENABLE)						    // 模块使能

#if USER_MOTION_MODULE

//----------------------------------------------------------------------------//
// 时间基准
//----------------------------------------------------------------------------//
#define MOTION_TIMER_BASE                  (1)                                  // MS
#define MOTION_TIMER_CNT_MS(x)             ((x)/MOTION_TIMER_BASE)              // MS

//----------------------------------------------------------------------------//
// 引脚定义
//----------------------------------------------------------------------------//
#define MOTOR_PIN_NO 						(-1)
#define MOTOR_MAX_NUM 						FUNC_ARRAY_MEMS(lcst_user_motion_gpio_a)

// 马达
#define MOTOR_1 							(0)
#define MOTOR_2 							(1)

// 马达的动作
#define MOTOR_STOP	 						(0)
#define MOTOR_BRAKE 						(1)
#define MOTOR_FOREWARD 						(2)
#define MOTOR_BACKWARD 						(3)

// 动作
#define MOTION_MOTION_1 					(0)
#define MOTION_MOTION_2 					(1)
#define MOTION_MOTION_3 					(2)
#define MOTION_MOTION_4 					(3)
#define MOTION_MOTION_5 					(4)
#define MOTION_MOTION_6 					(5)

// 动作
#define MOTION_MOTION_1 					(0)
#define MOTION_MOTION_2 					(1)
#define MOTION_MOTION_3 					(2)
#define MOTION_MOTION_4 					(3)
#define MOTION_MOTION_5 					(4)
#define MOTION_MOTION_6 					(5)
#define MOTION_MOTION_7 					(6)
#define MOTION_MOTION_8 					(7)

// 动作别名
#define MOTION_STOP	 						(MOTION_MOTION_1)
#define MOTION_BRAKE 						(MOTION_MOTION_2)
#define MOTION_FOREWARD 					(MOTION_MOTION_3)
#define MOTION_BACKWARD 					(MOTION_MOTION_4)

#define MOTION_COQUETRY 					(MOTION_BACKWARD)
#define MOTION_SONG  					    (MOTION_MOTION_5)
#define MOTION_RECORD  					    (MOTION_MOTION_6)
#define MOTION_SET_STOP  					(MOTION_MOTION_7)     // 空动作
#define MOTION_RESET  					    (MOTION_MOTION_8)     // 复位动作


typedef struct Motion_T
{
	u8 cmd;
	u8 step;
	u8 update_flag;
	u32 timer_cnt;
}Motion_Type;

extern Motion_Type gst_motion;

void user_motion_init(void);
void motion_motor_set_cmd(u8 motor, u8 cmd);
u8 motion_motor_get_cmd(u8 motor);
void user_motion_set_cmd(u8 cmd);
u8 user_motion_get_cmd(void);
void user_motion_process(void);

#endif //endof #if USER_MOTION_MODULE

#endif //end of _USER_MOTION_H_
