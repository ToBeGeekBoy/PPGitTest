/*******************************************************************************
* File Name     		:	user_led.c
* Copyright				:	2017 LGW Corporation, All Rights Reserved.
* Module Name			:	led
* Author	    		:	Pillar Peng
* Date of Creat 		:	2017/06/01
* Date of Last Edit		:	2017/06/02
*******************************************************************************/
#include "user_led.h"

#if USER_LED_MODULE

// 请勿改用 静态变量
user_led_t gst_led =
{
	.cmd 		     = 0,
	.update_flag     = 0,
	.process_step    = 0,
	.timer_cnt       = 0,
};

const user_gpio_type lcst_user_led_gpio_a[]=
{
    {.GPIOx = GPIOA, .Pin = 13},
    // {.GPIOx = GPIOA, .Pin = 5},
    // {.GPIOx = GPIOA, .Pin = 2},
    // {.GPIOx = GPIOA, .Pin = 3},
    // {.GPIOx = GPIOA, .Pin = 4},
    // {.GPIOx = GPIOA, .Pin = 5},
    // {.GPIOx = GPIOB, .Pin = 6},
    // {.GPIOx = GPIOB, .Pin = 7},
};

//----------------------------------------------------------------------------//
// Function Name :	user_led_init
// Description	 :	LED 初始化
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
void user_led_init(void)
{
    int i=0;

    for ( i= 0; i < LED_MAX_NUM; i++)
    {
        // IO 初始化
        GPIO_Init(lcst_user_led_gpio_a[i].GPIOx, lcst_user_led_gpio_a[i].Pin, (LED_OFF?GPIO_MODE_OUT_HI:GPIO_MODE_OUT_LOW));   // PA0
    }

	user_led_set_cmd(LED_CMD_ALL_OFF);
}
//----------------------------------------------------------------------------//
// Function Name :	led_set_status
// Description	 :	设置 LED 的状态
// Parameters	 :
// 					@u8 led_num:
// 					@u8 sta
// 					- LED_OFF
// 					- LED_ON
// 					- LED_TOGGLE = 2
// 					以上两个宏定义的值会根据你选择的驱动模式有关。
// 					具体设置 在 H 文件中。
// Return		 :	NONE
//----------------------------------------------------------------------------//
void led_set_status(u8 led_num, u8 sta)
{
	if(LED_TOGGLE == sta)
	{
        GPIO_Toggle(lcst_user_led_gpio_a[led_num].GPIOx, lcst_user_led_gpio_a[led_num].Pin);
	}
	else
	{
        GPIO_Write(lcst_user_led_gpio_a[led_num].GPIOx, lcst_user_led_gpio_a[led_num].Pin, sta);
	}
}
//----------------------------------------------------------------------------//
// Function Name :	user_led_all_off
// Description	 :	LED all on
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
void user_led_all_off(void)
{
	int i=0;

    for ( i= 0; i < LED_MAX_NUM; i++)
    {
        // IO 初始化
        GPIO_Write(lcst_user_led_gpio_a[i].GPIOx, lcst_user_led_gpio_a[i].Pin, LED_OFF);
    }
}
//----------------------------------------------------------------------------//
// Function Name :	user_led_all_on
// Description	 :	LED all on
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
void user_led_all_on(void)
{
    int i=0;

    for ( i= 0; i < LED_MAX_NUM; i++)
    {
        // IO 初始化
        GPIO_Write(lcst_user_led_gpio_a[i].GPIOx, lcst_user_led_gpio_a[i].Pin, LED_ON);
    }
}
//----------------------------------------------------------------------------//
// Function Name :	user_led_set_cmd
// Description	 :	设置 LED 命令
// 					各种 led 的亮灭的模式
// Parameters	 :
// 					@u8 cmd
// 					- 0 : LED_CMD_0
// 					- 1 : LED_CMD_1
// 					- 2 : LED_CMD_2
// 					- 3 : LED_CMD_3
// 					- 4 : LED_CMD_4
// 					- 5 : LED_CMD_5
// 					- 6 : LED_CMD_6
// 					- 7 : LED_CMD_7
// 					- 8 : LED_CMD_8
// 					- 9 : LED_CMD_9
// Return		 :	NONE
//----------------------------------------------------------------------------//
void user_led_set_cmd(u8 cmd)
{
	gst_led.cmd = cmd;
    gst_led.timer_cnt = 0;
    gst_led.process_step = 0;

	gst_led.update_flag = 1;														// 位置 cmd 更新标志
}
//----------------------------------------------------------------------------//
// Function Name :	user_led_get_cmd
// Description	 :	获取 LED 的命令
// 					各种 led 的亮灭的模式
// Parameters	 :	NONE
// Return		 :
// 					@u8 cmd
// 					- 0 : LED_CMD_0
// 					- 1 : LED_CMD_1
// 					- 2 : LED_CMD_2
// 					- 3 : LED_CMD_3
// 					- 4 : LED_CMD_4
// 					- 5 : LED_CMD_5
// 					- 6 : LED_CMD_6
// 					- 7 : LED_CMD_7
// 					- 8 : LED_CMD_8
// 					- 9 : LED_CMD_9
//----------------------------------------------------------------------------//
u8 user_led_get_cmd(void)
{
	return gst_led.cmd;
}
//----------------------------------------------------------------------------//
// Function Name :	led_flash_process_0
// Description	 :	flash tpye 0
// 					LED all off
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
void led_flash_process_0(void)
{
	if(gst_led.update_flag)
	{
		gst_led.update_flag = 0;

		user_led_all_off();
	}
}
//----------------------------------------------------------------------------//
// Function Name :	led_flash_process_1
// Description	 :	flash tpye 1
// 					LED all on
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
void led_flash_process_1(void)
{
	if(gst_led.update_flag)
	{
		gst_led.update_flag = 0;

		user_led_all_on();
	}
}
//----------------------------------------------------------------------------//
// Function Name :	led_flash_process_2
// Description	 :	3hz闪烁
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
void led_flash_process_2(void)
{
	if(gst_led.update_flag)
	{
		gst_led.update_flag = 0;

		gst_led.timer_cnt=0;
		gst_led.process_step=0;

        user_systick_clear(&gst_led.timer_cnt);
		user_led_all_on();
	}

	switch(gst_led.process_step)
	{
		case 0:
			if(user_systick_get_time(&gst_led.timer_cnt) >= LED_TIMER_CNT_MS(170))						// 100 ms 闪烁
			{
				user_systick_clear(&gst_led.timer_cnt);

				led_set_status(0, LED_TOGGLE);

				gst_led.process_step = 0;
			}
			break;
	}
}
//----------------------------------------------------------------------------//
// Function Name :	user_led_process
// Description	 :	LED 处理
// Parameters	 :	NONE
// Return		 :	NONE
//----------------------------------------------------------------------------//
void user_led_process(void)
{
	switch(gst_led.cmd)
	{
		case LED_CMD_ALL_OFF:
			// 关闭所有灯
			led_flash_process_0();
			break;

		case LED_CMD_ALL_ON:
			// 打开所有灯
			led_flash_process_1();
			break;

		case LED_CMD_ALL_FLASH_3HZ:
			// 3hz闪烁
			led_flash_process_2();
			break;
	}
}

#endif //end of #if USER_LED_MODULE