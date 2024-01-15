#include "nx1_timer.h"


//------------------------------------------------------------------//
// Timer init
// Parameter: 
//          timerx   : TIMER0,TIMER1,TIMER2,PWMATIMER,PWMBTIMER
//          divide   : TIMER_CLKDIV_1....
//          counter  : preload counter , can't use 0,freq=clock/(counter+1)
// return value:
//          NONE
//------------------------------------------------------------------//    
#if _EF_SERIES
void TIMER_Init(TIMER_TypeDef *timerx,U16 counter)
#else
void TIMER_Init(TIMER_TypeDef *timerx,U8 divide,U16 counter)
#endif
{
    U32 _timerCtlTmp = (C_TMR_Auto_Reload_En|C_TMR_CLK_Src_HiCLK);
#if !_EF_SERIES
    _timerCtlTmp |= divide;
#endif
                    
    timerx->Ctrl = _timerCtlTmp;
    timerx->Data = counter;
}
//------------------------------------------------------------------//
// Enable or Disable timer
// Parameter: 
//          timerx : TIMER0,TIMER1,TIMER2,PWMATIMER,PWMBTIMER
//          cmd    : ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------//    
void TIMER_Cmd(TIMER_TypeDef *timerx,U8 cmd)
{
    U32 _timerCtrl = timerx->Ctrl;
    if(cmd) {
        _timerCtrl |= C_TMR_En;
    } else {
        _timerCtrl &= (~C_TMR_En);
    }
    timerx->Ctrl = _timerCtrl; 
}    
//------------------------------------------------------------------//
// Enable or Disable timer interrupt
// Parameter: 
//          timerx:TIMER0,TIMER1,TIMER2,PWMATIMER,PWMBTIMER
//          cmd: ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------//    
void TIMER_IntCmd(TIMER_TypeDef *timerx,U8 cmd)
{
    if(cmd)
    {
        timerx->INT_Ctrl = C_TMR_En;
    }
    else
    {
        timerx->INT_Ctrl = C_TMR_Dis;
    }        
}
//------------------------------------------------------------------//
// Set clock source
// Parameter: 
//          timerx:TIMER0,TIMER1,TIMER2
//          source:TIMER_CLKSRC_HICLK,TIMER_CLKSRC_LOCLK,TIMER_CLKSRC_EX_PIN
// return value:
//          NONE
//------------------------------------------------------------------//    
void TIMER_SetClkSrc(TIMER_TypeDef *timerx,U16 source)
{
    timerx->Ctrl &= (~C_TMR_CLK_Src);
 	timerx->Ctrl |= source;
}    
