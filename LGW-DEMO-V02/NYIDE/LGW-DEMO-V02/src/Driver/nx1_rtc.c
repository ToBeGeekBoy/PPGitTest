#include "nx1_rtc.h"


//------------------------------------------------------------------//
// RTC interrupt enable or disable
// Parameter: 
//          freq:RTC_2HZ,RTC_64HZ,RTC_1KHZ,RTC_4KHZ or RTC_16KHZ
//          cmd: ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------//    
void RTC_IntCmd(U8 freq,U8 cmd)
{
    if(cmd){
        P_RTC->INT_Ctrl|=freq;
    }else{
        P_RTC->INT_Ctrl&=(~freq);
    }        
}
//------------------------------------------------------------------//
// Clear Status
// Parameter: 
//          None      
// return value:
//          None
//------------------------------------------------------------------//        
void RTC_ClrSts(void)
{
	U16 RTC_INT_temp = P_RTC->INT_Ctrl;
#if ((_IC_BODY==0x12A44)||(_IC_BODY==0x11A22))
	P_RTC->INT_Ctrl = (C_RTC_4KHz_IDis|C_RTC_1KHz_IDis|C_RTC_64Hz_IDis|C_RTC_2Hz_IDis);
#else
	P_RTC->INT_Ctrl = (C_RTC_16KHz_IDis|C_RTC_1KHz_IDis|C_RTC_64Hz_IDis|C_RTC_2Hz_IDis);
#endif
    P_RTC->Flag &= RTC_INT_temp;
    P_RTC->INT_Ctrl = RTC_INT_temp;
}
//------------------------------------------------------------------//
// RTC Interrupt Re-Enable
// Parameter: 
//          None      
// return value:
//          None
//------------------------------------------------------------------// 
void RTC_Int_ReEn(void)
{
	U16 RTC_INT_temp = P_RTC->INT_Ctrl;
#if ((_IC_BODY==0x12A44)||(_IC_BODY==0x11A22))
	P_RTC->INT_Ctrl = (C_RTC_4KHz_IDis|C_RTC_1KHz_IDis|C_RTC_64Hz_IDis|C_RTC_2Hz_IDis);
#else
	P_RTC->INT_Ctrl = (C_RTC_16KHz_IDis|C_RTC_1KHz_IDis|C_RTC_64Hz_IDis|C_RTC_2Hz_IDis);
#endif
	P_RTC->INT_Ctrl = RTC_INT_temp;
}