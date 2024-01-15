/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "include.h"
#include "nx1_dac.h"
#include "nx1_spi.h"
//#include "nx1_gpio.h"
#include "nx1_lib.h"
//#include "debug.h"

#if ((!_SBC2_PLAY)&&_SBC2_CH2_PLAY)
#error "_SBC2_CH2_PLAY used only for mixer play with _SBC2_PLAY. Please ENABLE _SBC2_PLAY." 
#endif

#if _SBC2_PLAY||_SBC2_CH2_PLAY

/* Defines -------------------------------------------------------------------*/

#define SMOOTH_ENABLE          1
#if		(_SBC2_PLAY==ENABLE && _SBC2_PLAY_BACK_EQ==ENABLE) \
        || (_SBC2_CH2_PLAY==ENABLE && _SBC2_CH2_PLAY_BACK_EQ==ENABLE)
	#include "FilterTab.h"
	#if	FILTER_NUMBER<=0//check table size
		#error "Filter table is not existed, please update NYIDE and create filter file."
	#endif
    #if		(_SBC2_PLAY==ENABLE && _SBC2_PLAY_BACK_EQ==ENABLE)
        #define _SBC2_PLAY_BACK_USE_CUSTOMIZE//define EQ customize
    #endif
    #if		(_SBC2_CH2_PLAY==ENABLE && _SBC2_CH2_PLAY_BACK_EQ==ENABLE)
        #define _SBC2_PLAY_BACK_USE_CUSTOMIZE_CH2//define EQ customize
    #endif
#endif
#if _SBC2_PLAY==ENABLE
S_AUDIO_FUNC_IDX s_SBC2_FUNC = {
    .Play           =   SBC2_Play,
    .Pause          =   SBC2_Pause,
    .Resume         =   SBC2_Resume,
    .Stop           =   SBC2_Stop,
    .SetVolume      =   SBC2_SetVolume,
    .GetStatus      =   SBC2_GetStatus,
#if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE)
    .SetEQFilter    =   SBC2_SetEQFilter,
#endif
#if _AUDIO_MIX_CONTROL
    .SetMixCtrl     =   SBC2_SetMixCtrl,
#endif
};
#endif
#if _SBC2_CH2_PLAY==ENABLE
S_AUDIO_FUNC_IDX s_SBC2_CH2_FUNC = {
    .Play           =   SBC2_CH2_Play,
    .Pause          =   SBC2_CH2_Pause,
    .Resume         =   SBC2_CH2_Resume,
    .Stop           =   SBC2_CH2_Stop,
    .SetVolume      =   SBC2_CH2_SetVolume,
    .GetStatus      =   SBC2_CH2_GetStatus,
#if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE_CH2)
    .SetEQFilter    =   SBC2_CH2_SetEQFilter,
#endif
#if _AUDIO_MIX_CONTROL
    .SetMixCtrl     =   SBC2_CH2_SetMixCtrl,
#endif
};
#endif

/* Global Variables ----------------------------------------------------------*/
#if _SBC2_PLAY
#if _AUDIO_MIX_CONTROL
U8 Sbc2MixCtrl=MIX_CTRL_100;
#endif
#endif
#if _SBC2_CH2_PLAY
#if _AUDIO_MIX_CONTROL
U8 Sbc2MixCtrl_CH2=MIX_CTRL_100;
#endif
#endif

/* Static Variables ----------------------------------------------------------*/
#if	_SBC2_PLAY
static volatile struct AUDIO_BUF_CTL{
    S32 process_count;	//decode/encode count
    S32 fifo_count;	    //decode/encode count
    U32 samples;	    //samples
#if _SBC2_QIO_MODULE
    U32 sample_count;	    //samples
    //U32 fifo_count2;	    //decode/encode count
#endif//_SBC2_QIO_MODULE
    U16 sample_rate;    //sample_rate
#if _SBC2_AUDIO_BUF_X3	
	U16 preDecCount;
	U8  preDecFrame;
#endif
    U8 	in_size;         //data frame size
    U8 	state;           //channel state
    U8 	shadow_state;    //channel shadow state
    U8 	index;
    U8 	decode_state;
    U8 	out_count;
    U8 	buf_swap;
	U8  upsample;
}AudioBufCtl;
#endif

#if _SBC2_CH2_PLAY
static volatile struct AUDIO_BUF_CTL_CH2{
    S32 process_count;	//decode/encode count
    S32 fifo_count;	    //decode/encode count
    U32 samples;	    //samples
#if _SBC2_CH2_QIO_MODULE
    U32 sample_count;	    //samples
    //U32 fifo_count2;	    //decode/encode count
#endif//_SBC2_CH2_QIO_MODULE
    U16 sample_rate;    //sample_rate
    U8 	in_size;         //data frame size
    U8 	state;           //channel state
    U8 	shadow_state;    //channel shadow state
    U8 	index;
    U8 	decode_state;
    U8 	out_count;
    U8 	buf_swap;
	U8  upsample;
}AudioBufCtl_CH2;
#endif

#if _SBC2_PLAY
static U16 AudioVol;
static U8  OpMode,ChVol;
#endif

#if _SBC2_CH2_PLAY
static U16 AudioVol_CH2;
static U8  OpMode_CH2,ChVol_CH2;
#endif


#if _SBC2_PLAY
static volatile struct{
    U16 *buf;
    U16 size;
    U16 offset;
}BufCtl;  
#endif 

#if _SBC2_CH2_PLAY
static volatile struct{
    U16 *buf;
    U16 size;
    U16 offset;
}BufCtl_CH2;  
#endif 

static const U8 Signature[]="NX1";

#if SMOOTH_ENABLE

//#if _SBC2_PLAY
typedef struct Smooth_Ctl{
    S16 count;              //sample count in process
    U8  step_sample;        //sample size for each step
    U8  smooth;             //up or down or none
    U8  state;              //current state
}SmoothCtl;

#if _SBC2_PLAY
static volatile SmoothCtl    SmoothCtl_CH1;
#endif
#if _SBC2_CH2_PLAY
static volatile SmoothCtl    SmoothCtl_CH2;
#endif
#endif

#if _SBC2_PLAY
static U8  StorageMode;
static U32 StartAddr,CurAddr;
static U8  PlayEndFlag;
#endif

#if _SBC2_CH2_PLAY
static U8  StorageMode_CH2;
static U32 StartAddr_CH2,CurAddr_CH2;
static U8  PlayEndFlag_CH2;
#endif


#if _SBC2_AUDIO_BUF_X3
U8 SBC2_PlayRepeatFlag;
#endif

#if _AUDIO_VOLUME_CHANGE_SMOOTH
static volatile VOLCTRL_T    VolCtrl;
static volatile VOLCTRL_T    VolCtrl_CH2;
#endif

#if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE)
static EQCtl EQFilterCtl;
#endif
#if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE)
static U8	u8EQFilterIndex=0;
#endif
#if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE_CH2)
static EQCtl EQFilterCtl_CH2;
#endif
#if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE_CH2)
static U8	u8EQFilterIndex_CH2=0;
#endif
/* Dynamic Allocate RAM-------------------------------------------------------*/
#if _USE_HEAP_RAM

static U16 *SBC2TempBuf=NULL;

#if _SBC2_PLAY
static U16 *SBC2Ram=NULL;
static U16 *AudioBuf=NULL;
#if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE)
static S32 *EqBuf=NULL;
#endif
#endif
#if _SBC2_CH2_PLAY
static U16 *SBC2Ram_CH2=NULL;
static U16 *AudioBuf_CH2=NULL;
#if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE_CH2)
static S32 *EqBuf_CH2=NULL;
#endif
#endif
#else

#if	_SBC2_PLAY
static U16 SBC2Ram[SBC2_RAM_ALLOCATE_SIZE];		//SBC2_RAM_ALLOCATE_SIZE=172
static U16 AudioBuf[SBC2_AUDIO_BUF_SIZE];//640:1ch play ; 480:2ch mixer play 
#if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE)
static S32 EqBuf[SBC2_EQ_BUF_SIZE];
#endif
#endif
#if _SBC2_CH2_PLAY
static U16 SBC2Ram_CH2[SBC2_RAM_ALLOCATE_SIZE];		//SBC2_RAM_ALLOCATE_SIZE=172
static U16 AudioBuf_CH2[SBC2_CH2_AUDIO_BUF_SIZE];//480
#if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE_CH2)
static S32 EqBuf_CH2[SBC2_EQ_BUF_SIZE];
#endif
#endif

#if	_SBC2_PLAY||_SBC2_CH2_PLAY
static U16 SBC2TempBuf[320]; 
#endif

#endif

#if _SBC2_AUDIO_BUF_X3
void RepeatReSetting_SBC2(U8 fifoSize, U8 size);
#endif

/* Extern Functions ----------------------------------------------------------*/
extern void CB_ReadData(U8 mode, U32 *cur_addr, U8 *buf,U16 size);
extern void CB_Seek(U32 offset);
extern void CB_WriteHeader(U8 mode, U32 *cur_addr,U8 *buf,U16 size);
extern U8   CB_SetStartAddr(U32 addr,U8 mode,U8 rw,U32 *start_addr);
extern void CB_WriteData(U8 mode, U32 *cur_addr,U8 *buf,U16 size);
#if _SBC2_PLAY
extern void CB_SBC2_PlayStart(void);
extern void CB_SBC2_PlayEnd(void);
extern void CB_SBC2_InitDac(CH_TypeDef *chx,U16 sample_rate,U8 upsample);
extern void CB_SBC2_DacCmd(CH_TypeDef *chx,U8 cmd);
extern void CB_SBC2_ChangeSR(CH_TypeDef *chx,U16 sample_rate);
extern void CB_SBC2_InitAdc(U16 sample_rate);
extern void CB_SBC2_AdcCmd(U8 cmd);
extern void CB_SBC2_WriteDac(U8 size,U16* psrc16);
#endif
#if _SBC2_CH2_PLAY
extern void CB_SBC2_CH2_PlayStart(void);
extern void CB_SBC2_CH2_PlayEnd(void);
extern void CB_SBC2_CH2_InitDac(CH_TypeDef *chx,U16 sample_rate,U8 upsample);
extern void CB_SBC2_CH2_DacCmd(CH_TypeDef *chx,U8 cmd);
extern void CB_SBC2_CH2_WriteDac(U8 size,U16* psrc16);
#endif

//------------------------------------------------------------------//
// Allocate SBC-2 CH1 memory
// Parameter: 
//          func: Choose Allocate function
// return value:
//          NONE
//------------------------------------------------------------------// 
#if _USE_HEAP_RAM
#if _SBC2_PLAY
static void SBC2_AllocateMemory(U16 func)
{
    if(SBC2Ram==NULL && (func&MEMORY_ALLOCATE_PLAY))    //SBC2Ram & AudioBuf & SpeedCtlBuf together
    {
        SBC2Ram   = (U16 *)MemAlloc((SBC2_RAM_ALLOCATE_SIZE)<<1);		//SBC2_RAM_ALLOCATE_SIZE=492=>172
        AudioBuf = (U16 *)MemAlloc(SBC2_AUDIO_BUF_SIZE<<1);	
    }
    if(SBC2TempBuf==NULL && (func&MEMORY_ALLOCATE_PLAY))    //SBC2TempBuf
    {
        SBC2TempBuf = (U16 *)MemAlloc((SBC2_RAM_ALLOCATE_SIZE_01)<<1);		//320
    }
}
//------------------------------------------------------------------//
// Free SBC-2 CH1 memory
// Parameter: 
//          func: Choose Allocate function
// return value:
//          NONE
//------------------------------------------------------------------// 
static void SBC2_FreeMemory(U16 func)
{
    if(SBC2Ram!=NULL && (func&MEMORY_ALLOCATE_PLAY))    
    {
        MemFree(SBC2Ram);
        SBC2Ram=NULL;
        MemFree(AudioBuf);
        AudioBuf=NULL;
#if _SBC2_CH2_PLAY
        if(SBC2Ram_CH2==NULL) //check SBC CH2 is released.
        {
            MemFree(SBC2TempBuf);
            SBC2TempBuf=NULL;
        }
#else
		MemFree(SBC2TempBuf);
        SBC2TempBuf=NULL;
#endif   
#if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE)
        if(EqBuf!=NULL)
        {
            MemFree(EqBuf);
            EqBuf=NULL;
            EQFilterCtl.Buf=NULL;
        }
#endif
    }
}
#endif

//------------------------------------------------------------------//
// Allocate SBC-2 CH2 memory
// Parameter: 
//          func: Choose Allocate function
// return value:
//          NONE
//------------------------------------------------------------------// 
#if _SBC2_CH2_PLAY
static void SBC2_CH2_AllocateMemory(U16 func)
{
    if(SBC2Ram_CH2==NULL && (func&MEMORY_ALLOCATE_PLAY))    //SBC2Ram & AudioBuf & SpeedCtlBuf together
    {
        SBC2Ram_CH2   = (U16 *)MemAlloc(SBC2_CH2_RAM_ALLOCATE_SIZE<<1);		//SBC2_RAM_ALLOCATE_SIZE=492=>172
        AudioBuf_CH2 = (U16 *)MemAlloc(SBC2_CH2_AUDIO_BUF_SIZE<<1);	
    }  
    if(SBC2TempBuf==NULL && (func&MEMORY_ALLOCATE_PLAY))    //SBC2TempBuf
    {
        SBC2TempBuf = (U16 *)MemAlloc((SBC2_RAM_ALLOCATE_SIZE_01)<<1);		//320
    }
}
//------------------------------------------------------------------//
// Free SBC-2 CH2 memory
// Parameter: 
//          func: Choose Allocate function
// return value:
//          NONE
//------------------------------------------------------------------// 
static void SBC2_CH2_FreeMemory(U16 func)
{
    if(SBC2Ram_CH2!=NULL && (func&MEMORY_ALLOCATE_PLAY))    
    {
        MemFree(SBC2Ram_CH2);
        SBC2Ram_CH2=NULL;
        MemFree(AudioBuf_CH2);
        AudioBuf_CH2=NULL;
#if _SBC2_PLAY
        if(SBC2Ram==NULL) //check SBC CH1 is released.
        {
            MemFree(SBC2TempBuf);
            SBC2TempBuf=NULL;
        }
#endif
#if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE_CH2)
        if(EqBuf_CH2!=NULL)
        {
            MemFree(EqBuf_CH2);
            EqBuf_CH2=NULL;
            EQFilterCtl_CH2.Buf=NULL;
        }
#endif
    }
}

#endif
#endif


#if _SBC2_PLAY
//------------------------------------------------------------------//
// ISR
// Parameter: 
//          size: fifo fill size 
// return value:
//          NONE
//------------------------------------------------------------------// 
void SBC2_DacIsr(U8 size){
    U16 temp;
#if	_SBC2_QIO_MODULE
	U16 _QioFrameFlag;
#endif//_SBC2_QIO_MODULE
    S16 *psrc16,gain;
    U8 i,len,size2;
#if _SBC2_AUDIO_BUF_X3
	U8 fifoSize = size;
	U8 lastFifoFlag = 0;
#endif
	if(AudioBufCtl.shadow_state==STATUS_PLAYING){
#if _SBC2_QIO_MODULE      
		QIO_Resume(QIO_USE_SBC2_CH1);
#endif//_SBC2_QIO_MODULE
         
        temp=BufCtl.size-BufCtl.offset;
        if(size>temp){
            size=temp;
        }   
#if _SBC2_QIO_MODULE
#if _SBC2_AUDIO_BUF_X3
		U32 _QioDiffSamples = AudioBufCtl.samples-AudioBufCtl.fifo_count-1;
		for(i=0;i<fifoSize;i++){
			QIO_FrameFlag_FIFO_BitGet(QIO_USE_SBC2_CH1, (U8*)&_QioFrameFlag);
			QIO_DacIsr(QIO_USE_SBC2_CH1, (U8)_QioFrameFlag);
			if(i == _QioDiffSamples){
				QIO_FrameFlag_FIFO_RstGet(QIO_USE_SBC2_CH1);
			}
		}
#else
		for(i=0;i<size;i++){
			QIO_FrameFlag_FIFO_BitGet(QIO_USE_SBC2_CH1, (U8*)&_QioFrameFlag);
			QIO_DacIsr(QIO_USE_SBC2_CH1, (U8)_QioFrameFlag);
		}
#endif
#endif//_SBC2_QIO_MODULE
	#if _SBC2_AUDIO_BUF_X3
		if((AudioBufCtl.fifo_count+size)>AudioBufCtl.samples){
//			dprintf("%d,%d\r\n",AudioBufCtl.fifo_count,AudioBufCtl.samples);
			size=AudioBufCtl.samples-AudioBufCtl.fifo_count;
//			dprintf("size=%d\r\n",size);
			lastFifoFlag = 1;
		}
	#endif
//        dprintf("size,%d\r\n",size);
        AudioBufCtl.fifo_count+=size;   
        
#if SMOOTH_ENABLE       
        //smooth process
        if(SmoothCtl_CH1.smooth!=SMOOTH_NONE){  
            size2= size;
            psrc16=(S16 *)(BufCtl.buf+BufCtl.offset);
        
            while(size2){
                if(SmoothCtl_CH1.state==SMOOTH_STEP){
                    if(SmoothCtl_CH1.smooth==SMOOTH_DOWN){
                        for(i=0;i<size2;i++){
                            *psrc16++=0;
                        }       
#if _SBC2_QIO_MODULE
						if(AudioBufCtl.state == STATUS_STOP)
						{
							QIO_DelayStop(QIO_USE_SBC2_CH1,ENABLE);
						}
						else
						{
							QIO_Pause(QIO_USE_SBC2_CH1);
						}
#endif//_SBC2_QIO_MODULE
                        AudioBufCtl.shadow_state=AudioBufCtl.state;
                        if(AudioBufCtl.state==STATUS_STOP){
							PlayEndFlag = BIT_SET;
                        }  
                    }
                    SmoothCtl_CH1.smooth=SMOOTH_NONE;
                    break;            
                }
                if(SmoothCtl_CH1.smooth==SMOOTH_DOWN){
                    gain=MAX_SMOOTH_GAIN-(SmoothCtl_CH1.state+1)*(MAX_SMOOTH_GAIN/SMOOTH_STEP);
                }else{
                    gain=(SmoothCtl_CH1.state+1)*(MAX_SMOOTH_GAIN/SMOOTH_STEP);
                }    
                len=SmoothCtl_CH1.step_sample-SmoothCtl_CH1.count;
            
                if(len>size2){
                    len=size2;
                }    
                 
                for(i=0;i<len;i++){
                    *psrc16=((S32)(*psrc16)*gain)>>15;
                    psrc16++;
                }         
                SmoothCtl_CH1.count+=len;
                size2-=len;
                if(SmoothCtl_CH1.count==SmoothCtl_CH1.step_sample){
                    SmoothCtl_CH1.count=0;
                    SmoothCtl_CH1.state++;   
                    //dprintf("s:%d,%d\r\n",SmoothCtl_CH1.state,gain);
                }          
            }  
        }
#endif


        psrc16=(S16 *)(BufCtl.buf+BufCtl.offset);
//		dprintf("%d\r\n",BufCtl.offset);
 
        CB_SBC2_WriteDac(size,(U16 *)psrc16);
        
        BufCtl.offset+=size;
//#if(_SBC2_PLAY&&_SBC2_CH2_PLAY)        
//        if(((BufCtl.offset)%160==0)&&(BufCtl.offset>=160))
//        {
//        	GPIOB->Data ^=0x100;	//PB8
//        }
//#else
//
//		if(((BufCtl.offset)%320==0)&&(BufCtl.offset>=320))
//        {
//        	GPIOB->Data ^=0x100;	//PB8
//        }
//#endif		

        if(BufCtl.offset==BufCtl.size){
            BufCtl.offset=0;   
			
        } 
	#if _SBC2_AUDIO_BUF_X3
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
			CB_SBC2_WriteDac(fifoSize, tempbuf);	
			BufCtl.offset += (fifoSize - size);
			AudioBufCtl.fifo_count += (fifoSize - size);
		}
	#endif
        

        if(AudioBufCtl.fifo_count>=AudioBufCtl.samples){
#if _SBC2_QIO_MODULE && !_SBC2_AUDIO_BUF_X3
			QIO_DelayStop(QIO_USE_SBC2_CH1,DISABLE);
			QIO_FrameFlag_FIFO_RstGet(QIO_USE_SBC2_CH1);
#endif//_SBC2_QIO_MODULE && !_SBC2_AUDIO_BUF_X3
            AudioBufCtl.state=STATUS_STOP;
            AudioBufCtl.shadow_state=AudioBufCtl.state;
			PlayEndFlag = BIT_SET;
		#if _SBC2_AUDIO_BUF_X3
			if (SBC2_PlayRepeatFlag) {
//				dprintf("fifoSize=%d,size=%d\r\n",fifoSize,size);
				RepeatReSetting_SBC2(fifoSize, size);
			}			
#if _SBC2_QIO_MODULE
			else {
				QIO_DelayStop(QIO_USE_SBC2_CH1,DISABLE);
				QIO_FrameFlag_FIFO_RstGet(QIO_USE_SBC2_CH1);
			}
#endif//_SBC2_QIO_MODULE
		#endif
            //dprintf("stop s,%d,%d\r\n",AudioBufCtl.fifo_count,AudioBufCtl.samples);
        }
    
    }
    else
    {
        CB_SBC2_WriteDac(size,(U16 *)0);
    }   
}  
#endif

#if _SBC2_CH2_PLAY
//------------------------------------------------------------------//
// ISR
// Parameter: 
//          size: fifo fill size 
// return value:
//          NONE
//------------------------------------------------------------------// 
void SBC2_CH2_DacIsr(U8 size){
    U16 temp;
#if _SBC2_CH2_QIO_MODULE
	U16 _QioFrameFlag;
#endif//_SBC2_CH2_QIO_MODULE
    S16 *psrc16,gain;
    U8 i,len,size2;

	if(AudioBufCtl_CH2.shadow_state==STATUS_PLAYING){
#if _SBC2_CH2_QIO_MODULE      
		QIO_Resume(QIO_USE_SBC2_CH2);
#endif//_SBC2_CH2_QIO_MODULE
         
        temp=BufCtl_CH2.size-BufCtl_CH2.offset;
        if(size>temp){
            size=temp;
        }   
#if _SBC2_CH2_QIO_MODULE
		for(i=0;i<size;i++){
			QIO_FrameFlag_FIFO_BitGet(QIO_USE_SBC2_CH2,(U8*)&_QioFrameFlag);
			QIO_DacIsr(QIO_USE_SBC2_CH2, (U8)_QioFrameFlag);
		}
#endif//_SBC2_CH2_QIO_MODULE
        //dprintf("size,%d\r\n",size);
        AudioBufCtl_CH2.fifo_count+=size;   
        
#if SMOOTH_ENABLE       
        //smooth process
        if(SmoothCtl_CH2.smooth!=SMOOTH_NONE){  
            size2= size;
            psrc16=(S16 *)(BufCtl_CH2.buf+BufCtl_CH2.offset);
        
            while(size2){
                if(SmoothCtl_CH2.state==SMOOTH_STEP){
                    if(SmoothCtl_CH2.smooth==SMOOTH_DOWN){
                        for(i=0;i<size2;i++){
                            *psrc16++=0;
                        }       
#if _SBC2_CH2_QIO_MODULE
						if(AudioBufCtl_CH2.state == STATUS_STOP)
						{
							QIO_DelayStop(QIO_USE_SBC2_CH2,ENABLE);
						}
						else
						{
							QIO_Pause(QIO_USE_SBC2_CH2);
						}
#endif//_SBC2_CH2_QIO_MODULE
                        AudioBufCtl_CH2.shadow_state=AudioBufCtl_CH2.state;
                        if(AudioBufCtl_CH2.state==STATUS_STOP){
							PlayEndFlag_CH2 = BIT_SET;
                        }  
                    }
                    SmoothCtl_CH2.smooth=SMOOTH_NONE;
                    break;            
                }
                if(SmoothCtl_CH2.smooth==SMOOTH_DOWN){
                    gain=MAX_SMOOTH_GAIN-(SmoothCtl_CH2.state+1)*(MAX_SMOOTH_GAIN/SMOOTH_STEP);
                }else{
                    gain=(SmoothCtl_CH2.state+1)*(MAX_SMOOTH_GAIN/SMOOTH_STEP);
                }    
                len=SmoothCtl_CH2.step_sample-SmoothCtl_CH2.count;
            
                if(len>size2){
                    len=size2;
                }    
                 
                for(i=0;i<len;i++){
                    *psrc16=((S32)(*psrc16)*gain)>>15;
                    psrc16++;
                }         
                SmoothCtl_CH2.count+=len;
                size2-=len;
                if(SmoothCtl_CH2.count==SmoothCtl_CH2.step_sample){
                    SmoothCtl_CH2.count=0;
                    SmoothCtl_CH2.state++;   
                    //dprintf("s:%d,%d\r\n",SmoothCtl.state,gain);
                }          
            }  
        }
#endif


        psrc16=(S16 *)(BufCtl_CH2.buf+BufCtl_CH2.offset);
//		dprintf("%d\r\n",BufCtl.offset);

        CB_SBC2_CH2_WriteDac(size,(U16 *)psrc16);
        
        BufCtl_CH2.offset+=size;
        

//		if(((BufCtl_CH2.offset)%160==0)&&(BufCtl_CH2.offset>=160))
//        {
//        	GPIOB->Data ^=0x400;	//PB10
//        }
	        
        
        if(BufCtl_CH2.offset==BufCtl_CH2.size){
            BufCtl_CH2.offset=0;   
//			GPIOB->Data ^=0x1000; 
        } 
        

        if(AudioBufCtl_CH2.fifo_count>=AudioBufCtl_CH2.samples){
#if _SBC2_CH2_QIO_MODULE
			QIO_DelayStop(QIO_USE_SBC2_CH2,DISABLE);
			QIO_FrameFlag_FIFO_RstGet(QIO_USE_SBC2_CH2);
#endif//_SBC2_CH2_QIO_MODULE
            AudioBufCtl_CH2.state=STATUS_STOP;
            AudioBufCtl_CH2.shadow_state=AudioBufCtl_CH2.state;
			PlayEndFlag_CH2 = BIT_SET;
            //dprintf("stop s,%d,%d\r\n",AudioBufCtl.fifo_count,AudioBufCtl.samples);
        }
    }
    else
    {
        CB_SBC2_CH2_WriteDac(size,(U16 *)0);
    }   
}  
#endif
  
#if SMOOTH_ENABLE
//------------------------------------------------------------------//
// Smooth up or down
// Parameter: 
//          ch: SMOOTH_CH0,SMOOTH_CH1
//          smooth: SMOOTH_UP,SMOOTH_DOWN,SMOOTH_NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
static void SmoothFunc(U8 ch, volatile SmoothCtl *pSmoothCtl, U8 smooth)
{
	pSmoothCtl->count=0;  
	pSmoothCtl->state=0; 	

	switch(ch)
	{
	    #if	_SBC2_PLAY	
	    case 0x0:
			if(smooth==SMOOTH_UP){
				pSmoothCtl->smooth=SMOOTH_UP;
				pSmoothCtl->step_sample=AudioBufCtl.sample_rate*SMOOTH_UP_TIME/1000/SMOOTH_STEP;
			}else if(smooth==SMOOTH_DOWN){
				pSmoothCtl->smooth=SMOOTH_DOWN;
				pSmoothCtl->step_sample=AudioBufCtl.sample_rate*SMOOTH_DOWN_TIME/1000/SMOOTH_STEP;
			}else{
				pSmoothCtl->smooth=SMOOTH_NONE;
			} 	
	    break;
	    #endif
	            
	    #if	_SBC2_CH2_PLAY
	    case 0x01: 
			if(smooth==SMOOTH_UP){
				pSmoothCtl->smooth=SMOOTH_UP;
				pSmoothCtl->step_sample=AudioBufCtl_CH2.sample_rate*SMOOTH_UP_TIME/1000/SMOOTH_STEP;
			}else if(smooth==SMOOTH_DOWN){
				pSmoothCtl->smooth=SMOOTH_DOWN;
				pSmoothCtl->step_sample=AudioBufCtl_CH2.sample_rate*SMOOTH_DOWN_TIME/1000/SMOOTH_STEP;
			}else{
				pSmoothCtl->smooth=SMOOTH_NONE;
			} 	
	        break;
	    #endif
     }

}     
     
#endif
#if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE)
//------------------------------------------------------------------//
// Init EQ parameter when audio play
// Parameter: 
//          NONE
// return value:
//          EXIT_FAILURE, EXIT_SUCCESS
//------------------------------------------------------------------//   
static U8 initPlayBackEQParameter()
{
	if(SBC2_GetStatus()!=STATUS_STOP)return EXIT_FAILURE;//error
#if _USE_HEAP_RAM
	if(EqBuf==NULL)
	{
		EqBuf = (S32 *) MemAlloc(SBC2_EQ_BUF_SIZE*sizeof(S32));//Free at SBC2_FreeMemory()
	}
#endif	
	memset((void *)EqBuf, 0, SBC2_EQ_BUF_SIZE*sizeof(S32));
	EQFilterCtl.SampleRate = AudioBufCtl.sample_rate;
	EQFilterCtl.Buf = EqBuf;
	
#if	defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE)//customize EQ
	EQFilterCtl.EQGroupSelect=CUSTOMIZE;
#endif

#if	defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE)//customize EQ
	EQFilterCtl.PitchIndex=0;//fix 0
#endif
#if	defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE)//customize EQ
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
#if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE_CH2)
//------------------------------------------------------------------//
// Init EQ parameter when audio play(CH2)
// Parameter: 
//          NONE
// return value:
//          EXIT_FAILURE, EXIT_SUCCESS
//------------------------------------------------------------------//   
static U8 initPlayBackEQParameter_CH2()
{
	if(SBC2_CH2_GetStatus()!=STATUS_STOP)return EXIT_FAILURE;//error
#if _USE_HEAP_RAM
	if(EqBuf_CH2==NULL)
	{
		EqBuf_CH2 = (S32 *) MemAlloc(SBC2_EQ_BUF_SIZE*sizeof(S32));//Free at SBC2_CH2_FreeMemory()
	}
#endif	
	memset((void *)EqBuf_CH2, 0, SBC2_EQ_BUF_SIZE*sizeof(S32));
	EQFilterCtl_CH2.SampleRate = AudioBufCtl_CH2.sample_rate;
	EQFilterCtl_CH2.Buf = EqBuf_CH2;
	
#if	defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE_CH2)//customize EQ
	EQFilterCtl_CH2.EQGroupSelect=CUSTOMIZE;
#endif

#if	defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE_CH2)//customize EQ
	EQFilterCtl_CH2.PitchIndex=0;//fix 0
#endif
#if	defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE_CH2)//customize EQ
	if(u8EQFilterIndex_CH2 >= FILTER_NUMBER){
		EQFilterCtl_CH2.FilterPara1 = _AllPassFilterParaTab;//all pass parameter
	} else {
		EQFilterCtl_CH2.FilterPara1 = (S16*)FilterList[u8EQFilterIndex_CH2];//UserDefine
	}
#endif
	
	EQFilterCtl_CH2.FilterPara2=NULL;
	EQFilterCtl_CH2.Gain1=0;
	EQFilterCtl_CH2.Gain2=0;
	return (EQInit(&EQFilterCtl_CH2)==1)?EXIT_SUCCESS:EXIT_FAILURE;//(EQInit)-1:failure, 1:success
}
#endif
#if _SBC2_PLAY
//------------------------------------------------------------------//
// Decode one audio frame CH1
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
static void DecodeFrame(void)
{
    U16 bpf,mark_sample=0;
    U16 *audio_buf,i;
#if	_SBC2_QIO_MODULE
	U16 _QioFrameFlag,size;
#endif//_SBC2_QIO_MODULE

#if _SBC2_AUDIO_BUF_X3
/*	U16 tempOffset = BufCtl.offset;
	if((AudioBufCtl.index*SBC2_DEC_OUT_SIZE <= tempOffset) && (tempOffset < (AudioBufCtl.index+1)*SBC2_DEC_OUT_SIZE) && AudioBufCtl.shadow_state==STATUS_PLAYING){
		//return;
	}*/
	if (AudioBufCtl.decode_state == 1) {
		AudioBufCtl.decode_state = 0;
	}

	if(!AudioBufCtl.index){
		audio_buf=AudioBuf;
	} else if (AudioBufCtl.index==1){
		audio_buf=AudioBuf+SBC2_DEC_OUT_SIZE;
	} else {
		audio_buf=AudioBuf+SBC2_DEC_OUT_SIZE*2;
	}
#else 

#if(_SBC2_PLAY&&_SBC2_CH2_PLAY)
    U16 *audio_buf_tmp;
	if(!AudioBufCtl.index){
      	audio_buf=AudioBuf;				//0, A buffer
		audio_buf_tmp=NULL;
    }
    else if(AudioBufCtl.index==1)
    {
        audio_buf=AudioBuf+SBC2_DEC_OUT_SIZE;				//0+320, C buffer
        audio_buf_tmp=AudioBuf;								//0, A buffer
    }
    else// (AudioBufCtl.index==2)
    {
    	audio_buf=AudioBuf+(SBC2_DEC_OUT_SIZE/2);				// 0+160, B buffer
		audio_buf_tmp=NULL;
	}
#else
	if(!AudioBufCtl.index)
	{
		audio_buf=AudioBuf;
	} 
	else
	{
		audio_buf=AudioBuf+SBC2_DEC_OUT_SIZE;
	}
#endif
#endif

//    if(mute){
//        for(i=0;i<SBC2_DEC_OUT_SIZE;i++){
//            audio_buf[i]=0;
//        }   
//        AudioBufCtl.toggle^=1;
//        return;        
//    }       

        CB_ReadData(StorageMode,&CurAddr,(U8 *)audio_buf,2);
        // Modify for Encode bit number error
        bpf=SBC2BPF(0,audio_buf[0],&mark_sample);
        
#if	_SBC2_QIO_MODULE
		_QioFrameFlag = (mark_sample!=0) ? mark_sample : 0;
		size = SBC2_DEC_OUT_SIZE;
		if((size+AudioBufCtl.sample_count)>AudioBufCtl.samples)
		{
			size=AudioBufCtl.samples-AudioBufCtl.sample_count;
		}
		if(QIO_FrameFlag_FIFO_BitPut(QIO_USE_SBC2_CH1,(U8*)&_QioFrameFlag)==EXIT_SUCCESS)
		{
			_QioFrameFlag = 0;
			for(i=1;i<size;i++)
			{
				QIO_FrameFlag_FIFO_BitPut(QIO_USE_SBC2_CH1,(U8*)&_QioFrameFlag);
			}
		}
		AudioBufCtl.sample_count+=size;
		if(AudioBufCtl.samples == AudioBufCtl.sample_count)
		{
			AudioBufCtl.sample_count = 0;
			QIO_FrameFlag_FIFO_RstPut(QIO_USE_SBC2_CH1);
		}
#endif//_SBC2_QIO_MODULE
		
        if(mark_sample!=0)
    	{
    		CB_ReadData(StorageMode,&CurAddr,(U8 *)audio_buf,2);
    		bpf=SBC2BPF(0,audio_buf[0],&mark_sample);
    	}
        CB_ReadData(StorageMode,&CurAddr,(U8 *)(audio_buf+1),bpf-2);

#if(_SBC2_PLAY&&_SBC2_CH2_PLAY&&(!_SBC2_AUDIO_BUF_X3))
		if(!AudioBufCtl.index)
		{
    		SBC2Decode(0,(void *)audio_buf,(void *)(audio_buf+SBC2_DEC_OUT_SIZE/2));		// addr1= AudioBuf+0, addr2=AudioBuf+160
			AudioBufCtl.buf_swap=0;
    	}
    	else if(AudioBufCtl.index==1)
    	{
    		SBC2Decode(0,(U16 *)audio_buf,(U16 *)AudioBuf);									// addr1= AudioBuf+320, addr2=AudioBuf+0
			AudioBufCtl.buf_swap=1;
    	}
    	else if(AudioBufCtl.index==2)
    	{
    		SBC2Decode(0,(void *)audio_buf,(void *)(audio_buf+(SBC2_DEC_OUT_SIZE/2)));		// addr1= AudioBuf+160, addr2=AudioBuf+320
			AudioBufCtl.buf_swap=0;
    	}
#else
		SBC2Decode(0,(void *)audio_buf,(void *)(audio_buf+SBC2_DEC_OUT_SIZE/2));       
#endif
	AudioBufCtl.process_count+=SBC2_DEC_OUT_SIZE;

	#if _SBC2_AUDIO_BUF_X3	
		//if(AudioBufCtl.preDecFrame){
		AudioBufCtl.preDecCount += SBC2_DEC_OUT_SIZE;
		//}		
	#endif
	
		AudioBufCtl.index++;
#if(_SBC2_PLAY&&(!_SBC2_AUDIO_BUF_X3)&&(!_SBC2_CH2_PLAY))
		if(AudioBufCtl.index == 2) 
		{
			AudioBufCtl.index = 0;
		}
#else
		if(AudioBufCtl.index == 3) 
		{
			AudioBufCtl.index = 0;
		}
#endif
	


        //volume 
        for(i=0;i<SBC2_DEC_OUT_SIZE;i++){
#if(_SBC2_PLAY&&_SBC2_CH2_PLAY&&(!_SBC2_AUDIO_BUF_X3))
            if(AudioBufCtl.buf_swap==1)	// C buf process is finish, change to A buf process. (It is not continus buffer control.)
            {
            	if(i>=(SBC2_DEC_OUT_SIZE/2))
            	{
                    U16 _shift_i = i-(SBC2_DEC_OUT_SIZE/2);
                    #if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE)
                        audio_buf_tmp[_shift_i] = EQProcess(audio_buf_tmp[_shift_i],&EQFilterCtl);
                    #endif
            		#if _AUDIO_VOLUME_CHANGE_SMOOTH
			        	audio_buf_tmp[_shift_i]=((S16)audio_buf_tmp[_shift_i]*AUDIO_GetVolumeSmooth(&VolCtrl))>>Q15_SHIFT;
					#else
						audio_buf_tmp[_shift_i]=((S16)audio_buf_tmp[_shift_i]*AudioVol)>>Q15_SHIFT;
					#endif
            	}
            	else
            	{
                    #if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE)
                        audio_buf[i] = EQProcess(audio_buf[i],&EQFilterCtl);
                    #endif
            		#if _AUDIO_VOLUME_CHANGE_SMOOTH
			        	audio_buf[i]=((S16)audio_buf[i]*AUDIO_GetVolumeSmooth(&VolCtrl))>>Q15_SHIFT;
					#else
						audio_buf[i]=((S16)audio_buf[i]*AudioVol)>>Q15_SHIFT;
					#endif	
            	}
            }
            else
            {           
                #if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE)
                    audio_buf[i] = EQProcess(audio_buf[i],&EQFilterCtl);
                #endif
            	#if _AUDIO_VOLUME_CHANGE_SMOOTH
		        	audio_buf[i]=((S16)audio_buf[i]*AUDIO_GetVolumeSmooth(&VolCtrl))>>Q15_SHIFT;
				#else
					audio_buf[i]=((S16)audio_buf[i]*AudioVol)>>Q15_SHIFT;
				#endif
        	}		
#else
        #if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE)
            audio_buf[i] = EQProcess(audio_buf[i],&EQFilterCtl);
        #endif
        #if _AUDIO_VOLUME_CHANGE_SMOOTH
        	audio_buf[i]=((S16)audio_buf[i]*AUDIO_GetVolumeSmooth(&VolCtrl))>>Q15_SHIFT;
		#else
			audio_buf[i]=((S16)audio_buf[i]*AudioVol)>>Q15_SHIFT;
		#endif
#endif
        }
         
    
#if _SBC2_AUDIO_BUF_X3
	AudioBufCtl.decode_state++;
#endif
} 
#endif   

#if _SBC2_CH2_PLAY
//------------------------------------------------------------------//
// Decode one audio frame CH2
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
static void DecodeFrame_CH2(void)
{
    U16 bpf,mark_sample=0;
    U16 *audio_buf,i;
#if	_SBC2_CH2_QIO_MODULE
	U16 _QioFrameFlag,size;
#endif//_SBC2_CH2_QIO_MODULE
    U16 *audio_buf_tmp;
   

	    if(!AudioBufCtl_CH2.index){
        	audio_buf=AudioBuf_CH2;									//0, A buffer
			audio_buf_tmp=NULL;
		}
    	else if(AudioBufCtl_CH2.index==1)
    	{
        	audio_buf=AudioBuf_CH2+SBC2_DEC_OUT_SIZE;				//0+320, C buffer
        	audio_buf_tmp=AudioBuf_CH2;								//0, A buffer
    	}
    	else// (AudioBufCtl_CH2.index==2)
    	{
    		audio_buf=AudioBuf_CH2+(SBC2_DEC_OUT_SIZE/2);			// 0+160, B buffer
			audio_buf_tmp=NULL;
		}



//    if(mute){
//        for(i=0;i<SBC2_DEC_OUT_SIZE;i++){
//            audio_buf[i]=0;
//        }   
//        AudioBufCtl.toggle^=1;
//        return;        
//    }        
        CB_ReadData(StorageMode_CH2,&CurAddr_CH2,(U8 *)audio_buf,2);
        // Modify for Encode bit number error
	    bpf=SBC2BPF(1,audio_buf[0],&mark_sample);
	        
#if	_SBC2_CH2_QIO_MODULE
    	_QioFrameFlag = (mark_sample!=0) ? mark_sample : 0;
		size = SBC2_DEC_OUT_SIZE;
		if((size+AudioBufCtl_CH2.sample_count)>AudioBufCtl_CH2.samples)
		{
			size=AudioBufCtl_CH2.samples-AudioBufCtl_CH2.sample_count;
		}
		if(QIO_FrameFlag_FIFO_BitPut(QIO_USE_SBC2_CH2,(U8*)&_QioFrameFlag)==EXIT_SUCCESS)
		{
			_QioFrameFlag = 0;
			for(i=1;i<size;i++)
			{
				QIO_FrameFlag_FIFO_BitPut(QIO_USE_SBC2_CH2,(U8*)&_QioFrameFlag);
			}
		}
		AudioBufCtl_CH2.sample_count+=size;
		if(AudioBufCtl_CH2.samples == AudioBufCtl_CH2.sample_count)
		{
			QIO_FrameFlag_FIFO_RstPut(QIO_USE_SBC2_CH2);
		}
#endif//_SBC2_CH2_QIO_MODULE
			
	    if(mark_sample!=0)
	    {
	    	CB_ReadData(StorageMode_CH2,&CurAddr_CH2,(U8 *)audio_buf,2);
	    	bpf=SBC2BPF(1,audio_buf[0],&mark_sample);
	    }
	        
	    CB_ReadData(StorageMode_CH2,&CurAddr_CH2,(U8 *)(audio_buf+1),bpf-2);      																		
		
		if(!AudioBufCtl_CH2.index) 
		{
    		SBC2Decode(1,(void *)audio_buf,(void *)(audio_buf+SBC2_DEC_OUT_SIZE/2));		// addr1= AudioBuf+0, addr2=AudioBuf+160
			AudioBufCtl_CH2.buf_swap=0;
    	}
    	else if(AudioBufCtl_CH2.index==1)  
    	{
    		SBC2Decode(1,(U16 *)audio_buf,(U16 *)AudioBuf_CH2);								// addr1= AudioBuf+320, addr2=AudioBuf+0
			AudioBufCtl_CH2.buf_swap=1;
    	}
    	else if(AudioBufCtl_CH2.index==2)
    	{
    		SBC2Decode(1,(void *)audio_buf,(void *)(audio_buf+(SBC2_DEC_OUT_SIZE/2)));		// addr1= AudioBuf+160, addr2=AudioBuf+320
			AudioBufCtl_CH2.buf_swap=0;
    	}


		AudioBufCtl_CH2.process_count+=(SBC2_DEC_OUT_SIZE);

		AudioBufCtl_CH2.index++;
		if(AudioBufCtl_CH2.index == 3) 
		{
			AudioBufCtl_CH2.index = 0;
		}

        //volume 
        for(i=0;i<SBC2_DEC_OUT_SIZE;i++){

            if(AudioBufCtl_CH2.buf_swap==1)	// C buf process is finish, change to A buf process. (It is not continus buffer control.)
            {
            	if(i>=(SBC2_DEC_OUT_SIZE/2))
            	{
                    U16 _shift_i = i-(SBC2_DEC_OUT_SIZE/2);
                    #if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE_CH2)
                        audio_buf_tmp[_shift_i] = EQProcess(audio_buf_tmp[_shift_i],&EQFilterCtl_CH2);
                    #endif
            		#if _AUDIO_VOLUME_CHANGE_SMOOTH
			        	audio_buf_tmp[_shift_i]=((S16)audio_buf_tmp[_shift_i]*AUDIO_GetVolumeSmooth(&VolCtrl_CH2))>>Q15_SHIFT;
					#else
						audio_buf_tmp[_shift_i]=((S16)audio_buf_tmp[_shift_i]*AudioVol_CH2)>>Q15_SHIFT;
					#endif
            	}
            	else
            	{
                    #if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE_CH2)
                        audio_buf[i] = EQProcess(audio_buf[i],&EQFilterCtl_CH2);
                    #endif
            		#if _AUDIO_VOLUME_CHANGE_SMOOTH
			        	audio_buf[i]=((S16)audio_buf[i]*AUDIO_GetVolumeSmooth(&VolCtrl_CH2))>>Q15_SHIFT;
					#else
						audio_buf[i]=((S16)audio_buf[i]*AudioVol_CH2)>>Q15_SHIFT;
					#endif	
            	}
            }
            else
            {           
                #if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE_CH2)
                    audio_buf[i] = EQProcess(audio_buf[i],&EQFilterCtl_CH2);
                #endif
            	#if _AUDIO_VOLUME_CHANGE_SMOOTH
		        	audio_buf[i]=((S16)audio_buf[i]*AUDIO_GetVolumeSmooth(&VolCtrl_CH2))>>Q15_SHIFT;
				#else
					audio_buf[i]=((S16)audio_buf[i]*AudioVol_CH2)>>Q15_SHIFT;
				#endif
        	}
        }
        
}    
#endif

#if	_SBC2_PLAY
//------------------------------------------------------------------//
// CH1 Start play voice
// Parameter: 
//          index: file index or absolute address
//          mode:OTP_MODE,EF_MODE,SPI_MODE,SPI1_MODE,EF_UD_MODE
// return value:
//          0:ok
//          others:fail
//------------------------------------------------------------------// 
U8 SBC2_Play(U32 index,U8 mode){
    struct AUDIO_HEADER *pheader;
    
    if(AudioBufCtl.shadow_state==STATUS_PLAYING){
#if SMOOTH_ENABLE        
            AudioBufCtl.state=STATUS_STOP;
            SmoothFunc(SMOOTH_CH0,&SmoothCtl_CH1,SMOOTH_DOWN);
            if(!DacMixerRamp_IsBias()) {
                AudioBufCtl.shadow_state=STATUS_STOP;
#if _SBC2_QIO_MODULE      
                QIO_Stop(QIO_USE_SBC2_CH1,ENABLE);
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
    SBC2_AllocateMemory(MEMORY_ALLOCATE_PLAY);
#else
	AUDIO_ServiceLoop();
#endif

    memset((void *)&AudioBufCtl, 0, sizeof(AudioBufCtl));
  
    BufCtl.offset=0;  
    BufCtl.size=SBC2_AUDIO_BUF_SIZE;	//SBC-2=640(320*2) for 1ch play ; 480(160*3) for 2ch mixer play
    BufCtl.buf=AudioBuf;
    
    if(CB_SetStartAddr(index,mode,0,&StartAddr)){
		return E_AUDIO_INVALID_IDX;
	}
    CurAddr=StartAddr;
    StorageMode=mode;
		
    CB_ReadData(StorageMode,&CurAddr,(U8 *)AudioBuf,HEADER_LEN);
    

    pheader=(struct AUDIO_HEADER *)AudioBuf;
    
    //dprintf("Play:%x,%x,%x\r\n",pheader->signature[0],pheader->signature[1],pheader->signature[2]);
    
    if((pheader->codec!=CODEC_SBC2)&&(pheader->codec!=CODEC_SBC2_E)){  		//
        return E_AUDIO_UNSUPPORT_CODEC;    
    }  

	GIE_DISABLE();
    S8 hw_ch = DacAssigner_Regist(kSBC2_CH1,pheader->sample_rate);
	GIE_ENABLE();
    if(hw_ch<0){
        //dprintf("No play Invalid sr\r\n");
        return E_AUDIO_INVALID_SAMPLERATE;
    }
	//dprintf("codec %d reg hw_ch %d\r\n",kSBC2_CH1,hw_ch);

#if _SBC2_QIO_MODULE
	if(pheader->nyq_offset)
	{
		QIO_Play(QIO_USE_SBC2_CH1, mode, (StartAddr+HEADER_LEN+pheader->nyq_offset));
	}
#endif//_SBC2_QIO_MODULE	
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
        SmoothFunc(SMOOTH_CH0,&SmoothCtl_CH1,SMOOTH_UP);
//    }else{
//        SmoothFunc(SMOOTH_CH0,&SmoothCtl_CH1,SMOOTH_NONE);
//    }  
#endif    
    //dprintf("pheader->decode_in_size,%d\r\n",pheader->decode_in_size);
    
	SBC2DecInit(0,(S16 *)SBC2Ram,(S16 *)SBC2TempBuf,AudioBufCtl.sample_rate,pheader->bandwidth,pheader->bit_per_frame&0x7FFF,pheader->decode_in_size,pheader->decode_out_size,pheader->bit_per_frame>>15);
    
    if(pheader->codec==CODEC_SBC2_E && pheader->samples>SBC2_DEC_OUT_SIZE){
		CB_ReadData(StorageMode,&CurAddr,(U8 *)SBC2Ram,160<<1);
	}
	
    memset((void *)AudioBuf, 0, SBC2_AUDIO_BUF_SIZE*2);
    
    //dprintf("AudioBufCtl.process_count,%d\r\n",AudioBufCtl.process_count);
    
	CB_SBC2_InitDac(pHwCtl[hw_ch].p_dac,AudioBufCtl.sample_rate,AudioBufCtl.upsample);

#if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE)
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
    
//  GPIOB->Data &= (~(1<<8));
	DecodeFrame();
//	GPIOB->Data |= (1<<8);

    OpMode=0;
    AudioBufCtl.state=  STATUS_PLAYING;
    AudioBufCtl.shadow_state=AudioBufCtl.state;
    
    CB_SBC2_PlayStart();     
    
	CB_SBC2_DacCmd(pHwCtl[hw_ch].p_dac,ENABLE);
    return 0;
}   
#endif

#if	_SBC2_CH2_PLAY
//------------------------------------------------------------------//
// CH2 Start play voice
// Parameter: 
//          index: file index or absolute address
//          mode:OTP_MODE,EF_MODE,SPI_MODE,SPI1_MODE,EF_UD_MODE
// return value:
//          0:ok
//          others:fail
//------------------------------------------------------------------// 
U8 SBC2_CH2_Play(U32 index,U8 mode){
    struct AUDIO_HEADER *pheader;
    
    if(AudioBufCtl_CH2.shadow_state==STATUS_PLAYING){
#if SMOOTH_ENABLE        
            AudioBufCtl_CH2.state=STATUS_STOP;
            SmoothFunc(SMOOTH_CH1,&SmoothCtl_CH2,SMOOTH_DOWN);
            if(!DacMixerRamp_IsBias()) {
                AudioBufCtl_CH2.shadow_state=STATUS_STOP;
#if _SBC2_CH2_QIO_MODULE      
                QIO_Stop(QIO_USE_SBC2_CH2,ENABLE);
#endif
            }
            while(AudioBufCtl_CH2.shadow_state==STATUS_PLAYING){
                AUDIO_ServiceLoop();
            }
#endif        
    }
#if _USE_HEAP_RAM    
#if _VR_FUNC
    VR_Halt();
#endif
	AUDIO_ServiceLoop();
    SBC2_CH2_AllocateMemory(MEMORY_ALLOCATE_PLAY);
#else
	AUDIO_ServiceLoop();
#endif

    memset((void *)&AudioBufCtl_CH2, 0, sizeof(AudioBufCtl_CH2));
    
    
    BufCtl_CH2.offset=0;  
    BufCtl_CH2.size=SBC2_CH2_AUDIO_BUF_SIZE;	//SBC-2=480 (160*3)
    BufCtl_CH2.buf=AudioBuf_CH2;
    
    if(CB_SetStartAddr(index,mode,0,&StartAddr_CH2)){
		return E_AUDIO_INVALID_IDX;
	}
    CurAddr_CH2=StartAddr_CH2;
    StorageMode_CH2=mode;
		
    CB_ReadData(StorageMode_CH2,&CurAddr_CH2,(U8 *)AudioBuf_CH2,HEADER_LEN);
	
    pheader=(struct AUDIO_HEADER *)AudioBuf_CH2;
    
    //dprintf("Play:%x,%x,%x\r\n",pheader->signature[0],pheader->signature[1],pheader->signature[2]);
    
    if((pheader->codec!=CODEC_SBC2)&&(pheader->codec!=CODEC_SBC2_E)){  		
        return E_AUDIO_UNSUPPORT_CODEC;    
    }     

	GIE_DISABLE();
    S8 hw_ch = DacAssigner_Regist(kSBC2_CH2,pheader->sample_rate);
	GIE_ENABLE();
    if(hw_ch<0){
        //dprintf("No play Invalid sr\r\n");
        return E_AUDIO_INVALID_SAMPLERATE;
    }
	//dprintf("codec %d reg hw_ch %d\r\n",kSBC2_CH2,hw_ch);
   
#if _SBC2_CH2_QIO_MODULE
	if(pheader->nyq_offset)
	{
		QIO_Play(QIO_USE_SBC2_CH2, mode, (StartAddr_CH2+HEADER_LEN+pheader->nyq_offset));
	}
#endif//_SBC2_CH2_QIO_MODULE
//    dprintf("bit_per_frame:%x\r\n",pheader->bit_per_frame);   
//    dprintf("decode_in_size:%x\r\n",pheader->decode_in_size);   
//    dprintf("decode_out_size:%x\r\n",pheader->decode_out_size); 
//    dprintf("sample rate:%x\r\n",pheader->sample_rate); 
//    dprintf("samples:%x\r\n",pheader->samples); 
    
    AudioBufCtl_CH2.samples=pheader->samples;
    AudioBufCtl_CH2.sample_rate=pheader->sample_rate;
	AudioBufCtl_CH2.upsample=pheader->flag&0x1;

#if SMOOTH_ENABLE     
//    if(ramp&RAMP_UP){
        SmoothFunc(SMOOTH_CH1,&SmoothCtl_CH2,SMOOTH_UP);
//    }else{
//        SmoothFunc(SMOOTH_CH1,&SmoothCtl_CH2,SMOOTH_NONE);
//    }  
#endif    
    //dprintf("pheader->decode_in_size,%d\r\n",pheader->decode_in_size);
    
	SBC2DecInit(1,(S16 *)SBC2Ram_CH2,(S16 *)SBC2TempBuf,AudioBufCtl_CH2.sample_rate,pheader->bandwidth,pheader->bit_per_frame&0x7FFF,pheader->decode_in_size,pheader->decode_out_size,pheader->bit_per_frame>>15);

	if(pheader->codec==CODEC_SBC2_E && pheader->samples>SBC2_DEC_OUT_SIZE){
		CB_ReadData(StorageMode_CH2,&CurAddr_CH2,(U8 *)SBC2Ram_CH2,160<<1);		
	}	   
    
    memset((void *)AudioBuf_CH2, 0, SBC2_CH2_AUDIO_BUF_SIZE*2);
    
    //dprintf("AudioBufCtl.process_count,%d\r\n",AudioBufCtl.process_count);
    
	CB_SBC2_CH2_InitDac(pHwCtl[hw_ch].p_dac,AudioBufCtl_CH2.sample_rate,AudioBufCtl_CH2.upsample);
#if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE_CH2)
	if(initPlayBackEQParameter_CH2()!=EXIT_SUCCESS){
		return 1;//init error
	}
#endif
    AudioVol_CH2=  (ChVol_CH2+1)*(MixerVol+1); 
        
    if(!ChVol_CH2){
        AudioVol_CH2=0;
    }    
    if(AudioVol_CH2==256){
        AudioVol_CH2=255;
    }    
    AudioVol_CH2=AudioVol_CH2<<7;

#if _AUDIO_VOLUME_CHANGE_SMOOTH
    AUDIO_InitVolumeSmooth(&VolCtrl_CH2,AudioBufCtl_CH2.sample_rate,AudioVol_CH2);
#endif

//  GPIOB->Data &= (~(1<<12));
	DecodeFrame_CH2();
//	GPIOB->Data |= (1<<12);
    
    OpMode_CH2=0;
    AudioBufCtl_CH2.state=  STATUS_PLAYING;
    AudioBufCtl_CH2.shadow_state=AudioBufCtl_CH2.state;
    
    CB_SBC2_CH2_PlayStart();     
    
	CB_SBC2_CH2_DacCmd(pHwCtl[hw_ch].p_dac,ENABLE);
    
    return 0;
}  
#endif 

#if	_SBC2_PLAY
//------------------------------------------------------------------//
// CH1 Pause playing
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void SBC2_Pause(void){
    if(AudioBufCtl.state==STATUS_PLAYING){
  
#if SMOOTH_ENABLE              
            AudioBufCtl.state=STATUS_PAUSE;
            SmoothFunc(SMOOTH_CH0,&SmoothCtl_CH1,SMOOTH_DOWN);
#endif           
    }      
}
#endif   

#if	_SBC2_CH2_PLAY
//------------------------------------------------------------------//
// CH2 Pause playing
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void SBC2_CH2_Pause(void){
    if(AudioBufCtl_CH2.state==STATUS_PLAYING){
 

#if SMOOTH_ENABLE              
            AudioBufCtl_CH2.state=STATUS_PAUSE;
            SmoothFunc(SMOOTH_CH1,&SmoothCtl_CH2,SMOOTH_DOWN);
#endif        
    }       
} 
#endif

#if	_SBC2_PLAY
//------------------------------------------------------------------//
// CH1 Resume playing
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void SBC2_Resume(void){
	if(AudioBufCtl.state==STATUS_PAUSE&&(!OpMode)){
        if(DacAssigner_codecUseDacCh(kSBC2_CH1)<0) {
            GIE_DISABLE();
            S8 hw_ch = DacAssigner_Regist(kSBC2_CH1,AudioBufCtl.sample_rate);
            GIE_ENABLE();
            if(hw_ch<0) return;//error
            
            CB_SBC2_InitDac(pHwCtl[hw_ch].p_dac,AudioBufCtl.sample_rate,AudioBufCtl.upsample);
            CB_SBC2_DacCmd(pHwCtl[hw_ch].p_dac,ENABLE);
        }

#if SMOOTH_ENABLE         
            SmoothFunc(SMOOTH_CH0,&SmoothCtl_CH1,SMOOTH_UP);
#endif        
        AudioBufCtl.state=STATUS_PLAYING;
        AudioBufCtl.shadow_state=AudioBufCtl.state;
    }            
} 
#endif  

#if	_SBC2_CH2_PLAY
//------------------------------------------------------------------//
// CH2 Resume playing
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void SBC2_CH2_Resume(void){
	if(AudioBufCtl_CH2.state==STATUS_PAUSE&&(!OpMode_CH2)){
        if(DacAssigner_codecUseDacCh(kSBC2_CH2)<0) {
            GIE_DISABLE();
            S8 hw_ch = DacAssigner_Regist(kSBC2_CH2,AudioBufCtl_CH2.sample_rate);
            GIE_ENABLE();
            if(hw_ch<0) return;//error
            
            CB_SBC2_CH2_InitDac(pHwCtl[hw_ch].p_dac,AudioBufCtl_CH2.sample_rate,AudioBufCtl_CH2.upsample);
            CB_SBC2_CH2_DacCmd(pHwCtl[hw_ch].p_dac,ENABLE);
        }

#if SMOOTH_ENABLE         
            SmoothFunc(SMOOTH_CH1,&SmoothCtl_CH2,SMOOTH_UP);
#endif        
        AudioBufCtl_CH2.state=STATUS_PLAYING;
        AudioBufCtl_CH2.shadow_state=AudioBufCtl_CH2.state;
    }               
} 
#endif

#if	_SBC2_PLAY
//------------------------------------------------------------------//
// CH1 Stop playing
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void SBC2_Stop(void){
    
    if((AudioBufCtl.state==STATUS_PLAYING||AudioBufCtl.state==STATUS_PAUSE)&&(!OpMode)){
#if SMOOTH_ENABLE         
            AudioBufCtl.state=STATUS_STOP;
            SmoothFunc(SMOOTH_CH0,&SmoothCtl_CH1,SMOOTH_DOWN);
			if (AudioBufCtl.shadow_state == STATUS_PAUSE
                || !DacMixerRamp_IsBias()
            ) {
				AudioBufCtl.shadow_state = STATUS_STOP;
#if _SBC2_QIO_MODULE
				QIO_Stop(QIO_USE_SBC2_CH1,ENABLE);
#endif//_SBC2_QIO_MODULE
			}
#endif             
    }    
} 
#endif

#if	_SBC2_CH2_PLAY
//------------------------------------------------------------------//
// CH2 Stop playing
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void SBC2_CH2_Stop(void){
    
    if((AudioBufCtl_CH2.state==STATUS_PLAYING||AudioBufCtl_CH2.state==STATUS_PAUSE)&&(!OpMode_CH2)){
#if SMOOTH_ENABLE         
            AudioBufCtl_CH2.state=STATUS_STOP;
            SmoothFunc(SMOOTH_CH1,&SmoothCtl_CH2,SMOOTH_DOWN);
			if (AudioBufCtl_CH2.shadow_state == STATUS_PAUSE
                || !DacMixerRamp_IsBias()
            ) {
				AudioBufCtl_CH2.shadow_state = STATUS_STOP;
#if _SBC2_CH2_QIO_MODULE
				QIO_Stop(QIO_USE_SBC2_CH2,ENABLE);
#endif//_SBC2_CH2_QIO_MODULE
			}
#endif        
    
    }  
} 
#endif

#if	_SBC2_PLAY
//------------------------------------------------------------------//
// CH1 Get current status
// Parameter: 
//          NONE
// return value:
//          status:STATUS_PLAYING,STATUS_PAUSE,STATUS_STOP
//------------------------------------------------------------------// 
U8 SBC2_GetStatus(void){
    return AudioBufCtl.shadow_state;    
}
#endif

#if	_SBC2_CH2_PLAY
//------------------------------------------------------------------//
// CH2 Get current status
// Parameter: 
//          NONE
// return value:
//          status:STATUS_PLAYING,STATUS_PAUSE,STATUS_STOP
//------------------------------------------------------------------// 
U8 SBC2_CH2_GetStatus(void){
    return AudioBufCtl_CH2.shadow_state;    
}
#endif

#if	_SBC2_PLAY
//------------------------------------------------------------------//
// CH1 Set audio volume
// Parameter: 
//          vol:0~15
// return value:
//          NONE
//------------------------------------------------------------------// 
void SBC2_SetVolume(U8 vol){   
    if(vol>CH_VOL_15){
        vol=CH_VOL_15;
    }       
    ChVol=vol;   
}
//------------------------------------------------------------------//
// CH1 Set audio MixCtrl
// Parameter: 
//          mixCtrl:MIX_CTRL_0 / MIX_CTRL_25 / MIX_CTRL_50 / MIX_CTRL_75 / MIX_CTRL_100
// return value:
//          NONE
//------------------------------------------------------------------// 
#if _AUDIO_MIX_CONTROL
void SBC2_SetMixCtrl(U8 mixCtrl){
    if(mixCtrl>MIX_CTRL_100) mixCtrl=MIX_CTRL_100;
    Sbc2MixCtrl=mixCtrl;
}
#endif
#endif

#if	_SBC2_CH2_PLAY
//------------------------------------------------------------------//
// CH2 Set audio volume
// Parameter: 
//          vol:0~15
// return value:
//          NONE
//------------------------------------------------------------------// 
void SBC2_CH2_SetVolume(U8 vol){   
    if(vol>CH_VOL_15){
        vol=CH_VOL_15;
    }       
    ChVol_CH2=vol;   
}
//------------------------------------------------------------------//
// CH2 Set audio MixCtrl
// Parameter: 
//          mixCtrl:MIX_CTRL_0 / MIX_CTRL_25 / MIX_CTRL_50 / MIX_CTRL_75 / MIX_CTRL_100
// return value:
//          NONE
//------------------------------------------------------------------// 
#if _AUDIO_MIX_CONTROL
void SBC2_CH2_SetMixCtrl(U8 mixCtrl){
    if(mixCtrl>MIX_CTRL_100) mixCtrl=MIX_CTRL_100;
    Sbc2MixCtrl_CH2=mixCtrl;
}
#endif
#endif
#if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE)
//------------------------------------------------------------------//
// Set customize filter index
// Parameter: 
//          index: (0~FILTER_NUMBER-1)User define filter, (other)All Pass
// return value:
//          NONE
//------------------------------------------------------------------// 
void SBC2_SetEQFilter(U8 index){
	u8EQFilterIndex = index;
#if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE)
	if(u8EQFilterIndex >= FILTER_NUMBER){
		EQFilterCtl.FilterPara1 = _AllPassFilterParaTab;//all pass parameter
	} else {
		EQFilterCtl.FilterPara1 = (S16*)FilterList[u8EQFilterIndex];//UserDefine
	}
#endif
}
#endif
#if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE_CH2)
//------------------------------------------------------------------//
// Set customize filter index
// Parameter: 
//          index: (0~FILTER_NUMBER-1)User define filter, (other)All Pass
// return value:
//          NONE
//------------------------------------------------------------------// 
void SBC2_CH2_SetEQFilter(U8 index){
	u8EQFilterIndex_CH2 = index;
#if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE_CH2)
	if(u8EQFilterIndex_CH2 >= FILTER_NUMBER){
		EQFilterCtl_CH2.FilterPara1 = _AllPassFilterParaTab;//all pass parameter
	} else {
		EQFilterCtl_CH2.FilterPara1 = (S16*)FilterList[u8EQFilterIndex_CH2];//UserDefine
	}
#endif
}
#endif
#if	_SBC2_PLAY
//------------------------------------------------------------------//
// Codec CH1 init
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void SBC2_Init(void){
    ChVol=CH_VOL_15;       
#if _AUDIO_MIX_CONTROL
    SBC2_SetMixCtrl(MIX_CTRL_100);
#endif
#if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE)
	SBC2_SetEQFilter(FILTER_NUMBER);
#endif      
}
#endif

#if	_SBC2_CH2_PLAY
//------------------------------------------------------------------//
// Codec CH2 init
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void SBC2_CH2_Init(void){
    ChVol_CH2=CH_VOL_15;       
#if _AUDIO_MIX_CONTROL
    SBC2_CH2_SetMixCtrl(MIX_CTRL_100);
#endif
#if defined(_SBC2_PLAY_BACK_USE_CUSTOMIZE_CH2)
	SBC2_CH2_SetEQFilter(FILTER_NUMBER);
#endif 
}
#endif

#if	_SBC2_PLAY
//------------------------------------------------------------------//
// Service loop
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
void SBC2_ServiceLoop(void){

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

	#if _SBC2_AUDIO_BUF_X3
        if((AudioBufCtl.fifo_count+(SBC2_DEC_OUT_SIZE)*2-AudioBufCtl.process_count)>=0){
	#else 
	#if(_SBC2_PLAY&&_SBC2_CH2_PLAY)
        if((AudioBufCtl.fifo_count+(SBC2_DEC_OUT_SIZE/2)-AudioBufCtl.process_count)>=0){
	#else
		if((AudioBufCtl.fifo_count+(SBC2_DEC_OUT_SIZE)-AudioBufCtl.process_count)>=0){
	#endif
	#endif
			if(AudioBufCtl.process_count<AudioBufCtl.samples)
			{
//				GPIOB->Data &= (~(1<<9));
                DecodeFrame();
//                GPIOB->Data |= (1<<9);
			#if _SBC2_AUDIO_BUF_X3
				if(AudioBufCtl.preDecFrame){
					AudioBufCtl.preDecFrame = 0;
				}
			#endif
            }
            
		#if _SBC2_AUDIO_BUF_X3
			else {
				if (SBC2_PlayRepeatFlag) {
					if ((!AudioBufCtl.preDecFrame) && (AudioBufCtl.decode_state==1)) {
						U8 tempBuf[HEADER_LEN];
						struct AUDIO_HEADER *pheader;
						CurAddr=StartAddr;
						CB_ReadData(StorageMode,&CurAddr,(U8 *)tempBuf,HEADER_LEN);
						pheader=(struct AUDIO_HEADER *)tempBuf;
						SBC2DecInit(0,(S16*)SBC2Ram,(S16*)SBC2TempBuf,AudioBufCtl.sample_rate,pheader->bandwidth,pheader->bit_per_frame&0x7FFF,pheader->decode_in_size,pheader->decode_out_size,pheader->bit_per_frame>>15);
						if(pheader->codec==CODEC_SBC2_E && pheader->samples>SBC2_DEC_OUT_SIZE){
							CB_ReadData(StorageMode,&CurAddr,(U8 *)(SBC2Ram),160<<1);
						}
						AudioBufCtl.preDecFrame = 1;
						AudioBufCtl.preDecCount = 0;
#if _SBC2_QIO_MODULE
						QIO_ReGetStorageDataSet(QIO_USE_SBC2_CH1);
#endif//_SBC2_QIO_MODULE
					}
					
					if((AudioBufCtl.preDecFrame == 1) || (AudioBufCtl.preDecFrame == 2)){
						
//						GPIOB->Data &= (~(1<<10));
						DecodeFrame();
//						GPIOB->Data |= (1<<10);
						AudioBufCtl.preDecFrame++;	
					}
				}
			}
		#endif
        }           
    }    


#if _USE_HEAP_RAM                            
    if((AudioBufCtl.shadow_state==STATUS_STOP) && SBC2Ram!=NULL )        
    {
        //dprintf("shadow_state %x state %x SBCRam %x\r\n",AudioBufCtl.shadow_state,AudioBufCtl.state,SBCRam);
        SBC2_FreeMemory(MEMORY_ALLOCATE_PLAY);
    }
#endif
    if(     AudioBufCtl.shadow_state!=STATUS_PLAYING
        &&  DacAssigner_codecUseDacCh(kSBC2_CH1)>=0
    ){
		GIE_DISABLE();
		DacAssigner_unRegist(kSBC2_CH1);
		GIE_ENABLE();
		//dprintf("Free codec %d hw_ch\r\n",kSBC2_CH1);
    }
	if (PlayEndFlag == BIT_SET) {
		PlayEndFlag = BIT_UNSET;
		CB_SBC2_PlayEnd();
	}
}
#if _SBC2_AUDIO_BUF_X3

void RepeatReSetting_SBC2(U8 fifoSize, U8 size)
{
	AudioBufCtl.process_count = AudioBufCtl.preDecCount;

	if (BufCtl.offset % SBC2_DEC_OUT_SIZE) {
		BufCtl.offset = ((BufCtl.offset + SBC2_DEC_OUT_SIZE)/SBC2_DEC_OUT_SIZE)*SBC2_DEC_OUT_SIZE;
		if (BufCtl.offset > (SBC2_DEC_OUT_SIZE*2)) {
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
	CB_SBC2_WriteDac(fifoSize, tempbuf);	
	BufCtl.offset += (fifoSize - size);
	AudioBufCtl.fifo_count = (fifoSize - size);

	AudioBufCtl.state = STATUS_PLAYING;
    AudioBufCtl.shadow_state = AudioBufCtl.state;
}

#endif
#endif


#if	_SBC2_CH2_PLAY
//------------------------------------------------------------------//
// Service loop
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
void SBC2_CH2_ServiceLoop(void){
     
	if(AudioBufCtl_CH2.shadow_state==STATUS_PLAYING){

        AudioVol_CH2=  (ChVol_CH2+1)*(MixerVol+1); 
        
        if(!ChVol_CH2){
            AudioVol_CH2=0;
        }    
        if(AudioVol_CH2==256){
            AudioVol_CH2=255;
        }    
        AudioVol_CH2=AudioVol_CH2<<7;
#if _AUDIO_VOLUME_CHANGE_SMOOTH
        AUDIO_SetVolumeSmooth(&VolCtrl_CH2,AudioVol_CH2);
#endif   
      
        if((AudioBufCtl_CH2.fifo_count+(SBC2_DEC_OUT_SIZE/2)-AudioBufCtl_CH2.process_count)>=0){
			if(AudioBufCtl_CH2.process_count<AudioBufCtl_CH2.samples)
			{
//				GPIOB->Data &= (~(1<<13));
                DecodeFrame_CH2();
//              	GPIOB->Data |= (1<<13);
            }
        }          
    }    


#if _USE_HEAP_RAM                            
    if((AudioBufCtl_CH2.shadow_state==STATUS_STOP) && SBC2Ram_CH2!=NULL )        
    {
        //dprintf("shadow_state %x state %x SBCRam %x\r\n",AudioBufCtl.shadow_state,AudioBufCtl.state,SBCRam);
        SBC2_CH2_FreeMemory(MEMORY_ALLOCATE_PLAY);
    }
#endif
    if(     AudioBufCtl_CH2.shadow_state!=STATUS_PLAYING
        &&  DacAssigner_codecUseDacCh(kSBC2_CH2)>=0
    ){
		GIE_DISABLE();
		DacAssigner_unRegist(kSBC2_CH2);
		GIE_ENABLE();
		//dprintf("Free codec %d hw_ch\r\n",kSBC2_CH2);
    }
	if (PlayEndFlag_CH2 == BIT_SET) {
		CB_SBC2_CH2_PlayEnd();
		PlayEndFlag_CH2 = BIT_UNSET;
	}

}
#endif
#endif
