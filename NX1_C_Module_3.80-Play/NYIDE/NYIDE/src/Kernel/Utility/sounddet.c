#include "include.h"
#include "nx1_adc.h"
#include "nx1_gpio.h"
#include "debug.h"
#include "utility_define.h"
#include "nx1_timer.h"


#include <string.h>
//#define DEBUG_SOUND_DETECTION
#ifdef  DEBUG_SOUND_DETECTION
#define Dprintf dprintf
#else
#define Dprintf(...) 
#endif


#if _ADPCM_RECORD && _ADPCM_RECORD_ERASING && _ADPCM_RECORD_SOUND_TRIG
#if _ADPCM_RECORD_SAMPLE_RATE==SAMPLE_RATE_8000
#define VAD_TOTAL_SAMPLES   128
#elif _ADPCM_RECORD_SAMPLE_RATE==SAMPLE_RATE_10000
#define VAD_TOTAL_SAMPLES   160
#elif _ADPCM_RECORD_SAMPLE_RATE==SAMPLE_RATE_12000
#define VAD_TOTAL_SAMPLES   192
#elif _ADPCM_RECORD_SAMPLE_RATE==SAMPLE_RATE_16000
#define VAD_TOTAL_SAMPLES   256
#else
#define VAD_TOTAL_SAMPLES	1
#error "_ADPCM_RECORD_SAMPLE_RATE only supports 8000/10000/12000/16000Hz."
#endif
#else//_SOUND_DETECTION only
#ifndef PROCESS_SAMPLES_USER_DEFINED
#if SOUND_DETECTION_SAMPLE_RATE==SAMPLE_RATE_8000
#define SOUND_DETECTION_PROCESS_SAMPLES   128
#elif SOUND_DETECTION_SAMPLE_RATE==SAMPLE_RATE_10000
#define SOUND_DETECTION_PROCESS_SAMPLES   160
#elif SOUND_DETECTION_SAMPLE_RATE==SAMPLE_RATE_12000
#define SOUND_DETECTION_PROCESS_SAMPLES   192
#elif SOUND_DETECTION_SAMPLE_RATE==SAMPLE_RATE_16000
#define SOUND_DETECTION_PROCESS_SAMPLES   256
#else
#error "SOUND_DETECTION_SAMPLE_RATE only supports 8000/10000/12000/16000Hz."
#endif
#endif
#endif

#define CLB_ENABLE			1
#define CLB_INIT_CNT        25 
#define CLB_INTERVAL_CNT	62 //1sec
#define CLB

#define ABS(a) (((a) < 0) ? -(a) : (a))
#if !defined(DC_OFFSET_MINUS)
#define DC_OFFSET_MINUS		0
#endif

/* Global Word for Inline C-------------------------------------------------------*/
U16 g_sd_high_threshold;
U16 g_sd_low_threshold;
U16 g_sd_active_count;
U16 g_sd_mute_count;
#if DC_OFFSET_MINUS
S16 g_sd_dc_offset;
#endif

/* Dynamic Allocate RAM-------------------------------------------------------*/
static volatile struct{
    U16 average;
    U16 count;
    U8  state;
    U8  sts;            // 0=wait for sound detect, 1=sound detected and wait for sound mute, 2=sound muted
}VadCtl;
#if CLB_ENABLE
static volatile struct{
	U32 val;
	U16 min_val;
	U16 cnt;
	U16 init_cnt;
}ClbCtrl;
#endif
static U32 total_sum;
static U16 total_cnt;
static U8  average_first_flag;

extern void CB_SOUND_DETECTION_DetectSound(void);
extern void CB_SOUND_DETECTION_MuteSound(void);

#if _SOUND_DETECTION
#define SOUND_DETECTION_BUF_SIZE		SOUND_DETECTION_PROCESS_SAMPLES

static S16 sd_adc[SOUND_DETECTION_BUF_SIZE] ={ 0 };
static S16 sd_adc_tmp[SOUND_DETECTION_BUF_SIZE] ={ 0 };
static volatile struct{
    U16 adcBufOffset;   //0~256
    U8  state;          //STATUS_PLAYING,STATUS_STOP
    U8  process_flag;
}sdCtl;



//------------------------------------------------------------------//
// Sound Detection Start
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SOUND_DETECTION_Start(void)
{
	if(sdCtl.state==STATUS_PLAYING) return;
	SOUND_DETECTION_Init();
	InitAdc(SOUND_DETECTION_SAMPLE_RATE);
	ADC_Cmd(ENABLE); // HW initial
	sdCtl.state=STATUS_PLAYING;
}

//------------------------------------------------------------------//
// Sound Detection Stop
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SOUND_DETECTION_Stop()
{
    if(sdCtl.state==STATUS_STOP) return;

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
    ADC_Cmd(DISABLE); // HW initial
    TIMER_Cmd(ADC_TIMER,DISABLE);//to trigger ADC fifo
    sdCtl.state=STATUS_STOP;
}

//------------------------------------------------------------------//
// Sound Detection Adc Isr
// Parameter: 
//          size: data-buffer fill size
//       pdest32: data-buffer address
// return value:
//          none
//------------------------------------------------------------------//
void SOUND_DETECTION_AdcIsr(U8 size, U32 *pdest32)
{
    if(sdCtl.state==STATUS_STOP) return;
    U16 i;
    for(i = 0; i < size ; i++) {
        sd_adc[sdCtl.adcBufOffset++]=((*pdest32++)-32768);
        if (sdCtl.adcBufOffset == SOUND_DETECTION_BUF_SIZE) {
            memcpy(sd_adc_tmp,sd_adc,sizeof(S16)*SOUND_DETECTION_BUF_SIZE);
            sdCtl.process_flag=1;
            sdCtl.adcBufOffset = 0;
        }
    }
}
#endif

//------------------------------------------------------------------//
// Voice activity detection process
// Parameter:
//          audio_buf: audio buffer
//          samples: samples to process
// return value:
//          NONE
//------------------------------------------------------------------//
void SOUND_DETECTION_Process(S16 *audio_buf,U16 samples)
{
    U16 i;
    U32 sum;

#if DC_OFFSET_MINUS
	U8 verBflag = isFDBverB();
#endif
    // collect samples
    for(i=0;i<samples;i++)
    {
	#if DC_OFFSET_MINUS
		S16 audio_temp = audio_buf[i];
		if(verBflag){
			if(AdcEnFlag)
				audio_temp = 0;
			else 
				audio_temp -= g_sd_dc_offset;
			audio_buf[i] = audio_temp;		// rewrite the data
		}
        total_sum += ABS(audio_temp);
	#else	
		total_sum += ABS(audio_buf[i]);
	#endif
        total_cnt++;
    }
#if _ADPCM_RECORD && _ADPCM_RECORD_ERASING && _ADPCM_RECORD_SOUND_TRIG
    if(total_cnt<VAD_TOTAL_SAMPLES)
    {
        return;
    }
    total_sum /= VAD_TOTAL_SAMPLES;
#else//_SOUND_DETECTION only
    total_sum /= samples;
#endif

    sum=total_sum;
    total_sum = total_cnt = 0;

#if CLB_ENABLE
	
	if(CLB_INIT_CNT>ClbCtrl.init_cnt)
	{
		ClbCtrl.init_cnt++;
		return;		
	}
	
	if(0==ClbCtrl.cnt++)
	{
		ClbCtrl.val = sum;
	}
	else if(CLB_INTERVAL_CNT>ClbCtrl.cnt)
	{
		/*if(ClbCtrl.val > sum)
		{
			ClbCtrl.val = sum;
		}*/
		ClbCtrl.val += sum;
	}
	else
	{
		ClbCtrl.val += sum;
		ClbCtrl.min_val = ClbCtrl.val/ClbCtrl.cnt;
		ClbCtrl.cnt = 0;
        Dprintf("SOUND_DETECTION min %d, time = %d\r\n",ClbCtrl.min_val, SysTick);
	}
#endif
    //VadCtl.average=VadCtl.average*0.9+sum*0.1;
    //Dprintf("Vavg %d sum %d\r\n",VadCtl.average,sum);

    if(VadCtl.state==0)
    {
        VadCtl.average=VadCtl.average*0.9+sum*0.1;
        if(average_first_flag<10)	
        {
        	average_first_flag++;
        	return;
        }
        if(sum>VadCtl.average)
        {
            sum=sum-VadCtl.average;
        }
        else
        {
            sum=0;
        }
        if(sum>g_sd_high_threshold)
        {
        	Dprintf("Aavg %d sum %d\r\n",VadCtl.average,sum);
            VadCtl.count++;
        }
        else
        {
            VadCtl.count=0;
        }
        if(VadCtl.count>=g_sd_active_count)
        {
            // Detect Sound
            VadCtl.state=1;
            VadCtl.count=0;
            VadCtl.sts++;
            CB_SOUND_DETECTION_DetectSound();
			Dprintf("DetectSound\r\n");
        }
    }
    else
    {
        if(sum<VadCtl.average)
        {
            VadCtl.count++;
        }
        else
        {
            sum=sum-VadCtl.average;
            if(sum>g_sd_low_threshold)
            {
                VadCtl.count=0;
				Dprintf("MuteReStart\r\n");
            }
            else
            {
            	Dprintf("Mavg %d sum %d\r\n",VadCtl.average,sum);
                VadCtl.count++;
            }
        }

        if(VadCtl.count>=g_sd_mute_count)
        {
            // Mute Sound
            VadCtl.state=2;
            VadCtl.count=0;
            VadCtl.sts++;
            CB_SOUND_DETECTION_MuteSound();
			Dprintf("MuteSound\r\n");
        }
    }
}
//------------------------------------------------------------------//
// Voice activity detection int
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SOUND_DETECTION_Init(void)
{
#if CLB_ENABLE==0
    U8 verBflag = isFDBverB();
#endif
#if _SPI_MODULE
    g_sd_high_threshold = XIP_SOUND_DETECTION_Parameters(0);
    g_sd_low_threshold  = XIP_SOUND_DETECTION_Parameters(1);
    g_sd_active_count   = XIP_SOUND_DETECTION_Parameters(2);
    g_sd_mute_count 	= XIP_SOUND_DETECTION_Parameters(3);
#if DC_OFFSET_MINUS
	g_sd_dc_offset      = XIP_SOUND_DETECTION_Parameters(4);
#endif
#else 
    g_sd_high_threshold = SOUND_DETECTION_HIGH_THRESHOLD;
    g_sd_low_threshold  = SOUND_DETECTION_LOW_THRESHOLD;
    g_sd_active_count   = SOUND_DETECTION_ACTIVE_COUNT;
    g_sd_mute_count 	= SOUND_DETECTION_MUTE_COUNT;
#if DC_OFFSET_MINUS
	g_sd_dc_offset 		= DC_OFFSET;
#endif
#endif	
    total_sum = total_cnt = 0;
#if CLB_ENABLE
    memset((void *)&VadCtl, 0, sizeof(VadCtl));
    VadCtl.average=ClbCtrl.min_val;
    ClbCtrl.init_cnt = 0;    
    Dprintf("SOUND_DETECTION Init avg %d\r\n",VadCtl.average);
#else
    memset((void *)&VadCtl, 0, sizeof(VadCtl));
#if _IC_BODY==0x12A44
    if(verBflag)
    {
        VadCtl.average=19000;
    }
    else
    {
        VadCtl.average=200;
    } 
#else
    VadCtl.average=19000;
#endif
#endif
}
//------------------------------------------------------------------//
// Get VAD status
// Parameter:
//          NONE
// return value:
//          0: wait for sound detect
//          1: sound detected and wait for sound mute
//          2: sound muted
//------------------------------------------------------------------//
U8 SOUND_DETECTION_GetSts(void)
{
    return VadCtl.sts;
}

#if _SOUND_DETECTION
//------------------------------------------------------------------//
// Sound Detection Get Status
// Parameter: 
//          NONE
// return value:
//          STATUS_STOP, STATUS_PLAYING
//------------------------------------------------------------------//
U8  SOUND_DETECTION_GetStatus(void)
{
    return sdCtl.state;
}

//------------------------------------------------------------------//
// SOUND DETECTION Service loop
// Parameter: 
//          none
// return value:
//          none
//------------------------------------------------------------------//
void SOUND_DETECTION_ServiceLoop(void)
{
	if(sdCtl.state==STATUS_STOP) return;
	if(sdCtl.process_flag==0) return; // adc buf no new data
    sdCtl.process_flag=0;
	SOUND_DETECTION_Process(sd_adc_tmp,SOUND_DETECTION_BUF_SIZE);
}
#endif
