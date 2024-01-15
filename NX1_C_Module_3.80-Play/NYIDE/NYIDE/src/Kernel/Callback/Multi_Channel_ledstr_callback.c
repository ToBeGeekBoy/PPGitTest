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
#if _LEDSTR_MODULE 

#include "LEDSTRTab.h"

#ifndef _LEDSTR_Basic_Count
#define _LEDSTR_Basic_Count	((C_High_Clock+8000)/ 16000)
#endif

#define _CLK_IN_MS	(C_High_Clock/1000)
#ifndef _LEDSTR_Max_Amount
#define _LEDSTR_Max_Amount	(((_LEDSTR_TIMING * _CLK_IN_MS)+(_LEDSTR_Basic_Count/2))/_LEDSTR_Basic_Count)
#endif

#ifndef _LEDSTR_LED_TOTAL_NUM
#define	_LEDSTR_LED_TOTAL_NUM		   	((_LEDSTR_CH_NUM>0)?(_LEDSTR_CH0_NUM):0)	\
									+	((_LEDSTR_CH_NUM>1)?(_LEDSTR_CH1_NUM):0)	\
									+	((_LEDSTR_CH_NUM>2)?(_LEDSTR_CH2_NUM):0)	\
									+	((_LEDSTR_CH_NUM>3)?(_LEDSTR_CH3_NUM):0)	\
									+	((_LEDSTR_CH_NUM>4)?(_LEDSTR_CH4_NUM):0)	\
									+	((_LEDSTR_CH_NUM>5)?(_LEDSTR_CH5_NUM):0)	\
									+	((_LEDSTR_CH_NUM>6)?(_LEDSTR_CH6_NUM):0)	\
									+	((_LEDSTR_CH_NUM>7)?(_LEDSTR_CH7_NUM):0)	\
									+	((_LEDSTR_CH_NUM>8)?(_LEDSTR_CH8_NUM):0)	\
									+	((_LEDSTR_CH_NUM>9)?(_LEDSTR_CH9_NUM):0)	\
									+	((_LEDSTR_CH_NUM>10)?(_LEDSTR_CH10_NUM):0)	\
									+	((_LEDSTR_CH_NUM>11)?(_LEDSTR_CH11_NUM):0)	\
									+	((_LEDSTR_CH_NUM>12)?(_LEDSTR_CH12_NUM):0)	\
									+	((_LEDSTR_CH_NUM>13)?(_LEDSTR_CH13_NUM):0)	\
									+	((_LEDSTR_CH_NUM>14)?(_LEDSTR_CH14_NUM):0)	\
									+	((_LEDSTR_CH_NUM>15)?(_LEDSTR_CH15_NUM):0)	
#endif

#if	(_LEDSTR_Max_Amount-1)<(_LEDSTR_LED_TOTAL_NUM+_LEDSTR_CH_NUM)
#error	"LEDSTR over number!"
#endif

#if _LEDSTR_CH_NUM==0
	#error "LEDSTR_CH_NUM can not be 0."
#endif

#if _LEDSTR_CH_NUM>16
	#error "LEDSTR_CH_NUM over 16."
#endif

#if		((_LEDSTR_CH_NUM>0)&&_LEDSTR_CH0_PIN>GPIO_MAX)	\
		||((_LEDSTR_CH_NUM>1)&&_LEDSTR_CH1_PIN>GPIO_MAX)	\
		||((_LEDSTR_CH_NUM>2)&&_LEDSTR_CH2_PIN>GPIO_MAX)	\
		||((_LEDSTR_CH_NUM>3)&&_LEDSTR_CH3_PIN>GPIO_MAX)	\
		||((_LEDSTR_CH_NUM>4)&&_LEDSTR_CH4_PIN>GPIO_MAX)	\
		||((_LEDSTR_CH_NUM>5)&&_LEDSTR_CH5_PIN>GPIO_MAX)	\
		||((_LEDSTR_CH_NUM>6)&&_LEDSTR_CH6_PIN>GPIO_MAX)	\
		||((_LEDSTR_CH_NUM>7)&&_LEDSTR_CH7_PIN>GPIO_MAX)	\
		||((_LEDSTR_CH_NUM>8)&&_LEDSTR_CH8_PIN>GPIO_MAX)	\
		||((_LEDSTR_CH_NUM>9)&&_LEDSTR_CH9_PIN>GPIO_MAX)	\
		||((_LEDSTR_CH_NUM>10)&&_LEDSTR_CH10_PIN>GPIO_MAX)	\
		||((_LEDSTR_CH_NUM>11)&&_LEDSTR_CH11_PIN>GPIO_MAX)	\
		||((_LEDSTR_CH_NUM>12)&&_LEDSTR_CH12_PIN>GPIO_MAX)	\
		||((_LEDSTR_CH_NUM>13)&&_LEDSTR_CH13_PIN>GPIO_MAX)	\
		||((_LEDSTR_CH_NUM>14)&&_LEDSTR_CH14_PIN>GPIO_MAX)	\
		||((_LEDSTR_CH_NUM>15)&&_LEDSTR_CH15_PIN>GPIO_MAX)
		#error "LEDSTR OUTPUT Pin undefine"
#endif

#if		((_LEDSTR_CH_NUM>0)&&_LEDSTR_CH0_NUM>_LEDSTR_LED_TOTAL_NUM)	\
		||((_LEDSTR_CH_NUM>1)&&_LEDSTR_CH1_NUM>_LEDSTR_LED_TOTAL_NUM)	\
		||((_LEDSTR_CH_NUM>2)&&_LEDSTR_CH2_NUM>_LEDSTR_LED_TOTAL_NUM)	\
		||((_LEDSTR_CH_NUM>3)&&_LEDSTR_CH3_NUM>_LEDSTR_LED_TOTAL_NUM)	\
		||((_LEDSTR_CH_NUM>4)&&_LEDSTR_CH4_NUM>_LEDSTR_LED_TOTAL_NUM)	\
		||((_LEDSTR_CH_NUM>5)&&_LEDSTR_CH5_NUM>_LEDSTR_LED_TOTAL_NUM)	\
		||((_LEDSTR_CH_NUM>6)&&_LEDSTR_CH6_NUM>_LEDSTR_LED_TOTAL_NUM)	\
		||((_LEDSTR_CH_NUM>7)&&_LEDSTR_CH7_NUM>_LEDSTR_LED_TOTAL_NUM)	\
		||((_LEDSTR_CH_NUM>8)&&_LEDSTR_CH8_NUM>_LEDSTR_LED_TOTAL_NUM)	\
		||((_LEDSTR_CH_NUM>9)&&_LEDSTR_CH9_NUM>_LEDSTR_LED_TOTAL_NUM)	\
		||((_LEDSTR_CH_NUM>10)&&_LEDSTR_CH10_NUM>_LEDSTR_LED_TOTAL_NUM)	\
		||((_LEDSTR_CH_NUM>11)&&_LEDSTR_CH11_NUM>_LEDSTR_LED_TOTAL_NUM)	\
		||((_LEDSTR_CH_NUM>12)&&_LEDSTR_CH12_NUM>_LEDSTR_LED_TOTAL_NUM)	\
		||((_LEDSTR_CH_NUM>13)&&_LEDSTR_CH13_NUM>_LEDSTR_LED_TOTAL_NUM)	\
		||((_LEDSTR_CH_NUM>14)&&_LEDSTR_CH14_NUM>_LEDSTR_LED_TOTAL_NUM)	\
		||((_LEDSTR_CH_NUM>15)&&_LEDSTR_CH15_NUM>_LEDSTR_LED_TOTAL_NUM)
		#error "LEDSTR Channel number over _LEDSTR_LED_TOTAL_NUM"
#endif

const  U16 LEDSTR_Num_List[16] = {   	((_LEDSTR_CH_NUM>0)?(_LEDSTR_CH0_NUM):0)	\
									,	((_LEDSTR_CH_NUM>1)?(_LEDSTR_CH1_NUM):0)	\
									,	((_LEDSTR_CH_NUM>2)?(_LEDSTR_CH2_NUM):0)	\
									,	((_LEDSTR_CH_NUM>3)?(_LEDSTR_CH3_NUM):0)	\
									,	((_LEDSTR_CH_NUM>4)?(_LEDSTR_CH4_NUM):0)	\
									,	((_LEDSTR_CH_NUM>5)?(_LEDSTR_CH5_NUM):0)	\
									,	((_LEDSTR_CH_NUM>6)?(_LEDSTR_CH6_NUM):0)	\
									,	((_LEDSTR_CH_NUM>7)?(_LEDSTR_CH7_NUM):0)	\
									,	((_LEDSTR_CH_NUM>8)?(_LEDSTR_CH8_NUM):0)	\
									,	((_LEDSTR_CH_NUM>9)?(_LEDSTR_CH9_NUM):0)	\
									,	((_LEDSTR_CH_NUM>10)?(_LEDSTR_CH10_NUM):0)	\
									,	((_LEDSTR_CH_NUM>11)?(_LEDSTR_CH11_NUM):0)	\
									,	((_LEDSTR_CH_NUM>12)?(_LEDSTR_CH12_NUM):0)	\
									,	((_LEDSTR_CH_NUM>13)?(_LEDSTR_CH13_NUM):0)	\
									,	((_LEDSTR_CH_NUM>14)?(_LEDSTR_CH14_NUM):0)	\
									,	((_LEDSTR_CH_NUM>15)?(_LEDSTR_CH15_NUM):0)	};
									
									
const  U16 LEDSTR_IO_List[16] = {   	((_LEDSTR_CH_NUM>0)?(_LEDSTR_CH0_PIN):0)	\
									,	((_LEDSTR_CH_NUM>1)?(_LEDSTR_CH1_PIN):0)	\
									,	((_LEDSTR_CH_NUM>2)?(_LEDSTR_CH2_PIN):0)	\
									,	((_LEDSTR_CH_NUM>3)?(_LEDSTR_CH3_PIN):0)	\
									,	((_LEDSTR_CH_NUM>4)?(_LEDSTR_CH4_PIN):0)	\
									,	((_LEDSTR_CH_NUM>5)?(_LEDSTR_CH5_PIN):0)	\
									,	((_LEDSTR_CH_NUM>6)?(_LEDSTR_CH6_PIN):0)	\
									,	((_LEDSTR_CH_NUM>7)?(_LEDSTR_CH7_PIN):0)	\
									,	((_LEDSTR_CH_NUM>8)?(_LEDSTR_CH8_PIN):0)	\
									,	((_LEDSTR_CH_NUM>9)?(_LEDSTR_CH9_PIN):0)	\
									,	((_LEDSTR_CH_NUM>10)?(_LEDSTR_CH10_PIN):0)	\
									,	((_LEDSTR_CH_NUM>11)?(_LEDSTR_CH11_PIN):0)	\
									,	((_LEDSTR_CH_NUM>12)?(_LEDSTR_CH12_PIN):0)	\
									,	((_LEDSTR_CH_NUM>13)?(_LEDSTR_CH13_PIN):0)	\
									,	((_LEDSTR_CH_NUM>14)?(_LEDSTR_CH14_PIN):0)	\
									,	((_LEDSTR_CH_NUM>15)?(_LEDSTR_CH15_PIN):0)	};
									
							
const U16 LEDSTR_parameter[] = {
	_LEDSTR_CH_NUM,
	_LEDSTR_LED_TOTAL_NUM,
	_LEDSTR_Max_Amount,
};

U32 Color_buf[2][_LEDSTR_LED_TOTAL_NUM] ;
U8 LEDSTR_Play_List[_LEDSTR_CH_NUM] = { 0 };
U8 LEDSTR_Play_Temp_List[_LEDSTR_CH_NUM] = { 0 };
U8 LEDSTR_Clean_List[_LEDSTR_CH_NUM] = { 0 };
U16 LEDSTR_Count_List[_LEDSTR_CH_NUM] = { 0 };
U8 LEDSTR_Code_Unit[_LEDSTR_CH_NUM] = { 0 };
U8 LEDSTR_Keep[_LEDSTR_CH_NUM] = { 0 };
U8 LEDSTR_Format[_LEDSTR_CH_NUM] = { 0 };
U32 LEDSTR_Address_List[_LEDSTR_CH_NUM] = { 0 };
U32 LEDSTR_Length_List[_LEDSTR_CH_NUM] = { 0 };

U16 IO_H,IO_L;
U32 *Play_buf;
U8 LEDSTR_Port;
U8 LEDSTRBufCtl_state;

void RGB_Pattern(GPIO_TypeDef *gpiox,U32 code);
//------------------------------------------------------------------//
// LEDSTR Init
// Parameter:
//          NONE
// return value:
//     		NONE
//------------------------------------------------------------------//
void CB_LEDSTR_Init(void)
{	
	U8 k;
	for(k=0 ; k<_LEDSTR_CH_NUM ; k++)
	{
		GPIO_Init(GPIO_GetPort(LEDSTR_IO_List[k]),GPIO_GetPin(LEDSTR_IO_List[k]),GPIO_MODE_OUT_LOW);
	}
#if _LEDSTR_TIMER==USE_TIMER0
	SMU_PeriphClkCmd(SMU_PERIPH_TMR,ENABLE);
#if _EF_SERIES
	TIMER_Init(LEDSTR_TIMER,(_LEDSTR_Basic_Count-1));
#else
	TIMER_Init(LEDSTR_TIMER,TIMER_CLKDIV_1,(_LEDSTR_Basic_Count-1));
#endif
	TIMER_IntCmd(LEDSTR_TIMER,ENABLE);
	INTC_IrqCmd(IRQ_BIT_TMR0,ENABLE);
	INTC_SetPriority(IRQ_VALVE_TMR0,IRQ_PRIORITY_0);
#elif _LEDSTR_TIMER==USE_TIMER1
	SMU_PeriphClkCmd(SMU_PERIPH_TMR,ENABLE);
#if _EF_SERIES
	TIMER_Init(LEDSTR_TIMER,(_LEDSTR_Basic_Count-1));
#else
	TIMER_Init(LEDSTR_TIMER,TIMER_CLKDIV_1,(_LEDSTR_Basic_Count-1));
#endif
	TIMER_IntCmd(LEDSTR_TIMER,ENABLE);
	INTC_IrqCmd(IRQ_BIT_TMR1,ENABLE);
	INTC_SetPriority(IRQ_VALVE_TMR1,IRQ_PRIORITY_0);
#elif _LEDSTR_TIMER==USE_TIMER2
	SMU_PeriphClkCmd(SMU_PERIPH_TMR,ENABLE);
#if _EF_SERIES
	TIMER_Init(LEDSTR_TIMER,(_LEDSTR_Basic_Count-1));
#else
	TIMER_Init(LEDSTR_TIMER,TIMER_CLKDIV_1,(_LEDSTR_Basic_Count-1));
#endif
	TIMER_IntCmd(LEDSTR_TIMER,ENABLE);
	INTC_IrqCmd(IRQ_BIT_TMR2,ENABLE);
	INTC_SetPriority(IRQ_VALVE_TMR2,IRQ_PRIORITY_0);
#elif _LEDSTR_TIMER==USE_TIMER_PWMA
	SMU_PeriphClkCmd(SMU_PERIPH_PWM,ENABLE);
#if _EF_SERIES
	TIMER_Init(LEDSTR_TIMER,(_LEDSTR_Basic_Count-1));
#else
	TIMER_Init(LEDSTR_TIMER,TIMER_CLKDIV_1,(_LEDSTR_Basic_Count-1));
#endif
	TIMER_IntCmd(LEDSTR_TIMER,ENABLE);
	INTC_IrqCmd(IRQ_BIT_PWMA,ENABLE);
	INTC_SetPriority(IRQ_VALVE_PWMA,IRQ_PRIORITY_0);
#elif _LEDSTR_TIMER==USE_TIMER_PWMB
    SMU_PeriphClkCmd(SMU_PERIPH_PWM,ENABLE);
#if _EF_SERIES
	TIMER_Init(LEDSTR_TIMER,(_LEDSTR_Basic_Count-1));
#else
	TIMER_Init(LEDSTR_TIMER,TIMER_CLKDIV_1,(_LEDSTR_Basic_Count-1));
#endif
	TIMER_IntCmd(LEDSTR_TIMER,ENABLE);
	INTC_IrqCmd(IRQ_BIT_PWMB,ENABLE);
	INTC_SetPriority(IRQ_VALVE_PWMB,IRQ_PRIORITY_0);
#elif _LEDSTR_TIMER==USE_TIMER3
#if _EF_SERIES
	SMU_PeriphClkCmd(SMU_PERIPH_TMR,ENABLE);
	TIMER_Init(LEDSTR_TIMER,(_LEDSTR_Basic_Count-1));
	TIMER_IntCmd(LEDSTR_TIMER,ENABLE);
	INTC_IrqCmd(IRQ_BIT_TMR3,ENABLE);
	INTC_SetPriority(IRQ_VALVE_TMR3,IRQ_PRIORITY_0);
#else
	#error	"LEDSTR does not support Timer 3"
#endif
#endif
	LEDSTRBufCtl_state= STATUS_STOP;
}
//------------------------------------------------------------------//
// LEDSTR Start
// Parameter:
//         	NONE
// return value:
//     		NONE
//------------------------------------------------------------------//
void CB_LEDSTR_Start(void)
{
#if _LEDSTRn_MODULE
	LEDSTRn_Stop();
#endif	
#if _LEDSTRm_MODULE
	LEDSTRm_Stop();
#endif	
}
//------------------------------------------------------------------//
// LEDSTR Init IO
// Parameter:
//         	pin:
// return value:
//     		NONE
//------------------------------------------------------------------//
void CB_LEDSTR_Init_IO(U8 pin)
{
	LEDSTR_Port=GPIO_GetPortIdx(pin);
	pin=GPIO_GetPin(pin);
	GPIO_Init(GPIO_PortList[LEDSTR_Port],pin,GPIO_MODE_OUT_LOW);
    IO_H=(1<<pin);
	IO_L=~(1<<pin);
}
//------------------------------------------------------------------//
// CB_LEDSTR_Timer_Cmd
// Parameter:
//          NONE
// return value:
//     		NONE
//------------------------------------------------------------------//
void CB_LEDSTR_Timer_Cmd(U8 cmd)
{
	TIMER_Cmd(LEDSTR_TIMER,cmd);
}

void CB_LEDSTR_SPI0_ReceiveData(void *buf,U16 len)
{
#if	_SPI_MODULE
	SPI_ReceiveData(buf,len);	
#endif	
}

void CB_LEDSTR_SPI1_ReceiveData(void *buf,U16 len)
{
#if (_SPI1_MODULE && _SPI1_USE_FLASH)
	SPI1_ReceiveData(buf,len);	
#endif
}

void CB_LEDSTR_RGB_Pattern(U8 LEDSTR_Port, U32 code)
{
	RGB_Pattern(GPIO_PortList[LEDSTR_Port],code);
}

U32 CB_LEDSTR_CheckTypeAddress(U32 index, U8 mode)
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

void CB_LEDSTR_GetData(U8* _buf, U32 _addr, U32 _size, U8 _mode)
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

void CB_LEDSTR_ReadStart(U32 addr, U8 mode)
{
    switch(mode) {
        default:
        case OTP_MODE:
            break; // do nothing
        case SPI_MODE:
#if _SPI_MODULE        
            SPI_FastReadStart(addr);
 #endif    
            break;
        case SPI1_MODE:
#if (_SPI1_MODULE && _SPI1_USE_FLASH)       
            SPI1_FastReadStart(addr);
 #endif    
            break;
    }
}
void CB_LEDSTR_ReceiveData(void *buf, U32 addr, U16 len, U8 mode)
{
    switch(mode) {
        default:
            break;
        case OTP_MODE:
            memcpy(buf,(void*)addr, len);
            break;
        case SPI_MODE:
            CB_LEDSTR_SPI0_ReceiveData(buf, len);
            break;
        case SPI1_MODE:
            CB_LEDSTR_SPI1_ReceiveData(buf, len);
            break;
    }
}
void CB_LEDSTR_ReadEnd(U8 mode)
{
    switch(mode) {
        default:
        case OTP_MODE:
            break; // do nothing
        case SPI_MODE:
 #if _SPI_MODULE       
            SPI_FastReadStop();
 #endif          
            break;
        case SPI1_MODE:
#if (_SPI1_MODULE && _SPI1_USE_FLASH)      
            SPI1_FastReadStop();
 #endif            
            break;
    }
}
#pragma GCC optimize ("O0")

#if _EF_SERIES
//====================================================================================
//  32MHz wait one state code pattern 
//====================================================================================
#if OPTION_HIGH_FREQ_CLK==32000000
void RGB_Pattern(GPIO_TypeDef *gpiox,U32 code)
{
		__asm__(".align 4\t\n"
			  "movpi45 $r10,#0x17\t\n"					//r10=23
			  "movi55 $r8,#0x1\t\n"    
			  "mov55 $r9,%[IO_L]\t\n"
			  "lwi333 $r6,[%0+#0x4]\t\n"
			  "j8 check_data\t\n"
			  "code0:\t\n"
			  "or $r6,$r6,%[IO_H]\t\n"
			  "swi333 $r6,[%0+#0x4]\t\n"
			  "subi45 $r10,#0x1\t\n"
			  "and $r6,$r6,$r9\t\n"
//		T0H		     
#if	_LEDSTR_CODE_0>=1
			  "nop\t\n"
			  "nop\t\n" 
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0>=2
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0>=3
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0>=4
			  "nop\t\n"
			  "nop\t\n"
#endif				 
			  "swi333 $r6,[%0+#0x4]\t\n"
			  "nop16\t\n"
//		T0L	 
#if _LEDSTR_CODE_0<=4
		      "nop\t\n"	
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0<=3
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0<=2
			  "nop\t\n"			  
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0<=1
			  "nop\t\n"
			  "nop\t\n"
#endif			 
			  "beqc $r10,#-1,end\t\n"				//r10=0,go End
			  "check_data:\t\n"
			  "sll $r7,$r8,$r10\t\n"
			  "and $r7,$r7,%1\t\n"
			  "output_code:\t\n"
			  "beqz $r7,code0\t\n"					//r7 to output code0 or code1
			  "code1:\t\n"
			  "or $r6,$r6,%[IO_H]\t\n"
			  "swi333 $r6,[%0+#0x4]\t\n"
			  "subi45 $r10,#0x1\t\n"	
			  "and $r6,$r6,$r9\t\n"
//		T1H	
#if	_LEDSTR_CODE_1>=1
		  	  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"	
			  "nop\t\n"			  
#endif				 
#if _LEDSTR_CODE_1>=2
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1>=3
			  "nop\t\n"
			  "nop\t\n"	
#endif				 
#if _LEDSTR_CODE_1>=4			  

			  "nop\t\n"	
			  "nop\t\n" 	
#endif
			  "sll $r7,$r8,$r10\t\n"
			  "and33 $r7,%1\t\n"
			  "swi333 $r6,[%0+#0x4]\t\n"
//		T1L	
#if	_LEDSTR_CODE_1<=4
//			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1<=3
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1<=2
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1<=1
			  "nop\t\n"
			  "nop\t\n"
#endif
			  "bnec $r10,#-1,output_code\t\n"	
			  "end:\t\n"
			  :
		      :"r"(gpiox),"r"(code),[IO_H] "r" (IO_H) ,[IO_L] "r" (IO_L)
		      :"$r6", "$r7", "$r8", "$r9", "$r10"
		     );
}
//====================================================================================
//  40MHz wait one state code pattern 
//====================================================================================
#elif	OPTION_HIGH_FREQ_CLK==40000000
void RGB_Pattern(GPIO_TypeDef *gpiox,U32 code)
{
		__asm__(".align 4\t\n"
			  "movpi45 $r10,#0x17\t\n"					//r10=23
			  "movi55 $r8,#0x1\t\n"    
			  "mov55 $r9,%[IO_L]\t\n"
			  "lwi333 $r6,[%0+#0x4]\t\n"
			  "j8 check_data\t\n"
			  "code0:\t\n"
			  "or $r6,$r6,%[IO_H]\t\n"
			  "swi333 $r6,[%0+#0x4]\t\n"
			  "subi45 $r10,#0x1\t\n"
			  "and $r6,$r6,$r9\t\n"
//		T0H		     
#if	_LEDSTR_CODE_0>=1
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"	
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0>=2
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0>=3
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0>=4
			  "nop\t\n"
			  "nop\t\n"
#endif				 
			  "swi333 $r6,[%0+#0x4]\t\n"
			  "nop16\t\n"
//		T0L	 
#if _LEDSTR_CODE_0<=4
		      "nop\t\n"	
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"			  
#endif				 
#if _LEDSTR_CODE_0<=3
			  "nop\t\n"
			  "nop\t\n"			  
#endif				 
#if _LEDSTR_CODE_0<=2
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0<=1			  
			  "nop\t\n"
			  "nop\t\n"
#endif			 
			  "beqc $r10,#-1,end\t\n"				//r10=0,go End
			  "check_data:\t\n"
			  "sll $r7,$r8,$r10\t\n"
			  "and $r7,$r7,%1\t\n"
			  "output_code:\t\n"
			  "beqz $r7,code0\t\n"					//r7 to output code0 or code1
			  "code1:\t\n"
			  "or $r6,$r6,%[IO_H]\t\n"
			  "swi333 $r6,[%0+#0x4]\t\n"
			  "subi45 $r10,#0x1\t\n"	
			  "and $r6,$r6,$r9\t\n"
//		T1H	
#if	_LEDSTR_CODE_1>=1
		  	  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"	
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1>=2
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1>=3
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1>=4
			  "nop\t\n"
			  "nop\t\n" 
#endif
			  "sll $r7,$r8,$r10\t\n"
			  "and33 $r7,%1\t\n"
			  "swi333 $r6,[%0+#0x4]\t\n"
//		T1L	
#if	_LEDSTR_CODE_1<=4
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1<=3
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1<=2
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1<=1
			  "nop\t\n"
#endif
			  "bnec $r10,#-1,output_code\t\n"	
			  "end:\t\n"
			  :
		      :"r"(gpiox),"r"(code),[IO_H] "r" (IO_H) ,[IO_L] "r" (IO_L)
		      :"$r6", "$r7", "$r8", "$r9", "$r10"
		     );
}
//====================================================================================
//  48MHz wait one state code pattern 
//====================================================================================
#elif	OPTION_HIGH_FREQ_CLK==48000000
void RGB_Pattern(GPIO_TypeDef *gpiox,U32 code)
{
		__asm__(".align 4\t\n"
			  "movpi45 $r10,#0x17\t\n"					//r10=23
			  "movi55 $r8,#0x1\t\n"    
			  "mov55 $r9,%[IO_L]\t\n"
			  "lwi333 $r6,[%0+#0x4]\t\n"
			  "j8 check_data\t\n"
			  "code0:\t\n"
			  "or $r6,$r6,%[IO_H]\t\n"
			  "swi333 $r6,[%0+#0x4]\t\n"
			  "subi45 $r10,#0x1\t\n"
			  "and $r6,$r6,$r9\t\n"
//		T0H		     
#if	_LEDSTR_CODE_0>=1
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"	
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0>=2
			  "nop\t\n"
			  "nop\t\n"

#endif				 
#if _LEDSTR_CODE_0>=3
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0>=4
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
			  "swi333 $r6,[%0+#0x4]\t\n"
			  "nop16\t\n"
//		T0L	 
#if _LEDSTR_CODE_0<=4
		      "nop\t\n"	
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0<=3
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0<=2
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0<=1
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"	
#endif			 
			  "beqc $r10,#-1,end\t\n"				//r10=0,go End
			  "check_data:\t\n"
			  "sll $r7,$r8,$r10\t\n"
			  "and $r7,$r7,%1\t\n"
			  "output_code:\t\n"
			  "beqz $r7,code0\t\n"					//r7 to output code0 or code1
			  "code1:\t\n"
			  "or $r6,$r6,%[IO_H]\t\n"
			  "swi333 $r6,[%0+#0x4]\t\n"
			  "subi45 $r10,#0x1\t\n"	
			  "and $r6,$r6,$r9\t\n"
//		T1H	
#if	_LEDSTR_CODE_1>=1
//		  	  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"	
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1>=2
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1>=3
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1>=4
			  "nop\t\n"
			  "nop\t\n" 
			  "nop\t\n" 	 
#endif
			  "sll $r7,$r8,$r10\t\n"
			  "and33 $r7,%1\t\n"
			  "swi333 $r6,[%0+#0x4]\t\n"
//		T1L	
#if	_LEDSTR_CODE_1<=4
			  "nop\t\n"
			  "nop\t\n"
//			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1<=3
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1<=2
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1<=1
			  "nop\t\n"
			  "nop\t\n"
#endif
			  "bnec $r10,#-1,output_code\t\n"	
			  "end:\t\n"
			  :
		      :"r"(gpiox),"r"(code),[IO_H] "r" (IO_H) ,[IO_L] "r" (IO_L)
		      :"$r6", "$r7", "$r8", "$r9", "$r10"
		     );
}

#else
#error	"LEDSTR only support high clock frequency 32MHz/40MHz/48MHz!"
#endif

#else

//====================================================================================
//  16MHz code pattern
//====================================================================================
#if OPTION_HIGH_FREQ_CLK==16000000
void RGB_Pattern(GPIO_TypeDef *gpiox,U32 code)
{
		__asm__(".align 4\t\n"
			  "movpi45 $r10,#0x17\t\n"					//r10=23
			  "movi55 $r8,#0x1\t\n"    
			  "mov55 $r9,%[IO_L]\t\n"
			  "lwi333 $r6,[%0+#0x4]\t\n"
			  "j8 check_data\t\n"
			  "code0:\t\n"
			  "or $r6,$r6,%[IO_H]\t\n"
			  "swi333 $r6,[%0+#0x4]\t\n"
			  "subi45 $r10,#0x1\t\n"
			  "and $r6,$r6,$r9\t\n"
//		T0H		     
#if	_LEDSTR_CODE_0>=1
			  "nop\t\n"
			  "nop\t\n" 
#endif				 
#if _LEDSTR_CODE_0>=2
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0>=3
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0>=4
			  "nop\t\n"
			  "nop\t\n"
#endif				 
			  "swi333 $r6,[%0+#0x4]\t\n"
			  "nop16\t\n"
//		T0L	 
#if _LEDSTR_CODE_0<=4
		      "nop\t\n"	
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0<=3
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0<=2
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0<=1
			  "nop\t\n"
			  "nop\t\n"
#endif			 
			  "beqc $r10,#-1,end\t\n"				//r10=0,go End
			  "check_data:\t\n"
			  "sll $r7,$r8,$r10\t\n"
			  "and $r7,$r7,%1\t\n"
			  "output_code:\t\n"
			  "beqz $r7,code0\t\n"					//r7 to output code0 or code1
			  "code1:\t\n"
			  "or $r6,$r6,%[IO_H]\t\n"
			  "swi333 $r6,[%0+#0x4]\t\n"
			  "subi45 $r10,#0x1\t\n"	
			  "and $r6,$r6,$r9\t\n"
//		T1H	
#if	_LEDSTR_CODE_1>=1
		  	  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"	
#endif				 
#if _LEDSTR_CODE_1>=2
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1>=3
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1>=4
			  "nop\t\n" 	
#endif
			  "sll $r7,$r8,$r10\t\n"
			  "and33 $r7,%1\t\n"
			  "swi333 $r6,[%0+#0x4]\t\n"
//		T1L	
#if	_LEDSTR_CODE_1<=4
//			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1<=3
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1<=2
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1<=1
			  "nop\t\n"
			  "nop\t\n"
#endif
			  "bnec $r10,#-1,output_code\t\n"	
			  "end:\t\n"
			  :
		      :"r"(gpiox),"r"(code),[IO_H] "r" (IO_H) ,[IO_L] "r" (IO_L)
		      :"$r6", "$r7", "$r8", "$r9", "$r10"
		     );
}
//====================================================================================
//  24MHz code pattern
//====================================================================================
#elif OPTION_HIGH_FREQ_CLK==24000000
void RGB_Pattern(GPIO_TypeDef *gpiox,U32 code)
{
		__asm__(".align 4\t\n"
			  "movpi45 $r10,#0x17\t\n"					//r10=23
			  "movi55 $r8,#0x1\t\n"    
			  "mov55 $r9,%[IO_L]\t\n"
			  "lwi333 $r6,[%0+#0x4]\t\n"
			  "j8 check_data\t\n"
			  "code0:\t\n"
			  "or $r6,$r6,%[IO_H]\t\n"
			  "swi333 $r6,[%0+#0x4]\t\n"
			  "subi45 $r10,#0x1\t\n"
			  "and $r6,$r6,$r9\t\n"
//		T0H		     
#if	_LEDSTR_CODE_0>=1
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"	
			  "nop\t\n" 
#endif				 
#if _LEDSTR_CODE_0>=2
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0>=3
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0>=4
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
			  "swi333 $r6,[%0+#0x4]\t\n"
			  "nop16\t\n"
//		T0L	 
#if _LEDSTR_CODE_0<=4
		      "nop\t\n"	
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0<=3
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0<=2
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0<=1
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"	
#endif			 
			  "beqc $r10,#-1,end\t\n"				//r10=0,go End
			  "check_data:\t\n"
			  "sll $r7,$r8,$r10\t\n"
			  "and $r7,$r7,%1\t\n"
			  "output_code:\t\n"
			  "beqz $r7,code0\t\n"					//r7 to output code0 or code1
			  "code1:\t\n"
			  "or $r6,$r6,%[IO_H]\t\n"
			  "swi333 $r6,[%0+#0x4]\t\n"
			  "subi45 $r10,#0x1\t\n"	
			  "and $r6,$r6,$r9\t\n"
//		T1H	
#if	_LEDSTR_CODE_1>=1
		  	  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"	
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1>=2
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1>=3
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1>=4
			  "nop\t\n"
			  "nop\t\n" 
			  "nop\t\n" 	
#endif
			  "sll $r7,$r8,$r10\t\n"
			  "and33 $r7,%1\t\n"
			  "swi333 $r6,[%0+#0x4]\t\n"
//		T1L	
#if	_LEDSTR_CODE_1<=4
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1<=3
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1<=2
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1<=1
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif
			  "bnec $r10,#-1,output_code\t\n"	
			  "end:\t\n"
			  :
		      :"r"(gpiox),"r"(code),[IO_H] "r" (IO_H) ,[IO_L] "r" (IO_L)
		      :"$r6", "$r7", "$r8", "$r9", "$r10"
		     );
}
//====================================================================================
//  32MHz code pattern 
//====================================================================================

#elif OPTION_HIGH_FREQ_CLK==32000000

void RGB_Pattern(GPIO_TypeDef *gpiox,U32 code)
{
		__asm__(".align 4\t\n"
			  "movpi45 $r10,#0x17\t\n"					//r10=23
			  "movi55 $r8,#0x1\t\n"    
			  "mov55 $r9,%[IO_L]\t\n"
			  "lwi333 $r6,[%0+#0x4]\t\n"
			  "j8 check_data\t\n"
			  "code0:\t\n"
			  "or $r6,$r6,%[IO_H]\t\n"
			  "swi333 $r6,[%0+#0x4]\t\n"
			  "subi45 $r10,#0x1\t\n"
			  "and $r6,$r6,$r9\t\n"
//		T0H		     
#if	_LEDSTR_CODE_0>=1		     
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"	
			  "nop\t\n" 
			  "nop\t\n"
			  "nop\t\n"	
			  "nop\t\n" 
#endif				 
#if _LEDSTR_CODE_0>=2		    
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0>=3		    
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0>=4		  
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
			  "swi333 $r6,[%0+#0x4]\t\n"
			  "nop16\t\n"
//		T0L	 
#if _LEDSTR_CODE_0<=4		  
		      "nop\t\n"			  
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"	
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0<=3
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0<=2
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_0<=1
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"	
			  "nop\t\n"	
#endif			 
			  "beqc $r10,#-1,end\t\n"				//r10=0,go End
			  "check_data:\t\n"
			  "sll $r7,$r8,$r10\t\n"
			  "and $r7,$r7,%1\t\n"
			  "output_code:\t\n"
			  "beqz $r7,code0\t\n"					//r7 to output code0 or code1
			  "code1:\t\n"
			  "or $r6,$r6,%[IO_H]\t\n"
			  "swi333 $r6,[%0+#0x4]\t\n"
			  "subi45 $r10,#0x1\t\n"	
			  "and $r6,$r6,$r9\t\n"
//		T1H	
#if	_LEDSTR_CODE_1>=1
		  	  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"	
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"	
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1>=2
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1>=3
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1>=4
			  "nop\t\n"
			  "nop\t\n" 
			  "nop\t\n" 	
#endif
			  "sll $r7,$r8,$r10\t\n"
			  "and33 $r7,%1\t\n"
			  "swi333 $r6,[%0+#0x4]\t\n"
//		T1L	
#if	_LEDSTR_CODE_1<=4
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"	

#endif				 
#if _LEDSTR_CODE_1<=3
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1<=2
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif				 
#if _LEDSTR_CODE_1<=1
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
			  "nop\t\n"
#endif
			  "bnec $r10,#-1,output_code\t\n"	
			  "end:\t\n"
 			  :
		      :"r"(gpiox),"r"(code),[IO_H] "r" (IO_H) ,[IO_L] "r" (IO_L)
		      :"$r6", "$r7", "$r8", "$r9", "$r10"
		     );
}

#else
#error	"LEDSTR only support high clock frequency 16MHz/24MHz/32MHz!"
#endif
#endif

#endif