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

#if _LEDSTRn_MODULE 

#if _LEDSTRn_IO_TOTAL_NUM==0
	#error "LEDSTRn_IO_TOTAL_NUM can not be 0."
#endif

#if _LEDSTRn_IO_TOTAL_NUM>16
	#error "LEDSTRn_IO_TOTAL_NUM over 16."
#endif

#if		((_LEDSTRn_IO_TOTAL_NUM>0)&&_LEDSTRn_OUTPUT0>GPIO_MAX)	\
		||((_LEDSTRn_IO_TOTAL_NUM>1)&&_LEDSTRn_OUTPUT1>GPIO_MAX)	\
		||((_LEDSTRn_IO_TOTAL_NUM>2)&&_LEDSTRn_OUTPUT2>GPIO_MAX)	\
		||((_LEDSTRn_IO_TOTAL_NUM>3)&&_LEDSTRn_OUTPUT3>GPIO_MAX)	\
		||((_LEDSTRn_IO_TOTAL_NUM>4)&&_LEDSTRn_OUTPUT4>GPIO_MAX)	\
		||((_LEDSTRn_IO_TOTAL_NUM>5)&&_LEDSTRn_OUTPUT5>GPIO_MAX)	\
		||((_LEDSTRn_IO_TOTAL_NUM>6)&&_LEDSTRn_OUTPUT6>GPIO_MAX)	\
		||((_LEDSTRn_IO_TOTAL_NUM>7)&&_LEDSTRn_OUTPUT7>GPIO_MAX)	\
		||((_LEDSTRn_IO_TOTAL_NUM>8)&&_LEDSTRn_OUTPUT8>GPIO_MAX)	\
		||((_LEDSTRn_IO_TOTAL_NUM>9)&&_LEDSTRn_OUTPUT9>GPIO_MAX)	\
		||((_LEDSTRn_IO_TOTAL_NUM>10)&&_LEDSTRn_OUTPUT10>GPIO_MAX)	\
		||((_LEDSTRn_IO_TOTAL_NUM>11)&&_LEDSTRn_OUTPUT11>GPIO_MAX)	\
		||((_LEDSTRn_IO_TOTAL_NUM>12)&&_LEDSTRn_OUTPUT12>GPIO_MAX)	\
		||((_LEDSTRn_IO_TOTAL_NUM>13)&&_LEDSTRn_OUTPUT13>GPIO_MAX)	\
		||((_LEDSTRn_IO_TOTAL_NUM>14)&&_LEDSTRn_OUTPUT14>GPIO_MAX)	\
		||((_LEDSTRn_IO_TOTAL_NUM>15)&&_LEDSTRn_OUTPUT15>GPIO_MAX)
		#error "LEDSTRn OUTPUT Pin undefine"
#endif

#define _LEDSTRn_CHK_SAMEPORT	(	((_LEDSTRn_IO_TOTAL_NUM>0)?(0x1<<GPIO_GetPortIdx(_LEDSTRn_OUTPUT0)):0)	\
								|	((_LEDSTRn_IO_TOTAL_NUM>1)?(0x1<<GPIO_GetPortIdx(_LEDSTRn_OUTPUT1)):0)	\
								|	((_LEDSTRn_IO_TOTAL_NUM>2)?(0x1<<GPIO_GetPortIdx(_LEDSTRn_OUTPUT2)):0)	\
								|	((_LEDSTRn_IO_TOTAL_NUM>3)?(0x1<<GPIO_GetPortIdx(_LEDSTRn_OUTPUT3)):0)	\
								|	((_LEDSTRn_IO_TOTAL_NUM>4)?(0x1<<GPIO_GetPortIdx(_LEDSTRn_OUTPUT4)):0)	\
								|	((_LEDSTRn_IO_TOTAL_NUM>5)?(0x1<<GPIO_GetPortIdx(_LEDSTRn_OUTPUT5)):0)	\
								|	((_LEDSTRn_IO_TOTAL_NUM>6)?(0x1<<GPIO_GetPortIdx(_LEDSTRn_OUTPUT6)):0)	\
								|	((_LEDSTRn_IO_TOTAL_NUM>7)?(0x1<<GPIO_GetPortIdx(_LEDSTRn_OUTPUT7)):0)	\
								|	((_LEDSTRn_IO_TOTAL_NUM>8)?(0x1<<GPIO_GetPortIdx(_LEDSTRn_OUTPUT8)):0)	\
								|	((_LEDSTRn_IO_TOTAL_NUM>9)?(0x1<<GPIO_GetPortIdx(_LEDSTRn_OUTPUT9)):0)	\
								|	((_LEDSTRn_IO_TOTAL_NUM>10)?(0x1<<GPIO_GetPortIdx(_LEDSTRn_OUTPUT10)):0)	\
								|	((_LEDSTRn_IO_TOTAL_NUM>11)?(0x1<<GPIO_GetPortIdx(_LEDSTRn_OUTPUT11)):0)	\
								|	((_LEDSTRn_IO_TOTAL_NUM>12)?(0x1<<GPIO_GetPortIdx(_LEDSTRn_OUTPUT12)):0)	\
								|	((_LEDSTRn_IO_TOTAL_NUM>13)?(0x1<<GPIO_GetPortIdx(_LEDSTRn_OUTPUT13)):0)	\
								|	((_LEDSTRn_IO_TOTAL_NUM>14)?(0x1<<GPIO_GetPortIdx(_LEDSTRn_OUTPUT14)):0)	\
								|	((_LEDSTRn_IO_TOTAL_NUM>15)?(0x1<<GPIO_GetPortIdx(_LEDSTRn_OUTPUT15)):0)	)
#if	((_LEDSTRn_CHK_SAMEPORT)&(_LEDSTRn_CHK_SAMEPORT-1))>0
	#error "LEDSTRn OUTPUT Port different"
#endif


#define _LEDSTRn_CHK_SAMEPIN			(	((_LEDSTRn_IO_TOTAL_NUM>0)?(1<<GPIO_GetPin(_LEDSTRn_OUTPUT0)):0)	\
										|	((_LEDSTRn_IO_TOTAL_NUM>1)?(1<<GPIO_GetPin(_LEDSTRn_OUTPUT1)):0)	\
										|	((_LEDSTRn_IO_TOTAL_NUM>2)?(1<<GPIO_GetPin(_LEDSTRn_OUTPUT2)):0)	\
										|	((_LEDSTRn_IO_TOTAL_NUM>3)?(1<<GPIO_GetPin(_LEDSTRn_OUTPUT3)):0)	\
										|	((_LEDSTRn_IO_TOTAL_NUM>4)?(1<<GPIO_GetPin(_LEDSTRn_OUTPUT4)):0)	\
										|	((_LEDSTRn_IO_TOTAL_NUM>5)?(1<<GPIO_GetPin(_LEDSTRn_OUTPUT5)):0)	\
										|	((_LEDSTRn_IO_TOTAL_NUM>6)?(1<<GPIO_GetPin(_LEDSTRn_OUTPUT6)):0)	\
										|	((_LEDSTRn_IO_TOTAL_NUM>7)?(1<<GPIO_GetPin(_LEDSTRn_OUTPUT7)):0)	\
										|	((_LEDSTRn_IO_TOTAL_NUM>8)?(1<<GPIO_GetPin(_LEDSTRn_OUTPUT8)):0)	\
										|	((_LEDSTRn_IO_TOTAL_NUM>9)?(1<<GPIO_GetPin(_LEDSTRn_OUTPUT9)):0)	\
										|	((_LEDSTRn_IO_TOTAL_NUM>10)?(1<<GPIO_GetPin(_LEDSTRn_OUTPUT10)):0)	\
										|	((_LEDSTRn_IO_TOTAL_NUM>11)?(1<<GPIO_GetPin(_LEDSTRn_OUTPUT11)):0)	\
										|	((_LEDSTRn_IO_TOTAL_NUM>12)?(1<<GPIO_GetPin(_LEDSTRn_OUTPUT12)):0)	\
										|	((_LEDSTRn_IO_TOTAL_NUM>13)?(1<<GPIO_GetPin(_LEDSTRn_OUTPUT13)):0)	\
										|	((_LEDSTRn_IO_TOTAL_NUM>14)?(1<<GPIO_GetPin(_LEDSTRn_OUTPUT14)):0)	\
										|	((_LEDSTRn_IO_TOTAL_NUM>15)?(1<<GPIO_GetPin(_LEDSTRn_OUTPUT15)):0)	)						

#if (	 \
			 ((_LEDSTRn_CHK_SAMEPIN&(0x1<<0))>0)	\
			+((_LEDSTRn_CHK_SAMEPIN&(0x1<<1))>0)	\
			+((_LEDSTRn_CHK_SAMEPIN&(0x1<<2))>0)	\
			+((_LEDSTRn_CHK_SAMEPIN&(0x1<<3))>0)	\
			+((_LEDSTRn_CHK_SAMEPIN&(0x1<<4))>0)	\
			+((_LEDSTRn_CHK_SAMEPIN&(0x1<<5))>0)	\
			+((_LEDSTRn_CHK_SAMEPIN&(0x1<<6))>0)	\
			+((_LEDSTRn_CHK_SAMEPIN&(0x1<<7))>0)	\
			+((_LEDSTRn_CHK_SAMEPIN&(0x1<<8))>0)	\
			+((_LEDSTRn_CHK_SAMEPIN&(0x1<<9))>0)	\
			+((_LEDSTRn_CHK_SAMEPIN&(0x1<<10))>0)	\
			+((_LEDSTRn_CHK_SAMEPIN&(0x1<<11))>0)	\
			+((_LEDSTRn_CHK_SAMEPIN&(0x1<<12))>0)	\
			+((_LEDSTRn_CHK_SAMEPIN&(0x1<<13))>0)	\
			+((_LEDSTRn_CHK_SAMEPIN&(0x1<<14))>0)	\
			+((_LEDSTRn_CHK_SAMEPIN&(0x1<<15))>0)	\
		)!=_LEDSTRn_IO_TOTAL_NUM
		#error "LEDSTRn OUTPUT Pin repeat "
#endif
#if		  ((_LEDSTRn_IO_TOTAL_NUM>1)&&(((GPIO_GetPin(_LEDSTRn_OUTPUT1))-(GPIO_GetPin(_LEDSTRn_OUTPUT0)))!=1))	\
		||((_LEDSTRn_IO_TOTAL_NUM>2)&&(((GPIO_GetPin(_LEDSTRn_OUTPUT2))-(GPIO_GetPin(_LEDSTRn_OUTPUT1)))!=1))	\
		||((_LEDSTRn_IO_TOTAL_NUM>3)&&(((GPIO_GetPin(_LEDSTRn_OUTPUT3))-(GPIO_GetPin(_LEDSTRn_OUTPUT2)))!=1))	\
		||((_LEDSTRn_IO_TOTAL_NUM>4)&&(((GPIO_GetPin(_LEDSTRn_OUTPUT4))-(GPIO_GetPin(_LEDSTRn_OUTPUT3)))!=1))	\
		||((_LEDSTRn_IO_TOTAL_NUM>5)&&(((GPIO_GetPin(_LEDSTRn_OUTPUT5))-(GPIO_GetPin(_LEDSTRn_OUTPUT4)))!=1))	\
		||((_LEDSTRn_IO_TOTAL_NUM>6)&&(((GPIO_GetPin(_LEDSTRn_OUTPUT6))-(GPIO_GetPin(_LEDSTRn_OUTPUT5)))!=1))	\
		||((_LEDSTRn_IO_TOTAL_NUM>7)&&(((GPIO_GetPin(_LEDSTRn_OUTPUT7))-(GPIO_GetPin(_LEDSTRn_OUTPUT6)))!=1))	\
		||((_LEDSTRn_IO_TOTAL_NUM>8)&&(((GPIO_GetPin(_LEDSTRn_OUTPUT8))-(GPIO_GetPin(_LEDSTRn_OUTPUT7)))!=1))	\
		||((_LEDSTRn_IO_TOTAL_NUM>9)&&(((GPIO_GetPin(_LEDSTRn_OUTPUT9))-(GPIO_GetPin(_LEDSTRn_OUTPUT8)))!=1))	\
		||((_LEDSTRn_IO_TOTAL_NUM>10)&&(((GPIO_GetPin(_LEDSTRn_OUTPUT10))-(GPIO_GetPin(_LEDSTRn_OUTPUT9)))!=1))	\
		||((_LEDSTRn_IO_TOTAL_NUM>11)&&(((GPIO_GetPin(_LEDSTRn_OUTPUT11))-(GPIO_GetPin(_LEDSTRn_OUTPUT10)))!=1))	\
		||((_LEDSTRn_IO_TOTAL_NUM>12)&&(((GPIO_GetPin(_LEDSTRn_OUTPUT12))-(GPIO_GetPin(_LEDSTRn_OUTPUT11)))!=1))	\
		||((_LEDSTRn_IO_TOTAL_NUM>13)&&(((GPIO_GetPin(_LEDSTRn_OUTPUT13))-(GPIO_GetPin(_LEDSTRn_OUTPUT12)))!=1))	\
		||((_LEDSTRn_IO_TOTAL_NUM>14)&&(((GPIO_GetPin(_LEDSTRn_OUTPUT14))-(GPIO_GetPin(_LEDSTRn_OUTPUT13)))!=1))	\
		||((_LEDSTRn_IO_TOTAL_NUM>15)&&(((GPIO_GetPin(_LEDSTRn_OUTPUT15))-(GPIO_GetPin(_LEDSTRn_OUTPUT14)))!=1))
		#error "LEDSTRn OUTPUT Pin discontinuousr "
#endif


// #if	(((_LEDSTRn_CHK_SAMEPIN & 0xff) ==0) || ((_LEDSTRn_CHK_SAMEPIN & 0xff00) ==0))
// 	#define _LEDSTRn_Data_Byte				1
// #else
// 	#define _LEDSTRn_Data_Byte				2
// #endif
#define _LEDSTRn_Data_Byte				2


#define _LEDSTRn_Total_Amount			((_LEDSTRn_TIMING * 10000) / 625)
#if _LEDSTRn_Data_Byte==1 && _LEDSTRn_IO_TOTAL_NUM>8
#error	"_LEDSTRn_IO_TOTAL_NUM over size"
#endif

const U16 LEDSTRn_parameter[] = {
	_LEDSTRn_Data_Byte,_LEDSTRn_IO_TOTAL_NUM,_LEDSTRn_MAX_NUM,_LEDSTRn_Total_Amount,
};
const U8 LEDSTRn_PinMap[] = {
#if	_LEDSTRn_IO_TOTAL_NUM > 0
	GPIO_GetPin(_LEDSTRn_OUTPUT0),
#endif
#if	_LEDSTRn_IO_TOTAL_NUM > 1
	GPIO_GetPin(_LEDSTRn_OUTPUT1),
#endif
#if	_LEDSTRn_IO_TOTAL_NUM > 2
	GPIO_GetPin(_LEDSTRn_OUTPUT2),
#endif
#if	_LEDSTRn_IO_TOTAL_NUM > 3
	GPIO_GetPin(_LEDSTRn_OUTPUT3),
#endif
#if	_LEDSTRn_IO_TOTAL_NUM > 4
	GPIO_GetPin(_LEDSTRn_OUTPUT4),
#endif
#if	_LEDSTRn_IO_TOTAL_NUM > 5
	GPIO_GetPin(_LEDSTRn_OUTPUT5),
#endif
#if	_LEDSTRn_IO_TOTAL_NUM > 6
	GPIO_GetPin(_LEDSTRn_OUTPUT6),
#endif
#if	_LEDSTRn_IO_TOTAL_NUM > 7
	GPIO_GetPin(_LEDSTRn_OUTPUT7),
#endif
#if	_LEDSTRn_IO_TOTAL_NUM > 8
	GPIO_GetPin(_LEDSTRn_OUTPUT8),
#endif
#if	_LEDSTRn_IO_TOTAL_NUM > 9
	GPIO_GetPin(_LEDSTRn_OUTPUT9),
#endif
#if	_LEDSTRn_IO_TOTAL_NUM > 10
	GPIO_GetPin(_LEDSTRn_OUTPUT10),
#endif
#if	_LEDSTRn_IO_TOTAL_NUM > 11
	GPIO_GetPin(_LEDSTRn_OUTPUT11),
#endif
#if	_LEDSTRn_IO_TOTAL_NUM > 12
	GPIO_GetPin(_LEDSTRn_OUTPUT12),
#endif
#if	_LEDSTRn_IO_TOTAL_NUM > 13
	GPIO_GetPin(_LEDSTRn_OUTPUT13),
#endif
#if	_LEDSTRn_IO_TOTAL_NUM > 14
	GPIO_GetPin(_LEDSTRn_OUTPUT14),
#endif
#if	_LEDSTRn_IO_TOTAL_NUM > 15
	GPIO_GetPin(_LEDSTRn_OUTPUT15),
#endif
};
//#if _LEDSTRn_Data_Byte==1
//U8 Color_buf_T[_LEDSTRn_MAX_NUM][24] __attribute__((aligned(4)));
//#else
U16 Color_buf_T[_LEDSTRn_MAX_NUM][24] __attribute__((aligned(4)));
//#endif
 
void RGB_Pattern1(GPIO_TypeDef *gpiox,U8 *code);
void RGB_Pattern2(GPIO_TypeDef *gpiox,U16 *code);

U8 IO_shift;	
U16 IO_Mask,IO_Mask_not;
GPIO_TypeDef *LEDSTRn_port;// = GPIO_GetPort(_LEDSTRn_OUTPUT0);
U16 LED_num[_LEDSTRn_IO_TOTAL_NUM] = { 0 };
U8 LEDSTRn_ID[_LEDSTRn_IO_TOTAL_NUM] = { 0 };
U8 LEDSTRn_ID_const[_LEDSTRn_IO_TOTAL_NUM] = { 0 };
U8 LEDSTRn_ID_temp[_LEDSTRn_IO_TOTAL_NUM] =  { 0 };

void CB_Multi_LEDSTR_Init(void)
{
	LEDSTRn_port = GPIO_GetPort(_LEDSTRn_OUTPUT0);
	memcpy(LEDSTRn_ID,LEDSTRn_PinMap,sizeof(LEDSTRn_ID_temp));
	memcpy(LEDSTRn_ID_const,LEDSTRn_PinMap,sizeof(LEDSTRn_ID_temp));
	memcpy(LEDSTRn_ID_temp,LEDSTRn_PinMap,sizeof(LEDSTRn_ID_temp));
	// extern U8 LEDSTRn_ID[_LEDSTRn_IO_TOTAL_NUM];
#if _LEDSTRn_TIMER==USE_TIMER0
	SMU_PeriphClkCmd(SMU_PERIPH_TMR,ENABLE);
#if _EF_SERIES
	TIMER_Init(LEDSTRn_TIMER,C_Timer_Setting_16K);
#else
	TIMER_Init(LEDSTRn_TIMER,TIMER_CLKDIV_1,C_Timer_Setting_16K);
#endif
	TIMER_IntCmd(LEDSTRn_TIMER,ENABLE);
	INTC_IrqCmd(IRQ_BIT_TMR0,ENABLE);
	INTC_SetPriority(IRQ_VALVE_TMR0,IRQ_PRIORITY_0);
#elif _LEDSTRn_TIMER==USE_TIMER1
	SMU_PeriphClkCmd(SMU_PERIPH_TMR,ENABLE);
#if _EF_SERIES
	TIMER_Init(LEDSTRn_TIMER,C_Timer_Setting_16K);
#else
	TIMER_Init(LEDSTRn_TIMER,TIMER_CLKDIV_1,C_Timer_Setting_16K);
#endif
	TIMER_IntCmd(LEDSTRn_TIMER,ENABLE);
	INTC_IrqCmd(IRQ_BIT_TMR1,ENABLE);
	INTC_SetPriority(IRQ_VALVE_TMR1,IRQ_PRIORITY_0);
#elif _LEDSTRn_TIMER==USE_TIMER2
	SMU_PeriphClkCmd(SMU_PERIPH_TMR,ENABLE);
#if _EF_SERIES
	TIMER_Init(LEDSTRn_TIMER,C_Timer_Setting_16K);
#else
	TIMER_Init(LEDSTRn_TIMER,TIMER_CLKDIV_1,C_Timer_Setting_16K);
#endif
	TIMER_IntCmd(LEDSTRn_TIMER,ENABLE);
	INTC_IrqCmd(IRQ_BIT_TMR2,ENABLE);
	INTC_SetPriority(IRQ_VALVE_TMR2,IRQ_PRIORITY_0);
#elif _LEDSTRn_TIMER==USE_TIMER_PWMA
	SMU_PeriphClkCmd(SMU_PERIPH_PWM,ENABLE);
#if _EF_SERIES
	TIMER_Init(LEDSTRn_TIMER,C_Timer_Setting_16K);
#else
	TIMER_Init(LEDSTRn_TIMER,TIMER_CLKDIV_1,C_Timer_Setting_16K);
#endif
	TIMER_IntCmd(LEDSTRn_TIMER,ENABLE);
	INTC_IrqCmd(IRQ_BIT_PWMA,ENABLE);
	INTC_SetPriority(IRQ_VALVE_PWMA,IRQ_PRIORITY_0);
#elif _LEDSTRn_TIMER==USE_TIMER_PWMB
    SMU_PeriphClkCmd(SMU_PERIPH_PWM,ENABLE);
#if _EF_SERIES
	TIMER_Init(LEDSTRn_TIMER,C_Timer_Setting_16K);
#else
	TIMER_Init(LEDSTRn_TIMER,TIMER_CLKDIV_1,C_Timer_Setting_16K);
#endif
	TIMER_IntCmd(LEDSTRn_TIMER,ENABLE);
	INTC_IrqCmd(IRQ_BIT_PWMB,ENABLE);
	INTC_SetPriority(IRQ_VALVE_PWMB,IRQ_PRIORITY_0);
#elif _LEDSTRn_TIMER==USE_TIMER3
#if _EF_SERIES
	SMU_PeriphClkCmd(SMU_PERIPH_TMR,ENABLE);
	TIMER_Init(LEDSTRn_TIMER,C_Timer_Setting_16K);
	TIMER_IntCmd(LEDSTRn_TIMER,ENABLE);
	INTC_IrqCmd(IRQ_BIT_TMR3,ENABLE);
	INTC_SetPriority(IRQ_VALVE_TMR3,IRQ_PRIORITY_0);
#else
	#error	"LEDSTRn does not support Timer 3"
#endif
#endif
	U8 k;
	for(k=0 ;k<_LEDSTRn_IO_TOTAL_NUM; k++)
	{
		IO_Mask |= (1 << LEDSTRn_ID[k]);
	}
	IO_Mask_not = ~IO_Mask;
	GPIOA->Data &= IO_Mask_not;
	GPIOA->DIR &= IO_Mask_not;
#if _LEDSTRn_Data_Byte==1
	if(IO_Mask & 0xff)
		IO_shift=0;
	else
		IO_shift=8;
#endif
}

void CB_LEDSTRn_Start(void)
{
#if _LEDSTR_MODULE
	LEDSTR_Stop(-1);
#endif	
#if _LEDSTRm_MODULE
	LEDSTRm_Stop();
#endif	
}

void CB_LEDSTRn_GetData(U8* _buf, U32 _addr, U32 _size, U8 _mode)
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
            break;//error
    }
}

U32 CB_LEDSTRn_CheckTypeAddress(U32 index, U8 mode)
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
void CB_LEDSTRn_Timer_Cmd(U8 cmd)
{
	TIMER_Cmd(LEDSTRn_TIMER,cmd);
}

void CB_LEDSTRn_ReadStart(U32 addr,U8 _mode)
{
    switch(_mode) {
#if _EF_SERIES
        case EF_UD_MODE:
        case EF_MODE:
            
            break;
#endif
#if	_SPI_MODULE
        //case SPI_UD_MODE:
        case SPI_MODE:
            SPI_FastReadStart(addr);
            break;
#endif
#if (_SPI1_MODULE && _SPI1_USE_FLASH)
        //case SPI1_UD_MODE:
        case SPI1_MODE:
            SPI1_FastReadStart(addr);
            break;
#endif
        default:
            break;//error
    }
}

void CB_LEDSTRn_ReadEnd(U8 _mode)
{
    switch(_mode) {
#if _EF_SERIES
        case EF_UD_MODE:
        case EF_MODE:
            
            break;
#endif
#if	_SPI_MODULE
        //case SPI_UD_MODE:
        case SPI_MODE:
            SPI_FastReadStop();
            break;
#endif
#if (_SPI1_MODULE && _SPI1_USE_FLASH)
        //case SPI1_UD_MODE:
        case SPI1_MODE:
            SPI1_FastReadStop();
            break;
#endif
        default:
            
            break;//error
    }
}

void CB_LEDSTRn_ReadByte(U32 *data,U8 len,U32 _addr,U8 _mode)
{
    switch(_mode) {
#if _EF_SERIES
        case EF_UD_MODE:
        case EF_MODE:
            	memcpy(data,(U8 *)_addr,len);	
            break;
#endif
#if	_SPI_MODULE
        //case SPI_UD_MODE:
        case SPI_MODE:
            do {
		        SPI_TypeDef *SPI = P_SPI0;
				SPI->Ctrl &= (~C_SPI_FIFO_Length);
				SPI->Ctrl = SPI->Ctrl | (C_SPI_Rx_Start | ((len-1)<<SPI_DATA_NUM_OFFSET));
				
				while(SPI->Ctrl&C_SPI_Rx_Busy);	
				*data = SPI->Data;
			} while(0);
            break;
#endif
#if (_SPI1_MODULE && _SPI1_USE_FLASH)
        //case SPI1_UD_MODE:
        case SPI1_MODE:
            do {
		        SPI_TypeDef *SPI = P_SPI1;
				SPI->Ctrl &= (~C_SPI_FIFO_Length);
				SPI->Ctrl = SPI->Ctrl | (C_SPI_Rx_Start | ((len-1)<<SPI_DATA_NUM_OFFSET));
				
				while(SPI->Ctrl&C_SPI_Rx_Busy);	
				*data = SPI->Data;
			} while(0);
            break;
#endif
    }
}
void CB_RGB_Pattern1(void *code)
{
	RGB_Pattern1(LEDSTRn_port,code);
}

void CB_RGB_Pattern2(void *code)
{
	RGB_Pattern2(LEDSTRn_port,code);
}

#pragma GCC optimize ("O0")
#if _EF_SERIES
//====================================================================================
//  40MHz wait one state code pattern 
//====================================================================================
#if	OPTION_HIGH_FREQ_CLK==40000000
void RGB_Pattern1(GPIO_TypeDef *gpiox,U8 *code)
{
		__asm__(".align 4\t\n"
				"addi $r8,$r1,#0x18\t\n"	
				"lwi333 $r6,[$r0+#0x4]\t\n"
				"Start:\t\n"
				"or33 $r6,%[IO_Mask]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRn_CODE_0>=1		     
			  	"nop\t\n"
			  	"nop\t\n"
				"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0>=3		
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0>=4		
			  	"nop\t\n"	
#endif				 				
				"lbi.bi $r7,[$r1],#0x1\t\n"
				"sll $r7,$r7,%[IO_shift]\t\n"
				"or33 $r7,%[IO_Mask_not]\t\n"
				"and33 $r6,$r7\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRn_CODE_0<=1		     
			  	"nop\t\n"
			  	"nop\t\n"		  	
#endif				 
#if _LEDSTRn_CODE_0<=2		 
			  	"nop\t\n"
				"nop\t\n"	
#endif				 
#if _LEDSTRn_CODE_0<=3		
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0<=4		

#endif			
	 						
#if	_LEDSTRn_CODE_1>=1		     
	  	
#endif				 
#if _LEDSTRn_CODE_1>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1>=3		
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1>=4		
			  	"nop\t\n"
				"nop\t\n"			  			
#endif	
				"and33 $r6,%[IO_Mask_not]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRn_CODE_1<=1		     
			  	"nop\t\n"	
#endif				 
#if _LEDSTRn_CODE_1<=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1<=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1<=4		
			  	"nop\t\n"
				"nop\t\n"
				"nop\t\n"
#endif					
				"bne $r1,$r8,Start\t\n"
			  :
		      :"r"(gpiox), "r"(code), [IO_Mask] "r" (IO_Mask) ,[IO_Mask_not] "r" (IO_Mask_not) ,[IO_shift] "r" (IO_shift)
		      :"$r6", "$r7", "$r8", "$r9"
		     );
}

void RGB_Pattern2(GPIO_TypeDef *gpiox,U16 *code)
{
		__asm__(".align 4\t\n"
				"addi $r8,$r1,#0x30\t\n"	
				"lwi333 $r6,[$r0+#0x4]\t\n"
				"Start:\t\n"
				"or33 $r6,%[IO_Mask]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRn_CODE_0>=1		     
			  	"nop\t\n"
			  	"nop\t\n"
				"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0>=3		
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0>=4		
			  	"nop\t\n"	
#endif				 				
				"nop\t\n"
				"lhi.bi $r7,[$r1],#0x2\t\n"				
				"or33 $r7,%[IO_Mask_not]\t\n"
				"and33 $r6,$r7\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRn_CODE_0<=1		     
			  	"nop\t\n"
			  	"nop\t\n"		  	
#endif				 
#if _LEDSTRn_CODE_0<=2		 
			  	"nop\t\n"
				"nop\t\n"	
#endif				 
#if _LEDSTRn_CODE_0<=3		
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0<=4		

#endif			
	 						
#if	_LEDSTRn_CODE_1>=1		     
	  	
#endif				 
#if _LEDSTRn_CODE_1>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1>=3		
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1>=4		
			  	"nop\t\n"
				"nop\t\n"			  			
#endif	
				"and33 $r6,%[IO_Mask_not]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRn_CODE_1<=1		     
			  	"nop\t\n"	
#endif				 
#if _LEDSTRn_CODE_1<=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1<=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1<=4		
			  	"nop\t\n"
				"nop\t\n"
				"nop\t\n"
#endif					
				"bne $r1,$r8,Start\t\n"
			  :
		      :"r"(gpiox), "r"(code), [IO_Mask] "r" (IO_Mask) ,[IO_Mask_not] "r" (IO_Mask_not) ,[IO_shift] "r" (IO_shift)
		      :"$r6", "$r7", "$r8", "$r9"
		     );
}

//====================================================================================
//  48MHz wait one state code pattern 
//====================================================================================
#elif	OPTION_HIGH_FREQ_CLK==48000000

void RGB_Pattern1(GPIO_TypeDef *gpiox,U8 *code)
{
			__asm__(".align 4\t\n"
				"addi $r8,$r1,#0x18\t\n"	
				"lwi333 $r6,[$r0+#0x4]\t\n"
				"Start:\t\n"
				"or33 $r6,%[IO_Mask]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRn_CODE_0>=1		     
			  	"nop\t\n"
			  	"nop\t\n"
				"nop\t\n"
				"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0>=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0>=4		
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"	
#endif				 				
				"lbi.bi $r7,[$r1],#0x1\t\n"
				"sll $r7,$r7,%[IO_shift]\t\n"
				"or33 $r7,%[IO_Mask_not]\t\n"
				"and33 $r6,$r7\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRn_CODE_0<=1		     
			  	"nop\t\n"
			  	"nop\t\n"			  	
#endif				 
#if _LEDSTRn_CODE_0<=2		 
			  	"nop\t\n"
				"nop\t\n"	
#endif				 
#if _LEDSTRn_CODE_0<=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0<=4		

#endif			
	 						
#if	_LEDSTRn_CODE_1>=1		     
	  	
#endif				 
#if _LEDSTRn_CODE_1>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1>=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1>=4		
			  	"nop\t\n"
				"nop\t\n"			  				
#endif	
				"and33 $r6,%[IO_Mask_not]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRn_CODE_1<=1		     
			  	"nop\t\n"
			  	"nop\t\n"		  		
#endif				 
#if _LEDSTRn_CODE_1<=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1<=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1<=4		
			  	"nop\t\n"
				"nop\t\n"
				"nop\t\n"
				"nop\t\n"
#endif					
				"bne $r1,$r8,Start\t\n"
			  :
		      :"r"(gpiox), "r"(code), [IO_Mask] "r" (IO_Mask) ,[IO_Mask_not] "r" (IO_Mask_not) ,[IO_shift] "r" (IO_shift)
		      :"$r6", "$r7", "$r8", "$r9"
		     );
}


void RGB_Pattern2(GPIO_TypeDef *gpiox,U16 *code)
{
		__asm__(".align 4\t\n"
				"addi $r8,$r1,#0x30\t\n"	
				"lwi333 $r6,[$r0+#0x4]\t\n"
				"Start1:\t\n"
				"or33 $r6,%[IO_Mask]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRn_CODE_0>=1		     
			  	"nop\t\n"
			  	"nop\t\n"
				"nop\t\n"
				"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0>=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0>=4		
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"	
#endif				 				
				"nop\t\n"
				"lhi.bi $r7,[$r1],#0x2\t\n"				
				"or33 $r7,%[IO_Mask_not]\t\n"
				"and33 $r6,$r7\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRn_CODE_0<=1		     
			  	"nop\t\n"
			  	"nop\t\n"			  	
#endif				 
#if _LEDSTRn_CODE_0<=2		 
			  	"nop\t\n"
				"nop\t\n"	
#endif				 
#if _LEDSTRn_CODE_0<=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0<=4		

#endif			
	 						
#if	_LEDSTRn_CODE_1>=1		     
	  	
#endif				 
#if _LEDSTRn_CODE_1>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1>=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1>=4		
			  	"nop\t\n"
				"nop\t\n"			  				
#endif	
				"and33 $r6,%[IO_Mask_not]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRn_CODE_1<=1		     
			  	"nop\t\n"
			  	"nop\t\n"		  		
#endif				 
#if _LEDSTRn_CODE_1<=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1<=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1<=4		
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
#endif

#else
//====================================================================================
//  OTP 32MHz code pattern 
//====================================================================================
#if OPTION_HIGH_FREQ_CLK==32000000

void RGB_Pattern1(GPIO_TypeDef *gpiox,U8 *code)
{
		__asm__(".align 4\t\n"
				"addi $r8,$r1,#0x18\t\n"	
				"lwi333 $r6,[$r0+#0x4]\t\n"
				"Start:\t\n"
				"or33 $r6,%[IO_Mask]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRn_CODE_0>=1		     
			  	"nop\t\n"
			  	"nop\t\n"
				"nop\t\n"
				"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0>=3		
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0>=4		
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"	
#endif				 				
				"lbi.bi $r7,[$r1],#0x1\t\n"
				"sll $r7,$r7,%[IO_shift]\t\n"
				"or33 $r7,%[IO_Mask_not]\t\n"
				"and33 $r6,$r7\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRn_CODE_0<=1		     
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"			  	
#endif				 
#if _LEDSTRn_CODE_0<=2		 
			  	"nop\t\n"
			  	"nop\t\n"
				"nop\t\n"	
#endif				 
#if _LEDSTRn_CODE_0<=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0<=4		

#endif			
	 						
#if	_LEDSTRn_CODE_1>=1		     
	  	
#endif				 
#if _LEDSTRn_CODE_1>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1>=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1>=4		
			  	"nop\t\n"
				"nop\t\n"			  	
			  	"nop\t\n"
			  	"nop\t\n"			
#endif	
				"and33 $r6,%[IO_Mask_not]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRn_CODE_1<=1		     
			  	"nop\t\n"
			  	"nop\t\n"		  	
			  	"nop\t\n"		
#endif				 
#if _LEDSTRn_CODE_1<=2		 
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1<=3		
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1<=4		
			  	"nop\t\n"
				"nop\t\n"
				"nop\t\n"
				"nop\t\n"	
				"nop\t\n"
#endif					
				"bne $r1,$r8,Start\t\n"
			  :
		      :"r"(gpiox), "r"(code), [IO_Mask] "r" (IO_Mask) ,[IO_Mask_not] "r" (IO_Mask_not) ,[IO_shift] "r" (IO_shift)
		      :"$r6", "$r7", "$r8", "$r9"
		     );
}

void RGB_Pattern2(GPIO_TypeDef *gpiox,U16 *code)
{
		__asm__(".align 4\t\n"
				"addi $r8,$r1,#0x30\t\n"	
				"lwi333 $r6,[$r0+#0x4]\t\n"
				"Start:\t\n"
				"or33 $r6,%[IO_Mask]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRn_CODE_0>=1		     
			  	"nop\t\n"
			  	"nop\t\n"
				"nop\t\n"
				"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0>=3		
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0>=4		
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"	
#endif				 				
				"nop\t\n"
				"lhi.bi $r7,[$r1],#0x2\t\n"				
				"or33 $r7,%[IO_Mask_not]\t\n"
				"and33 $r6,$r7\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRn_CODE_0<=1		     
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"			  	
#endif				 
#if _LEDSTRn_CODE_0<=2		 
			  	"nop\t\n"
			  	"nop\t\n"
				"nop\t\n"	
#endif				 
#if _LEDSTRn_CODE_0<=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0<=4		

#endif			
	 						
#if	_LEDSTRn_CODE_1>=1		     
	  	
#endif				 
#if _LEDSTRn_CODE_1>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1>=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1>=4		
			  	"nop\t\n"
				"nop\t\n"			  	
			  	"nop\t\n"
			  	"nop\t\n"			
#endif	
				"and33 $r6,%[IO_Mask_not]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRn_CODE_1<=1		     
			  	"nop\t\n"
			  	"nop\t\n"		  	
			  	"nop\t\n"		
#endif				 
#if _LEDSTRn_CODE_1<=2		 
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1<=3		
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1<=4		
			  	"nop\t\n"
				"nop\t\n"
				"nop\t\n"
				"nop\t\n"	
				"nop\t\n"
#endif					
				"bne $r1,$r8,Start\t\n"
			  :
		      :"r"(gpiox), "r"(code), [IO_Mask] "r" (IO_Mask) ,[IO_Mask_not] "r" (IO_Mask_not) ,[IO_shift] "r" (IO_shift)
		      :"$r6", "$r7", "$r8", "$r9"
		     );
}
//====================================================================================
//  OTP 24MHz code pattern 
//====================================================================================
#elif	OPTION_HIGH_FREQ_CLK==24000000
void RGB_Pattern1(GPIO_TypeDef *gpiox,U8 *code)
{
		__asm__(".align 4\t\n"
				"addi $r8,$r1,#0x18\t\n"	
				"lwi333 $r6,[$r0+#0x4]\t\n"
				"Start:\t\n"
				"or33 $r6,%[IO_Mask]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRn_CODE_0>=1		     
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0>=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0>=4		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 				
				"lbi.bi $r7,[$r1],#0x1\t\n"
				"sll $r7,$r7,%[IO_shift]\t\n"
				"or33 $r7,%[IO_Mask_not]\t\n"
				"and33 $r6,$r7\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRn_CODE_0<=1		     
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"				  	
#endif				 
#if _LEDSTRn_CODE_0<=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0<=3		
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0<=4		

#endif			
	 						
#if	_LEDSTRn_CODE_1>=1		     
	  	
#endif				 
#if _LEDSTRn_CODE_1>=2		 
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1>=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1>=4		
				"nop\t\n"			  	
			  	"nop\t\n"
			  	"nop\t\n"			
#endif	
				"and33 $r6,%[IO_Mask_not]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRn_CODE_1<=1		     
			  	"nop\t\n"
			  	"nop\t\n"		  		
#endif				 
#if _LEDSTRn_CODE_1<=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1<=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1<=4		
				"nop\t\n"
				"nop\t\n"	
				"nop\t\n"
#endif					
				"bne $r1,$r8,Start\t\n"
			  :
		      :"r"(gpiox), "r"(code), [IO_Mask] "r" (IO_Mask) ,[IO_Mask_not] "r" (IO_Mask_not) ,[IO_shift] "r" (IO_shift)
		      :"$r6", "$r7", "$r8", "$r9"
		     );
}

void RGB_Pattern2(GPIO_TypeDef *gpiox,U16 *code)
{
		__asm__(".align 4\t\n"
				"addi $r8,$r1,#0x30\t\n"	
				"lwi333 $r6,[$r0+#0x4]\t\n"
				"Start:\t\n"
				"or33 $r6,%[IO_Mask]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRn_CODE_0>=1		     
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0>=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0>=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0>=4		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 				
				"nop\t\n"
				"lhi.bi $r7,[$r1],#0x2\t\n"				
				"or33 $r7,%[IO_Mask_not]\t\n"
				"and33 $r6,$r7\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRn_CODE_0<=1		     
			  	"nop\t\n"
			  	"nop\t\n"
			  	"nop\t\n"				  	
#endif				 
#if _LEDSTRn_CODE_0<=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0<=3		
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_0<=4		

#endif			
	 						
#if	_LEDSTRn_CODE_1>=1		     
	  	
#endif				 
#if _LEDSTRn_CODE_1>=2		 
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1>=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1>=4		
				"nop\t\n"			  	
			  	"nop\t\n"
			  	"nop\t\n"			
#endif	
				"and33 $r6,%[IO_Mask_not]\t\n"
				"swi333 $r6,[$r0+#0x4]\t\n"
#if	_LEDSTRn_CODE_1<=1		     
			  	"nop\t\n"
			  	"nop\t\n"		  		
#endif				 
#if _LEDSTRn_CODE_1<=2		 
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1<=3		
			  	"nop\t\n"
			  	"nop\t\n"
#endif				 
#if _LEDSTRn_CODE_1<=4		
				"nop\t\n"
				"nop\t\n"	
				"nop\t\n"
#endif					
				"bne $r1,$r8,Start\t\n"
			  :
		      :"r"(gpiox), "r"(code), [IO_Mask] "r" (IO_Mask) ,[IO_Mask_not] "r" (IO_Mask_not) ,[IO_shift] "r" (IO_shift)
		      :"$r6", "$r7", "$r8", "$r9"
		     );
}
#else
#error	"LEDSTRn only support high clock frequency 24MHz/32MHz!"
#endif
#endif
#endif		//_LEDSTRn_MODULE
