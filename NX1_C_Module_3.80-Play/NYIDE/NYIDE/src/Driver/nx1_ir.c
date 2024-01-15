#include "nx1_gpio.h"
#include "nx1_timer.h"
#include "nx1_ir.h"

#define abs(a)		((a)>0?(a):(-a))

#ifndef	_IR_NO_CARRIER
#define _IR_NO_CARRIER	0
#endif
#ifndef _IR_NO_CARRIER_IDLE_LEVEL
#define _IR_NO_CARRIER_IDLE_LEVEL    1
#endif

#if _EF_SERIES
S_IR_RESOURCE s_IR_Resource[] = 
{
#if defined(P_TIMER0)
    {
        .timer  = P_TIMER0,
        .port   = IR0_TX_PORT,
        .pin    = IR0_TX_PIN,
    },
#endif
#if defined(P_TIMER1)
    {
        .timer  = P_TIMER1,
        .port   = IR1_TX_PORT,
        .pin    = IR1_TX_PIN,
    },
#endif
#if defined(P_TIMER2)
    {
        .timer  = P_TIMER2,
        .port   = IR2_TX_PORT,
        .pin    = IR2_TX_PIN,
    },
#endif
#if defined(P_TIMER3)
    {
        .timer  = P_TIMER3,
        .port   = IR3_TX_PORT,
        .pin    = IR3_TX_PIN,
    },
#endif
    {
        0
    },
};

#endif
#if !_EF_SERIES && defined(P_IR)
//------------------------------------------------------------------//
// IR frequence to parameter(for OTP IC body use)
// Parameter: 
//          _freqRatio   : calculate from (_IR_CLK_BASE/carry Frequence)
//          p_irDivide   : parameter for IR hardware
//          p_irPeriod   : parameter for IR hardware
// return value:
//          NONE
//------------------------------------------------------------------//    
static void IR_freqRatio2Para(U32 _freqRatio, U16 *p_irDivide, U16 *p_irPeriod)
{
#define _IR_DIVIDE_MAX  16
#define _IR_DIVIDE_MIN  1
#define _IR_PERIOD_MAX  32
#define _IR_PERIOD_MIN  2
    //1st, boundary codition check
    if(_freqRatio <= (_IR_DIVIDE_MIN*_IR_PERIOD_MIN)) {
        *p_irDivide = _IR_DIVIDE_MIN;
        *p_irPeriod = _IR_PERIOD_MIN;
        return;
    } else if (_freqRatio >= (_IR_DIVIDE_MAX*_IR_PERIOD_MAX)) {
        *p_irDivide = _IR_DIVIDE_MAX;
        *p_irPeriod = _IR_PERIOD_MAX;
        return;
    } else;
    
    //2nd, find the start point
    U16 _periodChk = 0;
    U16 _divideChk = 0;
    if(_freqRatio <= (_IR_DIVIDE_MAX*_IR_PERIOD_MIN)) {
        _periodChk = _IR_PERIOD_MIN;
        _divideChk = (_freqRatio+(_IR_PERIOD_MIN>>1))/_IR_PERIOD_MIN;
    } else {
        _divideChk = _IR_DIVIDE_MAX;
        _periodChk = (_freqRatio+(_IR_DIVIDE_MAX>>1))/_IR_DIVIDE_MAX;
    }
    
    //3th, look up the best parameter
    S16 _errorTemp = 30000;
    U16 _periodTemp = _periodChk;
    U16 _divideTemp = _divideChk;
    while(1) {
        //3-1. test
        //printf("test Divide:%d,\tPeriod:%d,\t\n",_divideChk,_periodChk);
        S16 _error = _freqRatio-(_periodChk*_divideChk);
        //3-2. check
        if(_error == 0) {
            _periodTemp = _periodChk;
            _divideTemp = _divideChk;
            break;//test end
        } else if (abs(_error) < abs(_errorTemp)) {
            _periodTemp = _periodChk;
            _divideTemp = _divideChk;
            _errorTemp = _error;
        } else if (abs(_error) == abs(_errorTemp)) {
            if(_error < 0) {//adjustable
                _periodTemp = _periodChk;
                _divideTemp = _divideChk;
                _errorTemp = _error;
            }
        } else;
        //3-3. set next point
        if(_error > 0) {
            if(++_periodChk > _IR_PERIOD_MAX) {
                break;//test end
            }
        } else {
            if(--_divideChk < (_IR_DIVIDE_MIN)) {
                break;//test end
            }
        }
    }
    *p_irPeriod = _periodTemp;
    *p_irDivide = _divideTemp;
}
#endif
//------------------------------------------------------------------//
// IR init
// Parameter: 
//          irIndex   : USE_IR0_TX, USE_IR1_TX, USE_IR2_TX, USE_IR3_TX
//          stop      : IR_STOP_HIGH,IR_STOP_LOW
//          carryFreq : Carry frequence
// return value:
//          NONE
//------------------------------------------------------------------//    
void IR_Init(IR_TX_SELECT irIndex,U8 stop,U32 carryFreq)
{
	if(!carryFreq || irIndex>= num_USABLE_IR_TX) return;
#if _EF_SERIES
    GPIO_Init(
        s_IR_Resource[irIndex].port
        ,s_IR_Resource[irIndex].pin
        ,GPIO_MODE_ALT
    );
    U32 _counter = (( _IR_CLK_BASE+(carryFreq>>1))/carryFreq)-1;
    _counter = (_counter<=0xFFFF)?_counter:0xFFFF;
    s_IR_Resource[irIndex].timer->Data = _counter;
    s_IR_Resource[irIndex].timer->Ctrl = 
        ( C_TMR_Dis
        | C_TMR_Auto_Reload_En
        | C_IR_Dis
        | ((stop==IR_STOP_LOW)?C_IR_Stop_0:C_IR_Stop_1)
        | C_TMR_CLK_Src_HiCLK );
#else
#if defined(P_IR)
    GPIO_Init(IR0_TX_PORT,IR0_TX_PIN,GPIO_MODE_ALT);
    U32  _freqRatio =  ( _IR_CLK_BASE+(carryFreq>>1))/carryFreq;
    U16  _irDivide = 0;
    U16  _irPeriod = 0;
    IR_freqRatio2Para(_freqRatio, &_irDivide, &_irPeriod);
    _irDivide = (_irDivide-1)&0xF;
    _irPeriod = (_irPeriod-1)&0x1F;
    P_IR->Ctrl = 
        ( ((stop==IR_STOP_LOW)?C_IR_Idle_Low:C_IR_Idle_High) 
        | ((U32)_irDivide<<4) 
        | ((U32)_irPeriod<<8));
#endif
#endif
}    
//------------------------------------------------------------------//
// Enable or Disable the carrier of IR
// Parameter: 
//          irIndex   : USE_IR0_TX, USE_IR1_TX, USE_IR2_TX, USE_IR3_TX
//          cmd       : ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------//    
void IR_CARRIER_Cmd(IR_TX_SELECT irIndex, U8 cmd)
{
#if _IR_NO_CARRIER
    U8 mode;

    if(irIndex>= num_USABLE_IR_TX) return;

    if(cmd)
    {
        mode = GPIO_MODE_ALT;
    }
    else
    {
#if _IR_NO_CARRIER_IDLE_LEVEL
        mode = GPIO_MODE_OUT_HI;
#else
        mode = GPIO_MODE_OUT_LOW;
#endif
    }
#if _EF_SERIES
    GPIO_Init( s_IR_Resource[irIndex].port
              ,s_IR_Resource[irIndex].pin
              ,mode
            );
#else
#if defined(P_IR)
    GPIO_Init(IR0_TX_PORT,IR0_TX_PIN,mode);
#endif
#endif
#endif
}
//------------------------------------------------------------------//
// Enable or Disable IR
// Parameter: 
//          irIndex   : USE_IR0_TX, USE_IR1_TX, USE_IR2_TX, USE_IR3_TX
//          cmd       : ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------//    
void IR_Cmd(IR_TX_SELECT irIndex,U8 cmd)
{
    if(irIndex>= num_USABLE_IR_TX) return;

#if _IR_NO_CARRIER
    {
        U8  mode;

#if _EF_SERIES
        mode = ( s_IR_Resource[irIndex].port->MFP >> (s_IR_Resource[irIndex].pin<<1) )&0x3;

        if(!mode) //GPIO
        {
            if(cmd ^ _IR_NO_CARRIER_IDLE_LEVEL)
            {
                s_IR_Resource[irIndex].port->Data |= (1<<s_IR_Resource[irIndex].pin);
            }
            else
            {
                s_IR_Resource[irIndex].port->Data &= (~(1<<s_IR_Resource[irIndex].pin));
            }
            return;
        }
#else
        mode = (P_PORTA->MFP>>5)&0x01;
        if(!mode) //GPIO
        {
            if(cmd ^ _IR_NO_CARRIER_IDLE_LEVEL)
            {
                P_PORTA->Data |= (1<<5);
            }
            else
            {
                P_PORTA->Data &= (~(1<<5));
            }
            return;
        }
#endif
    }
#endif
#if _EF_SERIES
    if(cmd) {
        s_IR_Resource[irIndex].timer->Ctrl |= (C_IR_En|C_TMR_En);
    } else {
        s_IR_Resource[irIndex].timer->Ctrl &= ~(C_IR_En|C_TMR_En); 
   	} 	
#else
#if defined(P_IR)
    if(cmd) {
        P_IR->Ctrl |= C_IR_Run;
    } else {
        P_IR->Ctrl &= (~C_IR_Run); //C_IR_Idle
   	} 	
#endif
#endif
}
