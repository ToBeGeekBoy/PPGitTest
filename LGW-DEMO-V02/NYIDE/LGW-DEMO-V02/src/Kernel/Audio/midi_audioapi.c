/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "include.h"
#include "nx1_dac.h"
#include "nx1_spi.h"
#include "nx1_gpio.h"

#if _MIDI_PLAY
static const U8 LibVer[]="V0.14_230810.00";

#if MIDI_STORAGE_TYPE==0	//otp
#include "MidiTab.h"
#endif

#if _MIDI_INST_NOTEON
U8 MIDI_BusyFlag;
volatile U8 NoteOn_MidiVol = 0xFF;//MIDI volume @Instrument NoteOn(0xFF:max)
volatile U8 NoteOn_InstVol = 0xFF;//Instrument volume @Instrument NoteOn(0XFF:max)

U8 NoteOn_Enable_Flag=0;				//NoteOn ENABLE/DISABLE flag	
U16 NoteOn_CH_BusyFlag=0;			//to recongine MIDI/Note_On ,0xFFFF(16 bits)
static struct {
	U8 patch;
	U8 pitch;
}BufOKON;
U8 Midi_OKON_Opt=0;//init disable
static U8 Midi_PlayInit_Opt=0;//0:Song(default), 1:NoteOn
#endif




/* Defines -------------------------------------------------------------------*/
#define AUDIO_OUT_SIZE     MIDI_DEC_OUT_SIZE
#define AUDIO_OUT_COUNT    MIDI_AUDIO_OUT_COUNT
#define AUDIO_BUF_SIZE     MIDI_AUDIO_BUF_SIZE

#define SMOOTH_ENABLE          	1
#define Out32Buf				((S32*)MIDIRam+32)

S_AUDIO_FUNC_IDX s_MIDI_FUNC = {
    .Play           =   MIDI_Play,
    .Pause          =   MIDI_Pause,
    .Resume         =   MIDI_Resume,
    .Stop           =   MIDI_Stop,
    .SetVolume      =   MIDI_SetVolume,
    .GetStatus      =   MIDI_GetStatus,
#if _AUDIO_MIX_CONTROL
    .SetMixCtrl     =   MIDI_SetMixCtrl,
#endif
};
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

/* Global Variables ----------------------------------------------------------*/
#if _AUDIO_MIX_CONTROL
U8 MidiMixCtrl=MIX_CTRL_100;
#endif

/* Static Variables ----------------------------------------------------------*/
static volatile struct AUDIO_BUF_CTL{
#if _MIDI_INST_NOTEON
	S32 tmp_process_count;
#endif	
    S32 process_count;	//decode/encode count
    S32 fifo_count;	    //decode/encode count
    //U16 vol;            //volume
    U8 in_size;         //data frame size
    U8 state;           //channel state
    U8 shadow_state;    //channel shadow state
//    U8 ramp;            //smooth ramp
    U8 index;
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

static U8  PlayEndFlag;

#if _AUDIO_VOLUME_CHANGE_SMOOTH
static volatile VOLCTRL_T    VolCtrl;
#endif

static SPI_TypeDef *SPI;

/* Dynamic Allocate RAM-------------------------------------------------------*/
#if _USE_HEAP_RAM
static U32 *MIDIRam=NULL;
static U16 *AudioBuf=NULL;
#else
static U32 MIDIRam[MIDI_RAM_ALLOCATE_SIZE];
static U16 AudioBuf[AUDIO_BUF_SIZE];
#endif
/* Extern Functions ----------------------------------------------------------*/
extern void SPI_BurstRead(U8 *buf,U32 addr,U16 len);
extern void CB_MIDI_PlayStart(void);
extern void CB_MIDI_PlayEnd(void);
extern void CB_MIDI_InitDac(CH_TypeDef *chx,U16 sample_rate);
extern void CB_MIDI_DacCmd(CH_TypeDef *chx,U8 cmd);
extern void CB_MIDI_WriteDac(U8 size,U16* psrc16);
extern void CB_Midi_GetStartAddr(U32 *index,U32 *addr,U8 mode);

U8 Get_Pitch_C(U8 Clear_Pitch){return 0;}//20181128 for weak function of Library
#pragma weak Get_Pitch = Get_Pitch_C
//------------------------------------------------------------------//
// Allocate MIDI memory
// Parameter: 
//          func: Choose Allocate function
// return value:
//          NONE
//------------------------------------------------------------------// 
#if _USE_HEAP_RAM
static void MIDI_AllocateMemory(U16 func)
{
    if(MIDIRam==NULL && (func&MEMORY_ALLOCATE_PLAY))    // MIDIRam & AudioBuf together
    {
        MIDIRam =  (U32 *)MemAlloc(MIDI_RAM_ALLOCATE_SIZE<<2);
        AudioBuf = (U16 *)MemAlloc(AUDIO_BUF_SIZE<<1);
        //dprintf("MIDI Allocate mem\r\n");
    }
    //else
        //dprintf("MIDI Allocate twice. Ignore...\r\n");
    
}
//------------------------------------------------------------------//
// Free MIDI memory
// Parameter: 
//          func: Choose Allocate function
// return value:
//          NONE
//------------------------------------------------------------------// 
static void MIDI_FreeMemory(U16 func)
{
    if(MIDIRam!=NULL && (func&MEMORY_ALLOCATE_PLAY))    
    {
        MemFree(MIDIRam);
        MIDIRam=NULL;
        MemFree(AudioBuf);
        AudioBuf=NULL;
        //dprintf("MIDI Free Mem\r\n");
    }
    //else
    //{
    //    dprintf("MIDI Already Free\r\n");
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
void MIDI_DacIsr(U8 size){
    U16 temp;
    S16 *psrc16,gain;
    U8 i,len,size2;

#if _MIDI_INST_NOTEON
	if(AudioBufCtl.shadow_state){
#else    
	if(AudioBufCtl.shadow_state==STATUS_PLAYING){
#endif    
  
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
        CB_MIDI_WriteDac(size,(U16 *)psrc16);
        
        BufCtl.offset+=size;

        if(BufCtl.offset==BufCtl.size){
            BufCtl.offset=0;    
        }       
#if !_MIDI_INST_NOTEON
        if(AudioBufCtl.fifo_count>AudioBufCtl.process_count){
            if(MIDIChkEnd()){
                AudioBufCtl.state=STATUS_STOP;
                AudioBufCtl.shadow_state=AudioBufCtl.state;
                PlayEndFlag = BIT_SET;
            }
            //dprintf("%d\r\n",MIDIChkEnd());
            //dprintf("stop m,%d,%d\r\n",AudioBufCtl.fifo_count,AudioBufCtl.process_count);
        }
#endif
    }else{
//        for(i=0;i<size;i++){
//            *pdest32=0;           
//        }   
        CB_MIDI_WriteDac(size,(U16 *)0);
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
	    SmoothCtl.step_sample=MIDI_SAMPLE_RATE*SMOOTH_UP_TIME/1000/SMOOTH_STEP;
    }else if(smooth==SMOOTH_DOWN){
	    SmoothCtl.smooth=SMOOTH_DOWN;
	    SmoothCtl.step_sample=MIDI_SAMPLE_RATE*SMOOTH_DOWN_TIME/1000/SMOOTH_STEP;
    }else{
        SmoothCtl.smooth=SMOOTH_NONE;
    }
}     
#endif
//------------------------------------------------------------------//
// Decode one audio frame
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
static void DecodeFrame(void)
{
    U8 i;
    U16 *audio_buf;
    
    audio_buf=AudioBuf+(AUDIO_OUT_SIZE*AudioBufCtl.index);
    
    AudioBufCtl.index++;
    if(AudioBufCtl.index==AUDIO_OUT_COUNT){
        AudioBufCtl.index=0;    
    }    
    
//    USER_MIDI_ReadData((U8 *)audio_buf,AudioBufCtl.in_size);
    i=0;
    MIDIDecode(&i,(S16 *)audio_buf,1);
    //dprintf("len:%d\r\n",i);
    AudioBufCtl.process_count+=i;
   
    for(i=0;i<AUDIO_OUT_SIZE;i++){
#if _AUDIO_VOLUME_CHANGE_SMOOTH
		Out32Buf[i]=(Out32Buf[i]*AUDIO_GetVolumeSmooth(&VolCtrl))>>Q15_SHIFT;
#else
		Out32Buf[i]=(Out32Buf[i]*AudioVol)>>Q15_SHIFT;
#endif
		if		(Out32Buf[i]> MAX_INT16)	audio_buf[i] = MAX_INT16;
		else if	(Out32Buf[i]< MIN_INT16)	audio_buf[i] = MIN_INT16;
		else								audio_buf[i] = (S16)Out32Buf[i];
    //    audio_buf[i]=((S16)audio_buf[i]*(32767-DAC_DATA_OFFSET))>>Q15_SHIFT;
    }
}    
//------------------------------------------------------------------//
// Start play voice
// Parameter: 
//          index: file index or absolute address
//          mode:OTP_MODE,SPI_MODE,SPI1_MODE
// return value:
//          0:ok
//          others:fail
//------------------------------------------------------------------// 
static U8 _MIDI_Play(U32 index,U8 mode){

    U32 addr;

#if _MIDI_INST_NOTEON
	if(Midi_PlayInit_Opt==0)//0:song
	{	
		//onekey_enable=DISABLE;
		onekey_enable=Midi_OKON_Opt;		
		onekey_press=DISABLE;
		MIDILearn_enable=DISABLE;	

#if _MIDI_INST_RECORD
		U8 tmp = InstNote_Rec_GetStatus();
		if(tmp==STATUS_PAUSE || tmp==STATUS_RECORDING){
			InstNote_Rec_Stop();
			while(InstNote_Rec_GetStatus()){
				AUDIO_ServiceLoop();
			}
		}
#endif//_MIDI_INST_RECORD	
		
		MIDI_BusyFlag=STATUS_STOP;
		if(Inst_MIDI_GetStatus()){
			MIDI_BusyFlag=MIDI_STOP;
			while(Inst_MIDI_GetStatus()){
				AUDIO_ServiceLoop();
			}
		}
		
		if(NoteOn_GetStatus()){
			if(mode==SPI_MODE){
				SPI=P_SPI0;
				CB_Midi_GetStartAddr(&index,&addr,mode);
				MIDIInit(index,addr,MIDIRam,mode);
			}
			else if(mode==OTP_MODE){
#if MIDI_STORAGE_TYPE==0
#ifdef MIDI_EVENTTAB_NUMBER
				if(index>=MIDI_EVENTTAB_NUMBER) return E_AUDIO_INVALID_IDX;
#endif
#endif
				addr=0;
				MIDIInit(index,addr,MIDIRam,mode);
			}
			AudioBufCtl.state=  STATUS_PLAYING;
			AudioBufCtl.shadow_state=AudioBufCtl.state;
			MIDI_BusyFlag=STATUS_PLAYING;
			PlayEndFlag = BIT_SET;
  
			CB_MIDI_PlayStart(); 			
			return 0;
		}
	}
	
#else
	
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
	
#endif//_MIDI_INST_NOTEON
 
#if _USE_HEAP_RAM    
#if _VR_FUNC
    VR_Halt();
#endif
	AUDIO_ServiceLoop();
    MIDI_AllocateMemory(MEMORY_ALLOCATE_PLAY);
#else
	AUDIO_ServiceLoop();	
#endif
    memset((void *)&AudioBufCtl, 0, sizeof(AudioBufCtl));
    
    S8 hw_ch = DacAssigner_codecUseDacCh(kMIDI);
	if(hw_ch<0){
		GIE_DISABLE();
		hw_ch = DacAssigner_Regist(kMIDI,MIDI_SAMPLE_RATE);
		GIE_ENABLE();
		if(hw_ch<0){
			//dprintf("No play Invalid sr\r\n");
			return E_AUDIO_INVALID_SAMPLERATE;
		}
		//dprintf("codec %d reg hw_ch %d\r\n",kMIDI,hw_ch);
	}

    BufCtl.offset=0;
    BufCtl.size=AUDIO_BUF_SIZE;//sizeof(AudioBuf)/2;
    BufCtl.buf=AudioBuf;
    
#if SMOOTH_ENABLE
#if _MIDI_INST_NOTEON
	if(Inst_MIDI_GetStatus()==STATUS_STOP && NoteOn_GetStatus()==STATUS_STOP)
#endif
	{	
		SmoothFunc(SMOOTH_UP);
	}
#endif

	if(mode==SPI_MODE){
		SPI=P_SPI0;
		CB_Midi_GetStartAddr(&index,&addr,mode);
#if _MIDI_INST_NOTEON	
		if(Midi_PlayInit_Opt==0)
#endif
		{
			MIDIInit(index,addr,MIDIRam,mode);			
		}	
	}
#if _SPI1_MODULE && _SPI1_USE_FLASH
	else if(mode==SPI1_MODE){
		SPI=P_SPI1;
		CB_Midi_GetStartAddr(&index,&addr,mode);		
		MIDIInit(index,addr,MIDIRam,mode);								
	}
#endif		
	else if(mode==OTP_MODE){

#if MIDI_STORAGE_TYPE==0 
#ifdef MIDI_EVENTTAB_NUMBER
#if MIDI_EVENTTAB_NUMBER==0
	#error "No MIDI song exist."
#endif
		if(index>=MIDI_EVENTTAB_NUMBER) return E_AUDIO_INVALID_IDX;
#else
	#error "Please update NYIDE."
#endif	
#endif

		addr=0;
#if _MIDI_INST_NOTEON	
		if(Midi_PlayInit_Opt==0)
#endif
		{
			MIDIInit(index,addr,MIDIRam,mode);
		}
	}

#if _MIDI_INST_NOTEON
	MIDIInit_PIANO(index,addr,(S32*)MIDIRam);
	if(!NoteOn_Enable_Flag){	
		Inst_NoteOn_init();
		NoteOn_Enable_Flag = ENABLE;	
	}
#endif

    memset((void *)AudioBuf, 0, AUDIO_BUF_SIZE<<1);
	
    AudioVol=  (ChVol+1)*(MixerVol+1);
    if(!ChVol){
        AudioVol=0;
    }    
    if(AudioVol==256){
        AudioVol=255;
    }
	AudioVol=AudioVol<<7;
#if _AUDIO_VOLUME_CHANGE_SMOOTH
	AUDIO_InitVolumeSmooth(&VolCtrl, MIDI_SAMPLE_RATE, AudioVol);
#endif
    
    DecodeFrame();
	DecodeFrame();
	DecodeFrame();
       
	CB_MIDI_InitDac(pHwCtl[hw_ch].p_dac,MIDI_SAMPLE_RATE);
   

#if _MIDI_INST_NOTEON
	AudioBufCtl.shadow_state=STATUS_PLAYING;
	if(Midi_PlayInit_Opt==0)
	{	
		AudioBufCtl.state= STATUS_PLAYING;
		MIDI_BusyFlag=STATUS_PLAYING;
		PlayEndFlag = BIT_SET;
		CB_MIDI_PlayStart(); 
	}
	Midi_PlayInit_Opt=0;//clear
#else
    AudioBufCtl.state=  STATUS_PLAYING;
    AudioBufCtl.shadow_state=AudioBufCtl.state;
	CB_MIDI_PlayStart(); 
#endif
 
	CB_MIDI_DacCmd(pHwCtl[hw_ch].p_dac,ENABLE);
     
    return 0;
}

//------------------------------------------------------------------//
// Start play voice
// Parameter: 
//          index: file index or absolute address
//          mode:OTP_MODE,SPI_MODE
// return value:
//          0:ok
//          others:fail
//------------------------------------------------------------------// 
U8 MIDI_Play(U32 index,U8 mode){

	U8 _status;

	_status=_MIDI_Play(index,mode);
	
	return _status;
}

//------------------------------------------------------------------//
// Pause playing
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void MIDI_Pause(void){
    if(AudioBufCtl.state==STATUS_PLAYING){

#if _MIDI_INST_NOTEON

		AudioBufCtl.state=STATUS_PAUSE;
		MIDI_BusyFlag=MIDI_STOP;
		
#else
	
#if SMOOTH_ENABLE        
//        if(AudioBufCtl.ramp&RAMP_DOWN){
            AudioBufCtl.state=STATUS_PAUSE;
            SmoothFunc(SMOOTH_DOWN);

//        }else
#endif        
//        {
//            AudioBufCtl.state=STATUS_PAUSE;
//        }    
#endif//_MIDI_INST_NOTEON
    }
} 
//------------------------------------------------------------------//
// Resume playing
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void MIDI_Resume(void){
	if(AudioBufCtl.state==STATUS_PAUSE){
        if(DacAssigner_codecUseDacCh(kMIDI)<0) {
#if _MIDI_INST_NOTEON
#if _USE_HEAP_RAM
			if(MIDIRam!=NULL)
#endif
			{
				memset((void *)AudioBuf, 0, AUDIO_BUF_SIZE<<1);
			}
			AudioBufCtl.tmp_process_count = AudioBufCtl.process_count;	
#endif			
            GIE_DISABLE();
            S8 hw_ch = DacAssigner_Regist(kMIDI,MIDI_SAMPLE_RATE);
            GIE_ENABLE();
            if(hw_ch<0) return;//error
        
            CB_MIDI_InitDac(pHwCtl[hw_ch].p_dac,MIDI_SAMPLE_RATE);
            CB_MIDI_DacCmd(pHwCtl[hw_ch].p_dac,ENABLE);
        }

#if _MIDI_INST_NOTEON

#if SMOOTH_ENABLE		
		if(NoteOn_GetStatus()==STATUS_STOP){
			SmoothFunc(SMOOTH_UP);
		}
#endif  			
		AudioBufCtl.state=STATUS_PLAYING;
		MIDI_BusyFlag=STATUS_PLAYING;
		
#else
	
#if SMOOTH_ENABLE        
//        if(AudioBufCtl.ramp&RAMP_UP){
            SmoothFunc(SMOOTH_UP);
//        }
#endif        
        AudioBufCtl.state=STATUS_PLAYING;
        AudioBufCtl.shadow_state=AudioBufCtl.state;
#endif//_MIDI_INST_NOTEON        
    }          
} 
//------------------------------------------------------------------//
// Stop all MIDI function
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void MIDI_Stop(void){
    if(AudioBufCtl.state==STATUS_PLAYING||AudioBufCtl.state==STATUS_PAUSE){

#if _MIDI_INST_NOTEON

		onekey_enable=DISABLE;
		MIDILearn_enable=DISABLE;
		AudioBufCtl.state=STATUS_STOP;
		MIDI_BusyFlag=MIDI_STOP;
		
#else	
	
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
	//            CB_MIDI_PlayEnd();
	//        }     
#endif//_MIDI_INST_NOTEON	
    }  
} 

//------------------------------------------------------------------//
// Get current status
// Parameter: 
//          NONE
// return value:
//          status:STATUS_PLAYING,STATUS_PAUSE,STATUS_STOP
//------------------------------------------------------------------// 
U8 MIDI_GetStatus(void){
#if _MIDI_INST_NOTEON
	U8 sts;
	
	if(AudioBufCtl.shadow_state && AudioBufCtl.tmp_process_count==AudioBufCtl.process_count) return STATUS_PLAYING;

	if(MIDIChkEnd()){
		return STATUS_STOP;
	}
	else {
		if(onekey_enable)
			sts=OneKey_GetStatus();
		else 
			sts=AudioBufCtl.state;
		
			if		(sts==STATUS_PAUSE && Inst_MIDI_GetStatus()==STATUS_STOP) {
				return STATUS_PAUSE;
			}
			else if	(sts==STATUS_STOP && Inst_MIDI_GetStatus()==STATUS_STOP) {
				return STATUS_STOP;
			}
			else 
				return STATUS_PLAYING;	//song also have no notes, so no need to judge Inst_MIDI_GetStatus API.		
	}	

#else	
    return AudioBufCtl.shadow_state;    
#endif //_MIDI_INST_NOTEON
}

//------------------------------------------------------------------//
// Get current status
// Parameter: 
//          NONE
// return value:
//          status:STATUS_PLAYING,STATUS_PAUSE,STATUS_STOP,STATUS_RECORDING
//------------------------------------------------------------------// 
U8 MIDI_GetStatus_All(void){
#if _MIDI_INST_NOTEON

#if _MIDI_INST_RECORD
	if(InstNote_Rec_GetStatus()!=STATUS_STOP)
		return InstNote_Rec_GetStatus();
#endif
	U8 sts;
	
	if(AudioBufCtl.shadow_state && AudioBufCtl.tmp_process_count==AudioBufCtl.process_count) return STATUS_PLAYING;
	
	if(NoteOn_GetStatus()) return STATUS_PLAYING;

	if(MIDIChkEnd()){
		return STATUS_STOP;
	}
	else {
		if(onekey_enable)
			sts=OneKey_GetStatus();
		else 
			sts=AudioBufCtl.state;
		
			if		(sts==STATUS_PAUSE && Inst_MIDI_GetStatus()==STATUS_STOP) {
				return STATUS_PAUSE;
			}
			else if	(sts==STATUS_STOP && Inst_MIDI_GetStatus()==STATUS_STOP) {
				return STATUS_STOP;
			}
			else 
				return STATUS_PLAYING;	//song also have no notes, so no need to judge Inst_MIDI_GetStatus API.		
	}	

#else	
    return AudioBufCtl.shadow_state;    
#endif //_MIDI_INST_NOTEON
}

//------------------------------------------------------------------//
// Set audio volume
// Parameter: 
//          vol:0~15
// return value:
//          NONE
//------------------------------------------------------------------// 
void MIDI_SetVolume(U8 vol){   
    if(vol>CH_VOL_15){
        vol=CH_VOL_15;
    }    
    ChVol=vol;  
}
//------------------------------------------------------------------//
// Set audio MixCtrl
// Parameter: 
//          mixCtrl:MIX_CTRL_0 / MIX_CTRL_25 / MIX_CTRL_50 / MIX_CTRL_75 / MIX_CTRL_100
// return value:
//          NONE
//------------------------------------------------------------------// 
#if _AUDIO_MIX_CONTROL
void MIDI_SetMixCtrl(U8 mixCtrl){
    if(mixCtrl>MIX_CTRL_100) mixCtrl=MIX_CTRL_100;
    MidiMixCtrl=mixCtrl;
}
#endif
//------------------------------------------------------------------//
// Set MIDI Main channel
// Parameter: 
//          chx : 0x0001 (MIDI_CH1), 0x0003 (MIDI_CH1 & 2)
// return value:
//          NONE
//------------------------------------------------------------------// 
void MIDI_SetMainChannel(U16 chx)
{
	MIDI_MainCh=chx;
}
//------------------------------------------------------------------//
// Codec init
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void MIDI_Init(void){
    ChVol=CH_VOL_15;
#if _AUDIO_MIX_CONTROL
    MIDI_SetMixCtrl(MIX_CTRL_100);
#endif
	MIDI_SetMainChannel(0x0001);
}
//------------------------------------------------------------------//
// Service loop
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
void MIDI_ServiceLoop(void){
#if _MIDI_INST_NOTEON
	
	if(AudioBufCtl.shadow_state){

        AudioVol=  (ChVol+1)*(MixerVol+1); 
        
        if(!ChVol){
            AudioVol=0;
        }    
        if(AudioVol==256){
            AudioVol=255;
        }    
        //MIDISetVolume(vol);
		AudioVol=AudioVol<<7;
#if _AUDIO_VOLUME_CHANGE_SMOOTH
        AUDIO_SetVolumeSmooth(&VolCtrl,AudioVol);
#endif

        if((AudioBufCtl.fifo_count+(AUDIO_BUF_SIZE-AUDIO_OUT_SIZE)-AudioBufCtl.process_count)>=0){
            DecodeFrame();
        }

		if(MIDI_GetStatus_All()==STATUS_STOP){
			AudioBufCtl.shadow_state=STATUS_STOP;
#if _USE_HEAP_RAM 
			if(MIDIRam!=NULL){
				MIDI_FreeMemory(MEMORY_ALLOCATE_PLAY);
			}
#endif			
		}
		if(MIDI_GetStatus()==STATUS_STOP){
			if(PlayEndFlag){
				AudioBufCtl.state=STATUS_STOP;
				PlayEndFlag=BIT_UNSET;
				CB_MIDI_PlayEnd();
			}
		}

	}//AudioBufCtl.shadow_state
	
#else
	
	if(AudioBufCtl.shadow_state==STATUS_PLAYING){
      
        AudioVol=  (ChVol+1)*(MixerVol+1); 
        
        if(!ChVol){
            AudioVol=0;
        }    
        if(AudioVol==256){
            AudioVol=255;
        }    
        //MIDISetVolume(vol);
		AudioVol=AudioVol<<7;
#if _AUDIO_VOLUME_CHANGE_SMOOTH
        AUDIO_SetVolumeSmooth(&VolCtrl,AudioVol);
#endif 

        if((AudioBufCtl.fifo_count+(AUDIO_BUF_SIZE-AUDIO_OUT_SIZE)-AudioBufCtl.process_count)>=0){
            DecodeFrame();
        }    
    }       

#if _USE_HEAP_RAM                            
    if((AudioBufCtl.shadow_state==STATUS_STOP) && MIDIRam!=NULL )           
    {
        MIDI_FreeMemory(MEMORY_ALLOCATE_PLAY);
    }
#endif    
	if (PlayEndFlag == BIT_SET) {
		PlayEndFlag = BIT_UNSET;
		CB_MIDI_PlayEnd();
	}
#endif //_MIDI_INST_NOTEON	

    U8 _sts = MIDI_GetStatus_All();
    if(     DacAssigner_codecUseDacCh(kMIDI)>=0
        &&  !(_sts==STATUS_PLAYING|| _sts==STATUS_RECORDING)
    ){
		GIE_DISABLE();
		DacAssigner_unRegist(kMIDI);
		GIE_ENABLE();
		//dprintf("Free codec %d hw_ch\r\n",kMIDI);
    }
}    





  

#if _MIDI_INST_NOTEON

//------------------------------------------------------------------//
// Module initialization of MIDI_NoteOn
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void INST_NoteOn_INIT(void)
{
	if(AudioBufCtl.shadow_state) {
		if(     DacAssigner_codecUseDacCh(kMIDI)<0
			) {
#if _USE_HEAP_RAM		
				if(MIDIRam!=NULL)
#endif				
				{
					memset((void *)AudioBuf, 0, AUDIO_BUF_SIZE<<1);
				}
				AudioBufCtl.tmp_process_count = AudioBufCtl.process_count;

				GIE_DISABLE();
				S8 hw_ch = DacAssigner_Regist(kMIDI,MIDI_SAMPLE_RATE);
				GIE_ENABLE();
				if(hw_ch<0) return;//error
			
				CB_MIDI_InitDac(pHwCtl[hw_ch].p_dac,MIDI_SAMPLE_RATE);
				CB_MIDI_DacCmd(pHwCtl[hw_ch].p_dac,ENABLE);
			}

		return;
	}

	Midi_PlayInit_Opt=1;//1:NoteOn

#if MIDI_STORAGE_TYPE==1			//SPI mode
	U32 addr, addrBase;
	U16 idx_sentence=0, idx_order;
	U8  tempbuf[SPI_TAB_INFO_ENTRY_SIZE];
	
	if(!NoteOn_Enable_Flag){ 
		addrBase=SpiOffset();		
		idx_sentence=0;
		
		do{ /*** One-time initialization for searching midi information ***/
			SPI_BurstRead(tempbuf,addrBase+SPI_BASE_INFO_SIZE+(SPI_TAB_INFO_ENTRY_SIZE*idx_sentence),SPI_TAB_INFO_ENTRY_SIZE);
			
			if		((tempbuf[3]>>4)==SPI_DATA_TYPE_MIDI)
			{
				_MIDI_Play(idx_sentence,SPI_MODE);
				//dprintf("idx_sentence = %d\n\r",idx_sentence);	
				break;
			}
			else if	((tempbuf[3]>>4)==SPI_DATA_TYPE_SENTENCE)
			{
				addr=addrBase+(tempbuf[0]|(tempbuf[1]<<8)|(tempbuf[2]<<16)|(tempbuf[3]<<24));
				addr&=(~SPI_DATA_TYPE_MASK);
				idx_order=0;
				do{
					SPI_BurstRead(tempbuf, addr+(SPI_TAB_INFO_ENTRY_SIZE*idx_order),SPI_TAB_INFO_ENTRY_SIZE);
					if((tempbuf[3]>>4)==4) //sentence-midi
					{
						addr&=(~SPI_DATA_TYPE_MASK2);
						_MIDI_Play(ADDRESS_MODE|addr,SPI_MODE);
						//dprintf("idx_sentence = %d, idx_order = %d \n\r", idx_sentence, idx_order);
						break;
					}
					idx_order++;
				}while( !((tempbuf[3]>>3)&0x1) ); 	//check sentence end
			}
			
			idx_sentence++;
			if(idx_sentence==65535) return;
			
		}while(!NoteOn_Enable_Flag);
	}
	else{
		_MIDI_Play(idx_sentence,SPI_MODE);	
	}
	
#elif MIDI_STORAGE_TYPE==0			//OTP mode

	_MIDI_Play(0,OTP_MODE);	
#else								//Not supported
#error "The storage type of MIDI isn't supported."	
#endif
}

//------------------------------------------------------------------//
// Stop MIDI of INST_Mode
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void INST_MIDI_STOP(void){
	MIDI_Stop();
} 

//------------------------------------------------------------------//
// one_key_one_note funtion & trigger mode setting
// Parameter: 
//			cmd	: ENABLE/DISABLE (OKON/MIDI mode)
//			trig: 0/1 (Standard/Instrument)  
// return value:
//          NONE
//------------------------------------------------------------------//
void OneKey_Func(U8 cmd, U8 trig)
{
	//if(!AudioBufCtl.shadow_state) return;

	if(cmd==DISABLE)
		OKON_Reset=0;	//clear
		
	//onekey_trig_mode=trig;
	MIDI_OKON.trig_mode=trig;
	onekey_enable=cmd;
	Midi_OKON_Opt=cmd;

    if(     DacAssigner_codecUseDacCh(kMIDI)<0
    ) {
#if _USE_HEAP_RAM		
		if(MIDIRam!=NULL)
#endif		
		{
			memset((void *)AudioBuf, 0, AUDIO_BUF_SIZE<<1);	
		}
		AudioBufCtl.tmp_process_count = AudioBufCtl.process_count;
				
        GIE_DISABLE();
        S8 hw_ch = DacAssigner_Regist(kMIDI,MIDI_SAMPLE_RATE);
        GIE_ENABLE();
        if(hw_ch<0) return;//error
    
        CB_MIDI_InitDac(pHwCtl[hw_ch].p_dac,MIDI_SAMPLE_RATE);
        CB_MIDI_DacCmd(pHwCtl[hw_ch].p_dac,ENABLE);
    }
}
//------------------------------------------------------------------//
// one_key_one_note play
// Parameter: 
//          index: file index or absolute address
//          mode:OTP_MODE,SPI_MODE
// return value:
//          NONE
//------------------------------------------------------------------// 
void OneKey_Play(U32 index,U8 mode)
{
	MIDI_Play(index,mode);
	//onekey_enable=ENABLE;//enable one key one note
	OneKey_Func(ENABLE, OKON_TRIG_STD);
}
//------------------------------------------------------------------//
// OKON Key_press
// Parameter: 
//			NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void OKON_press(void)
{
	if(AudioBufCtl.state==STATUS_PAUSE) return;
	
	if(onekey_enable) {
		onekey_press+=1;

		if(MIDI_OKON.trig_mode==OKON_TRIG_INST)
		{
			InstNote_On(MIDI_OKON.patch, MIDI_OKON.pitch, MIDI_OKON.velocity);
			//dprintf("On_%d,%d,%d\n\r",MIDI_OKON.patch,MIDI_OKON.pitch,MIDI_OKON.velocity);
			BufOKON.patch=MIDI_OKON.patch;
			BufOKON.pitch=MIDI_OKON.pitch;
		}
	
		if(     DacAssigner_codecUseDacCh(kMIDI)<0
		) {
#if _USE_HEAP_RAM		
			if(MIDIRam!=NULL)
#endif
			{
				memset((void *)AudioBuf, 0, AUDIO_BUF_SIZE<<1);
			}
			AudioBufCtl.tmp_process_count = AudioBufCtl.process_count;

			GIE_DISABLE();
			S8 hw_ch = DacAssigner_Regist(kMIDI,MIDI_SAMPLE_RATE);
			GIE_ENABLE();
			if(hw_ch<0) return;//error
		
			CB_MIDI_InitDac(pHwCtl[hw_ch].p_dac,MIDI_SAMPLE_RATE);
			CB_MIDI_DacCmd(pHwCtl[hw_ch].p_dac,ENABLE);
		}
	}
}
//------------------------------------------------------------------//
// OKON Key_release
// Parameter: 
//			NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void OKON_release(void)
{
	if(onekey_enable&&MIDI_OKON.trig_mode==OKON_TRIG_INST)
	{
		InstNote_Off(BufOKON.patch, BufOKON.pitch);
		//dprintf("Off_%d,%d\n\r",BufOKON.patch,BufOKON.pitch);
	}
}

//-------------------------------------Learning Mode---------------------------------------//
//------------------------------------------------------------------//
//Learning_Mode:play
// Parameter: 
//          index: file index or absolute address
//          mode:OTP_MODE,SPI_MODE
// return value:
//          NONE
//------------------------------------------------------------------// 
void MIDI_Learn_Play(U32 index,U8 mode)
{
	MIDI_Play(index,mode);
	MIDILearn_enable=ENABLE;//enable teach_mode	
}
//------------------------------------------------------------------//
// Learning_Mode:Note_Return
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void MIDIL_NoteReturn(void)
{
	MIDI_LearnInput.NoteReturn=MIDIL_FLAG_ENABLE;
}
//------------------------------------------------------------------//
// Learning_Mode:Goto NextNote
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void MIDIL_NextNote(void)
{
	MIDI_LearnInput.NextNote+=1;
}
//------------------------------------------------------------------//
// Learning_Mode:Set Mark_Start
// Parameter: 
//          MarkStart:Start of Group
// return value:
//          NONE
//------------------------------------------------------------------// 
void MIDIL_MStart(U8 MarkStart)
{
	MIDI_LearnInput.MStart=MarkStart;
}
//------------------------------------------------------------------//
// Learning_Mode:Set Mark_Stop
// Parameter: 
//          Mark_StopStop of Group
// return value:
//          NONE
//------------------------------------------------------------------// 
void MIDIL_MStop(U8 MarkStop)
{
	MIDI_LearnInput.MStop=MarkStop;
}
//------------------------------------------------------------------//
// Learning_Mode:Get Current Note(Number of Pitch)
// Parameter: 
//          NONE
// return value:
//          current Note(Number of Pitch)//if==0 ->not been prepared
//------------------------------------------------------------------// 
U8 MIDIL_GetNote(void)
{
	U8 temp=MIDI_LearnOutput.Note;
	MIDI_LearnOutput.NextFlag=MIDIL_FLAG_DISABLE;//clear flag
	return temp;
}
//------------------------------------------------------------------//
// Learning_Mode:Note has(not) been updated
// Parameter: 
//          NONE
// return value:
//          MIDIL_FLAG_ENABLE:updated	;MIDIL_FLAG_DISABLE:not yet
//------------------------------------------------------------------// 
U8 MIDIL_NoteUpdate(void)
{
	return MIDI_LearnOutput.NextFlag;
}
//------------------------------------------------------------------//
// Learning_Mode:Group Check
// Parameter: 
//          NONE
// return value:
//          MIDI_LearnOutput.NextFlag//MIDIL_FLAG_ENABLE->Pitch_Number has been updated
//------------------------------------------------------------------// 
U8 MIDIL_GroupCheck(void)
{
	return MIDI_LearnOutput.NextGroup;
}
//------------------------------------------------------------------//
// Learning_Mode:Group Processed
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void MIDIL_GroupProcessed(void)
{
	MIDI_LearnOutput.NextGroup=MIDIL_GROUP_PROCESSED;
}
//---------------------------------------Learning Mode END----------------------------------------//



//------------------------------------------------------------------//
// Get current NoteOn status
// Parameter: 
//          NONE
// return value:
//          status:STATUS_PLAYING:1,STATUS_STOP:0
//------------------------------------------------------------------// 
U8 NoteOn_GetStatus(void)
{
	extern U8 noteOn_EventPw, noteOn_EventPr;
	return ( (NoteOn_CH_BusyFlag==0 && noteOn_EventPw==noteOn_EventPr)? STATUS_STOP:STATUS_PLAYING );
}

//------------------------------------------------------------------//
// Stop NoteOn
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void NoteOn_Stop(void)
{
	InstNote_AllOff();
}
//------------------------------------------------------------------//
// Get current MIDI status on instrument mode
// Parameter: 
//          NONE
// return value:
//          1:busy, 0:free
//------------------------------------------------------------------// 
U8 Inst_MIDI_GetStatus(void)
{
	return(MIDIChFree_GetStatus());
}
//------------------------------------------------------------------//

#if _MIDI_INST_RECORD
void CB_InstNote_Rec_PreSetting(void)
{	
	if(AudioBufCtl.shadow_state==STATUS_STOP) return;

	MIDI_Stop();
	InstNote_AllOff();
	
	while(Inst_MIDI_GetStatus() || NoteOn_GetStatus() || MIDI_BusyFlag==MIDI_STOP){
		AUDIO_ServiceLoop();
	}
}
#endif

#endif//#if _MIDI_INST_NOTEON


#if MIDI_STORAGE_TYPE==1 //spi_driver for midi library used.

static U8   Spi0CmdBusMode,Spi0AddrBusMode,Spi0DataBusMode;
#if _SPI1_MODULE && _SPI1_USE_FLASH
static U8   Spi1CmdBusMode,Spi1AddrBusMode,Spi1DataBusMode;
#endif

//------------------------------------------------------------------//
// Set SPI bus mode for midi library used.
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void Spi_SetBusMode(void)
{	
#if (_SPI_ACCESS_MODE==SPI_1_1_1_MODE) || (_EF_SERIES && (_SPI_ACCESS_MODE==SPI_1_1_1_MODE_3WIRE))
	Spi0CmdBusMode=SPI_DATA_1; Spi0AddrBusMode=SPI_DATA_1; Spi0DataBusMode=SPI_DATA_1;
#elif _SPI_ACCESS_MODE==SPI_1_2_2_MODE
	Spi0CmdBusMode=SPI_DATA_1; Spi0AddrBusMode=SPI_DATA_2; Spi0DataBusMode=SPI_DATA_2;
#elif _SPI_ACCESS_MODE==SPI_1_1_2_MODE
	Spi0CmdBusMode=SPI_DATA_1; Spi0AddrBusMode=SPI_DATA_1; Spi0DataBusMode=SPI_DATA_2;
#elif _SPI_ACCESS_MODE==SPI_1_4_4_MODE
	Spi0CmdBusMode=SPI_DATA_1; Spi0AddrBusMode=SPI_DATA_4; Spi0DataBusMode=SPI_DATA_4;
#endif

#if _SPI1_MODULE && _SPI1_USE_FLASH
#if _EF_SERIES
#if (_SPI1_ACCESS_MODE==SPI_1_1_1_MODE) || (_SPI1_ACCESS_MODE==SPI_1_1_1_MODE_3WIRE)
	Spi1CmdBusMode=SPI1_DATA_1; Spi1AddrBusMode=SPI1_DATA_1; Spi1DataBusMode=SPI1_DATA_1;
#elif _SPI1_ACCESS_MODE==SPI_1_2_2_MODE 
	Spi1CmdBusMode=SPI1_DATA_1; Spi1AddrBusMode=SPI1_DATA_2; Spi1DataBusMode=SPI1_DATA_2;
#elif _SPI1_ACCESS_MODE==SPI_1_1_2_MODE
	Spi1CmdBusMode=SPI1_DATA_1; Spi1AddrBusMode=SPI1_DATA_1; Spi1DataBusMode=SPI1_DATA_2;
#endif
#else
	Spi1CmdBusMode=SPI_DATA_1; Spi1AddrBusMode=SPI_DATA_1; Spi1DataBusMode=SPI_DATA_1;
#endif
#endif
}
//------------------------------------------------------------------//
// Send SPI command+address for midi library used.
// Parameter: 
//          addr : address
// return value:
//          NONE
//------------------------------------------------------------------// 
static void _Spi_SendCmdAddr(U32 addr)
{
	U32 tmp_addrbus, tmp_addr;
	U8  tmp_cmdbus, flagDummy=0;
	
#if _SPI1_MODULE && _SPI1_USE_FLASH
	if(SPI==P_SPI1){
#if _EF_SERIES
#if (_SPI1_ACCESS_MODE==SPI_1_1_1_MODE) || (_SPI1_ACCESS_MODE==SPI_1_1_1_MODE_3WIRE)
		SPI->Data=SPI_READ_CMD; 
		tmp_addrbus=(C_SPI_Tx_Start |((_SPI1_ADDR_BYTE+0-1)<<SPI_DATA_NUM_OFFSET)| Spi1AddrBusMode); 
#elif _SPI1_ACCESS_MODE==SPI_1_2_2_MODE
		SPI->Data=SPI_2READ_CMD; 
		tmp_addrbus=(C_SPI_Tx_Start |((_SPI1_ADDR_BYTE+1-1)<<SPI_DATA_NUM_OFFSET)| Spi1AddrBusMode); 
#elif _SPI1_ACCESS_MODE==SPI_1_1_2_MODE
		SPI->Data=SPI_DUAL_READ_CMD; 
		tmp_addrbus=(C_SPI_Tx_Start |((_SPI1_ADDR_BYTE+1-1)<<SPI_DATA_NUM_OFFSET)| Spi1AddrBusMode); 
#endif
#else //OTP series
		SPI->Data=SPI_READ_CMD; 
		tmp_addrbus=(C_SPI_Tx_Start |((_SPI1_ADDR_BYTE+0-1)<<SPI_DATA_NUM_OFFSET)| Spi1AddrBusMode); 
#endif	
		tmp_cmdbus=Spi1CmdBusMode;	
#if   _SPI1_ADDR_BYTE==3
		tmp_addr= ((addr&0x0000FF)<<16)|((addr&0x00FF00))|((addr&0xFF0000)>>16);
#elif _SPI1_ADDR_BYTE==4
		tmp_addr= ((addr&0x000000FF)<<24)|((addr&0x0000FF00)<<8)|((addr&0x00FF0000)>>8)|((addr&0xFF000000)>>24);
#else
	#error	"_SPI1_ADDR_BYTE setting error"	
#endif

#if (_SPI1_ADDR_BYTE==3 && _SPI1_ACCESS_MODE==SPI_1_4_4_MODE) || (_SPI1_ADDR_BYTE==4 && (_SPI1_ACCESS_MODE==SPI_1_2_2_MODE || _SPI1_ACCESS_MODE==SPI_1_1_2_MODE))
		flagDummy=1;
#endif		
	}
	else
#endif //_SPI1_MODULE && _SPI1_USE_FLASH
	{
#if (SPI_FIFO_SIZE < 0x8)
#if (_SPI_ACCESS_MODE==SPI_1_1_1_MODE) || (_EF_SERIES && (_SPI_ACCESS_MODE==SPI_1_1_1_MODE_3WIRE))
		SPI->Data=SPI_READ_CMD;
#elif _SPI_ACCESS_MODE==SPI_1_2_2_MODE
		SPI->Data=SPI_2READ_CMD;	
#elif _SPI_ACCESS_MODE==SPI_1_1_2_MODE
		SPI->Data=SPI_DUAL_READ_CMD; 
#elif _SPI_ACCESS_MODE==SPI_1_4_4_MODE
		SPI->Data=SPI_4READ_CMD;
#endif
		tmp_addrbus=(C_SPI_Tx_Start |((_SPI_ADDR_BYTE-1)<<SPI_DATA_NUM_OFFSET)| Spi0AddrBusMode);
#else		
#if (_SPI_ACCESS_MODE==SPI_1_1_1_MODE) || (_EF_SERIES && (_SPI_ACCESS_MODE==SPI_1_1_1_MODE_3WIRE))
		SPI->Data=SPI_READ_CMD;
		tmp_addrbus=(C_SPI_Tx_Start |((_SPI_ADDR_BYTE+0-1)<<SPI_DATA_NUM_OFFSET)| Spi0AddrBusMode); 
#elif _SPI_ACCESS_MODE==SPI_1_2_2_MODE
		SPI->Data=SPI_2READ_CMD;
		tmp_addrbus=(C_SPI_Tx_Start |((_SPI_ADDR_BYTE+1-1)<<SPI_DATA_NUM_OFFSET)| Spi0AddrBusMode); 
#elif _SPI_ACCESS_MODE==SPI_1_1_2_MODE
		SPI->Data=SPI_DUAL_READ_CMD; 
		tmp_addrbus=(C_SPI_Tx_Start |((_SPI_ADDR_BYTE+1-1)<<SPI_DATA_NUM_OFFSET)| Spi0AddrBusMode); 
#elif _SPI_ACCESS_MODE==SPI_1_4_4_MODE
		SPI->Data=SPI_4READ_CMD; 
		tmp_addrbus=(C_SPI_Tx_Start |((_SPI_ADDR_BYTE+3-1)<<SPI_DATA_NUM_OFFSET)| Spi0AddrBusMode); 
#endif
#endif //(SPI_FIFO_SIZE < 0x8)
		tmp_cmdbus=Spi0CmdBusMode;	
#if   _SPI_ADDR_BYTE==3
		tmp_addr= ((addr&0x0000FF)<<16)|((addr&0x00FF00))|((addr&0xFF0000)>>16);
#elif _SPI_ADDR_BYTE==4
		tmp_addr= ((addr&0x000000FF)<<24)|((addr&0x0000FF00)<<8)|((addr&0x00FF0000)>>8)|((addr&0xFF000000)>>24);
#else
	#error	"_SPI_ADDR_BYTE setting error"		
#endif

#if (_SPI_ADDR_BYTE==3 && _SPI_ACCESS_MODE==SPI_1_4_4_MODE) || (_SPI_ADDR_BYTE==4 && (_SPI_ACCESS_MODE==SPI_1_2_2_MODE || _SPI_ACCESS_MODE==SPI_1_1_2_MODE || _SPI_ACCESS_MODE==SPI_1_4_4_MODE))
		flagDummy=1;
#endif
	}
	
	SPI->Ctrl &= (~(C_SPI_FIFO_Length | C_SPI_DataMode));
	SPI->Ctrl |= C_SPI_Tx_Start | tmp_cmdbus;
	do{
	}while(SPI->Ctrl&C_SPI_Tx_Busy);

#if (SPI_FIFO_SIZE < 0x8)
	SPI->Data = tmp_addr;
	SPI->Ctrl &= (~(C_SPI_FIFO_Length | C_SPI_DataMode));
	SPI->Ctrl |=tmp_addrbus;
	do{
    }while(SPI->Ctrl&C_SPI_Tx_Busy);
	
	if(flagDummy) {
		SPI->Data=0;//put dummy data_0
	}
#if (_SPI_ACCESS_MODE==SPI_1_2_2_MODE || _SPI_ACCESS_MODE==SPI_1_1_2_MODE || _SPI_ACCESS_MODE==SPI_1_4_4_MODE)	//process dummy
	SPI->Ctrl &= (~(C_SPI_FIFO_Length | C_SPI_DataMode));
#if (_SPI_ACCESS_MODE==SPI_1_4_4_MODE)	
	SPI->Ctrl |= (C_SPI_Tx_Start |((3-1)<<SPI_DATA_NUM_OFFSET)| Spi0AddrBusMode);		
#else 
	SPI->Ctrl |= (C_SPI_Tx_Start |((1-1)<<SPI_DATA_NUM_OFFSET)| Spi0AddrBusMode);		
#endif
	do{
	}while(SPI->Ctrl&C_SPI_Tx_Busy);
#endif //process dummy

#else
	SPI->Data = tmp_addr;
	if(flagDummy) {
		SPI->Data=0;//put dummy data_0
	}

	SPI->Ctrl &= (~(C_SPI_FIFO_Length | C_SPI_DataMode));
	SPI->Ctrl |=tmp_addrbus;
	do{
	}while(SPI->Ctrl&C_SPI_Tx_Busy);	
#endif	
}
//------------------------------------------------------------------//
// SPI Blocking read for midi library used.
// Parameter: 
//          buf  : data buffer
//          addr : start address
//          size : length to read(bytes)
// return value:
//          NONE
//------------------------------------------------------------------// 
void Spi_ReadDataBlocking(U32 *buf,U32 addr,U8 size)
{
	U8 i,j,k;	
	U8 tmp_databus, tmp_fifo_size;
	
#if _SPI1_MODULE && _SPI1_USE_FLASH	
	if(SPI==P_SPI1)
	{	
		SPI1_CS_PORT->Data&=~(1<<SPI1_CS_PIN);
		tmp_databus=Spi1DataBusMode;
		tmp_fifo_size=SPI1_FIFO_SIZE;
	}
	else
#endif	
	{
		SPI0_CS_PORT->Data&=~(1<<SPI0_CS_PIN);
		tmp_databus=Spi0DataBusMode;
		tmp_fifo_size=SPI_FIFO_SIZE;
	}
	
	_Spi_SendCmdAddr(addr);

	SPI->Ctrl &= (~(C_SPI_DataMode));
    SPI->Ctrl |= tmp_databus;
	
	while(size){
        
        k=size;
        if(size>tmp_fifo_size){
            k=tmp_fifo_size;
        }    
        SPI->Ctrl &= (~C_SPI_FIFO_Length);        
        SPI->Ctrl |= (C_SPI_Rx_Start | ((k-1)<<SPI_DATA_NUM_OFFSET));      
        do{
        }while(SPI->Ctrl&C_SPI_Rx_Busy); 
    
        j=(k+3)>>2;
    
        for(i=0;i<j;i++)
        {
            *buf++=SPI->Data;
        }
        size-=k;
    }	

#if _SPI1_MODULE && _SPI1_USE_FLASH	
	if(SPI==P_SPI1)
		SPI1_CS_PORT->Data|=(1<<SPI1_CS_PIN);
	else
#endif	
	{
		SPI0_CS_PORT->Data|=(1<<SPI0_CS_PIN);
	}	
}
//------------------------------------------------------------------//
// SPI Non-Blocking read setting for midi library used.
// Parameter: 
//          addr : start address
//          size : length to read(bytes)
// return value:
//          NONE
//------------------------------------------------------------------// 
void Spi_ReadDataNonBlocking(U32 addr,U8 size)
{
	U8 tmp_databus;
		
#if _SPI1_MODULE && _SPI1_USE_FLASH
	if(SPI==P_SPI1){
		SPI1_CS_PORT->Data|=(1<<SPI1_CS_PIN);
		SPI1_CS_PORT->Data&=~(1<<SPI1_CS_PIN);
		
		tmp_databus=Spi1DataBusMode;
		if(size>SPI1_FIFO_SIZE)
			size=SPI1_FIFO_SIZE;	
	}
	else
#endif
	{
		SPI0_CS_PORT->Data|=(1<<SPI0_CS_PIN);
		SPI0_CS_PORT->Data&=~(1<<SPI0_CS_PIN);
		
		tmp_databus=Spi0DataBusMode;
		if(size>SPI_FIFO_SIZE)
			size=SPI_FIFO_SIZE;			
	}
	
	_Spi_SendCmdAddr(addr);
	SPI->Ctrl &= (~(C_SPI_FIFO_Length | C_SPI_DataMode));
    SPI->Ctrl |= C_SPI_Rx_Start | tmp_databus | ((size-1)<<SPI_DATA_NUM_OFFSET); 
}
//------------------------------------------------------------------//
// SPI Non-Blocking get data for midi library used.
// Parameter: 
//          buf      : data buffer
//          size     : length to read(bytes)
//			rx_start : continuing to get data
// return value:
//          NONE
//------------------------------------------------------------------// 
void Spi_GetDataFromFifo(U32 *buf,U8 size,U8 rx_start)
{
	U8 i,j,j_tmp;
	
#if _SPI1_MODULE && _SPI1_USE_FLASH
	if(SPI==P_SPI1){
#if   SPI1_FIFO_SIZE==8
		i=(size+7)>>3;
#else
		#error	"SPI1_FIFO_SIZE setting error"
#endif
		if(size>SPI1_FIFO_SIZE)
			j=(SPI1_FIFO_SIZE+3)>>2;
		else
			j=(size+3)>>2;
	}
	else
#endif	
	{
#if   SPI_FIFO_SIZE==4
		i=(size+3)>>2;
#elif SPI_FIFO_SIZE==16
		i=(size+15)>>4;
#else
		#error	"SPI_FIFO_SIZE setting error"
#endif
		if(size>SPI_FIFO_SIZE)
			j=(SPI_FIFO_SIZE+3)>>2;
		else
			j=(size+3)>>2;
	}
	
	j_tmp=j;
	
	do{
		do{			
		}while(SPI->Ctrl&C_SPI_Rx_Start);

		do{
			*buf++=SPI->Data;
		}while(--j);
		
		i--;
		if(i==0){
			break;
		}
		else{
			j=j_tmp;
			SPI->Ctrl|=C_SPI_Rx_Start; 
		}
	}while(1);	

	if(rx_start)
		SPI->Ctrl|=C_SPI_Rx_Start; 		
}
//------------------------------------------------------------------//
// SPI Non-Blocking continuing to get data for midi library used.
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void Spi_ReceiveStart(void)
{
	SPI->Ctrl|=C_SPI_Rx_Start; 
}
//------------------------------------------------------------------//
// SPI reset fifo for midi library used.
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void Spi_FlushFifo(void)
{
	while(SPI->Ctrl&C_SPI_Rx_Start);
	SPI->FIFO_RST|=(C_SPI_FIFO_Reset);
	
#if _SPI1_MODULE && _SPI1_USE_FLASH
	if(SPI==P_SPI1){
		SPI1_CS_PORT->Data|=(1<<SPI1_CS_PIN);
	}
	else
#endif
	{
		SPI0_CS_PORT->Data|=(1<<SPI0_CS_PIN);
	}
}
//------------------------------------------------------------------//
// SPI cs pin disable for midi library used.
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void Spi_CS_Disable(void)
{
#if _SPI1_MODULE && _SPI1_USE_FLASH
	if(SPI==P_SPI1){
		SPI1_CS_PORT->Data|=(1<<SPI1_CS_PIN);
	}
	else
#endif
	{
		SPI0_CS_PORT->Data|=(1<<SPI0_CS_PIN);
	}
}
#endif//#if MIDI_STORAGE_TYPE

#endif