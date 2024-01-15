#include "include.h"
#include "nx1_lib.h"
#include "nx1_reg.h"
#include "nx1_gpio.h"
#include "nx1_timer.h"
#include "nx1_intc.h"
#include "nx1_spi.h"
#include "nx1_rtc.h"
#include "nx1_pwm.h"
#include "nx1_smu.h"
#include "UserDefinedTab.h"
#include <string.h>
#include "LEDSTRTab.h"
#if _LEDSTRm_MODULE

#if _LEDSTRm_IO_TOTAL_NUM==0
	#error "LEDSTRm_IO_TOTAL_NUM can not be 0."
#endif

#if _LEDSTRm_IO_TOTAL_NUM>16
	#error "LEDSTRm_IO_TOTAL_NUM over 16."
#endif

#if		((_LEDSTRm_IO_TOTAL_NUM>0)&&_LEDSTRm_OUTPUT0>GPIO_MAX)	\
		||((_LEDSTRm_IO_TOTAL_NUM>1)&&_LEDSTRm_OUTPUT1>GPIO_MAX)	\
		||((_LEDSTRm_IO_TOTAL_NUM>2)&&_LEDSTRm_OUTPUT2>GPIO_MAX)	\
		||((_LEDSTRm_IO_TOTAL_NUM>3)&&_LEDSTRm_OUTPUT3>GPIO_MAX)	\
		||((_LEDSTRm_IO_TOTAL_NUM>4)&&_LEDSTRm_OUTPUT4>GPIO_MAX)	\
		||((_LEDSTRm_IO_TOTAL_NUM>5)&&_LEDSTRm_OUTPUT5>GPIO_MAX)	\
		||((_LEDSTRm_IO_TOTAL_NUM>6)&&_LEDSTRm_OUTPUT6>GPIO_MAX)	\
		||((_LEDSTRm_IO_TOTAL_NUM>7)&&_LEDSTRm_OUTPUT7>GPIO_MAX)	\
		||((_LEDSTRm_IO_TOTAL_NUM>8)&&_LEDSTRm_OUTPUT8>GPIO_MAX)	\
		||((_LEDSTRm_IO_TOTAL_NUM>9)&&_LEDSTRm_OUTPUT9>GPIO_MAX)	\
		||((_LEDSTRm_IO_TOTAL_NUM>10)&&_LEDSTRm_OUTPUT10>GPIO_MAX)	\
		||((_LEDSTRm_IO_TOTAL_NUM>11)&&_LEDSTRm_OUTPUT11>GPIO_MAX)	\
		||((_LEDSTRm_IO_TOTAL_NUM>12)&&_LEDSTRm_OUTPUT12>GPIO_MAX)	\
		||((_LEDSTRm_IO_TOTAL_NUM>13)&&_LEDSTRm_OUTPUT13>GPIO_MAX)	\
		||((_LEDSTRm_IO_TOTAL_NUM>14)&&_LEDSTRm_OUTPUT14>GPIO_MAX)	\
		||((_LEDSTRm_IO_TOTAL_NUM>15)&&_LEDSTRm_OUTPUT15>GPIO_MAX)
		#error "LEDSTRm OUTPUT Pin undefine"
#endif


#define _LEDSTRm_CHK_SAMEPORT	(	((_LEDSTRm_IO_TOTAL_NUM>0)?(0x1<<GPIO_GetPortIdx(_LEDSTRm_OUTPUT0)):0)	\
								|	((_LEDSTRm_IO_TOTAL_NUM>1)?(0x1<<GPIO_GetPortIdx(_LEDSTRm_OUTPUT1)):0)	\
								|	((_LEDSTRm_IO_TOTAL_NUM>2)?(0x1<<GPIO_GetPortIdx(_LEDSTRm_OUTPUT2)):0)	\
								|	((_LEDSTRm_IO_TOTAL_NUM>3)?(0x1<<GPIO_GetPortIdx(_LEDSTRm_OUTPUT3)):0)	\
								|	((_LEDSTRm_IO_TOTAL_NUM>4)?(0x1<<GPIO_GetPortIdx(_LEDSTRm_OUTPUT4)):0)	\
								|	((_LEDSTRm_IO_TOTAL_NUM>5)?(0x1<<GPIO_GetPortIdx(_LEDSTRm_OUTPUT5)):0)	\
								|	((_LEDSTRm_IO_TOTAL_NUM>6)?(0x1<<GPIO_GetPortIdx(_LEDSTRm_OUTPUT6)):0)	\
								|	((_LEDSTRm_IO_TOTAL_NUM>7)?(0x1<<GPIO_GetPortIdx(_LEDSTRm_OUTPUT7)):0)	\
								|	((_LEDSTRm_IO_TOTAL_NUM>8)?(0x1<<GPIO_GetPortIdx(_LEDSTRm_OUTPUT8)):0)	\
								|	((_LEDSTRm_IO_TOTAL_NUM>9)?(0x1<<GPIO_GetPortIdx(_LEDSTRm_OUTPUT9)):0)	\
								|	((_LEDSTRm_IO_TOTAL_NUM>10)?(0x1<<GPIO_GetPortIdx(_LEDSTRm_OUTPUT10)):0)	\
								|	((_LEDSTRm_IO_TOTAL_NUM>11)?(0x1<<GPIO_GetPortIdx(_LEDSTRm_OUTPUT11)):0)	\
								|	((_LEDSTRm_IO_TOTAL_NUM>12)?(0x1<<GPIO_GetPortIdx(_LEDSTRm_OUTPUT12)):0)	\
								|	((_LEDSTRm_IO_TOTAL_NUM>13)?(0x1<<GPIO_GetPortIdx(_LEDSTRm_OUTPUT13)):0)	\
								|	((_LEDSTRm_IO_TOTAL_NUM>14)?(0x1<<GPIO_GetPortIdx(_LEDSTRm_OUTPUT14)):0)	\
								|	((_LEDSTRm_IO_TOTAL_NUM>15)?(0x1<<GPIO_GetPortIdx(_LEDSTRm_OUTPUT15)):0)	)
								
#if	((_LEDSTRm_CHK_SAMEPORT)&(_LEDSTRm_CHK_SAMEPORT-1))>0
	#error "LEDSTRm OUTPUT Port different"
#endif

#define _LEDSTRm_CHK_SAMEPIN			(	((_LEDSTRm_IO_TOTAL_NUM>0)?(1<<GPIO_GetPin(_LEDSTRm_OUTPUT0)):0)	\
										|	((_LEDSTRm_IO_TOTAL_NUM>1)?(1<<GPIO_GetPin(_LEDSTRm_OUTPUT1)):0)	\
										|	((_LEDSTRm_IO_TOTAL_NUM>2)?(1<<GPIO_GetPin(_LEDSTRm_OUTPUT2)):0)	\
										|	((_LEDSTRm_IO_TOTAL_NUM>3)?(1<<GPIO_GetPin(_LEDSTRm_OUTPUT3)):0)	\
										|	((_LEDSTRm_IO_TOTAL_NUM>4)?(1<<GPIO_GetPin(_LEDSTRm_OUTPUT4)):0)	\
										|	((_LEDSTRm_IO_TOTAL_NUM>5)?(1<<GPIO_GetPin(_LEDSTRm_OUTPUT5)):0)	\
										|	((_LEDSTRm_IO_TOTAL_NUM>6)?(1<<GPIO_GetPin(_LEDSTRm_OUTPUT6)):0)	\
										|	((_LEDSTRm_IO_TOTAL_NUM>7)?(1<<GPIO_GetPin(_LEDSTRm_OUTPUT7)):0)	\
										|	((_LEDSTRm_IO_TOTAL_NUM>8)?(1<<GPIO_GetPin(_LEDSTRm_OUTPUT8)):0)	\
										|	((_LEDSTRm_IO_TOTAL_NUM>9)?(1<<GPIO_GetPin(_LEDSTRm_OUTPUT9)):0)	\
										|	((_LEDSTRm_IO_TOTAL_NUM>10)?(1<<GPIO_GetPin(_LEDSTRm_OUTPUT10)):0)	\
										|	((_LEDSTRm_IO_TOTAL_NUM>11)?(1<<GPIO_GetPin(_LEDSTRm_OUTPUT11)):0)	\
										|	((_LEDSTRm_IO_TOTAL_NUM>12)?(1<<GPIO_GetPin(_LEDSTRm_OUTPUT12)):0)	\
										|	((_LEDSTRm_IO_TOTAL_NUM>13)?(1<<GPIO_GetPin(_LEDSTRm_OUTPUT13)):0)	\
										|	((_LEDSTRm_IO_TOTAL_NUM>14)?(1<<GPIO_GetPin(_LEDSTRm_OUTPUT14)):0)	\
										|	((_LEDSTRm_IO_TOTAL_NUM>15)?(1<<GPIO_GetPin(_LEDSTRm_OUTPUT15)):0)	)
															
#if (	 \
			 ((_LEDSTRm_CHK_SAMEPIN&(0x1<<0))>0)	\
			+((_LEDSTRm_CHK_SAMEPIN&(0x1<<1))>0)	\
			+((_LEDSTRm_CHK_SAMEPIN&(0x1<<2))>0)	\
			+((_LEDSTRm_CHK_SAMEPIN&(0x1<<3))>0)	\
			+((_LEDSTRm_CHK_SAMEPIN&(0x1<<4))>0)	\
			+((_LEDSTRm_CHK_SAMEPIN&(0x1<<5))>0)	\
			+((_LEDSTRm_CHK_SAMEPIN&(0x1<<6))>0)	\
			+((_LEDSTRm_CHK_SAMEPIN&(0x1<<7))>0)	\
			+((_LEDSTRm_CHK_SAMEPIN&(0x1<<8))>0)	\
			+((_LEDSTRm_CHK_SAMEPIN&(0x1<<9))>0)	\
			+((_LEDSTRm_CHK_SAMEPIN&(0x1<<10))>0)	\
			+((_LEDSTRm_CHK_SAMEPIN&(0x1<<11))>0)	\
			+((_LEDSTRm_CHK_SAMEPIN&(0x1<<12))>0)	\
			+((_LEDSTRm_CHK_SAMEPIN&(0x1<<13))>0)	\
			+((_LEDSTRm_CHK_SAMEPIN&(0x1<<14))>0)	\
			+((_LEDSTRm_CHK_SAMEPIN&(0x1<<15))>0)	\
		)!=_LEDSTRm_IO_TOTAL_NUM
		#error "LEDSTRm OUTPUT Pin repeat "
#endif
#if		  ((_LEDSTRm_IO_TOTAL_NUM>1)&&(((GPIO_GetPin(_LEDSTRm_OUTPUT1))-(GPIO_GetPin(_LEDSTRm_OUTPUT0)))!=1))	\
		||((_LEDSTRm_IO_TOTAL_NUM>2)&&(((GPIO_GetPin(_LEDSTRm_OUTPUT2))-(GPIO_GetPin(_LEDSTRm_OUTPUT1)))!=1))	\
		||((_LEDSTRm_IO_TOTAL_NUM>3)&&(((GPIO_GetPin(_LEDSTRm_OUTPUT3))-(GPIO_GetPin(_LEDSTRm_OUTPUT2)))!=1))	\
		||((_LEDSTRm_IO_TOTAL_NUM>4)&&(((GPIO_GetPin(_LEDSTRm_OUTPUT4))-(GPIO_GetPin(_LEDSTRm_OUTPUT3)))!=1))	\
		||((_LEDSTRm_IO_TOTAL_NUM>5)&&(((GPIO_GetPin(_LEDSTRm_OUTPUT5))-(GPIO_GetPin(_LEDSTRm_OUTPUT4)))!=1))	\
		||((_LEDSTRm_IO_TOTAL_NUM>6)&&(((GPIO_GetPin(_LEDSTRm_OUTPUT6))-(GPIO_GetPin(_LEDSTRm_OUTPUT5)))!=1))	\
		||((_LEDSTRm_IO_TOTAL_NUM>7)&&(((GPIO_GetPin(_LEDSTRm_OUTPUT7))-(GPIO_GetPin(_LEDSTRm_OUTPUT6)))!=1))	\
		||((_LEDSTRm_IO_TOTAL_NUM>8)&&(((GPIO_GetPin(_LEDSTRm_OUTPUT8))-(GPIO_GetPin(_LEDSTRm_OUTPUT7)))!=1))	\
		||((_LEDSTRm_IO_TOTAL_NUM>9)&&(((GPIO_GetPin(_LEDSTRm_OUTPUT9))-(GPIO_GetPin(_LEDSTRm_OUTPUT8)))!=1))	\
		||((_LEDSTRm_IO_TOTAL_NUM>10)&&(((GPIO_GetPin(_LEDSTRm_OUTPUT10))-(GPIO_GetPin(_LEDSTRm_OUTPUT9)))!=1))	\
		||((_LEDSTRm_IO_TOTAL_NUM>11)&&(((GPIO_GetPin(_LEDSTRm_OUTPUT11))-(GPIO_GetPin(_LEDSTRm_OUTPUT10)))!=1))	\
		||((_LEDSTRm_IO_TOTAL_NUM>12)&&(((GPIO_GetPin(_LEDSTRm_OUTPUT12))-(GPIO_GetPin(_LEDSTRm_OUTPUT11)))!=1))	\
		||((_LEDSTRm_IO_TOTAL_NUM>13)&&(((GPIO_GetPin(_LEDSTRm_OUTPUT13))-(GPIO_GetPin(_LEDSTRm_OUTPUT12)))!=1))	\
		||((_LEDSTRm_IO_TOTAL_NUM>14)&&(((GPIO_GetPin(_LEDSTRm_OUTPUT14))-(GPIO_GetPin(_LEDSTRm_OUTPUT13)))!=1))	\
		||((_LEDSTRm_IO_TOTAL_NUM>15)&&(((GPIO_GetPin(_LEDSTRm_OUTPUT15))-(GPIO_GetPin(_LEDSTRm_OUTPUT14)))!=1))
		#error "LEDSTRn OUTPUT Pin discontinuousr "
#endif

void CB_LEDSTRm_GetData(U8* _buf, U32 _addr, U32 _size, U8 _mode);

#define _LEDSTRm_Max_Amount			((_LEDSTRm_TIMING * 10000) / 625)
#define _LEDSTRm_RGB_BYTE			3
#define _LEDSTRm_RGB_BIT			24

#if	_LEDSTRm_IO_TOTAL_NUM<=8
#define _LEDSTRm_Data_Byte			1
#else
#define _LEDSTRm_Data_Byte			2
#endif

U8 LEDSTRm_ID[_LEDSTRm_IO_TOTAL_NUM] = { 0 };
U8 LEDSTRm_ID_const[_LEDSTRm_IO_TOTAL_NUM] = { 0 };
U8 LEDSTRm_ID_temp[_LEDSTRm_IO_TOTAL_NUM] =  { 0 };

const U16 LEDSTRm_parameter[] = {
	_LEDSTRm_Data_Byte,_LEDSTRm_IO_TOTAL_NUM,_LEDSTRm_MAX_NUM,_LEDSTRm_Max_Amount
};


void RGB_Pattern3(GPIO_TypeDef *gpiox,U8 *code);

void RGB_Pattern4(GPIO_TypeDef *gpiox,U16 *code);


U8 LEDm_pattern_flag;
U8 Color_buf_T1[24] __attribute__((aligned(4)));
U8 temp_buf_T1[24] __attribute__((aligned(4)));
U16 Color_buf_T2[24] __attribute__((aligned(4)));
U16 temp_buf_T2[24] __attribute__((aligned(4)));

U8 IO_num_temp;
U16 IO_Mask=0,IO_Mask_not;
U8 IO_shift;
static	GPIO_TypeDef *LEDSTRm_port;
//------------------------------------------------------------------//
// LEDSTRm Init
// Parameter:
//          NONE
// return value:
//     		NONE
//------------------------------------------------------------------//
void CB_LEDSTRm_Init(void)
{
#if	_LEDSTRm_IO_TOTAL_NUM > 0
	LEDSTRm_ID[0] = GPIO_GetPin(_LEDSTRm_OUTPUT0);
	LEDSTRm_ID_const[0] = LEDSTRm_ID[0];
	LEDSTRm_ID_temp[0] = LEDSTRm_ID[0];
	LEDSTRm_port=GPIO_GetPort(_LEDSTRm_OUTPUT0);
	IO_shift = LEDSTRm_ID[0];
#endif
#if	_LEDSTRm_IO_TOTAL_NUM > 1
	LEDSTRm_ID[1] = GPIO_GetPin(_LEDSTRm_OUTPUT1);
	LEDSTRm_ID_const[1] = LEDSTRm_ID[1];
	LEDSTRm_ID_temp[1] = LEDSTRm_ID[1];
	if(LEDSTRm_port!=GPIO_GetPort(_LEDSTRm_OUTPUT1))
		return;
#endif
#if	_LEDSTRm_IO_TOTAL_NUM > 2
	LEDSTRm_ID[2] = GPIO_GetPin(_LEDSTRm_OUTPUT2);
	LEDSTRm_ID_const[2] = LEDSTRm_ID[2];
	LEDSTRm_ID_temp[2] = LEDSTRm_ID[2];	
#endif
#if	_LEDSTRm_IO_TOTAL_NUM > 3
	LEDSTRm_ID[3] = GPIO_GetPin(_LEDSTRm_OUTPUT3);
	LEDSTRm_ID_const[3] = LEDSTRm_ID[3];
	LEDSTRm_ID_temp[3] = LEDSTRm_ID[3];	
#endif
#if	_LEDSTRm_IO_TOTAL_NUM > 4
	LEDSTRm_ID[4] = GPIO_GetPin(_LEDSTRm_OUTPUT4);
	LEDSTRm_ID_const[4] = LEDSTRm_ID[4];
	LEDSTRm_ID_temp[4] = LEDSTRm_ID[4];	
#endif
#if	_LEDSTRm_IO_TOTAL_NUM > 5
	LEDSTRm_ID[5] = GPIO_GetPin(_LEDSTRm_OUTPUT5);
	LEDSTRm_ID_const[5] = LEDSTRm_ID[5];
	LEDSTRm_ID_temp[5] = LEDSTRm_ID[5];	
#endif
#if	_LEDSTRm_IO_TOTAL_NUM > 6
	LEDSTRm_ID[6] = GPIO_GetPin(_LEDSTRm_OUTPUT6);
	LEDSTRm_ID_const[6] = LEDSTRm_ID[6];
	LEDSTRm_ID_temp[6] = LEDSTRm_ID[6];	
#endif
#if	_LEDSTRm_IO_TOTAL_NUM > 7
	LEDSTRm_ID[7] = GPIO_GetPin(_LEDSTRm_OUTPUT7);
	LEDSTRm_ID_const[7] = LEDSTRm_ID[7];
	LEDSTRm_ID_temp[7] = LEDSTRm_ID[7];	
#endif
#if	_LEDSTRm_IO_TOTAL_NUM > 8
	LEDSTRm_ID[8] = GPIO_GetPin(_LEDSTRm_OUTPUT8);
	LEDSTRm_ID_const[8] = LEDSTRm_ID[8];
	LEDSTRm_ID_temp[8] = LEDSTRm_ID[8];	
#endif
#if	_LEDSTRm_IO_TOTAL_NUM > 9
	LEDSTRm_ID[9] = GPIO_GetPin(_LEDSTRm_OUTPUT9);
	LEDSTRm_ID_const[9] = LEDSTRm_ID[9];
	LEDSTRm_ID_temp[9] = LEDSTRm_ID[9];	
#endif
#if	_LEDSTRm_IO_TOTAL_NUM > 10
	LEDSTRm_ID[10] = GPIO_GetPin(_LEDSTRm_OUTPUT10);
	LEDSTRm_ID_const[10] = LEDSTRm_ID[10];
	LEDSTRm_ID_temp[10] = LEDSTRm_ID[10];	
#endif
#if	_LEDSTRm_IO_TOTAL_NUM > 11
	LEDSTRm_ID[11] = GPIO_GetPin(_LEDSTRm_OUTPUT11);
	LEDSTRm_ID_const[11] = LEDSTRm_ID[11];
	LEDSTRm_ID_temp[11] = LEDSTRm_ID[11];	
#endif
#if	_LEDSTRm_IO_TOTAL_NUM > 12
	LEDSTRm_ID[12] = GPIO_GetPin(_LEDSTRm_OUTPUT12);
	LEDSTRm_ID_const[12] = LEDSTRm_ID[12];
	LEDSTRm_ID_temp[12] = LEDSTRm_ID[12];	
#endif
#if	_LEDSTRm_IO_TOTAL_NUM > 13
	LEDSTRm_ID[13] = GPIO_GetPin(_LEDSTRm_OUTPUT13);
	LEDSTRm_ID_const[13] = LEDSTRm_ID[13];
	LEDSTRm_ID_temp[13] = LEDSTRm_ID[13];	
#endif
#if	_LEDSTRm_IO_TOTAL_NUM > 14
	LEDSTRm_ID[14] = GPIO_GetPin(_LEDSTRm_OUTPUT14);
	LEDSTRm_ID_const[14] = LEDSTRm_ID[14];
	LEDSTRm_ID_temp[14] = LEDSTRm_ID[14];	
#endif
#if	_LEDSTRm_IO_TOTAL_NUM > 15
	LEDSTRm_ID[15] = GPIO_GetPin(_LEDSTRm_OUTPUT15);
	LEDSTRm_ID_const[15] = LEDSTRm_ID[15];
	LEDSTRm_ID_temp[15] = LEDSTRm_ID[15];	
#endif


#if _LEDSTRm_TIMER==USE_TIMER0
	SMU_PeriphClkCmd(SMU_PERIPH_TMR,ENABLE);
#if _EF_SERIES
	TIMER_Init(LEDSTRm_TIMER,C_Timer_Setting_16K);
#else
	TIMER_Init(LEDSTRm_TIMER,TIMER_CLKDIV_1,C_Timer_Setting_16K);
#endif
	TIMER_IntCmd(LEDSTRm_TIMER,ENABLE);
	INTC_IrqCmd(IRQ_BIT_TMR0,ENABLE);
	INTC_SetPriority(IRQ_VALVE_TMR0,IRQ_PRIORITY_0);
#elif _LEDSTRm_TIMER==USE_TIMER1
	SMU_PeriphClkCmd(SMU_PERIPH_TMR,ENABLE);
#if _EF_SERIES
	TIMER_Init(LEDSTRm_TIMER,C_Timer_Setting_16K);
#else
	TIMER_Init(LEDSTRm_TIMER,TIMER_CLKDIV_1,C_Timer_Setting_16K);
#endif
	TIMER_IntCmd(LEDSTRm_TIMER,ENABLE);
	INTC_IrqCmd(IRQ_BIT_TMR1,ENABLE);
	INTC_SetPriority(IRQ_VALVE_TMR1,IRQ_PRIORITY_0);
#elif _LEDSTRm_TIMER==USE_TIMER2
	SMU_PeriphClkCmd(SMU_PERIPH_TMR,ENABLE);
#if _EF_SERIES
	TIMER_Init(LEDSTRm_TIMER,C_Timer_Setting_16K);
#else
	TIMER_Init(LEDSTRm_TIMER,TIMER_CLKDIV_1,C_Timer_Setting_16K);
#endif
	TIMER_IntCmd(LEDSTRm_TIMER,ENABLE);
	INTC_IrqCmd(IRQ_BIT_TMR2,ENABLE);
	INTC_SetPriority(IRQ_VALVE_TMR2,IRQ_PRIORITY_0);
#elif _LEDSTRm_TIMER==USE_TIMER_PWMA
	SMU_PeriphClkCmd(SMU_PERIPH_PWM,ENABLE);
#if _EF_SERIES
	TIMER_Init(LEDSTRm_TIMER,C_Timer_Setting_16K);
#else
	TIMER_Init(LEDSTRm_TIMER,TIMER_CLKDIV_1,C_Timer_Setting_16K);
#endif
	TIMER_IntCmd(LEDSTRm_TIMER,ENABLE);
	INTC_IrqCmd(IRQ_BIT_PWMA,ENABLE);
	INTC_SetPriority(IRQ_VALVE_PWMA,IRQ_PRIORITY_0);
#elif _LEDSTRm_TIMER==USE_TIMER_PWMB
    SMU_PeriphClkCmd(SMU_PERIPH_PWM,ENABLE);
#if _EF_SERIES
	TIMER_Init(LEDSTRm_TIMER,C_Timer_Setting_16K);
#else
	TIMER_Init(LEDSTRm_TIMER,TIMER_CLKDIV_1,C_Timer_Setting_16K);
#endif
	TIMER_IntCmd(LEDSTRm_TIMER,ENABLE);
	INTC_IrqCmd(IRQ_BIT_PWMB,ENABLE);
	INTC_SetPriority(IRQ_VALVE_PWMB,IRQ_PRIORITY_0);
#elif _LEDSTRm_TIMER==USE_TIMER3
#if _EF_SERIES
	SMU_PeriphClkCmd(SMU_PERIPH_TMR,ENABLE);
	TIMER_Init(LEDSTRm_TIMER,C_Timer_Setting_16K);
	TIMER_IntCmd(LEDSTRm_TIMER,ENABLE);
	INTC_IrqCmd(IRQ_BIT_TMR3,ENABLE);
	INTC_SetPriority(IRQ_VALVE_TMR3,IRQ_PRIORITY_0);
#else
	#error	"LEDSTRm does not support Timer 3"
#endif
#endif
}

void CB_LEDSTRm_Start(void)
{
#if _LEDSTR_MODULE
	LEDSTR_Stop(-1);
#endif	
#if _LEDSTRn_MODULE
	LEDSTRn_Stop();
#endif	
	U8 k;
	for(k=0 ;k<_LEDSTRm_IO_TOTAL_NUM; k++)
	{
		IO_Mask |= (1 << LEDSTRm_ID[k]);					
	}
	IO_Mask_not = ~IO_Mask;
	GPIOA->Data &= IO_Mask_not;
	GPIOA->DIR &= IO_Mask_not;
}

void CB_LEDSTRm_GetData(U8* _buf, U32 _addr, U32 _size, U8 _mode)
{
    switch(_mode) {
#if _EF_SERIES
        case EF_UD_MODE:
        case EF_MODE:
            memcpy((void*)_buf,(void*)_addr,_size);
            break;
#endif
#if	_SPI_MODULE
        //case SPI_UD_MODE:
        case SPI_MODE:
            SPI_BurstRead(_buf,_addr,_size);
            break;
#endif
#if (_SPI1_MODULE && _SPI1_USE_FLASH)
        //case SPI1_UD_MODE:
        case SPI1_MODE:
            SPI1_BurstRead(_buf,_addr,_size);
            break;
#endif
        default:
            //memset((void*)_buf,(void*)0,_size);
            break;//error
    }
}

void CB_LEDSTRm_Timer_Cmd(U8 cmd)
{
	TIMER_Cmd(LEDSTRm_TIMER,cmd);
}

void CB_RGB_Pattern3(void *code)
{
	RGB_Pattern3(LEDSTRm_port,code);
}

void CB_RGB_Pattern4(void *code)
{
	RGB_Pattern4(LEDSTRm_port,code);
}

U32 CB_LEDSTRm_CheckTypeAddress(U32 index, U8 mode)
{
	if(index&ADDRESS_MODE) {
		return (index&~ADDRESS_MODE);
	}
	switch(mode) 
 	{
 //		case EF_MODE:
 		case OTP_MODE:	// same with EF_MODE
 				return 	(index>=LED_FILE_NUMBER)?0:LEDStripList[index];
 				
		case EF_UD_MODE:		
 		case SPI_MODE:
 		case SPI1_MODE:
 				return	UserDefinedData_GetAddressUseType(index, kUD_LED_STRING, mode);

 		default:
 				return 0;//error
 	}
}

#pragma GCC optimize ("O0")
#if _EF_SERIES

//====================================================================================
//  32MHz wait one state code pattern 
//====================================================================================
#if OPTION_HIGH_FREQ_CLK==32000000
/*
void RGB_Pattern(GPIO_TypeDef *gpiox,U16 *code)
{
		__asm__(".align 4\t\n"
				"addi $r8,$r1,#0x18\t\n"
				"movi $r5,IO_Mask_not\t\n"
				"lwi333 $r6,[$r0+#0x4]\t\n"
				"nop16\t\n"
				"Start:\t\n"
				"ori $r6,$r6,IO_Mask_a\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
				"nop16\t\n"
#if	_LEDSTRm_CODE_0>=1		     
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=3		
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=4		
			  	"nop\t\n"
#endif				 				
				"lbi.bi $r7,[$r1],#0x1\t\n"
				"ori $r7,$r7,QFan_LED_Mask\t\n"
				"and33 $r6,$r7\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRm_CODE_0<=1		     
			  	"nop\t\n"
			  	"nop\t\n"		  	
#endif				 
#if _LEDSTRm_CODE_0<=2		 
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0<=3		
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0<=4		

#endif			
	 						
#if	_LEDSTRm_CODE_1>=1		     
	  	
#endif				 
#if _LEDSTRm_CODE_1>=2		 
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1>=3		
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1>=4		
			  	"nop\t\n"
				"nop\t\n"			  	
#endif	
				"and33 $r6,$r5\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRm_CODE_1<=1		     
			  	"nop\t\n"	
#endif				 
#if _LEDSTRm_CODE_1<=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1<=3		
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1<=4		
				"nop\t\n"
				"nop\t\n"	
#endif					
				"bne $r1,$r8,Start\t\n"
			  :
		      :"r"(gpiox),"r"(code) 
		      :"$r5", "$r6", "$r7", "$r8", "$r9"
		     );

}*/
//====================================================================================
//  40MHz wait one state code pattern 
//====================================================================================
#elif	OPTION_HIGH_FREQ_CLK==40000000

void RGB_Pattern3(GPIO_TypeDef *gpiox,U8 *code)
{
		__asm__(".align 4\t\n"
				"mov55 $r8,$r1\t\n"
				"addi $r1,$r1,#0x17\t\n"
				"addi $r8,$r8,#-1\t\n"
				"lwi333 $r6,[$r0+#0x4]\t\n"
				"Start1:\t\n"
				"or33 $r6,%[IO_Mask]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRm_CODE_0>=1		     
			  	"nop\t\n"
			  	"nop\t\n"
				"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=3		
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=4		
			  	"nop\t\n"	
#endif				 		
				"lbi.bi $r7,[$r1],#-1\t\n"
				"sll $r7,$r7,%[IO_shift]\t\n"

				"or33 $r7,%[IO_Mask_not]\t\n"
				"and33 $r6,$r7\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRm_CODE_0<=1		     
			  	"nop\t\n"
			  	"nop\t\n"		  	
#endif				 
#if _LEDSTRm_CODE_0<=2		 
			  	"nop\t\n"
				"nop\t\n"	
#endif				 
#if _LEDSTRm_CODE_0<=3		
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0<=4		

#endif			
	 						
#if	_LEDSTRm_CODE_1>=1		     
	  	
#endif				 
#if _LEDSTRm_CODE_1>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1>=3		
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1>=4		
			  	"nop\t\n"
				"nop\t\n"			  			
#endif	
				"and33 $r6,%[IO_Mask_not]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRm_CODE_1<=1		     
			  	"nop\t\n"	
#endif				 
#if _LEDSTRm_CODE_1<=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1<=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1<=4		
			  	"nop\t\n"
				"nop\t\n"
				"nop\t\n"
#endif					
				"bne $r1,$r8,Start1\t\n"
			  :
		      :"r"(gpiox), "r"(code), [IO_Mask] "r" (IO_Mask) ,[IO_Mask_not] "r" (IO_Mask_not) ,[IO_shift] "r" (IO_shift)
		      :"$r6", "$r7", "$r8", "$r9"
		     );
}

void RGB_Pattern4(GPIO_TypeDef *gpiox,U16 *code)
{
		__asm__(".align 4\t\n"
				"mov55 $r8,$r1\t\n"
				"addi $r1,$r1,#0x2E\t\n"
				"addi $r8,$r8,#-2\t\n"
				"lwi333 $r6,[$r0+#0x4]\t\n"
				"Start2:\t\n"
				"or33 $r6,%[IO_Mask]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRm_CODE_0>=1		     
			  	"nop\t\n"
			  	"nop\t\n"
				"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=3		
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=4		
			  	"nop\t\n"	
#endif				 		
				"nop\t\n"
				"lhi.bi $r7,[$r1],#-2\t\n"				
				"or33 $r7,%[IO_Mask_not]\t\n"
				"and33 $r6,$r7\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRm_CODE_0<=1		     
			  	"nop\t\n"
			  	"nop\t\n"		  	
#endif				 
#if _LEDSTRm_CODE_0<=2		 
			  	"nop\t\n"
				"nop\t\n"	
#endif				 
#if _LEDSTRm_CODE_0<=3		
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0<=4		

#endif			
	 						
#if	_LEDSTRm_CODE_1>=1		     
	  	
#endif				 
#if _LEDSTRm_CODE_1>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1>=3		
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1>=4		
			  	"nop\t\n"
				"nop\t\n"			  			
#endif	
				"and33 $r6,%[IO_Mask_not]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRm_CODE_1<=1		     
			  	"nop\t\n"	
#endif				 
#if _LEDSTRm_CODE_1<=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1<=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1<=4		
			  	"nop\t\n"
				"nop\t\n"
				"nop\t\n"
#endif					
				"bne $r1,$r8,Start2\t\n"
			  :
		      :"r"(gpiox), "r"(code), [IO_Mask] "r" (IO_Mask) ,[IO_Mask_not] "r" (IO_Mask_not) ,[IO_shift] "r" (IO_shift)
		      :"$r6", "$r7", "$r8", "$r9"
		     );
}

//====================================================================================
//  48MHz wait one state code pattern 
//====================================================================================
#elif	OPTION_HIGH_FREQ_CLK==48000000
void RGB_Pattern3(GPIO_TypeDef *gpiox,U8 *code)
{
		__asm__(".align 4\t\n"
				"mov55 $r8,$r1\t\n"
				"addi $r1,$r1,#0x17\t\n"
				"addi $r8,$r8,#-1\t\n"
				"lwi333 $r6,[$r0+#0x4]\t\n"
				"Start1:\t\n"
				"or33 $r6,%[IO_Mask]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRm_CODE_0>=1		     
			  	"nop\t\n"
			  	"nop\t\n"
				"nop\t\n"
				"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=4		
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"	
#endif		
				"lbi.bi $r7,[$r1],#-1\t\n"
				"sll $r7,$r7,%[IO_shift]\t\n"

				"or33 $r7,%[IO_Mask_not]\t\n"
				"and33 $r6,$r7\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRm_CODE_0<=1		     
			  	"nop\t\n"
			  	"nop\t\n"			  	
#endif				 
#if _LEDSTRm_CODE_0<=2		 
			  	"nop\t\n"
				"nop\t\n"	
#endif				 
#if _LEDSTRm_CODE_0<=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0<=4		

#endif			
	 						
#if	_LEDSTRm_CODE_1>=1		     
	  	
#endif				 
#if _LEDSTRm_CODE_1>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1>=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1>=4		
			  	"nop\t\n"
				"nop\t\n"			  				
#endif	
				"and33 $r6,%[IO_Mask_not]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRm_CODE_1<=1		     
			  	"nop\t\n"
			  	"nop\t\n"		  		
#endif				 
#if _LEDSTRm_CODE_1<=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1<=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1<=4		
			  	"nop\t\n"
				"nop\t\n"
				"nop\t\n"
				"nop\t\n"
#endif					
				"bne $r1,$r8,Start1\t\n"
			  :
		      :"r"(gpiox), "r"(code), [IO_Mask] "r" (IO_Mask) ,[IO_Mask_not] "r" (IO_Mask_not) ,[IO_shift] "r" (IO_shift)
		      :"$r6", "$r7", "$r8", "$r9"
		     );
}


void RGB_Pattern4(GPIO_TypeDef *gpiox,U16 *code)
{
		__asm__(".align 4\t\n"
				"mov55 $r8,$r1\t\n"
				"addi $r1,$r1,#0x2E\t\n"
				"addi $r8,$r8,#-2\t\n"
				"lwi333 $r6,[$r0+#0x4]\t\n"
				"Start2:\t\n"
				"or33 $r6,%[IO_Mask]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRm_CODE_0>=1		     
			  	"nop\t\n"
			  	"nop\t\n"
				"nop\t\n"
				"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=4		
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"	
#endif		
				"nop\t\n"
				"lhi.bi $r7,[$r1],#-2\t\n"				
				"or33 $r7,%[IO_Mask_not]\t\n"
				"and33 $r6,$r7\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRm_CODE_0<=1		     
			  	"nop\t\n"
			  	"nop\t\n"			  	
#endif				 
#if _LEDSTRm_CODE_0<=2		 
			  	"nop\t\n"
				"nop\t\n"	
#endif				 
#if _LEDSTRm_CODE_0<=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0<=4		

#endif			
	 						
#if	_LEDSTRm_CODE_1>=1		     
	  	
#endif				 
#if _LEDSTRm_CODE_1>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1>=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1>=4		
			  	"nop\t\n"
				"nop\t\n"			  				
#endif	
				"and33 $r6,%[IO_Mask_not]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRm_CODE_1<=1		     
			  	"nop\t\n"
			  	"nop\t\n"		  		
#endif				 
#if _LEDSTRm_CODE_1<=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1<=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1<=4		
			  	"nop\t\n"
				"nop\t\n"
				"nop\t\n"
				"nop\t\n"
#endif					
				"bne $r1,$r8,Start2\t\n"
			  :
		      :"r"(gpiox), "r"(code), [IO_Mask] "r" (IO_Mask) ,[IO_Mask_not] "r" (IO_Mask_not) ,[IO_shift] "r" (IO_shift)
		      :"$r6", "$r7", "$r8", "$r9"
		     );
}
#endif

#else
//====================================================================================
//  OTP 32MHz code pattern 
//====================================================================================
#if OPTION_HIGH_FREQ_CLK==32000000

void RGB_Pattern3(GPIO_TypeDef *gpiox,U8 *code)
{
		__asm__(".align 4\t\n"
				"mov55 $r8,$r1\t\n"
				"addi $r1,$r1,#0x17\t\n"
				"addi $r8,$r8,#-1\t\n"
				"lwi333 $r6,[$r0+#0x4]\t\n"
				"Start1:\t\n"
				"or33 $r6,%[IO_Mask]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRm_CODE_0>=1		     
			  	"nop\t\n"
			  	"nop\t\n"
				"nop\t\n"
				"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=3		
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=4		
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"	
#endif				 
				"lbi.bi $r7,[$r1],#-1\t\n"
				"sll $r7,$r7,%[IO_shift]\t\n"

				"or33 $r7,%[IO_Mask_not]\t\n"
				"and33 $r6,$r7\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRm_CODE_0<=1		     
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"			  	
#endif				 
#if _LEDSTRm_CODE_0<=2		 
			  	"nop\t\n"
			  	"nop\t\n"
				"nop\t\n"	
#endif				 
#if _LEDSTRm_CODE_0<=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0<=4		

#endif			
	 						
#if	_LEDSTRm_CODE_1>=1		     
	  	
#endif				 
#if _LEDSTRm_CODE_1>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1>=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1>=4		
			  	"nop\t\n"
				"nop\t\n"			  	
			  	"nop\t\n"
			  	"nop\t\n"			
#endif	
				"and33 $r6,%[IO_Mask_not]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRm_CODE_1<=1		     
			  	"nop\t\n"
			  	"nop\t\n"		  	
			  	"nop\t\n"		
#endif				 
#if _LEDSTRm_CODE_1<=2		 
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1<=3		
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1<=4		
			  	"nop\t\n"
				"nop\t\n"
				"nop\t\n"
				"nop\t\n"	
				"nop\t\n"
#endif					
				"bne $r1,$r8,Start1\t\n"
			  :
		      :"r"(gpiox), "r"(code), [IO_Mask] "r" (IO_Mask) ,[IO_Mask_not] "r" (IO_Mask_not) ,[IO_shift] "r" (IO_shift)
		      :"$r6", "$r7", "$r8", "$r9"
		     );
}
void RGB_Pattern4(GPIO_TypeDef *gpiox,U16 *code)
{
		__asm__(".align 4\t\n"
				"mov55 $r8,$r1\t\n"
				"addi $r1,$r1,#0x2E\t\n"
				"addi $r8,$r8,#-2\t\n"
				"lwi333 $r6,[$r0+#0x4]\t\n"
				"Start2:\t\n"
				"or33 $r6,%[IO_Mask]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRm_CODE_0>=1		     
			  	"nop\t\n"
			  	"nop\t\n"
				"nop\t\n"
				"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=3		
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=4		
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"	
#endif				 
				"nop\t\n"
				"lhi.bi $r7,[$r1],#-2\t\n"				
				"or33 $r7,%[IO_Mask_not]\t\n"
				"and33 $r6,$r7\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRm_CODE_0<=1		     
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"			  	
#endif				 
#if _LEDSTRm_CODE_0<=2		 
			  	"nop\t\n"
			  	"nop\t\n"
				"nop\t\n"	
#endif				 
#if _LEDSTRm_CODE_0<=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0<=4		

#endif			
	 						
#if	_LEDSTRm_CODE_1>=1		     
	  	
#endif				 
#if _LEDSTRm_CODE_1>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1>=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1>=4		
			  	"nop\t\n"
				"nop\t\n"			  	
			  	"nop\t\n"
			  	"nop\t\n"			
#endif	
				"and33 $r6,%[IO_Mask_not]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRm_CODE_1<=1		     
			  	"nop\t\n"
			  	"nop\t\n"		  	
			  	"nop\t\n"		
#endif				 
#if _LEDSTRm_CODE_1<=2		 
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1<=3		
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1<=4		
			  	"nop\t\n"
				"nop\t\n"
				"nop\t\n"
				"nop\t\n"	
				"nop\t\n"
#endif					
				"bne $r1,$r8,Start2\t\n"
			  :
		      :"r"(gpiox), "r"(code), [IO_Mask] "r" (IO_Mask) ,[IO_Mask_not] "r" (IO_Mask_not) ,[IO_shift] "r" (IO_shift)
		      :"$r6", "$r7", "$r8", "$r9"
		     );
}
//====================================================================================
//  OTP 24MHz code pattern 
//====================================================================================
#elif	OPTION_HIGH_FREQ_CLK==24000000
void RGB_Pattern3(GPIO_TypeDef *gpiox,U8 *code)
{
		__asm__(".align 4\t\n"
				"mov55 $r8,$r1\t\n"
				"addi $r1,$r1,#0x17\t\n"
				"addi $r8,$r8,#-1\t\n"
				"lwi333 $r6,[$r0+#0x4]\t\n"
				"Start1:\t\n"
				"or33 $r6,%[IO_Mask]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRm_CODE_0>=1		     
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=4		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 				
				"lbi.bi $r7,[$r1],#-1\t\n"
				"sll $r7,$r7,%[IO_shift]\t\n"

				"or33 $r7,%[IO_Mask_not]\t\n"
				"and33 $r6,$r7\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRm_CODE_0<=1		     
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"				  	
#endif				 
#if _LEDSTRm_CODE_0<=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0<=3		
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0<=4		

#endif			
	 						
#if	_LEDSTRm_CODE_1>=1		     
	  	
#endif				 
#if _LEDSTRm_CODE_1>=2		 
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1>=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1>=4		
				"nop\t\n"			  	
			  	"nop\t\n"
			  	"nop\t\n"			
#endif	
				"and33 $r6,%[IO_Mask_not]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRm_CODE_1<=1		     
			  	"nop\t\n"
			  	"nop\t\n"		  		
#endif				 
#if _LEDSTRm_CODE_1<=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1<=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1<=4		
				"nop\t\n"
				"nop\t\n"	
				"nop\t\n"
#endif					
				"bne $r1,$r8,Start1\t\n"
			  :
		      :"r"(gpiox), "r"(code), [IO_Mask] "r" (IO_Mask) ,[IO_Mask_not] "r" (IO_Mask_not) ,[IO_shift] "r" (IO_shift)
		      :"$r6", "$r7", "$r8", "$r9"
		     );
}
void RGB_Pattern4(GPIO_TypeDef *gpiox,U16 *code)
{
		__asm__(".align 4\t\n"
				"mov55 $r8,$r1\t\n"
				"addi $r1,$r1,#0x2E\t\n"
				"addi $r8,$r8,#-2\t\n"
				"lwi333 $r6,[$r0+#0x4]\t\n"
				"Start2:\t\n"
				"or33 $r6,%[IO_Mask]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRm_CODE_0>=1		     
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0>=4		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 				
				"nop\t\n"
				"lhi.bi $r7,[$r1],#-2\t\n"				
				"or33 $r7,%[IO_Mask_not]\t\n"
				"and33 $r6,$r7\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRm_CODE_0<=1		     
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"				  	
#endif				 
#if _LEDSTRm_CODE_0<=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0<=3		
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_0<=4		

#endif			
	 						
#if	_LEDSTRm_CODE_1>=1		     
	  	
#endif				 
#if _LEDSTRm_CODE_1>=2		 
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1>=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1>=4		
				"nop\t\n"			  	
			  	"nop\t\n"
			  	"nop\t\n"			
#endif	
				"and33 $r6,%[IO_Mask_not]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRm_CODE_1<=1		     
			  	"nop\t\n"
			  	"nop\t\n"		  		
#endif				 
#if _LEDSTRm_CODE_1<=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1<=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRm_CODE_1<=4		
				"nop\t\n"
				"nop\t\n"	
				"nop\t\n"
#endif					
				"bne $r1,$r8,Start2\t\n"
			  :
		      :"r"(gpiox), "r"(code), [IO_Mask] "r" (IO_Mask) ,[IO_Mask_not] "r" (IO_Mask_not) ,[IO_shift] "r" (IO_shift)
		      :"$r6", "$r7", "$r8", "$r9"
		     );
}
#else
#error	"LEDSTRm only support high clock frequency 24MHz/32MHz!"
#endif

#endif


#endif //#if _LEDSTRm_MODULE 