#include "nx1_dac.h"
#include "nx1_smu.h"
#include "nx1_timer.h"
#include "nx1_intc.h"
#include "include.h"


//////////////////////////////////////////////////////////////////////
//  Define
//////////////////////////////////////////////////////////////////////



//------------------------------------------------------------------//
// DAC Init
// Parameter: 
//          chx        : DAC_CH0, DAC_CH1
//          trig_src   : DAC_TIMER0_TRIG,DAC_TIMER1_TRIG,DAC_TIMER2_TRIG
//          fifo_level : fifo threshold, DAC_FIFO_THD
// return value:
//          NONE
//------------------------------------------------------------------//    
void DAC_Init(CH_TypeDef *chx,U8 trig_src,U8 fifo_level)
{
    do {
#if _DAC_MODE
        if(DacMixerRamp_IsDacModeUse()==true) {
#if EF11FS_SERIES
#else
            chx->CH_FIFO = ( (fifo_level<<C_DAC_FIFO_Level_Offset) );
#endif
            break; //do-while(0)
        }
#endif
#if EF11FS_SERIES
        chx->CH_FIFO = (C_DAC_FIFO_Reset);
#else
        chx->CH_FIFO = (C_DAC_FIFO_Reset | (fifo_level<<C_DAC_FIFO_Level_Offset) );
#endif
        chx->CH_Ctrl &= ~(C_DAC_Data_Mixer_En);
    } while(0);

    chx->CH_Ctrl &= ~(C_DAC_Trig);
    chx->CH_Ctrl |= (trig_src);
    
}    
//------------------------------------------------------------------//
// Set PP gain immediately.
// Parameter: 
//          chx        : DAC_CH0, DAC_CH1
//          upsample: 
//                    1: upsample *4 or *16, 
//                    0: no upsample
// return value:
//          NONE
//------------------------------------------------------------------//    
void DAC_SetUpSample(CH_TypeDef *chx,U8 upsample)
{
#if _EF_SERIES
    chx->CH_Ctrl &= ~(C_DAC_Up_Sample_En | C_DAC_Up_Sample);
#else
    chx->CH_Ctrl &= ~(C_DAC_Up_Sample_En);
#endif
    if(upsample) {
	#if _EF_SERIES
		chx->CH_Ctrl |= (C_DAC_Up_Sample_En | (_DAC_UP_SAMPLE_SETTING<<12));
	#else
        chx->CH_Ctrl |= C_DAC_Up_Sample_En;
	#endif
    }
}
//------------------------------------------------------------------//
// Set PP gain immediately.
// Parameter: 
//          gain:dac gain(0~15)
// return value:
//          NONE
//------------------------------------------------------------------//    
void DAC_SetPPIpGain(U8 gain)
{
#if !_EF_SERIES
#define PP_GAIN_VER       (((*(U8 *)0x170)>>7)&0x01)
    if(!PP_GAIN_VER) gain = gain^0xF;	
	
    P_DAC->CH0.CH_Ctrl &= (~C_DAC_PPGain);
    P_DAC->CH0.CH_Ctrl |= ((gain&0xF)<<C_DAC_PPGain_Offset);
#undef PP_GAIN_VER
#endif
}    
//------------------------------------------------------------------//
// Get PP gain.
// Parameter: 
//          NONE
// return value:
//          gain:dac gain(0~15)
//------------------------------------------------------------------//    
U8 DAC_GetPPIpGain(void)
{
#if !_EF_SERIES
#define PP_GAIN_VER       (((*(U8 *)0x170)>>7)&0x01)
    U8 gain = (P_DAC->CH0.CH_Ctrl>>C_DAC_PPGain_Offset)&0xF;
    if(!PP_GAIN_VER) gain = gain^0xF;	
    return gain;
#undef PP_GAIN_VER
#else
    return 0;
#endif
}
//------------------------------------------------------------------//
// Is PP enable.
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//    
bool DAC_IsPPEnable(void)
{
#if !_EF_SERIES
    U32 _chCtrl = P_DAC->CH0.CH_Ctrl;
    return ((_chCtrl&C_DAC_Out_Mode)==C_DAC_Out_Mode_PushPull && (_chCtrl&C_DAC_En)==C_DAC_En);
#else
    return false;
#endif
}
#if _EF_SERIES
//------------------------------------------------------------------//
// ENABLE or Disable DAC PWM
// Parameter: 
//          cmd:ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------//    
void DAC_PWMCmd(U8 cmd)
{
    if(cmd==ENABLE)
    {
        P_DAC->CH0.CH_Ctrl &= (~C_DAC_Out_Mode);
        P_DAC->CH0.CH_Ctrl |= (C_DAC_Out_Mode_PWM | C_DAC_En);
	#if	((_IC_BODY==0x12F51)||(_IC_BODY==0x12FB0052)||(_IC_BODY==0x12FB0054))   // only for NX13FS51 body
		P_DAC->PWM_Ctrl |= ((NX1EF_getPwmCurrent()<<4) | C_DAC_PWM_En);
	#else
		P_DAC->PWM_Ctrl |= (C_DAC_PWM_En);
	#endif
    }
    else
    {
		P_DAC->CH0.CH_Ctrl &= (~(C_DAC_En|C_DAC_Out_Mode_VolDAC));
	#if	((_IC_BODY==0x12F51)||(_IC_BODY==0x12FB0052)||(_IC_BODY==0x12FB0054))
		P_DAC->PWM_Ctrl &= ~(0xF0 | C_DAC_PWM_En);
	#else
		P_DAC->PWM_Ctrl &= ~(C_DAC_PWM_En);
	#endif
    }        
}    
#else
//------------------------------------------------------------------//
// ENABLE or Disable Push-pll
// Parameter: 
//          cmd:ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------//    
void DAC_PPCmd(U8 cmd)  //Check PPCmd function!!
{
    if(cmd==ENABLE)
    {
        P_DAC->CH0.CH_Ctrl &= (~C_DAC_Out_Mode);
        P_DAC->CH0.CH_Ctrl |= (C_DAC_Out_Mode_PushPull | C_DAC_En);
    }
    else
    {
		P_DAC->CH0.CH_Ctrl &= (~(C_DAC_En|C_DAC_Out_Mode_VolDAC));
    }        
}    
#endif
//------------------------------------------------------------------//
// ENABLE or Disable voltage DAC
// Parameter: 
//          cmd:ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------//    
void DAC_VoltageDacCmd(U8 cmd)  //Check DacCmd function !!
{
    if(cmd==ENABLE)
    {
        P_DAC->CH0.CH_Ctrl |= (C_DAC_En|C_DAC_Out_Mode_VolDAC);
    }
    else
    {
        P_DAC->CH0.CH_Ctrl &= (~(C_DAC_En|C_DAC_Out_Mode_VolDAC));
    }        
}    
//------------------------------------------------------------------//
// ENABLE or Disable HW channel converter.
// Parameter: 
//          chx  : DAC_CH0, DAC_CH1
//          cmd  : ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------//    
void DAC_Cmd(CH_TypeDef *chx,U8 cmd)
{  
    if(cmd==ENABLE)
    {
        chx->CH_Ctrl |= (C_DAC_Trig_Timer_En|C_DAC_Data_Mixer_En);
	#if EF11FS_SERIES
		chx->CH_FIFO |= C_DAC_FIFO_Empty_IEn; 
	#else
        chx->CH_FIFO |= C_DAC_FIFO_Level_IEn; 
	#endif
    }
    else
    {
    	chx->CH_Ctrl &= (~(C_DAC_Trig_Timer_En|C_DAC_Data_Mixer_En));
	#if EF11FS_SERIES
    	chx->CH_FIFO &= (~C_DAC_FIFO_Empty_IEn); 
	#else
    	chx->CH_FIFO &= (~C_DAC_FIFO_Level_IEn); 
	#endif
    }       
}       
//------------------------------------------------------------------//
// ENABLE or Disable DAC line-in
// Parameter: 
//          cmd:ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------//    
void DAC_LineInCmd(U8 cmd)
{
    if(cmd==ENABLE)
    {
        P_DAC->CH0.CH_Ctrl |= C_DAC_EXT_AUX_In_En;        
    }
    else
    {
    	P_DAC->CH0.CH_Ctrl &= (~C_DAC_EXT_AUX_In_En);
    }       
}       
//------------------------------------------------------------------//
// DAC ramp control 
// Parameter: 
//          chx: DAC_CH0, DAC_CH1
//          cmd:RAMP_UP,RAMP_DOWN
// return value:
//          NONE
//------------------------------------------------------------------//     
void DacRamp(CH_TypeDef *chx, U8 cmd)
{
    extern void DelayLoop2(U32 count);
    U16 data;
    
    SMU_PeriphClkCmd(SMU_PERIPH_DAC,ENABLE);

    if(cmd==RAMP_UP)
    {
        chx->CH_Ctrl &= (~C_DAC_Trig);
        data=0x8000;
//        data=0x7FFF;
//        data=0;
        chx->CH_Data=data;

        //DAC_PPCmd(ENABLE);
     
        DAC_VoltageDacCmd(ENABLE);
        while(1)
        {
            data++;
            chx->CH_Data=data; 
            if(data==0)
            {
                break;
            }      
            DelayLoop2(1);
        }    
    }
    else
    {
        chx->CH_Ctrl &= (~C_DAC_Trig);
        data=0;
        chx->CH_Data=data;
        while(1)
        {
            data--;
            chx->CH_Data=data; 
            if(data==0x8000)
            {
                break;
            }       
        }    
        //DAC_PPCmd(DISABLE);
        DAC_VoltageDacCmd(DISABLE);
    }    
}    
//------------------------------------------------------------------//
// Disable DAC trigger by timer.
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
U8 DAC_Trig_Timer_disable(void)
{
	U8 DAC_status = DAC_trig_None;
	#if _IS_APPS_USE_DAC
		if((P_DAC->CH0.CH_Ctrl) & (C_DAC_Trig_Timer_En))
		{
			DAC_status += DAC0_trig;
			DAC_Cmd(DAC_CH0, DISABLE);	
		}
		if((P_DAC->CH1.CH_Ctrl) & (C_DAC_Trig_Timer_En))
		{
			DAC_status += DAC1_trig;
			DAC_Cmd(DAC_CH1, DISABLE);
		}	 
	#endif
	return DAC_status;
}
//------------------------------------------------------------------//
// Enable DAC trigger by timer.
// Parameter: 
//          DAC_status:Previous DAC status
// return value:
//          NONE
//------------------------------------------------------------------// 
void DAC_Trig_Timer_enable(U8 DAC_status)
{
	#if _IS_APPS_USE_DAC
		if(DAC_status & DAC0_trig)//DAC0 determine 
		{
			DAC_Cmd(DAC_CH0,ENABLE);
		}
		if(DAC_status & DAC1_trig)//DAC1 determine
		{
			DAC_Cmd(DAC_CH1,ENABLE);
		}	
	#endif
}

//------------------------------------------------------------------//
// Enable DAC trigger by timer for DAC mode.
// Parameter: 
//          DAC_status:Previous DAC status
//			upsample: upsample status
// return value:
//          NONE
//------------------------------------------------------------------// 
void DAC_MODE_Trig_Timer_enable(U8 DAC_status,U8 upsample)
{
	#if _IS_APPS_USE_DAC
	U8 i;
	if(DAC_status&DAC0_trig)
    {
    	#if _DAC_CH0_TIMER==USE_TIMER0
        	P_DAC->CH0.CH_Ctrl |= DAC_TIMER0_TRIG;
      	#elif _DAC_CH0_TIMER==USE_TIMER1
      		P_DAC->CH0.CH_Ctrl |= DAC_TIMER1_TRIG;
      	#else
      		P_DAC->CH0.CH_Ctrl |= DAC_TIMER2_TRIG;
      	#endif
    
    	//Write DAC FIFO data to avoid the FIFO empty before dac timer trigger 
        for(i=0;i<DAC_FIFO_LEN;i++)
        {
            DAC_CH0->CH_Data=0x0;
        }

      	if(upsample&(1<<0))
		{
			P_DAC->CH0.CH_Ctrl |= C_DAC_Up_Sample_En;
		}
      		DAC_Cmd(DAC_CH0,ENABLE);
      }
     if(DAC_status&DAC1_trig)
     {
		#if _DAC_CH1_TIMER==USE_TIMER0
        	P_DAC->CH1.CH_Ctrl |= DAC_TIMER0_TRIG;
      	#elif _DAC_CH1_TIMER==USE_TIMER1
      		P_DAC->CH1.CH_Ctrl |= DAC_TIMER1_TRIG;
      	#else
      		P_DAC->CH1.CH_Ctrl |= DAC_TIMER2_TRIG;
      	#endif
      	
      	//Write DAC FIFO data to avoid the FIFO empty before dac timer trigger 
        for(i=0;i<DAC_FIFO_LEN;i++)
        {
            DAC_CH0->CH_Data=0x0;
        }
        
      	if(upsample&(1<<1))
		{
			P_DAC->CH1.CH_Ctrl |= C_DAC_Up_Sample_En;
		}
		DAC_Cmd(DAC_CH1,ENABLE);
	 }

	#endif
}

//------------------------------------------------------------------//
// DAC ramp control for mixer channel
// Parameter: 
//          cmd:RAMP_UP,RAMP_DOWN, DISABLE
// return value:
//          NONE
//------------------------------------------------------------------//     
void DacMixerRamp(U8 DAC_status,U8 cmd)
{
    extern void DelayLoop2(U32 count);
    U16 data;
    
    if(cmd==RAMP_UP)	//ramp up mix 
    {
		data=0x8000;

    	DAC_CH0->CH_Data=data;
        DAC_CH0->CH_Ctrl &= (~C_DAC_Trig_Timer_En);
        DAC_CH0->CH_Ctrl &= ~(C_DAC_Trig|C_DAC_Up_Sample_En);
        DAC_CH0->CH_Ctrl |= DAC_MANUAL_TRIG;
	

        DAC_CH1->CH_Data=data;
        DAC_CH1->CH_Ctrl &= (~C_DAC_Trig_Timer_En);
        DAC_CH1->CH_Ctrl &= ~(C_DAC_Trig|C_DAC_Up_Sample_En);
        DAC_CH1->CH_Ctrl |= DAC_MANUAL_TRIG;
 
        while(1)
        {
            data++;
            DAC_CH0->CH_Data=data;
			DAC_CH1->CH_Data=data;

            if(data==0)
            {
                break;
            }      
            DelayLoop2(_AUDIO_RAMP_UP_TIME);
        }    
    }
    else if(cmd==RAMP_DOWN) // ramp down independently , no mix
    {  	
    	if(DAC_status&DAC0_trig)
		{
	    	#if _DAC_CH0_TIMER==USE_TIMER0
	            TIMER_IntCmd(TIMER0,DISABLE);
	      	#elif _DAC_CH0_TIMER==USE_TIMER1
	      		TIMER_IntCmd(TIMER1,DISABLE);
	      	#else
	      		TIMER_IntCmd(TIMER2,DISABLE);
	      	#endif
	    
			DAC_CH0->CH_FIFO |= C_DAC_FIFO_Reset;
	        DAC_CH0->CH_Ctrl &= (~C_DAC_Trig_Timer_En);
	        DAC_CH0->CH_Ctrl &= ~(C_DAC_Trig|C_DAC_Up_Sample_En);
	        DAC_CH0->CH_Ctrl |= DAC_MANUAL_TRIG;
        }
        if(DAC_status&DAC1_trig)
		{
	    	#if _DAC_CH1_TIMER==USE_TIMER0
	            TIMER_IntCmd(TIMER0,DISABLE);
	      	#elif _DAC_CH1_TIMER==USE_TIMER1
	      		TIMER_IntCmd(TIMER1,DISABLE);
	      	#else
	      		TIMER_IntCmd(TIMER2,DISABLE);
	      	#endif
	        
	        
	        DAC_CH1->CH_FIFO |= C_DAC_FIFO_Reset;
	        DAC_CH1->CH_Ctrl &= (~C_DAC_Trig_Timer_En);
	        DAC_CH1->CH_Ctrl &= ~(C_DAC_Trig|C_DAC_Up_Sample_En);
	        DAC_CH1->CH_Ctrl |= DAC_MANUAL_TRIG;
        }
        data=0;
        
        if(DAC_status&DAC0_trig)
		{
        	DAC_CH0->CH_Data=data;
        }
        if(DAC_status&DAC1_trig)
		{
       		DAC_CH1->CH_Data=data;
        }
        
        while(1)
        {
            data--;
            if(DAC_status&DAC0_trig)
			{
            	DAC_CH0->CH_Data=data;
        	}
        	if(DAC_status&DAC1_trig)
			{
        		DAC_CH1->CH_Data=data;
            }
            if(data==0x8000)
            {
                break;
            }     
            DelayLoop2(_AUDIO_RAMP_DOWN_TIME);
        }    
    } 
}   


