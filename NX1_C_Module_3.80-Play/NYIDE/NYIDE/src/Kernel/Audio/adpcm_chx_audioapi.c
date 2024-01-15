
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "include.h"
#include "nx1_dac.h"
//#include "debug.h"
//#include "nx1_gpio.h"


#if _ADPCM_CH2_PLAY || _ADPCM_CH3_PLAY || _ADPCM_CH4_PLAY || _ADPCM_CH5_PLAY || _ADPCM_CH6_PLAY || _ADPCM_CH7_PLAY || _ADPCM_CH8_PLAY

/* Defines -------------------------------------------------------------------*/
#define SMOOTH_ENABLE          1
        
#if _ADPCM_CH2_PLAY

#if	SMOOTH_ENABLE 
#define SMOOTH_CH2_ENABLE          1
#endif

#if		_ADPCM_CH2_PLAY_BACK_EQ==ENABLE
	#include "FilterTab.h"
	#if	FILTER_NUMBER<=0//check table size
		#error "CH2 Filter table is not existed, please update NYIDE and create filter file."
	#endif
	#define _ADPCM_CH2_PLAY_BACK_USE_CUSTOMIZE//define EQ customize
#endif

S_AUDIO_FUNC_IDX s_ADPCM_CH2_FUNC = {
    .Play           =   ADPCM_CH2_Play,
    .Pause          =   ADPCM_CH2_Pause,
    .Resume         =   ADPCM_CH2_Resume,
    .Stop           =   ADPCM_CH2_Stop,
    .SetVolume      =   ADPCM_CH2_SetVolume,
    .GetStatus      =   ADPCM_CH2_GetStatus,
#if defined(_ADPCM_CH2_PLAY_BACK_USE_CUSTOMIZE)
    .SetEQFilter    =   ADPCM_CH2_SetEQFilter,
#endif

#if _AUDIO_MIX_CONTROL
    .SetMixCtrl     =   ADPCM_CH2_SetMixCtrl,
#endif
};
#endif

#if _ADPCM_CH3_PLAY

#if	SMOOTH_ENABLE 
#define SMOOTH_CH3_ENABLE          1
#endif

#if		_ADPCM_CH3_PLAY_BACK_EQ==ENABLE
	#include "FilterTab.h"
	#if	FILTER_NUMBER<=0//check table size
		#error "CH3 Filter table is not existed, please update NYIDE and create filter file."
	#endif
	#define _ADPCM_CH3_PLAY_BACK_USE_CUSTOMIZE//define EQ customize
#endif

S_AUDIO_FUNC_IDX s_ADPCM_CH3_FUNC = {
    .Play           =   ADPCM_CH3_Play,
    .Pause          =   ADPCM_CH3_Pause,
    .Resume         =   ADPCM_CH3_Resume,
    .Stop           =   ADPCM_CH3_Stop,
    .SetVolume      =   ADPCM_CH3_SetVolume,
    .GetStatus      =   ADPCM_CH3_GetStatus,
#if defined(_ADPCM_CH3_PLAY_BACK_USE_CUSTOMIZE)
    .SetEQFilter    =   ADPCM_CH3_SetEQFilter,
#endif

#if _AUDIO_MIX_CONTROL
    .SetMixCtrl     =   ADPCM_CH3_SetMixCtrl,
#endif
};
#endif

#if _ADPCM_CH4_PLAY

#if	SMOOTH_ENABLE 
#define SMOOTH_CH4_ENABLE          1
#endif

#if		_ADPCM_CH4_PLAY_BACK_EQ==ENABLE
	#include "FilterTab.h"
	#if	FILTER_NUMBER<=0//check table size
		#error "CH4 Filter table is not existed, please update NYIDE and create filter file."
	#endif
	#define _ADPCM_CH4_PLAY_BACK_USE_CUSTOMIZE//define EQ customize
#endif

S_AUDIO_FUNC_IDX s_ADPCM_CH4_FUNC = {
    .Play           =   ADPCM_CH4_Play,
    .Pause          =   ADPCM_CH4_Pause,
    .Resume         =   ADPCM_CH4_Resume,
    .Stop           =   ADPCM_CH4_Stop,
    .SetVolume      =   ADPCM_CH4_SetVolume,
    .GetStatus      =   ADPCM_CH4_GetStatus,
#if defined(_ADPCM_CH4_PLAY_BACK_USE_CUSTOMIZE)
    .SetEQFilter    =   ADPCM_CH4_SetEQFilter,
#endif

#if _AUDIO_MIX_CONTROL
    .SetMixCtrl     =   ADPCM_CH4_SetMixCtrl,
#endif
};
#endif

#if _ADPCM_CH5_PLAY

#if	SMOOTH_ENABLE 
#define SMOOTH_CH5_ENABLE          1
#endif

#if		_ADPCM_CH5_PLAY_BACK_EQ==ENABLE
	#include "FilterTab.h"
	#if	FILTER_NUMBER<=0//check table size
		#error "CH5 Filter table is not existed, please update NYIDE and create filter file."
	#endif
	#define _ADPCM_CH5_PLAY_BACK_USE_CUSTOMIZE//define EQ customize
#endif

S_AUDIO_FUNC_IDX s_ADPCM_CH5_FUNC = {
    .Play           =   ADPCM_CH5_Play,
    .Pause          =   ADPCM_CH5_Pause,
    .Resume         =   ADPCM_CH5_Resume,
    .Stop           =   ADPCM_CH5_Stop,
    .SetVolume      =   ADPCM_CH5_SetVolume,
    .GetStatus      =   ADPCM_CH5_GetStatus,
#if defined(_ADPCM_CH5_PLAY_BACK_USE_CUSTOMIZE)
    .SetEQFilter    =   ADPCM_CH5_SetEQFilter,
#endif

#if _AUDIO_MIX_CONTROL
    .SetMixCtrl     =   ADPCM_CH5_SetMixCtrl,
#endif
};
#endif

#if _ADPCM_CH6_PLAY

#if	SMOOTH_ENABLE 
#define SMOOTH_CH6_ENABLE          1
#endif

#if		_ADPCM_CH6_PLAY_BACK_EQ==ENABLE
	#include "FilterTab.h"
	#if	FILTER_NUMBER<=0//check table size
		#error "CH6 Filter table is not existed, please update NYIDE and create filter file."
	#endif
	#define _ADPCM_CH6_PLAY_BACK_USE_CUSTOMIZE//define EQ customize
#endif

S_AUDIO_FUNC_IDX s_ADPCM_CH6_FUNC = {
    .Play           =   ADPCM_CH6_Play,
    .Pause          =   ADPCM_CH6_Pause,
    .Resume         =   ADPCM_CH6_Resume,
    .Stop           =   ADPCM_CH6_Stop,
    .SetVolume      =   ADPCM_CH6_SetVolume,
    .GetStatus      =   ADPCM_CH6_GetStatus,
#if defined(_ADPCM_CH6_PLAY_BACK_USE_CUSTOMIZE)
    .SetEQFilter    =   ADPCM_CH6_SetEQFilter,
#endif

#if _AUDIO_MIX_CONTROL
    .SetMixCtrl     =   ADPCM_CH6_SetMixCtrl,
#endif
};
#endif

#if _ADPCM_CH7_PLAY

#if	SMOOTH_ENABLE 
#define SMOOTH_CH7_ENABLE          1
#endif

#if		_ADPCM_CH7_PLAY_BACK_EQ==ENABLE
	#include "FilterTab.h"
	#if	FILTER_NUMBER<=0//check table size
		#error "CH7 Filter table is not existed, please update NYIDE and create filter file."
	#endif
	#define _ADPCM_CH7_PLAY_BACK_USE_CUSTOMIZE//define EQ customize
#endif

S_AUDIO_FUNC_IDX s_ADPCM_CH7_FUNC = {
    .Play           =   ADPCM_CH7_Play,
    .Pause          =   ADPCM_CH7_Pause,
    .Resume         =   ADPCM_CH7_Resume,
    .Stop           =   ADPCM_CH7_Stop,
    .SetVolume      =   ADPCM_CH7_SetVolume,
    .GetStatus      =   ADPCM_CH7_GetStatus,
#if defined(_ADPCM_CH7_PLAY_BACK_USE_CUSTOMIZE)
    .SetEQFilter    =   ADPCM_CH7_SetEQFilter,
#endif

#if _AUDIO_MIX_CONTROL
    .SetMixCtrl     =   ADPCM_CH7_SetMixCtrl,
#endif
};
#endif

#if _ADPCM_CH8_PLAY

#if	SMOOTH_ENABLE 
#define SMOOTH_CH8_ENABLE          1
#endif

#if		_ADPCM_CH8_PLAY_BACK_EQ==ENABLE
	#include "FilterTab.h"
	#if	FILTER_NUMBER<=0//check table size
		#error "CH8 Filter table is not existed, please update NYIDE and create filter file."
	#endif
	#define _ADPCM_CH8_PLAY_BACK_USE_CUSTOMIZE//define EQ customize
#endif

S_AUDIO_FUNC_IDX s_ADPCM_CH8_FUNC = {
    .Play           =   ADPCM_CH8_Play,
    .Pause          =   ADPCM_CH8_Pause,
    .Resume         =   ADPCM_CH8_Resume,
    .Stop           =   ADPCM_CH8_Stop,
    .SetVolume      =   ADPCM_CH8_SetVolume,
    .GetStatus      =   ADPCM_CH8_GetStatus,
#if defined(_ADPCM_CH8_PLAY_BACK_USE_CUSTOMIZE)
    .SetEQFilter    =   ADPCM_CH8_SetEQFilter,
#endif

#if _AUDIO_MIX_CONTROL
    .SetMixCtrl     =   ADPCM_CH8_SetMixCtrl,
#endif
};
#endif

#if (	defined(_ADPCM_CH2_PLAY_BACK_USE_CUSTOMIZE) || defined(_ADPCM_CH3_PLAY_BACK_USE_CUSTOMIZE)\
	 || defined(_ADPCM_CH4_PLAY_BACK_USE_CUSTOMIZE) || defined(_ADPCM_CH5_PLAY_BACK_USE_CUSTOMIZE)\
	 || defined(_ADPCM_CH6_PLAY_BACK_USE_CUSTOMIZE) || defined(_ADPCM_CH7_PLAY_BACK_USE_CUSTOMIZE)\
	 || defined(_ADPCM_CH8_PLAY_BACK_USE_CUSTOMIZE) )
	 	#define _ADPCM_CHX_PLAY_BACK_USE_CUSTOMIZE		ENABLE
#else
		#define _ADPCM_CHX_PLAY_BACK_USE_CUSTOMIZE		DISABLE
#endif


#if (	_ADPCM_CH2_QIO_MODULE || _ADPCM_CH3_QIO_MODULE\
	 || _ADPCM_CH4_QIO_MODULE || _ADPCM_CH5_QIO_MODULE\
	 || _ADPCM_CH6_QIO_MODULE || _ADPCM_CH7_QIO_MODULE\
	 || _ADPCM_CH8_QIO_MODULE )
#define QIO_USE_CH(CH)		(0x20|(CH+1))
#endif

/* Global Variables ----------------------------------------------------------*/
#if _AUDIO_MIX_CONTROL
#if _ADPCM_CH2_PLAY
U8 AdpcmMixCtrl_CH2=MIX_CTRL_100;
#endif
#if _ADPCM_CH3_PLAY
U8 AdpcmMixCtrl_CH3=MIX_CTRL_100;
#endif
#if _ADPCM_CH4_PLAY
U8 AdpcmMixCtrl_CH4=MIX_CTRL_100;
#endif
#if _ADPCM_CH5_PLAY
U8 AdpcmMixCtrl_CH5=MIX_CTRL_100;
#endif
#if _ADPCM_CH6_PLAY
U8 AdpcmMixCtrl_CH6=MIX_CTRL_100;
#endif
#if _ADPCM_CH7_PLAY
U8 AdpcmMixCtrl_CH7=MIX_CTRL_100;
#endif
#if _ADPCM_CH8_PLAY
U8 AdpcmMixCtrl_CH8=MIX_CTRL_100;
#endif
#endif

/* Static Variables ----------------------------------------------------------*/
typedef struct AUDIO_BUF_CTL{
    S32 process_count;	//decode/encode count
    S32 fifo_count;	    //decode/encode count
    U32 samples;	    //samples
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
    U32 sample_count;	//samples
    U32 fifo_count2;	//decode/encode count
#endif       
    U16 sample_rate;    //sample_rate
    U8 out_size;        //audio frame size
    U8 in_size;         //data frame size
    U8 state;           //channel state
    U8 shadow_state;    //channel shadow state
    U8 index;
    U8 upsample;
}AudioBufCtl;

#define AUDIO_BUF_CTL_SIZE		(sizeof(AudioBufCtl))

typedef struct{
    U16 *buf;
    U16 size;
    U16 offset;
    U16 rd_offset;
    U16 wr_offset;
}BufCtl; 

#if SMOOTH_ENABLE
typedef struct Smooth_Ctl{
    S16 count;              //sample count in process
    U8  step_sample;        //sample size for each step
    U8  smooth;             //up or down or none
    U8  state;              //current state
}SmoothCtl;
#endif

typedef struct{
	U32 StartAddr;
	U32 CurAddr;
    U16 AudioVol;  			
	U8  OpMode;
	U8  ChVol;
	U8  StorageMode; 
	U8  PlayEndFlag;  
}StaticVar;


#if _ADPCM_CH2_PLAY
AudioBufCtl  _AudioBufCtl_CH2;
AudioBufCtl* AudioBufCtl_CH2 = &_AudioBufCtl_CH2;

BufCtl	_BufCtl_CH2;
BufCtl* BufCtl_CH2 = &_BufCtl_CH2;

static volatile SmoothCtl    SmoothCtl_CH2;

#if _AUDIO_VOLUME_CHANGE_SMOOTH
static volatile VOLCTRL_T    VolCtrl_CH2;
#endif

#if defined(_ADPCM_CH2_PLAY_BACK_USE_CUSTOMIZE)
EQCtl	_EQFilterCtl_CH2;
EQCtl* EQFilterCtl_CH2 = &_EQFilterCtl_CH2;
#endif

StaticVar	_StaticVar_CH2;
StaticVar* StaticVar_CH2 = &_StaticVar_CH2;
#endif

#if _ADPCM_CH3_PLAY
AudioBufCtl  _AudioBufCtl_CH3;
AudioBufCtl* AudioBufCtl_CH3 = &_AudioBufCtl_CH3;

BufCtl	_BufCtl_CH3;
BufCtl* BufCtl_CH3 = &_BufCtl_CH3;

static volatile SmoothCtl    SmoothCtl_CH3;

#if _AUDIO_VOLUME_CHANGE_SMOOTH
static volatile VOLCTRL_T    VolCtrl_CH3;
#endif

#if defined(_ADPCM_CH3_PLAY_BACK_USE_CUSTOMIZE)
EQCtl	_EQFilterCtl_CH3;
EQCtl* EQFilterCtl_CH3 = &_EQFilterCtl_CH3;
#endif

StaticVar	_StaticVar_CH3;
StaticVar* StaticVar_CH3 = &_StaticVar_CH3;
#endif

#if _ADPCM_CH4_PLAY
AudioBufCtl  _AudioBufCtl_CH4;
AudioBufCtl* AudioBufCtl_CH4 = &_AudioBufCtl_CH4;

BufCtl	_BufCtl_CH4;
BufCtl* BufCtl_CH4 = &_BufCtl_CH4;

static volatile SmoothCtl    SmoothCtl_CH4;

#if _AUDIO_VOLUME_CHANGE_SMOOTH
static volatile VOLCTRL_T    VolCtrl_CH4;
#endif

#if defined(_ADPCM_CH4_PLAY_BACK_USE_CUSTOMIZE)
EQCtl	_EQFilterCtl_CH4;
EQCtl* EQFilterCtl_CH4 = &_EQFilterCtl_CH4;
#endif

StaticVar	_StaticVar_CH4;
StaticVar* StaticVar_CH4 = &_StaticVar_CH4;
#endif

#if _ADPCM_CH5_PLAY
AudioBufCtl  _AudioBufCtl_CH5;
AudioBufCtl* AudioBufCtl_CH5 = &_AudioBufCtl_CH5;

BufCtl	_BufCtl_CH5;
BufCtl* BufCtl_CH5 = &_BufCtl_CH5;

static volatile SmoothCtl    SmoothCtl_CH5;

#if _AUDIO_VOLUME_CHANGE_SMOOTH
static volatile VOLCTRL_T    VolCtrl_CH5;
#endif

#if defined(_ADPCM_CH5_PLAY_BACK_USE_CUSTOMIZE)
EQCtl	_EQFilterCtl_CH5;
EQCtl* EQFilterCtl_CH5 = &_EQFilterCtl_CH5;
#endif

StaticVar	_StaticVar_CH5;
StaticVar* StaticVar_CH5 = &_StaticVar_CH5;
#endif

#if _ADPCM_CH6_PLAY
AudioBufCtl  _AudioBufCtl_CH6;
AudioBufCtl* AudioBufCtl_CH6 = &_AudioBufCtl_CH6;

BufCtl	_BufCtl_CH6;
BufCtl* BufCtl_CH6 = &_BufCtl_CH6;

static volatile SmoothCtl    SmoothCtl_CH6;

#if _AUDIO_VOLUME_CHANGE_SMOOTH
static volatile VOLCTRL_T    VolCtrl_CH6;
#endif

#if defined(_ADPCM_CH6_PLAY_BACK_USE_CUSTOMIZE)
EQCtl	_EQFilterCtl_CH6;
EQCtl* EQFilterCtl_CH6 = &_EQFilterCtl_CH6;
#endif

StaticVar	_StaticVar_CH6;
StaticVar* StaticVar_CH6 = &_StaticVar_CH6;
#endif

#if _ADPCM_CH7_PLAY
AudioBufCtl  _AudioBufCtl_CH7;
AudioBufCtl* AudioBufCtl_CH7 = &_AudioBufCtl_CH7;

BufCtl	_BufCtl_CH7;
BufCtl* BufCtl_CH7 = &_BufCtl_CH7;

static volatile SmoothCtl    SmoothCtl_CH7;

#if _AUDIO_VOLUME_CHANGE_SMOOTH
static volatile VOLCTRL_T    VolCtrl_CH7;
#endif

#if defined(_ADPCM_CH7_PLAY_BACK_USE_CUSTOMIZE)
EQCtl	_EQFilterCtl_CH7;
EQCtl* EQFilterCtl_CH7 = &_EQFilterCtl_CH7;
#endif

StaticVar	_StaticVar_CH7;
StaticVar* StaticVar_CH7 = &_StaticVar_CH7;
#endif

#if _ADPCM_CH8_PLAY
AudioBufCtl  _AudioBufCtl_CH8;
AudioBufCtl* AudioBufCtl_CH8 = &_AudioBufCtl_CH8;

BufCtl	_BufCtl_CH8;
BufCtl* BufCtl_CH8 = &_BufCtl_CH8;

static volatile SmoothCtl    SmoothCtl_CH8;

#if _AUDIO_VOLUME_CHANGE_SMOOTH
static volatile VOLCTRL_T    VolCtrl_CH8;
#endif

#if defined(_ADPCM_CH8_PLAY_BACK_USE_CUSTOMIZE)
EQCtl	_EQFilterCtl_CH8;
EQCtl* EQFilterCtl_CH8 = &_EQFilterCtl_CH8;
#endif

StaticVar	_StaticVar_CH8;
StaticVar* StaticVar_CH8 = &_StaticVar_CH8;
#endif

static const U8 Signature[]="NX1";

#if _ADPCM_CHX_PLAY_BACK_USE_CUSTOMIZE
static U8	u8EQFilterIndex;  
static U8 	AdpcmEqChMask=0;  
#endif

/* Dynamic Allocate RAM-------------------------------------------------------*/
#if _USE_HEAP_RAM
// Temporary use, store buffer address 
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
static U16 *AudioBuf_CHX=NULL;
#if PR_READ_CHX
static U8 *EncDataBuf_CHX=NULL;
#endif
#else
static U8 *EncDataBuf_CHX=NULL;
#endif
#if (_ADPCM_CHX_PLAY_BACK_USE_CUSTOMIZE)
static S32 *EqBuf_CHX=NULL;
#endif

static U8  *ModeBuf_CHX=NULL;
static U8  *DecBuf_CHX=NULL;
//

#if _ADPCM_CH2_PLAY
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
static U16 *AudioBuf_CH2=NULL;
#if PR_READ_CHX
static U8 *EncDataBuf_CH2=NULL;
#endif
#else
static U8  *EncDataBuf_CH2=NULL;
#endif

#if defined(_ADPCM_CH2_PLAY_BACK_USE_CUSTOMIZE)
static S32 *EqBuf_CH2=NULL;
#endif

static U8  *ModeBuf_CH2=NULL;
static U8  *DecBuf_CH2=NULL;
#endif

#if _ADPCM_CH3_PLAY
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
static U16 *AudioBuf_CH3=NULL;
#if PR_READ_CHX
static U8 *EncDataBuf_CH3=NULL;
#endif
#else
static U8  *EncDataBuf_CH3=NULL;
#endif

#if defined(_ADPCM_CH3_PLAY_BACK_USE_CUSTOMIZE)
static S32 *EqBuf_CH3=NULL;
#endif

static U8  *ModeBuf_CH3=NULL;
static U8  *DecBuf_CH3=NULL;
#endif

#if _ADPCM_CH4_PLAY
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
static U16 *AudioBuf_CH4=NULL;
#if PR_READ_CHX
static U8 *EncDataBuf_CH4=NULL;
#endif
#else
static U8  *EncDataBuf_CH4=NULL;
#endif

#if defined(_ADPCM_CH4_PLAY_BACK_USE_CUSTOMIZE)
static S32 *EqBuf_CH4=NULL;
#endif

static U8  *ModeBuf_CH4=NULL;
static U8  *DecBuf_CH4=NULL;
#endif

#if _ADPCM_CH5_PLAY
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
static U16 *AudioBuf_CH5=NULL;
#if PR_READ_CHX
static U8 *EncDataBuf_CH5=NULL;
#endif
#else
static U8  *EncDataBuf_CH5=NULL;
#endif

#if defined(_ADPCM_CH5_PLAY_BACK_USE_CUSTOMIZE)
static S32 *EqBuf_CH5=NULL;
#endif

static U8  *ModeBuf_CH5=NULL;
static U8  *DecBuf_CH5=NULL;
#endif

#if _ADPCM_CH6_PLAY
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
static U16 *AudioBuf_CH6=NULL;
#if PR_READ_CHX
static U8 *EncDataBuf_CH6=NULL;
#endif
#else
static U8  *EncDataBuf_CH6=NULL;
#endif

#if defined(_ADPCM_CH6_PLAY_BACK_USE_CUSTOMIZE)
static S32 *EqBuf_CH6=NULL;
#endif

static U8  *ModeBuf_CH6=NULL;
static U8  *DecBuf_CH6=NULL;
#endif

#if _ADPCM_CH7_PLAY
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
static U16 *AudioBuf_CH7=NULL;
#if PR_READ_CHX
static U8 *EncDataBuf_CH7=NULL;
#endif
#else
static U8  *EncDataBuf_CH7=NULL;
#endif

#if defined(_ADPCM_CH7_PLAY_BACK_USE_CUSTOMIZE)
static S32 *EqBuf_CH7=NULL;
#endif

static U8  *ModeBuf_CH7=NULL;
static U8  *DecBuf_CH7=NULL;
#endif

#if _ADPCM_CH8_PLAY
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
static U16 *AudioBuf_CH8=NULL;
#if PR_READ_CHX
static U8 *EncDataBuf_CH8=NULL;
#endif
#else
static U8  *EncDataBuf_CH8=NULL;
#endif

#if defined(_ADPCM_CH8_PLAY_BACK_USE_CUSTOMIZE)
static S32 *EqBuf_CH8=NULL;
#endif

static U8  *ModeBuf_CH8=NULL;
static U8  *DecBuf_CH8=NULL;
#endif
#else//_USE_HEAP_RAM
#if _ADPCM_CH2_PLAY
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
static U16 AudioBuf_CH2[ADPCM_CHX_AUDIO_BUF_SIZE];
#if PR_READ_CHX
static U8 EncDataBuf_CH2[ADPCM_CHX_DATA_BUF_SIZE];
#endif
#else
static U8 EncDataBuf_CH2[ADPCM_CHX_DATA_BUF_SIZE];
#endif

#if defined(_ADPCM_CH2_PLAY_BACK_USE_CUSTOMIZE)
static S32 EqBuf_CH2[ADPCM_EQ_BUF_SIZE];
#endif

static U8 ModeBuf_CH2[ADPCM_MODE_SIZE];
static U8 DecBuf_CH2[ADPCM_DEC_PARAM_SIZE];
#endif

#if _ADPCM_CH3_PLAY
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
static U16 AudioBuf_CH3[ADPCM_CHX_AUDIO_BUF_SIZE];
#if PR_READ_CHX
static U8 EncDataBuf_CH3[ADPCM_CHX_DATA_BUF_SIZE];
#endif
#else
static U8 EncDataBuf_CH3[ADPCM_CHX_DATA_BUF_SIZE];
#endif

#if defined(_ADPCM_CH3_PLAY_BACK_USE_CUSTOMIZE)
static S32 EqBuf_CH3[ADPCM_EQ_BUF_SIZE];
#endif

static U8 ModeBuf_CH3[ADPCM_MODE_SIZE];
static U8 DecBuf_CH3[ADPCM_DEC_PARAM_SIZE];
#endif

#if _ADPCM_CH4_PLAY
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
static U16 AudioBuf_CH4[ADPCM_CHX_AUDIO_BUF_SIZE];
#if PR_READ_CHX
static U8 EncDataBuf_CH4[ADPCM_CHX_DATA_BUF_SIZE];
#endif
#else
static U8 EncDataBuf_CH4[ADPCM_CHX_DATA_BUF_SIZE];
#endif

#if defined(_ADPCM_CH4_PLAY_BACK_USE_CUSTOMIZE)
static S32 EqBuf_CH4[ADPCM_EQ_BUF_SIZE];
#endif

static U8 ModeBuf_CH4[ADPCM_MODE_SIZE];
static U8 DecBuf_CH4[ADPCM_DEC_PARAM_SIZE];
#endif

#if _ADPCM_CH5_PLAY
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
static U16 AudioBuf_CH5[ADPCM_CHX_AUDIO_BUF_SIZE];
#if PR_READ_CHX
static U8 EncDataBuf_CH5[ADPCM_CHX_DATA_BUF_SIZE];
#endif
#else
static U8 EncDataBuf_CH5[ADPCM_CHX_DATA_BUF_SIZE];
#endif

#if defined(_ADPCM_CH5_PLAY_BACK_USE_CUSTOMIZE)
static S32 EqBuf_CH5[ADPCM_EQ_BUF_SIZE];
#endif

static U8 ModeBuf_CH5[ADPCM_MODE_SIZE];
static U8 DecBuf_CH5[ADPCM_DEC_PARAM_SIZE];
#endif

#if _ADPCM_CH6_PLAY
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
static U16 AudioBuf_CH6[ADPCM_CHX_AUDIO_BUF_SIZE];
#if PR_READ_CHX
static U8 EncDataBuf_CH6[ADPCM_CHX_DATA_BUF_SIZE];
#endif
#else
static U8 EncDataBuf_CH6[ADPCM_CHX_DATA_BUF_SIZE];
#endif

#if defined(_ADPCM_CH6_PLAY_BACK_USE_CUSTOMIZE)
static S32 EqBuf_CH6[ADPCM_EQ_BUF_SIZE];
#endif

static U8 ModeBuf_CH6[ADPCM_MODE_SIZE];
static U8 DecBuf_CH6[ADPCM_DEC_PARAM_SIZE];
#endif

#if _ADPCM_CH7_PLAY
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
static U16 AudioBuf_CH7[ADPCM_CHX_AUDIO_BUF_SIZE];
#if PR_READ_CHX
static U8 EncDataBuf_CH7[ADPCM_CHX_DATA_BUF_SIZE];
#endif
#else
static U8 EncDataBuf_CH7[ADPCM_CHX_DATA_BUF_SIZE];
#endif

#if defined(_ADPCM_CH7_PLAY_BACK_USE_CUSTOMIZE)
static S32 EqBuf_CH7[ADPCM_EQ_BUF_SIZE];
#endif

static U8 ModeBuf_CH7[ADPCM_MODE_SIZE];
static U8 DecBuf_CH7[ADPCM_DEC_PARAM_SIZE];
#endif

#if _ADPCM_CH8_PLAY
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
static U16 AudioBuf_CH8[ADPCM_CHX_AUDIO_BUF_SIZE];
#if PR_READ_CHX
static U8 EncDataBuf_CH8[ADPCM_CHX_DATA_BUF_SIZE];
#endif
#else
static U8 EncDataBuf_CH8[ADPCM_CHX_DATA_BUF_SIZE];
#endif

#if defined(_ADPCM_CH8_PLAY_BACK_USE_CUSTOMIZE)
static S32 EqBuf_CH8[ADPCM_EQ_BUF_SIZE];
#endif

static U8 ModeBuf_CH8[ADPCM_MODE_SIZE];
static U8 DecBuf_CH8[ADPCM_DEC_PARAM_SIZE];
#endif

#endif//_USE_HEAP_RAM

#if _ADPCM_CH2_PLAY
S8 ADPCM1GetByte(void);
#endif
#if _ADPCM_CH3_PLAY
S8 ADPCM2GetByte(void);
#endif
#if _ADPCM_CH4_PLAY
S8 ADPCM3GetByte(void);
#endif
#if _ADPCM_CH5_PLAY
S8 ADPCM4GetByte(void);
#endif
#if _ADPCM_CH6_PLAY
S8 ADPCM5GetByte(void);
#endif
#if _ADPCM_CH7_PLAY
S8 ADPCM6GetByte(void);
#endif
#if _ADPCM_CH8_PLAY
S8 ADPCM7GetByte(void);
#endif

U8  ADPCM_CHX_GetStatus(U8 ch);
U8 _ADPCM_CHX_GetStatus(volatile AudioBufCtl *ABC);
S16 _EQProcess(S16 buf_data,U8 ch);

/* Extern Functions ----------------------------------------------------------*/
extern void CB_ReadData(U8 mode, U32 *cur_addr, U8 *buf,U16 size);
extern U8   CB_SetStartAddr(U32 addr,U8 mode,U8 rw,U32 *start_addr);

#if _ADPCM_CH2_PLAY
extern void CB_ADPCM_CH2_PlayStart(void);
extern void CB_ADPCM_CH2_PlayEnd(void);
extern void CB_ADPCM_CH2_InitDac(CH_TypeDef *chx,U16 sample_rate,U8 upsample);
extern void CB_ADPCM_CH2_DacCmd(CH_TypeDef *chx,U8 cmd);
extern void CB_ADPCM_CH2_WriteDac(U8 size,U16* psrc16);
#endif

#if _ADPCM_CH3_PLAY
extern void CB_ADPCM_CH3_PlayStart(void);
extern void CB_ADPCM_CH3_PlayEnd(void);
extern void CB_ADPCM_CH3_InitDac(CH_TypeDef *chx,U16 sample_rate,U8 upsample);
extern void CB_ADPCM_CH3_DacCmd(CH_TypeDef *chx,U8 cmd);
extern void CB_ADPCM_CH3_WriteDac(U8 size,U16* psrc16);
#endif

#if _ADPCM_CH4_PLAY
extern void CB_ADPCM_CH4_PlayStart(void);
extern void CB_ADPCM_CH4_PlayEnd(void);
extern void CB_ADPCM_CH4_InitDac(CH_TypeDef *chx,U16 sample_rate,U8 upsample);
extern void CB_ADPCM_CH4_DacCmd(CH_TypeDef *chx,U8 cmd);
extern void CB_ADPCM_CH4_WriteDac(U8 size,U16* psrc16);
#endif

#if _ADPCM_CH5_PLAY
extern void CB_ADPCM_CH5_PlayStart(void);
extern void CB_ADPCM_CH5_PlayEnd(void);
extern void CB_ADPCM_CH5_InitDac(CH_TypeDef *chx,U16 sample_rate,U8 upsample);
extern void CB_ADPCM_CH5_DacCmd(CH_TypeDef *chx,U8 cmd);
extern void CB_ADPCM_CH5_WriteDac(U8 size,U16* psrc16);
#endif

#if _ADPCM_CH6_PLAY
extern void CB_ADPCM_CH6_PlayStart(void);
extern void CB_ADPCM_CH6_PlayEnd(void);
extern void CB_ADPCM_CH6_InitDac(CH_TypeDef *chx,U16 sample_rate,U8 upsample);
extern void CB_ADPCM_CH6_DacCmd(CH_TypeDef *chx,U8 cmd);
extern void CB_ADPCM_CH6_WriteDac(U8 size,U16* psrc16);
#endif

#if _ADPCM_CH7_PLAY
extern void CB_ADPCM_CH7_PlayStart(void);
extern void CB_ADPCM_CH7_PlayEnd(void);
extern void CB_ADPCM_CH7_InitDac(CH_TypeDef *chx,U16 sample_rate,U8 upsample);
extern void CB_ADPCM_CH7_DacCmd(CH_TypeDef *chx,U8 cmd);
extern void CB_ADPCM_CH7_WriteDac(U8 size,U16* psrc16);
#endif

#if _ADPCM_CH8_PLAY
extern void CB_ADPCM_CH8_PlayStart(void);
extern void CB_ADPCM_CH8_PlayEnd(void);
extern void CB_ADPCM_CH8_InitDac(CH_TypeDef *chx,U16 sample_rate,U8 upsample);
extern void CB_ADPCM_CH8_DacCmd(CH_TypeDef *chx,U8 cmd);
extern void CB_ADPCM_CH8_WriteDac(U8 size,U16* psrc16);
#endif

#if _USE_HEAP_RAM
//------------------------------------------------------------------//
// Allocate channel memory
// Parameter: 
//			ch: select channel 
// return value:
//          NONE
//------------------------------------------------------------------// 
static void MemAllocateSelect(U8 ch)
{
	switch(ch){
#if _ADPCM_CH2_PLAY	
	case 1:
	#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
		if(AudioBuf_CH2==NULL){
    		AudioBuf_CH2 = (U16 *)MemAlloc(ADPCM_CHX_AUDIO_BUF_SIZE<<1);
    		AudioBuf_CHX = AudioBuf_CH2;
		}
		else
		{
			AudioBuf_CHX = AudioBuf_CH2;
		}
		
		#if PR_READ_CHX
		if(EncDataBuf_CH2==NULL){
    		EncDataBuf_CH2 = MemAlloc(ADPCM_CHX_DATA_BUF_SIZE);
    		EncDataBuf_CHX = EncDataBuf_CH2;
		}
		else
		{
			EncDataBuf_CHX = EncDataBuf_CH2;
		}
		#endif
	#else
		if(EncDataBuf_CH2==NULL){
    		EncDataBuf_CH2 = MemAlloc(ADPCM_CHX_DATA_BUF_SIZE);
    		EncDataBuf_CHX = EncDataBuf_CH2;
		}
		else
		{
			EncDataBuf_CHX = EncDataBuf_CH2;
		}
	#endif	
		if(ModeBuf_CH2==NULL){
    		ModeBuf_CH2 = MemAlloc(ADPCM_MODE_SIZE);
    		ModeBuf_CHX = ModeBuf_CH2;
		}
		else
		{
			ModeBuf_CHX = ModeBuf_CH2;
		}
		if(DecBuf_CH2==NULL){
    		DecBuf_CH2 = MemAlloc(ADPCM_DEC_PARAM_SIZE);
    		DecBuf_CHX = DecBuf_CH2;
		}
		else
		{
			DecBuf_CHX = DecBuf_CH2;
		}
		break;
#endif
#if _ADPCM_CH3_PLAY	
	case 2:
	#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
		if(AudioBuf_CH3==NULL){
    		AudioBuf_CH3 = (U16 *)MemAlloc(ADPCM_CHX_AUDIO_BUF_SIZE<<1);
    		AudioBuf_CHX = AudioBuf_CH3;
		}
		else
		{
			AudioBuf_CHX = AudioBuf_CH3;
		}
		#if PR_READ_CHX
		if(EncDataBuf_CH3==NULL){
    		EncDataBuf_CH3 = MemAlloc(ADPCM_CHX_DATA_BUF_SIZE);
    		EncDataBuf_CHX = EncDataBuf_CH3;
		}
		else
		{
			EncDataBuf_CHX = EncDataBuf_CH3;
		}
		#endif
	#else
		if(EncDataBuf_CH3==NULL){
    		EncDataBuf_CH3 = MemAlloc(ADPCM_CHX_DATA_BUF_SIZE);
    		EncDataBuf_CHX = EncDataBuf_CH3;
		}
		else
		{
			EncDataBuf_CHX = EncDataBuf_CH3;
		}
	#endif	
		if(ModeBuf_CH3==NULL){
    		ModeBuf_CH3 = MemAlloc(ADPCM_MODE_SIZE);
    		ModeBuf_CHX = ModeBuf_CH3;
		}
		else
		{
			ModeBuf_CHX = ModeBuf_CH3;
		}
		if(DecBuf_CH3==NULL){
    		DecBuf_CH3 = MemAlloc(ADPCM_DEC_PARAM_SIZE);
    		DecBuf_CHX = DecBuf_CH3;
		}
		else
		{
			DecBuf_CHX = DecBuf_CH3;
		}
		break;
#endif
#if _ADPCM_CH4_PLAY	
	case 3:
	#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
		if(AudioBuf_CH4==NULL){
    		AudioBuf_CH4 = (U16 *)MemAlloc(ADPCM_CHX_AUDIO_BUF_SIZE<<1);
    		AudioBuf_CHX = AudioBuf_CH4;
		}
		else
		{
			AudioBuf_CHX = AudioBuf_CH4;
		}
		#if PR_READ_CHX
		if(EncDataBuf_CH4==NULL){
    		EncDataBuf_CH4 = MemAlloc(ADPCM_CHX_DATA_BUF_SIZE);
    		EncDataBuf_CHX = EncDataBuf_CH4;
		}
		else
		{
			EncDataBuf_CHX = EncDataBuf_CH4;
		}
		#endif
	#else
		if(EncDataBuf_CH4==NULL){
    		EncDataBuf_CH4 = MemAlloc(ADPCM_CHX_DATA_BUF_SIZE);
    		EncDataBuf_CHX = EncDataBuf_CH4;
    	}
    	else
		{
			EncDataBuf_CHX = EncDataBuf_CH4;
		}
    #endif
   		if(ModeBuf_CH4==NULL){
    		ModeBuf_CH4 = MemAlloc(ADPCM_MODE_SIZE);
    		ModeBuf_CHX = ModeBuf_CH4;
		}
		else
		{
			ModeBuf_CHX = ModeBuf_CH4;
		}
		if(DecBuf_CH4==NULL){
    		DecBuf_CH4 = MemAlloc(ADPCM_DEC_PARAM_SIZE);
    		DecBuf_CHX = DecBuf_CH4;
		}
		else
		{
			DecBuf_CHX = DecBuf_CH4;
		}
		break;
#endif
#if _ADPCM_CH5_PLAY	
	case 4:
	#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
		if(AudioBuf_CH5==NULL){
    		AudioBuf_CH5 = (U16 *)MemAlloc(ADPCM_CHX_AUDIO_BUF_SIZE<<1);
    		AudioBuf_CHX = AudioBuf_CH5;
    	}
    	else
		{
			AudioBuf_CHX = AudioBuf_CH5;
		}
		#if PR_READ_CHX
		if(EncDataBuf_CH5==NULL){
    		EncDataBuf_CH5 = MemAlloc(ADPCM_CHX_DATA_BUF_SIZE);
    		EncDataBuf_CHX = EncDataBuf_CH5;
		}
		else
		{
			EncDataBuf_CHX = EncDataBuf_CH5;
		}
		#endif
    #else
    	if(EncDataBuf_CH5==NULL){
    		EncDataBuf_CH5 = MemAlloc(ADPCM_CHX_DATA_BUF_SIZE);
    		EncDataBuf_CHX = EncDataBuf_CH5;
    	}
    	else
		{
			EncDataBuf_CHX = EncDataBuf_CH5;
		}
    #endif
    	if(ModeBuf_CH5==NULL){
    		ModeBuf_CH5 = MemAlloc(ADPCM_MODE_SIZE);
    		ModeBuf_CHX = ModeBuf_CH5;
		}
		else
		{
			ModeBuf_CHX = ModeBuf_CH5;
		}
		if(DecBuf_CH5==NULL){
    		DecBuf_CH5 = MemAlloc(ADPCM_DEC_PARAM_SIZE);
    		DecBuf_CHX = DecBuf_CH5;
		}
		else
		{
			DecBuf_CHX = DecBuf_CH5;
		}
		break;
#endif
#if _ADPCM_CH6_PLAY	
	case 5:
	#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
		if(AudioBuf_CH6==NULL){
    		AudioBuf_CH6 = (U16 *)MemAlloc(ADPCM_CHX_AUDIO_BUF_SIZE<<1);
    		AudioBuf_CHX = AudioBuf_CH6;
    	}
    	else
		{
			AudioBuf_CHX = AudioBuf_CH6;
		}
		#if PR_READ_CHX
		if(EncDataBuf_CH6==NULL){
    		EncDataBuf_CH6 = MemAlloc(ADPCM_CHX_DATA_BUF_SIZE);
    		EncDataBuf_CHX = EncDataBuf_CH6;
		}
		else
		{
			EncDataBuf_CHX = EncDataBuf_CH6;
		}
		#endif
    #else
    	if(EncDataBuf_CH6==NULL){
    		EncDataBuf_CH6 = MemAlloc(ADPCM_CHX_DATA_BUF_SIZE);
    		EncDataBuf_CHX = EncDataBuf_CH6;
    	}
    	else
		{
			EncDataBuf_CHX = EncDataBuf_CH6;
		}
    #endif
   		if(ModeBuf_CH6==NULL){
    		ModeBuf_CH6 = MemAlloc(ADPCM_MODE_SIZE);
    		ModeBuf_CHX = ModeBuf_CH6;
		}
		else
		{
			ModeBuf_CHX = ModeBuf_CH6;
		}
		if(DecBuf_CH6==NULL){
    		DecBuf_CH6 = MemAlloc(ADPCM_DEC_PARAM_SIZE);
    		DecBuf_CHX = DecBuf_CH6;
		}
		else
		{
			DecBuf_CHX = DecBuf_CH6;
		}
		break;
#endif
#if _ADPCM_CH7_PLAY	
	case 6:
	#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
		if(AudioBuf_CH7==NULL){
    		AudioBuf_CH7 = (U16 *)MemAlloc(ADPCM_CHX_AUDIO_BUF_SIZE<<1);
    		AudioBuf_CHX = AudioBuf_CH7;
    	}
    	else
		{
			AudioBuf_CHX = AudioBuf_CH7;
		}
		#if PR_READ_CHX
		if(EncDataBuf_CH7==NULL){
    		EncDataBuf_CH7 = MemAlloc(ADPCM_CHX_DATA_BUF_SIZE);
    		EncDataBuf_CHX = EncDataBuf_CH7;
		}
		else
		{
			EncDataBuf_CHX = EncDataBuf_CH7;
		}
		#endif
    #else
    	if(EncDataBuf_CH7==NULL){
    		EncDataBuf_CH7 = MemAlloc(ADPCM_CHX_DATA_BUF_SIZE);
    		EncDataBuf_CHX = EncDataBuf_CH7;
    	}
    	else
		{
			EncDataBuf_CHX = EncDataBuf_CH7;
		}
    #endif
    	if(ModeBuf_CH7==NULL){
    		ModeBuf_CH7 = MemAlloc(ADPCM_MODE_SIZE);
    		ModeBuf_CHX = ModeBuf_CH7;
		}
		else
		{
			ModeBuf_CHX = ModeBuf_CH7;
		}
		if(DecBuf_CH7==NULL){
    		DecBuf_CH7 = MemAlloc(ADPCM_DEC_PARAM_SIZE);
    		DecBuf_CHX = DecBuf_CH7;
		}
		else
		{
			DecBuf_CHX = DecBuf_CH7;
		}
		break;
#endif
#if _ADPCM_CH8_PLAY	
	case 7:
	#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
		if(AudioBuf_CH8==NULL){
    		AudioBuf_CH8 = (U16 *)MemAlloc(ADPCM_CHX_AUDIO_BUF_SIZE<<1);
    		AudioBuf_CHX = AudioBuf_CH8;
    	}
    	else
		{
			AudioBuf_CHX = AudioBuf_CH8;
		}
		#if PR_READ_CHX
		if(EncDataBuf_CH8==NULL){
    		EncDataBuf_CH8 = MemAlloc(ADPCM_CHX_DATA_BUF_SIZE);
    		EncDataBuf_CHX = EncDataBuf_CH8;
		}
		else
		{
			EncDataBuf_CHX = EncDataBuf_CH8;
		}
		#endif
    #else
    	if(EncDataBuf_CH8==NULL){
    		EncDataBuf_CH8 = MemAlloc(ADPCM_CHX_DATA_BUF_SIZE);
    		EncDataBuf_CHX = EncDataBuf_CH8;
		}
		else
		{
			EncDataBuf_CHX = EncDataBuf_CH8;
		}
	#endif
		if(ModeBuf_CH8==NULL){
    		ModeBuf_CH8 = MemAlloc(ADPCM_MODE_SIZE);
    		ModeBuf_CHX = ModeBuf_CH8;
		}
		else
		{
			ModeBuf_CHX = ModeBuf_CH8;
		}
		if(DecBuf_CH8==NULL){
    		DecBuf_CH8 = MemAlloc(ADPCM_DEC_PARAM_SIZE);
    		DecBuf_CHX = DecBuf_CH8;	
		}
		else
		{
			DecBuf_CHX = DecBuf_CH8;
		}
		break;
#endif		
	default:
		break;
	}
}
#if _ADPCM_CHX_PLAY_BACK_USE_CUSTOMIZE
//------------------------------------------------------------------//
// Allocate channel of EqBuf memory
// Parameter: 
//			ch: select channel 
// return value:
//          NONE
//------------------------------------------------------------------// 
static void MemAllocateEqBuf(U8 ch)
{
	switch(ch){
#if _ADPCM_CH2_PLAY && (defined(_ADPCM_CH2_PLAY_BACK_USE_CUSTOMIZE))
	case 1:
		if(EqBuf_CH2==NULL)
		{
    		EqBuf_CH2 = (S32 *) MemAlloc(ADPCM_EQ_BUF_SIZE*sizeof(S32));
    		EqBuf_CHX = EqBuf_CH2;
    	}
    	else
    	{
    		EqBuf_CHX = EqBuf_CH2;
    	}
		break;
#endif
#if _ADPCM_CH3_PLAY && (defined(_ADPCM_CH3_PLAY_BACK_USE_CUSTOMIZE))
	case 2:
		if(EqBuf_CH3==NULL)
    	{
    		EqBuf_CH3 = (S32 *) MemAlloc(ADPCM_EQ_BUF_SIZE*sizeof(S32));
    		EqBuf_CHX = EqBuf_CH3;
    	}
    	else
    	{
    		EqBuf_CHX = EqBuf_CH3;
    	}	
		break;	
#endif
#if _ADPCM_CH4_PLAY && (defined(_ADPCM_CH4_PLAY_BACK_USE_CUSTOMIZE))	
	case 3:
		if(EqBuf_CH4==NULL)
		{
    		EqBuf_CH4 = (S32 *) MemAlloc(ADPCM_EQ_BUF_SIZE*sizeof(S32));
    		EqBuf_CHX = EqBuf_CH4;
		}
		else
    	{
    		EqBuf_CHX = EqBuf_CH4;
    	}
		break;
#endif
#if _ADPCM_CH5_PLAY && (defined(_ADPCM_CH5_PLAY_BACK_USE_CUSTOMIZE))	
	case 4:
		if(EqBuf_CH5==NULL)
		{
    		EqBuf_CH5 = (S32 *) MemAlloc(ADPCM_EQ_BUF_SIZE*sizeof(S32));
    		EqBuf_CHX = EqBuf_CH5;
		}
		else
    	{
    		EqBuf_CHX = EqBuf_CH5;
    	}
		break;
#endif
#if _ADPCM_CH6_PLAY && (defined(_ADPCM_CH6_PLAY_BACK_USE_CUSTOMIZE))	
	case 5:
		if(EqBuf_CH6==NULL)
		{
    		EqBuf_CH6 = (S32 *) MemAlloc(ADPCM_EQ_BUF_SIZE*sizeof(S32));
    		EqBuf_CHX = EqBuf_CH6;
		}
		else
    	{
    		EqBuf_CHX = EqBuf_CH6;
    	}
		break;
#endif
#if _ADPCM_CH7_PLAY && (defined(_ADPCM_CH7_PLAY_BACK_USE_CUSTOMIZE))
	case 6:
		if(EqBuf_CH7==NULL)
		{
    		EqBuf_CH7 = (S32 *) MemAlloc(ADPCM_EQ_BUF_SIZE*sizeof(S32));
    		EqBuf_CHX = EqBuf_CH7;
		}
		else
    	{
    		EqBuf_CHX = EqBuf_CH7;
    	}
		break;
#endif
#if _ADPCM_CH8_PLAY && (defined(_ADPCM_CH8_PLAY_BACK_USE_CUSTOMIZE))
	case 7:
		if(EqBuf_CH8==NULL)
		{
    		EqBuf_CH8 = (S32 *) MemAlloc(ADPCM_EQ_BUF_SIZE*sizeof(S32));
    		EqBuf_CHX = EqBuf_CH8;
		}
		else
    	{
    		EqBuf_CHX = EqBuf_CH8;
    	}
		break;
#endif		
	default:
		break;
	}	
}
#endif
//------------------------------------------------------------------//
// Memfree channel memory
// Parameter: 
//			ch: select channel 
// return value:
//          NONE
//------------------------------------------------------------------// 
static void MemFreeSelect(U8 ch)
{
	switch(ch){
#if _ADPCM_CH2_PLAY	
	case 1:
	#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
		if(AudioBuf_CH2!=NULL){
		    MemFree(AudioBuf_CH2);
      		AudioBuf_CH2=NULL;
    		AudioBuf_CHX = AudioBuf_CH2;
		}
		#if PR_READ_CHX
		if(EncDataBuf_CH2!=NULL){
		    MemFree(EncDataBuf_CH2);
      		EncDataBuf_CH2=NULL;
    		EncDataBuf_CHX = EncDataBuf_CH2;
		}
		#endif
	#else
		if(EncDataBuf_CH2!=NULL){
		    MemFree(EncDataBuf_CH2);
      		EncDataBuf_CH2=NULL;
    		EncDataBuf_CHX = EncDataBuf_CH2;
		}
	#endif	
		if(ModeBuf_CH2!=NULL){
		    MemFree(ModeBuf_CH2);
      		ModeBuf_CH2=NULL;
    		ModeBuf_CHX = ModeBuf_CH2;
		}
		if(DecBuf_CH2!=NULL){
		    MemFree(DecBuf_CH2);
      		DecBuf_CH2=NULL;
    		DecBuf_CHX = DecBuf_CH2;
		}
	#if defined(_ADPCM_CH2_PLAY_BACK_USE_CUSTOMIZE)
		if(EqBuf_CH2!=NULL){
    		MemFree(EqBuf_CH2);
        	EqBuf_CH2=NULL;
			EQFilterCtl_CH2->Buf=NULL;
		}			
	#endif
		break;
#endif
#if _ADPCM_CH3_PLAY	
	case 2:
	#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
		if(AudioBuf_CH3!=NULL){
		    MemFree(AudioBuf_CH3);
      		AudioBuf_CH3=NULL;
    		AudioBuf_CHX = AudioBuf_CH3;
		}
		#if PR_READ_CHX
		if(EncDataBuf_CH3!=NULL){
		    MemFree(EncDataBuf_CH3);
      		EncDataBuf_CH3=NULL;
    		EncDataBuf_CHX = EncDataBuf_CH3;
		}
		#endif
	#else
		if(EncDataBuf_CH3!=NULL){
		    MemFree(EncDataBuf_CH3);
      		EncDataBuf_CH3=NULL;
    		EncDataBuf_CHX = EncDataBuf_CH3;
		}
	#endif	
		if(ModeBuf_CH3!=NULL){
		    MemFree(ModeBuf_CH3);
      		ModeBuf_CH3=NULL;
    		ModeBuf_CHX = ModeBuf_CH3;
		}
		if(DecBuf_CH3!=NULL){
		    MemFree(DecBuf_CH3);
      		DecBuf_CH3=NULL;
    		DecBuf_CHX = DecBuf_CH3;
		}
	#if defined(_ADPCM_CH3_PLAY_BACK_USE_CUSTOMIZE)
		if(EqBuf_CH3!=NULL){
    		MemFree(EqBuf_CH3);
        	EqBuf_CH3=NULL;
			EQFilterCtl_CH3->Buf=NULL;
		}			
	#endif
		break;
#endif
#if _ADPCM_CH4_PLAY	
	case 3:
	#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
		if(AudioBuf_CH4!=NULL){
		    MemFree(AudioBuf_CH4);
      		AudioBuf_CH4=NULL;
    		AudioBuf_CHX = AudioBuf_CH4;
		}
		#if PR_READ_CHX
		if(EncDataBuf_CH4!=NULL){
		    MemFree(EncDataBuf_CH4);
      		EncDataBuf_CH4=NULL;
    		EncDataBuf_CHX = EncDataBuf_CH4;
		}
		#endif
	#else
		if(EncDataBuf_CH4!=NULL){
		    MemFree(EncDataBuf_CH4);
      		EncDataBuf_CH4=NULL;
    		EncDataBuf_CHX = EncDataBuf_CH4;
		}
	#endif	
		if(ModeBuf_CH4!=NULL){
		    MemFree(ModeBuf_CH4);
      		ModeBuf_CH4=NULL;
    		ModeBuf_CHX = ModeBuf_CH4;
		}
		if(DecBuf_CH4!=NULL){
		    MemFree(DecBuf_CH4);
      		DecBuf_CH4=NULL;
    		DecBuf_CHX = DecBuf_CH4;
		}
	#if defined(_ADPCM_CH4_PLAY_BACK_USE_CUSTOMIZE)
		if(EqBuf_CH4!=NULL){
    		MemFree(EqBuf_CH4);
        	EqBuf_CH4=NULL;
			EQFilterCtl_CH4->Buf=NULL;
		}			
	#endif
		break;
#endif
#if _ADPCM_CH5_PLAY	
	case 4:
	#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
		if(AudioBuf_CH5!=NULL){
		    MemFree(AudioBuf_CH5);
      		AudioBuf_CH5=NULL;
    		AudioBuf_CHX = AudioBuf_CH5;
		}
		#if PR_READ_CHX
		if(EncDataBuf_CH5!=NULL){
		    MemFree(EncDataBuf_CH5);
      		EncDataBuf_CH5=NULL;
    		EncDataBuf_CHX = EncDataBuf_CH5;
		}
		#endif
	#else
		if(EncDataBuf_CH5!=NULL){
		    MemFree(EncDataBuf_CH5);
      		EncDataBuf_CH5=NULL;
    		EncDataBuf_CHX = EncDataBuf_CH5;
		}
	#endif	
		if(ModeBuf_CH5!=NULL){
		    MemFree(ModeBuf_CH5);
      		ModeBuf_CH5=NULL;
    		ModeBuf_CHX = ModeBuf_CH5;
		}
		if(DecBuf_CH5!=NULL){
		    MemFree(DecBuf_CH5);
      		DecBuf_CH5=NULL;
    		DecBuf_CHX = DecBuf_CH5;
		}
	#if defined(_ADPCM_CH5_PLAY_BACK_USE_CUSTOMIZE)
		if(EqBuf_CH5!=NULL){
    		MemFree(EqBuf_CH5);
        	EqBuf_CH5=NULL;
			EQFilterCtl_CH5->Buf=NULL;
		}			
	#endif
		break;
#endif
#if _ADPCM_CH6_PLAY	
	case 5:
	#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
		if(AudioBuf_CH6!=NULL){
		    MemFree(AudioBuf_CH6);
      		AudioBuf_CH6=NULL;
    		AudioBuf_CHX = AudioBuf_CH6;
		}
		#if PR_READ_CHX
		if(EncDataBuf_CH6!=NULL){
		    MemFree(EncDataBuf_CH6);
      		EncDataBuf_CH6=NULL;
    		EncDataBuf_CHX = EncDataBuf_CH6;
		}
		#endif
	#else
		if(EncDataBuf_CH6!=NULL){
		    MemFree(EncDataBuf_CH6);
      		EncDataBuf_CH6=NULL;
    		EncDataBuf_CHX = EncDataBuf_CH6;
		}
	#endif	
		if(ModeBuf_CH6!=NULL){
		    MemFree(ModeBuf_CH6);
      		ModeBuf_CH6=NULL;
    		ModeBuf_CHX = ModeBuf_CH6;
		}
		if(DecBuf_CH6!=NULL){
		    MemFree(DecBuf_CH6);
      		DecBuf_CH6=NULL;
    		DecBuf_CHX = DecBuf_CH6;
		}
	#if defined(_ADPCM_CH6_PLAY_BACK_USE_CUSTOMIZE)
		if(EqBuf_CH6!=NULL){
    		MemFree(EqBuf_CH6);
        	EqBuf_CH6=NULL;
			EQFilterCtl_CH6->Buf=NULL;
		}			
	#endif
		break;
#endif
#if _ADPCM_CH7_PLAY	
	case 6:
	#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
		if(AudioBuf_CH7!=NULL){
		    MemFree(AudioBuf_CH7);
      		AudioBuf_CH7=NULL;
    		AudioBuf_CHX = AudioBuf_CH7;
		}
		#if PR_READ_CHX
		if(EncDataBuf_CH7!=NULL){
		    MemFree(EncDataBuf_CH7);
      		EncDataBuf_CH7=NULL;
    		EncDataBuf_CHX = EncDataBuf_CH7;
		}
		#endif
	#else
		if(EncDataBuf_CH7!=NULL){
		    MemFree(EncDataBuf_CH7);
      		EncDataBuf_CH7=NULL;
    		EncDataBuf_CHX = EncDataBuf_CH7;
		}
	#endif	
		if(ModeBuf_CH7!=NULL){
		    MemFree(ModeBuf_CH7);
      		ModeBuf_CH7=NULL;
    		ModeBuf_CHX = ModeBuf_CH7;
		}
		if(DecBuf_CH7!=NULL){
		    MemFree(DecBuf_CH7);
      		DecBuf_CH7=NULL;
    		DecBuf_CHX = DecBuf_CH7;
		}
	#if defined(_ADPCM_CH7_PLAY_BACK_USE_CUSTOMIZE)
		if(EqBuf_CH7!=NULL){
    		MemFree(EqBuf_CH7);
        	EqBuf_CH7=NULL;
			EQFilterCtl_CH7->Buf=NULL;
		}			
	#endif
		break;
#endif
#if _ADPCM_CH8_PLAY	
	case 7:
	#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
		if(AudioBuf_CH8!=NULL){
		    MemFree(AudioBuf_CH8);
      		AudioBuf_CH8=NULL;
    		AudioBuf_CHX = AudioBuf_CH8;
		}
		#if PR_READ_CHX
		if(EncDataBuf_CH8!=NULL){
		    MemFree(EncDataBuf_CH8);
      		EncDataBuf_CH8=NULL;
    		EncDataBuf_CHX = EncDataBuf_CH8;
		}
		#endif
	#else
		if(EncDataBuf_CH8!=NULL){
		    MemFree(EncDataBuf_CH8);
      		EncDataBuf_CH8=NULL;
    		EncDataBuf_CHX = EncDataBuf_CH8;
		}
	#endif	
		if(ModeBuf_CH8!=NULL){
		    MemFree(ModeBuf_CH8);
      		ModeBuf_CH8=NULL;
    		ModeBuf_CHX = ModeBuf_CH8;
		}
		if(DecBuf_CH8!=NULL){
		    MemFree(DecBuf_CH8);
      		DecBuf_CH8=NULL;
    		DecBuf_CHX = DecBuf_CH8;
		}
	#if defined(_ADPCM_CH8_PLAY_BACK_USE_CUSTOMIZE)
		if(EqBuf_CH8!=NULL){
    		MemFree(EqBuf_CH8);
        	EqBuf_CH8=NULL;
			EQFilterCtl_CH8->Buf=NULL;
		}			
	#endif
		break;
#endif		
	default:
		break;
	}
}
//------------------------------------------------------------------//
// Allocate ADPCM memory
// Parameter: 
//          func	: Choose Allocate function
//			ch		: select channel 
// return value:
//          NONE
//------------------------------------------------------------------// 
static void ADPCM_AllocateMemory(U16 func,U8 ch)
{
    if(func&MEMORY_ALLOCATE_PLAY)    
    {
    	MemAllocateSelect(ch);
	}
}
//------------------------------------------------------------------//
// Free ADPCM memory
// Parameter: 
//          func	: Choose Allocate function
//			ch		: select channel 
// return value:
//          NONE
//------------------------------------------------------------------// 
static void ADPCM_FreeMemory(U16 func,U8 ch)
{
    if(func&MEMORY_ALLOCATE_PLAY)
    {
    	MemFreeSelect(ch);
    }
}
#endif //_USE_HEAP_RAM end

#if _AUDIO_VOLUME_CHANGE_SMOOTH
//------------------------------------------------------------------//
// AUDIO_SetVolumeSmooth for select ch
// Parameter: 
//			ch	: select channel 
//          vol	: volume of channel
// return value:
//          NONE
//------------------------------------------------------------------//
void _AUDIO_SetVolumeSmooth(U8 ch, U16 vol){
	switch(ch){
	#if _ADPCM_CH2_PLAY
	case 1: // ch2
		AUDIO_SetVolumeSmooth(&VolCtrl_CH2,vol);
		break;
	#endif
	#if _ADPCM_CH3_PLAY
	case 2: // ch3
		AUDIO_SetVolumeSmooth(&VolCtrl_CH3,vol);
		break;	
	#endif
	#if _ADPCM_CH4_PLAY
	case 3: // ch4
		AUDIO_SetVolumeSmooth(&VolCtrl_CH4,vol);	
		break;
	#endif
	#if _ADPCM_CH5_PLAY
	case 4: // ch5
		AUDIO_SetVolumeSmooth(&VolCtrl_CH5,vol);
		break;
	#endif
	#if _ADPCM_CH6_PLAY
	case 5: // ch6
		AUDIO_SetVolumeSmooth(&VolCtrl_CH6,vol);
		break;
	#endif
	#if _ADPCM_CH7_PLAY
	case 6: // ch7
		AUDIO_SetVolumeSmooth(&VolCtrl_CH7,vol);	
		break;
	#endif
	#if _ADPCM_CH8_PLAY
	case 7: // ch8
		AUDIO_SetVolumeSmooth(&VolCtrl_CH8,vol);
		break;
	#endif
	default:
		break;
	}
}

//------------------------------------------------------------------//
// AUDIO_GetVolumeSmooth for select ch
// Parameter: 
//			ch: select channel 
// return value:
//          volume of channel
//------------------------------------------------------------------//
S16 _AUDIO_GetVolumeSmooth(U8 ch){
	switch(ch){
	#if _ADPCM_CH2_PLAY
	case 1: // ch2
		return AUDIO_GetVolumeSmooth(&VolCtrl_CH2);
		break;
	#endif
	#if _ADPCM_CH3_PLAY
	case 2: // ch3
		return AUDIO_GetVolumeSmooth(&VolCtrl_CH3);
		break;	
	#endif
	#if _ADPCM_CH4_PLAY
	case 3: // ch4
		return AUDIO_GetVolumeSmooth(&VolCtrl_CH4);	
		break;
	#endif
	#if _ADPCM_CH5_PLAY
	case 4: // ch5
		return AUDIO_GetVolumeSmooth(&VolCtrl_CH5);
		break;
	#endif
	#if _ADPCM_CH6_PLAY
	case 5: // ch6
		return AUDIO_GetVolumeSmooth(&VolCtrl_CH6);
		break;
	#endif
	#if _ADPCM_CH7_PLAY
	case 6: // ch7
		return AUDIO_GetVolumeSmooth(&VolCtrl_CH7);	
		break;
	#endif
	#if _ADPCM_CH8_PLAY
	case 7: // ch8
		return AUDIO_GetVolumeSmooth(&VolCtrl_CH8);
		break;
	#endif
	default:
		return 0;
		break;
	}
}

//------------------------------------------------------------------//
// AUDIO_GetVolumeSmoothINT for select ch
// Parameter: 
//			ch: select channel 
// return value:
//          volume of channel
//------------------------------------------------------------------//
S16 _AUDIO_GetVolumeSmoothINT(U8 ch){
	switch(ch){
	#if _ADPCM_CH2_PLAY
	case 1: // ch2
		return AUDIO_GetVolumeSmoothINT(&VolCtrl_CH2);
		break;
	#endif
	#if _ADPCM_CH3_PLAY
	case 2: // ch3
		return AUDIO_GetVolumeSmoothINT(&VolCtrl_CH3);
		break;	
	#endif
	#if _ADPCM_CH4_PLAY
	case 3: // ch4
		return AUDIO_GetVolumeSmoothINT(&VolCtrl_CH4);	
		break;
	#endif
	#if _ADPCM_CH5_PLAY
	case 4: // ch5
		return AUDIO_GetVolumeSmoothINT(&VolCtrl_CH5);
		break;
	#endif
	#if _ADPCM_CH6_PLAY
	case 5: // ch6
		return AUDIO_GetVolumeSmoothINT(&VolCtrl_CH6);
		break;
	#endif
	#if _ADPCM_CH7_PLAY
	case 6: // ch7
		return AUDIO_GetVolumeSmoothINT(&VolCtrl_CH7);	
		break;
	#endif
	#if _ADPCM_CH8_PLAY
	case 7: // ch8
		return AUDIO_GetVolumeSmoothINT(&VolCtrl_CH8);
		break;
	#endif
	default:
		return 0;
		break;
	}
}

//------------------------------------------------------------------//
// AUDIO_GetVolumeSmoothINT for select ch
// Parameter: 
//          sr			: sample rate
//			audiovol	: audio volume fo channel
//			ch			: select channel 
// return value:
//          NONE
//------------------------------------------------------------------//
void _AUDIO_InitVolumeSmooth(U16 sr, U16 audiovol, U8 ch){
	switch(ch){
	#if _ADPCM_CH2_PLAY
	case 1: // ch2
		AUDIO_InitVolumeSmooth(&VolCtrl_CH2,sr,audiovol);
		break;
	#endif
	#if _ADPCM_CH3_PLAY
	case 2: // ch3
		AUDIO_InitVolumeSmooth(&VolCtrl_CH3,sr,audiovol);
		break;	
	#endif
	#if _ADPCM_CH4_PLAY
	case 3: // ch4
		AUDIO_InitVolumeSmooth(&VolCtrl_CH4,sr,audiovol);
		break;
	#endif
	#if _ADPCM_CH5_PLAY
	case 4: // ch5
		AUDIO_InitVolumeSmooth(&VolCtrl_CH5,sr,audiovol);
		break;
	#endif
	#if _ADPCM_CH6_PLAY
	case 5: // ch6
		AUDIO_InitVolumeSmooth(&VolCtrl_CH6,sr,audiovol);
		break;
	#endif
	#if _ADPCM_CH7_PLAY
	case 6: // ch7
		AUDIO_InitVolumeSmooth(&VolCtrl_CH7,sr,audiovol);	
		break;
	#endif
	#if _ADPCM_CH8_PLAY
	case 7: // ch8
		AUDIO_InitVolumeSmooth(&VolCtrl_CH8,sr,audiovol);
		break;
	#endif
	default:
		break;
	}
}
#endif
//------------------------------------------------------------------//
// ADPCMDecInit for select ch
// Parameter: 
//          *buf	: DecBuf of channel
//			ch	    : select channel 
// return value:
//          NONE
//------------------------------------------------------------------//
void _ADPCMDecInit(U8 *buf,U8 ch){
	switch(ch){
	#if _ADPCM_CH2_PLAY
	case 1: // ch2
		ADPCMDecInit(buf, ADPCM1GetByte, ch);	
		break;
	#endif
	#if _ADPCM_CH3_PLAY
	case 2: // ch3
		ADPCMDecInit(buf, ADPCM2GetByte, ch);	
		break;		
	#endif
	#if _ADPCM_CH4_PLAY
	case 3: // ch4
		ADPCMDecInit(buf, ADPCM3GetByte, ch);	
		break;
	#endif
	#if _ADPCM_CH5_PLAY
	case 4: // ch5
		ADPCMDecInit(buf, ADPCM4GetByte, ch);	
		break;
	#endif
	#if _ADPCM_CH6_PLAY
	case 5: // ch6
		ADPCMDecInit(buf, ADPCM5GetByte, ch);	
		break;
	#endif
	#if _ADPCM_CH7_PLAY
	case 6: // ch7
		ADPCMDecInit(buf, ADPCM6GetByte, ch);	
		break;
	#endif
	#if _ADPCM_CH8_PLAY
	case 7: // ch8
		ADPCMDecInit(buf, ADPCM7GetByte, ch);	
		break;
	#endif
	default:
		break;
	}
}

//------------------------------------------------------------------//
// _CB_ADPCM_InitDac for select ch
// Parameter: 
//          hw_ch         : DAC_CH0, DAC_CH1
//          sample_rate   : sample rate
//		    upsample	  : upsample flag
//			ch			  : select channel 		
// return value:
//          data
//------------------------------------------------------------------//
void CB_ADPCM_CHX_InitDac(S8 hw_ch,U16 sample_rate,U8 upsample,U8 ch){
	switch(ch){
	#if _ADPCM_CH2_PLAY
	case 1: // ch2
		CB_ADPCM_CH2_InitDac(pHwCtl[hw_ch].p_dac,sample_rate,upsample);
		break;
	#endif
	#if _ADPCM_CH3_PLAY
	case 2: // ch3
		CB_ADPCM_CH3_InitDac(pHwCtl[hw_ch].p_dac,sample_rate,upsample);
		break;
	#endif
	#if _ADPCM_CH4_PLAY
	case 3: // ch4
		CB_ADPCM_CH4_InitDac(pHwCtl[hw_ch].p_dac,sample_rate,upsample);
		break;
	#endif
	#if _ADPCM_CH5_PLAY
	case 4: // ch5
		CB_ADPCM_CH5_InitDac(pHwCtl[hw_ch].p_dac,sample_rate,upsample);
		break;
	#endif
	#if _ADPCM_CH6_PLAY
	case 5: // ch6
		CB_ADPCM_CH6_InitDac(pHwCtl[hw_ch].p_dac,sample_rate,upsample);
		break;
	#endif
	#if _ADPCM_CH7_PLAY
	case 6: // ch7
		CB_ADPCM_CH7_InitDac(pHwCtl[hw_ch].p_dac,sample_rate,upsample);
		break;
	#endif
	#if _ADPCM_CH8_PLAY
	case 7: // ch8
		CB_ADPCM_CH8_InitDac(pHwCtl[hw_ch].p_dac,sample_rate,upsample);
		break;
	#endif
	default:
		break;
	}
}

//------------------------------------------------------------------//
// _CB_ADPCM_InitDac for select ch
// Parameter: 
//          hw_ch         : DAC_CH0, DAC_CH1
//			ch			  : select channel 
//          cmd           : ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------//
void CB_ADPCM_CHX_DacCmd(S8 hw_ch,U8 ch,U8 cmd){
	switch(ch){
	#if _ADPCM_CH2_PLAY
	case 1: // ch2
		CB_ADPCM_CH2_DacCmd(pHwCtl[hw_ch].p_dac,cmd);
		break;
	#endif
	#if _ADPCM_CH3_PLAY
	case 2: // ch3
		CB_ADPCM_CH3_DacCmd(pHwCtl[hw_ch].p_dac,cmd);
		break;
	#endif
	#if _ADPCM_CH4_PLAY
	case 3: // ch4
		CB_ADPCM_CH4_DacCmd(pHwCtl[hw_ch].p_dac,cmd);
		break;
	#endif
	#if _ADPCM_CH5_PLAY
	case 4: // ch5
		CB_ADPCM_CH5_DacCmd(pHwCtl[hw_ch].p_dac,cmd);
		break;
	#endif
	#if _ADPCM_CH6_PLAY
	case 5: // ch6
		CB_ADPCM_CH6_DacCmd(pHwCtl[hw_ch].p_dac,cmd);
		break;
	#endif
	#if _ADPCM_CH7_PLAY
	case 6: // ch7
		CB_ADPCM_CH7_DacCmd(pHwCtl[hw_ch].p_dac,cmd);
		break;
	#endif
	#if _ADPCM_CH8_PLAY
	case 7: // ch8
		CB_ADPCM_CH8_DacCmd(pHwCtl[hw_ch].p_dac,cmd);
		break;
	#endif
	default:
		break;
	}
}

//------------------------------------------------------------------//
// CB_ADPCM_WriteDac for select ch
// Parameter: 
//          size	: size to write
//          psrc16	: source pointer
//			ch	    : select channel 
// return value:
//          NONE
//------------------------------------------------------------------//
void CB_ADPCM_CHX_WriteDac(U8 size,U16 *psrc16,U8 ch)
{
	switch(ch){
	#if _ADPCM_CH2_PLAY
	case 1: // ch2
		CB_ADPCM_CH2_WriteDac(size,psrc16);
		break;
	#endif
	#if _ADPCM_CH3_PLAY
	case 2: // ch3
		CB_ADPCM_CH3_WriteDac(size,psrc16);
		break;
	#endif
	#if _ADPCM_CH4_PLAY
	case 3: // ch4
		CB_ADPCM_CH4_WriteDac(size,psrc16);
		break;
	#endif
	#if _ADPCM_CH5_PLAY
	case 4: // ch5
		CB_ADPCM_CH5_WriteDac(size,psrc16);
		break;
	#endif
	#if _ADPCM_CH6_PLAY
	case 5: // ch6
		CB_ADPCM_CH6_WriteDac(size,psrc16);
		break;
	#endif
	#if _ADPCM_CH7_PLAY
	case 6: // ch7
		CB_ADPCM_CH7_WriteDac(size,psrc16);
		break;
	#endif
	#if _ADPCM_CH8_PLAY
	case 7: // ch8
		CB_ADPCM_CH8_WriteDac(size,psrc16);
		break;
	#endif
	default:
		break;
	}	
}

//------------------------------------------------------------------//
// CB_ADPCM_PlayStart for select ch
// Parameter: 
//			ch			  : select channel 
// return value:
//          NONE
//------------------------------------------------------------------//
void CB_ADPCM_CHX_PlayStart(U8 ch){
	switch(ch){
	#if _ADPCM_CH2_PLAY
	case 1: // ch2
		CB_ADPCM_CH2_PlayStart();	
		break;
	#endif
	#if _ADPCM_CH3_PLAY
	case 2: // ch3
		CB_ADPCM_CH3_PlayStart();	
		break;	
	#endif
	#if _ADPCM_CH4_PLAY
	case 3: // ch4
		CB_ADPCM_CH4_PlayStart();	
		break;
	#endif
	#if _ADPCM_CH5_PLAY
	case 4: // ch5
		CB_ADPCM_CH5_PlayStart();	
		break;
	#endif
	#if _ADPCM_CH6_PLAY
	case 5: // ch6
		CB_ADPCM_CH6_PlayStart();	
		break;
	#endif
	#if _ADPCM_CH7_PLAY
	case 6: // ch7
		CB_ADPCM_CH7_PlayStart();	
		break;
	#endif
	#if _ADPCM_CH8_PLAY
	case 7: // ch8
		CB_ADPCM_CH8_PlayStart();	
		break;
	#endif
	default:
		break;
	}
}

//------------------------------------------------------------------//
// CB_ADPCM_PlayEnd for select ch
// Parameter: 
//			ch			  : select channel 
// return value:
//          NONE
//------------------------------------------------------------------//
void CB_ADPCM_CHX_PlayEnd(U8 ch){
	switch(ch){
	#if _ADPCM_CH2_PLAY
	case 1: // ch2
		CB_ADPCM_CH2_PlayEnd();	
		break;
	#endif
	#if _ADPCM_CH3_PLAY
	case 2: // ch3
		CB_ADPCM_CH3_PlayEnd();	
		break;	
	#endif
	#if _ADPCM_CH4_PLAY
	case 3: // ch4
		CB_ADPCM_CH4_PlayEnd();	
		break;
	#endif
	#if _ADPCM_CH5_PLAY
	case 4: // ch5
		CB_ADPCM_CH5_PlayEnd();	
		break;
	#endif
	#if _ADPCM_CH6_PLAY
	case 5: // ch6
		CB_ADPCM_CH6_PlayEnd();	
		break;
	#endif
	#if _ADPCM_CH7_PLAY
	case 6: // ch7
		CB_ADPCM_CH7_PlayEnd();	
		break;
	#endif
	#if _ADPCM_CH8_PLAY
	case 7: // ch8
		CB_ADPCM_CH8_PlayEnd();	
		break;
	#endif
	default:
		break;
	}
}

//------------------------------------------------------------------//
// DacAssigner_codecUseDacCh for select ch
// Parameter: 
//			ch			: select channel 
// return value:
//          DAC_CH		: 0,1
//			-1			: No useable DAC CH
//------------------------------------------------------------------//
S8 _DacAssigner_codecUseDacCh(U8 ch){
	switch(ch){
	#if _ADPCM_CH2_PLAY
	case 1: // ch2
		return DacAssigner_codecUseDacCh(kADPCM_CH2);	
		break;
	#endif
	#if _ADPCM_CH3_PLAY
	case 2: // ch3
		return DacAssigner_codecUseDacCh(kADPCM_CH3);	
		break;	
	#endif
	#if _ADPCM_CH4_PLAY
	case 3: // ch4
		return DacAssigner_codecUseDacCh(kADPCM_CH4);
		break;
	#endif
	#if _ADPCM_CH5_PLAY
	case 4: // ch5
		return DacAssigner_codecUseDacCh(kADPCM_CH5);
		break;
	#endif
	#if _ADPCM_CH6_PLAY
	case 5: // ch6
		return DacAssigner_codecUseDacCh(kADPCM_CH6);
		break;
	#endif
	#if _ADPCM_CH7_PLAY
	case 6: // ch7
		return DacAssigner_codecUseDacCh(kADPCM_CH7);
		break;
	#endif
	#if _ADPCM_CH8_PLAY
	case 7: // ch8
		return DacAssigner_codecUseDacCh(kADPCM_CH8);
		break;
	#endif
	default:
		return -1;
		break;
	}
}

//------------------------------------------------------------------//
// DacAssigner_Regist for select ch
// Parameter: 
//			sample_rate : sample rate of channel
//			ch			: select channel 
// return value:
//          DAC_CH		: 0,1
//			-1			: No useable DAC CH
//------------------------------------------------------------------//
S8 _DacAssigner_Regist(U16 sample_rate, U8 ch){
	switch(ch){
	#if _ADPCM_CH2_PLAY
	case 1: // ch2
		return DacAssigner_Regist(kADPCM_CH2,sample_rate);
		break;
	#endif
	#if _ADPCM_CH3_PLAY
	case 2: // ch3
		return DacAssigner_Regist(kADPCM_CH3,sample_rate);
		break;	
	#endif
	#if _ADPCM_CH4_PLAY
	case 3: // ch4
		return DacAssigner_Regist(kADPCM_CH4,sample_rate);
		break;
	#endif
	#if _ADPCM_CH5_PLAY
	case 4: // ch5
		return DacAssigner_Regist(kADPCM_CH5,sample_rate);
		break;
	#endif
	#if _ADPCM_CH6_PLAY
	case 5: // ch6
		return DacAssigner_Regist(kADPCM_CH6,sample_rate);
		break;
	#endif
	#if _ADPCM_CH7_PLAY
	case 6: // ch7
		return DacAssigner_Regist(kADPCM_CH7,sample_rate);
		break;
	#endif
	#if _ADPCM_CH8_PLAY
	case 7: // ch8
		return DacAssigner_Regist(kADPCM_CH8,sample_rate);
		break;
	#endif	
	default:
		return -1;
		break;
	}
}


//------------------------------------------------------------------//
// DacAssigner_unRegist for select ch
// Parameter: 
//			ch			: select channel 
// return value:
//          NONE
//------------------------------------------------------------------//
void _DacAssigner_unRegist(U8 ch){
	switch(ch){
	#if _ADPCM_CH2_PLAY
	case 1: // ch2
		DacAssigner_unRegist(kADPCM_CH2);
		break;
	#endif
	#if _ADPCM_CH3_PLAY
	case 2: // ch3
		DacAssigner_unRegist(kADPCM_CH3);
		break;	
	#endif
	#if _ADPCM_CH4_PLAY
	case 3: // ch4
		DacAssigner_unRegist(kADPCM_CH4);
		break;
	#endif
	#if _ADPCM_CH5_PLAY
	case 4: // ch5
		DacAssigner_unRegist(kADPCM_CH5);
		break;
	#endif
	#if _ADPCM_CH6_PLAY
	case 5: // ch6
		DacAssigner_unRegist(kADPCM_CH6);
		break;
	#endif
	#if _ADPCM_CH7_PLAY
	case 6: // ch7
		DacAssigner_unRegist(kADPCM_CH7);
		break;
	#endif
	#if _ADPCM_CH8_PLAY
	case 7: // ch8
		DacAssigner_unRegist(kADPCM_CH8);
		break;
	#endif
	default:
		break;
	}
}
#if _ADPCM_CHX_PLAY_BACK_USE_CUSTOMIZE
//------------------------------------------------------------------//
// EQProcess for select ch
// Parameter: 
//          buf_data	  : buf data
//			ch			  : select channel 
// return value:
//          data
//------------------------------------------------------------------//
S16 _EQProcess(S16 buf_data,U8 ch){
	switch(ch){
	#if _ADPCM_CH2_PLAY && (defined(_ADPCM_CH2_PLAY_BACK_USE_CUSTOMIZE))
	case 1: // ch2
		return EQProcess(buf_data,EQFilterCtl_CH2);
		break;
	#endif
	#if _ADPCM_CH3_PLAY && (defined(_ADPCM_CH3_PLAY_BACK_USE_CUSTOMIZE))
	case 2: // ch3
		return EQProcess(buf_data,EQFilterCtl_CH3);
		break;		
	#endif
	#if _ADPCM_CH4_PLAY && (defined(_ADPCM_CH4_PLAY_BACK_USE_CUSTOMIZE))	
	case 3: // ch4
		return EQProcess(buf_data,EQFilterCtl_CH4);
		break;
	#endif
	#if _ADPCM_CH5_PLAY && (defined(_ADPCM_CH5_PLAY_BACK_USE_CUSTOMIZE))
	case 4: // ch5
		return EQProcess(buf_data,EQFilterCtl_CH5);
		break;
	#endif
	#if _ADPCM_CH6_PLAY && (defined(_ADPCM_CH6_PLAY_BACK_USE_CUSTOMIZE))
	case 5: // ch6
		return EQProcess(buf_data,EQFilterCtl_CH6);
		break;
	#endif
	#if _ADPCM_CH7_PLAY && (defined(_ADPCM_CH7_PLAY_BACK_USE_CUSTOMIZE))
	case 6: // ch7
		return EQProcess(buf_data,EQFilterCtl_CH7);
		break;
	#endif
	#if _ADPCM_CH8_PLAY && (defined(_ADPCM_CH8_PLAY_BACK_USE_CUSTOMIZE))
	case 7: // ch8
		return EQProcess(buf_data,EQFilterCtl_CH8);
		break;
	#endif
	default:
		return 0;
		break;
	}
}
#endif



//------------------------------------------------------------------//
// Get 1 byte, called by adpcm decode, frame base use
// Parameter: 
//			BufCtl		: BufCtl struct of channel 
//			StaticVar	: StaticVar struct of channel
//         	buf			: EncDataBuf of channel (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE0) 
// return value:
//          data
//------------------------------------------------------------------//
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)&&(!PR_READ_CHX) 
S8 _ADPCMGetByte(volatile BufCtl *BC, StaticVar *SV){
    S8 data;
    
    CB_ReadData(SV->StorageMode,&(SV->CurAddr),(U8*)&data,1); 
    return data;  
}
#else
S8 _ADPCMGetByte(volatile BufCtl *BC, StaticVar *SV, U8* buf){
    S8 data;
    
    data=buf[BC->rd_offset];
    BC->rd_offset++;
    if(BC->rd_offset==ADPCM_CHX_DATA_BUF_SIZE){
        BC->rd_offset=0;
    }    
    return data;
}    
#endif 

//------------------------------------------------------------------//
// Dac ISR of CHX
// Parameter: 
//       	size			: fifo fill size 
//       	pdest32			: fifo address
//	 	 	AudioBufCtl		: AudioBufCtl struct of channel
//		 	BufCtl			: BufCtl struct of channel
//	   	 	SmoothCtl		: SmoothCtl struct of channel	
//       	StaticVar		: StaticVar struct of channel
//		 	ch			 	: select channel
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CHX_DacIsr(U8 size,S16 *buf,volatile AudioBufCtl *ABC,volatile BufCtl *BC,volatile SmoothCtl *SC,StaticVar *SV,U8 ch){   
    S16 *psrc16,gain;
    U8 i,len,size2;
    
	if(ABC->shadow_state==STATUS_PLAYING){

#if _ADPCM_CHX_QIO_MODULE      
		QIO_Resume(QIO_USE_CH(ch));
#endif//_ADPCM_CHX_QIO_MODULE
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)            
		S16 temp;
        temp=BC->size-BC->offset;
        if(size>temp){
            size=temp;
        }   
#if _ADPCM_CHX_QIO_MODULE
		U16 _QioFrameFlag;
		for(i=0;i<size;i++){
			QIO_FrameFlag_FIFO_BitGet(QIO_USE_CH(ch),(U8*)&_QioFrameFlag);
			QIO_DacIsr(QIO_USE_CH(ch), (U8)_QioFrameFlag);
		}
#endif//_ADPCM_CHX_QIO_MODULE
#else
		U16 _QioFrameFlag;
        for(i=0;i<size;i++){
            buf[i]=ADPCMDecode(&_QioFrameFlag, ch);
#if _ADPCM_CHX_QIO_MODULE
			QIO_DacIsr(QIO_USE_CH(ch), (U8)_QioFrameFlag);
#endif//_ADPCM_CHX_QIO_MODULE
#if _ADPCM_CHX_PLAY_BACK_USE_CUSTOMIZE
			if(AdpcmEqChMask&(1<<ch)){
				buf[i] = _EQProcess(buf[i],ch);
			}
#endif
#if _AUDIO_VOLUME_CHANGE_SMOOTH
            buf[i]=((S16)buf[i]*_AUDIO_GetVolumeSmoothINT(ch))>>Q15_SHIFT;
#else
            buf[i]=((S16)buf[i]*SV->AudioVol)>>Q15_SHIFT;
#endif
        }    

#endif        
        ABC->fifo_count+=size;   
        
#if SMOOTH_ENABLE && (SMOOTH_CH2_ENABLE || SMOOTH_CH3_ENABLE || SMOOTH_CH4_ENABLE || SMOOTH_CH5_ENABLE || SMOOTH_CH6_ENABLE || SMOOTH_CH7_ENABLE || SMOOTH_CH8_ENABLE)      
        //smooth process
        if(SC->smooth!=SMOOTH_NONE){  
            size2= size;
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)               
            psrc16=(S16 *)(BC->buf+BC->offset);
#else
            psrc16=buf;
#endif        
            while(size2){
                if(SC->state==SMOOTH_STEP){
                    if(SC->smooth==SMOOTH_DOWN){
                        for(i=0;i<size2;i++){
                            *psrc16++=0;
                        }       
#if _ADPCM_CHX_QIO_MODULE
						if(ABC->state == STATUS_STOP)
						{
							QIO_DelayStop(QIO_USE_CH(ch),ENABLE);
						}
						else
						{
							QIO_Pause(QIO_USE_CH(ch));
						}
#endif//_ADPCM_CHX_QIO_MODULE
                        ABC->shadow_state=ABC->state;
                        if(ABC->state==STATUS_STOP){
                            SV->PlayEndFlag = BIT_SET;
                        }  
                    }
                    SC->smooth=SMOOTH_NONE;
                    break;            
                }
                if(SC->smooth==SMOOTH_DOWN){
                    gain=MAX_SMOOTH_GAIN-(SC->state+1)*(MAX_SMOOTH_GAIN/SMOOTH_STEP);
                }else{
                    gain=(SC->state+1)*(MAX_SMOOTH_GAIN/SMOOTH_STEP);
                }    
                len=SC->step_sample-SC->count;
            
                if(len>size2){
                    len=size2;
                }    
                 
                for(i=0;i<len;i++){
                    *psrc16=((S32)(*psrc16)*gain)>>15;
                    psrc16++;
                }         
                SC->count+=len;
                size2-=len;
                if(SC->count==SC->step_sample){
                    SC->count=0;
                    SC->state++;   
                }          
            }  
        }
#endif  

#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
        psrc16=(S16 *)(BC->buf+BC->offset); 
        CB_ADPCM_CHX_WriteDac(size,(U16 *)psrc16,ch);
        BC->offset+=size;

        if(BC->offset==BC->size){
            BC->offset=0;    
        }       
#else      
        CB_ADPCM_CHX_WriteDac(size,(U16 *)buf,ch);
#endif

        if(ABC->fifo_count>=ABC->samples){
#if _ADPCM_CHX_QIO_MODULE
			QIO_DelayStop(QIO_USE_CH(ch),DISABLE);
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
			QIO_FrameFlag_FIFO_RstGet(QIO_USE_CH(ch));
#endif
#endif//_ADPCM_CHX_QIO_MODULE
            ABC->state=STATUS_STOP;
            ABC->shadow_state=ABC->state;
            SV->PlayEndFlag = BIT_SET;
        }
    }else{
        CB_ADPCM_CHX_WriteDac(size,(U16 *)0,ch);
                
    }                 
}   

#if SMOOTH_ENABLE && (SMOOTH_CH2_ENABLE || SMOOTH_CH3_ENABLE || SMOOTH_CH4_ENABLE || SMOOTH_CH5_ENABLE || SMOOTH_CH6_ENABLE || SMOOTH_CH7_ENABLE || SMOOTH_CH8_ENABLE)
//------------------------------------------------------------------//
// Smooth up or down of CHX
// Parameter: 
//          AudioBufCtl	: AudioBufCtl struct of channel
//			SmoothCtl	: SmoothCtl struct of channel
//          smooth		: SMOOTH_UP,SMOOTH_DOWN,SMOOTH_NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
static void _SmoothFunc(volatile AudioBufCtl *ABC,volatile SmoothCtl *SC,U8 smooth){
	SC->count=0;  
	SC->state=0;  
	
    if(smooth==SMOOTH_UP){
	    SC->smooth=SMOOTH_UP;
	    SC->step_sample=ABC->sample_rate*SMOOTH_UP_TIME/1000/SMOOTH_STEP;
    }else if(smooth==SMOOTH_DOWN){
	    SC->smooth=SMOOTH_DOWN;
	    SC->step_sample=ABC->sample_rate*SMOOTH_DOWN_TIME/1000/SMOOTH_STEP;
    }else{
        SC->smooth=SMOOTH_NONE;
    }
}     

//------------------------------------------------------------------//
// Smooth up or down function of channel
// Parameter: 
//          smooth	: SMOOTH_UP,SMOOTH_DOWN,SMOOTH_NONE
//		 	ch		: select channel
// return value:
//          NONE
//------------------------------------------------------------------//     
static void SmoothFunc(U8 smooth,U8 ch){
	switch(ch){
	#if SMOOTH_CH2_ENABLE
	case 1: // ch2
		_SmoothFunc(AudioBufCtl_CH2,&SmoothCtl_CH2,smooth);
		break;
	#endif
	#if SMOOTH_CH3_ENABLE
	case 2: // ch3
		_SmoothFunc(AudioBufCtl_CH3,&SmoothCtl_CH3,smooth);
		break;
	#endif	
	#if SMOOTH_CH4_ENABLE
	case 3: // ch4
		_SmoothFunc(AudioBufCtl_CH4,&SmoothCtl_CH4,smooth);
		break;
	#endif
	#if SMOOTH_CH5_ENABLE
	case 4: // ch5
		_SmoothFunc(AudioBufCtl_CH5,&SmoothCtl_CH5,smooth);
		break;	
	#endif	
	#if SMOOTH_CH6_ENABLE
	case 5: // ch6
		_SmoothFunc(AudioBufCtl_CH6,&SmoothCtl_CH6,smooth);
		break;
	#endif	
	#if SMOOTH_CH7_ENABLE
	case 6: // ch7
		_SmoothFunc(AudioBufCtl_CH7,&SmoothCtl_CH7,smooth);
		break;
	#endif
	#if SMOOTH_CH8_ENABLE
	case 7: // ch8
		_SmoothFunc(AudioBufCtl_CH8,&SmoothCtl_CH8,smooth);
		break;	
	#endif
	default:
		break;
	}
}
#endif//#if SMOOTH_ENABLE end

#if _ADPCM_CHX_PLAY_BACK_USE_CUSTOMIZE 
//------------------------------------------------------------------//
// Init EQ parameter when audio play of channel
// Parameter: 
//	 	 	AudioBufCtl		: AudioBufCtl struct  of channel
//	 	 	EQCtl			: EQCtl struct  of channel
//       	*buf 			: EqBuf of channel
//		 	ch			 	: select channel 
// return value:
//          EXIT_FAILURE, EXIT_SUCCESS
//------------------------------------------------------------------//   
static U8 _initPlayBackEQParameter(volatile AudioBufCtl *ABC,EQCtl *EFC,S32 *buf,U8 ch)
{
	if(ADPCM_CHX_GetStatus(ch)!=STATUS_STOP)return EXIT_FAILURE;//error 
#if _USE_HEAP_RAM
	MemAllocateEqBuf(ch);
	buf=EqBuf_CHX;
#endif	
	memset((void *)buf, 0, ADPCM_EQ_BUF_SIZE*sizeof(S32));
	EFC->SampleRate = ABC->sample_rate;
	EFC->Buf = buf;
	
#if	_ADPCM_CHX_PLAY_BACK_USE_CUSTOMIZE//customize EQ
	EFC->EQGroupSelect=CUSTOMIZE;
	EFC->PitchIndex=0;//fix 0
	if(u8EQFilterIndex >= FILTER_NUMBER){
		EFC->FilterPara1 = _AllPassFilterParaTab;//all pass parameter
	} else {
		EFC->FilterPara1 = (S16*)FilterList[u8EQFilterIndex];//UserDefine
	}
#endif
	
	EFC->FilterPara2=NULL;
	EFC->Gain1=0;
	EFC->Gain2=0;
	return (EQInit(EFC)==1)?EXIT_SUCCESS:EXIT_FAILURE;//(EQInit)-1:failure, 1:success 
}

//------------------------------------------------------------------//
// Init EQ parameter when audio play
// Parameter: 
//			ch			  : select channel 
// return value:
//          EXIT_FAILURE, EXIT_SUCCESS
//------------------------------------------------------------------//   
static U8 initPlayBackEQParameter(U8 ch){
	switch(ch){
	#if _ADPCM_CH2_PLAY && (defined(_ADPCM_CH2_PLAY_BACK_USE_CUSTOMIZE))
	case 1: // ch2
		return _initPlayBackEQParameter(AudioBufCtl_CH2,EQFilterCtl_CH2,EqBuf_CH2,ch);
		break;
	#endif
	#if _ADPCM_CH3_PLAY && (defined(_ADPCM_CH3_PLAY_BACK_USE_CUSTOMIZE))
	case 2: // ch3
		return _initPlayBackEQParameter(AudioBufCtl_CH3,EQFilterCtl_CH3,EqBuf_CH3,ch);
		break;
	#endif
	#if _ADPCM_CH4_PLAY && (defined(_ADPCM_CH4_PLAY_BACK_USE_CUSTOMIZE))
	case 3: // ch4
		return _initPlayBackEQParameter(AudioBufCtl_CH4,EQFilterCtl_CH4,EqBuf_CH4,ch);
		break;
	#endif
	#if _ADPCM_CH6_PLAY && (defined(_ADPCM_CH5_PLAY_BACK_USE_CUSTOMIZE))
	case 4: // ch5
		return _initPlayBackEQParameter(AudioBufCtl_CH5,EQFilterCtl_CH5,EqBuf_CH5,ch);
		break;
	#endif
	#if _ADPCM_CH6_PLAY && (defined(_ADPCM_CH6_PLAY_BACK_USE_CUSTOMIZE))
	case 5: // ch6
		return _initPlayBackEQParameter(AudioBufCtl_CH6,EQFilterCtl_CH6,EqBuf_CH6,ch);
		break;
	#endif
	#if _ADPCM_CH7_PLAY && (defined(_ADPCM_CH7_PLAY_BACK_USE_CUSTOMIZE))
	case 6: // ch7
		return _initPlayBackEQParameter(AudioBufCtl_CH7,EQFilterCtl_CH7,EqBuf_CH7,ch);
		break;
	#endif
	#if _ADPCM_CH8_PLAY && (defined(_ADPCM_CH8_PLAY_BACK_USE_CUSTOMIZE))
	case 7: // ch8
		return _initPlayBackEQParameter(AudioBufCtl_CH8,EQFilterCtl_CH8,EqBuf_CH8,ch);
		break;
	#endif
	default: 
		return EXIT_FAILURE;
		break;
	}
}
#endif

#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
//------------------------------------------------------------------//
// Decode one audio frame of CHX
// Parameter: 
//	 	 	AudioBufCtl		: AudioBufCtl struct of channel
//			BufCtl			: BufCtl struct of channel (PR_READ_CHX is ENABLE)
//       	StaticVar		: StaticVar struct of channel
//       	*buf1			: AudioBuf of channel
//       	*buf2			: EncDataBuf of channel (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)&&(PR_READ_CHX)
//		 	ch				: select channel 
// return value:
//          NONE
//------------------------------------------------------------------//     
#if PR_READ_CHX
static void _DecodeFrame(volatile AudioBufCtl *ABC,volatile BufCtl *BC,StaticVar *SV,U16 *buf1,U8 *buf2,U8 ch)
#else
static void _DecodeFrame(volatile AudioBufCtl *ABC,StaticVar *SV,U16 *buf1,U8 ch)
#endif
{
    U16 *audio_buf,i;
    U16 size;
	U16 _QioFrameFlag;

    size = ADPCM_CHX_AUDIO_OUT_SIZE;
    if((size+ABC->sample_count)>ABC->samples)
    {
        size=ABC->samples-ABC->sample_count;
    } 
    audio_buf=buf1+(ADPCM_CHX_AUDIO_OUT_SIZE*ABC->index);
    for(i=0;i<size;i++)
    {
        audio_buf[i]=ADPCMDecode(&_QioFrameFlag, ch);   
#if	_ADPCM_CHX_QIO_MODULE
#if	(_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
		QIO_FrameFlag_FIFO_BitPut(QIO_USE_CH(ch),(U8*)&_QioFrameFlag);
#endif
#endif//_ADPCM_CHX_QIO_MODULE
    } 
    
    #if PR_READ_CHX	
    //Pre-store Data of SPI Flash to buffer for adpcm decode
    U16 size2;        
    U16 bufctl_rd_offset = BC->rd_offset;
			
	if(bufctl_rd_offset!=BC->wr_offset){
		if(bufctl_rd_offset>BC->wr_offset){
			size2=bufctl_rd_offset-BC->wr_offset;   
		}else{
			size2=ADPCM_CHX_DATA_BUF_SIZE-BC->wr_offset;  
		}     
		CB_ReadData(SV->StorageMode,&(SV->CurAddr),buf2+BC->wr_offset,size2);    
		BC->wr_offset+=size2; 
		if(BC->wr_offset==ADPCM_CHX_DATA_BUF_SIZE){
			BC->wr_offset=0;                
		}  
	}
    #endif   
    
    ABC->sample_count+=size;
    ABC->index++;
    if(ABC->index==ADPCM_CHX_AUDIO_OUT_COUNT){
        ABC->index=0;    
    }    
    ABC->process_count+=ADPCM_CHX_AUDIO_OUT_SIZE;
#if	_ADPCM_CHX_QIO_MODULE
#if	(_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
	if(ABC->samples == ABC->sample_count)
	{
		QIO_FrameFlag_FIFO_RstPut(QIO_USE_CH(ch));
	}
#endif
#endif//_ADPCM_CHX_QIO_MODULE

    //volume 
    for(i=0;i<ADPCM_CHX_AUDIO_OUT_SIZE;i++){
#if _ADPCM_CHX_PLAY_BACK_USE_CUSTOMIZE
		if(AdpcmEqChMask&(1<<ch)){
			audio_buf[i] = _EQProcess(audio_buf[i],ch);
		}
#endif
#if _AUDIO_VOLUME_CHANGE_SMOOTH
        audio_buf[i]=((S16)audio_buf[i]*_AUDIO_GetVolumeSmooth(ch))>>Q15_SHIFT;
#else
        audio_buf[i]=((S16)audio_buf[i]*SV->AudioVol)>>Q15_SHIFT;
#endif
    }   
}
//------------------------------------------------------------------//
// Decode one audio frame 
// Parameter: 
//			ch			  : select channel 
// return value:
//          NONE
//------------------------------------------------------------------//     
static void DecodeFrame(U8 ch)
{
#if PR_READ_CHX
	switch(ch){
	#if _ADPCM_CH2_PLAY
	case 1: // ch2
		_DecodeFrame(AudioBufCtl_CH2,BufCtl_CH2,StaticVar_CH2,AudioBuf_CH2,EncDataBuf_CH2,ch);
		break;
	#endif	
	#if _ADPCM_CH3_PLAY
	case 2: // ch3
		_DecodeFrame(AudioBufCtl_CH3,BufCtl_CH3,StaticVar_CH3,AudioBuf_CH3,EncDataBuf_CH3,ch);
		break;
	#endif	
	#if _ADPCM_CH4_PLAY
	case 3: // ch4
		_DecodeFrame(AudioBufCtl_CH4,BufCtl_CH4,StaticVar_CH4,AudioBuf_CH4,EncDataBuf_CH4,ch);
		break;
	#endif	
	#if _ADPCM_CH5_PLAY
	case 4: // ch5
		_DecodeFrame(AudioBufCtl_CH5,BufCtl_CH5,StaticVar_CH5,AudioBuf_CH5,EncDataBuf_CH5,ch);
		break;	
	#endif	
	#if _ADPCM_CH6_PLAY
	case 5: // ch6
		_DecodeFrame(AudioBufCtl_CH6,BufCtl_CH6,StaticVar_CH6,AudioBuf_CH6,EncDataBuf_CH6,ch);
		break;
	#endif	
	#if _ADPCM_CH7_PLAY
	case 6: // ch7
		_DecodeFrame(AudioBufCtl_CH7,BufCtl_CH7,StaticVar_CH7,AudioBuf_CH7,EncDataBuf_CH7,ch);
		break;
	#endif	
	#if _ADPCM_CH8_PLAY
	case 7: // ch8
		_DecodeFrame(AudioBufCtl_CH8,BufCtl_CH8,StaticVar_CH8,AudioBuf_CH8,EncDataBuf_CH8,ch);
		break;
	#endif		
	default:
		break;
	}
#else
	switch(ch){
	#if _ADPCM_CH2_PLAY
	case 1: // ch2
		_DecodeFrame(AudioBufCtl_CH2,StaticVar_CH2,AudioBuf_CH2,ch);
		break;
	#endif	
	#if _ADPCM_CH3_PLAY
	case 2: // ch3
		_DecodeFrame(AudioBufCtl_CH3,StaticVar_CH3,AudioBuf_CH3,ch);
		break;
	#endif	
	#if _ADPCM_CH4_PLAY
	case 3: // ch4
		_DecodeFrame(AudioBufCtl_CH4,StaticVar_CH4,AudioBuf_CH4,ch);
		break;
	#endif	
	#if _ADPCM_CH5_PLAY
	case 4: // ch5
		_DecodeFrame(AudioBufCtl_CH5,StaticVar_CH5,AudioBuf_CH5,ch);
		break;	
	#endif	
	#if _ADPCM_CH6_PLAY
	case 5: // ch6
		_DecodeFrame(AudioBufCtl_CH6,StaticVar_CH6,AudioBuf_CH6,ch);
		break;
	#endif	
	#if _ADPCM_CH7_PLAY
	case 6: // ch7
		_DecodeFrame(AudioBufCtl_CH7,StaticVar_CH7,AudioBuf_CH7,ch);
		break;
	#endif	
	#if _ADPCM_CH8_PLAY
	case 7: // ch8
		_DecodeFrame(AudioBufCtl_CH8,StaticVar_CH8,AudioBuf_CH8,ch);
		break;
	#endif		
	default:
		break;
	}	
#endif	
}
#endif //(_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)



//------------------------------------------------------------------//
// Start play voice of channel, frame base
// Parameter: 
//       	index			: file index or absolute address
//       	mode			: OTP_MODE,EF_MODE,SPI_MODE,SPI1_MODE,EF_UD_MODE
//	 	 	AudioBufCtl		: AudioBufCtl struct of channel
//	 	 	BufCtl			: BufCtl struct of channel
//	 	 	StaticVar		: StaticVar struct of channel
//       	*buf1			: AudioBuf of channel(_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1), EncDataBuf of channel(_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE0)
//       	*buf2			: ModeBuf of channel
//       	*buf3			: DecBuf of channel
//       	*buf4			: EncDataBuf of channel (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)&&(PR_READ_CHX)
//		 	ch			  	: select channel  
// return value:
//          0:ok
//          others:fail
//------------------------------------------------------------------// 
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
#if PR_READ_CHX
U8 ADPCM_CHX_Play(U32 index,U8 mode,volatile AudioBufCtl *ABC,volatile BufCtl *BC, StaticVar *SV,U16 *buf1,U8 *buf2,U8 *buf3,U8 *buf4,U8 ch)
#else
U8 ADPCM_CHX_Play(U32 index,U8 mode,volatile AudioBufCtl *ABC,volatile BufCtl *BC, StaticVar *SV,U16 *buf1,U8 *buf2,U8 *buf3,U8 ch)
#endif
#else
U8 ADPCM_CHX_Play(U32 index,U8 mode,volatile AudioBufCtl *ABC,volatile BufCtl *BC, StaticVar *SV,U8 *buf1,U8 *buf2,U8 *buf3,U8 ch)
#endif
{
    struct AUDIO_HEADER *pheader;
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE0)
    U8 header[HEADER_LEN];
#endif 
	
    if(ABC->shadow_state==STATUS_PLAYING){
#if SMOOTH_ENABLE && (SMOOTH_CH2_ENABLE || SMOOTH_CH3_ENABLE || SMOOTH_CH4_ENABLE || SMOOTH_CH5_ENABLE || SMOOTH_CH6_ENABLE || SMOOTH_CH7_ENABLE || SMOOTH_CH8_ENABLE)        
            ABC->state=STATUS_STOP;
            SmoothFunc(SMOOTH_DOWN,ch);
            if(!DacMixerRamp_IsBias()) {
                ABC->shadow_state=STATUS_STOP;
#if _ADPCM_CHX_QIO_MODULE      
                QIO_Stop(QIO_USE_CH(ch),ENABLE);
#endif
            }
            while(ABC->shadow_state==STATUS_PLAYING){
                AUDIO_ServiceLoop();
            }
#endif        
    }
#if _USE_HEAP_RAM    
#if _VR_FUNC
    VR_Halt();
#endif
	AUDIO_ServiceLoop();
    ADPCM_AllocateMemory(MEMORY_ALLOCATE_PLAY,ch);	
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)		
	buf1 = AudioBuf_CHX;
	#if PR_READ_CHX
	buf4 = EncDataBuf_CHX;
	#endif
#else
	buf1 = EncDataBuf_CHX;
#endif
	buf2 = ModeBuf_CHX;
	buf3 = DecBuf_CHX;
#else
	AUDIO_ServiceLoop();
#endif        

    memset((void *)ABC, 0, AUDIO_BUF_CTL_SIZE);	
    
    if(CB_SetStartAddr(index,mode,0,&(SV->StartAddr))){
//    dprintf("ch%d E_AUDIO_INVALID_IDX\r\n",ch+1);
		return E_AUDIO_INVALID_IDX;
	}
    SV->CurAddr=SV->StartAddr;
    SV->StorageMode=mode;

#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)    
    CB_ReadData(SV->StorageMode,&(SV->CurAddr),(U8 *)buf1,HEADER_LEN);
    
    pheader=(struct AUDIO_HEADER *)buf1;
#else
    CB_ReadData(SV->StorageMode,&(SV->CurAddr),(U8 *)header,HEADER_LEN);
    
    pheader=(struct AUDIO_HEADER *)header;
#endif    
   
    if(pheader->codec!=CODEC_ADPCM){  
        return E_AUDIO_UNSUPPORT_CODEC;    
    }     

	GIE_DISABLE();

	S8 hw_ch = _DacAssigner_Regist(pheader->sample_rate,ch);	

	GIE_ENABLE();
    if(hw_ch<0){
        //dprintf("No play Invalid sr\r\n");
        return E_AUDIO_INVALID_SAMPLERATE;
    }

#if _ADPCM_CHX_QIO_MODULE
	if(pheader->nyq_offset)
	{
		QIO_Play(QIO_USE_CH(ch), mode, (SV->StartAddr+HEADER_LEN+pheader->nyq_offset));
	}
#endif//_ADPCM_CHX_QIO_MODULE    
    ABC->samples=pheader->samples;
    ABC->in_size=pheader->decode_in_size;
    ABC->out_size=pheader->decode_out_size;
    ABC->sample_rate=pheader->sample_rate;
    ABC->upsample=pheader->flag&0x1;
    BC->offset=0;
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
    BC->size=ADPCM_CHX_AUDIO_BUF_SIZE;
    BC->buf=buf1;
#endif
 
#if SMOOTH_ENABLE && (SMOOTH_CH2_ENABLE || SMOOTH_CH3_ENABLE || SMOOTH_CH4_ENABLE || SMOOTH_CH5_ENABLE || SMOOTH_CH6_ENABLE || SMOOTH_CH7_ENABLE || SMOOTH_CH8_ENABLE) 
    SmoothFunc(SMOOTH_UP,ch);	
#endif
    SetADPCMMode(ABC->in_size,ABC->out_size,buf2,ch);
    _ADPCMDecInit(buf3, ch);			
    CB_ADPCM_CHX_InitDac(hw_ch,ABC->sample_rate,ABC->upsample,ch);		
#if _ADPCM_CHX_PLAY_BACK_USE_CUSTOMIZE		
	if(AdpcmEqChMask&(1<<ch))
	{
		if(initPlayBackEQParameter(ch)!=EXIT_SUCCESS){
			return 1;//init error
		}
	}
#endif

    SV->AudioVol=  (SV->ChVol+1)*(MixerVol+1); 
        
    if(!(SV->ChVol)){
        SV->AudioVol=0;
    }    
    if(SV->AudioVol==256){
        SV->AudioVol=255;
    }    
    SV->AudioVol=SV->AudioVol<<7;
#if _AUDIO_VOLUME_CHANGE_SMOOTH
    _AUDIO_InitVolumeSmooth(ABC->sample_rate,SV->AudioVol,ch);
#endif
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
#if	PR_READ_CHX
	CB_ReadData(SV->StorageMode,&(SV->CurAddr),buf4,ADPCM_CHX_DATA_BUF_SIZE);
    BC->rd_offset=0;
    BC->wr_offset=0;   
#endif 
    memset((void *)buf1, 0, ADPCM_CHX_AUDIO_BUF_SIZE*2);
    DecodeFrame(ch);
#else
    CB_ReadData(SV->StorageMode,&(SV->CurAddr),buf1,ADPCM_CHX_DATA_BUF_SIZE);
    BC->rd_offset=0;
    BC->wr_offset=0;
#endif

    SV->OpMode=0;
    ABC->state=  STATUS_PLAYING;
    ABC->shadow_state=ABC->state;
    CB_ADPCM_CHX_PlayStart(ch);  
	CB_ADPCM_CHX_DacCmd(hw_ch,ch,ENABLE);
    return 0;
}   

//------------------------------------------------------------------//
// Pause playing of CHX
// Parameter: 
//	 	 	AudioBufCtl		: AudioBufCtl struct of channel
//		 	ch			 	: select channel 
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CHX_Pause(volatile AudioBufCtl *ABC,U8 ch){
    if(ABC->state==STATUS_PLAYING){
#if SMOOTH_ENABLE  && (SMOOTH_CH2_ENABLE || SMOOTH_CH3_ENABLE || SMOOTH_CH4_ENABLE || SMOOTH_CH5_ENABLE || SMOOTH_CH6_ENABLE || SMOOTH_CH7_ENABLE || SMOOTH_CH8_ENABLE)       
		ABC->state=STATUS_PAUSE;
        SmoothFunc(SMOOTH_DOWN,ch);
#endif             
    }     
} 

//------------------------------------------------------------------//
// Resume playing of CHX
// Parameter: 
//	 	 	AudioBufCtl		: AudioBufCtl struct of channel
//	 	 	StaticVar		: StaticVar struct of channel
//		 	ch			 	: select channel 
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CHX_Resume(volatile AudioBufCtl *ABC, StaticVar *SV,U8 ch){
	if(ABC->state==STATUS_PAUSE&&(!(SV->OpMode))){
        if(_DacAssigner_codecUseDacCh(ch)<0) {
            GIE_DISABLE();

            S8 hw_ch = _DacAssigner_Regist(ABC->sample_rate,ch);
            GIE_ENABLE();
            if(hw_ch<0) return;//error
        
            CB_ADPCM_CHX_InitDac(hw_ch,ABC->sample_rate,ABC->upsample,ch); 

            CB_ADPCM_CHX_DacCmd(hw_ch,ch,ENABLE);
        }

#if SMOOTH_ENABLE && (SMOOTH_CH2_ENABLE || SMOOTH_CH3_ENABLE || SMOOTH_CH4_ENABLE || SMOOTH_CH5_ENABLE || SMOOTH_CH6_ENABLE || SMOOTH_CH7_ENABLE || SMOOTH_CH8_ENABLE)        
        SmoothFunc(SMOOTH_UP,ch);
#endif
        ABC->state=STATUS_PLAYING;
        ABC->shadow_state=ABC->state;
    }                
} 

//------------------------------------------------------------------//
// Stop playing of CHX
// Parameter: 
//	 	 	AudioBufCtl		: AudioBufCtl struct of channel
//	 		StaticVar		: StaticVar struct of channel
//		 	ch			 	: select channel 
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CHX_Stop(volatile AudioBufCtl *ABC, StaticVar *SV,U8 ch){
    if((ABC->state==STATUS_PLAYING||ABC->state==STATUS_PAUSE)&&(!(SV->OpMode))){ 
#if SMOOTH_ENABLE  && (SMOOTH_CH2_ENABLE || SMOOTH_CH3_ENABLE || SMOOTH_CH4_ENABLE || SMOOTH_CH5_ENABLE || SMOOTH_CH6_ENABLE || SMOOTH_CH7_ENABLE || SMOOTH_CH8_ENABLE)       
            ABC->state=STATUS_STOP;
            SmoothFunc(SMOOTH_DOWN,ch);
			if (ABC->shadow_state == STATUS_PAUSE
              || !DacMixerRamp_IsBias()
            ) {
				ABC->shadow_state = STATUS_STOP;
#if _ADPCM_CHX_QIO_MODULE
				QIO_Stop(QIO_USE_CH(ch),ENABLE);
#endif//_ADPCM_CHX_QIO_MODULE
			}
#endif            
    }              
} 

//------------------------------------------------------------------//
// Get current status of CHX
// Parameter: 
//	 	 	AudioBufCtl		: AudioBufCtl struct of channel
// return value:
//          status			: STATUS_PLAYING,STATUS_PAUSE,STATUS_STOP
//------------------------------------------------------------------// 
U8 _ADPCM_CHX_GetStatus(volatile AudioBufCtl *ABC){
    return ABC->shadow_state;    
}

//------------------------------------------------------------------//
// Get current status of channel
// Parameter: 
//			ch			  : select channel 
// return value:
//          status		  : STATUS_PLAYING,STATUS_PAUSE,STATUS_STOP
//------------------------------------------------------------------// 
U8 ADPCM_CHX_GetStatus(U8 ch){
	switch(ch){
	#if _ADPCM_CH2_PLAY
	case 1: // ch2
		return ADPCM_CH2_GetStatus();
		break;
	#endif
	#if _ADPCM_CH3_PLAY
	case 2: // ch3
		return ADPCM_CH3_GetStatus();
		break;
	#endif
	#if _ADPCM_CH4_PLAY
	case 3: // ch4
		return ADPCM_CH4_GetStatus();
		break;
	#endif
	#if _ADPCM_CH5_PLAY
	case 4: // ch5
		return ADPCM_CH5_GetStatus();
		break;
	#endif
	#if _ADPCM_CH6_PLAY
	case 5: // ch6
		return ADPCM_CH6_GetStatus();
		break;
	#endif
	#if _ADPCM_CH7_PLAY
	case 6: // ch7
		return ADPCM_CH7_GetStatus();
		break;
	#endif
	#if _ADPCM_CH8_PLAY
	case 7: // ch8
		return ADPCM_CH8_GetStatus();
		break;
	#endif
	default:
		return  0;
		break;
	}      
}

#if _ADPCM_CHX_PLAY_BACK_USE_CUSTOMIZE	 
//------------------------------------------------------------------//
// Set customize filter index
// Parameter: 
//	 	 	EQCtl	: EQCtl struct  of channel
//          index	: (0~FILTER_NUMBER-1)User define filter, (other)All Pass
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CHX_SetEQFilter(volatile EQCtl *EFC,U8 index){
	u8EQFilterIndex = index;
	if(u8EQFilterIndex >= FILTER_NUMBER){
		EFC->FilterPara1 = _AllPassFilterParaTab;//all pass parameter
	} else {
		EFC->FilterPara1 = (S16*)FilterList[u8EQFilterIndex];//UserDefine
	}
}
#endif

//------------------------------------------------------------------//
// Set audio volume of CHX
// Parameter: 
//			StaticVar:StaticVar struct of channel
//          vol:0~15
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CHX_SetVolume(volatile StaticVar *SV,U8 vol){   
    if(vol>CH_VOL_15){
        vol=CH_VOL_15;
    }    
    SV->ChVol=vol; 
}

//------------------------------------------------------------------//
// Set audio MixCtrl of channel
// Parameter: 
//          mixCtrl		: MIX_CTRL_0 / MIX_CTRL_25 / MIX_CTRL_50 / MIX_CTRL_75 / MIX_CTRL_100
//			ch			: select channel 
// return value:
//          NONE
//------------------------------------------------------------------// 
#if _AUDIO_MIX_CONTROL
void ADPCM_CHX_SetMixCtrl(U8 mixCtrl, U8 ch){
    if(mixCtrl>MIX_CTRL_100) mixCtrl=MIX_CTRL_100;
    
	switch(ch){
#if _ADPCM_CH2_PLAY
	case 1: // ch2
    AdpcmMixCtrl_CH2=mixCtrl;
    break;
#endif
#if _ADPCM_CH3_PLAY
	case 2: // ch3
    AdpcmMixCtrl_CH3=mixCtrl;
    break;
#endif
#if _ADPCM_CH4_PLAY
	case 3: // ch4
    AdpcmMixCtrl_CH4=mixCtrl;
    break;
#endif
#if _ADPCM_CH5_PLAY
	case 4: // ch5
    AdpcmMixCtrl_CH5=mixCtrl;
    break;
#endif
#if _ADPCM_CH6_PLAY
	case 5: // ch6
    AdpcmMixCtrl_CH6=mixCtrl;
    break;
#endif
#if _ADPCM_CH7_PLAY
	case 6: // ch7
    AdpcmMixCtrl_CH7=mixCtrl;
    break;
#endif
#if _ADPCM_CH8_PLAY
	case 7: // ch8
    AdpcmMixCtrl_CH8=mixCtrl;
    break;
#endif
	default:
		break;
	}
}
#endif
//------------------------------------------------------------------//
// Codec init of channel
// Parameter: 
//          StaticVar	  : StaticVar struct of channel
//			ch			  : select channel 
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CHX_Init(StaticVar *SV,U8 ch){
    SV->ChVol=CH_VOL_15;    
#if _AUDIO_MIX_CONTROL
	switch(ch){
#if _ADPCM_CH2_PLAY
	case 1: // ch2
    ADPCM_CH2_SetMixCtrl(MIX_CTRL_100);
    break;
#endif
#if _ADPCM_CH3_PLAY
	case 2: // ch3
    ADPCM_CH3_SetMixCtrl(MIX_CTRL_100);
    break;
#endif
#if _ADPCM_CH4_PLAY
	case 3: // ch4
    ADPCM_CH4_SetMixCtrl(MIX_CTRL_100);
    break;
#endif
#if _ADPCM_CH5_PLAY
	case 4: // ch5
    ADPCM_CH5_SetMixCtrl(MIX_CTRL_100);
    break;
#endif
#if _ADPCM_CH6_PLAY
	case 5: // ch6
    ADPCM_CH6_SetMixCtrl(MIX_CTRL_100);
    break;
#endif
#if _ADPCM_CH7_PLAY
	case 6: // ch7
    ADPCM_CH7_SetMixCtrl(MIX_CTRL_100);
    break;
#endif
#if _ADPCM_CH8_PLAY
	case 7: // ch8
    ADPCM_CH8_SetMixCtrl(MIX_CTRL_100);
    break;
#endif
	default:
		break;
	}
#endif

#if _ADPCM_CHX_PLAY_BACK_USE_CUSTOMIZE 
	switch(ch){
#if _ADPCM_CH2_PLAY&&defined(_ADPCM_CH2_PLAY_BACK_USE_CUSTOMIZE)
	case 1: // ch2
    ADPCM_CH2_SetEQFilter(FILTER_NUMBER);
    AdpcmEqChMask |= 1<<1;
    break;
#endif
#if _ADPCM_CH3_PLAY&&defined(_ADPCM_CH3_PLAY_BACK_USE_CUSTOMIZE)
	case 2: // ch3
    ADPCM_CH3_SetEQFilter(FILTER_NUMBER);
    AdpcmEqChMask |= 1<<2;
    break;
#endif
#if _ADPCM_CH4_PLAY&&defined(_ADPCM_CH4_PLAY_BACK_USE_CUSTOMIZE)
	case 3: // ch4
    ADPCM_CH4_SetEQFilter(FILTER_NUMBER);
    AdpcmEqChMask |= 1<<3;
    break;
#endif
#if _ADPCM_CH5_PLAY&&defined(_ADPCM_CH5_PLAY_BACK_USE_CUSTOMIZE)
	case 4: // ch5
    ADPCM_CH5_SetEQFilter(FILTER_NUMBER);
    AdpcmEqChMask |= 1<<4;
    break;
#endif
#if _ADPCM_CH6_PLAY&&defined(_ADPCM_CH6_PLAY_BACK_USE_CUSTOMIZE)
	case 5: // ch6
    ADPCM_CH6_SetEQFilter(FILTER_NUMBER);
    AdpcmEqChMask |= 1<<5;
    break;
#endif
#if _ADPCM_CH7_PLAY&&defined(_ADPCM_CH7_PLAY_BACK_USE_CUSTOMIZE)
	case 6: // ch7
    ADPCM_CH7_SetEQFilter(FILTER_NUMBER);
    AdpcmEqChMask |= 1<<6;
    break;
#endif
#if _ADPCM_CH8_PLAY&&defined(_ADPCM_CH8_PLAY_BACK_USE_CUSTOMIZE)
	case 7: // ch8
    ADPCM_CH8_SetEQFilter(FILTER_NUMBER);
    AdpcmEqChMask |= 1<<7;
    break;
#endif
	default:
		break;
	}
#endif
}

//------------------------------------------------------------------//
// Service loop of CHX
// Parameter: 
//	 	 	AudioBufCtl		: AudioBufCtl struct of channel
//	 	 	BufCtl			: BufCtl struct of channel
//	 	 	StaticVar		: StaticVar struct of channel
//			*buf			: AudioBuf of channel(_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1), EncDataBuf of channel(_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE0)
//		 	ch			 	: select channel
// return value:
//          NONE
//------------------------------------------------------------------//     
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)	
void ADPCM_CHX_ServiceLoop(volatile AudioBufCtl *ABC,volatile BufCtl *BC,StaticVar *SV,U16 *buf,U8 ch)
#else
void ADPCM_CHX_ServiceLoop(volatile AudioBufCtl *ABC,volatile BufCtl *BC,StaticVar *SV,U8 *buf,U8 ch)
#endif
{
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE0)
    U16 size;
#endif    
    
	if(ABC->shadow_state==STATUS_PLAYING){
    	GIE_DISABLE();
        SV->AudioVol=  (SV->ChVol+1)*(MixerVol+1); 
        
        if(!(SV->ChVol)){
            SV->AudioVol=0;
        }    
        if(SV->AudioVol==256){
            SV->AudioVol=255;
        }    
        SV->AudioVol=SV->AudioVol<<7;
        GIE_ENABLE();
#if _AUDIO_VOLUME_CHANGE_SMOOTH
        _AUDIO_SetVolumeSmooth(ch,SV->AudioVol);
#endif

#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)//frame base
        if((ABC->fifo_count+(ADPCM_CHX_AUDIO_BUF_SIZE-ADPCM_CHX_AUDIO_OUT_SIZE)-ABC->process_count)>=0){
            DecodeFrame(ch);
        }
#else //sample base
		U16 bufctl_rd_offset = BC->rd_offset;
        if(bufctl_rd_offset!=BC->wr_offset){
            if(bufctl_rd_offset>BC->wr_offset){
                size=bufctl_rd_offset-BC->wr_offset;    
            }else{
                size=ADPCM_CHX_DATA_BUF_SIZE-BC->wr_offset;  
            } 
            #if PR_READ_CHX	
            if((size>=ADPCM_CHX_DATA_BUF_BASE_SIZE)||(bufctl_rd_offset<BC->wr_offset))
            {
            #endif            
			    CB_ReadData(SV->StorageMode,&(SV->CurAddr),buf+BC->wr_offset,size); 
			    BC->wr_offset+=size;
			    if(BC->wr_offset==ADPCM_CHX_DATA_BUF_SIZE){
			    	BC->wr_offset=0;    
			    }         
            #if PR_READ_CHX	
            }
            #endif    
        }
#endif            
    }   
#if _USE_HEAP_RAM                            
    if(ABC->shadow_state==STATUS_STOP && buf!=NULL)
    {
        ADPCM_FreeMemory(MEMORY_ALLOCATE_PLAY,ch);
    }
#endif
    if(ABC->shadow_state!=STATUS_PLAYING &&  _DacAssigner_codecUseDacCh(ch)>=0)
    {
		GIE_DISABLE();
		_DacAssigner_unRegist(ch);
		GIE_ENABLE();
    }

	if (SV->PlayEndFlag == BIT_SET) {
		SV->PlayEndFlag = BIT_UNSET;
		CB_ADPCM_CHX_PlayEnd(ch);
	}
}  
  
//================== ADPCM CH2 =====================================//
#if _ADPCM_CH2_PLAY
//------------------------------------------------------------------//
// Get 1 byte, called by adpcm decode, CH2
// Parameter: 
//          NONE
// return value:
//          data
//------------------------------------------------------------------//
S8 ADPCM1GetByte(void){
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)&&(!PR_READ_CHX) 
	return _ADPCMGetByte(BufCtl_CH2,StaticVar_CH2);
#else
	return _ADPCMGetByte(BufCtl_CH2,StaticVar_CH2,EncDataBuf_CH2);
#endif	
}

//------------------------------------------------------------------//
// Dac ISR of CH2
// Parameter: 
//          size: fifo fill size 
//       pdest32: fifo address
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH2_DacIsr(U8 size,S16 *buf){
	ADPCM_CHX_DacIsr(size,buf,AudioBufCtl_CH2,BufCtl_CH2,&SmoothCtl_CH2,StaticVar_CH2,1);
}

//------------------------------------------------------------------//
// Start play voice of CH2
// Parameter: 
//          index	: file index or absolute address
//          mode	: OTP_MODE,EF_MODE,SPI_MODE,SPI1_MODE,EF_UD_MODE
// return value:
//          0:ok
//          others:fail
//------------------------------------------------------------------// 
U8 ADPCM_CH2_Play(U32 index,U8 mode){
	U8 state;
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
#if PR_READ_CHX
	state=ADPCM_CHX_Play(index,mode,AudioBufCtl_CH2,BufCtl_CH2,StaticVar_CH2,AudioBuf_CH2,ModeBuf_CH2,DecBuf_CH2,EncDataBuf_CH2,1);
#else
	state=ADPCM_CHX_Play(index,mode,AudioBufCtl_CH2,BufCtl_CH2,StaticVar_CH2,AudioBuf_CH2,ModeBuf_CH2,DecBuf_CH2,1);
#endif
	return state;
#else
	state=ADPCM_CHX_Play(index,mode,AudioBufCtl_CH2,BufCtl_CH2,StaticVar_CH2,EncDataBuf_CH2,ModeBuf_CH2,DecBuf_CH2,1);
	return state;
#endif	

}
//------------------------------------------------------------------//
// Pause playing of CH2
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH2_Pause(void){
 	ADPCM_CHX_Pause(AudioBufCtl_CH2,1);
} 

//------------------------------------------------------------------//
// Resume playing of CH2
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH2_Resume(void){
	ADPCM_CHX_Resume(AudioBufCtl_CH2,StaticVar_CH2,1);
} 

//------------------------------------------------------------------//
// Stop playing of CH2
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH2_Stop(void){
    ADPCM_CHX_Stop(AudioBufCtl_CH2,StaticVar_CH2,1);
} 

//------------------------------------------------------------------//
// Get current status of CH2
// Parameter: 
//          NONE
// return value:
//          status:STATUS_PLAYING,STATUS_PAUSE,STATUS_STOP
//------------------------------------------------------------------// 
U8 ADPCM_CH2_GetStatus(void){
    return _ADPCM_CHX_GetStatus(AudioBufCtl_CH2);    
}

#if defined(_ADPCM_CH2_PLAY_BACK_USE_CUSTOMIZE)
//------------------------------------------------------------------//
// Set customize filter index of CH2
// Parameter: 
//          index: (0~FILTER_NUMBER-1)User define filter, (other)All Pass
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH2_SetEQFilter(U8 index){
	ADPCM_CHX_SetEQFilter(EQFilterCtl_CH2,index);
}
#endif

//------------------------------------------------------------------//
// Set audio volume of CH2
// Parameter: 
//          vol:0~15
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH2_SetVolume(U8 vol){   
	ADPCM_CHX_SetVolume(StaticVar_CH2,vol);
}

//------------------------------------------------------------------//
// Set audio MixCtrl for CH2
// Parameter: 
//          mixCtrl:MIX_CTRL_0 / MIX_CTRL_25 / MIX_CTRL_50 / MIX_CTRL_75 / MIX_CTRL_100
// return value:
//          NONE
//------------------------------------------------------------------// 
#if _AUDIO_MIX_CONTROL
void ADPCM_CH2_SetMixCtrl(U8 mixCtrl){
    ADPCM_CHX_SetMixCtrl(mixCtrl,1);
}
#endif

//------------------------------------------------------------------//
// Codec init of ADPCM CH2
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH2_Init(void){
	ADPCM_CHX_Init(StaticVar_CH2,1);
}

//------------------------------------------------------------------//
// Service loop of CH2
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
void ADPCM_CH2_ServiceLoop(void){
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)	
	ADPCM_CHX_ServiceLoop(AudioBufCtl_CH2,BufCtl_CH2,StaticVar_CH2,AudioBuf_CH2,1);
#else
	ADPCM_CHX_ServiceLoop(AudioBufCtl_CH2,BufCtl_CH2,StaticVar_CH2,EncDataBuf_CH2,1);
#endif
}

#endif//_ADPCM_CH2_PLAY

//================== ADPCM CH3 =====================================//
#if _ADPCM_CH3_PLAY
//------------------------------------------------------------------//
// Get 1 byte, called by adpcm decode, CH3
// Parameter: 
//          NONE
// return value:
//          data
//------------------------------------------------------------------//
S8 ADPCM2GetByte(void){
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)&&(!PR_READ_CHX) 
	return _ADPCMGetByte(BufCtl_CH3,StaticVar_CH3);
#else
	return _ADPCMGetByte(BufCtl_CH3,StaticVar_CH3,EncDataBuf_CH3);	
#endif	
}

//------------------------------------------------------------------//
// Dac ISR of CH3
// Parameter: 
//          size: fifo fill size 
//       pdest32: fifo address
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH3_DacIsr(U8 size,S16 *buf){
	ADPCM_CHX_DacIsr(size,buf,AudioBufCtl_CH3,BufCtl_CH3,&SmoothCtl_CH3,StaticVar_CH3,2);
}

//------------------------------------------------------------------//
// Start play voice of of CH3
// Parameter: 
//          index	: file index or absolute address
//          mode	: OTP_MODE,EF_MODE,SPI_MODE,SPI1_MODE,EF_UD_MODE
// return value:
//          0:ok
//          others:fail
//------------------------------------------------------------------// 
U8 ADPCM_CH3_Play(U32 index,U8 mode){
	U8 state;	
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
#if PR_READ_CHX
	state=ADPCM_CHX_Play(index,mode,AudioBufCtl_CH3,BufCtl_CH3,StaticVar_CH3,AudioBuf_CH3,ModeBuf_CH3,DecBuf_CH3,EncDataBuf_CH3,2);
#else
	state=ADPCM_CHX_Play(index,mode,AudioBufCtl_CH3,BufCtl_CH3,StaticVar_CH3,AudioBuf_CH3,ModeBuf_CH3,DecBuf_CH3,2);
#endif	
	return state;
#else
	state=ADPCM_CHX_Play(index,mode,AudioBufCtl_CH3,BufCtl_CH3,StaticVar_CH3,EncDataBuf_CH3,ModeBuf_CH3,DecBuf_CH3,2);
	return state;
#endif	

}
//------------------------------------------------------------------//
// Pause playing of CH3
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH3_Pause(void){
 	ADPCM_CHX_Pause(AudioBufCtl_CH3,2);
} 

//------------------------------------------------------------------//
// Resume playing of CH3
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH3_Resume(void){
	ADPCM_CHX_Resume(AudioBufCtl_CH3,StaticVar_CH3,2);
} 

//------------------------------------------------------------------//
// Stop playing of CH3
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH3_Stop(void){
    ADPCM_CHX_Stop(AudioBufCtl_CH3,StaticVar_CH3,2);  
} 

//------------------------------------------------------------------//
// Get current status of CH3
// Parameter: 
//          NONE
// return value:
//          status:STATUS_PLAYING,STATUS_PAUSE,STATUS_STOP
//------------------------------------------------------------------// 
U8 ADPCM_CH3_GetStatus(void){
    return _ADPCM_CHX_GetStatus(AudioBufCtl_CH3);    
}

#if defined(_ADPCM_CH3_PLAY_BACK_USE_CUSTOMIZE)
//------------------------------------------------------------------//
// Set customize filter index of CH3
// Parameter: 
//          index: (0~FILTER_NUMBER-1)User define filter, (other)All Pass
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH3_SetEQFilter(U8 index){
	ADPCM_CHX_SetEQFilter(EQFilterCtl_CH3,index);
}
#endif


//------------------------------------------------------------------//
// Set audio volume of CH3
// Parameter: 
//          vol:0~15
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH3_SetVolume(U8 vol){   
	ADPCM_CHX_SetVolume(StaticVar_CH3,vol);
}

//------------------------------------------------------------------//
// Set audio MixCtrl for CH3
// Parameter: 
//          mixCtrl:MIX_CTRL_0 / MIX_CTRL_25 / MIX_CTRL_50 / MIX_CTRL_75 / MIX_CTRL_100
// return value:
//          NONE
//------------------------------------------------------------------// 
#if _AUDIO_MIX_CONTROL
void ADPCM_CH3_SetMixCtrl(U8 mixCtrl){
    ADPCM_CHX_SetMixCtrl(mixCtrl,2);
}
#endif

//------------------------------------------------------------------//
// Codec init of ADPCM CH3
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH3_Init(void){
	ADPCM_CHX_Init(StaticVar_CH3,2);
}

//------------------------------------------------------------------//
// Service loop of CH3
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
void ADPCM_CH3_ServiceLoop(void){
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)	
	ADPCM_CHX_ServiceLoop(AudioBufCtl_CH3,BufCtl_CH3,StaticVar_CH3,AudioBuf_CH3,2);	
#else
	ADPCM_CHX_ServiceLoop(AudioBufCtl_CH3,BufCtl_CH3,StaticVar_CH3,EncDataBuf_CH3,2);
#endif
}
#endif//_ADPCM_CH3_PLAY

//================== ADPCM CH4 =====================================//
#if _ADPCM_CH4_PLAY
//------------------------------------------------------------------//
// Get 1 byte, called by adpcm decode, CH4
// Parameter: 
//          NONE
// return value:
//          data
//------------------------------------------------------------------//
S8 ADPCM3GetByte(void){
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)&&(!PR_READ_CHX) 
	return _ADPCMGetByte(BufCtl_CH4,StaticVar_CH4);
#else
	return _ADPCMGetByte(BufCtl_CH4,StaticVar_CH4,EncDataBuf_CH4);	
#endif	
}

//------------------------------------------------------------------//
// Dac ISR of CH4
// Parameter: 
//          size: fifo fill size 
//       pdest32: fifo address
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH4_DacIsr(U8 size,S16 *buf){
	ADPCM_CHX_DacIsr(size,buf,AudioBufCtl_CH4,BufCtl_CH4,&SmoothCtl_CH4,StaticVar_CH4,3);
}

//------------------------------------------------------------------//
// Start play voice of of CH4
// Parameter: 
//          index	: file index or absolute address
//          mode	: OTP_MODE,EF_MODE,SPI_MODE,SPI1_MODE,EF_UD_MODE
// return value:
//          0:ok
//          others:fail
//------------------------------------------------------------------// 
U8 ADPCM_CH4_Play(U32 index,U8 mode){
	U8 state;	
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
#if PR_READ_CHX
	state=ADPCM_CHX_Play(index,mode,AudioBufCtl_CH4,BufCtl_CH4,StaticVar_CH4,AudioBuf_CH4,ModeBuf_CH4,DecBuf_CH4,EncDataBuf_CH4,3);
#else
	state=ADPCM_CHX_Play(index,mode,AudioBufCtl_CH4,BufCtl_CH4,StaticVar_CH4,AudioBuf_CH4,ModeBuf_CH4,DecBuf_CH4,3);
#endif	
	return state;
#else
	state=ADPCM_CHX_Play(index,mode,AudioBufCtl_CH4,BufCtl_CH4,StaticVar_CH4,EncDataBuf_CH4,ModeBuf_CH4,DecBuf_CH4,3);
	return state;
#endif	

}
//------------------------------------------------------------------//
// Pause playing of CH4
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH4_Pause(void){
 	ADPCM_CHX_Pause(AudioBufCtl_CH4,3);
} 

//------------------------------------------------------------------//
// Resume playing of CH4
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH4_Resume(void){
	ADPCM_CHX_Resume(AudioBufCtl_CH4,StaticVar_CH4,3);
} 

//------------------------------------------------------------------//
// Stop playing of CH4
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH4_Stop(void){
    ADPCM_CHX_Stop(AudioBufCtl_CH4,StaticVar_CH4,3);  
} 

//------------------------------------------------------------------//
// Get current status of CH4
// Parameter: 
//          NONE
// return value:
//          status:STATUS_PLAYING,STATUS_PAUSE,STATUS_STOP
//------------------------------------------------------------------// 
U8 ADPCM_CH4_GetStatus(void){
    return _ADPCM_CHX_GetStatus(AudioBufCtl_CH4);    
}

#if defined(_ADPCM_CH4_PLAY_BACK_USE_CUSTOMIZE)
//------------------------------------------------------------------//
// Set customize filter index of CH4
// Parameter: 
//          index: (0~FILTER_NUMBER-1)User define filter, (other)All Pass
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH4_SetEQFilter(U8 index){
	ADPCM_CHX_SetEQFilter(EQFilterCtl_CH4,index);
}
#endif


//------------------------------------------------------------------//
// Set audio volume of CH4
// Parameter: 
//          vol:0~15
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH4_SetVolume(U8 vol){   
	ADPCM_CHX_SetVolume(StaticVar_CH4,vol);
}

//------------------------------------------------------------------//
// Set audio MixCtrl for CH4
// Parameter: 
//          mixCtrl:MIX_CTRL_0 / MIX_CTRL_25 / MIX_CTRL_50 / MIX_CTRL_75 / MIX_CTRL_100
// return value:
//          NONE
//------------------------------------------------------------------// 
#if _AUDIO_MIX_CONTROL
void ADPCM_CH4_SetMixCtrl(U8 mixCtrl){
    ADPCM_CHX_SetMixCtrl(mixCtrl,3);
}
#endif

//------------------------------------------------------------------//
// Codec init of ADPCM CH4
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH4_Init(void){
	ADPCM_CHX_Init(StaticVar_CH4,3);
}

//------------------------------------------------------------------//
// Service loop of CH4
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
void ADPCM_CH4_ServiceLoop(void){
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)	
	ADPCM_CHX_ServiceLoop(AudioBufCtl_CH4,BufCtl_CH4,StaticVar_CH4,AudioBuf_CH4,3);	
#else
	ADPCM_CHX_ServiceLoop(AudioBufCtl_CH4,BufCtl_CH4,StaticVar_CH4,EncDataBuf_CH4,3);
#endif
}
#endif//_ADPCM_CH4_PLAY

//================== ADPCM CH5 =====================================//
#if _ADPCM_CH5_PLAY
//------------------------------------------------------------------//
// Get 1 byte, called by adpcm decode, CH5
// Parameter: 
//          NONE
// return value:
//          data
//------------------------------------------------------------------//
S8 ADPCM4GetByte(void){
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)&&(!PR_READ_CHX) 
	return _ADPCMGetByte(BufCtl_CH5,StaticVar_CH5);
#else
	return _ADPCMGetByte(BufCtl_CH5,StaticVar_CH5,EncDataBuf_CH5);	
#endif	
}

//------------------------------------------------------------------//
// Dac ISR of CH5
// Parameter: 
//          size: fifo fill size 
//       pdest32: fifo address
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH5_DacIsr(U8 size,S16 *buf){
	ADPCM_CHX_DacIsr(size,buf,AudioBufCtl_CH5,BufCtl_CH5,&SmoothCtl_CH5,StaticVar_CH5,4);
}

//------------------------------------------------------------------//
// Start play voice of of CH5
// Parameter: 
//          index	: file index or absolute address
//          mode	: OTP_MODE,EF_MODE,SPI_MODE,SPI1_MODE,EF_UD_MODE
// return value:
//          0:ok
//          others:fail
//------------------------------------------------------------------// 
U8 ADPCM_CH5_Play(U32 index,U8 mode){
	U8 state;	
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
#if PR_READ_CHX
	state=ADPCM_CHX_Play(index,mode,AudioBufCtl_CH5,BufCtl_CH5,StaticVar_CH5,AudioBuf_CH5,ModeBuf_CH5,DecBuf_CH5,EncDataBuf_CH5,4);
#else
	state=ADPCM_CHX_Play(index,mode,AudioBufCtl_CH5,BufCtl_CH5,StaticVar_CH5,AudioBuf_CH5,ModeBuf_CH5,DecBuf_CH5,4);
#endif	
	return state;
#else
	state=ADPCM_CHX_Play(index,mode,AudioBufCtl_CH5,BufCtl_CH5,StaticVar_CH5,EncDataBuf_CH5,ModeBuf_CH5,DecBuf_CH5,4);
	return state;
#endif	
	
}
//------------------------------------------------------------------//
// Pause playing of CH5
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH5_Pause(void){
 	ADPCM_CHX_Pause(AudioBufCtl_CH5,4);
} 

//------------------------------------------------------------------//
// Resume playing of CH5
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH5_Resume(void){
	ADPCM_CHX_Resume(AudioBufCtl_CH5,StaticVar_CH5,4);
} 

//------------------------------------------------------------------//
// Stop playing of CH5
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH5_Stop(void){
    ADPCM_CHX_Stop(AudioBufCtl_CH5,StaticVar_CH5,4);
} 

//------------------------------------------------------------------//
// Get current status of CH5
// Parameter: 
//          NONE
// return value:
//          status:STATUS_PLAYING,STATUS_PAUSE,STATUS_STOP
//------------------------------------------------------------------// 
U8 ADPCM_CH5_GetStatus(void){
    return _ADPCM_CHX_GetStatus(AudioBufCtl_CH5);    
}

#if defined(_ADPCM_CH5_PLAY_BACK_USE_CUSTOMIZE)
//------------------------------------------------------------------//
// Set customize filter index of CH5
// Parameter: 
//          index: (0~FILTER_NUMBER-1)User define filter, (other)All Pass
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH5_SetEQFilter(U8 index){
	ADPCM_CHX_SetEQFilter(EQFilterCtl_CH5,index);
}
#endif


//------------------------------------------------------------------//
// Set audio volume of CH5
// Parameter: 
//          vol:0~15
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH5_SetVolume(U8 vol){   
	ADPCM_CHX_SetVolume(StaticVar_CH5,vol);
}

//------------------------------------------------------------------//
// Set audio MixCtrl for CH5
// Parameter: 
//          mixCtrl:MIX_CTRL_0 / MIX_CTRL_25 / MIX_CTRL_50 / MIX_CTRL_75 / MIX_CTRL_100
// return value:
//          NONE
//------------------------------------------------------------------// 
#if _AUDIO_MIX_CONTROL
void ADPCM_CH5_SetMixCtrl(U8 mixCtrl){
    ADPCM_CHX_SetMixCtrl(mixCtrl,4);
}
#endif

//------------------------------------------------------------------//
// Codec init of ADPCM CH5
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH5_Init(void){
	ADPCM_CHX_Init(StaticVar_CH5,4);
}

//------------------------------------------------------------------//
// Service loop of CH5
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
void ADPCM_CH5_ServiceLoop(void){
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)	
	ADPCM_CHX_ServiceLoop(AudioBufCtl_CH5,BufCtl_CH5,StaticVar_CH5,AudioBuf_CH5,4);	
#else
	ADPCM_CHX_ServiceLoop(AudioBufCtl_CH5,BufCtl_CH5,StaticVar_CH5,EncDataBuf_CH5,4);
#endif
}
#endif//_ADPCM_CH5_PLAY

//================== ADPCM CH6 =====================================//
#if _ADPCM_CH6_PLAY
//------------------------------------------------------------------//
// Get 1 byte, called by adpcm decode, CH6
// Parameter: 
//          NONE
// return value:
//          data
//------------------------------------------------------------------//
S8 ADPCM5GetByte(void){
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)&&(!PR_READ_CHX) 
	return _ADPCMGetByte(BufCtl_CH6,StaticVar_CH6);
#else
	return _ADPCMGetByte(BufCtl_CH6,StaticVar_CH6,EncDataBuf_CH6);	
#endif	
}

//------------------------------------------------------------------//
// Dac ISR of CH6
// Parameter: 
//          size: fifo fill size 
//       pdest32: fifo address
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH6_DacIsr(U8 size,S16 *buf){
	ADPCM_CHX_DacIsr(size,buf,AudioBufCtl_CH6,BufCtl_CH6,&SmoothCtl_CH6,StaticVar_CH6,5);
}

//------------------------------------------------------------------//
// Start play voice of of CH6
// Parameter: 
//          index	: file index or absolute address
//          mode	: OTP_MODE,EF_MODE,SPI_MODE,SPI1_MODE,EF_UD_MODE
// return value:
//          0:ok
//          others:fail
//------------------------------------------------------------------// 
U8 ADPCM_CH6_Play(U32 index,U8 mode){
	U8 state;	
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
#if PR_READ_CHX
	state=ADPCM_CHX_Play(index,mode,AudioBufCtl_CH6,BufCtl_CH6,StaticVar_CH6,AudioBuf_CH6,ModeBuf_CH6,DecBuf_CH6,EncDataBuf_CH6,5);
#else
	state=ADPCM_CHX_Play(index,mode,AudioBufCtl_CH6,BufCtl_CH6,StaticVar_CH6,AudioBuf_CH6,ModeBuf_CH6,DecBuf_CH6,5);
#endif	
	return state;
#else
	state=ADPCM_CHX_Play(index,mode,AudioBufCtl_CH6,BufCtl_CH6,StaticVar_CH6,EncDataBuf_CH6,ModeBuf_CH6,DecBuf_CH6,5);
	return state;
#endif	
	
}
//------------------------------------------------------------------//
// Pause playing of CH6
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH6_Pause(void){
 	ADPCM_CHX_Pause(AudioBufCtl_CH6,5);
} 

//------------------------------------------------------------------//
// Resume playing of CH6
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH6_Resume(void){
	ADPCM_CHX_Resume(AudioBufCtl_CH6,StaticVar_CH6,5);
} 

//------------------------------------------------------------------//
// Stop playing of CH6
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH6_Stop(void){
    ADPCM_CHX_Stop(AudioBufCtl_CH6,StaticVar_CH6,5);  
} 

//------------------------------------------------------------------//
// Get current status of CH6
// Parameter: 
//          NONE
// return value:
//          status:STATUS_PLAYING,STATUS_PAUSE,STATUS_STOP
//------------------------------------------------------------------// 
U8 ADPCM_CH6_GetStatus(void){
    return _ADPCM_CHX_GetStatus(AudioBufCtl_CH6);    
}

#if defined(_ADPCM_CH6_PLAY_BACK_USE_CUSTOMIZE)
//------------------------------------------------------------------//
// Set customize filter index of CH6
// Parameter: 
//          index: (0~FILTER_NUMBER-1)User define filter, (other)All Pass
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH6_SetEQFilter(U8 index){
	ADPCM_CHX_SetEQFilter(EQFilterCtl_CH6,index);
}
#endif


//------------------------------------------------------------------//
// Set audio volume of CH6
// Parameter: 
//          vol:0~15
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH6_SetVolume(U8 vol){   
	ADPCM_CHX_SetVolume(StaticVar_CH6,vol);
}

//------------------------------------------------------------------//
// Set audio MixCtrl for CH6
// Parameter: 
//          mixCtrl:MIX_CTRL_0 / MIX_CTRL_25 / MIX_CTRL_50 / MIX_CTRL_75 / MIX_CTRL_100
// return value:
//          NONE
//------------------------------------------------------------------// 
#if _AUDIO_MIX_CONTROL
void ADPCM_CH6_SetMixCtrl(U8 mixCtrl){
    ADPCM_CHX_SetMixCtrl(mixCtrl,5);
}
#endif

//------------------------------------------------------------------//
// Codec init of ADPCM CH6
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH6_Init(void){
	ADPCM_CHX_Init(StaticVar_CH6,5);
}

//------------------------------------------------------------------//
// Service loop of CH6
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
void ADPCM_CH6_ServiceLoop(void){
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)	
	ADPCM_CHX_ServiceLoop(AudioBufCtl_CH6,BufCtl_CH6,StaticVar_CH6,AudioBuf_CH6,5);	
#else
	ADPCM_CHX_ServiceLoop(AudioBufCtl_CH6,BufCtl_CH6,StaticVar_CH6,EncDataBuf_CH6,5);
#endif
}
#endif//_ADPCM_CH6_PLAY

//================== ADPCM CH7 =====================================//
#if _ADPCM_CH7_PLAY
//------------------------------------------------------------------//
// Get 1 byte, called by adpcm decode, CH7
// Parameter: 
//          NONE
// return value:
//          data
//------------------------------------------------------------------//
S8 ADPCM6GetByte(void){
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)&&(!PR_READ_CHX) 
	return _ADPCMGetByte(BufCtl_CH7,StaticVar_CH7);
#else
	return _ADPCMGetByte(BufCtl_CH7,StaticVar_CH7,EncDataBuf_CH7);	
#endif	
}

//------------------------------------------------------------------//
// Dac ISR of CH7
// Parameter: 
//          size: fifo fill size 
//       pdest32: fifo address
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH7_DacIsr(U8 size,S16 *buf){
	ADPCM_CHX_DacIsr(size,buf,AudioBufCtl_CH7,BufCtl_CH7,&SmoothCtl_CH7,StaticVar_CH7,6);
}

//------------------------------------------------------------------//
// Start play voice of of CH7
// Parameter: 
//          index	: file index or absolute address
//          mode	: OTP_MODE,EF_MODE,SPI_MODE,SPI1_MODE,EF_UD_MODE
// return value:
//          0:ok
//          others:fail
//------------------------------------------------------------------// 
U8 ADPCM_CH7_Play(U32 index,U8 mode){
	U8 state;	
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
#if PR_READ_CHX
	state=ADPCM_CHX_Play(index,mode,AudioBufCtl_CH7,BufCtl_CH7,StaticVar_CH7,AudioBuf_CH7,ModeBuf_CH7,DecBuf_CH7,EncDataBuf_CH7,6);
#else
	state=ADPCM_CHX_Play(index,mode,AudioBufCtl_CH7,BufCtl_CH7,StaticVar_CH7,AudioBuf_CH7,ModeBuf_CH7,DecBuf_CH7,6);
#endif	
	return state;
#else
	state=ADPCM_CHX_Play(index,mode,AudioBufCtl_CH7,BufCtl_CH7,StaticVar_CH7,EncDataBuf_CH7,ModeBuf_CH7,DecBuf_CH7,6);
	return state;
#endif	

}
//------------------------------------------------------------------//
// Pause playing of CH7
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH7_Pause(void){
 	ADPCM_CHX_Pause(AudioBufCtl_CH7,6);
} 

//------------------------------------------------------------------//
// Resume playing of CH7
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH7_Resume(void){
	ADPCM_CHX_Resume(AudioBufCtl_CH7,StaticVar_CH7,6);
} 

//------------------------------------------------------------------//
// Stop playing of CH7
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH7_Stop(void){
    ADPCM_CHX_Stop(AudioBufCtl_CH7,StaticVar_CH7,6);  
} 

//------------------------------------------------------------------//
// Get current status of CH7
// Parameter: 
//          NONE
// return value:
//          status:STATUS_PLAYING,STATUS_PAUSE,STATUS_STOP
//------------------------------------------------------------------// 
U8 ADPCM_CH7_GetStatus(void){
    return _ADPCM_CHX_GetStatus(AudioBufCtl_CH7);    
}

#if defined(_ADPCM_CH7_PLAY_BACK_USE_CUSTOMIZE)
//------------------------------------------------------------------//
// Set customize filter index of CH7
// Parameter: 
//          index: (0~FILTER_NUMBER-1)User define filter, (other)All Pass
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH7_SetEQFilter(U8 index){
	ADPCM_CHX_SetEQFilter(EQFilterCtl_CH7,index);
}
#endif


//------------------------------------------------------------------//
// Set audio volume of CH7
// Parameter: 
//          vol:0~15
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH7_SetVolume(U8 vol){   
	ADPCM_CHX_SetVolume(StaticVar_CH7,vol);
}

//------------------------------------------------------------------//
// Set audio MixCtrl for CH7
// Parameter: 
//          mixCtrl:MIX_CTRL_0 / MIX_CTRL_25 / MIX_CTRL_50 / MIX_CTRL_75 / MIX_CTRL_100
// return value:
//          NONE
//------------------------------------------------------------------// 
#if _AUDIO_MIX_CONTROL
void ADPCM_CH7_SetMixCtrl(U8 mixCtrl){
    ADPCM_CHX_SetMixCtrl(mixCtrl,6);
}
#endif

//------------------------------------------------------------------//
// Codec init of ADPCM CH7
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH7_Init(void){
	ADPCM_CHX_Init(StaticVar_CH7,6);
}

//------------------------------------------------------------------//
// Service loop of CH7
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
void ADPCM_CH7_ServiceLoop(void){
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)	
	ADPCM_CHX_ServiceLoop(AudioBufCtl_CH7,BufCtl_CH7,StaticVar_CH7,AudioBuf_CH7,6);	
#else
	ADPCM_CHX_ServiceLoop(AudioBufCtl_CH7,BufCtl_CH7,StaticVar_CH7,EncDataBuf_CH7,6);
#endif
}
#endif//_ADPCM_CH7_PLAY

//================== ADPCM CH8 =====================================//
#if _ADPCM_CH8_PLAY
//------------------------------------------------------------------//
// Get 1 byte, called by adpcm decode, CH8
// Parameter: 
//          NONE
// return value:
//          data
//------------------------------------------------------------------//
S8 ADPCM7GetByte(void){
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)&&(!PR_READ_CHX) 
	return _ADPCMGetByte(BufCtl_CH8,StaticVar_CH8);
#else
	return _ADPCMGetByte(BufCtl_CH8,StaticVar_CH8,EncDataBuf_CH8);	
#endif	
}

//------------------------------------------------------------------//
// Dac ISR of CH8
// Parameter: 
//          size: fifo fill size 
//       pdest32: fifo address
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH8_DacIsr(U8 size,S16 *buf){
	ADPCM_CHX_DacIsr(size,buf,AudioBufCtl_CH8,BufCtl_CH8,&SmoothCtl_CH8,StaticVar_CH8,7);
}

//------------------------------------------------------------------//
// Start play voice of of CH8
// Parameter: 
//          index	: file index or absolute address
//          mode	: OTP_MODE,EF_MODE,SPI_MODE,SPI1_MODE,EF_UD_MODE
// return value:
//          0:ok
//          others:fail
//------------------------------------------------------------------// 
U8 ADPCM_CH8_Play(U32 index,U8 mode){
	U8 state;	
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)
#if PR_READ_CHX
	state=ADPCM_CHX_Play(index,mode,AudioBufCtl_CH8,BufCtl_CH8,StaticVar_CH8,AudioBuf_CH8,ModeBuf_CH8,DecBuf_CH8,EncDataBuf_CH8,7);
#else
	state=ADPCM_CHX_Play(index,mode,AudioBufCtl_CH8,BufCtl_CH8,StaticVar_CH8,AudioBuf_CH8,ModeBuf_CH8,DecBuf_CH8,7);
#endif	
	return state;
#else
	state=ADPCM_CHX_Play(index,mode,AudioBufCtl_CH8,BufCtl_CH8,StaticVar_CH8,EncDataBuf_CH8,ModeBuf_CH8,DecBuf_CH8,7);
	return state;
#endif	
    
}
//------------------------------------------------------------------//
// Pause playing of CH8
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH8_Pause(void){
 	ADPCM_CHX_Pause(AudioBufCtl_CH8,7);
} 

//------------------------------------------------------------------//
// Resume playing of CH8
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH8_Resume(void){
	ADPCM_CHX_Resume(AudioBufCtl_CH8,StaticVar_CH8,7);
} 

//------------------------------------------------------------------//
// Stop playing of CH8
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH8_Stop(void){
    ADPCM_CHX_Stop(AudioBufCtl_CH8,StaticVar_CH8,7);  
} 

//------------------------------------------------------------------//
// Get current status of CH8
// Parameter: 
//          NONE
// return value:
//          status:STATUS_PLAYING,STATUS_PAUSE,STATUS_STOP
//------------------------------------------------------------------// 
U8 ADPCM_CH8_GetStatus(void){
    return _ADPCM_CHX_GetStatus(AudioBufCtl_CH8);    
}

#if defined(_ADPCM_CH8_PLAY_BACK_USE_CUSTOMIZE)
//------------------------------------------------------------------//
// Set customize filter index of CH8
// Parameter: 
//          index: (0~FILTER_NUMBER-1)User define filter, (other)All Pass
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH8_SetEQFilter(U8 index){
	ADPCM_CHX_SetEQFilter(EQFilterCtl_CH8,index);
}
#endif


//------------------------------------------------------------------//
// Set audio volume of CH8
// Parameter: 
//          vol:0~15
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH8_SetVolume(U8 vol){   
	ADPCM_CHX_SetVolume(StaticVar_CH8,vol);
}

//------------------------------------------------------------------//
// Set audio MixCtrl for CH8
// Parameter: 
//          mixCtrl:MIX_CTRL_0 / MIX_CTRL_25 / MIX_CTRL_50 / MIX_CTRL_75 / MIX_CTRL_100
// return value:
//          NONE
//------------------------------------------------------------------// 
#if _AUDIO_MIX_CONTROL
void ADPCM_CH8_SetMixCtrl(U8 mixCtrl){
    ADPCM_CHX_SetMixCtrl(mixCtrl,7);
}
#endif

//------------------------------------------------------------------//
// Codec init of ADPCM CH8
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void ADPCM_CH8_Init(void){
	ADPCM_CHX_Init(StaticVar_CH8,7);
}

//------------------------------------------------------------------//
// Service loop of CH8
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
void ADPCM_CH8_ServiceLoop(void){
#if (_ADPCM_CHX_DECODE_FLOW_TYPE==TYPE1)	
	ADPCM_CHX_ServiceLoop(AudioBufCtl_CH8,BufCtl_CH8,StaticVar_CH8,AudioBuf_CH8,7);	
#else
	ADPCM_CHX_ServiceLoop(AudioBufCtl_CH8,BufCtl_CH8,StaticVar_CH8,EncDataBuf_CH8,7);
#endif
}
#endif//_ADPCM_CH8_PLAY

#endif//_ADPCM_CHX_PLAY
