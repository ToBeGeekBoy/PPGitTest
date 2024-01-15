/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "include.h"
#include "nx1_dac.h"
#include "nx1_adc.h"
#include "nx1_spi.h"
#include "nx1_smu.h"
#include "nx1_intc.h"
#include "nx1_timer.h"
#include "nx1_wdt.h"
#include "debug.h"
#include "nx1_gpio.h"
#include "nx1_lib.h"
#include "VoiceTab.h"

//////////////////////////////////////////////////////////////////////
//  Defines
//////////////////////////////////////////////////////////////////////
#if _DAC_CH0_TIMER==USE_TIMER1
#define DAC_CH0_TIMER               P_TIMER1
#define DAC_CH0_TRIGER              DAC_TIMER1_TRIG
#elif _DAC_CH0_TIMER==USE_TIMER2
#define DAC_CH0_TIMER               P_TIMER2
#define DAC_CH0_TRIGER              DAC_TIMER2_TRIG
#else
#define DAC_CH0_TIMER               P_TIMER0
#define DAC_CH0_TRIGER              DAC_TIMER0_TRIG
#endif

#if _DAC_CH1_TIMER==USE_TIMER0
#define DAC_CH1_TIMER               P_TIMER0
#define DAC_CH1_TRIGER              DAC_TIMER0_TRIG
#elif _DAC_CH1_TIMER==USE_TIMER2
#define DAC_CH1_TIMER               P_TIMER2
#define DAC_CH1_TRIGER              DAC_TIMER2_TRIG
#else
#define DAC_CH1_TIMER               P_TIMER1
#define DAC_CH1_TRIGER              DAC_TIMER1_TRIG
#endif

#if _ADC_TIMER==USE_TIMER1
#define ADC_TIMER                   P_TIMER1
#define ADC_TRIGER                  ADC_TIMER1_TRIG
#elif _ADC_TIMER==USE_TIMER2
#define ADC_TIMER                   P_TIMER2
#define ADC_TRIGER                  ADC_TIMER2_TRIG
#else
#define ADC_TIMER                   P_TIMER0
#define ADC_TRIGER                  ADC_TIMER0_TRIG
#endif

P_DAC_TIMER pHwCtl[] = {
	{DAC_CH0	,DAC_CH0_TIMER},
	{DAC_CH1	,DAC_CH1_TIMER},
};

//////////////////////////////////////////////////////////////////////
//  Variables
//////////////////////////////////////////////////////////////////////
static U8 DACUpsample, DACUpsampleSetCheck;
U8 MixerVol;
static U8 ChInfo[_AUDIO_API_CH_NUM];
static S_AUDIO_FUNC_IDX *ChFunc[_AUDIO_API_CH_NUM];
static S_AUDIO_PARAMETER ChPara[_AUDIO_API_CH_NUM];

#if _AUDIO_MULTI_CHANNEL
U8 MixingChanNum[2];
#endif
#if _SPI_MODULE || (_SPI1_MODULE && _SPI1_USE_FLASH)
static U8 MarkNumber;
#endif
#if _AUDIO_SENTENCE
static U32 SentenceAddrTemp;
#if _ACTION_MODULE
U32 SentenceAddr[_AUDIO_API_CH_NUM+_VIO_CH_NUM];
static U16 SentenceOrder[_AUDIO_API_CH_NUM+_VIO_CH_NUM];
U32 SentenceStatusMuteTimer[_AUDIO_API_CH_NUM+_VIO_CH_NUM];
U8 SentenceCB[_AUDIO_API_CH_NUM+_VIO_CH_NUM];
static U8 SentenceCBorder[_AUDIO_API_CH_NUM+_VIO_CH_NUM];
static U8 SentenceActionIO[_VIO_CH_NUM];
static U8 SentenceActionBusy[_VIO_CH_NUM];
#else
U32 SentenceAddr[_AUDIO_API_CH_NUM];
static U16 SentenceOrder[_AUDIO_API_CH_NUM];
U32 SentenceStatusMuteTimer[_AUDIO_API_CH_NUM];
U8 SentenceCB[_AUDIO_API_CH_NUM];
static U8 SentenceCBorder[_AUDIO_API_CH_NUM];
#endif
#endif
extern void CB_SPI_MARK(U8 ch, U8 markNum, U8 index);
extern void CB_AUDIO_SetChVolume(U8 ch,U8 vol);
const U32 PlayList[]= 
{
	(U32) 0,
};

// #if _SBC_ECHO_SOUND || _SBC_CH2_ECHO_SOUND || _RT_ECHO_SOUND || _PCM_ECHO_SOUND || _PCM_CH2_ECHO_SOUND || _PCM_CH3_ECHO_SOUND
// const U16 EchoNewDelay[3] = {640,800,1280}; //
// #elif   _ADPCM_ECHO_SOUND || _ADPCM_CH2_ECHO_SOUND 
// #endif

const S16 _AllPassFilterParaTab[12]={ 4096, -197, 61, 61, -197, 4096, 4096, 385, 608, 608, 385, 4096, }; //Allpass

extern void CB_InitAdc(void);
#if _RT_PLAY
extern U8 RT_PlayFlag;
#endif

#pragma weak PlayList
//------------------------------------------------------------------//
// Set PP gain
// Parameter: 
//          gain:dac gain(0~15)
// return value:
//          NONE
//------------------------------------------------------------------//    
void SetPPGain(U8 gain)
{
	DAC_SetPPIpGain(gain);
} 
//------------------------------------------------------------------//
// Enable or DISABLE Noise Filter 
// NOTICE: This function can't work when playing any voice. 
// Parameter: 
//          chx: DAC_CH0, DAC_CH1
//          cmd: ENABLE, DISABLE, other values(Noise Filter is set by SPI_Encoder or NYIDE Resource Manager)
// return value:
//          NONE
//------------------------------------------------------------------//    
void SetNoiseFilter(CH_TypeDef *chx,U8 cmd)
{
	U8 ch_shift;
    if(chx==DAC_CH0){
		ch_shift = 0;
    }else if(chx==DAC_CH1){
		ch_shift = 1;
    }else{
		return;
	}	
	
	DACUpsampleSetCheck |= (1<<ch_shift);
	if(cmd == ENABLE){
		DACUpsample |= (1<<ch_shift);
	}else if(cmd == DISABLE){
		DACUpsample &= ~(1<<ch_shift);
	}else{
		DACUpsampleSetCheck &= ~(1<<ch_shift);
		DACUpsample &= ~(1<<ch_shift);
	}	
}
//------------------------------------------------------------------//
// Enable or DISABLE upsample 
// Parameter: 
//          cmd:ENABLE,DISABLE,0xFF(upsample set by SPI_Encoder Noise Filter)
// return value:
//          NONE
//------------------------------------------------------------------//    
void SetUpsample(U8 cmd)
{
    SetNoiseFilter(DAC_CH0, cmd);
    SetNoiseFilter(DAC_CH1, cmd);
}

//------------------------------------------------------------------//
// Get upsample status
// Parameter: 
//          NONE
// return value:
//          DACUpsample
//------------------------------------------------------------------//    
U8 GetUpsampleStatus(void)
{
    return DACUpsample;
}

//------------------------------------------------------------------//
// Change sample rate
// Parameter: 
//          chx         : DAC_CH0, DAC_CH1
//          sample_rate : sample rate
// return value:
//          NONE
//------------------------------------------------------------------//    
void SetSampleRate(CH_TypeDef *chx,U16 sample_rate)
{
#if _API_ARGUMENT_PROTECT
    if(!sample_rate) {return;}
#endif
    
    U8 hw_ch;
    U16 divisor;
    TIMER_TypeDef *timerx;
    
    if(chx==DAC_CH0) {
        hw_ch=0;
        timerx=DAC_CH0_TIMER;
    } else if(chx==DAC_CH1) {
        hw_ch=1;
        timerx=DAC_CH1_TIMER;
    } else {
        return;
    }
    
    if(DACUpsample&(1<<hw_ch)) {
    #if _EF_SERIES
        #if _RT_PLAY
            if (RT_PlayFlag) 
            {
                divisor=((((OPTION_HIGH_FREQ_CLK+(sample_rate>>1))/sample_rate)+(64/2))/64)*64/_DAC_UP_SAMPLE_POINT-1;
            } 
            else
        #endif 
            {
                divisor=((OPTION_HIGH_FREQ_CLK+(sample_rate*(_DAC_UP_SAMPLE_POINT/2)))/(sample_rate*_DAC_UP_SAMPLE_POINT))-1;
            }
    #else
        divisor=((OPTION_HIGH_FREQ_CLK+(sample_rate*2))/(sample_rate*4))-1;
    #endif // #if _EF_SERIES
    } else {
        divisor=((OPTION_HIGH_FREQ_CLK+(sample_rate>>1))/sample_rate)-1;
    }
    timerx->Data=divisor;
    
#if _DAC_MODE
    if(DacMixerRamp_IsDacModeUse()==true) {
        GIE_DISABLE();
        if((DacMixerRamp_GetRampCh()==hw_ch && !DacMixerRamp_IsBias())) {
            DacMixerRamp_BorrowTimer(hw_ch);
        }
        GIE_ENABLE();
    }
#endif

}     
//------------------------------------------------------------------//
// Init Dac 
// Parameter: 
//          chx: DAC_CH0, DAC_CH1
//          sample_rate:sample rate
//          upsample: 1:enable , 0:disable
// return value:
//          NONE
//------------------------------------------------------------------//   
void InitDac(CH_TypeDef *chx,U32 sample_rate,U8 upsample)
{
#if _API_ARGUMENT_PROTECT
    if(!sample_rate) {return;}
#endif
    U8 i;
    U16 divisor;

#if _VR_FUNC
    VR_Halt();
#endif
    
	SMU_PeriphClkCmd(SMU_PERIPH_DAC,ENABLE);
    S8 hw_ch;
    U32 trig_source;
    U32 intc_dac;
    TIMER_TypeDef *timerx;
    
    if(chx==DAC_CH0) {
        hw_ch=HW_CH0;
        trig_source=DAC_CH0_TRIGER;
        intc_dac=IRQ_BIT_DAC_CH0;
        timerx=DAC_CH0_TIMER;
    } else if(chx==DAC_CH1) {
        hw_ch=HW_CH1;
        trig_source=DAC_CH1_TRIGER;
        intc_dac=IRQ_BIT_DAC_CH1;
        timerx=DAC_CH1_TIMER;
    } else {
        return;
    }
    
    
    if((DACUpsampleSetCheck&(1<<hw_ch))){
		upsample = (DACUpsample>>hw_ch)&0x1;
	}
    
    if(upsample) {
#if _EF_SERIES
    #if _RT_PLAY
	    if (RT_PlayFlag) 
	    {
            divisor=((((OPTION_HIGH_FREQ_CLK+(sample_rate>>1))/sample_rate)+(64/2))/64)*64/_DAC_UP_SAMPLE_POINT-1;
	    } 
        else
    #endif 
        {
		    divisor=((OPTION_HIGH_FREQ_CLK+(sample_rate*(_DAC_UP_SAMPLE_POINT/2)))/(sample_rate*_DAC_UP_SAMPLE_POINT))-1;
        }
#else
	    divisor=((OPTION_HIGH_FREQ_CLK+(sample_rate*2))/(sample_rate*4))-1;
#endif // #if _EF_SERIES
        DACUpsample |= (1<<hw_ch);
	} else {
		divisor=((OPTION_HIGH_FREQ_CLK+(sample_rate>>1))/sample_rate)-1;
        DACUpsample &= ~(1<<hw_ch);
	}
    
    TIMER_Cmd(timerx,DISABLE);//Stop Timer before change divisor setting
#if _EF_SERIES
    TIMER_Init(timerx,divisor);
#else
    TIMER_Init(timerx,TIMER_CLKDIV_1,divisor);
#endif
	
    GIE_DISABLE();
    U16 _firstData=0;
    DacMixerRampUp(hw_ch);
#if _DAC_MODE
    if(DacMixerRamp_IsDacModeUse()==true) {
        _firstData = DacMixerRamp_GetData(hw_ch);
        if((DacMixerRamp_GetRampCh()==hw_ch && !DacMixerRamp_IsBias())) {
            DacMixerRamp_BorrowTimer(hw_ch);
        }
    }
#endif
    GIE_ENABLE();
    
    if( !(chx->CH_Ctrl & C_DAC_Trig_Timer_En)) {
        DAC_Init(chx,trig_source,DAC_FIFO_THD);
    }

    GIE_DISABLE();
    do {
        if((((chx->CH_Ctrl)>>3)&0x01)==upsample) break;
#if _DAC_MODE
        if( DacMixerRamp_IsDacModeUse()==true
         && DacMixerRamp_GetRampCh()==hw_ch
         && !DacMixerRamp_IsBias()              ) break;
#endif
        DAC_SetUpSample(chx,upsample);
    } while(0);
    GIE_ENABLE();
    
    TIMER_Cmd(timerx,ENABLE);//to trigger DAC fifo
    
    do {
#if _DAC_MODE
        if(DacMixerRamp_IsDacModeUse()==true) break;
#endif
#if _EF_SERIES
        DAC_PWMCmd(ENABLE);
#else
        AUDIO_SetPPGain(AUDIO_GetPPGain());
#endif 
    } while(0);

    if(!(chx->CH_Ctrl&C_DAC_Trig_Timer_En)) {
        for(i=0;i<DAC_FIFO_LEN;i++) {
            chx->CH_Data=_firstData;
        }
    }
    INTC_IrqCmd(intc_dac,ENABLE);
}    
#if _IS_APPS_USE_ADC_MIC
//------------------------------------------------------------------//
// Init Adc Flow 
// Parameter: 
//          sample_rate:sample rate
// return value:
//          NONE
//------------------------------------------------------------------//   
void InitAdc(U16 sample_rate)
{
    U16 divisor;
    
	SMU_PeriphClkCmd(SMU_PERIPH_ADC,ENABLE);
    SMU_PeriphReset(SMU_PERIPH_ADC);
    
#if _EF_SERIES
    divisor=((((OPTION_HIGH_FREQ_CLK+(sample_rate>>1))/sample_rate)+(64/2))/64)*64-1;
#else
#if _RT_PLAY //Timer divisor sync with ITP setting.
	if (RT_PlayFlag)
	{
        divisor=(((OPTION_HIGH_FREQ_CLK+(sample_rate*2))/(sample_rate*4))*4)-1;
    } 
    else
#endif
    {
        divisor=((OPTION_HIGH_FREQ_CLK+(sample_rate>>1))/sample_rate)-1;
    }
#endif
      
#if _EF_SERIES
    TIMER_Init(ADC_TIMER,divisor);
#else
    TIMER_Init(ADC_TIMER,TIMER_CLKDIV_1,divisor);
#endif
    TIMER_Cmd(ADC_TIMER,ENABLE);//to trigger ADC fifo  
#if _SL_MODULE   
	if(SL_GetStatus()==SL_INIT)
	{
		_SL_ADC_Init(ADC_TRIGER,_SL_ADC_FIFO_Level);
	}	
	else
   	{
    	ADC_Init(ADC_TRIGER,ADC_CH_MIC,ADC_FIFO_THD);
	}
#else
		ADC_Init(ADC_TRIGER,ADC_CH_MIC,ADC_FIFO_THD);
#endif

#if _EF_SERIES	
	SMU_ALDOCmd(ENABLE);
	GPIOC->MFP = 0xFF;
#endif    
    ADC_MicBiasCmd(ENABLE);

    //Delayms(200);
    //ADC_SetScanMode(ADC_SCAN_2);
    AdcEnFlag=1;
    if(sample_rate==16000)
    {
#if _AUDIO_IDLE_INTO_HALT
    	AdcEnCnt=1600*2;
#else
		AdcEnCnt=1600;
#endif
    }
    else if(sample_rate==12000)
    {
#if _AUDIO_IDLE_INTO_HALT
    	AdcEnCnt=1200*2;
#else
		AdcEnCnt=1200;
#endif
    }
    else if(sample_rate==10000)
    {
#if _AUDIO_IDLE_INTO_HALT
    	AdcEnCnt=1000*2;
#else
		AdcEnCnt=1000;
#endif
    }
    else
    {
#if _AUDIO_IDLE_INTO_HALT
    	AdcEnCnt=800*2;
#else
		AdcEnCnt=800;
#endif
    }
	
	CB_InitAdc();
    INTC_IrqCmd(IRQ_BIT_ADC,ENABLE);
}
#endif
//------------------------------------------------------------------//
// Audio Control Init
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void AudioCtlInit(void){

    MixerVol=CH_VOL_15;
#if _AUDIO_MULTI_CHANNEL
    MixingChanNum[HW_CH0] = MixingChanNum[HW_CH1] = 1;
#endif
}  
//------------------------------------------------------------------//
// Get current status, only called by C_Module firmware code
// Parameter: 
//          ch:audio channel
// return value:
//          status:STATUS_PLAYING,STATUS_PAUSE,STATUS_STOP
//------------------------------------------------------------------// 
static U8 AUDIO_GetStatus_fw(U8 ch){
#if _API_ARGUMENT_PROTECT
    if(ch >= _AUDIO_API_CH_NUM) return STATUS_STOP;
#endif
    return (ChFunc[ch]!=NULL)?(ChFunc[ch]->GetStatus()):(STATUS_STOP);
}
//------------------------------------------------------------------//
// Get current status
// Parameter: 
//          ch:audio channel or action channel(When _AUDIO_SENTENCE and _ACTION_MODULE are ENABLE)
// return value:
//          status:STATUS_PLAYING,STATUS_PAUSE,STATUS_STOP
//------------------------------------------------------------------// 
U8 AUDIO_GetStatus(U8 ch){

    U8 sts;
#if _AUDIO_SENTENCE && _ACTION_MODULE
	if(ch >= _AUDIO_API_CH_NUM) 
	{
		sts = ACTION_GetIOStatus(ch);
	} 
	else
#endif	
	{
		sts = AUDIO_GetStatus_fw(ch);
	}
	
#if _AUDIO_SENTENCE
	if(SentenceAddr[ch]) {
		if((sts == STATUS_PAUSE) || ((SentenceStatusMuteTimer[ch]>>28) == STATUS_PAUSE))
			sts = STATUS_PAUSE;
		else 
			sts = STATUS_PLAYING;
	}
#endif
    return sts;
}
//------------------------------------------------------------------//
// Get current all status 
// Parameter: 
//          NONE
// return value:
//          status:STATUS_PLAYING,STATUS_PAUSE,STATUS_STOP
//------------------------------------------------------------------// 
U8 AUDIO_GetStatus_All(void){
	U8 sts;
    sts = STATUS_STOP;

#if _ADPCM_PLAY
	sts |= ADPCM_GetStatus();
#endif

#if _ADPCM_CH2_PLAY
	sts |= ADPCM_CH2_GetStatus();
#endif

#if _ADPCM_CH3_PLAY
	sts |= ADPCM_CH3_GetStatus();
#endif

#if _ADPCM_CH4_PLAY
	sts |= ADPCM_CH4_GetStatus();
#endif

#if _ADPCM_CH5_PLAY
	sts |= ADPCM_CH5_GetStatus();
#endif

#if _ADPCM_CH6_PLAY
	sts |= ADPCM_CH6_GetStatus();
#endif

#if _ADPCM_CH7_PLAY
	sts |= ADPCM_CH7_GetStatus();
#endif

#if _ADPCM_CH8_PLAY
	sts |= ADPCM_CH8_GetStatus();
#endif

#if _SBC_PLAY
    sts |= SBC_GetStatus();
#endif

#if _SBC_CH2_PLAY
    sts |= SBC_CH2_GetStatus();
#endif

#if _SBC2_PLAY
    sts |= SBC2_GetStatus();
#endif

#if _SBC2_CH2_PLAY
    sts |= SBC2_CH2_GetStatus();
#endif

#if _CELP_PLAY
    sts |= CELP_GetStatus();
#endif

#if _MIDI_PLAY
	sts |= MIDI_GetStatus_All();
#endif

#if _PCM_PLAY
    sts |= PCM_GetStatus();
#endif

#if _PCM_CH2_PLAY
	sts |= PCM_CH2_GetStatus();
#endif

#if _PCM_CH3_PLAY
	sts |= PCM_CH3_GetStatus();
#endif

#if _RT_PLAY
	sts |= RT_GetStatus();
#endif
	
#if _WAVE_ID_MODULE
	sts |= WaveID_GetStatus();
#endif

    return sts;
}
#if _AUDIO_VOLUME_CHANGE_SMOOTH
//------------------------------------------------------------------//
// Init smooth volume
// Parameter: 
//          pVolCtrl :  the pointer of vol control
//          sr       :  sample rate
//          total_vol:  initial volume
// return value:
//          NONE
//------------------------------------------------------------------// 
void AUDIO_InitVolumeSmooth(volatile VOLCTRL_T *pVolCtrl, U16 sr, U16 total_value)
{
    U32 divisor;
    S16 step;

    divisor = sr * _AUDIO_VOLUME_SMOOTH_TIME/1000;
    step = 82; //base on 8k sample rate
    
    if(divisor != 0)
    {
        step = 32768 / divisor;
    }
    
    pVolCtrl->step = step;
    pVolCtrl->current_value = pVolCtrl->target_value = total_value;
}
//------------------------------------------------------------------//
// Set the target volume of smooth volume
// Parameter: 
//          pVolCtrl :  the pointer of vol control
//          totalvol :  0~255
// return value:
//          NONE
//------------------------------------------------------------------// 
void AUDIO_SetVolumeSmooth(volatile VOLCTRL_T *pVolCtrl, U16 total_vol)
{
    pVolCtrl->target_value = (S16)total_vol;

    //dprintf("target %d step %d cur %d\r\n",pVolCtrl->target_value,pVolCtrl->step,pVolCtrl->current_value);
}
//------------------------------------------------------------------//
// Get instant smooth volume
// Parameter: 
//          pVolCtrl :  the pointer of vol control
// return value:
//          return vol
//------------------------------------------------------------------// 
S16 AUDIO_GetVolumeSmooth(volatile VOLCTRL_T *pVolCtrl)
{
    S16 step;
    S16 target_value;
    S16 current_value;
    S16 vol;

    step            = pVolCtrl->step;
    target_value    = pVolCtrl->target_value;
    current_value   = pVolCtrl->current_value;


    if(current_value == target_value)
    {
        return target_value;
    }
    else if(current_value < target_value)
    {
        current_value += step;
        if(current_value > target_value)
        {
            current_value = target_value;
        }
    }
    else if(current_value > target_value)
    {
        current_value -= step;
        if(current_value < target_value)
        {
            current_value = target_value;
        }
    }
    vol = current_value;

    pVolCtrl->current_value = current_value;
    //dprintf("target %d step %d cur %d\r\n",target_value,step,current_value);

    return vol;
}
//------------------------------------------------------------------//
// Get instant smooth volume in interrupt service loop.
// Parameter: 
//          pVolCtrl :  the pointer of vol control
// return value:
//          return vol
//------------------------------------------------------------------// 
S16 AUDIO_GetVolumeSmoothINT(volatile VOLCTRL_T *pVolCtrl)
{
    S16 step;
    S16 target_value;
    S16 current_value;
    S16 vol;

    step            = pVolCtrl->step;
    target_value    = pVolCtrl->target_value;
    current_value   = pVolCtrl->current_value;


    if(current_value < target_value)
    {
        current_value += step;
        if(current_value > target_value)
        {
            current_value = target_value;
        }
    }
    else if(current_value > target_value)
    {
        current_value -= step;
        if(current_value < target_value)
        {
            current_value = target_value;
        }
    }
    vol = current_value;

    pVolCtrl->current_value = current_value;
    //dprintf("target %d step %d cur %d\r\n",target_value,step,current_value);

    return vol;
}
#endif
//------------------------------------------------------------------//
// Set audio channel volume
// Parameter: 
//          ch:audio channel
//          vol:0~15
// return value:
//          NONE
//------------------------------------------------------------------// 
void AUDIO_SetChVolume(U8 ch,U8 vol){ 
#if _API_ARGUMENT_PROTECT
    if(ch >= _AUDIO_API_CH_NUM) return;
#endif
    ChPara[ch].Volume = ((vol > CH_VOL_15)?(CH_VOL_15):(vol));//storage vol

    if(ChFunc[ch]!=NULL) ChFunc[ch]->SetVolume(ChPara[ch].Volume);
}
//------------------------------------------------------------------//
// Set audio mixer volume
// Parameter: 
//          vol:0~15
// return value:
//          NONE
//------------------------------------------------------------------// 
void AUDIO_SetMixerVolume(U8 vol){ 
    S8 _vol = vol;
    _vol = (_vol > CH_VOL_15)?CH_VOL_15:
           (_vol < CH_VOL_0)?CH_VOL_0:
           _vol;
    MixerVol = _vol;
}
//------------------------------------------------------------------//
// Get audio mixer volume
// Parameter: 
//          NONE
// return value:
//          vol:0~15
//------------------------------------------------------------------// 
U8 AUDIO_GetMixerVolume(void){ 
    return MixerVol;
}
//------------------------------------------------------------------//
// Set PP Gain
// Parameter: 
//          NONE
// return value:
//          vol:0(DISABLE)~16
//------------------------------------------------------------------// 
#if !_EF_SERIES
static U8 u8PpGain = _PP_GAIN+1;//default
#endif
void AUDIO_SetPPGain(U8 gain){ 
#if !_EF_SERIES
    S8 _gain = gain;
    _gain =(_gain > 16)?16:
           (_gain < 0)?0:
           _gain;
    u8PpGain = _gain;
    
#if _DAC_MODE
    if(DacMixerRamp_IsDacModeUse()==true) return;
#endif
    
    if(_gain==0) {
        DAC_PPCmd(DISABLE);
    } else {
        DAC_PPCmd(ENABLE);
        DAC_SetPPIpGain(_gain-1);//remap to 0~15
    }
#endif
}
//------------------------------------------------------------------//
// Set PP Gain
// Parameter: 
//          NONE
// return value:
//          vol:0(DISABLE)~16
//------------------------------------------------------------------// 
U8 AUDIO_GetPPGain(void){ 
#if !_EF_SERIES
    return u8PpGain;
#else
    return 0;
#endif
}
//------------------------------------------------------------------//
// Pause playing
// Parameter: 
//          ch:audio channel or action channel(When _AUDIO_SENTENCE and _ACTION_MODULE are ENABLE)
// return value:
//          NONE
//------------------------------------------------------------------// 
void AUDIO_Pause(U8 ch){
#if _API_ARGUMENT_PROTECT
#if _AUDIO_SENTENCE && _ACTION_MODULE
    if(ch >= _AUDIO_API_CH_NUM+_VIO_CH_NUM) return;
#else
    if(ch >= _AUDIO_API_CH_NUM) return;
#endif
#endif
    if(ch < _AUDIO_API_CH_NUM){
        if(ChFunc[ch]!=NULL) ChFunc[ch]->Pause();
    } else {
#if _AUDIO_SENTENCE && _ACTION_MODULE
        ACTION_PauseIO(ch);		
#endif
    }
#if _AUDIO_SENTENCE
	if((SentenceStatusMuteTimer[ch]>>28) == STATUS_MUTE) {	// mute to pause
		SentenceStatusMuteTimer[ch] &= (~SPI_DATA_TYPE_MASK2);
		SentenceStatusMuteTimer[ch] |= (STATUS_PAUSE<<28);
	}
#endif
}
//------------------------------------------------------------------//
// Get file count in SPI
// Parameter: 
//          NONE
// return value:
//          type
//------------------------------------------------------------------//     
U16 AUDIO_GetSpiFileCount(void){
    U8 tempbuf[4];
    U32 addr;
     
    addr=SpiOffset();
               
    SPI_BurstRead(tempbuf,addr,SPI_FILE_COUNT_SIZE);
    
    return(tempbuf[0]|(tempbuf[1]<<8));
}    
#if _SPI1_MODULE && _SPI1_USE_FLASH
//------------------------------------------------------------------//
// Get file count in SPI1
// Parameter: 
//          NONE
// return value:
//          type
//------------------------------------------------------------------//     
U16 AUDIO_GetSpi1FileCount(void){
    U8 tempbuf[4];
    U32 addr;
     
    addr=Spi1_Offset();
               
    SPI1_BurstRead(tempbuf,addr,SPI_FILE_COUNT_SIZE);
    
    return(tempbuf[0]|(tempbuf[1]<<8));
}    
#endif
//------------------------------------------------------------------//
// Get codec type from index
// Parameter: 
//          index: file index or absolute address
//          mode:OTP_MODE,EF_MODE,SPI_MODE,SPI1_MODE
// return value:
//          type
//------------------------------------------------------------------// 
U8 GetCodec(U32 index,U8 mode){
    U32 addr;
    U8 codec=0, offset;
    struct AUDIO_HEADER *header;
    
    header=(struct AUDIO_HEADER *)0;
    offset=(U32)&(header->codec);
    
    switch(mode) {
        case    OTP_MODE:
            if(index&ADDRESS_MODE) {
                addr=index&(~ADDRESS_MODE);
            } else {
                if(index >= VOICE_FILE_NUMBER) {
                    return CODEC_NOT_CODEC; //file index fail
                }
                addr=PlayList[index];
            }
            codec=*((U8 *)(addr+offset));
            break;
#if _EF_SERIES
        case    EF_UD_MODE:
            if(index&ADDRESS_MODE) {
                addr=index&(~ADDRESS_MODE);
            } else {
                addr=UserDefinedData_GetAddressUseType(index,kUD_VOICE,EF_UD_MODE);
                if(!addr) {
                    return CODEC_NOT_CODEC; //file index fail
                }
            }
            codec=*((U8 *)(addr+offset));
            break;
#endif
#if _SPI_MODULE
        case    SPI_MODE:
            if(index&ADDRESS_MODE){
                addr=index&(~ADDRESS_MODE);
                SPI_BurstRead((U8 *)&codec,addr+offset,1);
            }else{
                if(index >= AUDIO_GetSpiFileCount()){	
                    return CODEC_NOT_CODEC;	//file index fail
                }
                U8 tempbuf[SPI_TAB_INFO_ENTRY_SIZE];
                U32 addrBase=SpiOffset(); 
                SPI_BurstRead(tempbuf,addrBase+SPI_BASE_INFO_SIZE+(SPI_TAB_INFO_ENTRY_SIZE*index),SPI_TAB_INFO_ENTRY_SIZE);
                addr=addrBase+(tempbuf[0]|(tempbuf[1]<<8)|(tempbuf[2]<<16)|(tempbuf[3]<<24));
                addr&=(~SPI_DATA_TYPE_MASK);
                if((tempbuf[3]>>4)==SPI_DATA_TYPE_MIDI){
                    codec=CODEC_MIDI;
                }else if((tempbuf[3]>>4)==SPI_DATA_TYPE_SENTENCE){
                #if _AUDIO_SENTENCE
                    SentenceAddrTemp = addr; //sentence location offset, not absolution address
                #endif
                    codec = CODEC_NOT_CODEC;
                }else if((tempbuf[3]>>4)==SPI_DATA_TYPE_MARK){
                    MarkNumber = tempbuf[0]|(tempbuf[1]<<8);
                    codec = CODEC_NOT_CODEC_MARK;
                }else{
                    SPI_BurstRead((U8 *)&codec,addr+offset,1);
                }
            }
            break;
#endif
#if _SPI1_MODULE && _SPI1_USE_FLASH
        case    SPI1_MODE:
            if(index&ADDRESS_MODE){
                addr=index&(~ADDRESS_MODE);
                SPI1_BurstRead((U8 *)&codec,addr+offset,1);
            }else{
                if(index >= AUDIO_GetSpi1FileCount()){
                    return CODEC_NOT_CODEC;	//file index fail
                }
                U8 tempbuf[SPI_TAB_INFO_ENTRY_SIZE];
                U32 addrBase=Spi1_Offset();
                SPI1_BurstRead(tempbuf,addrBase+SPI_BASE_INFO_SIZE+(SPI_TAB_INFO_ENTRY_SIZE*index),SPI_TAB_INFO_ENTRY_SIZE);
                addr=addrBase+(tempbuf[0]|(tempbuf[1]<<8)|(tempbuf[2]<<16)|(tempbuf[3]<<24));
                addr&=(~SPI_DATA_TYPE_MASK);
                if((tempbuf[3]>>4)==SPI_DATA_TYPE_MIDI){
                    codec=CODEC_MIDI;
                }else if((tempbuf[3]>>4)==SPI_DATA_TYPE_SENTENCE){
                #if _AUDIO_SENTENCE
                    SentenceAddrTemp = addr;
                #endif
                    codec = CODEC_NOT_CODEC;
                }else if((tempbuf[3]>>4)==SPI_DATA_TYPE_MARK){
                    MarkNumber = tempbuf[0]|(tempbuf[1]<<8);
                    codec = CODEC_NOT_CODEC_MARK;
                }else{
                    SPI1_BurstRead((U8 *)&codec,addr+offset,1);
                }
            }
            break;
#endif
        default:
            return CODEC_NOT_CODEC; //fail storage mode
    }
    return codec;
}
//------------------------------------------------------------------//
// Start play voice
// Parameter: 
//          ch:audio channel or action channel(When _AUDIO_SENTENCE and _ACTION_MODULE are ENABLE)
//          index: file index or absolute address
//          mode:OTP_MODE,EF_MODE,SPI_MODE,SPI1_MODE
// return value:
//          0:ok
//          others:fail
//------------------------------------------------------------------// 
U8 AUDIO_Play(U8 ch,U32 index,U8 mode){
#if _API_ARGUMENT_PROTECT
#if _AUDIO_SENTENCE && _ACTION_MODULE
    if(ch >= _AUDIO_API_CH_NUM+_VIO_CH_NUM) return E_AUDIO_UNSUPPORT_CODEC;
#else
    if(ch >= _AUDIO_API_CH_NUM) return E_AUDIO_UNSUPPORT_CODEC;
#endif
#endif
    U8 sts, codec;

    if(AUDIO_GetStatus(ch)!=STATUS_STOP){
		AUDIO_Stop(ch);	
        while(AUDIO_GetStatus(ch)==STATUS_PLAYING){
        	AUDIO_ServiceLoop();
        }
    }
#if _AUDIO_SENTENCE && _ACTION_MODULE
	if(ACTION_GetIOStatus(ch)!=STATUS_STOP){
		ACTION_StopIO(ch);
	}
#endif
    
    if(mode&MIDI_MODE)
    {
        mode&=(~MIDI_MODE);
        if(mode==OTP_MODE)
        {
            codec=CODEC_MIDI;
        }else
        {
            codec=GetCodec(index,mode);
        }            
    }else
    {    
        codec=GetCodec(index,mode);
	#if _SPI_MODULE
		if(codec == CODEC_NOT_CODEC_MARK){
			CB_SPI_MARK(ch, MarkNumber, index);
			MarkNumber = 0;
		}
	#endif 
	#if _AUDIO_SENTENCE
		if (SentenceAddrTemp) {
			SentenceAddr[ch] = SentenceAddrTemp;
			SentenceAddrTemp = 0;
			SentenceOrder[ch] = 0;	
			SentenceCBorder[ch] = 0;
		}
	#endif
    }
	
	sts = E_AUDIO_UNSUPPORT_CODEC;
	if ((codec != CODEC_NOT_CODEC) && (codec != CODEC_NOT_CODEC_MARK) && (codec != CODEC_ACTION)){    
		sts = AUDIO_Play_AutoSelect(ch, index, mode, codec);
	}
	
    return sts;
}    
//------------------------------------------------------------------//
// Resume playing
// Parameter: 
//          ch:audio channel or action channel(When _AUDIO_SENTENCE and _ACTION_MODULE are ENABLE)
// return value:
//          NONE
//------------------------------------------------------------------// 
void AUDIO_Resume(U8 ch){
#if _API_ARGUMENT_PROTECT
#if _AUDIO_SENTENCE && _ACTION_MODULE
    if(ch >= _AUDIO_API_CH_NUM+_VIO_CH_NUM) return;
#else
    if(ch >= _AUDIO_API_CH_NUM) return;
#endif
#endif
    if(ch < _AUDIO_API_CH_NUM){
        if(ChFunc[ch]!=NULL) ChFunc[ch]->Resume();
    } else {
#if _AUDIO_SENTENCE && _ACTION_MODULE
        ACTION_ResumeIO(ch);		
#endif
    }
#if _AUDIO_SENTENCE
	if((SentenceStatusMuteTimer[ch]>>28) == STATUS_PAUSE) {	// pause to mute
		SentenceStatusMuteTimer[ch] &= (~SPI_DATA_TYPE_MASK2);
		SentenceStatusMuteTimer[ch] |= (STATUS_MUTE<<28);
	}
#endif
}    
//------------------------------------------------------------------//
// Stop playing
// Parameter: 
//          ch:audio channel or action channel(When _AUDIO_SENTENCE and _ACTION_MODULE are ENABLE)
// return value:
//          NONE
//------------------------------------------------------------------// 
void AUDIO_Stop(U8 ch){
#if _API_ARGUMENT_PROTECT
#if _AUDIO_SENTENCE && _ACTION_MODULE
    if(ch >= _AUDIO_API_CH_NUM+_VIO_CH_NUM) return;
#else
    if(ch >= _AUDIO_API_CH_NUM) return;
#endif
#endif
    if(ch < _AUDIO_API_CH_NUM){
        if(ChFunc[ch]!=NULL) ChFunc[ch]->Stop();
    } else {
#if _AUDIO_SENTENCE && _ACTION_MODULE
        ACTION_StopIO(ch);		
#endif
    }
#if _AUDIO_SENTENCE
    if(SentenceAddr[ch]){
        Sentence_Stop(ch);
    }
#endif
} 
//------------------------------------------------------------------//
// Set EQ Filter
// Parameter: 
//          ch:audio channel
//          index: (0~FILTER_NUMBER-1)User define filter, (other)All Pass
// return value:
//          NONE
//------------------------------------------------------------------// 
void AUDIO_SetEQFilter(U8 ch, U8 index)
{
#if _API_ARGUMENT_PROTECT
    if(ch >= _AUDIO_API_CH_NUM) return;
#endif
    ChPara[ch].EQFilter_Index = index;//storage parameter
    
    if((ChFunc[ch]==NULL) || (ChFunc[ch]->SetEQFilter==NULL)) return;
    ChFunc[ch]->SetEQFilter(ChPara[ch].EQFilter_Index);
}
//------------------------------------------------------------------//
// Set speed ratio
// Parameter: 
//          ch:audio channel
//          index: -12~12
// return value:
//          NONE
//------------------------------------------------------------------// 
void AUDIO_SetSpeed(U8 ch, S8 index)
{
#if _AUDIO_SPEED_CHANGE
#if _API_ARGUMENT_PROTECT
    if(ch >= _AUDIO_API_CH_NUM) return;
#endif
    if(index > SPEED_P12)   index = SPEED_P12;
    if(index < SPEED_N12)   index = SPEED_N12;
    ChPara[ch].Speed_Index = index;//storage index
    
    if((ChFunc[ch]==NULL) || (ChFunc[ch]->SetSpeed==NULL)) return;
    ChFunc[ch]->SetSpeed(ChPara[ch].Speed_Index);
#endif
}    
//------------------------------------------------------------------//
// Set pitch ratio
// Parameter: 
//          ch:audio channel
//          index: -12~12
// return value:
//          NONE
//------------------------------------------------------------------// 
void AUDIO_SetPitch(U8 ch, S8 index)
{
#if _AUDIO_PITCH_CHANGE
#if _API_ARGUMENT_PROTECT
    if(ch >= _AUDIO_API_CH_NUM) return;
#endif
    if(index > PITCH_P12)   index = PITCH_P12;
    if(index < PITCH_N12)   index = PITCH_N12;
    ChPara[ch].Pitch_Index = index;//storage index
    
    if((ChFunc[ch]==NULL) || (ChFunc[ch]->SetPitch==NULL)) return;
    ChFunc[ch]->SetPitch(ChPara[ch].Pitch_Index);
#endif
}    
//------------------------------------------------------------------//
// Enable/Disable Robot Sound Effect
// Parameter: 
//          ch:audio channel
//          cmd: 1:enable,0:disable
//          index: carrier index,0~15
// return value:
//          NONE
//------------------------------------------------------------------// 
void AUDIO_SetRobot1Sound(U8 ch, U8 cmd,U8 index)
{
#if _AUDIO_ROBOT1_SOUND
#if _API_ARGUMENT_PROTECT
    if(ch >= _AUDIO_API_CH_NUM) return;
#endif
    ChPara[ch].Robot1_Cmd = ((cmd)?(ENABLE):(DISABLE));//storage parameter
    ChPara[ch].Robot1_Index = (index>ROBOT_15)?ROBOT_15:index;

    if((ChFunc[ch]==NULL) || (ChFunc[ch]->SetRobot1Sound==NULL)) return;
    ChFunc[ch]->SetRobot1Sound(ChPara[ch].Robot1_Cmd,ChPara[ch].Robot1_Index);
#endif
}    
//------------------------------------------------------------------//
// Enable/Disable Robot01 Sound Effect
// Parameter: 
//          ch: audio channel
//          cmd: 1:enable,0:disable
//          type: robot01 type ,0~2
//          trhes: threshold of input voice ,0~7
//                 0 is the smallest threshold
//                 1 is the largest threshold
// return value:
//          NONE
//------------------------------------------------------------------// 
void AUDIO_SetRobot2Sound(U8 ch, U8 cmd,U8 type, U8 thres){
#if	_AUDIO_ROBOT2_SOUND
#if _API_ARGUMENT_PROTECT
    if(ch >= _AUDIO_API_CH_NUM) return;
#endif
    ChPara[ch].Robot2_Cmd = ((cmd)?(ENABLE):(DISABLE));//storage parameter
    ChPara[ch].Robot2_Type = ((type>2)?(2):(type));
    ChPara[ch].Robot2_Thres = ((thres>7)?(7):(thres));

    if((ChFunc[ch]==NULL) || (ChFunc[ch]->SetRobot2Sound==NULL)) return;
    ChFunc[ch]->SetRobot2Sound(
        ChPara[ch].Robot2_Cmd,
        ChPara[ch].Robot2_Type,
        ChPara[ch].Robot2_Thres);
#endif
} 
//------------------------------------------------------------------//
// Enable/Disable Robot03 Sound Effect
// Parameter: 
//          ch: audio channel
//          cmd: 1:enable,0:disable
//          type: robot01 type ,0~2
//          trhes: threshold of input voice ,0~7
//                 0 is the smallest threshold
//                 1 is the largest threshold
// return value:
//          NONE
//------------------------------------------------------------------// 
void AUDIO_SetRobot3Sound(U8 ch, U8 cmd,U8 type, U8 thres){
#if _AUDIO_ROBOT3_SOUND
#if _API_ARGUMENT_PROTECT
    if(ch >= _AUDIO_API_CH_NUM) return;
#endif
    ChPara[ch].Robot3_Cmd = ((cmd)?(ENABLE):(DISABLE));//storage parameter
    ChPara[ch].Robot3_Type = ((type>2)?(2):(type));
    ChPara[ch].Robot3_Thres = ((thres>7)?(7):(thres));

    if((ChFunc[ch]==NULL) || (ChFunc[ch]->SetRobot3Sound==NULL)) return;
    ChFunc[ch]->SetRobot3Sound(
        ChPara[ch].Robot3_Cmd,
        ChPara[ch].Robot3_Type,
        ChPara[ch].Robot3_Thres);
#endif
} 
//------------------------------------------------------------------//
// Enable/Disable Robot04 Sound Effect
// Parameter: 
//          ch: audio channel
//          cmd: 1:enable,0:disable
// return value:
//          NONE
//------------------------------------------------------------------// 
void AUDIO_SetRobot4Sound(U8 ch, U8 cmd){
#if _AUDIO_ROBOT4_SOUND
#if _API_ARGUMENT_PROTECT
    if(ch >= _AUDIO_API_CH_NUM) return;
#endif
    ChPara[ch].Robot4_Cmd = ((cmd)?(ENABLE):(DISABLE));//storage parameter

    if((ChFunc[ch]==NULL) || (ChFunc[ch]->SetRobot4Sound==NULL)) return;
    ChFunc[ch]->SetRobot4Sound(
        ChPara[ch].Robot4_Cmd);
#endif
} 
//------------------------------------------------------------------//
// Enable/Disable Reverb Sound Effect
// Parameter: 
//          ch: audio channel
//          cmd: 1:enable,0:disable
//          index: Reverb index,0~7
// return value:
//          NONE
//------------------------------------------------------------------// 
void AUDIO_SetReverbSound(U8 ch, U8 cmd,U8 index){
#if _AUDIO_REVERB_SOUND
#if _API_ARGUMENT_PROTECT
    if(ch >= _AUDIO_API_CH_NUM) return;
#endif
    ChPara[ch].Reverb_Cmd = ((cmd)?(ENABLE):(DISABLE));//storage parameter
    ChPara[ch].Reverb_Index = ((index>7)?(7):(index));

    if((ChFunc[ch]==NULL) || (ChFunc[ch]->SetReverbSound==NULL)) return;
    ChFunc[ch]->SetReverbSound(ChPara[ch].Reverb_Cmd,ChPara[ch].Reverb_Index);
#endif
} 
//------------------------------------------------------------------//
// Enable/Disable Echo Sound Effect
// Parameter: 
//          ch: audio channel
//          cmd: 1:enable,0:disable
//          index: Echo index,0~7
// return value:
//          NONE
//------------------------------------------------------------------// 
void AUDIO_SetEchoSound(U8 ch, U8 cmd,U8 index){
#if _AUDIO_ECHO_SOUND
#if _API_ARGUMENT_PROTECT
    if(ch >= _AUDIO_API_CH_NUM) return;
#endif
    ChPara[ch].Echo_Cmd = ((cmd)?(ENABLE):(DISABLE));//storage parameter
    ChPara[ch].Echo_Index = ((index>7)?(7):(index));

    if(ChFunc[ch]==NULL) return;
    if((ChFunc[ch]->SetEchoSound3rdGen!=NULL)) {
        ChFunc[ch]->SetEchoSound3rdGen(ChPara[ch].Echo_Cmd);
    } else if (ChFunc[ch]->SetEchoSound!=NULL) {
        ChFunc[ch]->SetEchoSound(ChPara[ch].Echo_Cmd,ChPara[ch].Echo_Index);
    } else;
#endif
} 
//------------------------------------------------------------------//
// Enable/Disable Darth-Vader Sound Effect
// Parameter: 
//          ch: audio channel
//          Cmd: 1:enable,0:disable
//          PitchIndex: -12 ~ +12
//			CarrierType: 0, 1, 2, 3
// return value:
//          NONE
//------------------------------------------------------------------// 
void AUDIO_SetDarthSound(U8 ch, U8 Cmd, S8 PitchIndex, U8 CarrierType){
#if _AUDIO_DARTH_SOUND
#if _API_ARGUMENT_PROTECT
    if(ch >= _AUDIO_API_CH_NUM) return;
#endif
    ChPara[ch].Darth_Cmd          = ((Cmd)?(ENABLE):(DISABLE));
    if(PitchIndex > PITCH_P12)   PitchIndex = PITCH_P12;
    if(PitchIndex < PITCH_N12)   PitchIndex = PITCH_N12;
    ChPara[ch].Darth_PitchIndex   =  PitchIndex;
    ChPara[ch].Darth_CarrierType  = ((CarrierType>3)?(3):(CarrierType));

    if((ChFunc[ch]==NULL) || (ChFunc[ch]->SetDarthSound==NULL)) return;
    ChFunc[ch]->SetDarthSound(
        ChPara[ch].Darth_Cmd,
        ChPara[ch].Darth_PitchIndex,
        ChPara[ch].Darth_CarrierType);
#endif
} 
//------------------------------------------------------------------//
// Set speedpitchN ratio
// Parameter: 
//          ch: audio channel
//          speedpitch_index: -12 ~ +12
// return value:
//          NONE
//------------------------------------------------------------------// 
void AUDIO_SetSpeedPitchN(U8 ch, S8 speedpitch_index){
#if _AUDIO_SPEEDPITCH_N     
#if _API_ARGUMENT_PROTECT
    if(ch >= _AUDIO_API_CH_NUM) return;
#endif
    if(speedpitch_index > PITCH_P12)   speedpitch_index = PITCH_P12;
    if(speedpitch_index < PITCH_N12)   speedpitch_index = PITCH_N12;
    ChPara[ch].SpeedPitch_Index   =  speedpitch_index;

    if((ChFunc[ch]==NULL) || (ChFunc[ch]->SetSpeedPitchN==NULL)) return;
    ChFunc[ch]->SetSpeedPitchN(
        ChPara[ch].SpeedPitch_Index);
#endif
} 
//------------------------------------------------------------------//
// Set MixCtrl ratio
// Parameter: 
//          ch: audio channel
//          mixCtrl: MIX_CTRL_0 / MIX_CTRL_25 / MIX_CTRL_50 / MIX_CTRL_75 / MIX_CTRL_100
// return value:
//          NONE
//------------------------------------------------------------------// 
void AUDIO_SetMixCtrl(U8 ch, U8 mixCtrl){
#if _AUDIO_MIX_CONTROL
#if _API_ARGUMENT_PROTECT
    if(ch >= _AUDIO_API_CH_NUM) return;
#endif
    if(mixCtrl > MIX_CTRL_100)   mixCtrl = MIX_CTRL_100;
    ChPara[ch].MixCtrl   =  mixCtrl;

    if((ChFunc[ch]==NULL) || (ChFunc[ch]->SetMixCtrl==NULL)) return;
    ChFunc[ch]->SetMixCtrl(ChPara[ch].MixCtrl);
#endif
}
//------------------------------------------------------------------//
// Set Animal Roar effect
// Parameter: 
//          ch: audio channel
//          cmd: ENABLE/DISABLE
//          speed_index: -12~12
//          pitch_index: -12~12
//          reverb_index: 0~3
// return value:
//          NONE
//------------------------------------------------------------------// 
void AUDIO_SetAnimalRoar(U8 ch, U8 cmd, S8 speed_index, S8 pitch_index, U8 reverb_index){
#if _AUDIO_ANIMAL_ROAR
#if _API_ARGUMENT_PROTECT
    if(ch >= _AUDIO_API_CH_NUM) return;
#endif
    ChPara[ch].AnimalRoar_cmd             = ((cmd)?(ENABLE):(DISABLE));
    if(speed_index > SPEED_P12)   speed_index = SPEED_P12;
    if(speed_index < SPEED_N12)   speed_index = SPEED_N12;
    ChPara[ch].AnimalRoar_speed_index     =  speed_index;
    if(pitch_index > PITCH_P12)   pitch_index = PITCH_P12;
    if(pitch_index < PITCH_N12)   pitch_index = PITCH_N12;
    ChPara[ch].AnimalRoar_pitch_index     =  pitch_index;
    ChPara[ch].AnimalRoar_reverb_index    = ((reverb_index>3)?(3):(reverb_index));

    if((ChFunc[ch]==NULL) || (ChFunc[ch]->SetAnimalRoar==NULL)) return;
    ChFunc[ch]->SetAnimalRoar(
        ChPara[ch].AnimalRoar_cmd,
        ChPara[ch].AnimalRoar_speed_index,
        ChPara[ch].AnimalRoar_pitch_index,
        ChPara[ch].AnimalRoar_reverb_index);
#endif
}
//------------------------------------------------------------------//
// Is Record Section Used
// Parameter: 
//          addr:Record Section address (wo/ADDRESS_MODE)
//          mode:EF_MODE / SPI_MODE / SPI1_MODE
// return value:
//          true / false
//------------------------------------------------------------------// 
bool AUDIO_IsRecorded(U32 addr, U8 mode)
{
    if(!addr) return false;
#if (_SPI_MODULE &&_MIDI_PLAY && _MIDI_INST_NOTEON && _MIDI_INST_RECORD)
    if(mode==SPI_MODE) {
        U32 _instRecInfo[3];
        SPI_BurstRead((U8*)_instRecInfo,addr,sizeof(_instRecInfo));
        if( _instRecInfo[2]==0x4D494449) return true;//"MIDI" == 0x4D494449
    }
#endif
#if _SBC_RECORD || _ADPCM_RECORD || _PCM_RECORD || _AUDIO_RECORD
    U8 _codec = GetCodec(ADDRESS_MODE|addr,mode);
    if(_codec==CODEC_SBC||_codec==CODEC_ADPCM||_codec==CODEC_PCM) return true;
#endif
    return false;

}
//------------------------------------------------------------------//
// Codec init
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void AUDIO_Init(void){
    U8 ch;
    for(ch=0;ch<_AUDIO_API_CH_NUM;ch++) {
        ChPara[ch] = (S_AUDIO_PARAMETER){
        	.Volume = CH_VOL_15,
        	.EQFilter_Index = 0xFF,
#if _AUDIO_SPEED_CHANGE
        	.Speed_Index = SPEED_P0,
#endif
#if _AUDIO_PITCH_CHANGE
        	.Pitch_Index = PITCH_P0,
#endif
#if _AUDIO_ROBOT1_SOUND
        	.Robot1_Cmd = DISABLE,
        	.Robot1_Index = ROBOT_0,
#endif
#if	_AUDIO_ROBOT2_SOUND
        	.Robot2_Cmd = DISABLE,
        	.Robot2_Type = 0,
        	.Robot2_Thres = 0,
#endif
#if _AUDIO_ROBOT3_SOUND
        	.Robot3_Cmd = DISABLE,
        	.Robot3_Type = 0,
        	.Robot3_Thres = 0,
#endif
#if _AUDIO_ROBOT4_SOUND
        	.Robot4_Cmd = DISABLE,
#endif
#if _AUDIO_REVERB_SOUND
        	.Reverb_Cmd = DISABLE,
        	.Reverb_Index = 0,
#endif
#if _AUDIO_ECHO_SOUND
        	.Echo_Cmd = DISABLE,
        	.Echo_Index = 0,
#endif
#if	_AUDIO_DARTH_SOUND
            .Darth_Cmd          =   DISABLE,
            .Darth_PitchIndex   =   PITCH_P0,
            .Darth_CarrierType  =   0,
#endif   
#if	_AUDIO_SPEEDPITCH_N
            .SpeedPitch_Index   =   0,
#endif         
#if _AUDIO_MIX_CONTROL
            .MixCtrl            =   MIX_CTRL_100,
#endif
#if _AUDIO_ANIMAL_ROAR
            .AnimalRoar_cmd           =   DISABLE,
            .AnimalRoar_speed_index   =   SPEED_P0,
            .AnimalRoar_pitch_index   =   PITCH_P0,
            .AnimalRoar_reverb_index  =   0,
#endif
        };
    }
#if _ADPCM_PLAY    
    ADPCM_Init();
#endif    
#if _ADPCM_CH2_PLAY
    ADPCM_CH2_Init();     
#endif
#if _ADPCM_CH3_PLAY
    ADPCM_CH3_Init();     
#endif
#if _ADPCM_CH4_PLAY
    ADPCM_CH4_Init();     
#endif
#if _ADPCM_CH5_PLAY
    ADPCM_CH5_Init();     
#endif
#if _ADPCM_CH6_PLAY
    ADPCM_CH6_Init();     
#endif
#if _ADPCM_CH7_PLAY
    ADPCM_CH7_Init();     
#endif
#if _ADPCM_CH8_PLAY
    ADPCM_CH8_Init();     
#endif
#if _SBC_PLAY    
    SBC_Init();
#endif    
#if _SBC_CH2_PLAY
    SBC_CH2_Init();     
#endif
#if _SBC2_PLAY    
    SBC2_Init();
#endif 
#if _SBC2_CH2_PLAY
    SBC2_CH2_Init();     
#endif
#if _CELP_PLAY
    CELP_Init();     
#endif
#if _MIDI_PLAY
    MIDI_Init();     
#endif
#if _PCM_PLAY
    PCM_Init();     
#endif
#if _PCM_CH2_PLAY
    PCM_CH2_Init();     
#endif
#if _PCM_CH3_PLAY
    PCM_CH3_Init();     
#endif
#if _RT_PLAY
    RT_Init();     
#endif
#if _WAVE_ID_MODULE
	WaveID_Init();
#endif
}
//------------------------------------------------------------------//
// Service loop
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
void AUDIO_ServiceLoop(void){
    
#if _MIDI_PLAY    
#if _MIDI_INST_NOTEON && _MIDI_INST_RECORD
	InstNote_RecordPlay_ServiceLoop();	
#endif
	MIDI_ServiceLoop();
#endif

#if _ADPCM_PLAY    
    ADPCM_ServiceLoop();   
#endif
#if _ADPCM_CH2_PLAY
    ADPCM_CH2_ServiceLoop();   
#endif
#if _ADPCM_CH3_PLAY
    ADPCM_CH3_ServiceLoop();   
#endif
#if _ADPCM_CH4_PLAY
    ADPCM_CH4_ServiceLoop();   
#endif
#if _ADPCM_CH5_PLAY
    ADPCM_CH5_ServiceLoop();   
#endif
#if _ADPCM_CH6_PLAY
    ADPCM_CH6_ServiceLoop();   
#endif
#if _ADPCM_CH7_PLAY
    ADPCM_CH7_ServiceLoop();   
#endif
#if _ADPCM_CH8_PLAY
    ADPCM_CH8_ServiceLoop();   
#endif


#if _SBC_PLAY    
    SBC_ServiceLoop();   
#endif
#if _SBC_CH2_PLAY
    SBC_CH2_ServiceLoop();   
#endif
#if _SBC2_PLAY    
    SBC2_ServiceLoop();   
#endif
#if _SBC2_CH2_PLAY
    SBC2_CH2_ServiceLoop();   
#endif
#if _CELP_PLAY    
    CELP_ServiceLoop();   
#endif

#if _PCM_PLAY    
    PCM_ServiceLoop();   
#endif
#if _PCM_CH2_PLAY    
    PCM_CH2_ServiceLoop();   
#endif
#if _PCM_CH3_PLAY    
    PCM_CH3_ServiceLoop();   
#endif
#if _RT_PLAY
    RT_ServiceLoop();   
#endif

#if _AUDIO_RECORD
	AudioRecord_ServiceLoop();	
#endif

#if _IS_APPS_USE_DAC
	DacAssigner_ServiceLoop();
#endif

#if _IS_APPS_USE_DAC
    DacMixerRamp_ServiceLoop();
#endif

}       
//------------------------------------------------------------------//
// Service loop "Here" "NOW" until Non-Playing
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//    
void AUDIO_ServiceLoop_Rightnow(void)
{
	U8 i, sts;	
	do {
		AUDIO_ServiceLoop();
		WDT_Clear();
		sts = STATUS_STOP;
		for(i = 0; i < _AUDIO_API_CH_NUM; i++){
			sts |= AUDIO_GetStatus(i);
		}		
	} while (sts & STATUS_PLAYING);

	U32 tempSysTick = SysTick;
	while (SysTick <= (tempSysTick+5)) {	
		AUDIO_ServiceLoop();
	}	
}

static S_AUDIO_FUNC_IDX * const AUDIO_FUNC_LIST[] = {
    [CH_UNKNOWN]=NULL,
#if _SBC_PLAY
    [CH_SBC]=&s_SBC_FUNC,
#endif
#if _SBC_CH2_PLAY
    [CH_SBC_CH2]=&s_SBC_CH2_FUNC,
#endif
#if _ADPCM_PLAY
    [CH_ADPCM]=&s_ADPCM_FUNC,
#endif
#if _ADPCM_CH2_PLAY
    [CH_ADPCM_CH2]=&s_ADPCM_CH2_FUNC,
#endif
#if _CELP_PLAY
    [CH_CELP]=&s_CELP_FUNC,
#endif
#if _PCM_PLAY
    [CH_PCM]=&s_PCM_FUNC,
#endif
#if _MIDI_PLAY
    [CH_MIDI]=&s_MIDI_FUNC,
#endif
#if _ADPCM_CH3_PLAY
    [CH_ADPCM_CH3]=&s_ADPCM_CH3_FUNC,
#endif
#if _ADPCM_CH4_PLAY
    [CH_ADPCM_CH4]=&s_ADPCM_CH4_FUNC,
#endif
#if _PCM_CH2_PLAY
    [CH_PCM_CH2]=&s_PCM_CH2_FUNC,
#endif
#if _PCM_CH3_PLAY
    [CH_PCM_CH3]=&s_PCM_CH3_FUNC,
#endif
#if _SBC2_PLAY
    [CH_SBC2]=&s_SBC2_FUNC,
#endif
#if _SBC2_CH2_PLAY
    [CH_SBC2_CH2]=&s_SBC2_CH2_FUNC,
#endif
#if _ADPCM_CH5_PLAY
    [CH_ADPCM_CH5]=&s_ADPCM_CH5_FUNC,
#endif
#if _ADPCM_CH6_PLAY
    [CH_ADPCM_CH6]=&s_ADPCM_CH6_FUNC,
#endif
#if _ADPCM_CH7_PLAY
    [CH_ADPCM_CH7]=&s_ADPCM_CH7_FUNC,
#endif
#if _ADPCM_CH8_PLAY
    [CH_ADPCM_CH8]=&s_ADPCM_CH8_FUNC,
#endif
    [num_AUDIO_DECODER_LIST]=NULL,
};
#if _ADPCM_PLAY
#if !defined(__ADPCM_FIXED_MAPPING)
#define __ADPCM_FIXED_MAPPING (_QCODE_BUILD&&( _ADPCM_SPEED_CHANGE   \
                                             ||_ADPCM_PITCH_CHANGE   \
                                             ||_ADPCM_SPEEDPITCH_N   \
                                             ||_ADPCM_ROBOT1_SOUND   \
                                             ||_ADPCM_ROBOT2_SOUND   \
                                             ||_ADPCM_ROBOT3_SOUND   \
                                             ||_ADPCM_ROBOT4_SOUND   \
                                             ||_ADPCM_REVERB_SOUND   \
                                             ||_ADPCM_ECHO_SOUND     \
                                             ||_ADPCM_DARTH_SOUND    \
                                             ||_ADPCM_ANIMAL_ROAR    \
                                             ||_ADPCM_RECORD         ))
// _ADPCM_PLAY_REPEAT?? _ADPCM_SW_UPSAMPLE??
#endif
#if !defined(__ADPCM_FIXED_CH)
#define __ADPCM_FIXED_CH 0
#endif
static const U32 _existADPCM = (
    0
#if _ADPCM_CH8_PLAY
    |(1<<CH_ADPCM_CH8)
#endif
#if _ADPCM_CH7_PLAY
    |(1<<CH_ADPCM_CH7)
#endif
#if _ADPCM_CH6_PLAY
    |(1<<CH_ADPCM_CH6)
#endif
#if _ADPCM_CH5_PLAY
    |(1<<CH_ADPCM_CH5)
#endif
#if _ADPCM_CH4_PLAY
    |(1<<CH_ADPCM_CH4)
#endif
#if _ADPCM_CH3_PLAY
    |(1<<CH_ADPCM_CH3)
#endif
#if _ADPCM_CH2_PLAY
    |(1<<CH_ADPCM_CH2)
#endif
#if !__ADPCM_FIXED_MAPPING
    |(1<<CH_ADPCM)
#endif
);
#endif //#if _ADPCM_PLAY

#if _SBC_PLAY
#if !defined(__SBC_FIXED_MAPPING)
#define __SBC_FIXED_MAPPING (_QCODE_BUILD&&( _SBC_SPEED_CHANGE \
                                           ||_SBC_PITCH_CHANGE \
                                           ||_SBC_ROBOT1_SOUND \
                                           ||_SBC_ROBOT2_SOUND \
                                           ||_SBC_ROBOT3_SOUND \
                                           ||_SBC_REVERB_SOUND \
                                           ||_SBC_ECHO_SOUND   \
                                           ||_SBC_RECORD       ))
// _SBC_AUDIO_BUF_X3??
#endif
#if !defined(__SBC_FIXED_CH)
#define __SBC_FIXED_CH 0
#endif
static const U32 _existSBC = (
    0
#if _SBC_CH2_PLAY
    |(1<<CH_SBC_CH2)
#endif
#if !__SBC_FIXED_MAPPING
    |(1<<CH_SBC)
#endif
);
#endif //#if _SBC_PLAY

#if _SBC2_PLAY
#if !defined(__SBC2_FIXED_MAPPING)
#define __SBC2_FIXED_MAPPING DISABLE
// _SBC2_AUDIO_BUF_X3??
#endif
#if !defined(__SBC2_FIXED_CH)
#define __SBC2_FIXED_CH 0
#endif
static const U32 _existSBC2 = (
    0
#if _SBC2_CH2_PLAY
    |(1<<CH_SBC2_CH2)
#endif
#if !__SBC2_FIXED_MAPPING
    |(1<<CH_SBC2)
#endif
);
#endif //#if _SBC2_PLAY

#if _CELP_PLAY
#if !defined(__CELP_FIXED_MAPPING)
#define __CELP_FIXED_MAPPING (_QCODE_BUILD&&( _CELP_SPEED_CHANGE   \
                                            ||_CELP_PITCH_CHANGE   \
                                            ||_CELP_ROBOT1_SOUND   \
                                            ||_CELP_ROBOT2_SOUND   \
                                            ||_CELP_ROBOT3_SOUND   \
                                            ||_CELP_REVERB_SOUND   \
                                            ||_CELP_ECHO_SOUND     ))
#endif
#if !defined(__CELP_FIXED_CH)
#define __CELP_FIXED_CH 0
#endif
static const U32 _existCELP = (
    0
#if !__CELP_FIXED_MAPPING
    |(1<<CH_CELP)
#endif
);
#endif //#if _CELP_PLAY

#if _PCM_PLAY
#if !defined(__PCM_FIXED_MAPPING)
#define __PCM_FIXED_MAPPING (_QCODE_BUILD&&( _PCM_SPEED_CHANGE \
                                           ||_PCM_PITCH_CHANGE \
                                           ||_PCM_ROBOT1_SOUND \
                                           ||_PCM_ROBOT2_SOUND \
                                           ||_PCM_ROBOT3_SOUND \
                                           ||_PCM_REVERB_SOUND \
                                           ||_PCM_ECHO_SOUND   \
                                           ||_PCM_RECORD       ))
#endif
#if !defined(__PCM_FIXED_CH)
#define __PCM_FIXED_CH 0
#endif
static const U32 _existPCM = (
    0
#if _PCM_CH3_PLAY
    |(1<<CH_PCM_CH3)
#endif
#if _PCM_CH2_PLAY
    |(1<<CH_PCM_CH2)
#endif
#if !__PCM_FIXED_MAPPING
    |(1<<CH_PCM)
#endif
);
#endif //#if _PCM_PLAY

#if _MIDI_PLAY
#if !defined(__MIDI_FIXED_MAPPING)
#define __MIDI_FIXED_MAPPING DISABLE
#endif
#if !defined(__MIDI_FIXED_CH)
#define __MIDI_FIXED_CH 0
#endif
static const U32 _existMIDI = (
    0
#if !__MIDI_FIXED_MAPPING
    |(1<<CH_MIDI)
#endif
);
#endif //#if _MIDI_PLAY

static U32 _getExistDecoder(U8 ch, U8 codec)
{
    U32 _existDecoder;
    switch(codec) {
        default:
            _existDecoder = 0;
            break;
#if _ADPCM_PLAY
        case CODEC_ADPCM:
            _existDecoder = _existADPCM;
#if __ADPCM_FIXED_MAPPING
            _existDecoder=(ch==__ADPCM_FIXED_CH)?(1<<CH_ADPCM):_existDecoder;
#endif
            break;
#endif
#if _SBC_PLAY
        case CODEC_SBC:
        case CODEC_SBC_E:
            _existDecoder = _existSBC;
#if __SBC_FIXED_MAPPING
            _existDecoder=(ch==__SBC_FIXED_CH)?(1<<CH_SBC):_existDecoder;
#endif
            break;
#endif
#if _SBC2_PLAY
        case CODEC_SBC2:
        case CODEC_SBC2_E:
            _existDecoder = _existSBC2;
#if __SBC2_FIXED_MAPPING
            _existDecoder=(ch==__SBC2_FIXED_CH)?(1<<CH_SBC2):_existDecoder;
#endif
            break;
#endif
#if _CELP_PLAY
        case CODEC_CELP:
            _existDecoder = _existCELP;
#if __CELP_FIXED_MAPPING
            _existDecoder=(ch==__CELP_FIXED_CH)?(1<<CH_CELP):_existDecoder;
#endif
            break;
#endif
#if _PCM_PLAY
        case CODEC_PCM:
            _existDecoder = _existPCM;
#if __PCM_FIXED_MAPPING
            _existDecoder=(ch==__PCM_FIXED_CH)?(1<<CH_PCM):_existDecoder;
#endif
            break;
#endif
#if _MIDI_PLAY
        case CODEC_MIDI:
            _existDecoder = _existMIDI;
#if __MIDI_FIXED_MAPPING
            _existDecoder=(ch==__MIDI_FIXED_CH)?(1<<CH_MIDI):_existDecoder;
#endif
            break;
#endif
    }
    return _existDecoder;
}
//------------------------------------------------------------------//
// play voice by selecting automatically algorithm resource
// Parameter: 
//          ch:audio channel
//          index: file index or absolute address
//          mode: OTP_MODE,EF_MODE,SPI_MODE,SPI1_MODE
//			codec: codec type
// return value:
//          0:ok
//          others:fail
//------------------------------------------------------------------// 
U8 AUDIO_Play_AutoSelect(U8 ch, U32 index, U8 mode, U8 codec)
{
#if _API_ARGUMENT_PROTECT
    if(ch >= _AUDIO_API_CH_NUM) return E_AUDIO_UNSUPPORT_CODEC;
#endif
    
    // Step 0, free channel in pause status, release Heap RAM.
    if(AUDIO_GetStatus_fw(ch)==STATUS_PAUSE) {
        AUDIO_Stop(ch);
        ChInfo[ch] = CH_UNKNOWN;
        ChFunc[ch] = NULL;
    }
    
    // Step 1, collect used decoder.
    U32 _usedDecoder = 0;
    U8 i;
    for(i = 0; i < _AUDIO_API_CH_NUM; i++) {
        if(AUDIO_GetStatus_fw(i) == STATUS_STOP) {
            ChInfo[i] = CH_UNKNOWN;
            ChFunc[i] = NULL;
        }
        _usedDecoder |= (1<<ChInfo[i]);
    }
    
    // Step 2, get usable decoder
    U32 _usableDecoder = _getExistDecoder(ch,codec)&(~_usedDecoder);
    if(_usableDecoder==0) return E_AUDIO_UNSUPPORT_CODEC;
    _usableDecoder=Get_LS1B_Position(_usableDecoder);
    
    // Step 3, get function table.
    ChFunc[ch]=AUDIO_FUNC_LIST[_usableDecoder];
    ChInfo[ch]=_usableDecoder;
    
    // Step 4, init SFX parameter.
    S_AUDIO_PARAMETER *_para = &ChPara[ch];
#if _AUDIO_MIX_CONTROL
    AUDIO_SetMixCtrl(ch, _para->MixCtrl);
#endif
    AUDIO_SetChVolume(ch, _para->Volume);
    AUDIO_SetEQFilter(ch, _para->EQFilter_Index);
#if _AUDIO_SPEED_CHANGE
    AUDIO_SetSpeed(ch, _para->Speed_Index);
#endif
#if _AUDIO_PITCH_CHANGE
    AUDIO_SetPitch(ch, _para->Pitch_Index);
#endif
#if _AUDIO_ROBOT1_SOUND
    AUDIO_SetRobot1Sound(ch, _para->Robot1_Cmd,
                             _para->Robot1_Index);
#endif
#if _AUDIO_ROBOT2_SOUND
    AUDIO_SetRobot2Sound(ch, _para->Robot2_Cmd,
                             _para->Robot2_Type,
                             _para->Robot2_Thres);
#endif
#if _AUDIO_ROBOT3_SOUND
    AUDIO_SetRobot3Sound(ch, _para->Robot3_Cmd,
                             _para->Robot3_Type,
                             _para->Robot3_Thres);
#endif
#if _AUDIO_ROBOT4_SOUND
    AUDIO_SetRobot4Sound(ch, _para->Robot4_Cmd);
#endif
#if _AUDIO_REVERB_SOUND
    AUDIO_SetReverbSound(ch, _para->Reverb_Cmd,
                             _para->Reverb_Index);
#endif
#if _AUDIO_ECHO_SOUND
    AUDIO_SetEchoSound(ch, _para->Echo_Cmd,
                           _para->Echo_Index);
#endif
#if _AUDIO_DARTH_SOUND
    AUDIO_SetDarthSound(ch, _para->Darth_Cmd,
                            _para->Darth_PitchIndex,
                            _para->Darth_CarrierType);
#endif 
#if _AUDIO_SPEEDPITCH_N
    AUDIO_SetSpeedPitchN(ch, _para->SpeedPitch_Index);
#endif       
#if _AUDIO_ANIMAL_ROAR
    AUDIO_SetAnimalRoar(ch, _para->AnimalRoar_cmd,
                            _para->AnimalRoar_speed_index,
                            _para->AnimalRoar_pitch_index,
                            _para->AnimalRoar_reverb_index);
#endif
    return ChFunc[ch]->Play(index,mode);
}
#if _AUDIO_SENTENCE
//------------------------------------------------------------------//
// Sentence Service loop
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void Sentence_ServiceLoop(void)
{
	U8 i, ch_GetStatus;
	
	#if _ACTION_MODULE
	for(i = 0; i < (_AUDIO_API_CH_NUM + _VIO_CH_NUM); i++){	
		if (i < _AUDIO_API_CH_NUM) {
			ch_GetStatus = AUDIO_GetStatus_fw(i);
		} else {
			ch_GetStatus = ACTION_GetIOStatus(i);
		}
	#else
	for(i = 0; i < _AUDIO_API_CH_NUM; i++){	
		ch_GetStatus = AUDIO_GetStatus_fw(i);
	#endif	
		if (SentenceAddr[i] && (ch_GetStatus == STATUS_STOP) && ((SentenceStatusMuteTimer[i]&(~SPI_DATA_TYPE_MASK2)) == 0) && !SentenceCB[i]) {	// SPI sentence condition
			if (!(SentenceOrder[i] >> 15)){
				/////
				U8 tempbuf[SPI_TAB_INFO_ENTRY_SIZE];
				U32 addr, spiOffset;
				U8 codec;
			
				spiOffset=SpiOffset();
				
				SPI_BurstRead(tempbuf,SentenceAddr[i]+(SPI_TAB_INFO_ENTRY_SIZE*SentenceOrder[i]),SPI_TAB_INFO_ENTRY_SIZE);
				addr=spiOffset+(tempbuf[0]|(tempbuf[1]<<8)|(tempbuf[2]<<16)|(tempbuf[3]<<24));
				
				addr&=(~SPI_DATA_TYPE_MASK2);
				codec = (tempbuf[3] >> 4) & 0xF;
				/////
		
				SentenceStatusMuteTimer[i] = (STATUS_PLAYING<<28) | 0;	// default is playing
				switch (codec) {
					case 1:
					case 2:
						codec = CODEC_ADPCM;
						break;   
					case 3:
						codec = CODEC_SBC;	// include CODEC_SBC_E
						break;     
					case 4:
						codec = CODEC_MIDI;
						break;
				#if _ACTION_MODULE
					case 5:
						codec = CODEC_ACTION;
						break;   
				#endif
					case 6:
						codec = CODEC_CELP;
						break;   
					case 7:  
						codec = CODEC_PCM;
						break; 
					case 8:
						codec = CODEC_SBC2;	// include CODEC_SBC2_E
						break;
					case 0:
						SentenceStatusMuteTimer[i] = (STATUS_MUTE<<28) | (addr - spiOffset);
					default:
						codec = CODEC_NOT_CODEC;
						break;
					case 9:
						SentenceCB[i] = 1;		
						CB_SPI_MARK(i, (addr-spiOffset)&0xFF, SentenceCBorder[i]);
						SentenceCBorder[i]++;
						
						codec = CODEC_NOT_CODEC;
						break;
				}
				//dprintf("CH= %d, SentenceOrder= %d, Time= %d\r\n", i, SentenceOrder[i], SysTick);
				
				if (codec != CODEC_NOT_CODEC) {
				#if _ACTION_MODULE
					if (codec == CODEC_ACTION) {
						ACTION_PlayIO(i, ADDRESS_MODE|addr, SentenceActionIO[i-_AUDIO_API_CH_NUM], ACTION_NO_REPEAT, SentenceActionBusy[i-_AUDIO_API_CH_NUM], SPI_MODE);
					} else
				#endif 
					{
						AUDIO_Play_AutoSelect(i, ADDRESS_MODE|addr, SPI_MODE, codec);
					}
				}
				SentenceOrder[i]++;
				
				if((tempbuf[3] >> 3) & 0x1){	// EndFlag
					SentenceOrder[i] |= 0x8000;
				}
			} else {							// EndFlag flow
				Sentence_Stop(i);
			}
		}
	}
}
//------------------------------------------------------------------//
// Sentence Stop setting
// Parameter: 
//          ch:audio channel
// return value:
//          NONE
//------------------------------------------------------------------//
void Sentence_Stop(U8 ch)
{
	SentenceAddr[ch] = 0;
	SentenceStatusMuteTimer[ch] = (STATUS_STOP<<28) | 0;
	SentenceCB[ch] = 0;
}
//------------------------------------------------------------------//
// Sentence ACTION_IO preSetting
// Parameter: 
//			ch:vio channel
//          port: assign gpio port, port|pin
//          busy: 0:busy_low,1:busy_high
// return value:
//          NONE
//------------------------------------------------------------------//    
#if _ACTION_MODULE
void Sentence_ACTION_IO_Setting(U8 ch, U8 port, U8 busy)
{
	SentenceActionIO[ch-_AUDIO_API_CH_NUM] = port;
	SentenceActionBusy[ch-_AUDIO_API_CH_NUM] = busy;
}
#endif
#endif
#if _AUDIO_MULTI_CHANNEL
//------------------------------------------------------------------//
// Set the number of mixing channel in HW0
// Parameter: 
//          num: the number of mixing channel
// return value:
//          NONE
//------------------------------------------------------------------//  
void AUDIO_Hw0_SetMixingChanNum(U8 num)
{
    MixingChanNum[HW_CH0] = num;
}
//------------------------------------------------------------------//
// Set the number of mixing channel in HW1
// Parameter: 
//          num: the number of mixing channel
// return value:
//          NONE
//------------------------------------------------------------------//  
void AUDIO_Hw1_SetMixingChanNum(U8 num)
{
    MixingChanNum[HW_CH1] = num;
}
#endif

//------------------------------------------------------------------//
// Get Software channel by codec
// Parameter: 
//          codec: codec type
// return value:
//          return: sw channel number, 0xFF means no channels
//------------------------------------------------------------------//  
U8 AUDIO_GetAudioSwChan(U8 codec)
{
    U8 i;
    for(i=0;i<_AUDIO_API_CH_NUM;i++) {
        if(codec==ChInfo[i]) return i;
    }
    return 0xFF;
}

