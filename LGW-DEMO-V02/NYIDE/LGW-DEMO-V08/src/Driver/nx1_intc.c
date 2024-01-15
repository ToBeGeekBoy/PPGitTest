#include "nx1_intc.h"
#include "include.h"


//------------------------------------------------------------------//
// Enable or Disable interrupt
// Parameter: 
//          irqs:IRQ_BIT_DAC,....
//          cmd:ENABLE,Disable
// return value:
//          NONE
//------------------------------------------------------------------//    
void INTC_IrqCmd(U32 irqs,U8 cmd)
{
    U32 old;
    
    GIE_DISABLE(); 
    old=__nds32__mfsr(NDS32_SR_INT_MASK2);
    
    if(cmd==ENABLE){
        __nds32__mtsr(old|irqs, NDS32_SR_INT_MASK2);
    }else{
        __nds32__mtsr(old&(~irqs), NDS32_SR_INT_MASK2);
    }
    GIE_ENABLE();         
}    
//------------------------------------------------------------------//
// Clear pending irq
// Parameter: 
//          irqs:IRQ_BIT_DAC,....
// return value:
//          NONE
//------------------------------------------------------------------//    
void INTC_ClrPending(U32 irqs)
{
    __nds32__mtsr(irqs, NDS32_SR_INT_PEND2);
}  
//------------------------------------------------------------------//
// Get irq pending status
// Parameter: 
//          irq:IRQ_BIT_DAC,....
// return value:
//          0: no irq pending, !0:irq pending
//------------------------------------------------------------------//    
U32 INTC_GetPending(U32 irq)
{
   return  (__nds32__mfsr(NDS32_SR_INT_PEND)&irq);
}  
//------------------------------------------------------------------//
// Set default irq priority
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//    
void INTC_SetDefaultPriority(void)
{
    volatile U32 old;
    
    //Dac0/Dac1/ADC set the highest priority.
    //Others set third priority.
    old = 0xAAAAAA80;
    __nds32__mtsr(old, __NDS32_REG_INT_PRI__);
    old = 0xAAAAAAAA;
    __nds32__mtsr(old, __NDS32_REG_INT_PRI2__);
} 

//------------------------------------------------------------------//
// Set irq priority
// Parameter: 
//          irq:IRQ_VALVE_DAC_CH0, IRQ_VALVE_DAC_CH1....
//          priority:IRQ_PRIORITY_0(highest),IRQ_PRIORITY_1,IRQ_PRIORITY_2,IRQ_PRIORITY_3
// return value:
//          NONE
//------------------------------------------------------------------//    
void INTC_SetPriority(U8 irq,U8 priority)
{
    U32 old;
    
    // Changing the priorities of DAC is not suggested.
    // If DAC FIFO is empty and interpolator(Noise_Filter) turns on, DAC output will be wrong. 
    if(irq<IRQ_VALVE_ADC)
    {
    	return;
    }
#if _EF_SERIES
    if(irq<IRQ_VALVE_RESERVED)
#else
    if(irq<IRQ_VALVE_GPIOC)
#endif	
    {
        old=__nds32__mfsr(__NDS32_REG_INT_PRI__);
        irq <<= 1;
        old=old&(~(3<<irq));
        __nds32__mtsr(old|(priority<<irq), __NDS32_REG_INT_PRI__);
    }
    else
    {
        old=__nds32__mfsr(__NDS32_REG_INT_PRI2__);
        irq -= 16; // PRI2 offset
        irq <<= 1;
        old=old&(~(3<<irq));
        __nds32__mtsr(old|(priority<<irq), __NDS32_REG_INT_PRI2__);
    }
}  
void INTC_EnableNestINT(void)
{
	__nds32__setgie_en();
    __nds32__dsb();
} 
//------------------------------------------------------------------//
// Trigger SWI
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//    
void INTC_TrigSWI(void)
{
	__nds32__mtsr(0x10000, NDS32_SR_INT_PEND);
	__nds32__dsb();
}  
//------------------------------------------------------------------//
// Clear SWI
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//    
void INTC_ClrSWI(void)
{
	__nds32__mtsr(0x0, NDS32_SR_INT_PEND);
	__nds32__dsb();
}  
