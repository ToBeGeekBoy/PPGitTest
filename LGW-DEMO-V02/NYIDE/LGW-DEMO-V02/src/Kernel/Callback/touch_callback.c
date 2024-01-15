
//============================================
#include "nx1_lib.h"
#include "include.h"
#include "nx1_wdt.h"
#include "nx1_smu.h"
#include "nx1_pwm.h"
#include "nx1_timer.h"
#include "nx1_intc.h"
#include "nx1_rtc.h"
#include "nx1_gpio.h"
#include "TouchTab.h"

#if _TOUCHKEY_MODULE
//------------------------------------------------------------------//
// Touch Initial CB
// Parameter: 
//			NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_TouchInit()
{
	U8 i;
	for(i=0;i<C_TouchMaxPAD;i++)
	{		
		Touch_CheckPAD(i);	
	}
	Touch_SleepTime(C_TouchWakeTime);							//	Initial Touch Wake Down Count Time.
	Touch_SetWakeUpMaxPAD(C_TouchWakeUpMaxPAD);					//	Initial Touch WakeUp PAD.
	//==========================================================//	ENABLE PWM CAPUTRE
#if _EF_SERIES
	SMU_PeriphClkCmd(SMU_PERIPH_TMR,ENABLE);			
	TIMER0->INT_Ctrl = (0x1<<1);//CaptureIntCmd(CAP_RISING_FALLING,ENABLE);	
	TIMER0->Ctrl &= ~C_SWT_Stop_Trgs;//init rising trg, set 0
	TIMER_Init(TIMER0,0xffff);
	TIMER_Cmd(TIMER0,DISABLE);  		
	INTC_IrqCmd(IRQ_BIT_TMR0,DISABLE);	
	TIMER0->Ctrl &= ~C_SWT_En;//CaptureCmd(DISABLE);
#else//OTP Series
	SMU_PeriphClkCmd(SMU_PERIPH_PWM,ENABLE);			
	PWM_CaptureIntCmd(CAP_RISING_FALLING,DISABLE);	
	TIMER_Init(PWMATIMER,TIMER_CLKDIV_1,0xffff);	 
	TIMER_Cmd(PWMATIMER,DISABLE);  		
	INTC_IrqCmd(IRQ_BIT_CAPTURE,DISABLE);	
	PWM_CaptureCmd(DISABLE);
#endif
	//==========================================================
	
	Touch_SetSensitivityLevel(TOUCH_PAD0,TOUCH_SENSITIVE_LV0);	// PAD0  
	Touch_SetSensitivityLevel(TOUCH_PAD1,TOUCH_SENSITIVE_LV0);
	Touch_SetSensitivityLevel(TOUCH_PAD2,TOUCH_SENSITIVE_LV0);
	Touch_SetSensitivityLevel(TOUCH_PAD3,TOUCH_SENSITIVE_LV0);
	Touch_SetSensitivityLevel(TOUCH_PAD4,TOUCH_SENSITIVE_LV0);
	Touch_SetSensitivityLevel(TOUCH_PAD5,TOUCH_SENSITIVE_LV0);
	Touch_SetSensitivityLevel(TOUCH_PAD6,TOUCH_SENSITIVE_LV0);
	Touch_SetSensitivityLevel(TOUCH_PAD7,TOUCH_SENSITIVE_LV0);
	Touch_SetSensitivityLevel(TOUCH_PAD8,TOUCH_SENSITIVE_LV0);
	Touch_SetSensitivityLevel(TOUCH_PAD9,TOUCH_SENSITIVE_LV0);
	Touch_SetSensitivityLevel(TOUCH_PAD10,TOUCH_SENSITIVE_LV0);
	Touch_SetSensitivityLevel(TOUCH_PAD11,TOUCH_SENSITIVE_LV0);
	Touch_SetSensitivityLevel(TOUCH_PAD12,TOUCH_SENSITIVE_LV0);
	Touch_SetSensitivityLevel(TOUCH_PAD13,TOUCH_SENSITIVE_LV0);
	Touch_SetSensitivityLevel(TOUCH_PAD14,TOUCH_SENSITIVE_LV0);
	Touch_SetSensitivityLevel(TOUCH_PAD15,TOUCH_SENSITIVE_LV0);
}

//------------------------------------------------------------------//
// Touch Press CB
// Parameter: 
//			u8KeyValue:TOUCH_PAD0 ~ TOUCH_PAD15
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_TouchPress(U8 u8KeyValue)
{
	Touch_SetEnforceTime(C_TouchEnforceTime);		// Set Enforce Calibration Downcount time = C_TouchEnforceTime
#if _QCODE_BUILD
    QC_TouchKey_PushEvent((0x1<<u8KeyValue),0);
#endif
}

//------------------------------------------------------------------//
// Touch Release CB
// Parameter: 
//			u8KeyValue:TOUCH_PAD0 ~ TOUCH_PAD15
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_TouchRelease(U8 u8KeyValue)
{
	Touch_SleepTime(C_TouchWakeTime);
#if _QCODE_BUILD
    QC_TouchKey_PushEvent(0,(0x1<<u8KeyValue));
#endif
}

//------------------------------------------------------------------//
// Touch Enforce Calibration CB
// Parameter: 
//			u8PADCount:TOUCH_PAD0 ~ TOUCH_PAD15
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_EnforceCalibration(U8 u8PADCount)
{
	CB_TouchRelease(u8PADCount);

}

//------------------------------------------------------------------//
// Touch Calibration CB
// Parameter: 
//			u8PADCount:TOUCH_PAD0 ~ TOUCH_PAD15
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_Calibration(U8 u8PADCount)
{

}
//------------------------------------------------------------------//
// Touch Start CB
// Parameter: 
//			u8ScanPADIdx:Scan PAD Index
//			u8MaxConvertTimes:Max Convert times 
//			p_u32TouchCaptureValue:pointer ofCapture value temp
// return value:
//          None
//------------------------------------------------------------------// 
void CB_Touch_Start(U8 u8ScanPADIdx, U8 u8MaxConvertTimes, U32* u32TouchCaptureValue)
{
	Touch_Start(u8ScanPADIdx, u8MaxConvertTimes, u32TouchCaptureValue);
}
//===================================================
#endif