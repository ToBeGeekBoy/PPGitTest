#include "nx1_wdt.h"


//------------------------------------------------------------------//
// HW ISR Default Handler
//------------------------------------------------------------------//
#pragma weak HW0_IRAM_ISR   = Default_IRAM_Handler
#pragma weak HW1_IRAM_ISR   = Default_IRAM_Handler
#pragma weak HW2_IRAM_ISR   = Default_IRAM_Handler
#pragma weak HW3_IRAM_ISR   = Default_IRAM_Handler
#pragma weak HW4_IRAM_ISR   = Default_IRAM_Handler
#pragma weak HW5_IRAM_ISR   = Default_IRAM_Handler
#pragma weak HW6_IRAM_ISR   = Default_IRAM_Handler
#pragma weak HW7_IRAM_ISR   = Default_IRAM_Handler
#pragma weak HW8_IRAM_ISR   = Default_IRAM_Handler
#pragma weak HW9_IRAM_ISR   = Default_IRAM_Handler
#pragma weak HW10_IRAM_ISR  = Default_IRAM_Handler
#pragma weak HW11_IRAM_ISR  = Default_IRAM_Handler
#pragma weak HW12_IRAM_ISR  = Default_IRAM_Handler
#pragma weak HW13_IRAM_ISR  = Default_IRAM_Handler
#pragma weak HW14_IRAM_ISR  = Default_IRAM_Handler
#pragma weak HW15_IRAM_ISR  = Default_IRAM_Handler
#pragma weak HW16_IRAM_ISR  = Default_IRAM_Handler
#pragma weak HW17_IRAM_ISR  = Default_IRAM_Handler
#pragma weak HW18_IRAM_ISR  = Default_IRAM_Handler
#pragma weak HW19_IRAM_ISR  = Default_IRAM_Handler
#pragma weak HW20_IRAM_ISR  = Default_IRAM_Handler
#pragma weak HW21_IRAM_ISR  = Default_IRAM_Handler
#pragma weak HW22_IRAM_ISR  = Default_IRAM_Handler
#pragma weak HW23_IRAM_ISR  = Default_IRAM_Handler

void  HW0_IRAM_ISR(void);
void  HW1_IRAM_ISR(void);
void  HW2_IRAM_ISR(void);
void  HW3_IRAM_ISR(void);
void  HW4_IRAM_ISR(void);
void  HW5_IRAM_ISR(void);
void  HW6_IRAM_ISR(void);
void  HW7_IRAM_ISR(void);
void  HW8_IRAM_ISR(void);
void  HW9_IRAM_ISR(void);
void HW10_IRAM_ISR(void);
void HW11_IRAM_ISR(void);
void HW12_IRAM_ISR(void);
void HW13_IRAM_ISR(void);
void HW14_IRAM_ISR(void);
void HW15_IRAM_ISR(void);
void HW16_IRAM_ISR(void);
void HW17_IRAM_ISR(void);
void HW18_IRAM_ISR(void);
void HW19_IRAM_ISR(void);
void HW20_IRAM_ISR(void);
void HW21_IRAM_ISR(void);
void HW22_IRAM_ISR(void);
void HW23_IRAM_ISR(void);

const void *const CPUVectorTable_IRAM[24]={
HW0_IRAM_ISR,
HW1_IRAM_ISR,
HW2_IRAM_ISR,
HW3_IRAM_ISR,
HW4_IRAM_ISR,
HW5_IRAM_ISR,
HW6_IRAM_ISR,
HW7_IRAM_ISR,
HW8_IRAM_ISR,
HW9_IRAM_ISR,
HW10_IRAM_ISR,
HW11_IRAM_ISR,
HW12_IRAM_ISR,
HW13_IRAM_ISR,
HW14_IRAM_ISR,
HW15_IRAM_ISR,
HW16_IRAM_ISR,
HW17_IRAM_ISR,
HW18_IRAM_ISR,
HW19_IRAM_ISR,
HW20_IRAM_ISR,
HW21_IRAM_ISR,
HW22_IRAM_ISR,
HW23_IRAM_ISR,
};

void Default_IRAM_Handler(void) __attribute__((unused));
void Default_IRAM_Handler(void)
{
    //HW ISR Default Handler
    //If PC come here, HWx_ISR does not set correctly!!!
    //Please check HWx_ISR reference.
    while (1){
        //WDT_Clear();
    }
}

//------------------------------------------------------------------//
// Execption Handler
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
__attribute__((unused))
void ExceptionHandler_IRAM()
{
    //Exception Handler
    //If PC come here, exception catch here
    //Please Check IC Body Selection. Maybe access illegal address.
	
    // Change WDT time for ExceptionHandler
	P_WDT->Ctrl = WDT_TIME_188ms;
    while (1){
        //WDT_Clear();
    }
}


