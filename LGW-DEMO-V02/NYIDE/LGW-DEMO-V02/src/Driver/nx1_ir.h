#ifndef NX1_IR_H
#define NX1_IR_H

#include "nx1_reg.h"

//////////////////////////////////////////////////////////////////////
// Constant Define
//////////////////////////////////////////////////////////////////////
#define _IR_CLK_BASE                (OPTION_HIGH_FREQ_CLK/2)
#if _EF_SERIES
#define IR0_TX_PORT				    GPIOA
#define IR0_TX_PIN					1
#define IR1_TX_PORT				    GPIOA
#define IR1_TX_PIN					2
#define IR2_TX_PORT				    GPIOC
#define IR2_TX_PIN					0
#define IR3_TX_PORT				    GPIOC
#define IR3_TX_PIN					1
#else
#define IR0_TX_PORT				    GPIOA
#define IR0_TX_PIN					5
#endif
//------------------------------------------------------------------//
//  Function   : IR_Init
//  Parameter  : stop
//------------------------------------------------------------------//
#define IR_STOP_HIGH                1
#define IR_STOP_LOW                 0

//------------------------------------------------------------------//
//  Function   : IR_Init
//  Parameter  : divide
//------------------------------------------------------------------//
#define IR_CLKDIV_1                 C_IR_CLK_Div_1
#define IR_CLKDIV_2                 C_IR_CLK_Div_2
#define IR_CLKDIV_3                 C_IR_CLK_Div_3
#define IR_CLKDIV_4                 C_IR_CLK_Div_4
#define IR_CLKDIV_5                 C_IR_CLK_Div_5
#define IR_CLKDIV_6                 C_IR_CLK_Div_6
#define IR_CLKDIV_7                 C_IR_CLK_Div_7
#define IR_CLKDIV_8                 C_IR_CLK_Div_8
#define IR_CLKDIV_9                 C_IR_CLK_Div_9
#define IR_CLKDIV_10                C_IR_CLK_Div_10
#define IR_CLKDIV_11                C_IR_CLK_Div_11
#define IR_CLKDIV_12                C_IR_CLK_Div_12
#define IR_CLKDIV_13                C_IR_CLK_Div_13
#define IR_CLKDIV_14                C_IR_CLK_Div_14
#define IR_CLKDIV_15                C_IR_CLK_Div_15
#define IR_CLKDIV_16                C_IR_CLK_Div_16

//------------------------------------------------------------------//
//  Function   : IR_Init
//  Parameter  : irIndex
//------------------------------------------------------------------//
typedef enum{
#if (_EF_SERIES && defined(P_TIMER0))  \
    ||(!_EF_SERIES && defined(P_IR))
    USE_IR0_TX,
#endif
#if (_EF_SERIES && defined(P_TIMER1))
    USE_IR1_TX,
#endif
#if (_EF_SERIES && defined(P_TIMER2))
    USE_IR2_TX,
#endif
#if (_EF_SERIES && defined(P_TIMER3))
    USE_IR3_TX,
#endif
	num_USABLE_IR_TX
}IR_TX_SELECT;

//------------------------------------------------------------------//
//  Function   : IR_Init
//  Parameter  : N/A
//------------------------------------------------------------------//
#if _EF_SERIES
typedef const struct {
	TIMER_TypeDef*  timer;
    GPIO_TypeDef*   port;
    U8              pin;
}S_IR_RESOURCE;
extern S_IR_RESOURCE s_IR_Resource[];
#endif

//////////////////////////////////////////////////////////////////////
// External Function
//////////////////////////////////////////////////////////////////////

extern void IR_Init(IR_TX_SELECT irIndex,U8 stop,U32 carryFreq);
extern void IR_Cmd(IR_TX_SELECT irIndex,U8 cmd);
extern void IR_CARRIER_Cmd(IR_TX_SELECT irIndex, U8 cmd);





#endif//#ifndef NX1_PWM_H
