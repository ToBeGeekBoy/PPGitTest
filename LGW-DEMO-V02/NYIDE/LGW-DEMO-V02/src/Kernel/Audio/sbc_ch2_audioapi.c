/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "include.h"
#include "nx1_dac.h"

#if _SBC_CH2_PLAY

/* Defines -------------------------------------------------------------------*/

#define SMOOTH_ENABLE          1

#if		_SBC_CH2_PLAY_BACK_EQ==ENABLE
	#include "FilterTab.h"
	#if	FILTER_NUMBER<=0//check table size
		#error "Filter table is not existed, please update NYIDE and create filter file."
	#endif
	#define _SBC_PLAY_BACK_USE_CUSTOMIZE//define EQ customize
#endif
S_AUDIO_FUNC_IDX s_SBC_CH2_FUNC = {
    .Play           =   SBC_CH2_Play,
    .Pause          =   SBC_CH2_Pause,
    .Resume         =   SBC_CH2_Resume,
    .Stop           =   SBC_CH2_Stop,
    .SetVolume      =   SBC_CH2_SetVolume,
    .GetStatus      =   SBC_CH2_GetStatus,
#if defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
    .SetEQFilter    =   SBC_CH2_SetEQFilter,
#endif
#if _SBC_CH2_SPEED_CHANGE
    .SetSpeed       =   SBC_CH2_SetSpeed,
#endif
#if _SBC_CH2_PITCH_CHANGE
    .SetPitch       =   SBC_CH2_SetPitch,
#endif
#if _SBC_CH2_ROBOT1_SOUND
    .SetRobot1Sound =   SBC_CH2_SetRobot1Sound,
#endif
#if _SBC_CH2_ROBOT2_SOUND
    .SetRobot2Sound =   SBC_CH2_SetRobot2Sound,
#endif
#if _SBC_CH2_ROBOT3_SOUND
    .SetRobot3Sound =   SBC_CH2_SetRobot3Sound,
#endif
#if _SBC_CH2_REVERB_SOUND
    .SetReverbSound =   SBC_CH2_SetReverbSound,
#endif
#if _SBC_CH2_ECHO_SOUND
    .SetEchoSound   =   SBC_CH2_SetEchoSound,
#endif
#if _AUDIO_MIX_CONTROL
    .SetMixCtrl     =   SBC_CH2_SetMixCtrl,
#endif
};
/* Static Functions ----------------------------------------------------------*/

/* Constant Table ------------------------------------------------------------*/
#if _SBC_CH2_SPEED_CHANGE
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

#if _SBC_CH2_PITCH_CHANGE
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
#if _SBC_CH2_ROBOT1_SOUND 
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
#if _SBC_CH2_ECHO_SOUND
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

/* Global Variables ----------------------------------------------------------*/
#if _AUDIO_MIX_CONTROL
U8 SbcMixCtrl_CH2=MIX_CTRL_100;
#endif

/* Static Variables ----------------------------------------------------------*/
static volatile struct AUDIO_BUF_CTL{
    S32 process_count;	//decode/encode count
    S32 fifo_count;	    //decode/encode count
    U32 samples;	    //samples
#if _SBC_CH2_SPEED_CHANGE||_SBC_CH2_PITCH_CHANGE||_SBC_CH2_QIO_MODULE
    U32 sample_count;	    //samples
    U32 fifo_count2;	    //decode/encode count
#endif//_SBC_CH2_SPEED_CHANGE||_SBC_CH2_PITCH_CHANGE||_SBC_CH2_QIO_MODULE
    //U16 vol;            //volume
    U16 sample_rate;    //sample_rate
    U8 in_size;         //data frame size
    U8 state;           //channel state
    U8 shadow_state;    //channel shadow state
    //U8 ramp;            //smooth ramp
    U8 index;
    U8 decode_state;
	U8 upsample;
}AudioBufCtl;

static U16 AudioVol;
static U8  OpMode,ChVol;

static volatile struct{
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

#if _SBC_CH2_SPEED_CHANGE||_SBC_CH2_PITCH_CHANGE
static struct{
    U16 index;
    U8 speed_ratio_index;            //speed control ratio
    U8 pitch_ratio_index;            //pitch control ratio
}SpeedCtl;

#endif


#if _SBC_CH2_ROBOT1_SOUND 
static struct{
    U8 enable;
    U8 change;
    U8 index;
}RobotCtl;
#endif 

#if _SBC_CH2_ROBOT2_SOUND
static struct{
    U8 enable;
    U8 type;
    U16 thres;
    U16 detectthres;
}Robot01Ctl;
#endif 
#if _SBC_CH2_ROBOT3_SOUND
static struct{
    U8 enable;
    U8 type;
    U16 thres;
}Robot02Ctl;
#endif 

#if _SBC_CH2_REVERB_SOUND
static U8 ReverbCmd,ReverbEffect;
#endif

#if _SBC_CH2_ECHO_SOUND
static EchoCtl_T EchoCtl;
static S8 EchoCmd;
#endif

static U8  StorageMode;
static U32 StartAddr,CurAddr;

static U8  PlayEndFlag;
#if _AUDIO_VOLUME_CHANGE_SMOOTH
static volatile VOLCTRL_T    VolCtrl;
#endif

#if defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
static EQCtl EQFilterCtl;
#endif
#if defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
static U8	u8EQFilterIndex=0;
#endif

/* Dynamic Allocate RAM-------------------------------------------------------*/
#if _USE_HEAP_RAM
static U16 *SBCRam=NULL;
static U16 *AudioBuf=NULL;
#if _SBC_CH2_SPEED_CHANGE||_SBC_CH2_PITCH_CHANGE
static U16 *SpeedCtlBuf=NULL;
#endif
#if _SBC_CH2_ECHO_SOUND
static S16 *EchoBuf=NULL;
#endif

#if _SBC_CH2_ROBOT2_SOUND
#if _ROBOT_SOUND_COMPRESS
#define SBC_ROBOT_BUF_SIZE      800
#else
#define SBC_ROBOT_BUF_SIZE      1600
#endif
static S16 *Robot01_buf=NULL;
#endif

#if _SBC_CH2_ROBOT3_SOUND
#if _ROBOT_SOUND_COMPRESS
#define SBC_ROBOT02_BUF_SIZE      (36+320+1000)
#else
#define SBC_ROBOT02_BUF_SIZE      (36+320+2000)
#endif
static S16 *Robot02_buf=NULL;
#endif

#if _SBC_CH2_REVERB_SOUND
#define SBC_REVERB_BUF_SIZE   830 //word
static S16 *Reverb_x=NULL;
static S16 *Reverb_y=NULL;
#endif

#if defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
static S32 *EqBuf=NULL;
#endif

#else
static U16 SBCRam[SBC_CH2_RAM_ALLOCATE_SIZE];
static U16 AudioBuf[SBC_CH2_AUDIO_BUF_SIZE];
#if _SBC_CH2_SPEED_CHANGE||_SBC_CH2_PITCH_CHANGE
static U16 SpeedCtlBuf[SBC_DEC_OUT_SIZE+SPEED_CTL_FRAME_SIZE];
#endif
#if _SBC_CH2_ECHO_SOUND
static S16 EchoBuf[SBC_ECHO_BUF_SIZE];
#endif
#if _SBC_CH2_ROBOT2_SOUND
static S16 Robot01_buf[SBC_ROBOT_BUF_SIZE];
#endif

#if _SBC_CH2_ROBOT3_SOUND
static S16 Robot02_buf[SBC_ROBOT02_BUF_SIZE];
#endif

#if _SBC_CH2_REVERB_SOUND
static S16 Reverb_x[SBC_REVERB_BUF_SIZE];
static S16 Reverb_y[SBC_REVERB_BUF_SIZE];
#endif

#if defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
static S32 EqBuf[SBC_EQ_BUF_SIZE];
#endif

#endif



/* Extern Functions ----------------------------------------------------------*/
extern void CB_ReadData(U8 mode, U32 *cur_addr, U8 *buf,U16 size);
extern U8   CB_SetStartAddr(U32 addr,U8 mode,U8 rw,U32 *start_addr);
extern void CB_SBC_CH2_PlayStart(void);
extern void CB_SBC_CH2_PlayEnd(void);
extern void CB_SBC_CH2_InitDac(CH_TypeDef *chx,U16 sample_rate,U8 upsample);
extern void CB_SBC_CH2_DacCmd(CH_TypeDef *chx,U8 cmd);
extern void CB_SBC_CH2_ChangeSR(CH_TypeDef *chx,U16 sample_rate);
extern void CB_SBC_CH2_WriteDac(U8 size,U16* psrc16);

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
        SBCRam   = (U16 *)MemAlloc(SBC_CH2_RAM_ALLOCATE_SIZE<<1);
        AudioBuf = (U16 *)MemAlloc(SBC_CH2_AUDIO_BUF_SIZE<<1);
#if _SBC_CH2_SPEED_CHANGE||_SBC_CH2_PITCH_CHANGE
        SpeedCtlBuf = MemAlloc((SBC_DEC_OUT_SIZE+SPEED_CTL_FRAME_SIZE)<<1);
#endif
#if _SBC_CH2_ECHO_SOUND
        EchoBuf = (U16 *)MemAlloc(SBC_ECHO_BUF_SIZE<<1);
#endif        
    }
    //else
    //    dprintf("Allocate twice. Ignore...\r\n");
    
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
#if _SBC_CH2_SPEED_CHANGE||_SBC_CH2_PITCH_CHANGE
        MemFree(SpeedCtlBuf);
        SpeedCtlBuf=NULL;
#endif
        //dprintf("Free Mem\r\n");
#if _SBC_CH2_ROBOT2_SOUND    
        if(Robot01_buf!=NULL)
        {
            MemFree(Robot01_buf);
            Robot01_buf = NULL;
        }
#endif
#if _SBC_CH2_ROBOT3_SOUND    
        if(Robot02_buf!=NULL)
        {
            MemFree(Robot02_buf);
            Robot02_buf = NULL;
        }
#endif
#if _SBC_CH2_REVERB_SOUND
        if(Reverb_x!=NULL)
        {
            MemFree(Reverb_x);
            Reverb_x = NULL;
            MemFree(Reverb_y);
            Reverb_y = NULL;
        }
#endif
#if _SBC_CH2_ECHO_SOUND
		MemFree(EchoBuf);
		EchoBuf=NULL;
#endif
#if defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
        if(EqBuf!=NULL)
        {
            MemFree(EqBuf);
            EqBuf=NULL;
            EQFilterCtl.Buf=NULL;
        }
#endif
    }
    //else
    //{
    //    dprintf("Already Free\r\n");
    //}

}
#endif
//------------------------------------------------------------------//
// ISR
// Parameter: 
//          size: fifo fill size 
// return value:
//          NONE
//------------------------------------------------------------------// 
void SBC_CH2_DacIsr(U8 size){
    U16 temp;
#if _SBC_CH2_QIO_MODULE
	U16 _QioFrameFlag;
#endif//_SBC_CH2_QIO_MODULE
    S16 *psrc16,gain;
    U8 i,len,size2;
    
	if(AudioBufCtl.shadow_state==STATUS_PLAYING){
#if _SBC_CH2_QIO_MODULE      
		QIO_Resume(QIO_USE_SBC_CH2);
#endif//_SBC_CH2_QIO_MODULE
         
        temp=BufCtl.size-BufCtl.offset;
        if(size>temp){
            size=temp;
        }   
#if _SBC_CH2_QIO_MODULE
		for(i=0;i<size;i++){
			QIO_FrameFlag_FIFO_BitGet(QIO_USE_SBC_CH2,(U8*)&_QioFrameFlag);
			QIO_DacIsr(QIO_USE_SBC_CH2, (U8)_QioFrameFlag);
		}
#endif//_SBC_CH2_QIO_MODULE
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
#if _SBC_CH2_QIO_MODULE
						if(AudioBufCtl.state == STATUS_STOP)
						{
							QIO_DelayStop(QIO_USE_SBC_CH2,ENABLE);
						}
						else
						{
							QIO_Pause(QIO_USE_SBC_CH2);
						}
#endif//_SBC_CH2_QIO_MODULE
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
        CB_SBC_CH2_WriteDac(size,(U16 *)psrc16);
        
        BufCtl.offset+=size;

        if(BufCtl.offset==BufCtl.size){
            BufCtl.offset=0;    
        }       
        
#if _SBC_CH2_SPEED_CHANGE||_SBC_CH2_PITCH_CHANGE
        if(AudioBufCtl.fifo_count>=AudioBufCtl.fifo_count2){
            AudioBufCtl.state=STATUS_STOP;
            AudioBufCtl.shadow_state=AudioBufCtl.state;
            PlayEndFlag = BIT_SET;
            //dprintf("stop s,%d,%d\r\n",AudioBufCtl.fifo_count,AudioBufCtl.fifo_count2);
        }
#else
        if(AudioBufCtl.fifo_count>=AudioBufCtl.samples){
#if _SBC_CH2_QIO_MODULE
			QIO_DelayStop(QIO_USE_SBC_CH2,DISABLE);
			QIO_FrameFlag_FIFO_RstGet(QIO_USE_SBC_CH2);
#endif//_SBC_CH2_QIO_MODULE
            AudioBufCtl.state=STATUS_STOP;
            AudioBufCtl.shadow_state=AudioBufCtl.state;
            PlayEndFlag = BIT_SET;
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
        CB_SBC_CH2_WriteDac(size,(U16 *)0);
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
	if(SBC_CH2_GetStatus()!=STATUS_STOP)return EXIT_FAILURE;//error
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
#if _SBC_CH2_SPEED_CHANGE||_SBC_CH2_PITCH_CHANGE
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
    U16 *audio_buf,i,size,len;
    S16 *pdest,gain;
    U32 speed_ctl_size; 
    
    //while((SPEED_CTL_FRAME_SIZE*2)-SpeedCtl.index>=SPEED_CTL_FRAME_SIZE){
    if(SpeedCtl.index<SPEED_CTL_FRAME_SIZE){
        
        if(AudioBufCtl.sample_count<AudioBufCtl.samples){
            audio_buf=SpeedCtlBuf+SpeedCtl.index;
        
            CB_ReadData(StorageMode,&CurAddr,(U8 *)audio_buf,2);
        
            // Modify for Encode bit number error
        	bpf=SBCBPF(1,audio_buf[0],&mark_sample);
        	
        	if(mark_sample!=0)
        	{
        		CB_ReadData(StorageMode,&CurAddr,(U8 *)audio_buf,2);
        		bpf=SBCBPF(1,audio_buf[0],&mark_sample);
        	}
        	
            CB_ReadData(StorageMode,&CurAddr,(U8 *)(audio_buf+1),bpf-2);
            
            SBCDecode(1,SBC_DEC_OUT_SIZE,(void *)audio_buf,0);  
            
            SBCDecode(1,SBC_DEC_OUT_SIZE,(void *)audio_buf,1);
       
#if _SBC_CH2_ROBOT2_SOUND
            if(Robot01Ctl.enable)
            {
#if _ROBOT_SOUND_COMPRESS                    
                ROBOT_EFFECT_COMPRESS_01((S16 *)audio_buf);
#else
                ROBOT_EFFECT_01((S16 *)audio_buf);
#endif
            }
#endif
#if _SBC_CH2_ROBOT3_SOUND
            if(Robot02Ctl.enable)
            {
#if _ROBOT_SOUND_COMPRESS
                ROBOT_EFFECT_COMPRESS_02((S16 *)audio_buf);
#else
                ROBOT_EFFECT_02((S16 *)audio_buf);
#endif

            }
#endif      
     
#if _SBC_CH2_REVERB_SOUND
            if(ReverbCmd)
            {
                Reverb_Effect((short *)audio_buf,SBC_DEC_OUT_SIZE);
            }
#endif     
#if _SBC_CH2_ECHO_SOUND
            if(EchoCmd)
            {
                EchoVoiceProcess(audio_buf, SBC_DEC_OUT_SIZE, &EchoCtl);
            }
#endif
            SpeedCtl.index+=SBC_DEC_OUT_SIZE;
            AudioBufCtl.sample_count+=SBC_DEC_OUT_SIZE;
            AudioBufCtl.fifo_count2=AudioBufCtl.fifo_count+(SBC_CH2_AUDIO_BUF_SIZE*2);
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
    
    audio_buf=AudioBuf+(SBC_CH2_AUDIO_OUT_SIZE*AudioBufCtl.index);
    
    AudioBufCtl.index++;
    if(AudioBufCtl.index==SBC_CH2_AUDIO_OUT_COUNT){
        AudioBufCtl.index=0;    
    }    
    
    if(SpeedCtl.speed_ratio_index==RATIO_INIT_INDEX && SpeedCtl.pitch_ratio_index==RATIO_INIT_INDEX ){
        speed_ctl_size=SBC_CH2_AUDIO_OUT_SIZE;
        for(i=0;i<SBC_CH2_AUDIO_OUT_SIZE;i++){
            audio_buf[i]=SpeedCtlBuf[i];
        }   
    }else{   
        speed_ctl_size=SPEED_CTL_FRAME_SIZE;
     
    //GPIO_Write(GPIOA,0,0);    
        TimeStretchProcess(audio_buf, SpeedCtlBuf, &speed_ctl_size);
        speed_ctl_size=SPEED_CTL_FRAME_SIZE-speed_ctl_size; 
    }
    //dprintf("%d,%d\r\n",speed_ctl_size,size);
    SpeedCtl.index-=speed_ctl_size;
    for(i=0;i<SpeedCtl.index;i++){
        SpeedCtlBuf[i]=SpeedCtlBuf[i+speed_ctl_size];
    }    
    AudioBufCtl.process_count+=SBC_CH2_AUDIO_OUT_SIZE;
#if _SBC_CH2_ROBOT1_SOUND
    if(RobotCtl.enable){
        if(RobotCtl.change){
            RoboticVoiceInit(AudioBufCtl.sample_rate, CarrierTbl[RobotCtl.index], SBC_CH2_AUDIO_OUT_SIZE);
            //dprintf("RobotCtl:%d\r\n",RobotCtl.carrier);
            RobotCtl.change=0;
        }    
        RoboticVoiceProcess((short *)audio_buf,(short *)audio_buf);
    }
#endif 
        
    //volume 
    for(i=0;i<SBC_CH2_AUDIO_OUT_SIZE;i++){
#if defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
		audio_buf[i] = EQProcess(audio_buf[i],&EQFilterCtl);
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
#if	_SBC_CH2_QIO_MODULE
	U16 _QioFrameFlag,size;
#endif//_SBC_CH2_QIO_MODULE
    
    if(AudioBufCtl.index){
        audio_buf=AudioBuf+SBC_CH2_AUDIO_OUT_SIZE;
    }else{
        audio_buf=AudioBuf;
    }        
    
//    if(mute){
//        for(i=0;i<SBC_DEC_OUT_SIZE;i++){
//            audio_buf[i]=0;
//        }   
//        AudioBufCtl.toggle^=1;
//        return;        
//    }        
    if(!AudioBufCtl.decode_state){
       
        CB_ReadData(StorageMode,&CurAddr,(U8 *)audio_buf,2);
        
        // Modify for Encode bit number error
		bpf=SBCBPF(1,audio_buf[0],&mark_sample);
        	
#if	_SBC_CH2_QIO_MODULE
    	_QioFrameFlag = (mark_sample!=0) ? mark_sample : 0;
		size = SBC_DEC_OUT_SIZE;
		if((size+AudioBufCtl.sample_count)>AudioBufCtl.samples)
		{
			size=AudioBufCtl.samples-AudioBufCtl.sample_count;
		}
		if(QIO_FrameFlag_FIFO_BitPut(QIO_USE_SBC_CH2,(U8*)&_QioFrameFlag)==EXIT_SUCCESS)
		{
			_QioFrameFlag = 0;
			for(i=1;i<size;i++)
			{
				QIO_FrameFlag_FIFO_BitPut(QIO_USE_SBC_CH2,(U8*)&_QioFrameFlag);
			}
		}
		AudioBufCtl.sample_count+=size;
		if(AudioBufCtl.samples == AudioBufCtl.sample_count)
		{
			QIO_FrameFlag_FIFO_RstPut(QIO_USE_SBC_CH2);
		}
#endif//_SBC_CH2_QIO_MODULE
    	if(mark_sample!=0)
    	{
    		CB_ReadData(StorageMode,&CurAddr,(U8 *)audio_buf,2);
    		bpf=SBCBPF(1,audio_buf[0],&mark_sample);
    	}
        CB_ReadData(StorageMode,&CurAddr,(U8 *)(audio_buf+1),bpf-2);

        SBCDecode(1,SBC_CH2_AUDIO_OUT_SIZE,(void *)audio_buf,0);
        
    }else{
        SBCDecode(1,SBC_CH2_AUDIO_OUT_SIZE,(void *)audio_buf,1);
        AudioBufCtl.process_count+=SBC_CH2_AUDIO_OUT_SIZE;
        
        AudioBufCtl.index^=1;

#if _SBC_CH2_ROBOT2_SOUND
        if(Robot01Ctl.enable)
        {
#if _ROBOT_SOUND_COMPRESS                    
            ROBOT_EFFECT_COMPRESS_01((S16 *)audio_buf);
#else
            ROBOT_EFFECT_01((S16 *)audio_buf);
#endif
        }
#endif
#if _SBC_CH2_ROBOT3_SOUND
        if(Robot02Ctl.enable)
        {
#if _ROBOT_SOUND_COMPRESS
            ROBOT_EFFECT_COMPRESS_02((S16 *)audio_buf);
#else
            ROBOT_EFFECT_02((S16 *)audio_buf);
#endif

        }
#endif


#if _SBC_CH2_ROBOT1_SOUND  
        if(RobotCtl.enable){
            if(RobotCtl.change){
                RoboticVoiceInit(AudioBufCtl.sample_rate, CarrierTbl[RobotCtl.index], SBC_CH2_AUDIO_OUT_SIZE);
                //dprintf("RobotCtl:%d\r\n",RobotCtl.carrier);
                RobotCtl.change=0;
            }    
            RoboticVoiceProcess((short *)audio_buf,(short *)audio_buf);
        }
#endif    
//         for(i=0;i<SBC_DEC_OUT_SIZE;i++){
//                   audio_buf[i]=0;
//          }      
#if _SBC_CH2_REVERB_SOUND
        if(ReverbCmd)
        {
            Reverb_Effect((short *)audio_buf,SBC_DEC_OUT_SIZE);
        }
#endif
                               
#if _SBC_CH2_ECHO_SOUND
        if(EchoCmd)
        {
            EchoVoiceProcess((S16 *)audio_buf, SBC_DEC_OUT_SIZE, &EchoCtl);
        }
#endif
        //volume 
        for(i=0;i<SBC_CH2_AUDIO_OUT_SIZE;i++){
#if defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
			audio_buf[i] = EQProcess(audio_buf[i],&EQFilterCtl);
#endif
#if _AUDIO_VOLUME_CHANGE_SMOOTH
            audio_buf[i]=((S16)audio_buf[i]*AUDIO_GetVolumeSmooth(&VolCtrl))>>Q15_SHIFT;
#else              
            audio_buf[i]=((S16)audio_buf[i]*AudioVol)>>Q15_SHIFT;
#endif
        }   
        
    }    
    
    AudioBufCtl.decode_state^=1;
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
U8 SBC_CH2_Play(U32 index,U8 mode){
    struct AUDIO_HEADER *pheader;
    
    if(AudioBufCtl.shadow_state==STATUS_PLAYING){
#if SMOOTH_ENABLE        
            AudioBufCtl.state=STATUS_STOP;
            SmoothFunc(SMOOTH_DOWN);
            if(!DacMixerRamp_IsBias()) {
                AudioBufCtl.shadow_state=STATUS_STOP;
#if _SBC_CH2_QIO_MODULE      
                QIO_Stop(QIO_USE_SBC_CH2,ENABLE);
#endif
            }
            while(AudioBufCtl.shadow_state==STATUS_PLAYING){
                AUDIO_ServiceLoop();
            }
#endif        
    }
#if _USE_HEAP_RAM    
#if _VR_FUNC
    VR_Halt();
#endif
	AUDIO_ServiceLoop();
    SBC_AllocateMemory(MEMORY_ALLOCATE_PLAY);
#else
	AUDIO_ServiceLoop();
#endif
#if _SBC_CH2_ECHO_SOUND
    memset((void *)EchoBuf, 0, SBC_ECHO_BUF_SIZE<<1);
#endif
    memset((void *)&AudioBufCtl, 0, sizeof(AudioBufCtl));
    
#if _SBC_CH2_SPEED_CHANGE||_SBC_CH2_PITCH_CHANGE
    AudioBufCtl.fifo_count2=-1;
#endif    
//    AudioBufCtl.ramp=ramp;
    
//    SmoothCtl.p_parameter=0;
    
    BufCtl.offset=0;
    BufCtl.size=SBC_CH2_AUDIO_BUF_SIZE;
    BufCtl.buf=AudioBuf;
    
    if(CB_SetStartAddr(index,mode,0,&StartAddr)){
		return E_AUDIO_INVALID_IDX;
	}
    CurAddr=StartAddr;
    StorageMode=mode;    
    
    CB_ReadData(StorageMode,&CurAddr,(U8 *)AudioBuf,HEADER_LEN);
    
    pheader=(struct AUDIO_HEADER *)AudioBuf;
    
//    dprintf("Play:%x,%x,%x\r\n",pheader->signature[0],pheader->signature[1],pheader->signature[2]);
    
    if(pheader->codec!=CODEC_SBC && pheader->codec!=CODEC_SBC_E){
        return E_AUDIO_UNSUPPORT_CODEC;    
    }     

	GIE_DISABLE();
#if	_SBC_CH2_PITCH_CHANGE
    S8 hw_ch = DacAssigner_Regist(kSBC_CH2,SBC_CH2_EXCLUSIVE);
#else
	S8 hw_ch = DacAssigner_Regist(kSBC_CH2,pheader->sample_rate);
#endif
	GIE_ENABLE();
	if(hw_ch<0){
        //dprintf("No play Invalid sr\r\n");
        return E_AUDIO_INVALID_SAMPLERATE;
    }
	//dprintf("codec %d reg hw_ch %d\r\n",kSBC_CH2,hw_ch);

#if _SBC_CH2_QIO_MODULE
	if(pheader->nyq_offset)
	{
		QIO_Play(QIO_USE_SBC_CH2, mode, (StartAddr+HEADER_LEN+pheader->nyq_offset));
	}
#endif//_SBC_CH2_QIO_MODULE
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
    
    SBCDecInit(1,SBCRam,AudioBufCtl.sample_rate,pheader->bandwidth,pheader->bit_per_frame&0x7FFF,pheader->decode_in_size,pheader->decode_out_size,pheader->bit_per_frame>>15);     
	if(pheader->codec==CODEC_SBC_E && pheader->samples>SBC_DEC_OUT_SIZE){
		CB_ReadData(StorageMode,&CurAddr,(U8 *)(SBCRam+320),160<<1);
	}
	
//#if _SBC_CH2_SPEED_CHANGE
//    for(i=0;i<AUDIO_BUF_COUNT;i++){
//        DecodeFrame();    
//    }    
//#else    
//    DecodeFrame();
//    DecodeFrame();
//#endif    
    
    memset((void *)AudioBuf, 0, SBC_CH2_AUDIO_BUF_SIZE*2);
    
    //dprintf("AudioBufCtl.process_count,%d\r\n",AudioBufCtl.process_count);
	CB_SBC_CH2_InitDac(pHwCtl[hw_ch].p_dac,AudioBufCtl.sample_rate,AudioBufCtl.upsample);
    
#if _SBC_CH2_SPEED_CHANGE
    SpeedCtl.index=0;
#if _SBC_CH2_PITCH_CHANGE        
    TimeStretchInit(1,SpeedRatioTbl[SpeedCtl.speed_ratio_index]/PitchRatioTbl[SpeedCtl.pitch_ratio_index]);
#else
    TimeStretchInit(1,SpeedRatioTbl[SpeedCtl.speed_ratio_index]);
#endif
    
#endif
#if _SBC_CH2_PITCH_CHANGE
    SpeedCtl.index=0;
#if _SBC_CH2_SPEED_CHANGE
    TimeStretchInit(0,SpeedRatioTbl[SpeedCtl.speed_ratio_index]/PitchRatioTbl[SpeedCtl.pitch_ratio_index]);
#else
    TimeStretchInit(0,1/PitchRatioTbl[SpeedCtl.pitch_ratio_index]);
#endif    


	CB_SBC_CH2_ChangeSR(pHwCtl[hw_ch].p_dac,AudioBufCtl.sample_rate*PitchRatioTbl[SpeedCtl.pitch_ratio_index]);




#endif
#if _SBC_CH2_ROBOT2_SOUND
    if(Robot01Ctl.enable)
    {
#if _USE_HEAP_RAM
        if(Robot01_buf==NULL)   Robot01_buf = (S16 *) MemAlloc(SBC_ROBOT_BUF_SIZE<<1);
#endif
        memset((void *)Robot01_buf, 0, SBC_ROBOT_BUF_SIZE*2);
        ROBOT_EFFECT_01_Init(Robot01_buf,Robot01Ctl.type,Robot01Ctl.thres,Robot01Ctl.detectthres,AudioBufCtl.sample_rate,SBC_DEC_OUT_SIZE,40,1600);
    }
#endif
#if _SBC_CH2_ROBOT3_SOUND
    if(Robot02Ctl.enable)
    {
#if _USE_HEAP_RAM
        if(Robot02_buf==NULL)   Robot02_buf = (S16 *) MemAlloc(SBC_ROBOT02_BUF_SIZE<<1);
#endif
        memset((void *)Robot02_buf, 0, SBC_ROBOT02_BUF_SIZE*2);
        ROBOT_EFFECT_02_Init(Robot02_buf,Robot02Ctl.type,Robot02Ctl.thres,AudioBufCtl.sample_rate);
    }
#endif

#if _SBC_CH2_REVERB_SOUND
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
#if defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
	if(initPlayBackEQParameter()!=EXIT_SUCCESS){
		return 1;//init error
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


#if _SBC_CH2_SPEED_CHANGE||_SBC_CH2_PITCH_CHANGE
    DecodeFrame();
#else    
    DecodeFrame();
    DecodeFrame();
#endif
    OpMode=0;
    AudioBufCtl.state=  STATUS_PLAYING;
    AudioBufCtl.shadow_state=AudioBufCtl.state;
    
    CB_SBC_CH2_PlayStart();   
   
    CB_SBC_CH2_DacCmd(pHwCtl[hw_ch].p_dac,ENABLE);

    
    return 0;
}   
//------------------------------------------------------------------//
// Pause playing
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void SBC_CH2_Pause(void){
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
} 
//------------------------------------------------------------------//
// Resume playing
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void SBC_CH2_Resume(void){
	if(AudioBufCtl.state==STATUS_PAUSE&&(!OpMode)){
        if(DacAssigner_codecUseDacCh(kSBC_CH2)<0) {
            GIE_DISABLE();
#if	_SBC_CH2_PITCH_CHANGE
            S8 hw_ch = DacAssigner_Regist(kSBC_CH2,SBC_CH2_EXCLUSIVE);
#else
            S8 hw_ch = DacAssigner_Regist(kSBC_CH2,AudioBufCtl.sample_rate);
#endif
            GIE_ENABLE();
            if(hw_ch<0) return;//error
        
            CB_SBC_CH2_InitDac(pHwCtl[hw_ch].p_dac,AudioBufCtl.sample_rate,AudioBufCtl.upsample);
#if	_SBC_CH2_PITCH_CHANGE
            SBC_CH2_SetPitch(SpeedCtl.pitch_ratio_index-RATIO_INIT_INDEX);
#endif
            CB_SBC_CH2_DacCmd(pHwCtl[hw_ch].p_dac,ENABLE);
        }

#if SMOOTH_ENABLE         
//        if(AudioBufCtl.ramp&RAMP_UP){
            SmoothFunc(SMOOTH_UP);
//        }
#endif        
        AudioBufCtl.state=STATUS_PLAYING;
        AudioBufCtl.shadow_state=AudioBufCtl.state;
    }            
} 
//------------------------------------------------------------------//
// Stop playing
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void SBC_CH2_Stop(void){
    
    if((AudioBufCtl.state==STATUS_PLAYING||AudioBufCtl.state==STATUS_PAUSE)&&(!OpMode)){
#if SMOOTH_ENABLE         
//        if(AudioBufCtl.ramp&RAMP_DOWN){
            AudioBufCtl.state=STATUS_STOP;
            SmoothFunc(SMOOTH_DOWN);
			if (AudioBufCtl.shadow_state == STATUS_PAUSE
                || !DacMixerRamp_IsBias()
            ) {
				AudioBufCtl.shadow_state = STATUS_STOP;
#if _SBC_CH2_QIO_MODULE
				QIO_Stop(QIO_USE_SBC_CH2,ENABLE);
#endif//_SBC_CH2_QIO_MODULE
			}
//        }else
#endif        
//        {
//            AudioBufCtl.state=STATUS_STOP;
//            CB_SBC_CH2_PlayEnd();
//        }     
    } 
} 
//------------------------------------------------------------------//
// Get current status
// Parameter: 
//          NONE
// return value:
//          status:STATUS_PLAYING,STATUS_PAUSE,STATUS_STOP
//------------------------------------------------------------------// 
U8 SBC_CH2_GetStatus(void){
    return AudioBufCtl.shadow_state;    
}
//------------------------------------------------------------------//
// Set audio volume
// Parameter: 
//          vol:0~15
// return value:
//          NONE
//------------------------------------------------------------------// 
void SBC_CH2_SetVolume(U8 vol){   
    if(vol>CH_VOL_15){
        vol=CH_VOL_15;
    }      
    ChVol=vol;   
}
#if defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
//------------------------------------------------------------------//
// Set customize filter index
// Parameter: 
//          index: (0~FILTER_NUMBER-1)User define filter, (other)All Pass
// return value:
//          NONE
//------------------------------------------------------------------// 
void SBC_CH2_SetEQFilter(U8 index){
	u8EQFilterIndex = index;
#if defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
	if(u8EQFilterIndex >= FILTER_NUMBER){
		EQFilterCtl.FilterPara1 = _AllPassFilterParaTab;//all pass parameter
	} else {
		EQFilterCtl.FilterPara1 = (S16*)FilterList[u8EQFilterIndex];//UserDefine
	}
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
void SBC_CH2_SetSpeed(S8 index){   
    
#if _SBC_CH2_SPEED_CHANGE
    if(AudioBufCtl.sample_rate<=SPEED_CTL_SAMPLE_RATE){
        index+=RATIO_INIT_INDEX;
        SpeedCtl.speed_ratio_index=index;
#if _SBC_CH2_PITCH_CHANGE        
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
void SBC_CH2_SetPitch(S8 index){   
    
#if _SBC_CH2_PITCH_CHANGE
    if(AudioBufCtl.sample_rate<=SPEED_CTL_SAMPLE_RATE){
        index+=RATIO_INIT_INDEX;
        SpeedCtl.pitch_ratio_index=index;
#if _SBC_CH2_SPEED_CHANGE        
        TimeStretchInit(0,SpeedRatioTbl[SpeedCtl.speed_ratio_index]/PitchRatioTbl[index]);
#else
        TimeStretchInit(0,1/PitchRatioTbl[index]);
#endif
#if defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
        EQFilterCtl.PitchIndex=0;
#endif
        S8	hw_ch = DacAssigner_codecUseDacCh(kSBC_CH2);
		if(hw_ch<0)return;//No register hw_ch
        CB_SBC_CH2_ChangeSR(pHwCtl[hw_ch].p_dac,AudioBufCtl.sample_rate*PitchRatioTbl[index]);

    }
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
void SBC_CH2_SetRobot1Sound(U8 cmd,U8 index){
#if _SBC_CH2_ROBOT1_SOUND   
   
    RobotCtl.enable=cmd;
    
    if(RobotCtl.enable){
        RobotCtl.change=1;
        RobotCtl.index=index;
    }    
#endif    
}  
#if _SBC_CH2_ROBOT2_SOUND 
//------------------------------------------------------------------//
// Enable/Disable Robot Sound Effect
// Parameter: 
//          cmd: 1:enable,0:disable
//          type: robot type 0~2
//          thres: robot threshold 0~7
// return value:
//          NONE
//------------------------------------------------------------------// 
void SBC_CH2_SetRobot2Sound(U8 cmd,U8 type, U8 thres){
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

#if _SBC_CH2_ROBOT3_SOUND 
//------------------------------------------------------------------//
// Enable/Disable Robot Sound Effect
// Parameter: 
//          cmd: 1:enable,0:disable
//          type: robot type 0~2
//          thres: robot threshold 0~7
// return value:
//          NONE
//------------------------------------------------------------------// 
void SBC_CH2_SetRobot3Sound(U8 cmd,U8 type, U8 thres){
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

#if _SBC_CH2_REVERB_SOUND
//------------------------------------------------------------------//
// Enable/Disable Reverb Sound Effect
// Parameter: 
//          cmd: 1:enable,0:disable
//          index: Effect index
// return value:
//          NONE
//------------------------------------------------------------------// 
void SBC_CH2_SetReverbSound(U8 cmd,U8 index){
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
 
#if _SBC_CH2_ECHO_SOUND
//------------------------------------------------------------------//
// Enable/Disable Echo Sound Effect
// Parameter:
//          cmd: 1:enable,0:disable
//          index: echo type
// return value:
//          NONE
//------------------------------------------------------------------//
void SBC_CH2_SetEchoSound(U8 cmd,U8 index){

    if(cmd == ENABLE)
    {
#if _USE_HEAP_RAM
        if(EchoBuf==NULL)   EchoBuf = (U16 *)MemAlloc(SBC_ECHO_BUF_SIZE<<1);
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
void SBC_CH2_SetMixCtrl(U8 mixCtrl){
    if(mixCtrl>MIX_CTRL_100) mixCtrl=MIX_CTRL_100;
    SbcMixCtrl_CH2=mixCtrl;
}
#endif
//------------------------------------------------------------------//
// Codec init
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void SBC_CH2_Init(void){
    ChVol=CH_VOL_15; 
#if _AUDIO_MIX_CONTROL
    SBC_CH2_SetMixCtrl(MIX_CTRL_100);
#endif
#if _SBC_CH2_SPEED_CHANGE||_SBC_CH2_PITCH_CHANGE
    SpeedCtl.speed_ratio_index=RATIO_INIT_INDEX;
    SpeedCtl.pitch_ratio_index=RATIO_INIT_INDEX;
#endif        
#if defined(_SBC_PLAY_BACK_USE_CUSTOMIZE)
	SBC_CH2_SetEQFilter(FILTER_NUMBER);
#endif      
#if _SBC_CH2_ECHO_SOUND
    EchoCmd = DISABLE;
#endif
}
//------------------------------------------------------------------//
// Service loop
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
void SBC_CH2_ServiceLoop(void){
     
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
        
        
#if _SBC_CH2_SPEED_CHANGE||_SBC_CH2_PITCH_CHANGE
        if((AudioBufCtl.fifo_count+(SBC_CH2_AUDIO_BUF_SIZE-SBC_CH2_AUDIO_OUT_SIZE)-AudioBufCtl.process_count)>=0){
            //dprintf("%d,%d,%d\r\n",AudioBufCtl.fifo_count,AudioBufCtl.process_count,AudioBufCtl.toggle);
            //if(AudioBufCtl.process_count<AudioBufCtl.samples){
                DecodeFrame();
            //}
        }
#else      
        //if((AudioBufCtl.fifo_count+(SBC_DEC_OUT_SIZE*AUDIO_BUF_COUNT)-AudioBufCtl.process_count)>=SBC_DEC_OUT_SIZE){
        if((AudioBufCtl.fifo_count+(SBC_CH2_AUDIO_BUF_SIZE-SBC_CH2_AUDIO_OUT_SIZE)-AudioBufCtl.process_count)>=0){
            //dprintf("%d,%d,%d\r\n",AudioBufCtl.fifo_count,AudioBufCtl.process_count,AudioBufCtl.toggle);
            if(AudioBufCtl.process_count<AudioBufCtl.samples){
                DecodeFrame();
            }
        }
#endif            
    }        
#if _USE_HEAP_RAM                            
    if((AudioBufCtl.shadow_state==STATUS_STOP) && SBCRam!=NULL )        
    {
        SBC_FreeMemory(MEMORY_ALLOCATE_PLAY);
    }
#endif
    if(     AudioBufCtl.shadow_state!=STATUS_PLAYING
        &&  DacAssigner_codecUseDacCh(kSBC_CH2)>=0
    ){
        GIE_DISABLE();
		DacAssigner_unRegist(kSBC_CH2);
		GIE_ENABLE();
		//dprintf("Free codec %d hw_ch\r\n",kSBC_CH2);
    }
	if (PlayEndFlag == BIT_SET) {
		PlayEndFlag = BIT_UNSET;
		CB_SBC_CH2_PlayEnd();
	}
}    

#if	_SBC_CH2_QIO_MODULE
#if _SBC_CH2_SPEED_CHANGE||_SBC_CH2_PITCH_CHANGE
#error	"QIO does not support _SBC_CH2_SPEED_CHANGE, _SBC_CH2_PITCH_CHANGE"
#endif
#endif//_SBC_CH2_QIO_MODULE

#endif//_SBC_CH2_PLAY

