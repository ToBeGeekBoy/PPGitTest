/*******************************************************************************
* File Name     		:	user_storage_break_point.c
* Copyright				:	2023 LGW Corporation, All Rights Reserved.
* Module Name			:	communication
* Author	    		:	songuo
*===============================================================================
* Log    (Detail of Modification)
*===============================================================================
* Code Version    : V03
*   -> Data 	  : 2023-03-17 10:59:10 Mon
*   -> History    :
*       1. Initial
*       2. 添加user_storage_cmd()函数，用于存储变量
*       3. 去除user_storage_set_cmd()函数、user_storage_get_cmd()函数
*       4. 添加了user_storage_get_work_status(),用于获取存储工作状态
*       5. 修改了user_storage_cmd_process()函数和user_storage_save_customize_var()函数
*       6. 将第一次存储标志变量改为u32类型，用于4字节对齐
*       7. 把之前sbc_audioapi.c文件里的关于播放断点的操作函数移到此文件。
*       8. 在user_storage_init()函数里调用user_storage_get_sbc_breakpoint_val()函数获取
*          sbc_audioapi.c文件里关于断点信息的指针
*       9. 在执行存储时，增加调用SBC_AutoSave_GetEraseState()函数判断当前存储状态，需要在
*          存储空闲时才可以进行存储操作
*-------------------------------------------------------------------------------
*===============================================================================
* Program Description
*===============================================================================
* Function:
*   1.需要将user_storage_init()放到user_main.c文件中的user_init()
*     user_storage_init()完成了各自定义变量的对应存储地址初始化和各自定义变量值的初始化；
*   2.需要将user_storage_cmd_process()放到主循环中
*   3.需要使用存储自定义断点播放功能时：
*     使能宏USER_CUSTOMIZE_BREAK_POINT_CMD；
*     设置宏USER_SBC_AUD_BREAK_POINT_NUM，设置自定义断点个数
*     需要保存断点时，调用 user_storage_save_breakpoint() 函数；
*     需要恢复断点播放时，调用 user_storage_restore_breakpoint() 函数，再调用SBC_Seek()函数和AUDIO_Play()函数；
*     需要清除指定断点时，调用 user_storage_clean_breakpoint() 函数；
*     需要清除全部断点时，调用 user_storage_clean_all_breakpoint() 函数；
*   4.需要使用存储自定义变量功能时：
*     使能宏USER_CUSTOMIZE_VAR_CMD；
*     在user_customize_var_t结构体中定义需要的变量；
*     自定义对应的用户使用的变量用于获取存储的自定义变量值；
*     更改user_storage_init()函数，确保第一次存储时的初始化自定义变量值；
*     更改user_storage_get_all_customize_var_value()函数，确保初始化时获取自定义变量值的正确性；
*     需要更新自定义变量值时，调用user_storage_save_customize_var()函数；
*   5.若遇到保存的变量值不能正常保存的问题，应查看宏 USER_STORAGE_VAR_SIZE 的值，大多数情况下是保存变量
*     的总字节数没有4字节对齐造成的
*   6.在sbc_audioapi.c文件里新增了SbcRamEBufCtrl结构体控制存储操作，在进行存储操作前，需要调用
*     SBC_AutoSave_GetEraseState()函数判断当前存储状态，存储空闲时才可以进行存储操作
*   7.有时候会遇到，设置保存变量值的宏 USER_STORAGE_VAR_SIZE 明明都已经是4字节对齐，但是，需要保存的变量
*     还是不能正常保存的问题，这时候，把NYIDE编译的所有bin文件清空，再次重新编译，再次烧录时，就不会出现
*     这个问题了。
*******************************************************************************/
#include "user_storage_break_point.h"
#include "debug.h"

#if USER_STORAGE_BREAK_POINT_MOUDLE
//============================================================================//
// Declaration of Variable
//============================================================================//

//============================================================================//
// Definition of Global Variable
//============================================================================//
//自定义断点信息
#if USER_CUSTOMIZE_BREAK_POINT_CMD
    //存储使用的断点
    user_customize_audioplay_breakpoint_t gst_user_customize_audioplay_breakpoint[USER_SBC_AUD_BREAK_POINT_NUM];

    //用户调用断点
    user_customize_audioplay_breakpoint_t gst_user_customize_play_breakpoint[USER_SBC_AUD_BREAK_POINT_NUM];

    // 保存从sbc_audioapi.c文件里关于断点信息的指针
    u32 *gu32_p_SbcSeekCtl_CurAddr;
    u32 *gu32_p_SbcSeekCtl_process_count;
    u16 *gu16_p_SbcSeekCtl_rand;
    u32 *gu32_p_CurAddr;
    u16 *gu16_p_SBCRam;
    s32 volatile *gs32_p_AudioBufCtl_process_count;
#endif

#if USER_CUSTOMIZE_VAR_CMD
    //存储使用的变量
    user_customize_var_t gst_user_customize_var;

    //用户调用的变量
    u16 gu16_song_index = 0;            //歌曲下标值
    u8 gu8_volume = 0;                  //音量值
    // u8 gu8_var3 = 0;                 //变量3
    // u8 gu8_var4 = 0;                 //变量4
#endif

#if USER_CUSTOMIZE_VAR_CMD || USER_CUSTOMIZE_BREAK_POINT_CMD
    //----------------------------------------------------------------------------//
    //第一次存储标志变量，目的是为了第一次存储时初始化变量值
    //由于第一次存储时前的所有定义需要存储的变量值都是0，所以不好判断此次是不是第一次存储，
    //所以用此变量来判断是不是第一次存储.
    //这里用u32类型是为了4字节对齐，没有4字节对齐的话会存储无效
    //----------------------------------------------------------------------------//
    u32 gu32_user_customize_first_storage_flag = 0;
#endif

//存储状态
u8 gu8_storage_status = 0;
//============================================================================//
// Definition of Local Variable
//============================================================================//

//============================================================================//
// Declaration of Function Prorotype
//============================================================================//
#if USER_CUSTOMIZE_VAR_CMD || USER_CUSTOMIZE_BREAK_POINT_CMD
    //----------------------------------------------------------------------------//
    // Function Name : user_storage_set_customize_first_storage_flag
    // Description	 : 设置自定义的第一次存储标志变量的存储地址
    // Parameters	 : NONE
    // Returns		 : NONE
    //----------------------------------------------------------------------------//
    void user_storage_set_customize_first_storage_flag(void)
    {
        STORAGE_SetVar((u8 *)&gu32_user_customize_first_storage_flag, sizeof(gu32_user_customize_first_storage_flag));
    }
#endif

#if USER_CUSTOMIZE_VAR_CMD
    //----------------------------------------------------------------------------//
    // Function Name : user_storage_set_customize_var
    // Description	 : 设置自定义变量存储地址
    // Parameters	 : NONE
    // Returns		 : NONE
    //----------------------------------------------------------------------------//
    void user_storage_set_customize_var(void)
    {
        STORAGE_SetVar((void *)&gst_user_customize_var, USER_CUSTOMIZE_VAR_SIZE);
    }
    //----------------------------------------------------------------------------//
    // Function Name : user_storage_get_all_customize_var_value
    // Description	 : 得到全部用户变量值，若读取到的存储变量值错误则是得到对应变量的初始值
    // Parameters	 : NONE
    // Returns		 : NONE
    //----------------------------------------------------------------------------//
    void user_storage_get_all_customize_var_value(void)
    {
        u8 storage_flag = 0;

        //检查读取到音量值是否有效
        if(
               gst_user_customize_var.volume >= USER_VOLUME_MIN
            && gst_user_customize_var.volume <= USER_VOLUME_MAX
        )
        {
            //获取音量值
            gu8_volume = gst_user_customize_var.volume;
        }
        else
        {
            //存储的音量值出错，重新赋初始值
            gst_user_customize_var.volume = USER_VAR_1_INIT_VALUE;
            //用户使用的变量也赋初值
            gu8_volume = USER_VAR_1_INIT_VALUE;

            storage_flag = 1;
        }

        //检查读取的歌曲下标是否有效
        if(
               gst_user_customize_var.song_index >= USER_SONG_STRAT_INDEX
            && gst_user_customize_var.song_index <= USER_SONG_END_INDEX
        )
        {
            //获取歌曲下标
            gu16_song_index = gst_user_customize_var.song_index;
        }
        else
        {
            //存储的歌曲下标出错，重新赋初始值
            gst_user_customize_var.song_index = USER_VAR_2_INIT_VALUE;
            //用户使用的变量也赋初值
            gu16_song_index = USER_VAR_2_INIT_VALUE;

            storage_flag = 1;
        }

/*
        //检查读取到的变量3是否合法
        if(gst_user_customize_var.var_3 >= 0)       //检测是否合法条件
        {
            //获取变量3值
            gu8_var3 = gst_user_customize_var.var_3;
        }
        else
        {
            //存储的歌曲下标出错，重新赋初始值
            gst_user_customize_var.var_3 = USER_VAR_3_INIT_VALUE;
            //用户使用的变量也赋初值
            gu8_var3 = USER_VAR_3_INIT_VALUE;

            storage_flag = 1;
        }

        //检查读取到的变量4是否合法
        if(gst_user_customize_var.var_4 >= 0)       //检测是否合法条件
        {
            //获取变量4值
            gu8_var4 = gst_user_customize_var.var_4;
        }
        else
        {
            //存储的歌曲下标出错，重新赋初始值
            gst_user_customize_var.var_4 = USER_VAR_4_INIT_VALUE;
            //用户使用的变量也赋初值
            gu8_var4 = USER_VAR_4_INIT_VALUE;

            storage_flag = 1;
        }
*/
        if(storage_flag)
        {
            //读取到的变量值出错，重新存储变量
            user_storage_cmd();
        }
    }
    //----------------------------------------------------------------------------//
    // Function Name : user_storage_save_customize_var
    // Description	 : 存储用户变量的值，若对应用户变量值跟对应存储变量值相同则不存储
    // Parameters	 : NONE
    // Returns		 : NONE
    //----------------------------------------------------------------------------//
    void user_storage_save_customize_var(void)
    {
        //检查当前变量值是否跟存储的变量值不同，不同则更新
        if(
               gu8_volume != gst_user_customize_var.volume
            || gu16_song_index != gst_user_customize_var.song_index
            // || gu8_var3 != gst_user_customize_var.var_3
            // || gu8_var4 != gst_user_customize_var.var_4
        )
        {
            gst_user_customize_var.volume = gu8_volume;
            gst_user_customize_var.song_index = gu16_song_index;
            // gst_user_customize_var.var_3 = gu8_var3;
            // gst_user_customize_var.var_4 = gu8_var4;

            //存储变量
            user_storage_cmd();
        }
    }
#endif

#if USER_CUSTOMIZE_BREAK_POINT_CMD
    //----------------------------------------------------------------------------//
    // Function Name : user_storage_set_customize_audioplay_breakpoint
    // Description	 : 设置自定义播放断点存储地址
    // Parameters	 : NONE
    // Returns		 : NONE
    //----------------------------------------------------------------------------//
    void user_storage_set_customize_audioplay_breakpoint(void)
    {
        STORAGE_SetVar((void *)gst_user_customize_audioplay_breakpoint, USER_CUSTOMIZE_BREAK_POINT_SIZE);
    }
    //----------------------------------------------------------------------------//
    // Function Name : user_storage_get_customize_play_break_point
    // Description	 : 得到对应用户断点值，若检测存储断点值出错则重新赋初值
    // Parameters	 : @u32 p_index:用户断点下标
    //                              USER_CUSTOMIZE_BREAK_POINT_1_INDEX //自定义断点1
    //                              USER_CUSTOMIZE_BREAK_POINT_2_INDEX //自定义断点2
    // Returns		 : NONE
    //----------------------------------------------------------------------------//
    void user_storage_get_customize_play_break_point(u32 p_index)
    {
        //获取用户使用的断点信息值
        gst_user_customize_play_breakpoint[p_index].curaddr = gst_user_customize_audioplay_breakpoint[p_index].curaddr;
        gst_user_customize_play_breakpoint[p_index].process_ount = gst_user_customize_audioplay_breakpoint[p_index].process_ount;
        gst_user_customize_play_breakpoint[p_index].rand[0] = gst_user_customize_audioplay_breakpoint[p_index].rand[0];
        gst_user_customize_play_breakpoint[p_index].rand[1] = gst_user_customize_audioplay_breakpoint[p_index].rand[1];
    }
    //----------------------------------------------------------------------------//
    // Function Name : user_storage_get_all_customize_play_break_point
    // Description	 : 得到所有用户断点值，若检测存储断点值出错则重新赋初值
    // Parameters	 : NONE
    // Returns		 : NONE
    //----------------------------------------------------------------------------//
    void user_storage_get_all_customize_play_break_point(void)
    {
        u32 i = 0;

        for(i = 0;i < USER_SBC_AUD_BREAK_POINT_NUM;++i)
        {
            user_storage_get_customize_play_break_point(i);
        }
    }
    //----------------------------------------------------------------------------//
    // Function Name : user_storage_save_breakpoint
    // Description   : 保存自定义断点信息
    // Parameters    :
    //                  @u8 p_index : 断点下标
    // Returns       :
    //                  NONE
    //----------------------------------------------------------------------------//
    void user_storage_save_breakpoint(u32 p_index)
    {
        u8 storege_flag = 0;

        //检查自带断点值是否跟当前播放的断点值不同，不同则更新
        if(
            *gu32_p_SbcSeekCtl_CurAddr != *gu32_p_CurAddr
            || *gu32_p_SbcSeekCtl_process_count != *gs32_p_AudioBufCtl_process_count
            || gu16_p_SbcSeekCtl_rand[0] != gu16_p_SBCRam[488]
            || gu16_p_SbcSeekCtl_rand[1] != gu16_p_SBCRam[489]
        )
        {
            //更新自带断点值
            *gu32_p_SbcSeekCtl_CurAddr = *gu32_p_CurAddr;
            *gu32_p_SbcSeekCtl_process_count = *gs32_p_AudioBufCtl_process_count;
            gu16_p_SbcSeekCtl_rand[0] = gu16_p_SBCRam[488];
            gu16_p_SbcSeekCtl_rand[1] = gu16_p_SBCRam[489];

            storege_flag = 1;
        }

        //检查用户断点值是否跟当前播放的断点值不同，不同则更新
        if(
            gst_user_customize_play_breakpoint[p_index].curaddr != *gu32_p_CurAddr
            || gst_user_customize_play_breakpoint[p_index].process_ount != *gs32_p_AudioBufCtl_process_count
            || gst_user_customize_play_breakpoint[p_index].rand[0] != gu16_p_SBCRam[488]
            || gst_user_customize_play_breakpoint[p_index].rand[1] != gu16_p_SBCRam[489]
        )
        {
            //更新用户断点值
            gst_user_customize_play_breakpoint[p_index].curaddr = *gu32_p_CurAddr;
            gst_user_customize_play_breakpoint[p_index].process_ount = *gs32_p_AudioBufCtl_process_count;
            gst_user_customize_play_breakpoint[p_index].rand[0] = gu16_p_SBCRam[488];
            gst_user_customize_play_breakpoint[p_index].rand[1] = gu16_p_SBCRam[489];

            //更新存储断点值
            gst_user_customize_audioplay_breakpoint[p_index].curaddr = *gu32_p_CurAddr;
            gst_user_customize_audioplay_breakpoint[p_index].process_ount = *gs32_p_AudioBufCtl_process_count;
            gst_user_customize_audioplay_breakpoint[p_index].rand[0] = gu16_p_SBCRam[488];
            gst_user_customize_audioplay_breakpoint[p_index].rand[1] = gu16_p_SBCRam[489];

            storege_flag = 1;
        }

        if(storege_flag)
        {
            //存储断点信息,会先执行一次存储操作，若存储失败则继续执行存储操作，直到成功为止
            user_storage_cmd();
        }
    }
    //----------------------------------------------------------------------------//
    // Function Name : user_storage_clean_breakpoint
    // Description   : 清除自定义断点信息
    // Parameters    :
    //                  @u8 p_index : 断点下标
    // Returns       :
    //                  NONE
    //----------------------------------------------------------------------------//
    void user_storage_clean_breakpoint(u32 p_index)
    {
        //清除存储断点值
        gst_user_customize_audioplay_breakpoint[p_index].curaddr = 0;
        gst_user_customize_audioplay_breakpoint[p_index].process_ount = 0;
        gst_user_customize_audioplay_breakpoint[p_index].rand[0] = 0;
        gst_user_customize_audioplay_breakpoint[p_index].rand[1] = 0;

        //清除用户断点值
        gst_user_customize_play_breakpoint[p_index].curaddr = 0;
        gst_user_customize_play_breakpoint[p_index].process_ount = 0;
        gst_user_customize_play_breakpoint[p_index].rand[0] = 0;
        gst_user_customize_play_breakpoint[p_index].rand[1] = 0;

        //存储断点信息,会先执行一次存储操作，若存储失败则继续执行存储操作，直到成功为止
        user_storage_cmd();
    }
    //----------------------------------------------------------------------------//
    // Function Name : user_storage_restore_breakpoint
    // Description   : 恢复断点信息
    // Parameters    :
    //                  @u8 p_index : 断点下标
    // Returns       :
    //                  NONE
    //----------------------------------------------------------------------------//
    void user_storage_restore_breakpoint(u32 p_index)
    {
        //恢复断点信息
        *gu32_p_SbcSeekCtl_CurAddr = gst_user_customize_play_breakpoint[p_index].curaddr;
        *gu32_p_SbcSeekCtl_process_count = gst_user_customize_play_breakpoint[p_index].process_ount;
        gu16_p_SbcSeekCtl_rand[0] = gst_user_customize_play_breakpoint[p_index].rand[0];
        gu16_p_SbcSeekCtl_rand[1] = gst_user_customize_play_breakpoint[p_index].rand[1];

        SbcRamEBufCtrl_Stop(SPI_MODE);
    }
    //----------------------------------------------------------------------------//
    // Function Name : user_storage_clean_all_breakpoint
    // Description   : 清除所有保存的断点信息
    // Parameters    :
    //                  NONE
    // Returns       :
    //                  NONE
    //----------------------------------------------------------------------------//
    void user_storage_clean_all_breakpoint(void)
    {
        u32 i = 0;

        //清除自带的断点信息
        user_storage_clean_sbc_self_contained_breakpoint();

        //清除所有用户断点值和存储断点值
        for(i = 0;i < USER_SBC_AUD_BREAK_POINT_NUM;++i)
        {
            //清除存储断点值
            gst_user_customize_audioplay_breakpoint[i].curaddr = 0;
            gst_user_customize_audioplay_breakpoint[i].process_ount = 0;
            gst_user_customize_audioplay_breakpoint[i].rand[0] = 0;
            gst_user_customize_audioplay_breakpoint[i].rand[1] = 0;

            //清除用户断点值
            gst_user_customize_play_breakpoint[i].curaddr = 0;
            gst_user_customize_play_breakpoint[i].process_ount = 0;
            gst_user_customize_play_breakpoint[i].rand[0] = 0;
            gst_user_customize_play_breakpoint[i].rand[1] = 0;
        }

        //存储断点信息,会先执行一次存储操作，若存储失败则继续执行存储操作，直到成功为止
        user_storage_cmd();
    }
    //----------------------------------------------------------------------------//
    // Function Name : user_storage_clean_sbc_self_contained_breakpoint
    // Description   : 清除sbc自带的播放断点值
    // Parameters    :
    //                  NONE
    // Returns       :
    //                  NONE
    //----------------------------------------------------------------------------//
    void user_storage_clean_sbc_self_contained_breakpoint(void)
    {
        *gu32_p_SbcSeekCtl_CurAddr = 0;
        *gu32_p_SbcSeekCtl_process_count = 0;
        gu16_p_SbcSeekCtl_rand[0] = 0;
        gu16_p_SbcSeekCtl_rand[1] = 0;

        SbcRamEBufCtrl_Stop(SPI_MODE);
    }
#endif
//----------------------------------------------------------------------------//
// Function Name : user_storage_cmd
// Description	 : 储存命令,会先存储一次，失败的话会在user_storage_cmd_process()函数
//                 里面继续存储，直到成功
// Parameters	 : NONE
// Returns		 : NONE
//----------------------------------------------------------------------------//
void user_storage_cmd(void)
{
    s8 storage_status = 0x7F;

    if(C_FLASH_ERASE_IDLE == SBC_AutoSave_GetEraseState())
    {
        //存储变量
        storage_status = STORAGE_Save(_STORAGE_MODE);

        if(USER_STORAGE_BREAK_POINT_STATUS_ERROR == storage_status)
        {
            //存储出错时则继续执行存储处理
            gu8_storage_status = USER_STORAGE_BREAK_POINT_STATUS_UNDONE;

            #if _UART_MODULE && _DEBUG_MODE
                dprintf("storage error,continues execute\r\n");
            #endif
        }
        else if(USER_STORAGE_BREAK_POINT_STATUS_SUCCESS == storage_status)
        {
            //存储成功时则不再执行存储处理
            gu8_storage_status = USER_STORAGE_BREAK_POINT_STATUS_DONE;

            #if _UART_MODULE && _DEBUG_MODE
                dprintf("storage success\r\n");
                dprintf("storage var num:%d\r\n",USER_STORAGE_VAR_CNT);
                dprintf("storage var size:%d\r\n",USER_STORAGE_VAR_SIZE);
            #endif
        }
    }
    else
    {
        // 继续等待存储空闲后继续存储
        gu8_storage_status = USER_STORAGE_BREAK_POINT_STATUS_UNDONE;

        #if _UART_MODULE && _DEBUG_MODE
            dprintf("storage busy,continues execute\r\n");
        #endif
    }
}
//----------------------------------------------------------------------------//
// Function Name : user_storage_init
// Description	 : 储存变量时的初始化处理
// Parameters	 : NONE
// Returns		 : NONE
//----------------------------------------------------------------------------//
void user_storage_init(void)
{
    #if USER_CUSTOMIZE_BREAK_POINT_CMD
        // 获取sbc_audioapi.c文件里关于断点信息的指针
        user_storage_get_sbc_breakpoint_val();
    #endif

    #if USER_CUSTOMIZE_BREAK_POINT_CMD
        u32 i = 0;
    #endif

    #if USER_CUSTOMIZE_VAR_CMD || USER_CUSTOMIZE_BREAK_POINT_CMD
        //设置自定义的第一次存储标志变量的存储地址
        user_storage_set_customize_first_storage_flag();
    #endif

    #if USER_CUSTOMIZE_VAR_CMD
        //设置自定义变量地址
        user_storage_set_customize_var();
    #endif

	#if USER_CUSTOMIZE_BREAK_POINT_CMD
		//设置自定义播放断点存储地址
		user_storage_set_customize_audioplay_breakpoint();
	#endif

    #if USER_CUSTOMIZE_VAR_CMD || USER_CUSTOMIZE_BREAK_POINT_CMD
        //第一次存储时初始化需要存储的变量
        if(0 == gu32_user_customize_first_storage_flag)
        {
            gu32_user_customize_first_storage_flag = 1;

            //自定义变量初始化
            #if USER_CUSTOMIZE_VAR_CMD
                //音量值
                gst_user_customize_var.volume = USER_VAR_1_INIT_VALUE;
                //歌曲下标
                gst_user_customize_var.song_index = USER_VAR_2_INIT_VALUE;
                //变量4
                // gst_user_customize_var.var_3 = USER_VAR_3_INIT_VALUE;
                //变量5
                // gst_user_customize_var.var_4 = USER_VAR_4_INIT_VALUE;
            #endif

            //自定义断点信息初始化
            #if USER_CUSTOMIZE_BREAK_POINT_CMD
                for(i = 0;i < USER_SBC_AUD_BREAK_POINT_NUM;++i)
                {
                    gst_user_customize_audioplay_breakpoint[i].curaddr = USER_CUSTOMIZE_BREAK_POINT_CURADDR_VAR_INIT_VALUE;
                    gst_user_customize_audioplay_breakpoint[i].process_ount = USER_CUSTOMIZE_BREAK_POINT_PROCESS_OUNT_VAR_INIT_VALUE;
                    gst_user_customize_audioplay_breakpoint[i].rand[0] = USER_CUSTOMIZE_BREAK_POINT_RAND_0_VAR_INIT_VALUE;
                    gst_user_customize_audioplay_breakpoint[i].rand[1] = USER_CUSTOMIZE_BREAK_POINT_RAND_1_VAR_INIT_VALUE;
                }
            #endif

            //存储变量
            user_storage_cmd();
        }
    #endif

    #if USER_CUSTOMIZE_VAR_CMD
        //获取到全部用户变量值
        user_storage_get_all_customize_var_value();
    #endif

    //获取到全部用户断点值
    #if USER_CUSTOMIZE_BREAK_POINT_CMD
        user_storage_get_all_customize_play_break_point();
    #endif
}
//----------------------------------------------------------------------------//
// Function Name : user_storage_get_status
// Description	 : 得到储存状态
// Parameters	 : NONE
// Returns		 : USER_STORAGE_BREAK_POINT_STATUS_UNDONE  //存储断点信息未完成
//                 USER_STORAGE_BREAK_POINT_STATUS_DONE    //存储断点信息完成
//----------------------------------------------------------------------------//
u8 user_storage_get_status(void)
{
    return gu8_storage_status;
}
//----------------------------------------------------------------------------//
// Function Name : user_storage_get_work_status
// Description	 : 得到储存工作状态
// Parameters	 : NONE
// Returns		 : STATUS_IDLE         //空闲
//                 STATUS_WORKING     //工作
//----------------------------------------------------------------------------//
u8 user_storage_get_work_status(void)
{
    if(
        USER_STORAGE_BREAK_POINT_STATUS_DONE == gu8_storage_status
        || 0 == gu8_storage_status
    )
    {
        return STATUS_IDLE;
    }

    return STATUS_WORKING;
}
//----------------------------------------------------------------------------//
// Function Name : user_storage_cmd_process
// Description	 : 储存命令处理,需放在主循环中
// Parameters	 : NONE
// Returns		 : NONE
//----------------------------------------------------------------------------//
void user_storage_cmd_process(void)
{
    s8 storage_status = 0x7F;

    if(C_FLASH_ERASE_IDLE == SBC_AutoSave_GetEraseState())
    {
        if(USER_STORAGE_BREAK_POINT_STATUS_UNDONE == gu8_storage_status)
        {
            //存储断点信息,若存储失败则继续执行存储操作，直到成功为止
            storage_status = STORAGE_Save(_STORAGE_MODE);

            if(USER_STORAGE_BREAK_POINT_STATUS_SUCCESS == storage_status)
            {
                //存储断点信息成功
                gu8_storage_status = USER_STORAGE_BREAK_POINT_STATUS_DONE;

                #if _UART_MODULE && _DEBUG_MODE
                    dprintf("storage success\r\n");
                    dprintf("storage var num:%d\r\n",USER_STORAGE_VAR_CNT);
                    dprintf("storage var size:%d\r\n",USER_STORAGE_VAR_SIZE);
                #endif
            }
            else
            {
                gu8_storage_status = USER_STORAGE_BREAK_POINT_STATUS_UNDONE;

                #if _UART_MODULE && _DEBUG_MODE
                    dprintf("storage undone,continues execute\r\n");
                #endif
            }
        }
    }
}

#endif


