/* Includes ------------------------------------------------------------------*/

#include "include.h"
#include "user_main.h"
//#include "nx1_gpio.h"

void CB_SOUND_DETECTION_DetectSound(void)
{
    #if USER_TALKBACK_MODULE
        if(ENABLE == user_talk_back_get_cmd())
        {
            #if _ADPCM_RECORD && _ADPCM_RECORD_ERASING && _ADPCM_RECORD_SOUND_TRIG
                ADPCM_SaveStartFifoCnt();
            #endif

            #if USER_TALK_BACK_HINT_EN
                USER_TALK_BACK_LED_ON();
            #endif

            gst_user_talk_back.detect_sound_flag = 1;
        }
    #else
        #if _ADPCM_RECORD && _ADPCM_RECORD_ERASING && _ADPCM_RECORD_SOUND_TRIG
            ADPCM_SaveStartFifoCnt();
        #endif
    #endif
}

void CB_SOUND_DETECTION_MuteSound(void)
{
    #if USER_TALKBACK_MODULE
        if(ENABLE == user_talk_back_get_cmd())
        {
            #if _ADPCM_RECORD && _ADPCM_RECORD_ERASING && _ADPCM_RECORD_SOUND_TRIG
                ADPCM_SaveEndFifoCnt();
            #endif

            #if USER_TALK_BACK_HINT_EN
                USER_TALK_BACK_LED_OFF();
            #endif

            gst_user_talk_back.detect_sound_flag = 0;
        }
    #else
        #if _ADPCM_RECORD && _ADPCM_RECORD_ERASING && _ADPCM_RECORD_SOUND_TRIG
            ADPCM_SaveEndFifoCnt();
        #endif
    #endif

    #if _SOUND_DETECTION
        SOUND_DETECTION_Init();
    #endif
}