/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "include.h"
#include "nx1_dac.h"
#include "nx1_gpio.h"
#include "nx1_timer.h"


#if (_WAVE_ID_MODULE)

#if !_EF_SERIES
#if (OPTION_HIGH_FREQ_CLK!=32000000)
#error "WaveID only support high clock frequency 32MHz! "
#endif
#endif
	
#if(_UART_MODULE&&_DEBUG_MODE)
#include "debug.h"
#define _WID_PRINT				0	//ENABLE
#endif


/* User Define **************************************************************/
#ifndef _WID_CMD_HIT_LEVEL
#define _WID_CMD_HIT_LEVEL		0 	// a) Command support number with hit rate.
									// b) Default is 0.
#endif
#define _WID_TX_REPEAT_TIMES	1	// a) WID_Tx repeat times, 500mS at a time.
									// b) Default is 1.
/****************************************************************************/

#if 	_WID_CMD_HIT_LEVEL==0		//Hit rate:95% (Default)
#define _WID_ALL_CMD_NUM		12
#elif	_WID_CMD_HIT_LEVEL==1		//Hit rate:90%
#define _WID_ALL_CMD_NUM		19
#elif	_WID_CMD_HIT_LEVEL==2		//Hit rate:85%
#define _WID_ALL_CMD_NUM		30
#elif	_WID_CMD_HIT_LEVEL==3		//Hit rate:80%
#define _WID_ALL_CMD_NUM		38
#else 
#error "Setting range is 0~3."
#endif

const S8 WID_CmdTab[][6]	=	{
#if	_WID_CMD_HIT_LEVEL>=0 		//hit rate:95%
	{1,	0,	1,	2,	3,	0},		//cmd18=>cmd0
	{3,	1,	3,	2,	3,	0},		//cmd50=>cmd1
	{0,	1,	2,	1,	2,	0},		//cmd4 =>cmd2
	{1,	0,	2,	1,	3,	1}, 	//cmd19=>cmd3
	{1,	2,	0,	1,	2,	0},		//cmd22=>cmd4
	{3,	1,	0,	1,	3,	1}, 	//cmd47=>cmd5
	{3,	0,	2,	1,	2,	3},		//cmd43=>cmd6
	{0,	1,	2,	0,	1,	2},		//cmd3 =>cmd7
	{0,	1,	2,	3,	2,	3},		//cmd5 =>cmd8
	{1,	0,	1,	3,	0,	2},		//cmd17=>cmd9
	{1,	0,	3,	1,	2,	0},		//cmd21=>cmd10
	{1,	2,	0,	3,	2,	3}, 	//cmd23=>cmd11
#endif	

#if	_WID_CMD_HIT_LEVEL>=1 		//hit rate:90%
	{1,	3,	0,	3,	1,	0}, 	//cmd24=>cmd12
	{3,	0,	2,	3,	2,	0},		//cmd44=>cmd13
	{1,	3,	2,	1,	3,	2},		//cmd27=>cmd14
	{0,	3,	1,	0,	1,	2},		//cmd14=>cmd15
	{3,	0,	1,	3,	1,	0},		//cmd42=>cmd16
	{0,	2,	1,	0,	2,	1},		//cmd9 =>cmd17
	{0,	3,	2,	3,	1,	3},		//cmd16=>cmd18	
#endif

#if	_WID_CMD_HIT_LEVEL>=2 		//hit rate:85%
	{0,	1,	0,	1,	0,	1}, 	//cmd0 =>cmd19
	{0,	3,	1,	3,	2,	3}, 	//cmd15=>cmd20
	{1,	0,	3,	0,	1,	2},		//cmd20=>cmd21
	{1,	3,	1,	0,	3,	0},		//cmd25=>cmd22
	{3,	1,	2,	3,	1,	3},		//cmd48=>cmd23
	{3,	2,	0,	1,	3,	2},		//cmd51=>cmd24
	{0,	1,	0,	2,	3,	0},		//cmd2 =>cmd25
	{3,	0,	1,	0,	2,	1},		//cmd41=>cmd26
	{3,	0,	3,	2,	0,	3},		//cmd46=>cmd27
	{3,	2,	3,	1,	0,	2},		//cmd55=>cmd28	
	{3,	2,	3,	0,	3,	0},		//cmd56=>cmd29
#endif															

#if	_WID_CMD_HIT_LEVEL>=3 		//hit rate:80%
	{3,	2,	1,	2,	1,	2},		//cmd53=>cmd30
	{0,	2,	1,	3,	1,	0},		//cmd10=>cmd31
	{3,	1,	3,	1,	0,	1},		//cmd49=>cmd32
	{2,	3,	1,	2,	0,	3},		//cmd37=>cmd33
	{3,	2,	0,	2,	0,	3},		//cmd52=>cmd34
	{0,	1,	0,	3,	0,	2},		//cmd1 =>cmd35
	{0,	2,	3,	1,	3,	2}, 	//cmd11=>cmd36
	{1,	3,	1,	3,	0,	1},		//cmd26=>cmd37
#endif

};

#define _WID_FreqType_NUM		4
//TXD
#if _EF_SERIES
#define _WID_TX_UP_SAMPLES		_DAC_UP_SAMPLE_POINT
#else
#define _WID_TX_UP_SAMPLES		4
#endif
#if _WID_TX_UP_SAMPLES!=4
#error "WaveID only support _DAC_UP_SAMPLE_POINT(4)! "
#endif
#define _WID_TX_SAMPLES_IN_WAVE 2//only support 2 with ITP
#define _WID_TX_FREQ_NUM		10
#define _WID_TX_TONE_PEAK		25000
#define _WID_TX_TONE_SIDE		(U16)(_WID_TX_TONE_PEAK*0.707)
#define _WID_TX_SINE_TIMES		8
#define _WID_TX_FREQ_HOLD_TIME	40//ms, fade in don't over it
#define _WID_TX_FADE_IN_STEP	125
#define _WID_TX_FADE_OUT_STEP	125
#define _WID_TX_RAMP_UP_STEP	1
#define _WID_TX_RAMP_DN_STEP	1
//RXD
#define _WID_SAMPLE_RATE		SAMPLE_RATE_48000
#define _WID_HEADER_FREQ_NUM	4
#define _WID_DATA_FREQ_NUM		(_WID_TX_FREQ_NUM - _WID_HEADER_FREQ_NUM)

#define DC_REMOVE				1

const S8 WID_HeadTab[_WID_HEADER_FREQ_NUM]={0, 1, 3, 2};	
													
const S8 (*widRX_TabPtr)[_WID_DATA_FREQ_NUM];
static U8 WID_TxDataTemp[_WID_TX_FREQ_NUM]={0};
static S8 widRx_Buf[_WID_DATA_FREQ_NUM]={0};
															
#define WID_FREQ_00				17062.5
#define WID_FREQ_01				17437.5
#define WID_FREQ_02				17812.5
#define WID_FREQ_03				18187.5

#define TONE_FQ_00				(U32)(WID_FREQ_00*_WID_TX_SAMPLES_IN_WAVE)
#define TONE_FQ_01				(U32)(WID_FREQ_01*_WID_TX_SAMPLES_IN_WAVE)
#define TONE_FQ_02				(U32)(WID_FREQ_02*_WID_TX_SAMPLES_IN_WAVE)
#define TONE_FQ_03				(U32)(WID_FREQ_03*_WID_TX_SAMPLES_IN_WAVE)	

#define FSYS 					OPTION_HIGH_FREQ_CLK
#define TONE_TMR_CNT(tone)		(U16)((FSYS + (tone*_WID_TX_UP_SAMPLES)/2)/(tone*_WID_TX_UP_SAMPLES))
#define Fq0						(TONE_TMR_CNT(TONE_FQ_00)-1)
#define Fq1						(TONE_TMR_CNT(TONE_FQ_01)-1)
#define Fq2						(TONE_TMR_CNT(TONE_FQ_02)-1)
#define Fq3						(TONE_TMR_CNT(TONE_FQ_03)-1)	
													
const U16 wid_cmd_tableB[_WID_FreqType_NUM] = {Fq0, Fq1, Fq2, Fq3};

static const S16 IIR_HPF17p9Hz_48k_8P2[13] = {
 //* HPF Fc = 17.9KHz, SR 48000, 8 Poles, *//
 //*Enhanced*//
 16384,
 22153,
 15008,
 2310,
 -4619,
 2310,
 16384,
 22153,
 15008,
 2310,
 -4619,
 2310,
 14,
};

static const S16 IIR_LPF17kHz_48k_6P[13] = {
 //* LPF Fc = 17KHz, SR 48000, 6 Poles, *//
 //*Enhanced*//
16384,
18370,
14922,
4081,
-4773,
4081,
16384,
18370,
14922,
4081,
-4773,
4081,
14,
};	

static volatile struct AUDIO_BUF_CTL{
    S32 process_count;	//decode/encode count
    S32 fifo_count;	    //decode/encode count
    //U32 samples;	    //samples     
    //U16 sample_rate;  //sample_rate
    U16 out_size;       //audio frame size
    //U16 in_size;      //data frame size
    U8 state;           //channel state
    U8 shadow_state;    //channel shadow state
    U8 out_count;
    U8 index;
}AudioBufCtl;

//static U16 AudioVol;
//static U8  ChVol;

typedef struct{
    S16 *buf;
    U16 size;
    U16 offset;
}BUF_CTL_T;   

static volatile  BUF_CTL_T BufCtl;

U8 recive_send_flag=0;	//0 RECIVE  1 SEND
U8 recive_start=0;
U8 tone_init=0;	
U8 WID_State=0;

typedef enum {
    _FADE_OUT,
    _FADE_IN,
    _FREQ_CARRIER,
    _FREQ_RAMP,
} _TXD_STATE;
static struct {
	_TXD_STATE  txdState;//0~3
	U32		    baseTime;
	S16		    dat_FadeAmp;//+-20000
	S16			dat_root;//+-14140
    S16         dat_FastTab[_WID_TX_SAMPLES_IN_WAVE];
	U16		    targetTone;//max 351 @48MHz w/ 17062.5Hz tone
    U16		    currentTone;//max 351 @48MHz w/ 17062.5Hz tone
	U8		    cntFreqNum;//0~_WID_TX_FREQ_NUM(10)
    U8		    cnt_FreqChg;//0~_WID_TX_SINE_TIMES(8)
    U8		    cntRepeat;//0~_WID_TX_REPEAT_TIMES
    U8          sineWavePhase;//0~_WID_TX_SAMPLES_IN_WAVE-1
    S8			hw_ch;
}WIDtxCtrl;

/* Dynamic Allocate RAM-------------------------------------------------------*/
#if _USE_HEAP_RAM
static S16	*AudioBuf=NULL;	
Wave_IDBuf* waveidbuf = NULL;
FFTctl* waveifftctl= NULL;
#if DC_REMOVE
static S32 	*RunFilterBuf=NULL;
#endif
/* Fixed RAM  ---------------------------------------------------------------*/
#else
static S16	AudioBuf[WID_BUF_SIZE_01];
Wave_IDBuf _waveidbuf;
Wave_IDBuf* waveidbuf  = &_waveidbuf;
FFTctl _waveifftctl;
FFTctl* waveifftctl  = &_waveifftctl;
#if DC_REMOVE
static S32  RunFilterBuf[WID_BUF_SIZE_05];
#endif		
#endif

U16	waveid_buf_size;
U8	waveid_fftctl_size;

/* Extern Functions ----------------------------------------------------------*/
extern void CB_WaveID_DacCmd(CH_TypeDef *chx,U8 cmd);
extern void CB_WaveID_WriteDac(U8 size,S16 *psrc16);
extern void CB_WaveID_InitAdc(U16 sample_rate);
extern void CB_WaveID_InitDac(CH_TypeDef *chx,U32 sample_rate);
extern void CB_WaveID_AdcCmd(U8 cmd);

//------------------------------------------------------------------//
// Allocate WaveID memory
// Parameter: 
//          func: Choose Allocate function
// return value:
//          NONE
//------------------------------------------------------------------// 
#if _USE_HEAP_RAM
static void WaveID_AllocateMemory(U16 func)
{
    if(AudioBuf==NULL && (func&MEMORY_ALLOCATE_PLAY))    
    {
        AudioBuf	= (S16 *)MemAlloc(WID_BUF_SIZE_01<<1);

		if(waveidbuf==NULL)
		{
			waveidbuf=(Wave_IDBuf*)MemAlloc(sizeof(Wave_IDBuf));
		}
		if(waveifftctl==NULL)
		{
			waveifftctl=(FFTctl*)MemAlloc(sizeof(FFTctl));
		}
	#if DC_REMOVE
		RunFilterBuf= (S32 *)MemAlloc(WID_BUF_SIZE_05<<2);
	#endif	
    }
		
}
//------------------------------------------------------------------//
// Free WaveID memory
// Parameter: 
//          func: Choose Allocate function
// return value:
//          NONE
//------------------------------------------------------------------// 
static void WaveID_FreeMemory(U16 func)
{
    if(AudioBuf!=NULL && (func&MEMORY_ALLOCATE_PLAY))    
    {
        MemFree(AudioBuf);
        AudioBuf=NULL;

		if(waveidbuf!=NULL)
		{
			MemFree(waveidbuf);
			waveidbuf=NULL;
		}	
		if(waveifftctl!=NULL)
		{
			MemFree(waveifftctl);
			waveifftctl=NULL;
		}	
	#if DC_REMOVE
		MemFree(RunFilterBuf);
		RunFilterBuf=NULL;		
	#endif	
	}
}
#endif
//------------------------------------------------------------------//
// WaveID Dac ISR
// Parameter: 
//          size: fifo fill size
// return value:
//          NONE
//------------------------------------------------------------------//
void WaveID_DacIsr(U8 size, S16* _dacBuf)
{	
	if(AudioBufCtl.shadow_state!=STATUS_PLAYING) return;

    U8  _dacDataCount = 0;
    while(size--) {
        _dacBuf[_dacDataCount] = WIDtxCtrl.dat_FastTab[WIDtxCtrl.sineWavePhase];
        ++_dacDataCount;
        WIDtxCtrl.sineWavePhase = (WIDtxCtrl.sineWavePhase + 1)%_WID_TX_SAMPLES_IN_WAVE;
        
        if(WIDtxCtrl.sineWavePhase) continue;
        
        switch(WIDtxCtrl.txdState) {
            case _FADE_OUT:
                WIDtxCtrl.dat_FadeAmp -= _WID_TX_FADE_OUT_STEP;
                if(WIDtxCtrl.dat_FadeAmp<=0) {//Fade out complete
                    WIDtxCtrl.dat_FadeAmp = 0;
                    recive_send_flag = 0;
                    AudioBufCtl.state = STATUS_STOP;
                    AudioBufCtl.shadow_state = AudioBufCtl.state;
                    DacAssigner_unRegist(kWID);//@interrupt
                }
                WIDtxCtrl.dat_root = (WIDtxCtrl.dat_FadeAmp*181)>>8;
#if _WID_TX_SAMPLES_IN_WAVE==8
                WIDtxCtrl.dat_FastTab[1] = WIDtxCtrl.dat_FadeAmp;
                WIDtxCtrl.dat_FastTab[0] = WIDtxCtrl.dat_FastTab[2] = WIDtxCtrl.dat_root;
                //WIDtxCtrl.dat_FastTab[3] = WIDtxCtrl.dat_FastTab[7] = 0;
                WIDtxCtrl.dat_FastTab[4] = WIDtxCtrl.dat_FastTab[6] = -WIDtxCtrl.dat_root;
                WIDtxCtrl.dat_FastTab[5] = -WIDtxCtrl.dat_FadeAmp;
#elif _WID_TX_SAMPLES_IN_WAVE==4
                WIDtxCtrl.dat_FastTab[0] = WIDtxCtrl.dat_FadeAmp;
                //WIDtxCtrl.dat_FastTab[1] = WIDtxCtrl.dat_FastTab[3] = 0;
                WIDtxCtrl.dat_FastTab[2] = -WIDtxCtrl.dat_FadeAmp;
#elif _WID_TX_SAMPLES_IN_WAVE==2
                WIDtxCtrl.dat_FastTab[0] = WIDtxCtrl.dat_FadeAmp;
                WIDtxCtrl.dat_FastTab[1] = -WIDtxCtrl.dat_FadeAmp;
#endif
                break;
            case _FADE_IN:
                WIDtxCtrl.dat_FadeAmp += _WID_TX_FADE_IN_STEP;
                if(WIDtxCtrl.dat_FadeAmp>=_WID_TX_TONE_PEAK) {//Fade in complete
                    WIDtxCtrl.dat_FadeAmp = _WID_TX_TONE_PEAK;
                    WIDtxCtrl.baseTime = SysTick;
                    WIDtxCtrl.targetTone = wid_cmd_tableB[WID_TxDataTemp[WIDtxCtrl.cntFreqNum]];
                    WIDtxCtrl.currentTone = WIDtxCtrl.targetTone;
                    WIDtxCtrl.txdState = _FREQ_CARRIER;
                }
                WIDtxCtrl.dat_root = (WIDtxCtrl.dat_FadeAmp*181)>>8;
#if _WID_TX_SAMPLES_IN_WAVE==8
                WIDtxCtrl.dat_FastTab[1] = WIDtxCtrl.dat_FadeAmp;
                WIDtxCtrl.dat_FastTab[0] = WIDtxCtrl.dat_FastTab[2] = WIDtxCtrl.dat_root;
                //WIDtxCtrl.dat_FastTab[3] = WIDtxCtrl.dat_FastTab[7] = 0;
                WIDtxCtrl.dat_FastTab[4] = WIDtxCtrl.dat_FastTab[6] = -WIDtxCtrl.dat_root;
                WIDtxCtrl.dat_FastTab[5] = -WIDtxCtrl.dat_FadeAmp;
#elif _WID_TX_SAMPLES_IN_WAVE==4
                WIDtxCtrl.dat_FastTab[0] = WIDtxCtrl.dat_FadeAmp;
                //WIDtxCtrl.dat_FastTab[1] = WIDtxCtrl.dat_FastTab[3] = 0;
                WIDtxCtrl.dat_FastTab[2] = -WIDtxCtrl.dat_FadeAmp;
#elif _WID_TX_SAMPLES_IN_WAVE==2
                WIDtxCtrl.dat_FastTab[0] = WIDtxCtrl.dat_FadeAmp;
                WIDtxCtrl.dat_FastTab[1] = -WIDtxCtrl.dat_FadeAmp;
#endif
                break;
            case _FREQ_CARRIER:
                if((SysTick-_WID_TX_FREQ_HOLD_TIME) >= WIDtxCtrl.baseTime) {
                	WIDtxCtrl.baseTime = SysTick;
                    ++WIDtxCtrl.cntFreqNum;
                    if(WIDtxCtrl.cntFreqNum>=_WID_TX_FREQ_NUM) {
                        WIDtxCtrl.cntFreqNum = 0;
                        ++WIDtxCtrl.cntRepeat;
                        if(WIDtxCtrl.cntRepeat>=_WID_TX_REPEAT_TIMES) {
                            WIDtxCtrl.cntRepeat = 0;
                            WIDtxCtrl.txdState = _FADE_OUT;
                            break;
                        }
                    }
                    WIDtxCtrl.targetTone = wid_cmd_tableB[WID_TxDataTemp[WIDtxCtrl.cntFreqNum]];
                    if(WIDtxCtrl.currentTone!=WIDtxCtrl.targetTone) {
                        WIDtxCtrl.txdState = _FREQ_RAMP;
                    }
                }
                break;
            case _FREQ_RAMP:
                    ++WIDtxCtrl.cnt_FreqChg;
                    if(WIDtxCtrl.cnt_FreqChg>=_WID_TX_SINE_TIMES) {
                        WIDtxCtrl.cnt_FreqChg = 0;
                        if(WIDtxCtrl.currentTone>WIDtxCtrl.targetTone) {
                            WIDtxCtrl.currentTone -= _WID_TX_RAMP_UP_STEP;
                            if(WIDtxCtrl.currentTone<WIDtxCtrl.targetTone) {
                                WIDtxCtrl.currentTone = WIDtxCtrl.targetTone;
                            }
                        } else if(WIDtxCtrl.currentTone<WIDtxCtrl.targetTone) {
                            WIDtxCtrl.currentTone += _WID_TX_RAMP_DN_STEP;
                            if(WIDtxCtrl.currentTone>WIDtxCtrl.targetTone) {
                                WIDtxCtrl.currentTone = WIDtxCtrl.targetTone;
                            }
                        } else {
                            WIDtxCtrl.txdState = _FREQ_CARRIER;
                        }
                        pHwCtl[WIDtxCtrl.hw_ch].p_tmr->Data = WIDtxCtrl.currentTone;
                    }
                break;
        }
    }
    CB_WaveID_WriteDac(_dacDataCount,_dacBuf);
}
//------------------------------------------------------------------//
// WaveID Adc ISR
// Parameter: 
//          size: data-buffer fill size 
//       pdest32: data-buffer address
// return value:
//          NONE
//------------------------------------------------------------------// 
void WaveID_AdcIsr(U8 size, U32 *pdest32){
    S16 *pdst16;
    U8 i;
    S16 temp;
	
    if(AudioBufCtl.state==STATUS_RECORDING){
        //GPIOC->Data ^=0x01;
      	//GPIOC->Data &=~(1<<1);
        temp=BufCtl.size-BufCtl.offset;
        if(size>temp){
            size=temp;
        }   
        //dprintf("size,%d\r\n",size);
        AudioBufCtl.fifo_count+=size;   
        pdst16=(S16 *)(BufCtl.buf+BufCtl.offset);
        
        for(i=0;i<size;i++){
            temp=((*pdest32++)-32768);   
            *pdst16++=temp;       
        }   
        BufCtl.offset+=size;
          
        if(BufCtl.offset==BufCtl.size){
            BufCtl.offset=0;    
        }
		//GPIOC->Data |=(1<<1);		
    }             
}

//------------------------------------------------------------------//
// Encode one frame
// Parameter: 
//          NONE
// return value:
//          0:not ready
//			others: result after encoding 
//------------------------------------------------------------------// 
static S8* EncodeFrame(void)
{
	S16 *audio_buf;
	S16 *filter_buf;
	U8 RdyFlag=0;
	U16 i=0;
	
	//GPIOC->Data &= ~(1<<0);
	audio_buf=AudioBuf+AudioBufCtl.out_size*AudioBufCtl.index;
	filter_buf=audio_buf;
	//dprintf("%d,%d,%d\n\r ",audio_buf[0],audio_buf[1],audio_buf[2]);
	AudioBufCtl.index++;
	if(AudioBufCtl.index==AudioBufCtl.out_count){
		AudioBufCtl.index=0;   
    }
	
	for(i=0;i<AudioBufCtl.out_size;i++) {		
	#if DC_REMOVE
		RunFilter((S16*)filter_buf, RunFilterBuf, DCRemove);
	#endif

		WID_RunFilter((S16*)filter_buf, (S32*)waveidbuf->widReg_Buf, (S16*)IIR_HPF17p9Hz_48k_8P2);//faced	
		filter_buf++;
	}
	
	RdyFlag = Wave_IDProcess((S16*)audio_buf, waveidbuf, waveifftctl);
	
	if(RdyFlag) {
#if _WID_PRINT
		for(i=0;i<10;i++)
			dprintf("%d,",waveidbuf.widFrq_Buf[i]);			
		dprintf("\n\r");		
#endif	
		for(i=0;i<_WID_DATA_FREQ_NUM;i++) {	
			widRx_Buf[i]=waveidbuf->widFrq_Buf[i+_WID_HEADER_FREQ_NUM];		
		}
		memset((void *)waveidbuf->widFrq_Buf, 0, WID_BUF_SIZE_03);			
	}

	
	AudioBufCtl.process_count+=AudioBufCtl.out_size;
	//GPIOC->Data |=  (1<<0);
	if(RdyFlag) 
		return widRx_Buf;
	else
		return 0;			
} 
//------------------------------------------------------------------//
// WaveID init
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void WaveID_Init(void){
	
	U8 i;
	
    //ChVol=CH_VOL_15;
	
	WID_State=DISABLE;
	
#if _USE_HEAP_RAM
		if(waveidbuf==NULL)
		{
			waveidbuf=(Wave_IDBuf*)MemAlloc(sizeof(Wave_IDBuf));
		}
		if(waveifftctl==NULL)
		{
			waveifftctl=(FFTctl*)MemAlloc(sizeof(FFTctl));
		}
#endif
	waveid_buf_size = ((sizeof(Wave_IDBuf)+3)>>2)<<2;
	waveid_fftctl_size = ((sizeof(FFTctl)+3)>>2)<<2;
	memset(waveidbuf, 0, waveid_buf_size);		
	memset(waveifftctl, 0, waveid_fftctl_size);	
	
	for(i=0;i<=6;i++)
		waveidbuf->widStack_Buf[i]=-1;
		
	for(i=0;i<=17;i++)
		waveidbuf->widEc_Buf[i]=-1;
	
	for(i=36;i<=38;i++)
		waveidbuf->widEc_Buf[i]=-1;
		
	waveidbuf->LowerLimitThreshold = 3500;
	waveidbuf->FrameSize = 256;
	Wave_IDInit(waveidbuf, waveifftctl);	
	
}
//------------------------------------------------------------------//
// Start record voice
// Parameter: 
//          sample_rate:sample rate
// return value:
//          0:ok
//          others:fail
//------------------------------------------------------------------// 
U8 WaveID_InitRxD(U16 sample_rate)
{
	U8 i;
#if _USE_HEAP_RAM
	//AUDIO_ServiceLoop();
	WaveID_AllocateMemory(MEMORY_ALLOCATE_PLAY);
#endif
    memset((void *)&AudioBufCtl, 0, sizeof(AudioBufCtl));

    //AudioBufCtl.sample_rate=sample_rate;
    AudioBufCtl.out_size=WID_OUT_SIZE;
    //AudioBufCtl.in_size=(WID_OUT_SIZE<<1);
    AudioBufCtl.out_count=WID_BUF_SIZE_01/AudioBufCtl.out_size;
	
    BufCtl.offset=0;   
    BufCtl.size=AudioBufCtl.out_size*AudioBufCtl.out_count;
    BufCtl.buf=AudioBuf;

	memset((void *)AudioBuf, 0, WID_BUF_SIZE_01<<1);
	waveid_buf_size = ((sizeof(Wave_IDBuf)+3)>>2)<<2;
	waveid_fftctl_size = ((sizeof(FFTctl)+3)>>2)<<2;
	memset(waveidbuf, 0, waveid_buf_size);		
	memset(waveifftctl, 0, waveid_fftctl_size);	
	for(i=0;i<=6;i++)
		waveidbuf->widStack_Buf[i]=-1;
		
	for(i=0;i<=17;i++)
		waveidbuf->widEc_Buf[i]=-1;
	
	for(i=36;i<=38;i++)
		waveidbuf->widEc_Buf[i]=-1;
		
	waveidbuf->LowerLimitThreshold = 3500;
	waveidbuf->FrameSize = 256;
	Wave_IDInit(waveidbuf, waveifftctl);
	
#if DC_REMOVE
	memset((void *)RunFilterBuf, 0, WID_BUF_SIZE_05<<2);
#endif
	
    CB_WaveID_InitAdc(sample_rate);   
    
    //AudioBufCtl.state=  STATUS_PLAYING;
	AudioBufCtl.state=  STATUS_RECORDING;
    AudioBufCtl.shadow_state=AudioBufCtl.state;      
    CB_WaveID_AdcCmd(ENABLE);
    return 0;
}

//------------------------------------------------------------------//
// Stop Send voice
// Parameter:
//			NONE
// return value:
//			NONE
//------------------------------------------------------------------//
void WaveID_StopTxD(void)
{
    if(AudioBufCtl.shadow_state==STATUS_PLAYING) {
        AudioBufCtl.state=STATUS_STOP;
        WIDtxCtrl.txdState = _FADE_OUT;

        if(!DacMixerRamp_IsBias()) {
            GIE_DISABLE();
            WIDtxCtrl.dat_FadeAmp = 0;
            recive_send_flag = 0;
            AudioBufCtl.shadow_state = AudioBufCtl.state;
            DacAssigner_unRegist(kWID);
            GIE_ENABLE();
        }

        while(AudioBufCtl.shadow_state==STATUS_PLAYING){
            AUDIO_ServiceLoop();
        }
    }
}
//------------------------------------------------------------------//
// Start WaveID Function
// Parameter:
//			NONE
// return value:
//			NONE
//------------------------------------------------------------------//
void WaveID_Start(void)
{
	recive_start=0;
	WID_State=ENABLE;
}

//------------------------------------------------------------------//
// Stop WaveID Function
// Parameter:
//			NONE
// return value:
//			NONE
//------------------------------------------------------------------//
void WaveID_Stop(void)
{	
    WaveID_StopTxD();
#if _USE_HEAP_RAM							
    WaveID_FreeMemory(MEMORY_ALLOCATE_PLAY);	
#endif	

	AudioBufCtl.state=STATUS_STOP;
	AudioBufCtl.shadow_state=AudioBufCtl.state;
	
	WID_State=DISABLE;
}

//------------------------------------------------------------------//
// Stop record voice
// Parameter:
//			NONE
// return value:
//			NONE
//------------------------------------------------------------------//
void WaveID_StopRxD(void)
{	
	CB_WaveID_AdcCmd(DISABLE);

#if _USE_HEAP_RAM							
    WaveID_FreeMemory(MEMORY_ALLOCATE_PLAY);	
#endif										

}
//------------------------------------------------------------------//
// Get current status
// Parameter: 
//          NONE
// return value:
//          status:STATUS_PLAYING,STATUS_RECORDING,STATUS_STOP
//------------------------------------------------------------------// 
U8 WaveID_GetStatus(void){
    return AudioBufCtl.state;    
}
//------------------------------------------------------------------//
// Send WaveID Command
// Parameter: 
//          cmd: WaveID Command table index (0~n)
// return value:
//          None
//------------------------------------------------------------------// 
void WaveID_SendCMD(U8 cmd)
{
	U8 i;
	if(WID_State==ENABLE && cmd<_WID_ALL_CMD_NUM){	
#if 1		
        WaveID_StopTxD();
#endif	
		//WaveID_Stop();
		WaveID_StopRxD();
		//CB_WaveID_AdcCmd(DISABLE);

		GIE_DISABLE();
		S8 hw_ch = DacAssigner_Regist(kWID,WID_EXCLUSIVE);
		GIE_ENABLE();
		if(hw_ch<0){
			//dprintf("No play Invalid sr\r\n");
			return;
		}
		//dprintf("codec %d reg hw_ch %d\r\n",kWID,hw_ch);



		for(i=0 ; i<_WID_HEADER_FREQ_NUM ; i++)//copy head
		{
			WID_TxDataTemp[i]=WID_HeadTab[i];
		}
		for(i=0 ; i<_WID_DATA_FREQ_NUM ; i++)//copy cmd
		{
			WID_TxDataTemp[i+_WID_HEADER_FREQ_NUM]=WID_CmdTab[cmd][i];
		}
		recive_send_flag=1;
		recive_start=0;
		if(recive_send_flag)
		{
            memset((void *)&WIDtxCtrl, 0, sizeof(WIDtxCtrl));
            WIDtxCtrl.txdState = _FADE_IN;
            WIDtxCtrl.hw_ch = hw_ch;
			CB_WaveID_InitDac(pHwCtl[hw_ch].p_dac,TONE_FQ_00);
			pHwCtl[hw_ch].p_tmr->Data = Fq0;
			AudioBufCtl.state=STATUS_PLAYING;
			AudioBufCtl.shadow_state=AudioBufCtl.state;
			CB_WaveID_DacCmd(pHwCtl[hw_ch].p_dac,ENABLE);
		}
	}
}
//------------------------------------------------------------------//
// Service loop
// Parameter: 
//          NONE
// return value:
//			0:no command
//			others: command after encoding
//------------------------------------------------------------------//     
S8* WaveID_ServiceLoop(void)
{
    //if(AudioBufCtl.state==STATUS_PLAYING)
	if(AudioBufCtl.state==STATUS_RECORDING)
    {        
		if((AudioBufCtl.fifo_count-AudioBufCtl.process_count)>=AudioBufCtl.out_size) { 
			//PC0 verify
            return EncodeFrame();        
		}
    }   
    return 0;
}
//------------------------------------------------------------------//
// Receive WaveID Command
// Parameter: 
//          NONE
// return value:
//          -1 		: no command
//			others 	: command index 
//------------------------------------------------------------------//  
S8 WaveID_ReceiveCMD(void)
{
	S8 *rx_cmpr;
	U8 idx,sts=0;
	
	if(WID_State==ENABLE) {
	
		//if(recive_send_flag==0 && AUDIO_GetStatus_All()==0) {
		if(recive_send_flag==0 ) {	
		//if(AudioBufCtl.shadow_state==STATUS_RECORDING) {
			if(recive_start==0)	{
				WaveID_InitRxD(_WID_SAMPLE_RATE);
				recive_start=1;
			}
			
			rx_cmpr=WaveID_ServiceLoop();//it will return 1-d arrary
		#if 1
			if(rx_cmpr !=0) {
				widRX_TabPtr=WID_CmdTab;

				for(idx=0; idx<_WID_ALL_CMD_NUM; idx++) 
				{
					sts=memcmp( widRX_TabPtr, rx_cmpr, _WID_DATA_FREQ_NUM);
					if(sts!=0)
						widRX_TabPtr++;
					else
						return idx;	
				}
			}
			//else 
				//dprintf("loop out 0\n\r");
		#endif	
			
		}
	}
	return -1;
}

							
#endif //end (_WAVE_ID_MODULE)
