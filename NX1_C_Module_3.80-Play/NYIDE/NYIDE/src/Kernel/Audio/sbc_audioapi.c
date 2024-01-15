/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "include.h"
#include "nx1_dac.h"
#include "nx1_spi.h"
#include "nx1_fmc.h"
#include "nx1_intc.h"
//#include "nx1_gpio.h"
#include "debug.h"
#include "user_storage_break_point.h"

#if _SBC_PLAY||_SBC_RECORD

#if _EF_SERIES && _SBC_RECORD && _SBC_RECORD_ERASING
#if OPTION_IRAM
#define AudioBufCtl 			AudioBufCtl_SBC
#define BufCtl					BufCtl_SBC
#else
//#error "If you need to use _SBC_RECORD_ERASING, Config Block Setting IRAM should be set as ENABLE"
#endif
extern U32 intcCmdTemp;
#endif
/* Defines -------------------------------------------------------------------*/
//#if _SBC_SPEED_CHANGE||_SBC_PITCH_CHANGE
//#define AUDIO_OUT_SIZE     SPEED_CTL_OUT_SIZE
//#define AUDIO_OUT_COUNT    6
//#define AUDIO_BUF_SIZE     (AUDIO_OUT_SIZE*AUDIO_OUT_COUNT)
//static U16 AudioBuf[AUDIO_BUF_SIZE];
//#else

//#endif
#define Dprintf(...)
//#define Dprintf     dprintf
#define AUTO_SAVE_PERIOD        ((_SBC_SECTOR_ERASE_MAXTIME > _SBC_AUTO_SAVE_PERIOD)? _SBC_SECTOR_ERASE_MAXTIME : _SBC_AUTO_SAVE_PERIOD)
#define AUTO_SAVE_COUNT         (_SBC_AUTO_SAVE_SR * AUTO_SAVE_PERIOD/1000)


#define SMOOTH_ENABLE          1

#define AGC_CTL_ENABLE         0


#define TARGET_RMS     			MINUS_5DB                           // Adjustable, Target RMS signal magnitude
#define ALPHA           		4096*4//4096//4096                  // 0.125; coeff. of 1st order IIR LPF
#define BETA            		1024//(2048/2)                      // 0.125; step size constant
#define FULL_SCALE_LIMIT 		33000//45000//26000//32000
#define DIGITAL_GAIN_VAD_TH     1

#define NOISE_GATE_ENABLE   	1

#define LIMITER_THRESHOLD       26000
#define RECORD_ERASE_SPI_START_TIME_RSV		512
#define RECORD_ERASE_SPI_START_TIME 		(SPI_SECTOR_SIZE-RECORD_ERASE_SPI_START_TIME_RSV)
#define RECORD_ERASE_FMC_START_TIME_RSV		(40*3)
#define RECORD_ERASE_FMC_START_TIME 		(MAIN_FLASH_SECTORSIZE-RECORD_ERASE_FMC_START_TIME_RSV)

#define DC_REMOVE				1

#define NG_ON_OFF_SAMPLE        160//10ms
#define NG_ATTACK_SAMPLE        80//5ms
#define NG_RELEASE_SAMPLE       (320*5)//100ms

#if		_SBC_RECORD==ENABLE
	#if		_SBC_RECORD_PLAY_EQ==ENABLE
		#if		_SBC_RECORD_PLAY_TYPE==TYPE1//DRC force EQ2
			#define _SBC_RECORD_PLAY_USE_EQ2//define EQ2 and DRC
			#define _SBC_RECORD_PLAY_USE_DRC
		#elif		_SBC_EQ_TYPE==TYPE0
            #define _SBC_RECORD_PLAY_USE_EQ0//define EQ0
		#elif	_SBC_EQ_TYPE==TYPE1
            #define _SBC_RECORD_PLAY_USE_EQ1//define EQ1
		#elif	_SBC_EQ_TYPE==CUSTOMIZE//customize EQ
			#include "FilterTab.h"
			#if	FILTER_NUMBER<=0//check table size
				#error "Filter table is not existed, please update NYIDE and create filter file."
			#endif
			#define _SBC_RECORD_PLAY_USE_CUSTOMIZE//define EQ customize
		#endif
	#endif
	#if		_SBC_RECORD_PLAY_TYPE==TYPE2
		#define _SBC_RECORD_PLAY_USE_DIGITAL_GAIN//define DigiGain
	#endif
#endif

#if		_SBC_PLAY_BACK_EQ==ENABLE
	#include "FilterTab.h"
	#if	FILTER_NUMBER<=0//check table size
		#error "Filter table is not existed, please update NYIDE and create filter file."
	#endif
	#define _SBC_PLAY_BACK_USE_CUSTOMIZE//define EQ customize
#endif
S_AUDIO_FUNC_IDX s_SBC_FUNC = {
    .Play           =   SBC_Play,
    .Pause          =   SBC_Pause,
    .Resume         =   SBC_Resume,
    .Stop           =   SBC_Stop,
    .SetVolume      =   SBC_SetVolume,
    .GetStatus      =   SBC_GetStatus,
#if defined(_SBC_RECORD_PLAY_USE_CUSTOMIZE)   \
    || defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
    .SetEQFilter    =   SBC_SetEQFilter,
#endif
#if _SBC_SPEED_CHANGE
    .SetSpeed       =   SBC_SetSpeed,
#endif
#if _SBC_PITCH_CHANGE
    .SetPitch       =   SBC_SetPitch,
#endif
#if _SBC_ROBOT1_SOUND
    .SetRobot1Sound =   SBC_SetRobot1Sound,
#endif
#if _SBC_ROBOT2_SOUND
    .SetRobot2Sound =   SBC_SetRobot2Sound,
#endif
#if _SBC_ROBOT3_SOUND
    .SetRobot3Sound =   SBC_SetRobot3Sound,
#endif
#if _SBC_ROBOT4_SOUND
    .SetRobot4Sound =   SBC_SetRobot4Sound,
#endif
#if _SBC_REVERB_SOUND
    .SetReverbSound =   SBC_SetReverbSound,
#endif
#if _SBC_ECHO_SOUND
    .SetEchoSound   =   SBC_SetEchoSound,
#endif
#if _AUDIO_MIX_CONTROL
    .SetMixCtrl     =   SBC_SetMixCtrl,
#endif
};
/* Static Functions ----------------------------------------------------------*/
#if _SBC_RECORD
static void WriteHeader(void);
#endif

/* Constant Table ------------------------------------------------------------*/
#if _SBC_SPEED_CHANGE
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
    1.5,
    1.58,
    1.67,
    1.75,
    1.83,
    1.92,
    2,
};
#endif

#if _SBC_PITCH_CHANGE
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
#if _SBC_ROBOT1_SOUND
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
#if _SBC_ECHO_SOUND
static const U8 EchoCoef[3][8] = {
    {  4,  4,  5,  5,  6,  6,  7,  7 },
    { 30, 35, 40, 43, 47, 52, 55, 60 },
    {  9, 12, 15, 17, 22, 25, 27, 29 }};
#endif

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

#if defined(_SBC_RECORD_PLAY_USE_DIGITAL_GAIN)
static const S16 SBCDigiAmpArray[14] = {
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
static const U16 SBCVADArray[9] = {

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
	5,              //aspirated voice keep level  3 ~ 5;
};
static const short SBCDigiAmpSet[2] = {

	32,     //frame size
	0,       //sample base or frame base, value 0 or 1
};
#endif

/* Global Variables ----------------------------------------------------------*/
#if _AUDIO_MIX_CONTROL
U8 SbcMixCtrl=MIX_CTRL_100;
#endif

/* Static Variables ----------------------------------------------------------*/
SCOPE_TYPE volatile struct AUDIO_BUF_CTL{
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
}AudioBufCtl;
#if	_SBC_RECORD_ERASING
static RecordErasingCtrl_T RecordErasingCtrl;
#endif

static U16 AudioVol;
static U8  OpMode,ChVol;

#if _SBC_RECORD

#if DC_REMOVE
static S32 RunFilterBuf[8];
#endif

#if AGC_CTL_ENABLE
static U8 AgcCnt;
#endif

static U32 g_playrecord;
static U32 RecordSize;
static U16 SkipTailTime = 0;
#endif

#if _SBC_AUTO_SAVE_TIME
static U32 autosave_cnt;
#endif

SCOPE_TYPE volatile struct{
    U16 *buf;
    U16 size;
    U16 offset;
}BufCtl;

static const U8 Signature[]="NX1";

#if SMOOTH_ENABLE
static volatile struct{
    S16 count;              //sample count in process
    U8  step_sample;        //sample size for each step
    U8  smooth;             //up or down or none
    U8  state;              //current state
}SmoothCtl;
#endif

#if _SBC_SPEED_CHANGE||_SBC_PITCH_CHANGE
static struct{
    U16 index;
    U8 speed_ratio_index;            //speed control ratio
    U8 pitch_ratio_index;            //pitch control ratio
}SpeedCtl;


#endif


#if _SBC_ROBOT1_SOUND
static struct{
    U8 enable;
    U8 change;
    U8 index;
}RobotCtl;
#endif

#if _SBC_ROBOT2_SOUND
static struct{
    U8 enable;
    U8 type;
    U16 thres;
    U16 detectthres;
}Robot01Ctl;
#endif
#if _SBC_ROBOT3_SOUND
static struct{
    U8 enable;
    U8 type;
    U16 thres;
}Robot02Ctl;
#endif

#if _SBC_ROBOT4_SOUND
static struct{
    U8 enable;
}Robot04Ctl;
#endif

#if _SBC_AUTO_SAVE_TIME//_SBC_SEEK
static struct{
    U32 CurAddr;
    U32 process_count;
    U16 rand[2];
    U8  cmd;
}SbcSeekCtl;
#endif

#if _SBC_AUTO_SAVE_TIME
static struct{
    U32 cur_addr;
    U8  *buf;
    U16 rd;
    U8  state;
}SbcRamEBufCtrl;
#endif

#if _SBC_REVERB_SOUND
static U8 ReverbCmd,ReverbEffect;
#endif

#if _SBC_ECHO_SOUND
static EchoCtl_T EchoCtl;
static S8 EchoCmd;
#endif

static U8  StorageMode;
static U32 StartAddr,CurAddr;

static U8  PlayEndFlag;

#if _SBC_AUDIO_BUF_X3
U8 SBC_PlayRepeatFlag;
#endif

#if defined(_SBC_RECORD_PLAY_USE_DIGITAL_GAIN)
static S16 SBCDigiAmpBuf[36];
static S16 SBCDigiAmpCTLBuf[34];
#endif

#if _AUDIO_VOLUME_CHANGE_SMOOTH
static volatile VOLCTRL_T    VolCtrl;
#endif

#if defined(_SBC_RECORD_PLAY_USE_DRC)
static S8 DRC_buf[14] = { 0 };
static volatile DRCFixedCtl drcFixedCtl;
#endif

#if defined(_SBC_RECORD_PLAY_USE_EQ0)			\
	||defined(_SBC_RECORD_PLAY_USE_EQ1)		    \
	||defined(_SBC_RECORD_PLAY_USE_EQ2)	    	\
	||defined(_SBC_RECORD_PLAY_USE_CUSTOMIZE)	\
	||defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
static EQCtl EQFilterCtl;
#endif
#if defined(_SBC_RECORD_PLAY_USE_CUSTOMIZE)	\
	||defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
static U8	u8EQFilterIndex=0;
#endif

/* Dynamic Allocate RAM-------------------------------------------------------*/
#if _USE_HEAP_RAM
static U16 *SBCRam=NULL;
static U16 *AudioBuf=NULL;
#if _SBC_SPEED_CHANGE||_SBC_PITCH_CHANGE
static U16 *SpeedCtlBuf=NULL;
#endif
#if _SBC_ECHO_SOUND
static S16 *EchoBuf=NULL;
#endif

#if _SBC_ROBOT2_SOUND
static S16 *Robot01_buf=NULL;
#endif

#if _SBC_ROBOT3_SOUND
static S16 *Robot02_buf=NULL;
#endif

#if _SBC_ROBOT4_SOUND
ROBOT4Ctl* SBCRobot4ctl = NULL;
ROBOT4Buf* SBCRobot4buf = NULL;
FFTctl* SBCRobot4FFTctl = NULL;
#endif

#if _SBC_REVERB_SOUND
static S16 *Reverb_x=NULL;
static S16 *Reverb_y=NULL;
#endif

#if _SBC_RECORD
static U8  *DataBuf=NULL;
#if _SBC_RECORD_ERASING
static U8 *EraseRecordBuf=NULL;
#endif

#if AGC_CTL_ENABLE
static U16 *AgcBuf=NULL;
#endif
#endif

#if defined(_SBC_RECORD_PLAY_USE_EQ0)			\
	||defined(_SBC_RECORD_PLAY_USE_EQ1)		    \
	||defined(_SBC_RECORD_PLAY_USE_EQ2)		    \
	||defined(_SBC_RECORD_PLAY_USE_CUSTOMIZE)	\
	||defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
static S32 *EqBuf=NULL;
#endif

#if _SBC_AUTO_SAVE_TIME
static U8 *SbcRamEBuf=NULL;
#endif

#else
static U16 SBCRam[SBC_RAM_ALLOCATE_SIZE];
static U16 AudioBuf[SBC_AUDIO_BUF_SIZE];
#if _SBC_SPEED_CHANGE||_SBC_PITCH_CHANGE
static U16 SpeedCtlBuf[SBC_DEC_OUT_SIZE+SPEED_CTL_FRAME_SIZE];
#endif
#if _SBC_ECHO_SOUND
static S16 EchoBuf[SBC_ECHO_BUF_SIZE];
#endif
#if _SBC_ROBOT2_SOUND
static S16 Robot01_buf[SBC_ROBOT_BUF_SIZE];
#endif

#if _SBC_ROBOT3_SOUND
static S16 Robot02_buf[SBC_ROBOT02_BUF_SIZE];
#endif

#if _SBC_ROBOT4_SOUND
ROBOT4Ctl  _SBCRobot4ctl;
ROBOT4Ctl* SBCRobot4ctl = &_SBCRobot4ctl;
ROBOT4Buf _SBCRobot4buf;
ROBOT4Buf* SBCRobot4buf = &_SBCRobot4buf;
FFTctl  _SBCRobot4FFTctl;
FFTctl* SBCRobot4FFTctl = &_SBCRobot4FFTctl;
#endif

#if _SBC_REVERB_SOUND
static S16 Reverb_x[SBC_REVERB_BUF_SIZE];
static S16 Reverb_y[SBC_REVERB_BUF_SIZE];
#endif

#if _SBC_RECORD
static U8 DataBuf[SBC_DEC_IN_SIZE];
#if _SBC_RECORD_ERASING
static U8 EraseRecordBuf[SBC_ERASE_RECORD_SIZE];
#endif

#if AGC_CTL_ENABLE
static U16 AgcBuf[AGC_FRAME_SIZE];
#endif
#endif

#if defined(_SBC_RECORD_PLAY_USE_EQ0)			\
	||defined(_SBC_RECORD_PLAY_USE_EQ1)		    \
	||defined(_SBC_RECORD_PLAY_USE_EQ2)		    \
	||defined(_SBC_RECORD_PLAY_USE_CUSTOMIZE)	\
	||defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
static S32 EqBuf[SBC_EQ_BUF_SIZE];
#endif

#if _SBC_AUTO_SAVE_TIME
static U8 SbcRamEBuf[SBC_RAM_ERASE_BUF_SIZE];
#endif

#endif

#if _SBC_AUDIO_BUF_X3
void RepeatReSetting(U8 fifoSize, U8 size);
#endif


/* Extern Functions ----------------------------------------------------------*/
extern void CB_ReadData(U8 mode, U32 *cur_addr, U8 *buf,U16 size);
extern void CB_Seek(U32 offset);
extern void CB_WriteHeader(U8 mode, U32 *cur_addr,U8 *buf,U16 size);
extern U8   CB_SetStartAddr(U32 addr,U8 mode,U8 rw,U32 *start_addr);
extern void CB_WriteData(U8 mode, U32 *cur_addr,U8 *buf,U16 size);
extern void CB_SBC_PlayStart(void);
extern void CB_SBC_PlayEnd(void);
extern void CB_SBC_RecStart(void);
extern void CB_SBC_RecEnd(void);
extern void CB_SBC_InitDac(CH_TypeDef *chx,U16 sample_rate,U8 upsample);
extern void CB_SBC_DacCmd(CH_TypeDef *chx,U8 cmd);
extern void CB_SBC_ChangeSR(CH_TypeDef *chx,U16 sample_rate);
extern void CB_SBC_InitAdc(U16 sample_rate);
extern void CB_SBC_AdcCmd(U8 cmd);
extern void CB_SBC_WriteDac(U8 size,U16* psrc16);
extern void DigitalGainInit(S16 *max);
extern void DigitalGainProcess(S16 *in,U16 samples,S16 *max);

#if defined(_SBC_RECORD_PLAY_USE_DIGITAL_GAIN)
extern void RTDigiAmpProcess(S16 *in, S16 *buf, S16 samples, short *RTAgcBuf);
extern void RTDigiAmpInit(const short *RT_AGC_Table,const  short *RT_AGC_Table_1, S16 *tempbuf, short *RT_AGCBuf);
extern void RTVadInit(const U16* RTVadTable, short* RTVadBuf);
#endif

//------------------------------------------------------------------//
// Allocate SBC memory
// Parameter:
//          func: Choose Allocate function
// return value:
//          NONE
//------------------------------------------------------------------//
#if _USE_HEAP_RAM
static void SBC_AllocateMemory(U16 func)
{
    if(SBCRam==NULL && (func&MEMORY_ALLOCATE_PLAY))    //SBCRam & AudioBuf & SpeedCtlBuf together
    {
        SBCRam   = (U16 *)MemAlloc(SBC_RAM_ALLOCATE_SIZE<<1);
        AudioBuf = (U16 *)MemAlloc(SBC_AUDIO_BUF_SIZE<<1);
#if _SBC_AUTO_SAVE_TIME
        SbcRamEBuf = (U8 *)MemAlloc(SBC_RAM_ERASE_BUF_SIZE);
        SbcRamEBufCtrl.buf = SbcRamEBuf;
        Dprintf("Allo SbcRamEBuf 0x%x\r\n",SbcRamEBuf);
#endif
#if _SBC_SPEED_CHANGE||_SBC_PITCH_CHANGE
        SpeedCtlBuf = MemAlloc((SBC_DEC_OUT_SIZE+SPEED_CTL_FRAME_SIZE)<<1);
#endif
        //dprintf("SBC Allocate SBCRam %x AudioBuf %x\r\n",SBCRam,AudioBuf);
    }
    //else
    //    dprintf("SBC Allocate twice. Ignore...\r\n");

#if _SBC_RECORD
    if(DataBuf==NULL && (func&MEMORY_ALLOCATE_RECORD))   //DataBuf & AgcBuf together
    {
        DataBuf = MemAlloc(SBC_DEC_IN_SIZE);
        SBCRam   = (U16 *)MemAlloc(SBC_RAM_ALLOCATE_SIZE<<1);
        AudioBuf = (U16 *)MemAlloc(SBC_AUDIO_BUF_SIZE<<1);
#if _SBC_RECORD_ERASING
		EraseRecordBuf = (U8 *)MemAlloc(SBC_ERASE_RECORD_SIZE);
#endif
#if AGC_CTL_ENABLE
        AgcBuf = MemAlloc(AGC_FRAME_SIZE<<1);
#endif
    }
    //else
    //    dprintf("SBC Allocate twice or dont allocate REC. Ignore...\r\n");
#endif

}
//------------------------------------------------------------------//
// Free SBC memory
// Parameter:
//          func: Choose Allocate function
// return value:
//          NONE
//------------------------------------------------------------------//
static void SBC_FreeMemory(U16 func)
{
    if(SBCRam!=NULL && (func&MEMORY_ALLOCATE_PLAY))
    {
        MemFree(SBCRam);
        SBCRam=NULL;
        MemFree(AudioBuf);
        AudioBuf=NULL;
#if _SBC_SPEED_CHANGE||_SBC_PITCH_CHANGE
        MemFree(SpeedCtlBuf);
        SpeedCtlBuf=NULL;
#endif
#if _SBC_AUTO_SAVE_TIME
		if(SbcRamEBuf!=NULL)
		{
            Dprintf("Free SbcRamEBuf\r\n");
       		MemFree(SbcRamEBuf);
        	SbcRamEBuf = NULL;
        }
#endif
#if _SBC_ROBOT2_SOUND
        if(Robot01_buf!=NULL)
        {
            MemFree(Robot01_buf);
            Robot01_buf = NULL;
        }
#endif
#if _SBC_ROBOT3_SOUND
        if(Robot02_buf!=NULL)
        {
            MemFree(Robot02_buf);
            Robot02_buf = NULL;
        }
#endif


#if _SBC_ROBOT4_SOUND
        if(SBCRobot4ctl!=NULL)
        {
            MemFree(SBCRobot4ctl);
            SBCRobot4ctl=NULL;
        }

        if(SBCRobot4buf!=NULL)
        {
            MemFree(SBCRobot4buf);
            SBCRobot4buf=NULL;
        }

        if(SBCRobot4FFTctl!=NULL)
        {
            MemFree(SBCRobot4FFTctl);
            SBCRobot4FFTctl=NULL;
        }

#endif

#if _SBC_REVERB_SOUND
        if(Reverb_x!=NULL)
        {
            MemFree(Reverb_x);
            Reverb_x = NULL;
            MemFree(Reverb_y);
            Reverb_y = NULL;
        }
#endif
#if _SBC_ECHO_SOUND
        if(EchoBuf!=NULL)
        {
            MemFree(EchoBuf);
            EchoBuf=NULL;
        }
#endif
#if defined(_SBC_RECORD_PLAY_USE_EQ0)		    \
	||defined(_SBC_RECORD_PLAY_USE_EQ1)		    \
	||defined(_SBC_RECORD_PLAY_USE_EQ2)		    \
	||defined(_SBC_RECORD_PLAY_USE_CUSTOMIZE)	\
	||defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
        if(EqBuf!=NULL)
        {
            MemFree(EqBuf);
            EqBuf=NULL;
            EQFilterCtl.Buf=NULL;
        }
#endif
        //dprintf("SBC Free Mem\r\n");
    }
    //else
    //{
    //    dprintf("SBC Already Free\r\n");
    //}
#if _SBC_RECORD
    if(DataBuf!=NULL && (func&MEMORY_ALLOCATE_RECORD))   //DataBuf & AgcBuf together
    {
        MemFree(DataBuf);
        DataBuf=NULL;
        MemFree(SBCRam);
        SBCRam=NULL;
        MemFree(AudioBuf);
        AudioBuf=NULL;
#if _SBC_RECORD_ERASING
		MemFree(EraseRecordBuf);
		EraseRecordBuf=NULL;
#endif
#if AGC_CTL_ENABLE
        MemFree(AgcBuf);
        AgcBuf=NULL;
#endif
        //dprintf("SBC Free Mem REC\r\n");
    }
    //else
    //    dprintf("SBC Already Free REC\r\n");
#endif
}
#endif

//------------------------------------------------------------------//
// ISR
// Parameter:
//          size: fifo fill size
// return value:
//          NONE
//------------------------------------------------------------------//
void SBC_DacIsr(U8 size){
    U16 temp;
#if	_SBC_QIO_MODULE
	U16 _QioFrameFlag;
#endif//_SBC_QIO_MODULE
    S16 *psrc16,gain;
    U8 i,len,size2;
#if _SBC_AUDIO_BUF_X3
	U8 fifoSize = size;
	U8 lastFifoFlag = 0;
#endif

	if(AudioBufCtl.shadow_state==STATUS_PLAYING){
#if _SBC_QIO_MODULE
		QIO_Resume(QIO_USE_SBC_CH1);
#endif//_SBC_QIO_MODULE

        temp=BufCtl.size-BufCtl.offset;
        if(size>temp){
            size=temp;
        }
#if _SBC_QIO_MODULE
#if _SBC_AUDIO_BUF_X3
		U32 _QioDiffSamples = AudioBufCtl.samples-AudioBufCtl.fifo_count-1;
		for(i=0;i<fifoSize;i++){
			QIO_FrameFlag_FIFO_BitGet(QIO_USE_SBC_CH1, (U8*)&_QioFrameFlag);
			QIO_DacIsr(QIO_USE_SBC_CH1, (U8)_QioFrameFlag);
			if(i == _QioDiffSamples){
				QIO_FrameFlag_FIFO_RstGet(QIO_USE_SBC_CH1);
			}
		}
#else
		for(i=0;i<size;i++){
			QIO_FrameFlag_FIFO_BitGet(QIO_USE_SBC_CH1, (U8*)&_QioFrameFlag);
			QIO_DacIsr(QIO_USE_SBC_CH1, (U8)_QioFrameFlag);
		}
#endif
#endif//_SBC_QIO_MODULE
	#if _SBC_AUDIO_BUF_X3
		if((AudioBufCtl.fifo_count+size)>AudioBufCtl.samples){
			size=AudioBufCtl.samples-AudioBufCtl.fifo_count;
			lastFifoFlag = 1;
		}
	#endif
        //dprintf("size,%d\r\n",size);
        AudioBufCtl.fifo_count+=size;

#if SMOOTH_ENABLE
        //smooth process
        if(SmoothCtl.smooth!=SMOOTH_NONE){
            size2= size;
            psrc16=(S16 *)(BufCtl.buf+BufCtl.offset);

            while(size2){
                if(SmoothCtl.state==SMOOTH_STEP){
                    if(SmoothCtl.smooth==SMOOTH_DOWN){
                        for(i=0;i<size2;i++){
                            *psrc16++=0;
                        }
#if _SBC_QIO_MODULE
						if(AudioBufCtl.state == STATUS_STOP)
						{
							QIO_DelayStop(QIO_USE_SBC_CH1,ENABLE);
						}
						else
						{
							QIO_Pause(QIO_USE_SBC_CH1);
						}
#endif//_SBC_QIO_MODULE
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
        psrc16=(S16 *)(BufCtl.buf+BufCtl.offset);

//        for(i=0;i<size;i++){
//            *pdest32=*psrc16;
//            *psrc16++=0;
//        }
        CB_SBC_WriteDac(size,(U16 *)psrc16);

        BufCtl.offset+=size;

        if(BufCtl.offset==BufCtl.size){
            BufCtl.offset=0;
        }
	#if _SBC_AUDIO_BUF_X3
		if(!lastFifoFlag && (size < fifoSize)){
			psrc16=(S16 *)(BufCtl.buf+BufCtl.offset);
			U16 tempbuf[fifoSize];
			for (i = 0; i < fifoSize; i++) {
				tempbuf[i] = 0;
				if (i >= size) {
					tempbuf[i] = *psrc16;
					psrc16++;
				}
			}
			CB_SBC_WriteDac(fifoSize, tempbuf);
			BufCtl.offset += (fifoSize - size);
			AudioBufCtl.fifo_count += (fifoSize - size);
		}
	#endif

#if _SBC_SPEED_CHANGE||_SBC_PITCH_CHANGE
        if(AudioBufCtl.fifo_count>=AudioBufCtl.fifo_count2){
            AudioBufCtl.state=STATUS_STOP;
            AudioBufCtl.shadow_state=AudioBufCtl.state;
			PlayEndFlag = BIT_SET;
            //dprintf("stop s,%d,%d\r\n",AudioBufCtl.fifo_count,AudioBufCtl.fifo_count2);
        }
#else
        if(AudioBufCtl.fifo_count>=AudioBufCtl.samples){
#if _SBC_QIO_MODULE && !_SBC_AUDIO_BUF_X3
			QIO_DelayStop(QIO_USE_SBC_CH1,DISABLE);
			QIO_FrameFlag_FIFO_RstGet(QIO_USE_SBC_CH1);
#endif//_SBC_QIO_MODULE && !_SBC_AUDIO_BUF_X3
            AudioBufCtl.state=STATUS_STOP;
            AudioBufCtl.shadow_state=AudioBufCtl.state;
			PlayEndFlag = BIT_SET;
		#if _SBC_AUDIO_BUF_X3
			if (SBC_PlayRepeatFlag) {
				RepeatReSetting(fifoSize, size);
			}
#if _SBC_QIO_MODULE
			else {
				QIO_DelayStop(QIO_USE_SBC_CH1,DISABLE);
				QIO_FrameFlag_FIFO_RstGet(QIO_USE_SBC_CH1);
			}
#endif//_SBC_QIO_MODULE
		#endif
            //dprintf("stop s,%d,%d\r\n",AudioBufCtl.fifo_count,AudioBufCtl.samples);
        }
#endif
//
//        if((AudioBufCtl.fifo_count-(AudioBufCtl.process_count))>(SPEED_CTL_OUT_SIZE*AUDIO_BUF_COUNT)){
//            dprintf("A\r\n");
//        }
    }else{
//        for(i=0;i<size;i++){
//            *pdest32=0;
//        }
        CB_SBC_WriteDac(size,(U16 *)0);
    }
}
#if _SBC_AUTO_SAVE_TIME
//------------------------------------------------------------------//
// Set SBC erasing control state as C_FLASH_ERASE_START
// Parameter:
//          None
// return value:
//          0 : set state success
//          -1: set state fail
//------------------------------------------------------------------//
S8 SbcRamEBufCtrl_SetEraseSt(void)
{
    S8 ret;
    if(SbcRamEBufCtrl.state==C_FLASH_ERASE_IDLE)
    {
        SbcRamEBufCtrl.state = C_FLASH_ERASE_START;
        ret = 0;
    }
    else
    {
        ret = -1;
    }
    return ret;
}
//------------------------------------------------------------------//
// SBC erasing buffer process
// Parameter:
//          mode     : storage mode
//          *cur_addr: current address
//          *buf     : data buffer
//          size     : data size to read(bytes)
// return value:
//          NONE
//------------------------------------------------------------------//
void SbcRamEBufCtrl_Proc(U8 mode,U32 *cur_addr, U8 *buf,U16 size)
{
    if(mode == SPI_MODE)
    {
        if(SbcRamEBufCtrl.state == C_FLASH_ERASE_START)  // only support SPI_MODE
        {
            S32 tmp_addr;
            tmp_addr =  STORAGE_GetNonBlockSectorAddr(); //// Get STORAGE address
            Dprintf("Erase addr 0x%x\r\n",tmp_addr);
            if(tmp_addr==-1)
            {
                Dprintf("CB_ReadData Get address fail\r\n");
                SPI_BurstRead(buf,*cur_addr,size);
                *cur_addr+=size;
                return;
            }
            Dprintf("rd %d\r\n",SbcRamEBufCtrl.rd);
            SbcRamEBufCtrl.rd = 0;
            SPI_BurstRead(SbcRamEBufCtrl.buf, *cur_addr, SBC_RAM_ERASE_BUF_SIZE);
            SPI_StartSectorErase((U32)tmp_addr);

            memcpy(buf, SbcRamEBufCtrl.buf, size);
            SbcRamEBufCtrl.rd      += size;
            SbcRamEBufCtrl.cur_addr = size + *cur_addr;
            SbcRamEBufCtrl.state = C_FLASH_ERASE_GOING;
            *cur_addr+=size;
        }
        else if(SbcRamEBufCtrl.state == C_FLASH_ERASE_GOING)
        {
            U16 tmp_size;
            tmp_size = SbcRamEBufCtrl.rd + size;
            if(tmp_size < SBC_RAM_ERASE_BUF_SIZE)
            {
                memcpy(buf, SbcRamEBufCtrl.buf+SbcRamEBufCtrl.rd, size);
                SbcRamEBufCtrl.rd        = tmp_size;
                SbcRamEBufCtrl.cur_addr += size;
            }
            else
            {
                Dprintf("RAM buffer is not enough %d\r\n",tmp_size);
            }

            // if erase done.
            //Dprintf("busy %d\r\n",SPI_CheckBusyFlag() );
            if(SbcRamEBufCtrl.rd > (SBC_RAM_ERASE_BUF_SIZE - 90))
            {
                if(SPI_CheckBusyFlag() == 0)
                {
                    SbcRamEBufCtrl.state = C_FLASH_ERASE_IDLE;
                    STORAGE_NonBlockSectorEraseDone(); //STORAGE
                    Dprintf("Erase flow finish\r\n");
                }
            }
            *cur_addr+=size;
        }
        else // C_FLASH_ERASE_IDLE
        {
            CB_ReadData(mode,cur_addr,buf,size);
        }
    }
    else
    {
        CB_ReadData(mode,cur_addr,buf,size);
    }
}
//------------------------------------------------------------------//
// Stop SBC erasing control flow
// Parameter:
//          mode     : storage mode
// return value:
//          0 : Stop success
//          -1: Stop Fail
//------------------------------------------------------------------//
S8 SbcRamEBufCtrl_Stop(U8 mode)
{
    if(mode!=SPI_MODE)
    {
        return -1;
    }

    if(SPI_WaitEraseDone())
    {
        Dprintf("Storage is erasing(busy)\r\n");
        return -1;
    }
    STORAGE_NonBlockSectorEraseDone();
    autosave_cnt = 0;
    SbcRamEBufCtrl.cur_addr = 0;
    SbcRamEBufCtrl.rd       = 0;
    SbcRamEBufCtrl.state    = C_FLASH_ERASE_IDLE;
    return 0;
}
//------------------------------------------------------------------//
// Reset SBC sequence auto save
// Parameter:
//          mode     : storage mode
// return value:
//          0 : Reset Success
//          -1: Reset Fail
//------------------------------------------------------------------//
S8 SBC_AutoSave_Reset(U8 mode)
{
    if(SbcRamEBufCtrl_Stop(mode))
    {
        return -1;
    }
    SbcSeekCtl.CurAddr = 0;
    STORAGE_Save(mode);
    if(SbcRamEBufCtrl_Stop(mode))
    {
        return -1;
    }
    return 0;
}
//------------------------------------------------------------------//
// Get erase state
// Parameter:
//          None
// return value:
//          return : C_FLASH_ERASE_IDLE / C_FLASH_ERASE_START / C_FLASH_ERASE_GOING
//                   If state is not in C_FLASH_ERASE_IDLE, Flash can not acccess.
//------------------------------------------------------------------//
S8 SBC_AutoSave_GetEraseState(void)
{
    return SbcRamEBufCtrl.state;
}
///------------------------------------------------------------------//
// SBC set variables of STORAGE_MODULE
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
//_SBC_SEEK
void SBC_StorageSetVar(void)
{
    STORAGE_SetVar((void *)&SbcSeekCtl, 12); //cmd does not save, save size can not exceed 63 bytes.
}
//------------------------------------------------------------------//
// Seek SBC play sequence
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
//_SBC_SEEK
void SBC_Seek(void)
{
    // SPI_MODE only
    SbcSeekCtl.cmd = ENABLE;
#if _SBC_AUTO_SAVE_TIME
    Dprintf("Seek CurA 0x%x p %d r1 %d r2 %d\r\n",SbcSeekCtl.CurAddr,SbcSeekCtl.process_count,SbcSeekCtl.rand[0],SbcSeekCtl.rand[1]);
#endif
}
#endif
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
#endif

#if defined(_SBC_RECORD_PLAY_USE_EQ0)	\
	||defined(_SBC_RECORD_PLAY_USE_EQ1)	\
	||defined(_SBC_RECORD_PLAY_USE_EQ2)	\
	||defined(_SBC_RECORD_PLAY_USE_CUSTOMIZE)
//------------------------------------------------------------------//
// Init EQ parameter when audio play
// Parameter:
//          NONE
// return value:
//          EXIT_FAILURE, EXIT_SUCCESS
//------------------------------------------------------------------//
static U8 initRecPlayEQParameter()
{
	if(SBC_GetStatus()!=STATUS_STOP)return EXIT_FAILURE;//error
#if _SBC_RECORD
	if(!g_playrecord)return EXIT_FAILURE;//error
#endif
#if _USE_HEAP_RAM
	if(EqBuf==NULL)
	{
		EqBuf = (S32 *) MemAlloc(SBC_EQ_BUF_SIZE*sizeof(S32));//Free at SBC_FreeMemory()
	}
#endif
	memset((void *)EqBuf, 0, SBC_EQ_BUF_SIZE*sizeof(S32));
	EQFilterCtl.SampleRate = AudioBufCtl.sample_rate;
	EQFilterCtl.Buf = EqBuf;
#if	defined(_SBC_RECORD_PLAY_USE_EQ0)
	EQFilterCtl.EQGroupSelect=TYPE0;
#elif	defined(_SBC_RECORD_PLAY_USE_EQ1)
	EQFilterCtl.EQGroupSelect=TYPE1;
#elif	defined(_SBC_RECORD_PLAY_USE_EQ2)
	EQFilterCtl.EQGroupSelect=TYPE2;
#elif	defined(_SBC_RECORD_PLAY_USE_CUSTOMIZE)//customize EQ
	EQFilterCtl.EQGroupSelect=CUSTOMIZE;
#endif

#if defined(_SBC_RECORD_PLAY_USE_EQ0)	\
	||defined(_SBC_RECORD_PLAY_USE_EQ1)	\
	||defined(_SBC_RECORD_PLAY_USE_EQ2)
#if	_SBC_PITCH_CHANGE
	EQFilterCtl.PitchIndex=SpeedCtl.pitch_ratio_index-RATIO_INIT_INDEX;
#else
	EQFilterCtl.PitchIndex=0;
#endif
#elif	defined(_SBC_RECORD_PLAY_USE_CUSTOMIZE)//customize EQ
	EQFilterCtl.PitchIndex=0;//fix 0
#endif

#if defined(_SBC_RECORD_PLAY_USE_EQ0)	\
	||defined(_SBC_RECORD_PLAY_USE_EQ1)	\
	||defined(_SBC_RECORD_PLAY_USE_EQ2)
	EQFilterCtl.FilterPara1=NULL;
#elif	defined(_SBC_RECORD_PLAY_USE_CUSTOMIZE)//customize EQ
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

#if defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
//------------------------------------------------------------------//
// Init EQ parameter when audio play
// Parameter:
//          NONE
// return value:
//          EXIT_FAILURE, EXIT_SUCCESS
//------------------------------------------------------------------//
static U8 initPlayBackEQParameter()
{
	if(SBC_GetStatus()!=STATUS_STOP)return EXIT_FAILURE;//error
#if _SBC_RECORD
	if(g_playrecord)return EXIT_FAILURE;//error
#endif
#if _USE_HEAP_RAM
	if(EqBuf==NULL)
	{
		EqBuf = (S32 *) MemAlloc(SBC_EQ_BUF_SIZE*sizeof(S32));//Free at SBC_FreeMemory()
	}
#endif
	memset((void *)EqBuf, 0, SBC_EQ_BUF_SIZE*sizeof(S32));
	EQFilterCtl.SampleRate = AudioBufCtl.sample_rate;
	EQFilterCtl.Buf = EqBuf;

#if	defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)//customize EQ
	EQFilterCtl.EQGroupSelect=CUSTOMIZE;
#endif

#if	defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)//customize EQ
	EQFilterCtl.PitchIndex=0;//fix 0
#endif
#if	defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)//customize EQ
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
#if _SBC_SPEED_CHANGE||_SBC_PITCH_CHANGE
//------------------------------------------------------------------//
// Decode one audio frame
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
static void DecodeFrame(void)
{
    U16 bpf,mark_sample=0;
    U16 *audio_buf,i;
    U32 speed_ctl_size;

    //while((SPEED_CTL_FRAME_SIZE*2)-SpeedCtl.index>=SPEED_CTL_FRAME_SIZE){
    if(SpeedCtl.index<SPEED_CTL_FRAME_SIZE){

        if(AudioBufCtl.sample_count<AudioBufCtl.samples){
            audio_buf=SpeedCtlBuf+SpeedCtl.index;

            CB_ReadData(StorageMode,&CurAddr,(U8 *)audio_buf,2);

            // Modify for Encode bit number error
        	bpf=SBCBPF(0,audio_buf[0],&mark_sample);

        	if(mark_sample!=0)
        	{
        		CB_ReadData(StorageMode,&CurAddr,(U8 *)audio_buf,2);
        		bpf=SBCBPF(0,audio_buf[0],&mark_sample);
        	}

            CB_ReadData(StorageMode,&CurAddr,(U8 *)(audio_buf+1),bpf-2);

            SBCDecode(0,SBC_DEC_OUT_SIZE,(void *)audio_buf,0);

            SBCDecode(0,SBC_DEC_OUT_SIZE,(void *)audio_buf,1);

#if _SBC_ROBOT2_SOUND
            if(Robot01Ctl.enable)
            {
#if _ROBOT_SOUND_COMPRESS
                ROBOT_EFFECT_COMPRESS_01((S16 *)audio_buf);
#else
                ROBOT_EFFECT_01((S16 *)audio_buf);
#endif
            }
#endif
#if _SBC_ROBOT3_SOUND
            if(Robot02Ctl.enable)
            {
#if _ROBOT_SOUND_COMPRESS
                ROBOT_EFFECT_COMPRESS_02((S16 *)audio_buf);
#else
                ROBOT_EFFECT_02((S16 *)audio_buf);
#endif

            }
#endif


#if _SBC_ROBOT4_SOUND
 		// Process exceed 90
			if(Robot04Ctl.enable){
				ROBOT_EFFECT_04((short *)audio_buf, SBCRobot4ctl, SBCRobot4buf, SBCRobot4FFTctl);
			}
#endif

#if _SBC_REVERB_SOUND
            if(ReverbCmd)
            {
                Reverb_Effect((short *)audio_buf,SBC_DEC_OUT_SIZE);
            }
#endif

#if _SBC_ECHO_SOUND
            if(EchoCmd)
            {
                EchoVoiceProcess((S16 *)audio_buf, SBC_DEC_OUT_SIZE, &EchoCtl);
            }
#endif
            SpeedCtl.index+=SBC_DEC_OUT_SIZE;
            AudioBufCtl.sample_count+=SBC_DEC_OUT_SIZE;
            AudioBufCtl.fifo_count2=AudioBufCtl.fifo_count+(SBC_AUDIO_BUF_SIZE*2);
        }else{

            audio_buf=SpeedCtlBuf+SpeedCtl.index;

            memset((void *)audio_buf, 0, SBC_DEC_OUT_SIZE*2);

            SpeedCtl.index+=SBC_DEC_OUT_SIZE;
//
//            AudioBufCtl.sample_count+=SBC_DEC_OUT_SIZE;
//            if((AudioBufCtl.sample_count-AudioBufCtl.samples)>){
//                FifoCmd(AudioBufCtl.dac,DISABLE);
//                AudioBufCtl.state=STATUS_STOP;
//                dprintf("stop s,%d,%d\r\n",AudioBufCtl.fifo_count,AudioBufCtl.samples);
//            }
        }
        //dprintf("D...\r\n");
    }

    audio_buf=AudioBuf+(SPEED_CTL_OUT_SIZE*AudioBufCtl.index);

    AudioBufCtl.index++;
    if(AudioBufCtl.index==AudioBufCtl.out_count){
        AudioBufCtl.index=0;
    }


    if(SpeedCtl.speed_ratio_index==RATIO_INIT_INDEX && SpeedCtl.pitch_ratio_index==RATIO_INIT_INDEX ){
        speed_ctl_size=SPEED_CTL_OUT_SIZE;
        for(i=0;i<SPEED_CTL_OUT_SIZE;i++){
            audio_buf[i]=SpeedCtlBuf[i];
        }
    }else{
        speed_ctl_size=SPEED_CTL_FRAME_SIZE;

    //GPIO_Write(GPIOA,0,0);
        TimeStretchProcess(audio_buf, SpeedCtlBuf, &speed_ctl_size);
        speed_ctl_size=SPEED_CTL_FRAME_SIZE-speed_ctl_size;
    }

    //GPIO_Write(GPIOA,0,1);
    //

    //dprintf("%d,%d\r\n",speed_ctl_size,size);
    SpeedCtl.index-=speed_ctl_size;
    for(i=0;i<SpeedCtl.index;i++){
        SpeedCtlBuf[i]=SpeedCtlBuf[i+speed_ctl_size];
    }
    AudioBufCtl.process_count+=SPEED_CTL_OUT_SIZE;

#if _SBC_ROBOT1_SOUND
    if(RobotCtl.enable){
        if(RobotCtl.change){
            RoboticVoiceInit(AudioBufCtl.sample_rate, CarrierTbl[RobotCtl.index], SPEED_CTL_OUT_SIZE);
            //dprintf("RobotCtl:%d\r\n",RobotCtl.carrier);
            RobotCtl.change=0;
        }
        RoboticVoiceProcess((short *)audio_buf,(short *)audio_buf);
    }
#endif

    //volume
    for(i=0;i<SPEED_CTL_OUT_SIZE;i++){
#if _SBC_RECORD
        if(g_playrecord)
        {
#if defined(_SBC_RECORD_PLAY_USE_EQ0)	\
	||defined(_SBC_RECORD_PLAY_USE_EQ1)	\
	||defined(_SBC_RECORD_PLAY_USE_EQ2)	\
	||defined(_SBC_RECORD_PLAY_USE_CUSTOMIZE)
#if _SBC_ROBOT4_SOUND
				if(!Robot04Ctl.enable)
#endif
			audio_buf[i] = EQProcess(audio_buf[i],&EQFilterCtl);
#endif

#if defined(_SBC_RECORD_PLAY_USE_DRC)
#if _SBC_ROBOT4_SOUND
				if(!Robot04Ctl.enable)
#endif
			DRC(&drcFixedCtl,(S16*) &audio_buf[i], DRC_buf);
#elif defined(_SBC_RECORD_PLAY_USE_DIGITAL_GAIN)
			RTDigiAmpProcess((S16*) &audio_buf[i], SBCDigiAmpBuf, 1,SBCDigiAmpCTLBuf);
#endif
        }
        else
        {
#if defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
			audio_buf[i] = EQProcess(audio_buf[i],&EQFilterCtl);
#endif
        }
#else
#if defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
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
#else
//------------------------------------------------------------------//
// Decode one audio frame
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
static void DecodeFrame(void)
{
    U16 bpf,mark_sample=0;
    U16 *audio_buf,i;
#if	_SBC_QIO_MODULE
	U16 _QioFrameFlag,size;
#endif//_SBC_QIO_MODULE

#if _SBC_AUDIO_BUF_X3
/*	U16 tempOffset = BufCtl.offset;
	if((AudioBufCtl.index*SBC_DEC_OUT_SIZE <= tempOffset) && (tempOffset < (AudioBufCtl.index+1)*SBC_DEC_OUT_SIZE) && AudioBufCtl.shadow_state==STATUS_PLAYING){
		//return;
	}*/
	if (AudioBufCtl.decode_state == 2) {
		AudioBufCtl.decode_state = 0;
	}

	if(!AudioBufCtl.index){
		audio_buf=AudioBuf;
	} else if (AudioBufCtl.index==1){
		audio_buf=AudioBuf+SBC_DEC_OUT_SIZE;
	} else {
		audio_buf=AudioBuf+SBC_DEC_OUT_SIZE*2;
	}
#else
    if(AudioBufCtl.index){
        audio_buf=AudioBuf+SBC_DEC_OUT_SIZE;
    }else{
        audio_buf=AudioBuf;
    }
#endif

//    if(mute){
//        for(i=0;i<SBC_DEC_OUT_SIZE;i++){
//            audio_buf[i]=0;
//        }
//        AudioBufCtl.toggle^=1;
//        return;
//    }
    if(!AudioBufCtl.decode_state){

#if _SBC_AUTO_SAVE_TIME
        SbcRamEBufCtrl_Proc(StorageMode,&CurAddr,(U8 *)audio_buf,2);
#else
        CB_ReadData(StorageMode,&CurAddr,(U8 *)audio_buf,2);
#endif

        // Modify for Encode bit number error
       	bpf=SBCBPF(0,audio_buf[0],&mark_sample);

#if	_SBC_QIO_MODULE
		_QioFrameFlag = (mark_sample!=0) ? mark_sample : 0;
		size = SBC_DEC_OUT_SIZE;
		if((size+AudioBufCtl.sample_count)>AudioBufCtl.samples)
		{
			size=AudioBufCtl.samples-AudioBufCtl.sample_count;
		}
		if(QIO_FrameFlag_FIFO_BitPut(QIO_USE_SBC_CH1,(U8*)&_QioFrameFlag)==EXIT_SUCCESS)
		{
			_QioFrameFlag = 0;
			for(i=1;i<size;i++)
			{
				QIO_FrameFlag_FIFO_BitPut(QIO_USE_SBC_CH1,(U8*)&_QioFrameFlag);
			}
		}
		AudioBufCtl.sample_count+=size;
		if(AudioBufCtl.samples == AudioBufCtl.sample_count)
		{
			AudioBufCtl.sample_count = 0;
			QIO_FrameFlag_FIFO_RstPut(QIO_USE_SBC_CH1);
		}
#endif//_SBC_QIO_MODULE

    	if(mark_sample!=0)
    	{
    		CB_ReadData(StorageMode,&CurAddr,(U8 *)audio_buf,2);
    		bpf=SBCBPF(0,audio_buf[0],&mark_sample);
    	}
#if _SBC_AUTO_SAVE_TIME
        SbcRamEBufCtrl_Proc(StorageMode,&CurAddr,(U8 *)(audio_buf+1),bpf-2);
#else
        CB_ReadData(StorageMode,&CurAddr,(U8 *)(audio_buf+1),bpf-2);
#endif


        SBCDecode(0,SBC_DEC_OUT_SIZE,(void *)audio_buf,0);

    }else{
        SBCDecode(0,SBC_DEC_OUT_SIZE,(void *)audio_buf,1);
        AudioBufCtl.process_count+=SBC_DEC_OUT_SIZE;
#if _SBC_AUTO_SAVE_TIME
        if(StorageMode == SPI_MODE)
        {
            autosave_cnt += SBC_DEC_OUT_SIZE;
            //Dprintf("as cnt %d\r\n",autosave_cnt);

            if(autosave_cnt > AUTO_SAVE_COUNT)
            {
                autosave_cnt = 0;
                SbcSeekCtl.CurAddr = CurAddr;
                SbcSeekCtl.process_count = AudioBufCtl.process_count;
                SbcSeekCtl.rand[0] = *(SBCRam+488);
                SbcSeekCtl.rand[1] = *(SBCRam+489);
                STORAGE_Save(StorageMode);
            }
        }
        //Dprintf("CA 0x%x p %d r %d %d\r\n",CurAddr, AudioBufCtl.process_count,*(SBCRam+488), *(SBCRam+489));
#endif

	#if _SBC_AUDIO_BUF_X3
		//if(AudioBufCtl.preDecFrame){
		AudioBufCtl.preDecCount += SBC_DEC_OUT_SIZE;
		//}
		AudioBufCtl.index++;
		if(AudioBufCtl.index == 3) {
			AudioBufCtl.index = 0;
		}
	#else
	    AudioBufCtl.index^=1;
	#endif


#if _SBC_ROBOT2_SOUND
        if(Robot01Ctl.enable)
        {
#if _ROBOT_SOUND_COMPRESS
            ROBOT_EFFECT_COMPRESS_01((S16 *)audio_buf);
#else
            ROBOT_EFFECT_01((S16 *)audio_buf);
#endif
        }
#endif
#if _SBC_ROBOT3_SOUND
        if(Robot02Ctl.enable)
        {
#if _ROBOT_SOUND_COMPRESS
            ROBOT_EFFECT_COMPRESS_02((S16 *)audio_buf);
#else
            ROBOT_EFFECT_02((S16 *)audio_buf);
#endif

        }
#endif



#if _SBC_ROBOT1_SOUND
        if(RobotCtl.enable){
            if(RobotCtl.change){
                RoboticVoiceInit(AudioBufCtl.sample_rate, CarrierTbl[RobotCtl.index], SBC_DEC_OUT_SIZE);
                //dprintf("RobotCtl:%d\r\n",RobotCtl.carrier);
                RobotCtl.change=0;
            }
            RoboticVoiceProcess((short *)audio_buf,(short *)audio_buf);
        }
#endif

#if _SBC_ROBOT4_SOUND
 		// Process exceed 90
			if(Robot04Ctl.enable){
				ROBOT_EFFECT_04((short *)audio_buf, SBCRobot4ctl, SBCRobot4buf, SBCRobot4FFTctl);
			}
#endif


//         for(i=0;i<SBC_DEC_OUT_SIZE;i++){
//                   audio_buf[i]=0;
//          }
#if _SBC_REVERB_SOUND
        if(ReverbCmd)
        {
            Reverb_Effect((short *)audio_buf,SBC_DEC_OUT_SIZE);
        }
#endif

#if _SBC_ECHO_SOUND
        if(EchoCmd)
        {
            EchoVoiceProcess((S16 *)audio_buf, SBC_DEC_OUT_SIZE, &EchoCtl);
        }
#endif

        //volume
        for(i=0;i<SBC_DEC_OUT_SIZE;i++){
#if _SBC_RECORD
            if(g_playrecord)
            {
#if defined(_SBC_RECORD_PLAY_USE_EQ0)	\
	||defined(_SBC_RECORD_PLAY_USE_EQ1)	\
	||defined(_SBC_RECORD_PLAY_USE_EQ2)	\
	||defined(_SBC_RECORD_PLAY_USE_CUSTOMIZE)
			#if _SBC_ROBOT4_SOUND
				if(!Robot04Ctl.enable)
			#endif
                audio_buf[i] = EQProcess(audio_buf[i],&EQFilterCtl);
#endif

#if defined(_SBC_RECORD_PLAY_USE_DRC)
#if _SBC_ROBOT4_SOUND
				if(!Robot04Ctl.enable)
#endif
                DRC(&drcFixedCtl,(S16*) &audio_buf[i], DRC_buf);
#elif defined(_SBC_RECORD_PLAY_USE_DIGITAL_GAIN)
				RTDigiAmpProcess((S16*) &audio_buf[i], SBCDigiAmpBuf, 1,SBCDigiAmpCTLBuf);
#endif
            }
            else
            {
#if defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
                audio_buf[i] = EQProcess(audio_buf[i],&EQFilterCtl);
#endif
            }
#else
#if defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
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

#if _SBC_AUDIO_BUF_X3
	AudioBufCtl.decode_state++;
#else
    AudioBufCtl.decode_state^=1;
#endif
}
#endif
#if _SBC_RECORD
//------------------------------------------------------------------//
// Stop record
// Parameter:
//          None
// return value:
//          None
//------------------------------------------------------------------//
static void  StopRecord(void)
{
    CB_SBC_AdcCmd(DISABLE);
#if _SBC_RECORD_ERASING
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
        //dprintf("Stop Record restore Flash CurAddr 0x%x\r\n",CurAddr);
    }
    RecordErasingCtrl.status = C_RECORD_ERASE_IDLE;
#endif
	if(AudioBufCtl.samples>HEADER_LEN)
	{
    	WriteHeader();
	}
    AudioBufCtl.state=STATUS_STOP;
    AudioBufCtl.shadow_state=STATUS_STOP;
#if _USE_HEAP_RAM
    SBC_FreeMemory(MEMORY_ALLOCATE_RECORD);
#endif

#if _EF_SERIES && _SBC_RECORD_ERASING
	if (StorageMode == EF_MODE)
	{
		GIE_DISABLE();
		P_SMU->INTV_BA = C_INTV_BA_Dis;
		GIE_ENABLE();
		INTC_IrqCmd(intcCmdTemp,ENABLE);
	}
#endif

    CB_SBC_RecEnd();
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
U8 SBC_Play(U32 index,U8 mode){
    struct AUDIO_HEADER *pheader;

    if(AudioBufCtl.shadow_state==STATUS_PLAYING){
#if SMOOTH_ENABLE
            AudioBufCtl.state=STATUS_STOP;
            SmoothFunc(SMOOTH_DOWN);
            if(!DacMixerRamp_IsBias()) {
                AudioBufCtl.shadow_state=STATUS_STOP;
#if _SBC_QIO_MODULE
                QIO_Stop(QIO_USE_SBC_CH1,ENABLE);
#endif
            }
            while(AudioBufCtl.shadow_state==STATUS_PLAYING){
                AUDIO_ServiceLoop();
            }
#endif
    }
#if _SBC_RECORD
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
    SBC_AllocateMemory(MEMORY_ALLOCATE_PLAY);
#else
	AUDIO_ServiceLoop();
#endif

#if _SBC_AUTO_SAVE_TIME
#if _USE_HEAP_RAM==0
    SbcRamEBufCtrl.buf = SbcRamEBuf;
#endif
    if(SbcRamEBufCtrl_Stop(mode))
    {
        return E_AUDIO_STORAGE_BUSY;
    }
#endif

#if _SBC_ECHO_SOUND
    if(EchoCmd)
    {
#if _USE_HEAP_RAM
        if(EchoBuf==NULL)
            EchoBuf = (S16 *)MemAlloc(SBC_ECHO_BUF_SIZE<<1);
#endif
        memset((void *)EchoBuf, 0, SBC_ECHO_BUF_SIZE<<1);
    }
#endif
    memset((void *)&AudioBufCtl, 0, sizeof(AudioBufCtl));

#if _SBC_SPEED_CHANGE||_SBC_PITCH_CHANGE
    AudioBufCtl.fifo_count2=-1;
    AudioBufCtl.out_count=SBC_AUDIO_BUF_SIZE/SPEED_CTL_OUT_SIZE;
    if(AudioBufCtl.out_count&0x1){
        AudioBufCtl.out_count--;
    }
#endif
//    AudioBufCtl.ramp=ramp;

//    SmoothCtl.p_parameter=0;

    BufCtl.offset=0;
#if _SBC_SPEED_CHANGE||_SBC_PITCH_CHANGE
    BufCtl.size=AudioBufCtl.out_count*SPEED_CTL_OUT_SIZE;
#else
    BufCtl.size=SBC_AUDIO_BUF_SIZE;
#endif
    BufCtl.buf=AudioBuf;

    if(CB_SetStartAddr(index,mode,0,&StartAddr)){
		return E_AUDIO_INVALID_IDX;
	}
    CurAddr=StartAddr;
    StorageMode=mode;

    CB_ReadData(StorageMode,&CurAddr,(U8 *)AudioBuf,HEADER_LEN);

    pheader=(struct AUDIO_HEADER *)AudioBuf;

    //dprintf("Play:%x,%x,%x\r\n",pheader->signature[0],pheader->signature[1],pheader->signature[2]);

#if _SBC_RECORD
    bool _isSignature2Set1 = (pheader->signature[2]=='1');
#endif

    if(pheader->codec!=CODEC_SBC && pheader->codec!=CODEC_SBC_E){
        return E_AUDIO_UNSUPPORT_CODEC;
    }

	GIE_DISABLE();
#if	_SBC_PITCH_CHANGE
    S8 hw_ch = DacAssigner_Regist(kSBC_CH1,SBC_CH1_EXCLUSIVE);
#else
	S8 hw_ch = DacAssigner_Regist(kSBC_CH1,pheader->sample_rate);
#endif
	GIE_ENABLE();
    if(hw_ch<0){
        //dprintf("No play Invalid sr\r\n");
        return E_AUDIO_INVALID_SAMPLERATE;
    }
	//dprintf("codec %d reg hw_ch %d\r\n",kSBC_CH1,hw_ch);

#if _SBC_QIO_MODULE
	if(pheader->nyq_offset)
	{
		QIO_Play(QIO_USE_SBC_CH1, mode, (StartAddr+HEADER_LEN+pheader->nyq_offset));
	}
#endif//_SBC_QIO_MODULE
//    dprintf("bit_per_frame:%x\r\n",pheader->bit_per_frame);
//    dprintf("decode_in_size:%x\r\n",pheader->decode_in_size);
//    dprintf("decode_out_size:%x\r\n",pheader->decode_out_size);
//    dprintf("sample rate:%x\r\n",pheader->sample_rate);
//    dprintf("samples:%x\r\n",pheader->samples);

    AudioBufCtl.samples=pheader->samples;
    AudioBufCtl.sample_rate=pheader->sample_rate;
	AudioBufCtl.upsample=pheader->flag&0x1;

#if SMOOTH_ENABLE
//    if(ramp&RAMP_UP){
        SmoothFunc(SMOOTH_UP);
//    }else{
//        SmoothFunc(SMOOTH_NONE);
//    }
#endif
    //dprintf("pheader->decode_in_size,%d\r\n",pheader->decode_in_size);

    SBCDecInit(0,SBCRam,AudioBufCtl.sample_rate,pheader->bandwidth,pheader->bit_per_frame&0x7FFF,pheader->decode_in_size,pheader->decode_out_size,pheader->bit_per_frame>>15);

#if _SBC_AUTO_SAVE_TIME//_SBC_SEEK
    // Load sbc parameter
    if(SbcSeekCtl.cmd && SbcSeekCtl.CurAddr!=0)
    {
        U32 addr_start,addr_next;

        addr_start = CurAddr - HEADER_LEN;
        addr_next  = addr_start + pheader->file_size;
        Dprintf("Song Address 0x%x Next Address 0x%x\r\n",addr_start,addr_next);
        if(addr_start<SbcSeekCtl.CurAddr && addr_next>SbcSeekCtl.CurAddr)
        {
            AudioBufCtl.fifo_count = AudioBufCtl.process_count = SbcSeekCtl.process_count;
            memcpy((SBCRam+488), &(SbcSeekCtl.rand[0]), 4);
            CurAddr = SbcSeekCtl.CurAddr;
        }
    }
    else
    {
        if(pheader->codec==CODEC_SBC_E && pheader->samples>SBC_DEC_OUT_SIZE){
            CB_ReadData(StorageMode,&CurAddr,(U8 *)(SBCRam+320),160<<1);
        }
    }
    SbcSeekCtl.cmd = DISABLE;
#else
	if(pheader->codec==CODEC_SBC_E && pheader->samples>SBC_DEC_OUT_SIZE){
		CB_ReadData(StorageMode,&CurAddr,(U8 *)(SBCRam+320),160<<1);
	}
#endif

//#if _SBC_SPEED_CHANGE
//    for(i=0;i<AUDIO_BUF_COUNT;i++){
//        DecodeFrame();
//    }
//#else
//    DecodeFrame();
//    DecodeFrame();
//#endif

    memset((void *)AudioBuf, 0, SBC_AUDIO_BUF_SIZE*2);

    //dprintf("AudioBufCtl.process_count,%d\r\n",AudioBufCtl.process_count);

	CB_SBC_InitDac(pHwCtl[hw_ch].p_dac,AudioBufCtl.sample_rate,AudioBufCtl.upsample);

#if _SBC_SPEED_CHANGE
    SpeedCtl.index=0;

#if _SBC_PITCH_CHANGE
    TimeStretchInit(1,SpeedRatioTbl[SpeedCtl.speed_ratio_index]/PitchRatioTbl[SpeedCtl.pitch_ratio_index]);
#else
    TimeStretchInit(1,SpeedRatioTbl[SpeedCtl.speed_ratio_index]);
#endif

#endif
#if _SBC_PITCH_CHANGE
    SpeedCtl.index=0;
#if _SBC_SPEED_CHANGE
    TimeStretchInit(0,SpeedRatioTbl[SpeedCtl.speed_ratio_index]/PitchRatioTbl[SpeedCtl.pitch_ratio_index]);
#else
    TimeStretchInit(0,1/PitchRatioTbl[SpeedCtl.pitch_ratio_index]);
#endif



	CB_SBC_ChangeSR(pHwCtl[hw_ch].p_dac,AudioBufCtl.sample_rate*PitchRatioTbl[SpeedCtl.pitch_ratio_index]);

#endif


#if _SBC_ROBOT2_SOUND
    if(Robot01Ctl.enable)
    {
#if _USE_HEAP_RAM
        if(Robot01_buf==NULL)   Robot01_buf = (S16 *) MemAlloc(SBC_ROBOT_BUF_SIZE<<1);
#endif
        memset((void *)Robot01_buf, 0, SBC_ROBOT_BUF_SIZE*2);
        ROBOT_EFFECT_01_Init(Robot01_buf,Robot01Ctl.type,Robot01Ctl.thres,Robot01Ctl.detectthres,AudioBufCtl.sample_rate,SBC_DEC_OUT_SIZE,40,1600);
    }
#endif
#if _SBC_ROBOT3_SOUND
    if(Robot02Ctl.enable)
    {
#if _USE_HEAP_RAM
        if(Robot02_buf==NULL)   Robot02_buf = (S16 *) MemAlloc(SBC_ROBOT02_BUF_SIZE<<1);
#endif
        memset((void *)Robot02_buf, 0, SBC_ROBOT02_BUF_SIZE*2);
        ROBOT_EFFECT_02_Init(Robot02_buf,Robot02Ctl.type,Robot02Ctl.thres,AudioBufCtl.sample_rate);
    }
#endif


#if _SBC_ROBOT4_SOUND
	if(Robot04Ctl.enable){

		#if _USE_HEAP_RAM
		if(SBCRobot4ctl==NULL)
        {
        	SBCRobot4ctl = (ROBOT4Ctl*) MemAlloc(SBC_ROBOT4_CTL_SIZE);
        }
		if(SBCRobot4buf==NULL)
        {
        	SBCRobot4buf = (ROBOT4Buf*) MemAlloc(SBC_ROBOT4_BUF_SIZE);
        }
        if(SBCRobot4FFTctl==NULL)
        {
        	SBCRobot4FFTctl = (FFTctl*)MemAlloc(SBC_ROBOT4_FFT_CTL_SIZE);
        }
        #endif
		memset((void *)SBCRobot4ctl, 0, SBC_ROBOT4_CTL_SIZE);
		SBCRobot4ctl->FFTSize 		  = 	ROBOT4_FFT_SIZE;
		SBCRobot4ctl->Samplerate 	  = 	AudioBufCtl.sample_rate;
		SBCRobot4ctl->Overlapratio 	= 	ROBOT4_OVERLAP_RATIO;
		SBCRobot4ctl->InputSize 	  = 	SBC_ROBOT4_DATA_LENGTH;
		memset((void *)SBCRobot4buf, 0, SBC_ROBOT4_BUF_SIZE);
		memset((void *)SBCRobot4FFTctl, 0, SBC_ROBOT4_FFT_CTL_SIZE);
		ROBOT_EFFECT_04_Init(SBCRobot4ctl, SBCRobot4buf, SBCRobot4FFTctl);

	}
#endif

#if _SBC_REVERB_SOUND
    if(ReverbCmd)
    {
#if _USE_HEAP_RAM
        if(Reverb_x==NULL)
        {
            Reverb_x = (S16 *) MemAlloc(SBC_REVERB_BUF_SIZE<<1);
            Reverb_y = (S16 *) MemAlloc(SBC_REVERB_BUF_SIZE<<1);
        }
#endif
        Reverb_Effect_Init(Reverb_x,Reverb_y,ReverbEffect);
    }
#endif

#if _SBC_RECORD
    g_playrecord = ((index&ADDRESS_MODE)>0)&&_isSignature2Set1;
    if(g_playrecord)
    {
#if defined(_SBC_RECORD_PLAY_USE_EQ0)	\
	||defined(_SBC_RECORD_PLAY_USE_EQ1)	\
	||defined(_SBC_RECORD_PLAY_USE_EQ2)	\
	||defined(_SBC_RECORD_PLAY_USE_CUSTOMIZE)
		if(initRecPlayEQParameter()!=EXIT_SUCCESS){
			return 1;//init error
		}
#endif

#if defined(_SBC_RECORD_PLAY_USE_DRC)
		drcFixedCtl.InputThreshold = -19.19f;
		drcFixedCtl.InputRatio = 20.47f;
		DRC_Init(&drcFixedCtl, DRC_buf, AudioBufCtl.sample_rate);
#elif defined(_SBC_RECORD_PLAY_USE_DIGITAL_GAIN)
		memset((void *)SBCDigiAmpCTLBuf,0,34*2);
		memset((void *)SBCDigiAmpBuf,0,36*2);
        RTDigiAmpInit(SBCDigiAmpArray, SBCDigiAmpSet, SBCDigiAmpBuf, SBCDigiAmpCTLBuf);
        RTVadInit(SBCVADArray,(SBCDigiAmpCTLBuf+22));
#endif
    }
	else
	{
#if defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
		if(initPlayBackEQParameter()!=EXIT_SUCCESS){
			return 1;//init error
		}
#endif
	}
#else
#if defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
	if(initPlayBackEQParameter()!=EXIT_SUCCESS){
		return 1;//init error
	}
#endif
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
#if _SBC_SPEED_CHANGE||_SBC_PITCH_CHANGE
    DecodeFrame();
#else
    DecodeFrame();
    DecodeFrame();
#endif

    OpMode=0;
    AudioBufCtl.state=  STATUS_PLAYING;
    AudioBufCtl.shadow_state=AudioBufCtl.state;

    CB_SBC_PlayStart();

	CB_SBC_DacCmd(pHwCtl[hw_ch].p_dac,ENABLE);

    return 0;
}
//------------------------------------------------------------------//
// Pause playing
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SBC_Pause(void){
    if(AudioBufCtl.state==STATUS_PLAYING){

#if SMOOTH_ENABLE
//        if(AudioBufCtl.ramp&RAMP_DOWN){
            AudioBufCtl.state=STATUS_PAUSE;
            SmoothFunc(SMOOTH_DOWN);
//        }else
#endif

//----------------------------------------------------------------------------//
// 增加宏设置sbc播放暂停时是否清除播放断点 2023-09-14 添加
//----------------------------------------------------------------------------//
#if _SBC_AUTO_SAVE_TIME && USER_SBC_PLAY_END_CLEAN_BREAK_POINT
        if(SbcRamEBufCtrl_Stop(StorageMode))
        {
            return;
        }
#endif

//        {
//            AudioBufCtl.state=STATUS_PAUSE;
//        }
    }
#if _SBC_RECORD
    if(AudioBufCtl.state==STATUS_RECORDING){
        CB_SBC_AdcCmd(DISABLE);
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
void SBC_Resume(void){
	if(AudioBufCtl.state==STATUS_PAUSE&&(!OpMode)){
        if(DacAssigner_codecUseDacCh(kSBC_CH1)<0) {
            GIE_DISABLE();
#if	_SBC_PITCH_CHANGE
            S8 hw_ch = DacAssigner_Regist(kSBC_CH1,SBC_CH1_EXCLUSIVE);
#else
            S8 hw_ch = DacAssigner_Regist(kSBC_CH1,AudioBufCtl.sample_rate);
#endif
            GIE_ENABLE();
            if(hw_ch<0) return;//error

            CB_SBC_InitDac(pHwCtl[hw_ch].p_dac,AudioBufCtl.sample_rate,AudioBufCtl.upsample);
#if	_SBC_PITCH_CHANGE
            SBC_SetPitch(SpeedCtl.pitch_ratio_index-RATIO_INIT_INDEX);
#endif
            CB_SBC_DacCmd(pHwCtl[hw_ch].p_dac,ENABLE);
        }

#if SMOOTH_ENABLE
//        if(AudioBufCtl.ramp&RAMP_UP){
            SmoothFunc(SMOOTH_UP);
//        }
#endif
        AudioBufCtl.state=STATUS_PLAYING;
        AudioBufCtl.shadow_state=AudioBufCtl.state;
    }
#if _SBC_RECORD
    if(AudioBufCtl.state==STATUS_PAUSE&&OpMode){
        AudioBufCtl.state=STATUS_RECORDING;
        CB_SBC_AdcCmd(ENABLE);
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
void SBC_Stop(void){

    if((AudioBufCtl.state==STATUS_PLAYING||AudioBufCtl.state==STATUS_PAUSE)&&(!OpMode)){
#if SMOOTH_ENABLE
//        if(AudioBufCtl.ramp&RAMP_DOWN){
            AudioBufCtl.state=STATUS_STOP;
            SmoothFunc(SMOOTH_DOWN);
			if (AudioBufCtl.shadow_state == STATUS_PAUSE
                || !DacMixerRamp_IsBias()
            ) {
				AudioBufCtl.shadow_state = STATUS_STOP;
#if _SBC_QIO_MODULE
				QIO_Stop(QIO_USE_SBC_CH1,ENABLE);
#endif//_SBC_QIO_MODULE
			}
//        }else
#endif
//        {
//            AudioBufCtl.state=STATUS_STOP;
//            CB_SBC_PlayEnd();
//        }

//----------------------------------------------------------------------------//
// 增加宏设置sbc播放停止时是否清除播放断点 2023-06-27 添加
//----------------------------------------------------------------------------//
#if _SBC_AUTO_SAVE_TIME && USER_SBC_PLAY_END_CLEAN_BREAK_POINT
            SBC_AutoSave_Reset(StorageMode);
#endif

    }
#if _SBC_RECORD
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
U8 SBC_GetStatus(void){
    return AudioBufCtl.shadow_state;
}
//------------------------------------------------------------------//
// Set audio volume
// Parameter:
//          vol:0~15
// return value:
//          NONE
//------------------------------------------------------------------//
void SBC_SetVolume(U8 vol){
    if(vol>CH_VOL_15){
        vol=CH_VOL_15;
    }
    ChVol=vol;
}
#if defined(_SBC_RECORD_PLAY_USE_CUSTOMIZE)\
	||defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
//------------------------------------------------------------------//
// Set customize filter index
// Parameter:
//          index: (0~FILTER_NUMBER-1)User define filter, (other)All Pass
// return value:
//          NONE
//------------------------------------------------------------------//
void SBC_SetEQFilter(U8 index){
	u8EQFilterIndex = index;
#if	_SBC_RECORD
	if(g_playrecord)
	{
#if defined(_SBC_RECORD_PLAY_USE_CUSTOMIZE)
		if(u8EQFilterIndex >= FILTER_NUMBER){
			EQFilterCtl.FilterPara1 = _AllPassFilterParaTab;//all pass parameter
		} else {
			EQFilterCtl.FilterPara1 = (S16*)FilterList[u8EQFilterIndex];//UserDefine
		}
#endif
	}
	else
	{
#if defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
		if(u8EQFilterIndex >= FILTER_NUMBER){
			EQFilterCtl.FilterPara1 = _AllPassFilterParaTab;//all pass parameter
		} else {
			EQFilterCtl.FilterPara1 = (S16*)FilterList[u8EQFilterIndex];//UserDefine
		}
#endif
	}
#else
#if defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
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
void SBC_SetSpeed(S8 index){

#if _SBC_SPEED_CHANGE
    if(AudioBufCtl.sample_rate<=SPEED_CTL_SAMPLE_RATE){
        index+=RATIO_INIT_INDEX;
        SpeedCtl.speed_ratio_index=index;
#if _SBC_PITCH_CHANGE
        TimeStretchInit(0,SpeedRatioTbl[index]/PitchRatioTbl[SpeedCtl.pitch_ratio_index]);
#else
        TimeStretchInit(0,SpeedRatioTbl[index]);
#endif
    }
#endif
}
//------------------------------------------------------------------//
// Set pitch ratio
// Parameter:
//          index: -12~12
// return value:
//          NONE
//------------------------------------------------------------------//
void SBC_SetPitch(S8 index){
#if _SBC_PITCH_CHANGE
    if(AudioBufCtl.sample_rate<=SPEED_CTL_SAMPLE_RATE){
        index+=RATIO_INIT_INDEX;
        SpeedCtl.pitch_ratio_index=index;
#if _SBC_SPEED_CHANGE
        TimeStretchInit(0,SpeedRatioTbl[SpeedCtl.speed_ratio_index]/PitchRatioTbl[index]);
#else
        TimeStretchInit(0,1/PitchRatioTbl[index]);
#endif
#if _SBC_RECORD
        if(g_playrecord)
        {
#if defined(_SBC_RECORD_PLAY_USE_EQ0)	\
	||defined(_SBC_RECORD_PLAY_USE_EQ1)	\
	||defined(_SBC_RECORD_PLAY_USE_EQ2)
			EQFilterCtl.PitchIndex=SpeedCtl.pitch_ratio_index-RATIO_INIT_INDEX;
#elif defined(_SBC_RECORD_PLAY_USE_CUSTOMIZE)
			EQFilterCtl.PitchIndex=0;
#endif
        }
        else
        {
#if defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
            EQFilterCtl.PitchIndex=0;
#endif
        }
#else
#if defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
        EQFilterCtl.PitchIndex=0;
#endif
#endif
		S8	hw_ch = DacAssigner_codecUseDacCh(kSBC_CH1);
		if(hw_ch<0)return;//No register hw_ch
		CB_SBC_ChangeSR(pHwCtl[hw_ch].p_dac,AudioBufCtl.sample_rate*PitchRatioTbl[index]);

    }
#endif
}
//------------------------------------------------------------------//
// Enable/Disable Robot Sound Effect
// Parameter:
//          cmd: 1:enable,0:disable
//          index: carrier index,0~15
// return value:
//          NONE
//------------------------------------------------------------------//
void SBC_SetRobot1Sound(U8 cmd,U8 index){
#if _SBC_ROBOT1_SOUND

    RobotCtl.enable=cmd;

    if(RobotCtl.enable){
        RobotCtl.change=1;
        RobotCtl.index=index;
    }
#endif
}
#if _SBC_ROBOT2_SOUND
//------------------------------------------------------------------//
// Enable/Disable Robot Sound Effect
// Parameter:
//          cmd: 1:enable,0:disable
//          type: robot type 0~2
//          thres: robot threshold 0~7
// return value:
//          NONE
//------------------------------------------------------------------//
void SBC_SetRobot2Sound(U8 cmd,U8 type, U8 thres){
    Robot01Ctl.enable=cmd;

    if(Robot01Ctl.enable)
    {
        Robot01Ctl.type=type;
        Robot01Ctl.thres= Robot_Thres[thres];
        Robot01Ctl.detectthres = 100;
#if _USE_HEAP_RAM
        if(Robot01_buf==NULL)
        {
            Robot01_buf = (S16 *)MemAlloc(SBC_ROBOT_BUF_SIZE<<1);
        }
#endif
        memset((void *)Robot01_buf, 0, SBC_ROBOT_BUF_SIZE*2);
        ROBOT_EFFECT_01_Init(Robot01_buf,Robot01Ctl.type,Robot01Ctl.thres,Robot01Ctl.detectthres,AudioBufCtl.sample_rate,SBC_DEC_OUT_SIZE,40,1600);
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

#if _SBC_ROBOT3_SOUND
//------------------------------------------------------------------//
// Enable/Disable Robot Sound Effect
// Parameter:
//          cmd: 1:enable,0:disable
//          type: robot type 0~2
//          thres: robot threshold 0~7
// return value:
//          NONE
//------------------------------------------------------------------//
void SBC_SetRobot3Sound(U8 cmd,U8 type, U8 thres){
    Robot02Ctl.enable=cmd;

    if(Robot02Ctl.enable)
    {
        Robot02Ctl.type=type;
        Robot02Ctl.thres= Robot02_Thres[thres];
#if _USE_HEAP_RAM
        if(Robot02_buf==NULL)
        {
            Robot02_buf = (S16 *)MemAlloc(SBC_ROBOT02_BUF_SIZE<<1);
        }
#endif
        memset((void *)Robot02_buf, 0, SBC_ROBOT02_BUF_SIZE*2);
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


#if _SBC_ROBOT4_SOUND
//------------------------------------------------------------------//
// Enable/Disable Robot4 Sound Effect
// Parameter:
//          cmd: 1:enable,0:disable
// return value:
//          NONE
//------------------------------------------------------------------//
void SBC_SetRobot4Sound(U8 cmd){

if(cmd == ENABLE)
    {
		#if _USE_HEAP_RAM
		if(SBCRobot4ctl==NULL)
        {
        	SBCRobot4ctl = (ROBOT4Ctl*) MemAlloc(SBC_ROBOT4_CTL_SIZE);
        }
		if(SBCRobot4buf==NULL)
        {
        	SBCRobot4buf = (ROBOT4Buf*) MemAlloc(SBC_ROBOT4_BUF_SIZE);
        }
        if(SBCRobot4FFTctl==NULL)
        {
        	SBCRobot4FFTctl = (FFTctl*) MemAlloc(SBC_ROBOT4_FFT_CTL_SIZE);
        }
        #endif
		memset((void *)SBCRobot4ctl, 0, SBC_ROBOT4_CTL_SIZE);
		SBCRobot4ctl->FFTSize 		  = 	ROBOT4_FFT_SIZE;
		SBCRobot4ctl->Samplerate 	  = 	AudioBufCtl.sample_rate;
		SBCRobot4ctl->Overlapratio 	= 	ROBOT4_OVERLAP_RATIO;
		SBCRobot4ctl->InputSize 	  = 	SBC_ROBOT4_DATA_LENGTH;
		memset((void *)SBCRobot4buf, 0, SBC_ROBOT4_BUF_SIZE);
		memset((void *)SBCRobot4FFTctl, 0, SBC_ROBOT4_FFT_CTL_SIZE);
		ROBOT_EFFECT_04_Init(SBCRobot4ctl, SBCRobot4buf, SBCRobot4FFTctl);

       	Robot04Ctl.enable = cmd;
    }
    else
    {
#if _USE_HEAP_RAM
		if(SBCRobot4ctl==NULL)
        {
	 		MemFree(SBCRobot4ctl);
	 		SBCRobot4ctl=NULL;
 		}

		if(SBCRobot4buf==NULL)
        {
	 		MemFree(SBCRobot4buf);
	 		SBCRobot4buf=NULL;
 		}

 		if(SBCRobot4FFTctl==NULL)
        {
	 		MemFree(SBCRobot4FFTctl);
	 		SBCRobot4FFTctl=NULL;
 		}
#endif
		Robot04Ctl.enable = cmd;
    }
}
//}
#endif

#if _SBC_REVERB_SOUND
//------------------------------------------------------------------//
// Enable/Disable Reverb Sound Effect
// Parameter:
//          cmd: 1:enable,0:disable
//          index: Effect index,0~7
// return value:
//          NONE
//------------------------------------------------------------------//
void SBC_SetReverbSound(U8 cmd,U8 index){
    ReverbCmd = cmd;
    if(cmd)
    {
#if _USE_HEAP_RAM
        if(Reverb_x==NULL)
        {
            Reverb_x = (S16 *)MemAlloc(SBC_REVERB_BUF_SIZE<<1);
            Reverb_y = (S16 *)MemAlloc(SBC_REVERB_BUF_SIZE<<1);
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
#if _SBC_ECHO_SOUND
//------------------------------------------------------------------//
// Enable/Disable Echo Sound Effect
// Parameter:
//          cmd: 1:enable,0:disable
//          index: echo type,0~7
// return value:
//          NONE
//------------------------------------------------------------------//
void SBC_SetEchoSound(U8 cmd,U8 index){

    if(cmd == ENABLE)
    {
#if _USE_HEAP_RAM
        if(EchoBuf==NULL)   EchoBuf = (S16 *)MemAlloc(SBC_ECHO_BUF_SIZE<<1);
#endif
        EchoCtl.flag = 0;
        EchoCtl.count= 0;
        EchoCtl.frameCount = 0;
        EchoCtl.buf = EchoBuf;
        EchoCtl.EchoBufL = SBC_ECHO_DATA_LENGTH;
        EchoCtl.EchoNB = EchoCoef[0][index];;
        EchoCtl.EchoWT = EchoCoef[1][index];;
        EchoCtl.RingBufPeriod = EchoCoef[2][index];
        EchoCmd = ENABLE;
    }
    else
    {
#if _USE_HEAP_RAM
        if(EchoBuf!=NULL)
        {
            MemFree(EchoBuf);
            EchoBuf=NULL;
        }
#endif
        EchoCmd = DISABLE;
    }
}
#endif
//------------------------------------------------------------------//
// Set audio MixCtrl
// Parameter:
//          mixCtrl:MIX_CTRL_0 / MIX_CTRL_25 / MIX_CTRL_50 / MIX_CTRL_75 / MIX_CTRL_100
// return value:
//          NONE
//------------------------------------------------------------------//
#if _AUDIO_MIX_CONTROL
void SBC_SetMixCtrl(U8 mixCtrl){
    if(mixCtrl>MIX_CTRL_100) mixCtrl=MIX_CTRL_100;
    SbcMixCtrl=mixCtrl;
}
#endif
//------------------------------------------------------------------//
// Codec init
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SBC_Init(void){
    ChVol=CH_VOL_15;
#if _AUDIO_MIX_CONTROL
    SBC_SetMixCtrl(MIX_CTRL_100);
#endif
#if _SBC_SPEED_CHANGE||_SBC_PITCH_CHANGE
    SpeedCtl.speed_ratio_index=RATIO_INIT_INDEX;
    SpeedCtl.pitch_ratio_index=RATIO_INIT_INDEX;
#endif
#if defined(_SBC_RECORD_PLAY_USE_CUSTOMIZE)\
	||defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
	SBC_SetEQFilter(FILTER_NUMBER);
#endif
#if _SBC_ECHO_SOUND
    EchoCmd = DISABLE;
#endif
#if _SBC_RECORD_ERASING
    memset((void *)&RecordErasingCtrl,0,sizeof(RecordErasingCtrl));
#endif
}
//------------------------------------------------------------------//
// Start record voice
// Parameter:
//          index: file index or absolute address
//          mode:SPI_MODE,EF_MODE
//          size: record size
// return value:
//          0:ok
//          others:fail
//------------------------------------------------------------------//
#if _SBC_RECORD
U8 SBC_Record(U32 index,U8 mode,U32 size){
	AUDIO_ServiceLoop();
#if _USE_HEAP_RAM
    SBC_AllocateMemory(MEMORY_ALLOCATE_RECORD);
#if _SBC_RECORD_ERASING
	RecordErasingCtrl.record_buf = EraseRecordBuf;
#endif
#endif
    CB_SBC_AdcCmd(DISABLE);
    memset((void *)&AudioBufCtl, 0, sizeof(AudioBufCtl));

    AudioBufCtl.sample_rate=16000;
    //AudioBufCtl.out_size=SBC_DEC_OUT_SIZE;
    AudioBufCtl.in_size=40;
    AudioBufCtl.out_count=SBC_AUDIO_BUF_SIZE/SBC_DEC_OUT_SIZE;


#if NOISE_GATE_ENABLE
    NGInit(NG_RELEASE_SAMPLE,NG_ATTACK_SAMPLE,NG_ON_OFF_SAMPLE);
#endif

#if DC_REMOVE
	memset((void *)RunFilterBuf, 0, 8<<2);
#endif

#if AGC_CTL_ENABLE
    AgcCnt=0;
    AgcInit(ALPHA,BETA,TARGET_RMS);
    LimiterInit(LIMITER_THRESHOLD);
#endif

    CB_SetStartAddr(index,mode,1,&StartAddr);
    CurAddr=StartAddr+HEADER_LEN;
    StorageMode=mode;

    BufCtl.offset=0;
    BufCtl.size=SBC_AUDIO_BUF_SIZE;
    BufCtl.buf=AudioBuf;

    //SBCEncInit(AudioBufCtl.in_size*50*8,AudioBufCtl.sample_rate/2);
    SBCEncInit(SBCRam,AudioBufCtl.in_size*50*8,7000);

    AudioBufCtl.samples+= HEADER_LEN;

    CB_SBC_InitAdc(AudioBufCtl.sample_rate);

    OpMode=1;
    AudioBufCtl.state=STATUS_RECORDING;
    AudioBufCtl.shadow_state=STATUS_RECORDING;

    CB_SBC_RecStart();

	if (mode == SPI_MODE)
		while(SPI_CheckBusyFlag()) //non-busy
		{
			//dprintf("Wait0 busy\r\n");
		}
#if _SPI1_MODULE && _SPI1_USE_FLASH
	else if (mode == SPI1_MODE)
		while(SPI1_CheckBusyFlag()) //non-busy
		{
			//dprintf("Wait0 busy\r\n");
		}
#endif

#if _SBC_RECORD_ERASING
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
    #if _ADPCM_PLAY && _ADPCM_PLAY_SR_FOR_SBC_REC && (_ADPCM_PLAY_AUDIO_CH==HW_CH0)
		INTC_IrqCmd(~(IRQ_BIT_ADC|IRQ_BIT_RTC|IRQ_BIT_DAC_CH0),DISABLE);
    #elif _ADPCM_PLAY && _ADPCM_PLAY_SR_FOR_SBC_REC && (_ADPCM_PLAY_AUDIO_CH==HW_CH1)
        INTC_IrqCmd(~(IRQ_BIT_ADC|IRQ_BIT_RTC|IRQ_BIT_DAC_CH1),DISABLE);
    #else
		INTC_IrqCmd(~(IRQ_BIT_ADC|IRQ_BIT_RTC),DISABLE);
    #endif
		GIE_DISABLE();
		P_SMU->INTV_BA = C_INTV_BA_En;
		GIE_ENABLE();
	}
#endif
	if ((mode == SPI_MODE) || (mode == SPI1_MODE))
	{
		RecordErasingCtrl.total_count = (size+(SPI_SECTOR_SIZE-1))/SPI_SECTOR_SIZE;
	}
#endif

    CB_SBC_AdcCmd(ENABLE);
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
void SBC_AdcIsr(U8 size, U32 *pdest32){
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
        //dprintf("size,%d\r\n",size);
        if((AudioBufCtl.fifo_count+size-AudioBufCtl.process_count)>SBC_AUDIO_BUF_SIZE){
            for(i=0;i<size;i++){
                temp=((*pdest32++)-32768);
            }
            size=0;
        }
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
    //U16 j;
    //S16 *temp,sample,gain;
    //U32 sum;
//    if(AudioBufCtl.index){
//        audio_buf=AudioBuf+SBC_DEC_OUT_SIZE;
//    }else{
//        audio_buf=AudioBuf;
//    }
//    AudioBufCtl.index^=1;
    audio_buf=(S16 *)AudioBuf+SBC_DEC_OUT_SIZE*AudioBufCtl.index;

    AudioBufCtl.index++;
    if(AudioBufCtl.index==AudioBufCtl.out_count){
        AudioBufCtl.index=0;
    }

#if AGC_CTL_ENABLE
    LimiterProcess(audio_buf,audio_buf,LIMITER_THRESHOLD,SBC_DEC_OUT_SIZE,0);
#endif

#if DC_REMOVE
    U16 i;
	for(i=0;i<SBC_DEC_OUT_SIZE;i++)//unit word
	{
		RunFilter(&audio_buf[i], RunFilterBuf, DCRemove);
	}
#endif

#if NOISE_GATE_ENABLE
    NGProcess(audio_buf,audio_buf,SBC_DEC_OUT_SIZE>>1);
    NGProcess(audio_buf+(SBC_DEC_OUT_SIZE>>1),audio_buf+(SBC_DEC_OUT_SIZE>>1),SBC_DEC_OUT_SIZE>>1);
#endif

    SBCEncode(SBC_DEC_OUT_SIZE,(U16*)audio_buf,DataBuf);

    AudioBufCtl.process_count+=SBC_DEC_OUT_SIZE;

#if _SBC_RECORD_ERASING
    if(RecordErasingCtrl.status == C_RECORD_ERASE_START && (RecordErasingCtrl.waiting_channel & C_RECORD_RECORD_SBC))
    {
        RecordErasingCtrl.record_buf_index = 0;
        RecordErasingCtrl.record_buf = EraseRecordBuf;
        RecordErasingCtrl.waiting_channel &= ~C_RECORD_RECORD_SBC;
		memcpy(&RecordErasingCtrl.record_buf[RecordErasingCtrl.record_buf_index],DataBuf,AudioBufCtl.in_size);
        RecordErasingCtrl.record_buf_index += AudioBufCtl.in_size;
    }
    else if(RecordErasingCtrl.status == C_RECORD_ERASE_RUN)
    {
        memcpy(&RecordErasingCtrl.record_buf[RecordErasingCtrl.record_buf_index],DataBuf,AudioBufCtl.in_size);
        RecordErasingCtrl.record_buf_index += AudioBufCtl.in_size;
    }
    else
    {
        CB_WriteData(StorageMode,&CurAddr,(U8 *)DataBuf,AudioBufCtl.in_size);
    }
#else
    CB_WriteData(StorageMode,&CurAddr,(U8 *)DataBuf,AudioBufCtl.in_size);
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
void SBC_SkipTailSetting(U16 ms)
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
static U16 SBC_SkipTail_Samples(U16 now_sample_rate)
{
	U16 skip_samples = (now_sample_rate/1000)*SkipTailTime;//calculate tail samples
	return (skip_samples+319-((skip_samples+319)%320));
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

    pheader=(struct AUDIO_HEADER *)DataBuf;

    memset(pheader,0,HEADER_LEN);

    pheader->bit_per_frame=AudioBufCtl.in_size*8;
    pheader->sample_rate=AudioBufCtl.sample_rate;
    pheader->header_size=HEADER_LEN;
    pheader->file_size=AudioBufCtl.samples;
    pheader->codec=CODEC_SBC;
    //pheader->bandwidth=AudioBufCtl.sample_rate/2;
    pheader->bandwidth=7000;

    memcpy(pheader->signature,Signature,sizeof(Signature));

    //pheader->samples=AudioBufCtl.process_count;
    U16 SkipTailSamples = SBC_SkipTail_Samples(pheader->sample_rate);
    if(AudioBufCtl.process_count > SkipTailSamples)
    {
    	pheader->samples=AudioBufCtl.process_count - SkipTailSamples;
    }
    else
    {
    	pheader->samples=320;//min frame samples
    }
	pheader->flag |= 0x1;

    CurAddr=StartAddr;
    CB_WriteHeader(StorageMode,&CurAddr,(U8 *)DataBuf,HEADER_LEN);
}
#if _SBC_RECORD_ERASING
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
void SBC_GetRecordEraseCtrl(U32 *recorderasingctrl)
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
void SBC_StartRecordErase(U32 address, U32 length, U16 chan)
{
    memset((void *)&RecordErasingCtrl,0,sizeof(RecordErasingCtrl));
    RecordErasingCtrl.status            = C_RECORD_ERASE_START;
    RecordErasingCtrl.start_addr        = address;
    RecordErasingCtrl.length            = length;
    RecordErasingCtrl.erase_addr        = address;
    RecordErasingCtrl.active_channel    |= chan;
    RecordErasingCtrl.waiting_channel   = RecordErasingCtrl.active_channel;

    //dprintf("-------------------  RecordErasingCtrl  ---------------\r\n");
    //dprintf("start_addr 0x%x\r\n",RecordErasingCtrl.start_addr);
    //dprintf("length 0x%d\r\n",RecordErasingCtrl.length);
    //dprintf("chan %d\r\n",RecordErasingCtrl.active_channel);

}
//------------------------------------------------------------------//
// Stop Record Erase
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SBC_StopRecordErase(void)
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
U16 SBC_GetRecordErase(void)
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
void SBC_RecordEraseServiceloop(void)
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
            //dprintf("(Run)Erase Done length %d\r\n",RecordErasingCtrl.record_buf_index);

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
            //dprintf("(Start)Erase Address 0x%x\r\n",RecordErasingCtrl.erase_addr);
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
                //dprintf("Erase Idle\r\n");
            }
        }
    }
}
#endif
#endif
//------------------------------------------------------------------//
// Service loop
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SBC_ServiceLoop(void){

#if _SBC_RECORD && _SBC_RECORD_ERASING
    SBC_RecordEraseServiceloop();
#endif

	if(AudioBufCtl.shadow_state==STATUS_PLAYING){

        AudioVol=  (ChVol+1)*(MixerVol+1);

        if(!ChVol){
            AudioVol=0;
        }
        if(AudioVol==256){
            AudioVol=255;
        }
        AudioVol=AudioVol<<7;
#if _AUDIO_VOLUME_CHANGE_SMOOTH
        AUDIO_SetVolumeSmooth(&VolCtrl,AudioVol);
#endif


#if _SBC_SPEED_CHANGE||_SBC_PITCH_CHANGE
        if((AudioBufCtl.fifo_count+(BufCtl.size-SPEED_CTL_OUT_SIZE)-AudioBufCtl.process_count)>=0){
            //dprintf("%d,%d,%d\r\n",AudioBufCtl.fifo_count,AudioBufCtl.process_count,AudioBufCtl.toggle);
            //if(AudioBufCtl.process_count<AudioBufCtl.samples){
                DecodeFrame();
            //}
        }
#else
        //if((AudioBufCtl.fifo_count+(SBC_DEC_OUT_SIZE*AUDIO_BUF_COUNT)-AudioBufCtl.process_count)>=SBC_DEC_OUT_SIZE){
	#if _SBC_AUDIO_BUF_X3
        if((AudioBufCtl.fifo_count+SBC_DEC_OUT_SIZE*2-AudioBufCtl.process_count)>=0){
	#else
        if((AudioBufCtl.fifo_count+SBC_DEC_OUT_SIZE-AudioBufCtl.process_count)>=0){
	#endif
            //dprintf("%d,%d,%d\r\n",AudioBufCtl.fifo_count,AudioBufCtl.process_count,AudioBufCtl.toggle);
            if(AudioBufCtl.process_count<AudioBufCtl.samples){
                DecodeFrame();
			#if _SBC_AUDIO_BUF_X3
				if(AudioBufCtl.preDecFrame){
					AudioBufCtl.preDecFrame = 0;
				}
			#endif
            }
		#if _SBC_AUDIO_BUF_X3
			else {
				if (SBC_PlayRepeatFlag) {
					if ((!AudioBufCtl.preDecFrame) && (AudioBufCtl.decode_state==2)) {
						U8 tempBuf[HEADER_LEN];
						struct AUDIO_HEADER *pheader;
						CurAddr=StartAddr;
						CB_ReadData(StorageMode,&CurAddr,(U8 *)tempBuf,HEADER_LEN);
						pheader=(struct AUDIO_HEADER *)tempBuf;
						SBCDecInit(0,SBCRam,pheader->sample_rate,pheader->bandwidth,pheader->bit_per_frame&0x7FFF,pheader->decode_in_size,pheader->decode_out_size,pheader->bit_per_frame>>15);
						if(pheader->codec==CODEC_SBC_E && pheader->samples>SBC_DEC_OUT_SIZE){
							CB_ReadData(StorageMode,&CurAddr,(U8 *)(SBCRam+320),160<<1);
						}
						AudioBufCtl.preDecFrame = 1;
						AudioBufCtl.preDecCount = 0;
#if _SBC_QIO_MODULE
						QIO_ReGetStorageDataSet(QIO_USE_SBC_CH1);
#endif//_SBC_QIO_MODULE
					}

					if((AudioBufCtl.preDecFrame == 1) || (AudioBufCtl.preDecFrame == 2)){
						DecodeFrame();
						DecodeFrame();
						AudioBufCtl.preDecFrame++;
					}
				}
			}
		#endif
        }
#endif
    }

#if _SBC_RECORD
    if(AudioBufCtl.state==STATUS_RECORDING){
        if((AudioBufCtl.fifo_count-AudioBufCtl.process_count)>=SBC_DEC_OUT_SIZE){
            //dprintf("%d\r\n",VoiceIdx);
            EncodeFrame();
        }
    }
#endif

#if _USE_HEAP_RAM
#if _SBC_RECORD
    if((AudioBufCtl.shadow_state==STATUS_STOP) && (AudioBufCtl.state!=STATUS_RECORDING) && SBCRam!=NULL )
#else
    if((AudioBufCtl.shadow_state==STATUS_STOP) && SBCRam!=NULL )
#endif
    {
        //dprintf("shadow_state %x state %x SBCRam %x\r\n",AudioBufCtl.shadow_state,AudioBufCtl.state,SBCRam);
        SBC_FreeMemory(MEMORY_ALLOCATE_PLAY);
    }
#endif
    if(     AudioBufCtl.shadow_state!=STATUS_PLAYING
        &&  DacAssigner_codecUseDacCh(kSBC_CH1)>=0
    ){
        GIE_DISABLE();
		DacAssigner_unRegist(kSBC_CH1);
		GIE_ENABLE();
		//dprintf("Free codec %d hw_ch\r\n",kSBC_CH1);
    }
	if (PlayEndFlag == BIT_SET) {
		PlayEndFlag = BIT_UNSET;

//----------------------------------------------------------------------------//
// 增加宏设置sbc播放结束时是否清除播放断点 2023-09-14 添加
//----------------------------------------------------------------------------//
#if _SBC_AUTO_SAVE_TIME && USER_SBC_PLAY_END_CLEAN_BREAK_POINT
        SBC_AutoSave_Reset(StorageMode);
#endif

		CB_SBC_PlayEnd();
	}
}
#if _SBC_AUDIO_BUF_X3
void RepeatReSetting(U8 fifoSize, U8 size)
{
	AudioBufCtl.process_count = AudioBufCtl.preDecCount;

	if (BufCtl.offset % SBC_DEC_OUT_SIZE) {
		BufCtl.offset = ((BufCtl.offset + SBC_DEC_OUT_SIZE)/SBC_DEC_OUT_SIZE)*SBC_DEC_OUT_SIZE;
		if (BufCtl.offset > (SBC_DEC_OUT_SIZE*2)) {
			BufCtl.offset = 0;
		}
	}

	U8 i;
	U16 tempbuf[fifoSize];
	U16 *psrc16 = BufCtl.buf + BufCtl.offset;
 	for (i = 0; i < fifoSize; i++) {
		tempbuf[i] = 0;
 		if (i >= size) {
			tempbuf[i] = *psrc16;
            psrc16++;
		}
	}
	CB_SBC_WriteDac(fifoSize, tempbuf);
	BufCtl.offset += (fifoSize - size);
	AudioBufCtl.fifo_count = (fifoSize - size);

	AudioBufCtl.state = STATUS_PLAYING;
    AudioBufCtl.shadow_state = AudioBufCtl.state;
}
#endif
#if	_SBC_QIO_MODULE
#if _SBC_SPEED_CHANGE||_SBC_PITCH_CHANGE
#error	"QIO does not support _SBC_SPEED_CHANGE, _SBC_PITCH_CHANGE"
#endif
#endif//_SBC_QIO_MODULE

#if _SBC_AUTO_SAVE_TIME
#if _SBC_SPEED_CHANGE||_SBC_PITCH_CHANGE
#error	"_SBC_AUTO_SAVE_TIME does not support _SBC_SPEED_CHANGE, _SBC_PITCH_CHANGE"
#endif
#if _STORAGE_MODULE==0
#error	"_SBC_AUTO_SAVE_TIME must turn on _STORAGE_MODULE to save data"
#endif
#if _AUDIO_SENTENCE
#warning "_SBC_AUTO_SAVE_TIME does not support sentence play"
#endif
#if _SBC_AUDIO_BUF_X3
#warning "_SBC_AUTO_SAVE_TIME does not support repeat/loop play"
#endif

#endif

//----------------------------------------------------------------------------//
//  USER_STORAGE 模块获取sbc播放断点相关信息指针 2023-09-14添加
//----------------------------------------------------------------------------//
#if USER_STORAGE_BREAK_POINT_MOUDLE && _STORAGE_MODULE && USER_CUSTOMIZE_BREAK_POINT_CMD
    void user_storage_get_sbc_breakpoint_val(void)
    {
        gu32_p_SbcSeekCtl_CurAddr = &SbcSeekCtl.CurAddr;
        gu32_p_SbcSeekCtl_process_count = &SbcSeekCtl.process_count;
        gu16_p_SbcSeekCtl_rand = &SbcSeekCtl.rand[0];
        gu32_p_CurAddr = &CurAddr;
        gu16_p_SBCRam = SBCRam;
        gs32_p_AudioBufCtl_process_count = &AudioBufCtl.process_count;
    }
#endif

#endif//_SBC_PLAY
