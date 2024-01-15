#include "include.h"
#include "nx1_i2c.h"
//------------------------------------------------------------------//
// SYS is any App Busy??
// Parameter: 
//          NONE
// return value:
//          true / false 
//------------------------------------------------------------------//
bool CB_SYS_isAppBusy(void)
{
    #if _QCODE_BUILD 
    if(QC_IsBusy()==true) return true;
    #endif

    if(!SleepFlag) return true;

#if _I2C_MODULE
    if(I2C_IsBusBusy()==true) return true;
#if _I2C_DEVICE_MODE == I2C_SLAVE
    if(I2C_IsAddrHit()==true || I2C_IsCmpl()==true) return true;
#endif
#endif

    #if _PD_PLAY
    if(PD_GetStatus()!=STATUS_STOP) return true;
    #endif

    #if _SOUND_DETECTION
    if(SOUND_DETECTION_GetStatus()!=STATUS_STOP) return true;
    #endif

    #if _IR_TX_EN&&_IR_MODULE
    if(IR_GetTxStatus()!=0) return true;
    #endif	

    #if _QFID_MODULE
    if(QFID_GetStatus()!=DISABLE) return true;
    #endif

    #if _LEDSTR_MODULE
    U8 _LEDSTR_Ch;
    for(_LEDSTR_Ch = 0; _LEDSTR_Ch < _LEDSTR_CH_NUM; _LEDSTR_Ch++) {
        if(LEDSTR_GetStatus(_LEDSTR_Ch)==STATUS_PLAYING) return true;
    }
//    if(LEDSTR_GetStatus() != STATUS_STOP) return true;
    #endif
    
    #if _LEDSTRn_MODULE
        if(LEDSTRn_GetStatus()!=STATUS_STOP) return true;
    #endif

    #if _LEDSTRm_MODULE
        if(LEDSTRm_GetStatus()!=STATUS_STOP) return true;
    #endif

    #if _SL_MODULE
    if(SL_GetStatus() != SL_OFF) return true;
    #endif
    
    #if _VR_FUNC
    if(VRState == VR_ON) return true;
    #endif	

    #if _AUDIO_SENTENCE
    U8 _sentCh;
    for(_sentCh = 0; _sentCh < _AUDIO_SENTENCE_CH_NUM; _sentCh++) {
        if((SentenceStatusMuteTimer[_sentCh]>>28) == STATUS_MUTE) return true;
    }
    #endif	
    
    #if _ACTION_MODULE
    U8 _vioCh;
    for(_vioCh = 0; _vioCh < _VIO_CH_NUM; _vioCh++) {
        if(ACTION_GetIOStatus(_vioCh+_AUDIO_API_CH_NUM)==STATUS_PLAYING) return true;
    }
    #endif
    
    #if _AUDIO_RECORD
    if(AudioRecord_GetStatus()==STATUS_AUDIO_RECORDING) return true;
    #endif

    U8 _audioSts = AUDIO_GetStatus_All();  //get music_status
    if((_audioSts == STATUS_PLAYING) || (_audioSts == STATUS_RECORDING)) return true;

    #if _IS_APPS_USE_DAC
    if(!DacAssigner_isEmpty()) return true;//check it after AUDIO_GetStatus_All 
    #endif
    
    #if _IS_APPS_USE_DAC
    if(!DacMixerRamp_IsIdle()) {//check it after DacAssigner_isEmpty
        DacMixerRamp_SetRampDown();//  trig ramp down!!!!
        return true;
    }
    #endif
    
    return false;
}

//------------------------------------------------------------------//
// SYS is sleepable?
// Parameter: 
//          NONE
// return value:
//          true / false 
//------------------------------------------------------------------//
bool CB_SYS_isSleepable(void)
{
    #if _TOUCHKEY_MODULE
    if(Touch_GetPADStatus()) return false;//TouchKey failure
    if(!Touch_GetSleepFlag()) return false;//State machine in busy
    #endif
    
    #if _QCODE_BUILD 
    if(QC_IsSleepable()==false) return false;
    #endif
    
    #if (_IR_MODULE && _IR_LOW_POWER_TYPE && _IR_RX_EN)
    if(!IR_LoPwr_RxGetSleepFlag()) return false;    
    #endif
    
    return true;
}