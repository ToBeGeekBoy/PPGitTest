/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "include.h"
#include "nx1_dac.h"
#include "nx1_spi.h"
#include "nx1_fmc.h"
#include "nx1_intc.h"
//#include "nx1_gpio.h"
//#include "debug.h"
#include "utility_define.h"



#if _ADPCM_PLAY||_ADPCM_RECORD

//#define DEBUG_RECORDERASING
#ifdef  DEBUG_RECORDERASING
#define REprintf dprintf
#else
#define REprintf(...) 
#endif

#if _EF_SERIES && ((_ADPCM_RECORD && _ADPCM_RECORD_ERASING) || (_SBC_RECORD && _SBC_RECORD_ERASING && _ADPCM_PLAY && _ADPCM_PLAY_SR_FOR_SBC_REC))
#if OPTION_IRAM
#define AudioBufCtl 			AudioBufCtl_ADPCM
#define BufCtl					BufCtl_ADPCM
#else
//#error "If you need to use _ADPCM_RECORD_ERASING, Config Block Setting IRAM should be set as ENABLE"
#endif
extern U32 intcCmdTemp;
#endif
/* Defines -------------------------------------------------------------------*/
#if VOICE_CHANGER_ADPCM_USED
#define AGC_CTL_ENABLE          (VC_ADPCM_AGC_FUNC)
#define TARGET_RMS              (VC_ADPCM_AGC_TARGET_RMS)
#define ALPHA                   (VC_ADPCM_AGC_ALPHA)
#define BETA                    (VC_ADPCM_AGC_BETA)
#define LIMITER_THRESHOLD       (VC_ADPCM_AGC_LIMITER)

#define NOISE_GATE_ENABLE       (VC_ADPCM_NOISE_GATE_FUNC)
#define NG_ON_OFF_SAMPLE_16K   	(VC_ADPCM_NG_ON_OFF_SAMPLE_16K)
#define NG_ATTACK_SAMPLE_16K    (VC_ADPCM_NG_ATTACK_SAMPLE_16K)
#define NG_RELEASE_SAMPLE_16K   (VC_ADPCM_NG_RELEASE_SAMPLE_16K)
#define NG_ON_OFF_SAMPLE_8K     (VC_ADPCM_NG_ON_OFF_SAMPLE_8K)
#define NG_ATTACK_SAMPLE_8K     (VC_ADPCM_NG_ATTACK_SAMPLE_8K)
#define NG_RELEASE_SAMPLE_8K    (VC_ADPCM_NG_RELEASE_SAMPLE_8K)
#else
#define AGC_CTL_ENABLE      	0

#define TARGET_RMS     			MINUS_5DB                           // Adjustable, Target RMS signal magnitude 			
#define ALPHA           		4096*4//4096//4096                  // 0.125; coeff. of 1st order IIR LPF 
#define BETA            		1024//(2048/2)                      // 0.125; step size constant 
#define LIMITER_THRESHOLD   	30000
#define FULL_SCALE_LIMIT 		33000//45000//26000//32000
#define DIGITAL_GAIN_VAD_TH     1



#define NOISE_GATE_ENABLE   	1

#define NG_ON_OFF_SAMPLE_16K   	160//10ms
#define NG_ATTACK_SAMPLE_16K    80//5ms
#define NG_RELEASE_SAMPLE_16K   (320*5)//100ms

#define NG_ON_OFF_SAMPLE_8K     80//10ms
#define NG_ATTACK_SAMPLE_8K     40//5ms
#define NG_RELEASE_SAMPLE_8K    (160*5)//100ms
#endif //VOICE_CHANGER_ADPCM_USED

#define SMOOTH_ENABLE           1

#define SOUND_TRIG_RECOVER      1           // only for _ADPCM_RECORD_SOUND_TRIG

#define RECORD_ERASE_SPI_START_TIME_RSV		512
#define RECORD_ERASE_SPI_START_TIME 		(SPI_SECTOR_SIZE-RECORD_ERASE_SPI_START_TIME_RSV)
#define RECORD_ERASE_FMC_START_TIME_RSV		(21*5)
#define RECORD_ERASE_FMC_START_TIME 		(MAIN_FLASH_SECTORSIZE-RECORD_ERASE_FMC_START_TIME_RSV)
#define NEW_ECHO    			0

#if _ADPCM_ECHO_SOUND&&(_ADPCM_ECHO_EFFECT==DELAY_USER_DEFINE)&&(((!_ADPCM_ECHO_UD_DELAY_TIME)&&(_ADPCM_ECHO_UD_REPEAT_TIME))||((_ADPCM_ECHO_UD_DELAY_TIME)&&(!_ADPCM_ECHO_UD_REPEAT_TIME)))
	#error "ECHO effect user define mode, the _ADPCM_ECHO_UD_DELAY_TIME & _ADPCM_ECHO_UD_REPEAT_TIME must both be set!"
#endif

#define DC_REMOVE				1

#if		_ADPCM_RECORD==ENABLE
	#if		_ADPCM_RECORD_PLAY_EQ==ENABLE
		#if		_ADPCM_RECORD_PLAY_TYPE==TYPE1//DRC force EQ2
			#define _ADPCM_RECORD_PLAY_USE_EQ2//define EQ2 and DRC
			#define _ADPCM_RECORD_PLAY_USE_DRC
		#elif		_ADPCM_EQ_TYPE==TYPE0
			#define _ADPCM_RECORD_PLAY_USE_EQ0//define EQ0
		#elif	_ADPCM_EQ_TYPE==TYPE1
			#define _ADPCM_RECORD_PLAY_USE_EQ1//define EQ1
		#elif	_ADPCM_EQ_TYPE==CUSTOMIZE//customize EQ
			#include "FilterTab.h"
			#if	FILTER_NUMBER<=0//check table size
				#error "Filter table is not existed, please update NYIDE and create filter file."
			#endif
			#define _ADPCM_RECORD_PLAY_USE_CUSTOMIZE//define EQ customize
		#endif
	#endif
	#if		_ADPCM_RECORD_PLAY_TYPE==TYPE2
		#define _ADPCM_RECORD_PLAY_USE_DIGITAL_GAIN//define DigiGain
	#endif
#endif

#if		_ADPCM_PLAY_BACK_EQ==ENABLE
	#include "FilterTab.h"
	#if	FILTER_NUMBER<=0//check table size
		#error "Filter table is not existed, please update NYIDE and create filter file."
	#endif
	#define _ADPCM_PLAY_BACK_USE_CUSTOMIZE//define EQ customize
#endif

S_AUDIO_FUNC_IDX s_ADPCM_FUNC = {
    .Play           =   ADPCM_Play,
    .Pause          =   ADPCM_Pause,
    .Resume         =   ADPCM_Resume,
    .Stop           =   ADPCM_Stop,
    .SetVolume      =   ADPCM_SetVolume,
    .GetStatus      =   ADPCM_GetStatus,
#if defined(_ADPCM_RECORD_PLAY_USE_CUSTOMIZE)   \
    || defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
    .SetEQFilter    =   ADPCM_SetEQFilter,
#endif
#if _ADPCM_SPEED_CHANGE
    .SetSpeed       =   ADPCM_SetSpeed,
#endif
#if _ADPCM_PITCH_CHANGE
    .SetPitch       =   ADPCM_SetPitch,
#endif
#if _ADPCM_ROBOT1_SOUND
    .SetRobot1Sound =   ADPCM_SetRobot1Sound,
#endif
#if _ADPCM_ROBOT2_SOUND
    .SetRobot2Sound =   ADPCM_SetRobot2Sound,
#endif
#if _ADPCM_ROBOT3_SOUND
    .SetRobot3Sound =   ADPCM_SetRobot3Sound,
#endif
#if _ADPCM_ROBOT4_SOUND	
    .SetRobot4Sound =   ADPCM_SetRobot4Sound,
#endif
#if _ADPCM_REVERB_SOUND
    .SetReverbSound =   ADPCM_SetReverbSound,
#endif
#if _ADPCM_ECHO_SOUND
    //Parameter not compact with other
    //.SetEchoSound   =   ADPCM_SetEchoSound,
    .SetEchoSound3rdGen=ADPCM_SetEchoSound,  
#endif
#if _ADPCM_DARTH_SOUND
    .SetDarthSound  =   ADPCM_SetDarthSound,
#endif
#if _ADPCM_SPEEDPITCH_N
    .SetSpeedPitchN  =  ADPCM_SetSpeedPitchN,
#endif
#if _AUDIO_MIX_CONTROL
    .SetMixCtrl     =   ADPCM_SetMixCtrl,
#endif
#if _ADPCM_ANIMAL_ROAR
    .SetAnimalRoar  =   ADPCM_SetAnimalRoar,
#endif
};

/* Static Functions ----------------------------------------------------------*/
#if _ADPCM_RECORD
static void WriteHeader(void);
#endif
#if _ADPCM_DARTH_SOUND
static void SetDarthSound_Init(U8 rst);
#endif
/* Constant Table ------------------------------------------------------------*/
//static const U16 VolTbl[16]={
//    0,
//    2920-DAC_DATA_OFFSET,
//    3471-DAC_DATA_OFFSET,
//    4125-DAC_DATA_OFFSET,
//    4903-DAC_DATA_OFFSET,
//    5827-DAC_DATA_OFFSET,
//    6925-DAC_DATA_OFFSET,
//    8231-DAC_DATA_OFFSET,
//    9783-DAC_DATA_OFFSET,
//    11627-DAC_DATA_OFFSET,
//    13818-DAC_DATA_OFFSET,
//    16423-DAC_DATA_OFFSET,
//    19519-DAC_DATA_OFFSET,
//    23198-DAC_DATA_OFFSET,
//    27571-DAC_DATA_OFFSET,
//    32767-DAC_DATA_OFFSET,
//}; 





#if _ADPCM_SPEED_CHANGE || _ADPCM_ANIMAL_ROAR
static const float SpeedRatioTbl[25]={
    0.5,
    0.54,
    0.58,
    0.62,
    0.67,
    0.71,
    0.75,
    0.79,
    0.83,
    0.87,
    0.92,
    0.96,
    1,
    1.08,
    1.17,
    1.25,
    1.33,
    1.42,
    1.45,
    1.58,
    1.67,
    1.75,
    1.83,
    1.92,
    2,
};  
#endif

#if _ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ANIMAL_ROAR
static const float PitchRatioTbl[25]={
    0.5,
    0.53,
    0.56,
    0.59,
    0.63,
    0.67,
    0.72,
    0.75,
    0.79,
    0.84,
    0.89,
    0.94,
    1,
    1.06,
    1.12,
    1.19,
    1.26,
    1.33,
    1.41,
    1.50,
    1.59,
    1.68,
    1.78,
    1.89,
    2,
};  
#endif

#if _ADPCM_ROBOT1_SOUND 
static const U16 CarrierTbl[16]={
    220,
    250,
    280,
    310,
    340,
    370,
    400,
    440,
    495,
    550,
    606,
    660,
    715,
    770,
    825,
    880,
};    
#endif 

#if defined(_ADPCM_RECORD_PLAY_USE_DIGITAL_GAIN)
#if !VOICE_CHANGER_ADPCM_USED
static const S16 ADPCMDigiAmpArray[14] = {
	/* in office */
	15,     // (%),MinGain 
	200,    // (%),MaxGain 
	100,    // (%),LimitThreshold
	4096,   // EnvUpdateVel=4096/32767
	4,      // GainUpdateVel=1(slow)~16(fast)
	150,    // GainReleaseVel=1(fast)~256(slow)
	55,     // (%),TargetRMS, (40~80 is better)
	1966,   // StepSize=1966/32767
	/*RT_VAD function*/
	1,//0, 	    // RT_VAD: on=1, off=0
	3000,   // FixedVADTH, when RT_VAD off, it works.
	1,      // Gain status, 0 ~ 2
	10,     // (%), unvoice段落 decrease MinGain 
	100,    // (%), unvoice段落 decrease MaxGain 
	200,    // unvoice段落 decrease GainReleaseVel=1(fast)~256(slow)
};
static const U16 ADPCMVADArray[9] = {
	
	/*Limitation*/ 
	100,    //(%),Max Avg Limit
	3,      //(%),Min Avg Limit 
	/* Initialize*/
    0,      // Average Value Update initialize
	/* Start Detection */
	2, //3,		// Average history velocity, Threshold更新速度: 0(固定閾值),1(更新慢)~32(更新快).
	10,//3000,	// Voice AVG threshold :0~32767 
	2,		// Start Detect condition : Voice duration(frames) 
	0,		// Start Detect condition : Voice max peak :0~32767 
	/* End Detection */
	30,//7,		// End Detect condition : Voice duration(frames)
	5,          //aspirated voice keep level  3 ~ 5; 
};
static const S16 ADPCMDigiAmpSet[2] = {	
	32,     //frame size
	0,       //sample base or frame base, value 0 or 1
};
#endif //VOICE_CHANGER_ADPCM_USED
#endif

#if _ADPCM_DARTH_SOUND
static const U8 StepSizeTb[4] = { 4, 5, 6, 5 };
#endif

/* Global Variables ----------------------------------------------------------*/
#if _AUDIO_MIX_CONTROL
U8 AdpcmMixCtrl=MIX_CTRL_100;
#endif

/* Static Variables ----------------------------------------------------------*/
SCOPE_TYPE volatile struct AUDIO_BUF_CTL{
    S32 process_count;	//decode/encode count
    S32 fifo_count;	    //decode/encode count
    S32 fifo_count_tmp;
    U32 samples;	    //samples
	
#if _ADPCM_ECHO_SOUND && !(_ADPCM_SPEED_CHANGE || _ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ROBOT3_SOUND|| _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR)
	U32 samples2;		//for echo-delay used
#endif	
	
#if _ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC || _ADPCM_DARTH_SOUND || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)
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
}AudioBufCtl;
#if _ADPCM_RECORD_ERASING 
static RecordErasingCtrl_T  RecordErasingCtrl;
#endif
//
//#if _ADPCM_RECORD
//static U32 StorageIndex;
//#endif
#if _ADPCM_RECORD
#if DC_REMOVE
static S32 RunFilterBuf[8];
#endif
#if AGC_CTL_ENABLE
static U8 AgcCnt;
#endif

static U32 g_playrecord;
static U32 RecordSize;
static U16 SkipTailTime = 0;
#if SMOOTH_ENABLE
static U8 playEndSmoothDownFlag;
#if _ADPCM_SPEED_CHANGE || _ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND ||_ADPCM_ROBOT3_SOUND || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR
static U8 fifo_count2_lastFlag;
#endif
#endif
#endif

static U16 AudioVol;
static U8  OpMode,ChVol;

SCOPE_TYPE volatile struct{
    U16 *buf;
    U16 size;
    U16 offset;
    U16 rd_offset;
    U16 wr_offset;
}BufCtl;   

#if SMOOTH_ENABLE
static volatile struct{
    S16 count;              //sample count in process
    U8  step_sample;        //sample size for each step
    U8  smooth;             //up or down or none
    U8  state;              //current state
}SmoothCtl;
#endif

static const U8 Signature[]="NX1";

#if _ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR
static struct{
    U16 index;
#if _ADPCM_ROBOT3_SOUND
    U16 rb_ptr;
#endif
#if _ADPCM_SPEED_CHANGE
    U8 speed_ratio_index;            //speed control ratio
#endif
#if _ADPCM_PITCH_CHANGE
    U8 pitch_ratio_index;            //pitch control ratio
#endif
#if _ADPCM_DARTH_SOUND
    U8 darth_ratio_index;            //darth_vader ratio
#endif
#if _ADPCM_SPEEDPITCH_N
    S8 speedpitch_ratio_index;       //speedpitch control ratio
#endif
}SpeedCtl;
#endif

#if _ADPCM_SPEED_CHANGE&&_ADPCM_PITCH_CHANGE
static U8  SpeedPitchFlag=0, SpeedPitchInitFlag;
#endif

#if _ADPCM_SPEEDPITCH_N
static U8  SpeedPitchNFlag=0, SpeedPitchNInitFlag;
#endif

#if _ADPCM_ROBOT1_SOUND 
static struct{
    U8 enable;
    U8 change;
    U8 index;
}RobotCtl;
#endif 

#if _ADPCM_ROBOT2_SOUND
static struct{
    U8 enable;
    U8 type;
    U16 thres;
    U16 detectthres;
}Robot01Ctl;
#endif 
#if _ADPCM_ROBOT3_SOUND
static struct{
    U8 enable;
    U8 type;
    U16 thres;
}Robot02Ctl;
#endif 

#if _ADPCM_ROBOT4_SOUND
static struct{
    U32 offset;
    U8  enable;
}Robot04Ctl;
#endif

#if _ADPCM_REVERB_SOUND
static U8 ReverbCmd,ReverbEffect;
#endif

#if _ADPCM_ECHO_SOUND
static struct{
    U8	Cmd;
    U32 Offset;
	U8  SR_Limit;
}EchoCtrl;

static U32 echo_bufSize;

static volatile NewEchoCtrl echoCtrl_T;
#endif

#if _ADPCM_DARTH_SOUND
static struct{
    U8 Cmd;
    S8 Effect;
}DarthCtrl;
#endif

#if _ADPCM_ANIMAL_ROAR
static struct{
    U8 cmd; 
    U8 initFlag;
}AnimalRoarCtl;
static DINOSAURCTL AnimalRoarALGOCtl;
#endif

#if (_ADPCM_ECHO_SOUND || _ADPCM_ROBOT4_SOUND ||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)) && !(_ADPCM_SPEED_CHANGE || _ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR)
static struct{
	U32	RefFrame;
	U32	CntFrame;
	U8	Last;
}playEnd;

static U8	lastAudio;
#endif

static U8  StorageMode;
static U32 StartAddr,CurAddr;
#if _ADPCM_RECORD && _ADPCM_RECORD_ERASING && _ADPCM_RECORD_SOUND_TRIG && SOUND_TRIG_RECOVER
static U32 RecoverAddr;
#endif

SCOPE_TYPE U8  PlayEndFlag;

#if _ADPCM_PLAY_REPEAT
U8 ADPCM_PlayRepeatFlag;
#endif

#if _AUDIO_VOLUME_CHANGE_SMOOTH
static volatile VOLCTRL_T    VolCtrl;
#endif

#if defined(_ADPCM_RECORD_PLAY_USE_DIGITAL_GAIN)
static S16 ADPCMDigiAmpBuf[36];
static S16 ADPCMDigiAmpCTLBuf[34];
#endif 

#if defined(_ADPCM_RECORD_PLAY_USE_DRC)
static S8 DRC_buf[14] = { 0 };
static volatile DRCFixedCtl drcFixedCtl;
#endif

#if defined(_ADPCM_RECORD_PLAY_USE_EQ0)			\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ1)		\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ2)		\
	||defined(_ADPCM_RECORD_PLAY_USE_CUSTOMIZE)	\
	||defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
static EQCtl EQFilterCtl;
#endif
#if defined(_ADPCM_RECORD_PLAY_USE_CUSTOMIZE)	\
	||defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
static U8	u8EQFilterIndex=0;
#endif

#if _ADPCM_SW_UPSAMPLE
static volatile struct{
    S16 dataBuf[2];
    S8  upsampleIndex;
    U8  cmd;
    U8  upsampleFactor;
}swUpsampleCtrl;
#endif

/* Dynamic Allocate RAM-------------------------------------------------------*/
#if _USE_HEAP_RAM
#if !(_ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)) || _ADPCM_RECORD
static U8  *EncDataBuf=NULL;
#if _ADPCM_RECORD
static U8  *EncBuf=NULL;
#endif
#endif
#if _ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR
static U16 *SpeedCtlBuf=NULL;
#endif
#if _ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_RECORD || _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR || (_ADPCM_DECODE_FLOW_TYPE==TYPE1)
static U16 *AudioBuf=NULL;
#if PR_READ&&(!_ADPCM_RECORD)//If the "_ADPCM_RECORD" is enable, the "EncDataBuf" has already defined. 
static U8  *EncDataBuf=NULL;
#endif
#endif

#if (_ADPCM_PITCH_CHANGE&&_ADPCM_SPEED_CHANGE) 
SPEEDPITCHBUF *SpeedPitchBuf = NULL;  
#endif

#if _ADPCM_SPEEDPITCH_N
S8* SpeedPitchN = NULL;
#endif

#if (_ADPCM_ECHO_SOUND || _ADPCM_ROBOT4_SOUND || (_ADPCM_DECODE_FLOW_TYPE==TYPE1)) && !(_ADPCM_SPEED_CHANGE || _ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR)
static U16 *tmp_AudioBuf=NULL; 
#endif

#if _ADPCM_ECHO_SOUND
static S16  *EchoBuf=NULL; 
#endif

#if _ADPCM_ROBOT2_SOUND
static S16 *Robot01_buf=NULL;
#endif

#if _ADPCM_ROBOT3_SOUND
static S16 *Robot02_buf=NULL;
#endif

#if _ADPCM_ROBOT4_SOUND
ROBOT4Ctl* Robot4ctl = NULL;
ROBOT4Buf* Robot4buf = NULL;
FFTctl* Robot4FFTctl = NULL;
#endif

#if _ADPCM_REVERB_SOUND
static S16 *Reverb_x=NULL;
static S16 *Reverb_y=NULL;
#endif

#if _ADPCM_RECORD
#if _ADPCM_RECORD_ERASING
static U8 *EraseRecordBuf=NULL;
#endif
#if AGC_CTL_ENABLE
static U16 *AgcBuf=NULL;
#endif
#endif

#if defined(_ADPCM_RECORD_PLAY_USE_EQ0)			\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ1)		\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ2)		\
	||defined(_ADPCM_RECORD_PLAY_USE_CUSTOMIZE)	\
	||defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
static S32 *EqBuf=NULL;
#endif

#if _ADPCM_DARTH_SOUND
static DARTH_VADER_RAM_T *DarthVaderRam=NULL;
static DARTH_VADER_CTRL_T *DarthVaderCtrl=NULL;
#endif

#if _ADPCM_ANIMAL_ROAR
static S8 *AnimalRoarTotalRam=NULL;
#endif

static U8  *ModeBuf=NULL;
static U8  *DecBuf=NULL;
#else
#if !(_ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)) || _ADPCM_RECORD
static U8 EncDataBuf[ADPCM_DATA_BUF_SIZE];
#if _ADPCM_RECORD
static U8 EncBuf[ADPCM_ENC_ALL_SIZE];
#endif
#endif
#if (_ADPCM_SPEED_CHANGE&&_ADPCM_PITCH_CHANGE) || _ADPCM_ANIMAL_ROAR
static U16 SpeedCtlBuf[SPEED_CTL_FRAME_SIZE+130];
#elif _ADPCM_ROBOT3_SOUND
static U16 SpeedCtlBuf[SPEED_CTL_FRAME_SIZE+40];
#elif _ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_SPEEDPITCH_N
static U16 SpeedCtlBuf[SPEED_CTL_FRAME_SIZE];
#endif

#if _ADPCM_ROBOT4_SOUND
ROBOT4Ctl  _Robot4ctl;
ROBOT4Ctl* Robot4ctl = &_Robot4ctl;
ROBOT4Buf _Robot4buf;
ROBOT4Buf* Robot4buf = &_Robot4buf;
FFTctl  _Robot4FFTctl;
FFTctl* Robot4FFTctl = &_Robot4FFTctl;
#endif

#if _ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_RECORD || _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR || (_ADPCM_DECODE_FLOW_TYPE==TYPE1)
static U16 AudioBuf[ADPCM_AUDIO_BUF_SIZE];
#if PR_READ&&(!_ADPCM_RECORD)//If the "_ADPCM_RECORD" is enable, the "EncDataBuf" has already defined.  
static U8 EncDataBuf[ADPCM_DATA_BUF_SIZE];
#endif
#endif

#if _ADPCM_PITCH_CHANGE&&_ADPCM_SPEED_CHANGE
SPEEDPITCHBUF _SpeedPitchBuf;
SPEEDPITCHBUF* SpeedPitchBuf = &_SpeedPitchBuf;
#endif

#if (_ADPCM_ECHO_SOUND || _ADPCM_ROBOT4_SOUND||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)) && !(_ADPCM_SPEED_CHANGE || _ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR)
static U16 tmp_AudioBuf[ADPCM_AUDIO_BUF_SIZE01]; 
#endif

#if _ADPCM_ECHO_SOUND
static S16  EchoBuf[NEW_ADPCM_ECHO_BUF_SIZE];	//Need s/w tool support, now set max. size(16k)	
#endif

#if _ADPCM_ROBOT2_SOUND
static S16 Robot01_buf[ADPCM_ROBOT_BUF_SIZE];
#endif

#if _ADPCM_ROBOT3_SOUND
static S16 Robot02_buf[ADPCM_ROBOT02_BUF_SIZE];
#endif

#if _ADPCM_REVERB_SOUND
static S16 Reverb_x[ADPCM_REVERB_BUF_SIZE];
static S16 Reverb_y[ADPCM_REVERB_BUF_SIZE];
#endif

#if _ADPCM_RECORD
#if _ADPCM_RECORD_ERASING
static U8 EraseRecordBuf[ADPCM_ERASE_RECORD_SIZE];
#endif
#if AGC_CTL_ENABLE
static U16 AgcBuf[AGC_FRAME_SIZE];
#endif
#endif

#if defined(_ADPCM_RECORD_PLAY_USE_EQ0)			\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ1)		\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ2)		\
	||defined(_ADPCM_RECORD_PLAY_USE_CUSTOMIZE)	\
	||defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
static S32 EqBuf[ADPCM_EQ_BUF_SIZE];
#endif

#if _ADPCM_DARTH_SOUND
static DARTH_VADER_RAM_T DarthVaderRam[1];
static DARTH_VADER_CTRL_T DarthVaderCtrl[1];
#endif

#if _ADPCM_ANIMAL_ROAR
static S8 AnimalRoarTotalRam[ANIMAL_ROAR_RAM_SIZE];
#endif

#if _ADPCM_SPEEDPITCH_N
S8 SpeedPitchN[SpeedPitchNSIZE];
#endif

static U8 ModeBuf[ADPCM_MODE_SIZE];
static U8 DecBuf[ADPCM_DEC_PARAM_SIZE]; 
#endif

/* Extern Functions ----------------------------------------------------------*/
extern void CB_ReadData(U8 mode, U32 *cur_addr, U8 *buf,U16 size);
extern void CB_Seek(U32 offset);
extern void CB_EraseData(U8 mode, U32 addr,U32 size);
extern void CB_WriteData(U8 mode, U32 *cur_addr,U8 *buf,U16 size);
extern void CB_WriteHeader(U8 mode, U32 *cur_addr,U8 *buf,U16 size);
extern U8   CB_SetStartAddr(U32 addr,U8 mode,U8 rw,U32 *start_addr);
extern void CB_ADPCM_PlayStart(void);
extern void CB_ADPCM_PlayEnd(void);
extern void CB_ADPCM_RecStart(void);
extern void CB_ADPCM_RecEnd(void);
extern void CB_ADPCM_InitDac(CH_TypeDef *chx,U16 sample_rate,U8 upsample);
extern void CB_ADPCM_DacCmd(CH_TypeDef *chx,U8 cmd);
extern void CB_ADPCM_ChangeSR(CH_TypeDef *chx,U16 sample_rate);
extern void CB_ADPCM_InitAdc(U16 sample_rate);
extern void CB_ADPCM_AdcCmd(U8 cmd);
extern void CB_ADPCM_WriteDac(U8 size,U16* psrc16);
extern void RateChangeInit(S8 reset, S8 factor, S8 *tempobuf);
extern void RateChange(S16 *inbuffer, S16 *remainsize, S8 *tempobuf);
extern void RateChangeA(S16 *inbuffer, S16 *remainsize, S8 *tempobuf);

extern void SpeedPitchChange_Init(S8 reset, float speed_scale, float pitch_scale, SPEEDPITCHBUF *SpeedPitchBuf);
extern S16 SpeedPitchChange(S16 *inbuffer_A, S32 *InSamplesRemain, SPEEDPITCHBUF *SpeedPitchBuf);

extern void DinosaurSound_Init(DINOSAURCTL *dinosaurCtl, S8 *dinosaurRam);
extern S16 DinosaurSound(S16 *inbuffer, S32 *InSamplesRemain, S8 *dinosaurRam);

extern void DigitalGainInit(S16 *max);
extern void DigitalGainProcess(S16 *in,U16 samples,S16 *max);

#if defined(_ADPCM_RECORD_PLAY_USE_DIGITAL_GAIN)
extern void RTDigiAmpProcess(S16 *in, S16 *buf, S16 samples, short *RTAgcBuf);
extern void RTDigiAmpInit(const short *RT_AGC_Table,const  short *RT_AGC_Table_1, S16 *tempbuf, short *RT_AGCBuf);
extern void RTVadInit(const U16* RTVadTable, short* RTVadBuf);
#endif

//------------------------------------------------------------------//
// Allocate ADPCM memory
// Parameter: 
//          func: Choose Allocate function
// return value:
//          NONE
//------------------------------------------------------------------// 
#if _USE_HEAP_RAM
static void ADPCM_AllocateMemory(U16 func)
{
    if(func&MEMORY_ALLOCATE_PLAY)    //EncDataBuf & AudioBuf & SpeedCtlBuf together
    {
#if !(_ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC || _ADPCM_DARTH_SOUND || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR ||(_ADPCM_DECODE_FLOW_TYPE==TYPE1))
        if(EncDataBuf==NULL)   EncDataBuf = MemAlloc(ADPCM_DATA_BUF_SIZE);
#elif (_ADPCM_SPEED_CHANGE&&_ADPCM_PITCH_CHANGE) || _ADPCM_ANIMAL_ROAR 
		if(SpeedCtlBuf==NULL)   SpeedCtlBuf = MemAlloc((SPEED_CTL_FRAME_SIZE+130)<<1);
        if(AudioBuf==NULL)
            AudioBuf = (U16 *)MemAlloc(ADPCM_AUDIO_BUF_SIZE<<1);
#elif  _ADPCM_ROBOT3_SOUND
        if(SpeedCtlBuf==NULL)   SpeedCtlBuf = MemAlloc((SPEED_CTL_FRAME_SIZE+40)<<1);
        if(AudioBuf==NULL)
            AudioBuf = (U16 *)MemAlloc(ADPCM_AUDIO_BUF_SIZE<<1);
#elif _ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_SPEEDPITCH_N
        if(SpeedCtlBuf==NULL)   SpeedCtlBuf = MemAlloc(SPEED_CTL_FRAME_SIZE<<1);
        if(AudioBuf==NULL)
            AudioBuf = (U16 *)MemAlloc(ADPCM_AUDIO_BUF_SIZE<<1);
#elif (_ADPCM_ROBOT2_SOUND||_ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC) && !(_ADPCM_ECHO_SOUND || _ADPCM_ROBOT4_SOUND)
        if(AudioBuf==NULL)
            AudioBuf = (U16 *)MemAlloc(ADPCM_AUDIO_BUF_SIZE<<1); 	
#elif _ADPCM_ECHO_SOUND||_ADPCM_ROBOT2_SOUND||_ADPCM_ROBOT4_SOUND||_ADPCM_REVERB_SOUND||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)
        if(AudioBuf==NULL)
            AudioBuf = (U16 *)MemAlloc(ADPCM_AUDIO_BUF_SIZE<<1); 
		if(tmp_AudioBuf==NULL)
			tmp_AudioBuf = (U16 *)MemAlloc(ADPCM_AUDIO_BUF_SIZE01<<1); 
#endif 

#if PR_READ
if(EncDataBuf==NULL)   EncDataBuf = MemAlloc(ADPCM_DATA_BUF_SIZE);
#endif

    }	   
    //else
        //dprintf("Allocate twice. func(%x)...\r\n",func);
#if _ADPCM_RECORD
    if(func&MEMORY_ALLOCATE_RECORD)   
    {
        if(AudioBuf==NULL)
            AudioBuf = (U16 *)MemAlloc(ADPCM_AUDIO_BUF_SIZE<<1);
        if(EncDataBuf==NULL)
            EncDataBuf = MemAlloc(ADPCM_DATA_BUF_SIZE);
        if(EncBuf==NULL)
        	EncBuf = MemAlloc(ADPCM_ENC_ALL_SIZE);
#if _ADPCM_RECORD_ERASING
        if(EraseRecordBuf==NULL)
			EraseRecordBuf = (U8 *)MemAlloc(ADPCM_ERASE_RECORD_SIZE);		
#endif	
#if AGC_CTL_ENABLE            
        if(AgcBuf==NULL) 
            AgcBuf = (U16 *)MemAlloc(AGC_FRAME_SIZE<<1);
#endif
    }
    //else
        //dprintf("Allocate twice REC. func(%x)...\r\n",func);
#endif
        if(ModeBuf==NULL)
            ModeBuf = MemAlloc(ADPCM_MODE_SIZE); 
		if(DecBuf==NULL)
            DecBuf = MemAlloc(ADPCM_DEC_PARAM_SIZE); 		
}
//------------------------------------------------------------------//
// Free ADPCM memory
// Parameter: 
//          func: Choose Allocate function
// return value:
//          NONE
//------------------------------------------------------------------// 
static void ADPCM_FreeMemory(U16 func)
{
    if(func&MEMORY_ALLOCATE_PLAY)    
    {
#if !(_ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC || _ADPCM_DARTH_SOUND || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR || (_ADPCM_DECODE_FLOW_TYPE==TYPE1)) || _ADPCM_RECORD   
        if(EncDataBuf!=NULL)
        {
            MemFree(EncDataBuf);
            EncDataBuf=NULL;
        }
#endif

#if PR_READ
        if(EncDataBuf!=NULL)
        {
            MemFree(EncDataBuf);
            EncDataBuf=NULL;
        }
#endif

#if _ADPCM_SPEEDPITCH_N
   		if(SpeedPitchN!=NULL)
        {
            MemFree(SpeedPitchN);
            SpeedPitchN=NULL;
        }		
#endif

#if _ADPCM_SPEED_CHANGE&&_ADPCM_PITCH_CHANGE
        if(SpeedPitchBuf != NULL){
        	MemFree(SpeedPitchBuf);
            SpeedPitchBuf=NULL;
        }
#endif        
        
#if _ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_ROBOT3_SOUND || _ADPCM_DARTH_SOUND || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR
        if(SpeedCtlBuf!=NULL)
        {
            MemFree(SpeedCtlBuf);
            SpeedCtlBuf=NULL;
        }
#endif
#if _ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC || _ADPCM_DARTH_SOUND || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR || (_ADPCM_DECODE_FLOW_TYPE==TYPE1)
        if(AudioBuf!=NULL)
        {
            MemFree(AudioBuf);
            AudioBuf=NULL;
        }
	#if (_ADPCM_ECHO_SOUND || _ADPCM_ROBOT4_SOUND || (_ADPCM_DECODE_FLOW_TYPE==TYPE1)) && !(_ADPCM_SPEED_CHANGE || _ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR)
		if(tmp_AudioBuf!=NULL)
		{
	        MemFree(tmp_AudioBuf);
            tmp_AudioBuf=NULL;		
		}
	#endif
#endif
#if _ADPCM_ROBOT2_SOUND    
        if(Robot01_buf!=NULL)
        {
            MemFree(Robot01_buf);
            Robot01_buf = NULL;
        }
#endif
#if _ADPCM_ROBOT3_SOUND    
        if(Robot02_buf!=NULL)
        {
            MemFree(Robot02_buf);
            Robot02_buf = NULL;
        }
#endif


#if _ADPCM_ROBOT4_SOUND 
        if(Robot4ctl!=NULL)
        {    
            MemFree(Robot4ctl);
            Robot4ctl=NULL;
        }
        
        if(Robot4buf!=NULL)
        {    
            MemFree(Robot4buf);
            Robot4buf=NULL;
        }
        
        if(Robot4FFTctl!=NULL)
        {    
            MemFree(Robot4FFTctl);
            Robot4FFTctl=NULL;
        }
        
#endif

#if _ADPCM_REVERB_SOUND
        if(Reverb_x!=NULL)
        {
            MemFree(Reverb_x);
            Reverb_x = NULL;
            MemFree(Reverb_y);
            Reverb_y = NULL;
        }
#endif
#if _ADPCM_ECHO_SOUND
        if(EchoBuf != NULL){
        	MemFree(EchoBuf);
            EchoBuf=NULL;
        }
#endif

#if _ADPCM_DARTH_SOUND
        if(DarthVaderRam!=NULL)
        {
            //Dprintf("Darth free addr 0x%x\r\n",DarthVaderRam);
            MemFree(DarthVaderRam);
            DarthVaderRam = NULL;
            DarthVaderCtrl = NULL;
        }
#endif        

#if _ADPCM_ANIMAL_ROAR
        if(AnimalRoarTotalRam!=NULL)
        {
            MemFree(AnimalRoarTotalRam);
            AnimalRoarTotalRam = NULL;
            AnimalRoarCtl.initFlag = 0;
        }
#endif

#if defined(_ADPCM_RECORD_PLAY_USE_EQ0)			\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ1)		\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ2)		\
	||defined(_ADPCM_RECORD_PLAY_USE_CUSTOMIZE)	\
	||defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
		if(EqBuf!=NULL)
		{
    		MemFree(EqBuf);
            EqBuf=NULL;
			EQFilterCtl.Buf=NULL;
		}			
#endif
        
        //dprintf("Free Mem\r\n");
    }
    //else
    //{
    //    dprintf("Already Free\r\n");
    //}
#if  _ADPCM_RECORD 
    if(func&MEMORY_ALLOCATE_RECORD)   
    {
        if(AudioBuf!=NULL)
        {
            MemFree(AudioBuf);
            AudioBuf=NULL;
        }
        if(EncDataBuf!=NULL)
        {
            MemFree(EncDataBuf);
            EncDataBuf=NULL;
        }
        if(EncBuf!=NULL)
        {
        	MemFree(EncBuf);
        	EncBuf=NULL;
        }
#if _ADPCM_RECORD_ERASING
        if(EraseRecordBuf!=NULL)
        {
            MemFree(EraseRecordBuf);
            EraseRecordBuf=NULL;
        }
#endif	
#if AGC_CTL_ENABLE        
        if(AgcBuf!=NULL)
        {
            MemFree(AgcBuf);
            AgcBuf=NULL;
        }
#endif        
        //dprintf("Free Mem REC\r\n");
    }
    //else
    //    dprintf("Already Free REC\r\n");
#endif    
	    if(ModeBuf!=NULL)
        {
        	MemFree(ModeBuf);
        	ModeBuf=NULL;
        }
		
		if(DecBuf!=NULL)
        {
        	MemFree(DecBuf);
        	DecBuf=NULL;
        }
}
#endif

//------------------------------------------------------------------//
// Get 1 byte, called by adpcm decode
// Parameter: 
//          NONE
// return value:
//          data
//------------------------------------------------------------------//
S8 ADPCM0GetByte(void){
#if (!PR_READ)&&(_ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR ||(_ADPCM_DECODE_FLOW_TYPE==TYPE1))
    S8 data;
#if _ADPCM_PLAY_SR_FOR_SBC_REC
	if(StorageMode == SPI_MODE)
	{
		while(SPI_CheckBusyFlag());
	}
#endif
    CB_ReadData(StorageMode,&CurAddr,(U8 *)&data,1);
    return data;  
#else
    S8 data;
    
    data=EncDataBuf[BufCtl.rd_offset];
    BufCtl.rd_offset++;
    if(BufCtl.rd_offset==ADPCM_DATA_BUF_SIZE){
        BufCtl.rd_offset=0;
    }

    return data;
#endif 
  
}    

#if PR_READ	
//------------------------------------------------------------------//
// Pre-store Encode Data of SPI Flash to buffer for adpcm decode
// Parameter: 
//          NONE
// return value:
//          data
//------------------------------------------------------------------//  
void  ADPCM_PreStoreEncDataBuf(void)
{
    U16 size;     
    
#if _ADPCM_PLAY_SR_FOR_SBC_REC
	if(StorageMode == SPI_MODE)
	{
		while(SPI_CheckBusyFlag());
	}
#endif
    
    U16 bufctl_rd_offset = BufCtl.rd_offset;
			
	if(bufctl_rd_offset!=BufCtl.wr_offset)
	{
		if(bufctl_rd_offset>BufCtl.wr_offset)
		{
			size=bufctl_rd_offset-BufCtl.wr_offset;   
		}
		else
		{
			size=ADPCM_DATA_BUF_SIZE-BufCtl.wr_offset;  
		}     
		CB_ReadData(StorageMode,&CurAddr,EncDataBuf+BufCtl.wr_offset,size);    
		BufCtl.wr_offset+=size; 
		if(BufCtl.wr_offset==ADPCM_DATA_BUF_SIZE)
		{
			BufCtl.wr_offset=0;                
		}  
	}
}
#endif	 



//------------------------------------------------------------------//
// Dac ISR
// Parameter: 
//          size: fifo fill size 
//           buf: fifo buffer
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_DacIsr(U8 size,S16 *buf){
    
    S16 *psrc16,gain;
    U8 i,len,size2;
#if _ADPCM_PLAY_REPEAT && !(_ADPCM_SPEED_CHANGE || _ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR || (_ADPCM_DECODE_FLOW_TYPE==TYPE1))
	U8 fifoSize = size;
#endif


	if(AudioBufCtl.shadow_state==STATUS_PLAYING){
#if _ADPCM_QIO_MODULE      
		QIO_Resume(QIO_USE_ADPCM_CH1);
#endif//_ADPCM_QIO_MODULE
#if _ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND  || _ADPCM_ECHO_SOUND  || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR ||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)
		S16 temp;
        temp=BufCtl.size-BufCtl.offset;
        if(size>temp){
            size=temp;
        }   
#if _ADPCM_QIO_MODULE
		U16 _QioFrameFlag;
		for(i=0;i<size;i++){
			QIO_FrameFlag_FIFO_BitGet(QIO_USE_ADPCM_CH1,(U8*)&_QioFrameFlag);
			QIO_DacIsr(QIO_USE_ADPCM_CH1, (U8)_QioFrameFlag);
		}
#endif//_ADPCM_QIO_MODULE
#else
	#if _ADPCM_PLAY_REPEAT
		if((AudioBufCtl.fifo_count+size)>AudioBufCtl.samples){
			size=AudioBufCtl.samples-AudioBufCtl.fifo_count;
		}
	#endif	
		U16 _QioFrameFlag;
        for(i=0;i<size;i++){
        #if _ADPCM_SW_UPSAMPLE
            if(swUpsampleCtrl.cmd)
            {
                swUpsampleCtrl.upsampleIndex++;
                if(swUpsampleCtrl.upsampleIndex%swUpsampleCtrl.upsampleFactor)
                {
                    buf[i]=((swUpsampleCtrl.dataBuf[1]-swUpsampleCtrl.dataBuf[0])*swUpsampleCtrl.upsampleIndex)/swUpsampleCtrl.upsampleFactor+swUpsampleCtrl.dataBuf[0];
                }
                else
                {
                    swUpsampleCtrl.dataBuf[0]=swUpsampleCtrl.dataBuf[1];
                    swUpsampleCtrl.dataBuf[1]=ADPCMDecode(&_QioFrameFlag, 0);
                    buf[i]=swUpsampleCtrl.dataBuf[0];
                    swUpsampleCtrl.upsampleIndex=0;
                }
            }
            else
        #endif
            buf[i]=ADPCMDecode(&_QioFrameFlag, 0);
#if _ADPCM_QIO_MODULE
			QIO_DacIsr(QIO_USE_ADPCM_CH1, (U8)_QioFrameFlag);
#endif//_ADPCM_QIO_MODULE
#if	_ADPCM_RECORD
			if(g_playrecord)
			{
#if defined(_ADPCM_RECORD_PLAY_USE_EQ0)		\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ1)	\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ2)	\
	||defined(_ADPCM_RECORD_PLAY_USE_CUSTOMIZE)
				buf[i] = EQProcess(buf[i],&EQFilterCtl);
#endif

#if defined(_ADPCM_RECORD_PLAY_USE_DRC)
				DRC(&drcFixedCtl,(S16*) &buf[i], DRC_buf);
#elif defined(_ADPCM_RECORD_PLAY_USE_DIGITAL_GAIN)
				RTDigiAmpProcess((S16*) &buf[i], ADPCMDigiAmpBuf, 1, ADPCMDigiAmpCTLBuf);  
#endif
			}
			else
			{
#if defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
				buf[i] = EQProcess(buf[i],&EQFilterCtl);
#endif
			}
#else
#if defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
			buf[i] = EQProcess(buf[i],&EQFilterCtl);
#endif
#endif

#if _AUDIO_VOLUME_CHANGE_SMOOTH
            buf[i]=((S16)buf[i]*AUDIO_GetVolumeSmoothINT(&VolCtrl))>>Q15_SHIFT;
#else            
            buf[i]=((S16)buf[i]*AudioVol)>>Q15_SHIFT;
#endif

        }    

#if _ADPCM_ROBOT1_SOUND  
        if(RobotCtl.enable){
            if(RobotCtl.change){
                RoboticVoiceInit(AudioBufCtl.sample_rate, CarrierTbl[RobotCtl.index], size);
                //dprintf("RobotCtl:%d\r\n",RobotCtl.carrier);
                RobotCtl.change=0;
            }    
            RoboticVoiceProcess((short *)buf,(short *)buf);
        }
#endif                    
#endif        
//        //dprintf("size,%d\r\n",size);
    #if _ADPCM_SW_UPSAMPLE
        if(swUpsampleCtrl.cmd)
        {
            if(swUpsampleCtrl.upsampleIndex==0)
            {
                AudioBufCtl.fifo_count+=size;  
            }
            // else
        }
        else
    #endif
        AudioBufCtl.fifo_count+=size;   

#if SMOOTH_ENABLE      
        //smooth process
        if(SmoothCtl.smooth!=SMOOTH_NONE){  
            size2= size;
#if _ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC  || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR ||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)
            psrc16=(S16 *)(BufCtl.buf+BufCtl.offset);
#else
            psrc16=buf;
#endif        
            while(size2){
                if(SmoothCtl.state==SMOOTH_STEP){
                    if(SmoothCtl.smooth==SMOOTH_DOWN){
                        for(i=0;i<size2;i++){
                            *psrc16++=0;
                        }       
#if _ADPCM_QIO_MODULE
						if(AudioBufCtl.state == STATUS_STOP)
						{
							QIO_DelayStop(QIO_USE_ADPCM_CH1,ENABLE);
						}
						else
						{
							QIO_Pause(QIO_USE_ADPCM_CH1);
						}
#endif//_ADPCM_QIO_MODULE
                        AudioBufCtl.shadow_state=AudioBufCtl.state;
                        if(AudioBufCtl.state==STATUS_STOP){
                            PlayEndFlag = BIT_SET;
                        }  
                    }
                    SmoothCtl.smooth=SMOOTH_NONE;
                    break;            
                }
                if(SmoothCtl.smooth==SMOOTH_DOWN){
                    gain=MAX_SMOOTH_GAIN-(SmoothCtl.state+1)*(MAX_SMOOTH_GAIN/SMOOTH_STEP);
                }else{
                    gain=(SmoothCtl.state+1)*(MAX_SMOOTH_GAIN/SMOOTH_STEP);
                }    
                len=SmoothCtl.step_sample-SmoothCtl.count;
            
                if(len>size2){
                    len=size2;
                }    
                 
                for(i=0;i<len;i++){
                    *psrc16=((S32)(*psrc16)*gain)>>15;
                    psrc16++;
                }         
                SmoothCtl.count+=len;
                size2-=len;
                if(SmoothCtl.count==SmoothCtl.step_sample){
                    SmoothCtl.count=0;
                    SmoothCtl.state++;   
                    //dprintf("s:%d,%d\r\n",SmoothCtl.state,gain);
                }          
            }  
        }
#endif  

#if _ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC  || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR || (_ADPCM_DECODE_FLOW_TYPE==TYPE1)
        psrc16=(S16 *)(BufCtl.buf+BufCtl.offset);
        
        CB_ADPCM_WriteDac(size,(U16 *)psrc16);
        
        BufCtl.offset+=size;

        if(BufCtl.offset==BufCtl.size){
            BufCtl.offset=0;    
        }       

#else        
        CB_ADPCM_WriteDac(size,(U16 *)buf);
#endif

#if _ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND ||_ADPCM_ROBOT3_SOUND || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR
        if(AudioBufCtl.fifo_count>=AudioBufCtl.fifo_count2){
#if _ADPCM_QIO_MODULE
			QIO_DelayStop(QIO_USE_ADPCM_CH1,DISABLE);
#if _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_ROBOT4_SOUND||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)
			QIO_FrameFlag_FIFO_RstGet(QIO_USE_ADPCM_CH1);
#endif
#endif//_ADPCM_QIO_MODULE
            AudioBufCtl.state=STATUS_STOP;
            AudioBufCtl.shadow_state=AudioBufCtl.state;
            PlayEndFlag = BIT_SET;
            //dprintf("stop c,%d,%d\r\n",AudioBufCtl.fifo_count,AudioBufCtl.fifo_count2);
        }
#else        
	
	#if _ADPCM_ECHO_SOUND
		if(EchoCtrl.Cmd)
		{
			if(AudioBufCtl.fifo_count>=AudioBufCtl.samples2){
#if _ADPCM_QIO_MODULE
			QIO_DelayStop(QIO_USE_ADPCM_CH1,DISABLE);
#if _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)
			QIO_FrameFlag_FIFO_RstGet(QIO_USE_ADPCM_CH1);
#endif
#endif//_ADPCM_QIO_MODULE
				AudioBufCtl.state=STATUS_STOP;
				AudioBufCtl.shadow_state=AudioBufCtl.state;
				PlayEndFlag = BIT_SET;
				//dprintf("stop b,%d,%d\r\n",AudioBufCtl.fifo_count,AudioBufCtl.samples2);
			}
		}
		else
	#endif
		{
			if(AudioBufCtl.fifo_count>=AudioBufCtl.samples){
			#if _ADPCM_QIO_MODULE && !_ADPCM_PLAY_REPEAT
				QIO_DelayStop(QIO_USE_ADPCM_CH1,DISABLE);
			#if _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND ||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)
				QIO_FrameFlag_FIFO_RstGet(QIO_USE_ADPCM_CH1);
			#endif
			#endif//_ADPCM_QIO_MODULE && !_ADPCM_PLAY_REPEAT
				AudioBufCtl.state=STATUS_STOP;
				AudioBufCtl.shadow_state=AudioBufCtl.state;
				PlayEndFlag = BIT_SET;
				//dprintf("stop a,%d,%d\r\n",AudioBufCtl.fifo_count,AudioBufCtl.samples);
			#if _ADPCM_PLAY_REPEAT
				if (ADPCM_PlayRepeatFlag) {
					#if (_ADPCM_DECODE_FLOW_TYPE==TYPE0)
					ADPCMDecInit(DecBuf, ADPCM0GetByte, 0);
					U8 i;
					U16 tempbuf[fifoSize];
					for (i = 0; i < fifoSize; i++) {
						tempbuf[i] = 0;
						if (i >= size) {
							U16 _QioFrameFlag2;
							tempbuf[i]=ADPCMDecode(&_QioFrameFlag2, 0);
						#if _ADPCM_QIO_MODULE
							QIO_DacIsr(QIO_USE_ADPCM_CH1, (U8)_QioFrameFlag2);
						#endif//_ADPCM_QIO_MODULE
						#if _AUDIO_VOLUME_CHANGE_SMOOTH
							tempbuf[i]=((S16)tempbuf[i]*AUDIO_GetVolumeSmoothINT(&VolCtrl))>>Q15_SHIFT;
						#else
							tempbuf[i]=((S16)tempbuf[i]*AudioVol)>>Q15_SHIFT;
						#endif
						} 
					}
					CB_ADPCM_WriteDac(fifoSize, tempbuf);	
					AudioBufCtl.fifo_count = (fifoSize - size);
					#endif
					
					AudioBufCtl.state = STATUS_PLAYING;
					AudioBufCtl.shadow_state = AudioBufCtl.state;
				}
			#if _ADPCM_QIO_MODULE&&(_ADPCM_DECODE_FLOW_TYPE==TYPE0)
				else {
					QIO_DelayStop(QIO_USE_ADPCM_CH1,DISABLE);
				#if _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND ||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)
					QIO_FrameFlag_FIFO_RstGet(QIO_USE_ADPCM_CH1);
				#endif
				}
			#endif//_ADPCM_QIO_MODULE
			#endif
			}			
		}	
#endif        
    }else{
//        for(i=0;i<size;i++){
//            *pdest32=0;           
//        }
        CB_ADPCM_WriteDac(size,(U16 *)0);
    }                 

}    
#if SMOOTH_ENABLE
//------------------------------------------------------------------//
// Smooth up or down
// Parameter: 
//          ch: audio channel
//          smooth: SMOOTH_UP,SMOOTH_DOWN,SMOOTH_NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
static void SmoothFunc(U8 smooth){
	SmoothCtl.count=0;  
	SmoothCtl.state=0;  
	
    if(smooth==SMOOTH_UP){
	    SmoothCtl.smooth=SMOOTH_UP;
        SmoothCtl.step_sample=AudioBufCtl.sample_rate*SMOOTH_UP_TIME/1000/SMOOTH_STEP;
    }else if(smooth==SMOOTH_DOWN){
	    SmoothCtl.smooth=SMOOTH_DOWN;
	    SmoothCtl.step_sample=AudioBufCtl.sample_rate*SMOOTH_DOWN_TIME/1000/SMOOTH_STEP;
    }else{
        SmoothCtl.smooth=SMOOTH_NONE;
    }
}     

#if _ADPCM_RECORD
static void PlayEndCheck(void)
{
	if(g_playrecord && (playEndSmoothDownFlag==0))
    {
    #if _ADPCM_SPEED_CHANGE || _ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND ||_ADPCM_ROBOT3_SOUND || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR
        if(fifo_count2_lastFlag && (AudioBufCtl.fifo_count>=(AudioBufCtl.fifo_count2-(AudioBufCtl.sample_rate*SMOOTH_DOWN_TIME/1000/SMOOTH_STEP)*SMOOTH_STEP))) //==SMOOTH_DOWN_TIME or !=SMOOTH_DOWN_TIME
        {
            playEndSmoothDownFlag = 1;
            SmoothFunc(SMOOTH_DOWN);
        }
    #else
	#if _ADPCM_ECHO_SOUND
		if(EchoCtrl.Cmd)
		{
			if(AudioBufCtl.fifo_count>=(AudioBufCtl.samples2-(AudioBufCtl.sample_rate*SMOOTH_DOWN_TIME/1000/SMOOTH_STEP)*SMOOTH_STEP))
            {
                playEndSmoothDownFlag = 1;
                SmoothFunc(SMOOTH_DOWN);
			}
		}
		else
	#endif
		{
			if(AudioBufCtl.fifo_count>=(AudioBufCtl.samples-(AudioBufCtl.sample_rate*SMOOTH_DOWN_TIME/1000/SMOOTH_STEP)*SMOOTH_STEP))
            {
                playEndSmoothDownFlag = 1;
                SmoothFunc(SMOOTH_DOWN);
            }
        }
    #endif
    }
}
#endif
#endif

#if defined(_ADPCM_RECORD_PLAY_USE_EQ0)		\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ1)	\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ2)	\
	||defined(_ADPCM_RECORD_PLAY_USE_CUSTOMIZE)
//------------------------------------------------------------------//
// Init EQ parameter when audio play
// Parameter: 
//          NONE
// return value:
//          EXIT_FAILURE, EXIT_SUCCESS
//------------------------------------------------------------------//   
static U8 initRecPlayEQParameter()
{
	if(ADPCM_GetStatus()!=STATUS_STOP)return EXIT_FAILURE;//error
#if _ADPCM_RECORD
	if(!g_playrecord)return EXIT_FAILURE;//error
#endif
#if _USE_HEAP_RAM
	if(EqBuf==NULL)
	{
		EqBuf = (S32 *) MemAlloc(ADPCM_EQ_BUF_SIZE*sizeof(S32));//Free at ADPCM_FreeMemory()
	}
#endif	
	memset((void *)EqBuf, 0, ADPCM_EQ_BUF_SIZE*sizeof(S32));
	EQFilterCtl.SampleRate = AudioBufCtl.sample_rate;
	EQFilterCtl.Buf = EqBuf;
#if	defined(_ADPCM_RECORD_PLAY_USE_EQ0)
	EQFilterCtl.EQGroupSelect=TYPE0;
#elif	defined(_ADPCM_RECORD_PLAY_USE_EQ1)
	EQFilterCtl.EQGroupSelect=TYPE1;
#elif	defined(_ADPCM_RECORD_PLAY_USE_EQ2)
	EQFilterCtl.EQGroupSelect=TYPE2;
#elif	defined(_ADPCM_RECORD_PLAY_USE_CUSTOMIZE)//customize EQ
	EQFilterCtl.EQGroupSelect=CUSTOMIZE;
#endif

#if defined(_ADPCM_RECORD_PLAY_USE_EQ0)		\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ1)	\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ2)
#if	_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND
    EQFilterCtl.PitchIndex = 0;
#if _ADPCM_PITCH_CHANGE
    if(SpeedCtl.pitch_ratio_index-RATIO_INIT_INDEX)
    {
        EQFilterCtl.PitchIndex = SpeedCtl.pitch_ratio_index-RATIO_INIT_INDEX;
    }
#endif
#if _ADPCM_DARTH_SOUND
    if(SpeedCtl.darth_ratio_index-RATIO_INIT_INDEX)
    {
        EQFilterCtl.PitchIndex = SpeedCtl.darth_ratio_index-RATIO_INIT_INDEX;
    }
#endif

#else
	EQFilterCtl.PitchIndex=0;
#endif
#elif	defined(_ADPCM_RECORD_PLAY_USE_CUSTOMIZE)//customize EQ
	EQFilterCtl.PitchIndex=0;//fix 0
#endif

#if defined(_ADPCM_RECORD_PLAY_USE_EQ0)		\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ1)	\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ2)
	EQFilterCtl.FilterPara1=NULL;
#elif	defined(_ADPCM_RECORD_PLAY_USE_CUSTOMIZE)//customize EQ
	if(u8EQFilterIndex >= FILTER_NUMBER){
		EQFilterCtl.FilterPara1 = _AllPassFilterParaTab;//all pass parameter
	} else {
		EQFilterCtl.FilterPara1 = (S16*)FilterList[u8EQFilterIndex];//UserDefine
	}
#endif
	
	EQFilterCtl.FilterPara2=NULL;
	EQFilterCtl.Gain1=0;
	EQFilterCtl.Gain2=0;
	return (EQInit(&EQFilterCtl)==1)?EXIT_SUCCESS:EXIT_FAILURE;//(EQInit)-1:failure, 1:success
}
#endif

#if defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
//------------------------------------------------------------------//
// Init EQ parameter when audio play
// Parameter: 
//          NONE
// return value:
//          EXIT_FAILURE, EXIT_SUCCESS
//------------------------------------------------------------------//   
static U8 initPlayBackEQParameter()
{
	if(ADPCM_GetStatus()!=STATUS_STOP)return EXIT_FAILURE;//error
#if _ADPCM_RECORD
	if(g_playrecord)return EXIT_FAILURE;//error
#endif
#if _USE_HEAP_RAM
	if(EqBuf==NULL)
	{
		EqBuf = (S32 *) MemAlloc(ADPCM_EQ_BUF_SIZE*sizeof(S32));//Free at ADPCM_FreeMemory()
	}
#endif	
	memset((void *)EqBuf, 0, ADPCM_EQ_BUF_SIZE*sizeof(S32));
	EQFilterCtl.SampleRate = AudioBufCtl.sample_rate;
	EQFilterCtl.Buf = EqBuf;
	
#if	defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)//customize EQ
	EQFilterCtl.EQGroupSelect=CUSTOMIZE;
#endif

#if	defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)//customize EQ
	EQFilterCtl.PitchIndex=0;//fix 0
#endif
#if	defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)//customize EQ
	if(u8EQFilterIndex >= FILTER_NUMBER){
		EQFilterCtl.FilterPara1 = _AllPassFilterParaTab;//all pass parameter
	} else {
		EQFilterCtl.FilterPara1 = (S16*)FilterList[u8EQFilterIndex];//UserDefine
	}
#endif
	
	EQFilterCtl.FilterPara2=NULL;
	EQFilterCtl.Gain1=0;
	EQFilterCtl.Gain2=0;
	return (EQInit(&EQFilterCtl)==1)?EXIT_SUCCESS:EXIT_FAILURE;//(EQInit)-1:failure, 1:success
}
#endif
#if _ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR
//------------------------------------------------------------------//
// Decode one audio frame
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
static void DecodeFrame(void)
{

    U16 *audio_buf,i;
    U32 speed_ctl_size; 
    U16 size;
    U16 ctrl_length;
	U16 _QioFrameFlag;
#if (_ADPCM_PITCH_CHANGE&&_ADPCM_SPEED_CHANGE) || _ADPCM_ANIMAL_ROAR
    S32 out_size=0;
	U8	i_offset=0;
	U8	size_offset=1;
#endif
	
#if _ADPCM_RECORD
    U8  last_point_flag=0;
#if SMOOTH_ENABLE
    if (fifo_count2_lastFlag == 1) {
        return; //bypass DecodeFrame
    }
#endif
#endif
    
    
#if _ADPCM_ROBOT3_SOUND
    if(Robot02Ctl.enable)   ctrl_length = SPEED_CTL_FRAME_SIZE + 40;
#if _ADPCM_SPEED_CHANGE&&_ADPCM_PITCH_CHANGE
    else                    ctrl_length = SPEED_CTL_FRAME_SIZE + 130;
#else
    else                    ctrl_length = SPEED_CTL_FRAME_SIZE;
#endif
#else
#if (_ADPCM_SPEED_CHANGE&&_ADPCM_PITCH_CHANGE) || _ADPCM_ANIMAL_ROAR
	ctrl_length = SPEED_CTL_FRAME_SIZE + 130;
#else
    ctrl_length = SPEED_CTL_FRAME_SIZE;
#endif
#endif
    
    while(SpeedCtl.index<ctrl_length){
        
        if(AudioBufCtl.sample_count<AudioBufCtl.samples){
            audio_buf=SpeedCtlBuf+SpeedCtl.index;
            
            size=ctrl_length-SpeedCtl.index;
            
            if((size+AudioBufCtl.sample_count)>AudioBufCtl.samples){
                size=AudioBufCtl.samples-AudioBufCtl.sample_count;
            }    
            for(i=0;i<size;i++){
                audio_buf[i]=ADPCMDecode(&_QioFrameFlag, 0);    
#if	_ADPCM_QIO_MODULE && _ADPCM_ROBOT3_SOUND
				QIO_FrameFlag_FIFO_BitPut(QIO_USE_ADPCM_CH1,(U8*)&_QioFrameFlag);
#endif//_ADPCM_QIO_MODULE && _ADPCM_ROBOT3_SOUND
            }   
       #if PR_READ	     
            ADPCM_PreStoreEncDataBuf();
       #endif     
            SpeedCtl.index+=size;
            AudioBufCtl.sample_count+=size;
#if _ADPCM_RECORD
            //AudioBufCtl.fifo_count2=AudioBufCtl.process_count+ADPCM_AUDIO_OUT_SIZE;
            AudioBufCtl.fifo_count2=AudioBufCtl.fifo_count+(ADPCM_AUDIO_BUF_SIZE*2);

#if !_ADPCM_ECHO_SOUND  //ECHO SOUND does not handle last point of record voice
            if( g_playrecord && (AudioBufCtl.sample_count>=AudioBufCtl.samples) )
            {
                last_point_flag = 1;
            }
#endif

#else
            AudioBufCtl.fifo_count2=AudioBufCtl.fifo_count+(ADPCM_AUDIO_BUF_SIZE*2);
#endif        
#if _ADPCM_ECHO_SOUND
			if(EchoCtrl.Cmd)
			{
				//AudioBufCtl.fifo_count2 += ( (320*_ADPCM_RECORD_SAMPLE_RATE)+32-ADPCM_AUDIO_OUT_SIZE); //delay 320ms 
#if _ADPCM_ECHO_EFFECT==DELAY_1200ms
				AudioBufCtl.fifo_count2 += ( (1200*_ADPCM_RECORD_SAMPLE_RATE/1000)+32); //delay 1200ms
#elif _ADPCM_ECHO_EFFECT==DELAY_800ms
				AudioBufCtl.fifo_count2 += ( (800*_ADPCM_RECORD_SAMPLE_RATE/1000)+32); //delay 800ms
#elif _ADPCM_ECHO_EFFECT==DELAY_320ms	
				AudioBufCtl.fifo_count2 += ( (320*_ADPCM_RECORD_SAMPLE_RATE/1000)+32); //delay 320ms
#else//user define
				AudioBufCtl.fifo_count2 += ( (ADPCM_ECHO_UD_TOTAL_DELAY_TIME*_ADPCM_RECORD_SAMPLE_RATE/1000)+32); //delay _ADPCM_ECHO_UD_DELAY_TIME
#endif            
            }
#endif    
        }else{
		#if _ADPCM_PLAY_REPEAT
			if (ADPCM_PlayRepeatFlag) {
				CurAddr = StartAddr+HEADER_LEN;
				ADPCMDecInit(DecBuf, ADPCM0GetByte, 0);
				AudioBufCtl.sample_count = 0;

				AudioBufCtl.process_count -= AudioBufCtl.fifo_count;
				AudioBufCtl.fifo_count2 = ~0;
				AudioBufCtl.fifo_count = 0;
				AudioBufCtl.fifo_count_tmp = AudioBufCtl.process_count;
				#if PR_READ	
				CB_ReadData(StorageMode,&CurAddr,EncDataBuf,ADPCM_DATA_BUF_SIZE);
    			BufCtl.rd_offset=0;
    			BufCtl.wr_offset=0;   
				#endif
			} else {
				audio_buf=SpeedCtlBuf+SpeedCtl.index;
				
				size=ctrl_length-SpeedCtl.index;
				
				memset((void *)audio_buf, 0, size*2);
				
				SpeedCtl.index+=size;			
			}
		#else
            audio_buf=SpeedCtlBuf+SpeedCtl.index;
            
            size=ctrl_length-SpeedCtl.index;
            
            memset((void *)audio_buf, 0, size*2);
            
            SpeedCtl.index+=size;
		#endif 
        }
#if	_ADPCM_QIO_MODULE && _ADPCM_ROBOT3_SOUND
		if(AudioBufCtl.samples == AudioBufCtl.sample_count)
		{
			QIO_FrameFlag_FIFO_RstPut(QIO_USE_ADPCM_CH1);
		}
#endif//_ADPCM_QIO_MODULE && _ADPCM_ROBOT3_SOUND		
    }    

#if _ADPCM_ROBOT3_SOUND  
    
    if(Robot02Ctl.enable)
    {
        audio_buf=SpeedCtlBuf+SpeedCtl.rb_ptr;
#if _ROBOT_SOUND_COMPRESS
        ROBOT_EFFECT_COMPRESS_02((S16 *)audio_buf);
#else
        ROBOT_EFFECT_02((S16 *)audio_buf);
#endif
        U16 j,out_cnt;
		U32 access_ptr;
        SpeedCtl.rb_ptr += 320;
        out_cnt = (SpeedCtl.rb_ptr)/ADPCM_AUDIO_OUT_SIZE; //how many times output audio_buf
        speed_ctl_size = out_cnt*ADPCM_AUDIO_OUT_SIZE; //residual points that will process next time
        access_ptr = 0;
        for(j=0;j<out_cnt;j++)
        {
            audio_buf=AudioBuf+(ADPCM_AUDIO_OUT_SIZE*AudioBufCtl.index);
            for(i=0;i<ADPCM_AUDIO_OUT_SIZE;i++)
            {
#if	_ADPCM_RECORD
				if(g_playrecord)
				{
#if defined(_ADPCM_RECORD_PLAY_USE_EQ0)		\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ1)	\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ2)	\
	||defined(_ADPCM_RECORD_PLAY_USE_CUSTOMIZE)
					audio_buf[i] = EQProcess(SpeedCtlBuf[access_ptr],&EQFilterCtl);	
#endif

#if defined(_ADPCM_RECORD_PLAY_USE_DRC)
					DRC(&drcFixedCtl,(S16*) &SpeedCtlBuf[access_ptr], DRC_buf);
#elif defined(_ADPCM_RECORD_PLAY_USE_DIGITAL_GAIN)
					RTDigiAmpProcess((S16*) &SpeedCtlBuf[access_ptr], ADPCMDigiAmpBuf, 1, ADPCMDigiAmpCTLBuf);  
#endif
				}
				else
				{
#if defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
					audio_buf[i] = EQProcess(SpeedCtlBuf[access_ptr],&EQFilterCtl);
#endif
				}
#else
#if defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
				audio_buf[i] = EQProcess(SpeedCtlBuf[access_ptr],&EQFilterCtl);
#endif
#endif

#if _AUDIO_VOLUME_CHANGE_SMOOTH
                audio_buf[i] = ((S16)SpeedCtlBuf[access_ptr] * AUDIO_GetVolumeSmooth(&VolCtrl))>>Q15_SHIFT;
#else                
                audio_buf[i] = ((S16)SpeedCtlBuf[access_ptr] * AudioVol)>>Q15_SHIFT;
#endif
                if(speed_ctl_size<ctrl_length)
                {
                    SpeedCtlBuf[access_ptr] = SpeedCtlBuf[speed_ctl_size];
                    speed_ctl_size++;
                }
                access_ptr++;
            }
            
            AudioBufCtl.index++;
            if(AudioBufCtl.index==ADPCM_AUDIO_OUT_COUNT){
                AudioBufCtl.index=0;    
            }    
            AudioBufCtl.process_count+=ADPCM_AUDIO_OUT_SIZE;
            SpeedCtl.rb_ptr -= ADPCM_AUDIO_OUT_SIZE;
            SpeedCtl.index -=  ADPCM_AUDIO_OUT_SIZE;
        }             
    }
    else
#endif                      
    {
        audio_buf=AudioBuf+(ADPCM_AUDIO_OUT_SIZE*AudioBufCtl.index);
        
        AudioBufCtl.index++;
        if(AudioBufCtl.index==ADPCM_AUDIO_OUT_COUNT){
            AudioBufCtl.index=0;    
        }    
        
        if(	1
#if _ADPCM_SPEED_CHANGE
			&& SpeedCtl.speed_ratio_index==RATIO_INIT_INDEX
#endif
#if _ADPCM_PITCH_CHANGE
			&& SpeedCtl.pitch_ratio_index==RATIO_INIT_INDEX
#endif
#if _ADPCM_DARTH_SOUND
            && DarthCtrl.Cmd==DISABLE
#endif
#if _ADPCM_SPEEDPITCH_N
            && SpeedPitchNFlag==0
#endif
#if _ADPCM_ANIMAL_ROAR
            && AnimalRoarCtl.cmd==DISABLE
#endif
		){
            speed_ctl_size=ADPCM_AUDIO_OUT_SIZE;
            //out_size=ADPCM_AUDIO_OUT_SIZE;
#if _ADPCM_ECHO_SOUND
            // Process exceed 90
            if(EchoCtrl.Cmd)
            {
                while(EchoCtrl.Offset<ADPCM_AUDIO_OUT_SIZE)
                {
	                EchoSound((S16*)&SpeedCtlBuf[EchoCtrl.Offset],NEW_ADPCM_ECHO_DATA_LENGTH);
                    EchoCtrl.Offset += NEW_ADPCM_ECHO_DATA_LENGTH;
                }
                EchoCtrl.Offset -= ADPCM_AUDIO_OUT_SIZE;
            }
            
#endif

#if _ADPCM_ROBOT4_SOUND
 		// Process exceed 90
			if(Robot04Ctl.enable){
				while(Robot04Ctl.offset<ADPCM_AUDIO_OUT_SIZE)
                {	
					ROBOT_EFFECT_04((S16*)&SpeedCtlBuf[Robot04Ctl.offset], Robot4ctl, Robot4buf, Robot4FFTctl);	
					Robot04Ctl.offset+= ADPCM_ROBOT4_DATA_LENGTH;
				}
				Robot04Ctl.offset -= ADPCM_AUDIO_OUT_SIZE;
			}
#endif

            for(i=0;i<ADPCM_AUDIO_OUT_SIZE;i++){
                audio_buf[i]=SpeedCtlBuf[i];
            }   
        }else{   
            speed_ctl_size=ctrl_length;  
#if (_ADPCM_PITCH_CHANGE&&_ADPCM_SPEED_CHANGE) || _ADPCM_ANIMAL_ROAR 
#if _ADPCM_SPEEDPITCH_N   
			if(SpeedPitchNFlag)
			{
				RateChange((short *)SpeedCtlBuf, (short *)&speed_ctl_size, SpeedPitchN); // in,out
	            memcpy(audio_buf,(S16*)(SpeedPitchN + 58),ADPCM_AUDIO_OUT_SIZE*2);
        	}
			else		
#endif 
			{
#if _ADPCM_DARTH_SOUND
	            if(DarthCtrl.Cmd)
	            {
	                out_size = DarthVader((S16 *)audio_buf,(S16 *)SpeedCtlBuf,(S32 *)&speed_ctl_size,DarthVaderRam);
	            }
#if _ADPCM_PITCH_CHANGE||_ADPCM_SPEED_CHANGE
            	else
#endif
#endif
            #if (_ADPCM_PITCH_CHANGE&&_ADPCM_SPEED_CHANGE)
            	out_size = SpeedPitchChange((short *)SpeedCtlBuf, (int *)&speed_ctl_size, SpeedPitchBuf); // in,out 
            #elif _ADPCM_ANIMAL_ROAR
                out_size = DinosaurSound((S16*)SpeedCtlBuf, (S32*)&speed_ctl_size, AnimalRoarTotalRam);
            #endif
            
	            if(out_size!=0 && out_size!=90 && out_size!=180)
	            {
	                //dprintf("Error out_size................ %d\r\n",out_size);
	            }                        
	            if(out_size>ADPCM_AUDIO_OUT_SIZE) //only 2 times or 1 times ADPCM_AUDIO_OUT_SIZE
	            {
	            	size_offset=2;
	            	for(i=0;i<ADPCM_AUDIO_OUT_SIZE;i++)
	            	{       
                    #if (_ADPCM_PITCH_CHANGE&&_ADPCM_SPEED_CHANGE)
						audio_buf[i]=((S16)SpeedPitchBuf->OutBuf[i]);  	
                    #elif _ADPCM_ANIMAL_ROAR
                        audio_buf[i]=*(((S16 *)(AnimalRoarTotalRam + ANIMAL_ROAR_OUT_INDEX)+i));  
                    #endif
	        		}                
	        		AudioBufCtl.process_count+=ADPCM_AUDIO_OUT_SIZE;        	
	        		AudioBufCtl.frame_size=1;
	        
			        
			        audio_buf=AudioBuf+(ADPCM_AUDIO_OUT_SIZE*AudioBufCtl.index);
			        //memcpy(audio_buf,&TS_BUF[AUDIO_OUT_SIZE],AUDIO_OUT_SIZE*2);
			        for(i=0;i<ADPCM_AUDIO_OUT_SIZE;i++)
	            	{
                    #if (_ADPCM_PITCH_CHANGE&&_ADPCM_SPEED_CHANGE)
						audio_buf[i]=((S16)SpeedPitchBuf->OutBuf[i+ADPCM_AUDIO_OUT_SIZE]);
                    #elif _ADPCM_ANIMAL_ROAR
                        audio_buf[i]=*(((S16 *)(AnimalRoarTotalRam + ANIMAL_ROAR_OUT_INDEX)+ADPCM_AUDIO_OUT_SIZE+i));  
                    #endif
	        		}
			        AudioBufCtl.index++;
			        if(AudioBufCtl.index==ADPCM_AUDIO_OUT_COUNT)
			        {
			            AudioBufCtl.index=0;    
			        }
	            }
	            else if(out_size==0)
	            {
	            	AudioBufCtl.frame_size=0;
	            	size_offset=1;
					if(AudioBufCtl.index==0)
	                {
	                    AudioBufCtl.index=(ADPCM_AUDIO_OUT_COUNT-1);    
	                }  
	                else
	                {
	                    AudioBufCtl.index--;
	                }
	                speed_ctl_size=ctrl_length-speed_ctl_size; 
	                SpeedCtl.index-=speed_ctl_size;
	                for(i=0;i<SpeedCtl.index;i++){
	                    SpeedCtlBuf[i]=SpeedCtlBuf[i+speed_ctl_size];
	                }
	                return;
	        	} 
	            else
	            {
	            	AudioBufCtl.frame_size=0;
	            	size_offset=1;
#if _ADPCM_DARTH_SOUND
	                if(DarthCtrl.Cmd==DISABLE)             
#endif                
                #if (_ADPCM_PITCH_CHANGE&&_ADPCM_SPEED_CHANGE)
	            	memcpy(audio_buf,SpeedPitchBuf->OutBuf,ADPCM_AUDIO_OUT_SIZE*2);
                #elif _ADPCM_ANIMAL_ROAR
                    memcpy(audio_buf, (S16 *)(AnimalRoarTotalRam + ANIMAL_ROAR_OUT_INDEX),ADPCM_AUDIO_OUT_SIZE*2);
                #endif
	            }        
        	}

#else// No Speed/Pitch together

#if _ADPCM_SPEEDPITCH_N
			if(SpeedPitchNFlag)
            {
            	RateChange((short *)SpeedCtlBuf, (short *)&speed_ctl_size, SpeedPitchN); // in,out
            	memcpy(audio_buf,(S16*)(SpeedPitchN + 58),ADPCM_AUDIO_OUT_SIZE*2);
            }
            else
#endif   
			{
#if _ADPCM_DARTH_SOUND
	            if(DarthCtrl.Cmd)
	            {
	                DarthVader((S16 *)audio_buf,(S16 *)SpeedCtlBuf,(S32 *)&speed_ctl_size,DarthVaderRam);
	            }     
#if _ADPCM_PITCH_CHANGE||_ADPCM_SPEED_CHANGE
				else
#endif
#endif            
#if _ADPCM_PITCH_CHANGE||_ADPCM_SPEED_CHANGE
            TimeStretchProcess(audio_buf, SpeedCtlBuf, &speed_ctl_size);
#endif
			}
#endif           
            speed_ctl_size=ctrl_length-speed_ctl_size; 
        }
        //dprintf("%d,%d\r\n",speed_ctl_size,size);
        SpeedCtl.index-=speed_ctl_size;
        for(i=0;i<SpeedCtl.index;i++){
            SpeedCtlBuf[i]=SpeedCtlBuf[i+speed_ctl_size];
        }   

        AudioBufCtl.process_count+=ADPCM_AUDIO_OUT_SIZE;
            

#if _ADPCM_ROBOT1_SOUND  
        if(RobotCtl.enable){
            if(RobotCtl.change){
                RoboticVoiceInit(AudioBufCtl.sample_rate, CarrierTbl[RobotCtl.index], ADPCM_AUDIO_OUT_SIZE);
                //dprintf("RobotCtl:%d\r\n",RobotCtl.carrier);
                RobotCtl.change=0;
            }    
            RoboticVoiceProcess((short *)audio_buf,(short *)audio_buf);
        }
#endif    
#if _ADPCM_ROBOT2_SOUND
        if(Robot01Ctl.enable)
        {
#if _ROBOT_SOUND_COMPRESS                    
            ROBOT_EFFECT_COMPRESS_01((S16 *)audio_buf);
#else
            ROBOT_EFFECT_01((S16 *)audio_buf);
#endif
        }
#endif


#if _ADPCM_REVERB_SOUND
        if(ReverbCmd)
        {
            Reverb_Effect((short *)audio_buf,ADPCM_AUDIO_OUT_SIZE);
        }
#endif                             

#if NEW_ECHO
        // Echo buffer 90+90+32. Second 90 is for TS_Buffer.
        // Rearrage Echo buffer
        
        // Go into Echo process. Frame is 32.

        // Rearrange Echo buffer
        
#endif

#if (_ADPCM_PITCH_CHANGE&&_ADPCM_SPEED_CHANGE) || _ADPCM_ANIMAL_ROAR
// Volume flow with _ADPCM_PITCH_CHANGE&&_ADPCM_SPEED_CHANGE 
		if(out_size>ADPCM_AUDIO_OUT_SIZE)
		{
			if(AudioBufCtl.index<=1)
			{
				AudioBufCtl.index=AudioBufCtl.index+ADPCM_AUDIO_OUT_COUNT-2;
			}
			else
			{
				AudioBufCtl.index-=2;
			}
			audio_buf=AudioBuf+(ADPCM_AUDIO_OUT_SIZE*AudioBufCtl.index);
        }
        //volume
        for(i=0;i<ADPCM_AUDIO_OUT_SIZE*size_offset;i++)
        {
        	if((out_size>ADPCM_AUDIO_OUT_SIZE)&&(AudioBufCtl.index==(ADPCM_AUDIO_OUT_COUNT-1))&&(i==ADPCM_AUDIO_OUT_SIZE))
        	{
        		audio_buf=AudioBuf;
        		i_offset=90;
        	}      	
#if	_ADPCM_RECORD
			if(g_playrecord)
			{
#if defined(_ADPCM_RECORD_PLAY_USE_EQ0)		\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ1)	\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ2)	\
	||defined(_ADPCM_RECORD_PLAY_USE_CUSTOMIZE)
				audio_buf[i-i_offset] = EQProcess(audio_buf[i-i_offset],&EQFilterCtl);		
#endif

#if defined(_ADPCM_RECORD_PLAY_USE_DRC)
				DRC(&drcFixedCtl,(S16*) &audio_buf[i-i_offset], DRC_buf);
#elif defined(_ADPCM_RECORD_PLAY_USE_DIGITAL_GAIN)
				RTDigiAmpProcess((S16*) &audio_buf[i-i_offset], ADPCMDigiAmpBuf, 1, ADPCMDigiAmpCTLBuf);  
#endif
			}
			else
			{
#if defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE) 
				audio_buf[i-i_offset] = EQProcess(audio_buf[i-i_offset],&EQFilterCtl);
#endif
			}
#else
#if defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
				audio_buf[i-i_offset] = EQProcess(audio_buf[i-i_offset],&EQFilterCtl);
#endif
#endif


#if _AUDIO_VOLUME_CHANGE_SMOOTH
			audio_buf[i-i_offset]=((S16)audio_buf[i-i_offset]*AUDIO_GetVolumeSmooth(&VolCtrl))>>Q15_SHIFT;
#else 
			audio_buf[i-i_offset]=((S16)audio_buf[i-i_offset]*AudioVol)>>Q15_SHIFT;
#endif

        }
        
        if(out_size>ADPCM_AUDIO_OUT_SIZE)
		{
			if(AudioBufCtl.index<(ADPCM_AUDIO_OUT_COUNT-2))
			{
				AudioBufCtl.index+=2;
			}
			else
			{
				AudioBufCtl.index=AudioBufCtl.index-ADPCM_AUDIO_OUT_COUNT+2;
			}
			size_offset=1;
			if(i_offset==90)
			{
				i_offset=0;
			}
        }

#else 
// Volume flow without _ADPCM_PITCH_CHANGE&&_ADPCM_SPEED_CHANGE
        //volume
        for(i=0;i<ADPCM_AUDIO_OUT_SIZE;i++)
        {     	
#if	_ADPCM_RECORD
			if(g_playrecord)
			{
#if defined(_ADPCM_RECORD_PLAY_USE_EQ0)		\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ1)	\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ2)	\
	||defined(_ADPCM_RECORD_PLAY_USE_CUSTOMIZE)
				audio_buf[i] = EQProcess(audio_buf[i],&EQFilterCtl);	
#endif

#if defined(_ADPCM_RECORD_PLAY_USE_DRC)
				DRC(&drcFixedCtl,(S16*) &audio_buf[i], DRC_buf);
#elif defined(_ADPCM_RECORD_PLAY_USE_DIGITAL_GAIN)
				RTDigiAmpProcess((S16*) &audio_buf[i], ADPCMDigiAmpBuf, 1, ADPCMDigiAmpCTLBuf);  

#endif
			}
			else
			{
#if defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
				audio_buf[i] = EQProcess(audio_buf[i],&EQFilterCtl);
#endif
			}
#else
#if defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
				audio_buf[i] = EQProcess(audio_buf[i],&EQFilterCtl);
#endif
#endif


#if _AUDIO_VOLUME_CHANGE_SMOOTH
            audio_buf[i]=((S16)audio_buf[i]*AUDIO_GetVolumeSmooth(&VolCtrl))>>Q15_SHIFT;
#else 
            audio_buf[i]=((S16)audio_buf[i]*AudioVol)>>Q15_SHIFT;
#endif

        }
#endif// end of volume flow       

        
#if _ADPCM_RECORD      
		if(last_point_flag)
        {
            AudioBufCtl.fifo_count2=AudioBufCtl.process_count;
        #if SMOOTH_ENABLE
            fifo_count2_lastFlag = 1;
        #endif
        }
#endif        
    }
   
}
#elif (_ADPCM_ROBOT2_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC) && !(_ADPCM_ECHO_SOUND || _ADPCM_ROBOT4_SOUND)
//------------------------------------------------------------------//
// Decode one audio frame
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
static void DecodeFrame(void) 
{
    U16 *audio_buf,i;
    //U32 speed_ctl_size; 
    U16 size;
	U16 _QioFrameFlag;

    size = ADPCM_AUDIO_OUT_SIZE;
    if((size+AudioBufCtl.sample_count)>AudioBufCtl.samples)
    {
        size=AudioBufCtl.samples-AudioBufCtl.sample_count;
    } 
    audio_buf=AudioBuf+(ADPCM_AUDIO_OUT_SIZE*AudioBufCtl.index);
    for(i=0;i<size;i++)
    {
        audio_buf[i]=ADPCMDecode(&_QioFrameFlag, 0);   
#if	_ADPCM_QIO_MODULE
#if (_ADPCM_ROBOT2_SOUND || _ADPCM_REVERB_SOUND||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)) && !(_ADPCM_ECHO_SOUND)
		QIO_FrameFlag_FIFO_BitPut(QIO_USE_ADPCM_CH1,(U8*)&_QioFrameFlag);
#endif
#endif//_ADPCM_QIO_MODULE
    } 
    #if PR_READ	
    ADPCM_PreStoreEncDataBuf();
    #endif
    
    AudioBufCtl.sample_count+=size;
    AudioBufCtl.index++;
    if(AudioBufCtl.index==ADPCM_AUDIO_OUT_COUNT){
        AudioBufCtl.index=0;    
    }    
    AudioBufCtl.process_count+=ADPCM_AUDIO_OUT_SIZE;
#if	_ADPCM_QIO_MODULE
#if (_ADPCM_ROBOT2_SOUND || _ADPCM_REVERB_SOUND ||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)) && !(_ADPCM_ECHO_SOUND)
	if(AudioBufCtl.samples == AudioBufCtl.sample_count)
	{
		QIO_FrameFlag_FIFO_RstPut(QIO_USE_ADPCM_CH1);
	}
#endif
#endif//_ADPCM_QIO_MODULE

#if _ADPCM_ROBOT1_SOUND  
        if(RobotCtl.enable){
            if(RobotCtl.change){
                RoboticVoiceInit(AudioBufCtl.sample_rate, CarrierTbl[RobotCtl.index], ADPCM_AUDIO_OUT_SIZE);
                //dprintf("RobotCtl:%d\r\n",RobotCtl.carrier);
                RobotCtl.change=0;
            }    
            RoboticVoiceProcess((short *)audio_buf,(short *)audio_buf);
        }
#endif   

#if _ADPCM_ROBOT2_SOUND
    if(Robot01Ctl.enable)
    {
#if _ROBOT_SOUND_COMPRESS                    
        ROBOT_EFFECT_COMPRESS_01((S16 *)audio_buf);
#else
        ROBOT_EFFECT_01((S16 *)audio_buf);
#endif
    }
#endif

#if _ADPCM_REVERB_SOUND
    if(ReverbCmd)
    {
        Reverb_Effect((short *)audio_buf,ADPCM_AUDIO_OUT_SIZE);
    }
#endif                 

    //volume 
    for(i=0;i<ADPCM_AUDIO_OUT_SIZE;i++)
	{
#if	_ADPCM_RECORD
		if(g_playrecord)
		{
#if defined(_ADPCM_RECORD_PLAY_USE_EQ0)		\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ1)	\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ2)	\
	||defined(_ADPCM_RECORD_PLAY_USE_CUSTOMIZE)
			audio_buf[i] = EQProcess(audio_buf[i],&EQFilterCtl);
#endif

#if defined(_ADPCM_RECORD_PLAY_USE_DRC)
			DRC(&drcFixedCtl,(S16*) &audio_buf[i], DRC_buf);
#elif defined(_ADPCM_RECORD_PLAY_USE_DIGITAL_GAIN)
			RTDigiAmpProcess((S16*) &audio_buf[i], ADPCMDigiAmpBuf, 1, ADPCMDigiAmpCTLBuf);  
#endif
		}
		else
		{
#if defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
			audio_buf[i] = EQProcess(audio_buf[i],&EQFilterCtl);
#endif
		}
#else
#if defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
		audio_buf[i] = EQProcess(audio_buf[i],&EQFilterCtl);
#endif
#endif
#if _AUDIO_VOLUME_CHANGE_SMOOTH
        audio_buf[i]=((S16)audio_buf[i]*AUDIO_GetVolumeSmooth(&VolCtrl))>>Q15_SHIFT;
#else 
        audio_buf[i]=((S16)audio_buf[i]*AudioVol)>>Q15_SHIFT;
#endif
    }  	
}
   
#elif _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND || (_ADPCM_DECODE_FLOW_TYPE==TYPE1)
//------------------------------------------------------------------//
// Decode one audio frame
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
static void DecodeFrame(void)
{
    U16 *audio_buf,i;
	U16 _QioFrameFlag;
    //U32 speed_ctl_size; 

    audio_buf=AudioBuf+(ADPCM_AUDIO_OUT_SIZE*AudioBufCtl.index);
	AudioBufCtl.index++;
    if(AudioBufCtl.index==ADPCM_AUDIO_OUT_COUNT){
        AudioBufCtl.index=0;    
    }    
	
    AudioBufCtl.process_count+=ADPCM_AUDIO_OUT_SIZE;
	

	for(i=0; i<lastAudio; i++)									//old left-shift 
		tmp_AudioBuf[i]=tmp_AudioBuf[ADPCM_AUDIO_OUT_SIZE+i];	

	if(lastAudio<ADPCM_AUDIO_OUT_SIZE) {
			
		playEnd.CntFrame++;
		
		if(playEnd.CntFrame<=playEnd.RefFrame) {
			for(i=lastAudio;i<(96+lastAudio);i++){				//old + new 
				tmp_AudioBuf[i]=ADPCMDecode(&_QioFrameFlag, 0);   
#if	_ADPCM_QIO_MODULE
#if	_ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_ROBOT4_SOUND ||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)
				QIO_FrameFlag_FIFO_BitPut(QIO_USE_ADPCM_CH1,(U8*)&_QioFrameFlag);
#endif
#endif//_ADPCM_QIO_MODULE
			}
		#if PR_READ		
            ADPCM_PreStoreEncDataBuf();
		#endif
		
			if(playEnd.CntFrame==playEnd.RefFrame && playEnd.Last!=0) {	
				for(i=(lastAudio+playEnd.Last) ; i<(96+lastAudio) ; i++)	
					tmp_AudioBuf[i]=0; 
			}	
		}
		else {
			for(i=lastAudio;i<(96+lastAudio);i++) {				
				tmp_AudioBuf[i]=0;   
			}
		}
		lastAudio+=(96-ADPCM_AUDIO_OUT_SIZE);					//a multiple of 6 (final:90)		
	}
	else if(lastAudio==ADPCM_AUDIO_OUT_SIZE)		
		lastAudio=0;

#if	_ADPCM_QIO_MODULE
#if	_ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND ||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)
	if(playEnd.CntFrame==playEnd.RefFrame)
	{
		QIO_FrameFlag_FIFO_RstPut(QIO_USE_ADPCM_CH1);
	}
#endif
#endif//_ADPCM_QIO_MODULE

			#if (_ADPCM_PLAY_REPEAT&&(_ADPCM_DECODE_FLOW_TYPE==TYPE1)&&(!(_ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND))) // Repeat function only for playback(frame base)
			if (ADPCM_PlayRepeatFlag&&(playEnd.CntFrame>playEnd.RefFrame)) {
				CurAddr = StartAddr+HEADER_LEN;
				ADPCMDecInit(DecBuf, ADPCM0GetByte, 0);
				playEnd.CntFrame=0;
				AudioBufCtl.process_count -= AudioBufCtl.fifo_count;  // last process count
				AudioBufCtl.fifo_count = 0;
				#if PR_READ	
				CB_ReadData(StorageMode,&CurAddr,EncDataBuf,ADPCM_DATA_BUF_SIZE);
    			BufCtl.rd_offset=0;
    			BufCtl.wr_offset=0;   
				#endif
			}
			#endif

#if _ADPCM_ECHO_SOUND
	if(EchoCtrl.Cmd) {
		S16 *echo_inbuf;

		if(EchoCtrl.Offset<ADPCM_AUDIO_OUT_SIZE) 
		{				
			for(i=EchoCtrl.Offset ; i<(96+EchoCtrl.Offset) ; i+=NEW_ADPCM_ECHO_DATA_LENGTH)	//3-times frames			
			{	
				echo_inbuf = (S16*)&tmp_AudioBuf[i];
				EchoSound((S16*)echo_inbuf,NEW_ADPCM_ECHO_DATA_LENGTH);
			}
			EchoCtrl.Offset=lastAudio;
		}
		else if (EchoCtrl.Offset==ADPCM_AUDIO_OUT_SIZE)
			EchoCtrl.Offset=0;
	}
#endif
#if _ADPCM_ROBOT4_SOUND
	if(Robot04Ctl.enable){
		if(Robot04Ctl.offset<ADPCM_AUDIO_OUT_SIZE) 
		{				
			for(i=Robot04Ctl.offset ; i<(ADPCM_ROBOT4_DATA_LENGTH+Robot04Ctl.offset) ; i+=ADPCM_ROBOT4_DATA_LENGTH) 
			{	
				ROBOT_EFFECT_04((S16*)&tmp_AudioBuf[i], Robot4ctl, Robot4buf, Robot4FFTctl);
			}
			Robot04Ctl.offset=lastAudio;
		}
		else if (Robot04Ctl.offset==ADPCM_AUDIO_OUT_SIZE)
			Robot04Ctl.offset=0;
	}
#endif

#if _ADPCM_ROBOT1_SOUND  
        if(RobotCtl.enable){
            if(RobotCtl.change){
                RoboticVoiceInit(AudioBufCtl.sample_rate, CarrierTbl[RobotCtl.index], ADPCM_AUDIO_OUT_SIZE);
                //dprintf("RobotCtl:%d\r\n",RobotCtl.carrier);
                RobotCtl.change=0;
            }    
            RoboticVoiceProcess((short *)tmp_AudioBuf,(short *)tmp_AudioBuf);
        }
#endif   	

#if _ADPCM_ROBOT2_SOUND
    if(Robot01Ctl.enable)
    {
#if _ROBOT_SOUND_COMPRESS                    
		ROBOT_EFFECT_COMPRESS_01((S16 *)tmp_AudioBuf);
#else
		ROBOT_EFFECT_01((S16 *)tmp_AudioBuf);
#endif
    }
#endif

#if _ADPCM_REVERB_SOUND
    if(ReverbCmd)
    {
        Reverb_Effect((short *)tmp_AudioBuf,ADPCM_AUDIO_OUT_SIZE);
    }
#endif                 

	for(i=0; i<ADPCM_AUDIO_OUT_SIZE; i++)
		audio_buf[i]=tmp_AudioBuf[i];
	
	//volume 
    for(i=0;i<ADPCM_AUDIO_OUT_SIZE;i++)
	{
#if	_ADPCM_RECORD
		if(g_playrecord)
		{
#if defined(_ADPCM_RECORD_PLAY_USE_EQ0)		\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ1)	\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ2)	\
	||defined(_ADPCM_RECORD_PLAY_USE_CUSTOMIZE)
			audio_buf[i] = EQProcess(audio_buf[i],&EQFilterCtl);
#endif

#if defined(_ADPCM_RECORD_PLAY_USE_DRC)
			DRC(&drcFixedCtl,(S16*) &audio_buf[i], DRC_buf);
#elif defined(_ADPCM_RECORD_PLAY_USE_DIGITAL_GAIN)
			RTDigiAmpProcess((S16*) &audio_buf[i], ADPCMDigiAmpBuf, 1, ADPCMDigiAmpCTLBuf);  
#endif
		}
		else
		{
#if defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
			audio_buf[i] = EQProcess(audio_buf[i],&EQFilterCtl);
#endif
		}
#else
#if defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
		audio_buf[i] = EQProcess(audio_buf[i],&EQFilterCtl);
#endif
#endif

#if _AUDIO_VOLUME_CHANGE_SMOOTH
        audio_buf[i]=((S16)audio_buf[i]*AUDIO_GetVolumeSmooth(&VolCtrl))>>Q15_SHIFT;
#else 
        audio_buf[i]=((S16)audio_buf[i]*AudioVol)>>Q15_SHIFT;
#endif
    }  
}
#endif

#if _ADPCM_RECORD 
//------------------------------------------------------------------//
// Stop record
// Parameter: 
//          None
// return value:
//          None
//------------------------------------------------------------------// 
static void  StopRecord(void)
{	
    CB_ADPCM_AdcCmd(DISABLE);
#if _ADPCM_RECORD_ERASING
    if(RecordErasingCtrl.status == C_RECORD_ERASE_RUN)
    {
		if (StorageMode == SPI_MODE)
			while(SPI_CheckBusyFlag()==1); //Wait busy
	#if _SPI1_MODULE && _SPI1_USE_FLASH
		else if (StorageMode == SPI1_MODE)
			while(SPI1_CheckBusyFlag()==1); //Wait busy
	#endif
		
		RecordErasingCtrl.status = C_RECORD_ERASE_DONE;
        // Save data to Flash From RAM
        // Record write back
        CB_WriteData(StorageMode,&CurAddr,(U8 *)RecordErasingCtrl.record_buf,RecordErasingCtrl.record_buf_index);
        // Playback...
        REprintf("Stop Record restore Flash CurAddr 0x%x\r\n",CurAddr);
    }
    RecordErasingCtrl.status = C_RECORD_ERASE_IDLE;
#endif    
#if _ADPCM_RECORD_ERASING && _ADPCM_RECORD_SOUND_TRIG && SOUND_TRIG_RECOVER
#define bufSize 64
U8 i, tempBuf[bufSize];
if(SOUND_DETECTION_GetSts() != 0)    // sts != wait for sound detect
{
#endif
    if(AudioBufCtl.samples>HEADER_LEN)
	{
	   	WriteHeader();
	}
#if _ADPCM_RECORD_ERASING && _ADPCM_RECORD_SOUND_TRIG && SOUND_TRIG_RECOVER
    CB_EraseData(StorageMode, RecoverAddr, SPI_SECTOR_SIZE);
    CurAddr = StartAddr;
    for(i=0; i<(SPI_SECTOR_SIZE/bufSize); i++)
    {
        CB_ReadData(StorageMode, &CurAddr, tempBuf, bufSize);
        CB_WriteData(StorageMode, &RecoverAddr, tempBuf, bufSize);
    }
}
else                    // sts == wait for sound detect
{
    CurAddr = StartAddr;
    for(i=0; i<(SPI_SECTOR_SIZE/bufSize); i++)
    {
        CB_ReadData(StorageMode, &RecoverAddr, tempBuf, bufSize);
        CB_WriteData(StorageMode, &CurAddr, tempBuf, bufSize);
    }
}
#endif
    AudioBufCtl.state=STATUS_STOP;
    AudioBufCtl.shadow_state=STATUS_STOP;
#if _USE_HEAP_RAM
    ADPCM_FreeMemory(MEMORY_ALLOCATE_RECORD);
#endif

#if _EF_SERIES && _ADPCM_RECORD_ERASING
	if (StorageMode == EF_MODE)
	{
		GIE_DISABLE();
		P_SMU->INTV_BA = C_INTV_BA_Dis;
		GIE_ENABLE();
		INTC_IrqCmd(intcCmdTemp,ENABLE);
	}
#endif

    CB_ADPCM_RecEnd();
}
#endif
//------------------------------------------------------------------//
// Start play voice
// Parameter: 
//          index: file index or absolute address
//          mode:OTP_MODE,EF_MODE,SPI_MODE,SPI1_MODE,EF_UD_MODE
// return value:
//          0:ok
//          others:fail
//------------------------------------------------------------------// 
U8 ADPCM_Play(U32 index,U8 mode){
    struct AUDIO_HEADER *pheader;
#if !(_ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR||(_ADPCM_DECODE_FLOW_TYPE==TYPE1))
    U8 header[HEADER_LEN];
#endif   

    if(AudioBufCtl.shadow_state==STATUS_PLAYING){

#if SMOOTH_ENABLE        
            AudioBufCtl.state=STATUS_STOP;
            SmoothFunc(SMOOTH_DOWN);
            if(!DacMixerRamp_IsBias()) {
                AudioBufCtl.shadow_state=STATUS_STOP;
#if _ADPCM_QIO_MODULE      
                QIO_Stop(QIO_USE_ADPCM_CH1,ENABLE);
#endif
            }
            while(AudioBufCtl.shadow_state==STATUS_PLAYING){
                AUDIO_ServiceLoop();
            }
//            return 0;      
//        }else
#endif            
//        {
//            AudioBufCtl.state=STATUS_STOP;     
//        }    
    }
    
//    DebugBuf=(U16 *)0x2800;
#if _ADPCM_RECORD
    if(AudioBufCtl.shadow_state==STATUS_RECORDING)
    {
        StopRecord();
    }
#endif

#if _USE_HEAP_RAM    
#if _VR_FUNC
    VR_Halt();
#endif
	AUDIO_ServiceLoop();
    ADPCM_AllocateMemory(MEMORY_ALLOCATE_PLAY);
#else
	AUDIO_ServiceLoop();
#endif        
    memset((void *)&AudioBufCtl, 0, sizeof(AudioBufCtl));
    
#if _ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ROBOT3_SOUND ||_ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR

    AudioBufCtl.fifo_count2=-1;
#endif      
//    AudioBufCtl.ramp=ramp;
    
 //   SmoothCtl.p_parameter=0;
    
    if(CB_SetStartAddr(index,mode,0,&StartAddr)){
		return E_AUDIO_INVALID_IDX;
	}

    StorageMode=mode;
    CurAddr=StartAddr;

#if _ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC ||_ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)
#if _ADPCM_PLAY_SR_FOR_SBC_REC
	if(mode == SPI_MODE)
	{
		while(SPI_CheckBusyFlag());
	}
#endif
    CB_ReadData(StorageMode,&CurAddr,(U8 *)AudioBuf,HEADER_LEN);
    
    pheader=(struct AUDIO_HEADER *)AudioBuf;
#else
    CB_ReadData(StorageMode,&CurAddr,(U8 *)header,HEADER_LEN);
    
    pheader=(struct AUDIO_HEADER *)header;
#if _ADPCM_SW_UPSAMPLE
    if(swUpsampleCtrl.cmd)
    {
        pheader->sample_rate *= swUpsampleCtrl.upsampleFactor;
        pheader->samples += 1;
        // initial swUpsampleCtrl
        swUpsampleCtrl.upsampleIndex = -1;
        // swUpsampleCtrl.dataBuf[0] = 0;
        swUpsampleCtrl.dataBuf[1] = 0;
    }
#endif
#endif    
    //dprintf("Play:%x,%x,%x\r\n",pheader->signature[0],pheader->signature[1],pheader->signature[2]);
#if _ADPCM_RECORD
    bool _isSignature2Set1 = (pheader->signature[2]=='1');
#endif
    if(pheader->codec!=CODEC_ADPCM){  
        return E_AUDIO_UNSUPPORT_CODEC;    
    }     
    if(pheader->samples==0)
    {
    	return 1;	//samples is 0
    }

	GIE_DISABLE();
#if	_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND
    S8 hw_ch = DacAssigner_Regist(kADPCM_CH1,ADPCM_CH1_EXCLUSIVE);
#else
	S8 hw_ch = DacAssigner_Regist(kADPCM_CH1,pheader->sample_rate);
#endif
	GIE_ENABLE();
    if(hw_ch<0){
        //dprintf("No play Invalid sr\r\n");
        return E_AUDIO_INVALID_SAMPLERATE;
    }

#if _ADPCM_QIO_MODULE
	if(pheader->nyq_offset)
	{
		QIO_Play(QIO_USE_ADPCM_CH1, mode, (StartAddr+HEADER_LEN+pheader->nyq_offset));
	}
#endif//_ADPCM_QIO_MODULE    
    AudioBufCtl.samples=pheader->samples;
    AudioBufCtl.in_size=pheader->decode_in_size;
    AudioBufCtl.out_size=pheader->decode_out_size;
    AudioBufCtl.sample_rate=pheader->sample_rate;
    AudioBufCtl.upsample=pheader->flag&0x1;   

#if	_ADPCM_ECHO_SOUND    
    if((EchoCtrl.Cmd)&&(AudioBufCtl.sample_rate>_ADPCM_ECHO_MAX_SAMPLE_RATE))
	{
    	//To avoid the resource miss match. 
//    	dprintf("The sample rate of playing is more than sample rate of setting\r\n");
		return E_AUDIO_INVALID_SAMPLERATE;
	}
#endif
    
    
#if _ADPCM_ROBOT4_SOUND && (_ADPCM_SPEED_CHANGE || _ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR)
	Robot04Ctl.offset=0;
#endif	

#if (_ADPCM_ECHO_SOUND || _ADPCM_ROBOT4_SOUND || (_ADPCM_DECODE_FLOW_TYPE==TYPE1)) && !(_ADPCM_SPEED_CHANGE || _ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR)
	lastAudio=0;
#if _ADPCM_ECHO_SOUND
	EchoCtrl.Offset=0;
#endif
#if _ADPCM_ROBOT4_SOUND
	Robot04Ctl.offset=0;
#endif

	memset((void *)tmp_AudioBuf, 0, ADPCM_AUDIO_BUF_SIZE01<<1);
	memset((void *)&playEnd, 0, sizeof(playEnd));							
	playEnd.Last= AudioBufCtl.samples%96;									
	if(playEnd.Last!=0)														
		playEnd.RefFrame= (AudioBufCtl.samples/96)+1;						
	else																	
		playEnd.RefFrame= (AudioBufCtl.samples/96);							
	//dprintf("sample:%d,RefFrame%d,Last:%d\n\r", AudioBufCtl.samples,playEnd.RefFrame,playEnd.Last);	
#endif
    
    BufCtl.offset=0;
#if _ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)
    BufCtl.size=ADPCM_AUDIO_BUF_SIZE;
    BufCtl.buf=AudioBuf;
#endif

//    
#if SMOOTH_ENABLE
//    if(ramp&RAMP_UP){
        SmoothFunc(SMOOTH_UP);
//    }else{
//        SmoothFunc(SMOOTH_NONE);
//    }  
#endif

    
    SetADPCMMode(pheader->decode_in_size,pheader->decode_out_size,ModeBuf,0);    
    ADPCMDecInit(DecBuf, ADPCM0GetByte, 0);

	CB_ADPCM_InitDac(pHwCtl[hw_ch].p_dac,AudioBufCtl.sample_rate,AudioBufCtl.upsample);
   
         
///////////////
#if (_ADPCM_SPEED_CHANGE&&_ADPCM_PITCH_CHANGE)
	if(AudioBufCtl.sample_rate<=SPEED_CTL_SAMPLE_RATE)
	{
		SpeedCtl.index=0;
		SpeedPitchInitFlag = 0;

        if(SpeedCtl.speed_ratio_index==RATIO_INIT_INDEX && SpeedCtl.pitch_ratio_index==RATIO_INIT_INDEX)
		{
            #if _USE_HEAP_RAM
			if(SpeedPitchBuf != NULL){
        		MemFree(SpeedPitchBuf);
            	SpeedPitchBuf=NULL;
        	}
			#endif 
            
            
            //return;//no change
		}
        else if(SpeedCtl.speed_ratio_index==RATIO_INIT_INDEX)
        {
			#if _USE_HEAP_RAM

	        if(SpeedPitchBuf == NULL){
	            SpeedPitchBuf=(SPEEDPITCHBUF *)MemAlloc(SPEEDPITCHBUFSIZE);
	        }
			#endif 
			
			SpeedPitchInitFlag = 1;
			SpeedPitchBuf->PerformanceIndex = (S8)_ADPCM_SPEEDPITCH_HQ;
			SpeedPitchChange_Init(SpeedPitchInitFlag, 1/PitchRatioTbl[SpeedCtl.pitch_ratio_index], 1.0,SpeedPitchBuf);
        }
        else if(SpeedCtl.pitch_ratio_index==RATIO_INIT_INDEX)
        {
		    #if _USE_HEAP_RAM
	        if(SpeedPitchBuf == NULL){
	            SpeedPitchBuf=(SPEEDPITCHBUF *)MemAlloc(SPEEDPITCHBUFSIZE);
	        }
			#endif 
		    
		    SpeedPitchInitFlag = 1;
		    SpeedPitchBuf->PerformanceIndex = (S8)_ADPCM_SPEEDPITCH_HQ;
		    SpeedPitchChange_Init(SpeedPitchInitFlag, SpeedRatioTbl[SpeedCtl.speed_ratio_index], 1.0,SpeedPitchBuf);
        }
        else
        {
		    #if _USE_HEAP_RAM
	        if(SpeedPitchBuf == NULL){
	            SpeedPitchBuf=(SPEEDPITCHBUF *)MemAlloc(SPEEDPITCHBUFSIZE);
	        }
			#endif 
		    
			SpeedPitchInitFlag = 1;
			SpeedPitchBuf->PerformanceIndex = (S8)_ADPCM_SPEEDPITCH_HQ;
			SpeedPitchChange_Init(SpeedPitchInitFlag, SpeedRatioTbl[SpeedCtl.speed_ratio_index], 1/PitchRatioTbl[SpeedCtl.pitch_ratio_index],SpeedPitchBuf);
        }
		
        CB_ADPCM_ChangeSR(pHwCtl[hw_ch].p_dac,AudioBufCtl.sample_rate*PitchRatioTbl[SpeedCtl.pitch_ratio_index]);
		
	}
#else
#if _ADPCM_SPEED_CHANGE
    SpeedCtl.index=0;
    TimeStretchInit(1,SpeedRatioTbl[SpeedCtl.speed_ratio_index]);
#endif
#if _ADPCM_PITCH_CHANGE
    SpeedCtl.index=0;
    TimeStretchInit(0,1/PitchRatioTbl[SpeedCtl.pitch_ratio_index]);
    CB_ADPCM_ChangeSR(pHwCtl[hw_ch].p_dac,AudioBufCtl.sample_rate*PitchRatioTbl[SpeedCtl.pitch_ratio_index]);
#endif

#endif	
////////////

#if _ADPCM_SPEEDPITCH_N
   if((AudioBufCtl.sample_rate<=SPEED_CTL_SAMPLE_RATE)&&SpeedPitchNFlag)
   {
		SpeedCtl.index=0;
		SpeedPitchNInitFlag = 0;

        if(SpeedCtl.speedpitch_ratio_index==0)
		{
            #if _USE_HEAP_RAM
	        if(SpeedPitchN!=NULL)
	        {
	            MemFree(SpeedPitchN);
	            SpeedPitchN=NULL;
	        }			
			#endif 
		}
        else
        {
		    #if _USE_HEAP_RAM
 			if(SpeedPitchN==NULL)
	        {
	            SpeedPitchN = (S8 *)MemAlloc(SpeedPitchNSIZE);
	        }		
			#endif 
		    
			SpeedPitchNInitFlag = 1;
			RateChangeInit((S8)SpeedPitchNInitFlag, (S8)SpeedCtl.speedpitch_ratio_index,SpeedPitchN);
        }
        CB_ADPCM_ChangeSR(pHwCtl[hw_ch].p_dac,AudioBufCtl.sample_rate); 
    }
#endif

#if _ADPCM_ANIMAL_ROAR
    if(AnimalRoarCtl.cmd)
    {
        AnimalRoarALGOCtl.PerformanceIndex = _ADPCM_SPEEDPITCH_HQ;
    #if _USE_HEAP_RAM
        if(AnimalRoarTotalRam==NULL)
        {
            AnimalRoarTotalRam=(S8 *)MemAlloc(ANIMAL_ROAR_RAM_SIZE);
        }
    #endif
        U8 tempInit;
        if(AnimalRoarCtl.initFlag){
            tempInit = 0;
        }else{
            tempInit = 1;
            AnimalRoarCtl.initFlag = 1;
        }    
        AnimalRoarALGOCtl.Reset = tempInit;                       // speed pitch init reset
        DinosaurSound_Init(&AnimalRoarALGOCtl, AnimalRoarTotalRam); // AnimalRoar Initial

        CB_ADPCM_ChangeSR(pHwCtl[hw_ch].p_dac,AudioBufCtl.sample_rate*((float)1.0f/AnimalRoarALGOCtl.PitchScale));
    }
#endif

#if _ADPCM_DARTH_SOUND
    if(DarthCtrl.Cmd)
    {
        SpeedCtl.index=0;
        SetDarthSound_Init(1);
        CB_ADPCM_ChangeSR(pHwCtl[hw_ch].p_dac,AudioBufCtl.sample_rate*PitchRatioTbl[SpeedCtl.darth_ratio_index]);
    }
#endif

#if _ADPCM_ROBOT2_SOUND
    if(Robot01Ctl.enable)
    {
#if _USE_HEAP_RAM
        if(Robot01_buf==NULL)   Robot01_buf = (S16 *) MemAlloc(ADPCM_ROBOT_BUF_SIZE<<1);
#endif
        memset((void *)Robot01_buf, 0, ADPCM_ROBOT_BUF_SIZE*2);
        ROBOT_EFFECT_01_Init(Robot01_buf,Robot01Ctl.type,Robot01Ctl.thres,Robot01Ctl.detectthres,AudioBufCtl.sample_rate,ADPCM_AUDIO_OUT_SIZE,90,1620);
    }
#endif
#if _ADPCM_ROBOT3_SOUND
    SpeedCtl.rb_ptr=0;
    SpeedCtl.index=0;
    if(Robot02Ctl.enable)
    {
#if _USE_HEAP_RAM
        if(Robot02_buf==NULL)   Robot02_buf = (S16 *) MemAlloc(ADPCM_ROBOT02_BUF_SIZE<<1);
#endif        
        memset((void *)Robot02_buf, 0, ADPCM_ROBOT02_BUF_SIZE*2);
        ROBOT_EFFECT_02_Init(Robot02_buf,Robot02Ctl.type,Robot02Ctl.thres,AudioBufCtl.sample_rate);
    }
#endif

#if _ADPCM_ROBOT4_SOUND
	if(Robot04Ctl.enable){
	
		#if _USE_HEAP_RAM	
		if(Robot4ctl==NULL)
        {
        	Robot4ctl = (ROBOT4Ctl*) MemAlloc(ADPCM_ROBOT4_CTL_SIZE);
        }
		if(Robot4buf==NULL)
        {
        	Robot4buf = (ROBOT4Buf*) MemAlloc(ADPCM_ROBOT4_BUF_SIZE);
        }
        if(Robot4FFTctl==NULL)
        {
        	Robot4FFTctl = (FFTctl*)MemAlloc(ADPCM_ROBOT4_FFT_CTL_SIZE);
        }
        #endif    
		memset((void *)Robot4ctl, 0, ADPCM_ROBOT4_CTL_SIZE);		
		Robot4ctl->FFTSize 		= 	ROBOT4_FFT_SIZE;
		#if _ADPCM_RECORD
		if(g_playrecord) 
		{
			Robot4ctl->Samplerate 	= 	_ADPCM_RECORD_SAMPLE_RATE;  
		}
		else
		{
			Robot4ctl->Samplerate 	= 	AudioBufCtl.sample_rate;
		}
		#else
			Robot4ctl->Samplerate 	= 	AudioBufCtl.sample_rate;
		#endif
		Robot4ctl->Overlapratio 	= 	ROBOT4_OVERLAP_RATIO;
		Robot4ctl->InputSize 	= 	ADPCM_ROBOT4_DATA_LENGTH;	
		memset((void *)Robot4buf, 0, ADPCM_ROBOT4_BUF_SIZE);			
		memset((void *)Robot4FFTctl, 0, ADPCM_ROBOT4_FFT_CTL_SIZE);				
		ROBOT_EFFECT_04_Init(Robot4ctl, Robot4buf, Robot4FFTctl);
		
	}
#endif

#if _ADPCM_REVERB_SOUND
    if(ReverbCmd)
    {
#if _USE_HEAP_RAM
        if(Reverb_x==NULL)
        {
            Reverb_x = (S16 *) MemAlloc(ADPCM_REVERB_BUF_SIZE<<1);
            Reverb_y = (S16 *) MemAlloc(ADPCM_REVERB_BUF_SIZE<<1);
        }
#endif        
        Reverb_Effect_Init(Reverb_x,Reverb_y,ReverbEffect);
    }
#endif

#if _ADPCM_RECORD
#if SMOOTH_ENABLE
    playEndSmoothDownFlag = 0;
#if _ADPCM_SPEED_CHANGE || _ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND ||_ADPCM_ROBOT3_SOUND || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR
    fifo_count2_lastFlag = 0;
#endif
#endif
    g_playrecord = ((index&ADDRESS_MODE)>0)&&_isSignature2Set1;
    if(g_playrecord)
    {
		if(AudioBufCtl.sample_rate!=_ADPCM_RECORD_SAMPLE_RATE){
//			dprintf("record sample rate miss match\r\n");
			return E_AUDIO_INVALID_SAMPLERATE;
		}
#if defined(_ADPCM_RECORD_PLAY_USE_EQ0)		\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ1)	\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ2)	\
	||defined(_ADPCM_RECORD_PLAY_USE_CUSTOMIZE)
		if(initRecPlayEQParameter()!=EXIT_SUCCESS){
			return 1;//init error
		}
#endif

#if defined(_ADPCM_RECORD_PLAY_USE_DRC)
		drcFixedCtl.InputThreshold = -19.19f;
		drcFixedCtl.InputRatio = 20.47f;
		DRC_Init(&drcFixedCtl, DRC_buf, AudioBufCtl.sample_rate);
#elif defined(_ADPCM_RECORD_PLAY_USE_DIGITAL_GAIN)	
		memset((void *)ADPCMDigiAmpCTLBuf,0,34*2);
		memset((void *)ADPCMDigiAmpBuf,0,36*2);
		RTDigiAmpInit(ADPCMDigiAmpArray, ADPCMDigiAmpSet, ADPCMDigiAmpBuf, ADPCMDigiAmpCTLBuf);
		RTVadInit(ADPCMVADArray, (ADPCMDigiAmpCTLBuf+22));
#endif
    } 
	else
	{
#if defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
		if(initPlayBackEQParameter()!=EXIT_SUCCESS){
			return 1;//init error
		}
#endif
	}
#else
#if defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
	if(initPlayBackEQParameter()!=EXIT_SUCCESS){
		return 1;//init error
	}
#endif
#endif


#if _ADPCM_ECHO_SOUND

#if !(_ADPCM_SPEED_CHANGE || _ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR)

	if((AudioBufCtl.samples%NEW_ADPCM_ECHO_DATA_LENGTH)!=0)
		AudioBufCtl.samples2=((AudioBufCtl.samples/NEW_ADPCM_ECHO_DATA_LENGTH)+1)*NEW_ADPCM_ECHO_DATA_LENGTH;	
	else
		AudioBufCtl.samples2=AudioBufCtl.samples;

	if	(AudioBufCtl.sample_rate==SAMPLE_RATE_8000)		
#if _ADPCM_ECHO_EFFECT==DELAY_1200ms
		AudioBufCtl.samples2+=9600;		//add 1200ms echo-delay (std.)9600 
#elif _ADPCM_ECHO_EFFECT==DELAY_800ms
		AudioBufCtl.samples2+=6400;		//add 800ms echo-delay (std.)6400
#elif _ADPCM_ECHO_EFFECT==DELAY_320ms
		AudioBufCtl.samples2+=2560;		//add 320ms echo-delay (std.)2560	
#else	//user define
		AudioBufCtl.samples2+=(ADPCM_ECHO_UD_TOTAL_DELAY_TIME*AudioBufCtl.sample_rate)/1000; 
#endif
	else if	(AudioBufCtl.sample_rate==SAMPLE_RATE_10000)	
#if _ADPCM_ECHO_EFFECT==DELAY_1200ms
		AudioBufCtl.samples2+=12000;	//add 1200ms echo-delay (std.)12000 
#elif _ADPCM_ECHO_EFFECT==DELAY_800ms
		AudioBufCtl.samples2+=8000;		//add 800ms echo-delay (std.)8000
#elif _ADPCM_ECHO_EFFECT==DELAY_320ms
		AudioBufCtl.samples2+=3200;		//add 320ms echo-delay (std.)3200
#else	//user define
		AudioBufCtl.samples2+=(ADPCM_ECHO_UD_TOTAL_DELAY_TIME*AudioBufCtl.sample_rate)/1000; 
#endif
	else if	(AudioBufCtl.sample_rate==SAMPLE_RATE_12000)
#if _ADPCM_ECHO_EFFECT==DELAY_1200ms
		AudioBufCtl.samples2+=14400;	//add 1200ms echo-delay (std.)14400 
#elif _ADPCM_ECHO_EFFECT==DELAY_800ms
		AudioBufCtl.samples2+=9600;		//add 800ms echo-delay (std.)9600
#elif _ADPCM_ECHO_EFFECT==DELAY_320ms
		AudioBufCtl.samples2+=3840;		//add 320ms echo-delay (std.)3840	
#else	//user define
		AudioBufCtl.samples2+=(ADPCM_ECHO_UD_TOTAL_DELAY_TIME*AudioBufCtl.sample_rate)/1000;
#endif
	else if	(AudioBufCtl.sample_rate==SAMPLE_RATE_16000)
#if _ADPCM_ECHO_EFFECT==DELAY_1200ms
		AudioBufCtl.samples2+=19200;	//add 1200ms echo-delay (std.)19200 
#elif _ADPCM_ECHO_EFFECT==DELAY_800ms
		AudioBufCtl.samples2+=12800;	//add 800ms echo-delay (std.)12800
#elif _ADPCM_ECHO_EFFECT==DELAY_320ms
		AudioBufCtl.samples2+=5120;		//add 320ms echo-delay (std.)5120	
#else	//user define
		AudioBufCtl.samples2+=(ADPCM_ECHO_UD_TOTAL_DELAY_TIME*AudioBufCtl.sample_rate)/1000;
#endif
#endif


	if((AudioBufCtl.sample_rate==SAMPLE_RATE_8000)||(AudioBufCtl.sample_rate==SAMPLE_RATE_10000)||(AudioBufCtl.sample_rate==SAMPLE_RATE_12000)||(AudioBufCtl.sample_rate==SAMPLE_RATE_16000))
	{
#if _ADPCM_ECHO_QUALITY
	echo_bufSize=(((((AudioBufCtl.sample_rate * ADPCM_ECHO_UD_DELAY_TIME + 16000 )/32000)*21*ADPCM_ECHO_UD_REPEAT_TIME)+21+1)>>1)+62;
#else
	echo_bufSize=(((((AudioBufCtl.sample_rate * ADPCM_ECHO_UD_DELAY_TIME + 8000 )/16000)*9*ADPCM_ECHO_UD_REPEAT_TIME)+9+1)>>1)+62;
#endif
	}
	else
	{
		EchoCtrl.Cmd=DISABLE;		
		EchoCtrl.SR_Limit=ENABLE;
	}

    if(EchoCtrl.Cmd)
    {
	
#if _USE_HEAP_RAM
		if(EchoBuf!=NULL){	
			MemFree(EchoBuf);
			EchoBuf=NULL;
		}
		if(EchoBuf==NULL){
			EchoBuf = (S16 *)MemAlloc(NEW_ADPCM_ECHO_BUF_SIZE*2);
		}
         
		memset((void *)EchoBuf, 0, NEW_ADPCM_ECHO_BUF_SIZE*2); 	
#else
		memset((void *)EchoBuf, 0, NEW_ADPCM_ECHO_BUF_SIZE*2);	
#endif              


#if (_ADPCM_ECHO_EFFECT!=DELAY_USER_DEFINE)
		echoCtrl_T.Cmd = 0;		//cmd 0 is default
	#if _ADPCM_ECHO_EFFECT == 1200
		echoCtrl_T.iEchoRepeatTimes = 6;
	#elif _ADPCM_ECHO_EFFECT == 800
		echoCtrl_T.iEchoRepeatTimes = 4;
	#elif _ADPCM_ECHO_EFFECT == 320
		echoCtrl_T.iEchoRepeatTimes = 2;
	#endif
		echoCtrl_T.bitmode = 4;		//Basic
		echoCtrl_T.iEchoTotalDelayTime = _ADPCM_ECHO_EFFECT;
		if(_ADPCM_ECHO_QUALITY){
			echoCtrl_T.bitmode = 5;
		}
#else		
	//error message 
	#if _ADPCM_ECHO_UD_REPEAT_TIME != DISABLE 
		#if (_ADPCM_ECHO_UD_REPEAT_TIME > 6) || (_ADPCM_ECHO_UD_REPEAT_TIME < /*3*/1)
			#error "Please check the _ADPCM_ECHO_UD_REPEAT_TIME(1~6)."
		#endif
	#endif
	
	#if ADPCM_ECHO_UD_TOTAL_DELAY_TIME != DISABLE 
		#if ((ADPCM_ECHO_UD_TOTAL_DELAY_TIME > 1200) || (ADPCM_ECHO_UD_TOTAL_DELAY_TIME < 50))
			#error "Please check the ADPCM_ECHO_UD_TOTAL_DELAY_TIME(50ms~1200ms)."
		#endif
	#endif
	echoCtrl_T.Cmd = 1;		//cmd 1 is user define
	
	if ((_ADPCM_ECHO_UD_REPEAT_TIME <= 6) && (_ADPCM_ECHO_UD_REPEAT_TIME >= 1)) {
		echoCtrl_T.iEchoRepeatTimes = _ADPCM_ECHO_UD_REPEAT_TIME;
	}
	echoCtrl_T.iEchoTotalDelayTime = ADPCM_ECHO_UD_TOTAL_DELAY_TIME;
	echoCtrl_T.bitmode = 4;		//Basic
	if (_ADPCM_ECHO_QUALITY){
		echoCtrl_T.bitmode = 5;
	}
	
#endif
		echoCtrl_T.iSampleRate = AudioBufCtl.sample_rate;	//or play sample rate
		char EchoStatus;
		EchoStatus = EchoSound_init((S16*)EchoBuf, echoCtrl_T.iEchoTotalDelayTime, echo_bufSize,  &echoCtrl_T);
		if(EchoStatus == -2){
		}
		else if(EchoStatus == -3){
		}


    }
#endif

    AudioVol=  (ChVol+1)*(MixerVol+1); 
        
    if(!ChVol){
        AudioVol=0;
    }    
    if(AudioVol==256){
        AudioVol=255;
    }    
    AudioVol=AudioVol<<7;
#if _AUDIO_VOLUME_CHANGE_SMOOTH
    AUDIO_InitVolumeSmooth(&VolCtrl,AudioBufCtl.sample_rate,AudioVol);
#endif

    AudioBufCtl.frame_size=0;
    AudioBufCtl.fifo_count_tmp=0;
//     
//    DecodeFrame();
//    DecodeFrame();
#if _ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)
#if	PR_READ
	CB_ReadData(StorageMode,&CurAddr,EncDataBuf,ADPCM_DATA_BUF_SIZE);    
    BufCtl.rd_offset=0;
    BufCtl.wr_offset=0;   
#endif 
    memset((void *)AudioBuf, 0, ADPCM_AUDIO_BUF_SIZE*2);

    DecodeFrame();
#if _ADPCM_DARTH_SOUND
    if((DarthCtrl.Cmd)||(!AudioBufCtl.fifo_count_tmp)) //add condition (!AudioBufCtl.fifo_count_tmp) for avoid noise form flow loading
    {
        DecodeFrame();
        DecodeFrame();
    }
#endif    
#if (_ADPCM_SPEED_CHANGE&&_ADPCM_PITCH_CHANGE) || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR
 	while(AudioBufCtl.process_count < (ADPCM_AUDIO_BUF_SIZE-180)){
		DecodeFrame();
	}
#elif _ADPCM_PLAY_SR_FOR_SBC_REC
 	while(AudioBufCtl.process_count < (ADPCM_AUDIO_BUF_SIZE-1*ADPCM_AUDIO_OUT_SIZE)){
		DecodeFrame();
	}
#endif    
#else
    CB_ReadData(StorageMode,&CurAddr,EncDataBuf,ADPCM_DATA_BUF_SIZE);    
    BufCtl.rd_offset=0;
    BufCtl.wr_offset=0;
#if _ADPCM_PLAY_REPEAT
    AudioBufCtl.encode_size = pheader->file_size-HEADER_LEN;
	if(pheader->nyq_offset){                              // for compatibility with versions prior SPI_Encoder 2.01
		AudioBufCtl.encode_size = pheader->nyq_offset;
	}
#endif
#endif

    OpMode=0;
    AudioBufCtl.state=  STATUS_PLAYING;
    AudioBufCtl.shadow_state=AudioBufCtl.state;
    
    CB_ADPCM_PlayStart();   
	CB_ADPCM_DacCmd(pHwCtl[hw_ch].p_dac,ENABLE);
        
    return 0;
}   

//------------------------------------------------------------------//
// Pause playing
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_Pause(void){
    if(AudioBufCtl.state==STATUS_PLAYING){
	
#if SMOOTH_ENABLE        
//        if(AudioBufCtl.ramp&RAMP_DOWN){
            AudioBufCtl.state=STATUS_PAUSE;
            SmoothFunc(SMOOTH_DOWN);
//        }else
#endif            
//        {
//            AudioBufCtl.state=STATUS_PAUSE;
//        }    
    }
#if _ADPCM_RECORD 
    if(AudioBufCtl.state==STATUS_RECORDING){
        CB_ADPCM_AdcCmd(DISABLE);
        AudioBufCtl.state=STATUS_PAUSE;      
    }    
#endif         
} 
//------------------------------------------------------------------//
// Resume playing
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_Resume(void){
	if(AudioBufCtl.state==STATUS_PAUSE&&(!OpMode)){
        if(DacAssigner_codecUseDacCh(kADPCM_CH1)<0) {
            GIE_DISABLE();
#if	_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ANIMAL_ROAR
            S8 hw_ch = DacAssigner_Regist(kADPCM_CH1,ADPCM_CH1_EXCLUSIVE);
#else
            S8 hw_ch = DacAssigner_Regist(kADPCM_CH1,AudioBufCtl.sample_rate);
#endif
            GIE_ENABLE();
            if(hw_ch<0) return;//error
        
            CB_ADPCM_InitDac(pHwCtl[hw_ch].p_dac,AudioBufCtl.sample_rate,AudioBufCtl.upsample);

#if _ADPCM_DARTH_SOUND
            if(DarthCtrl.Cmd)
            {
                CB_ADPCM_ChangeSR(pHwCtl[hw_ch].p_dac,AudioBufCtl.sample_rate*PitchRatioTbl[SpeedCtl.darth_ratio_index]);
            }
#if	_ADPCM_PITCH_CHANGE                
            else
#endif
#endif            
#if	_ADPCM_PITCH_CHANGE                
            ADPCM_SetPitch(SpeedCtl.pitch_ratio_index-RATIO_INIT_INDEX);
#elif _ADPCM_ANIMAL_ROAR
            if(AnimalRoarCtl.cmd)
            {
                CB_ADPCM_ChangeSR(pHwCtl[hw_ch].p_dac,AudioBufCtl.sample_rate*((float)1.0f/AnimalRoarALGOCtl.PitchScale));
            }
#endif            

            CB_ADPCM_DacCmd(pHwCtl[hw_ch].p_dac,ENABLE);
        }
	
#if SMOOTH_ENABLE        
//        if(AudioBufCtl.ramp&RAMP_UP){
            SmoothFunc(SMOOTH_UP);
//        }
#endif        
        AudioBufCtl.state=STATUS_PLAYING;
        AudioBufCtl.shadow_state=AudioBufCtl.state;
    }    
#if _ADPCM_RECORD 
    if(AudioBufCtl.state==STATUS_PAUSE&&OpMode){
        AudioBufCtl.state=STATUS_RECORDING;  
        CB_ADPCM_AdcCmd(ENABLE);
    }    
#endif               
} 
//------------------------------------------------------------------//
// Stop playing
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_Stop(void){
    if((AudioBufCtl.state==STATUS_PLAYING||AudioBufCtl.state==STATUS_PAUSE)&&(!OpMode)){ 
#if SMOOTH_ENABLE        
//        if(AudioBufCtl.ramp&RAMP_DOWN){
            AudioBufCtl.state=STATUS_STOP;
            SmoothFunc(SMOOTH_DOWN);
			if (AudioBufCtl.shadow_state == STATUS_PAUSE
                || !DacMixerRamp_IsBias()
            ) {
				AudioBufCtl.shadow_state = STATUS_STOP;
#if _ADPCM_QIO_MODULE
				QIO_Stop(QIO_USE_ADPCM_CH1,ENABLE);
#endif//_ADPCM_QIO_MODULE
			}
//        }else
#endif            
//        {
//            AudioBufCtl.state=STATUS_STOP;
//            CB_ADPCM_PlayEnd();
//        }
    }            
#if _ADPCM_RECORD 
    if((AudioBufCtl.state==STATUS_RECORDING||AudioBufCtl.state==STATUS_PAUSE)&&OpMode){
        StopRecord();
    }    
#endif     
} 
//------------------------------------------------------------------//
// Get current status
// Parameter: 
//          NONE
// return value:
//          status:STATUS_PLAYING,STATUS_PAUSE,STATUS_STOP
//------------------------------------------------------------------// 
U8 ADPCM_GetStatus(void){
    return AudioBufCtl.shadow_state;    
}
//------------------------------------------------------------------//
// Set audio volume
// Parameter: 
//          vol:0~15
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_SetVolume(U8 vol){   
    
    if(vol>CH_VOL_15){
        vol=CH_VOL_15;
    }    
    ChVol=  vol;  
}
#if defined(_ADPCM_RECORD_PLAY_USE_CUSTOMIZE)\
	||defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
//------------------------------------------------------------------//
// Set customize filter index
// Parameter: 
//          index: (0~FILTER_NUMBER-1)User define filter, (other)All Pass
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_SetEQFilter(U8 index){
	u8EQFilterIndex = index;
#if	_ADPCM_RECORD
	if(g_playrecord)
	{
#if defined(_ADPCM_RECORD_PLAY_USE_CUSTOMIZE)
		if(u8EQFilterIndex >= FILTER_NUMBER){
			EQFilterCtl.FilterPara1 = _AllPassFilterParaTab;//all pass parameter
		} else {
			EQFilterCtl.FilterPara1 = (S16*)FilterList[u8EQFilterIndex];//UserDefine
		}
#endif
	}
	else
	{
#if defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
		if(u8EQFilterIndex >= FILTER_NUMBER){
			EQFilterCtl.FilterPara1 = _AllPassFilterParaTab;//all pass parameter
		} else {
			EQFilterCtl.FilterPara1 = (S16*)FilterList[u8EQFilterIndex];//UserDefine
		}
#endif
	}
#else
#if defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
	if(u8EQFilterIndex >= FILTER_NUMBER){
		EQFilterCtl.FilterPara1 = _AllPassFilterParaTab;//all pass parameter
	} else {
		EQFilterCtl.FilterPara1 = (S16*)FilterList[u8EQFilterIndex];//UserDefine
	}
#endif
#endif
}
#endif
//------------------------------------------------------------------//
// Set speed ratio
// Parameter: 
//          index: -12~12
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_SetSpeed(S8 index){   
#if _ADPCM_DARTH_SOUND && _ADPCM_SPEED_CHANGE
    if(DarthCtrl.Cmd) 
    {
        //Dprintf("Auto Disable Darth in Speed\r\n");
        ADPCM_SetDarthSound(DISABLE,0,0);
    }
#endif
#if (_ADPCM_PITCH_CHANGE&&_ADPCM_SPEED_CHANGE)
	S8 speed_index_tmp,pitch_index_tmp;
	speed_index_tmp=index;
	pitch_index_tmp=SpeedCtl.pitch_ratio_index-RATIO_INIT_INDEX;
	ADPCM_SetSpeedPitch(speed_index_tmp,pitch_index_tmp);
#else
#if _ADPCM_SPEED_CHANGE
    if(AudioBufCtl.sample_rate<=SPEED_CTL_SAMPLE_RATE){
        index+=RATIO_INIT_INDEX;
        SpeedCtl.speed_ratio_index=index;
        TimeStretchInit(0,SpeedRatioTbl[index]);
    }        
#endif  
#endif   
}
//------------------------------------------------------------------//
// Set pitch ratio
// Parameter: 
//          index: -12~12
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_SetPitch(S8 index){   
#if _ADPCM_DARTH_SOUND && _ADPCM_PITCH_CHANGE
    if(DarthCtrl.Cmd) 
    {
        //Dprintf("Auto Disable Darth in Pitch\r\n");
        ADPCM_SetDarthSound(DISABLE,0,0);
    }
#endif
    
#if (_ADPCM_PITCH_CHANGE&&_ADPCM_SPEED_CHANGE)	
	S8 speed_index_tmp,pitch_index_tmp;
	speed_index_tmp=SpeedCtl.speed_ratio_index-RATIO_INIT_INDEX;
	pitch_index_tmp=index;
	ADPCM_SetSpeedPitch(speed_index_tmp,pitch_index_tmp);	
#else    
#if _ADPCM_PITCH_CHANGE
    if(AudioBufCtl.sample_rate<=SPEED_CTL_SAMPLE_RATE){
        index+=RATIO_INIT_INDEX;
        SpeedCtl.pitch_ratio_index=index;
        TimeStretchInit(0,1/PitchRatioTbl[index]);
		
#if _ADPCM_RECORD
		if(g_playrecord)
		{
#if defined(_ADPCM_RECORD_PLAY_USE_EQ0)		\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ1)	\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ2)
			EQFilterCtl.PitchIndex=SpeedCtl.pitch_ratio_index-RATIO_INIT_INDEX;
#elif defined(_ADPCM_RECORD_PLAY_USE_CUSTOMIZE)
			EQFilterCtl.PitchIndex=0;
#endif
		}
		else
		{
#if defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
			EQFilterCtl.PitchIndex=0;
#endif
		}
#else
#if defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
		EQFilterCtl.PitchIndex=0;
#endif
#endif

        S8 hw_ch = DacAssigner_codecUseDacCh(kADPCM_CH1);
		if(hw_ch<0)return;//No register hw_ch
		CB_ADPCM_ChangeSR(pHwCtl[hw_ch].p_dac,AudioBufCtl.sample_rate*PitchRatioTbl[index]);
    }
#endif     
#endif
}

#if (_ADPCM_PITCH_CHANGE&&_ADPCM_SPEED_CHANGE)
void ADPCM_SetSpeedPitch(S8 speed_index, S8 pitch_index)
{
	if(AudioBufCtl.sample_rate<=SPEED_CTL_SAMPLE_RATE)
	{
#if _ADPCM_DARTH_SOUND
        if(DarthCtrl.Cmd)
        {
            //Dprintf("Auto Disable Darth in SpPitch\r\n");
            ADPCM_SetDarthSound(DISABLE,0,0);
        }
#endif
		speed_index+=RATIO_INIT_INDEX;
		pitch_index+=RATIO_INIT_INDEX;
		SpeedCtl.speed_ratio_index=speed_index;
		SpeedCtl.pitch_ratio_index=pitch_index;
		if(speed_index==RATIO_INIT_INDEX && pitch_index==RATIO_INIT_INDEX)
		{
            #if _USE_HEAP_RAM
	        if(SpeedPitchBuf != NULL){
            	MemFree(SpeedPitchBuf);
           	 	SpeedPitchBuf = NULL;
	        }
			#endif 
			SpeedPitchFlag=0;
			SpeedPitchInitFlag = 0;
            //return;//no change
		}
        else if(speed_index==RATIO_INIT_INDEX)
        {
			#if _USE_HEAP_RAM
	        if(SpeedPitchBuf == NULL){
	            SpeedPitchBuf=(SPEEDPITCHBUF *)MemAlloc(SPEEDPITCHBUFSIZE);
	        }
			#endif   
			U8 tempInit;
			if(SpeedPitchInitFlag){
				tempInit = 0;
			}else{
				tempInit = 1;
				SpeedPitchInitFlag = 1;
			}
			SpeedPitchBuf->PerformanceIndex = (S8)_ADPCM_SPEEDPITCH_HQ;
		  	SpeedPitchChange_Init(tempInit, 1/PitchRatioTbl[pitch_index], 1.0,SpeedPitchBuf);
		  	SpeedPitchFlag=0;
        }
        else if(pitch_index==RATIO_INIT_INDEX)
        {
        	#if _USE_HEAP_RAM
	        if(SpeedPitchBuf == NULL){
	            SpeedPitchBuf=(SPEEDPITCHBUF *)MemAlloc(SPEEDPITCHBUFSIZE);
	        }
			#endif 
			U8 tempInit;
			if(SpeedPitchInitFlag){
				tempInit = 0;
			}else{
				tempInit = 1;
				SpeedPitchInitFlag = 1;
			}
			SpeedPitchBuf->PerformanceIndex = (S8)_ADPCM_SPEEDPITCH_HQ;
        	SpeedPitchChange_Init(tempInit, SpeedRatioTbl[speed_index], 1.0,SpeedPitchBuf); 
        	SpeedPitchFlag=0;
        }
        else
        {
        	#if _USE_HEAP_RAM
	        if(SpeedPitchBuf == NULL){
	            SpeedPitchBuf=(SPEEDPITCHBUF *)MemAlloc(SPEEDPITCHBUFSIZE);
	        }
			#endif 
			U8 tempInit;
			if(SpeedPitchInitFlag){
				tempInit = 0;
			}else{
				tempInit = 1;
				SpeedPitchInitFlag = 1;
			}
			SpeedPitchBuf->PerformanceIndex = (S8)_ADPCM_SPEEDPITCH_HQ;
		    SpeedPitchChange_Init(tempInit, SpeedRatioTbl[speed_index], 1/PitchRatioTbl[pitch_index],SpeedPitchBuf);
		    SpeedPitchFlag=1;
        }
#if _ADPCM_RECORD
    if(g_playrecord)
	{
#if defined(_ADPCM_RECORD_PLAY_USE_EQ0)		\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ1)	\
	||defined(_ADPCM_RECORD_PLAY_USE_EQ2)
		EQFilterCtl.PitchIndex=SpeedCtl.pitch_ratio_index-RATIO_INIT_INDEX;
#elif defined(_ADPCM_RECORD_PLAY_USE_CUSTOMIZE)
		EQFilterCtl.PitchIndex=0;
#endif
	}
	else
	{
#if defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
		EQFilterCtl.PitchIndex=0;
#endif
	}
#else
#if defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
	EQFilterCtl.PitchIndex=0;
#endif
#endif
		AudioBufCtl.frame_size=0;

		S8 hw_ch = DacAssigner_codecUseDacCh(kADPCM_CH1);
		if(hw_ch<0)return;//No register hw_ch
		CB_ADPCM_ChangeSR(pHwCtl[hw_ch].p_dac,AudioBufCtl.sample_rate*PitchRatioTbl[pitch_index]);
		
	}   
}   
#endif
//------------------------------------------------------------------//
// Set Animal Roar effect
// Parameter: 
//          cmd: ENABLE/DISABLE
//          speed_index: -12~12
//          pitch_index: -12~12
//          reverb_index: 0~3
// return value:
//          NONE
//------------------------------------------------------------------// 
#if _ADPCM_ANIMAL_ROAR
void ADPCM_SetAnimalRoar(U8 cmd, S8 speed_index, S8 pitch_index, U8 reverb_index)
{
    AnimalRoarCtl.cmd = cmd;

    if(AnimalRoarCtl.cmd)
    {
        speed_index+=RATIO_INIT_INDEX;
        pitch_index+=RATIO_INIT_INDEX;
        AnimalRoarALGOCtl.PerformanceIndex = _ADPCM_SPEEDPITCH_HQ;
        AnimalRoarALGOCtl.SpeedScale = SpeedRatioTbl[speed_index];
        AnimalRoarALGOCtl.PitchScale = ((float)1.0f / PitchRatioTbl[pitch_index]);
        AnimalRoarALGOCtl.ReverbIndex = reverb_index;
    #if _USE_HEAP_RAM
        if(AnimalRoarTotalRam==NULL)
        {
            AnimalRoarTotalRam=(S8 *)MemAlloc(ANIMAL_ROAR_RAM_SIZE);
        }
    #endif
        U8 tempInit;
        if(AnimalRoarCtl.initFlag){
            tempInit = 0;
        }else{
            tempInit = 1;
            AnimalRoarCtl.initFlag = 1;
        }    
        AnimalRoarALGOCtl.Reset = tempInit;                         // speed pitch init reset
        DinosaurSound_Init(&AnimalRoarALGOCtl, AnimalRoarTotalRam); // AnimalRoar Initial
    }
    else
    {
        pitch_index=RATIO_INIT_INDEX;
    #if _USE_HEAP_RAM
        if(AnimalRoarTotalRam!=NULL)
        {
            MemFree(AnimalRoarTotalRam);
            AnimalRoarTotalRam = NULL;
            AnimalRoarCtl.initFlag = 0;
        }
    #endif
    }
    S8 hw_ch = DacAssigner_codecUseDacCh(kADPCM_CH1);
    if(hw_ch<0)return;//No register hw_ch
    CB_ADPCM_ChangeSR(pHwCtl[hw_ch].p_dac,AudioBufCtl.sample_rate*PitchRatioTbl[pitch_index]);
}
#endif
#if _ADPCM_SPEEDPITCH_N
//------------------------------------------------------------------//
// Set speedpitchN ratio
// Parameter: 
//          index: -12~12
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_SetSpeedPitchN(S8 speedpitch_index)
{
	if(AudioBufCtl.sample_rate<=SPEED_CTL_SAMPLE_RATE)
	{
		SpeedCtl.speedpitch_ratio_index=speedpitch_index;
		if(speedpitch_index==0)
		{
            #if _USE_HEAP_RAM
	        if(SpeedPitchN!=NULL)
	        {
	            MemFree(SpeedPitchN);
	            SpeedPitchN=NULL;
	        }			
			#endif 
			SpeedPitchNFlag=0;
			SpeedPitchNInitFlag = 0;
		}
        else
        {
        	#if _USE_HEAP_RAM
	        if(SpeedPitchN==NULL)
	        {
	            SpeedPitchN= (S8 *)MemAlloc(SpeedPitchNSIZE);;
	        }			

			#endif 
			U8 tempInit;
			if(SpeedPitchNInitFlag){
				tempInit = 0;
			}else{
				tempInit = 1;
				SpeedPitchNInitFlag = 1;
			}

			RateChangeInit((S8)tempInit, (S8)SpeedCtl.speedpitch_ratio_index, SpeedPitchN);
		    SpeedPitchNFlag=1;
        }


		GIE_DISABLE();
		S8 hw_ch = DacAssigner_codecUseDacCh(kADPCM_CH1);
		GIE_ENABLE();
		if(hw_ch<0)return;//No register hw_ch
		CB_ADPCM_ChangeSR(pHwCtl[hw_ch].p_dac,AudioBufCtl.sample_rate);
		
	}   
}   
#endif


//------------------------------------------------------------------//
// Enable/Disable Robot Sound Effect
// Parameter: 
//          cmd: 1:enable,0:disable
//          index: carrier index,0~15
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_SetRobot1Sound(U8 cmd,U8 index){
#if _ADPCM_ROBOT1_SOUND   
   
    RobotCtl.enable=cmd;
    
    if(RobotCtl.enable){
        RobotCtl.change=1;
        RobotCtl.index=index;
    }    
#endif    
}   
#if _ADPCM_ROBOT2_SOUND 
//------------------------------------------------------------------//
// Enable/Disable Robot Sound Effect
// Parameter: 
//          cmd: 1:enable,0:disable
//          type: robot type 0~2
//          thres: robot threshold 0~7
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_SetRobot2Sound(U8 cmd,U8 type, U8 thres){
    Robot01Ctl.enable=cmd;
    
    if(Robot01Ctl.enable)
    {
        Robot01Ctl.type=type;
        Robot01Ctl.thres= Robot_Thres[thres]; 
        Robot01Ctl.detectthres = 30;
#if _USE_HEAP_RAM
        if(Robot01_buf==NULL)
        {
            Robot01_buf = (S16 *)MemAlloc(ADPCM_ROBOT_BUF_SIZE<<1);
        }
#endif           
        memset((void *)Robot01_buf, 0, ADPCM_ROBOT_BUF_SIZE*2);
        ROBOT_EFFECT_01_Init(Robot01_buf,Robot01Ctl.type,Robot01Ctl.thres,Robot01Ctl.detectthres,AudioBufCtl.sample_rate,ADPCM_AUDIO_OUT_SIZE,90,1620);
    }    
#if _USE_HEAP_RAM
	else
	{
        if(Robot01_buf!=NULL)
        {
            MemFree(Robot01_buf);
            Robot01_buf = NULL;
        }
	}
#endif 
}
#endif
#if _ADPCM_ROBOT3_SOUND 
//------------------------------------------------------------------//
// Enable/Disable Robot Sound Effect
// Parameter: 
//          cmd: 1:enable,0:disable
//          type: robot type 0~2
//          thres: robot threshold 0~7
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_SetRobot3Sound(U8 cmd,U8 type, U8 thres){
    Robot02Ctl.enable=cmd;
    
    if(Robot02Ctl.enable)
    {
        Robot02Ctl.type=type;
        Robot02Ctl.thres= Robot02_Thres[thres];     
#if _USE_HEAP_RAM
        if(Robot02_buf==NULL)
        {
            Robot02_buf = (S16 *)MemAlloc(ADPCM_ROBOT02_BUF_SIZE<<1);
        }
#endif        
        memset((void *)Robot02_buf, 0, ADPCM_ROBOT02_BUF_SIZE*2);
        ROBOT_EFFECT_02_Init(Robot02_buf,Robot02Ctl.type,Robot02Ctl.thres,AudioBufCtl.sample_rate);
    }    
#if _USE_HEAP_RAM
    else
    {
        if(Robot02_buf!=NULL)
        {
            MemFree(Robot02_buf);
            Robot02_buf = NULL;
        }
    }
#endif     
}
#endif
#if _ADPCM_REVERB_SOUND
//------------------------------------------------------------------//
// Enable/Disable Reverb Sound Effect
// Parameter: 
//          cmd: 1:enable,0:disable
//          index: Effect index,0~7
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_SetReverbSound(U8 cmd,U8 index){
    ReverbCmd = cmd;
    if(cmd)
    {
#if _USE_HEAP_RAM
        if(Reverb_x==NULL)
        {
            Reverb_x = (S16 *)MemAlloc(ADPCM_REVERB_BUF_SIZE<<1);
            Reverb_y = (S16 *)MemAlloc(ADPCM_REVERB_BUF_SIZE<<1);
        }
#endif        
        ReverbEffect = index;
        Reverb_Effect_Init(Reverb_x,Reverb_y,index);
    }    
#if _USE_HEAP_RAM
    else
    {
        if(Reverb_x!=NULL)
        {
            MemFree(Reverb_x);
            Reverb_x = NULL;
            MemFree(Reverb_y);
            Reverb_y = NULL;
        }
    }
#endif    
}        
#endif
#if _ADPCM_ECHO_SOUND
//------------------------------------------------------------------//
// Enable/Disable Echo Sound Effect
// Parameter:
//          cmd: 1:enable,0:disable
// return value:
//          NONE
//------------------------------------------------------------------//
void ADPCM_SetEchoSound(U8 cmd){

if(AudioBufCtl.shadow_state==STATUS_PLAYING){
    if((AudioBufCtl.sample_rate>_ADPCM_ECHO_MAX_SAMPLE_RATE)&&(cmd))
	{
		ADPCM_Stop();
		while(AudioBufCtl.shadow_state==STATUS_PLAYING){
        	AUDIO_ServiceLoop();
        }
    	//To avoid the resource miss match. 
//    	dprintf("The sample rate of playing is more than sample rate of setting\r\n");
		return;
	}
}

    if(cmd == ENABLE && EchoCtrl.SR_Limit==DISABLE)
    {		
#if _USE_HEAP_RAM
		if(EchoBuf==NULL){
			EchoBuf = (S16 *)MemAlloc(NEW_ADPCM_ECHO_BUF_SIZE*2);
		}
		memset((void *)EchoBuf, 0, NEW_ADPCM_ECHO_BUF_SIZE*2);	
#else
		memset((void *)EchoBuf, 0, NEW_ADPCM_ECHO_BUF_SIZE*2);	
#endif       
        
#if (_ADPCM_ECHO_EFFECT!=DELAY_USER_DEFINE)	//Default
		echoCtrl_T.Cmd = 0;		//cmd 0 is default
#if _ADPCM_ECHO_EFFECT == 1200
		echoCtrl_T.iEchoRepeatTimes = 6;
#elif _ADPCM_ECHO_EFFECT == 800
		echoCtrl_T.iEchoRepeatTimes = 4;
#elif _ADPCM_ECHO_EFFECT == 320
		echoCtrl_T.iEchoRepeatTimes = 2;
#endif
		echoCtrl_T.bitmode = 4;		//Basic
		echoCtrl_T.iEchoTotalDelayTime = _ADPCM_ECHO_EFFECT;
		if (_ADPCM_ECHO_QUALITY) {
			echoCtrl_T.bitmode = 5;
		}
#else		
//error message 
#if _ADPCM_ECHO_UD_REPEAT_TIME != DISABLE 
#if (_ADPCM_ECHO_UD_REPEAT_TIME > 6) || (_ADPCM_ECHO_UD_REPEAT_TIME < /*3*/1)
		#error "Please check the _ADPCM_ECHO_UD_REPEAT_TIME(1~6)."
#endif
#endif
				
#if ADPCM_ECHO_UD_TOTAL_DELAY_TIME != DISABLE 
#if ((ADPCM_ECHO_UD_TOTAL_DELAY_TIME > 1200) || (ADPCM_ECHO_UD_TOTAL_DELAY_TIME < 50))
		#error "Please check the ADPCM_ECHO_UD_TOTAL_DELAY_TIME(50ms~1200ms)."
#endif
#endif
		echoCtrl_T.Cmd = 1;		//cmd 1 is user define	
		if ((_ADPCM_ECHO_UD_REPEAT_TIME <= 6) && (_ADPCM_ECHO_UD_REPEAT_TIME >= 1)) {
			echoCtrl_T.iEchoRepeatTimes = _ADPCM_ECHO_UD_REPEAT_TIME;
		}
		echoCtrl_T.iEchoTotalDelayTime = ADPCM_ECHO_UD_TOTAL_DELAY_TIME;
		echoCtrl_T.bitmode = 4;		//Basic
		if (_ADPCM_ECHO_QUALITY) {
			echoCtrl_T.bitmode = 5;
		}
				
#endif
		echoCtrl_T.iSampleRate = AudioBufCtl.sample_rate;	//or play sample rate
		char EchoStatus;
		EchoStatus = EchoSound_init((S16*)EchoBuf, echoCtrl_T.iEchoTotalDelayTime, echo_bufSize  ,  &echoCtrl_T);
		if(EchoStatus == -2){
			return;//1;//init error
		}
		else if(EchoStatus == -3){
		return;//1;//init error
		}
		
#if _ADPCM_ECHO_SOUND && !(_ADPCM_SPEED_CHANGE || _ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_SPEEDPITCH_N)
		EchoCtrl.Offset=lastAudio;
#endif
        EchoCtrl.Cmd = ENABLE;
    }
    else
    {
#if _USE_HEAP_RAM
		if(EchoBuf!=NULL){	
			MemFree(EchoBuf);
			EchoBuf=NULL;
		}
#endif          
        EchoCtrl.SR_Limit=DISABLE;
		EchoCtrl.Cmd = DISABLE;
    }
}
#endif

#if _ADPCM_ROBOT4_SOUND 
//------------------------------------------------------------------//
// Enable/Disable ROBOT4 Sound Effect
// Parameter:
//          cmd: 1:enable,0:disable
// return value:
//          NONE
//------------------------------------------------------------------//
void ADPCM_SetRobot4Sound(U8 cmd){
	
    if(cmd == ENABLE)
    {	
		#if _USE_HEAP_RAM
		if(Robot4ctl==NULL)
        {
        	Robot4ctl = (ROBOT4Ctl*) MemAlloc(ADPCM_ROBOT4_CTL_SIZE);
        }
		if(Robot4buf==NULL)
        {
        	Robot4buf = (ROBOT4Buf*) MemAlloc(ADPCM_ROBOT4_BUF_SIZE);
        }
        if(Robot4FFTctl==NULL)
        {
        	Robot4FFTctl = (FFTctl*) MemAlloc(ADPCM_ROBOT4_FFT_CTL_SIZE);
        }
        #endif   
		memset((void *)Robot4ctl, 0, ADPCM_ROBOT4_CTL_SIZE);
		Robot4ctl->FFTSize 		= 	ROBOT4_FFT_SIZE;
		#if _ADPCM_RECORD
		if(g_playrecord) 
		{
			Robot4ctl->Samplerate 	= 	_ADPCM_RECORD_SAMPLE_RATE;  
		}
		else
		{
			Robot4ctl->Samplerate 	= 	AudioBufCtl.sample_rate;
		}
		#else
			Robot4ctl->Samplerate 	= 	AudioBufCtl.sample_rate;
		#endif
		Robot4ctl->Overlapratio 	= 	ROBOT4_OVERLAP_RATIO;
		Robot4ctl->InputSize 	= 	ADPCM_ROBOT4_DATA_LENGTH;	
		memset((void *)Robot4buf, 0, ADPCM_ROBOT4_BUF_SIZE);
		memset((void *)Robot4FFTctl, 0, ADPCM_ROBOT4_FFT_CTL_SIZE);	
		
		ROBOT_EFFECT_04_Init(Robot4ctl, Robot4buf, Robot4FFTctl);  
		
		#if _ADPCM_ROBOT4_SOUND && !(_ADPCM_SPEED_CHANGE || _ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_SPEEDPITCH_N )
		Robot04Ctl.offset=lastAudio;
		#endif
        Robot04Ctl.enable = cmd;
    }
    else
    {
#if _USE_HEAP_RAM
		if(Robot4ctl==NULL)
        {
	 		MemFree(Robot4ctl);
	 		Robot4ctl=NULL;
 		}
 		
		if(Robot4buf==NULL)
        {
	 		MemFree(Robot4buf);
	 		Robot4buf=NULL;
 		}
 		
 		if(Robot4FFTctl==NULL)
        {
	 		MemFree(Robot4FFTctl);
	 		Robot4FFTctl=NULL;
 		}
#endif          
		Robot04Ctl.enable = cmd;
    }
}
#endif

#if _ADPCM_DARTH_SOUND
//------------------------------------------------------------------//
// Init Darth Vader Effect
// Parameter: 
//          rst : reset algorithm. 1 is reset.
// return value:
//          NONE
//------------------------------------------------------------------// 
static void SetDarthSound_Init(U8 rst)
{
    float ratio;

    if(AudioBufCtl.sample_rate>SPEED_CTL_SAMPLE_RATE)
    {
        //Dprintf("sample rate out of 16K %d\r\n",AudioBufCtl.sample_rate);
        return; // sample rate out of spec
    }

    if(rst)
    {
#if _USE_HEAP_RAM  
        if(DarthVaderRam==NULL)
        {
            void *tmp_ptr;

            tmp_ptr = MemAlloc(sizeof(DARTH_VADER_RAM_T)+sizeof(DARTH_VADER_CTRL_T));
            DarthVaderRam = (DARTH_VADER_RAM_T *) tmp_ptr;
            DarthVaderCtrl = (DARTH_VADER_CTRL_T *) (tmp_ptr+sizeof(DARTH_VADER_RAM_T));
            //Dprintf("Allocate DV_RAM_addr 0x%x DV_Ctrl_addr 0x%x\r\n",(U32)DarthVaderRam,(U32)DarthVaderCtrl);
        }  
#endif          
        memset((void *)DarthVaderRam, 0, sizeof(DARTH_VADER_RAM_T));
        memset((void *)DarthVaderCtrl, 0, sizeof(DARTH_VADER_CTRL_T));
        DarthVaderRam->StepSizeTb           = StepSizeTb;
        DarthVaderRam->Cycle_number         = 50;
        DarthVaderRam->Step_cycle_number    = sizeof(StepSizeTb);    
    }
    DarthVaderRam->EffectType           = DarthCtrl.Effect; 
    ratio                               = 1.0f / PitchRatioTbl[SpeedCtl.darth_ratio_index];

    if(DarthCtrl.Effect>=0 && DarthCtrl.Effect<=2)
    {
        DarthVaderCtrl->DelayA          = 72;
        DarthVaderCtrl->DelayB          = 50;
        DarthVaderCtrl->FPShiftLevel    = 7;
        DarthVaderCtrl->FbDelayALevel   = 0.44; 
        DarthVaderCtrl->FbDelayBLevel   = 0.44;
        DarthVaderCtrl->InSumLevel      = 0.625;
    }
    else if(DarthCtrl.Effect>2)
    {
        DarthVaderCtrl->DelayA          = 0;
        DarthVaderCtrl->DelayB          = 0;
        DarthVaderCtrl->FPShiftLevel    = 7;
        DarthVaderCtrl->FbDelayALevel   = 0;
        DarthVaderCtrl->FbDelayBLevel   = 0;
        DarthVaderCtrl->InSumLevel      = 1;
    }
    DarthVader_Init(rst, ratio, DarthVaderCtrl, DarthVaderRam); // rst=1 -> reset


}
//------------------------------------------------------------------//
// Set Darth Vader Effect
// Parameter: 
//			Cmd: 1:enable, 0:disable
//          PitchIndex: -12 ~ +12
//			CarrierType: 0, 1, 2, 3
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_SetDarthSound(U8 Cmd, S8 PitchIndex, U8 CarrierType)
{
    if( (Cmd+DarthCtrl.Cmd)==DISABLE)
    {
        return;
    }
    DarthCtrl.Effect        = CarrierType;
    SpeedCtl.darth_ratio_index = PitchIndex + RATIO_INIT_INDEX;

    if(Cmd)
    {
#if _ADPCM_PITCH_CHANGE && _ADPCM_SPEED_CHANGE
        if((SpeedCtl.pitch_ratio_index-RATIO_INIT_INDEX) || (SpeedCtl.speed_ratio_index-RATIO_INIT_INDEX)) 
        {
            //Dprintf("Auto Disable Speed/Pitch\r\n");
            ADPCM_SetSpeedPitch(DISABLE,DISABLE);
        }
#else        
#if _ADPCM_PITCH_CHANGE        
        if(SpeedCtl.pitch_ratio_index-RATIO_INIT_INDEX) 
        {
            //Dprintf("Auto Disable Pitch\r\n");
            ADPCM_SetPitch(DISABLE);
        }
#endif
#if _ADPCM_SPEED_CHANGE        
        if(SpeedCtl.speed_ratio_index-RATIO_INIT_INDEX)
        {
            //Dprintf("Auto Disable Speed\r\n");
            ADPCM_SetSpeed(0);
        }
#endif        
#endif
        SetDarthSound_Init(1);

    }
    else
    {
        SpeedCtl.darth_ratio_index = RATIO_INIT_INDEX;
#if _USE_HEAP_RAM
        if(DarthVaderRam!=NULL)
        {
            //Dprintf("Darth free addr 0x%x\r\n",DarthVaderRam);
            MemFree(DarthVaderRam);
            DarthVaderRam = NULL;
            DarthVaderCtrl = NULL;
        }
#endif
    }

    DarthCtrl.Cmd           = Cmd;
    //Dprintf("SetDarth %d Effect %d Pitch %d\r\n",DarthCtrl.Cmd,DarthCtrl.Effect,SpeedCtl.darth_ratio_index);
    S8 hw_ch = DacAssigner_codecUseDacCh(kADPCM_CH1);
    if(hw_ch<0)return;//No register hw_ch
    CB_ADPCM_ChangeSR(pHwCtl[hw_ch].p_dac,AudioBufCtl.sample_rate*PitchRatioTbl[SpeedCtl.darth_ratio_index]);
}
#endif        
//------------------------------------------------------------------//
// Check effect status for select decodeframe flow
// Parameter:
//          NONE
// return value:
//          1: Run normal flow of decodeframe
//			0: Run release pre-deocoede loading flow of decodeframe
//------------------------------------------------------------------//
U8 ADPCM_DecodeSetFlow(void)
{
	U8 flow=0;

#if (_ADPCM_SPEED_CHANGE&&_ADPCM_PITCH_CHANGE) || _ADPCM_DARTH_SOUND || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR 
	flow=1;	
#else		
	flow=0;
#endif

#if _ADPCM_ANIMAL_ROAR
		if(AnimalRoarCtl.cmd)
			return 1;
#else
#if _ADPCM_DARTH_SOUND 
		if(DarthCtrl.Cmd)
			return 1;
#endif

#if _ADPCM_SPEEDPITCH_N 
		if(SpeedPitchNFlag)
			return 0;
#endif

#if _ADPCM_ROBOT1_SOUND  
#if _ADPCM_ROBOT3_SOUND
		if(RobotCtl.enable)
			return 0;	// only _ADPCM_ROBOT3_SOUND&_ADPCM_ROBOT1_SOUND Enable ,Robot1 & Robot3 can't release "start continues decodeframe" status.
#else
#if (_ADPCM_SPEED_CHANGE&&_ADPCM_PITCH_CHANGE)
		if(RobotCtl.enable)
			return 1;
#else
		if(RobotCtl.enable)
			return 0;
#endif			
#endif
#endif

#if _ADPCM_ROBOT2_SOUND 
#if (_ADPCM_SPEED_CHANGE&&_ADPCM_PITCH_CHANGE) || _ADPCM_DARTH_SOUND||_ADPCM_SPEEDPITCH_N
		if(Robot01Ctl.enable)
			return 1;
#else
		if(Robot01Ctl.enable)
			return 0;				
#endif
#endif

#if _ADPCM_ROBOT3_SOUND
#if (_ADPCM_SPEED_CHANGE&&_ADPCM_PITCH_CHANGE) || _ADPCM_DARTH_SOUND||_ADPCM_SPEEDPITCH_N
		if(Robot02Ctl.enable)
			return 0;			// only _ADPCM_DARTH_SOUND&_ADPCM_ROBOT3_SOUND Enable ,Robot3 can't release "start continues decodeframe" status.	
#else
		if(Robot02Ctl.enable)
			return 0;			// Robot3 can't release "start continues decodeframe" status.
#endif
#endif

#if _ADPCM_ROBOT4_SOUND 
#if (_ADPCM_SPEED_CHANGE&&_ADPCM_PITCH_CHANGE) || _ADPCM_DARTH_SOUND||_ADPCM_SPEEDPITCH_N
		if(Robot04Ctl.enable)
			return 1;
#else
		if(Robot04Ctl.enable)
			return 0;
#endif
#endif

#if _ADPCM_REVERB_SOUND 
#if (_ADPCM_SPEED_CHANGE&&_ADPCM_PITCH_CHANGE) || _ADPCM_DARTH_SOUND||_ADPCM_SPEEDPITCH_N 
		if(ReverbCmd)
			return 1;
#else
		if(ReverbCmd)
			return 0;	// Robot3 Enable, Reverb/Robot3/adpcm play can't release "start continues decodeframe" status.
#endif			
#endif

#if _ADPCM_ECHO_SOUND 
#if (_ADPCM_SPEED_CHANGE&&_ADPCM_PITCH_CHANGE) || _ADPCM_DARTH_SOUND||_ADPCM_SPEEDPITCH_N
		if(EchoCtrl.Cmd)
			return 1;
#else
		if(EchoCtrl.Cmd)
			return 0;	// "Only Echo Enable" or "Robot3 Enable, Echo/Robot3/adpcm play" can't release "start continues decodeframe" status.
#endif			
#endif

#endif // _ADPCM_ANIMAL_ROAR

	if(flow){
		return 1;
	}else{
		return 0;
	}
	
}
 
//------------------------------------------------------------------//
// Set audio MixCtrl
// Parameter: 
//          mixCtrl:MIX_CTRL_0 / MIX_CTRL_25 / MIX_CTRL_50 / MIX_CTRL_75 / MIX_CTRL_100
// return value:
//          NONE
//------------------------------------------------------------------// 
#if _AUDIO_MIX_CONTROL
void ADPCM_SetMixCtrl(U8 mixCtrl){
    if(mixCtrl>MIX_CTRL_100) mixCtrl=MIX_CTRL_100;
    AdpcmMixCtrl=mixCtrl;
}
#endif
//------------------------------------------------------------------//
// Codec init
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_Init(void){
    ChVol=CH_VOL_15;  
#if _AUDIO_MIX_CONTROL
    ADPCM_SetMixCtrl(MIX_CTRL_100);
#endif
#if _ADPCM_SPEED_CHANGE
	ADPCM_SetSpeed(0);
#endif
#if _ADPCM_PITCH_CHANGE
	ADPCM_SetPitch(0);
#endif
#if _ADPCM_SPEEDPITCH_N
	 SpeedCtl.speedpitch_ratio_index=0;
#endif
#if _ADPCM_DARTH_SOUND
    SpeedCtl.darth_ratio_index = RATIO_INIT_INDEX;
#endif
#if defined(_ADPCM_RECORD_PLAY_USE_CUSTOMIZE)\
	||defined(_ADPCM_PLAY_BACK_USE_CUSTOMIZE)
	ADPCM_SetEQFilter(FILTER_NUMBER);
#endif
#if _ADPCM_ECHO_SOUND
	memset((void *)&EchoCtrl, 0, sizeof(EchoCtrl));	
	memset((void *)&echoCtrl_T, 0, sizeof(echoCtrl_T));	
#endif
#if _ADPCM_RECORD && _ADPCM_RECORD_ERASING
    memset((void *)&RecordErasingCtrl,0,sizeof(RecordErasingCtrl));
#endif    
}
//------------------------------------------------------------------//
// Enable software data upsample (MUST be set before processing ADPCM_Play())
// Parameter: 
//          cmd: ENABLE, DISABLE
//          upsampleFactor: upsample factor, positive integer
// return value:
//          NONE
//------------------------------------------------------------------//
#if _ADPCM_SW_UPSAMPLE
void ADPCM_SW_UpsampleCmd(U8 cmd, U8 upsampleFactor)
{
    swUpsampleCtrl.cmd = cmd;
    swUpsampleCtrl.upsampleFactor = upsampleFactor;
}
#endif
//------------------------------------------------------------------//
// Start record voice
// Parameter: 
//          index: file index or absolute address
//          mode:SPI_MODE,EF_MODE
//          sample_rate:sample rate
//          size: record size
// return value:
//          0:ok
//          others:fail
//------------------------------------------------------------------// 
#if _ADPCM_RECORD
U8 ADPCM_Record(U32 index,U8 mode,U16 sample_rate,U32 size){
#if _ADPCM_RECORD_ERASING && _ADPCM_RECORD_SOUND_TRIG && SOUND_TRIG_RECOVER
    if (size < (SPI_SECTOR_SIZE*2))     // 8KB
        return 1;

    size = ((size / SPI_SECTOR_SIZE) - 1) * SPI_SECTOR_SIZE;    // size is changed
#endif
	AUDIO_ServiceLoop();
#if _USE_HEAP_RAM
    ADPCM_AllocateMemory(MEMORY_ALLOCATE_RECORD);
#if _ADPCM_RECORD_ERASING
	RecordErasingCtrl.record_buf = EraseRecordBuf;
#endif
#endif    
    CB_ADPCM_AdcCmd(DISABLE);
    memset((void *)&AudioBufCtl, 0, sizeof(AudioBufCtl));
    
    AudioBufCtl.sample_rate=sample_rate;
    AudioBufCtl.out_size=32;
    AudioBufCtl.in_size=21;

#if NOISE_GATE_ENABLE
    if(sample_rate<16000){
        NGInit(NG_RELEASE_SAMPLE_8K,NG_ATTACK_SAMPLE_8K,NG_ON_OFF_SAMPLE_8K);
    }else{
        NGInit(NG_RELEASE_SAMPLE_16K,NG_ATTACK_SAMPLE_16K,NG_ON_OFF_SAMPLE_16K);
    }    
#endif

#if DC_REMOVE
	memset((void *)RunFilterBuf, 0, 8<<2);
#endif

//    StorageIndex=index;
#if AGC_CTL_ENABLE    
    AgcCnt=0;
    AgcInit(ALPHA,BETA,TARGET_RMS);   
    LimiterInit(LIMITER_THRESHOLD);
#endif
    
    CB_SetStartAddr(index,mode,1,&StartAddr);
#if _ADPCM_RECORD_ERASING && _ADPCM_RECORD_SOUND_TRIG
#if SOUND_TRIG_RECOVER
    RecoverAddr = StartAddr + size;
#endif
#if _EF_SERIES
	if (mode == EF_MODE)
	{
		FMC_BurstErase_Sector(StartAddr, MAIN_FLASH_SECTORSIZE*8);		// need to refer SPI_StartSectorErase
		RecordErasingCtrl.erase_addr += (MAIN_FLASH_SECTORSIZE*(8-1));
		RecordErasingCtrl.count += (8-1);								// need to refer SPI_StartSectorErase
	}
#endif
	if (mode == SPI_MODE)
	{
    //wait erase
		while(SPI_CheckBusyFlag()) //non-busy
		{
			REprintf("Wait0 busy\r\n");
		}
		SPI_StartSectorErase(StartAddr);
	}
#if _SPI1_MODULE && _SPI1_USE_FLASH
	else if (mode == SPI1_MODE)
	{
    //wait erase
		while(SPI1_CheckBusyFlag()) //non-busy
		{
			REprintf("Wait0 busy\r\n");
		}
		SPI1_StartSectorErase(StartAddr);
	}
#endif
#endif
    CurAddr=StartAddr+HEADER_LEN;
    AudioBufCtl.samples+= HEADER_LEN;
    StorageMode=mode;
    
    AudioBufCtl.out_count=ADPCM_AUDIO_BUF_SIZE/AudioBufCtl.out_size;
    BufCtl.offset=0;   
    BufCtl.size=AudioBufCtl.out_size*AudioBufCtl.out_count;
    BufCtl.buf=AudioBuf;
    
    SetADPCMMode(AudioBufCtl.in_size,AudioBufCtl.out_size,ModeBuf,0);       
    ADPCMEncInit(EncBuf);
    
	if (mode == SPI_MODE)
    //wait erase, avoid XIP run fail
		while(SPI_CheckBusyFlag()) //non-busy
		{
			REprintf("Wait busy\r\n");
		}   
#if _SPI1_MODULE && _SPI1_USE_FLASH
	else if (mode == SPI1_MODE)
	//wait erase, avoid XIP run fail
		while(SPI1_CheckBusyFlag()) //non-busy
		{
			REprintf("Wait busy\r\n");
		} 
#endif	
    CB_ADPCM_InitAdc(sample_rate);
    
    OpMode=1;
    AudioBufCtl.state=STATUS_RECORDING;
    AudioBufCtl.shadow_state=STATUS_RECORDING;
    
    CB_ADPCM_RecStart();

	if (mode == SPI_MODE)
		while(SPI_CheckBusyFlag()) //non-busy
		{
			REprintf("Wait busy\r\n");
		}
#if _SPI1_MODULE && _SPI1_USE_FLASH
	else if (mode == SPI1_MODE)
		while(SPI1_CheckBusyFlag()) //non-busy
		{
			//dprintf("Wait0 busy\r\n");
		} 
#endif

#if _ADPCM_RECORD_ERASING
#if _ADPCM_RECORD_SOUND_TRIG
    SOUND_DETECTION_Init();
#endif
#if _EF_SERIES
	if (mode == EF_MODE)
	{
		RecordErasingCtrl.total_count = (size+(MAIN_FLASH_SECTORSIZE-1))/MAIN_FLASH_SECTORSIZE;
        GIE_DISABLE();
		extern char __load_start_instram1;
		extern char __load_size_instram1;
		memcpy((void *)0x300000, &__load_start_instram1, (size_t)&__load_size_instram1);
        GIE_ENABLE();
		intcCmdTemp = __nds32__mfsr(NDS32_SR_INT_MASK2);
		INTC_IrqCmd(~(IRQ_BIT_ADC|IRQ_BIT_RTC),DISABLE);
		GIE_DISABLE();
		P_SMU->INTV_BA = C_INTV_BA_En;
		GIE_ENABLE();
	}
#endif
	if ((mode == SPI_MODE) || (mode == SPI1_MODE))
	{
		RecordErasingCtrl.total_count = (size+(SPI_SECTOR_SIZE-1))/SPI_SECTOR_SIZE;
	}
    REprintf("t count %d\r\n",RecordErasingCtrl.total_count);
#endif
    CB_ADPCM_AdcCmd(ENABLE);
    RecordSize=size;
    

    return 0;
}
//extern const U8  VOICEFILEX[ ];
//U32 VoiceIdx=0;
//------------------------------------------------------------------//
// Adc ISR
// Parameter: 
//          size: data-buffer fill size 
//       pdest32: data-buffer address
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_AdcIsr(U8 size, U32 *pdest32){
    S16 *pdst16;
    U8 i;
    U16 temp;
    
    if(AudioBufCtl.state==STATUS_RECORDING){
      
        temp=BufCtl.size-BufCtl.offset;
        if(size>temp){
            size=temp;
        }   
#if AGC_CTL_ENABLE        
        temp=AGC_FRAME_SIZE-AgcCnt;
        if(size>temp){
            size=temp;
        }   
#endif          
        if((AudioBufCtl.fifo_count+size-AudioBufCtl.process_count)>(AudioBufCtl.out_count*AudioBufCtl.out_size)){
            for(i=0;i<size;i++){
                temp=((*pdest32++)-32768);    
            }   
            //dprintf("E\r\n");
            size=0;
        }    
        //dprintf("size,%d\r\n",size);
        AudioBufCtl.fifo_count+=size;   
        

        pdst16=(S16 *)(BufCtl.buf+BufCtl.offset);
        
        for(i=0;i<size;i++){
            temp=((*pdest32++)-32768);   
            *pdst16++=temp;
#if AGC_CTL_ENABLE            
            AgcBuf[AgcCnt+i]=temp;
#endif              
//            temp=(*pdest32-32768);     
//            *pdst16++= (VOICEFILEX[VoiceIdx]|(VOICEFILEX[VoiceIdx+1]<<8));
//            VoiceIdx+=2;
        }   
        
        BufCtl.offset+=size;
        
#if AGC_CTL_ENABLE        
        AgcCnt+=size;
        if(AgcCnt==AGC_FRAME_SIZE){
            AgcCnt=0;

            AgcProcess((S16 *)AgcBuf,AGC_FRAME_SIZE);   
        }     
#endif      
        if(BufCtl.offset==BufCtl.size){
            BufCtl.offset=0;    
        }       
    }             
}   
//------------------------------------------------------------------//
// Encode one frame
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//  
static void EncodeFrame(void)
{
    S16 *audio_buf;
    //U8 n;
    //S32 out;
     
    audio_buf=(S16 *)AudioBuf+AudioBufCtl.out_size*AudioBufCtl.index;
    
    AudioBufCtl.index++;
    if(AudioBufCtl.index==AudioBufCtl.out_count){
        AudioBufCtl.index=0;   
    }    

#if AGC_CTL_ENABLE    
    LimiterProcess(audio_buf,audio_buf,LIMITER_THRESHOLD,AudioBufCtl.out_size,0);
#endif

#if DC_REMOVE
    U8 i;
	for(i=0;i<AudioBufCtl.out_size;i++)//unit word
	{
		RunFilter(&audio_buf[i], RunFilterBuf, DCRemove);
	}
#endif

#if _ADPCM_RECORD_ERASING && _ADPCM_RECORD_SOUND_TRIG    //_SOUND_DETECT
    SOUND_DETECTION_Process(audio_buf,AudioBufCtl.out_size); //input audio_buf exceeds one out_size versus process_count
    if(AudioBufCtl.shadow_state==STATUS_STOP)
    {
        REprintf("SOUND_DETECTION stop\r\n");
        return;
    }
#endif

#if NOISE_GATE_ENABLE
    NGProcess(audio_buf,audio_buf,AudioBufCtl.out_size);
#endif       
		
    ADPCMEncode(AudioBufCtl.out_size,(U16 *)audio_buf,EncDataBuf);
    
    AudioBufCtl.process_count+=AudioBufCtl.out_size;
#if _ADPCM_RECORD_ERASING
    if(RecordErasingCtrl.status == C_RECORD_ERASE_START && (RecordErasingCtrl.waiting_channel & C_RECORD_RECORD_ADPCM))
    {
        RecordErasingCtrl.record_buf_index = 0;
        RecordErasingCtrl.record_buf = EraseRecordBuf;
        RecordErasingCtrl.waiting_channel &= ~C_RECORD_RECORD_ADPCM;
		memcpy(&RecordErasingCtrl.record_buf[RecordErasingCtrl.record_buf_index],EncDataBuf,AudioBufCtl.in_size);
        RecordErasingCtrl.record_buf_index += AudioBufCtl.in_size;
    }
    else if(RecordErasingCtrl.status == C_RECORD_ERASE_RUN)
    {
        memcpy(&RecordErasingCtrl.record_buf[RecordErasingCtrl.record_buf_index],EncDataBuf,AudioBufCtl.in_size);
        RecordErasingCtrl.record_buf_index += AudioBufCtl.in_size;
    }
#if _ADPCM_RECORD_SOUND_TRIG
	else if(RecordErasingCtrl.status == C_RECORD_ERASE_WAIT)	
	{
		memcpy(&RecordErasingCtrl.record_buf[RecordErasingCtrl.record_buf_index],EncDataBuf,AudioBufCtl.in_size);
        RecordErasingCtrl.record_buf_index += AudioBufCtl.in_size;
		if(RecordErasingCtrl.record_buf_index >= ADPCM_ERASE_RECORD_SIZE)
		{
			RecordErasingCtrl.record_buf_index = 0;
		}
		return;
	}
#endif
    else
    {
        CB_WriteData(StorageMode,&CurAddr,(U8 *)EncDataBuf,AudioBufCtl.in_size);
    }    
     
#else
    CB_WriteData(StorageMode,&CurAddr,(U8 *)EncDataBuf,AudioBufCtl.in_size);
#endif

    AudioBufCtl.samples+=AudioBufCtl.in_size;
    if((AudioBufCtl.samples+AudioBufCtl.in_size)>=RecordSize)
    {
        StopRecord();
    }

}   
//------------------------------------------------------------------//
// Record Skip Tail time setting
// Parameter: 
//          ms: Skip tail length (Unit:ms)
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_SkipTailSetting(U16 ms)
{
	SkipTailTime = ms;
}
//------------------------------------------------------------------//
// Record Skip Tail samples calculate
// Parameter: 
//          now_sample_rate: The sample rate right now.
// return value:
//          NONE
//------------------------------------------------------------------// 
U16 ADPCM_SkipTail_Samples(U16 now_sample_rate)
{
	U16 skip_samples = (now_sample_rate/1000)*SkipTailTime;//calculate tail samples
	return (skip_samples+31-((skip_samples+31)%32));
}
//------------------------------------------------------------------//
// Write audio header
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
static void WriteHeader(void){
    struct AUDIO_HEADER *pheader;

    pheader=(struct AUDIO_HEADER *)EncDataBuf;
    
    memset(pheader,0,HEADER_LEN);
    
    pheader->decode_in_size=AudioBufCtl.in_size;
    pheader->decode_out_size=AudioBufCtl.out_size;
    pheader->sample_rate=AudioBufCtl.sample_rate;
    pheader->header_size=HEADER_LEN;
    pheader->codec=CODEC_ADPCM;   
    
    memcpy(pheader->signature,Signature,sizeof(Signature));

    pheader->file_size=AudioBufCtl.samples;
 	if((AudioBufCtl.samples-HEADER_LEN)/AudioBufCtl.in_size*AudioBufCtl.out_size != AudioBufCtl.process_count)	// (exceptional condition) or (C_RECORD_ERASE_WAIT then stop)
	{
		AudioBufCtl.process_count = (AudioBufCtl.samples-HEADER_LEN)/AudioBufCtl.in_size*AudioBufCtl.out_size;
	}
	
    //pheader->samples=AudioBufCtl.process_count;
    U16 SkipTailSamples = ADPCM_SkipTail_Samples(pheader->sample_rate);
    if(AudioBufCtl.process_count > SkipTailSamples)
    {
    	pheader->samples=AudioBufCtl.process_count - SkipTailSamples;
    }
    else
    {
    	pheader->samples=32;//min frame samples
    }
    pheader->flag |= 0x1;
	
    CurAddr=StartAddr;
    CB_WriteHeader(StorageMode,&CurAddr,(U8 *)EncDataBuf,HEADER_LEN);
}  

#if _ADPCM_RECORD_ERASING
//------------------------------------------------------------------//
// Start Record Erase
// Parameter: 
//          address : start address (Must align 4k address)
//           length : erase total length
//             type : record type
//             chan : Decide which channel work together
// return value:
//           
//------------------------------------------------------------------// 
void ADPCM_GetRecordEraseCtrl(U32 *recorderasingctrl)
{
    //dprintf("input 0x%x\r\n",recorderasingctrl);
    *recorderasingctrl = (U32)&RecordErasingCtrl;
    //dprintf("active[0x%x] 0x%x out 0x%x\r\n",(RecordErasingCtrl_T *)&RecordErasingCtrl,RecordErasingCtrl.active_channel,recorderasingctrl);
}
    
//------------------------------------------------------------------//
// Start Record Erase
// Parameter: 
//          address : start address (Must align 4k address)
//           length : erase total length
//             chan : Decide which channel work together
// return value:
//           
//------------------------------------------------------------------// 
void ADPCM_StartRecordErase(U32 address, U32 length, U16 chan)
{
    memset((void *)&RecordErasingCtrl,0,sizeof(RecordErasingCtrl));
#if _ADPCM_RECORD_SOUND_TRIG
    RecordErasingCtrl.status            = C_RECORD_ERASE_WAIT;
	RecordErasingCtrl.record_buf 		= EraseRecordBuf;
	RecordErasingCtrl.record_buf_index  = 0;
#else
    RecordErasingCtrl.status            = C_RECORD_ERASE_START;
#endif
    
    RecordErasingCtrl.active_channel    |= chan;
    RecordErasingCtrl.waiting_channel   = RecordErasingCtrl.active_channel;
    RecordErasingCtrl.start_addr        = address;    
    RecordErasingCtrl.length            = length;
    RecordErasingCtrl.erase_addr        = address;

    REprintf("-------------------  RecordErasingCtrl  ---------------\r\n");
    REprintf("start_addr 0x%x\r\n",RecordErasingCtrl.start_addr);
    REprintf("length 0x%x\r\n",RecordErasingCtrl.length);
    REprintf("chan %d\r\n",RecordErasingCtrl.active_channel);
    
}
//------------------------------------------------------------------//
// Stop Record Erase
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_StopRecordErase(void)
{
    RecordErasingCtrl.status = C_RECORD_ERASE_IDLE;
}
//------------------------------------------------------------------//
// Get Status of Record Erase
// Parameter: 
//          NONE
// return value:
//          return : status
//------------------------------------------------------------------// 
U16 ADPCM_GetRecordErase(void)
{
    return  RecordErasingCtrl.status;
}
//------------------------------------------------------------------//
// Serviceloop of Record Erase
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_RecordEraseServiceloop(void)
{
    if(RecordErasingCtrl.status == C_RECORD_ERASE_RUN)
    {
        // RAM buffer is ready and Erasing is going.        
        // Check Erase done...
		U8 storageNonBusyFlag = 0;
	#if _EF_SERIES
		if (StorageMode == EF_MODE)
		{
			storageNonBusyFlag = 1;
		}
	#endif
		if (StorageMode == SPI_MODE)
		{
			if(SPI_CheckBusyFlag()==0) //non-busy
			{
				storageNonBusyFlag = 1;
			}
		}
	#if _SPI1_MODULE && _SPI1_USE_FLASH
		else if (StorageMode == SPI1_MODE)
		{
			if(SPI1_CheckBusyFlag()==0) //non-busy
			{
				storageNonBusyFlag = 1;
			}
		}		
	#endif
		
		if (storageNonBusyFlag)
        {
            RecordErasingCtrl.status = C_RECORD_ERASE_DONE;
            REprintf("(Run)Erase Done length %d\r\n",RecordErasingCtrl.record_buf_index);
		    
            // Save data to Flash From RAM
            // Record write back
            CB_WriteData(StorageMode,&CurAddr,(U8 *)RecordErasingCtrl.record_buf,RecordErasingCtrl.record_buf_index);
        }
    }
    else if(RecordErasingCtrl.status == C_RECORD_ERASE_START)
    {
        if(RecordErasingCtrl.waiting_channel==C_WHOLE_CHANNEL_COMPLETE)
        {
            RecordErasingCtrl.status = C_RECORD_ERASE_RUN;
            //GPIOC->Data ^= 0x1; 
		#if _EF_SERIES
			if (StorageMode == EF_MODE)
			{
				FMC_BurstErase_Sector(RecordErasingCtrl.erase_addr, MAIN_FLASH_SECTORSIZE);
			}
		#endif
			if (StorageMode == SPI_MODE)
			{
				SPI_StartSectorErase(RecordErasingCtrl.erase_addr);
			}
		#if _SPI1_MODULE && _SPI1_USE_FLASH
			else if (StorageMode == SPI1_MODE)
			{
				SPI1_StartSectorErase(RecordErasingCtrl.erase_addr);
			}
		#endif
            REprintf("(Start)Erase Address 0x%x\r\n",RecordErasingCtrl.erase_addr);
        }
    }
    else if(RecordErasingCtrl.status == C_RECORD_ERASE_DONE)
    {
        // waiting next sector to erase
        //dprintf("CurrAddr 0x%x\r\n",CurAddr);
		U8 eraseStorageFlag = 0;
	#if _EF_SERIES
		if (StorageMode == EF_MODE)
		{
			if (CurAddr>(RecordErasingCtrl.erase_addr+RECORD_ERASE_FMC_START_TIME))
			{
				eraseStorageFlag = 1;
			}
		}
	#endif
		if ((StorageMode == SPI_MODE) || (StorageMode == SPI1_MODE))
		{
			if (CurAddr>(RecordErasingCtrl.erase_addr+RECORD_ERASE_SPI_START_TIME))
			{
				eraseStorageFlag = 1;
			}
		}
		
		if (eraseStorageFlag)
        {     
            RecordErasingCtrl.status = C_RECORD_ERASE_START;
		#if _EF_SERIES
			if (StorageMode == EF_MODE)
			{
				RecordErasingCtrl.erase_addr += MAIN_FLASH_SECTORSIZE;
			}
		#endif
			if ((StorageMode == SPI_MODE) || (StorageMode == SPI1_MODE))
			{
				RecordErasingCtrl.erase_addr += SPI_SECTOR_SIZE;
			}
            RecordErasingCtrl.count++;
            RecordErasingCtrl.waiting_channel = RecordErasingCtrl.active_channel;
            if(RecordErasingCtrl.count == RecordErasingCtrl.total_count)
            {
                RecordErasingCtrl.status = C_RECORD_ERASE_IDLE;
                // Save data to Flash From RAM
                // Record write back
                //SPI_BurstWrite(RecordErasingCtrl.record_buf, CurAddr, RecordErasingCtrl.record_buf_index);
                //CB_WriteData(StorageMode,&CurAddr,(U8 *)RecordErasingCtrl.record_buf,RecordErasingCtrl.record_buf_index);
                // Playback...
                REprintf("Erase Idle\r\n");
            }
        }
    }
}
#if _ADPCM_RECORD_SOUND_TRIG
//------------------------------------------------------------------//
// Save Fifo count at start point
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
void ADPCM_SaveStartFifoCnt(void)
{
	if(RecordErasingCtrl.status == C_RECORD_ERASE_WAIT)		// bypass SoundTrig Timeout and Detect Sound again.
	{
		RecordErasingCtrl.status = C_RECORD_ERASE_DONE;		// ADPCM_Record() erase sector first.		
		U16 active_encode_size = ((SOUND_DETECTION_RECORD_HEAD_TIME*_ADPCM_RECORD_SAMPLE_RATE/1000+(AudioBufCtl.out_size>>1))/AudioBufCtl.out_size*AudioBufCtl.in_size);
		U16 active_check_count = active_encode_size/AudioBufCtl.in_size*AudioBufCtl.out_size;

		if((AudioBufCtl.process_count > active_check_count) && (ADPCM_ERASE_RECORD_SIZE >= active_encode_size))
		{
			AudioBufCtl.fifo_count = AudioBufCtl.fifo_count - AudioBufCtl.process_count + active_check_count;
			AudioBufCtl.process_count = active_check_count;
			AudioBufCtl.samples = HEADER_LEN + active_encode_size;
			if (RecordErasingCtrl.record_buf_index >= active_encode_size)
			{
				CB_WriteData(StorageMode,&CurAddr,(U8 *)RecordErasingCtrl.record_buf+(RecordErasingCtrl.record_buf_index-active_encode_size),active_encode_size);
			}
			else
			{
				U16 len1;//,len2;
				len1 = active_encode_size - RecordErasingCtrl.record_buf_index;
				if(len1)
				{
					CB_WriteData(StorageMode,&CurAddr,(U8 *)RecordErasingCtrl.record_buf+(ADPCM_ERASE_RECORD_SIZE-len1),len1);
				}
				//len2 = RecordErasingCtrl.record_buf_index;
				if(RecordErasingCtrl.record_buf_index)
				{
					CB_WriteData(StorageMode,&CurAddr,(U8 *)RecordErasingCtrl.record_buf,RecordErasingCtrl.record_buf_index);
				}
			}
		}
		else
		{
			AudioBufCtl.fifo_count -= AudioBufCtl.process_count;
			AudioBufCtl.process_count = 0;
		}
	}
}
//------------------------------------------------------------------//
// Save Fifo count at end point
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
void ADPCM_SaveEndFifoCnt(void)
{
    StopRecord();
}
#endif //#if _ADPCM_RECORD_SOUND_TRIG
#endif //#if _ADPCM_RECORD_ERASING
#endif //#if _ADPCM_RECORD

//------------------------------------------------------------------//
// Service loop
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
void ADPCM_ServiceLoop(void){
#if !_ADPCM_ROBOT3_SOUND && !_ADPCM_SPEED_CHANGE && !_ADPCM_PITCH_CHANGE  && !_ADPCM_SPEEDPITCH_N && !_ADPCM_DARTH_SOUND && !_ADPCM_ECHO_SOUND && !_ADPCM_ROBOT2_SOUND && !_ADPCM_REVERB_SOUND && !_ADPCM_ROBOT4_SOUND && (_ADPCM_PLAY_SR_FOR_SBC_REC==0) && !_ADPCM_ANIMAL_ROAR && !(_ADPCM_DECODE_FLOW_TYPE==TYPE1)
    U16 size;
#endif

    U8  tmp_state;

#if _ADPCM_ROBOT3_SOUND||_ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE||_ADPCM_SPEEDPITCH_N||_ADPCM_DARTH_SOUND||_ADPCM_ECHO_SOUND||_ADPCM_ROBOT2_SOUND||_ADPCM_REVERB_SOUND||_ADPCM_ROBOT4_SOUND||_ADPCM_PLAY_SR_FOR_SBC_REC||_ADPCM_ANIMAL_ROAR||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)
    U16 process_size;
#endif
    
#if _ADPCM_RECORD && _ADPCM_RECORD_ERASING
    ADPCM_RecordEraseServiceloop();
#endif
        
	if(AudioBufCtl.shadow_state==STATUS_PLAYING){

#if !(_ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE||_ADPCM_SPEEDPITCH_N||_ADPCM_DARTH_SOUND||_ADPCM_ECHO_SOUND||_ADPCM_ROBOT2_SOUND||_ADPCM_REVERB_SOUND||_ADPCM_ROBOT4_SOUND||_ADPCM_ROBOT3_SOUND||_ADPCM_PLAY_SR_FOR_SBC_REC||_ADPCM_ANIMAL_ROAR||(_ADPCM_DECODE_FLOW_TYPE==TYPE1))        
       	GIE_DISABLE();
#endif
        AudioVol=  (ChVol+1)*(MixerVol+1); 
        
        if(!ChVol){
            AudioVol=0;
        }    
        if(AudioVol==256){
            AudioVol=255;
        }    
        AudioVol=AudioVol<<7;
#if !(_ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE||_ADPCM_SPEEDPITCH_N||_ADPCM_DARTH_SOUND||_ADPCM_ECHO_SOUND||_ADPCM_ROBOT2_SOUND||_ADPCM_REVERB_SOUND|| _ADPCM_ROBOT4_SOUND||_ADPCM_ROBOT3_SOUND||_ADPCM_PLAY_SR_FOR_SBC_REC||_ADPCM_ANIMAL_ROAR||(_ADPCM_DECODE_FLOW_TYPE==TYPE1))
		GIE_ENABLE();
#endif
#if _AUDIO_VOLUME_CHANGE_SMOOTH
        AUDIO_SetVolumeSmooth(&VolCtrl,AudioVol);
#endif

#if     _ADPCM_ROBOT3_SOUND
        if(Robot02Ctl.enable)
        {
        	process_size=360;
        }
        else
        {
        
#if (_ADPCM_SPEED_CHANGE&&_ADPCM_PITCH_CHANGE) 
        	if(SpeedPitchFlag)
        	{
        		if(AudioBufCtl.frame_size)
	        	{
        			process_size=ADPCM_AUDIO_OUT_SIZE<<1;
	        	}
	        	else
	        	{
	        		process_size=ADPCM_AUDIO_OUT_SIZE;
	        	}
        	}
        	else
#endif
        	{
#if _ADPCM_SPEEDPITCH_N              
                if(SpeedPitchNFlag)
        	    {
        		    process_size=ADPCM_AUDIO_OUT_SIZE<<1;
        	    }
                else
#endif
                {
        		    process_size=ADPCM_AUDIO_OUT_SIZE;
                }
        	}
        }
    
		if(ADPCM_DecodeSetFlow())
        {
	        if(!AudioBufCtl.fifo_count_tmp)
	        {
	        	AudioBufCtl.fifo_count_tmp=process_size;
	        }
			if(((AudioBufCtl.fifo_count+(ADPCM_AUDIO_BUF_SIZE-process_size)-AudioBufCtl.process_count)>=0)&&(AudioBufCtl.fifo_count>0)&&((AudioBufCtl.fifo_count>AudioBufCtl.fifo_count_tmp)))
	        {
	        	AudioBufCtl.fifo_count_tmp+=process_size;
	            DecodeFrame();
	        }
        }
        else
        {
        	if((AudioBufCtl.fifo_count+(ADPCM_AUDIO_BUF_SIZE-process_size)-AudioBufCtl.process_count)>=0)
			{
            	DecodeFrame();
        	}
        }    
        
#elif  _ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE||_ADPCM_DARTH_SOUND||_ADPCM_ECHO_SOUND||_ADPCM_ROBOT2_SOUND||_ADPCM_ROBOT4_SOUND||_ADPCM_REVERB_SOUND||_ADPCM_PLAY_SR_FOR_SBC_REC||_ADPCM_SPEEDPITCH_N|| _ADPCM_ANIMAL_ROAR||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)
#if (_ADPCM_SPEED_CHANGE&&_ADPCM_PITCH_CHANGE) || _ADPCM_ANIMAL_ROAR
#if (_ADPCM_SPEED_CHANGE&&_ADPCM_PITCH_CHANGE)
        if(SpeedPitchFlag)
#elif _ADPCM_ANIMAL_ROAR
        if (AnimalRoarCtl.cmd)
#endif
        {
        	if(AudioBufCtl.frame_size)
        	{
        		process_size=ADPCM_AUDIO_OUT_SIZE<<1;
        	}
        	else
        	{
        		process_size=ADPCM_AUDIO_OUT_SIZE;
        	}
        }
        else
#endif
        {
#if _ADPCM_SPEEDPITCH_N              
            if(SpeedPitchNFlag)
        	{
        	    process_size=ADPCM_AUDIO_OUT_SIZE<<1;
        	}
            else
#endif      
            {      
        	    process_size=ADPCM_AUDIO_OUT_SIZE;
            }
        }

		if(ADPCM_DecodeSetFlow())
        {
	        if(!AudioBufCtl.fifo_count_tmp)
	        {
	        	AudioBufCtl.fifo_count_tmp=process_size;
	        }
	        if(((AudioBufCtl.fifo_count+(ADPCM_AUDIO_BUF_SIZE-process_size)-AudioBufCtl.process_count)>=0)&&(AudioBufCtl.fifo_count>0)&&((AudioBufCtl.fifo_count>AudioBufCtl.fifo_count_tmp)))
	        {
	        	AudioBufCtl.fifo_count_tmp+=process_size;
	            DecodeFrame();
	        }
        }
        else
        {
        	if((AudioBufCtl.fifo_count+(ADPCM_AUDIO_BUF_SIZE-process_size)-AudioBufCtl.process_count)>=0)
			{
            	DecodeFrame();
        	}
        }           
#else

        U16 bufctl_rd_offset = BufCtl.rd_offset;
        if(bufctl_rd_offset!=BufCtl.wr_offset)
        {
            if(bufctl_rd_offset>BufCtl.wr_offset)
            {
                size=bufctl_rd_offset-BufCtl.wr_offset;    
            }
            else
            {
                size=ADPCM_DATA_BUF_SIZE-BufCtl.wr_offset;  
            }     
            #if PR_READ	
            if((size>=ADPCM_DATA_BUF_BASE_SIZE)||(bufctl_rd_offset<BufCtl.wr_offset))
            {
            #endif
            	CB_ReadData(StorageMode,&CurAddr,EncDataBuf+BufCtl.wr_offset,size);    
            
			#if _ADPCM_PLAY_REPEAT	
				if((CurAddr-(StartAddr+HEADER_LEN)) >= AudioBufCtl.encode_size)
				{
					size -= ((CurAddr-(StartAddr+HEADER_LEN)) - AudioBufCtl.encode_size);
					CurAddr = StartAddr+HEADER_LEN;
				#if _ADPCM_QIO_MODULE
					QIO_ReGetStorageDataSet(QIO_USE_ADPCM_CH1);
				#endif
				}
			#endif	
	            BufCtl.wr_offset+=size; 
	            if(BufCtl.wr_offset==ADPCM_DATA_BUF_SIZE)
	            {
	                BufCtl.wr_offset=0;                
            	}  
            #if PR_READ	
            }
            #endif
         }
        
             
#endif
    #if _ADPCM_RECORD && SMOOTH_ENABLE
        PlayEndCheck();
    #endif
    }

#if _ADPCM_RECORD    
    if(AudioBufCtl.state==STATUS_RECORDING){
        if((AudioBufCtl.fifo_count-AudioBufCtl.process_count)>=AudioBufCtl.out_size){
            //dprintf("%d\r\n",VoiceIdx);
            EncodeFrame();
        }          
    }   
#endif


    tmp_state = AudioBufCtl.shadow_state;

	
#if _USE_HEAP_RAM     
#if _ADPCM_RECORD && (!(_ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)))
    if((tmp_state==STATUS_STOP) && (AudioBufCtl.state!=STATUS_RECORDING) && EncDataBuf!=NULL )
#elif _ADPCM_RECORD && (_ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR||(_ADPCM_DECODE_FLOW_TYPE==TYPE1))
    if((tmp_state==STATUS_STOP) && (AudioBufCtl.state!=STATUS_RECORDING) && AudioBuf!=NULL )
#elif (!(_ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)))
    if(tmp_state==STATUS_STOP && EncDataBuf!=NULL)
#elif _ADPCM_SPEED_CHANGE||_ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ECHO_SOUND || _ADPCM_ROBOT2_SOUND || _ADPCM_ROBOT3_SOUND || _ADPCM_ROBOT4_SOUND || _ADPCM_REVERB_SOUND || _ADPCM_PLAY_SR_FOR_SBC_REC || _ADPCM_SPEEDPITCH_N || _ADPCM_ANIMAL_ROAR||(_ADPCM_DECODE_FLOW_TYPE==TYPE1)
    if(tmp_state==STATUS_STOP && AudioBuf!=NULL)
#else
    if(tmp_state==STATUS_STOP)        
#endif
    {
        ADPCM_FreeMemory(MEMORY_ALLOCATE_PLAY);
    }
#endif //_USE_HEAP_RAM
    if(     tmp_state!=STATUS_PLAYING
        &&  DacAssigner_codecUseDacCh(kADPCM_CH1)>=0
    ){
		GIE_DISABLE();
		DacAssigner_unRegist(kADPCM_CH1);
		GIE_ENABLE();
    }
	if (PlayEndFlag == BIT_SET) {
		PlayEndFlag = BIT_UNSET;
		CB_ADPCM_PlayEnd();
	}
}    
#endif//_ADPCM_PLAY
