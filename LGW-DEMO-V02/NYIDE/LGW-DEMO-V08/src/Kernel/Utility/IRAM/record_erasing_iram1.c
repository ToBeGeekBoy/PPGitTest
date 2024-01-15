// =========================================================================
// Date: 2/5/2021
// Description:
// =========================================================================
#include "include.h"
#include "nx1_lib.h"
#include "nx1_adc.h"
#include "nx1_dac.h"
#include "nx1_gpio.h"
#include "user_main.h"

#if _EF_SERIES && ((_ADPCM_RECORD && _ADPCM_RECORD_ERASING) || (_SBC_RECORD && _SBC_RECORD_ERASING))
U32 intcCmdTemp;

#if OPTION_IRAM
#if (_ADPCM_RECORD && _ADPCM_RECORD_ERASING) || (_SBC_RECORD && _SBC_RECORD_ERASING && _ADPCM_PLAY && _ADPCM_PLAY_SR_FOR_SBC_REC)
struct AUDIO_BUF_CTL_ADPCM{
    S32 process_count;	//decode/encode count
    S32 fifo_count;	    //decode/encode count
    S32 fifo_count_tmp;
    U32 samples;	    //samples

#if _ADPCM_ECHO_SOUND && !(_ADPCM_SPEED_CHANGE || _ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ROBOT3_SOUND|| _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR)
	U32 samples2;		//for echo-delay used
#endif

#if _ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC || _ADPCM_DARTH_SOUND || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR
    U32 sample_count;	//samples
    U32 fifo_count2;	//decode/encode count
#endif
#if _ADPCM_PLAY_REPEAT && !(_ADPCM_SPEED_CHANGE || _ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR)
	U32 encode_size;
#endif
    //U16 vol;            //volume
    U16 sample_rate;    //sample_rate
    U8 out_size;        //audio frame size
    U8 in_size;         //data frame size
    U8 state;           //channel state
    U8 shadow_state;    //channel shadow state
//    U8 ramp;            //smooth ramp
#if _ADPCM_RECORD
    U8 out_count;
#endif
    U8 index;
    U8 upsample;
    U8  frame_size;
};

struct BUF_CTL_ADPCM{
    U16 *buf;
    U16 size;
    U16 offset;
    U16 rd_offset;
    U16 wr_offset;
};

extern struct AUDIO_BUF_CTL_ADPCM AudioBufCtl_ADPCM;
extern struct BUF_CTL_ADPCM BufCtl_ADPCM;

#if _ADPCM_RECORD && _ADPCM_RECORD_ERASING
//------------------------------------------------------------------//
// Adc ISR
// Parameter:
//          size: data-buffer fill size
//       pdest32: data-buffer address
// return value:
//          NONE
//------------------------------------------------------------------//
static void ADPCM_Adc_IRAM_Isr(U8 size, U32 *pdest32){
    S16 *pdst16;
    U8 i;
    U16 temp;

    if(AudioBufCtl_ADPCM.state==STATUS_RECORDING){

        temp=BufCtl_ADPCM.size-BufCtl_ADPCM.offset;
        if(size>temp){
            size=temp;
        }
#if 0//AGC_CTL_ENABLE
        temp=AGC_FRAME_SIZE-AgcCnt;
        if(size>temp){
            size=temp;
        }
#endif
        if((AudioBufCtl_ADPCM.fifo_count+size-AudioBufCtl_ADPCM.process_count)>(AudioBufCtl_ADPCM.out_count*AudioBufCtl_ADPCM.out_size)){
            for(i=0;i<size;i++){
                temp=((*pdest32++)-32768);
            }
            //dprintf("E\r\n");
            size=0;
        }
        //dprintf("size,%d\r\n",size);
        AudioBufCtl_ADPCM.fifo_count+=size;


        pdst16=(S16 *)(BufCtl_ADPCM.buf+BufCtl_ADPCM.offset);

        for(i=0;i<size;i++){
            temp=((*pdest32++)-32768);
            *pdst16++=temp;
#if 0//AGC_CTL_ENABLE
            AgcBuf[AgcCnt+i]=temp;
#endif
//            temp=(*pdest32-32768);
//            *pdst16++= (VOICEFILEX[VoiceIdx]|(VOICEFILEX[VoiceIdx+1]<<8));
//            VoiceIdx+=2;
        }

        BufCtl_ADPCM.offset+=size;

#if 0//AGC_CTL_ENABLE
        AgcCnt+=size;
        if(AgcCnt==AGC_FRAME_SIZE){
            AgcCnt=0;

            AgcProcess((S16 *)AgcBuf,AGC_FRAME_SIZE);
        }
#endif
        if(BufCtl_ADPCM.offset==BufCtl_ADPCM.size){
            BufCtl_ADPCM.offset=0;
        }
    }
}
#endif //#if _ADPCM_RECORD && _ADPCM_RECORD_ERASING

#if _SBC_RECORD && _SBC_RECORD_ERASING && _ADPCM_PLAY && _ADPCM_PLAY_SR_FOR_SBC_REC
#define DAC_FIFO_BURST_SIZE     (DAC_FIFO_LEN-DAC_FIFO_THD)
#define MIX_BUF_SIZE            DAC_FIFO_BURST_SIZE
extern S32 DacMixBuf[MIX_BUF_SIZE];
extern U8  PlayEndFlag;
//------------------------------------------------------------------//
// Write Dac Fifo
// Parameter:
//          size: size to write
//          psrc16: source pointer
// return value:
//          NONE
//------------------------------------------------------------------//
void CB_ADPCM_WriteDac_IRAM(U8 size,U16* psrc16)
{
    U8 i;

    if(psrc16!=0)
    {
        for(i=0;i<size;i++)
        {
#if _AUDIO_MIX_CONTROL
            extern U8 AdpcmMixCtrl;
            DacMixBuf[i]+=(S32)((((S16)*psrc16)*AdpcmMixCtrl)>>2);
#else
            DacMixBuf[i]+=(S32)(S16)*psrc16;
#endif
            psrc16++;
        }
    }
}

//------------------------------------------------------------------//
// Dac ISR
// Parameter:
//          size: fifo fill size
//           buf: fifo buffer
// return value:
//          NONE
//------------------------------------------------------------------//
void ADPCM_Dac_IRAM_Isr(U8 size,S16 *buf){

    S16 *psrc16;//,gain;
    //U8 i,len,size2;

	if(AudioBufCtl_ADPCM.shadow_state==STATUS_PLAYING){

#if _ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND  || _ADPCM_ECHO_SOUND  || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR
		S16 temp;
        temp=BufCtl_ADPCM.size-BufCtl_ADPCM.offset;
        if(size>temp){
            size=temp;
        }
#endif
//        //dprintf("size,%d\r\n",size);
        AudioBufCtl_ADPCM.fifo_count+=size;

#if _ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC  || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR
        psrc16=(S16 *)(BufCtl_ADPCM.buf+BufCtl_ADPCM.offset);

        CB_ADPCM_WriteDac_IRAM(size,(U16 *)psrc16);

        BufCtl_ADPCM.offset+=size;

        if(BufCtl_ADPCM.offset==BufCtl_ADPCM.size){
            BufCtl_ADPCM.offset=0;
        }

#endif

		{
			if(AudioBufCtl_ADPCM.fifo_count>=AudioBufCtl_ADPCM.samples){
				AudioBufCtl_ADPCM.state=STATUS_STOP;
				AudioBufCtl_ADPCM.shadow_state=AudioBufCtl_ADPCM.state;
				PlayEndFlag = BIT_SET;
				//dprintf("stop a,%d,%d\r\n",AudioBufCtl.fifo_count,AudioBufCtl.samples);
			}
		}
    }else{
        {

            CB_ADPCM_WriteDac_IRAM(size,(U16 *)0);
        }

    }
}
#endif //#if _SBC_RECORD && _SBC_RECORD_ERASING && _ADPCM_PLAY && _ADPCM_PLAY_SR_FOR_SBC_REC
#endif //#if (_ADPCM_RECORD && _ADPCM_RECORD_ERASING) || (_SBC_RECORD && _SBC_RECORD_ERASING && _ADPCM_PLAY && _ADPCM_PLAY_SR_FOR_SBC_REC)

#if _SBC_RECORD && _SBC_RECORD_ERASING
struct AUDIO_BUF_CTL_SBC{
    S32 process_count;	//decode/encode count
    S32 fifo_count;	    //decode/encode count
    U32 samples;	    //samples
#if _SBC_SPEED_CHANGE||_SBC_PITCH_CHANGE||_SBC_QIO_MODULE
    U32 sample_count;	    //samples
    U32 fifo_count2;	    //decode/encode count
#endif//_SBC_SPEED_CHANGE||_SBC_PITCH_CHANGE||_SBC_QIO_MODULE
    //U16 vol;            //volume
    U16 sample_rate;    //sample_rate
#if _SBC_AUDIO_BUF_X3
	U16 preDecCount;
	U8  preDecFrame;
#endif
    U8 in_size;         //data frame size
    U8 state;           //channel state
    U8 shadow_state;    //channel shadow state
    //U8 ramp;            //smooth ramp
    U8 index;
    U8 decode_state;
    U8 out_count;
	U8  upsample;
};

struct BUF_CTL_SBC{
    U16 *buf;
    U16 size;
    U16 offset;
};

extern struct AUDIO_BUF_CTL_SBC AudioBufCtl_SBC;
extern struct BUF_CTL_SBC BufCtl_SBC;

//------------------------------------------------------------------//
// Adc ISR
// Parameter:
//          size: data-buffer fill size
//       pdest32: data-buffer address
// return value:
//          NONE
//------------------------------------------------------------------//
static void SBC_Adc_IRAM_Isr(U8 size, U32 *pdest32){
    S16 *pdst16;
    U8 i;
    U16 temp;

    if(AudioBufCtl_SBC.state==STATUS_RECORDING){

        temp=BufCtl_SBC.size-BufCtl_SBC.offset;
        if(size>temp){
            size=temp;
        }
#if 0//AGC_CTL_ENABLE
        temp=AGC_FRAME_SIZE-AgcCnt;
        if(size>temp){
            size=temp;
        }
#endif
        //dprintf("size,%d\r\n",size);
        if((AudioBufCtl_SBC.fifo_count+size-AudioBufCtl_SBC.process_count)>SBC_AUDIO_BUF_SIZE){
            for(i=0;i<size;i++){
                temp=((*pdest32++)-32768);
            }
            size=0;
        }
        AudioBufCtl_SBC.fifo_count+=size;


        pdst16=(S16 *)(BufCtl_SBC.buf+BufCtl_SBC.offset);

        for(i=0;i<size;i++){
            temp=((*pdest32++)-32768);
            *pdst16++=temp;
#if 0//AGC_CTL_ENABLE
            AgcBuf[AgcCnt+i]=temp;
#endif
//            temp=(*pdest32-32768);
//            *pdst16++= (VOICEFILEX[VoiceIdx]|(VOICEFILEX[VoiceIdx+1]<<8));
//            VoiceIdx+=2;
        }
        BufCtl_SBC.offset+=size;

#if 0//AGC_CTL_ENABLE
        AgcCnt+=size;
        if(AgcCnt==AGC_FRAME_SIZE){
            AgcCnt=0;

            AgcProcess((S16 *)AgcBuf,AGC_FRAME_SIZE);
        }
#endif
        if(BufCtl_SBC.offset==BufCtl_SBC.size){
            BufCtl_SBC.offset=0;
        }
    }
}
#endif //#if _SBC_RECORD && _SBC_RECORD_ERASING


#define ADC_FIFO_BURST_SIZE     ADC_FIFO_THD
//------------------------------------------------------------------//
// ADC Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void ADC_IRAM_ISR(void)
{

#if _IS_APPS_USE_ADC_MIC
	U8 i;
	U32 AdcDataBuf[ADC_FIFO_BURST_SIZE];

	for(i = 0; i < ADC_FIFO_BURST_SIZE; i++){

		if(AdcEnFlag==1)
		{
			AdcDataBuf[i] = (P_ADC->Data_CH0);	//keep isr
			AdcDataBuf[i] = 0x8000;
			AdcEnCnt--;
			if(AdcEnCnt==0)
    		{
    			AdcEnFlag=0;
    		}

		}
		else
		{
			AdcDataBuf[i] = (P_ADC->Data_CH0);
		}
	}
#endif

#if _ADPCM_RECORD && _ADPCM_RECORD_ERASING
    ADPCM_Adc_IRAM_Isr(ADC_FIFO_BURST_SIZE, AdcDataBuf);
#endif
#if _SBC_RECORD && _SBC_RECORD_ERASING
    SBC_Adc_IRAM_Isr(ADC_FIFO_BURST_SIZE, AdcDataBuf);
#endif
}

#if _SBC_RECORD && _SBC_RECORD_ERASING && _ADPCM_PLAY && _ADPCM_PLAY_SR_FOR_SBC_REC
extern DAC_ASSIGNER_MEM dacAssigner[_ASSIGNABLE_DAC_NUM];
//----------------------------------------------------------------------------//
// get regist info
// Parameter:
//			_dacCh		:HW_CH0(0),HW_CH1(1)
// return value:
//          true/false
//----------------------------------------------------------------------------//
U32 DacAssigner_registInfo_IRAM(const S8 _dacCh)
{
	//if((_dacCh<0)||(_dacCh>=_ASSIGNABLE_DAC_NUM))return false;
    return dacAssigner[_dacCh].u32_dacCodecMem;
}
//----------------------------------------------------------------------------//
// DAC fifo data number check
// Parameter:
//			_dacCh		:HW_CH0(0),HW_CH1(1)
// return value:
//          EXIT_SUCCESS/EXIT_FAILURE
//----------------------------------------------------------------------------//
U8 DacAssigner_fifoDataNumberCheck_IRAM(const S8 _dacCh)
{
	//if((_dacCh<0)||(_dacCh>=_ASSIGNABLE_DAC_NUM))return EXIT_FAILURE;
	if(dacAssigner[_dacCh].u32_dacCodecMem!=0){
		dacAssigner[_dacCh].u8_fifoDataMem=DAC_FIFO_LEN*2;
		dacAssigner[_dacCh].u8_dacDisableFlag=0;
	}else{
		if(dacAssigner[_dacCh].u8_fifoDataMem>(DAC_FIFO_LEN-DAC_FIFO_THD)){
			dacAssigner[_dacCh].u8_fifoDataMem-=(DAC_FIFO_LEN-DAC_FIFO_THD);
		}else{
			dacAssigner[_dacCh].u8_fifoDataMem=0;
			dacAssigner[_dacCh].u8_dacDisableFlag=1;
		}
	}
	return EXIT_SUCCESS;
}

extern S16 _dacTempBuf[DAC_FIFO_BURST_SIZE];
extern U16 PWMCurrentFactor;
//------------------------------------------------------------------//
// DAC ISR serviceprocess
// Parameter:
//          _dacCh		:HW_CH0(0),HW_CH1(1)
// return value:
//          NONE
//------------------------------------------------------------------//
static inline void DAC_IRAM_ISR_ServiceLoop(const S8 _dacCh){
    //if((_dacCh<0)||(_dacCh>=_ASSIGNABLE_DAC_NUM))return;

    U8 i;
    for(i=0;i<MIX_BUF_SIZE;i++){
        DacMixBuf[i]=0;
    }

#if _ASSIGNABLE_DAC_NUM>0
	DacAssigner_fifoDataNumberCheck_IRAM(_dacCh);
	do {
        U32 _registInfo = DacAssigner_registInfo_IRAM(_dacCh);
#if _ADPCM_PLAY
		if(_registInfo & (0x1<<kADPCM_CH1)){
			ADPCM_Dac_IRAM_Isr(DAC_FIFO_BURST_SIZE,_dacTempBuf);
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
        if(DacMixBuf[i] > (MAX_INT16 - DAC_DATA_OFFSET)){
			DacMixBuf[i] = MAX_INT16 - DAC_DATA_OFFSET;
		}else if(DacMixBuf[i] < (MIN_INT16 - DAC_DATA_OFFSET)){
			DacMixBuf[i] = MIN_INT16 - DAC_DATA_OFFSET;
		}
        pHwCtl[_dacCh].p_dac->CH_Data = DacMixBuf[i] + DAC_DATA_OFFSET;
    }
}
#if _ADPCM_PLAY_AUDIO_CH==HW_CH0
//------------------------------------------------------------------//
// DAC CH0 FIFO Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void DAC_CH0_IRAM_ISR(void){
#if _ASSIGNABLE_DAC_NUM>=1
    DAC_IRAM_ISR_ServiceLoop(HW_CH0);
#endif
}
#endif //#if _ADPCM_PLAY_AUDIO_CH==HW_CH0

#if _ADPCM_PLAY_AUDIO_CH==HW_CH1
//------------------------------------------------------------------//
// DAC CH1 FIFO Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void DAC_CH1_IRAM_ISR(void){
#if _ASSIGNABLE_DAC_NUM>=2
    DAC_IRAM_ISR_ServiceLoop(HW_CH1);
#endif
}
#endif //#if _ADPCM_PLAY_AUDIO_CH==HW_CH1
#endif //#if _SBC_RECORD && _SBC_RECORD_ERASING && _ADPCM_PLAY && _ADPCM_PLAY_SR_FOR_SBC_REC

#if (_DIRECTKEY_MODULE) \
    || (_MATRIX_MODULE)
//------------------------------------------------------------------//
// Get lowest set bit of variable
// Parameter:
//          _value:1~U32_Max(Warning, 0 is illegal input)
// return value:
//          0~31(bit0~bit31)
//------------------------------------------------------------------//
U8	Get_LS1B_Position_IRAM(U32 _value)
{
    extern  U8              u8_DeBruijn_Bit_PosTab32[32];
	_value=_value&(-_value);//keepLowestBit, equal _value&((~_value)+1)
	return u8_DeBruijn_Bit_PosTab32[((_value*0x077CB531)>>27)];
}
#endif
//////////////////////////////////////////////////////////////////////
// Directkey
//////////////////////////////////////////////////////////////////////
#include "key.h"
#if _DIRECTKEY_MODULE && DIRECTKEY_PORT_NUM
extern	S_DKeyCtrl		sDKeyCtrl;
extern	K_DKeyInPara	kDKeyInPara[DIRECTKEY_PORT_NUM];
#endif
//------------------------------------------------------------------//
// Move key events to key queue by port
// Parameter:
//			keyEvent
//          keyEventVar
//          portLable
// return value:
//          NONE
//------------------------------------------------------------------//
#if _DIRECTKEY_MODULE && DIRECTKEY_PORT_NUM
static void KeyEventToQueueByPort_IRAM(U8 keyEvent, U16 *keyEventVar, U8 portLable) {
	while(*keyEventVar) {
		if(sDKeyCtrl.qBufCnt>=DKEY_QUEUE_LEN)
			break;
		U8 j = Get_LS1B_Position_IRAM(*keyEventVar);
		sDKeyCtrl.qBuf[sDKeyCtrl.qWIndex] = ((keyEvent<<6) | (portLable<<4) | j);
		sDKeyCtrl.qWIndex++;
		if(sDKeyCtrl.qWIndex==DKEY_QUEUE_LEN) {
			sDKeyCtrl.qWIndex = 0;
		}
		sDKeyCtrl.qBufCnt++;
		*keyEventVar &= ~(1<<j);
	}
}
#endif
//------------------------------------------------------------------//
// Scan keys by Port
// Parameter:
//          pPort
//          pPara
// return value:
//          NONE
//------------------------------------------------------------------//
#if _DIRECTKEY_MODULE && DIRECTKEY_PORT_NUM
static void KeyScanByPort_IRAM(S_DKeyPortCtrl *pPort, K_DKeyInPara *pPara) {
    U16 _data = ~(pPara->port->PAD);
    _data ^= (pPara->busyDefine);
    _data &= (pPara->mask) & (pPara->port->DIR);
#if DEBOUNCE_CNT>0
#if GPIOA_SHORT_DB_MASK||GPIOB_SHORT_DB_MASK||GPIOC_SHORT_DB_MASK||GPIOD_SHORT_DB_MASK
    U16 _sdData;
    if(pPara->shortDebounceMask) {
        _sdData = _data^(pPort->prev);
        _sdData &= (pPara->shortDebounceMask);
        pPort->pressEvent |= _sdData & _data;
        pPort->releaseEvent |= _sdData & (pPort->prev);
        pPort->prev ^= _sdData;
        pPort->checked ^= _sdData;

        KeyEventToQueueByPort_IRAM(DKEY_RELEASE, &(pPort->releaseEvent), pPara->portLable);
        KeyEventToQueueByPort_IRAM(DKEY_PRESS, &(pPort->pressEvent), pPara->portLable);
    }
#endif
	U16 _peekPrev = pPort->prev;
	pPort->prev = _data;
    if(_data == pPort->checked) {
        pPort->debounceCount = 0;
        return;
    }
    if(_data != _peekPrev){
		pPort->debounceCount = DEBOUNCE_CNT;//!!!
		return;
	}
    if(pPort->debounceCount > 1){
		--(pPort->debounceCount);
		return;
	}
	pPort->debounceCount = 0;
#else
    if(_data == pPort->checked) return;
#endif
    U16 _diff = _data^(pPort->checked);
	pPort->pressEvent |= _diff&_data;
	pPort->releaseEvent |= _diff&(pPort->checked);
	pPort->checked=_data;

	KeyEventToQueueByPort_IRAM(DKEY_RELEASE, &(pPort->releaseEvent), pPara->portLable);
	KeyEventToQueueByPort_IRAM(DKEY_PRESS, &(pPort->pressEvent), pPara->portLable);
}
#endif
//------------------------------------------------------------------//
// Scan keys
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
#if _DIRECTKEY_MODULE && DIRECTKEY_PORT_NUM
static void KeyScan_IRAM(void){
    U8 i;
    for(i=0;i<DIRECTKEY_PORT_NUM;++i) {
        KeyScanByPort_IRAM(&(sDKeyCtrl.sIn[i]), &kDKeyInPara[i]);
    }
}
#endif
//------------------------------------------------------------------//
// Directkey Timer Isr(Trigger by ms timer interrupt)
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void DIRECTKEY_IRAM_TimerIsr(){
#if _DIRECTKEY_MODULE && DIRECTKEY_PORT_NUM
    if(sDKeyCtrl.getKeySemaphore || !sDKeyCtrl.KeyScanCmd)return;
#if KEY_SCAN_TICK>1
	if(sDKeyCtrl.KeyScanCount>1) {
		--sDKeyCtrl.KeyScanCount;// KeyScanCount = KEY_SCAN_TICK~1
		return;
	}
	sDKeyCtrl.KeyScanCount = KEY_SCAN_TICK;
#endif
	KeyScan_IRAM();
#endif
}

//////////////////////////////////////////////////////////////////////
// time base
//////////////////////////////////////////////////////////////////////
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

extern U8 SysTickCount;
extern U8 TimebaseCount;
//------------------------------------------------------------------//
// Timebase service loop
// Parameter:
//          None
// return value:
//          NONE
//------------------------------------------------------------------//
static void TIMEBASE_IRAM_ServiceLoop(void)
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

		DIRECTKEY_IRAM_TimerIsr();

        //--------------------------------------------------------------------------
        // 2023-09-14添加，解决录音播放会有“噗“”噗”杂音伴随着人声的问题
        // 不要将按键等需要精准定时的处理放在这里，会导致“噗“”噗”杂音
        //--------------------------------------------------------------------------
        extern u32 gu32_user_systick_ms;
        extern u32 gu32_user_10ms_count;

        ++gu32_user_systick_ms;

        if(++gu32_user_10ms_count >= 10)
        {
            gu32_user_10ms_count = 0;

            gu8_flag_10ms = 1; // 10ms 标志位 按鍵扫描 使用. 置位的操作在 RTC_ISR() 函数中，该函数在 isr.c 文件中

            #if _VR_FUNC
                gu8_vr_flag_10ms = 1;
            #endif
        }
	}
}
//------------------------------------------------------------------//
// RTC Interrupt Service Routine
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void RTC_IRAM_ISR(void)
{
	U32 _tempRTC_Flag = (P_RTC->Flag)&(P_RTC->INT_Ctrl);
	P_RTC->Flag = _tempRTC_Flag;//clear flag
#if body_is_4KHZ
	if(_tempRTC_Flag & C_RTC_4KHz_Flag)
#else
	if(_tempRTC_Flag & C_RTC_16KHz_Flag)
#endif
	{
#if (_SYS_TIMEBASE==TIMEBASE_250us)||(_SYS_TIMEBASE==TIMEBASE_62us)
#if	TIMEBASE_COUNT>1
		--TimebaseCount;
		if(TimebaseCount==0){
			TimebaseCount=TIMEBASE_COUNT;
			TIMEBASE_IRAM_ServiceLoop();
		}
#else
		TIMEBASE_IRAM_ServiceLoop();
#endif
#endif

	}
	if(_tempRTC_Flag & C_RTC_1KHz_Flag)
	{
#if _SYS_TIMEBASE==TIMEBASE_1000us
		TIMEBASE_IRAM_ServiceLoop();
#endif
	}

}
#endif //#if OPTION_IRAM
#endif //#if _EF_SERIES && ((_ADPCM_RECORD && _ADPCM_RECORD_ERASING) || (_SBC_RECORD && _SBC_RECORD_ERASING))
