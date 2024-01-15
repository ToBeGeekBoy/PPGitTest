/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "include.h"
#include "nx1_dac.h"
#include "nx1_gpio.h"
#include "nx1_timer.h"
#include "nx1_smu.h"

#if _IS_APPS_USE_DAC
//////////////////////////////////////////////////////////////////////
//  Defines
//////////////////////////////////////////////////////////////////////
#if _DAC_MODE
#define _DAC_RAMPUP_TIME_IN_MS            (_AUDIO_RAMP_UP_TIME)
#define _DAC_RAMPDN_TIME_IN_MS            (_AUDIO_RAMP_DOWN_TIME)
#if _DAC_RAMPUP_TIME_IN_MS<=0 || _DAC_RAMPDN_TIME_IN_MS<=0
#error "_DAC_RAMPUP_TIME can not set 0."
#endif
#define _DAC_RAMPUP_TIME_IN_CLK     ((_DAC_RAMPUP_TIME_IN_MS)*((OPTION_HIGH_FREQ_CLK)/1000))
#define _DAC_RAMPDN_TIME_IN_CLK     ((_DAC_RAMPDN_TIME_IN_MS)*((OPTION_HIGH_FREQ_CLK)/1000))

#define _DAC_RAMP_FIX_PT_SHIFT      (14)
#define _DAC_RAMP_VOLTAGE_MAX       (0x10000<<_DAC_RAMP_FIX_PT_SHIFT)//Extend 14 bits @ bias value
#define _DAC_RAMP_VOLTAGE_MIN       (0x8000<<_DAC_RAMP_FIX_PT_SHIFT)//Extend 14 bits @ idle value
#define _DAC_RAMP_VOLTAGE_DIFF      (_DAC_RAMP_VOLTAGE_MAX-_DAC_RAMP_VOLTAGE_MIN)

#define _DAC_RAMPUP_VOLTAGE_PER_CLK ((_DAC_RAMP_VOLTAGE_DIFF+(_DAC_RAMPUP_TIME_IN_CLK/2))/(_DAC_RAMPUP_TIME_IN_CLK))
#define _DAC_RAMPDN_VOLTAGE_PER_CLK ((_DAC_RAMP_VOLTAGE_DIFF+(_DAC_RAMPDN_TIME_IN_CLK/2))/(_DAC_RAMPDN_TIME_IN_CLK))

#if _DAC_RAMPUP_VOLTAGE_PER_CLK<=0
#error "Ramp Up time too long."
#endif
#if _DAC_RAMPDN_VOLTAGE_PER_CLK<=0
#error "Ramp Down time too long."
#endif
#endif

#ifndef _DAC_RAMP_DEFAULT_CH
#define _DAC_RAMP_DEFAULT_CH    HW_CH0
#endif
#ifndef _DAC_RAMP_DEFAULT_SR
#define _DAC_RAMP_DEFAULT_SR    SAMPLE_RATE_8000
#endif
#ifndef _DAC_RAMP_DEFAULT_ITP
#define _DAC_RAMP_DEFAULT_ITP   DISABLE
#endif

typedef enum {
    kDacIdle,//0
    kWaitAmpOpenUp,
    kDacRampUp,
    kWaitRampUpChk,
    kDacBias,
    kWaitAmpShutDn,
    kDacRampDn,
    kWaitRampDnChk,
}DAC_RAMP_STS;

typedef struct {
    DAC_RAMP_STS    ctrlSts;
#if _AUDIO_AMP_SD_CTL
    U32             extAmpStartTime;
#endif
#if _DAC_MODE
    U32             outDataTemp;
    U16             timerBorrowData;
    U8              timerBorrowBit;
    U8              fifoDownCount;
    S8              useDacCh;//-1(default),0,1
    bool            isDacMode;
#endif
}S_DacRampCtrl;

//////////////////////////////////////////////////////////////////////
//  Variables
//////////////////////////////////////////////////////////////////////
S_DacRampCtrl sDacRampCtrl;
#if _AUDIO_AMP_SD_CTL
//------------------------------------------------------------------//
// Init Ext Amplifier SD pin
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
static void ExtAmplifier_Init(void)
{
#if (_AUDIO_AMP_SD_CTL_ACTIVE==ACTIVE_LOW)
    GPIO_Init(_AUDIO_AMP_SD_CTL_PORT,_AUDIO_AMP_SD_CTL_PIN,GPIO_OUTPUT_LOW); 
#else
    GPIO_Init(_AUDIO_AMP_SD_CTL_PORT,_AUDIO_AMP_SD_CTL_PIN,GPIO_OUTPUT_HIGH);
#endif
}

//------------------------------------------------------------------//
// Set Ext Amplifier ShutDown
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
static void ExtAmplifier_ShutDown(void)
{
#if (_AUDIO_AMP_SD_CTL_ACTIVE==ACTIVE_LOW)
    GPIO_Write(_AUDIO_AMP_SD_CTL_PORT,_AUDIO_AMP_SD_CTL_PIN,BIT_UNSET);
#else
    GPIO_Write(_AUDIO_AMP_SD_CTL_PORT,_AUDIO_AMP_SD_CTL_PIN,BIT_SET);
#endif
}

//------------------------------------------------------------------//
// Set Ext Amplifier OpenUp
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
static void ExtAmplifier_OpenUp(void)
{
#if (_AUDIO_AMP_SD_CTL_ACTIVE==ACTIVE_LOW)
    GPIO_Write(_AUDIO_AMP_SD_CTL_PORT,_AUDIO_AMP_SD_CTL_PIN,BIT_SET);
#else
    GPIO_Write(_AUDIO_AMP_SD_CTL_PORT,_AUDIO_AMP_SD_CTL_PIN,BIT_UNSET);
#endif
}
#endif//#if _AUDIO_AMP_SD_CTL

#if _DAC_MODE
//----------------------------------------------------------------------------//
// Get ITP information from DAC
// Parameter: 
//          _dacCh      :HW_CH0(0),HW_CH1(1)
// return value:
//          (NX1_OTP)1,4
//          (NX1_EF)1,4,8,16,32
//----------------------------------------------------------------------------//
static U8 _getItpPts(S8 _dacCh) {
    //if(_dacCh<0||_dacCh>=_ASSIGNABLE_DAC_NUM) return 0;
    U32 _dacCtrl = pHwCtl[_dacCh].p_dac->CH_Ctrl;

    if(_dacCtrl&C_DAC_Up_Sample_En) {
#if _EF_SERIES
        _dacCtrl = (_dacCtrl>>12)&0x3;//get C_DAC_Up_Sample
        return (4<<_dacCtrl);//base on 4 pts
#else
        return 4;//fix 4
#endif
    } else {
        return 1;
    }
}

//----------------------------------------------------------------------------//
// Get period infomation from Timer
// Parameter: 
//          _dacCh      :HW_CH0(0),HW_CH1(1)
// return value:
//          0x0~0xFFFF
//----------------------------------------------------------------------------//
static U16 _getPeriodCnt(S8 _dacCh) {
    //if(_dacCh<0||_dacCh>=_ASSIGNABLE_DAC_NUM) return -1;
    return ((pHwCtl[_dacCh].p_tmr->Data)&0xFFFF);//get C_TMR_Data+1
}

//----------------------------------------------------------------------------//
// dac ramp get main ch
// Parameter: 
//          NONE
// return value:
//          ramp ch:HW_CH0(0),HW_CH1(1)
//----------------------------------------------------------------------------//
S8 DacMixerRamp_GetRampCh() {
    S_DacRampCtrl *pCtrl = &sDacRampCtrl;
    return pCtrl->useDacCh;
}

//----------------------------------------------------------------------------//
// Get dac ramp data to fifo(call by ISR)
// Parameter: 
//          _dacCh      :HW_CH0(0),HW_CH1(1)
// return value:
//          0x8000 up to 0x0000
//----------------------------------------------------------------------------//
U16 DacMixerRamp_GetData(S8 _dacCh) {
    S_DacRampCtrl *pCtrl = &sDacRampCtrl;
    if(_dacCh != pCtrl->useDacCh) {
        return (pCtrl->ctrlSts==kDacIdle)?0x8000:0x0000;
    } else {
        return ((pCtrl->outDataTemp>>_DAC_RAMP_FIX_PT_SHIFT)&0xFFFF);
    }
}

//----------------------------------------------------------------------------//
// Borrow timer resource when dac ramping
// Parameter: 
//          _dacCh      :HW_CH0(0),HW_CH1(1)
// return value:
//          NONE
//----------------------------------------------------------------------------//
void DacMixerRamp_BorrowTimer(S8 _dacCh)
{
    S_DacRampCtrl *pCtrl = &sDacRampCtrl;
    if((GetUpsampleStatus()>>_dacCh)&0x1) {
#if _EF_SERIES
        pCtrl->timerBorrowBit = _DAC_UP_SAMPLE_SETTING+2;//2~5
#else
        pCtrl->timerBorrowBit = 2;
#endif
        pHwCtl[_dacCh].p_tmr->Data = 
            (_getPeriodCnt(_dacCh)<<(pCtrl->timerBorrowBit))&0xFFFF;
    } else {
        pCtrl->timerBorrowBit=0;
    }
    pCtrl->timerBorrowData = _getPeriodCnt(_dacCh);
}
#endif

//----------------------------------------------------------------------------//
// dac ramp module init
// Parameter: 
//          NONE
// return value:
//          NONE
//----------------------------------------------------------------------------//
void DacMixerRamp_Init(void) {
#if _AUDIO_AMP_SD_CTL
    ExtAmplifier_Init();
#endif
    S_DacRampCtrl *pCtrl = &sDacRampCtrl;
    memset((void*)pCtrl,0,sizeof(S_DacRampCtrl));
    pCtrl->ctrlSts = kDacIdle;
    
#if _DAC_MODE
    pCtrl->outDataTemp=_DAC_RAMP_VOLTAGE_MIN;
    pCtrl->fifoDownCount=DAC_FIFO_LEN*2;
    pCtrl->useDacCh=-1;
    pCtrl->isDacMode=false;//default PP or PWM
#endif
}

void DacMixerRamp_SetDacModeUse(bool _isDacModeUse)
{
#if _DAC_MODE
    if(DacMixerRamp_IsIdle()==false) return;
    _isDacModeUse = (_isDacModeUse>0); //Normalize to 0 and 1
    S_DacRampCtrl *pCtrl = &sDacRampCtrl;
    pCtrl->isDacMode=_isDacModeUse;
    SMU_PeriphReset(SMU_PERIPH_DAC);
#endif
}

bool DacMixerRamp_IsDacModeUse(void)
{
#if _DAC_MODE
    S_DacRampCtrl *pCtrl = &sDacRampCtrl;
    return pCtrl->isDacMode;
#else
    return false;
#endif
}

//----------------------------------------------------------------------------//
// dac ramp up setting(call it from InitDac)
// Parameter: 
//          _dacCh      :HW_CH0(0),HW_CH1(1)
// return value:
//          0x8000 up to 0x0000
//----------------------------------------------------------------------------//
void DacMixerRampUp(S8 _dacCh) {
    //if(_dacCh<0||_dacCh>=_ASSIGNABLE_DAC_NUM) return -1;
    S_DacRampCtrl *pCtrl = &sDacRampCtrl;
    //CH_TypeDef *chx = pHwCtl[_dacCh].p_dac;
    switch(pCtrl->ctrlSts) {
        case kDacIdle:
#if _DAC_MODE
            if(DacMixerRamp_IsDacModeUse()==true) {
                pCtrl->useDacCh = _dacCh;
                DAC_CH0->CH_Data = 0x8000;
                DAC_CH0->CH_Ctrl &= (~C_DAC_Trig_Timer_En);
                DAC_CH0->CH_Ctrl &= ~(C_DAC_Trig|C_DAC_Up_Sample_En);
                DAC_CH0->CH_Ctrl |= DAC_MANUAL_TRIG|C_DAC_Data_Mixer_En;
                DAC_CH1->CH_Data = 0x8000;
                DAC_CH1->CH_Ctrl &= (~C_DAC_Trig_Timer_En);
                DAC_CH1->CH_Ctrl &= ~(C_DAC_Trig|C_DAC_Up_Sample_En);
                DAC_CH1->CH_Ctrl |= DAC_MANUAL_TRIG|C_DAC_Data_Mixer_En;
                DAC_VoltageDacCmd(ENABLE);
                if(_dacCh==HW_CH0) {
                    DAC_CH1->CH_FIFO |= C_DAC_FIFO_Reset;
                    DAC_CH1->CH_Data = 0x0000;
                } else {
                    DAC_CH0->CH_FIFO |= C_DAC_FIFO_Reset;
                    DAC_CH0->CH_Data = 0x0000;
                }
                pCtrl->ctrlSts = kWaitAmpOpenUp;
                break;
            }
#endif
        case kWaitRampDnChk:    //fall through
        case kDacRampDn:
#if _DAC_MODE
            if(DacMixerRamp_IsDacModeUse()==true) {
                pCtrl->fifoDownCount=DAC_FIFO_LEN*2;
            }
#endif
#if _AUDIO_AMP_SD_CTL
            ExtAmplifier_OpenUp();
            pCtrl->extAmpStartTime = SysTick;
#endif
            pCtrl->ctrlSts = kDacRampUp;
            break;
        case kWaitAmpShutDn:
            pCtrl->ctrlSts = kWaitRampUpChk;
            break;
        default:
            break;
    }
}

//----------------------------------------------------------------------------//
// dac ramp down setting(DacAssigner must empty)(call it from SYS_ServiceLoop)
// Parameter: 
//          NONE
// return value:
//          0x8000 up to 0x0000
//----------------------------------------------------------------------------//
void DacMixerRampDn() {
    //if(_dacCh<0||_dacCh>=_ASSIGNABLE_DAC_NUM) return -1;
    S_DacRampCtrl *pCtrl = &sDacRampCtrl;
    switch(pCtrl->ctrlSts) {
        case kDacBias:
            pCtrl->ctrlSts = kWaitAmpShutDn;
            break;
        case kWaitRampUpChk:    //fall through
        case kDacRampUp:        //fall through
#if _DAC_MODE
            if(DacMixerRamp_IsDacModeUse()==true) {
                pCtrl->fifoDownCount=DAC_FIFO_LEN*2;
            }
#endif
#if _AUDIO_AMP_SD_CTL
            ExtAmplifier_ShutDown();
            pCtrl->extAmpStartTime = SysTick;
#endif
            pCtrl->ctrlSts = kDacRampDn;
            break;
        case kWaitAmpOpenUp:
            pCtrl->ctrlSts = kWaitRampDnChk;
            break;
        default:
            break;
    }
}

//----------------------------------------------------------------------------//
// MixerRamp Sync state machine (call by Main loop)
// Parameter: 
//          NONE
// return value:
//          NONE
//----------------------------------------------------------------------------//
void DacMixerRamp_SyncStsInPolling() {
    S_DacRampCtrl *pCtrl = &sDacRampCtrl;
#if _DAC_MODE
    S8 _dacCh;
    U8 upsample;
    CH_TypeDef *chx;
#endif
    switch(pCtrl->ctrlSts) {
        case kWaitAmpOpenUp:
#if _DAC_MODE
            if(DacMixerRamp_IsDacModeUse()==true) {
                pCtrl->fifoDownCount=DAC_FIFO_LEN*2;
            }
#endif
#if _AUDIO_AMP_SD_CTL
            ExtAmplifier_OpenUp();
            pCtrl->extAmpStartTime = SysTick;
#endif
            pCtrl->ctrlSts = kDacRampUp;
            break;
        case kWaitRampUpChk:
#if _DAC_MODE
            if(DacMixerRamp_IsDacModeUse()==true) {
                _dacCh=DacMixerRamp_GetRampCh();
                chx=pHwCtl[_dacCh].p_dac;
                upsample=(GetUpsampleStatus()>>_dacCh)&0x1;
                if((((chx->CH_Ctrl)>>3)&0x1)!=upsample) {
                    DAC_SetUpSample(chx,upsample);
                }
                if(pCtrl->timerBorrowData == _getPeriodCnt(_dacCh)) {//if no change
                    pHwCtl[_dacCh].p_tmr->Data = 
                        ((pCtrl->timerBorrowData)>>(pCtrl->timerBorrowBit))&0xFFFF;
                }
                pCtrl->timerBorrowBit = 0;
                pCtrl->timerBorrowData = 0;
            }
#endif
            pCtrl->ctrlSts = kDacBias;
            break;
        case kWaitAmpShutDn:
#if _DAC_MODE
            if(DacMixerRamp_IsDacModeUse()==true) {
                pCtrl->fifoDownCount=DAC_FIFO_LEN*2;
            }
#endif
#if _AUDIO_AMP_SD_CTL
            ExtAmplifier_ShutDown();
            pCtrl->extAmpStartTime = SysTick;
#endif
            pCtrl->ctrlSts = kDacRampDn;
            break;
        case kWaitRampDnChk:
            do {
#if _DAC_MODE
                if(DacMixerRamp_IsDacModeUse()==true) {
                    DAC_CH0->CH_Ctrl &= ~(C_DAC_Trig_Timer_En);
                #if EF11FS_SERIES
                    DAC_CH0->CH_FIFO &= ~(C_DAC_FIFO_Empty_IEn); 
                #else
                    DAC_CH0->CH_FIFO &= ~(C_DAC_FIFO_Level_IEn|C_DAC_FIFO_Empty_IEn|C_DAC_FIFO_Full_IEn); 
                #endif
                    DAC_CH0->CH_FIFO |= C_DAC_FIFO_Reset; 
                    DAC_CH0->CH_Ctrl &= ~(C_DAC_Trig|C_DAC_Up_Sample_En);
                    DAC_CH0->CH_Ctrl |= C_DAC_Trig_Manual;
                    DAC_CH0->CH_Data = 0x8000;
                    DAC_CH0->CH_Ctrl &= (~(C_DAC_Data_Mixer_En));
                    
                    DAC_CH1->CH_Ctrl &= ~(C_DAC_Trig_Timer_En);
                #if EF11FS_SERIES
                    DAC_CH1->CH_FIFO &= ~(C_DAC_FIFO_Empty_IEn); 
                #else
                    DAC_CH1->CH_FIFO &= ~(C_DAC_FIFO_Level_IEn|C_DAC_FIFO_Empty_IEn|C_DAC_FIFO_Full_IEn); 
                #endif
                    DAC_CH1->CH_FIFO |= C_DAC_FIFO_Reset; 
                    DAC_CH1->CH_Ctrl &= ~(C_DAC_Trig|C_DAC_Up_Sample_En);
                    DAC_CH1->CH_Ctrl |= C_DAC_Trig_Manual;
                    DAC_CH1->CH_Data = 0x8000;
                    DAC_CH1->CH_Ctrl &= (~(C_DAC_Data_Mixer_En));
                    pCtrl->useDacCh = -1;
                    break; //do-while(0)
                }
#endif
#if _EF_SERIES
                DAC_PWMCmd(DISABLE);
#else
                DAC_PPCmd(DISABLE);
#endif
            } while(0);
            pCtrl->ctrlSts = kDacIdle;
            break;
        default:
            break;
    }
}

//----------------------------------------------------------------------------//
// MixerRamp Sync state machine(call by ISR)(only main ramp ch work)
// Parameter: 
//          _dacCh      :HW_CH0(0),HW_CH1(1)
// return value:
//          NONE
//----------------------------------------------------------------------------//
void DacMixerRamp_SyncStsInInterrupt(S8 _dacCh) {
    //if(_dacCh<0||_dacCh>=_ASSIGNABLE_DAC_NUM) return -1;
    S_DacRampCtrl *pCtrl = &sDacRampCtrl;
    
#if _DAC_MODE
    if(DacMixerRamp_IsDacModeUse()==true && _dacCh != pCtrl->useDacCh) return;
#endif
    
    switch(pCtrl->ctrlSts) {
        case kDacRampUp:
#if _DAC_MODE
            if(DacMixerRamp_IsDacModeUse()==true) {
                pCtrl->outDataTemp+=_DAC_RAMPUP_VOLTAGE_PER_CLK*
                                    _getItpPts(_dacCh)*(_getPeriodCnt(_dacCh)+1);
                if(pCtrl->outDataTemp>=_DAC_RAMP_VOLTAGE_MAX) {
                    pCtrl->outDataTemp=_DAC_RAMP_VOLTAGE_MAX;//overflow protect
                    if(pCtrl->fifoDownCount) {
                        --(pCtrl->fifoDownCount);
                    }
                }
            }
#endif
            do {
#if _DAC_MODE
                if(DacMixerRamp_IsDacModeUse()==true) {
                    if(pCtrl->outDataTemp!=_DAC_RAMP_VOLTAGE_MAX) break;
                    if(pCtrl->fifoDownCount>0) break;
                }
#endif
#if _AUDIO_AMP_SD_CTL
                if((SysTick-pCtrl->extAmpStartTime) <= _AUDIO_AMP_WAKUP_TIME) break;
#endif
                pCtrl->ctrlSts = kWaitRampUpChk;
            } while(0);
            break;
        case kDacRampDn:
#if _DAC_MODE
            if(DacMixerRamp_IsDacModeUse()==true) {
                pCtrl->outDataTemp-=_DAC_RAMPDN_VOLTAGE_PER_CLK*
                                    _getItpPts(_dacCh)*(_getPeriodCnt(_dacCh)+1);
                if(pCtrl->outDataTemp<=_DAC_RAMP_VOLTAGE_MIN) {
                    pCtrl->outDataTemp=_DAC_RAMP_VOLTAGE_MIN;//underflow protect
                    if(pCtrl->fifoDownCount) {
                        --(pCtrl->fifoDownCount);
                    }
                }
            }
#endif
            do {
#if _DAC_MODE
                if(DacMixerRamp_IsDacModeUse()==true) {
                    if(pCtrl->outDataTemp!=_DAC_RAMP_VOLTAGE_MIN) break;
                    if(pCtrl->fifoDownCount>0) break;
                }
#endif
#if _AUDIO_AMP_SD_CTL
                if((SysTick-pCtrl->extAmpStartTime) <= _AUDIO_AMP_SHUTDOWN_TIME) break;
#endif
                pCtrl->ctrlSts = kWaitRampDnChk;
            } while(0);
            break;
        default:
            break;
    }
}

//----------------------------------------------------------------------------//
// dac ramp is Bias
// Parameter: 
//          NONE
// return value:
//          ramp state  :in Bias(true)
//                       still ramping  (false)
//----------------------------------------------------------------------------//
bool DacMixerRamp_IsBias() {
    S_DacRampCtrl *pCtrl = &sDacRampCtrl;
    return pCtrl->ctrlSts==kDacBias;
}

//----------------------------------------------------------------------------//
// dac ramp is Idle
// Parameter: 
//          NONE
// return value:
//          ramp state  :in Idle(true)
//                       still ramping  (false)
//----------------------------------------------------------------------------//
bool DacMixerRamp_IsIdle() {
    S_DacRampCtrl *pCtrl = &sDacRampCtrl;
    return pCtrl->ctrlSts==kDacIdle;
}

//----------------------------------------------------------------------------//
// ServiceLoop(Main)
// Parameter: 
//          NONE
// return value:
//          NONE
//----------------------------------------------------------------------------//
void DacMixerRamp_ServiceLoop(void) {
    GIE_DISABLE();
    DacMixerRamp_SyncStsInPolling();
    GIE_ENABLE();
}

//----------------------------------------------------------------------------//
// Menual set Ramp up process
// Parameter: 
//          NONE
// return value:
//          NONE
//----------------------------------------------------------------------------//
void DacMixerRamp_SetRampUp(void) {
    if(!DacAssigner_isEmpty()) return;
    InitDac(pHwCtl[_DAC_RAMP_DEFAULT_CH].p_dac
            ,_DAC_RAMP_DEFAULT_SR
            ,_DAC_RAMP_DEFAULT_ITP);//DacMixerRampUp in here
    DAC_Cmd(pHwCtl[_DAC_RAMP_DEFAULT_CH].p_dac,ENABLE);
}

//----------------------------------------------------------------------------//
// Menual set Ramp Down process
// Parameter: 
//          NONE
// return value:
//          NONE
//----------------------------------------------------------------------------//
void DacMixerRamp_SetRampDown(void) {
    if(!DacAssigner_isEmpty()) return;
    GIE_DISABLE();
    DacMixerRampDn();
    GIE_ENABLE();
}
#endif //#if _IS_APPS_USE_DAC