/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "include.h"
#include "nx1_dac.h"

#if _CELP_PLAY

/* Defines -------------------------------------------------------------------*/

#define CELP_SAMPLE_RATE    8000
#define SMOOTH_ENABLE          1

S_AUDIO_FUNC_IDX s_CELP_FUNC = {
    .Play           =   CELP_Play,
    .Pause          =   CELP_Pause,
    .Resume         =   CELP_Resume,
    .Stop           =   CELP_Stop,
    .SetVolume      =   CELP_SetVolume,
    .GetStatus      =   CELP_GetStatus,
#if _CELP_SPEED_CHANGE
    .SetSpeed       =   CELP_SetSpeed,
#endif
#if _CELP_PITCH_CHANGE
    .SetPitch       =   CELP_SetPitch,
#endif
#if _CELP_ROBOT1_SOUND
    .SetRobot1Sound =   CELP_SetRobot1Sound,
#endif
#if _CELP_ROBOT2_SOUND
    .SetRobot2Sound =   CELP_SetRobot2Sound,
#endif
#if _CELP_ROBOT3_SOUND
    .SetRobot3Sound =   CELP_SetRobot3Sound,
#endif
#if _CELP_REVERB_SOUND
    .SetReverbSound =   CELP_SetReverbSound,
#endif
#if _CELP_ECHO_SOUND
    .SetEchoSound   =   CELP_SetEchoSound,
#endif
#if _AUDIO_MIX_CONTROL
    .SetMixCtrl     =   CELP_SetMixCtrl,
#endif
};
/* Constant Table ------------------------------------------------------------*/
#if _CELP_SPEED_CHANGE
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

#if _CELP_PITCH_CHANGE
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
#if _CELP_ROBOT1_SOUND 
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
#if _CELP_ECHO_SOUND
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
U8 CelpMixCtrl=MIX_CTRL_100;
#endif

/* Static Variables ----------------------------------------------------------*/
static volatile struct AUDIO_BUF_CTL{
    S32 process_count;	//decode/encode count
    S32 fifo_count;	    //decode/encode count
    U32 samples;	    //samples
#if _CELP_SPEED_CHANGE||_CELP_PITCH_CHANGE
    U32 sample_count;	    //samples
    U32 fifo_count2;	    //decode/encode count
#endif    
    //U16 vol;            //volume
    U16 sample_rate;    //sample_rate
    U8 in_size;         //data frame size
    U8 state;           //channel state
    U8 shadow_state;    //channel shadow state
//    U8 ramp;            //smooth ramp
    U8 index;
    U8 upsample;
}AudioBufCtl;

static U16 AudioVol;
static U8 ChVol;

static volatile struct{
    U16 *buf;
    U16 size;
    U16 offset;
}BufCtl;   

#if SMOOTH_ENABLE
static volatile struct{
    S16 count;              //sample count in process
    U8  step_sample;        //sample size for each step
    U8  smooth;             //up or down or none
    U8  state;              //current state
}SmoothCtl;
#endif

#if _CELP_SPEED_CHANGE||_CELP_PITCH_CHANGE
static struct{
    U16 index;
    U8 speed_ratio_index;
    U8 pitch_ratio_index;
}SpeedCtl;

#endif

#if _CELP_ROBOT1_SOUND 
static struct{
    U8 enable;
    U8 change;
    U8 index;
}RobotCtl;
#endif 

#if _CELP_ROBOT2_SOUND
static struct{
    U8 enable;
    U8 type;
    U16 thres;
    U16 detectthres;
}Robot01Ctl;
#endif 
#if _CELP_ROBOT3_SOUND
static struct{
    U8 enable;
    U8 type;
    U16 thres;
}Robot02Ctl;
#endif 

#if _CELP_REVERB_SOUND
static U8 ReverbCmd,ReverbEffect;
#endif

#if _CELP_ECHO_SOUND
static EchoCtl_T EchoCtl;
static S8 EchoCmd;
#endif
static U8  StorageMode;
static U32 CurAddr;

static U8  PlayEndFlag;
#if _AUDIO_VOLUME_CHANGE_SMOOTH
static volatile VOLCTRL_T    VolCtrl;
#endif


/* Dynamic Allocate RAM-------------------------------------------------------*/
#if _USE_HEAP_RAM
static U16 *CELPRam=NULL;
static U16 *AudioBuf=NULL;
static U8 *DataBuf=NULL;
#if _CELP_SPEED_CHANGE||_CELP_PITCH_CHANGE
static U16 *SpeedCtlBuf=NULL;
#endif

#if _CELP_ECHO_SOUND
static S16 *EchoBuf=NULL;
#endif

#if _CELP_ROBOT2_SOUND
static S16 *Robot01_buf;
#endif

#if _CELP_ROBOT3_SOUND
static S16 *Robot02_buf;
#endif

#if _CELP_REVERB_SOUND
static S16 *Reverb_x;
static S16 *Reverb_y;
#endif


/* Fix RAM--------------------------------------------------------------------*/
#else
static U16 CELPRam[CELP_RAM_ALLOCATE_SIZE];
static U16 AudioBuf[CELP_AUDIO_BUF_SIZE];
static U8  DataBuf[CELP_DEC_IN_SIZE];

#if _CELP_SPEED_CHANGE||_CELP_PITCH_CHANGE
#if _CELP_ROBOT3_SOUND
static U16 SpeedCtlBuf[320+SPEED_CTL_FRAME_SIZE]; // for complex robot
#else
static U16 SpeedCtlBuf[CELP_DEC_OUT_SIZE+SPEED_CTL_FRAME_SIZE]; 
#endif
#endif
#if _CELP_ECHO_SOUND
static S16 EchoBuf[CELP_ECHO_BUF_SIZE];
#endif
#if _CELP_ROBOT2_SOUND
static S16 Robot01_buf[CELP_ROBOT_BUF_SIZE];
#endif

#if _CELP_ROBOT3_SOUND
static S16 Robot02_buf[CELP_ROBOT02_BUF_SIZE];
#endif

#if _CELP_REVERB_SOUND
static S16 Reverb_x[CELP_REVERB_BUF_SIZE];
static S16 Reverb_y[CELP_REVERB_BUF_SIZE];
#endif


#endif



/* Extern Functions ----------------------------------------------------------*/
extern void CB_ReadData(U8 mode, U32 *cur_addr, U8 *buf,U16 size);
extern U8   CB_SetStartAddr(U32 addr,U8 mode,U8 rw,U32 *start_addr);
extern void CB_CELP_PlayStart(void);
extern void CB_CELP_PlayEnd(void);
extern void CB_CELP_InitDac(CH_TypeDef *chx,U16 sample_rate,U8 upsample);
extern void CB_CELP_DacCmd(CH_TypeDef *chx,U8 cmd);
extern void CB_CELP_ChangeSR(CH_TypeDef *chx,U16 sample_rate);
extern void CB_CELP_WriteDac(U8 size,U16* psrc16);
//------------------------------------------------------------------//
// Allocate CELP memory
// Parameter: 
//          func: Choose Allocate function
// return value:
//          NONE
//------------------------------------------------------------------// 
#if _USE_HEAP_RAM
static void CELP_AllocateMemory(U16 func)
{
    if(CELPRam==NULL && (func&MEMORY_ALLOCATE_PLAY))    //CELPRam & AudioBuf & SpeedCtlBuf together
    {
        CELPRam   = (U16 *)MemAlloc(CELP_RAM_ALLOCATE_SIZE<<1);
        AudioBuf = (U16 *)MemAlloc(CELP_AUDIO_BUF_SIZE<<1);
        DataBuf  = (U8 *)MemAlloc(CELP_DEC_IN_SIZE);
#if _CELP_SPEED_CHANGE||_CELP_PITCH_CHANGE
#if _CELP_ROBOT3_SOUND
        SpeedCtlBuf = MemAlloc((320+SPEED_CTL_FRAME_SIZE)<<1);
#else
        SpeedCtlBuf = MemAlloc((CELP_DEC_OUT_SIZE+SPEED_CTL_FRAME_SIZE)<<1);
#endif
#endif
    }
    //else
    //    dprintf("Allocate twice. Ignore...\r\n");
}
//------------------------------------------------------------------//
// Free CELP memory
// Parameter: 
//          func: Choose Allocate function
// return value:
//          NONE
//------------------------------------------------------------------// 
static void CELP_FreeMemory(U16 func)
{
    if(CELPRam!=NULL && (func&MEMORY_ALLOCATE_PLAY))    
    {
        MemFree(CELPRam);
        CELPRam=NULL;
        MemFree(AudioBuf);
        AudioBuf=NULL;
        MemFree(DataBuf);
        DataBuf=NULL;
#if _CELP_SPEED_CHANGE||_CELP_PITCH_CHANGE
        MemFree(SpeedCtlBuf);
        SpeedCtlBuf=NULL;
#endif

#if _CELP_ROBOT2_SOUND    
        if(Robot01_buf!=NULL)
        {
            MemFree(Robot01_buf);
            Robot01_buf = NULL;
        }
#endif
#if _CELP_ROBOT3_SOUND    
        if(Robot02_buf!=NULL)
        {
            MemFree(Robot02_buf);
            Robot02_buf = NULL;
        }
#endif
#if _CELP_REVERB_SOUND
        if(Reverb_x!=NULL)
        {
            MemFree(Reverb_x);
            Reverb_x = NULL;
            MemFree(Reverb_y);
            Reverb_y = NULL;
        }
#endif
#if _CELP_ECHO_SOUND
        if(EchoBuf!=NULL)
        {
		MemFree(EchoBuf);
		EchoBuf=NULL;
        }
#endif
        //dprintf("Free Mem\r\n");
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
void CELP_DacIsr(U8 size){
    U16 temp;
    S16 *psrc16,gain;
    U8 i,len,size2;  
      
    if(AudioBufCtl.shadow_state==STATUS_PLAYING){
        
        temp=BufCtl.size-BufCtl.offset;
        if(size>temp){
            size=temp;
        }   
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
        CB_CELP_WriteDac(size,(U16 *)psrc16);
        
        BufCtl.offset+=size;

        if(BufCtl.offset==BufCtl.size){
            BufCtl.offset=0;    
        }            
#if _CELP_SPEED_CHANGE||_CELP_PITCH_CHANGE
        if(AudioBufCtl.fifo_count>=AudioBufCtl.fifo_count2){
            AudioBufCtl.state=STATUS_STOP;
            AudioBufCtl.shadow_state=AudioBufCtl.state;
            PlayEndFlag = BIT_SET;
            //dprintf("stop c,%d,%d\r\n",AudioBufCtl.fifo_count,AudioBufCtl.fifo_count2);
        }
#else
        if(AudioBufCtl.fifo_count>=AudioBufCtl.samples){
            AudioBufCtl.state=STATUS_STOP;
            AudioBufCtl.shadow_state=AudioBufCtl.state;
            PlayEndFlag = BIT_SET;
            //dprintf("stop c,%d,%d\r\n",AudioBufCtl.fifo_count,AudioBufCtl.samples);
        }
#endif        
    }else{
//        for(i=0;i<size;i++){
//            *pdest32=0;           
//        }    
        CB_CELP_WriteDac(size,(U16 *)0);       
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
	    SmoothCtl.step_sample=CELP_SAMPLE_RATE*SMOOTH_UP_TIME/1000/SMOOTH_STEP;
    }else if(smooth==SMOOTH_DOWN){
	    SmoothCtl.smooth=SMOOTH_DOWN;
	    SmoothCtl.step_sample=CELP_SAMPLE_RATE*SMOOTH_DOWN_TIME/1000/SMOOTH_STEP;
    }else{
        SmoothCtl.smooth=SMOOTH_NONE;
    }
}     
#endif
#if _CELP_SPEED_CHANGE||_CELP_PITCH_CHANGE
//------------------------------------------------------------------//
// Decode one audio frame
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
static void DecodeFrame(void)
{
    U8 bpf;
    U16 *audio_buf,i,size,j,out_cnt;
    S16 *pdest,gain;
    U32 speed_ctl_size; 
#if _CELP_ROBOT3_SOUND
	U16 *audio_buf2;
#endif    
    
    while(SpeedCtl.index<SPEED_CTL_FRAME_SIZE){
        
        if(AudioBufCtl.sample_count<AudioBufCtl.samples){
            audio_buf=SpeedCtlBuf+SpeedCtl.index;
#if _CELP_ROBOT3_SOUND
            audio_buf2= audio_buf+CELP_DEC_OUT_SIZE;
#endif
			
            CB_ReadData(StorageMode,&CurAddr,(U8 *)DataBuf,AudioBufCtl.in_size);
            CELPDecode(CELP_DEC_OUT_SIZE,DataBuf,(void *)audio_buf);   
#if _CELP_ROBOT3_SOUND
            CB_ReadData(StorageMode,&CurAddr,(U8 *)DataBuf,AudioBufCtl.in_size);
            CELPDecode(CELP_DEC_OUT_SIZE,DataBuf,(void *)audio_buf2);  
#endif
            
#if _CELP_ROBOT2_SOUND
            if(Robot01Ctl.enable)
            {
#if _ROBOT_SOUND_COMPRESS                    
                ROBOT_EFFECT_COMPRESS_01((S16 *)audio_buf);
#else
                ROBOT_EFFECT_01((S16 *)audio_buf);
#endif
            }
#endif

#if _CELP_ROBOT3_SOUND
            if(Robot02Ctl.enable)
            {
#if _ROBOT_SOUND_COMPRESS
                ROBOT_EFFECT_COMPRESS_02((S16 *)audio_buf);
#else
                ROBOT_EFFECT_02((S16 *)audio_buf);
#endif

            }
#endif      


#if _CELP_REVERB_SOUND
            if(ReverbCmd)
            {
                Reverb_Effect((short *)audio_buf,CELP_SOUND_EFFECT_SIZE);
            }
#endif                             


#if _CELP_ECHO_SOUND
            if(EchoCmd)
            {
                EchoVoiceProcess(audio_buf, CELP_SOUND_EFFECT_SIZE, &EchoCtl);
            }
#endif
           
            SpeedCtl.index+=CELP_SOUND_EFFECT_SIZE;
            AudioBufCtl.sample_count+=CELP_SOUND_EFFECT_SIZE;
            AudioBufCtl.fifo_count2=AudioBufCtl.fifo_count+(CELP_AUDIO_BUF_SIZE*2);
        }else{
         
            audio_buf=SpeedCtlBuf+SpeedCtl.index;
            
            memset((void *)audio_buf, 0, CELP_SOUND_EFFECT_SIZE*2);
            
            SpeedCtl.index+=CELP_SOUND_EFFECT_SIZE;
        }    
    }    
    
    audio_buf=AudioBuf+(CELP_AUDIO_OUT_SIZE*AudioBufCtl.index);
    
    AudioBufCtl.index++;
    if(AudioBufCtl.index==CELP_AUDIO_OUT_COUNT){
        AudioBufCtl.index=0;    
    }    
    
    if(SpeedCtl.speed_ratio_index==RATIO_INIT_INDEX && SpeedCtl.pitch_ratio_index==RATIO_INIT_INDEX){
        speed_ctl_size=CELP_AUDIO_OUT_SIZE;
        for(i=0;i<CELP_AUDIO_OUT_SIZE;i++){
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
    AudioBufCtl.process_count+=CELP_AUDIO_OUT_SIZE;
    
#if _CELP_ROBOT1_SOUND
    if(RobotCtl.enable){
        if(RobotCtl.change){
            RoboticVoiceInit(CELP_SAMPLE_RATE, CarrierTbl[RobotCtl.index], CELP_AUDIO_OUT_SIZE);
            //dprintf("RobotCtl:%d\r\n",RobotCtl.carrier);
            RobotCtl.change=0;
        }    
        RoboticVoiceProcess((short *)audio_buf,(short *)audio_buf);
    }
#endif  

    //volume 
    for(i=0;i<CELP_AUDIO_OUT_SIZE;i++){
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
    U16 i;
    U16 *audio_buf;
#if _CELP_ROBOT3_SOUND	
    U16 *audio_buf2;
#endif
     
    audio_buf=AudioBuf+(CELP_AUDIO_OUT_SIZE*AudioBufCtl.index);
#if _CELP_ROBOT3_SOUND	
    audio_buf2 = audio_buf+CELP_DEC_OUT_SIZE;
#endif
    
    AudioBufCtl.index++;
    if(AudioBufCtl.index==CELP_AUDIO_OUT_COUNT){
        AudioBufCtl.index=0;    
    }    
    

    CB_ReadData(StorageMode,&CurAddr,(U8 *)DataBuf,AudioBufCtl.in_size);
    CELPDecode(CELP_DEC_OUT_SIZE,DataBuf,(void *)audio_buf);
#if _CELP_ROBOT3_SOUND
    CB_ReadData(StorageMode,&CurAddr,(U8 *)DataBuf,AudioBufCtl.in_size);
    CELPDecode(CELP_DEC_OUT_SIZE,DataBuf,(void *)audio_buf2);  
    //dprintf("f %d p %d d %d\r\n",AudioBufCtl.fifo_count,AudioBufCtl.process_count,AudioBufCtl.process_count-AudioBufCtl.fifo_count);
#endif
    
    
#if _CELP_ROBOT1_SOUND  
    if(RobotCtl.enable){
        if(RobotCtl.change){
            RoboticVoiceInit(CELP_SAMPLE_RATE, CarrierTbl[RobotCtl.index], CELP_AUDIO_OUT_SIZE);
            //dprintf("RobotCtl:%d\r\n",RobotCtl.carrier);
            RobotCtl.change=0;
        }    
        RoboticVoiceProcess((short *)audio_buf,(short *)audio_buf);
    }
#endif    

#if _CELP_ROBOT2_SOUND
    if(Robot01Ctl.enable)
    {
#if _ROBOT_SOUND_COMPRESS                    
        ROBOT_EFFECT_COMPRESS_01((S16 *)audio_buf);
#else
        ROBOT_EFFECT_01((S16 *)audio_buf);
#endif
    }
#endif

#if _CELP_ROBOT3_SOUND
    if(Robot02Ctl.enable)
    {
#if _ROBOT_SOUND_COMPRESS
        ROBOT_EFFECT_COMPRESS_02((S16 *)audio_buf);
#else
        ROBOT_EFFECT_02((S16 *)audio_buf);
#endif

    }
#endif      


#if _CELP_REVERB_SOUND
    if(ReverbCmd)
    {
        Reverb_Effect((short *)audio_buf,CELP_AUDIO_OUT_SIZE);
    }
#endif                             

#if _CELP_ECHO_SOUND
    if(EchoCmd)
    {
        EchoVoiceProcess(audio_buf, CELP_AUDIO_OUT_SIZE, &EchoCtl);
    }
#endif
    
    AudioBufCtl.process_count+=CELP_AUDIO_OUT_SIZE;
    //volume 
    for(i=0;i<CELP_AUDIO_OUT_SIZE;i++){
#if _AUDIO_VOLUME_CHANGE_SMOOTH
        audio_buf[i]=((S16)audio_buf[i]*AUDIO_GetVolumeSmooth(&VolCtrl))>>Q15_SHIFT;
#else  
        audio_buf[i]=((S16)audio_buf[i]*AudioVol)>>Q15_SHIFT;
#endif
    }   
}    
#endif
//------------------------------------------------------------------//
// Start play voice
// Parameter: 
//          index: file index or absolute address
//          mode:OTP_MODE,EF_MODE,SPI_MODE,SPI1_MODE
// return value:
//          0:ok
//          others:fail
//------------------------------------------------------------------// 
U8 CELP_Play(U32 index,U8 mode){
    struct AUDIO_HEADER *pheader;
    
    if(AudioBufCtl.shadow_state==STATUS_PLAYING){
#if SMOOTH_ENABLE        
            AudioBufCtl.state=STATUS_STOP;
            SmoothFunc(SMOOTH_DOWN);
            if(!DacMixerRamp_IsBias()) {
                AudioBufCtl.shadow_state=STATUS_STOP;
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
    CELP_AllocateMemory(MEMORY_ALLOCATE_PLAY);
#else
	AUDIO_ServiceLoop();
#endif    
#if _CELP_ECHO_SOUND
    if(EchoCmd)
    {
#if _USE_HEAP_RAM
        if(EchoBuf==NULL)
            EchoBuf = (U16 *)MemAlloc(CELP_ECHO_BUF_SIZE<<1);
#endif
        memset((void *)EchoBuf, 0, CELP_ECHO_BUF_SIZE<<1);
    }
#endif        
    memset((void *)&AudioBufCtl, 0, sizeof(AudioBufCtl));
    
#if _CELP_SPEED_CHANGE||_CELP_PITCH_CHANGE
    AudioBufCtl.fifo_count2=-1;
#endif    
//    AudioBufCtl.ramp=ramp;
    
//    SmoothCtl.p_parameter=0;
    
    BufCtl.offset=0;
    BufCtl.size=CELP_AUDIO_BUF_SIZE;//sizeof(AudioBuf)/2;
    BufCtl.buf=AudioBuf;
    
    if(CB_SetStartAddr(index,mode,0,&CurAddr)){
		return E_AUDIO_INVALID_IDX;
	}
    StorageMode=mode;
    
    CB_ReadData(StorageMode,&CurAddr,(U8 *)AudioBuf,HEADER_LEN);
    
    pheader=(struct AUDIO_HEADER *)AudioBuf;
    
    if(pheader->codec!=CODEC_CELP){  
        return E_AUDIO_UNSUPPORT_CODEC;    
    }     
    
	GIE_DISABLE();
#if _CELP_PITCH_CHANGE
	S8 hw_ch = DacAssigner_Regist(kCELP,CELP_EXCLUSIVE);
#else
	S8 hw_ch = DacAssigner_Regist(kCELP,pheader->sample_rate);
#endif
	GIE_ENABLE();
    if(hw_ch<0){
        //dprintf("No play Invalid sr\r\n");
        return E_AUDIO_INVALID_SAMPLERATE;
    }
	//dprintf("codec %d reg hw_ch %d\r\n",kCELP,hw_ch);

//    dprintf("%x,%x,%x\r\n",pheader->signature[0],pheader->signature[1],pheader->signature[2]);
//    
    AudioBufCtl.samples=pheader->samples;
    AudioBufCtl.in_size=pheader->decode_in_size;
    AudioBufCtl.sample_rate=pheader->sample_rate;
    AudioBufCtl.upsample=pheader->flag&0x1;
    
//    dprintf("pheader->decode_in_size,%d\r\n",pheader->decode_in_size);
//    dprintf("pheader->decode_out_size,%d\r\n",pheader->decode_out_size);
//    dprintf("pheader->sample_rate,%d\r\n",pheader->sample_rate);

#if SMOOTH_ENABLE    
//    if(ramp&RAMP_UP){
        SmoothFunc(SMOOTH_UP);
//    }else{
//        SmoothFunc(SMOOTH_NONE);
//    }  
#endif    
    CELPDecInit(CELPRam);

	CB_CELP_InitDac(pHwCtl[hw_ch].p_dac,AudioBufCtl.sample_rate,AudioBufCtl.upsample);
 
#if _CELP_SPEED_CHANGE
    SpeedCtl.index=0;
#if _CELP_PITCH_CHANGE        
    TimeStretchInit(1,SpeedRatioTbl[SpeedCtl.speed_ratio_index]/PitchRatioTbl[SpeedCtl.pitch_ratio_index]);
#else
    TimeStretchInit(1,SpeedRatioTbl[SpeedCtl.speed_ratio_index]);
#endif    
#endif
#if _CELP_PITCH_CHANGE
    SpeedCtl.index=0;
#if _CELP_SPEED_CHANGE
    TimeStretchInit(0,SpeedRatioTbl[SpeedCtl.speed_ratio_index]/PitchRatioTbl[SpeedCtl.pitch_ratio_index]);
#else
    TimeStretchInit(0,1/PitchRatioTbl[SpeedCtl.pitch_ratio_index]);
#endif
	CB_CELP_ChangeSR(pHwCtl[hw_ch].p_dac,pheader->sample_rate*PitchRatioTbl[SpeedCtl.pitch_ratio_index]);
#endif           

#if _CELP_ROBOT2_SOUND
    if(Robot01Ctl.enable)
    {
#if _USE_HEAP_RAM
        if(Robot01_buf==NULL)   Robot01_buf = (S16 *) MemAlloc(CELP_ROBOT_BUF_SIZE<<1);
#endif
        memset((void *)Robot01_buf, 0, CELP_ROBOT_BUF_SIZE*2);
        ROBOT_EFFECT_01_Init(Robot01_buf,Robot01Ctl.type,Robot01Ctl.thres,Robot01Ctl.detectthres,CELP_SAMPLE_RATE,CELP_SOUND_EFFECT_SIZE,40,1600);
    }
#endif
#if _CELP_ROBOT3_SOUND
    //SpeedCtl.rb_ptr=0;
    //SpeedCtl.index=0;
    if(Robot02Ctl.enable)
    {
#if _USE_HEAP_RAM
        if(Robot02_buf==NULL)   Robot02_buf = (S16 *) MemAlloc(CELP_ROBOT02_BUF_SIZE<<1);
#endif        
        memset((void *)Robot02_buf, 0, CELP_ROBOT02_BUF_SIZE*2);
        ROBOT_EFFECT_02_Init(Robot02_buf,Robot02Ctl.type,Robot02Ctl.thres,CELP_SAMPLE_RATE);
    }
#endif

#if _CELP_REVERB_SOUND
    if(ReverbCmd)
    {
#if _USE_HEAP_RAM
        if(Reverb_x==NULL)
        {
            Reverb_x = (S16 *) MemAlloc(CELP_REVERB_BUF_SIZE<<1);
            Reverb_y = (S16 *) MemAlloc(CELP_REVERB_BUF_SIZE<<1);
        }
#endif        
        Reverb_Effect_Init(Reverb_x,Reverb_y,ReverbEffect);
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
    AUDIO_InitVolumeSmooth(&VolCtrl,CELP_SAMPLE_RATE,AudioVol);
#endif


    memset((void *)AudioBuf, 0, CELP_AUDIO_BUF_SIZE<<1);
    
    DecodeFrame();
    
    CB_CELP_PlayStart();   
    
    AudioBufCtl.state=  STATUS_PLAYING;
    AudioBufCtl.shadow_state=AudioBufCtl.state;
    
	CB_CELP_DacCmd(pHwCtl[hw_ch].p_dac,ENABLE);
    
    
    return 0;
}   
//------------------------------------------------------------------//
// Pause playing
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void CELP_Pause(void){
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
void CELP_Resume(void){
	if(AudioBufCtl.state==STATUS_PAUSE){
        if(DacAssigner_codecUseDacCh(kCELP)<0) {
            GIE_DISABLE();
#if	_CELP_PITCH_CHANGE
            S8 hw_ch = DacAssigner_Regist(kCELP,CELP_EXCLUSIVE);
#else
            S8 hw_ch = DacAssigner_Regist(kCELP,AudioBufCtl.sample_rate);
#endif
            GIE_ENABLE();
            if(hw_ch<0) return;//error
        
            CB_CELP_InitDac(pHwCtl[hw_ch].p_dac,AudioBufCtl.sample_rate,AudioBufCtl.upsample);
#if	_CELP_PITCH_CHANGE
            CELP_SetPitch(SpeedCtl.pitch_ratio_index-RATIO_INIT_INDEX);
#endif
            CB_CELP_DacCmd(pHwCtl[hw_ch].p_dac,ENABLE);
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
void CELP_Stop(void){
    if(AudioBufCtl.state==STATUS_PLAYING||AudioBufCtl.state==STATUS_PAUSE){ 

#if SMOOTH_ENABLE        
//        if(AudioBufCtl.ramp&RAMP_DOWN){
            AudioBufCtl.state=STATUS_STOP;
            SmoothFunc(SMOOTH_DOWN);
			if (AudioBufCtl.shadow_state == STATUS_PAUSE
                || !DacMixerRamp_IsBias()
            ) {
				AudioBufCtl.shadow_state = STATUS_STOP;
			}
//        }else
#endif        
//        {
//            AudioBufCtl.state=STATUS_STOP;
//            CB_CELP_PlayEnd();
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
U8 CELP_GetStatus(void){
    return AudioBufCtl.shadow_state;    
}
//------------------------------------------------------------------//
// Set audio volume
// Parameter: 
//          vol:0~15
// return value:
//          NONE
//------------------------------------------------------------------// 
void CELP_SetVolume(U8 vol){   
    if(vol>CH_VOL_15){
        vol=CH_VOL_15;
    }    
    ChVol=vol; 
}
//------------------------------------------------------------------//
// Set speed ratio
// Parameter: 
//          index:-12~12
// return value:
//          NONE
//------------------------------------------------------------------// 
void CELP_SetSpeed(S8 index){   
    
#if _CELP_SPEED_CHANGE
    index+=RATIO_INIT_INDEX;
    SpeedCtl.speed_ratio_index=index;
#if _CELP_PITCH_CHANGE
    TimeStretchInit(0,SpeedRatioTbl[index]/PitchRatioTbl[SpeedCtl.pitch_ratio_index]);
#else
    TimeStretchInit(0,SpeedRatioTbl[index]);
#endif
    
#endif     
}
//------------------------------------------------------------------//
// Set pitch ratio
// Parameter: 
//          index:-12~12
// return value:
//          NONE
//------------------------------------------------------------------// 
void CELP_SetPitch(S8 index){   
    
#if _CELP_PITCH_CHANGE
    index+=RATIO_INIT_INDEX;
    SpeedCtl.pitch_ratio_index=index;
#if _CELP_SPEED_CHANGE   
    TimeStretchInit(0,SpeedRatioTbl[SpeedCtl.speed_ratio_index]/PitchRatioTbl[index]);
#else
    TimeStretchInit(0,1/PitchRatioTbl[index]);
#endif
    
	S8 hw_ch = DacAssigner_codecUseDacCh(kCELP);
	if(hw_ch<0)return;//No register hw_ch
	CB_CELP_ChangeSR(pHwCtl[hw_ch].p_dac,CELP_SAMPLE_RATE*PitchRatioTbl[index]);  
#endif     
}
//------------------------------------------------------------------//
// Enable/Disable Robot Sound Effect
// Parameter: 
//          cmd: 1:enable,0:disable
//          index:carrier index,0~15
// return value:
//          NONE
//------------------------------------------------------------------// 
void CELP_SetRobot1Sound(U8 cmd,U8 index){
#if _CELP_ROBOT1_SOUND   
   
    RobotCtl.enable=cmd;
    
    if(RobotCtl.enable){
        RobotCtl.change=1;
        RobotCtl.index=index;
    }    
#endif    
}   
#if _CELP_ROBOT2_SOUND 
//------------------------------------------------------------------//
// Enable/Disable Robot Sound Effect
// Parameter: 
//          cmd: 1:enable,0:disable
//          type: robot type 0~2
//          thres: robot threshold 0~7
// return value:
//          NONE
//------------------------------------------------------------------// 
void CELP_SetRobot2Sound(U8 cmd,U8 type, U8 thres){
    Robot01Ctl.enable=cmd;
    
    if(Robot01Ctl.enable)
    {
        Robot01Ctl.type=type;
        Robot01Ctl.thres= Robot_Thres[thres]; 
#if _CELP_ROBOT3_SOUND        
        Robot01Ctl.detectthres = 100;
#else
        Robot01Ctl.detectthres = 53;
#endif
#if _USE_HEAP_RAM
        if(Robot01_buf==NULL)
        {
            Robot01_buf = (S16 *)MemAlloc(CELP_ROBOT_BUF_SIZE<<1);
        }
#endif         
        memset((void *)Robot01_buf, 0, CELP_ROBOT_BUF_SIZE*2);
        ROBOT_EFFECT_01_Init(Robot01_buf,Robot01Ctl.type,Robot01Ctl.thres,Robot01Ctl.detectthres,CELP_SAMPLE_RATE,CELP_SOUND_EFFECT_SIZE,40,1600);
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
#if _CELP_ROBOT3_SOUND 
//------------------------------------------------------------------//
// Enable/Disable Robot Sound Effect
// Parameter: 
//          cmd: 1:enable,0:disable
//          type: robot type 0~2
//          thres: robot threshold 0~7
// return value:
//          NONE
//------------------------------------------------------------------// 
void CELP_SetRobot3Sound(U8 cmd,U8 type, U8 thres){
    Robot02Ctl.enable=cmd;
    
    if(Robot02Ctl.enable)
    {
        Robot02Ctl.type=type;
        Robot02Ctl.thres= Robot02_Thres[thres];     
#if _USE_HEAP_RAM
        if(Robot02_buf==NULL)
        {
            Robot02_buf = (S16 *)MemAlloc(CELP_ROBOT02_BUF_SIZE<<1);
        }
#endif        
        memset((void *)Robot02_buf, 0, CELP_ROBOT02_BUF_SIZE*2);
        ROBOT_EFFECT_02_Init(Robot02_buf,Robot02Ctl.type,Robot02Ctl.thres,CELP_SAMPLE_RATE);
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
#if _CELP_REVERB_SOUND
//------------------------------------------------------------------//
// Enable/Disable Reverb Sound Effect
// Parameter: 
//          cmd: 1:enable,0:disable
//          index: Effect index,0~7
// return value:
//          NONE
//------------------------------------------------------------------// 
void CELP_SetReverbSound(U8 cmd,U8 index){
    ReverbCmd = cmd;
    if(cmd)
    {
#if _USE_HEAP_RAM
        if(Reverb_x==NULL)
        {
            Reverb_x = (S16 *)MemAlloc(CELP_REVERB_BUF_SIZE<<1);
            Reverb_y = (S16 *)MemAlloc(CELP_REVERB_BUF_SIZE<<1);
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
#if _CELP_ECHO_SOUND
//------------------------------------------------------------------//
// Enable/Disable Echo Sound Effect
// Parameter:
//          cmd: 1:enable,0:disable
//          index: echo type,0~7
// return value:
//          NONE
//------------------------------------------------------------------//
void CELP_SetEchoSound(U8 cmd,U8 index){

    if(cmd == ENABLE)
    {
#if _USE_HEAP_RAM
        if(EchoBuf==NULL)   EchoBuf = (U16 *)MemAlloc(CELP_ECHO_BUF_SIZE<<1);
#endif        
        EchoCtl.flag = 0;
        EchoCtl.count= 0;
        EchoCtl.frameCount = 0;
        EchoCtl.buf = EchoBuf;
        EchoCtl.EchoBufL = CELP_ECHO_DATA_LENGTH;
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
void CELP_SetMixCtrl(U8 mixCtrl){
    if(mixCtrl>MIX_CTRL_100) mixCtrl=MIX_CTRL_100;
    CelpMixCtrl=mixCtrl;
}
#endif
//------------------------------------------------------------------//
// Codec init
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void CELP_Init(void){
     ChVol=CH_VOL_15; 
#if _AUDIO_MIX_CONTROL
    CELP_SetMixCtrl(MIX_CTRL_100);
#endif
#if _CELP_SPEED_CHANGE||_CELP_PITCH_CHANGE
    SpeedCtl.speed_ratio_index=RATIO_INIT_INDEX;
    SpeedCtl.pitch_ratio_index=RATIO_INIT_INDEX;
#endif        
#if _CELP_ECHO_SOUND
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
void CELP_ServiceLoop(void){
    
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
        
        if((AudioBufCtl.fifo_count+(CELP_AUDIO_BUF_SIZE-CELP_AUDIO_OUT_SIZE)-AudioBufCtl.process_count)>=0){
            
            DecodeFrame();
        }    
    }
#if _USE_HEAP_RAM                            
    if((AudioBufCtl.shadow_state==STATUS_STOP) && CELPRam!=NULL )        
    {
        //dprintf("shadow_state %x state %x CELPRam %x\r\n",AudioBufCtl.shadow_state,AudioBufCtl.state,CELPRam);
        CELP_FreeMemory(MEMORY_ALLOCATE_PLAY);
    }
#endif
    if(     AudioBufCtl.shadow_state!=STATUS_PLAYING
        &&  DacAssigner_codecUseDacCh(kCELP)>=0
    ){
		GIE_DISABLE();
		DacAssigner_unRegist(kCELP);
		GIE_ENABLE();
		//dprintf("Free codec %d hw_ch\r\n",kCELP);
    }
	if (PlayEndFlag == BIT_SET) {
		PlayEndFlag = BIT_UNSET;
		CB_CELP_PlayEnd();
	}
}    
#endif
