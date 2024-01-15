#ifndef NX1_ADC_H
#define NX1_ADC_H

#include "nx1_reg.h"
#if		defined(P_ADC)
//////////////////////////////////////////////////////////////////////
// Constant Define
//////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------//
//  Function   : ADC_Init
//  Parameter  : trig_src
//------------------------------------------------------------------//
#define ADC_SW_TRIG                 C_ADC_Trig_Manual
#define ADC_TIMER0_TRIG             C_ADC_Trig_Timer0
#define ADC_TIMER1_TRIG             C_ADC_Trig_Timer1
#define ADC_TIMER2_TRIG             C_ADC_Trig_Timer2

//------------------------------------------------------------------//
//  Function   : ADC_Init, ADC_GetValue
//  Parameter  : channel
//------------------------------------------------------------------//
#define ADC_CH0                     0x0
#define ADC_CH1                     0x1
#define ADC_CH2                     0x2
#define ADC_CH3                     0x3
#define ADC_CH4                     0x4
#define ADC_CH5                     0x5
#define ADC_CH6                     0x6
#define ADC_CH7                     0x7
#define ADC_CH_MIC                  0x8

//------------------------------------------------------------------//
//  Function   : ADC_Init
//  Parameter  : fifo_level
//------------------------------------------------------------------//
#if _EF_SERIES
#define ADC_FIFO_THD                2
#else
#define ADC_FIFO_THD                4
#endif

//------------------------------------------------------------------//
//  function   : ADC_Init
//  Parameter  : fifo_level
//------------------------------------------------------------------//
#if _EF_SERIES
#define ADC_SCAN_DISABLE            C_ADC_Single_Mode
#define ADC_SCAN_2                  C_ADC_Scan_Mode_CH01
#define ADC_SCAN_3                  C_ADC_Scan_Mode_CH0102
#define ADC_SCAN_4                  C_ADC_Scan_Mode_CH010203
#else
#define ADC_SCAN_DISABLE            C_ADC_Single_Mode
#define ADC_SCAN_2                  C_ADC_Scan_Mode_CH10
#define ADC_SCAN_3                  C_ADC_Scan_Mode_CH210
#define ADC_SCAN_4                  C_ADC_Scan_Mode_CH3210
#endif


//------------------------------------------------------------------//
//  Function   : _SL_ADC_Scan_DEF
//  Parameter  : none
//------------------------------------------------------------------//
#if _SL_MODULE
#if _EF_SERIES
#define ADC_SL_CH01             	0xA
#define ADC_SL_CH0102             	0x9
#else
#define ADC_SL_CH10                 0xA
#define ADC_SL_CH210                0x9
#endif
#endif

//------------------------------------------------------------------//
//  Function   : _SL_ADC_Scan_Ch0_FIF0_Size
//  Parameter  : none
//------------------------------------------------------------------//
#if _SL_MODULE
#if _SL_MIC_NUM==3
#define _SL_ADC_FIFO_Level 			3
#elif _SL_MIC_NUM==2
#define _SL_ADC_FIFO_Level 			2
#endif
#endif

//////////////////////////////////////////////////////////////////////
// External Function
//////////////////////////////////////////////////////////////////////

extern void ADC_Init(U8 trig_src,U8 channel,U8 fifo_level);
extern void _SL_ADC_Init(U8 trig_src,U8 fifo_level);
extern void ADC_Cmd(U8 cmd);
extern void ADC_SwTrigStart(void);
extern void ADC_AgCmd(U8 cmd);
extern void ADC_SetGain(U16 gain);
extern U16  ADC_GetValue(U8 channel);
extern void ADC_SetScanMode(U8 mode);
extern void ADC_MicBiasCmd(U8 cmd);
extern void ADC_MicPreGain(U8 gain);

#endif //#if	 defined(P_ADC)

#endif //#ifndef NX1_ADC_H
