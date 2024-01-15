/*******************************************************************************
* File Name     		:	user_motion.c
* Copyright				:	2017 LGW Corporation, All Rights Reserved.
* Module Name			:	motion
* Author	    		:	Pillar Peng
* Date of Creat 		:	2017-06-19 11:14:00 Mon
* Date of Last Edit		:	2017-06-19 11:14:00 Mon
*===============================================================================
* Log    (Detail of Modification)
*===============================================================================
* 2017-06-19 11:14:00 Mon
*   -> Modified
*   1.
*******************************************************************************/

#include "user_motion.h"

#if USER_MOTION_MODULE

Motion_Type gst_motion;

const user_gpio_type lcst_user_motion_gpio_a[][2]=
{
    // motor1,
    {
        {.GPIOx = GPIOA, .Pin = 14},        // pin1
        {.GPIOx = GPIOA, .Pin = 15},        // pin2
    },
    // motor2,
    // {
    //     {.GPIOx = GPIOA, .Pin = 0},                 // pin1
    //     {.GPIOx = GPIOA, .Pin = 1},                 // pin2
    // }
};

u8 gu8_motor_cmd[MOTOR_MAX_NUM] = {0,};
//----------------------------------------------------------------------------//
// Function Name :	user_motion_init
// Description	 :	马达 初始化
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
void user_motion_init(void)
{
    int i;

    for (i = 0; i < MOTOR_MAX_NUM; i++)
    {
        // 如果是空引脚 则不用初始化
        if(MOTOR_PIN_NO != lcst_user_motion_gpio_a[i][0].Pin)
        {
            GPIO_Init(lcst_user_motion_gpio_a[i][0].GPIOx, lcst_user_motion_gpio_a[i][0].Pin, GPIO_MODE_OUT_LOW);
        }

        // 如果是空引脚 则不用初始化
        if(MOTOR_PIN_NO != lcst_user_motion_gpio_a[i][1].Pin)
        {
            GPIO_Init(lcst_user_motion_gpio_a[i][1].GPIOx, lcst_user_motion_gpio_a[i][1].Pin, GPIO_MODE_OUT_LOW);
        }

        gu8_motor_cmd[i] = MOTOR_STOP;
    }

    user_motion_set_cmd(MOTION_STOP);
}
//----------------------------------------------------------------------------//
// Function Name : motion_motor_set_cmd
// Description	 : 马达的驱动函数，选择哪个马达并且选择马达的动作（停止，刹车，正转，反转）
// Parameters	 :
// 					@motor: 马达的选择
// 						- MOTOR_1: 马达1
// 						- MOTOR_2: 马达2
//
// 					@direction: 马达的动作
// 						- MOTOR_STOP		：停止
// 						- MOTOR_BRAKE		：刹车
// 						- MOTOR_FOREWARD	：正转
// 						- MOTOR_BACKWARD	：反转
// Returns		 : NONE
//----------------------------------------------------------------------------//
void motion_motor_set_cmd(u8 motor, u8 cmd)
{
	u8 pin1_bit = 0;
	u8 pin2_bit = 0;

	gu8_motor_cmd[motor] = cmd;												// 记录 马达状态

	// 确定马达动作
	switch(cmd)
	{
		case MOTOR_STOP:
				pin1_bit = 0;
				pin2_bit = 0;
				break;

		case MOTOR_BRAKE:
				pin1_bit = 1;
				pin2_bit = 1;
				break;

		case MOTOR_FOREWARD:
				pin1_bit = 1;
				pin2_bit = 0;
				break;

		case MOTOR_BACKWARD:
				pin1_bit = 0;
				pin2_bit = 1;
				break;
	}

	if(MOTOR_PIN_NO != lcst_user_motion_gpio_a[motor][0].Pin)
	{
	    GPIO_Write(lcst_user_motion_gpio_a[motor][0].GPIOx, lcst_user_motion_gpio_a[motor][0].Pin, pin1_bit);
	}

	if(MOTOR_PIN_NO != lcst_user_motion_gpio_a[motor][1].Pin)
	{
	    GPIO_Write(lcst_user_motion_gpio_a[motor][1].GPIOx, lcst_user_motion_gpio_a[motor][1].Pin, pin2_bit);
	}
}
//----------------------------------------------------------------------------//
// Function Name :	motion_motor_getcmd
// Description	 :	获取 马达 cmd
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
u8 motion_motor_get_cmd(u8 motor)
{
	return gu8_motor_cmd[motor];
}
//----------------------------------------------------------------------------//
// Function Name :	user_motion_set_cmd
// Description	 :	设置动作
// Parameters	 :
// 					@cmd: 动作指令
// 					- 0 : MOTION_STOP		：停止
// 					- 1 : MOTION_BRAKE		：刹车
// 					- 2 : MOTION_FOREWARD	：正转
// 					- 3 : MOTION_BACKWARD	：反转
// Return		 :	NONE
//----------------------------------------------------------------------------//
void user_motion_set_cmd(u8 cmd)
{
	gst_motion.cmd = cmd;
	gst_motion.update_flag = 1;

    // // MOTION_SET_STOP 的功能：修改为 stop 状态，但是不处理
    // if(MOTION_SET_STOP == cmd)
    // {
    //     // gst_motion.cmd = MOTION_STOP;
    //     gst_motion.update_flag = 0;
    // }
}
//----------------------------------------------------------------------------//
// Function Name :	user_motion_get_cmd
// Description	 :	获取动作
// Parameters	 :	NONE
// Return		 :
// 					@cmd: 动作指令
// 					- 0 : MOTION_STOP		：停止
// 					- 1 : MOTION_BRAKE		：刹车
// 					- 2 : MOTION_FOREWARD	：正转
// 					- 3 : MOTION_BACKWARD	：反转
//----------------------------------------------------------------------------//
u8 user_motion_get_cmd(void)
{
	return gst_motion.cmd;
}
//----------------------------------------------------------------------------//
// Function Name :	user_motion_process1
// Description	 :	动作1 处理函数
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
void user_motion_process1(void)
{
	if(gst_motion.update_flag)
	{
		gst_motion.update_flag = 0;
		motion_motor_set_cmd(MOTOR_1, MOTOR_STOP);
	}
}
//----------------------------------------------------------------------------//
// Function Name :	user_motion_process2
// Description	 :	动作2 处理函数
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
void user_motion_process2(void)
{
	if(gst_motion.update_flag)
	{
		gst_motion.update_flag = 0;
		motion_motor_set_cmd(MOTOR_1, MOTOR_BRAKE);
	}
}
//----------------------------------------------------------------------------//
// Function Name :	user_motion_process3
// Description	 :	动作3 处理函数
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
void user_motion_process3(void)
{
	if(gst_motion.update_flag)
	{
		gst_motion.update_flag = 0;
		motion_motor_set_cmd(MOTOR_1, MOTOR_FOREWARD);
	}
}
//----------------------------------------------------------------------------//
// Function Name :	user_motion_process4
// Description	 :	动作4 处理函数
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
void user_motion_process4(void)
{
	if(gst_motion.update_flag)
	{
		gst_motion.update_flag = 0;
		motion_motor_set_cmd(MOTOR_1, MOTOR_BACKWARD);
	}
}
//----------------------------------------------------------------------------//
// Function Name :	user_motion_process5
// Description	 :	动作5 处理函数
// 					5s 前进，1s 停止，然后值循环
//
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
void user_motion_process5(void)
{
	if(gst_motion.update_flag)
	{
		gst_motion.update_flag = 0;

		gst_motion.step = 0;

		// gst_motion.timer_cnt = 0;
        user_systick_clear(&gst_motion.timer_cnt);
	}

	switch(gst_motion.step)
	{
		case 0:
			if(MOTOR_STOP != motion_motor_get_cmd(MOTOR_1))
			{
				motion_motor_set_cmd(MOTOR_1, MOTOR_STOP);
			}

			if(user_systick_get_time(&gst_motion.timer_cnt) >= MOTION_TIMER_CNT_MS(200))
			{
				// gst_motion.timer_cnt = 0;
                user_systick_clear(&gst_motion.timer_cnt);

				++gst_motion.step;
			}
			break;

		case 1:
			if(MOTOR_FOREWARD != motion_motor_get_cmd(MOTOR_1))
			{
				motion_motor_set_cmd(MOTOR_1, MOTOR_FOREWARD);
			}

			if(user_systick_get_time(&gst_motion.timer_cnt) >= MOTION_TIMER_CNT_MS(3000))
			{
				// gst_motion.timer_cnt = 0;
                user_systick_clear(&gst_motion.timer_cnt);
				gst_motion.step = 0;
			}
			break;

	}
}
//----------------------------------------------------------------------------//
// Function Name :	user_motion_process5
// Description	 :	动作5 处理函数
// 					5s 前进，1s 停止，然后值循环
//
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
void user_motion_process6(void)
{
	if(gst_motion.update_flag)
	{
		gst_motion.update_flag = 0;

		gst_motion.step = 0;

		// gst_motion.timer_cnt = 0;
        user_systick_clear(&gst_motion.timer_cnt);
	}

	switch(gst_motion.step)
	{
		case 0:
			if(MOTOR_STOP != motion_motor_get_cmd(MOTOR_1))
			{
				motion_motor_set_cmd(MOTOR_1, MOTOR_STOP);
			}

			if(user_systick_get_time(&gst_motion.timer_cnt) >= MOTION_TIMER_CNT_MS(200))
			{
				// gst_motion.timer_cnt = 0;
                user_systick_clear(&gst_motion.timer_cnt);

				++gst_motion.step;
			}
			break;

		case 1:
			if(MOTOR_FOREWARD != motion_motor_get_cmd(MOTOR_1))
			{
				motion_motor_set_cmd(MOTOR_1, MOTOR_FOREWARD);
			}

            //
			if(user_systick_get_time(&gst_motion.timer_cnt) >= MOTION_TIMER_CNT_MS(1000/3/2))
			{
				// gst_motion.timer_cnt = 0;
                user_systick_clear(&gst_motion.timer_cnt);

				++gst_motion.step;
			}
			break;

		case 2:
			if(MOTOR_STOP != motion_motor_get_cmd(MOTOR_1))
			{
				motion_motor_set_cmd(MOTOR_1, MOTOR_STOP);
			}

			if(user_systick_get_time(&gst_motion.timer_cnt) >= MOTION_TIMER_CNT_MS(1000/3/2))
			{
				// gst_motion.timer_cnt = 0;
                user_systick_clear(&gst_motion.timer_cnt);

				gst_motion.step = 1;
			}
			break;

	}
}
//----------------------------------------------------------------------------//
// Function Name :	user_motion_process8
// Description	 :	动作处理函数
//
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
void user_motion_process7(void)
{
	if(gst_motion.update_flag)
	{
		gst_motion.update_flag = 0;

		gst_motion.step = 0;

		// gst_motion.timer_cnt = 0;
        user_systick_clear(&gst_motion.timer_cnt);
	}

	switch(gst_motion.step)
	{
		case 0:
			if(MOTOR_STOP != motion_motor_get_cmd(MOTOR_1))
			{
				motion_motor_set_cmd(MOTOR_1, MOTOR_STOP);
			}

			if(user_systick_get_time(&gst_motion.timer_cnt) >= MOTION_TIMER_CNT_MS(100))
			{
                // gst_motion.timer_cnt = 0;
                user_systick_clear(&gst_motion.timer_cnt);
				++gst_motion.step;
			}
			break;

		case 1:
			if(MOTOR_BACKWARD != motion_motor_get_cmd(MOTOR_1))
			{
				motion_motor_set_cmd(MOTOR_1, MOTOR_BACKWARD);
			}

			if(user_systick_get_time(&gst_motion.timer_cnt) >= MOTION_TIMER_CNT_MS(250))
			{
                // gst_motion.timer_cnt = 0;
                user_systick_clear(&gst_motion.timer_cnt);
				++gst_motion.step;
			}
			break;

		case 2:
			if(MOTOR_STOP != motion_motor_get_cmd(MOTOR_1))
			{
				motion_motor_set_cmd(MOTOR_1, MOTOR_STOP);
			}

            user_motion_set_cmd(MOTOR_STOP);
			break;

	}
}
//----------------------------------------------------------------------------//
// Function Name :	user_motion_process
// Description	 :	动作 处理函数
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
void user_motion_process(void)
{
	switch(gst_motion.cmd)
	{
		case MOTION_MOTION_1:
			user_motion_process1();
			break;

		case MOTION_MOTION_2:
			user_motion_process2();
			break;

		case MOTION_MOTION_3:
			user_motion_process3();
			break;

		case MOTION_MOTION_4:
			user_motion_process4();
			break;

		case MOTION_MOTION_5:
			user_motion_process5();
			break;

		case MOTION_MOTION_6:
			user_motion_process6();
			break;

		case MOTION_MOTION_7:
			user_motion_process7();
			break;
	}
}
#endif // endof #if USER_MOTION_MODULE