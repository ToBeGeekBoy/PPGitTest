#ifndef NX1_INTC_H
#define NX1_INTC_H

#include  "nx1_lib.h"
#include <nds32_intrinsic.h>

//////////////////////////////////////////////////////////////////////
// Constant Define
//////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------//
//  Function   : INTC_SetPriority
//  Parameter  : irq
//------------------------------------------------------------------//
#if _EF_SERIES
#define IRQ_VALVE_DAC_CH0           (0)
#define IRQ_VALVE_DAC_CH1           (1)
#define IRQ_VALVE_ADC               (2)
#define IRQ_VALVE_FMC				(3)
#define IRQ_VALVE_TMR0              (4)
#define IRQ_VALVE_TMR1              (5)
#define IRQ_VALVE_TMR2              (6)
#define IRQ_VALVE_EXIT0             (7)
#define IRQ_VALVE_SPI0              (8)
#define IRQ_VALVE_EXIT1             (9)
#define IRQ_VALVE_RTC               (10)
#define IRQ_VALVE_PWMA              (11)
#define IRQ_VALVE_PWMB              (12)
#define IRQ_VALVE_TMR3              (14)
#define IRQ_VALVE_NFC               (15)
#define IRQ_VALVE_RESERVED          (16)
#define IRQ_VALVE_I2C               (17)
#define IRQ_VALVE_UART              (18)
#define IRQ_VALVE_SPI1              (19)
#define IRQ_VALVE_SWI               (21)
#define IRQ_VALVE_SWA               (22)
#else
#define IRQ_VALVE_DAC_CH0           (0)
#define IRQ_VALVE_DAC_CH1           (1)
#define IRQ_VALVE_ADC               (2)
#define IRQ_VALVE_TMR0              (4)
#define IRQ_VALVE_TMR1              (5)
#define IRQ_VALVE_TMR2              (6)
#define IRQ_VALVE_GPIOA             (7)
#define IRQ_VALVE_SPI0              (8)
#define IRQ_VALVE_GPIOB             (9)
#define IRQ_VALVE_RTC               (10)
#define IRQ_VALVE_PWMA              (11)
#define IRQ_VALVE_PWMB              (12)
#define IRQ_VALVE_CAPTURE           (13)
#define IRQ_VALVE_GPIOC             (16)
#define IRQ_VALVE_I2C               (17)
#define IRQ_VALVE_UART              (18)
#define IRQ_VALVE_SPI1              (19)
#define IRQ_VALVE_SDC               (20)
#define IRQ_VALVE_SWI               (21)
#define IRQ_VALVE_SWA               (22)
#endif

//------------------------------------------------------------------//
//  Function   : INTC_IrqCmd, INTC_ClrPending, INTC_GetPending
//  Parameter  : irq
//------------------------------------------------------------------//
#if _EF_SERIES
#define IRQ_BIT_DAC_CH0             (1<<0)
#define IRQ_BIT_DAC_CH1             (1<<1)
#define IRQ_BIT_ADC                 (1<<2)
#define IRQ_BIT_FMC                 (1<<3)
#define IRQ_BIT_TMR0                (1<<4)
#define IRQ_BIT_TMR1                (1<<5)
#define IRQ_BIT_TMR2                (1<<6)
#define IRQ_BIT_EXIT0               (1<<7)
#define IRQ_BIT_SPI0                (1<<8)
#define IRQ_BIT_EXIT1               (1<<9)
#define IRQ_BIT_RTC                 (1<<10)
#define IRQ_BIT_PWMA                (1<<11)
#define IRQ_BIT_PWMB                (1<<12)
#define IRQ_BIT_TMR3             	(1<<14)
#define IRQ_BIT_NFC          		(1<<15)
#define IRQ_BIT_I2C                 (1<<17)
#define IRQ_BIT_UART                (1<<18)
#define IRQ_BIT_SPI1                (1<<19)
#define IRQ_BIT_SWI                 (1<<21)
#define IRQ_BIT_SWA                 (1<<22)
#else
#define IRQ_BIT_DAC_CH0             (1<<0)
#define IRQ_BIT_DAC_CH1             (1<<1)
#define IRQ_BIT_ADC                 (1<<2)
#define IRQ_BIT_TMR0                (1<<4)
#define IRQ_BIT_TMR1                (1<<5)
#define IRQ_BIT_TMR2                (1<<6)
#define IRQ_BIT_GPIOA               (1<<7)
#define IRQ_BIT_SPI0                (1<<8)
#define IRQ_BIT_GPIOB               (1<<9)
#define IRQ_BIT_RTC                 (1<<10)
#define IRQ_BIT_PWMA                (1<<11)
#define IRQ_BIT_PWMB                (1<<12)
#define IRQ_BIT_CAPTURE             (1<<13)
//#define IRQ_BIT_RESERVED          (1<<14)
//#define IRQ_BIT_RESERVED          (1<<15)
#define IRQ_BIT_GPIOC               (1<<16)
#define IRQ_BIT_I2C                 (1<<17)
#define IRQ_BIT_UART                (1<<18)
#define IRQ_BIT_SPI1                (1<<19)
#define IRQ_BIT_SDC                 (1<<20)
#define IRQ_BIT_SWI                 (1<<21)
#define IRQ_BIT_SWA                 (1<<22)
#endif

//------------------------------------------------------------------//
//  Function   : INTC_SetPriority
//  Parameter  : priority
//------------------------------------------------------------------//
#define IRQ_PRIORITY_0              0
#define IRQ_PRIORITY_1              1
#define IRQ_PRIORITY_2              2
#define IRQ_PRIORITY_3              3



//////////////////////////////////////////////////////////////////////
// External Function
//////////////////////////////////////////////////////////////////////

extern void INTC_IrqCmd(U32 irqs,U8 cmd);
extern void INTC_ClrPending(U32 irqs);
extern U32  INTC_GetPending(U32 irq);
extern void INTC_SetDefaultPriority(void);
extern void INTC_SetPriority(U8 irq,U8 priority);
extern void INTC_EnableNestINT(void);
extern void INTC_TrigSWI(void);
extern void INTC_ClrSWI(void);
//extern void INTC_RegisterISR(U8 irq_idx,void *isr);





#endif //#ifndef NX1_INTC_H
