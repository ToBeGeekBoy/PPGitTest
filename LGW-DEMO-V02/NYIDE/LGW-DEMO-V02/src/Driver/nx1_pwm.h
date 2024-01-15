#ifndef NX1_PWM_H
#define NX1_PWM_H

#include "nx1_reg.h"
#if	defined(P_PWMA)||defined(P_PWMB)
//////////////////////////////////////////////////////////////////////
// Constant Define
//////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------//
//  Function   : PWM_Init, PWM_InitDuty, PWM_Cmd, PWM_SetDuty
//  Parameter  : pwmx
//------------------------------------------------------------------//
#if	defined(P_PWMA)
#define PWMA                        P_PWMA
#endif
#if	defined(P_PWMB)
#define PWMB                        P_PWMB
#endif

//------------------------------------------------------------------//
//  Function   : PWM_Init, PWM_InitDuty
//  Parameter  : mode
//------------------------------------------------------------------//
#define PWM_START_HIGH              C_PWM_Duty_Pol_High
#define PWM_START_LOW               C_PWM_Duty_Pol_Low

//------------------------------------------------------------------//
//  Function   : PWM_CaptureIntCmd
//  Parameter  : irq
//------------------------------------------------------------------//
#define PWM_INT_CAPTURE_RISING      C_PWMA_CAP_Rising_IEn
#define PWM_INT_CAPTURE_FALLING     C_PWMA_CAP_Falling_IEn

//------------------------------------------------------------------//
//  Function   : PWMA_TMR_Ctrl
//  Parameter  : mode (Low power IR)
//------------------------------------------------------------------//
#define PWMA_IRWK_MASK_DIN_ToHi     C_PWMA_IRWK_MASK_DIN_ToHi
#define PWMA_IRWK_MASK_DIN_ToLo     C_PWMA_IRWK_MASK_DIN_ToLo
#define PWMA_IRWK_NO_MASK           C_PWMA_IRWK_NO_MASK

#define PWMA_CLK_Src_HiCLK          C_PWMA_CLK_Src_HiCLK
#define PWMA_CLK_Src_LoCLK          C_PWMA_CLK_Src_LoCLK

//------------------------------------------------------------------//
//  Function   : PWM_Init, PWM_InitDuty, PWM_Cmd, PWM_SetDuty
//  Parameter  : idx
//------------------------------------------------------------------//
#define  PWMx0                      0
#define  PWMx1                      1
#define  PWMx2                      2
#define  PWMx3                      3

//------------------------------------------------------------------//
//  Function   : PWM_SetDuty
//  Parameter  : duty
//------------------------------------------------------------------//
#define  PWM_DUTY_16BIT             0x80000000
#define  MAX_UINT16		 	        65535


//////////////////////////////////////////////////////////////////////
// External Function
//////////////////////////////////////////////////////////////////////

extern void PWM_Init(PWM_TypeDef *pwmx,U8 idx,U8 mode,U16 count);
extern void PWM_InitDuty(PWM_TypeDef *pwmx,U8 idx,U8 mode,U8 duty);
extern void PWM_Cmd(PWM_TypeDef *pwmx,U8 idx,U8 mode,U8 cmd);
extern void PWM_SetDuty(PWM_TypeDef *pwmx,U8 idx,U32 duty);
extern void PWM_CapturePinSelect(U8 pin);
extern void PWM_CaptureCmd(U8 cmd);
extern void PWM_CaptureIntCmd(U8 irq,U8 cmd);
extern  U32 PWM_CaptureData(void);
extern void PWM_CapturePinSelect(U8 pin);



#endif	//#if	defined(P_PWMA)||defined(P_PWMB)

#endif //#ifndef NX1_PWM_H
