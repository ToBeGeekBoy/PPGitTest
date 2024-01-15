#include "nx1_smu.h"
#include "include.h"
#include "nx1_uart.h"
#include "nx1_dac.h"
#include "nx1_reg.h"
#include "nx1_timer.h"
#include "nx1_pwm.h"
#include "nx1_gpio.h"


#if _EF_SERIES
//------------------------------------------------------------------//
// Select cpu clock
// Parameter: 
//          divide    : SMU_SYSCLKDIV_1~SMU_SYSCLKDIV_128
// return value:
//          NONE
//------------------------------------------------------------------//    
void SMU_CpuClock(U32 divide)
{
    P_SMU->CLK_Ctrl = divide;
} 
#else
//------------------------------------------------------------------//
// Select cpu clock
// Parameter: 
//          divide    : SMU_SYSCLKDIV_1~SMU_SYSCLKDIV_128
//          clock_src : SMU_SYSCLK_FROM_HICLK,SMU_SYSCLK_FROM_LOCLK
// return value:
//          NONE
//------------------------------------------------------------------//    
void SMU_CpuClock(U32 divide,U8 clock_src)
{
    P_SMU->CLK_Ctrl = divide | clock_src | C_HiCLK_En;
}    
#endif
//------------------------------------------------------------------//
// Periphral reset
// Parameter: 
//          periphs:SMU_PERIPH_IR,....
// return value:
//          NONE
//------------------------------------------------------------------//    
void SMU_PeriphReset(U32 periphs)
{
    P_SMU->FUNC_RST |= periphs;
}    
//------------------------------------------------------------------//
// Enable or disable periphral clock
// Parameter: 
//          periphs: SMU_PERIPH_IR,....
//          cmd    : ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------//    
void SMU_PeriphClkCmd(U32 periphs,U8 cmd)
{
    if(cmd==ENABLE)
    {
        P_SMU->FUNC_Ctrl |= periphs;
    }
    else
    {
        P_SMU->FUNC_Ctrl &= (~periphs);
    }   
}       
//------------------------------------------------------------------//
// Get periphral clock setting
// Parameter: 
//          NONE
// return value:
//          return: Periph clock setting
//------------------------------------------------------------------//    
U16 SMU_GetPeriphClk(void)
{
    return ( (P_SMU->FUNC_Ctrl)&0xFFFF );
}       
//------------------------------------------------------------------//
// Cpu core reset
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//    
void SMU_CpuReset(void)
{
	SMU_ClrResetSrc(SMU_RESETSRC_CPU);
    P_SMU->RST_SW_Ctrl = (C_RST_SW_PROT_Value | C_RST_SW_CPU_Exe);
}
//------------------------------------------------------------------//
// Whole chip reset
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//    
void SMU_ChipReset(void)
{
	SMU_ClrResetSrc(SMU_RESETSRC_CHIP);
    P_SMU->RST_SW_Ctrl = (C_RST_SW_PROT_Value | C_RST_SW_Chip_Exe);
}
//------------------------------------------------------------------//
// Enter Sleep mode
// Parameter: 
//          mode: SMU_GREEN_MODE, SMU_HALT_MODE, SMU_STANDBY_MODE
// return value:
//          NONE
//------------------------------------------------------------------//    
void SMU_Sleep(U8 mode)
{   
    P_SMU->PWR_Ctrl &= (~C_SLP_Mode);
    P_SMU->PWR_Ctrl |= mode;
    STANDBY(2);    
}     
//------------------------------------------------------------------//
// Get reset source
// Parameter: 
//          NONE
// return value:
//          reset source : SMU_RESETSRC_WDT, SMU_RESETSRC_POR_LVR,
//                         SMU_RESETSRC_EXT, SMU_RESETSRC_CHIP, SMU_RESETSRC_CPU 
//------------------------------------------------------------------//    
U8 SMU_GetResetSrc(void)
{
    return P_SMU->RST_Flag;
}    
//------------------------------------------------------------------//
// Clear reset source
// Parameter: 
//          value: write 1 to clear
// return value:
//          NONE
//------------------------------------------------------------------//    
void SMU_ClrResetSrc(U8 value)
{
    P_SMU->RST_Flag=value;
}    
//------------------------------------------------------------------//
// Trig SWA interrupt
// Parameter: 
//          cmd: BIT_SET,BIT_UNSET
// return value:
//          NONE
//------------------------------------------------------------------//    
void SMU_SWATrig(U8 value)
{
	if (value)
	{
		P_SMU->SW_INT = C_SW_INT_PROT_Value | 1;
	}
	else
	{
		P_SMU->SW_INT = value;
	}
}
//------------------------------------------------------------------//
// Enable or disable LDO33
// Parameter: 
//          cmd: ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------//    
void SMU_LDO33Cmd(U8 cmd)
{
    if(cmd==ENABLE)
    {
        P_SMU->PWR_Ctrl |= C_LDOSPI0_En;
    }
    else
    {
        P_SMU->PWR_Ctrl &= (~C_LDOSPI0_En);
    }    
}   
#if _EF_SERIES
//------------------------------------------------------------------//
// Enable or Disable Analog LDO
// Parameter: 
//          cmd : ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------//    
void SMU_ALDOCmd(U8 cmd)
{
    if(cmd==ENABLE)
    {
        P_SMU->PWR_Ctrl |= C_ALDO_En;
    }
    else
    {
    	P_SMU->PWR_Ctrl &= (~C_ALDO_En);
    }       
}
#endif
#if !_EF_SERIES
//------------------------------------------------------------------//
// Enable or disable DCM
// Parameter: 
//          cmd: ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------//    
void SMU_DCMCmd(U8 cmd)
{
    if(cmd==ENABLE)
    {
        P_SMU->PWR_Ctrl |= C_CSC_Bias_En;
    }
    else
    {
        P_SMU->PWR_Ctrl &= (~C_CSC_Bias_En);
    }    
}   
#endif
//------------------------------------------------------------------//
// Enable or disable LVD
// Parameter: 
//          cmd: ENABLE,DISABLE
//          voltage: * SMU_LVD_2_2V,SMU_LVD_2_4V,SMU_LVD_2_6V (_EF_SERIES not supported),SMU_LVD_3_2V,SMU_LVD_3_4V,SMU_LVD_3_6V
//					 * SMU_LVD_2_0V,SMU_LVD_2_8V,SMU_LVD_3_0V (_EF_SERIES only)
// return value:
//          NONE
//------------------------------------------------------------------//    
void SMU_LVDCmd(U8 cmd,U16 voltage)
{
    P_SMU->PWR_Ctrl &= (~C_LVD);
    P_SMU->PWR_Ctrl |= voltage;
    
    if(cmd==ENABLE)
    {
        P_SMU->PWR_Ctrl |= C_LVD_En;
    }
    else
    {
        P_SMU->PWR_Ctrl &= (~C_LVD_En);
    }    
}   
//------------------------------------------------------------------//
// Get LVD status
// Parameter: 
//          NONE
// return value:
//          0 : not low voltage 
//          1 : low voltage
//------------------------------------------------------------------//    
U8 SMU_GetLVDSts(void)
{
    if(P_SMU->PWR_Ctrl&C_LVD_Flag)
    {
        return 1;
    }
    else
    {
        return 0;
    }    
}   



//------------------------------------------------------------------//
// Disable Interrupt before enter the halt mode
// Parameter: 
//          NONE
// return value:
//          NONE 
//------------------------------------------------------------------//    
int SMU_DIS_INT(void)
{

//Main function check and disable

#if _TIMER_MODULE
	#if _TIMER_TMR0_INT || (_VIO_TIMER==USE_TIMER0 && _ACTION_MODULE)
		if (((P_TIMER0->Ctrl)&C_TMR_En)==C_TMR_En)
		{
			TIMER_Cmd(TIMER0,DISABLE);
		}
	#endif
	
	#if _TIMER_TMR1_INT || (_VIO_TIMER==USE_TIMER1 && _ACTION_MODULE)
		if (((P_TIMER1->Ctrl)&C_TMR_En)==C_TMR_En)
		{
			TIMER_Cmd(TIMER1,DISABLE);
		}
	#endif
	
	#if _TIMER_TMR2_INT || (_VIO_TIMER==USE_TIMER2 && _ACTION_MODULE)
		if (((P_TIMER2->Ctrl)&C_TMR_En)==C_TMR_En)
		{
			TIMER_Cmd(TIMER2,DISABLE);
		}
	#endif
	
	#if _EF_SERIES
	#if (_TIMER_TMR3_INT || (_VIO_TIMER==USE_TIMER3 && _ACTION_MODULE))
		if (((P_TIMER3->Ctrl)&C_TMR_En)==C_TMR_En)
		{
			TIMER_Cmd(TIMER3,DISABLE);
		}
	#endif
	#endif	

	#if _PWMA_INT || (_VIO_TIMER==USE_TIMER_PWMA && _ACTION_MODULE)
		if (((PWMATIMER->Ctrl)&C_TMR_En)==C_TMR_En)
		{
			TIMER_Cmd(PWMATIMER,DISABLE);
		}
	#endif

	#if _PWMB_INT || (_VIO_TIMER==USE_TIMER_PWMB && _ACTION_MODULE)
		if (((PWMBTIMER->Ctrl)&C_TMR_En)==C_TMR_En)
		{
			TIMER_Cmd(PWMBTIMER,DISABLE);
		}
	#endif
#endif

#if _UART_MODULE
	if (((P_UART->Ctrl)&1)==1)
	{
		UART_IntCmd(UART,UART_RXFIFO_NOEMPTY,DISABLE);
	}
	if (((P_UART->Ctrl)&2)==2)
	{
		UART_IntCmd(UART,UART_TX_DONE,DISABLE);
	}
#if !EF11FS_SERIES
	if (((P_UART->Ctrl)&4)==4)
	{
		UART_IntCmd(UART,UART_RXFIFO_FULL,DISABLE);
	}
#endif
#endif

#if _GPIO_MODULE
	if(((P_PORT_EXT->EXT_INT_Ctrl)&1)==1)
	{
		GPIO_ExtIntCmd(TRIG_EXT0, DISABLE);
	}
#endif

#if _PWMA_MODULE
	#if _PWMA_INT
		if((P_PWMA->TMR_INT_Ctrl)&C_PWM_TMR_IEn == C_PWM_TMR_IEn)
		{
			P_PWMA->TMR_INT_Ctrl &= (~C_PWM_TMR_IEn);
		}
	#endif
#endif

#if _PWMB_MODULE
	#if _PWMB_INT
		if((P_PWMB->TMR_INT_Ctrl)&C_PWM_TMR_IEn == C_PWM_TMR_IEn)
		{
			P_PWMB->TMR_INT_Ctrl &= (~C_PWM_TMR_IEn);
		}
	#endif
#endif


#if _CAPTURE_MODULE
	#if _CAPTURE_INT
		if(((P_PWMA->CAP_INT_Ctrl)&PWM_INT_CAPTURE_RISING) == PWM_INT_CAPTURE_RISING)
		{
			P_PWMA->CAP_INT_Ctrl &= (~PWM_INT_CAPTURE_RISING);
		}
		if(((P_PWMA->CAP_INT_Ctrl)&PWM_INT_CAPTURE_FALLING) == PWM_INT_CAPTURE_FALLING)
		{
			P_PWMA->CAP_INT_Ctrl &= (~PWM_INT_CAPTURE_FALLING);
		}
	#endif
#endif


//ADC function check and disable
#if _IS_APPS_USE_ADC_MIC
    if(((P_ADC->FIFO_Ctrl)&C_ADC_CNV_Done_IEn) == C_ADC_CNV_Done_IEn)
    {
    	P_ADC->FIFO_Ctrl &= ~(C_ADC_CNV_Done_IEn);
    }
    if(((P_ADC->FIFO_Ctrl)&C_ADC_FIFO_Level_IEn) == C_ADC_FIFO_Level_IEn)
    {
    	P_ADC->FIFO_Ctrl &= ~(C_ADC_FIFO_Level_IEn);
    }
    if(((P_ADC->FIFO_Ctrl)&C_ADC_FIFO_Level_IEn) == C_ADC_FIFO_Level_IEn)
    {
    	P_ADC->FIFO_Ctrl &= ~(C_ADC_FIFO_Level_IEn);
    }
    if(((P_ADC->FIFO_Ctrl)&C_ADC_FIFO_Empty_IEn) == C_ADC_FIFO_Empty_IEn)
    {
    	P_ADC->FIFO_Ctrl &= ~(C_ADC_FIFO_Empty_IEn);
    }
    if(((P_ADC->FIFO_Ctrl)&C_ADC_FIFO_Full_IEn) == C_ADC_FIFO_Full_IEn)
    {
    	P_ADC->FIFO_Ctrl &= ~(C_ADC_FIFO_Full_IEn);
    }
#endif

//Audio function check and disable
#if _ADPCM_PLAY    
    	#if _ADPCM_PLAY_AUDIO_CH==0
			DAC_Cmd(DAC_CH0,DISABLE);
		#else
			DAC_Cmd(DAC_CH1,DISABLE);
		#endif   
#endif
#if _ADPCM_CH2_PLAY
    	#if _ADPCM_CH2_PLAY_AUDIO_CH==0
			DAC_Cmd(DAC_CH0,DISABLE);
		#else
			DAC_Cmd(DAC_CH1,DISABLE);
		#endif      
#endif
#if _ADPCM_CH3_PLAY
    	#if _ADPCM_CH3_PLAY_AUDIO_CH==0
			DAC_Cmd(DAC_CH0,DISABLE);
		#else
			DAC_Cmd(DAC_CH1,DISABLE);
		#endif      
#endif
#if _ADPCM_CH4_PLAY
    	#if _ADPCM_CH4_PLAY_AUDIO_CH==0
			DAC_Cmd(DAC_CH0,DISABLE);
		#else
			DAC_Cmd(DAC_CH1,DISABLE);
		#endif      
#endif
#if _SBC_PLAY    
    	#if _SBC_PLAY_AUDIO_CH==0
			DAC_Cmd(DAC_CH0,DISABLE);
		#else
			DAC_Cmd(DAC_CH1,DISABLE);
		#endif   
#endif
#if _SBC_CH2_PLAY
    	#if _SBC_CH2_PLAY_AUDIO_CH==0
			DAC_Cmd(DAC_CH0,DISABLE);
		#else
			DAC_Cmd(DAC_CH1,DISABLE);
		#endif    
#endif
#if _CELP_PLAY    
    	#if _CELP_PLAY_AUDIO_CH==0
			DAC_Cmd(DAC_CH0,DISABLE);
		#else
			DAC_Cmd(DAC_CH1,DISABLE);
		#endif   
#endif
#if _MIDI_PLAY    
    	#if _MIDI_PLAY_AUDIO_CH==0
			DAC_Cmd(DAC_CH0,DISABLE);
		#else
			DAC_Cmd(DAC_CH1,DISABLE);
		#endif   
#endif
#if _PCM_PLAY    
    	#if _PCM_PLAY_AUDIO_CH==0
			DAC_Cmd(DAC_CH0,DISABLE);
		#else
			DAC_Cmd(DAC_CH1,DISABLE);
		#endif   
#endif
#if _PCM_CH2_PLAY    
    	#if _PCM_CH2_PLAY_AUDIO_CH==0
			DAC_Cmd(DAC_CH0,DISABLE);
		#else
			DAC_Cmd(DAC_CH1,DISABLE);
		#endif   
#endif
#if _PCM_CH3_PLAY    
    	#if _PCM_CH3_PLAY_AUDIO_CH==0
			DAC_Cmd(DAC_CH0,DISABLE);
		#else
			DAC_Cmd(DAC_CH1,DISABLE);
		#endif   
#endif
#if _RT_PLAY
    	#if _RT_PLAY_AUDIO_CH==0
			DAC_Cmd(DAC_CH0,DISABLE);
		#else
			DAC_Cmd(DAC_CH1,DISABLE);
		#endif   
#endif
	return 0;
}   
