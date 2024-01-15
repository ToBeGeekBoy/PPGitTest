#include "include.h"
#include "nx1_smu.h"
#include "nx1_adc.h"

#if !body_none_ADC && _IS_ADC_INPUT

#if _IS_APPS_USE_ADC_MIC
//----------------------------------------------------------------------------//
// is ADC MIC busy?
// Parameter:
//          NONE
// return value:
//          true / false
//----------------------------------------------------------------------------//
static bool _isAdcMicAppBusy(void)
{
#if _VR_FUNC
    if(VRState!=VR_OFF) return true;
#endif
#if _SBC_RECORD
    if(SBC_GetStatus()==STATUS_RECORDING) return true;
#endif
#if _ADPCM_RECORD
    if(ADPCM_GetStatus()==STATUS_RECORDING) return true;
#endif
#if _PCM_RECORD
    if(PCM_GetStatus()==STATUS_RECORDING) return true;
#endif
#if _RT_PLAY
    if(RT_GetStatus()!=STATUS_STOP) return true;
#endif
#if _PD_PLAY
    if(PD_GetStatus()!=STATUS_STOP) return true;
#endif
#if _SOUND_DETECTION
    if(SOUND_DETECTION_GetStatus()!=STATUS_STOP) return true;
#endif
#if _WAVE_ID_MODULE
    if(WaveID_GetStatus()!=STATUS_STOP) return true;
#endif
#if _SL_MODULE
    if(SL_GetStatus()!=SL_OFF) return true;
#endif
    return false;
}

//----------------------------------------------------------------------------//
// Stop all ADC MIC APP
// Parameter:
//          NONE
// return value:
//          NONE
//----------------------------------------------------------------------------//
static void _stopAllAdcMicApp(void)
{
#if _VR_FUNC
    VR_Stop();
#endif
#if _SBC_RECORD
    if(SBC_GetStatus()==STATUS_RECORDING) SBC_Stop();
#endif
#if _ADPCM_RECORD
    if(ADPCM_GetStatus()==STATUS_RECORDING) ADPCM_Stop();
#endif
#if _PCM_RECORD
    if(PCM_GetStatus()==STATUS_RECORDING) PCM_Stop();
#endif
#if _RT_PLAY
    RT_Stop();
#endif
#if _PD_PLAY
    PD_Stop();
#endif
#if _SOUND_DETECTION
    SOUND_DETECTION_Stop();
#endif
#if _WAVE_ID_MODULE
    WaveID_Stop();
#endif
#if _SL_MODULE
    SL_Stop();
#endif
}
#endif

//----------------------------------------------------------------------------//
// is ADC Enable?
// Parameter:
//          NONE
// return value:
//          true / false
//----------------------------------------------------------------------------//
static bool _isAdcEnable(void)
{
    return (P_ADC->Ctrl & C_ADC_En)==C_ADC_En;
}

//----------------------------------------------------------------------------//
// is ADC manual trigger now?
// Parameter:
//          NONE
// return value:
//          true / false
//----------------------------------------------------------------------------//
static bool _isAdcManualTrig(void)
{
    return (P_ADC->Ctrl & C_ADC_Trig)==C_ADC_Trig_Manual;
}

//----------------------------------------------------------------------------//
// Set ADC manual trigger
// Parameter:
//          NONE
// return value:
//          NONE
//----------------------------------------------------------------------------//
static void _initAdcManualTrig(void)
{
    P_ADC->Ctrl = C_ADC_Trig_Manual
                 |C_ADC_CLK_Div_32
                 |C_ADC_En
                 |C_ADC_PreCharge_Dis
                 |C_ADC_Sample_Cycle_8;

    P_ADC->FIFO_Ctrl =(ADC_FIFO_THD<<C_ADC_FIFO_Level_Offset)
                      |C_ADC_FIFO_Reset;
}

//----------------------------------------------------------------------------//
// Init ADC input setting
// Parameter:
//          NONE
// return value:
//          NONE
//----------------------------------------------------------------------------//
static void _initAdcInput(void)
{
#if _EF_SERIES
    SMU_ALDOCmd(ENABLE);
#endif
    SMU_PeriphClkCmd(SMU_PERIPH_ADC,ENABLE);
#if _IS_APPS_USE_ADC_MIC
    if( _isAdcMicAppBusy() ) {
        _stopAllAdcMicApp();
        SMU_PeriphReset(SMU_PERIPH_ADC);
    }
#endif
    if( !_isAdcEnable() ||
        (_isAdcEnable() && !_isAdcManualTrig())
    ) {
        _initAdcManualTrig();
    }
}

//----------------------------------------------------------------------------//
// Get ADC input value
// Parameter:
//          _ch:0~7
// return value:
//          0x0~0xFFF0
//----------------------------------------------------------------------------//
static U16 _getAdcInput(U8 _ch)
{
    U32 _ctrl = P_ADC->Ctrl;
    _ctrl &= ~(C_ADC_CH|C_ADC_CH0_From_MIC);
    P_ADC->Ctrl = _ctrl | _ch;
    ADC_SwTrigStart();
    return ADC_GetValue(_ch);
}
#endif //#if !body_none_ADC && _IS_ADC_INPUT

//----------------------------------------------------------------------------//
// (User API)Get ADC input value
// Parameter:
//          _ch:0~7
// return value:
//          0x0~0xFFF0
//----------------------------------------------------------------------------//
U16 ADC_INPUT_GetValue(U8 _ch)
{
#if !body_none_ADC && _IS_ADC_INPUT
    if(!((0x1<<_ch)&_ADC_INPUT_MASK)) return 0;
    _initAdcInput();
    return _getAdcInput(_ch);
#else
    return 0;
#endif
}
