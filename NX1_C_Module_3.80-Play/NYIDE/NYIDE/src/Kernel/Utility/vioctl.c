/* Includes ------------------------------------------------------------------*/
#include "include.h"
#include <string.h>
#include "nx1_gpio.h"
#include "nx1_timer.h"
#include "nx1_pwm.h"
#include "nx1_smu.h"
#include "nx1_spi.h"
#include "IoTab.h"
#include "_queueTemplate.h"

#if _ACTION_MODULE
/* CB for GPIO -------------------------------------------------------------------*/
static inline U8 CB_VIO_getMaxPin(void)
{
    return GPIO_MAX;
}

static inline void CB_VIO_PinBusy(const U8 _pin, const U8 BusyLevel)
{
    if(_pin > CB_VIO_getMaxPin()) return;
    U8 _portIdx=GPIO_GetPortIdx(_pin);
    U16 _pinMask = 0x1<<GPIO_GetPin(_pin);
    if(BusyLevel) {
        GPIO_PortList[_portIdx]->Data |= _pinMask;
    } else {
        GPIO_PortList[_portIdx]->Data &= ~_pinMask;
    }
}

static inline void CB_VIO_PinNonBusy(const U8 _pin, const U8 BusyLevel)
{
    if(_pin > CB_VIO_getMaxPin()) return;
    U8 _portIdx=GPIO_GetPortIdx(_pin);
    U16 _pinMask = 0x1<<GPIO_GetPin(_pin);
    if(BusyLevel) {
        GPIO_PortList[_portIdx]->Data &= ~_pinMask;
    } else {
        GPIO_PortList[_portIdx]->Data |= _pinMask;
    }
}

static void CB_VIO_PinInitOutputHi(const U8 _pin)
{
    GPIO_Init(GPIO_GetPort(_pin),GPIO_GetPin(_pin),GPIO_MODE_OUT_HI);
}

static void CB_VIO_PinInitOutputLo(const U8 _pin)
{
    GPIO_Init(GPIO_GetPort(_pin),GPIO_GetPin(_pin),GPIO_MODE_OUT_LOW);
}

static bool CB_VIO_isPinOutputMode(const U8 _pin)
{
    return (GPIO_ReadMode(GPIO_GetPort(_pin),GPIO_GetPin(_pin))==0); //true=dir out
}

static bool CB_VIO_isPinOutputHi(const U8 _pin)
{
    return (GPIO_Read(GPIO_GetPort(_pin),GPIO_GetPin(_pin))!=0); //true=output Hi
}

/* Defines -------------------------------------------------------------------*/
#if		(_VIO_TIMER==USE_TIMER0)
	#define	P_VIO_TIMER	TIMER0
#elif	(_VIO_TIMER==USE_TIMER1)
	#define	P_VIO_TIMER	TIMER1
#elif	(_VIO_TIMER==USE_TIMER2)
	#define	P_VIO_TIMER	TIMER2
#elif	(_VIO_TIMER==USE_TIMER_PWMA)
	#define	P_VIO_TIMER	PWMATIMER
#elif	(_VIO_TIMER==USE_TIMER_PWMB)
	#define	P_VIO_TIMER	PWMBTIMER
#elif	(_VIO_TIMER==USE_TIMER3 && _EF_SERIES)
	#define	P_VIO_TIMER	TIMER3
#else// RTC is default
	//P_VIO_TIMER ndef
#endif

#define	VIO_CH_NUM	_VIO_CH_NUM
#ifndef VIO_MARK_FIFOSIZE
#define VIO_MARK_FIFOSIZE   8
#endif

#ifdef P_VIO_TIMER
#if (_NYIDE_TIMEBASE==TIMEBASE_1000us)
#define VIO_PWM_BASE_RATE				(1000)
#elif (_NYIDE_TIMEBASE==TIMEBASE_250us)
#define VIO_PWM_BASE_RATE				(4000)
#else
#define VIO_PWM_BASE_RATE				(16000)
#endif
#else//RTC
#if (_NYIDE_TIMEBASE==TIMEBASE_1000us)
#define VIO_PWM_BASE_RATE				(1024)
#elif (_NYIDE_TIMEBASE==TIMEBASE_250us)
#define VIO_PWM_BASE_RATE				(4096)
#else
#define VIO_PWM_BASE_RATE				(16384)
#endif
#define VIO_RTC_COMPENSATE_CYCLE		(128)
#define VIO_RTC_CYCLE_ON_DUTY			(125)
#endif

#define VIO_TIMER_TICK					(((OPTION_HIGH_FREQ_CLK+(VIO_PWM_BASE_RATE/2))/VIO_PWM_BASE_RATE)-1)
#define	VIO_PWM_FRAMERATE				_NYIDE_FRAMERATE
#define	VIO_PWM_TICK					(((VIO_PWM_BASE_RATE)+(VIO_PWM_FRAMERATE>>1))/VIO_PWM_FRAMERATE)	//max 256 @64Hz Rate
#define	VIO_PWM_getNextDuty(ND,TGD,DF)	((ND)+(((TGD)-(ND)+((DF)>>1))/(DF)))				//ND=u16_NowDutyNum, TGD=u16_TargetDutyNum, DF=u16_DiffFrame

#define VIO_GAMMA2_CORRECTION(ND)		((((U32)ND)*(((U32)ND)+2))>>16)						//gamma=2 correction
#ifdef	_VIO_GAMMA2_CORRECTION
#define	VIO_PWM_u16Duty2Tick(ND)		(((VIO_GAMMA2_CORRECTION(ND))*(VIO_PWM_TICK+1))>>16)	//ND=u16_NowDutyNum
#else
#define	VIO_PWM_u16Duty2Tick(ND)		(((ND)*(VIO_PWM_TICK+1))>>16)						//ND=u16_NowDutyNum
#endif

//#define	VIO_PWM_Ms2Frame(ms)		((U16)((ms)*VIO_PWM_FRAMERATE/1000))//Cost 335ns @32M
#define	VIO_PWM_Ms2Frame(ms)			((U16)(((ms)*VIO_PWM_FRAMERATE*131)>>17))//Cost 60ns @32M

#if (_NYIDE_TIMEBASE==TIMEBASE_1000us)
#define	VIO_PWM_Ms2Tick(ms)				(ms)
#elif (_NYIDE_TIMEBASE==TIMEBASE_250us)
#define	VIO_PWM_Ms2Tick(ms)				((U16)((ms)<<2))
#else//16K
#define	VIO_PWM_Ms2Tick(ms)				((U16)((ms)<<4))
#endif

#define	Type_VIO_Spec		(0x03)
#define	Type_VIO_Data		(0x01)
#define	Type_VIO_Mark		(0x00)
#define	Type_VIO_End		(0x04)

#ifndef _VIO_USE_PWMA0
#define _VIO_USE_PWMA0   DISABLE
#endif
#ifndef _VIO_USE_PWMA1
#define _VIO_USE_PWMA1   DISABLE
#endif
#ifndef _VIO_USE_PWMA2
#define _VIO_USE_PWMA2   DISABLE
#endif
#ifndef _VIO_USE_PWMA3
#define _VIO_USE_PWMA3   DISABLE
#endif
#ifndef _VIO_USE_PWMB0
#define _VIO_USE_PWMB0   DISABLE
#endif
#ifndef _VIO_USE_PWMB1
#define _VIO_USE_PWMB1   DISABLE
#endif
#ifndef _VIO_USE_PWMB2
#define _VIO_USE_PWMB2   DISABLE
#endif
#ifndef _VIO_USE_PWMB3
#define _VIO_USE_PWMB3   DISABLE
#endif

#define _VIO_HW_EXTENSION   (   (defined(P_PWMA)&&_VIO_USE_PWMA0)    \
                            ||  (defined(P_PWMA)&&_VIO_USE_PWMA1)    \
                            ||  (defined(P_PWMA)&&_VIO_USE_PWMA2)    \
                            ||  (defined(P_PWMA)&&_VIO_USE_PWMA3)    \
                            ||  (defined(P_PWMB)&&_VIO_USE_PWMB0)    \
                            ||  (defined(P_PWMB)&&_VIO_USE_PWMB1)    \
                            ||  (defined(P_PWMB)&&_VIO_USE_PWMB2)    \
                            ||  (defined(P_PWMB)&&_VIO_USE_PWMB3)    )
#if _VIO_HW_EXTENSION
#ifndef _VIO_HW_RESOLUTION
#define _VIO_HW_RESOLUTION  VIO_TIMER_TICK//1~65535
#endif
#if (_VIO_HW_RESOLUTION<1)||(_VIO_HW_RESOLUTION>65535)
#error "_VIO_HW_RESOLUTION only support 1~65535."
#endif
#if     defined(P_VIO_TIMER)                \
    &&  _VIO_TIMER==USE_TIMER_PWMA          \
    &&  _VIO_HW_RESOLUTION!=VIO_TIMER_TICK  \
    &&  defined(P_PWMA)                     \
    &&  (   _VIO_USE_PWMA0                  \
        ||  _VIO_USE_PWMA1                  \
        ||  _VIO_USE_PWMA2                  \
        ||  _VIO_USE_PWMA3  )
#error  "ACTION conflict with PMWA."
#endif
#if     defined(P_VIO_TIMER)                \
    &&  _VIO_TIMER==USE_TIMER_PWMB          \
    &&  _VIO_HW_RESOLUTION!=VIO_TIMER_TICK  \
    &&  defined(P_PWMB)                     \
    &&  (   _VIO_USE_PWMB0                  \
        ||  _VIO_USE_PWMB1                  \
        ||  _VIO_USE_PWMB2                  \
        ||  _VIO_USE_PWMB3  )
#error  "ACTION conflict with PMWB."
#endif
#ifdef	_VIO_GAMMA2_CORRECTION
#define	_HW_u16Duty2Tick(ND)			(((VIO_GAMMA2_CORRECTION(ND))*(_VIO_HW_RESOLUTION+1))>>16)  //ND = u16_NowDutyNum
#else
#define	_HW_u16Duty2Tick(ND)			(((ND)*(_VIO_HW_RESOLUTION+1))>>16)                            //ND = u16_NowDutyNum
#endif
typedef enum{
#if	defined(P_PWMA) && _VIO_USE_PWMA0
    _VIO_PWMA0_IDX,
#endif
#if defined(P_PWMA) && _VIO_USE_PWMA1
    _VIO_PWMA1_IDX,
#endif
#if defined(P_PWMA) && _VIO_USE_PWMA2
    _VIO_PWMA2_IDX,
#endif
#if defined(P_PWMA) && _VIO_USE_PWMA3
    _VIO_PWMA3_IDX,
#endif
#if defined(P_PWMB) && _VIO_USE_PWMB0
    _VIO_PWMB0_IDX,
#endif
#if defined(P_PWMB) && _VIO_USE_PWMB1
    _VIO_PWMB1_IDX,
#endif
#if defined(P_PWMB) && _VIO_USE_PWMB2
    _VIO_PWMB2_IDX,
#endif
#if defined(P_PWMB) && _VIO_USE_PWMB3
    _VIO_PWMB3_IDX,
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
#if defined(P_PWMA) && _VIO_USE_PWMA0
    {
        .pwmx   =   PWMA,
        .timerx =   PWMATIMER,
        .dutyCtl=   &(PWMA->IO0_Duty),
    },
#endif
#if	defined(P_PWMA) && _VIO_USE_PWMA1
    {
        .pwmx   =   PWMA,
        .timerx =   PWMATIMER,
        .dutyCtl=   &(PWMA->IO1_Duty),
    },
#endif
#if	defined(P_PWMA) && _VIO_USE_PWMA2
    {
        .pwmx   =   PWMA,
        .timerx =   PWMATIMER,
        .dutyCtl=   &(PWMA->IO2_Duty),
    },
#endif
#if	defined(P_PWMA) && _VIO_USE_PWMA3
    {
        .pwmx   =   PWMA,
        .timerx =   PWMATIMER,
        .dutyCtl=   &(PWMA->IO3_Duty),
    },
#endif
#if	defined(P_PWMB) && _VIO_USE_PWMB0
    {
        .pwmx   =   PWMB,
        .timerx =   PWMBTIMER,
        .dutyCtl=   &(PWMB->IO0_Duty),
    },
#endif
#if	defined(P_PWMB) && _VIO_USE_PWMB1
    {
        .pwmx   =   PWMB,
        .timerx =   PWMBTIMER,
        .dutyCtl=   &(PWMB->IO1_Duty),
    },
#endif
#if	defined(P_PWMB) && _VIO_USE_PWMB2
    {
        .pwmx   =   PWMB,
        .timerx =   PWMBTIMER,
        .dutyCtl=   &(PWMB->IO2_Duty),
    },
#endif
#if	defined(P_PWMB) && _VIO_USE_PWMB3
    {
        .pwmx   =   PWMB,
        .timerx =   PWMBTIMER,
        .dutyCtl=   &(PWMB->IO3_Duty),
    },
#endif
};

//------------------------------------------------------------------//
// CB_VIO_getHwPwmIdx(Map of gpio with hwPwmPin)
// Parameter: 
//          _pin: GPIO_PA0~GPIO_MAX, only support hw pwm pin
// return value:
//			Reference PWM_SELECT from 0~(num_USABLE_HW_PWM-1)
//------------------------------------------------------------------//
static U8 CB_VIO_getHwPwmIdx(const U8 _pin)
{
    switch(_pin) {
#if _EF_SERIES
#if defined(P_PWMA) && _VIO_USE_PWMA0
        case GPIO_PA12:
            return _VIO_PWMA0_IDX;
#endif
#if defined(P_PWMA) && _VIO_USE_PWMA1
        case GPIO_PA13:
            return _VIO_PWMA1_IDX;
#endif
#if defined(P_PWMA) && _VIO_USE_PWMA2
        case GPIO_PA14:
            return _VIO_PWMA2_IDX;
#endif
#if defined(P_PWMA) && _VIO_USE_PWMA3
        case GPIO_PA15:
            return _VIO_PWMA3_IDX;
#endif
#if defined(P_PWMB) && _VIO_USE_PWMB0
        case GPIO_PB0:
            return _VIO_PWMB0_IDX;
#endif
#if defined(P_PWMB) && _VIO_USE_PWMB1
        case GPIO_PB1:
            return _VIO_PWMB1_IDX;
#endif
#if defined(P_PWMB) && _VIO_USE_PWMB2
        case GPIO_PB2:
            return _VIO_PWMB2_IDX;
#endif
#if defined(P_PWMB) && _VIO_USE_PWMB3
        case GPIO_PB3:
            return _VIO_PWMB3_IDX;
#endif
#else
#if defined(P_PWMA) && _VIO_USE_PWMA0
        case GPIO_PA8:
            return _VIO_PWMA0_IDX;
#endif
#if defined(P_PWMA) && _VIO_USE_PWMA1
        case GPIO_PA9:
            return _VIO_PWMA1_IDX;
#endif
#if defined(P_PWMA) && _VIO_USE_PWMA2
        case GPIO_PA10:
            return _VIO_PWMA2_IDX;
#endif
#if defined(P_PWMA) && _VIO_USE_PWMA3
        case GPIO_PA11:
            return _VIO_PWMA3_IDX;
#endif
#if defined(P_PWMB) && _VIO_USE_PWMB0
        case GPIO_PB12:
            return _VIO_PWMB0_IDX;
#endif
#if defined(P_PWMB) && _VIO_USE_PWMB1
        case GPIO_PB13:
            return _VIO_PWMB1_IDX;
#endif
#if defined(P_PWMB) && _VIO_USE_PWMB2
        case GPIO_PB14:
            return _VIO_PWMB2_IDX;
#endif
#if defined(P_PWMB) && _VIO_USE_PWMB3
        case GPIO_PB15:
            return _VIO_PWMB3_IDX;
#endif
#endif
        default:
            return no_USEABLE_HW_PWM;//no pin
    }
}

//------------------------------------------------------------------//
// CB_VIO_HwPwmInit
// Parameter: 
//          _pin: GPIO_PA0~GPIO_MAX, only support hw pwm pin
//          _busyLevel: 
//          _initDuty:0~65535
// return value:
//			NONE
//------------------------------------------------------------------//
static U8 CB_VIO_HwPwmInit(const U8 _pin, const U8 _busyLevel, const U16 _initDuty)
{
    U8 _idx=CB_VIO_getHwPwmIdx(_pin);
    if(_idx==no_USEABLE_HW_PWM) return EXIT_FAILURE;
    S_PWM_RESOURCE* pPwmRes=&s_PWM_Resource[_idx];
    
    SMU_PeriphClkCmd(SMU_PERIPH_PWM,ENABLE);
    if(((pPwmRes->timerx->Ctrl)&C_TMR_En)==0) {
#if _EF_SERIES
        TIMER_Init(pPwmRes->timerx,_VIO_HW_RESOLUTION);
#else
        TIMER_Init(pPwmRes->timerx,TIMER_CLKDIV_1,_VIO_HW_RESOLUTION);
#endif
        TIMER_Cmd(pPwmRes->timerx,ENABLE);
    }

    U16 _initTick = _HW_u16Duty2Tick(_initDuty);
    U32 _pwmStartLevel=(_busyLevel==ACTION_BUSY_HIGH)?PWM_START_LOW:PWM_START_HIGH;
    *(pPwmRes->dutyCtl)=((_initTick)<<16)|_pwmStartLevel|C_PWM_OUT_En;//default enable
    
    GPIO_Init(GPIO_GetPort(_pin),GPIO_GetPin(_pin),GPIO_MODE_ALT);
    
    return EXIT_SUCCESS;
}

//------------------------------------------------------------------//
// CB_VIO_HwPwmCmd
// Parameter: 
//          _idx: Reference PWM_SELECT from 0~(num_USABLE_HW_PWM-1)
//          _cmd: ENABLE/DISABLE
// return value:
//			NONE
//------------------------------------------------------------------//
static void CB_VIO_HwPwmCmd(const U8 _idx, const U8 _cmd)
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
// CB_VIO_HwPwmSetDuty
// Parameter: 
//          _idx: Reference PWM_SELECT from 0~(num_USABLE_HW_PWM-1)
//          _duty:0~65535
// return value:
//			NONE
//------------------------------------------------------------------//
static inline void CB_VIO_HwPwmSetDuty(const U8 _idx, const U16 _duty)
{
    U16 _tick = _HW_u16Duty2Tick(_duty);
    S_PWM_RESOURCE* pPwmRes=&s_PWM_Resource[_idx];
    U32 _temp=*(pPwmRes->dutyCtl);
    _temp&=~(C_PWM_Duty);
    _temp|=(_tick<<16);//PWM_DUTY_VALUE_OFFSET
    *(pPwmRes->dutyCtl)=_temp;
}
#endif//_ACTION_HW_EXTENSION

#if !defined(VIO_CMD_FIFOSIZE)
#define VIO_CMD_FIFOSIZE    16
#endif
#if (VIO_CMD_FIFOSIZE&(VIO_CMD_FIFOSIZE-1))!=0
#error "VIO_CMD_FIFOSIZE only support power of 2.(16,32...etc)"
#endif
#define VIO_CMD_FIFOMASK    (VIO_CMD_FIFOSIZE-1)
typedef struct 
{
	U8					u8_Cmd_DataType;				//Byte0
	U8					u8_Cmd_TargetDuty;//0~255		//Byte1
	U16					u16_Cmd_TargetTime;//ms 0~4095	//Byte2~3
}S_VIO_CMD_TYPE __attribute__((aligned(4)));//FIFO use U32 to reduce process time.
typedef struct 
{
	U32					buf[VIO_CMD_FIFOSIZE];//FIFO use U32 to reduce process time.
	U8					pr;
	U8					pw;
}S_VIO_CMD_FIFO;
typedef struct 
{
	U16					u16_TargetDutyNum;//PWM setting
	U16					u16_DiffFrame;//PWM setting
	U16					u16_NowDutyNum;//PWM setting
	U16					u16_NowDutyTick;//PWM setting
	U8					u8_BusyLevel;//ACTION_BUSY_LOW, ACTION_BUSY_HIGH
    U8                  u8_Pin;//GPIO_PA0~GPIO_MAX
#if _VIO_HW_EXTENSION
    U8                  u8_HwPwmIdx;//0~n, no_USEABLE_HW_PWM(0xff) is no hw use
#endif
}S_VIO_CH_PWM;
typedef struct 
{
	U32					u32_StartAddr;//Data setting
	U32					u32_CurrAddr;//Data setting
	U16					u16_Timeline;//Timeline Unit:Tick
	U8					u8_Version;//Data version
	volatile    U8		u8_State;//STATUS_STOP(0),STATUS_PLAYING(1),STATUS_PAUSE(2)
	U8					u8_StorageMode;//OTP_MODE(0x2),SPI_MODE(0x0)
	U8					u8_RepeatOption;//ACTION_NO_REPEAT(0), ACTION_REPEAT(1)
	U8					u8_GetDataFlag;//1:Available GetData, 0:Stop GetData
#ifndef P_VIO_TIMER
	U8					u8_FreqCompensate;//only for RTC cover 2.4% offset
#endif
}S_VIO_CH_CTL;
typedef struct 
{
	U16					u16_TriangleTick;
	volatile    U8		u8_Option;
#ifdef P_VIO_TIMER
	U8					u8_TMR_semaphore;
#endif//#ifdef P_VIO_TIMER
}S_VIO_CTL;

/* Static Variable------------------------------------------------------------*/
static S_VIO_CTL        s_VIO_CTL;
_queueDefine(U8,VIO_MARK_FIFOSIZE,VIO_MARK);
static S_VIO_CH_CTL     s_VIO_CH_CTL[VIO_CH_NUM];
static S_VIO_CH_PWM		s_VIO_CH_PWM[VIO_CH_NUM];
static S_VIO_CMD_FIFO   s_VIO_Cmd_fifo[VIO_CH_NUM];

/* Extern	----------------------------------------------------------*/

//------------------------------------------------------------------//
// Actino Channel CMD_FIFO Initial
// Parameter:
//              _CH : 0~(VIO_CH_NUM-1)
// return value:
//              None
//------------------------------------------------------------------// 
static void VIO_CH_CMD_FIFO_Init(U8 _CH)
{
    S_VIO_CMD_FIFO *pChFifo=&s_VIO_Cmd_fifo[_CH];
    pChFifo->pr = pChFifo->pw = 0;
}
//------------------------------------------------------------------//
// Actino CMD FIFO empty check
// Parameter:
//              _CH : 0~(VIO_CH_NUM-1)
// return value:
//              true / false
//------------------------------------------------------------------//
static bool VIO_CH_CMD_FIFO_Is_Empty(U8 _CH)
{
    S_VIO_CMD_FIFO *pChFifo=&s_VIO_Cmd_fifo[_CH];
    return (pChFifo->pw==pChFifo->pr);
}
//------------------------------------------------------------------//
// Actino CMD FIFO full check
// Parameter:
//              _CH : 0~(VIO_CH_NUM-1)
// return value:
//              true / false
//------------------------------------------------------------------//
static bool VIO_CH_CMD_FIFO_Is_Full(U8 _CH)
{
    S_VIO_CMD_FIFO *pChFifo=&s_VIO_Cmd_fifo[_CH];
    return pChFifo->pw==((pChFifo->pr)^(VIO_CMD_FIFOMASK+1));
}
//------------------------------------------------------------------//
// Actino get CMD data from FIFO
// Parameter:
//              _CH : 0~(VIO_CH_NUM-1)
//              data: S_VIO_CMD_TYPE (U32*)
// return value:
//              1:is success, 0:not success
//------------------------------------------------------------------//
static U8   VIO_CH_CMD_FIFO_Get(U8 _CH, U32 *data)
{
    if(VIO_CH_CMD_FIFO_Is_Empty(_CH)) return EXIT_FAILURE;
    S_VIO_CMD_FIFO *pChFifo=&s_VIO_Cmd_fifo[_CH];
    *data=pChFifo->buf[pChFifo->pr & VIO_CMD_FIFOMASK];
    pChFifo->pr=(pChFifo->pr+1)&((2*(VIO_CMD_FIFOMASK+1))-1);
    return EXIT_SUCCESS;
}
//------------------------------------------------------------------//
// Actino put CMD data to FIFO
// Parameter:
//              _CH : 0~(VIO_CH_NUM-1)
//              data: S_VIO_CMD_TYPE (U32*)
// return value:
//              1:is success, 0:not success
//------------------------------------------------------------------//
static U8   VIO_CH_CMD_FIFO_Put(U8 _CH, U32 *data)
{
    if(VIO_CH_CMD_FIFO_Is_Full(_CH)) return EXIT_FAILURE;
    S_VIO_CMD_FIFO *pChFifo=&s_VIO_Cmd_fifo[_CH];
    pChFifo->buf[pChFifo->pw & VIO_CMD_FIFOMASK]=*data;
    pChFifo->pw=(pChFifo->pw+1)&((2*(VIO_CMD_FIFOMASK+1))-1);
    return EXIT_SUCCESS;
}
//------------------------------------------------------------------//
// Actino CH Initial
// Parameter:
//				_CH : 0~(VIO_CH_NUM-1)
// return value:
//				None
//------------------------------------------------------------------//  
static void VIO_CH_Init(U8 _CH)
{
    S_VIO_CH_CTL *pChCtl=&s_VIO_CH_CTL[_CH];
	memset(pChCtl,0,sizeof(S_VIO_CH_CTL));
    S_VIO_CH_PWM *pChPwm=&s_VIO_CH_PWM[_CH];
	memset(pChPwm,0,sizeof(S_VIO_CH_PWM));
    VIO_CH_CMD_FIFO_Init(_CH);
    pChPwm->u8_Pin=0xFF;
#if _VIO_HW_EXTENSION
    pChPwm->u8_HwPwmIdx=no_USEABLE_HW_PWM;
#endif
}
//------------------------------------------------------------------//
// reset pin output
// Parameter: 
//          _CH
// return value:
//			NONE
//------------------------------------------------------------------//
static void _resetPinOut(const U8 _CH)
{
    S_VIO_CH_PWM *pChPwm=&s_VIO_CH_PWM[_CH];
    if(pChPwm->u8_Pin>CB_VIO_getMaxPin()) return;
#if _VIO_HW_EXTENSION
    if(CB_VIO_isPinOutputHi(pChPwm->u8_Pin)==true) {
        CB_VIO_PinInitOutputHi(pChPwm->u8_Pin);
    } else {
        CB_VIO_PinInitOutputLo(pChPwm->u8_Pin);
    }
    if(pChPwm->u8_HwPwmIdx!=no_USEABLE_HW_PWM) {
        CB_VIO_HwPwmCmd(pChPwm->u8_HwPwmIdx,DISABLE);
        pChPwm->u8_HwPwmIdx=no_USEABLE_HW_PWM;
    }
#endif
    pChPwm->u8_Pin=0xFF;
}
//------------------------------------------------------------------//
// sync pin output
// Parameter: 
//          _CH
// return value:
//			NONE
//------------------------------------------------------------------//
static inline void _syncPinOut(U8 _CH)
{
    S_VIO_CH_PWM *pChPwm=&s_VIO_CH_PWM[_CH];
    pChPwm->u16_NowDutyTick=VIO_PWM_u16Duty2Tick(pChPwm->u16_NowDutyNum);
    
#if _VIO_HW_EXTENSION
    if(pChPwm->u8_HwPwmIdx!=no_USEABLE_HW_PWM) {
        CB_VIO_HwPwmSetDuty(pChPwm->u8_HwPwmIdx,pChPwm->u16_NowDutyNum);
        return;
    }
#endif

    if(pChPwm->u16_NowDutyTick>=s_VIO_CTL.u16_TriangleTick){
        CB_VIO_PinBusy(pChPwm->u8_Pin,pChPwm->u8_BusyLevel);
    } else {
        CB_VIO_PinNonBusy(pChPwm->u8_Pin,pChPwm->u8_BusyLevel);
    }
}
//------------------------------------------------------------------//
// ACTION Data decode
// Parameter: 
//          _CH
//          _type:
//          _len:
//          pData:
// return value:
//          NONE
//------------------------------------------------------------------//
static void _decode(U8 _CH, U8 _type, U8 _len, U8* pData)
{
    S_VIO_CH_CTL *pChCtl=&s_VIO_CH_CTL[_CH];
    S_VIO_CMD_TYPE _tempEvent;
    switch(_type){
        case Type_VIO_Spec:
            if(_len==1) {
                pChCtl->u8_Version=pData[0];
            }
            break;
        case Type_VIO_Data:
            if(_len==3) {
                _tempEvent=(S_VIO_CMD_TYPE){
                    .u8_Cmd_DataType=_type,
                    .u8_Cmd_TargetDuty=pData[2],
                    .u16_Cmd_TargetTime=(pData[1]<<8)+pData[0],
                };
                VIO_CH_CMD_FIFO_Put(_CH, (U32*)&_tempEvent);
            }
            break;
        case Type_VIO_Mark:
            if(_len==1) {
                _tempEvent=(S_VIO_CMD_TYPE){
                    .u8_Cmd_DataType=_type,
                    .u8_Cmd_TargetDuty=0,
                    .u16_Cmd_TargetTime=pData[0],
                };
                VIO_CH_CMD_FIFO_Put(_CH, (U32*)&_tempEvent);
            } else if(_len==2) {
                _tempEvent=(S_VIO_CMD_TYPE){
                    .u8_Cmd_DataType=_type,
                    .u8_Cmd_TargetDuty=0,
                    .u16_Cmd_TargetTime=(pData[1]<<8)+pData[0],
                };
                VIO_CH_CMD_FIFO_Put(_CH, (U32*)&_tempEvent);
            }
            break;
        case Type_VIO_End:
            if(_len==4) {
                _tempEvent=(S_VIO_CMD_TYPE){
                    .u8_Cmd_DataType=_type,
                    .u8_Cmd_TargetDuty=0,
                    .u16_Cmd_TargetTime=0,
                };
                VIO_CH_CMD_FIFO_Put(_CH, (U32*)&_tempEvent);
            }
            pChCtl->u32_CurrAddr=pChCtl->u32_StartAddr;//reget data
            break;
        default:
            break;
    }
}

//------------------------------------------------------------------//
// VIO Access data memory Start
// Parameter: 
//          _addr
//          _mode
// return value:
//          EXIT_SUCCESS(0):get data success
//          EXIT_FAILURE(1):get data error
//------------------------------------------------------------------//
static U8 _accessMemStart(U32 _addr, U8 _mode)
{
    switch(_mode) {
        default: //OTP_MODE,EF_UD_MODE
            break; // do nothing
#if _SPI_MODULE
        case SPI_MODE:
            SPI_FastReadStart(_addr);
            break;
#endif
#if (_SPI1_MODULE && _SPI1_USE_FLASH)
        case SPI1_MODE:
            SPI1_FastReadStart(_addr);
            break;
#endif
    }
    return EXIT_SUCCESS;
}

//------------------------------------------------------------------//
// VIO Access data memory
// Parameter: 
//          pData
//          _addr
//          _len
//          _mode
// return value:
//          EXIT_SUCCESS(0):get data success
//          EXIT_FAILURE(1):get data error
//------------------------------------------------------------------//
static U8 _accessMem(U8* pData, U32 _addr, U8 _len, U8 _mode)
{
    //if(!pData) return EXIT_FAILURE;
    switch(_mode) {
        default:
            break;
        case    OTP_MODE:
#if _EF_SERIES
        case    EF_UD_MODE:
#endif
            memcpy(pData,(void*)_addr,_len);
            break;
#if _SPI_MODULE
        case    SPI_MODE:
            SPI_ReceiveData(pData,_len);
            break;
#endif
#if _SPI1_MODULE&&_SPI1_USE_FLASH
        case    SPI1_MODE:
            SPI1_ReceiveData(pData,_len);
            break;
#endif
    }
    return EXIT_SUCCESS;
}

//------------------------------------------------------------------//
// VIO Access data memory Stop
// Parameter: 
//          _mode
// return value:
//          EXIT_SUCCESS(0):get data success
//          EXIT_FAILURE(1):get data error
//------------------------------------------------------------------//
static U8 _accessMemStop(U8 _mode)
{
    switch(_mode) {
        default: //OTP_MODE,EF_UD_MODE
            break; // do nothing
#if _SPI_MODULE
        case SPI_MODE:
            SPI_FastReadStop();
            break;
#endif
#if (_SPI1_MODULE && _SPI1_USE_FLASH)
        case SPI1_MODE:
            SPI1_FastReadStop();
            break;
#endif
    }
    return EXIT_SUCCESS;
}

#if !defined(VIO_GetDataSize)
#define VIO_GetDataSize 6
#endif
//------------------------------------------------------------------//
// VIO Get storage data
// Parameter: 
//          NONE
// return value:
//          EXIT_SUCCESS(0):get data success
//          EXIT_FAILURE(1):get data error
//------------------------------------------------------------------//
static U8   _getStorageData(U8 _CH)
{
    S_VIO_CH_CTL *pChCtl=&s_VIO_CH_CTL[_CH];
    if(pChCtl->u8_GetDataFlag==DISABLE) return EXIT_FAILURE;
    if(VIO_CH_CMD_FIFO_Is_Full(_CH)) return EXIT_FAILURE;
    
    //start access mem
    _accessMemStart(pChCtl->u32_CurrAddr, pChCtl->u8_StorageMode);
    
    //get type and len
    U8  pData[VIO_GetDataSize];
    _accessMem(
        pData,
        pChCtl->u32_CurrAddr,
        2,
        pChCtl->u8_StorageMode
    );
    U8 _type = pData[0], _len = pData[1];
    
    do {
        _accessMem(
            pData,
            (pChCtl->u32_CurrAddr+2), //offset 2B, skip current type and len
            (_len+2), //get current data and next type,len
            pChCtl->u8_StorageMode
        );
        pChCtl->u32_CurrAddr+=(_len+2);
        _decode(_CH, _type, _len, pData);
        if(_type==Type_VIO_End) {
            if(pChCtl->u8_RepeatOption==DISABLE) {
                pChCtl->u8_GetDataFlag=DISABLE;
            }
            break; //exit point1, need restart access memory.
        }
        if(VIO_CH_CMD_FIFO_Is_Full(_CH)) break;//exit point2
        _type=pData[_len];  //next type
        _len=pData[_len+1]; //next len
    } while(1);
    
    //stop access mem
    _accessMemStop(pChCtl->u8_StorageMode);
    return EXIT_SUCCESS;
}
//------------------------------------------------------------------//
// update pwm duty
// Parameter: 
//          _CH
// return value:
//			NONE
//------------------------------------------------------------------//
static void _updateDuty(U8 _CH)
{
    S_VIO_CH_CTL *pChCtl=&s_VIO_CH_CTL[_CH];
    S_VIO_CH_PWM *pChPwm=&s_VIO_CH_PWM[_CH];
    if(pChCtl->u8_State!=STATUS_PLAYING) return;
    if(pChPwm->u16_DiffFrame>0){//if+else Cost 1.14us @32M
        pChPwm->u16_NowDutyNum=VIO_PWM_getNextDuty(pChPwm->u16_NowDutyNum,pChPwm->u16_TargetDutyNum,pChPwm->u16_DiffFrame);
        --(pChPwm->u16_DiffFrame);
    } else {
        pChPwm->u16_NowDutyNum=pChPwm->u16_TargetDutyNum;
    }
}
//------------------------------------------------------------------//
// pwm generation
// Parameter: 
//          _CH
// return value:
//			NONE
//------------------------------------------------------------------//
static void _pwmGenerator(U8 _CH)
{
    S_VIO_CH_CTL *pChCtl=&s_VIO_CH_CTL[_CH];
    S_VIO_CH_PWM *pChPwm=&s_VIO_CH_PWM[_CH];
    if(pChCtl->u8_State==STATUS_STOP) return;
    if(s_VIO_CTL.u16_TriangleTick<VIO_PWM_TICK){
#if _VIO_HW_EXTENSION
        if(pChPwm->u8_HwPwmIdx!=no_USEABLE_HW_PWM) return;
#endif
        if(pChPwm->u16_NowDutyTick==s_VIO_CTL.u16_TriangleTick){
            CB_VIO_PinBusy(pChPwm->u8_Pin,pChPwm->u8_BusyLevel);
        }
    } else {
        _syncPinOut(_CH);
    }
}
//------------------------------------------------------------------//
// update pwm event
// Parameter: 
//          _CH
// return value:
//			NONE
//------------------------------------------------------------------//
static void _updatePwmEvent(U8 _CH)
{
    S_VIO_CH_CTL *pChCtl=&s_VIO_CH_CTL[_CH];
    S_VIO_CH_PWM *pChPwm=&s_VIO_CH_PWM[_CH];
    if(pChCtl->u8_State!=STATUS_PLAYING) return;
    S_VIO_CMD_TYPE _tempEvent;
    U16 _Duty=0;
    U8	_Mark=0;
    while(pChCtl->u16_Timeline==0){
        if(VIO_CH_CMD_FIFO_Get(_CH, (U32*)&_tempEvent)!=EXIT_SUCCESS){
            break;//no data, do next channel
        }
        if(_tempEvent.u8_Cmd_DataType==Type_VIO_Data){
            pChCtl->u16_Timeline=VIO_PWM_Ms2Tick(_tempEvent.u16_Cmd_TargetTime);//update Timeline
            pChPwm->u16_NowDutyNum=pChPwm->u16_TargetDutyNum;
            _Duty=((_tempEvent.u8_Cmd_TargetDuty)*257);
            if(_Duty!=pChPwm->u16_TargetDutyNum){
                pChPwm->u16_TargetDutyNum=_Duty;
                pChPwm->u16_DiffFrame=VIO_PWM_Ms2Frame(_tempEvent.u16_Cmd_TargetTime);
                if(pChPwm->u16_DiffFrame==0){//update now
                    pChPwm->u16_NowDutyNum=pChPwm->u16_TargetDutyNum;
                    _syncPinOut(_CH);
                }
            } else {
                pChPwm->u16_DiffFrame=0;
            }
        } else if(_tempEvent.u8_Cmd_DataType==Type_VIO_Mark){
            _Mark=((_tempEvent.u16_Cmd_TargetTime)&0xFF);
            if(_Mark !=0){//LoByte
                _queuePutData(VIO_MARK,&_Mark);
            }
            _Mark=((_tempEvent.u16_Cmd_TargetTime)>>8);
            if(_Mark !=0){//HiByte
                _queuePutData(VIO_MARK,&_Mark);
            }
        } else if(_tempEvent.u8_Cmd_DataType==Type_VIO_End){
            if(pChCtl->u8_RepeatOption==ACTION_REPEAT) continue;//get next data
            pChCtl->u8_State=STATUS_STOP;
            do {
                bool _isBusy = (pChPwm->u16_TargetDutyNum==65535);
#if _VIO_HW_EXTENSION
                if(pChPwm->u8_HwPwmIdx!=no_USEABLE_HW_PWM) {
                    CB_VIO_HwPwmSetDuty(pChPwm->u8_HwPwmIdx,(_isBusy?65535:0));
                    break;
                }
#endif
                if(_isBusy){
                    CB_VIO_PinBusy(pChPwm->u8_Pin,pChPwm->u8_BusyLevel);
                } else {
                    CB_VIO_PinNonBusy(pChPwm->u8_Pin,pChPwm->u8_BusyLevel);
                }
            } while(0);
            _resetPinOut(_CH);
#ifdef P_VIO_TIMER	
            if(s_VIO_CTL.u8_TMR_semaphore>0){
                --s_VIO_CTL.u8_TMR_semaphore;
                if(s_VIO_CTL.u8_TMR_semaphore==0){
                    TIMER_Cmd(P_VIO_TIMER,DISABLE);//disable TMR
                }
            }
#endif//#ifdef P_VIO_TIMER
            break;//do next channel
        } else;//other CMD, do nothing
    }
    
    do {
#ifndef	P_VIO_TIMER	//when use RTC, need compensate frequence
        const U8 _compensate_diff = (VIO_RTC_COMPENSATE_CYCLE-VIO_RTC_CYCLE_ON_DUTY);
        if(pChCtl->u8_FreqCompensate>=_compensate_diff){
            pChCtl->u8_FreqCompensate-=_compensate_diff;
        } else {
            pChCtl->u8_FreqCompensate+=VIO_RTC_CYCLE_ON_DUTY;
            break;//freeze Timeline
        }
#endif
        if(pChCtl->u16_Timeline!=0){
            --(pChCtl->u16_Timeline);
        }
    } while(0);
}
//------------------------------------------------------------------//
// Check if any vio control event @ INT
// Parameter: 
//			NONE
// return value:
//			NONE
//------------------------------------------------------------------//
void VIO_TimerIsr(void)
{
	if(s_VIO_CTL.u8_Option==DISABLE) return;
    
	U16	_CH;
	--s_VIO_CTL.u16_TriangleTick;
	for(_CH=s_VIO_CTL.u16_TriangleTick ;_CH<VIO_CH_NUM ;_CH+=VIO_PWM_TICK){//Update Duty
		_updateDuty(_CH);
	}

	if(s_VIO_CTL.u16_TriangleTick==0){
		s_VIO_CTL.u16_TriangleTick=VIO_PWM_TICK;
	}
	for(_CH=0;_CH<VIO_CH_NUM;++_CH){
		_pwmGenerator(_CH);
		_updatePwmEvent(_CH);
	}
}
#if	_SPI_MODULE || (_SPI1_MODULE&&_SPI1_USE_FLASH)
//------------------------------------------------------------------//
// get SPI data information
// Parameter: 
//			f_SpiOffset:		SpiOffset,Spi1_Offset
//			f_SpiBurstRead:		SPI_BurstRead,SPI1_BurstRead
//			f_SpiGetFileCount:	AUDIO_GetSpiFileCount,AUDIO_GetSpi1FileCount
//			index:				data index
//			addr:				return use
//			type:				return use
// return value:
//			EXIT_SUCCESS(0):
//			EXIT_FAILURE(1):
//------------------------------------------------------------------//
static U8 _getSpiDataInfo	(	U32 (*f_SpiOffset)(void),
								void (*f_SpiBurstRead)(U8*,U32,U16),
								U16 (*f_SpiGetFileCount)(void),
								U16 index,
								U32 *addr,
								U8	*type
							)
{
	*addr=0;
	*type=0xFF;
	if(index >= f_SpiGetFileCount()) return EXIT_FAILURE;
	
	U32 tempBuf=0;
	U32 tempAddr=f_SpiOffset();
    f_SpiBurstRead(	(U8*)&tempBuf,
    				tempAddr+SPI_BASE_INFO_SIZE+(SPI_TAB_INFO_ENTRY_SIZE*index),
    				SPI_TAB_INFO_ENTRY_SIZE);
    *addr=(tempBuf&0x07FFFFFF)+tempAddr;//get 27bits(0~26)
    *type=(tempBuf>>28)&0xF;//get 4bits(28~31)
    return EXIT_SUCCESS;
}
#endif
//------------------------------------------------------------------//
// Play IO
// Parameter: 
//			_CH:vio channel, ACTION_CH0 ~ ACTION_CHx (x=VIO_CH_NUM-1)
//			file: io action file
//			_Pin: GPIO_PA0~GPIO_MAX
//			repeat:ACTION_NO_REPEAT, ACTION_REPEAT
//			busy: ACTION_BUSY_LOW, ACTION_BUSY_HIGH
//			mode: OTP_MODE, EF_UD_MODE, SPI_MODE, SPI1_MODE
// return value:
//			EXIT_SUCCESS(0):Play success
//			other: fail
//------------------------------------------------------------------//  
U8 ACTION_PlayIO(U8 _CH,const U32 file,U8 _Pin,U8 repeat,U8 busy,U8 mode)
{
	U32 _addr=0;
#if	_SPI_MODULE || (_SPI1_MODULE&&_SPI1_USE_FLASH)
	U8	_type=0xFF;
#endif

	_CH -=_AUDIO_API_CH_NUM;
/*Input Data check -------------------------------------------------------------------*/
	if(_CH>=VIO_CH_NUM || _Pin > CB_VIO_getMaxPin()) return EXIT_FAILURE;//wrong channel or pin setting
	
	if(file & ADDRESS_MODE) {
		if(		mode==OTP_MODE
#if _EF_SERIES
            ||  mode==EF_UD_MODE
#endif
#if	_SPI_MODULE
			||	mode==SPI_MODE
#endif
#if	_SPI1_MODULE&&_SPI1_USE_FLASH
			||	mode==SPI1_MODE
#endif
		) {
			_addr=file&(~ADDRESS_MODE);
		} else;
	} else {
		switch(mode) {
#if	defined(IO_FILE_NUMBER)
			case	OTP_MODE:
				_addr=(file<IO_FILE_NUMBER)?IOActionList[file]:0;
				break;
#endif
#if _EF_SERIES
            case    EF_UD_MODE:
                _addr=UserDefinedData_GetAddressUseType(file, kUD_ACTION, EF_UD_MODE);
                break;
#endif
#if	_SPI_MODULE
			case	SPI_MODE:
					_getSpiDataInfo(SpiOffset,SPI_BurstRead,AUDIO_GetSpiFileCount,(U16)file,&_addr,&_type);
					if(_type!=SPI_DATA_TYPE_ACTION) _addr=0;
				break;
#endif//#if	_SPI_MODULE
#if	_SPI1_MODULE&&_SPI1_USE_FLASH
			case	SPI1_MODE:
					_getSpiDataInfo(Spi1_Offset,SPI1_BurstRead,AUDIO_GetSpi1FileCount,(U16)file,&_addr,&_type);
					if(_type!=SPI_DATA_TYPE_ACTION) _addr=0;
				break;
#endif//#if	_SPI1_MODULE&&_SPI1_USE_FLASH
			default:
				break;
		}
	}
	if(!_addr) return EXIT_FAILURE;//wrong address or wrong storage mode
	
	ACTION_StopIO((_CH+_AUDIO_API_CH_NUM));
/*Inital process -------------------------------------------------------------------*/	
	VIO_CH_Init(_CH);
	
    S_VIO_CH_PWM *pChPwm=&s_VIO_CH_PWM[_CH];
    pChPwm->u8_Pin=_Pin;
	pChPwm->u8_BusyLevel=(busy==ACTION_BUSY_HIGH)?ACTION_BUSY_HIGH:ACTION_BUSY_LOW;
	
    S_VIO_CH_CTL *pChCtl=&s_VIO_CH_CTL[_CH];
	pChCtl->u32_StartAddr=_addr;
	pChCtl->u32_CurrAddr=_addr;
	pChCtl->u8_StorageMode=mode;
	pChCtl->u8_RepeatOption=(repeat==ACTION_REPEAT)?ACTION_REPEAT:ACTION_NO_REPEAT;

	
	pChCtl->u8_GetDataFlag=ENABLE;
	_getStorageData(_CH);//fill CMD FIFO
	
/*Play Start -------------------------------------------------------------------*/

    GIE_DISABLE();
    do {
#if _VIO_HW_EXTENSION
        pChPwm->u8_HwPwmIdx = CB_VIO_getHwPwmIdx(pChPwm->u8_Pin);
        if(pChPwm->u8_HwPwmIdx!=no_USEABLE_HW_PWM) {
            bool _isOutHi = CB_VIO_isPinOutputHi(pChPwm->u8_Pin);
            bool _isBusyHi = (pChPwm->u8_BusyLevel==ACTION_BUSY_HIGH);
            U16 _initDuty = (_isOutHi==_isBusyHi)?65535:0;
            CB_VIO_HwPwmInit(pChPwm->u8_Pin, pChPwm->u8_BusyLevel, _initDuty);
            break;
        }
#endif
        
        if(CB_VIO_isPinOutputMode(pChPwm->u8_Pin)==true) {//Orig Mode is Out, init to Out same level
            if(CB_VIO_isPinOutputHi(pChPwm->u8_Pin)==true) {
                CB_VIO_PinInitOutputHi(pChPwm->u8_Pin);
            } else {
                CB_VIO_PinInitOutputLo(pChPwm->u8_Pin);
            }
            break;
        }
        
        if( ACTION_BUSY_HIGH== pChPwm->u8_BusyLevel) { //Orig Mode is In(or MFP), init to Out nonBusy
            CB_VIO_PinInitOutputLo(pChPwm->u8_Pin);
        } else {
            CB_VIO_PinInitOutputHi(pChPwm->u8_Pin);
        }
    } while(0);
	pChCtl->u8_State=STATUS_PLAYING;
	
#ifdef P_VIO_TIMER	
	if(s_VIO_CTL.u8_TMR_semaphore==0){
	#if _EF_SERIES
		TIMER_Init(P_VIO_TIMER,VIO_TIMER_TICK);	
	#else
		TIMER_Init(P_VIO_TIMER,TIMER_CLKDIV_1,VIO_TIMER_TICK);
	#endif
		TIMER_Cmd(P_VIO_TIMER,ENABLE);
	}
	++s_VIO_CTL.u8_TMR_semaphore;
#endif//#ifdef P_VIO_TIMER

	GIE_ENABLE();

	return	EXIT_SUCCESS;
}

//------------------------------------------------------------------//
// Pause IO
// Parameter: 
//			_CH:vio channel, ACTION_CH0 ~ ACTION_CHx (x=VIO_CH_NUM-1), ACTION_All_CH
// return value:
//			NONE
//------------------------------------------------------------------//  
void ACTION_PauseIO(U8 _CH)
{
	U8	_maxCH=0;
	
	_CH -=_AUDIO_API_CH_NUM;//offset
	
	if(_CH<VIO_CH_NUM){//single channel Pause
		//_CH=_CH;
		_maxCH=_CH+1;
	} else if(_CH==(ACTION_All_CH - _AUDIO_API_CH_NUM)){//all channel Pause
		_CH=0;
		_maxCH=_VIO_CH_NUM;
	} else {
		return;//wrong channel setting
	}
	
	GIE_DISABLE();
	for(/*init nothing*/;_CH<_maxCH;++_CH){
        S_VIO_CH_CTL *pChCtl=&s_VIO_CH_CTL[_CH];
		if(pChCtl->u8_State==STATUS_PLAYING){
			pChCtl->u8_State=STATUS_PAUSE;
		}
	}
	GIE_ENABLE();
}
//------------------------------------------------------------------//
// Resume IO
// Parameter: 
//			_CH:vio channel, ACTION_CH0 ~ ACTION_CHx (x=VIO_CH_NUM-1), ACTION_All_CH
// return value:
//			NONE
//------------------------------------------------------------------//  
void ACTION_ResumeIO(U8 _CH)
{
	U8	_maxCH=0;
	
	_CH -=_AUDIO_API_CH_NUM;//offset
	
	if(_CH<VIO_CH_NUM){//single channel Resume
		//_CH=_CH;
		_maxCH=_CH+1;
	} else if(_CH==(ACTION_All_CH - _AUDIO_API_CH_NUM)){//all channel Resume
		_CH=0;
		_maxCH=_VIO_CH_NUM;
	} else {
		return;//wrong channel setting
	}
	
	GIE_DISABLE();
	for(/*init nothing*/;_CH<_maxCH;++_CH){
        S_VIO_CH_CTL *pChCtl=&s_VIO_CH_CTL[_CH];
		if(pChCtl->u8_State==STATUS_PAUSE){
			pChCtl->u8_State=STATUS_PLAYING;
		}
	}
	GIE_ENABLE();
}
//------------------------------------------------------------------//
// Stop IO
// Parameter: 
//			_CH:vio channel, ACTION_CH0 ~ ACTION_CHx (x=VIO_CH_NUM-1), ACTION_All_CH
// return value:
//			NONE
//------------------------------------------------------------------//  
void ACTION_StopIO(U8 _CH)
{
	U8	_maxCH=0;
	
	_CH -=_AUDIO_API_CH_NUM;//offset
	
	if(_CH<VIO_CH_NUM){//single channel stop
		//_CH=_CH;
		_maxCH=_CH+1;
	} else if(_CH==(ACTION_All_CH - _AUDIO_API_CH_NUM)){//all channel stop
		_CH=0;
		_maxCH=_VIO_CH_NUM;
	} else {
		return;//wrong channel setting
	}
	
	GIE_DISABLE();
	for(/*init nothing*/;_CH<_maxCH;++_CH){
        S_VIO_CH_CTL *pChCtl=&s_VIO_CH_CTL[_CH];
        S_VIO_CH_PWM *pChPwm=&s_VIO_CH_PWM[_CH];
		if(pChCtl->u8_State !=STATUS_STOP){
            do {
#if _VIO_HW_EXTENSION
                if(pChPwm->u8_HwPwmIdx!=no_USEABLE_HW_PWM) {
                    CB_VIO_HwPwmSetDuty(pChPwm->u8_HwPwmIdx,0);
                    break;
                }
#endif
                CB_VIO_PinNonBusy(pChPwm->u8_Pin,pChPwm->u8_BusyLevel);
            } while(0);
            _resetPinOut(_CH);

			pChCtl->u8_State=STATUS_STOP;
			pChCtl->u8_GetDataFlag=DISABLE;
#ifdef P_VIO_TIMER	
			if(s_VIO_CTL.u8_TMR_semaphore>0){
				--s_VIO_CTL.u8_TMR_semaphore;
			}
#endif//#ifdef P_VIO_TIMER
		}
	}
#ifdef P_VIO_TIMER	
	if(s_VIO_CTL.u8_TMR_semaphore==0){
		TIMER_Cmd(P_VIO_TIMER,DISABLE);//disable TMR
	}
#endif//#ifdef P_VIO_TIMER
	GIE_ENABLE();
}
//------------------------------------------------------------------//
// Get IO current status
// Parameter: 
//			_CH:vio channel, ACTION_CH0 ~ ACTION_CHx (x=VIO_CH_NUM-1)
// return value:
//			status:STATUS_STOP,STATUS_PLAYING,STATUS_PAUSE
//------------------------------------------------------------------// 
U8 ACTION_GetIOStatus(U8 _CH){
	_CH -=_AUDIO_API_CH_NUM;
	if(_CH>=VIO_CH_NUM){
		return STATUS_STOP;
	}
    S_VIO_CH_CTL *pChCtl=&s_VIO_CH_CTL[_CH];
	return	pChCtl->u8_State;
} 
//------------------------------------------------------------------//
// Actino Service Loop
// Parameter: 
//				None
// return value:
//				None
//------------------------------------------------------------------//  
void ACTION_ServiceLoop(void)
{
	U8 i=0;
	for(i=0;i<VIO_CH_NUM;++i){//put data to fifo
		_getStorageData(i);
	}
    while(_queueGetData(VIO_MARK,&i)==EXIT_SUCCESS) {
		CB_ActionMark(i);
	}
}
//------------------------------------------------------------------//
// Actino Initial
// Parameter:
//				None
// return value:
//				None
//------------------------------------------------------------------//
void ACTION_Init(void)
{
	U8	_CH=0;
	memset(&s_VIO_CTL,0,sizeof(S_VIO_CTL));
	for(_CH=0;_CH<VIO_CH_NUM;++_CH){
		VIO_CH_Init(_CH);
	}
    _queueReset(VIO_MARK);
	
	s_VIO_CTL.u16_TriangleTick=VIO_PWM_TICK;
#ifdef P_VIO_TIMER	
	s_VIO_CTL.u8_TMR_semaphore=0;
#endif//#ifdef P_VIO_TIMER
	s_VIO_CTL.u8_Option=ENABLE;
}
//------------------------------------------------------------------//
// Actino Into Sleep
// Parameter: 
//				None
// return value:
//				None
//------------------------------------------------------------------//  
void ACTION_Sleep(void)
{
	U8 _CH=0;
	s_VIO_CTL.u8_Option=DISABLE;
#ifdef P_VIO_TIMER	
	if(s_VIO_CTL.u8_TMR_semaphore!=0){
		TIMER_Cmd(P_VIO_TIMER,DISABLE);//disable TMR
	}
#endif //#ifdef P_VIO_TIMER	
    GIE_DISABLE();
	for(_CH=0;_CH<VIO_CH_NUM;++_CH){//put data to fifo
        S_VIO_CH_CTL *pChCtl=&s_VIO_CH_CTL[_CH];
        S_VIO_CH_PWM *pChPwm=&s_VIO_CH_PWM[_CH];
		if(pChCtl->u8_State!=STATUS_STOP){
            do {
#if _VIO_HW_EXTENSION
                if(pChPwm->u8_HwPwmIdx!=no_USEABLE_HW_PWM) {
                    CB_VIO_HwPwmSetDuty(pChPwm->u8_HwPwmIdx,0);
                    break;
                }
#endif
                CB_VIO_PinNonBusy(pChPwm->u8_Pin,pChPwm->u8_BusyLevel);
            } while(0);
		}
	}
    GIE_ENABLE();
}

//------------------------------------------------------------------//
// Actino Wakeup
// Parameter: 
//				None
// return value:
//				None
//------------------------------------------------------------------//  
void ACTION_Wakeup(void)
{
#ifdef P_VIO_TIMER
	if(s_VIO_CTL.u8_TMR_semaphore!=0){
	#if _EF_SERIES
		TIMER_Init(P_VIO_TIMER,VIO_TIMER_TICK);	
	#else
		TIMER_Init(P_VIO_TIMER,TIMER_CLKDIV_1,VIO_TIMER_TICK);
	#endif
		TIMER_Cmd(P_VIO_TIMER,ENABLE);
	}
#endif //#ifdef P_VIO_TIMER	
	s_VIO_CTL.u8_Option=ENABLE;
}
//------------------------------------------------------------------//
// Check Action storage from SPI or not.
// Parameter: 
//			_CH:vio channel, ACTION_CH0 ~ ACTION_CHx (x=VIO_CH_NUM-1)
// return value:
//          true:SPI or SPI1, false:other
//------------------------------------------------------------------//      
bool ACTION_IsSpiAction(U8 _CH)
{
#if	_SPI_MODULE || (_SPI1_MODULE&&_SPI1_USE_FLASH)
	_CH -=_AUDIO_API_CH_NUM;
	if(_CH>=VIO_CH_NUM) return false;
	
    S_VIO_CH_CTL *pChCtl=&s_VIO_CH_CTL[_CH];
    U8 _mode = pChCtl->u8_StorageMode;
	return	((_mode==SPI_MODE)||(_mode==SPI1_MODE));
#else
	return false;
#endif//#if	_SPI_MODULE || (_SPI1_MODULE&&_SPI1_USE_FLASH)
}
#endif//#if	_ACTION_MODULE