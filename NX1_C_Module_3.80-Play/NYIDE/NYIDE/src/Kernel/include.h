#ifndef INCLUDE_H
#define INCLUDE_H

#ifndef  __ASSEMBLY__
#include "nx1_lib.h"
#endif

#include "nx1_config.h"
#include "nx1_reg.h"

#if _QCODE_BUILD
#include "qcode_api.h"
#include "debug.h"
#endif

#include "NX1_Tab.h"
#include "NX1_VoiceChangerTab.h"

#if !defined(_ADPCM_SPEEDPITCH_HQ)
#define	_ADPCM_SPEEDPITCH_HQ	DISABLE
#endif

#define _IS_APPS_USE_DAC        (_ADPCM_PLAY    ||\
                                 _ADPCM_CH2_PLAY||\
                                 _ADPCM_CH3_PLAY||\
                                 _ADPCM_CH4_PLAY||\
                                 _ADPCM_CH5_PLAY||\
                                 _ADPCM_CH6_PLAY||\
                                 _ADPCM_CH7_PLAY||\
                                 _ADPCM_CH8_PLAY||\
                                 _SBC_PLAY      ||\
                                 _SBC_CH2_PLAY  ||\
                                 _SBC2_PLAY     ||\
                                 _SBC2_CH2_PLAY ||\
                                 _CELP_PLAY     ||\
                                 _MIDI_PLAY     ||\
                                 _PCM_PLAY      ||\
                                 _PCM_CH2_PLAY  ||\
                                 _PCM_CH3_PLAY  ||\
                                 _RT_PLAY       ||\
                                 _WAVE_ID_MODULE||\
                                 _DAC_MODE      ||\
                                 _AUDIO_AMP_SD_CTL)
#define _IS_APPS_USE_ADC_MIC    (_VR_FUNC           ||\
                                 _SBC_RECORD        ||\
                                 _ADPCM_RECORD      ||\
                                 _PCM_RECORD        ||\
                                 _RT_PLAY           ||\
                                 _PD_PLAY           ||\
                                 _SOUND_DETECTION   ||\
                                 _WAVE_ID_MODULE    ||\
                                 _SL_MODULE )

#define _SL_BUF_SIZE			256
#define _SL_SampleRate			16000
#if _SL_MODULE
//_SL define
#if _SL_MIC_NUM==3
#define _SL_Base_RAM_SIZE      	(_SL_BUF_SIZE*6)
#define	_SL_STACK_SIZE			592
#elif _SL_MIC_NUM==2
#define _SL_Base_RAM_SIZE      	(_SL_BUF_SIZE*4)
#define	_SL_STACK_SIZE			372
#endif
#define _SL_RAM_SIZE      		((_SL_Base_RAM_SIZE*2)+(sizeof(SSLBuf1)))
	
#if _EF_SERIES
#if _SL_MIC_NUM==3
	#define _SL_ADC_CH0_FIFO_NUM	2
#elif _SL_MIC_NUM==2
	#define _SL_ADC_CH0_FIFO_NUM	1
#endif
#else
#if _SL_MIC_NUM==3
	#define _SL_ADC_FIFO_NUM		3//FIFO_LEVEL Set in adc.h ,_SL_ADC_FIFO_NUM & FIFO_LEVEL should equal
#elif _SL_MIC_NUM==2
	#define _SL_ADC_FIFO_NUM		2//FIFO_LEVEL Set in adc.h ,_SL_ADC_FIFO_NUM & FIFO_LEVEL should equal
#endif
#endif


typedef struct SSLBuf1
{
	S32 i1[256];
    S32 i2[256];
    #if _SL_MIC_NUM==3
    S32 i3[256];
    #endif
    S32 DCremove1[8];
    S32 DCremove2[8];
    #if _SL_MIC_NUM==3
    S32 DCremove3[8];
    #endif
    S32 AvgEnergy; 
    S16 Vad[12];
    S16 AzimuthStatistic[50];
    #if _SL_MIC_NUM==3
    S16 AzimuthCandidate[6];
    #endif
    S16 Insamples;
    S16 PreVadState;
    S16 TotalCandidate;
    S16 UpperDlyPos;
    S16 LowerDlyPos;
    S16 Dlypoint;
    #if _SL_MIC_NUM==2
    S16 InputPreData;		
	S8  EFFlag;				
    #endif
}SSLBuf1;
#endif

//-----------------------------
typedef struct _noisegatebuf_v2 {
        short   VadBuffer[12];
        short   InSize;
        short   vad_mute_scale;
        short   vad_mute_count;
        short   vad_turnon_scale;
        short   vad_turnon_count;
        char    VAD_MUTE_LV_1;
        char    VAD_MUTE_LV_2;
        char    VAD_MUTE_LV_3;
        char    SegmentPart;
        char    Remainder;
}NoiseGateBuf_v2;

extern S8 NGInit_v2(S16 samples, const S16 *control_1, const S16 *control_2, NoiseGateBuf_v2 *noisegatebuf_v2);
extern void  NGProcess_v2(S16 *buffer, NoiseGateBuf_v2 *noisegatebuf_v2);
//-----------------------------

typedef struct _FFTCtl {
     S16  FFTSize;
     U16 Fcount;
     const S16 *SinPtr;
     const S16 *BitreversePtr;
     const S16 *WindowPtr;
}FFTctl;


#define ADPCM_ROBOT4_DATA_LENGTH	96		
#define SBC_ROBOT4_DATA_LENGTH		320
#define RT_ROBOT4_DATA_LENGTH		320
#define	ROBOT4_FFT_SIZE  			256
#define ROBOT4_OVERLAP_SIZE 		ROBOT4_FFT_SIZE
#define ROBOT4_OVERFLOW_SIZE 		96
#define ROBOT4_PREIN_SIZE 			192
#define	ROBOT4_OVERLAP_RATIO 	  	75

typedef struct _ROBOT4Ctl {
	unsigned short Samplerate;
	unsigned short FFTSize;
	unsigned short InputSize;			
	unsigned char  Overlapratio;		//50 is 50%, 75 is 75%
}ROBOT4Ctl;

typedef struct _ROBOT4Buf {
	int	FFTBuf[ROBOT4_FFT_SIZE];
	short PreInBuf[ROBOT4_PREIN_SIZE];		
	short Overlapbuf[ROBOT4_OVERLAP_SIZE];	
	short OverflowBuf[ROBOT4_OVERFLOW_SIZE];		
	short *Window;
	short *Phase;
	short *EQ;
	unsigned char  OverlapSize;
	unsigned char  HopSize;
	unsigned char  Flg;
	unsigned char  Frames;
	unsigned char  RptTimes;		
}ROBOT4Buf;


typedef struct _RTChorusCtl{
	float delay_time_1;		// (ms) (delay time between voice 1 and voice 2)
	float delay_time_2;		// (ms) (delay time between voice 2 and voice 3)
	U16 sample_rate;		// 8000 / 10000 / 12000 / 16000
	S16 pitch_factor;		// factor range : -12 ~ 12
	S16 volume_voice_1;		// 0% ~ 100% (voice 1: pitch changed voice)
	S16 volume_voice_2;		// 0% ~ 100% (voice 2: original voice)
	S16 volume_voice_3;		// 0% ~ 100% (voice 3: pitch changed voice)
	S16 delay_sample_1;
	S16 delay_sample_2;
	U8 Cmd;
}RTChorusCtl;


//-----------------------------
// Howling Detect and Remove
#if _RT_PLAY && _RT_HOWLING_SUPPRESS

typedef struct _HRVarBuf{		
	S32 FFTBuf[256];	//FFT_size  256 -> FFTBuf 256
	S32 FilterBuf[8 * 2];	//Add the second buffer of filter. v3.1
	S32 ExFilterBuf[8 * 2];	//Add the second buffer of filter. v3.3
	S32 nowFrameEng;
	const S16 *FilterParm_Ptr;
	U16 SPCBuf[128];			// v3.7
	S16 FilterParam[13 * 2];			//Add the second parameters of filter. v3.1
	S16 ExFilterParam[13 * 2];			//Add the cutin parameters of filter. v3.3
	S16 pitch_update;
	U16 MaxPeak_Energy[2];		// v3.4
	S16 MaxPeakLoc[2];			// v3.4
	S16 PreMaxPeakLoc[2];			// v3.5
	U16 SampleRate;
	U16 start_freq;			// smallest frequency limit
	U16 end_freq;			//highest frequency limit
	U16 Input_size;
	U16 WindowStep;
	S16  Freq1_history[5];		// v3.5
	S16  Freq2_history[5];		// v3.5
	S16  PreFreq1;				// v3.5
	S16  PreFreq2;				// v3.5
	S16  LastFreqNumber;		// v3.5
	S8  State;					// v3.5
	S8  Buffer;					// v3.5
	U8  SameCntLim;				// v3.5
	U8  SPCShiftLevel;
	U8	SPCTotalShiftLevel;
	U8	copy_flag;			// data coopy flag
	U8	Accum_times;
	U8	Accum_count;
	U8	Accum_type;			//clare add 20230222
	U8  BuffFullFlag;
	U8  FreqChangeFlag;		// v3.1
	S8  FreqNumber;			// v3.3
	U8  ContinueFilt;		// v3.5
	U8  Offset;				// v3.7
}HRCtlBuf;

extern S8 HowlingRemove_Init(S16* hrSetting, HRCtlBuf* hrCtrlBuf, FFTctl* fftctl, const S16 *filterPtr);
extern S16 HowlingDetect(S16* InputBuf, HRCtlBuf* hrCtrlBuf, FFTctl* fftctl);
extern void HowlingRemove(S16* InputBuf, U16 frame_size, HRCtlBuf* hrCtrlBuf);
#endif
//-----------------------------

#define AGC_FRAME_SIZE          32

#if _EF_SERIES
#define SPI0_MISO_MTF_PIN      	0
#define SPI0_CSB_MTF_PIN       	1
#define SPI0_IO2_MTF_PIN        4
#define SPI1_CSB_MTF_PIN       	12
#define SPI1_MISO_MTF_PIN		15
#else
#define SPI0_CEN_MTF_PIN        0
#define SPI0_SCLK_MTF_PIN       1
#define SPI0_IO0_MTF_PIN        2
#define SPI0_IO1_MTF_PIN        3
#define SPI0_IO2_MTF_PIN        4
#define SPI1_SCLK_MTF_PIN       9
#endif
#if _EF_SERIES
#define I2C_SDA_MTF_PIN         4
#define I2C_SCL_MTF_PIN         5
#else
#define I2C_SDA_MTF_PIN         12
#endif

#if _EF_SERIES
#define PWMA0_PIN               12
#define PWMA1_PIN               13
#define PWMA2_PIN               14
#define PWMA3_PIN               15
#else
#define PWMA0_PIN               8
#define PWMA1_PIN               9
#define PWMA2_PIN               10
#define PWMA3_PIN               11
#endif

//PWM-IO
#define START_HIGH				1
#define START_LOW				0

#define ACTIVE_HIGH				1
#define ACTIVE_LOW				0

#define ADC_CH0_PIN             0
#define ADC_CH1_PIN             1
#define ADC_CH2_PIN             2
#define ADC_CH3_PIN             3
#if _EF_SERIES
#define PWMB0_PIN               0
#define PWMB1_PIN               1
#define PWMB2_PIN               2
#define PWMB3_PIN               3
#else
#define PWMB0_PIN               12
#define PWMB1_PIN               13
#define PWMB2_PIN               14
#define PWMB3_PIN               15
#endif

#define MIDIMARK_FUNC_NUM       8
#define MIDISECTION_FUNC_NUM	8

#if _EF_SERIES
#define DAC_DATA_OFFSET         0
#else
#define DAC_DATA_OFFSET         15
#endif

#define SMOOTH_DOWN         0
#define SMOOTH_UP           1
#define SMOOTH_NONE         2
#define SMOOTH_CH0          0
#define SMOOTH_CH1          1

#define	MINUS_1DB		(0.8913f*MAX_INT16)
#define	MINUS_2DB		(0.7943f*MAX_INT16)
#define	MINUS_3DB		(0.7079f*MAX_INT16)
#define	MINUS_4DB		(0.6310f*MAX_INT16)
#define	MINUS_5DB		(0.5623f*MAX_INT16)
#define	MINUS_6DB		(0.5012f*MAX_INT16)
#define	MINUS_7DB		(0.4467f*MAX_INT16)
#define	MINUS_8DB		(0.3981f*MAX_INT16)
#define	MINUS_9DB		(0.3548f*MAX_INT16)
#define	MINUS_12DB	    (0.2512f*MAX_INT16)


#define SPI_RESERVED_SIZE           1024
#define SPI_BASE_INFO_SIZE          16
#define SPI_TIMBRE_ADDR_OFFSET      6
#define SPI_TAB_INFO_ENTRY_SIZE     4
#define SPI_FILE_COUNT_SIZE         2
#define SPI_DATA_TYPE_MASK          0xF0000000
#define SPI_DATA_TYPE_MASK2         0xF8000000

#define SPI_DATA_TYPE_SENTENCE		0x0
#define SPI_DATA_TYPE_MIDI          0x4
#define SPI_DATA_TYPE_ACTION		0x5
#define SPI_DATA_TYPE_MARK			0x9

#define RATIO_INIT_INDEX        12

#define SPEED_CTL_SAMPLE_RATE        16000

#define E_AUDIO_OK                      0
#define E_AUDIO_INVALID_HEADER          1
#define E_AUDIO_UNSUPPORT_CODEC         2
#define E_AUDIO_INVALID_SAMPLERATE      3
#define E_AUDIO_INVALID_CHANNEL         4
#define E_AUDIO_UNSUPPORT_NYQ           5
#define E_AUDIO_UNSUPPORT_NYW           6
#define E_AUDIO_INVALID_IDX             7
#define E_AUDIO_SD_OPEN_FAIL            8
#define E_AUDIO_NOT_VOICE               9
#define E_AUDIO_NOT_MIDI                10
#define E_AUDIO_STORAGE_BUSY            11

#define MIDI_SAMPLE_RATE    QMIDI_SAMPLE_RATE
#define MIDI_STORAGE_TYPE   QMIDI_STORAGE_TYPE

#define Q15_SHIFT           15

typedef enum {
    CH_UNKNOWN=0,
#if _SBC_PLAY
    CH_SBC,
#endif
#if _SBC_CH2_PLAY
    CH_SBC_CH2,
#endif
#if _ADPCM_PLAY
    CH_ADPCM,
#endif
#if _ADPCM_CH2_PLAY
    CH_ADPCM_CH2,
#endif
#if _CELP_PLAY
    CH_CELP,
#endif
#if _PCM_PLAY
    CH_PCM,
#endif
#if _MIDI_PLAY
    CH_MIDI,
#endif
#if _ADPCM_CH3_PLAY
    CH_ADPCM_CH3,
#endif
#if _ADPCM_CH4_PLAY
    CH_ADPCM_CH4,
#endif
#if _PCM_CH2_PLAY
    CH_PCM_CH2,
#endif
#if _PCM_CH3_PLAY
    CH_PCM_CH3,
#endif
#if _SBC2_PLAY
    CH_SBC2,
#endif
#if _SBC2_CH2_PLAY
    CH_SBC2_CH2,
#endif
#if _ADPCM_CH5_PLAY
    CH_ADPCM_CH5,
#endif
#if _ADPCM_CH6_PLAY
    CH_ADPCM_CH6,
#endif
#if _ADPCM_CH7_PLAY
    CH_ADPCM_CH7,
#endif
#if _ADPCM_CH8_PLAY
    CH_ADPCM_CH8,
#endif
    num_AUDIO_DECODER_LIST,
} AUDIO_DECODER_LIST;

#define body_none_ADC       ((((OPTION_IC_BODY >> 12)&0xF) == 1) || (((OPTION_IC_BODY_EXTD >> 24)&0xF) == 1))
#define body_is_4KHZ		((_IC_BODY == 0x11A22) || (_IC_BODY == 0x12A44))
#define body_disable_ldo    (_IC_BODY != 0x12A44)
#define body_MultiTrgLdo    (_IC_BODY == 0x11A22)
#define body_SingleDAC		((_IC_BODY == 0x11F20) || (_IC_BODY == 0x11F21) || (_IC_BODY == 0x11F22) || (_IC_BODY == 0x11F23))
#define STANDBY(cond)                   __asm__ volatile ("standby "#cond);

#ifndef _SPI_KEEP_VDD_IN_SLEEP
#define SPI_KEEP_VDD_IN_SLEEP	DISABLE
#else
#define SPI_KEEP_VDD_IN_SLEEP	(((_SPI_KEEP_VDD_IN_SLEEP)==DISABLE)?DISABLE:ENABLE)
#endif

#define ADPCM_DEC_IN_SIZE   86//byte
#define ADPCM_DEC_OUT_SIZE  136//word

#define PCM_DEC_IN_SIZE     80//byte
#define PCM_DEC_OUT_SIZE    40//word

#define CELP_DEC_IN_SIZE    12//byte
#define CELP_DEC_OUT_SIZE   160//word

#define SMOOTH_DOWN_TIME    10//ms
#define SMOOTH_UP_TIME      5//ms
#define SMOOTH_STEP         30

#define MAX_SMOOTH_GAIN     32767//0.9999(Q15 format)

#define MAX_INT16 	        (32767)
#define MIN_INT16 	        (-32768)

#define SPEED_CTL_FRAME_SIZE    360
#define SPEED_CTL_OUT_SIZE      90

//#if QMIDI_CH_NUM==4
//#define MIDI_DEC_OUT_SIZE   32//word
#define MIDI_DEC_OUT_SIZE			(MIDI_SAMPLE_RATE*4/1000)
#if (QMIDI_TIMBRE_FORMAT&0x4)//ADPCM
#define MIDI_PROC_STATE_SIZE        ((128*QMIDI_CH_NUM)>>2)
#else//PCM ULAW
#define MIDI_PROC_STATE_SIZE        ((72*QMIDI_CH_NUM)>>2)//structure change
#endif
#define MIDI_RAM_ALLOCATE_SIZE      (32+MIDI_DEC_OUT_SIZE+MIDI_PROC_STATE_SIZE)
#define MIDI_AUDIO_OUT_COUNT        5
#define MIDI_AUDIO_BUF_SIZE         (MIDI_DEC_OUT_SIZE*MIDI_AUDIO_OUT_COUNT)

#if _VR_FUNC
#if _VR_SAMPLE_RATE==16000 && _VR_FRAME_RATE==625
#define BUF_SIZE                    (256)
#if _VR_STORAGE_TYPE
#define VR_STACK_SIZE               640
#else
#define VR_STACK_SIZE               536
#endif
#elif _VR_SAMPLE_RATE==16000 && _VR_FRAME_RATE==100
#define BUF_SIZE                    (160)
#if _VR_STORAGE_TYPE
#define VR_STACK_SIZE               640
#else
#define VR_STACK_SIZE               536
#endif
#elif _VR_SAMPLE_RATE==8000 && _VR_FRAME_RATE==625
#define BUF_SIZE                    (128)
#define VR_STACK_SIZE               832
#elif _VR_SAMPLE_RATE==8000 && _VR_FRAME_RATE==100
#define BUF_SIZE                    (80)
#define VR_STACK_SIZE               832
#else 
#error	"Not support VR file on the incorrect CSMT project settings." 	
#endif
#endif
#define VR_STATE_SIZE	    	    96
#if _USE_HEAP_RAM==DISABLE
#if (_VR_ENGINE_RAM_SIZE < _VR_SD_ENGINE_RAM_SIZE)
#undef	_VR_ENGINE_RAM_SIZE
#define	_VR_ENGINE_RAM_SIZE			_VR_SD_ENGINE_RAM_SIZE
#endif
#endif
#define _VR_RAM_SIZE                (_VR_ENGINE_RAM_SIZE+(BUF_SIZE*4))
#define _VR_SD_RAM_SIZE				(_VR_SD_ENGINE_RAM_SIZE+(BUF_SIZE*4))

/* * *                          SBC RAM Size                            * * */
#define SBC_DEC_IN_SIZE             80//byte  //bit-rate 32000
#define SBC_DEC_OUT_SIZE            320//word
#define SBC_ECHO_DATA_LENGTH        40
#define SBC_ECHO_BUF_SIZE           SBC_ECHO_DATA_LENGTH*30
#if _ROBOT_SOUND_COMPRESS
#define SBC_ROBOT_BUF_SIZE          800
#define SBC_ROBOT02_BUF_SIZE        (36+320+1000)
#else
#define SBC_ROBOT_BUF_SIZE          1600
#define SBC_ROBOT02_BUF_SIZE        (36+320+2000)
#endif
#define SBC_REVERB_BUF_SIZE         830 //word
#define SBC_ERASE_RECORD_SIZE       ((_SBC_SECTOR_ERASE_MAXTIME*16000/1000+160)/320*40)
#define SBC_EQ_BUF_SIZE		        24//dword

#define SBC_ROBOT4_CTL_SIZE		(sizeof(ROBOT4Ctl))
#define SBC_ROBOT4_BUF_SIZE		(sizeof(ROBOT4Buf))
#define SBC_ROBOT4_FFT_CTL_SIZE		(sizeof(FFTctl))

#if _SBC_AUTO_SAVE_TIME
#define SBC_RAM_ERASE_BUF_SIZE      (((((_SBC_AUTO_SAVE_SR/8) *_SBC_SECTOR_ERASE_MAXTIME / 1000) + 3) >>2) <<2)
#else
#define SBC_RAM_ERASE_BUF_SIZE      0
#endif

// CH1
#define SBC_AUDIO_OUT_SIZE          SBC_DEC_OUT_SIZE
#if _SBC_AUDIO_BUF_X3
#define SBC_AUDIO_OUT_COUNT     	3
#else
#define SBC_AUDIO_OUT_COUNT     	2
#endif
#define SBC_AUDIO_BUF_SIZE          (SBC_AUDIO_OUT_SIZE*SBC_AUDIO_OUT_COUNT)  //2 Bytes
#if _SBC_RECORD
#define SBC_RAM_ALLOCATE_SIZE       640
#else
#define SBC_RAM_ALLOCATE_SIZE       492
#endif
#define SBC_CH1_PLAY_RAM            ((SBC_AUDIO_BUF_SIZE<<1)+(SBC_RAM_ALLOCATE_SIZE<<1)+SBC_RAM_ERASE_BUF_SIZE)
#if _SBC_RECORD_ERASING
#define SBC_CH1_REC_RAM           	(SBC_CH1_PLAY_RAM + SBC_DEC_IN_SIZE+(AGC_FRAME_SIZE<<1) + SBC_ERASE_RECORD_SIZE - SBC_RAM_ERASE_BUF_SIZE)
#else
#define SBC_CH1_REC_RAM             (SBC_CH1_PLAY_RAM + SBC_DEC_IN_SIZE+(AGC_FRAME_SIZE<<1) - SBC_RAM_ERASE_BUF_SIZE)
#endif
#define SBC_CH1_PITCH_RAM           ((SBC_DEC_OUT_SIZE+SPEED_CTL_FRAME_SIZE)<<1)
#define SBC_CH1_ROBOT2_RAM          (SBC_ROBOT_BUF_SIZE<<1)
#define SBC_CH1_ROBOT3_RAM          (SBC_ROBOT02_BUF_SIZE<<1)
#define SBC_CH1_ROBOT4_RAM          (SBC_ROBOT4_BUF_SIZE+SBC_ROBOT4_FFT_CTL_SIZE+SBC_ROBOT4_CTL_SIZE)
#define SBC_CH1_REVERB_RAM          (SBC_REVERB_BUF_SIZE<<2)
#define SBC_CH1_ECHO_RAM            (SBC_ECHO_BUF_SIZE<<1)

// CH2
#if _SBC_CH2_SPEED_CHANGE||_SBC_CH2_PITCH_CHANGE
#define SBC_CH2_AUDIO_OUT_SIZE      SPEED_CTL_OUT_SIZE
#define SBC_CH2_AUDIO_OUT_COUNT     6
#define SBC_CH2_AUDIO_BUF_SIZE      (SBC_CH2_AUDIO_OUT_SIZE*SBC_CH2_AUDIO_OUT_COUNT)
#else
#define SBC_CH2_AUDIO_OUT_SIZE      SBC_DEC_OUT_SIZE
#define SBC_CH2_AUDIO_OUT_COUNT     2
#define SBC_CH2_AUDIO_BUF_SIZE      (SBC_CH2_AUDIO_OUT_SIZE*SBC_CH2_AUDIO_OUT_COUNT)
#endif
#define SBC_CH2_RAM_ALLOCATE_SIZE   492

#define SBC_CH2_PLAY_RAM            ((SBC_CH2_AUDIO_BUF_SIZE<<1)+(SBC_CH2_RAM_ALLOCATE_SIZE<<1))
#define SBC_CH2_PITCH_RAM           ((SBC_DEC_OUT_SIZE+SPEED_CTL_FRAME_SIZE)<<1)
#define SBC_CH2_ROBOT2_RAM          (SBC_ROBOT_BUF_SIZE<<1)
#define SBC_CH2_ROBOT3_RAM          (SBC_ROBOT02_BUF_SIZE<<1)
#define SBC_CH2_REVERB_RAM          (SBC_REVERB_BUF_SIZE<<2)
#define SBC_CH2_ECHO_RAM            (SBC_ECHO_BUF_SIZE<<1)


/* * *                          SBC-2 RAM Size                            * * */
#define SBC2_DEC_IN_SIZE             80//byte  //bit-rate 32000
#define SBC2_DEC_OUT_SIZE            320//word
#define SBC2_EQ_BUF_SIZE             24//dword
// SBC-2 CH1
#define SBC2_AUDIO_OUT_SIZE          SBC2_DEC_OUT_SIZE
#if _SBC2_AUDIO_BUF_X3
#define SBC2_AUDIO_OUT_COUNT     	 3
#define SBC2_AUDIO_BUF_SIZE          SBC2_AUDIO_OUT_SIZE*SBC2_AUDIO_OUT_COUNT
#else //_SBC2_AUDIO_BUF_X3
#if(_SBC2_PLAY&&_SBC2_CH2_PLAY)
#define SBC2_AUDIO_BUF_SIZE          480 //160*3, A-B-C buffer
#else
#define SBC2_AUDIO_OUT_COUNT     	 2
#define SBC2_AUDIO_BUF_SIZE          SBC2_AUDIO_OUT_SIZE*SBC2_AUDIO_OUT_COUNT
#endif
#endif//_SBC2_AUDIO_BUF_X3
#define SBC2_RAM_ALLOCATE_SIZE       172 //172+320(SBC2TempBuf)
#define SBC2_RAM_ALLOCATE_SIZE_01    320
//#endif
#define SBC2_CH1_PLAY_RAM            ((SBC2_AUDIO_BUF_SIZE<<1)+(SBC2_RAM_ALLOCATE_SIZE<<1)+(SBC2_RAM_ALLOCATE_SIZE_01<<1))

// SBC-2 CH2	
#define SBC2_CH2_AUDIO_BUF_SIZE          480 //160*3, A-B-C buffer

#define SBC2_CH2_RAM_ALLOCATE_SIZE       172 //172+320(SBC2TempBuf)
#define SBC2_CH2_PLAY_RAM            ((SBC2_CH2_AUDIO_BUF_SIZE<<1)+(SBC2_CH2_RAM_ALLOCATE_SIZE<<1))


/* * *                          ADPCM RAM Size                            * * */
#define ADPCM_ECHO_DATA_LENGTH      45
#define ADPCM_ECHO_BUF_SIZE         ADPCM_ECHO_DATA_LENGTH*30

#define NEW_ADPCM_ECHO_DATA_LENGTH  32
#define ADPCM_ECHO_UD_TOTAL_DELAY_TIME		(_ADPCM_ECHO_UD_DELAY_TIME*_ADPCM_ECHO_UD_REPEAT_TIME)	//support 50~1200ms

#if (_ADPCM_ECHO_EFFECT==DELAY_USER_DEFINE)	
#define ADPCM_ECHO_UD_DELAY_TIME		_ADPCM_ECHO_UD_DELAY_TIME
#define ADPCM_ECHO_UD_REPEAT_TIME		_ADPCM_ECHO_UD_REPEAT_TIME
	#if ADPCM_ECHO_UD_TOTAL_DELAY_TIME<50
		#error "The total delay time must be set >50ms."
	#elif  ADPCM_ECHO_UD_TOTAL_DELAY_TIME>1200
		#error "The total delay time must be set <1200ms."
	#else
		#if ADPCM_ECHO_UD_TOTAL_DELAY_TIME>DELAY_800ms		//800ms~1200ms
			#if _ADPCM_ECHO_MAX_SAMPLE_RATE>SAMPLE_RATE_8000
				#error "The sample rate of total delay time 800ms~1200ms is limit at 8000Hz."
			#endif
		#elif ADPCM_ECHO_UD_TOTAL_DELAY_TIME>DELAY_320ms  	//320ms~800ms
			#if _ADPCM_ECHO_MAX_SAMPLE_RATE>SAMPLE_RATE_12000
				#error "The sample rate of total delay time 320ms~800ms is limit at 12000Hz."
			#endif
		#endif
	#endif	
#elif _ADPCM_ECHO_EFFECT==DELAY_1200ms		
#define ADPCM_ECHO_UD_DELAY_TIME		200
#define ADPCM_ECHO_UD_REPEAT_TIME		6
	#if _ADPCM_ECHO_MAX_SAMPLE_RATE>SAMPLE_RATE_8000
		#error "The sample rate of DELAY_1200ms is limit at 8000Hz."
	#endif
#elif _ADPCM_ECHO_EFFECT==DELAY_800ms	
#define ADPCM_ECHO_UD_DELAY_TIME		200
#define ADPCM_ECHO_UD_REPEAT_TIME		4
	#if _ADPCM_ECHO_MAX_SAMPLE_RATE>SAMPLE_RATE_12000
		#error "The sample rate of DELAY_1200ms is limit at 12000Hz."
	#endif
#elif _ADPCM_ECHO_EFFECT==DELAY_320ms	
#define ADPCM_ECHO_UD_DELAY_TIME		160
#define ADPCM_ECHO_UD_REPEAT_TIME		2
#endif

#if _ADPCM_ECHO_QUALITY
	#define NEW_ADPCM_ECHO_BUF_SIZE  ((((((_ADPCM_ECHO_MAX_SAMPLE_RATE*ADPCM_ECHO_UD_DELAY_TIME + 16000 )/32000)*21*ADPCM_ECHO_UD_REPEAT_TIME)+21+1)/2)+62)	//word
#else
	#define NEW_ADPCM_ECHO_BUF_SIZE  ((((((_ADPCM_ECHO_MAX_SAMPLE_RATE*ADPCM_ECHO_UD_DELAY_TIME + 8000 )/16000)*9*ADPCM_ECHO_UD_REPEAT_TIME)+9+1)/2)+62)		//word
#endif	
	
#if _ROBOT_SOUND_COMPRESS
#define ADPCM_ROBOT_BUF_SIZE        810
#define ADPCM_ROBOT02_BUF_SIZE      (36+320+1000)
#else
#define ADPCM_ROBOT_BUF_SIZE        1620
#define ADPCM_ROBOT02_BUF_SIZE      (36+320+2000)
#endif
#define ADPCM_REVERB_BUF_SIZE       830 //word

#define ANIMAL_ROAR_OUT_INDEX		3860
#define ANIMAL_ROAR_RAM_SIZE		4256

#define ADPCM_EQ_BUF_SIZE			24//dword


typedef struct _speedpitchbuf {
	S16 SpeedPitchBuf[540];		//Speed pitch temp buffer
	S16 OutBuf[180];			//Output buffer
	S16 CtlA[7];
	S16 CtlB[7];
	S16 WinSize;
	S8 SpeedPitchFlag;
	S8 PerformanceIndex;
}SPEEDPITCHBUF;

typedef struct _dinosaurctl {
     float SpeedScale;          	//Speed pitch control
     float PitchScale;          	//Speed pitch control
     S8 Reset;                  	//Speed pitch control
     S8 PerformanceIndex; 	     	//Speed pitch control
     S8 ReverbIndex;            	//Reverb control
     S8 EnvelopeIndex;      	    //Envelope control
}DINOSAURCTL;

#if _ADPCM_SPEED_CHANGE&&_ADPCM_PITCH_CHANGE
#define SPEEDPITCHBUFSIZE	(sizeof(SPEEDPITCHBUF))
#endif

#if _ADPCM_SPEEDPITCH_N
#define SpeedPitchNSIZE	(244)
#endif

#define ADPCM_ERASE_RECORD_SIZE		((_ADPCM_SECTOR_ERASE_MAXTIME*_ADPCM_RECORD_SAMPLE_RATE/1000+16)/32*21)

typedef S8(*t_ADPCMGetByte)(void);
#define		ADPCM_FRAME_SIZE		32
#define 	FILTER_BANK  			4  /* filter number */
#define		ADPCM_MODE_SIZE			4 //size of setmode parameter
#define		ADPCM_ENC_PARAM_SIZE	12 //size of AdpcmEncParam
#define		ADPCM_DEC_PARAM_SIZE	15 //size of AdpcmDecParam
#define		ADPCM_ENC_ALL_SIZE		(ADPCM_ENC_PARAM_SIZE + 4 * FILTER_BANK * ADPCM_FRAME_SIZE) //size all of Adpcm Encoder Parameter.
// CH1 
#if _ADPCM_ROBOT3_SOUND
#define ADPCM_AUDIO_OUT_SIZE        SPEED_CTL_OUT_SIZE
#define ADPCM_AUDIO_OUT_COUNT       8
#define ADPCM_AUDIO_BUF_SIZE        (ADPCM_AUDIO_OUT_SIZE*ADPCM_AUDIO_OUT_COUNT)
#elif _ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE||_ADPCM_DARTH_SOUND||_ADPCM_SPEEDPITCH_N||_ADPCM_ANIMAL_ROAR
#define ADPCM_AUDIO_OUT_SIZE        SPEED_CTL_OUT_SIZE
#define ADPCM_AUDIO_OUT_COUNT       8
#define ADPCM_AUDIO_BUF_SIZE        (ADPCM_AUDIO_OUT_SIZE*ADPCM_AUDIO_OUT_COUNT)
#elif (_ADPCM_ECHO_SOUND||_ADPCM_ROBOT2_SOUND||_ADPCM_ROBOT3_SOUND||_ADPCM_ROBOT4_SOUND||_ADPCM_REVERB_SOUND||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)) && _ADPCM_RECORD
#define ADPCM_AUDIO_OUT_SIZE        SPEED_CTL_OUT_SIZE
#define ADPCM_AUDIO_OUT_COUNT       4
#define ADPCM_AUDIO_BUF_SIZE        (ADPCM_AUDIO_OUT_SIZE*ADPCM_AUDIO_OUT_COUNT)
#elif (_ADPCM_ECHO_SOUND||_ADPCM_ROBOT2_SOUND||_ADPCM_ROBOT3_SOUND||_ADPCM_ROBOT4_SOUND||_ADPCM_REVERB_SOUND|| (_ADPCM_DECODE_FLOW_TYPE==TYPE1))
#define ADPCM_AUDIO_OUT_SIZE        SPEED_CTL_OUT_SIZE
#define ADPCM_AUDIO_OUT_COUNT       2
#define ADPCM_AUDIO_BUF_SIZE        (ADPCM_AUDIO_OUT_SIZE*ADPCM_AUDIO_OUT_COUNT)
#elif _ADPCM_PLAY_SR_FOR_SBC_REC
#define ADPCM_AUDIO_OUT_SIZE        SPEED_CTL_OUT_SIZE
#define TEMP_OUT_COUNT_FOR_DACFIFO  ((60*_ADPCM_PLAY_SR_FOR_SBC_REC/1000+(SPEED_CTL_OUT_SIZE*3/2))/SPEED_CTL_OUT_SIZE)
#if !_EF_SERIES && ((TEMP_OUT_COUNT_FOR_DACFIFO%2)!=0)
#define ADPCM_AUDIO_OUT_COUNT		(TEMP_OUT_COUNT_FOR_DACFIFO+1)
#else
#define ADPCM_AUDIO_OUT_COUNT		TEMP_OUT_COUNT_FOR_DACFIFO
#endif
#define ADPCM_AUDIO_BUF_SIZE        (ADPCM_AUDIO_OUT_SIZE*ADPCM_AUDIO_OUT_COUNT)
#elif _ADPCM_RECORD 
#define ADPCM_AUDIO_OUT_SIZE        ADPCM_DEC_OUT_SIZE
#define ADPCM_AUDIO_OUT_COUNT       2
#define ADPCM_AUDIO_BUF_SIZE        (ADPCM_AUDIO_OUT_SIZE*ADPCM_AUDIO_OUT_COUNT)
#endif



#define ADPCM_ROBOT4_CTL_SIZE		(sizeof(ROBOT4Ctl))
#define ADPCM_ROBOT4_BUF_SIZE		(sizeof(ROBOT4Buf))
#define ADPCM_ROBOT4_FFT_CTL_SIZE		(sizeof(FFTctl))


#define ADPCM_AUDIO_BUF_SIZE01		180//word(2*90)	

#ifndef PR_READ
#define PR_READ		ENABLE  //Buffer read the SPI Data for decode, enable for reduce loading but increate RAM(About 256~448Bytes by voice effect).
#endif

#define ADPCM_DATA_BUF_BASE_SIZE    64

#if PR_READ
#if _ADPCM_ROBOT3_SOUND || _ADPCM_ANIMAL_ROAR
#define ADPCM_DATA_BUF_SIZE         (ADPCM_DATA_BUF_BASE_SIZE*7)
#elif (_ADPCM_SPEED_CHANGE&&_ADPCM_PITCH_CHANGE&&_ADPCM_RECORD) 
#define ADPCM_DATA_BUF_SIZE         (ADPCM_DATA_BUF_BASE_SIZE*6)
#elif (_ADPCM_SPEED_CHANGE&&_ADPCM_PITCH_CHANGE) 
#define ADPCM_DATA_BUF_SIZE         (ADPCM_DATA_BUF_BASE_SIZE*5)
#else
#define ADPCM_DATA_BUF_SIZE         (ADPCM_DATA_BUF_BASE_SIZE*4)
#endif
#else//#if PR_READ
#if !(_ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC||_ADPCM_SPEEDPITCH_N||_ADPCM_ANIMAL_ROAR||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)) || _ADPCM_RECORD 
#define ADPCM_DATA_BUF_SIZE         (ADPCM_DATA_BUF_BASE_SIZE*4)
#else
#define ADPCM_DATA_BUF_SIZE         0
#endif//#if PR_READ
#endif

#if !(_ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC||_ADPCM_SPEEDPITCH_N||_ADPCM_ANIMAL_ROAR||(_ADPCM_DECODE_FLOW_TYPE==TYPE1))
#define ADPCM_CH1_PLAY_RAM			((ADPCM_DATA_BUF_SIZE)+((_ADPCM_PLAY_BACK_EQ||(_ADPCM_RECORD&&_ADPCM_RECORD_PLAY_EQ))?(ADPCM_EQ_BUF_SIZE<<2):(0))+(ADPCM_DEC_PARAM_SIZE)+(ADPCM_MODE_SIZE)) 
#elif  _ADPCM_ROBOT3_SOUND||_ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE||_ADPCM_DARTH_SOUND||_ADPCM_SPEEDPITCH_N||_ADPCM_ANIMAL_ROAR
#if PR_READ
#define ADPCM_CH1_PLAY_RAM          ((ADPCM_DATA_BUF_SIZE)+(ADPCM_AUDIO_BUF_SIZE<<1)+((_ADPCM_PLAY_BACK_EQ||(_ADPCM_RECORD&&_ADPCM_RECORD_PLAY_EQ))?(ADPCM_EQ_BUF_SIZE<<2):(0))+(ADPCM_DEC_PARAM_SIZE)+(ADPCM_MODE_SIZE))
#else
#define ADPCM_CH1_PLAY_RAM          ((ADPCM_AUDIO_BUF_SIZE<<1)+((_ADPCM_PLAY_BACK_EQ||(_ADPCM_RECORD&&_ADPCM_RECORD_PLAY_EQ))?(ADPCM_EQ_BUF_SIZE<<2):(0))+(ADPCM_DEC_PARAM_SIZE)+(ADPCM_MODE_SIZE))
#endif
#elif (_ADPCM_ROBOT2_SOUND||_ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC) && !(_ADPCM_ECHO_SOUND || _ADPCM_ROBOT4_SOUND)
#if PR_READ
#define ADPCM_CH1_PLAY_RAM          ((ADPCM_DATA_BUF_SIZE)+(ADPCM_AUDIO_BUF_SIZE<<1)+((_ADPCM_PLAY_BACK_EQ||(_ADPCM_RECORD&&_ADPCM_RECORD_PLAY_EQ))?(ADPCM_EQ_BUF_SIZE<<2):(0))+(ADPCM_DEC_PARAM_SIZE)+(ADPCM_MODE_SIZE))
#else
#define ADPCM_CH1_PLAY_RAM          ((ADPCM_AUDIO_BUF_SIZE<<1)+((_ADPCM_PLAY_BACK_EQ||(_ADPCM_RECORD&&_ADPCM_RECORD_PLAY_EQ))?(ADPCM_EQ_BUF_SIZE<<2):(0))+(ADPCM_DEC_PARAM_SIZE)+(ADPCM_MODE_SIZE))
#endif
#elif _ADPCM_ECHO_SOUND||_ADPCM_ROBOT2_SOUND||_ADPCM_ROBOT4_SOUND||_ADPCM_REVERB_SOUND||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)
#if PR_READ
#define ADPCM_CH1_PLAY_RAM          ((ADPCM_DATA_BUF_SIZE)+(ADPCM_AUDIO_BUF_SIZE<<1)+(ADPCM_AUDIO_BUF_SIZE01<<1)+((_ADPCM_PLAY_BACK_EQ||(_ADPCM_RECORD&&_ADPCM_RECORD_PLAY_EQ))?(ADPCM_EQ_BUF_SIZE<<2):(0))+(ADPCM_DEC_PARAM_SIZE)+(ADPCM_MODE_SIZE))
#else
#define ADPCM_CH1_PLAY_RAM          ((ADPCM_AUDIO_BUF_SIZE<<1)+(ADPCM_AUDIO_BUF_SIZE01<<1)+((_ADPCM_PLAY_BACK_EQ||(_ADPCM_RECORD&&_ADPCM_RECORD_PLAY_EQ))?(ADPCM_EQ_BUF_SIZE<<2):(0))+(ADPCM_DEC_PARAM_SIZE)+(ADPCM_MODE_SIZE))
#endif
#endif 

#if _ADPCM_RECORD_ERASING
#define ADPCM_CH1_REC_RAM           (ADPCM_DATA_BUF_SIZE+ADPCM_ENC_ALL_SIZE+(ADPCM_AUDIO_BUF_SIZE<<1)+(AGC_FRAME_SIZE<<1)+ADPCM_ERASE_RECORD_SIZE)
#else
#define ADPCM_CH1_REC_RAM           (ADPCM_DATA_BUF_SIZE+ADPCM_ENC_ALL_SIZE+(ADPCM_AUDIO_BUF_SIZE<<1)+(AGC_FRAME_SIZE<<1))//
#endif

#if (_ADPCM_SPEED_CHANGE&&_ADPCM_PITCH_CHANGE)
#define ADPCM_CH1_PITCH_RAM         (((SPEED_CTL_FRAME_SIZE+130)<<1)+SPEEDPITCHBUFSIZE)
#else
#define ADPCM_CH1_PITCH_RAM         (SPEED_CTL_FRAME_SIZE<<1)
#endif

#if _ADPCM_ANIMAL_ROAR
#define ADPCM_CH1_ANIMAL_ROAR_RAM		(((SPEED_CTL_FRAME_SIZE+130)<<1)+ANIMAL_ROAR_RAM_SIZE)
#endif

#define ADPCM_CH1_ROBOT2_RAM       (ADPCM_ROBOT_BUF_SIZE<<1)
#define ADPCM_CH1_ROBOT3_RAM       ((ADPCM_ROBOT02_BUF_SIZE<<1)+ ((SPEED_CTL_FRAME_SIZE+40)<<1))

#if (_ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE||_ADPCM_DARTH_SOUND||_ADPCM_SPEEDPITCH_N)
#define ADPCM_CH1_ROBOT4_RAM       (ADPCM_ROBOT4_BUF_SIZE+ADPCM_ROBOT4_FFT_CTL_SIZE+ADPCM_ROBOT4_CTL_SIZE+((SPEED_CTL_FRAME_SIZE)<<1))
#else
#define ADPCM_CH1_ROBOT4_RAM       (ADPCM_ROBOT4_BUF_SIZE+ADPCM_ROBOT4_FFT_CTL_SIZE+ADPCM_ROBOT4_CTL_SIZE)
#endif

#define ADPCM_CH1_REVERB_RAM        (ADPCM_REVERB_BUF_SIZE<<2)
#define ADPCM_CH1_ECHO_RAM          (NEW_ADPCM_ECHO_BUF_SIZE<<1)

#if _ADPCM_DARTH_SOUND
#define ADPCM_CH1_DARTH_RAM         (sizeof(DARTH_VADER_CTRL_T)+sizeof(DARTH_VADER_RAM_T)+(SPEED_CTL_FRAME_SIZE<<1))
#endif

#if _ADPCM_SPEEDPITCH_N
#define ADPCM_CH1_SPN_RAM			(SpeedPitchNSIZE+(SPEED_CTL_FRAME_SIZE<<1))
#endif

#if (_ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE)||_ADPCM_ANIMAL_ROAR
#if (_ADPCM_SPEED_CHANGE&&_ADPCM_PITCH_CHANGE)||_ADPCM_ANIMAL_ROAR
#define ADPCM_SP_STACK_SIZE   416
#define ADPCM_ANIMAL_ROAR_STACK_SIZE	416
#else
#define ADPCM_SP_STACK_SIZE   368
#endif
#endif

#if (_ADPCM_DECODE_FLOW_TYPE==TYPE0)
#define	ADPCM_PLAY_STACK_SIZE	292
#else	
#define	ADPCM_PLAY_STACK_SIZE	268
#endif

#if (	_ADPCM_CH2_PLAY || _ADPCM_CH3_PLAY\
	 || _ADPCM_CH4_PLAY || _ADPCM_CH5_PLAY\
	 || _ADPCM_CH6_PLAY || _ADPCM_CH7_PLAY\
	 || _ADPCM_CH8_PLAY )
	 	#define _ADPCM_CHX_PLAY		ENABLE
#else
		#define _ADPCM_CHX_PLAY		DISABLE
#endif

#if (	_ADPCM_CH2_PLAY_BACK_EQ || _ADPCM_CH3_PLAY_BACK_EQ\
	 || _ADPCM_CH4_PLAY_BACK_EQ || _ADPCM_CH5_PLAY_BACK_EQ\
	 || _ADPCM_CH6_PLAY_BACK_EQ || _ADPCM_CH7_PLAY_BACK_EQ\
	 || _ADPCM_CH8_PLAY_BACK_EQ )
	 	#define _ADPCM_CHX_PLAY_BACK_EQ		ENABLE
#else
		#define _ADPCM_CHX_PLAY_BACK_EQ		DISABLE
#endif

#ifndef PR_READ_CHX
#define PR_READ_CHX		ENABLE //Buffer read the SPI Data for decode, enable for reduce loading but increate RAM(About 256Bytes by voice effect).
#endif

#if _ADPCM_CHX_PLAY
#define ADPCM_CHX_AUDIO_OUT_SIZE    SPEED_CTL_OUT_SIZE
#define ADPCM_CHX_AUDIO_OUT_COUNT   2
#define ADPCM_CHX_AUDIO_BUF_SIZE    (ADPCM_CHX_AUDIO_OUT_SIZE*ADPCM_CHX_AUDIO_OUT_COUNT)
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE0)||(PR_READ_CHX&&(_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1))
#define ADPCM_CHX_DATA_BUF_BASE_SIZE    64
#define ADPCM_CHX_DATA_BUF_SIZE     	(ADPCM_CHX_DATA_BUF_BASE_SIZE*4)
#else
#define ADPCM_CHX_DATA_BUF_SIZE     0
#endif
#endif //_ADPCM_CHX_PLAY

#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1) 
#if PR_READ_CHX
#define ADPCM_CHX_PLAY_RAM          ((ADPCM_CHX_DATA_BUF_SIZE)+(ADPCM_CHX_AUDIO_BUF_SIZE<<1)+((_ADPCM_CHX_PLAY_BACK_EQ)?(ADPCM_EQ_BUF_SIZE<<2):(0))+(ADPCM_DEC_PARAM_SIZE)+(ADPCM_MODE_SIZE))
#else
#define ADPCM_CHX_PLAY_RAM          ((ADPCM_CHX_AUDIO_BUF_SIZE<<1)+((_ADPCM_CHX_PLAY_BACK_EQ)?(ADPCM_EQ_BUF_SIZE<<2):(0))+(ADPCM_DEC_PARAM_SIZE)+(ADPCM_MODE_SIZE))
#endif
#else 
#define ADPCM_CHX_PLAY_RAM          ((ADPCM_CHX_DATA_BUF_SIZE)+((_ADPCM_CHX_PLAY_BACK_EQ)?(ADPCM_EQ_BUF_SIZE<<2):(0))+(ADPCM_DEC_PARAM_SIZE)+(ADPCM_MODE_SIZE))
#endif


#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE0)
#define	ADPCM_CH2_PLAY_STACK_SIZE	388
#else	
#define	ADPCM_CH2_PLAY_STACK_SIZE	376
#endif


#define	 ADPCM_CHX_NUM		(_ADPCM_PLAY+_ADPCM_CH2_PLAY+_ADPCM_CH3_PLAY+_ADPCM_CH4_PLAY+_ADPCM_CH5_PLAY+_ADPCM_CH6_PLAY+_ADPCM_CH7_PLAY+_ADPCM_CH8_PLAY)


#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE0)&&(_ADPCM_DECODE_FLOW_TYPE==TYPE0)
#if (ADPCM_CHX_NUM==8)
#define	ADPCM_CHX_PLAY_STACK_SIZE	444
#elif (ADPCM_CHX_NUM==7)
#define	ADPCM_CHX_PLAY_STACK_SIZE	444
#elif (ADPCM_CHX_NUM==6)
#define	ADPCM_CHX_PLAY_STACK_SIZE	444
#elif (ADPCM_CHX_NUM==5)
#define	ADPCM_CHX_PLAY_STACK_SIZE	444
#elif (ADPCM_CHX_NUM==4)
#define	ADPCM_CHX_PLAY_STACK_SIZE	396
#elif (ADPCM_CHX_NUM==3)
#define	ADPCM_CHX_PLAY_STACK_SIZE	388
#elif (ADPCM_CHX_NUM<3)
#define	ADPCM_CHX_PLAY_STACK_SIZE	388
#endif
#elif (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)&&(_ADPCM_DECODE_FLOW_TYPE==TYPE1)
#if (ADPCM_CHX_NUM==8)
#define	ADPCM_CHX_PLAY_STACK_SIZE	400
#elif (ADPCM_CHX_NUM==7)
#define	ADPCM_CHX_PLAY_STACK_SIZE	400
#elif (ADPCM_CHX_NUM==6)
#define	ADPCM_CHX_PLAY_STACK_SIZE	400
#elif (ADPCM_CHX_NUM==5)
#define	ADPCM_CHX_PLAY_STACK_SIZE	400
#elif (ADPCM_CHX_NUM==4)
#define	ADPCM_CHX_PLAY_STACK_SIZE	400
#elif (ADPCM_CHX_NUM==3)
#define	ADPCM_CHX_PLAY_STACK_SIZE	376
#elif (ADPCM_CHX_NUM<3)
#define	ADPCM_CHX_PLAY_STACK_SIZE	376
#endif
#else
#define	ADPCM_CHX_PLAY_STACK_SIZE	444		// If ADPCM ch1 decode flow set is different than ADPCM CHX that the stack size will force set to maximum.
#endif	//(_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE0)&&(_ADPCM_DECODE_FLOW_TYPE==TYPE0)


/* * *                         CELP RAM Size                            * * */
#if _CELP_SPEED_CHANGE||_CELP_PITCH_CHANGE
#define CELP_AUDIO_OUT_SIZE         SPEED_CTL_OUT_SIZE
#define CELP_AUDIO_OUT_COUNT        6
#define CELP_AUDIO_BUF_SIZE         (CELP_AUDIO_OUT_SIZE*CELP_AUDIO_OUT_COUNT)
#elif _CELP_ROBOT3_SOUND
#define CELP_AUDIO_OUT_SIZE         320//CELP_DEC_OUT_SIZE
#define CELP_AUDIO_OUT_COUNT        2
#define CELP_AUDIO_BUF_SIZE         (CELP_AUDIO_OUT_SIZE*CELP_AUDIO_OUT_COUNT)
#else
#define CELP_AUDIO_OUT_SIZE         CELP_DEC_OUT_SIZE
#define CELP_AUDIO_OUT_COUNT        2
#define CELP_AUDIO_BUF_SIZE         (CELP_AUDIO_OUT_SIZE*CELP_AUDIO_OUT_COUNT)
#endif
#if _CELP_ROBOT3_SOUND
#define CELP_SOUND_EFFECT_SIZE      320
#else
#define CELP_SOUND_EFFECT_SIZE      CELP_DEC_OUT_SIZE
#endif
#define CELP_RAM_ALLOCATE_SIZE      487
#define CELP_ECHO_DATA_LENGTH       40
#define CELP_ECHO_BUF_SIZE          CELP_ECHO_DATA_LENGTH*30
#if _ROBOT_SOUND_COMPRESS
#define CELP_ROBOT_BUF_SIZE         800
#else
#define CELP_ROBOT_BUF_SIZE         1600
#endif
#if _ROBOT_SOUND_COMPRESS
#define CELP_ROBOT02_BUF_SIZE       (36+320+1000)
#else
#define CELP_ROBOT02_BUF_SIZE       (36+320+2000)
#endif
#define CELP_REVERB_BUF_SIZE        830 //word

#define CELP_PLAY_RAM               ((CELP_AUDIO_BUF_SIZE<<1)+(CELP_RAM_ALLOCATE_SIZE<<1)+CELP_DEC_IN_SIZE)
#if _CELP_ROBOT3_SOUND
#define CELP_PITCH_RAM              ((320+SPEED_CTL_FRAME_SIZE)<<1)
#else
#define CELP_PITCH_RAM              ((CELP_DEC_OUT_SIZE+SPEED_CTL_FRAME_SIZE)<<1)
#endif
#define CELP_ROBOT2_RAM             (CELP_ROBOT_BUF_SIZE<<1)
#define CELP_ROBOT3_RAM             (CELP_ROBOT02_BUF_SIZE<<1)
#define CELP_REVERB_RAM             (CELP_REVERB_BUF_SIZE<<2)
#define CELP_ECHO_RAM               (CELP_ECHO_BUF_SIZE<<1)

/* * *                         PCM RAM Size                            * * */
#define PCM_ECHO_DATA_LENGTH        40
#define PCM_ECHO_BUF_SIZE           PCM_ECHO_DATA_LENGTH*30
#if _ROBOT_SOUND_COMPRESS
#define PCM_ROBOT_BUF_SIZE          800
#define PCM_ROBOT02_BUF_SIZE        (36+320+1000)
#else
#define PCM_ROBOT_BUF_SIZE          1600
#define PCM_ROBOT02_BUF_SIZE        (36+320+2000)
#endif
#if _PCM_ROBOT3_SOUND
#define SOUND_EFFECT_SIZE           320
#else
#define SOUND_EFFECT_SIZE           PCM_DEC_OUT_SIZE
#endif
#define PCM_REVERB_BUF_SIZE         830 //word
#ifndef	PCM_WITHOUT_SFX_AUDIO_OUT_COUNT
#define PCM_WITHOUT_SFX_AUDIO_OUT_COUNT	8
#endif

// CH1
#if _PCM_SPEED_CHANGE||_PCM_PITCH_CHANGE
#define PCM_AUDIO_OUT_SIZE          SPEED_CTL_OUT_SIZE
#define PCM_AUDIO_OUT_COUNT         6
#define PCM_AUDIO_BUF_SIZE          (PCM_AUDIO_OUT_SIZE*PCM_AUDIO_OUT_COUNT)
#elif _PCM_ROBOT3_SOUND
#define PCM_AUDIO_OUT_SIZE          320
#define PCM_AUDIO_OUT_COUNT         2
#define PCM_AUDIO_BUF_SIZE          (PCM_AUDIO_OUT_SIZE*PCM_AUDIO_OUT_COUNT)
#else
#define PCM_AUDIO_OUT_SIZE          PCM_DEC_OUT_SIZE
#define PCM_AUDIO_OUT_COUNT         PCM_WITHOUT_SFX_AUDIO_OUT_COUNT
#define PCM_AUDIO_BUF_SIZE          (PCM_AUDIO_OUT_SIZE*PCM_AUDIO_OUT_COUNT)
#endif

#define PCM_CH1_PLAY_RAM            ((PCM_AUDIO_BUF_SIZE<<1)) 
#define PCM_CH1_REC_RAM             ((PCM_AUDIO_BUF_SIZE<<1)+(AGC_FRAME_SIZE<<1))
#if _PCM_ROBOT3_SOUND
#define PCM_CH1_PITCH_RAM           ((320+SPEED_CTL_FRAME_SIZE)<<1)
#else
#define PCM_CH1_PITCH_RAM           ((PCM_DEC_OUT_SIZE+SPEED_CTL_FRAME_SIZE)<<1)
#endif
#define PCM_CH1_ROBOT2_RAM          (PCM_ROBOT_BUF_SIZE<<1)
#define PCM_CH1_ROBOT3_RAM          (PCM_ROBOT02_BUF_SIZE<<1)
#define PCM_CH1_REVERB_RAM          (PCM_REVERB_BUF_SIZE<<2)
#define PCM_CH1_ECHO_RAM            (PCM_ECHO_BUF_SIZE<<1)

// CH2
#if _PCM_CH2_SPEED_CHANGE||_PCM_CH2_PITCH_CHANGE
#define PCM_CH2_AUDIO_OUT_SIZE      SPEED_CTL_OUT_SIZE
#define PCM_CH2_AUDIO_OUT_COUNT     6
#define PCM_CH2_AUDIO_BUF_SIZE      (PCM_CH2_AUDIO_OUT_SIZE*PCM_CH2_AUDIO_OUT_COUNT)
#elif _PCM_CH2_ROBOT3_SOUND
#define PCM_CH2_AUDIO_OUT_SIZE      320
#define PCM_CH2_AUDIO_OUT_COUNT     2
#define PCM_CH2_AUDIO_BUF_SIZE      (PCM_CH2_AUDIO_OUT_SIZE*PCM_CH2_AUDIO_OUT_COUNT)
#else
#define PCM_CH2_AUDIO_OUT_SIZE      PCM_DEC_OUT_SIZE
#define PCM_CH2_AUDIO_OUT_COUNT     PCM_WITHOUT_SFX_AUDIO_OUT_COUNT
#define PCM_CH2_AUDIO_BUF_SIZE      (PCM_CH2_AUDIO_OUT_SIZE*PCM_CH2_AUDIO_OUT_COUNT)
#endif

#define PCM_CH2_PLAY_RAM            ((PCM_CH2_AUDIO_BUF_SIZE<<1)) 
#if _PCM_CH2_ROBOT3_SOUND
#define PCM_CH2_PITCH_RAM           ((320+SPEED_CTL_FRAME_SIZE)<<1)
#else
#define PCM_CH2_PITCH_RAM           ((PCM_DEC_OUT_SIZE+SPEED_CTL_FRAME_SIZE)<<1)
#endif
#define PCM_CH2_ROBOT2_RAM          (PCM_ROBOT_BUF_SIZE<<1)
#define PCM_CH2_ROBOT3_RAM          (PCM_ROBOT02_BUF_SIZE<<1)
#define PCM_CH2_REVERB_RAM          (PCM_REVERB_BUF_SIZE<<2)
#define PCM_CH2_ECHO_RAM            (PCM_ECHO_BUF_SIZE<<1)

//CH3
#if _PCM_CH3_SPEED_CHANGE||_PCM_CH3_PITCH_CHANGE
#define PCM_CH3_AUDIO_OUT_SIZE      SPEED_CTL_OUT_SIZE
#define PCM_CH3_AUDIO_OUT_COUNT     6
#define PCM_CH3_AUDIO_BUF_SIZE      (PCM_CH3_AUDIO_OUT_SIZE*PCM_CH3_AUDIO_OUT_COUNT)
#elif _PCM_CH3_ROBOT3_SOUND
#define PCM_CH3_AUDIO_OUT_SIZE      320
#define PCM_CH3_AUDIO_OUT_COUNT     2
#define PCM_CH3_AUDIO_BUF_SIZE      (PCM_CH3_AUDIO_OUT_SIZE*PCM_CH3_AUDIO_OUT_COUNT)
#else
#define PCM_CH3_AUDIO_OUT_SIZE      PCM_DEC_OUT_SIZE
#define PCM_CH3_AUDIO_OUT_COUNT     PCM_WITHOUT_SFX_AUDIO_OUT_COUNT
#define PCM_CH3_AUDIO_BUF_SIZE      (PCM_CH3_AUDIO_OUT_SIZE*PCM_CH3_AUDIO_OUT_COUNT)
#endif

#define PCM_CH3_PLAY_RAM            ((PCM_CH3_AUDIO_BUF_SIZE<<1)) 
#if _PCM_CH3_ROBOT3_SOUND
#define PCM_CH3_PITCH_RAM           ((320+SPEED_CTL_FRAME_SIZE)<<1)
#else
#define PCM_CH3_PITCH_RAM           ((PCM_DEC_OUT_SIZE+SPEED_CTL_FRAME_SIZE)<<1)
#endif
#define PCM_CH3_ROBOT2_RAM          (PCM_ROBOT_BUF_SIZE<<1)
#define PCM_CH3_ROBOT3_RAM          (PCM_ROBOT02_BUF_SIZE<<1)
#define PCM_CH3_REVERB_RAM          (PCM_REVERB_BUF_SIZE<<2)
#define PCM_CH3_ECHO_RAM            (PCM_ECHO_BUF_SIZE<<1)

/* * *                         RT RAM Size                            * * */
//--- Pitch change ---
#define RT_AUDIO_OUT_SIZE_01		80
#define RT_AUDIO_OUT_COUNT_01		4//3
#define RT_AUDIO_BUF_SIZE_01		(RT_AUDIO_OUT_SIZE_01*RT_AUDIO_OUT_COUNT_01)
#define DAC_BUF_COUNT_01			6
#define DAC_BUF_SIZE_01				(RT_AUDIO_OUT_SIZE_01*DAC_BUF_COUNT_01)
//--- Reverb & Robot ---
#define RT_AUDIO_OUT_SIZE_02		320
#define RT_AUDIO_OUT_COUNT_02		2
#define RT_AUDIO_BUF_SIZE_02		(RT_AUDIO_OUT_SIZE_02*RT_AUDIO_OUT_COUNT_02)
#define DAC_BUF_COUNT_02			2
#define DAC_BUF_SIZE_02				(RT_AUDIO_OUT_SIZE_02*DAC_BUF_COUNT_02)
//--- Echo & NO_SFX---
#define RT_AUDIO_OUT_SIZE_03		64//96//128//160
#if		_RT_SAMPLE_RATE==SAMPLE_RATE_8000
#define RT_AUDIO_OUT_COUNT_03		3   
#elif   _RT_SAMPLE_RATE==SAMPLE_RATE_12000
#define RT_AUDIO_OUT_COUNT_03		4   
#elif   _RT_SAMPLE_RATE==SAMPLE_RATE_16000
#define RT_AUDIO_OUT_COUNT_03		5   
#endif
#define RT_AUDIO_BUF_SIZE_03		(RT_AUDIO_OUT_SIZE_03*RT_AUDIO_OUT_COUNT_03)
#if		_RT_SAMPLE_RATE==SAMPLE_RATE_8000
#define DAC_BUF_COUNT_03			3
#elif	_RT_SAMPLE_RATE==SAMPLE_RATE_12000
#define DAC_BUF_COUNT_03			4
#elif	_RT_SAMPLE_RATE==SAMPLE_RATE_16000
#define DAC_BUF_COUNT_03			5
#endif
#define DAC_BUF_SIZE_03				(RT_AUDIO_OUT_SIZE_03*DAC_BUF_COUNT_03)

#if	_RT_REVERB_SOUND || _RT_ROBOT1_SOUND || _RT_ROBOT2_SOUND || _RT_ROBOT3_SOUND || _RT_ROBOT4_SOUND
#define RT_AUDIO_BUF_SIZE			RT_AUDIO_BUF_SIZE_02
#define DAC_BUF_SIZE				DAC_BUF_SIZE_02
#else	
#define RT_AUDIO_BUF_SIZE			RT_AUDIO_BUF_SIZE_01
#define DAC_BUF_SIZE				DAC_BUF_SIZE_01
#endif

#define RT_PITCH_BUF_SIZE			960

#define RT_ECHO_DATA_LENGTH         32

#define RT_ECHO_UD_TOTAL_DELAY_TIME		(_RT_ECHO_UD_DELAY_TIME*_RT_ECHO_UD_REPEAT_TIME)	//support 50~1200ms

#if (_RT_ECHO_EFFECT==DELAY_USER_DEFINE)	
#define RT_ECHO_UD_DELAY_TIME		_RT_ECHO_UD_DELAY_TIME
#define RT_ECHO_UD_REPEAT_TIME		_RT_ECHO_UD_REPEAT_TIME
	#if RT_ECHO_UD_TOTAL_DELAY_TIME<50
		#error "The total delay time must be set >50ms."
	#elif  RT_ECHO_UD_TOTAL_DELAY_TIME>1200
		#error "The total delay time must be set <1200ms."
	#else
		#if RT_ECHO_UD_TOTAL_DELAY_TIME>DELAY_800ms		//800ms~1200ms
			#if _RT_SAMPLE_RATE>SAMPLE_RATE_8000
				#error "The sample rate of total delay time 800ms~1200ms is limit at 8000Hz."
			#endif
		#elif RT_ECHO_UD_TOTAL_DELAY_TIME>DELAY_320ms  	//320ms~800ms
			#if _RT_SAMPLE_RATE>SAMPLE_RATE_12000
				#error "The sample rate of total delay time 320ms~800ms is limit at 12000Hz."
			#endif
		#endif
	#endif	
#elif _RT_ECHO_EFFECT==DELAY_1200ms		
#define RT_ECHO_UD_DELAY_TIME		200
#define RT_ECHO_UD_REPEAT_TIME		6
	#if _RT_SAMPLE_RATE>SAMPLE_RATE_8000
		#error "The sample rate of DELAY_1200ms is limit at 8000Hz."
	#endif
#elif _RT_ECHO_EFFECT==DELAY_800ms	
#define RT_ECHO_UD_DELAY_TIME		200
#define RT_ECHO_UD_REPEAT_TIME		4
	#if _RT_SAMPLE_RATE>SAMPLE_RATE_12000
		#error "The sample rate of DELAY_1200ms is limit at 12000Hz."
	#endif
#elif _RT_ECHO_EFFECT==DELAY_320ms	
#define RT_ECHO_UD_DELAY_TIME		160
#define RT_ECHO_UD_REPEAT_TIME		2
#endif

#if _RT_ECHO_QUALITY
	#define RT_ECHO_BUF_SIZE  ((((((_RT_SAMPLE_RATE*RT_ECHO_UD_DELAY_TIME + 16000 )/32000)*21*RT_ECHO_UD_REPEAT_TIME)+21+1)/2)+62)	//word
#else
	#define RT_ECHO_BUF_SIZE  ((((((_RT_SAMPLE_RATE*RT_ECHO_UD_DELAY_TIME + 8000 )/16000)*9*RT_ECHO_UD_REPEAT_TIME)+9+1)/2)+62)	//word
#endif	

#if _ROBOT_SOUND_COMPRESS
#define RT_ROBOT_BUF_SIZE           800
#define RT_ROBOT02_BUF_SIZE         (36+320+1000)
#else
#define RT_ROBOT_BUF_SIZE           1600
#define RT_ROBOT02_BUF_SIZE         (36+320+2000)
#endif


#define RT_ROBOT4_CTL_SIZE		(sizeof(ROBOT4Ctl))
#define RT_ROBOT4_BUF_SIZE		(sizeof(ROBOT4Buf))
#define RT_ROBOT4_FFT_CTL_SIZE		(sizeof(FFTctl))

#define RT_REVERB_BUF_SIZE          830 	//word
#define RT_GHOST_REVERB_BUF_SIZE	830		
#define RT_GHOST_ENVELOPETH_SIZE	8

#define RT_DARTH_CTL_SIZE		(sizeof(RT_DarthVaderBuf))
#define RT_DARTH_BUF_SIZE		(sizeof(RT_DarthVaderCtl))

#define RT_CHORUS_DEALY_SAMPLE_1	((_RT_SAMPLE_RATE*_RT_CHORUS_SOUND_DELAY1)/1000)
#define RT_CHORUS_DEALY_SAMPLE_2	((_RT_SAMPLE_RATE*_RT_CHORUS_SOUND_DELAY2)/1000)
#if (RT_CHORUS_DEALY_SAMPLE_1%160==0)
#define RT_CHORUS_MOD_OFFSET1	0
#else
#define RT_CHORUS_MOD_OFFSET1	1
#endif
#if (RT_CHORUS_DEALY_SAMPLE_2%160==0)
#define RT_CHORUS_MOD_OFFSET2	0
#else
#define RT_CHORUS_MOD_OFFSET2	1
#endif
#define RT_CHORUS_DEALY_BUF_SIZE1	(160*((RT_CHORUS_DEALY_SAMPLE_1/160)+RT_CHORUS_MOD_OFFSET1))//+160*(RT_CHORUS_DEALY_SAMPLE_1%160))//(160*(_RT_SAMPLE_RATE*(_RT_CHORUS_SOUND_DELAY1/1000)/160)+160*9/10)	
#define RT_CHORUS_DEALY_BUF_SIZE2	RT_CHORUS_DEALY_BUF_SIZE1+160*((RT_CHORUS_DEALY_SAMPLE_2/160)+RT_CHORUS_MOD_OFFSET2)//+160*(RT_CHORUS_DEALY_SAMPLE_2%160))//(_RT_CHORUS_SOUND_DELAY1+160*(_RT_SAMPLE_RATE*(_RT_CHORUS_SOUND_DELAY2/1000)/160)+160*9/10)


#define RT_CHORUS_BUF_SIZE	  		(56+(640+RT_CHORUS_DEALY_BUF_SIZE1)+RT_CHORUS_DEALY_BUF_SIZE2)//1656 //Sample rate§i?
#define RT_CHORUS_CTL_SIZE			(sizeof(RTChorusCtl))
#if _RT_PLAY && _RT_HOWLING_SUPPRESS
#define RT_HOWLING_RAM              ((((8+sizeof(FFTctl)+sizeof(HRCtlBuf))+3)>>2)<<2)
#else
#define RT_HOWLING_RAM 0
#endif
#define RT_BASE_RAM					((RT_AUDIO_BUF_SIZE<<1)+((AGC_FRAME_SIZE)<<1)+(DAC_BUF_SIZE<<1)+RT_HOWLING_RAM)		
#define RT_PITCH_RAM				((RT_PITCH_BUF_SIZE<<1)+RT_BASE_RAM)
#define RT_ROBOT1_RAM				(RT_BASE_RAM)
#define RT_ROBOT2_RAM				((RT_ROBOT_BUF_SIZE<<1)+RT_BASE_RAM)
#define RT_ROBOT3_RAM 				((RT_ROBOT02_BUF_SIZE<<1)+RT_BASE_RAM)
#define RT_ROBOT4_RAM 				(RT_ROBOT4_BUF_SIZE+RT_ROBOT4_FFT_CTL_SIZE+RT_ROBOT4_CTL_SIZE+RT_BASE_RAM)
#define RT_REVERB_RAM				((RT_REVERB_BUF_SIZE<<2)+RT_BASE_RAM)

#define RT_ECHO_RAM					((RT_ECHO_BUF_SIZE<<1)+RT_BASE_RAM)
#define RT_GHOST_RAM				((RT_GHOST_ENVELOPETH_SIZE<<2)+(RT_GHOST_REVERB_BUF_SIZE<<1)*2+(RT_PITCH_BUF_SIZE<<1)+RT_BASE_RAM)
#define RT_DARTH_RAM				(RT_DARTH_CTL_SIZE+RT_DARTH_BUF_SIZE+(RT_PITCH_BUF_SIZE<<1)+RT_BASE_RAM)
#define RT_CHORUS_RAM				((RT_CHORUS_BUF_SIZE<<1)+(RT_CHORUS_CTL_SIZE<<1)+(RT_PITCH_BUF_SIZE<<1)+RT_BASE_RAM)

/* * *                         WaveID RAM Size                        * * */
#define WID_OUT_SIZE				256
#define WID_OUT_COUNT				3
#define WID_BUF_SIZE_01				(WID_OUT_SIZE*WID_OUT_COUNT)
#define WID_BUF_SIZE_02				18  	//byte				
#define WID_BUF_SIZE_03				10		//byte
#define WID_BUF_SIZE_04				53		//byte
#define WID_BUF_SIZE_05				8		//word

#define WID_PLAY_RAM				((WID_BUF_SIZE_01<<1) + (WID_OUT_SIZE<<2) + ((WID_BUF_SIZE_02+3)&0xFFFC) + ((WID_BUF_SIZE_03+3)&0xFFFC) \
									+((WID_BUF_SIZE_04+3)&0xFFFC) + ((WID_BUF_SIZE_05<<2)*2)) //	PS. (WID_BUF_SIZE_05)*2, include DC_REMOVE
									
#define HEADER_LEN          32
#define SIGNATURE_LEN       3

#define MEMORY_ALLOCATE_PLAY        (1<<0)
#define MEMORY_ALLOCATE_RECORD      (1<<1)
#define MEMORY_ALLOCATE_VR          (1<<2)

#if _EF_SERIES && OPTION_IRAM && ((_ADPCM_RECORD && _ADPCM_RECORD_ERASING) || (_SBC_RECORD && _SBC_RECORD_ERASING))
#define SCOPE_TYPE
#define PARA_TYPE
#else
#define SCOPE_TYPE static
#define PARA_TYPE  const
#endif		

#ifndef  __ASSEMBLY__


struct AUDIO_HEADER{
    U8   signature[SIGNATURE_LEN]; 
    U8   header_size;
    U32  file_size;
    U32  samples;
    U32  nyq_offset;
    U16  decode_in_size;     //byte
    U16  decode_out_size;    //word  
    U16  bit_per_frame;
    U16  sample_rate;
    U16  bandwidth;
    U8   file_type; 
    U8   codec; 
	U8	 flag;
};
typedef struct {
    U16     status;
    U16     total_count;
    U32     start_addr;
    U32     length;
    U32     erase_addr;
    U8      *record_buf;
    U16     record_buf_index;
    U16     active_channel;       // active_channel   need initial to 0xFFFF
    U16     waiting_channel;      //  need initial to 0xFFFF
    U16     count;
}RecordErasingCtrl_T;

#if _SBC_ECHO_SOUND || _ADPCM_ECHO_SOUND || _CELP_ECHO_SOUND || _PCM_ECHO_SOUND || _SBC_CH2_ECHO_SOUND || _RT_ECHO_SOUND 
typedef struct{
    S16     *buf;
    U16     count;
    S16     EchoBufL;
    S16     EchoNB;
    S16     EchoWT;
    S16     RingBufPeriod;
    U8      frameCount;
    S8      flag;
}EchoCtl_T;

typedef struct _newechoctrl {		
	U8	Cmd;
	U8	bitmode;
	U8  iEchoRepeatTimes;
	U16 iOneEchoDelayTime;		//ms
	U16 iEchoTotalDelayTime;	//ms
	S16	iOneEchoDlyLength;
	S16	iEncFrameSize;		//x
	S16	iRingPeriod;		//x
	S16 iTotalEchoBufLength;
	S32 iSampleRate;
}NewEchoCtrl;
#endif

typedef struct _drc {
	S32 KneeWidth;			//dB of fixed point
	S32 KneeWidth_half;		//dB of fixed point
	S32 KneeStart;			//dB of fixed point
	S32 KneeEnd;			//dB of fixed point
	S32 Threshold;			//dB of fixed point
	S16 InSize;
	S16 AttackTau;			//The coefficient tau of attack time
	S16 ReleaseMaxTau;		//The coefficient tau of Release time
	S16 ReleaseMinTau;		//The coefficient tau of Release time
	S16 ReleaseAvgTau;		//The coefficient tau of Release time
	S16 LimTh;
	S32 SampleRate;
	S16 Slope;				// 1/(Ratio)
	S16	Alpha;
	S32 BoostGain;
	S16 AmpdBLevel;
	S16 InverseKneeWidth;
	U16 RMSCalculateSize;
	float InputThreshold;
	float InputRatio;
}DRCFixedCtl;

typedef struct {
	S32 SampleRate;			//data from AudioBufCtl.sample_rate, need modify type from U16 to S32
	S32 *Buf;				//need S32 * 16, use ADPCM_EQ_BUF_SIZE define
	const S16 *FilterPara1; //Table start address 
	const S16 *FilterPara2;	//Reserved, default to NULL
	U8 EQGroupSelect;		//0:c-module EQ type0;
							//1:c-module EQ type1;
							//2:c-module EQ for DRC only;
							//0xff: external filter parameters
	S8 PitchIndex;			//0(default)~127, 0 is all pass filter
	S8 Gain1;				//Reserved, default to 0
	S8 Gain2;				//Reserved, default to 0
}EQCtl;

typedef struct _rt_darthvaderbuf RT_DarthVaderBuf;
typedef void(*DarthFunc)(S16 , S16 *, RT_DarthVaderBuf *);
struct _rt_darthvaderbuf {	//RT_ROBOT5Buf
	DarthFunc DarthFuncPtr;
	const S16 *RoboTable;	//Robot parameters
	S16 DelayApos;
	S16 DelayBpos;
	S16 fbDelayALev;
	S16 fbDelayBLev;
	S16 fbInSumLev;
	U16 Step_size;			//Robot parameters
	U16 Fidx;				//Robot parameters
	S16	InSize;
	S16 PitchBasicSize;		//Fit RT_PitchChange index parameters
	S16	TS_insize;			//Fit RT_PitchChange index parameters
	S16 RTNumRemain;		//Fit RT_PitchChange index parameters
	S8  PitchDelayFrames;	//Fit RT_PitchChange index parameters
	S8  SaveFrames;			//Fit RT_PitchChange index parameters
	S8  FPShiftLevel;		//Fixed-point shift level
	U8  Step_cycle_number;	//Robot parameters//Need to fit the StepSizeTb numbers.
	U8  Step_cycle_count;	//Robot parameters
	U8  Cycle_number;		//Robot parameters
	U8  Cycle;				//Robot parameters
	const U8  *StepSizeTb;	//Robot parameters
};

typedef struct _rt_darthvaderctrl {
	float  FbDelayALevel;
	float  FbDelayBLevel;
	float  InSumLevel;
	S16 FrameSize;
	S16 DelayA;				//Samples
	S16 DelayB;				//Samples
	S8  FPShiftLevel;		//Fixed-point shift level
	S8 PitchFactor;			//add
}RT_DarthVaderCtl;

typedef struct _darthvaderctl{
    float   FbDelayALevel;
    float   FbDelayBLevel;
    float   InSumLevel;
    S16     DelayA;
    S16     DelayB;
    S8      FPShiftLevel;
}DARTH_VADER_CTRL_T;
typedef struct _darthvaderbuf DARTH_VADER_RAM_T;
typedef void(*pbDarthFunc)(S16, S16 *, DARTH_VADER_RAM_T *);
struct _darthvaderbuf {
    pbDarthFunc	pbDarthFuncPtr;
    S32	windowsize_ratio;
    S32	remainNumber;
    S16	*RoboTable; //
    S16	Offset;
    S16	gCorStr;
    S16	gSlope;
    //S16	*RoboTable;
    S16	DelayApos;
    S16	DelayBpos;
    S16	fbDelayALev;
    S16	fbDelayBLev;
    S16	fbInSumLev;
    S16	TotalNewInSamples;
    U16	Step_size;
    U16	Fidx;
    S8	FPShiftLevel;
    U8	EffectType;         //change effect 
    U8	Step_cycle_number;
    U8	Step_cycle_count;
    U8	Cycle_number;
    U8	Cycle;
    const U8 *StepSizeTb;
};

typedef PARA_TYPE struct {
	CH_TypeDef 		*p_dac;
	TIMER_TypeDef 	*p_tmr;
}P_DAC_TIMER;
extern P_DAC_TIMER pHwCtl[];
#if body_SingleDAC
#define _ASSIGNABLE_DAC_MAX	(1)
#else
#define _ASSIGNABLE_DAC_MAX	(2)
#endif
#ifndef	_ASSIGNABLE_DAC_NUM
#if _QCODE_BUILD
#define _ASSIGNABLE_DAC_REQUEST	    (_AUDIO_API_CH_NUM  \
                                    +_MIDI_INST_NOTEON  \
                                    +_WAVE_ID_MODULE    \
                                    +_RT_PLAY)
#if		_ASSIGNABLE_DAC_REQUEST>_ASSIGNABLE_DAC_MAX
#define	_ASSIGNABLE_DAC_NUM	_ASSIGNABLE_DAC_MAX
#else
#define	_ASSIGNABLE_DAC_NUM	_ASSIGNABLE_DAC_REQUEST
#endif
#else//C-Module
#define	_ASSIGNABLE_DAC_NUM	_ASSIGNABLE_DAC_MAX
#endif
#endif//#ifndef	_ASSIGNABLE_DAC_NUM
#define SBC_CH1_EXCLUSIVE	(-1)
#define SBC_CH2_EXCLUSIVE	(-2)
#define SBC2_CH1_EXCLUSIVE	(-3)
#define SBC2_CH2_EXCLUSIVE	(-4)
#define ADPCM_CH1_EXCLUSIVE	(-5)
#define ADPCM_CH2_EXCLUSIVE	(-6)
#define ADPCM_CH3_EXCLUSIVE	(-7)
#define ADPCM_CH4_EXCLUSIVE	(-8)
#define PCM_CH1_EXCLUSIVE	(-9)
#define PCM_CH2_EXCLUSIVE	(-10)
#define PCM_CH3_EXCLUSIVE	(-11)
#define CELP_EXCLUSIVE		(-12)
#define MIDI_EXCLUSIVE		(-13)
#define RT_EXCLUSIVE		(-14)
#define WID_EXCLUSIVE		(-15)
#define ADPCM_CH5_EXCLUSIVE	(-16)
#define ADPCM_CH6_EXCLUSIVE	(-17)
#define ADPCM_CH7_EXCLUSIVE	(-18)
#define ADPCM_CH8_EXCLUSIVE	(-19)
typedef enum{
	kUNKNOW_CODEC=0,
	kSBC_CH1,
	kSBC_CH2,
	kADPCM_CH1,
	kADPCM_CH2,
	kCELP,
	kPCM_CH1,
	kMIDI,
	kADPCM_CH3,
	kADPCM_CH4,
	kPCM_CH2,
	kPCM_CH3,
	kSBC2_CH1,
	kSBC2_CH2,
	kRT,
	kWID,
	kADPCM_CH5,
	kADPCM_CH6,
	kADPCM_CH7,
	kADPCM_CH8,
}DAC_ASSIGNER_LIST;//available 0~31
typedef struct{
	S32	s32_sampleRate;
	U32	u32_dacCodecMem;
	U8	u8_fifoDataMem;
	U8	u8_dacDisableFlag;
}DAC_ASSIGNER_MEM;
void DacAssigner_Init(void);
S8	 DacAssigner_Regist(const DAC_ASSIGNER_LIST _codec,const  S32 _sampleRate);
U8	 DacAssigner_unRegist(const DAC_ASSIGNER_LIST _codec);
S8	 DacAssigner_codecUseDacCh(const DAC_ASSIGNER_LIST _codec);
bool DacAssigner_isCodecUseDac(const DAC_ASSIGNER_LIST _codec,const S8 _dacCh);
U32  DacAssigner_registInfo(const S8 _dacCh);
bool DacAssigner_isEmpty();
U8   DacAssigner_fifoDataNumberCheck(const S8 _dacCh);
void DacAssigner_ServiceLoop(void);

S8   DacMixerRamp_GetRampCh();
U16  DacMixerRamp_GetData(S8 _dacCh);
void DacMixerRamp_Init(void);
void DacMixerRamp_SetDacModeUse(bool _isDacModeUse);
bool DacMixerRamp_IsDacModeUse(void);
void DacMixerRampUp(S8 _dacCh);
void DacMixerRampDn(void);
void DacMixerRamp_BorrowTimer(S8 _dacCh);
void DacMixerRamp_SyncStsInPolling();
void DacMixerRamp_SyncStsInInterrupt(S8 _dacCh);
bool DacMixerRamp_IsBias();
bool DacMixerRamp_IsIdle();
void DacMixerRamp_ServiceLoop(void);
void DacMixerRamp_SetRampUp(void);
void DacMixerRamp_SetRampDown(void);

extern const U16 Robot_Thres[8];
extern const U16 Robot02_Thres[8];

#define	VIO_READ_RAM		0
void	VIO_TimerIsr(void);

extern U8 MixerVol;
extern U8 MixingChanNum[2];

#if	_ACTION_MODULE
#define _AUDIO_SENTENCE_CH_NUM  (_AUDIO_API_CH_NUM + _VIO_CH_NUM)
#else
#define _AUDIO_SENTENCE_CH_NUM  (_AUDIO_API_CH_NUM)
#endif
#if _AUDIO_SENTENCE
extern U32 SentenceAddr[_AUDIO_SENTENCE_CH_NUM];
extern U32 SentenceStatusMuteTimer[_AUDIO_SENTENCE_CH_NUM];
extern U8 SentenceCB[_AUDIO_SENTENCE_CH_NUM];
#endif

extern U8 KeyScan_DebounceCheck(void);
extern U16 KeyScan_CheckedData(const U8 portIndex);
extern U16 KeyScan_UsedPin(const U8 portIndex);
extern void DIRECTKEY_Init(void);
extern void DIRECTKEY_TimerIsr(void);

extern void AudioCtlInit(void);
extern void InitDac(CH_TypeDef *chx,U32 sample_rate,U8 upsample);
extern void InitAdc(U16 sample_rate);
extern U8   ADC_GetPreGainParameter(void);
extern U8   ADC_GetPgaGainParameter(void);
extern U8   RT_GetPreGainParameter(void);
extern U8   RT_GetPgaGainParameter(void);

extern void RunFilter(short *Input, int *RunFilterBuf, const S16 *DCRemove);

extern void LimiterInit(U16 peak);
extern void LimiterProcess(void *InputData, S16 *Out, U16 Threshold, U16 Samples,U8 mode);

extern void AgcInit(U16 alpha,U16 beta,U16 target_rms);
extern void AgcProcess(S16 *in,U16 samples);

extern void NGInit(U16 release_sample,U8 attack_sample,U16 onoff_sample);
extern void NGProcess(S16 *audio_buf,S16 *out_buf,U8 samples);

extern void EchoCancelInit(void);
extern void EchoCancelProcess(S16 *InOutData, U16 samples, U16 alg);


extern S16  ADPCMDecode(U16 *mark_sample, U8 ch);
extern void SetADPCMMode(U8 in_frame_mode, U8 out_frame_size, U8* ModeBuf, U8 ch);
extern void ADPCMDecInit(U8* DecBuf, t_ADPCMGetByte getByte, U8 ch);
extern void ADPCM_SaveStartFifoCnt(void);
extern void ADPCM_SaveEndFifoCnt(void);




extern void MIDIInit(U32 index,U32 timbre_addr,U32 *ram_allocate,U8 mode);
extern U8   MIDIDecode(U8 *outlen,S16 *Out16Buf,U8 limiter);
extern U8   MIDIChkEnd(void);
extern void MIDIInit_PIANO(U32 MidiNumber,U32 TimbreStartAddress,S32 *ram_allocate);
extern U8 MIDIChFree_GetStatus(void);  //1:Busy,0:free
extern void CB_InstNote_Rec_PreSetting(void);

extern void ADPCMEncInit(U8* EncBuf);
extern U32  ADPCMEncode(U32 samples,U16 *psrc,U8 *pdst);
extern U16  SBCBPF(U8 channel, U16 flag, U16 *mark_sample);
extern U16  SBC2BPF(U8 channel, U16 flag, U16 *mark_sample);

extern void SBCDecInit(U8 channel,U16 *ram_allocate,U16 sample_rate,U16 bandwidth,U16 normal_bits_per_frame,
                U16 delta_bits_per_frame_u,U16 delta_bits_per_frame_d,U8 vbr);
extern U32  SBCDecode(U8 channel,U32 TotalSample,U16 *pdst,U8 state);
extern void SBCEncInit(U16 *ram_allocate,U32 bit_rate,U32 bandwidth);
extern U32  SBCEncode(U32 samples,U16 *psrc,U8 *pdst);

extern void SBC2DecInit(S8 channel, S16 *ram_allocate,S16 *Temp_ram_allocate, S16 sample_rate, S16 bandwidth, S16 normal_bits_per_frame,
S16 delta_bits_per_frame_u, S16 delta_bits_per_frame_d, S8 vbr);
extern int SBC2Decode(U8 channel, U16 *pdst01, U16 *pdst02);
extern void SBC2EncInit(U16 *ram_allocate,U32 bit_rate,U32 bandwidth);
extern U32 SBC2Encode(U32 samples,U16 *psrc,U8 *pdst);

extern U32  CELPDecode(U32 samples,U8 *psrc,U16 *pdst);
extern void CELPDecInit(U16 *ram_allocate);

extern U32  TimeStretchInit(char reset,float scale);
extern U32  TimeStretchProcess(U16 *pOutput, U16 *pInput, U32 *num_in_remain);

extern void RoboticVoiceInit(U16 fs, U16 fc, U16 framesz);
extern void RoboticVoiceProcess(S16 *in, S16 *out);
extern void ROBOT_EFFECT_01_Init(S16 *robot_buf, U8 type,U16 thres,U16 detectthres, S16 SampleRate,S16 framesize,S16 echo_frame_size,S16 echo_buf_length);
extern void ROBOT_EFFECT_01(S16 *input);
extern void ROBOT_EFFECT_COMPRESS_01(S16 *input);

extern void ROBOT_EFFECT_02_Init(S16 *robot_buf, U8 type,U16 thres, S16 SampleRate);
extern void ROBOT_EFFECT_02(S16 *input);
extern void ROBOT_EFFECT_COMPRESS_02(S16 *input);

extern S8 ROBOT_EFFECT_04_Init(ROBOT4Ctl *Robot4Ctl, ROBOT4Buf *Robot4Buf, FFTctl *Robot4fftctl);
extern void ROBOT_EFFECT_04(short *inbuf, ROBOT4Ctl *Robot4Ctl, ROBOT4Buf *Robot4Buf, FFTctl *Robot4fftctl);	

extern void RT_PitchChange_Init(char reset, short factor);
extern short RT_PitchChange_main(char flag,short *FW_inbuffer, short *FW_outbuffer, short *Temp_Buf, int InSamples);
#if _SBC_ECHO_SOUND || _ADPCM_ECHO_SOUND || _CELP_ECHO_SOUND || _PCM_ECHO_SOUND || _SBC_CH2_ECHO_SOUND || _RT_ECHO_SOUND
extern void EchoVoiceProcess(S16 *audio_buf, S16 datalength, EchoCtl_T *pCtl);

extern char EchoSound_init(S16 *pEchoBuf, U16 TotalDelayTimes, U16 echoRamSize, volatile NewEchoCtrl *EchoCtrl);
extern void EchoSound(short *InFrmBuf, short framesize);
#endif
extern void RT_GhostSound_init(S16 *Pitch_buffer, S16 Pitch_size, S16 *reverb_x_buffer, S16 *reverb_y_buffer, S32 *EnvBuffer, S16 PitchIndex,U8 envelopindex);
extern U8 RT_GhostSound(S8 flag, S16 *FW_inbuffer, S16 *FW_outbuffer, S16 *PitchTemp_Buf, S32 InSamples);

extern S8 RT_DarthVader_Init(S8 reset, RT_DarthVaderCtl *rt_darthctl, RT_DarthVaderBuf *rt_darthbuf, U8 Type);
extern S16 RT_DarthVader(S8 flag, S16 *FW_inbuffer, S16 *FW_outbuffer, S16 *Temp_Buf, S32 InSamples, RT_DarthVaderBuf *rt_darthbuf);

extern S8 DarthVader_Init(S8 reset, float ratio, DARTH_VADER_CTRL_T *darthctl, DARTH_VADER_RAM_T *darthbuf);
extern S16 DarthVader(S16 *EffectOut, S16 *Input, S32 *num_in_remain, DARTH_VADER_RAM_T *darthbuf);

extern void RT_Chorus_Init(S8 reset, S16* ChorusBuf,RTChorusCtl *chorusCtl);
extern S16 RT_Chorus(S8 flag, S16 *in_buf, S16 *out_buf, S16 *Pitch_Buf, S16* Chorus_Buf);
extern void RT_ChorusVolume_Reset(U8 vol_1, U8 vol_2, U8 vol_3, float gain, S16* Chorus_Buf);

extern void Reverb_Effect_Init(short *reverb_x, short *reverb_y, U8 effect);
extern void Reverb_Effect(short *inbuf, unsigned short InData_Length);


extern void DRC_Init(volatile DRCFixedCtl *drcParameters, S8 *Buf, S32 SR);
extern void DRC(volatile DRCFixedCtl *drcParameters, S16 *cInput, S8 *DRC_Buf);

extern const S16 _AllPassFilterParaTab[12];//@audioctl.c
extern S8	EQInit(EQCtl *Eqctrl);
extern S16	EQProcess(S16 input,EQCtl *Eqctrl);

extern void DelayLoop(U32 count);
extern void DelayLoop2(U32 count);
extern void DelayNOP(U32 count);

extern void GIE_ENABLE(void);
extern void GIE_DISABLE(void);

#if _QFID_MODULE
extern void QFID_Init(void);
extern void QFID_TimerIsr(void);
extern U8	QFID_GetStatus(void);
extern void CB_QfidFunc_Grp_Tag_detect(U8 _group, U8 _tagid);
extern void CB_QfidFunc_Grp_Tag_leave(U8 _group, U8 _tagid);
#endif//#if _QFID_MODULE

#if _LEDSTR_TIMER==USE_TIMER0
#define LEDSTR_TIMER                P_TIMER0
#elif _LEDSTR_TIMER==USE_TIMER1
#define LEDSTR_TIMER                P_TIMER1
#elif _LEDSTR_TIMER==USE_TIMER2
#define LEDSTR_TIMER                P_TIMER2
#elif _LEDSTR_TIMER==USE_TIMER3
#define LEDSTR_TIMER                P_TIMER3
#elif _LEDSTR_TIMER==USE_TIMER_PWMA
#define LEDSTR_TIMER                PWMATIMER
#elif _LEDSTR_TIMER==USE_TIMER_PWMB
#define LEDSTR_TIMER                PWMBTIMER
#else
#error	"LEDSTR_TIMER is not Support!" 
#endif

#if _LEDSTRn_TIMER==USE_TIMER0 
#define LEDSTRn_TIMER                P_TIMER0
#elif _LEDSTRn_TIMER==USE_TIMER1
#define LEDSTRn_TIMER                P_TIMER1
#elif _LEDSTRn_TIMER==USE_TIMER2
#define LEDSTRn_TIMER                P_TIMER2
#elif _LEDSTRn_TIMER==USE_TIMER3
#define LEDSTRn_TIMER                P_TIMER3
#elif _LEDSTRn_TIMER==USE_TIMER_PWMA
#define LEDSTRn_TIMER                PWMATIMER
#elif _LEDSTRn_TIMER==USE_TIMER_PWMB
#define LEDSTRn_TIMER                PWMBTIMER
#else
#error	"LEDSTRn_TIMER is not Support!" 
#endif

#if _LEDSTRm_TIMER==USE_TIMER0 
#define LEDSTRm_TIMER                P_TIMER0
#elif _LEDSTRm_TIMER==USE_TIMER1
#define LEDSTRm_TIMER                P_TIMER1
#elif _LEDSTRm_TIMER==USE_TIMER2
#define LEDSTRm_TIMER                P_TIMER2
#elif _LEDSTRm_TIMER==USE_TIMER3
#define LEDSTRm_TIMER                P_TIMER3
#elif _LEDSTRm_TIMER==USE_TIMER_PWMA
#define LEDSTRm_TIMER                PWMATIMER
#elif _LEDSTRm_TIMER==USE_TIMER_PWMB
#define LEDSTRm_TIMER                PWMBTIMER
#else
#error	"LEDSTRm_TIMER is not Support!" 
#endif
#if _USE_HEAP_RAM
extern void *MemAlloc( U16 xWantedSize );
extern void MemFree( void *pv );
#endif

//WaveID Module
#if _WAVE_ID_MODULE 

typedef struct _WaveIDBuf {
   S32  widReg_Buf[WID_BUF_SIZE_05];     //8
   S32  widFFT_Buf[WID_OUT_SIZE];           //256  
   U32  TempNonFrequencyCount;
   U32  NonFrequencyCount;
   S32  dynamicThreshold;
   S16  ValidFreqCount;
   S16  LowerLimitThreshold;
   S16	FrameSize;
   S8  widStack_Buf[WID_BUF_SIZE_02];     //18
   S8  widFrq_Buf[WID_BUF_SIZE_03];        //10
   S8  widEc_Buf[WID_BUF_SIZE_04];         //53
   S8   ECTimes;
}Wave_IDBuf;
extern void Wave_IDInit(Wave_IDBuf *waveidBuf, FFTctl *fftctl);
extern void WID_RunFilter(S16 *Input, S32 *Reg, S16 *Coef); 
extern char Wave_IDProcess(S16 *Input, Wave_IDBuf *waveidInBuf, FFTctl *fftctl);
extern void WaveID_DacIsr(U8 size, S16* _dacData);
#endif 


//Pitch detection
#define _PD_SampleRate	SAMPLE_RATE_8000				// Sample Rate only 8000 and 16000
#define PD_BUF_SIZE		256
#define PD_FFT_SIZE		256
#define PD_FFT_HSIZE	128

typedef struct _PDCtlBuf{		
	S32 FFTBuf[PD_FFT_SIZE];			//FFT_size  256 -> FFTBuf 256
								//FFT_size  512 -> FFTBuf 512
	U16 SPCBuf[65];				//FFT_size  256 -> SPCBuf 65
								//FFT_size  512 -> FFTBuf 129
	S16 PDBuf[57];				//10 + 25 +20 + 2;  parameters + harmonic freq + Peak freq
#if (_PD_SampleRate==16000)
	S16 OverLapBuf[PD_FFT_HSIZE];		//if smaple rate is 16000 that use this buf. 
#endif
	U16 Threshold;
 	U8 ShiftLevel;
}PDCtlBuf;

#if _PD_PLAY 
extern void PD_AdcIsr(U8 size, U32 *pdest32);
extern void PD_MilliSecondIsr(void);
extern S16 PitchDetect(S16 *InputData, const S16 *PDCtl,  PDCtlBuf *pdCtrlBuf, FFTctl *fftctl);
extern S8 PD_Init(const S16 *PDSetting, PDCtlBuf *PDCtrlBuf, FFTctl *fftctl);
extern U8  PD_GetStatus(void);
extern S16 PD_GetFreq(S16 Freq);
extern bool PD_GetFreqResult(S16 Freq,S16 r_factor);
extern void PD_Start(void);
extern void PD_Stop();
extern void PD_ServiceLoop(void);

#endif 

#define PD_CTL_BUF_SIZE			(sizeof(PDCtlBuf))
#define PD_FFT_CTL_SIZE			(sizeof(FFTctl))

#define PD_PLAY_RAM            ((PD_BUF_SIZE<<1)+PD_CTL_BUF_SIZE+PD_FFT_CTL_SIZE)


//Touch Module
#if _TOUCHKEY_MODULE
//=====touch.c
extern void Touch_Initial();
#define	Touch_ENABLE	Touch_Initial
extern void Touch_DISABLE();
extern void TOUCH_ServiceLoop();								//	Touch Service loop
extern void TOUCH_CAPTURE();									//	Touch capture function
extern void Touch_CheckPAD(U8 u8ScanPADIdx);
extern void	Touch_Start(U8 u8ScanPADIdx, U8 u8MaxConvertTimes, U32* u32TouchCaptureValue);
//=====touch library
extern void Touch_LibraryInitial();								//	Touch initial process
extern void Touch_LibraryStop();
extern void Touch_SetConvertOK();
extern void Touch_SetError();
extern void Touch_InitBeforeStandby();
extern U8 Touch_ProcessInStandby();
extern void Touch_InitAfterStandby();
#endif

//--MIDI Instrument Func--//
#if _MIDI_INST_NOTEON
extern U8 MIDI_BusyFlag;
extern volatile U8 NoteOn_MidiVol;
extern volatile U8 NoteOn_InstVol;
#endif

#if _XIP_COC
#if _SPI_MODULE==DISABLE
#error "If you need to use _XIP_COC, _SPI_MODULE should be set ENABLE"
#endif
#if _USE_HEAP_RAM==DISABLE
#error "If you need to use _XIP_COC, _USE_HEAP_RAM should be set ENABLE"
#endif
#endif

extern U16  COC_VR_RamSize(void);
extern U16  COC_VR_EngineRamSize(void);
extern U32  COC_VR_TimeOut(void);
extern S16  XIP_VAD_Parameters(U8 index);
extern U16	XIP_VR_VAD_Parameters(U8 index);
extern S16  XIP_SOUND_DETECTION_Parameters(U8 index);
extern U8   XIP_ADCGain_Parameters(U8 index);
extern U8   XIP_RTpitchChange_ADCGain_Parameters(U8 index);

#if _VR_FUNC
extern U8 VRState, VR_FuncType, VR_GroupState;
extern U32 hCSpotter;
#define CSPDLL_API
#define HANDLE	void*

#if _VR_PLATFORM == 16
extern CSPDLL_API HANDLE CSpotter16_Init_Sep(U8 *lpbyCYBase, U8 *lpbyModel, U32 nMaxTime, U8 *lpbyMemPool, U32 nMemSize, U8 *lpbyState, U32 nStateSize, S32 *pnErr);
extern CSPDLL_API HANDLE CSpotter16_Init_Multi(U8 *lpbyCYBase, U8 *lppbyModel[], U32 nNumModel, U32 nMaxTime, U8 *lpbyMemPool, U32 nMemSize, U8 *lpbyState, U32 nStateSize, S32 *pnErr);
extern CSPDLL_API S32 CSpotter16_Reset(HANDLE hCSpotter);
extern CSPDLL_API S32 CSpotter16_Release(HANDLE hCSpotter);
extern CSPDLL_API S32 CSpotter16_GetMemoryUsage_Sep(U8 *lpbyCYBase, U8 *lpbyModel, U32 nMaxTime);
extern CSPDLL_API S32 CSpotter16_GetMemoryUsage_Multi(U8 *lpbyCYBase, U8 *lppbyModel[], U32 nNumModel, U32 nMaxTime);
extern CSPDLL_API S32 CSpotter16_AddSample(HANDLE hCSpotter, S16 *lpsSample, U32 nNumSample);
extern CSPDLL_API S32 CSpotter16_GetResult(HANDLE hCSpotter);
extern CSPDLL_API S32 CSpotter16_GetResultScore(HANDLE hCSpotter);
extern CSPDLL_API S32 CSpotter16_GetNumWord(U8 *lpbyModel);
extern CSPDLL_API S32 CSpotter16_GetModelCmdReward(U8 *lpbyModel, U32 nCmdIdx);
extern CSPDLL_API S32 CSpotter16_SetRejectionLevel(HANDLE hCSpotter, S32 nRejectionLevel);
extern CSPDLL_API S32 CSpotter16_SetResponseTime(HANDLE hCSpotter, U32 nResponseTime);
//extern CSPDLL_API S32 CSpotter16_SetCmdReward(HANDLE hCSpotter, U32 nCmdIdx, S32 nReward);
extern CSPDLL_API S32 CSpotter16_SetCmdRewardOffset(HANDLE hCSpotter, U32 nCmdIdx, S32 nOffset);
//extern CSPDLL_API S32 CSpotter16_SetCmdResponseReward(HANDLE hCSpotter, U32 nCmdIdx, S32 nReward);
extern CSPDLL_API S32 CSpotter16_SetCmdResponseOffset(HANDLE hCSpotter, U32 nCmdIdx, S32 nOffset);
extern CSPDLL_API S32 CSpotter16_SetEnableNBest(HANDLE hCSpotter, U32 bEnable);
extern CSPDLL_API S32 CSpotter16_GetNBestScore(HANDLE hCSpotter, S32 pnCmdIdx[], S32 pnScore[], U32 nMaxNBest);
extern CSPDLL_API S32 CSpotter16_SetFreqWarpFactor(HANDLE hCSpotter, S32 nFactor256);

extern CSPDLL_API HANDLE CSpotter16_Init_Sep_FromSPI(U8 *lpbyCYBase, U8 *lpbyModel, U32 nMaxTime, U8 *lpbyMemPool, U32 nMemSize, U8 *lpbyState, U32 nStateSize, S32 *pnErr);
extern CSPDLL_API HANDLE CSpotter16_Init_Multi_FromSPI(U8 *lpbyCYBase, U8 *lppbyModel[], U32 nNumModel, U32 nMaxTime, U8 *lpbyMemPool, U32 nMemSize, U8 *lpbyState, U32 nStateSize, S32 *pnErr);
extern CSPDLL_API S32 CSpotter16_GetMemoryUsage_Sep_FromSPI(U8 *lpbyCYBase, U8 *lpbyModel, U32 nMaxTime);
extern CSPDLL_API S32 CSpotter16_GetMemoryUsage_Multi_FromSPI(U8 *lpbyCYBase, U8 *lppbyModel[], U32 nNumModel, U32 nMaxTime);
extern CSPDLL_API S32 CSpotter16_GetNumWord_FromSPI(U8 *lpbyModel);
extern CSPDLL_API S32 CSpotter16_GetModelCmdReward_FromSPI(U8 *lpbyModel, U32 nCmdIdx);

#define CSPOTTER_X_Reset								CSpotter16_Reset
#define CSPOTTER_X_AddSample							CSpotter16_AddSample
#define CSPOTTER_X_GetResult							CSpotter16_GetResult
#define CSPOTTER_X_GetResultScore						CSpotter16_GetResultScore
#define CSPOTTER_X_Release								CSpotter16_Release
#define CSPOTTER_X_SetEnableNBest						CSpotter16_SetEnableNBest
#define CSPOTTER_X_GetNBestScore						CSpotter16_GetNBestScore
#if _VR_STORAGE_TYPE
#define CSPOTTER_X_GetMemoryUsage_Multi					CSpotter16_GetMemoryUsage_Multi_FromSPI
#define CSPOTTER_X_Init_Multi							CSpotter16_Init_Multi_FromSPI
#define CSPOTTER_X_GetNumWord							CSpotter16_GetNumWord_FromSPI
#else
#define CSPOTTER_X_GetMemoryUsage_Multi					CSpotter16_GetMemoryUsage_Multi
#define CSPOTTER_X_Init_Multi							CSpotter16_Init_Multi
#define CSPOTTER_X_GetNumWord							CSpotter16_GetNumWord
#endif

#if _VR_VOICE_TAG
extern U32 *hCSpotterSD;
extern U8* SDModel;

extern CSPDLL_API HANDLE CSpotterSD16_Init(U8 *lpbyCYBase, U8 *lpbyMemPool, U32 nMemSize, S32 *pnErr);
extern CSPDLL_API S32 CSpotterSD16_GetMemoryUsage(U8 *lpbyCYBase, U32 bEnableSpeakerMode);
extern CSPDLL_API S32 CSpotterSD16_AddUttrStart(HANDLE hCSpotter, U32 nUttrID, U8 *lpszDataBuf, U32 nBufSize);
extern CSPDLL_API S32 CSpotterSD16_AddSample(HANDLE hCSpotter, S16 *lpsSample, U32 nNumSample);
extern CSPDLL_API S32 CSpotterSD16_AddUttrEnd(HANDLE hCSpotter);
extern CSPDLL_API S32 CSpotterSD16_DifferenceCheckWithRejLevel(HANDLE hCSpotter, S32 nRejLevel);
extern CSPDLL_API S32 CSpotterSD16_TrainWord(HANDLE hCSpotter, U8 *lpszModelAddr, U32 nBufSize, U32 nTagID, U32 nRejLevel, U32 *pnUsedSize);
extern CSPDLL_API S32 CSpotterSD16_DeleteWord(HANDLE hCSpotter, U8 *lpszModelAddr, U32 nBufSize, U32 nTagID, U32 *pnUsedSize);
extern CSPDLL_API S32 CSpotterSD16_GetCmdID(U8 *lpszModelAddr, U32 nTagID);
extern CSPDLL_API S32 CSpotterSD16_GetTagID(U8 *lpszModelAddr, S32 nCmdID);

extern CSPDLL_API HANDLE CSpotterSD16_Init_FromSPI(U8 *lpbyCYBase, U8 *lpbyMemPool, U32 nMemSize, S32 *pnErr);
extern CSPDLL_API S32 CSpotterSD16_GetMemoryUsage_FromSPI(U8 *lpbyCYBase, U32 bEnableSpeakerMode);
extern CSPDLL_API S32 CSpotterSD16_AddUttrStart_FromSPI(HANDLE hCSpotter, U32 nUttrID, U8 *lpszDataBuf, U32 nBufSize);
extern CSPDLL_API S32 CSpotterSD16_AddSample_FromSPI(HANDLE hCSpotter, S16 *lpsSample, U32 nNumSample);
extern CSPDLL_API S32 CSpotterSD16_AddUttrEnd_FromSPI(HANDLE hCSpotter);
extern CSPDLL_API S32 CSpotterSD16_DifferenceCheckWithRejLevel_FromSPI(HANDLE hCSpotter, S32 nRejLevel);
extern CSPDLL_API S32 CSpotterSD16_TrainWord_FromSPI(HANDLE hCSpotter, U8 *lpszModelAddr, U32 nBufSize, U32 nTagID, U32 nRejLevel, U32 *pnUsedSize);
extern CSPDLL_API S32 CSpotterSD16_DeleteWord_FromSPI(HANDLE hCSpotter, U8 *lpszModelAddr, U32 nBufSize, U32 nTagID, U32 *pnUsedSize);
extern CSPDLL_API S32 CSpotterSD16_GetCmdID_FromSPI(U8 *lpszModelAddr, U32 nTagID);
extern CSPDLL_API S32 CSpotterSD16_GetTagID_FromSPI(U8 *lpszModelAddr, S32 nCmdID);

extern CSPDLL_API S32 CSpotterSD16_Release(HANDLE hCSpotter);
extern CSPDLL_API S32 CSpotterSD16_GetUttrEPD(HANDLE hCSpotter, U32 *pnStart, U32 *pnEnd);
extern CSPDLL_API S32 CSpotterSD16_SetEpdLevel(HANDLE hCSpotter, U32 nEpdLevel);
extern CSPDLL_API S32 CSpotterSD16_SetBackgroundEnergyThreshd(HANDLE hCSpotter, U32 nThreshold);

#if _VR_STORAGE_TYPE
#define CSPOTTERSD_X_Init								CSpotterSD16_Init_FromSPI
#define CSPOTTERSD_X_GetMemoryUsage						CSpotterSD16_GetMemoryUsage_FromSPI
#define CSPOTTERSD_X_AddUttrStart						CSpotterSD16_AddUttrStart_FromSPI
#define CSPOTTERSD_X_AddSample							CSpotterSD16_AddSample_FromSPI
#define CSPOTTERSD_X_AddUttrEnd							CSpotterSD16_AddUttrEnd_FromSPI
#define CSPOTTERSD_X_DifferenceCheckWithRejLevel		CSpotterSD16_DifferenceCheckWithRejLevel_FromSPI
#define CSPOTTERSD_X_TrainWord							CSpotterSD16_TrainWord_FromSPI
#define CSPOTTERSD_X_DeleteWord							CSpotterSD16_DeleteWord_FromSPI
#define CSPOTTERSD_X_GetCmdID							CSpotterSD16_GetCmdID_FromSPI
#define CSPOTTERSD_X_GetTagID							CSpotterSD16_GetTagID_FromSPI
#else
#define CSPOTTERSD_X_Init								CSpotterSD16_Init
#define CSPOTTERSD_X_GetMemoryUsage						CSpotterSD16_GetMemoryUsage
#define CSPOTTERSD_X_AddUttrStart						CSpotterSD16_AddUttrStart
#define CSPOTTERSD_X_AddSample							CSpotterSD16_AddSample
#define CSPOTTERSD_X_AddUttrEnd							CSpotterSD16_AddUttrEnd
#define CSPOTTERSD_X_DifferenceCheckWithRejLevel		CSpotterSD16_DifferenceCheckWithRejLevel
#define CSPOTTERSD_X_TrainWord							CSpotterSD16_TrainWord
#define CSPOTTERSD_X_DeleteWord							CSpotterSD16_DeleteWord
#define CSPOTTERSD_X_GetCmdID							CSpotterSD16_GetCmdID
#define CSPOTTERSD_X_GetTagID							CSpotterSD16_GetTagID
#endif

#if _VR_VOICE_PASSWORD
extern U8* SVModel;
extern S32 SVscore, SVTagId;

extern CSPDLL_API S32 CSpotter16_GetMemoryUsage_SepWithSpeaker(U8 *lpbyCYBase, U8 *lpbyModel, U32 nMaxTime, U8 *lpbySpeakerModel);
extern CSPDLL_API S32 CSpotter16_GetMemoryUsage_MultiWithSpeaker(U8 *lpbyCYBase, U8 *lppbyModel[], U32 nNumModel, U32 nMaxTime, U8 *lpbySpeakerModel);
extern CSPDLL_API S32 CSpotterSD16_TrainWordForSpeaker(HANDLE hCSpotter, U8 *lpszModelAddr, U32 nBufSize, U32 nTagID, U32 *pnUsedSize);
extern CSPDLL_API S32 CSpotter16_SetSpeakerModel(HANDLE hCSpotter, U8 *lpbySpeakerModel);

extern CSPDLL_API S32 CSpotter16_GetMemoryUsage_SepWithSpeaker_FromSPI(U8 *lpbyCYBase, U8 *lpbyModel, U32 nMaxTime, U8 *lpbySpeakerModel);
extern CSPDLL_API S32 CSpotter16_GetMemoryUsage_MultiWithSpeaker_FromSPI(U8 *lpbyCYBase, U8 *lppbyModel[], U32 nNumModel, U32 nMaxTime, U8 *lpbySpeakerModel);
extern CSPDLL_API S32 CSpotterSD16_TrainWordForSpeaker_FromSPI(HANDLE hCSpotter, U8 *lpszModelAddr, U32 nBufSize, U32 nTagID, U32 *pnUsedSize);
extern CSPDLL_API S32 CSpotter16_SetSpeakerModel_FromSPI(HANDLE hCSpotter, U8 *lpbySpeakerModel);

extern CSPDLL_API S32 CSpotter16_SetSpeakerIdentLevel(HANDLE hCSpotter, S32 nIdentLevel);
extern CSPDLL_API S32 CSpotter16_GetSpeakerResult(HANDLE hCSpotter, S32 *pnScore);

#if _VR_STORAGE_TYPE
#define CSPOTTER_X_SetSpeakerModel						CSpotter16_SetSpeakerModel_FromSPI
#define CSPOTTER_X_GetMemoryUsage_MultiWithSpeaker		CSpotter16_GetMemoryUsage_MultiWithSpeaker_FromSPI
#define CSPOTTERSD_X_TrainWordForSpeaker				CSpotterSD16_TrainWordForSpeaker_FromSPI
#else
#define CSPOTTER_X_SetSpeakerModel						CSpotter16_SetSpeakerModel
#define CSPOTTER_X_GetMemoryUsage_MultiWithSpeaker		CSpotter16_GetMemoryUsage_MultiWithSpeaker
#define CSPOTTERSD_X_TrainWordForSpeaker				CSpotterSD16_TrainWordForSpeaker
#endif
#endif	//#if _VR_VOICE_PASSWORD
#endif	//#if _VR_VOICE_TAG

#elif _VR_PLATFORM == 32
extern CSPDLL_API S32 CSpotter_GetMemoryUsage_Multi(U8 *lpbyCYBase, U8 *lppbyModel[], U32 nNumModel, U32 nMaxTime);
extern CSPDLL_API HANDLE CSpotter_Init_Multi(U8 *lpbyCYBase, U8 *lppbyModel[], U32 nNumModel, U32 nMaxTime, U8 *lpbyMemPool, U32 nMemSize, U8 *lpbyState, U32 nStateSize, S32 *pnErr);
extern CSPDLL_API S32 CSpotter_GetNumWord(U8 *lpbyModel);
extern CSPDLL_API S32 CSpotter_Reset(HANDLE hCSpotter);
extern CSPDLL_API S32 CSpotter_AddSample(HANDLE hCSpotter, S16 *lpsSample, U32 nNumSample);
extern CSPDLL_API S32 CSpotter_GetResult(HANDLE hCSpotter);
extern CSPDLL_API S32 CSpotter_GetResultScore(HANDLE hCSpotter);
extern CSPDLL_API S32 CSpotter_Release(HANDLE hCSpotter);
extern CSPDLL_API S32 CSpotter_SetEnableNBest(HANDLE hCSpotter, U32 bEnable);
extern CSPDLL_API S32 CSpotter_GetNBestScore(HANDLE hCSpotter, S32 pnCmdIdx[], S32 pnScore[], U32 nMaxNBest);

extern CSPDLL_API S32 CSpotter_GetMemoryUsage_Multi_FromSPI(U8 *lpbyCYBase, U8 *lppbyModel[], U32 nNumModel, U32 nMaxTime);
extern CSPDLL_API HANDLE CSpotter_Init_Multi_FromSPI(U8 *lpbyCYBase, U8 *lppbyModel[], U32 nNumModel, U32 nMaxTime, U8 *lpbyMemPool, U32 nMemSize, U8 *lpbyState, U32 nStateSize, S32 *pnErr);
extern CSPDLL_API S32 CSpotter_GetNumWord_FromSPI(U8 *lpbyModel);

#define CSPOTTER_X_Reset								CSpotter_Reset
#define CSPOTTER_X_AddSample							CSpotter_AddSample
#define CSPOTTER_X_GetResult							CSpotter_GetResult
#define CSPOTTER_X_GetResultScore						CSpotter_GetResultScore
#define CSPOTTER_X_Release								CSpotter_Release
#define CSPOTTER_X_SetEnableNBest						CSpotter_SetEnableNBest
#define CSPOTTER_X_GetNBestScore						CSpotter_GetNBestScore
#if _VR_STORAGE_TYPE
#define CSPOTTER_X_GetMemoryUsage_Multi					CSpotter_GetMemoryUsage_Multi_FromSPI
#define CSPOTTER_X_Init_Multi							CSpotter_Init_Multi_FromSPI
#define CSPOTTER_X_GetNumWord							CSpotter_GetNumWord_FromSPI
#else
#define CSPOTTER_X_GetMemoryUsage_Multi					CSpotter_GetMemoryUsage_Multi
#define CSPOTTER_X_Init_Multi							CSpotter_Init_Multi
#define CSPOTTER_X_GetNumWord							CSpotter_GetNumWord
#endif
#endif	//#if _VR_PLATFORM == 16
#endif	//#if _VR_FUNC

//QIO Define
#if ((_ADPCM_QIO_MODULE && _ADPCM_PLAY)||		\
	(_ADPCM_CH2_QIO_MODULE && _ADPCM_CH2_PLAY)||\
	(_ADPCM_CH3_QIO_MODULE && _ADPCM_CH3_PLAY)||\
	(_ADPCM_CH4_QIO_MODULE && _ADPCM_CH4_PLAY)||\
	(_ADPCM_CH5_QIO_MODULE && _ADPCM_CH5_PLAY)||\
	(_ADPCM_CH6_QIO_MODULE && _ADPCM_CH6_PLAY)||\
	(_ADPCM_CH7_QIO_MODULE && _ADPCM_CH7_PLAY)||\
	(_ADPCM_CH8_QIO_MODULE && _ADPCM_CH8_PLAY)||\
	(_SBC_QIO_MODULE && _SBC_PLAY)||			\
	(_SBC_CH2_QIO_MODULE && _SBC_CH2_PLAY)||	\
	(_SBC2_QIO_MODULE && _SBC2_PLAY)||			\
	(_SBC2_CH2_QIO_MODULE && _SBC2_CH2_PLAY))
#define _QIO_MODULE	ENABLE
#else
#define _QIO_MODULE	DISABLE
#endif

#if (	_ADPCM_CH2_QIO_MODULE || _ADPCM_CH3_QIO_MODULE\
	 || _ADPCM_CH4_QIO_MODULE || _ADPCM_CH5_QIO_MODULE\
	 || _ADPCM_CH6_QIO_MODULE || _ADPCM_CH7_QIO_MODULE\
	 || _ADPCM_CH8_QIO_MODULE )
	 	#define _ADPCM_CHX_QIO_MODULE		ENABLE
#else
		#define _ADPCM_CHX_QIO_MODULE		DISABLE
#endif

#ifndef _QSW_PWM_MODULE
#define _QSW_PWM_MODULE _SW_PWM_MODULE
#else
#undef  _QSW_PWM_MODULE
#define _QSW_PWM_MODULE DISABLE
#endif

#if !defined(QIO_EVENT_FIFOSIZE)
#if _QSW_PWM_MODULE
#define QIO_EVENT_FIFOSIZE  ((_SW_PWM_CH_NUM<=5)?(16):      \
                             (_SW_PWM_CH_NUM<=11)?(32):     \
                             (_SW_PWM_CH_NUM<=22)?(64):(128))
#else
#define QIO_EVENT_FIFOSIZE          (16)
#endif
#endif

#if	_QIO_MODULE
#define	QIO_RESOURCE_IDLE			0x00
#define	QIO_USE_SBC_CH1				(0x10 | 0x01)
#define	QIO_USE_SBC_CH2				(0x10 | 0x02)
#define	QIO_USE_SBC2_CH1			(0x10 | 0x03)
#define	QIO_USE_SBC2_CH2			(0x10 | 0x04)
#define	QIO_USE_ADPCM_CH1			(0x20 | 0x01)  
#define	QIO_USE_ADPCM_CH2			(0x20 | 0x02)
#define	QIO_USE_ADPCM_CH3			(0x20 | 0x03)
#define	QIO_USE_ADPCM_CH4			(0x20 | 0x04)
#define	QIO_USE_ADPCM_CH5			(0x20 | 0x05) 
#define	QIO_USE_ADPCM_CH6			(0x20 | 0x06) 
#define	QIO_USE_ADPCM_CH7			(0x20 | 0x07) 
#define	QIO_USE_ADPCM_CH8			(0x20 | 0x08)
//u8_QIO_DelayStopFlag
#define	QIO_DelayStopDISABLE		0
#define	QIO_DelayStopENABLE			1
#define	QIO_DelayStopENwPinNonBusy	2

//QIO Heap RAM forcast
#define	QIO_READ_RAM				0

#define		MAX_AUDIO_BUF_SIZE	0
#ifdef		SBC_AUDIO_BUF_SIZE
	#if (_SBC_PLAY && _SBC_QIO_MODULE)
		#if	(SBC_AUDIO_BUF_SIZE > MAX_AUDIO_BUF_SIZE)
			#undef	MAX_AUDIO_BUF_SIZE
			#define	MAX_AUDIO_BUF_SIZE	SBC_AUDIO_BUF_SIZE
		#endif
	#endif
#endif
#ifdef		SBC_CH2_AUDIO_BUF_SIZE
	#if (_SBC_CH2_PLAY && _SBC_CH2_QIO_MODULE)
		#if	(SBC_CH2_AUDIO_BUF_SIZE > MAX_AUDIO_BUF_SIZE)
			#undef	MAX_AUDIO_BUF_SIZE
			#define	MAX_AUDIO_BUF_SIZE	SBC_CH2_AUDIO_BUF_SIZE
		#endif
	#endif
#endif
#ifdef		SBC2_AUDIO_BUF_SIZE
	#if (_SBC2_PLAY && _SBC2_QIO_MODULE)
		#if	(SBC2_AUDIO_BUF_SIZE > MAX_AUDIO_BUF_SIZE)
			#undef	MAX_AUDIO_BUF_SIZE
			#define	MAX_AUDIO_BUF_SIZE	SBC2_AUDIO_BUF_SIZE
		#endif
	#endif
#endif
#ifdef		SBC2_CH2_AUDIO_BUF_SIZE
	#if (_SBC2_CH2_PLAY && _SBC2_CH2_QIO_MODULE)
		#if	(SBC2_CH2_AUDIO_BUF_SIZE > MAX_AUDIO_BUF_SIZE)
			#undef	MAX_AUDIO_BUF_SIZE
			#define	MAX_AUDIO_BUF_SIZE	SBC2_CH2_AUDIO_BUF_SIZE
		#endif
	#endif
#endif
#ifdef		ADPCM_AUDIO_BUF_SIZE
	#if (_ADPCM_PLAY && _ADPCM_QIO_MODULE)
		#if	(ADPCM_AUDIO_BUF_SIZE > MAX_AUDIO_BUF_SIZE)
			#undef	MAX_AUDIO_BUF_SIZE
			#define	MAX_AUDIO_BUF_SIZE	ADPCM_AUDIO_BUF_SIZE
		#endif
	#endif
#endif
#ifdef		ADPCM_CHX_AUDIO_BUF_SIZE
	#if (_ADPCM_CHX_PLAY && _ADPCM_CHX_QIO_MODULE)
		#if	(ADPCM_CHX_AUDIO_BUF_SIZE > MAX_AUDIO_BUF_SIZE)
			#undef	MAX_AUDIO_BUF_SIZE
			#define	MAX_AUDIO_BUF_SIZE	ADPCM_CHX_AUDIO_BUF_SIZE
		#endif
	#endif
#endif

#if (MAX_AUDIO_BUF_SIZE==0)
	#define		QIO_FrameFlag_FIFOSIZE	0
#else
	#define		QIO_FrameFlag_FIFOSIZE	((MAX_AUDIO_BUF_SIZE+32+7)/8)
#endif
void	QIO_Play(U8 _AudioResource, U8 _StorageMode, U32 _StartAddr);
void	QIO_DelayStop(U8 _AudioResource, U8 _ForcePinNonBusy);
void	QIO_Stop(U8 _AudioResource, U8 _ForcePinNonBusy);
void	QIO_Pause(U8 _AudioResource);
void	QIO_Resume(U8 _AudioResource);
U8		QIO_ReGetStorageDataSet(U8 _AudioResource);
void	QIO_ServiceLoop();
U8		QIO_DacIsr(U8 _AudioResource, U8 _EventFlag);
void	QIO_Stop_woResource();
U8		QIO_FrameFlag_FIFO_BitPut(U8 _AudioResource, U8 *data);
U8		QIO_FrameFlag_FIFO_RstPut(U8 _AudioResource);
U8		QIO_FrameFlag_FIFO_BitGet(U8 _AudioResource, U8 *data);
U8		QIO_FrameFlag_FIFO_RstGet(U8 _AudioResource);
U8      QIO_SW_PWM_GetCmd(U32 *cmd);
#endif//#if _QIO_MODULE
			
//SW PWM-IO Define
#if		(_SW_PWM_MODULE)
#define	SW_PWM_CH_NUM					_SW_PWM_CH_NUM
#if (_NYIDE_TIMEBASE==TIMEBASE_1000us)
#define	SW_PWM_BASE_RATE				(1024)											//use RTC_1K
#elif (_NYIDE_TIMEBASE==TIMEBASE_250us)
#define	SW_PWM_BASE_RATE				(4096)											//use RTC_4K
#else
#define	SW_PWM_BASE_RATE				(16384)											//use RTC_16K
#endif
#define	SW_PWM_FRAMERATE				_NYIDE_FRAMERATE									//unit HZ(64/80/100/125/128/160/200/250)
#define	SW_PWM_TICK						(((SW_PWM_BASE_RATE)+(SW_PWM_FRAMERATE>>1))/SW_PWM_FRAMERATE)	//max 256 @64Hz Rate
#define	SW_PWM_getNextDuty(ND,TGD,DF)	((ND)+(((TGD)-(ND)+((DF)>>1))/(DF)))				//ND = u16_NowDutyNum, TGD = u16_TargetDutyNum, DF = u16_DiffFrame
#define SW_PWM_GAMMA2_CORRECTION(ND)	((((U32)ND)*(((U32)ND)+2))>>16)						//gamma=2 correction
#ifdef	_SW_PWM_GAMMA2_CORRECTION
#define	SW_PWM_u16Duty2Tick(ND)			(((SW_PWM_GAMMA2_CORRECTION(ND))*(SW_PWM_TICK+1))>>16)	//ND = u16_NowDutyNum
#else
#define	SW_PWM_u16Duty2Tick(ND)			(((ND)*(SW_PWM_TICK+1))>>16)						//ND = u16_NowDutyNum
#endif
//#define	SW_PWM_Ms2Frame(ms)			((U16)((ms)*SW_PWM_FRAMERATE/1000))//Cost 335ns @32M
#define	SW_PWM_Ms2Frame(ms)				((U16)(((ms)*SW_PWM_FRAMERATE*131)>>17))//Cost 120ns @32M

typedef	struct
{
	U8      u8_CMD_ch;
	U8      u8_CMD_TargetDutyNum;
	U16     u16_CMD_DiffFrame;
}S_SW_PWM_CMD_TYPE __attribute__((aligned(4)));//FIFO use U32 to reduce process time.

void	SW_PWM_Init();
void	SW_PWM_TimerIsr();
void	SW_PWM_ChDataInit();
void	SW_PWM_Sleep();
void	SW_PWM_WakeUp();
void	SW_PWM_ForcePinNonBusy();
U8      CB_SW_PWM_GetCmd(U32* cmd);
#endif//#if		(_SW_PWM_MODULE)


#if _AUDIO_VOLUME_CHANGE_SMOOTH
typedef struct VOL_CTRL{
    S16 step;
    S16 target_value;
    S16 current_value;
}VOLCTRL_T;
extern void AUDIO_InitVolumeSmooth(volatile VOLCTRL_T *pVolCtrl, U16 sr, U16 total_value);
extern void AUDIO_SetVolumeSmooth(volatile VOLCTRL_T *pVolCtrl, U16 total_vol);
extern S16 AUDIO_GetVolumeSmooth(volatile VOLCTRL_T *pVolCtrl);
extern S16 AUDIO_GetVolumeSmoothINT(volatile VOLCTRL_T *pVolCtrl);
#endif

#ifndef _AUDIO_SPEED_CHANGE
#define _AUDIO_SPEED_CHANGE		(\
									  (_SBC_PLAY		&&	_SBC_SPEED_CHANGE)\
									||(_SBC_CH2_PLAY	&&	_SBC_CH2_SPEED_CHANGE)\
									||(_ADPCM_PLAY		&&	_ADPCM_SPEED_CHANGE)\
									||(_CELP_PLAY		&&	_CELP_SPEED_CHANGE)\
									||(_PCM_PLAY		&&	_PCM_SPEED_CHANGE)\
									||(_PCM_CH2_PLAY	&&	_PCM_CH2_SPEED_CHANGE)\
									||(_PCM_CH3_PLAY	&&	_PCM_CH3_SPEED_CHANGE)\
								)
#endif
#ifndef	_AUDIO_PITCH_CHANGE
#define _AUDIO_PITCH_CHANGE		(\
									  (_SBC_PLAY		&&	_SBC_PITCH_CHANGE)\
									||(_SBC_CH2_PLAY	&&	_SBC_CH2_PITCH_CHANGE)\
									||(_ADPCM_PLAY		&&	_ADPCM_PITCH_CHANGE)\
									||(_CELP_PLAY		&&	_CELP_PITCH_CHANGE)\
									||(_PCM_PLAY		&&	_PCM_PITCH_CHANGE)\
									||(_PCM_CH2_PLAY	&&	_PCM_CH2_PITCH_CHANGE)\
									||(_PCM_CH3_PLAY	&&	_PCM_CH3_PITCH_CHANGE)\
								)
#endif
#ifndef	_AUDIO_ROBOT1_SOUND
#define _AUDIO_ROBOT1_SOUND		(\
									  (_SBC_PLAY		&&	_SBC_ROBOT1_SOUND)\
									||(_SBC_CH2_PLAY	&&	_SBC_CH2_ROBOT1_SOUND)\
									||(_ADPCM_PLAY		&&	_ADPCM_ROBOT1_SOUND)\
									||(_CELP_PLAY		&&	_CELP_ROBOT1_SOUND)\
									||(_PCM_PLAY		&&	_PCM_ROBOT1_SOUND)\
									||(_PCM_CH2_PLAY	&&	_PCM_CH2_ROBOT1_SOUND)\
									||(_PCM_CH3_PLAY	&&	_PCM_CH3_ROBOT1_SOUND)\
								)
#endif
#ifndef _AUDIO_ROBOT2_SOUND
#define _AUDIO_ROBOT2_SOUND		(\
									  (_SBC_PLAY		&&	_SBC_ROBOT2_SOUND)\
									||(_SBC_CH2_PLAY	&&	_SBC_CH2_ROBOT2_SOUND)\
									||(_ADPCM_PLAY		&&	_ADPCM_ROBOT2_SOUND)\
									||(_CELP_PLAY		&&	_CELP_ROBOT2_SOUND)\
									||(_PCM_PLAY		&&	_PCM_ROBOT2_SOUND)\
									||(_PCM_CH2_PLAY	&&	_PCM_CH2_ROBOT2_SOUND)\
									||(_PCM_CH3_PLAY	&&	_PCM_CH3_ROBOT2_SOUND)\
								)
#endif
#ifndef	_AUDIO_ROBOT3_SOUND
#define _AUDIO_ROBOT3_SOUND		(\
									  (_SBC_PLAY		&&	_SBC_ROBOT3_SOUND)\
									||(_SBC_CH2_PLAY	&&	_SBC_CH2_ROBOT3_SOUND)\
									||(_ADPCM_PLAY		&&	_ADPCM_ROBOT3_SOUND)\
									||(_CELP_PLAY		&&	_CELP_ROBOT3_SOUND)\
									||(_PCM_PLAY		&&	_PCM_ROBOT3_SOUND)\
									||(_PCM_CH2_PLAY	&&	_PCM_CH2_ROBOT3_SOUND)\
									||(_PCM_CH3_PLAY	&&	_PCM_CH3_ROBOT3_SOUND)\
								)
#endif
#ifndef	_AUDIO_ROBOT4_SOUND
#define _AUDIO_ROBOT4_SOUND		(\
									(_SBC_PLAY		&&	_SBC_ROBOT4_SOUND)\
									||(_ADPCM_PLAY	&&	_ADPCM_ROBOT4_SOUND))
#endif
#ifndef	_AUDIO_REVERB_SOUND
#define _AUDIO_REVERB_SOUND		(\
									  (_SBC_PLAY		&&	_SBC_REVERB_SOUND)\
									||(_SBC_CH2_PLAY	&&	_SBC_CH2_REVERB_SOUND)\
									||(_ADPCM_PLAY		&&	_ADPCM_REVERB_SOUND)\
									||(_CELP_PLAY		&&	_CELP_REVERB_SOUND)\
									||(_PCM_PLAY		&&	_PCM_REVERB_SOUND)\
									||(_PCM_CH2_PLAY	&&	_PCM_CH2_REVERB_SOUND)\
									||(_PCM_CH3_PLAY	&&	_PCM_CH3_REVERB_SOUND)\
								)
#endif
#ifndef	_AUDIO_ECHO_SOUND
#define _AUDIO_ECHO_SOUND		(\
									  (_SBC_PLAY		&&	_SBC_ECHO_SOUND)\
									||(_SBC_CH2_PLAY	&&	_SBC_CH2_ECHO_SOUND)\
									||(_ADPCM_PLAY		&&	_ADPCM_ECHO_SOUND)\
									||(_CELP_PLAY		&&	_CELP_ECHO_SOUND)\
									||(_PCM_PLAY		&&	_PCM_ECHO_SOUND)\
									||(_PCM_CH2_PLAY	&&	_PCM_CH2_ECHO_SOUND)\
									||(_PCM_CH3_PLAY	&&	_PCM_CH3_ECHO_SOUND)\
								)
#endif
#ifndef	_AUDIO_DARTH_SOUND
#define _AUDIO_DARTH_SOUND		(_ADPCM_PLAY		&&	_ADPCM_DARTH_SOUND)
#endif
#ifndef	_AUDIO_SPEEDPITCH_N
#define _AUDIO_SPEEDPITCH_N		(_ADPCM_PLAY		&&	_ADPCM_SPEEDPITCH_N)
#endif
#ifndef	_AUDIO_ANIMAL_ROAR
#define _AUDIO_ANIMAL_ROAR 		(_ADPCM_PLAY		&&	_ADPCM_ANIMAL_ROAR)
#endif
typedef struct {
    //U8      Play_Mode;
    U8      Volume;
    U8      EQFilter_Index;
#if _AUDIO_SPEED_CHANGE
    S8      Speed_Index;
#endif
#if _AUDIO_PITCH_CHANGE
    S8      Pitch_Index;
#endif
#if _AUDIO_ROBOT1_SOUND
    U8      Robot1_Cmd;
    U8      Robot1_Index;
#endif
#if	_AUDIO_ROBOT2_SOUND
    U8      Robot2_Cmd;
    U8      Robot2_Type;
    U8      Robot2_Thres;
#endif
#if _AUDIO_ROBOT3_SOUND
    U8      Robot3_Cmd;
    U8      Robot3_Type;
    U8      Robot3_Thres;
#endif
#if _AUDIO_ROBOT4_SOUND
    U8      Robot4_Cmd;
#endif
#if _AUDIO_REVERB_SOUND
    U8      Reverb_Cmd;
    U8      Reverb_Index;
#endif
#if _AUDIO_ECHO_SOUND
    U8      Echo_Cmd;//share with Echo 2ndGen
    U8      Echo_Index;
#endif
#if _AUDIO_DARTH_SOUND
    U8      Darth_Cmd;
    S8      Darth_PitchIndex;
    U8      Darth_CarrierType;
#endif
#if _AUDIO_SPEEDPITCH_N     
    S8 		SpeedPitch_Index;
#endif
#if _AUDIO_MIX_CONTROL
    U8      MixCtrl;
#endif
#if _AUDIO_ANIMAL_ROAR
	U8		AnimalRoar_cmd;
	S8 		AnimalRoar_speed_index;
	S8 		AnimalRoar_pitch_index;
	U8 		AnimalRoar_reverb_index;
#endif
}S_AUDIO_PARAMETER;

typedef const struct {
	U8		(*Play)(U32 index,U8 mode);
    void	(*Pause)(void);
    void	(*Resume)(void);
    void	(*Stop)(void);
    void	(*SetVolume)(U8 vol);
    U8		(*GetStatus)(void);
    void	(*SetEQFilter)(U8 index);
#if _AUDIO_SPEED_CHANGE
    void    (*SetSpeed)(S8 index);
#endif
#if _AUDIO_PITCH_CHANGE
    void    (*SetPitch)(S8 index);
#endif
#if _AUDIO_ROBOT1_SOUND
    void    (*SetRobot1Sound)(U8 cmd,U8 index);
#endif
#if	_AUDIO_ROBOT2_SOUND
    void    (*SetRobot2Sound)(U8 cmd,U8 type, U8 thres);
#endif
#if _AUDIO_ROBOT3_SOUND
    void    (*SetRobot3Sound)(U8 cmd,U8 type, U8 thres);
#endif
#if _AUDIO_ROBOT4_SOUND
    void    (*SetRobot4Sound)(U8 cmd);
#endif
#if _AUDIO_REVERB_SOUND
    void    (*SetReverbSound)(U8 cmd,U8 index);
#endif
#if _AUDIO_ECHO_SOUND
    void    (*SetEchoSound)(U8 cmd,U8 index);
    void    (*SetEchoSound3rdGen)(U8 cmd);
#endif
#if _AUDIO_DARTH_SOUND
    void    (*SetDarthSound)(U8 Cmd, S8 PitchIndex, U8 CarrierType);
#endif
#if _AUDIO_SPEEDPITCH_N
    void    (*SetSpeedPitchN)(S8 speedpitch_index);
#endif
#if _AUDIO_MIX_CONTROL
    void    (*SetMixCtrl)(U8 mixCtrl);
#endif
#if _AUDIO_ANIMAL_ROAR
	void	(*SetAnimalRoar)(U8 cmd, S8 speed_index, S8 pitch_index, U8 reverb_index);
#endif
}S_AUDIO_FUNC_IDX;
//AUDIO basic API interface with all codec
extern S_AUDIO_FUNC_IDX s_MIDI_FUNC;
extern S_AUDIO_FUNC_IDX s_SBC_FUNC;
extern S_AUDIO_FUNC_IDX s_SBC_CH2_FUNC;
extern S_AUDIO_FUNC_IDX s_SBC2_FUNC;
extern S_AUDIO_FUNC_IDX s_SBC2_CH2_FUNC;
extern S_AUDIO_FUNC_IDX s_ADPCM_FUNC;
extern S_AUDIO_FUNC_IDX s_ADPCM_CH2_FUNC;
extern S_AUDIO_FUNC_IDX s_ADPCM_CH3_FUNC;
extern S_AUDIO_FUNC_IDX s_ADPCM_CH4_FUNC;
extern S_AUDIO_FUNC_IDX s_ADPCM_CH5_FUNC;
extern S_AUDIO_FUNC_IDX s_ADPCM_CH6_FUNC;
extern S_AUDIO_FUNC_IDX s_ADPCM_CH7_FUNC;
extern S_AUDIO_FUNC_IDX s_ADPCM_CH8_FUNC;
extern S_AUDIO_FUNC_IDX s_CELP_FUNC;
extern S_AUDIO_FUNC_IDX s_PCM_FUNC;
extern S_AUDIO_FUNC_IDX s_PCM_CH2_FUNC;
extern S_AUDIO_FUNC_IDX s_PCM_CH3_FUNC;

//_AUDIO_RECORD
#define AUDIO_RECORD_ADPCM_BUF_SIZE  	(ADPCM_DEC_OUT_SIZE*6)//without ECHO use ADPCM_DEC_OUT_SIZE*3 or with ECHO use ADPCM_DEC_OUT_SIZE*6
#define AUDIO_RECORD_DAC_BUF_SIZE  		21
#define AUDIO_RECORD_REC_RAM           	(AUDIO_RECORD_DAC_BUF_SIZE+(AUDIO_RECORD_ADPCM_BUF_SIZE<<1)+ADPCM_ENC_ALL_SIZE)

extern void __attribute__((optimize("O0"),aligned(4))) SYS_OUTPUT_1KHz(GPIO_TypeDef *gpiox, U8 pin, U32 ms);

#ifndef _API_ARGUMENT_PROTECT
#if	_QCODE_BUILD
#define _API_ARGUMENT_PROTECT	DISABLE
#else
#define _API_ARGUMENT_PROTECT	ENABLE
#endif
#endif

#if _SYS_LVD_MONITOR_MODULE
void LVD_Monitor_Init();
void LVD_Monitor_ServiceLoop();
void LVD_Monitor_Sleep();
void LVD_Monitor_WakeUp();
void CB_LVD_Monitor_Event(U8 _lvdLevel);
#if _EF_SERIES
#define _LVD_MONITOR_LEVEL  8
#else
#define _LVD_MONITOR_LEVEL  6
#endif
#endif

#endif //#ifndef  __ASSEMBLY__

#endif //#ifndef INCLUDE_H


