#ifndef NX1_DAC_H
#define NX1_DAC_H

#include "nx1_reg.h"

//////////////////////////////////////////////////////////////////////
// Constant Define
//////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------//
//  Function   : DAC_Init, DAC_Cmd
//  Parameter  : dacx
//------------------------------------------------------------------//
#define DAC_CH0                     ((CH_TypeDef *)P_DAC_CH0_Ctrl)
#define DAC_CH1                     ((CH_TypeDef *)P_DAC_CH1_Ctrl)

//------------------------------------------------------------------//
//  Function   : DAC_Init
//  Parameter  : trig_src
//------------------------------------------------------------------//
#define DAC_MANUAL_TRIG             C_DAC_Trig_Manual
#define DAC_TIMER0_TRIG             C_DAC_Trig_Timer0
#define DAC_TIMER1_TRIG             C_DAC_Trig_Timer1
#define DAC_TIMER2_TRIG             C_DAC_Trig_Timer2

//------------------------------------------------------------------//
//  Function   : DAC_Init
//  Parameter  : fifo_level
//------------------------------------------------------------------//
#if _EF_SERIES
#if EF11FS_SERIES
    #define DAC_FIFO_THD            0
#else
    #define DAC_FIFO_THD            2
#endif
#else
#define DAC_FIFO_THD                4
#endif

//------------------------------------------------------------------//
//  Function   : None
//  Parameter  : DAC Fifo Length
//------------------------------------------------------------------//
#if _EF_SERIES
#if EF11FS_SERIES
    #define DAC_FIFO_LEN            1
#else
    #define DAC_FIFO_LEN            4
#endif
#else
#define DAC_FIFO_LEN                8
#endif

//------------------------------------------------------------------//
//  Function   : None
//  Parameter  : DAC Fifo trigger
//------------------------------------------------------------------//
#define DAC_trig_None	0
#define DAC0_trig		1
#define DAC1_trig		2

//------------------------------------------------------------------//
//  Function   : DAC_SetUpSample, InitDac, SetSampleRate
//  Parameter  : DAC Up Sample Point for EF Series 
//------------------------------------------------------------------//
#if _EF_SERIES
#ifndef _DAC_UP_SAMPLE_POINT
#define _DAC_UP_SAMPLE_POINT        16
#endif
#define _DAC_UP_SAMPLE_SETTING      (_DAC_UP_SAMPLE_POINT== 4?0:	\
									 _DAC_UP_SAMPLE_POINT== 8?1:	\
									 _DAC_UP_SAMPLE_POINT==16?2:	\
									 _DAC_UP_SAMPLE_POINT==32?3:3)
#endif

//////////////////////////////////////////////////////////////////////
// External Function
//////////////////////////////////////////////////////////////////////

extern void DAC_Init(CH_TypeDef * dacx,U8 trig_src,U8 fifo_level);
extern void DAC_SetUpSample(CH_TypeDef *chx,U8 upsample);
extern void DAC_SetPPIpGain(U8 gain);
extern U8   DAC_GetPPIpGain(void);
extern bool DAC_IsPPEnable(void);
extern void DAC_Cmd(CH_TypeDef * dacx,U8 cmd);
extern void DAC_LineInCmd(U8 cmd);
extern void SetNoiseFilter(CH_TypeDef *chx,U8 cmd);
extern void SetPPGain(U8 gain);
extern void SetSampleRate(CH_TypeDef * dacx,U16 sample_rate);
extern U8 DAC_Trig_Timer_disable(void);
extern void DAC_Trig_Timer_enable(U8 DAC_status);
extern void DAC_MODE_Trig_Timer_enable(U8 DAC_status,U8 upsample);
extern void DAC_PPCmd(U8 cmd);
extern void DAC_PWMCmd(U8 cmd);
extern void DAC_VoltageDacCmd(U8 cmd);
extern void DacRamp(CH_TypeDef *chx, U8 cmd);
extern void DacMixerRamp(U8 DAC_status,U8 cmd);


#endif //#ifndef NX1_DAC_H
