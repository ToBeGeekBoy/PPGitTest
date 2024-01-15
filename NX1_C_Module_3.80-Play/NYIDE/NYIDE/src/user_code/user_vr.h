/*******************************************************************************
* File Name     		:	user_vr.h
* Copyright				:	2017 LGW Corporation, All Rights Reserved.
* Module Name			:	voice recognition
* Author	    		:	Pillar Peng
*******************************************************************************/

#ifndef _USER_VR_H_
#define _USER_VR_H_

#include "include.h"
#include "nx1_lib.h"
#include "VRCmdTab.h"
#include "nx1_gpio.h"
#include "user_main.h"

#if _VR_FUNC

#define VR_MODE_NORMAL		            (0)             // 普通模式
#define VR_MODE_TRIGGER	                (1)             // 触发模式

#if _VR_VOICE_TAG
    #define VR_MODE 			        (VR_MODE_NORMAL)    // 只能使用普通模式
#else
    #define VR_MODE 			        (VR_MODE_TRIGGER)
#endif

#define VR_RESET_GROUP_INTERVAL	        (700)               // n*10 ms VR_GROUP 之间的 时间间隙，当 出现 group1 指令后，在这段时间没有说出 group2 的指令，则不在接受 group2 的指令，直接复位。只直接 group1 的指令

#define VR_TRIGGER_CMD_ID		        (0)
#define VR_GPR1_VALID_CMD_NUM	        (100)
#define VR_GPR2_VALID_CMD_NUM	        (100)
#define VR_GPR3_VALID_CMD_NUM	        (100)
//----------------------------------------------------------------------------//
// VR 的 组别 和 状态
//----------------------------------------------------------------------------//
#define VR_CMD_OFF                      (0)
#define VR_CMD_GRP1                     (1)
#define VR_CMD_GRP2                     (2)
#define VR_CMD_GRP3                     (3)
//----------------------------------------------------------------------------//
// VR LED 提示
//----------------------------------------------------------------------------//
#define VR_LED_MODE_EN                  (DISABLE)
#if VR_LED_MODE_EN
    #define VR_LED_MODE_PORT            (GPIOB)
    #define VR_LED_MODE_PIN             (14)

    #define VR_LED_ON                   (1)
    #define VR_LED_OFF                  (VR_LED_ON?0:1)
#endif

extern s8 gs8_vr_cmd_id;
extern u8 gu8_vr_cmd;

void user_vr_group_select(u8 state);

void user_vr_init(void);
void user_vr_cmd_process(s8 vr_cmd_id);
void user_vr_service_loop(void);

void user_vr_set_cmd(u8 cmd);
u8 user_vr_get_cmd(void);
u8 user_vr_get_status(void);

#if _VR_VOICE_TAG
    extern S8 VR_Train(U8 input, U8 action);

    void user_vr_voice_tag_train_set_cmd(u8 cmd, u8 tag_id, u8 tag_cmd);
    u8 user_vr_voice_tag_train_get_cmd(void);
    u8 user_vr_voice_tag_get_state(u8 tag_id);
#endif

#endif //end of _USER_VR_H_
#endif