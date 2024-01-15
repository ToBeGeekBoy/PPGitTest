#include <string.h>
#include "include.h"
#include "nx1_lib.h"
#include "nx1_smu.h"
#include "nx1_gpio.h"
#include "nx1_pwm.h"
#include "nx1_timer.h"

#if		(_SW_PWM_MODULE)
static inline U8 CB_SW_PWM_getMaxPin(void)
{
    return GPIO_MAX;
}

static inline void CB_SW_PWM_PinBusy(const U8 _pin, const U8 BusyLevel)
{
    if(_pin > CB_SW_PWM_getMaxPin()) return;
    U8 _portIdx=GPIO_GetPortIdx(_pin);
    U16 _pinMask = 0x1<<GPIO_GetPin(_pin);
    if(BusyLevel) {
        GPIO_PortList[_portIdx]->Data |= _pinMask;
    } else {
        GPIO_PortList[_portIdx]->Data &= ~_pinMask;
    }
}

static inline void CB_SW_PWM_PinNonBusy(const U8 _pin, const U8 BusyLevel)
{
    if(_pin > CB_SW_PWM_getMaxPin()) return;
    U8 _portIdx=GPIO_GetPortIdx(_pin);
    U16 _pinMask = 0x1<<GPIO_GetPin(_pin);
    if(BusyLevel) {
        GPIO_PortList[_portIdx]->Data &= ~_pinMask;
    } else {
        GPIO_PortList[_portIdx]->Data |= _pinMask;
    }
}

static void CB_SW_PWM_PinInitOutputHi(const U8 _pin)
{
    GPIO_Init(GPIO_GetPort(_pin),GPIO_GetPin(_pin),GPIO_MODE_OUT_HI);
}

static void CB_SW_PWM_PinInitOutputLo(const U8 _pin)
{
    GPIO_Init(GPIO_GetPort(_pin),GPIO_GetPin(_pin),GPIO_MODE_OUT_LOW);
}

static bool CB_SW_PWM_isPinOutputMode(const U8 _pin)
{
    return (GPIO_ReadMode(GPIO_GetPort(_pin),GPIO_GetPin(_pin))==0); //true=dir out
}

static bool CB_SW_PWM_isPinOutputHi(const U8 _pin)
{
    return (GPIO_Read(GPIO_GetPort(_pin),GPIO_GetPin(_pin))!=0); //true=output Hi
}

#ifndef _SW_PWM_USE_PWMA0
#define _SW_PWM_USE_PWMA0   DISABLE
#endif
#ifndef _SW_PWM_USE_PWMA1
#define _SW_PWM_USE_PWMA1   DISABLE
#endif
#ifndef _SW_PWM_USE_PWMA2
#define _SW_PWM_USE_PWMA2   DISABLE
#endif
#ifndef _SW_PWM_USE_PWMA3
#define _SW_PWM_USE_PWMA3   DISABLE
#endif
#ifndef _SW_PWM_USE_PWMB0
#define _SW_PWM_USE_PWMB0   DISABLE
#endif
#ifndef _SW_PWM_USE_PWMB1
#define _SW_PWM_USE_PWMB1   DISABLE
#endif
#ifndef _SW_PWM_USE_PWMB2
#define _SW_PWM_USE_PWMB2   DISABLE
#endif
#ifndef _SW_PWM_USE_PWMB3
#define _SW_PWM_USE_PWMB3   DISABLE
#endif

#define _SW_PWM_HW_EXTENSION    (   (defined(P_PWMA)&&_SW_PWM_USE_PWMA0)    \
                                ||  (defined(P_PWMA)&&_SW_PWM_USE_PWMA1)    \
                                ||  (defined(P_PWMA)&&_SW_PWM_USE_PWMA2)    \
                                ||  (defined(P_PWMA)&&_SW_PWM_USE_PWMA3)    \
                                ||  (defined(P_PWMB)&&_SW_PWM_USE_PWMB0)    \
                                ||  (defined(P_PWMB)&&_SW_PWM_USE_PWMB1)    \
                                ||  (defined(P_PWMB)&&_SW_PWM_USE_PWMB2)    \
                                ||  (defined(P_PWMB)&&_SW_PWM_USE_PWMB3)    )
#if _SW_PWM_HW_EXTENSION
#ifndef _SW_PWM_HW_RESOLUTION
#define _SW_PWM_HW_RESOLUTION         65535//1~65535
#endif
#if (_SW_PWM_HW_RESOLUTION<1)||(_SW_PWM_HW_RESOLUTION>65535)
#error "_SW_PWM_HW_RESOLUTION only support 1~65535."
#endif
#ifdef	_SW_PWM_GAMMA2_CORRECTION
#define	_HW_u16Duty2Tick(ND)			(((SW_PWM_GAMMA2_CORRECTION(ND))*(_SW_PWM_HW_RESOLUTION+1))>>16)  //ND = u16_NowDutyNum
#else
#define	_HW_u16Duty2Tick(ND)			(((ND)*(_SW_PWM_HW_RESOLUTION+1))>>16)                            //ND = u16_NowDutyNum
#endif
typedef enum{
#if	defined(P_PWMA) && _SW_PWM_USE_PWMA0
    _SW_PWM_PWMA0_IDX,
#endif
#if defined(P_PWMA) && _SW_PWM_USE_PWMA1
    _SW_PWM_PWMA1_IDX,
#endif
#if defined(P_PWMA) && _SW_PWM_USE_PWMA2
    _SW_PWM_PWMA2_IDX,
#endif
#if defined(P_PWMA) && _SW_PWM_USE_PWMA3
    _SW_PWM_PWMA3_IDX,
#endif
#if defined(P_PWMB) && _SW_PWM_USE_PWMB0
    _SW_PWM_PWMB0_IDX,
#endif
#if defined(P_PWMB) && _SW_PWM_USE_PWMB1
    _SW_PWM_PWMB1_IDX,
#endif
#if defined(P_PWMB) && _SW_PWM_USE_PWMB2
    _SW_PWM_PWMB2_IDX,
#endif
#if defined(P_PWMB) && _SW_PWM_USE_PWMB3
    _SW_PWM_PWMB3_IDX,
#endif
	num_USABLE_HW_PWM,
    no_USEABLE_HW_PWM=0xFF,
}PWM_SELECT;

typedef const struct {
	PWM_TypeDef*    pwmx;//PWMA, PWMB
    TIMER_TypeDef*  timerx;//PWMATIMER, PWMBTIMER
    volatile U32*   dutyCtl;
}S_PWM_RESOURCE;

static S_PWM_RESOURCE s_PWM_Resource[num_USABLE_HW_PWM] =
{
#if defined(P_PWMA) && _SW_PWM_USE_PWMA0
    {
        .pwmx   =   PWMA,
        .timerx =   PWMATIMER,
        .dutyCtl=   &(PWMA->IO0_Duty),
    },
#endif
#if	defined(P_PWMA) && _SW_PWM_USE_PWMA1
    {
        .pwmx   =   PWMA,
        .timerx =   PWMATIMER,
        .dutyCtl=   &(PWMA->IO1_Duty),
    },
#endif
#if	defined(P_PWMA) && _SW_PWM_USE_PWMA2
    {
        .pwmx   =   PWMA,
        .timerx =   PWMATIMER,
        .dutyCtl=   &(PWMA->IO2_Duty),
    },
#endif
#if	defined(P_PWMA) && _SW_PWM_USE_PWMA3
    {
        .pwmx   =   PWMA,
        .timerx =   PWMATIMER,
        .dutyCtl=   &(PWMA->IO3_Duty),
    },
#endif
#if	defined(P_PWMB) && _SW_PWM_USE_PWMB0
    {
        .pwmx   =   PWMB,
        .timerx =   PWMBTIMER,
        .dutyCtl=   &(PWMB->IO0_Duty),
    },
#endif
#if	defined(P_PWMB) && _SW_PWM_USE_PWMB1
    {
        .pwmx   =   PWMB,
        .timerx =   PWMBTIMER,
        .dutyCtl=   &(PWMB->IO1_Duty),
    },
#endif
#if	defined(P_PWMB) && _SW_PWM_USE_PWMB2
    {
        .pwmx   =   PWMB,
        .timerx =   PWMBTIMER,
        .dutyCtl=   &(PWMB->IO2_Duty),
    },
#endif
#if	defined(P_PWMB) && _SW_PWM_USE_PWMB3
    {
        .pwmx   =   PWMB,
        .timerx =   PWMBTIMER,
        .dutyCtl=   &(PWMB->IO3_Duty),
    },
#endif
};

//------------------------------------------------------------------//
// CB_SW_PWM_getHwPwmIdx(Map of gpio with hwPwmPin)
// Parameter: 
//          _pin: GPIO_PA0~GPIO_MAX, only support hw pwm pin
// return value:
//			Reference PWM_SELECT from 0~(num_USABLE_HW_PWM-1)
//------------------------------------------------------------------//
static U8 CB_SW_PWM_getHwPwmIdx(const U8 _pin)
{
    switch(_pin) {
#if _EF_SERIES
#if defined(P_PWMA) && _SW_PWM_USE_PWMA0
        case GPIO_PA12:
            return _SW_PWM_PWMA0_IDX;
#endif
#if defined(P_PWMA) && _SW_PWM_USE_PWMA1
        case GPIO_PA13:
            return _SW_PWM_PWMA1_IDX;
#endif
#if defined(P_PWMA) && _SW_PWM_USE_PWMA2
        case GPIO_PA14:
            return _SW_PWM_PWMA2_IDX;
#endif
#if defined(P_PWMA) && _SW_PWM_USE_PWMA3
        case GPIO_PA15:
            return _SW_PWM_PWMA3_IDX;
#endif
#if defined(P_PWMB) && _SW_PWM_USE_PWMB0
        case GPIO_PB0:
            return _SW_PWM_PWMB0_IDX;
#endif
#if defined(P_PWMB) && _SW_PWM_USE_PWMB1
        case GPIO_PB1:
            return _SW_PWM_PWMB1_IDX;
#endif
#if defined(P_PWMB) && _SW_PWM_USE_PWMB2
        case GPIO_PB2:
            return _SW_PWM_PWMB2_IDX;
#endif
#if defined(P_PWMB) && _SW_PWM_USE_PWMB3
        case GPIO_PB3:
            return _SW_PWM_PWMB3_IDX;
#endif
#else
#if defined(P_PWMA) && _SW_PWM_USE_PWMA0
        case GPIO_PA8:
            return _SW_PWM_PWMA0_IDX;
#endif
#if defined(P_PWMA) && _SW_PWM_USE_PWMA1
        case GPIO_PA9:
            return _SW_PWM_PWMA1_IDX;
#endif
#if defined(P_PWMA) && _SW_PWM_USE_PWMA2
        case GPIO_PA10:
            return _SW_PWM_PWMA2_IDX;
#endif
#if defined(P_PWMA) && _SW_PWM_USE_PWMA3
        case GPIO_PA11:
            return _SW_PWM_PWMA3_IDX;
#endif
#if defined(P_PWMB) && _SW_PWM_USE_PWMB0
        case GPIO_PB12:
            return _SW_PWM_PWMB0_IDX;
#endif
#if defined(P_PWMB) && _SW_PWM_USE_PWMB1
        case GPIO_PB13:
            return _SW_PWM_PWMB1_IDX;
#endif
#if defined(P_PWMB) && _SW_PWM_USE_PWMB2
        case GPIO_PB14:
            return _SW_PWM_PWMB2_IDX;
#endif
#if defined(P_PWMB) && _SW_PWM_USE_PWMB3
        case GPIO_PB15:
            return _SW_PWM_PWMB3_IDX;
#endif
#endif
        default:
            return no_USEABLE_HW_PWM;//no pin
    }
}

//------------------------------------------------------------------//
// CB_SW_PWM_HwPwmInit
// Parameter: 
//          _pin: GPIO_PA0~GPIO_MAX, only support hw pwm pin
//          _busyLevel: SW_PWM_BUSY_HIGH/SW_PWM_BUSY_LOW
//          _initDuty:0~65535
// return value:
//			NONE
//------------------------------------------------------------------//
static U8 CB_SW_PWM_HwPwmInit(const U8 _pin, const U8 _busyLevel, const U16 _initDuty)
{
    U8 _idx=CB_SW_PWM_getHwPwmIdx(_pin);
    if(_idx==no_USEABLE_HW_PWM) return EXIT_FAILURE;
    S_PWM_RESOURCE* pPwmRes=&s_PWM_Resource[_idx];
    
    SMU_PeriphClkCmd(SMU_PERIPH_PWM,ENABLE);
    if(((pPwmRes->timerx->Ctrl)&C_TMR_En)==0) {
#if _EF_SERIES
        TIMER_Init(pPwmRes->timerx,_SW_PWM_HW_RESOLUTION);
#else
        TIMER_Init(pPwmRes->timerx,TIMER_CLKDIV_1,_SW_PWM_HW_RESOLUTION);
#endif
        TIMER_Cmd(pPwmRes->timerx,ENABLE);
    }

    U16 _initTick = _HW_u16Duty2Tick(_initDuty);
    U32 _pwmStartLevel=(_busyLevel==SW_PWM_BUSY_HIGH)?PWM_START_LOW:PWM_START_HIGH;
    *(pPwmRes->dutyCtl)=((_initTick)<<16)|_pwmStartLevel|C_PWM_OUT_En;//default enable
    
    GPIO_Init(GPIO_GetPort(_pin),GPIO_GetPin(_pin),GPIO_MODE_ALT);
    
    return EXIT_SUCCESS;
}

//------------------------------------------------------------------//
// CB_SW_PWM_HwPwmCmd
// Parameter: 
//          _idx: Reference PWM_SELECT from 0~(num_USABLE_HW_PWM-1)
//          _cmd: ENABLE/DISABLE
// return value:
//			NONE
//------------------------------------------------------------------//
static void CB_SW_PWM_HwPwmCmd(const U8 _idx, const U8 _cmd)
{
    S_PWM_RESOURCE* pPwmRes=&s_PWM_Resource[_idx];
    U32 _temp=*(pPwmRes->dutyCtl);
    if(!_cmd) {
        _temp&=~C_PWM_OUT_En;
    } else {
        _temp|=C_PWM_OUT_En;
    }
    *(pPwmRes->dutyCtl)=_temp;
}

//------------------------------------------------------------------//
// CB_SW_PWM_HwPwmSetDuty
// Parameter: 
//          _idx: Reference PWM_SELECT from 0~(num_USABLE_HW_PWM-1)
//          _duty:0~65535
// return value:
//			NONE
//------------------------------------------------------------------//
static inline void CB_SW_PWM_HwPwmSetDuty(const U8 _idx, const U16 _duty)
{
    U16 _tick = _HW_u16Duty2Tick(_duty);
    S_PWM_RESOURCE* pPwmRes=&s_PWM_Resource[_idx];
    U32 _temp=*(pPwmRes->dutyCtl);
    _temp&=~(C_PWM_Duty);
    _temp|=(_tick<<16);//PWM_DUTY_VALUE_OFFSET
    *(pPwmRes->dutyCtl)=_temp;
}
#endif//_SW_PWM_HW_EXTENSION


typedef	struct
{
	U16					u16_TargetDutyNum;
	U16					u16_DiffFrame;
	U16					u16_NowDutyNum;
	U16					u16_NowDutyTick;
    U8                  u8_Pin;
	U8					u8_ChOption;//bool
#if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)
	U8					u8_BusyLevel;
#endif
#if _SW_PWM_HW_EXTENSION
    U8                  u8_HwPwmIdx;//0~n, no_USEABLE_HW_PWM(0xff) is no hw use
#endif
}S_SW_PWM_CH_DATA;
typedef	struct
{
	S_SW_PWM_CH_DATA	s_ChData[SW_PWM_CH_NUM];
	U16					u16_SW_PWM_TriangleTick;
	U8					u8_SW_PWM_Option;//bool
	U8					u8_SW_PWM_Pause;//bool
}S_SW_PWM_CTRL;
S_SW_PWM_CTRL		s_SW_PWM_CTRL;


//------------------------------------------------------------------//
// reset pin output
// Parameter: 
//          _CH:0~SW_PWM_CH_NUM
// return value:
//			NONE
//------------------------------------------------------------------//
static void _resetPinOut(const U8 _CH)
{
    S_SW_PWM_CH_DATA    *pChData=&s_SW_PWM_CTRL.s_ChData[_CH];
    if(pChData->u8_Pin>CB_SW_PWM_getMaxPin()) return;
#if _SW_PWM_HW_EXTENSION
    if(CB_SW_PWM_isPinOutputHi(pChData->u8_Pin)==true) {
        CB_SW_PWM_PinInitOutputHi(pChData->u8_Pin);
    } else {
        CB_SW_PWM_PinInitOutputLo(pChData->u8_Pin);
    }
    if(pChData->u8_HwPwmIdx!=no_USEABLE_HW_PWM) {
        CB_SW_PWM_HwPwmCmd(pChData->u8_HwPwmIdx,DISABLE);
        pChData->u8_HwPwmIdx=no_USEABLE_HW_PWM;
    }
#endif
    pChData->u8_Pin=0xFF;
}
//------------------------------------------------------------------//
// sync pin output
// Parameter: 
//          _CH:0~SW_PWM_CH_NUM
// return value:
//			NONE
//------------------------------------------------------------------//
static inline void _syncPinOut(const U8 _CH)
{
    S_SW_PWM_CH_DATA    *pChData=&s_SW_PWM_CTRL.s_ChData[_CH];
    pChData->u16_NowDutyTick = SW_PWM_u16Duty2Tick(pChData->u16_NowDutyNum);
    if(!pChData->u8_ChOption) return;
    
#if _SW_PWM_HW_EXTENSION
    if(pChData->u8_HwPwmIdx!=no_USEABLE_HW_PWM) {
        CB_SW_PWM_HwPwmSetDuty(pChData->u8_HwPwmIdx,pChData->u16_NowDutyNum);
        return;
    }
#endif

    if(pChData->u16_NowDutyTick>=s_SW_PWM_CTRL.u16_SW_PWM_TriangleTick) {
    #if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)
        CB_SW_PWM_PinBusy(pChData->u8_Pin, pChData->u8_BusyLevel);
    #else
        CB_SW_PWM_PinBusy(pChData->u8_Pin, _SW_PWM_BUSY_LEVEL);
    #endif
    } else {
    #if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)
        CB_SW_PWM_PinNonBusy(pChData->u8_Pin, pChData->u8_BusyLevel);
    #else
        CB_SW_PWM_PinNonBusy(pChData->u8_Pin, _SW_PWM_BUSY_LEVEL);
    #endif
    }
}
//------------------------------------------------------------------//
// update pwm duty
// Parameter: 
//          NONE
// return value:
//			NONE
//------------------------------------------------------------------//
static void _updateDuty(void)
{
    if(s_SW_PWM_CTRL.u8_SW_PWM_Pause) return;

	U16	_CH;
    for(_CH = s_SW_PWM_CTRL.u16_SW_PWM_TriangleTick ; _CH < SW_PWM_CH_NUM ; _CH += SW_PWM_TICK) {
        S_SW_PWM_CH_DATA    *pChData=&s_SW_PWM_CTRL.s_ChData[_CH];
        if(pChData->u16_DiffFrame) {//update new duty
            pChData->u16_NowDutyNum =
                SW_PWM_getNextDuty(
                    pChData->u16_NowDutyNum
                    ,pChData->u16_TargetDutyNum
                    ,pChData->u16_DiffFrame
                );
            pChData->u16_DiffFrame--;
        } else {
            pChData->u16_NowDutyNum = pChData->u16_TargetDutyNum;
        }
    }
}
//------------------------------------------------------------------//
// pwm generation
// Parameter: 
//          NONE
// return value:
//			NONE
//------------------------------------------------------------------//
static void _pwmGenerator(void)
{
    U8	_CH;
	if(s_SW_PWM_CTRL.u16_SW_PWM_TriangleTick<SW_PWM_TICK) {
		for(_CH=0;_CH<SW_PWM_CH_NUM;_CH++) {
            S_SW_PWM_CH_DATA    *pChData=&s_SW_PWM_CTRL.s_ChData[_CH];
			if(!pChData->u8_ChOption) continue;
#if _SW_PWM_HW_EXTENSION
            if(pChData->u8_HwPwmIdx!=no_USEABLE_HW_PWM) continue;
#endif
            if(pChData->u16_NowDutyTick==s_SW_PWM_CTRL.u16_SW_PWM_TriangleTick) {//update Pin State to HI
            #if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)
                CB_SW_PWM_PinBusy(pChData->u8_Pin, pChData->u8_BusyLevel);
            #else
                CB_SW_PWM_PinBusy(pChData->u8_Pin, _SW_PWM_BUSY_LEVEL);
            #endif
            }
		}
	} else {
		for(_CH=0;_CH<SW_PWM_CH_NUM;_CH++) {
			_syncPinOut(_CH);
		}
	}
}
//------------------------------------------------------------------//
// update pwm event
// Parameter: 
//          NONE
// return value:
//			NONE
//------------------------------------------------------------------//
static void _updatePwmEvent(void)
{
    S_SW_PWM_CMD_TYPE _tempCmd;
	while(CB_SW_PWM_GetCmd((U32*)&_tempCmd)==EXIT_SUCCESS) {//get new CMD
		if(_tempCmd.u8_CMD_ch >= SW_PWM_CH_NUM) continue;
        U8 _CH = _tempCmd.u8_CMD_ch;
        S_SW_PWM_CH_DATA    *pChData=&s_SW_PWM_CTRL.s_ChData[_CH];
        pChData->u16_NowDutyNum = pChData->u16_TargetDutyNum;
        pChData->u16_TargetDutyNum = _tempCmd.u8_CMD_TargetDutyNum*257;
        pChData->u16_DiffFrame = _tempCmd.u16_CMD_DiffFrame;
        
        if(!pChData->u16_DiffFrame) {//if DF==0, update now
            pChData->u16_NowDutyNum = pChData->u16_TargetDutyNum;
            _syncPinOut(_CH);
        }
	}
}
//------------------------------------------------------------------//
// SW PWM-IO Timer interrupt of serviceloop(Use RTC TIMEBASE frequence)
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void	SW_PWM_TimerIsr()
{
	if(!s_SW_PWM_CTRL.u8_SW_PWM_Option) return;
    s_SW_PWM_CTRL.u16_SW_PWM_TriangleTick--;
    _updateDuty();
    if(!s_SW_PWM_CTRL.u16_SW_PWM_TriangleTick) {
        s_SW_PWM_CTRL.u16_SW_PWM_TriangleTick = SW_PWM_TICK;	//Peak value
    }
	_pwmGenerator();
    _updatePwmEvent();
}

//------------------------------------------------------------------//
// SW PWM-IO Initial
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void	SW_PWM_Init()
{
	memset(&s_SW_PWM_CTRL,0,sizeof(S_SW_PWM_CTRL));
	s_SW_PWM_CTRL.u16_SW_PWM_TriangleTick = SW_PWM_TICK;//Peak value
    U8 _CH;
    for(_CH=0;_CH<SW_PWM_CH_NUM;++_CH) {
        S_SW_PWM_CH_DATA    *pChData=&s_SW_PWM_CTRL.s_ChData[_CH];
        pChData->u8_Pin=0xFF;
#if _SW_PWM_HW_EXTENSION
        pChData->u8_HwPwmIdx=no_USEABLE_HW_PWM;
#endif
    }
}

//------------------------------------------------------------------//
// SW PWM-IO Force Pin to NonBusy
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void	SW_PWM_ForcePinNonBusy()
{
	if(s_SW_PWM_CTRL.u8_SW_PWM_Option) return;//only work at u8_SW_PWM_Option DISABLE
    GIE_DISABLE();
    U8	_CH;
    for(_CH=0;_CH<SW_PWM_CH_NUM;_CH++) {
        S_SW_PWM_CH_DATA    *pChData=&s_SW_PWM_CTRL.s_ChData[_CH];
    #if _SW_PWM_HW_EXTENSION
        if(pChData->u8_HwPwmIdx!=no_USEABLE_HW_PWM) {
            CB_SW_PWM_HwPwmSetDuty(pChData->u8_HwPwmIdx,0);
            continue;
        }
    #endif
    #if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)
        CB_SW_PWM_PinNonBusy(pChData->u8_Pin, pChData->u8_BusyLevel);
    #else
        CB_SW_PWM_PinNonBusy(pChData->u8_Pin, _SW_PWM_BUSY_LEVEL);
    #endif
    }
    GIE_ENABLE();
}

//------------------------------------------------------------------//
// Start SW PWM-IO Module
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void	SW_PWM_Start()
{
	s_SW_PWM_CTRL.u8_SW_PWM_Pause = DISABLE;
	s_SW_PWM_CTRL.u8_SW_PWM_Option = ENABLE;
}

//------------------------------------------------------------------//
// Stop SW PWM-IO Module
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void	SW_PWM_Stop()
{
	U8	_CH;
	s_SW_PWM_CTRL.u8_SW_PWM_Option = DISABLE;
	s_SW_PWM_CTRL.u8_SW_PWM_Pause = DISABLE;
	for(_CH=0;_CH<SW_PWM_CH_NUM;_CH++) {
		SW_PWM_CH_Stop(_CH);
	}
}

//------------------------------------------------------------------//
// Pause SW PWM-IO Module
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void	SW_PWM_Pause()
{
	if(s_SW_PWM_CTRL.u8_SW_PWM_Option) {
		s_SW_PWM_CTRL.u8_SW_PWM_Pause = ENABLE;
	}
}

//------------------------------------------------------------------//
// Resume SW PWM-IO Module
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void	SW_PWM_Resume()
{
	s_SW_PWM_CTRL.u8_SW_PWM_Pause = DISABLE;
}

//------------------------------------------------------------------//
// SW PWM-IO Module Sleep
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void	SW_PWM_Sleep()
{
	if(s_SW_PWM_CTRL.u8_SW_PWM_Pause && s_SW_PWM_CTRL.u8_SW_PWM_Option) {
		s_SW_PWM_CTRL.u8_SW_PWM_Option = DISABLE;
		SW_PWM_ForcePinNonBusy();
	}
}

//------------------------------------------------------------------//
// SW PWM-IO Module WakeUp
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void	SW_PWM_WakeUp()
{
	if(s_SW_PWM_CTRL.u8_SW_PWM_Pause) {
		s_SW_PWM_CTRL.u8_SW_PWM_Option = ENABLE;
	}
}

//------------------------------------------------------------------//
// Set SW PWM-IO duty by percent
// Parameter: 
//          _CH:			SW PWM-IO Channel(0 ~ _SW_PWM_CH_NUM-1)
//			_Duty:			SW PWM-IO Duty(0 ~ 65535)
// return value:
//          EXIT_SUCCESS(0):Set Duty success
//			EXIT_FAILURE(1):Set Duty error
//------------------------------------------------------------------//
U8	SW_PWM_Duty_Set(U8 _CH, U16 _Duty)
{
	if(_CH >= SW_PWM_CH_NUM) return EXIT_FAILURE;//Setting Error
    S_SW_PWM_CH_DATA    *pChData=&s_SW_PWM_CTRL.s_ChData[_CH];
    GIE_DISABLE();
	pChData->u16_NowDutyNum = pChData->u16_TargetDutyNum = _Duty;
    pChData->u16_DiffFrame = 0;
    _syncPinOut(_CH);
    GIE_ENABLE();
	return	EXIT_SUCCESS;
}

//------------------------------------------------------------------//
// Set SW PWM-IO Channel
// Parameter: 
//          _CH:		SW PWM-IO Channel(0 ~ _SW_PWM_CH_NUM-1)
//			_Pin:		GPIO(GPIO_PA0 ~ GPIO_PC7)...Reference nx1_gpio.h
// return value:
//          EXIT_SUCCESS(0):Initial success
//			EXIT_FAILURE(1):Initial error
//------------------------------------------------------------------//
#if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)
U8	_SW_PWM_CH_Set(U8 _CH, U8 _Pin, U8 _BusyLevel)
#else
U8	_SW_PWM_CH_Set(U8 _CH, U8 _Pin)
#endif
{
    if(_CH >= SW_PWM_CH_NUM) return EXIT_FAILURE;//Setting Error
    
	S_SW_PWM_CH_DATA    *pChData=&s_SW_PWM_CTRL.s_ChData[_CH];
	if(_Pin <= CB_SW_PWM_getMaxPin()) {//check same pin return
        U8 _ch;
        for(_ch=0;_ch<SW_PWM_CH_NUM;++_ch) {
            if(_ch==_CH) continue;
            S_SW_PWM_CH_DATA    *pOtherChData=&s_SW_PWM_CTRL.s_ChData[_ch];
            if(pOtherChData->u8_Pin == _Pin) { //duplicate pin, disable it
                GIE_DISABLE();
                _resetPinOut(_ch);
                GIE_ENABLE();
            }
        }
        
		if(	(pChData->u8_Pin==_Pin)
		#if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)
			&&(pChData->u8_BusyLevel==_BusyLevel)
		#endif
		)return EXIT_FAILURE;
	}

	GIE_DISABLE();
    do {
        if(_Pin > CB_SW_PWM_getMaxPin()) {
            _resetPinOut(_CH);
            break;
        }
        pChData->u8_Pin = _Pin;
#if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)
        pChData->u8_BusyLevel = (_BusyLevel==SW_PWM_BUSY_HIGH)?SW_PWM_BUSY_HIGH:SW_PWM_BUSY_LOW;
#endif
        do {
#if _SW_PWM_HW_EXTENSION
            pChData->u8_HwPwmIdx = CB_SW_PWM_getHwPwmIdx(pChData->u8_Pin);
            if(pChData->u8_HwPwmIdx!=no_USEABLE_HW_PWM) {
                bool _isOutHi,_isBusyHi;
                _isOutHi=CB_SW_PWM_isPinOutputHi(pChData->u8_Pin);
#if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)
                _isBusyHi = (pChData->u8_BusyLevel==SW_PWM_BUSY_HIGH);
#else
                _isBusyHi = (_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_HIGH);
#endif
                U16 _initDuty;
                if(pChData->u8_ChOption) {
                    _initDuty = pChData->u16_NowDutyNum;
                } else {
                    _initDuty = (_isOutHi==_isBusyHi)?65535:0;
                }
#if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)
                CB_SW_PWM_HwPwmInit(pChData->u8_Pin, pChData->u8_BusyLevel, _initDuty);
#else
                CB_SW_PWM_HwPwmInit(pChData->u8_Pin, _SW_PWM_BUSY_LEVEL, _initDuty);
#endif
                break;
            }
#endif
            
            if(CB_SW_PWM_isPinOutputMode(pChData->u8_Pin)==true) {//Orig Mode is Out, init to Out same level
                if(CB_SW_PWM_isPinOutputHi(pChData->u8_Pin)==true) {
                    CB_SW_PWM_PinInitOutputHi(pChData->u8_Pin);
                } else {
                    CB_SW_PWM_PinInitOutputLo(pChData->u8_Pin);
                }
                break;
            }
            
            if( SW_PWM_BUSY_HIGH
#if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)
                == pChData->u8_BusyLevel
#else
                == _SW_PWM_BUSY_LEVEL
#endif
            ) { //Orig Mode is In(or MFP), init to Out nonBusy
                CB_SW_PWM_PinInitOutputLo(pChData->u8_Pin);
            } else {
                CB_SW_PWM_PinInitOutputHi(pChData->u8_Pin);
            }
        } while(0);
    } while(0);
	GIE_ENABLE();

	return	EXIT_SUCCESS;
}

//------------------------------------------------------------------//
// Set SW PWM-IO Channel
// Parameter: 
//          _CH:		SW PWM-IO Channel(0 ~ _SW_PWM_CH_NUM-1)
//			_Pin:		GPIO(GPIO_PA0 ~ GPIO_PC7)...Reference nx1_gpio.h
//			_InitDuty:	Pin Initial Duty(0 ~ 65535)
// return value:
//          EXIT_SUCCESS(0):Initial success
//			EXIT_FAILURE(1):Initial error
//------------------------------------------------------------------//
#if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)
U8	SW_PWM_CH_Set(U8 _CH, U8 _Pin, U8 _BusyLevel, U16 _InitDuty)
#else
U8	SW_PWM_CH_Set(U8 _CH, U8 _Pin, U16 _InitDuty)
#endif
{
	if(_CH >= SW_PWM_CH_NUM) return EXIT_FAILURE;//Setting Error
    
    if(SW_PWM_Duty_Set(_CH,_InitDuty)!=EXIT_SUCCESS) return EXIT_FAILURE;
#if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)
    if(_SW_PWM_CH_Set(_CH, _Pin, _BusyLevel)!=EXIT_SUCCESS) return EXIT_FAILURE;
#else
    if(_SW_PWM_CH_Set(_CH, _Pin)!=EXIT_SUCCESS) return EXIT_FAILURE;
#endif
    return EXIT_SUCCESS;
}

//------------------------------------------------------------------//
// Start SW PWM-IO channel
// Parameter: 
//          _CH:		SW PWM-IO Channel(0 ~ _SW_PWM_CH_NUM-1)
// return value:
//          EXIT_SUCCESS(0):Start success
//			EXIT_FAILURE(1):Start error
//------------------------------------------------------------------//
U8	SW_PWM_CH_Start(U8 _CH)
{
	if(_CH >= SW_PWM_CH_NUM) return	EXIT_FAILURE;//Setting Error
    
    S_SW_PWM_CH_DATA    *pChData=&s_SW_PWM_CTRL.s_ChData[_CH];
	pChData->u8_ChOption = ENABLE;//Enable ch pwm

	return EXIT_SUCCESS;
}

//------------------------------------------------------------------//
// Stop SW PWM-IO channel
// Parameter: 
//          _CH:		SW PWM-IO Channel(0 ~ _SW_PWM_CH_NUM-1)
// return value:
//          EXIT_SUCCESS(0):Stop success
//			EXIT_FAILURE(1):Stop error
//------------------------------------------------------------------//
U8	SW_PWM_CH_Stop(U8 _CH)
{
	if(_CH >= SW_PWM_CH_NUM) return	EXIT_FAILURE;//Setting Error
	
    S_SW_PWM_CH_DATA    *pChData=&s_SW_PWM_CTRL.s_ChData[_CH];
	pChData->u8_ChOption = DISABLE;//disable ch pwm
	
    bool _isBusy = (pChData->u16_TargetDutyNum == 65535);
    
#if _SW_PWM_HW_EXTENSION
    if(pChData->u8_HwPwmIdx!=no_USEABLE_HW_PWM) {
        GIE_DISABLE();
        CB_SW_PWM_HwPwmSetDuty(pChData->u8_HwPwmIdx,(_isBusy?65535:0));
        GIE_ENABLE();
        return EXIT_SUCCESS;
    }
#endif
	
	GIE_DISABLE();
    if(_isBusy) {//full duty
        #if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)
            CB_SW_PWM_PinBusy(pChData->u8_Pin, pChData->u8_BusyLevel);
        #else
            CB_SW_PWM_PinBusy(pChData->u8_Pin, _SW_PWM_BUSY_LEVEL);
        #endif
    } else {
        #if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)
            CB_SW_PWM_PinNonBusy(pChData->u8_Pin, pChData->u8_BusyLevel);
        #else
            CB_SW_PWM_PinNonBusy(pChData->u8_Pin, _SW_PWM_BUSY_LEVEL);
        #endif
    }
	GIE_ENABLE();
	return EXIT_SUCCESS;
}

//------------------------------------------------------------------//
// Get SW PWM-IO channel duty
// Parameter: 
//          _CH:		SW PWM-IO Channel(0 ~ _SW_PWM_CH_NUM-1)
// return value:
//          Duty:		0 ~ 65535(100%)
//------------------------------------------------------------------//
U16	SW_PWM_CH_GetDuty(U8 _CH)
{
	if(_CH >= SW_PWM_CH_NUM) return 0;
    
    S_SW_PWM_CH_DATA    *pChData=&s_SW_PWM_CTRL.s_ChData[_CH];
	return (pChData->u16_NowDutyNum);
}

//------------------------------------------------------------------//
// Get SW PWM-IO channel duty
// Parameter: 
//          _Percent:	0 ~ 100
// return value:
//          Duty:		0 ~ 65535(100%)
//------------------------------------------------------------------//
U16	SW_PWM_Percent2U16(U8 _Percent)
{
	return	((_Percent<100)?(((_Percent*65535)+50)/100):65535);
}

//------------------------------------------------------------------//
// SW PWM-IO ChData Initial
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void	SW_PWM_ChDataInit()
{
	U8 _CH;
	bool _isOutHi;
	bool _isBusyHi;
	for(_CH=0;_CH<SW_PWM_CH_NUM;++_CH) {
		S_SW_PWM_CH_DATA    *pChData = &(s_SW_PWM_CTRL.s_ChData[_CH]);
		if(pChData->u8_Pin<=CB_SW_PWM_getMaxPin()) {
			_isOutHi = CB_SW_PWM_isPinOutputHi(pChData->u8_Pin);
#if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)
			_isBusyHi = (pChData->u8_BusyLevel==SW_PWM_BUSY_HIGH);
#else
			_isBusyHi = (_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_HIGH);
#endif
			if(_isOutHi==_isBusyHi) {//now is busy level
				pChData->u16_TargetDutyNum = 65535;//initial setting
				pChData->u16_NowDutyNum = 65535;
			} else {
				pChData->u16_TargetDutyNum = 0;//initial setting
				pChData->u16_NowDutyNum = 0;
			}
		} else {
			pChData->u16_TargetDutyNum = 0;//initial setting
			pChData->u16_NowDutyNum = 0;
		}
		pChData->u16_DiffFrame = 0;
		pChData->u16_NowDutyTick = SW_PWM_u16Duty2Tick(pChData->u16_NowDutyNum);
#if _SW_PWM_HW_EXTENSION
        if(pChData->u8_HwPwmIdx!=no_USEABLE_HW_PWM) {
            CB_SW_PWM_HwPwmSetDuty(pChData->u8_HwPwmIdx,pChData->u16_NowDutyNum);
        }
#endif
	}
}
#endif//(_SW_PWM_MODULE)