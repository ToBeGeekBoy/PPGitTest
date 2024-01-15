/*******************************************************************************
* File Name     		:	user_vr.c
* Copyright				:	2020 LGW Corporation, All Rights Reserved.
* Module Name			:	VR
* Author	    		:	Pillar Peng
* Date of Creat 		:	2017-07-17 19:41:11 Mon
*===============================================================================
* Log    (Detail of Modification)
*===============================================================================
* Code Version    : V01
*   -> Data 	  : 2017-07-17 19:41:11 Mon
*   -> History    :
*       1. Initial
*-------------------------------------------------------------------------------
* Code Version    : V02
*   -> Data 	  : 2020-02-26 10:06:54 Wed
*   -> History    :
*       1. 添加未识别指令
*-------------------------------------------------------------------------------
* Code Version    : V03
*   -> Data 	  : 2020-04-13 09:58:16 Mon
*   -> History    :
*       1. user_vr_service_loop 函数中，增加 flag1_10ms/flag2_10ms
*       2. 把 LED process 添加到 user_vr_servi)ce_loop
*       3. 把 LED flash CMD 是闪烁方式。 亮一下后，马上灭。
*-------------------------------------------------------------------------------
* Code Version    : V04
*   -> Data 	  : 2022-01-18 14:46:07 Tue
*   -> History    :
*       1. 定时方式改为 systick 的方式
*-------------------------------------------------------------------------------
* Code Version    : V05
*   -> Data 	  : 2023-03-22 14:46:07 Tue
*   -> History    :
*       1.将VR_GroupCombo1[0]的值由(U8 *)VR_CMD_GROUP_1改为(U8 *)VR0_CMD_GROUP_1
*         将VR_GroupCombo2[0]的值由(U8 *)VR_CMD_GROUP_2改为(U8 *)VR0_CMD_GROUP_2
*         将VR_Start()函数里的实参(U8*)VR_CMD_CYBASE改为(U8*)VR0_CMD_CYBASE
*         NYIDE_V4.92版本支持多个.cvr文件，默认只有一个.cvr文件，故做以上修改，
*         不做以上修改的话编译会报错。
*-------------------------------------------------------------------------------
* Code Version    : V06
*   -> Data 	  : 2023-06-08 14:46:07 Tue
*   -> History    :
*       1. 增加vr_voice_tag功能和vr_voice_password功能，目前只支持1个组的语音词条训练,
*          且只支持vr文件和存放tag词条的bin文件存放在flash里.
*-------------------------------------------------------------------------------
* Code Version    : V07
*   -> Data 	  : 2023-07-07 14:46:07 Tue
*   -> History    :
*       1.获取语音识别组地址函数 VR_SPI_GroupAddr() 替换为 VR_GroupAddr()
*-------------------------------------------------------------------------------
*******************************************************************************/
#include "user_vr.h"

#if _VR_FUNC

u8 gu8_vr_cmd = VR_CMD_OFF;    // VR 的 GRP

s8 gs8_vr_cmd_id = -1;

u8 gu8_vr_flag_10ms = 0;

#if (VR_MODE == VR_MODE_TRIGGER)
    static u32 lu32_vr_timerout_cnt = 0;
#endif

// VR Group Combo Declaration
u8 *vr_groupcombo1[1];
u8 *vr_groupcombo2[1];

#if _VR_VOICE_TAG
    u8 *vr_group_sd[1];
#endif
//----------------------------------------------------------------------------//
// Function Name : user_vr_group_addr_init
// Description	 : 设置vr组地址
// Parameters	 : NONE
// Returns		 : NONE
//----------------------------------------------------------------------------//
void user_vr_group_addr_init(void)
{
    #if _VR_STORAGE_TYPE
        #if _VR_VOICE_TAG
            // 得到 存放tag词条bin文件地址
            SDModel = (u8*)RESOURCE_GetAddress(_VR_VOICE_TAG_RSRC_INDEX);
            // 保存
            vr_group_sd[0] = SDModel;
            #if _VR_VOICE_PASSWORD
                // 得到 存放tag词条bin文件地址
                SVModel = (u8*)RESOURCE_GetAddress(_VR_VOICE_PASSWORD_INDEX);
            #endif
        #endif

        // Set VR Group Combo Adress from SPI Mode
        #if _VR_VOICE_TAG
            vr_groupcombo1[0] = (u8*)VR_GroupAddr(1);
        #else
            vr_groupcombo1[0] = (u8*)VR_GroupAddr(1);
            vr_groupcombo2[0] = (u8*)VR_GroupAddr(2);
        #endif
    #else
        // Set VR Group Combo Address from OTP Mode
        vr_groupcombo1[0] = (u8*)VR0_CMD_GROUP_1;
        vr_groupcombo2[0] = (u8*)VR0_CMD_GROUP_2;
    #endif
}
//----------------------------------------------------------------------------//
// Function Name : user_vr_group_select
// Description	 : 设置vr组状态
// Parameters	 : @u8 state：状态
// Returns		 : NONE
//----------------------------------------------------------------------------//
void user_vr_group_select(u8 state)
{
    switch (state)
    {
        default:

        case VR_TRIGGER_STATE:
            #if (VR_LED_MODE_EN == ENABLE)
                GPIO_Write(VR_LED_MODE_PORT, VR_LED_MODE_PIN, VR_LED_OFF);
            #endif

            VR_SetGroupState(state, 1);

            #if _VR_STORAGE_TYPE
                #if _VR_VOICE_TAG
                    if(VR_GetNumWord(SDModel))
                    {
                        // 若已经有保存tag，则开始语音识别
                        VR_Start((u8*)VR_GroupAddr(0), (u8**)vr_group_sd);
                    }
                    else
                #endif
                    {
                        VR_Start((u8 *)VR_GroupAddr(0), (u8**)vr_groupcombo1); // Start from SPI Mode
                    }
            #else
                    VR_Start((u8 *)VR0_CMD_CYBASE, (u8 **)vr_groupcombo1); // Start from OTP Mode
            #endif
        break;

        #if !_VR_STORAGE_TYPE && _VR_VOICE_TAG
            case VR_CMD_STATE_1:
                #if (VR_LED_MODE_EN == ENABLE)
                    GPIO_Write(VR_LED_MODE_PORT, VR_LED_MODE_PIN, VR_LED_ON);
                #endif

                VR_SetGroupState(state, 1);

                #if _VR_STORAGE_TYPE
                    VR_Start((u8 *)VR_GroupAddr(0), (u8 **)vr_groupcombo2); // Start from SPI Mode
                #else
                    VR_Start((u8 *)VR0_CMD_CYBASE, (u8 **)vr_groupcombo2); // Start from OTP Mode
                #endif
            break;
        #else
            case VR_CMD_STATE_1:
                #if (VR_LED_MODE_EN == ENABLE)
                    GPIO_Write(VR_LED_MODE_PORT, VR_LED_MODE_PIN, VR_LED_ON);
                #endif

                VR_SetGroupState(state, 1);

                #if _VR_STORAGE_TYPE
                    VR_Start((u8 *)VR_GroupAddr(0), (u8 **)vr_groupcombo2); // Start from SPI Mode
                #else
                    VR_Start((u8 *)VR0_CMD_CYBASE, (u8 **)vr_groupcombo2); // Start from OTP Mode
                #endif
            break;
        #endif
    }
}
//----------------------------------------------------------------------------//
// Function Name : user_vr_init
// Description	 : VR 初始化 OTP 或者 SPI
// Parameters	 : NONE
// Returns		 : NONE
//----------------------------------------------------------------------------//
void user_vr_init(void)
{
    user_vr_group_addr_init();
    user_vr_group_select(VR_TRIGGER_STATE);

    #if (VR_LED_MODE_EN != DISABLE)
        GPIO_Init(VR_LED_MODE_PORT, VR_LED_MODE_PIN, (VR_LED_OFF)?GPIO_MODE_OUT_HI:GPIO_MODE_OUT_LOW);
    #endif

    user_vr_set_cmd(VR_CMD_OFF);
}
//----------------------------------------------------------------------------//
// Function Name : user_vr_set_cmd
// Description	 : VR 命令。
// Parameters	 :
//                  @u8 cmd.   OFF gpr1 grp2 ...
//                  - 0 : VR_CMD_OFF
//                  - 1 : VR_CMD_GRP1
//                  - 2 : VR_CMD_GRP2
//                  - 3 : VR_CMD_GRP3
//                  ...
// Returns		 : NONE
//----------------------------------------------------------------------------//
void user_vr_set_cmd(u8 cmd)
{
    gu8_vr_cmd = cmd;

    // user_vr_group_addr_init();

    if(VR_CMD_OFF == cmd)
    {
        VR_Stop();
    }
    else if(VR_CMD_GRP1 == cmd)
    {
        user_vr_group_select(VR_TRIGGER_STATE);
    }
    else if(VR_CMD_GRP2 == cmd)
    {
        user_vr_group_select(VR_CMD_STATE_1);
    }
    else if(VR_CMD_GRP3 == cmd)
    {
        user_vr_group_select(VR_CMD_STATE_2);
    }
    else
    {
        user_vr_group_select(VR_TRIGGER_STATE);
    }
}
//----------------------------------------------------------------------------//
// Function Name : user_vr_get_status
// Description   : 获取 vr 的状态
// Parameters    : NONE
// Returns       : @u8 status
//                  - 0 : VR_OFF
//                  - 1 : VR_ON
//                  - 2 : VR_HALT
//----------------------------------------------------------------------------//
u8 user_vr_get_cmd(void)
{
    return gu8_vr_cmd;
}
//----------------------------------------------------------------------------//
// Function Name : user_vr_get_status
// Description   : 获取 vr 的状态
// Parameters    : NONE
// Returns       : @u8 status
//                  - 0 : VR_OFF
//                  - 1 : VR_ON
//                  - 2 : VR_HALT
//----------------------------------------------------------------------------//
u8 user_vr_get_status(void)
{
    return VRState;
}
#if _VR_VOICE_TAG
    //----------------------------------------------------------------------------//
    // VR TAG 部分
    //----------------------------------------------------------------------------//
    // 注意：
    // 1. TAG 的数量 与 原本 CSMT 中的指令词条数量也有关系。应该是 与 CSMT 中，指令所占有 RAM 有关系，
    //    当 CSMT 原本的 RAM 很小时，TAG 数量变多则在新建 TAG 成功后，死机，不断复位。
    //    所以CSMT 中的指令词条数量需大于TAG数量，当出现死机状态时，增加CSMT 中的指令词条数量可以解决
    //    比如：
    //    CSMT 中，指令只有一个 “小威你好”，那么你在新建 VOICE TAG 时，指令超过 2 以上，则会出现死机。
    //
    // 2. 新建 TAG 的次序，与 TAGID 的序号无关。依次从 VR_ServiceLoop() 中读出的 ID 为：
    //    0 3 6 9 12 ...
    //
    // 3. nID = CSpotterSD16_GetCmdID_FromSPI(SDModel, TAGID1);
    //   可以从上面的函数，读出某一个特定的 ID 是否存在。
    //   -当 nID >=0 时， TAG 存在
    //   -当 nID <0  时， TAG 不存在
    //
    // 4. nx1_lib.h 中的宏定义 PASSWORD_ID,(默认只有1个，扩充到 16 个)
    //    #define PASSWORD_ID1				201
    //    #define PASSWORD_ID2				202
    //    #define PASSWORD_ID3				203
    //    #define PASSWORD_ID4				204
    //    #define PASSWORD_ID5				205
    //    #define PASSWORD_ID6				206
    //    #define PASSWORD_ID7				207
    //    #define PASSWORD_ID8				208
    //    #define PASSWORD_ID9				209
    //    #define PASSWORD_ID10				210
    //    #define PASSWORD_ID11				211
    //    #define PASSWORD_ID12				212
    //    #define PASSWORD_ID13				213
    //    #define PASSWORD_ID14				214
    //    #define PASSWORD_ID15				215
    //    #define PASSWORD_ID16				216
    //    nx1_lib.h 中的宏定义 TAGID,(默认只有8个，扩充到 16 个)
    //    #define TAGID1				    51
    //    #define TAGID2					52
    //    #define TAGID3					53
    //    #define TAGID4					54
    //    #define TAGID5					55
    //    #define TAGID6					56
    //    #define TAGID7					57
    //    #define TAGID8					58
    //    #define TAGID9					59
    //    #define TAGID10					60
    //    #define TAGID11					61
    //    #define TAGID12					62
    //    #define TAGID13					63
    //    #define TAGID14					64
    //    #define TAGID15					65
    //    #define TAGID16					66
    //
    // 5. 修改vrctl_callback.c文件中的宏定义为
    //    #define SD_MODEL_SIZE			    0x14000		//  4*0x5000（4 Voice_Tag Size 0x5000）
    //    如果想再增加tag数量，增大宏SD_MODEL_SIZE值就行，不过要考虑NX1的RAM和存放TAG词条的bin文件大小。
    //
    // 6. 需要打开 _VR_VOICE_PASSWORD功能时，需再添加一个SD_64KB.bin文件，否则训练词条时会一直复位
    //
    // 7. 没有打开_VR_VOICE_PASSWORD时，训练完所有词条后，语音识别都有效；
    //    打开_VR_VOICE_PASSWORD时,只有当前训练的词条语音识别有效，
    //    且识别的严谨度比没打开_VR_VOICE_PASSWORD高
    //
    // 8. 目前只支持1组的语音识别指令训练,且只支持vr文件和存放tag词条的bin文件存放在flash里
    //
    // 9. 清除所有词条时，原本的语音识别功能有效，存在训练词条时，只有训练词条语音识别有效
    //
    // 10.训练词条的回调函数在vrctl_callback.c文件中，
    //     CB_VR_TrainTag_PromptAction_BeforeTrainingStart(); // 训练刚开始时回调函数
    //     CB_VR_TrainTag_PromptAction_BeforeSayingTag();     // 开始说训练词条前回调函数
    //     CB_VR_TrainTag_BeforeTrainWord();                  // 训练开始时回调函数,检测词条是否最新
    //     CB_VR_TrainTag_AfterTrainWord();                   // 训练结束时回调函数
    //
    // 11.训练词条可以是任意语音指令，但不能过长
    //----------------------------------------------------------------------------//
    typedef struct user_vr_voice_tag__
    {
        u8 cmd;
        u16 tag_id;
        u8 tag_cmd;
        u32 tag_id_status;      // 所有tag_id的状态,1bit对应一个tag_id,0:不存在,1:存在
    }user_vr_voice_tag_type;

    user_vr_voice_tag_type gst_vr_voice_tag;
    //----------------------------------------------------------------------------//
    // Function Name : user_vr_voice_tag_train_set_cmd
    // Description	 : 设置每个tag命令
    // Parameters	 : @u8 cmd：DISABLE ENABLE
    //                 @u8 tag_id: TAGID1 ~ TAGID8
    //                 @u8 tag_cmd: TRAIN_DELETEMODEL: 删除模型，删除所有tag
    //                              TRAIN_CREATID:     创建新的tag
    //                              TRAIN_DELETEID:    删除指定tag
    // Returns		 : NONE
    //----------------------------------------------------------------------------//
    void user_vr_voice_tag_train_set_cmd(u8 cmd, u8 tag_id, u8 tag_cmd)
    {
        gst_vr_voice_tag.cmd = cmd;
        gst_vr_voice_tag.tag_id = tag_id;
        gst_vr_voice_tag.tag_cmd = tag_cmd;

        user_play_stop_all();

        // if(DISABLE == gst_vr_voice_tag.cmd)
        // {
        //     gst_vr_voice_tag.tag_id = 0;
        //     gst_vr_voice_tag.tag_cmd = 0;
        // }
    }
    //----------------------------------------------------------------------------//
    // Function Name : user_vr_voice_tag_train_get_cmd
    // Description	 : 得到命令
    // Parameters	 : NONE
    // Returns		 : DISABLE ENABLE
    //----------------------------------------------------------------------------//
    u8 user_vr_voice_tag_train_get_cmd(void)
    {
        return gst_vr_voice_tag.cmd;
    }
    //----------------------------------------------------------------------------//
    // Function Name : user_vr_voice_tag_get_state
    // Description	 : 判断是否有 tag
    // Parameters	 : @u8 tag_id: id
    // Returns		 : 0：无 TAG
    //                 1：有 TAG
    //----------------------------------------------------------------------------//
    u8 user_vr_voice_tag_get_state(u8 tag_id)
    {
        u8 sta = 0;

        if(0 == tag_id)
        {
            sta = (VR_GetNumWord(SDModel))?1:0;
        }
        else
        {
            // sta = (CSpotterSD16_GetCmdID_FromSPI(SDModel, tag_id)>=0)?1:0;

            #if _VR_VOICE_PASSWORD
                sta = ((gst_vr_voice_tag.tag_id_status >> (tag_id - PASSWORD_ID1)) & 0x01)?1:0;
            #else
                sta = ((gst_vr_voice_tag.tag_id_status >> (tag_id - TAGID1)) & 0x01)?1:0;
            #endif
        }

        return sta;
    }
    //----------------------------------------------------------------------------//
    // Function Name : user_vr_voice_tag_get_vr_id
    // Description	 : 得到触发的tag的id值，由于得到的tag_id是从TAGID1开始的，
    //                 所以减去TAGID1得到从0开始的id值
    // Parameters	 : @s8 id: id
    // Returns		 : id值
    //----------------------------------------------------------------------------//
    s8 user_vr_voice_tag_get_vr_id(s8 id)
    {
        s8 vr_cmd = -1;

        vr_cmd = id;

        if(VR_GetNumWord(SDModel))
        {
            s32 tag_id = CSpotterSD16_GetTagID_FromSPI(SDModel, id);

            #if _UART_MODULE && _DEBUG_MODE
                dprintf("tag_id:%d\r\n",tag_id);
            #endif

            #if _VR_VOICE_PASSWORD
                vr_cmd = tag_id - PASSWORD_ID1;
            #else
                vr_cmd = tag_id - TAGID1;
            #endif
        }

        return vr_cmd;
    }
    //----------------------------------------------------------------------------//
    // Function Name : user_vr_voice_tag_train_process
    // Description	 : 训练vr_tag词条处理
    // Parameters	 : NONE
    // Returns		 : NONE
    //----------------------------------------------------------------------------//
    void user_vr_voice_tag_train_process(void)
    {
        if(gst_vr_voice_tag.cmd)
        {
            s8 train_result = TRAIN_TAG_FAIL;   // 结果

            if(gst_vr_voice_tag.cmd)
            {
                // 训练tag词条
                train_result = VR_Train(gst_vr_voice_tag.tag_id, gst_vr_voice_tag.tag_cmd);
            }

            if (train_result == TRAIN_TAG_OK)
            {
                // 训练完成
                // user_play_audio(1,VOICE_TAG_RENAME_COMPLETE_AUD_SOUND);
                if(gst_vr_voice_tag.tag_id < TAGID14)
                {
                    AUDIO_Play(AUDIO_CH0,VOICE_TAG_RENAME_COMPLETE_AUD_SOUND,SPI_MODE);
                }
                else if(TAGID14 == gst_vr_voice_tag.tag_id)
                {
                    AUDIO_Play(AUDIO_CH0,VOICE_TAG_SETTING_CMD14_AUD,SPI_MODE);
                }
                else if(TAGID15 == gst_vr_voice_tag.tag_id)
                {
                    AUDIO_Play(AUDIO_CH0,VOICE_TAG_SETTING_CMD15_AUD,SPI_MODE);
                }
                else if(TAGID16 == gst_vr_voice_tag.tag_id)
                {
                    AUDIO_Play(AUDIO_CH0,VOICE_TAG_SETTING_CMD16_AUD,SPI_MODE);
                }

                // 设置对应tag_id状态为存在
                #if _VR_VOICE_PASSWORD
                    gst_vr_voice_tag.tag_id_status |= (u32)(1 << (gst_vr_voice_tag.tag_id-PASSWORD_ID1));
                #else
                    gst_vr_voice_tag.tag_id_status |= (u32)(1 << (gst_vr_voice_tag.tag_id-TAGID1));
                #endif
            }
            else if(train_result == TRAIN_TAG_FAIL)
            {
                if(gst_vr_voice_tag.cmd)
                {
                    // 训练失败
                    // user_play_audio(1,VOICE_TAG_RENAME_FAIL_AUD_SOUND);
                    AUDIO_Play(AUDIO_CH0,VOICE_TAG_RENAME_FAIL_AUD_SOUND,SPI_MODE);
                }
                else
                {
                    // 被打断
                    // user_play_audio(1,VOICE_TAG_SETTING_CANCLED_AUD_SOUND);
                    AUDIO_Play(AUDIO_CH0,VOICE_TAG_SETTING_CANCLED_AUD_SOUND,SPI_MODE);
                }
            }
            else if(train_result == DELETE_TAG_OK)
            {
                // 删除tag成功
                // user_play_audio(1,VOICE_TAG_ERASE_NAME_FINISHED_AUD_SOUND);
                if(gst_vr_voice_tag.tag_id < TAGID14)
                {
                    AUDIO_Play(AUDIO_CH0,VOICE_TAG_ERASE_NAME_FINISHED_AUD_SOUND,SPI_MODE);
                }
                else if(TAGID14 == gst_vr_voice_tag.tag_id)
                {
                    AUDIO_Play(AUDIO_CH0,VOICE_TAG_CLEAR_CMD14_AUD,SPI_MODE);
                }
                else if(TAGID15 == gst_vr_voice_tag.tag_id)
                {
                    AUDIO_Play(AUDIO_CH0,VOICE_TAG_CLEAR_CMD15_AUD,SPI_MODE);
                }
                else if(TAGID16 == gst_vr_voice_tag.tag_id)
                {
                    AUDIO_Play(AUDIO_CH0,VOICE_TAG_CLEAR_CMD16_AUD,SPI_MODE);
                }

                // 设置对应tag_id状态为不存在
                #if _VR_VOICE_PASSWORD
                    gst_vr_voice_tag.tag_id_status &= ~((u32)(1 << (gst_vr_voice_tag.tag_id-PASSWORD_ID1)));
                #else
                    gst_vr_voice_tag.tag_id_status &= ~((u32)(1 << (gst_vr_voice_tag.tag_id-TAGID1)));
                #endif
            }
            else if(train_result == DELETE_SD_MODEL_OK)
            {
                // 删除训练模型成功,删除所有tag词条
                // user_play_audio(1,VOICE_TAG_CLEAR_ALL_NAMES_AUD_SOUND);
                AUDIO_Play(AUDIO_CH0,VOICE_TAG_CLEAR_ALL_NAMES_AUD_SOUND,SPI_MODE);

                // 设置所有tag_id状态为不存在
                gst_vr_voice_tag.tag_id_status = 0;
            }

            user_vr_set_cmd(VR_CMD_GRP1);
        }

        // 清零
        gst_vr_voice_tag.cmd = DISABLE;
    }
    #endif
//----------------------------------------------------------------------------//
// Function Name : user_vr_service_loop
// Description	 : VR 扫描函数
// Parameters	 : NONE
// Returns		 : NONE
//----------------------------------------------------------------------------//
void user_vr_service_loop(void)
{
    // ----- 获取语音识别指令 -----
    gs8_vr_cmd_id = VR_ServiceLoop(); // 获取 语音识别的 指令编号

    if(
        VR_CMD_OFF != user_vr_get_cmd()

        #if USER_PLAY_MODULE
            && STATUS_STOP == user_play_get_all_status()
        #endif
    )
    {
    #if (VR_MODE == VR_MODE_NORMAL)
        if(gs8_vr_cmd_id != -1) // 有语音识别 指令
        {
            if (VR_CMD_GRP1 == user_vr_get_cmd())                                    // trigger grp
            {
                #if _VR_VOICE_TAG
                    // 若有tag触发
                    if(user_vr_voice_tag_get_state(0))
                    {
                        // 得到触发语音指令id
                        gs8_vr_cmd_id = user_vr_voice_tag_get_vr_id(gs8_vr_cmd_id);
                    }
                #endif

                if(gs8_vr_cmd_id >= 0 && gs8_vr_cmd_id < VR_GPR1_VALID_CMD_NUM)       // 只有有效指令，才需要处理
                {
                    user_vr_cmd_process(gs8_vr_cmd_id);                               // 根据指令 执行动作
                }
            }

            // 未识别的指令
            #if (_VR_VAD&&_VR_UNKNOWN_COMMAND)
            if(gs8_vr_cmd_id == -2)
            {

            }
            #endif  //endof #if (_VR_VAD&&_VR_UNKNOWN_COMMAND)

        } // end of if (gs8_vr_cmd_id != -1)

    // trigger mode - SYSTEM_MODE_TRIGGER
    #else
        if(gs8_vr_cmd_id != -1) // 有语音识别 指令
        {
            // 处理 VR 指令
            if (VR_CMD_GRP1 == user_vr_get_cmd())                                // trigger grp
            {
                if(gs8_vr_cmd_id >= 0)                                          // 中文触发指令相同则 触发有效
                {
                    // 清零计数
                    lu32_vr_timerout_cnt = 0;

                    user_vr_set_cmd(VR_CMD_GRP2);                                // 进入 GRP2

                    #if USER_PLAY_MODULE
                        user_play_audio(1, 0);
                    #endif
                }
            }
            else if (VR_CMD_GRP2 == user_vr_get_cmd())                            // 功能 GRP
            {
                if(gs8_vr_cmd_id >= 0)                                          // 只有有效指令，才需要处理
                {
                    // 清零计数
                    lu32_vr_timerout_cnt = 0;

                    user_vr_cmd_process(gs8_vr_cmd_id);                          // 根据指令 执行动作
                }
            }

            // 未识别的指令
            #if (_VR_VAD&&_VR_UNKNOWN_COMMAND)
            if(gs8_vr_cmd_id == -2)
            {

            }
            #endif  //endof #if (_VR_VAD&&_VR_UNKNOWN_COMMAND)
        }
        else // 没有指令的情况下，如果超过 时间 则回到 group 1 指令
        {
            // VR_GROUP 之间的 时间间隙，当 出现 group1 指令后，在这段时间没有说出 group2 的指令，则不在接受 group2 的指令，直接复位。只直接 group1 的指令
            if(VR_CMD_GRP2 == user_vr_get_cmd())
            {
                if(gu8_vr_flag_10ms)
                {
                    gu8_vr_flag_10ms = 0;

                    // 没有指令的情况下，如果超过 时间 则回到 group 1 指令
                    if(++lu32_vr_timerout_cnt >= VR_RESET_GROUP_INTERVAL)
                    {
                        // VR_GROUP 之间的 时间间隙，当 出现 group1 指令后，在这段时间没有说出 group2 的指令，则不在接受 group2 的指令，直接复位。只直接 group1 的指令
                        lu32_vr_timerout_cnt = 0;

                        user_vr_set_cmd(VR_CMD_GRP1);                               // 进入 GRP1
                    }
                }
            }
        } // end of if (gs8_vr_cmd_id != -1)
    #endif
    }

    #if _VR_VOICE_TAG
        // 训练vr_tag词条处理
        user_vr_voice_tag_train_process();
    #endif
}
//----------------------------------------------------------------------------//
// Function Name :
// Description	 :
// Parameters	 : NONE
// Returns		 : NONE
//----------------------------------------------------------------------------//
void user_vr_cmd_process(s8 vr_cmd_id)
{
    #if USER_PLAY_MODULE
        user_play_audio(1, vr_cmd_id);
    #endif

    switch (vr_cmd_id)
    {
        // 请开灯
        case 0:
            // GPIO_Init(GPIOA, 12, GPIO_OUTPUT_HIGH);
            break;

        // 请关灯
        case 1:
            // GPIO_Init(GPIOA, 12, GPIO_OUTPUT_LOW);
            break;

        // 小威你好
        case 2:

            break;

        // 小威再见
        case 3:

            break;

        // 唱歌
        case 4:

            break;

        // 学我说话
        case 5:

            #if USER_TALKBACK_MODULE
                #if _VR_FUNC
                    user_vr_set_cmd(VR_CMD_OFF);
                #endif

                user_system_stop_all_audio_module();
                #if AUDIO_PLAY_SOUND
                    user_play_mode_set_cmd(USER_PLAY_MODE_SINGLE_NO_PLAY);
                #endif

                user_talk_back_set_cmd(ENABLE);
                user_play_audio(1, USER_TALK_BACK_HINT_START);
            #endif

            break;
    }
}

#endif // endof _VR_FUNC