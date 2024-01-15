/* Includes ------------------------------------------------------------------*/
#include "string.h"
#include "include.h"
#include "nx1_dac.h"
//#include "nx1_gpio.h"

#define ABS(a) (((a) < 0) ? -(a) : (a))
#if _RT_PLAY
/* Defines -------------------------------------------------------------------*/
#if VOICE_CHANGER_RT_USED
#define AGC_ENALBE					(VC_RT_AGC_FUNC)
#define TARGET_RMS					(VC_RT_AGC_TARGET_RMS)
#define ALPHA						(VC_RT_AGC_ALPHA)
#define BETA						(VC_RT_AGC_BETA)

#define ECHO_CANCELL				(VC_RT_ECHO_CANCEL_FUNC)

#define NOISE_GATE_ENABLE			(VC_RT_NOISE_GATE_FUNC)
#define NG_ON_OFF_SAMPLE_16K		(VC_RT_NG_ON_OFF_SAMPLE_16K)
#define NG_ATTACK_SAMPLE_16K		(VC_RT_NG_ATTACK_SAMPLE_16K)
#define NG_RELEASE_SAMPLE_16K		(VC_RT_NG_RELEASE_SAMPLE_16K)
#define NG_ON_OFF_SAMPLE_8K			(VC_RT_NG_ON_OFF_SAMPLE_8K)
#define NG_ATTACK_SAMPLE_8K			(VC_RT_NG_ATTACK_SAMPLE_8K)
#define NG_RELEASE_SAMPLE_8K		(VC_RT_NG_RELEASE_SAMPLE_8K)
#else
#define AGC_ENALBE          		1//0

#define TARGET_RMS     				MINUS_7DB                           // Adjustable, Target RMS signal magnitude 			
#define ALPHA           			4096//4096//4096                    // 0.125; coeff. of 1st order IIR LPF 
#define BETA            			1024//(2048/2)                      // 0.125; step size constant 

#define ECHO_CANCELL        		0								
#define NOISE_GATE_ENABLE   		1//0

#define NG_ON_OFF_SAMPLE_16K        160//320//160//10ms
#define NG_ATTACK_SAMPLE_16K        80//160//80//5ms
#define NG_RELEASE_SAMPLE_16K		(320*5)//100ms

#define NG_ON_OFF_SAMPLE_8K         80//10ms
#define NG_ATTACK_SAMPLE_8K         40//5ms
#define NG_RELEASE_SAMPLE_8K        (160*5)//100ms

#if _RT_DIGITAL_GAIN_CTRL
static const S16 RTDigiAmpArray[14] = {
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
	1,      //Gain status, 0 ~ 2
	10,     //(%), unvoice段落 decrease MinGain 
	100,   //(%), unvoice段落 decrease MaxGain 
	200,   //unvoice段落 decrease GainReleaseVel=1(fast)~256(slow)	
};
static const U16 RTVadArray[9] = {
	
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
static const short RTDigiAmpSet[2] = {
	 
	32,     //frame size
	0,       //sample base or frame base, value 0 or 1
};	
#endif
#endif //VOICE_CHANGER_RT_USED

#define SMOOTH_ENABLE          		1

#define RT_AL_PITCH					0
#define RT_AL_OTHER					1
#define RT_AL_ECHO					2
#define RT_AL_GHOST					3
#define RT_AL_DARTH				    4
#define RT_AL_CHORUS				5
#define RT_AL_NO_SFX				6
//-----------------------------------------//
#if _RT_HOWLING_SUPPRESS
U8	U8_RT_Howling_Suppress_buf=0;	 
U8 	U8_RT_Howling_Suppress_Swtich=1;
U8 	U8_RT_Howling_Suppress_Flag=0;
U8	RT_Howling_Suppress_GetStatus(void);
static U8 Howling_start_sts=0;
void RT_Howling_Suppress_Init(void);
void RT_Howling_Suppress_ALG_Limit(void);
void RT_Howling_Suppress_Function(U8 cmd);
void RT_Howling_Suppress_Switch(void);
void RT_Howling_Suppress_ServiceLoop(S16 *Audio_buf_ptr);
#endif //_RT_HOWLING_SUPPRESS
//-----------------------------------------//

static const S16 NoiseGateArray_1[/*9*/7] = {
	/*Limitation*/
	100,    //(%),Max Avg Limit
	6,      //(%),Min Avg Limit 
	/* Initialize*/
	0,      // Average Value Update initialize
	/* Start Detection */
	//14,//8,		// Average history velocity, Threshold更新速度: 0(固定閾值),1(更新慢)~32(更新快).
	1000,		//3000,	// Voice AVG threshold :0~32767 
	2,		// Start Detect condition : Voice duration(frames) 
	0,		// Start Detect condition : Voice max peak :0~32767 
	/* End Detection */
	//10,//18,//7,		// End Detect condition : Voice duration(frames)  
	3,      //aspirated voice keep level  3 ~ 5; 
};

// default SR 8000, FrameSize 64
static S16 NoiseGateArray_2[2] = {
	/* Start Detection */
	14,// Average history velocity, Threshold更新速度: 0(固定閾值),1(更新慢)~32(更新快).
	/*End Detection */
	10,// End Detect condition : Voice duration(frames)  
};
#if _RT_ECHO_SOUND&&(_RT_ECHO_EFFECT==DELAY_USER_DEFINE)&&(((!_RT_ECHO_UD_DELAY_TIME)&&(_RT_ECHO_UD_REPEAT_TIME))||((_RT_ECHO_UD_DELAY_TIME)&&(!_RT_ECHO_UD_REPEAT_TIME)))
	#error "ECHO effect user define mode, the _RT_ECHO_UD_DELAY_TIME & _RT_ECHO_UD_REPEAT_TIME must both be set!"
#endif

#define DC_REMOVE				1

/* Static Functions ----------------------------------------------------------*/
//static short RTPitchTempBuf[1280]; 
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

//#if _RT_PITCH_CHANGE
//static const float PitchRatioTbl[25]={
//    2,//0.5,
//    1.89,//0.53,
//    1.78,//0.56,
//    1.68,//0.59,
//    1.59,//0.63,
//    1.50,//0.67,
//    1.41,//0.72,
//    1.33,//0.75,
//    1.26,//0.79,
//    1.19,//0.84,
//    1.12,//0.89,
//    1.06,//0.94,
//    1.04,//1.03,//1
//    0.94,//1.06,
//    0.89,//1.12,
//    0.84,//1.19,
//    0.79,//1.26,
//    0.75,//1.33,
//    0.72,//1.41,
//    0.67,//1.50,
//    0.63,//1.59,
//    0.59,//1.68,
//    0.56,//1.78,
//    0.53,//1.89,
//    0.5//2,
//};  
//#endif
#if _RT_ROBOT1_SOUND 
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

#if _RT_CHORUS_SOUND 
static const float ChorusVolGain[11]={
    1.0,
    1.1,
    1.2,
    1.3,
    1.4,
    1.5,
    1.6,
    1.7,
    1.8,
    1.9,
    2.0,
};    
#endif 

static const U16 PitchInTbl[25]={
    320/2,
    302/2,
    284/2,
    268/2,
    254/2,
    240/2,
    225/2,
    212/2,
    201/2,
    190/2,
    179/2,
    169/2,
    166/2,		
    300/2,
    284/2,
    268/2,
    252/2,
    240/2,
    230/2,
    214/2,
    201/2,
    188/2,
    179/2,
    169/2,
    160/2
};
static const U16 PitchOutTbl[25]={
    160/2,
    160/2,
    160/2,
    160/2,
    160/2,
    160/2,
    160/2,
    160/2,
    160/2,
    160/2,
    160/2,
    160/2,
    160/2,
    320/2,
    320/2,
    320/2,
    320/2,
    320/2,
    320/2,
    320/2,
    320/2,
    320/2,
    320/2,
    320/2,
    320/2
};

static const U16 PitchOutSrTbl[25]={
    8000, 
    8466,
    8989,
    9524,
    10063,
    10667,
    11348,
    12030,
    12698,
    13445,
    14286,
    15094,
    15384,	
    17021,
    17978,
    19048,
    20253,
    21333,
    22222,
    23881,
    25397,
    27119,
    28571,
    30189,
    32000
};

// first buffer different
static const U16 FirstBufferDiff[25]={
    0,
    252,
    200,
    316,
    276,
    0,
    368,
    328,
    272,
    468,
    432,
    396,
    668,
    500,
    456,
    468,
    476,
    0,
    708,
    664,
    772,
    736,
    876,
    852,
    0   
};

// buffer different offset1
static const U16 BuffDiffOffset1[25]={
    0, 
    16,
    40,
    64,
    96,
    0,
    56,
    56,
    56,
    56,
    56,
    96,
    320, 
    80,
    56,
    56,
    56,
    0,
    56,
    144,
    144,
    192,
    208,
    240,
    0   
};

// buffer different offset2
static const U16 BuffDiffOffset2[25]={
    0, 
    8,
    24,
    32,
    48,
    0,
    32,
    32,
    32,
    32,
    32,
    48,
    160, 
    40,
    32,
    32,
    32,
    0,
    32,
    72,
    72,
    96,
    104,
    120,
    0   
};

/* Static Variables ----------------------------------------------------------*/
static volatile struct AUDIO_BUF_CTL{
    S32 process_count;	//decode/encode count
    S32 fifo_count;	    //decode/encode count
    U32 samples;	    //samples      
    //U16 vol;            //volume
//    U16 sample_rate;    //sample_rate
    U16 out_size;        //audio frame size
    U16 in_size;         //data frame size
    //U8 state;           //channel state
    //U8 shadow_state;    //channel shadow state
//    U8 ramp;            //smooth ramp
    U8 out_count;
    U8 index;
    U8 start_flag;
	U8 dac_state;
	U8 adc_state;
}AudioBufCtl;
//
//#if _PCM_RECORD
//static U32 StorageIndex;
//#endif

#if DC_REMOVE
static S32 RunFilterBuf[8];
#endif

#if AGC_ENALBE
static U8 AgcCnt;
#endif

NoiseGateBuf_v2 NoiseGatebuf_v2;
//static U32 RecordSize;

static U16 AudioVol;
//static U8  OpMode;
static U8  ChVol;
static volatile U8  Flag_switch_AL=0;

/*typedef struct{
    U16 *buf;
    U16 size;
    U16 offset;
}BUF_CTL_T;   

static volatile  BUF_CTL_T BufCtl, DacBufCtl;*/

static volatile struct{
	U16 *buf;
	U16 size;
	U16 offset;
}BufCtl;

static volatile struct{
	S16 *buf;
	U16 size;
	U16 offset;
}DacBufCtl;

static volatile U16 DacBufWrOffset;
static volatile U32 DacBufWrCnt;

#if SMOOTH_ENABLE
static volatile struct{
    S16 count;              //sample count in process
    U8  step_sample;        //sample size for each step
    U8  smooth;             //up or down or none
    U8  state;              //current state
}SmoothCtl;
#endif

static const U8 Signature[]="NX1";

#if _RT_ROBOT1_SOUND 
static struct{
    U8 enable;
    U8 change;
    U8 index;
}RobotCtl;
#endif 

#if _RT_ROBOT2_SOUND
static struct{
    U8 enable;
    U8 type;
    U16 thres;
    U16 detectthres;
}Robot01Ctl;
#endif

#if _RT_ROBOT3_SOUND
static struct{
    U8 enable;
    U8 type;
    U16 thres;
}Robot02Ctl;
#endif

#if _RT_ROBOT4_SOUND
static struct{
    U8 enable;
}Robot04Ctl;
#endif

#if _RT_REVERB_SOUND
static U8 ReverbCmd,ReverbEffect;
#endif

#if _RT_ECHO_SOUND 
static struct{
    U8	Cmd;
}EchoCtrl;

static volatile NewEchoCtrl echoCtrl_RT;
#endif

#if _RT_GHOST_SOUND
static struct{
    U8 Cmd;
    S8 Effect;
    U8 EnvelopeType;
}GhostCtl;
#endif

#if _RT_DARTH_SOUND
static struct{
    U8 Cmd;
    S8 Pitchfactor;
    S8 Effect;
}DarthCtl;
#endif

#if _RT_CHORUS_SOUND
static struct{
    U8 Cmd;
}ChorusCtl; 

#endif

//static U8  StorageMode;
//static U32 StartAddr,CurAddr;

U8 RT_PlayFlag;

#if _RT_DIGITAL_GAIN_CTRL
static S16 RTDigiAmpBuf[36];
static S16 RTDigiAmpCTLBuf[34];
#endif 

/* Dynamic Allocate RAM-------------------------------------------------------*/
#if _USE_HEAP_RAM
static U16	*AudioBuf=NULL;
static S16	*DacBuf=NULL;

#if _RT_PITCH_CHANGE || _RT_GHOST_SOUND || _RT_DARTH_SOUND || _RT_CHORUS_SOUND
static S16	*PitchBuf=NULL;
#endif

#if _RT_ECHO_SOUND
static S16	*EchoBuf=NULL;	
#endif

#if _RT_ROBOT2_SOUND
static S16 *Robot01_buf=NULL;
#endif

#if _RT_ROBOT3_SOUND

static S16 *Robot02_buf=NULL;
#endif

#if _RT_ROBOT4_SOUND
ROBOT4Ctl* RTRobot4ctl = NULL;
ROBOT4Buf* RTRobot4buf = NULL;
FFTctl* RTRobot4FFTctl = NULL;
#endif

#if _RT_REVERB_SOUND
static S16 *Reverb_x=NULL;
static S16 *Reverb_y=NULL;
#endif

#if _RT_GHOST_SOUND
static S32 *EnvelopTHTemp=NULL;
static S16 *Ghost_Reverb_x=NULL;
static S16 *Ghost_Reverb_y=NULL;
#endif

#if _RT_DARTH_SOUND
const unsigned char StepSizeTb[4] = { 4, 5, 6, 5 };//, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2}; //30 -> 60 -> 90 -> 60 -> 30Hz...
RT_DarthVaderBuf *RT_DarthBuf = NULL;
RT_DarthVaderCtl *RT_DarthCtl = NULL;
#endif

#if _RT_CHORUS_SOUND
S16 *ChorusBuf = NULL;
RTChorusCtl* RT_ChorusCtl = NULL;
#endif

#if AGC_ENALBE
static U16 *AgcBuf=NULL;
#endif
//---------------------------------
// Howling Suppress
#if _RT_HOWLING_SUPPRESS
static S16 *HRInCtrl = NULL;
static FFTctl* HRFFTCtrl = NULL;
static HRCtlBuf* HRInBuf = NULL;
#endif
/* Fixed RAM  ---------------------------------------------------------------*/
#else
#if (_IC_BODY!=0x12A88)&&(_RT_ECHO_SOUND==ENABLE)&&(_RT_ECHO_EFFECT==DELAY_1200ms)&&(_RT_PLAY==ENABLE)
#error "Over RAM size, please ENABLE _USE_HEAP_RAM."
#endif

static  U16 AudioBuf[RT_AUDIO_BUF_SIZE];
static  S16 DacBuf[DAC_BUF_SIZE];
#if _RT_PITCH_CHANGE || _RT_GHOST_SOUND || _RT_DARTH_SOUND || _RT_CHORUS_SOUND
static S16 PitchBuf[RT_PITCH_BUF_SIZE];
#endif
#if _RT_ECHO_SOUND
static S16	EchoBuf[RT_ECHO_BUF_SIZE];	
#endif
#if _RT_ROBOT2_SOUND
static S16 Robot01_buf[RT_ROBOT_BUF_SIZE];
#endif

#if _RT_ROBOT3_SOUND
static S16 Robot02_buf[RT_ROBOT02_BUF_SIZE];
#endif

#if _RT_ROBOT4_SOUND
ROBOT4Ctl  _RTRobot4ctl;
ROBOT4Ctl* RTRobot4ctl = &_RTRobot4ctl;
ROBOT4Buf _RTRobot4buf;
ROBOT4Buf* RTRobot4buf = &_RTRobot4buf;
FFTctl  _RTRobot4FFTctl;
FFTctl* RTRobot4FFTctl = &_RTRobot4FFTctl;
#endif

#if _RT_REVERB_SOUND
static S16 Reverb_x[RT_REVERB_BUF_SIZE];
static S16 Reverb_y[RT_REVERB_BUF_SIZE];
#endif

#if _RT_GHOST_SOUND
static S32 EnvelopTHTemp[RT_GHOST_ENVELOPETH_SIZE];
static S16 Ghost_Reverb_x[RT_GHOST_REVERB_BUF_SIZE];
static S16 Ghost_Reverb_y[RT_GHOST_REVERB_BUF_SIZE];
#endif

#if _RT_DARTH_SOUND
const unsigned char StepSizeTb[4] = { 4, 5, 6, 5 };//, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2}; //30 -> 60 -> 90 -> 60 -> 30Hz...                                
RT_DarthVaderBuf   rt_darthvaderbuf;
RT_DarthVaderBuf*  RT_DarthBuf = &rt_darthvaderbuf;
RT_DarthVaderCtl   rt_darthvaderctl;
RT_DarthVaderCtl*  RT_DarthCtl = &rt_darthvaderctl;
#endif

#if _RT_CHORUS_SOUND
S16 ChorusBuf[RT_CHORUS_BUF_SIZE];
RTChorusCtl _RTChorusCtl;
RTChorusCtl* RT_ChorusCtl= &_RTChorusCtl;
#endif

#if AGC_ENALBE
static U16 AgcBuf[AGC_FRAME_SIZE];
#endif

//---------------------------------
// Howling Suppress
#if _RT_HOWLING_SUPPRESS
static S16 HRInCtrl[4];
static FFTctl _HRFFTCtrl;
static FFTctl* HRFFTCtrl = &_HRFFTCtrl;
static HRCtlBuf  _HRInBuf;
static HRCtlBuf* HRInBuf = &_HRInBuf;
#endif
//---------------------------------
#endif

static U8 RT_InternalPlay(void);
/* Extern Functions ----------------------------------------------------------*/
extern void CB_RT_PlayStart(void);
extern void CB_RT_InitDac(CH_TypeDef *chx,U16 sample_rate);
extern void CB_RT_DacCmd(CH_TypeDef *chx,U8 cmd);
extern void CB_RT_ChangeSR(CH_TypeDef *chx,U16 sample_rate);
extern void CB_RT_InitAdc(U16 sample_rate);
extern void CB_RT_AdcCmd(U8 cmd);
extern void CB_RT_WriteDac(U8 size,U16* psrc16);


#if _RT_DIGITAL_GAIN_CTRL
extern void RTDigiAmpProcess(S16 *in, S16 *buf, S16 samples, short *RTAgcBuf);
extern void RTDigiAmpInit(const short *RT_AGC_Table,const  short *RT_AGC_Table_1, S16 *tempbuf, short *RT_AGCBuf);
extern void RTVadInit(const U16* RTVadTable, short* RTVadBuf);
#endif
																		   
//------------------------------------------------------------------//
// Allocate PCM memory
// Parameter: 
//          func: Choose Allocate function
// return value:
//          NONE
//------------------------------------------------------------------// 
#if _USE_HEAP_RAM
static void RT_AllocateMemory(U16 func)
{
	if(AudioBuf==NULL && (func&MEMORY_ALLOCATE_PLAY))    // AudioBuf
	{
        if (Flag_switch_AL==RT_AL_NO_SFX)
        {
        	AudioBuf = (U16 *)MemAlloc(RT_AUDIO_BUF_SIZE_03<<1);       
			DacBuf = (S16 *)MemAlloc(DAC_BUF_SIZE_03<<1);
        }
	#if _RT_PITCH_CHANGE || _RT_GHOST_SOUND || _RT_DARTH_SOUND || _RT_CHORUS_SOUND
    	else if ((Flag_switch_AL==RT_AL_PITCH) || (Flag_switch_AL==RT_AL_GHOST) ||\
          	     (Flag_switch_AL==RT_AL_DARTH) || (Flag_switch_AL==RT_AL_CHORUS))
		{
			AudioBuf = (U16 *)MemAlloc(RT_AUDIO_BUF_SIZE_01<<1);       
			DacBuf = (S16 *)MemAlloc(DAC_BUF_SIZE_01<<1);
		}
	#endif	
	#if _RT_REVERB_SOUND || _RT_ROBOT1_SOUND || _RT_ROBOT2_SOUND || _RT_ROBOT3_SOUND ||_RT_ROBOT4_SOUND
		else if (Flag_switch_AL==RT_AL_OTHER)
		{
			AudioBuf = (U16 *)MemAlloc(RT_AUDIO_BUF_SIZE_02<<1);      
			DacBuf = (S16 *)MemAlloc(DAC_BUF_SIZE_02<<1);		
			
		#if  _RT_ROBOT4_SOUND
			if(RTRobot4ctl==NULL)
				RTRobot4ctl = (ROBOT4Ctl*)MemAlloc(RT_ROBOT4_CTL_SIZE);
			if(RTRobot4buf==NULL)
			    RTRobot4buf = (ROBOT4Buf*)MemAlloc(RT_ROBOT4_BUF_SIZE);
			if(RTRobot4FFTctl==NULL)
			    RTRobot4FFTctl = (FFTctl*)MemAlloc(RT_ROBOT4_FFT_CTL_SIZE);
		#endif  
		}
	#endif
	#if _RT_ECHO_SOUND 
		else if (Flag_switch_AL==RT_AL_ECHO)
		{
			AudioBuf = (U16 *)MemAlloc(RT_AUDIO_BUF_SIZE_03<<1);  
			DacBuf = (S16 *)MemAlloc(DAC_BUF_SIZE_03<<1);					
		}
	#endif
	#if AGC_ENALBE
		AgcBuf = (U16 *)MemAlloc(AGC_FRAME_SIZE<<1);
	#endif 
	}					
}
//------------------------------------------------------------------//
// Free RT memory
// Parameter: 
//          func: Choose Allocate function
// return value:
//          NONE
//------------------------------------------------------------------// 
static void RT_FreeMemory(U16 func)
{
    if(AudioBuf!=NULL && (func&MEMORY_ALLOCATE_PLAY))    
    {
        MemFree(AudioBuf);
        AudioBuf=NULL;
#if AGC_ENALBE
        MemFree(AgcBuf);
        AgcBuf=NULL;
#endif        
        MemFree(DacBuf);
        DacBuf = NULL;
		
#if _RT_PITCH_CHANGE || _RT_GHOST_SOUND || _RT_DARTH_SOUND || _RT_CHORUS_SOUND
        if(PitchBuf!=NULL)
        {
            MemFree(PitchBuf);
            PitchBuf=NULL;
        }
#endif
		
#if _RT_ROBOT2_SOUND    
        if(Robot01_buf!=NULL)
        {
            MemFree(Robot01_buf);
            Robot01_buf = NULL;
        }
#endif
#if _RT_ROBOT3_SOUND    
        if(Robot02_buf!=NULL)
        {
            MemFree(Robot02_buf);
            Robot02_buf = NULL;
        }
#endif
#if _RT_ROBOT4_SOUND 
        if(RTRobot4ctl!=NULL)
        {    
            MemFree(RTRobot4ctl);
            RTRobot4ctl=NULL;
        }
        
        if(RTRobot4buf!=NULL)
        {    
            MemFree(RTRobot4buf);
            RTRobot4buf=NULL;
        }
        
        if(RTRobot4FFTctl!=NULL)
        {    
            MemFree(RTRobot4FFTctl);
            RTRobot4FFTctl=NULL;
        }
        
#endif

#if _RT_CHORUS_SOUND
		if(ChorusBuf!=NULL)
		{
            MemFree(ChorusBuf);
            ChorusBuf=NULL;
		}
		if(RT_ChorusCtl!=NULL)
		{
            MemFree(RT_ChorusCtl);
            RT_ChorusCtl=NULL;
		}
#endif

#if _RT_REVERB_SOUND
        if(Reverb_x!=NULL)
        {
            MemFree(Reverb_x);
            Reverb_x = NULL;
            MemFree(Reverb_y);
            Reverb_y = NULL;
        }
#endif
#if _RT_ECHO_SOUND
        if(EchoBuf!=NULL)
        {
            MemFree(EchoBuf);
            EchoBuf=NULL;
        }
#endif 
#if _RT_GHOST_SOUND
		if(EnvelopTHTemp!=NULL)
		{
            MemFree(EnvelopTHTemp);
            EnvelopTHTemp=NULL;
			MemFree(Ghost_Reverb_x);
			Ghost_Reverb_x=NULL;
			MemFree(Ghost_Reverb_y);
			Ghost_Reverb_y=NULL;
		}
#endif
#if _RT_DARTH_SOUND
		if(RT_DarthBuf!=NULL)
		{
            MemFree(RT_DarthBuf);
            RT_DarthBuf=NULL;
			MemFree(RT_DarthCtl);
			RT_DarthCtl=NULL;
		}
#endif
		//---------------------
		// Howling_Suppress memory
#if _RT_HOWLING_SUPPRESS
		if(HRInCtrl!=NULL)
        {
            MemFree(HRInCtrl);
            HRInCtrl = NULL;
        }
#endif
        //--------------------
    }
}
#endif


//------------------------------------------------------------------//
// Dac ISR
// Parameter: 
//          size: fifo fill size 
// return value:
//          NONE
//------------------------------------------------------------------// 
void RT_DacIsr(U8 size)
{
    U16 temp;
    S16 *psrc16;//,gain;
    //U8 i,len,size2;
    //static U32 test_dac_val=0;
    
	if(AudioBufCtl.dac_state==STATUS_PLAYING){

        temp=DacBufCtl.size-DacBufCtl.offset;
        if(size>temp){
            size=temp;
        }   
        //dprintf("size,%d\r\n",size);
        //AudioBufCtl.fifo_count+=size;   
        
        psrc16=(S16 *)(DacBufCtl.buf+DacBufCtl.offset);
        
        CB_RT_WriteDac(size,(U16 *)psrc16);
        
        DacBufCtl.offset+=size;
        //AudioBufCtl.fifo_count2+=size;

        if(DacBufCtl.offset==DacBufCtl.size)
        {
            DacBufCtl.offset=0;    
        }       
    }else{
        //for(i=0;i<size;i++){
            //*pdest32=0;           
        //}   
        CB_RT_WriteDac(size,(U16 *)0);          
    }                 
}    
//------------------------------------------------------------------//
// Get current status
// Parameter: 
//          NONE
// return value:
//          status:STATUS_PLAYING,STATUS_PAUSE,STATUS_STOP
//------------------------------------------------------------------// 
U8 RT_GetStatus(void){
    return AudioBufCtl.adc_state;    
}

//------------------------------------------------------------------//
// Set audio volume
// Parameter: 
//          vol:0~15
// return value:
//          NONE
//------------------------------------------------------------------// 
void RT_SetVolume(U8 vol) {
    if(vol>CH_VOL_15){
        vol=CH_VOL_15;
    }
    ChVol=vol;
    AudioVol = ChVol <<11;
}

//------------------------------------------------------------------//
// Get audio volume
// Parameter: 
//          NONE
// return value:
//          0~15
//------------------------------------------------------------------// 
U8 RT_GetVolume(void) {
    return ChVol;
}
//------------------------------------------------------------------//
// Reset_RT_Func
// Parameter: 
//          cmd:	1:enable,0:disable	
//			idx_AL:	0:Pitch, 1:other, 2:Echo, 3:Ghost
// return value:
//          NONE
//------------------------------------------------------------------// 
U8 Reset_RT_Func(U8 cmd, U8 idx_AL) 
{
	U8 ret;
	RT_Stop();
	
	if(cmd==0)
		Flag_switch_AL=RT_AL_NO_SFX; 
	else {		
		Flag_switch_AL=idx_AL;
	}
	
	#if _RT_ROBOT1_SOUND
		memset((void *)&RobotCtl, 0, sizeof(RobotCtl));
	#endif
	#if _RT_ROBOT2_SOUND
		memset((void *)&Robot01Ctl, 0, sizeof(Robot01Ctl));
	#endif
	#if _RT_ROBOT3_SOUND
		memset((void *)&Robot02Ctl, 0, sizeof(Robot02Ctl));
	#endif
	#if _RT_ROBOT4_SOUND
		memset((void *)&Robot04Ctl, 0, sizeof(Robot04Ctl));
	#endif
	#if _RT_ECHO_SOUND
		memset((void *)&EchoCtrl, 0, sizeof(EchoCtrl));
		memset((void *)&echoCtrl_RT, 0, sizeof(echoCtrl_RT));
	#endif
	#if _RT_REVERB_SOUND
		ReverbCmd=0;
		ReverbEffect=0;
	#endif
	#if _RT_GHOST_SOUND
		memset((void *)&GhostCtl, 0, sizeof(GhostCtl));
	#endif
    #if _RT_DARTH_SOUND
        memset((void *)&DarthCtl, 0, sizeof(DarthCtl));    
    #endif
 	#if _RT_CHORUS_SOUND
		memset((void *)&ChorusCtl, 0, sizeof(ChorusCtl));
	#endif
	
	ret = RT_InternalPlay();
	//-----------------------------------------
	#if _RT_HOWLING_SUPPRESS
		RT_Howling_Suppress_Init();	// Howling Init
	#endif
	//-----------------------------------------
	return ret;	
}

//------------------------------------------------------------------//
// Set pitch ratio
// Parameter: 
//          index: -12~12
// return value:
//          NONE
//------------------------------------------------------------------// 
void RT_SetPitch(S8 index){
#if _RT_PITCH_CHANGE	
	Reset_RT_Func(ENABLE,RT_AL_PITCH);	//DISABLE: no SFX
	RT_PitchChange_Init(1,index);
#if _RT_HOWLING_SUPPRESS	
	RT_Howling_Suppress_Switch();
#endif	
#endif	
}
//------------------------------------------------------------------//
// Enable/Disable Robot Sound Effect
// Parameter: 
//          cmd: 1:enable,0:disable
//          index: carrier index
// return value:
//          NONE
//------------------------------------------------------------------// 
void RT_SetRobot1Sound(U8 cmd,U8 index){
#if _RT_ROBOT1_SOUND  

	Reset_RT_Func(cmd,RT_AL_OTHER);
	
    RobotCtl.enable=cmd;
    if(RobotCtl.enable){
        RobotCtl.change=1;
        RobotCtl.index=index;
    }
	else
	{
		RT_PitchChange_Init(0,0); // set to pitch factor0
	}
#if _RT_HOWLING_SUPPRESS	
	RT_Howling_Suppress_Switch();
#endif	   
#endif    
}
#if _RT_ROBOT2_SOUND
//------------------------------------------------------------------//
// Enable/Disable Robot Sound Effect
// Parameter:
//          cmd: 1:enable,0:disable
//          type: robot type 0~2
//          thres: robot threshold 0~7
// return value:
//          NONE
//------------------------------------------------------------------//
void RT_SetRobot2Sound(U8 cmd,U8 type, U8 thres){
	
	Reset_RT_Func(cmd,RT_AL_OTHER);
	
    Robot01Ctl.enable=cmd;
    if(Robot01Ctl.enable)
    {
        Robot01Ctl.type=type;
        Robot01Ctl.thres= Robot_Thres[thres];
        Robot01Ctl.detectthres = 100;
#if _USE_HEAP_RAM
        if(Robot01_buf==NULL)
        {
            Robot01_buf = (S16 *)MemAlloc(RT_ROBOT_BUF_SIZE<<1);
        }
#endif
        memset((void *)Robot01_buf, 0, RT_ROBOT_BUF_SIZE*2);
		ROBOT_EFFECT_01_Init(Robot01_buf,Robot01Ctl.type,Robot01Ctl.thres,Robot01Ctl.detectthres,_RT_SAMPLE_RATE,AudioBufCtl.out_size,40,1600);
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

	if(cmd==0)
		RT_PitchChange_Init(0,0);

#if _RT_HOWLING_SUPPRESS
	RT_Howling_Suppress_Switch();
#endif	
}
#endif

#if _RT_ROBOT3_SOUND
//------------------------------------------------------------------//
// Enable/Disable Robot Sound Effect
// Parameter:
//          cmd: 1:enable,0:disable
//          type: robot type 0~2
//          thres: robot threshold 0~7
// return value:
//          NONE
//------------------------------------------------------------------//
void RT_SetRobot3Sound(U8 cmd,U8 type, U8 thres){
    	
	Reset_RT_Func(cmd,RT_AL_OTHER);
	
	Robot02Ctl.enable=cmd;
    if(Robot02Ctl.enable)
    {
        Robot02Ctl.type=type;
        Robot02Ctl.thres= Robot02_Thres[thres];
#if _USE_HEAP_RAM
        if(Robot02_buf==NULL)
        {
            Robot02_buf = (S16 *)MemAlloc(RT_ROBOT02_BUF_SIZE<<1);
        }
#endif
        memset((void *)Robot02_buf, 0, RT_ROBOT02_BUF_SIZE*2);
		ROBOT_EFFECT_02_Init(Robot02_buf,Robot02Ctl.type,Robot02Ctl.thres,_RT_SAMPLE_RATE);
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

	if(cmd==0)
		RT_PitchChange_Init(0,0);

#if _RT_HOWLING_SUPPRESS
	RT_Howling_Suppress_Switch();
#endif	
}
#endif

#if _RT_ROBOT4_SOUND 
//------------------------------------------------------------------//
// Enable/Disable Robot4 Sound Effect
// Parameter: 
//          cmd: 1:enable,0:disable
// return value:
//          NONE
//------------------------------------------------------------------// 
void RT_SetRobot4Sound(U8 cmd){

  Reset_RT_Func(cmd,RT_AL_OTHER);
#if _RT_HOWLING_SUPPRESS	
	RT_Howling_Suppress_Switch();
#endif	
if(cmd == ENABLE)
    {	
		#if _USE_HEAP_RAM
		if(RTRobot4ctl==NULL)
        {
        	RTRobot4ctl = (ROBOT4Ctl*) MemAlloc(RT_ROBOT4_CTL_SIZE);
        }
		if(RTRobot4buf==NULL)
        {
        	RTRobot4buf = (ROBOT4Buf*) MemAlloc(RT_ROBOT4_BUF_SIZE);
        }
        if(RTRobot4FFTctl==NULL)
        {
        	RTRobot4FFTctl = (FFTctl*) MemAlloc(RT_ROBOT4_FFT_CTL_SIZE);
        }
        #endif   
		memset((void *)RTRobot4ctl, 0, RT_ROBOT4_CTL_SIZE);
		RTRobot4ctl->FFTSize 		= 	ROBOT4_FFT_SIZE;
		RTRobot4ctl->Samplerate 	= 	_RT_SAMPLE_RATE;
		RTRobot4ctl->Overlapratio 	= 	ROBOT4_OVERLAP_RATIO;
		RTRobot4ctl->InputSize 	= 	RT_ROBOT4_DATA_LENGTH;	
		memset((void *)RTRobot4buf, 0, RT_ROBOT4_BUF_SIZE);
		memset((void *)RTRobot4FFTctl, 0, RT_ROBOT4_FFT_CTL_SIZE);			
		
		ROBOT_EFFECT_04_Init(RTRobot4ctl, RTRobot4buf, RTRobot4FFTctl);
		
       	Robot04Ctl.enable = cmd;
    }
    else
    {
#if _USE_HEAP_RAM
		if(RTRobot4ctl==NULL)
        {
	 		MemFree(RTRobot4ctl);
	 		RTRobot4ctl=NULL;
 		}
 		
		if(RTRobot4buf==NULL)
        {
	 		MemFree(RTRobot4buf);
	 		RTRobot4buf=NULL;
 		}
 		
 		if(RTRobot4FFTctl==NULL)
        {
	 		MemFree(RTRobot4FFTctl);
	 		RTRobot4FFTctl=NULL;
 		}
#endif          
		Robot04Ctl.enable = cmd;
    }
}
#endif

#if _RT_REVERB_SOUND
//------------------------------------------------------------------//
// Enable/Disable Reverb Sound Effect
// Parameter:
//          cmd: 1:enable,0:disable
//          index: Effect index
// return value:
//          NONE
//------------------------------------------------------------------//
void RT_SetReverbSound(U8 cmd,U8 index){
    
	Reset_RT_Func(cmd,RT_AL_OTHER);
#if _RT_HOWLING_SUPPRESS	
	RT_Howling_Suppress_Switch();
#endif		
	ReverbCmd = cmd;
    if(cmd)
    {
#if _USE_HEAP_RAM
        if(Reverb_x==NULL)
        {
            Reverb_x = (S16 *)MemAlloc(RT_REVERB_BUF_SIZE<<1);
            Reverb_y = (S16 *)MemAlloc(RT_REVERB_BUF_SIZE<<1);
        }
#endif
		memset((void *)Reverb_x, 0, RT_REVERB_BUF_SIZE<<1);
		memset((void *)Reverb_y, 0, RT_REVERB_BUF_SIZE<<1);
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
	
	if(cmd==0)
		RT_PitchChange_Init(0,0);
}
#endif

#if _RT_ECHO_SOUND	
//------------------------------------------------------------------//
// Enable/Disable Echo Sound Effect
// Parameter:
//          cmd: 1:enable,0:disable
// return value:
//          NONE
//------------------------------------------------------------------//															
void RT_SetEchoSound(U8 cmd)
{
	Reset_RT_Func(cmd,RT_AL_ECHO);
#if _RT_HOWLING_SUPPRESS
	RT_Howling_Suppress_Switch();
#endif		
	EchoCtrl.Cmd= cmd;
	echoCtrl_RT.iSampleRate = _RT_SAMPLE_RATE;
	
#if (_RT_ECHO_EFFECT!=DELAY_USER_DEFINE)	//Default
	echoCtrl_RT.iEchoTotalDelayTime = _RT_ECHO_EFFECT;
	echoCtrl_RT.Cmd= 0;		//cmd 0 is default
#if _RT_ECHO_EFFECT == 1200
	echoCtrl_RT.iEchoRepeatTimes = 6;
#elif _RT_ECHO_EFFECT == 800
	echoCtrl_RT.iEchoRepeatTimes = 4;
#elif _RT_ECHO_EFFECT == 320
	echoCtrl_RT.iEchoRepeatTimes = 2;
#endif
	
	echoCtrl_RT.bitmode = 4;		//Basic
	if (_RT_ECHO_QUALITY) {
		echoCtrl_RT.bitmode = 5;	//advance
	}
#else	
	//error message 
#if _RT_ECHO_UD_REPEAT_TIME != DISABLE 
#if (_RT_ECHO_UD_REPEAT_TIME > 6) || (_RT_ECHO_UD_REPEAT_TIME < 1)
	#error "Please check the _RT_ECHO_UD_REPEAT_TIME(1~6)."
#endif
#endif
				
#if RT_ECHO_UD_TOTAL_DELAY_TIME != DISABLE 
#if ((RT_ECHO_UD_TOTAL_DELAY_TIME > 1200) || (RT_ECHO_UD_TOTAL_DELAY_TIME < 50))
	#error "Please check the RT_ECHO_UD_TOTAL_DELAY_TIME(50ms~1200ms)."
#endif
#endif
	
	echoCtrl_RT.Cmd= 1;		//cmd 1 is user define
	if ((_RT_ECHO_UD_REPEAT_TIME <= 6) && (_RT_ECHO_UD_REPEAT_TIME >= 1)) {
		echoCtrl_RT.iEchoRepeatTimes = _RT_ECHO_UD_REPEAT_TIME;
	}
	echoCtrl_RT.iEchoTotalDelayTime = RT_ECHO_UD_TOTAL_DELAY_TIME;
	echoCtrl_RT.bitmode = 4;		//Basic
	if (_RT_ECHO_QUALITY) {
		echoCtrl_RT.bitmode = 5;	//advance
	}
#endif

	if(cmd)
	{
		#if _USE_HEAP_RAM
		if(EchoBuf==NULL)
		{
			EchoBuf  = (S16 *)MemAlloc(RT_ECHO_BUF_SIZE*2);								
		}																				
		#endif
		memset((void *)EchoBuf, 0, RT_ECHO_BUF_SIZE*2);
		EchoSound_init((void *)EchoBuf, echoCtrl_RT.iEchoTotalDelayTime, RT_ECHO_BUF_SIZE, &echoCtrl_RT);
	}
	#if _USE_HEAP_RAM
	else
	{
		if(EchoBuf!=NULL)
		{
			MemFree(EchoBuf);
			EchoBuf = NULL;
		}
	}									
	#endif
	if(cmd==0)
		RT_PitchChange_Init(0,0);
}												
#endif

#if _RT_GHOST_SOUND	
//------------------------------------------------------------------//
// Set Ghost Effect
// Parameter: 
//			Cmd: 1:enable, 0:disable
//          PitchIndex: -12 ~ +12
//			EnvelopeType : 0, 1, 2
// return value:
//          NONE
//------------------------------------------------------------------// 
void RT_SetGhostSound(U8 Cmd, S8 PitchIndex, U8 EnvelopeType){
	
	Reset_RT_Func(Cmd,RT_AL_GHOST);
#if _RT_HOWLING_SUPPRESS
	RT_Howling_Suppress_Switch();
#endif		
	GhostCtl.Cmd = Cmd;
	GhostCtl.Effect = PitchIndex;
	GhostCtl.EnvelopeType = EnvelopeType;
	if(Cmd)
	{
		#if _USE_HEAP_RAM
		if(EnvelopTHTemp==NULL)
		{
			EnvelopTHTemp = (S32 *)MemAlloc(RT_GHOST_ENVELOPETH_SIZE<<2);		
			Ghost_Reverb_x = (S16 *)MemAlloc(RT_GHOST_REVERB_BUF_SIZE<<1);	
			Ghost_Reverb_y = (S16 *)MemAlloc(RT_GHOST_REVERB_BUF_SIZE<<1);	
		}
		#endif
		memset((void *)PitchBuf, 0, RT_PITCH_BUF_SIZE<<1);  
		memset((void *)EnvelopTHTemp, 0, RT_GHOST_ENVELOPETH_SIZE<<2);
		memset((void *)Ghost_Reverb_x, 0, RT_GHOST_REVERB_BUF_SIZE<<1);
		memset((void *)Ghost_Reverb_y, 0, RT_GHOST_REVERB_BUF_SIZE<<1);
		
		RT_GhostSound_init(PitchBuf, RT_PITCH_BUF_SIZE, Ghost_Reverb_x, Ghost_Reverb_y, EnvelopTHTemp, GhostCtl.Effect, GhostCtl.EnvelopeType);
	}
	#if _USE_HEAP_RAM
	else
	{
		if(EnvelopTHTemp!=NULL)
		{
            MemFree(EnvelopTHTemp);
            EnvelopTHTemp=NULL;
			MemFree(Ghost_Reverb_x);
			Ghost_Reverb_x=NULL;
			MemFree(Ghost_Reverb_y);
			Ghost_Reverb_y=NULL;
		}
	}									
	#endif
	if(Cmd==0)
		RT_PitchChange_Init(0,0);
}
#endif

#if _RT_DARTH_SOUND
//------------------------------------------------------------------//
// Set Darth Vader Effect
// Parameter: 
//			Cmd: 1:enable, 0:disable
//          PitchIndex: -12 ~ +12
//			CarrierType: 0, 1, 2, 3
// return value:
//          NONE
//------------------------------------------------------------------// 
void RT_SetDarthSound(U8 Cmd, S8 PitchIndex, U8 CarrierType){

    Reset_RT_Func(Cmd,RT_AL_DARTH);
#if _RT_HOWLING_SUPPRESS	
	RT_Howling_Suppress_Switch();
#endif	    
    DarthCtl.Cmd = Cmd;                
	DarthCtl.Effect = CarrierType;
	DarthCtl.Pitchfactor = PitchIndex;
    if(Cmd)
    {
#if _USE_HEAP_RAM  
        if(RT_DarthBuf==NULL)
        {
            RT_DarthBuf = (RT_DarthVaderBuf*)MemAlloc(sizeof(RT_DarthVaderBuf));	
        }
        if(RT_DarthCtl==NULL)
        {
            RT_DarthCtl = (RT_DarthVaderCtl*)MemAlloc(sizeof(RT_DarthVaderCtl));	
        }  
#endif  
        memset((void *)RT_DarthBuf, 0, sizeof(RT_DarthVaderBuf));
        memset((void *)RT_DarthCtl, 0, sizeof(RT_DarthVaderCtl));

        RT_DarthBuf->StepSizeTb = StepSizeTb;
        RT_DarthBuf->Cycle_number = 50;        
        RT_DarthBuf->Step_cycle_number = sizeof(StepSizeTb);    
    
        RT_DarthCtl->FrameSize = RT_AUDIO_OUT_SIZE_01;
        RT_DarthCtl->PitchFactor = DarthCtl.Pitchfactor;
        if(DarthCtl.Effect>=0 && DarthCtl.Effect<=2){
            RT_DarthCtl->DelayA = 72;
            RT_DarthCtl->DelayB = 50;
            RT_DarthCtl->FPShiftLevel = 7;
            RT_DarthCtl->FbDelayALevel = 0.44;
            RT_DarthCtl->FbDelayBLevel = 0.44;
            RT_DarthCtl->InSumLevel = 0.625;
            RT_DarthVader_Init(0, RT_DarthCtl, RT_DarthBuf, DarthCtl.Effect); 
        }
        else if(DarthCtl.Effect>2){
            RT_DarthCtl->DelayA = 0;
            RT_DarthCtl->DelayB = 0;
            RT_DarthCtl->FPShiftLevel = 7;
            RT_DarthCtl->FbDelayALevel = 0;
            RT_DarthCtl->FbDelayBLevel = 0;
            RT_DarthCtl->InSumLevel = 1;
            RT_DarthVader_Init(0, RT_DarthCtl, RT_DarthBuf, 2); 
        }
    }
#if _USE_HEAP_RAM    
    else
    {
		if(RT_DarthBuf!=NULL)
		{
            MemFree(RT_DarthBuf);
            RT_DarthBuf=NULL;
			MemFree(RT_DarthCtl);
			RT_DarthCtl=NULL;
		}
    }
#endif
    if(Cmd==0)
		RT_PitchChange_Init(0,0);   
}
#endif

#if _RT_CHORUS_SOUND	
//------------------------------------------------------------------//
// Set Chorus Effect
// Parameter: 
//			Cmd: 1:enable, 0:disable
//          PitchIndex: -12 ~ +12 , for adjust pitch channel-1 and pitch channel-2 pitch factor together
//			vol_1:		0~100, for adjust pitch channel-1 volume,	 	
//          vol_2: 		0~100, for adjust voice channel volume,	
//			vol_3:      0~100, for adjust pitch channel-2 volume,	 
//			gain:		0~10, for adjust voice total gain. Real gain value is 0=1.0, 1=1.1, 2=1.2......10=2.0
// return value:
//          NONE
//------------------------------------------------------------------// 
void RT_SetChorusSound(U8 Cmd, S8 PitchIndex,U8 vol_1,U8 vol_2,U8 vol_3, U8 gain){
	
	Reset_RT_Func(Cmd,RT_AL_CHORUS);
#if _RT_HOWLING_SUPPRESS	
	RT_Howling_Suppress_Switch();
#endif		
	ChorusCtl.Cmd = Cmd;
	
	if(Cmd)
	{
		#if _USE_HEAP_RAM
		if(ChorusBuf==NULL)
		{
			ChorusBuf= (S16 *)MemAlloc(RT_CHORUS_BUF_SIZE<<1);	
		}
		if(RT_ChorusCtl==NULL)
		{
			RT_ChorusCtl= (RTChorusCtl *)MemAlloc(RT_CHORUS_CTL_SIZE);	
		}
		#endif
		memset((void *)ChorusBuf, 0, RT_CHORUS_BUF_SIZE<<1);
		memset((void *)RT_ChorusCtl, 0, RT_CHORUS_CTL_SIZE);
		
		RT_ChorusCtl->sample_rate = _RT_SAMPLE_RATE;	
		RT_ChorusCtl->pitch_factor = PitchIndex;
	
		RT_ChorusCtl->Cmd = Cmd;
		RT_ChorusCtl->delay_time_1 = _RT_CHORUS_SOUND_DELAY1;
		RT_ChorusCtl->delay_time_2 = _RT_CHORUS_SOUND_DELAY2;
		
		RT_ChorusCtl->volume_voice_1 = vol_1;
		RT_ChorusCtl->volume_voice_2 = vol_2;
		RT_ChorusCtl->volume_voice_3 = vol_3;
		RT_Chorus_Init((S8)1, (S16 *)ChorusBuf, RT_ChorusCtl);	
		RT_SetChorusVolume(vol_1,vol_2,vol_3,gain);
	}
	#if _USE_HEAP_RAM
	else
	{
		if(ChorusBuf!=NULL)
		{
            MemFree(ChorusBuf);
            ChorusBuf=NULL;
		}
		if(RT_ChorusCtl!=NULL)
		{
            MemFree(RT_ChorusCtl);
            RT_ChorusCtl=NULL;
		}
	}									
	#endif
	
	if(Cmd==0)
		RT_PitchChange_Init(0,0);
}

//------------------------------------------------------------------//
// Set Chorus volume
// Parameter: 
//			vol_1:		0~100, for adjust pitch channel-1 volume,	 	
//          vol_2: 		0~100, for adjust voice channel volume,	
//			vol_3:      0~100, for adjust pitch channel-2 volume,	 
//			gain:		0~10,  for adjust voice total gain. Real gain value is 0=1.0, 1=1.1, 2=1.2......10=2.0
// return value:
//          NONE
//------------------------------------------------------------------// 
void RT_SetChorusVolume(U8 vol_1,U8 vol_2,U8 vol_3, U8 gain)
{
	if(vol_1>100)
		vol_1=100;
	if(vol_2>100)
		vol_2=100;
	if(vol_3>100)
		vol_3=100;
	if(gain >10)
		gain=10;

	RT_ChorusVolume_Reset(vol_1, vol_2, vol_3, ChorusVolGain[gain], (S16*)ChorusBuf);
}

#endif
//------------------------------------------------------------------//
// Codec init
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void RT_Init(void){

    ChVol=CH_VOL_15;    
    AudioVol = ChVol <<11;
	
	Flag_switch_AL=RT_AL_NO_SFX;
	RT_PitchChange_Init(1,0);	//rst, factor0   
}
//------------------------------------------------------------------//
// Stop record voice
// Parameter:
//			NONE
// return value:
//			NONE
//------------------------------------------------------------------//
void RT_Stop(void)
{	
	AudioBufCtl.adc_state = STATUS_STOP;
    AudioBufCtl.dac_state = STATUS_STOP;
    RT_PlayFlag = 0;
    CB_RT_AdcCmd(DISABLE);
#if _USE_HEAP_RAM
    RT_FreeMemory(MEMORY_ALLOCATE_PLAY);
#endif
	RT_PitchChange_Init(1,0);	//rst, factor0
	GIE_DISABLE();
	DacAssigner_unRegist(kRT);
	GIE_ENABLE();
	//dprintf("Free codec %d hw_ch\r\n",kRT);
}
//------------------------------------------------------------------//
// Start record voice
// Parameter: 
//			NONE
// return value:
//          0:ok
//          others:fail
//------------------------------------------------------------------// 
#if _RT_SAMPLE_RATE==SAMPLE_RATE_8000 || _RT_SAMPLE_RATE==SAMPLE_RATE_12000 || _RT_SAMPLE_RATE==SAMPLE_RATE_16000
static U8 RT_InternalPlay(void)
{
    //U16 pitch_out_sr;
	RT_Stop();
	GIE_DISABLE();
	S8 hw_ch = DacAssigner_Regist(kRT,RT_EXCLUSIVE);
	GIE_ENABLE();
	if(hw_ch<0){
		//dprintf("No play Invalid sr\r\n");
		return E_AUDIO_INVALID_SAMPLERATE;
	}
	//dprintf("codec %d reg hw_ch %d\r\n",kRT,hw_ch);
	
#if _USE_HEAP_RAM
	AUDIO_ServiceLoop();
	//RT_FreeMemory(MEMORY_ALLOCATE_PLAY);
    RT_AllocateMemory(MEMORY_ALLOCATE_PLAY);
    
#if _RT_PITCH_CHANGE || _RT_GHOST_SOUND || _RT_DARTH_SOUND || _RT_CHORUS_SOUND
    if ((Flag_switch_AL==RT_AL_PITCH) || (Flag_switch_AL==RT_AL_GHOST)
     || (Flag_switch_AL==RT_AL_DARTH) || (Flag_switch_AL==RT_AL_CHORUS))
	{
		PitchBuf = (S16 *) MemAlloc(RT_PITCH_BUF_SIZE<<1);
		memset((void *)PitchBuf, 0, RT_PITCH_BUF_SIZE<<1);    
	}
#endif	
#if _RT_ROBOT2_SOUND
    if(Robot01Ctl.enable && Robot01_buf==NULL)
    {
        Robot01_buf = (S16 *) MemAlloc(RT_ROBOT_BUF_SIZE<<1);
        memset((void *)Robot01_buf, 0, RT_ROBOT_BUF_SIZE*2);        
		ROBOT_EFFECT_01_Init(Robot01_buf,Robot01Ctl.type,Robot01Ctl.thres,Robot01Ctl.detectthres,_RT_SAMPLE_RATE,AudioBufCtl.out_size,40,1600);		
    }
#endif
#if _RT_ROBOT3_SOUND
    if(Robot02Ctl.enable && Robot02_buf==NULL)
    {
        Robot02_buf = (S16 *) MemAlloc(RT_ROBOT02_BUF_SIZE<<1);
        memset((void *)Robot02_buf, 0, RT_ROBOT02_BUF_SIZE*2);        
		ROBOT_EFFECT_02_Init(Robot02_buf,Robot02Ctl.type,Robot02Ctl.thres,_RT_SAMPLE_RATE);
    }
#endif

#if _RT_ROBOT4_SOUND
    if(Robot04Ctl.enable){
	
		#if _USE_HEAP_RAM	
		if(RTRobot4ctl==NULL)
        {
        	RTRobot4ctl = (ROBOT4Ctl*) MemAlloc(RT_ROBOT4_CTL_SIZE);
        }
		if(RTRobot4buf==NULL)
        {
        	RTRobot4buf = (ROBOT4Buf*) MemAlloc(RT_ROBOT4_BUF_SIZE);
        }
        if(RTRobot4FFTctl==NULL)
        {
        	RTRobot4FFTctl = (FFTctl*)MemAlloc(RT_ROBOT4_FFT_CTL_SIZE);
        }
        #endif    
		memset((void *)RTRobot4ctl, 0, RT_ROBOT4_CTL_SIZE);		
		RTRobot4ctl->FFTSize 		= 	ROBOT4_FFT_SIZE;
		RTRobot4ctl->Samplerate 	= 	_RT_SAMPLE_RATE;  
		RTRobot4ctl->Overlapratio 	= 	ROBOT4_OVERLAP_RATIO;
		RTRobot4ctl->InputSize 	= 	RT_ROBOT4_DATA_LENGTH;	
		memset((void *)RTRobot4buf, 0, RT_ROBOT4_BUF_SIZE);			
		memset((void *)RTRobot4FFTctl, 0, RT_ROBOT4_FFT_CTL_SIZE);				
		ROBOT_EFFECT_04_Init(RTRobot4ctl, RTRobot4buf, RTRobot4FFTctl);
		
	}
    
#endif

#if _RT_REVERB_SOUND
    if(ReverbCmd && Reverb_x==NULL)
    {
        Reverb_x = (S16 *) MemAlloc(RT_REVERB_BUF_SIZE<<1);
        Reverb_y = (S16 *) MemAlloc(RT_REVERB_BUF_SIZE<<1);
		memset((void *)Reverb_x, 0, RT_REVERB_BUF_SIZE<<1);
		memset((void *)Reverb_y, 0, RT_REVERB_BUF_SIZE<<1);
        Reverb_Effect_Init(Reverb_x,Reverb_y,ReverbEffect);
    }
#endif
#if _RT_ECHO_SOUND
	if(EchoCtrl.Cmd)
	{
		if(EchoBuf==NULL)
		{
			EchoBuf  = (S16 *)MemAlloc(RT_ECHO_BUF_SIZE*2);								
		}
		memset((void *)EchoBuf, 0, RT_ECHO_BUF_SIZE*2);
		EchoSound_init((void *)EchoBuf, echoCtrl_RT.iEchoTotalDelayTime, RT_ECHO_BUF_SIZE, &echoCtrl_RT);
	}
#endif
#if _RT_GHOST_SOUND
	if(GhostCtl.Cmd)
	{	
		if(EnvelopTHTemp==NULL)
		{
			EnvelopTHTemp = (S32 *)MemAlloc(RT_GHOST_ENVELOPETH_SIZE<<2);		
			Ghost_Reverb_x = (S16 *)MemAlloc(RT_GHOST_REVERB_BUF_SIZE<<1);	
			Ghost_Reverb_y = (S16 *)MemAlloc(RT_GHOST_REVERB_BUF_SIZE<<1);	
		}
		memset((void *)EnvelopTHTemp, 0, RT_GHOST_ENVELOPETH_SIZE<<2);
		memset((void *)Ghost_Reverb_x, 0, RT_GHOST_REVERB_BUF_SIZE<<1);
		memset((void *)Ghost_Reverb_y, 0, RT_GHOST_REVERB_BUF_SIZE<<1);
		
		RT_GhostSound_init(PitchBuf, RT_PITCH_BUF_SIZE, Ghost_Reverb_x, Ghost_Reverb_y, EnvelopTHTemp, GhostCtl.Effect, GhostCtl.EnvelopeType);
	}
#endif
#if _RT_DARTH_SOUND
    if(DarthCtl.Cmd)
    {
        if(RT_DarthBuf==NULL)
		{   
			RT_DarthBuf = (RT_DarthVaderBuf*)MemAlloc(sizeof(RT_DarthVaderBuf));	
		}
		if(RT_DarthCtl==NULL)
		{	
			RT_DarthCtl = (RT_DarthVaderCtl*)MemAlloc(sizeof(RT_DarthVaderCtl));	
		}
        memset((void *)RT_DarthBuf, 0, sizeof(RT_DarthVaderBuf));
        memset((void *)RT_DarthCtl, 0, sizeof(RT_DarthVaderCtl));

        RT_DarthBuf->StepSizeTb = StepSizeTb;
        RT_DarthBuf->Cycle_number = 50;        
        RT_DarthBuf->Step_cycle_number = sizeof(StepSizeTb);    
      
        RT_DarthCtl->FrameSize = RT_AUDIO_OUT_SIZE_01;
        RT_DarthCtl->PitchFactor = DarthCtl.Pitchfactor;
        if(DarthCtl.Effect>=0 && DarthCtl.Effect<=2){
            RT_DarthCtl->DelayA = 72;
            RT_DarthCtl->DelayB = 50;
            RT_DarthCtl->FPShiftLevel = 7;
            RT_DarthCtl->FbDelayALevel = 0.44;
            RT_DarthCtl->FbDelayBLevel = 0.44;
            RT_DarthCtl->InSumLevel = 0.625;
            RT_DarthVader_Init(0, RT_DarthCtl, RT_DarthBuf, DarthCtl.Effect); 
        }
        else if(DarthCtl.Effect>2){
            RT_DarthCtl->DelayA = 0;
            RT_DarthCtl->DelayB = 0;
            RT_DarthCtl->FPShiftLevel = 7;
            RT_DarthCtl->FbDelayALevel = 0;
            RT_DarthCtl->FbDelayBLevel = 0;
            RT_DarthCtl->InSumLevel = 1;
            RT_DarthVader_Init(0, RT_DarthCtl, RT_DarthBuf, 2); 
        } 
    }
#endif

#if _RT_CHORUS_SOUND
	if(ChorusCtl.Cmd)
	{
		#if _USE_HEAP_RAM
		if(ChorusBuf==NULL)
		{
			ChorusBuf= (S16 *)MemAlloc(RT_CHORUS_BUF_SIZE<<1);	
		}
		if(RT_ChorusCtl==NULL)
		{
			RT_ChorusCtl= (RTChorusCtl *)MemAlloc(RT_CHORUS_CTL_SIZE);	
		}
		#endif
		memset((void *)ChorusBuf, 0, RT_CHORUS_BUF_SIZE<<1);
		memset((void *)RT_ChorusCtl, 0, RT_CHORUS_CTL_SIZE);
		
		RT_ChorusCtl->sample_rate = _RT_SAMPLE_RATE;	
		RT_ChorusCtl->pitch_factor = 0;
		RT_ChorusCtl->delay_time_1 = _RT_CHORUS_SOUND_DELAY1;
		RT_ChorusCtl->delay_time_2 = _RT_CHORUS_SOUND_DELAY2;
		RT_ChorusCtl->volume_voice_2 = 33;
		RT_ChorusCtl->volume_voice_1 = 33;
		RT_ChorusCtl->volume_voice_3 = 33;
		RT_Chorus_Init((S8)0, (S16 *)ChorusBuf, RT_ChorusCtl);	
	}
#endif

#endif    
    memset((void *)&AudioBufCtl, 0, sizeof(AudioBufCtl));
 
    if(Flag_switch_AL==RT_AL_NO_SFX) // NO_SFX is the same as ECHO in/out buffer
    {
		AudioBufCtl.out_size= RT_AUDIO_OUT_SIZE_03;
		AudioBufCtl.in_size=(AudioBufCtl.out_size<<1);
		AudioBufCtl.out_count=RT_AUDIO_BUF_SIZE_03/AudioBufCtl.out_size;		
    }
#if _RT_PITCH_CHANGE || _RT_GHOST_SOUND || _RT_DARTH_SOUND || _RT_CHORUS_SOUND
    else if ((Flag_switch_AL==RT_AL_PITCH) || (Flag_switch_AL==RT_AL_GHOST)
          || (Flag_switch_AL==RT_AL_DARTH) || (Flag_switch_AL==RT_AL_CHORUS))
	{		
		AudioBufCtl.out_size= RT_AUDIO_OUT_SIZE_01;
		AudioBufCtl.in_size=(AudioBufCtl.out_size<<1);
		AudioBufCtl.out_count=RT_AUDIO_BUF_SIZE_01/AudioBufCtl.out_size;
		//dprintf("out %d in %d\r\n",AudioBufCtl.out_size,AudioBufCtl.in_size);
	}
#endif 
#if _RT_REVERB_SOUND || _RT_ROBOT1_SOUND || _RT_ROBOT2_SOUND || _RT_ROBOT3_SOUND|| _RT_ROBOT4_SOUND	 
	else if (Flag_switch_AL==RT_AL_OTHER)
	{		
		AudioBufCtl.out_size= RT_AUDIO_OUT_SIZE_02;
		AudioBufCtl.in_size=(AudioBufCtl.out_size<<1);
		AudioBufCtl.out_count=RT_AUDIO_BUF_SIZE_02/AudioBufCtl.out_size;
	}
#endif	
#if _RT_ECHO_SOUND	
	else if (Flag_switch_AL==RT_AL_ECHO)
	{
		AudioBufCtl.out_size= RT_AUDIO_OUT_SIZE_03;
		AudioBufCtl.in_size=(AudioBufCtl.out_size<<1);
		AudioBufCtl.out_count=RT_AUDIO_BUF_SIZE_03/AudioBufCtl.out_size;		
	}
#endif
//#else
	// unused, default path is pitch effect
    //AudioBufCtl.sample_rate=sample_rate;
    //AudioBufCtl.out_size=RT_AUDIO_OUT_SIZE;
    //AudioBufCtl.in_size=(RT_AUDIO_OUT_SIZE<<1);
//#endif    

    //dprintf("AudioBufCtl out %d in %d\r\n",AudioBufCtl.out_size,AudioBufCtl.in_size);

#if NOISE_GATE_ENABLE
    if(_RT_SAMPLE_RATE<16000){
        NGInit(NG_RELEASE_SAMPLE_8K,NG_ATTACK_SAMPLE_8K,NG_ON_OFF_SAMPLE_8K);
    }else{
        NGInit(NG_RELEASE_SAMPLE_16K,NG_ATTACK_SAMPLE_16K,NG_ON_OFF_SAMPLE_16K);
    }    

	S8 InitState = 0;	
	memset(&NoiseGatebuf_v2, 0, sizeof(NoiseGateBuf_v2));
		
	if (_RT_SAMPLE_RATE == 8000) {
		if (AudioBufCtl.out_size == 64) {
				NoiseGateArray_2[0] = 14;
				NoiseGateArray_2[1] = 10;
		}
		else if (AudioBufCtl.out_size == 80) {
				NoiseGateArray_2[1] = 8;
		}
		else if (AudioBufCtl.out_size == 320) {
			NoiseGateArray_2[1] = 3;
		}
	}
	else if (_RT_SAMPLE_RATE == 12000) {
		if (AudioBufCtl.out_size == 64) {
			NoiseGateArray_2[1] = 14;
		}
		else if (AudioBufCtl.out_size == 80) {
			NoiseGateArray_2[0] = 14;
			NoiseGateArray_2[1] = 10;
		}
		else if (AudioBufCtl.out_size == 320) {
			NoiseGateArray_2[1] = 3;
		}
	}
	else if (_RT_SAMPLE_RATE == 16000) {
		NoiseGateArray_2[0] = 12;
			
		if (AudioBufCtl.out_size == 64) {
			NoiseGateArray_2[1] = 18;
		}
		else if (AudioBufCtl.out_size == 80) {
			NoiseGateArray_2[1] = 12;
		}
		else if (AudioBufCtl.out_size == 320) {
			NoiseGateArray_2[1] = 4;
		}
	}
		
	//*InitState = EchoCancelInit_1(AudioBufCtl.out_size, EchoCancelArray_1, EchoCancelArray_2, &Echocancelbuf);
	InitState = NGInit_v2(AudioBufCtl.out_size, NoiseGateArray_1, NoiseGateArray_2, &NoiseGatebuf_v2);
	if(InitState < 0){
		//dprintf("ECHOCANCELTEST Init error\r\n");
	}
	NoiseGatebuf_v2.VAD_MUTE_LV_1 = 2;
	NoiseGatebuf_v2.VAD_MUTE_LV_2 = 4;
	NoiseGatebuf_v2.VAD_MUTE_LV_3 = 10; 
#endif

#if ECHO_CANCELL
    EchoCancelInit();
#endif
//    StorageIndex=index;

#if DC_REMOVE
	memset((void *)RunFilterBuf, 0, 8<<2);
#endif

#if AGC_ENALBE    
    AgcCnt=0;
    AgcInit(ALPHA,BETA,TARGET_RMS);   
#endif

#if _RT_DIGITAL_GAIN_CTRL
    memset((void *)RTDigiAmpCTLBuf,0,34*2);
    memset((void *)RTDigiAmpBuf,0,36*2);
    RTDigiAmpInit(RTDigiAmpArray, RTDigiAmpSet, RTDigiAmpBuf, RTDigiAmpCTLBuf);
    RTVadInit(RTVadArray,(RTDigiAmpCTLBuf+22));
#endif
     
    BufCtl.offset=0;   
    BufCtl.size=AudioBufCtl.out_size*AudioBufCtl.out_count;
    BufCtl.buf=AudioBuf;
    //dprintf("BufCtl size %d\r\n",BufCtl.size);

    RT_PlayFlag = 1;
    CB_RT_InitAdc(_RT_SAMPLE_RATE);  
	
    //RecordSize=size;
    
//----------------Play ---------------------------------
	CB_RT_InitDac(pHwCtl[hw_ch].p_dac,_RT_SAMPLE_RATE);
       
    //dprintf("tbl index %d samplerate %d %d\r\n",SpeedCtl.pitch_ratio_index,AudioBufCtl.sample_rate, pitch_out_sr*AudioBufCtl.sample_rate/16000);
    if ((Flag_switch_AL==RT_AL_NO_SFX)
 #if _RT_ECHO_SOUND
 		|| (Flag_switch_AL==RT_AL_ECHO)
 #endif
 	)
    {
		memset((void *)AudioBuf, 0, RT_AUDIO_BUF_SIZE_03*2);
		memset((void *)DacBuf, 0, DAC_BUF_SIZE_03*2);
    }
#if _RT_PITCH_CHANGE || _RT_GHOST_SOUND || _RT_DARTH_SOUND || _RT_CHORUS_SOUND
    else if ((Flag_switch_AL==RT_AL_PITCH) || (Flag_switch_AL==RT_AL_GHOST)
          || (Flag_switch_AL==RT_AL_DARTH) || (Flag_switch_AL==RT_AL_CHORUS))
	{
	    memset((void *)AudioBuf, 0, RT_AUDIO_BUF_SIZE_01*2);
	    memset((void *)DacBuf, 0, DAC_BUF_SIZE_01*2);
	    memset((void *)PitchBuf, 0, RT_PITCH_BUF_SIZE<<1);
	}
#endif	
#if _RT_REVERB_SOUND || _RT_ROBOT1_SOUND || _RT_ROBOT2_SOUND || _RT_ROBOT3_SOUND || _RT_ROBOT4_SOUND	
	else if	(Flag_switch_AL==RT_AL_OTHER)
	{
		memset((void *)AudioBuf, 0, RT_AUDIO_BUF_SIZE_02*2);
		memset((void *)DacBuf, 0, DAC_BUF_SIZE_02*2);
		}
#endif

    DacBufCtl.offset = 0;
    
    if ((Flag_switch_AL==RT_AL_NO_SFX)
 #if _RT_ECHO_SOUND
 		|| (Flag_switch_AL==RT_AL_ECHO)
 #endif
 	)
 	{
		DacBufCtl.size = DAC_BUF_SIZE_03;	// no SFX || Echo
	}	
#if _RT_PITCH_CHANGE || _RT_GHOST_SOUND || _RT_DARTH_SOUND || _RT_CHORUS_SOUND
    else if ((Flag_switch_AL==RT_AL_PITCH) || (Flag_switch_AL==RT_AL_GHOST)
          || (Flag_switch_AL==RT_AL_DARTH) || (Flag_switch_AL==RT_AL_CHORUS))
    {      
		DacBufCtl.size = DAC_BUF_SIZE_01;	// Pitch || Ghost || Darth || Chorus
	}
#endif	
#if _RT_REVERB_SOUND || _RT_ROBOT1_SOUND || _RT_ROBOT2_SOUND || _RT_ROBOT3_SOUND|| _RT_ROBOT4_SOUND	
	else if	(Flag_switch_AL==RT_AL_OTHER)
	{
		DacBufCtl.size = DAC_BUF_SIZE_02;	// Reverb || Robot 1~4
	}
#endif
	
    DacBufCtl.buf = DacBuf;
    DacBufWrOffset = 0;
    DacBufWrCnt = 0;
    
    //dprintf("DacBufCtl.size %d\r\n",DacBufCtl.size);

    //DecodeFrame();
    
    //OpMode=0;
    AudioBufCtl.adc_state =  STATUS_PLAYING;
    AudioBufCtl.dac_state =  STATUS_STOP;      
    
    CB_RT_AdcCmd(ENABLE);		
    CB_RT_PlayStart();
	CB_RT_DacCmd(pHwCtl[hw_ch].p_dac,ENABLE);

    return 0;
}
//------------------------------------------------------------------// 
U8 RT_Play(void)
{
    U8 ret;
    
	ret = Reset_RT_Func(ENABLE,RT_AL_NO_SFX);
#if _RT_HOWLING_SUPPRESS	
	RT_Howling_Suppress_Switch();
#endif	
    return ret;
}
#endif
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
void RT_AdcIsr(U8 size, U32 *pdest32){
    S16 *pdst16;
    U8 i;
    U16 temp;
    
    if(!DacMixerRamp_IsBias()) return;

	if(AudioBufCtl.adc_state==STATUS_PLAYING){

      	
        temp=BufCtl.size-BufCtl.offset;
        if(size>temp){
            size=temp;
        }   

#if AGC_ENALBE        
        temp=AGC_FRAME_SIZE-AgcCnt;
        if(size>temp){
            size=temp;
        }   
#endif        
        //dprintf("size,%d\r\n",size);
        AudioBufCtl.fifo_count+=size;   

        

        pdst16=(S16 *)(BufCtl.buf+BufCtl.offset);
        
        for(i=0;i<size;i++){
            temp=((*pdest32++)-32768);   
            *pdst16++=temp;            
#if AGC_ENALBE            
            AgcBuf[AgcCnt+i]=temp;
#endif            
//            temp=(*pdest32-32768);     
//            *pdst16++= (VOICEFILEX[VoiceIdx]|(VOICEFILEX[VoiceIdx+1]<<8));
//            VoiceIdx+=2;
        }   
        BufCtl.offset+=size;

#if AGC_ENALBE        
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
    S16 *audio_buf;//, *audio_buf_out;
    //U32 sum=0;
    //S16 *dac_buf;
    //U8 n,Vad_flag;
    U16 i;
    //S32 out;
    //U16 out_size;
    //S32 InSamplesRemain;
#if _RT_PITCH_CHANGE || _RT_GHOST_SOUND || _RT_DARTH_SOUND || _RT_CHORUS_SOUND    
	U8  toggle_process, get_samples=0; 
#endif
	S16 *audio_buf_out; 
    
    
    audio_buf=(S16 *)AudioBuf+AudioBufCtl.out_size*AudioBufCtl.index;
    
    AudioBufCtl.index++;
    if(AudioBufCtl.index==AudioBufCtl.out_count){
        //dprintf("out_count %d\r\n",AudioBufCtl.out_count);
        AudioBufCtl.index=0;   
    }   
	
#if DC_REMOVE
	for(i=0;i<AudioBufCtl.out_size;i++)//unit word
	{
		RunFilter(&audio_buf[i], RunFilterBuf, DCRemove);
	}
#endif	
	
	 
//----------------------------------- Echo Cancel        -------------------------
#if ECHO_CANCELL
#if VOICE_CHANGER_RT_USED
	EchoCancelProcess((S16 *)audio_buf, AudioBufCtl.out_size, VC_RT_ECHO_CANCEL_THRES);
#else
	U16 temp = 2000;
#if _RT_GHOST_SOUND || _RT_CHORUS_SOUND
	if ((Flag_switch_AL==RT_AL_GHOST)||(Flag_switch_AL==RT_AL_CHORUS))
		temp = 1800;
#endif
    EchoCancelProcess((S16 *)audio_buf, AudioBufCtl.out_size, temp);  // default value 2000 is for NX1_FDB Ver.B
#endif //VOICE_CHANGER_RT_USED
#endif    
    
#if NOISE_GATE_ENABLE
    NGProcess_v2((S16 *)audio_buf, &NoiseGatebuf_v2);
#endif   
    // Algorithm Process

//-----------------------Howling suppress--------------------------------//
#if _RT_HOWLING_SUPPRESS
	RT_Howling_Suppress_ServiceLoop((S16*)audio_buf); // Howling Suppress
#endif	
//-----------------------------------------------------------------------//

#if _RT_ROBOT1_SOUND  
    if(RobotCtl.enable){
        if(RobotCtl.change){
            RoboticVoiceInit(_RT_SAMPLE_RATE, CarrierTbl[RobotCtl.index], AudioBufCtl.out_size);
            //dprintf("RobotCtl:%d\r\n",RobotCtl.carrier);
            RobotCtl.change=0;
        }    
        RoboticVoiceProcess((short *)audio_buf,(short *)audio_buf);
    }
#endif             
#if _RT_ROBOT2_SOUND
    if(Robot01Ctl.enable)
    {
    	
#if _ROBOT_SOUND_COMPRESS
        ROBOT_EFFECT_COMPRESS_01((S16 *)audio_buf);
#else
        ROBOT_EFFECT_01((S16 *)audio_buf);
#endif
		
    }
#endif

#if _RT_ROBOT3_SOUND
    if(Robot02Ctl.enable)
    {
    	
#if _ROBOT_SOUND_COMPRESS
        ROBOT_EFFECT_COMPRESS_02((S16 *)audio_buf);
#else            
        ROBOT_EFFECT_02((S16 *)audio_buf);
#endif
		
    }
#endif

#if _RT_ROBOT4_SOUND
    if(Robot04Ctl.enable)
    {      
		ROBOT_EFFECT_04((short *)audio_buf, RTRobot4ctl, RTRobot4buf, RTRobot4FFTctl);
    }
#endif


#if _RT_REVERB_SOUND
    if(ReverbCmd)
    {
        Reverb_Effect((short *)audio_buf,AudioBufCtl.out_size);
    }
#endif

#if _RT_ECHO_SOUND
if(EchoCtrl.Cmd)
{
	S16 *echo_inbuf;
	
	for(i=0 ; i<AudioBufCtl.out_size ; i+=RT_ECHO_DATA_LENGTH)
	{
		echo_inbuf = (S16*)&audio_buf[i];
		EchoSound((S16*)echo_inbuf,RT_ECHO_DATA_LENGTH);
	}
}
#endif

    AudioBufCtl.process_count+=AudioBufCtl.out_size;
    AudioBufCtl.samples+=AudioBufCtl.in_size;  
//---------------------------------------------------//
// no SFX || Echo
	if((Flag_switch_AL==RT_AL_NO_SFX)
	#if _RT_ECHO_SOUND
		|| (Flag_switch_AL==RT_AL_ECHO)
	#endif
	)
	{
		audio_buf_out  = DacBuf+DacBufWrOffset;  
		DacBufWrOffset+= RT_AUDIO_OUT_SIZE_03;  	
		if(DacBufWrOffset==DacBufCtl.size) 
		{
			DacBufWrOffset = 0;
		}
		for(i = 0; i<RT_AUDIO_OUT_SIZE_03;i++)
		{   
		#if _RT_DIGITAL_GAIN_CTRL
			RTDigiAmpProcess(&audio_buf[i], RTDigiAmpBuf, 1, RTDigiAmpCTLBuf);  
		#endif
			audio_buf_out[i] = ((S16)*(audio_buf+i)*AudioVol)>>15;
		}
#if	  _RT_SAMPLE_RATE==SAMPLE_RATE_8000
			if(AudioBufCtl.start_flag==0 &&AudioBufCtl.process_count>=(RT_AUDIO_OUT_SIZE_03*3)) //*2
#elif _RT_SAMPLE_RATE==SAMPLE_RATE_12000
			if(AudioBufCtl.start_flag==0 &&AudioBufCtl.process_count>=(RT_AUDIO_OUT_SIZE_03*4)) //*3
#elif _RT_SAMPLE_RATE==SAMPLE_RATE_16000
			if(AudioBufCtl.start_flag==0 &&AudioBufCtl.process_count>=(RT_AUDIO_OUT_SIZE_03*5)) //*4
#endif
		{
				AudioBufCtl.dac_state = STATUS_PLAYING; 
				AudioBufCtl.adc_state = STATUS_PLAYING; 
				AudioBufCtl.start_flag=1;
		}		
	}
//---------------------------------------------------//	
// Pitch || Ghost || Darth || Chorus
#if _RT_PITCH_CHANGE || _RT_GHOST_SOUND || _RT_DARTH_SOUND || _RT_CHORUS_SOUND
    	else if	((Flag_switch_AL==RT_AL_PITCH) || (Flag_switch_AL==RT_AL_GHOST)
          	  || (Flag_switch_AL==RT_AL_DARTH) || (Flag_switch_AL==RT_AL_CHORUS))
		{	
			for(toggle_process=0; toggle_process<=1; toggle_process++)
			{
				audio_buf_out = DacBuf+DacBufWrOffset; 
		
			#if _RT_DIGITAL_GAIN_CTRL 
				if((Flag_switch_AL==RT_AL_GHOST)&&(!toggle_process))
				{
					for(i=0; i<AudioBufCtl.out_size; i++)
					{
						RTDigiAmpProcess(&audio_buf[i], RTDigiAmpBuf, 1, RTDigiAmpCTLBuf); 
					}
				}
			#endif
                switch(Flag_switch_AL) {
#if _RT_PITCH_CHANGE
                    case RT_AL_PITCH:
                        get_samples = RT_PitchChange_main( toggle_process,
                                                           (S16*)audio_buf,
                                                           (S16*)audio_buf_out,
                                                           (S16*)PitchBuf,
                                                           AudioBufCtl.out_size );
                        break;
#endif
#if _RT_GHOST_SOUND
                    case RT_AL_GHOST:
                        get_samples = RT_GhostSound( toggle_process,
                                                     (S16 *)audio_buf,
                                                     (S16*)audio_buf_out,
                                                     (S16*)PitchBuf,
                                                     AudioBufCtl.out_size );
                        break;
#endif
#if _RT_DARTH_SOUND
                    case RT_AL_DARTH:
                        get_samples = RT_DarthVader( toggle_process,
                                                     (S16 *)audio_buf,
                                                     (S16*)audio_buf_out,
                                                     (S16*)PitchBuf,
                                                     AudioBufCtl.out_size,
                                                     RT_DarthBuf );
                        break;
#endif
#if _RT_CHORUS_SOUND
                    case RT_AL_CHORUS:
                        get_samples = RT_Chorus( toggle_process,
                                                 (S16*)audio_buf,
                                                 (S16*)audio_buf_out,
                                                 (S16*)PitchBuf,
                                                 (S16*)ChorusBuf );
                        break;
#endif
                    default:
                        break;
                }
			
				for(i=0; i<get_samples; i++)
				{	
		#if _RT_DIGITAL_GAIN_CTRL
                if( (Flag_switch_AL==RT_AL_PITCH)
		#if _RT_DARTH_SOUND
                    || (Flag_switch_AL==RT_AL_DARTH)
		#endif
		#if _RT_CHORUS_SOUND
                    || (Flag_switch_AL==RT_AL_CHORUS)
		#endif
                )
				{
					RTDigiAmpProcess(&audio_buf_out[i], RTDigiAmpBuf, 1, RTDigiAmpCTLBuf);  
				}
		#endif
				audio_buf_out[i] = ((S16)*(audio_buf_out+i)*AudioVol)>>15;
			}
			
			if(get_samples==160) {
				DacBufWrOffset+=160;
				
				if(AudioBufCtl.start_flag==0 && DacBufWrOffset>= (RT_AUDIO_OUT_SIZE_01*4)) {			
					AudioBufCtl.dac_state = STATUS_PLAYING;  
					AudioBufCtl.adc_state = STATUS_PLAYING; 
					AudioBufCtl.start_flag=1;	
				}
				
				if(DacBufWrOffset==DacBufCtl.size)
					DacBufWrOffset=0;	
			}
		}
	}
#endif	

#if _RT_REVERB_SOUND || _RT_ROBOT1_SOUND || _RT_ROBOT2_SOUND || _RT_ROBOT3_SOUND || _RT_ROBOT4_SOUND	
	else if	(Flag_switch_AL==RT_AL_OTHER) // run Reverb/Robot_RT
	{
		audio_buf_out  = DacBuf+DacBufWrOffset;  
		DacBufWrOffset+= RT_AUDIO_OUT_SIZE_02;  	
		if(DacBufWrOffset==DacBufCtl.size) 
		{
			DacBufWrOffset = 0;
		}
		
		for(i = 0; i<RT_AUDIO_OUT_SIZE_02;i++)
		{  
		#if _RT_DIGITAL_GAIN_CTRL
            RTDigiAmpProcess(&audio_buf[i], RTDigiAmpBuf, 1, RTDigiAmpCTLBuf);  	
		#endif
			audio_buf_out[i] = ((S16)*(audio_buf+i)*AudioVol)>>15;
		}
		//-----------------------------------------------------------------------------------//
		//if Howling Suppress Enable, delay 1 buffer start playing for heavy loading.

		if(AudioBufCtl.start_flag==0 &&AudioBufCtl.process_count>=(RT_AUDIO_OUT_SIZE_02*2)) //*1
		{
			AudioBufCtl.dac_state = STATUS_PLAYING;  
			AudioBufCtl.adc_state = STATUS_PLAYING; 
			AudioBufCtl.start_flag=1;
		}
		//-----------------------------------------------------------------------------------//
	}
#endif
}
//------------------------------------------------------------------//
// Service loop
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
void RT_ServiceLoop(void)
{
	if(AudioBufCtl.adc_state==STATUS_PLAYING){


        if((AudioBufCtl.fifo_count-AudioBufCtl.process_count)>=AudioBufCtl.out_size)
        {            
			EncodeFrame();			
        }          
    }   
}    

//------------------------------------------------------------------//
// Howling_Suppress Function API
//---------------------------------------//
// Howling Get Status: Get Howling_Suppress now Status
#if _RT_HOWLING_SUPPRESS
U8	RT_Howling_Suppress_GetStatus(void)
{
	if((U8_RT_Howling_Suppress_Swtich==Howling_Suppress_ON) && (U8_RT_Howling_Suppress_Flag==Howling_Suppress_ON))
		return ENABLE;
	else
		return DISABLE;
}
//---------------------------------------//
// Howling Suppress Parameter Init:
// Re-trigger everytime before Howling Suppress enable
void RT_Howling_Suppress_Init(void)
{
	#if _USE_HEAP_RAM
    if(HRInCtrl==NULL)
    {
        U8 *ptr;
        ptr = (U8 *)MemAlloc(RT_HOWLING_RAM); //HRInCtrl, HRFFTCtrl, HRInBuf

        HRInCtrl = (S16 *)ptr;
        HRFFTCtrl= (FFTctl *) (ptr+8);
        HRInBuf  = (HRCtlBuf *) (ptr+8+sizeof(FFTctl));
    }
	#endif
	HRInCtrl[0] = AudioBufCtl.out_size;  //input frame size
    HRInCtrl[1] = 256;
    HRInCtrl[2] = _RT_SAMPLE_RATE;
    HRInCtrl[3] = 127;

	#if (_RT_SAMPLE_RATE == 8000)
    	HowlingRemove_Init(HRInCtrl, HRInBuf, HRFFTCtrl, notch_filter_parameters_8k); //alg will clear HRInBuf/HRFFTCtrl
	#elif (_RT_SAMPLE_RATE == 12000)
    	HowlingRemove_Init(HRInCtrl, HRInBuf, HRFFTCtrl, notch_filter_parameters_12k);
	#elif (_RT_SAMPLE_RATE == 16000) 
    	HowlingRemove_Init(HRInCtrl, HRInBuf, HRFFTCtrl, notch_filter_parameters_16k);
	#endif    

	if(Howling_start_sts==DISABLE)
	{
	    U8_RT_Howling_Suppress_Swtich = Howling_Suppress_ON; // Howling Suppress defualt on
		U8_RT_Howling_Suppress_buf = Howling_Suppress_ON; 
		Howling_start_sts=ENABLE;
	}
}
//---------------------------------------//
// Howling Suppress algorithm Limit
// For which algorithm, bypass Howling Suppress
// Support: RT Play, Echo, Pitch, Reverb, Robot1, Robot2, Robot4, Darth
// UnSupport: Chorus, Ghost, Robot3 (due to RAM Size >90%)
void RT_Howling_Suppress_ALG_Limit(void) 
{
	if((Flag_switch_AL==RT_AL_CHORUS)||(Flag_switch_AL==RT_AL_GHOST))	//Chorus, Ghost
	{
		U8_RT_Howling_Suppress_Swtich = Howling_Suppress_OFF; 	//default off
		U8_RT_Howling_Suppress_buf = Howling_Suppress_OFF;	    
	}
	else if(Flag_switch_AL==RT_AL_OTHER)	//Robot1,2,4+Reverb on, Robot3 off
	{	   	
	   	#if _RT_ROBOT3_SOUND
			if(Robot02Ctl.enable==ENABLE)
			{
	    		U8_RT_Howling_Suppress_Swtich = Howling_Suppress_OFF;
				U8_RT_Howling_Suppress_buf = Howling_Suppress_OFF; 
	 		}
	 	#endif	 		
	}
}
//---------------------------------------//
// Howling Suppress Command for USER
// cmd: ENABLE/DISABLE
void RT_Howling_Suppress_Function(U8 cmd)
{
	if(cmd==0)	// Disable
	{
		if(U8_RT_Howling_Suppress_Swtich!=Howling_Suppress_ON) return;			
		U8_RT_Howling_Suppress_buf=Howling_Suppress_OFF;
	}
	else		// Enable
	{
		if(U8_RT_Howling_Suppress_Swtich!=Howling_Suppress_OFF) return;
		U8_RT_Howling_Suppress_buf=Howling_Suppress_ON;	
		RT_Howling_Suppress_Init();	//Init parameter everytime when Enable.
	}
}
//---------------------------------------//
// Howling Suppress temp buffer Switch
// Storage Howling Suppress status, activity at the next trigger Sound effect API
void RT_Howling_Suppress_Switch(void)	//enable or disable Howling_Suppress need re-trigger sound effect API. 
{
	if(U8_RT_Howling_Suppress_buf==Howling_Suppress_ON)
	{
			U8_RT_Howling_Suppress_Swtich=Howling_Suppress_ON;
			U8_RT_Howling_Suppress_Flag=Howling_Suppress_ON;
	}
	else {
			U8_RT_Howling_Suppress_Swtich=Howling_Suppress_OFF;
			U8_RT_Howling_Suppress_Flag=Howling_Suppress_OFF;
	}
	RT_Howling_Suppress_ALG_Limit(); //  Limit: Chorus, Ghost, Robot3 
}
//---------------------------------------//
// Howling Suppress ServiceLoop
// Howling Detect + Howling Removal
void RT_Howling_Suppress_ServiceLoop(S16 *Audio_buf_ptr)
{
	if((U8_RT_Howling_Suppress_Swtich==Howling_Suppress_ON) && (U8_RT_Howling_Suppress_Flag==Howling_Suppress_ON))
	{
		HowlingDetect((S16*)Audio_buf_ptr, HRInBuf, HRFFTCtrl);	
		HowlingRemove((S16*)Audio_buf_ptr, HRInCtrl[0], HRInBuf); 
	}
}
#endif
#endif
//------------------------------------------------------------------//
