#include "include.h"
#include "nx1_adc.h"
#include "nx1_dac.h"
#include "nx1_timer.h"
#include "nx1_rtc.h"
#include "nx1_intc.h"
#include "nx1_smu.h"
#include "nx1_gpio.h"
#include "nx1_i2c.h"
#include "nx1_lib.h"
#include "nx1_uart.h"
#include "TouchTab.h"
#include "user_main.h"

//////////////////////////////////////////////////////////////////////
// Define
//////////////////////////////////////////////////////////////////////
// DAC/ADC
#define ADC_FIFO_BURST_SIZE     ADC_FIFO_THD
#define DAC_FIFO_BURST_SIZE     (DAC_FIFO_LEN-DAC_FIFO_THD)
#define MIX_BUF_SIZE            DAC_FIFO_BURST_SIZE
// time base
#if  _SYS_TIMEBASE==TIMEBASE_62us && body_is_4KHZ
#error "IC Body can not support TIMEBASE_62.5us"
#endif

#if  _SYS_TIMEBASE==TIMEBASE_250us && !body_is_4KHZ
#define TIMEBASE_COUNT				4
#else
#define TIMEBASE_COUNT				1
#endif

#if  _SYS_TIMEBASE==TIMEBASE_1000us
#define SYS_TICK_COUNT              1
#elif _SYS_TIMEBASE==TIMEBASE_250us
#define SYS_TICK_COUNT              4
#else//_SYS_TIMEBASE==TIMEBASE_62us
#define SYS_TICK_COUNT              16
#endif
//////////////////////////////////////////////////////////////////////
// Global Variable
//////////////////////////////////////////////////////////////////////
#if _IS_APPS_USE_DAC
S32 DacMixBuf[MIX_BUF_SIZE];
#if _ADPCM_PLAY || _ADPCM_CH2_PLAY || _ADPCM_CH3_PLAY || _ADPCM_CH4_PLAY    \
    || _WAVE_ID_MODULE || _ADPCM_CH5_PLAY || _ADPCM_CH6_PLAY || _ADPCM_CH7_PLAY || _ADPCM_CH8_PLAY
SCOPE_TYPE S16 _dacTempBuf[DAC_FIFO_BURST_SIZE];
#endif
#endif

#if _AUDIO_RECORD
S32    RecordMixBuf[MIX_BUF_SIZE];
#endif

#if  _SYS_TIMEBASE
SCOPE_TYPE U8 SysTickCount;
#if TIMEBASE_COUNT>1
SCOPE_TYPE U8 TimebaseCount;
#endif
volatile U32 SysTick=0;
#endif

#if _TOUCHKEY_MODULE
extern volatile U8	u8Touch1SCount,u8TouchFrameTime;
#endif


#if _EF_SERIES
U16 PWMCurrentFactor;
#endif

#if _IS_APPS_USE_DAC
//------------------------------------------------------------------//
// DAC ISR ServiceLoop
// Parameter:
//          _dacCh		:HW_CH0(0),HW_CH1(1)
// return value:
//          NONE
//------------------------------------------------------------------//
static inline void DAC_ISR_ServiceLoop(const S8 _dacCh){
    //if((_dacCh<0)||(_dacCh>=_ASSIGNABLE_DAC_NUM))return;
    CH_TypeDef *chx = pHwCtl[_dacCh].p_dac;

#if _LEDSTR_MODULE
extern U8 LED_pattern_flag;
	if(LEDSTR_TIMER->Flag == 1)
	{
		LEDSTR_Isr();
		LED_pattern_flag=0;
	}
#endif
#if _LEDSTRn_MODULE
extern U8 LEDn_pattern_flag;
	if(LEDSTRn_TIMER->Flag == 1)
	{
		LEDSTRn_Isr();
		LEDn_pattern_flag=0;
	}
#endif
#if _LEDSTRm_MODULE
extern U8 LEDm_pattern_flag;
	if(LEDSTRm_TIMER->Flag == 1)
	{
		LEDSTRm_Isr();
		LEDm_pattern_flag=0;
	}
#endif
    U8 i;
    for(i=0;i<MIX_BUF_SIZE;i++){
        DacMixBuf[i]=0;
    }

#if _ASSIGNABLE_DAC_NUM>0
	DacAssigner_fifoDataNumberCheck(_dacCh);
	do {
        if(!DacMixerRamp_IsBias()) {
#if _DAC_MODE
            if(DacMixerRamp_IsDacModeUse()==true) {
                for(i=0;i<MIX_BUF_SIZE;i++) {
                    DacMixerRamp_SyncStsInInterrupt(_dacCh);
                    chx->CH_Data = DacMixerRamp_GetData(_dacCh);
                }
                return;//do nothing and return
            }
#endif
            DacMixerRamp_SyncStsInInterrupt(_dacCh);
            break;//do nothing and go ahead
        }

        U32 _registInfo = DacAssigner_registInfo(_dacCh);
#if _WAVE_ID_MODULE
		if(_registInfo & (0x1<<kWID)){
			WaveID_DacIsr(DAC_FIFO_BURST_SIZE,_dacTempBuf);
			break;
		}
#endif
#if _RT_PLAY
		if(_registInfo & (0x1<<kRT)){
			RT_DacIsr(DAC_FIFO_BURST_SIZE);
			break;
		}
#endif
#if _SBC_PLAY
		if(_registInfo & (0x1<<kSBC_CH1)){
			SBC_DacIsr(DAC_FIFO_BURST_SIZE);
		}
#endif
#if _SBC_CH2_PLAY
		if(_registInfo & (0x1<<kSBC_CH2)){
			SBC_CH2_DacIsr(DAC_FIFO_BURST_SIZE);
		}
#endif
#if _SBC2_PLAY
		if(_registInfo & (0x1<<kSBC2_CH1)){
			SBC2_DacIsr(DAC_FIFO_BURST_SIZE);
		}
#endif
#if _SBC2_CH2_PLAY
		if(_registInfo & (0x1<<kSBC2_CH2)){
			SBC2_CH2_DacIsr(DAC_FIFO_BURST_SIZE);
		}
#endif
#if _MIDI_PLAY
		if(_registInfo & (0x1<<kMIDI)){
			MIDI_DacIsr(DAC_FIFO_BURST_SIZE);
		}
#endif
#if _ADPCM_PLAY
		if(_registInfo & (0x1<<kADPCM_CH1)){
			ADPCM_DacIsr(DAC_FIFO_BURST_SIZE,_dacTempBuf);
		}
#endif
#if _ADPCM_CH2_PLAY
		if(_registInfo & (0x1<<kADPCM_CH2)){
			ADPCM_CH2_DacIsr(DAC_FIFO_BURST_SIZE,_dacTempBuf);
		}
#endif
#if _ADPCM_CH3_PLAY
		if(_registInfo & (0x1<<kADPCM_CH3)){
			ADPCM_CH3_DacIsr(DAC_FIFO_BURST_SIZE,_dacTempBuf);
		}
#endif
#if _ADPCM_CH4_PLAY
		if(_registInfo & (0x1<<kADPCM_CH4)){
			ADPCM_CH4_DacIsr(DAC_FIFO_BURST_SIZE,_dacTempBuf);
		}
#endif
#if _ADPCM_CH5_PLAY
		if(_registInfo & (0x1<<kADPCM_CH5)){
			ADPCM_CH5_DacIsr(DAC_FIFO_BURST_SIZE,_dacTempBuf);
		}
#endif
#if _ADPCM_CH6_PLAY
		if(_registInfo & (0x1<<kADPCM_CH6)){
			ADPCM_CH6_DacIsr(DAC_FIFO_BURST_SIZE,_dacTempBuf);
		}
#endif
#if _ADPCM_CH7_PLAY
		if(_registInfo & (0x1<<kADPCM_CH7)){
			ADPCM_CH7_DacIsr(DAC_FIFO_BURST_SIZE,_dacTempBuf);
		}
#endif
#if _ADPCM_CH8_PLAY
		if(_registInfo & (0x1<<kADPCM_CH8)){
			ADPCM_CH8_DacIsr(DAC_FIFO_BURST_SIZE,_dacTempBuf);
		}
#endif
#if _CELP_PLAY
		if(_registInfo & (0x1<<kCELP)){
			CELP_DacIsr(DAC_FIFO_BURST_SIZE);
		}
#endif
#if _PCM_PLAY
		if(_registInfo & (0x1<<kPCM_CH1)){
			PCM_DacIsr(DAC_FIFO_BURST_SIZE);
		}
#endif
#if _PCM_CH2_PLAY
		if(_registInfo & (0x1<<kPCM_CH2)){
			PCM_CH2_DacIsr(DAC_FIFO_BURST_SIZE);
		}
#endif
#if _PCM_CH3_PLAY
		if(_registInfo & (0x1<<kPCM_CH3)){
			PCM_CH3_DacIsr(DAC_FIFO_BURST_SIZE);
		}
#endif
	} while(0);
#endif//#if _ASSIGNABLE_DAC_NUM>0

    for(i=0;i<MIX_BUF_SIZE;i++){
#if _AUDIO_MULTI_CHANNEL
        DacMixBuf[i]/=MixingChanNum[_dacCh];
#endif
	#if _EF_SERIES
		if(((P_DAC->CH0.CH_Ctrl&C_DAC_Out_Mode) == C_DAC_Out_Mode_PWM) && (DacMixBuf[i] > 0)){
			DacMixBuf[i] = (DacMixBuf[i] * PWMCurrentFactor) >> 12;
		}
	#endif
        if(DacMixBuf[i] > (MAX_INT16 - DAC_DATA_OFFSET)) {
			DacMixBuf[i] = MAX_INT16 - DAC_DATA_OFFSET;
		} else if(DacMixBuf[i] < (MIN_INT16 - DAC_DATA_OFFSET)) {
			DacMixBuf[i] = MIN_INT16 - DAC_DATA_OFFSET;
		}
        chx->CH_Data = DacMixBuf[i] + DAC_DATA_OFFSET;
    #if (_AUDIO_RECORD)
        if(_dacCh == _AUDIO_RECORD_AUDIO_CH) {
            RecordMixBuf[i] = DacMixBuf[i];
        }
    #endif
    }
#if (_AUDIO_RECORD)
    if(_dacCh == _AUDIO_RECORD_AUDIO_CH) {
		AudioRecord_DacIsr(DAC_FIFO_BURST_SIZE,RecordMixBuf);
    }
#endif
}
//------------------------------------------------------------------//
// DAC CH0 FIFO Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void DAC_CH0_ISR(void){
#if _ASSIGNABLE_DAC_NUM>=1
    DAC_ISR_ServiceLoop(HW_CH0);
#endif
}
//------------------------------------------------------------------//
// DAC CH1 FIFO Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void DAC_CH1_ISR(void){
#if _ASSIGNABLE_DAC_NUM>=2
    DAC_ISR_ServiceLoop(HW_CH1);
#endif
}
#endif
#if _IS_APPS_USE_ADC_MIC
//------------------------------------------------------------------//
// ADC Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void ADC_ISR(void){

#if _LEDSTR_MODULE
extern U8 LED_pattern_flag;
	if(LEDSTR_TIMER->Flag == 1)
	{
		LEDSTR_Isr();
		LED_pattern_flag=0;
	}
#endif
#if _LEDSTRn_MODULE
extern U8 LED_pattern_flag;
	if(LEDSTRn_TIMER->Flag == 1)
	{
		LEDSTRn_Isr();
		LED_pattern_flag=0;
	}
#endif
#if _LEDSTRm_MODULE
extern U8 LEDm_pattern_flag;
	if(LEDSTRm_TIMER->Flag == 1)
	{
		LEDSTRm_Isr();
		LEDm_pattern_flag=0;
	}
#endif
#if _IS_APPS_USE_ADC_MIC
	U8 i;
	U32 AdcDataBuf[ADC_FIFO_BURST_SIZE]={0};
	U8 ADC_FIFO_SIZE;

	#if _SL_MODULE
	#if _EF_SERIES
		P_ADC->Flag = C_ADC_Scan_CNV_Done_Flag; //Clear ADC Scan flag(NX1F), NX1K use FIFO no INT Flag.
        #if _SL_MIC_NUM==3
		U8  CH0_FIFO_NUM;
        #endif
		U32 AdcDataBuf_CH1[1]={0};
        #if _SL_MIC_NUM==3
		U32 AdcDataBuf_CH2[1]={0};
        #endif
	#else
		U32 AdcDataBuf_CH1[1]={0};
        #if _SL_MIC_NUM==3
		U32 AdcDataBuf_CH2[1]={0};
        #endif
	#endif
	#endif
	#if _SL_MODULE
	if(SL_GetStatus()==SL_ON)
	{
		ADC_FIFO_SIZE=1;
	}
	else
	{
		ADC_FIFO_SIZE=ADC_FIFO_THD;
	}
	#else
		ADC_FIFO_SIZE=ADC_FIFO_THD;
	#endif

#if !_EF_SERIES
	(void)P_ADC->Ctrl;//read register and discard
#endif

	for(i = 0; i < ADC_FIFO_SIZE; i++)
	{
		if(AdcEnFlag==1)
		{
		#if _SL_MODULE
		#if _EF_SERIES
			if(SL_GetStatus()==SL_ON)
			{
            #if _SL_MIC_NUM==3
				AdcDataBuf_CH2[i]   = (P_ADC->Data_CH2);	//keep isr
				AdcDataBuf_CH2[i]   = 0x8000;
            #endif
				AdcDataBuf_CH1[i]   = (P_ADC->Data_CH1);	//keep isr
				AdcDataBuf_CH1[i]   = 0x8000;
			}
		#else
			if(SL_GetStatus()==SL_ON)
			{
            #if _SL_MIC_NUM==3
				AdcDataBuf_CH2[i]   = (P_ADC->Data_CH0);	//keep isr
				AdcDataBuf_CH2[i]   = 0x8000;
            #endif
				AdcDataBuf_CH1[i]   = (P_ADC->Data_CH0);	//keep isr
				AdcDataBuf_CH1[i]   = 0x8000;
			}
		#endif
		#if _EF_SERIES
        #if _SL_MIC_NUM==3
			if(SL_GetStatus()==SL_ON)
			{
				for(CH0_FIFO_NUM=0; CH0_FIFO_NUM<(_SL_ADC_CH0_FIFO_NUM-1); CH0_FIFO_NUM++)
				{
					AdcDataBuf[i]   = (P_ADC->Data_CH0);	//keep isr
					AdcDataBuf[i]   = 0x8000;
				}
			}
        #endif
		#endif
		#endif
				AdcDataBuf[i]   = (P_ADC->Data_CH0);	//keep isr
				AdcDataBuf[i]   = 0x8000;
				AdcEnCnt--;
				if(AdcEnCnt==0)
				{
					AdcEnFlag=0;
				}
		}
		else
		{
		#if _SL_MODULE
		#if _EF_SERIES
			if(SL_GetStatus()==SL_ON)
			{
            #if _SL_MIC_NUM==3
				AdcDataBuf_CH2[i]  = (P_ADC->Data_CH2);		//get CH2 data
            #endif
				AdcDataBuf_CH1[i]  = (P_ADC->Data_CH1);		//get CH1 data
			}
		#else
			if(SL_GetStatus()==SL_ON)
			{
            #if _SL_MIC_NUM==3
				AdcDataBuf_CH2[i]  = (P_ADC->Data_CH0);		//get CH2 data
            #endif
				AdcDataBuf_CH1[i]  = (P_ADC->Data_CH0);		//get CH1 data
			}
		#endif
		#if _EF_SERIES
        #if _SL_MIC_NUM==3
			if(SL_GetStatus()==SL_ON)
			{
				for(CH0_FIFO_NUM=0; CH0_FIFO_NUM<(_SL_ADC_CH0_FIFO_NUM-1); CH0_FIFO_NUM++)
				{
					AdcDataBuf[i]   = (P_ADC->Data_CH0);	//get CH1 data*_SL_ADC_CH0_FIFO_NUM times
				}
			}
        #endif
		#endif
		#endif
			AdcDataBuf[i]  = (P_ADC->Data_CH0);
		}
	}
#endif


#if _VR_FUNC
    VR_AdcIsr(ADC_FIFO_BURST_SIZE, AdcDataBuf);
#endif
#if _ADPCM_RECORD
    ADPCM_AdcIsr(ADC_FIFO_BURST_SIZE, AdcDataBuf);
#endif
#if _SBC_RECORD
    SBC_AdcIsr(ADC_FIFO_BURST_SIZE, AdcDataBuf);
#endif
#if _PCM_RECORD
    PCM_AdcIsr(ADC_FIFO_BURST_SIZE, AdcDataBuf);
#endif
#if _RT_PLAY
    RT_AdcIsr(ADC_FIFO_BURST_SIZE, AdcDataBuf);
#endif
#if _WAVE_ID_MODULE
    WaveID_AdcIsr(ADC_FIFO_BURST_SIZE, AdcDataBuf);
#endif
#if _PD_PLAY
    PD_AdcIsr(ADC_FIFO_BURST_SIZE, AdcDataBuf);
#endif
#if _SOUND_DETECTION
    SOUND_DETECTION_AdcIsr(ADC_FIFO_BURST_SIZE, AdcDataBuf);
#endif
#if _SL_MODULE
#if _EF_SERIES
	#if _SL_MIC_NUM==3
		SL_AdcIsr(ADC_FIFO_SIZE, AdcDataBuf, AdcDataBuf_CH1, AdcDataBuf_CH2);
	#elif _SL_MIC_NUM==2
		SL_AdcIsr(ADC_FIFO_SIZE, AdcDataBuf, AdcDataBuf_CH1);
	#endif
#else
	#if _SL_MIC_NUM==3
		SL_AdcIsr(_SL_ADC_FIFO_NUM, AdcDataBuf, AdcDataBuf_CH1, AdcDataBuf_CH2);
	#elif _SL_MIC_NUM==2
		SL_AdcIsr(_SL_ADC_FIFO_NUM, AdcDataBuf, AdcDataBuf_CH1);
	#endif
#endif
#endif
}
#endif
#if _TIMER_TMR0_INT 									\
    || (_QCODE_BUILD && _INT_TMR0_PATH)                 \
	|| (_QFID_TIMER==USE_TIMER0 &&_QFID_MODULE)			\
	|| (_VIO_TIMER==USE_TIMER0 && _ACTION_MODULE) 		\
	|| (_LEDSTR_MODULE &&(_LEDSTR_TIMER==USE_TIMER0))	\
	|| (_LEDSTRn_MODULE &&(_LEDSTRn_TIMER==USE_TIMER0)) \
	|| (_TOUCHKEY_MODULE&&_EF_SERIES)
//------------------------------------------------------------------//
// Timer0 Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void TMR0_ISR(void)
{
    TIMER0->Flag = 1;
	// User defined code.
	// Code MUST be short and streamlined, or CPU performance will be degraded.
#if _LEDSTR_MODULE &&(_LEDSTR_TIMER==USE_TIMER0)
	GIE_DISABLE();
	LEDSTR_Isr();
	GIE_ENABLE();
#endif
#if _LEDSTRn_MODULE
	GIE_DISABLE();
	LEDSTRn_Isr();
	GIE_ENABLE();
#endif
#if _LEDSTRm_MODULE
	GIE_DISABLE();
	LEDSTRm_Isr();
	GIE_ENABLE();
#endif
#if (_VIO_TIMER==USE_TIMER0  && _ACTION_MODULE)
    VIO_TimerIsr();
#endif
#if _QFID_MODULE && (_QFID_TIMER == USE_TIMER0)
    QFID_TimerIsr();
#endif
#if (_TOUCHKEY_MODULE&&_EF_SERIES)
	TOUCH_CAPTURE();
#endif
#if (_QCODE_BUILD && _INT_TMR0_PATH)
    QC_DIRECT_CALLBACK(QC_IntTmr0Path);
#endif
}
#endif
#if _TIMER_TMR1_INT										\
    || (_QCODE_BUILD && _INT_TMR1_PATH)                 \
	|| (_QFID_TIMER==USE_TIMER1 &&_QFID_MODULE)			\
	|| (_VIO_TIMER==USE_TIMER1 && _ACTION_MODULE)		\
	|| (_LEDSTR_MODULE &&(_LEDSTR_TIMER==USE_TIMER1))	\
	|| (_LEDSTRn_MODULE &&(_LEDSTRn_TIMER==USE_TIMER1))	\
	|| (_LEDSTRm_MODULE &&(_LEDSTRm_TIMER==USE_TIMER1))
//------------------------------------------------------------------//
// Timer1 Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void TMR1_ISR(void)
{
    TIMER1->Flag = 1;
	// User defined code.
	// Code MUST be short and streamlined, or CPU performance will be degraded.
#if _LEDSTR_MODULE &&(_LEDSTR_TIMER==USE_TIMER1)
	GIE_DISABLE();
	LEDSTR_Isr();
	GIE_ENABLE();
#endif
#if _LEDSTRn_MODULE
	GIE_DISABLE();
	LEDSTRn_Isr();
	GIE_ENABLE();
#endif
#if _LEDSTRm_MODULE
	GIE_DISABLE();
	LEDSTRm_Isr();
	GIE_ENABLE();
#endif
#if (_VIO_TIMER==USE_TIMER1 && _ACTION_MODULE)
    VIO_TimerIsr();
#endif
#if _QFID_MODULE && (_QFID_TIMER == USE_TIMER1)
    QFID_TimerIsr();
#endif
#if (_QCODE_BUILD && _INT_TMR1_PATH)
    QC_DIRECT_CALLBACK(QC_IntTmr1Path);
#endif
}
#endif
#if _TIMER_TMR2_INT 									\
    || (_QCODE_BUILD && _INT_TMR2_PATH)                 \
	|| (_QFID_TIMER==USE_TIMER2 && _QFID_MODULE)		\
	|| (_VIO_TIMER==USE_TIMER2 && _ACTION_MODULE)		\
	|| (_LEDSTR_MODULE &&(_LEDSTR_TIMER==USE_TIMER2))	\
	|| (_LEDSTRn_MODULE &&(_LEDSTRn_TIMER==USE_TIMER2))	\
	|| (_LEDSTRm_MODULE &&(_LEDSTRm_TIMER==USE_TIMER2))	\
//------------------------------------------------------------------//
// Timer2 Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void TMR2_ISR(void)
{
    TIMER2->Flag = 1;
	// User defined code.
	// Code MUST be short and streamlined, or CPU performance will be degraded.
#if _LEDSTR_MODULE &&(_LEDSTR_TIMER==USE_TIMER2)
	GIE_DISABLE();
	LEDSTR_Isr();
	GIE_ENABLE();
#endif
#if _LEDSTRn_MODULE
	GIE_DISABLE();
	LEDSTRn_Isr();
	GIE_ENABLE();
#endif
#if _LEDSTRm_MODULE
	GIE_DISABLE();
	LEDSTRm_Isr();
	GIE_ENABLE();
#endif
#if (_VIO_TIMER==USE_TIMER2 && _ACTION_MODULE)
    VIO_TimerIsr();
#endif
#if _QFID_MODULE && (_QFID_TIMER == USE_TIMER2)
    QFID_TimerIsr();
#endif
#if (_QCODE_BUILD && _INT_TMR2_PATH)
    QC_DIRECT_CALLBACK(QC_IntTmr2Path);
#endif
}
#endif
#if _SYS_TIMEBASE
//------------------------------------------------------------------//
// RTC Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void RTC_ISR(void)
{
	// User defined code.
	// Code MUST be short and streamlined, or CPU performance will be degraded.
	U32 _tempRTC_Flag = (P_RTC->Flag)&(P_RTC->INT_Ctrl);
	P_RTC->Flag = _tempRTC_Flag;//clear flag
#if body_is_4KHZ
	if(_tempRTC_Flag & C_RTC_4KHz_Flag)
#else
	if(_tempRTC_Flag & C_RTC_16KHz_Flag)
#endif
	{
#if _QFID_MODULE && (_QFID_TIMER == USE_RTC) && !(body_is_4KHZ)
		QFID_TimerIsr();
#endif
#if (_IR_MODULE)&&(_IR_TX_EN||_IR_RX_EN)
		IR_TimerIsr();
#endif
#if (_SYS_TIMEBASE==TIMEBASE_250us)||(_SYS_TIMEBASE==TIMEBASE_62us)
#if	TIMEBASE_COUNT>1
		--TimebaseCount;
		if(TimebaseCount==0){
			TimebaseCount=TIMEBASE_COUNT;
			TIMEBASE_ServiceLoop();
		}
#else
		TIMEBASE_ServiceLoop();
#endif
#endif
#if _QCODE_BUILD && (_RTC_16384Hz_PATH || _QCODE_RTC_16KHZ_CALLBACK)
        QC_RTC_16384HZ_ISR();
#endif
#if _QCODE_BUILD && (_RTC_4096Hz_PATH || _QCODE_RTC_4KHZ_CALLBACK)
        QC_RTC_4096HZ_ISR();
#endif

	}
	if(_tempRTC_Flag & C_RTC_1KHz_Flag)
	{
#if _SYS_TIMEBASE==TIMEBASE_1000us
		TIMEBASE_ServiceLoop();
#endif
#if _QCODE_BUILD && (_RTC_1024Hz_PATH || _QCODE_RTC_1KHZ_CALLBACK)
        QC_RTC_1024HZ_ISR();
#endif

	}
	if(_tempRTC_Flag & C_RTC_64Hz_Flag)
	{
#if _TOUCHKEY_MODULE && C_TouchSleepFrame<500
        u8Touch1SCount++;
#endif
#if _QCODE_BUILD && (_RTC_64Hz_PATH || _QCODE_RTC_64HZ_CALLBACK)
        QC_RTC_64HZ_ISR();
#endif

	}
	if(_tempRTC_Flag & C_RTC_2Hz_Flag)
	{
#if _TOUCHKEY_MODULE && C_TouchSleepFrame>=500
        u8Touch1SCount++;
#endif
#if _QCODE_BUILD && (_500ms_PATH || _RTC_2Hz_PATH || _QCODE_RTC_2HZ_CALLBACK)
        QC_RTC_2HZ_ISR();
#endif

	}
#if !_EF_SERIES
	RTC_Int_ReEn();
#endif
}
//------------------------------------------------------------------//
// Start systick timer
// Parameter:
//          freq:systick frequency
// return value:
//          NONE
//------------------------------------------------------------------//
void SysTickInit(void)
{
    SysTickCount=SYS_TICK_COUNT;
#if TIMEBASE_COUNT>1
    TimebaseCount=TIMEBASE_COUNT;
#endif
}
//------------------------------------------------------------------//
// Timebase service loop
// Parameter:
//          None
// return value:
//          NONE
//------------------------------------------------------------------//
void TIMEBASE_ServiceLoop(void)
{
	do {	//correct frequence from 32768 to 32000
		#define _SYSTICK_CYCLE_ON_DUTY		125
		#define _SYSTICK_COMPENSATE_CYCLE	128
		static	U8	_freqCompensate=_SYSTICK_CYCLE_ON_DUTY;
		const	U8	_compensate_diff = (_SYSTICK_COMPENSATE_CYCLE-_SYSTICK_CYCLE_ON_DUTY);
		if(_freqCompensate>=_compensate_diff){
			_freqCompensate-=_compensate_diff;
		} else {
			_freqCompensate+=_SYSTICK_CYCLE_ON_DUTY;
			break;//freeze SysTickCount
		}
		--SysTickCount;
		#undef _SYSTICK_CYCLE_ON_DUTY
		#undef _SYSTICK_COMPENSATE_CYCLE
	} while(0);
	if(SysTickCount==0){
		SysTickCount=SYS_TICK_COUNT;
		SysTick+=1;
#if _MATRIX_MODULE
        Matrix_TimerIsr();
#endif
#if _TOUCHKEY_MODULE
		u8TouchFrameTime++;
#endif
#if _PD_PLAY
        PD_MilliSecondIsr();
#endif


#if _AUDIO_SENTENCE
		U8 i;
		#if _ACTION_MODULE
		for(i = 0; i < (_AUDIO_API_CH_NUM + _VIO_CH_NUM); i++){
		#else
		for(i = 0; i < _AUDIO_API_CH_NUM; i++){
		#endif
			if((SentenceStatusMuteTimer[i]>>28) == STATUS_MUTE && (SentenceStatusMuteTimer[i]&(~SPI_DATA_TYPE_MASK2))){
				SentenceStatusMuteTimer[i] -= 1;
				if((SentenceStatusMuteTimer[i]&(~SPI_DATA_TYPE_MASK2)) == 0){
					SentenceStatusMuteTimer[i] = (STATUS_STOP<<28) | 0;
				}
			}
		}
#endif
		DIRECTKEY_TimerIsr();

		//--------------------------------------------------------------------------
		// 2023-09-14添加,此处为1ms中断，用于用户计时
		// 注意：当母体为NX12/13FS、且有录音和录变音动作时，打开IRAM和nx1_config.h文件里的
		// 		 宏_SBC_RECORD_ERASING、_ADPCM_RECORD_ERASING时，不会调用此函数，其他情况
		//		 会调用此函数
		//--------------------------------------------------------------------------
		user_timebase_from_rtc_isr();
	}
#if (_VIO_TIMER==USE_RTC && _ACTION_MODULE)
	VIO_TimerIsr();
#endif
#if (_SW_PWM_MODULE)
	SW_PWM_TimerIsr();
#endif
}
#endif//#if _SYS_TIMEBASE
#if  _PWMA_INT												\
    || (_QCODE_BUILD && _INT_TMR_PWMA_PATH)                 \
	|| (_QFID_TIMER==USE_TIMER_PWMA && _QFID_MODULE)		\
	|| (_VIO_TIMER==USE_TIMER_PWMA && _ACTION_MODULE)		\
	|| (_LEDSTR_MODULE &&(_LEDSTR_TIMER==USE_TIMER_PWMA))	\
	|| (_LEDSTRn_MODULE &&(_LEDSTRn_TIMER==USE_TIMER_PWMA))	\
	|| (_LEDSTRm_MODULE &&(_LEDSTRm_TIMER==USE_TIMER_PWMA))
//------------------------------------------------------------------//
// PWMA Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void PWMA_ISR(void)
{
    P_PWMA->TMR_Flag=1;
#if _LEDSTR_MODULE &&(_LEDSTR_TIMER==USE_TIMER_PWMA)
	GIE_DISABLE();
	LEDSTR_Isr();
	GIE_ENABLE();
#endif
#if _LEDSTRn_MODULE
	GIE_DISABLE();
	LEDSTRn_Isr();
	GIE_ENABLE();
#endif
#if _LEDSTRm_MODULE
	GIE_DISABLE();
	LEDSTRm_Isr();
	GIE_ENABLE();
#endif
#if (_VIO_TIMER==USE_TIMER_PWMA && _ACTION_MODULE)
    VIO_TimerIsr();
#endif
#if _QFID_MODULE && (_QFID_TIMER == USE_TIMER_PWMA)
    QFID_TimerIsr();
#endif
#if (_QCODE_BUILD && _INT_TMR_PWMA_PATH)
    QC_DIRECT_CALLBACK(QC_IntTmrPwmaPath);
#endif
	// User defined code.
	// Code MUST be short and streamlined, or CPU performance will be degraded.
}
#endif
#if  _PWMB_INT												\
    || (_QCODE_BUILD && _INT_TMR_PWMB_PATH)                 \
	|| (_QFID_TIMER==USE_TIMER_PWMB && _QFID_MODULE)		\
	|| (_VIO_TIMER==USE_TIMER_PWMB && _ACTION_MODULE) 		\
	|| (_LEDSTR_MODULE &&(_LEDSTR_TIMER==USE_TIMER_PWMB))	\
	|| (_LEDSTRn_MODULE &&(_LEDSTRn_TIMER==USE_TIMER_PWMB))	\
	|| (_LEDSTRm_MODULE &&(_LEDSTRm_TIMER==USE_TIMER_PWMB))
//------------------------------------------------------------------//
// PWMB Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void PWMB_ISR(void)
{
    P_PWMB->TMR_Flag=1;
#if _LEDSTR_MODULE &&(_LEDSTR_TIMER==USE_TIMER_PWMB)
	GIE_DISABLE();
	LEDSTR_Isr();
	GIE_ENABLE();
#endif
#if _LEDSTRn_MODULE
	GIE_DISABLE();
	LEDSTRn_Isr();
	GIE_ENABLE();
#endif
#if _LEDSTRm_MODULE
	GIE_DISABLE();
	LEDSTRm_Isr();
	GIE_ENABLE();
#endif
#if (_VIO_TIMER==USE_TIMER_PWMB && _ACTION_MODULE)
    VIO_TimerIsr();
#endif
#if _QFID_MODULE && (_QFID_TIMER == USE_TIMER_PWMB)
    QFID_TimerIsr();
#endif
#if (_QCODE_BUILD && _INT_TMR_PWMB_PATH)
    QC_DIRECT_CALLBACK(QC_IntTmrPwmbPath);
#endif
	// User defined code.
	// Code MUST be short and streamlined, or CPU performance will be degraded.
}
#endif
#if !_EF_SERIES
#if  (_CAPTURE_INT||(_TOUCHKEY_MODULE&&!_EF_SERIES))
//------------------------------------------------------------------//
// Capture Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void CAPTURE_ISR(void)
{
#if _CAPTURE_INT
    U8 sts;

    sts=P_PWMA->CAP_Flag&P_PWMA->CAP_INT_Ctrl;
    P_PWMA->CAP_Flag=sts;
	// User defined code.
	// Code MUST be short and streamlined, or CPU performance will be degraded.
#endif

#if (_TOUCHKEY_MODULE&&!_EF_SERIES)
	TOUCH_CAPTURE();
#endif

}
#endif
#if _GPIOA_INT
//------------------------------------------------------------------//
// GPIOA Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void GPIOA_ISR(void)
{
	// User defined code.
	// Code MUST be short and streamlined, or CPU performance will be degraded.

	if((P_PORT_EXT->EXT_INT_Flag & C_EXT_INT_Flag) && (P_PORT_EXT->EXT_INT_Ctrl & C_EXT_INT_IEn))
	{
		P_PORT_EXT->EXT_INT_Flag = C_EXT_INT_Flag;
	}
	GPIO_ExtInt_ReEn();
}
#endif
#if _GPIOB_INT
//------------------------------------------------------------------//
// GPIOB Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void GPIOB_ISR(void)
{
	// User defined code.
	// Code MUST be short and streamlined, or CPU performance will be degraded.

    if((P_PORT_EXT->EXT_INT_Flag & C_EXT1_INT_Flag) && (P_PORT_EXT->EXT_INT_Ctrl & C_EXT1_INT_IEn))
	{
		P_PORT_EXT->EXT_INT_Flag = C_EXT1_INT_Flag;
	}
	GPIO_ExtInt_ReEn();
}
#endif
#if _GPIOC_INT
//------------------------------------------------------------------//
// GPIOC Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void GPIOC_ISR(void)
{
	// User defined code.
	// Code MUST be short and streamlined, or CPU performance will be degraded.

}
#endif
#endif
#if _SPI_INT
//------------------------------------------------------------------//
// SPI0 Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI0_ISR(void)
{
	// User defined code.
	// Code MUST be short and streamlined, or CPU performance will be degraded.

}
#endif
#if _SPI1_INT
//------------------------------------------------------------------//
// SPI1 Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SPI1_ISR(void)
{
	// User defined code.
	// Code MUST be short and streamlined, or CPU performance will be degraded.

}
#endif
#if _I2C_INT
//------------------------------------------------------------------//
// SPI1 Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void I2C_ISR(void)
{
	// User defined code.
	// Code MUST be short and streamlined, or CPU performance will be degraded.

}
#endif
#if _UART_MODULE && !_DEBUG_MODE
//------------------------------------------------------------------//
// UART Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void UART_ISR(void)
{
	// User defined code.
	// Code MUST be short and streamlined, or CPU performance will be degraded.


	// RX ISR example code
/*
  	U8 sts;
    sts = UART_GetFIFOSts(UART);

	if(sts & UART_RXFIFO_NOEMPTY){			// RX_MODE == UART_RXFIFO_NOEMPTY
    	extern U8 rxData;
        rxData = UART_ReceiveData(UART);
	}
#if !EF11FS_SERIES
    if(sts & UART_RXFIFO_FULL){				// RX_MODE == UART_RXFIFO_FULL
		extern U8 rxDataBuf[4];
        rxDataBuf[0] = UART_ReceiveData(UART);
		rxDataBuf[1] = UART_ReceiveData(UART);
		rxDataBuf[2] = UART_ReceiveData(UART);
		rxDataBuf[3] = UART_ReceiveData(UART);
	}
#endif
*/
}
#endif
#if _SW_INT
//------------------------------------------------------------------//
// SWI Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SWI_ISR(void)
{
	INTC_ClrSWI();

	// User defined code.
	// Code MUST be short and streamlined, or CPU performance will be degraded.
}
//------------------------------------------------------------------//
// SWA Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SWA_ISR(void)
{
	SMU_SWATrig(BIT_UNSET);

	// User defined code.
	// Code MUST be short and streamlined, or CPU performance will be degraded.
}
#endif
#if _EF_SERIES
#if _EXIT0_INT
//------------------------------------------------------------------//
// External INT0 Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void EXIT0_ISR(void)
{
	// User defined code.
	// Code MUST be short and streamlined, or CPU performance will be degraded.
	if((P_PORT_EXT->EXT_INT_Flag & C_EXT_INT_Flag) && (P_PORT_EXT->EXT_INT_Ctrl & C_EXT_INT_IEn))
	{
		P_PORT_EXT->EXT_INT_Flag = C_EXT_INT_Flag;
	}
	GPIO_ExtInt_ReEn();
}
#endif
#if _EXIT1_INT
//------------------------------------------------------------------//
// External INT1 Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void EXIT1_ISR(void)
{
	// User defined code.
	// Code MUST be short and streamlined, or CPU performance will be degraded.
    if((P_PORT_EXT->EXT_INT_Flag & C_EXT1_INT_Flag) && (P_PORT_EXT->EXT_INT_Ctrl & C_EXT1_INT_IEn))
	{
		P_PORT_EXT->EXT_INT_Flag = C_EXT1_INT_Flag;
	}
	GPIO_ExtInt_ReEn();
}
#endif
#if _TIMER_TMR3_INT										\
    || (_QCODE_BUILD && _INT_TMR3_PATH)                 \
	|| (_QFID_TIMER==USE_TIMER3 && _QFID_MODULE)		\
	|| (_VIO_TIMER==USE_TIMER3 && _ACTION_MODULE)		\
	||(_LEDSTR_MODULE &&(_LEDSTR_TIMER==USE_TIMER3))	\
	||(_LEDSTRn_MODULE &&(_LEDSTRn_TIMER==USE_TIMER3))	\
	||(_LEDSTRm_MODULE &&(_LEDSTRm_TIMER==USE_TIMER3))
//------------------------------------------------------------------//
// Timer3 Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void TMR3_ISR(void)
{
    TIMER3->Flag = 1;
	// User defined code.
	// Code MUST be short and streamlined, or CPU performance will be degraded.
#if _LEDSTR_MODULE &&(_LEDSTR_TIMER==USE_TIMER3)
	GIE_DISABLE();
	LEDSTR_Isr();
	GIE_ENABLE();
#endif
#if _LEDSTRn_MODULE
	GIE_DISABLE();
	LEDSTRn_Isr();
	GIE_ENABLE();
#endif
#if _LEDSTRm_MODULE
	GIE_DISABLE();
	LEDSTRm_Isr();
	GIE_ENABLE();
#endif
#if (_VIO_TIMER==USE_TIMER3 && _ACTION_MODULE)
    VIO_TimerIsr();
#endif
#if _QFID_MODULE && (_QFID_TIMER == USE_TIMER3)
    QFID_TimerIsr();
#endif
#if (_QCODE_BUILD && _INT_TMR3_PATH)
    QC_DIRECT_CALLBACK(QC_IntTmr3Path);
#endif
}
#endif
#if _FMC_INT
//------------------------------------------------------------------//
// FMC Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void FMC_ISR(void)
{
	P_FMC->Flag = 1;
	// User defined code.
	// Code MUST be short and streamlined, or CPU performance will be degraded.
}
#endif
#if _NFC_INT
//------------------------------------------------------------------//
// NFC Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void NFC_ISR(void)
{
	P_NFC->Flag = 1;
	// User defined code.
	// Code MUST be short and streamlined, or CPU performance will be degraded.
}
#endif
#endif
