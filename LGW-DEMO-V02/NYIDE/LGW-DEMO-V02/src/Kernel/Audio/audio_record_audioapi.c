#include <string.h>
#include "include.h"
#include "nx1_dac.h"
#include "nx1_adc.h"
#include "nx1_spi.h"
#include "nx1_intc.h"
#include "nx1_timer.h"
#include "debug.h"
#include "nx1_gpio.h"

#if _AUDIO_RECORD

#if _AUDIO_RECORD && _ADPCM_RECORD
#error	" The record function can only enable one from _AUDIO_RECORD or _ADPCM_RECORD"
#endif

#if _AUDIO_RECORD && _ADPCM_PLAY
//#error	" _AUDIO_RECORD function use decode & encode buffer of ADPCM CH1, so please change 'ENABLE' _ADPCM_PLAY to the other"
#endif

static const U8 Signature[]="NX1";

static void WriteHeader(void);

static U8  StorageMode;
static U32 StartAddr,CurAddr;

//#if _AUDIO_MULTI_CHANNEL
//extern S32 DacMixBuf[];
//#else
//extern U16 DacMixBuf[];
//#endif


#if _USE_HEAP_RAM
static U8  *DataBuf=NULL;
static U16 *AudioBuf=NULL;
static U8  *ModeBuf=NULL;
static U8  *EncBuf=NULL;
#else
static U16 AudioBuf[AUDIO_RECORD_ADPCM_BUF_SIZE];
static U8 DataBuf[AUDIO_RECORD_DAC_BUF_SIZE];
static U8 ModeBuf[ADPCM_MODE_SIZE];
static U8 EncBuf[ADPCM_ENC_ALL_SIZE];
#endif

static U32 RecordSize;
static volatile struct AUDIO_BUF_CTL{
    S32 process_count;		//decode/encode count
    S32 fifo_count;	    	//decode/encode count
    U32 samples;	    	//samples
    U32 record_start_addr;	//start address of record
    U32 record_size;		//record size
    U16 sample_rate;    	//sample_rate
    U8 out_size;        	//audio frame size
    U8 in_size;         	//data frame size
    U8 state;           	//channel state
    U8 shadow_state;    	//channel shadow state
    U8 out_count;
    U8 index;
}AudioBufCtl;

static volatile struct{
    U16 *buf;
    U16 size;
    U16 offset;
}BufCtl;

extern void CB_WriteData(U8 mode, U32 *cur_addr,U8 *buf,U16 size);
extern void CB_WriteHeader(U8 mode, U32 *cur_addr,U8 *buf,U16 size);
extern void CB_SetStartAddr(U32 addr,U8 mode,U8 rw,U32 *start_addr);

//------------------------------------------------------------------//
// Audio Record initial
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void AudioRecord_Init(U32 addr,U16 sample_rate,U32 size){

#if _USE_HEAP_RAM
	if(AudioBuf==NULL)
    {
    	AudioBuf = (U16 *)MemAlloc(AUDIO_RECORD_ADPCM_BUF_SIZE<<1);
	}
	if(EncBuf==NULL)
    {
    	EncBuf = MemAlloc(ADPCM_ENC_ALL_SIZE);
	}
	if(ModeBuf==NULL)
    {
    	ModeBuf = MemAlloc(ADPCM_MODE_SIZE);
	}
	if(DataBuf==NULL)
	{
		DataBuf = MemAlloc(AUDIO_RECORD_DAC_BUF_SIZE);
	}
#endif
	memset((void *)&AudioBufCtl, 0, sizeof(AudioBufCtl));

    AudioBufCtl.record_start_addr=addr;
	AudioBufCtl.sample_rate=sample_rate;
	AudioBufCtl.record_size=size;
    AudioBufCtl.out_size=32;
    AudioBufCtl.in_size=21;

	CB_SetStartAddr(AudioBufCtl.record_start_addr|ADDRESS_MODE,SPI_MODE,1,&StartAddr);
    CurAddr=StartAddr+HEADER_LEN;
    AudioBufCtl.samples+= HEADER_LEN;
	StorageMode=SPI_MODE;

    AudioBufCtl.out_count=(AUDIO_RECORD_ADPCM_BUF_SIZE)/AudioBufCtl.out_size;
    BufCtl.offset=0;
    BufCtl.size=AudioBufCtl.out_size*AudioBufCtl.out_count;
    BufCtl.buf=AudioBuf;

    SetADPCMMode(AudioBufCtl.in_size,AudioBufCtl.out_size,ModeBuf,0);       //AudioBufCtl.in_size=21,AudioBufCtl.out_size=32
    ADPCMEncInit(EncBuf);
	RecordSize=AudioBufCtl.record_size;
	AudioBufCtl.state=STATUS_AUDIO_RECORDING;
	AudioBufCtl.shadow_state=STATUS_AUDIO_RECORDING;
}

//------------------------------------------------------------------//
// Audio Record Start
// Parameter:
//          addr:  			start address
//			sample_rate:	sample rate, follow audio play source
//          size: 			record size
// return value:
//          NONE
//------------------------------------------------------------------//
void AudioRecord_Start(U32 addr,U16 sample_rate,U32 size){

	if(AudioBufCtl.shadow_state==STATUS_STOP){
		AudioRecord_Init(addr,sample_rate,size);
	}
}

//------------------------------------------------------------------//
// Audio Record Stop
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void AudioRecord_Stop(void){

	if(AudioBufCtl.shadow_state==STATUS_AUDIO_RECORDING){
		if(AudioBufCtl.samples>HEADER_LEN)
		{
	    	WriteHeader();
	    }
	    AudioBufCtl.state=STATUS_STOP;
	    AudioBufCtl.shadow_state=STATUS_STOP;
#if _USE_HEAP_RAM
        if(AudioBuf!=NULL)
        {
            MemFree(AudioBuf);
            AudioBuf=NULL;
        }

        if(DataBuf!=NULL)
        {
            MemFree(DataBuf);
            DataBuf=NULL;
        }

        if(EncBuf!=NULL)
        {
            MemFree(EncBuf);
            EncBuf=NULL;
        }
		
		if(ModeBuf!=NULL)
        {
            MemFree(ModeBuf);
            ModeBuf=NULL;
        }
#endif
	}
}

//------------------------------------------------------------------//
// Get current status
// Parameter:
//          NONE
// return value:
//          status:STATUS_AUDIO_RECORDING,STATUS_STOP
//------------------------------------------------------------------//
U8 AudioRecord_GetStatus(void){
    return AudioBufCtl.shadow_state;
}

//------------------------------------------------------------------//
// AudioRecord Dac ISR
// Parameter:
//          size: fifo fill size
//           buf: fifo buffer
// return value:
//          NONE
//------------------------------------------------------------------//
void AudioRecord_DacIsr(U8 size, S32 *pdest32){
    S16 *pdst16;
    U8 i;
    S16 temp;

    if((AudioBufCtl.shadow_state==STATUS_AUDIO_RECORDING)&&(AUDIO_GetStatus_All()&STATUS_PLAYING))
    {
        temp=BufCtl.size-BufCtl.offset;
        if(size>temp){
            size=temp;
        }

        AudioBufCtl.fifo_count+=size;
 		pdst16=(S16 *)(BufCtl.buf+BufCtl.offset);

         for(i=0;i<size;i++){
            temp=(*pdest32++);
            *pdst16++=temp;
        }

        BufCtl.offset+=size;

        if(BufCtl.offset==BufCtl.size){
            BufCtl.offset=0;
        }
    }
}


//------------------------------------------------------------------//
// Audio Record Service loop
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void AudioRecord_ServiceLoop(void){

	if((AudioBufCtl.shadow_state==STATUS_AUDIO_RECORDING)&&(AUDIO_GetStatus_All()&STATUS_PLAYING))
	{
		if((AudioBufCtl.fifo_count-AudioBufCtl.process_count)>=(AudioBufCtl.out_size))
		{
			S16 *audio_buf;

			audio_buf=(S16 *)AudioBuf+AudioBufCtl.out_size*AudioBufCtl.index;
		    AudioBufCtl.index++;
		    if(AudioBufCtl.index==AudioBufCtl.out_count){
		        AudioBufCtl.index=0;
	    	}
	    	ADPCMEncode(AudioBufCtl.out_size,(U16 *)audio_buf,DataBuf);
	    	AudioBufCtl.process_count+=AudioBufCtl.out_size;

	    	CB_WriteData(StorageMode,&CurAddr,(U8 *)DataBuf,AudioBufCtl.in_size);

		    AudioBufCtl.samples+=AudioBufCtl.in_size;


		    if((AudioBufCtl.samples+AudioBufCtl.in_size)>=RecordSize)
		    {
		        AudioRecord_Stop();
		        #if _DEBUG_MODE
		    	dprintf("AudioRecord_Stop\r\n");
		    	#endif
	    	}
    	}
	}
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

    pheader->decode_in_size=AudioBufCtl.in_size;
    pheader->decode_out_size=AudioBufCtl.out_size;
    pheader->sample_rate=AudioBufCtl.sample_rate;
    pheader->header_size=HEADER_LEN;
    pheader->codec=CODEC_ADPCM;

    memcpy(pheader->signature,Signature,sizeof(Signature));

    pheader->file_size=AudioBufCtl.samples;
    pheader->samples=AudioBufCtl.process_count;
    pheader->flag |= 0x1;
    CurAddr=StartAddr;
    CB_WriteHeader(StorageMode,&CurAddr,(U8 *)DataBuf,HEADER_LEN);
}

#endif