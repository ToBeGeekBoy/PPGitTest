/* Includes ------------------------------------------------------------------*/
#include "nx1_adc.h"
#include "include.h"
#include "nx1_timer.h"
#include <string.h>
//#include "debug.h"

#if _PD_PLAY

#if (_PD_TYPE==TYPE0)// Detect human voice
    #define LowFreq         31
    #define HighFreq        625
#elif (_PD_TYPE==TYPE1)// Detect pure tone
#if (_PD_TYPE1_DETECT_FREQ==TYPE0)
    #define LowFreq         31
    #define HighFreq        2000
#elif (_PD_TYPE1_DETECT_FREQ==TYPE1)
    #define LowFreq         1000
    #define HighFreq        3000
#elif (_PD_TYPE1_DETECT_FREQ==TYPE2)
    #define LowFreq         2000
    #define HighFreq        4000
#endif
#endif

const S16 PDCtrl[5] = {
    _PD_TYPE,               // Method : 0 or 1 , 0 -> Detect human voice;  1 -> Detect pure tone.
    LowFreq,                // Detect Lower Frequency Limit
    HighFreq,               // Detect Upper Frequency Limit
    PD_FFT_SIZE,            // Detect length
    _PD_SampleRate,         // Sample rate 8000/16000
};
#define ABS(a) (((a) < 0) ? -(a) : (a))

#if !defined(PD_EXAMPLE)
#define PD_EXAMPLE      DISABLE             //Collect the max count of main pitch frequency, Example ENABLE/DISABLE
#endif

#if PD_EXAMPLE
#define _PD_ResponseTime   350              // Response time unit:ms, default:350, max: 65536
#else
#define _PD_ResponseTime   32               // Response time unit:ms, default:32, max: 65536
#endif

#if PD_EXAMPLE
static const S16 pd_hz[19] = {
    62,93,125,156,187,218,250,281,312,343,375,406,437,468,500,531,562,593,625
};
static U16 statistic_pitch[20] ={ 0 };
static S16 MaxValue = 0;
S16 output_index;
S16 diff_value=0;
S8 clear_flg=1;
#endif

#if _USE_HEAP_RAM
static S16 *pd_adc=NULL;
static S16 *pd_adc_tmp=NULL;
PDCtlBuf* PDbuf = NULL;
FFTctl* PDFFTCtl = NULL;
#else
static S16 pd_adc[PD_BUF_SIZE] ={ 0 };
static S16 pd_adc_tmp[PD_BUF_SIZE] ={ 0 };
PDCtlBuf  PDbuf[1];
FFTctl  PDFFTCtl[1];
#endif

static volatile struct{
    U16 msTime;         //0~_PD_ResponseTime
    U16 adcBufOffset;   //0~256
#if PD_EXAMPLE
    S8  index;
#else
    S16 value;
#endif
    U8  state;          //STATUS_PLAYING,STATUS_STOP
    U8  process_flag;
    U8	r_factor_count;
    U16	r_factor_count_t;
}pdCtl;

extern void  CB_PD_GetFreqSuccess(void);

//------------------------------------------------------------------//
// Allocate Pitch detection memory
// Parameter: 
//          func: Choose Allocate function
// return value:
//          NONE
//------------------------------------------------------------------// 
#if _USE_HEAP_RAM
static void PD_AllocateMemory(U16 func)
{
    if(pd_adc==NULL && (func&MEMORY_ALLOCATE_PLAY)) {
        pd_adc  = (S16 *)MemAlloc(PD_BUF_SIZE<<1);
    }
    if(pd_adc_tmp==NULL && (func&MEMORY_ALLOCATE_PLAY)) {
        pd_adc_tmp  = (S16 *)MemAlloc(PD_BUF_SIZE<<1);
    }
    if(PDbuf==NULL && (func&MEMORY_ALLOCATE_PLAY)) {
        PDbuf = (PDCtlBuf*)MemAlloc(PD_CTL_BUF_SIZE);
    }
    if(PDFFTCtl==NULL && (func&MEMORY_ALLOCATE_PLAY)) {
        PDFFTCtl = (FFTctl*)MemAlloc(PD_FFT_CTL_SIZE);
    }
}

//------------------------------------------------------------------//
// Free Pitch detection memory
// Parameter: 
//          func: Choose Allocate function
// return value:
//          NONE
//------------------------------------------------------------------//
static void PD_FreeMemory(U16 func)
{
    if(pd_adc!=NULL && (func&MEMORY_ALLOCATE_PLAY)) {
        MemFree(pd_adc);
        pd_adc=NULL;
    }
    if(pd_adc_tmp!=NULL && (func&MEMORY_ALLOCATE_PLAY)) {
        MemFree(pd_adc_tmp);
        pd_adc_tmp=NULL;
    }
    if(PDbuf!=NULL && (func&MEMORY_ALLOCATE_PLAY)) {
        MemFree(PDbuf);
        PDbuf=NULL;
    }
    if(PDFFTCtl!=NULL && (func&MEMORY_ALLOCATE_PLAY)) {
        MemFree(PDFFTCtl);
        PDFFTCtl=NULL;
    }
}
#endif

//------------------------------------------------------------------//
// (static) Get PitchDetection result with algorighm API
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
static S32 _getPitchDetectionResult(void)
{
//Collect the max count of mainPitch
#if PD_EXAMPLE
    if (clear_flg == 1) {
        memset(statistic_pitch,0,sizeof(statistic_pitch));
        clear_flg = 0;
        output_index = -1;
        pdCtl.index=-1;
    }
    MaxValue = 0;
    S32 mainPitch = PitchDetect(pd_adc_tmp, PDCtrl, PDbuf, PDFFTCtl);
//Collect the max count of mainPitch
    U16 i;
    for(i=0 ; i<19 ; ++i) {
        if(mainPitch==pd_hz[i]) {
            ++statistic_pitch[i];
            break;
        }
    }
//Find peak frequency value  62~625Hz
    for (i = 0; i < 19; i++) {
        if (statistic_pitch[i] <= 3) continue;
        if (statistic_pitch[i] < MaxValue) continue;
        
        if (statistic_pitch[i] == MaxValue) { // warning i==0
            if (ABS((statistic_pitch[i] - statistic_pitch[i - 1])) < diff_value ) {
                output_index = i;
            }
        } else {
            MaxValue = statistic_pitch[i];
            output_index = i;
            if (i > 0) {
                diff_value = ABS((statistic_pitch[i] - statistic_pitch[i - 1]));
            }
        }
    }
    return output_index;
#else
    return PitchDetect(pd_adc_tmp, PDCtrl, PDbuf, PDFFTCtl);
#endif
}

//------------------------------------------------------------------//
// PitchDetection Start
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void PD_Start(void)
{
    if(pdCtl.state==STATUS_PLAYING) return;
    InitAdc(_PD_SampleRate);
    ADC_Cmd(ENABLE); // HW initial
#if PD_EXAMPLE
    pdCtl.index=0;
#endif
#if _USE_HEAP_RAM
    PD_AllocateMemory(MEMORY_ALLOCATE_PLAY);
#endif
    memset(PDbuf, 0, PD_CTL_BUF_SIZE);
    memset(PDFFTCtl, 0, PD_FFT_CTL_SIZE);
    PD_Init(PDCtrl, PDbuf, PDFFTCtl);
    pdCtl.adcBufOffset=0;
    pdCtl.msTime = 0;
    pdCtl.process_flag=0;
    pdCtl.r_factor_count=0;
    pdCtl.r_factor_count_t=0;
    pdCtl.state=STATUS_PLAYING;
}

//------------------------------------------------------------------//
// PitchDetection Stop
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void PD_Stop()
{
    if(pdCtl.state==STATUS_STOP) return;
    pdCtl.state=STATUS_STOP;
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
#if _USE_HEAP_RAM
    PD_FreeMemory(MEMORY_ALLOCATE_PLAY);
#endif
}
//------------------------------------------------------------------//
// PitchDetection Get Status
// Parameter: 
//          NONE
// return value:
//          STATUS_STOP, STATUS_PLAYING
//------------------------------------------------------------------//
U8  PD_GetStatus(void)
{
    return pdCtl.state;
}

//------------------------------------------------------------------//
// PitchDetection Get Freq
// Parameter: 
//          Freq: Set target frequency. The scale is 31Hz. Set 0 is for full range. If _PD_TYPE is TYPE0, force set to 0.
// return value:
//          0:no data
//			[_PD_TYPE = TYPE0]
//          Freq:62,93,125,156,187,218,250,281,312,343,375,406,437,468,500,531,562,593,625
//			[_PD_TYPE = TYPE1, _PD_TYPE1_DETECT_FREQ = TYPE0]
//          Freq:31~2000
//			[_PD_TYPE = TYPE1, _PD_TYPE1_DETECT_FREQ = TYPE1]
//          Freq:1000~3000
//			[_PD_TYPE = TYPE1, _PD_TYPE1_DETECT_FREQ = TYPE2]
//          Freq:2000~4000
//------------------------------------------------------------------//
S16 PD_GetFreq(S16 Freq)
{
    if(pdCtl.state==STATUS_STOP) return 0;
    if(pdCtl.msTime<=_PD_ResponseTime) return 0;
    pdCtl.msTime = 0;
    S16 PitchHz=0;
    
    #if (_PD_TYPE==TYPE0)
		Freq=0; //force set to 0
	#endif
    
    //over time check
    if(pdCtl.r_factor_count_t<511)	//limit time = 512*_PD_ResponseTime(default 32ms)
    {
    	pdCtl.r_factor_count_t++;
    }
    else
    {
    	pdCtl.r_factor_count_t=0;
    	pdCtl.r_factor_count=0;
    }
    
#if PD_EXAMPLE
    // Response time _PD_ResponseTime*1 ms
    if(pdCtl.index>=0) {
        PitchHz=pd_hz[pdCtl.index];
        clear_flg=1;
        pdCtl.index=0;
    } else {
        PitchHz=0;
    }
#else
    // Response time _PD_ResponseTime*1 ms
    if(pdCtl.value>(LowFreq+1) && pdCtl.value<(HighFreq+1)) {
        PitchHz=pdCtl.value;
        pdCtl.value=0;
    } else {
        PitchHz=0;
    }
#endif

	if((Freq!=0)&&((PitchHz<=(Freq-31))||(PitchHz>=(Freq+31))))// out of range, return 0
	{
		PitchHz=0;
	}

    return PitchHz;
}

//------------------------------------------------------------------//
// PitchDetection Get Freq by R_Factor (de-bounce)
// Parameter: 
//			Freq: Set target frequency. The scale is 31Hz. Set 0 is for full range. If _PD_TYPE is TYPE0, force set to 0.
//          r_factor: Debounce time or Response time = r_factor*32ms. r_factor>0
// return value:
//          true / false
//
//------------------------------------------------------------------//
bool PD_GetFreqResult(S16 Freq,S16 r_factor)
{
	S16	final_freq;
	
	if(!r_factor)
		r_factor=1;
		
	final_freq=PD_GetFreq(Freq);

	if(final_freq>0)
	{
		pdCtl.r_factor_count++;	
		//#if _DEBUG_MODE
		//dprintf("final_freq=%dhz,%d,%d\r\n",final_freq,pdCtl.r_factor_count,pdCtl.r_factor_count_t);
		//#endif
		if(pdCtl.r_factor_count==r_factor)
		{
			pdCtl.r_factor_count=0;
			
			//over time check
			if(pdCtl.r_factor_count_t>(r_factor*2))
			{
				pdCtl.r_factor_count_t=0;
				#if _DEBUG_MODE
				dprintf("over time\r\n");
				#endif
				//return 0;	//return frequency
				return false;
			}
			else
			{
				pdCtl.r_factor_count_t=0;
				#if _DEBUG_MODE
				dprintf("success\r\n");
				#endif
				CB_PD_GetFreqSuccess();
				//return final_freq; //return frequency
				return true;
			}
		}
	}
	return 0;
}

//------------------------------------------------------------------//
// PitchDetection ms ISR
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void PD_MilliSecondIsr(void)
{
    ++pdCtl.msTime;
}

//------------------------------------------------------------------//
// PD Adc Isr
// Parameter: 
//          size: data-buffer fill size
//       pdest32: data-buffer address
// return value:
//          none
//------------------------------------------------------------------//
void PD_AdcIsr(U8 size, U32 *pdest32)
{
    if(pdCtl.state==STATUS_STOP) return;
    U16 i;
    for(i = 0; i < size ; i++) {
        pd_adc[pdCtl.adcBufOffset++]=((*pdest32++)-32768);
        if (pdCtl.adcBufOffset == PD_BUF_SIZE) {
            memcpy(pd_adc_tmp,pd_adc,sizeof(S16)*PD_BUF_SIZE);
            pdCtl.process_flag=1;
            pdCtl.adcBufOffset = 0;
        }
    }
}
//------------------------------------------------------------------//
// PD Service loop
// Parameter: 
//          none
// return value:
//          none
//------------------------------------------------------------------//
void PD_ServiceLoop(void)
{
    if(pdCtl.state==STATUS_STOP) return;
    if(pdCtl.process_flag==0) return; // adc buf no new data
    pdCtl.process_flag=0;
#if PD_EXAMPLE
    pdCtl.index=_getPitchDetectionResult();
#else
    pdCtl.value=_getPitchDetectionResult();
#endif
}

#endif