#include "nx1_lib.h"
#include "include.h"
#include "nx1_timer.h"
#include "nx1_pwm.h"
#include "nx1_gpio.h"
#include "nx1_intc.h"
#include "TouchTab.h"
#include <string.h>

#if _TOUCHKEY_MODULE
static	U8		u8nowScanPADIdx=0,u8nowMaxConvertTimes=0,u8ConvertCount=0;
static	U16		u16InPinBit=0,u16OutPinBit=0;
static	U32* 	pTimerValueSum;
static	GPIO_TypeDef *p_PADInPort, *p_PADOutPort; 

//------------------------------------------------------------------//
// TOUCH Service Loop
// Parameter: 
//			NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void TOUCH_ServiceLoop()
{
	if(Touch_GetPADStatus()==0)
	{
		Touch_ScanKey();
	}
}
//------------------------------------------------------------------//
// TOUCH CAPTURE(Use PWMA/TIMER0 Capture Module)
// Parameter: 
//			NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void TOUCH_CAPTURE(void)
{
	GIE_DISABLE();
#if _EF_SERIES
	do {
		p_PADInPort->Data ^=   u16InPinBit;	
		p_PADInPort->DIR  &= ~(u16InPinBit);			//set touch InPin as output high or low	
		if ((TIMER0->Ctrl & C_SWT_Stop_From_1_To_0)) {	//is falling?
			*pTimerValueSum+=((0xFFFF)-(TIMER0->Data>>16));
			TIMER0->Ctrl = 							\
				(TIMER0->Ctrl & (~C_SWT_Stop_Trgs))	\
				| (C_SWT_Stop_From_0_To_1) ;			//CaptureIntCmd(CAP_RISING,ENABLE);
			u8ConvertCount++;	
		} else {
			*pTimerValueSum+=((0xFFFF)-(TIMER0->Data>>16));
			TIMER0->Ctrl = 							\
				(TIMER0->Ctrl & (~C_SWT_Stop_Trgs))	\
				| (C_SWT_Stop_From_1_To_0) ;			//CaptureIntCmd(CAP_FALLING,ENABLE);
		}
		TIMER0->Ctrl &= (~C_TMR_En);					//TIMER_Cmd(PWMATIMER,DISABLE);
		TIMER0->Flag = (C_SWT_Flag|C_TMR_Flag);			// clear capture Flag & timer flag 
		if(u8ConvertCount >= u8nowMaxConvertTimes)		//final cycle finish check
		{
			Touch_SetConvertOK();
			p_PADOutPort->Data &= ~(u16OutPinBit);		
			p_PADOutPort->DIR  &= ~(u16OutPinBit);
			p_PADInPort->Data  &= ~(u16InPinBit);		
			p_PADInPort->DIR   &= ~(u16InPinBit);		//init GPIO state
			__nds32__mtsr(												\
				(__nds32__mfsr(NDS32_SR_INT_MASK2)&(~IRQ_BIT_TMR0))		\
				, NDS32_SR_INT_MASK2);					//INTC_IrqCmd(IRQ_BIT_TMR0,DISABLE);
			TIMER0->INT_Ctrl = 0;						//CaptureIntCmd(CAP_RISING_FALLING,DISABLE);
			TIMER0->Ctrl &= (~C_SWT_En);				//CaptureCmd(DISABLE);
			break;
		}
		TIMER0->Data = 0xffff;
		p_PADOutPort->Data ^=(u16OutPinBit);			// set touch output out	
		p_PADInPort->DIR |= u16InPinBit;				// set touch in as input 
		TIMER0->Ctrl |= (C_TMR_En|C_SWT_Trig);			//TIMER_Cmd(TIMER0,ENABLE);
	} while(0);
#else//OTP Series
	do {
		p_PADInPort->Data ^=   u16InPinBit;
		p_PADInPort->DIR  &= ~(u16InPinBit);			//set touch InPin as output high or low	
		if (P_PWMA->CAP_Flag & CAP_FALLING) {
			*pTimerValueSum+=((0xFFFF)-(P_PWMA->CAP_Data&0xFFFF));
			P_PWMA->CAP_INT_Ctrl = CAP_RISING_FALLING;	//CaptureIntCmd(CAP_RISING_FALLING,ENABLE);
			u8ConvertCount++;	
		} else {
			*pTimerValueSum+=((0xFFFF)-(P_PWMA->CAP_Data>>16));
			P_PWMA->CAP_INT_Ctrl = CAP_RISING_FALLING;	//CaptureIntCmd(CAP_RISING_FALLING,ENABLE);
		}
		PWMATIMER->Ctrl &= (~C_TMR_En);					//TIMER_Cmd(PWMATIMER,DISABLE);
		P_PWMA->CAP_Flag = (C_PWMA_CAP_Rising_Flag|C_PWMA_CAP_Falling_Flag);// clear capture Flag
		if(u8ConvertCount >= u8nowMaxConvertTimes)		//final cycle finish check
		{
			Touch_SetConvertOK();
			p_PADOutPort->Data &= ~(u16OutPinBit);		
			p_PADOutPort->DIR  &= ~(u16OutPinBit);
			p_PADInPort->Data  &= ~(u16InPinBit);		
			p_PADInPort->DIR   &= ~(u16InPinBit);		//init GPIO state
			__nds32__mtsr(												\
				(__nds32__mfsr(NDS32_SR_INT_MASK2)&(~IRQ_BIT_CAPTURE))	\
				, NDS32_SR_INT_MASK2);					//INTC_IrqCmd(IRQ_BIT_CAPTURE,DISABLE);
			P_PWMA->CAP_INT_Ctrl = CAP_RISING_FALLING;			//CaptureIntCmd(CAP_RISING_FALLING,DISABLE);
			P_PWMA->TMR_Ctrl &= (~C_PWMA_CAP_En);		//PWM_CaptureCmd(DISABLE);
			break;
		}
		PWMATIMER->Data = 0xffff;
		p_PADOutPort->Data ^=(u16OutPinBit);			// set touch output out	
		p_PADInPort->DIR |= u16InPinBit;				// set touch in as input 
		PWMATIMER->Ctrl |= C_TMR_En;					//TIMER_Cmd(PWMATIMER,ENABLE);
	} while(0);
#endif
	GIE_ENABLE();

}

//------------------------------------------------------------------//
// Touch Start
// Parameter: 
//			u8ScanPADIdx:Scan PAD Index
//			u8MaxConvertTimes:Max Convert times 
//			p_u32TouchCaptureValue:pointer ofCapture value temp
// return value:
//          NONE
//------------------------------------------------------------------// 
void  Touch_Start(U8 u8ScanPADIdx, U8 u8MaxConvertTimes, U32* pTouchCaptureTemp)
{	
	u8nowScanPADIdx = u8ScanPADIdx;
	u8nowMaxConvertTimes = u8MaxConvertTimes;
	pTimerValueSum = pTouchCaptureTemp;

	*pTimerValueSum=0;
	u8ConvertCount=0;	
	GIE_DISABLE();
    
	U8 _inPin = Touch_GetPADInput(u8nowScanPADIdx);
	p_PADInPort = GPIO_GetPort(_inPin);
    _inPin = GPIO_GetPin(_inPin);
    u16InPinBit = 1<<(_inPin);
    
    U8 _outPin = Touch_GetPADOutput(u8nowScanPADIdx);
    p_PADOutPort = GPIO_GetPort(_outPin);
    _outPin = GPIO_GetPin(_outPin);
	u16OutPinBit= 1<<(_outPin);
    
#if _EF_SERIES
	__nds32__mtsr(
		(__nds32__mfsr(NDS32_SR_INT_MASK2)|IRQ_BIT_TMR0)
		, NDS32_SR_INT_MASK2);				//INTC_IrqCmd(IRQ_BIT_TMR0,ENABLE);
	//Use PA trg source;	
	//CapturePinSelect(_inPin);
	//init rising trg, set 0
	//CaptureIntCmd(CAP_RISING,ENABLE);
	//TIMER_SetClkSrc(TIMER0,TIMER_CLKSRC_HICLK);
	TIMER0->Ctrl =  ( C_SWT_Src_Ext 
					| (_inPin<<20)
					| C_SWT_Stop_From_0_To_1
					| C_SWT_En
					| C_TMR_CLK_Src_HiCLK
					| C_IR_Stop_0
					| C_IR_Dis
					| C_TMR_Auto_Reload_Dis
					| C_TMR_Dis
					);
	TIMER0->INT_Ctrl = C_SWT_IEn;
	TIMER0->Flag = (C_SWT_Flag|C_TMR_Flag);	// clear capture Flag & timer flag 
    TIMER0->Data = 0xffff;
	p_PADOutPort->Data |= u16OutPinBit;		// set touch output out	hi		
	p_PADInPort->DIR |= u16InPinBit;		// set touch in as input 
	TIMER0->Ctrl |= (C_TMR_En|C_SWT_Trig);	//TIMER_Cmd(TIMER0,ENABLE);
#else//OTP Series
	__nds32__mtsr(
		(__nds32__mfsr(NDS32_SR_INT_MASK2)|IRQ_BIT_CAPTURE)
		, NDS32_SR_INT_MASK2);	//INTC_IrqCmd(IRQ_BIT_CAPTURE,ENABLE);
	//PWM_CapturePinSelect(_inPin);
	//PWM_CaptureCmd(ENABLE);
	P_PWMA->TMR_Ctrl =  ( (_inPin<<16)
						| C_PWM_CLK_Div_1
						| C_PWMA_CAP_En
						| C_PWM_CPLM_Mode_Dis
						| C_PWM_Data_Reload_Start
						| C_PWM_Dis
						);
	P_PWMA->CAP_INT_Ctrl = CAP_RISING_FALLING;//CaptureIntCmd(CAP_RISING_FALLING,ENABLE);
	P_PWMA->CAP_Flag=(C_PWMA_CAP_Rising_Flag|C_PWMA_CAP_Falling_Flag);	// clear capture Flag 
    PWMATIMER->Data = 0xffff;
	p_PADOutPort->Data |= u16OutPinBit;		// set touch output out	hi		
	p_PADInPort->DIR |= u16InPinBit;		// set touch in as input 
	PWMATIMER->Ctrl |= C_TMR_En;//TIMER_Cmd(PWMATIMER,ENABLE);
#endif
	GIE_ENABLE();
}
//------------------------------------------------------------------//
// Touch Check PAD
// Parameter: 
//			u8ScanPADIdx:TOUCH_PAD0 ~ TOUCH_PAD15
// return value:
//          NONE
//------------------------------------------------------------------// 
void Touch_CheckPAD(U8 u8ScanPADIdx)
{
    U8 _inPin = Touch_GetPADInput(u8ScanPADIdx);
	p_PADInPort = GPIO_GetPort(_inPin);
	if(p_PADInPort!=GPIOA) {
		Touch_SetError();
		return;
	}
    _inPin = GPIO_GetPin(_inPin);
    
    U8 _outPin = Touch_GetPADOutput(u8ScanPADIdx);
	p_PADOutPort = GPIO_GetPort(_outPin);
	_outPin = GPIO_GetPin(_outPin);
    
    GPIO_Init(p_PADInPort, _inPin, GPIO_MODE_IN_FLOAT);
    GPIO_Init(p_PADOutPort, _outPin, GPIO_MODE_OUT_HI);

	DelayLoop2(320);

	if(GPIO_Read(p_PADInPort, _inPin))      //is input hi?
	{
		GPIO_Init(p_PADOutPort, _outPin, GPIO_MODE_OUT_LOW);
		DelayLoop2(320);
		if(!GPIO_Read(p_PADInPort, _inPin)) //is input lo?
		{		
            GPIO_Init(p_PADInPort, _inPin, GPIO_MODE_OUT_LOW);
            GPIO_Init(p_PADOutPort, _outPin, GPIO_MODE_OUT_LOW);
			return ;
		}
	}		
	Touch_SetError();
}

//------------------------------------------------------------------//
// Touch Hardware Initial
// Parameter: 
//			NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
static void _hardwareInitial()
{
	GIE_DISABLE();
	U8 i;
	for(i=0;i<C_TouchMaxPAD;++i) {
        U8 _inPin = Touch_GetPADInput(i);
        U8 _outPin = Touch_GetPADOutput(i);

		GPIO_Init(GPIO_GetPort(_inPin),GPIO_GetPin(_inPin),GPIO_MODE_OUT_LOW);
		GPIO_Init(GPIO_GetPort(_outPin),GPIO_GetPin(_outPin),GPIO_MODE_OUT_LOW);
	}
#if _EF_SERIES
	TIMER_IntCmd(TIMER0,DISABLE);
	TIMER_Cmd(TIMER0,DISABLE);//reset TMR0
	TIMER0->Flag = (C_SWT_Flag|C_TMR_Flag);
	INTC_IrqCmd(IRQ_BIT_TMR0,DISABLE);
	INTC_ClrPending(IRQ_BIT_TMR0);
#else
	PWM_CaptureIntCmd(CAP_RISING_FALLING,DISABLE);
	TIMER_Cmd(PWMATIMER,DISABLE);//reset PWMA
	P_PWMA->CAP_Flag = (C_PWMA_CAP_Rising_Flag|C_PWMA_CAP_Falling_Flag);
	INTC_IrqCmd(IRQ_BIT_CAPTURE,DISABLE);
	INTC_ClrPending(IRQ_BIT_CAPTURE);
#endif
	GIE_ENABLE();
}
//------------------------------------------------------------------//
// Touch Initial
// Parameter: 
//			NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void Touch_Initial()
{
	//step1. init Hardware
	_hardwareInitial();
	//step2, init RAM
	u8nowScanPADIdx=u8nowMaxConvertTimes=u8ConvertCount=0;
	u16InPinBit=u16OutPinBit=0;
	pTimerValueSum=NULL;
	p_PADInPort=NULL;
	p_PADOutPort=NULL;
	//step3, init LibraryRAM
	Touch_LibraryInitial();
}

//------------------------------------------------------------------//
// Touch Initial
// Parameter: 
//			NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void Touch_DISABLE()
{
	_hardwareInitial();
	Touch_LibraryStop();
}
#endif