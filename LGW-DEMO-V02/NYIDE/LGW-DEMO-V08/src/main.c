//==========================================================================
// File Name          : main.c
// Description        : Example of AUDIO Play
//==========================================================================
#include "nx1_lib.h"
#include "user_main.h"


int main(void)
{
    // U8 volume=0;

    SYS_Init();

    //--------------------------------------------------------------------------//
    // 用户初始化
    //--------------------------------------------------------------------------//
    user_init();

    while(1)
    {

        //----------------------------------------------------------------------//
        // 用户程序
        //----------------------------------------------------------------------//
        user_main_service_loop();

        #if _DIRECTKEY_MODULE
            switch (DIRECTKEY_GetKey())
            {
                case PA0_PRESS:
                    AUDIO_Play(AUDIO_CH0,0,SPI_MODE);
                    break;

                case PA1_PRESS:
                    AUDIO_Pause(AUDIO_CH0);
                    break;

                case PA2_PRESS:
                    AUDIO_Resume(AUDIO_CH0);
                    break;

                case PA3_PRESS:
                    AUDIO_Stop(AUDIO_CH0);
                    break;

                case PA4_PRESS:
                    volume++;
                    if(volume>CH_VOL_15)
                    {
                        volume=CH_VOL_0;
                    }
                    AUDIO_SetChVolume(AUDIO_CH0,volume);
                    break;

                case PA5_PRESS:
                    break;

                case PA6_PRESS:
                    break;

                case PA7_PRESS:
                    break;

            }
        #endif

        //----------------------------------------------------------------------//
        // 系统部分，谨慎擅自修改
        //----------------------------------------------------------------------//
        AUDIO_ServiceLoop();
		UTILITY_ServiceLoop();
        SYS_ServiceLoop();
    }
}