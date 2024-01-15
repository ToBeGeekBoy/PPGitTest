/* Includes ------------------------------------------------------------------*/
#include "string.h"
#include "stdarg.h"
#include "include.h"
#include "nx1_intc.h"
#include "nx1_smu.h"
#include "nx1_gpio.h"
#include "nx1_ir.h"
#include "nx1_timer.h"
#include "nx1_pwm.h"
#include "nx1_rtc.h"
#include "nx1_lib.h"

#if (_UART_MODULE&&_DEBUG_MODE)
#include "debug.h"
#endif

#if _QFID_MODULE
/* Defines -------------------------------------------------------------------*/
#define DEBUG_IO					DISABLE

#define _QFID_OUTPUT_DATA           DISABLE //not support now.

#ifdef	_QFID_DETECT_DEBOUNCE
#define QFID_DETECT_DEBOUNCE		_QFID_DETECT_DEBOUNCE
#else
#define	QFID_DETECT_DEBOUNCE		1
#endif

#ifdef _QFID_LEAVE_DEBOUNCE
#define QFID_LEAVE_DEBOUNCE			_QFID_LEAVE_DEBOUNCE
#else
#define QFID_LEAVE_DEBOUNCE			1
#endif

#define QFID_IS_USE_INPUT			(	((_QFID_GROUP_NUM>=1)&&(_QFID_GRP0_RESPONSE==QFID_MODE_ID_INPUT))	\
									||	((_QFID_GROUP_NUM>=2)&&(_QFID_GRP1_RESPONSE==QFID_MODE_ID_INPUT))	\
									||	((_QFID_GROUP_NUM>=3)&&(_QFID_GRP2_RESPONSE==QFID_MODE_ID_INPUT))	\
									||	((_QFID_GROUP_NUM>=4)&&(_QFID_GRP3_RESPONSE==QFID_MODE_ID_INPUT)))

#if 	(_QFID_TX_IO_TYPE==QFID_TYPE_IR) && (_IR_MODULE) && (!_EF_SERIES)
#error "_QFID_MODULE hardware resource crash with _IR_MODULE when _QFID_TX_IO_TYPE select QFID_TYPE_IR,"
#elif	(_QFID_TX_IO_TYPE==QFID_TYPE_PWM) && (_PWMA_MODULE)
#error "_QFID_MODULE hardware resource crash with _PWMA_MODULE when _QFID_TX_IO_TYPE select QFID_TYPE_PWM"
#elif	(_QFID_TX_IO_TYPE==QFID_TYPE_PWM) && (_CAPTURE_MODULE) && (!_EF_SERIES)
#error "_QFID_MODULE hardware resource crash with _CAPTURE_MODULE when _QFID_TX_IO_TYPE select QFID_TYPE_PWM"
#elif	(_QFID_TX_IO_TYPE==QFID_TYPE_PWM) && (_TOUCHKEY_MODULE) && (!_EF_SERIES)
#error "_QFID_MODULE hardware resource crash with _TOUCHKEY_MODULE when _QFID_TX_IO_TYPE select QFID_TYPE_PWM"
#elif	(_QFID_TX_IO_TYPE==QFID_TYPE_PWM) && (_QFID_TIMER==USE_TIMER_PWMA)
#error "_QFID_MODULE hardware resource crash when _QFID_TX_IO_TYPE select QFID_TYPE_PWM"
#endif

#define _QFID_IS_IN_WINDOW(a,L,H)		(((a)>=(L))&&((a)<=(H))&&((L)<(H)))

#if !_QFID_IS_IN_WINDOW(_QFID_GROUP_NUM,1,8)
#error "_QFID_GROUP_NUM only support 1~8."
#endif//#if (_QFID_GROUP_NUM<=0)||(_QFID_GROUP_NUM>8)

const U8 QFID_Type[]={
#if (_QFID_GROUP_NUM>=1)
#if 	(_QFID_GRP0_RESPONSE==QFID_MODE_ID)
	QFID_MODE_ID,
#elif	(_QFID_GRP0_RESPONSE==QFID_MODE_ID_INPUT)
	QFID_MODE_ID_INPUT,
#else
#error "_QFID_GRP0_RESPONSE only support QFID_MODE_ID or QFID_MODE_ID_INPUT."
#endif
#endif//#if (_QFID_GROUP_NUM>=1)
#if (_QFID_GROUP_NUM>=2)
#if 	(_QFID_GRP1_RESPONSE==QFID_MODE_ID)
	QFID_MODE_ID,
#elif	(_QFID_GRP1_RESPONSE==QFID_MODE_ID_INPUT)
	QFID_MODE_ID_INPUT,
#else
#error "_QFID_GRP1_RESPONSE only support QFID_MODE_ID or QFID_MODE_ID_INPUT."
#endif
#endif//#if (_QFID_GROUP_NUM>=2)
#if (_QFID_GROUP_NUM>=3)
#if 	(_QFID_GRP2_RESPONSE==QFID_MODE_ID)
	QFID_MODE_ID,
#elif	(_QFID_GRP2_RESPONSE==QFID_MODE_ID_INPUT)
	QFID_MODE_ID_INPUT,
#else
#error "_QFID_GRP2_RESPONSE only support QFID_MODE_ID or QFID_MODE_ID_INPUT."
#endif
#endif//#if (_QFID_GROUP_NUM>=3)
#if (_QFID_GROUP_NUM>=4)
#if 	(_QFID_GRP3_RESPONSE==QFID_MODE_ID)
	QFID_MODE_ID,
#elif	(_QFID_GRP3_RESPONSE==QFID_MODE_ID_INPUT)
	QFID_MODE_ID_INPUT,
#else
#error "_QFID_GRP3_RESPONSE only support QFID_MODE_ID or QFID_MODE_ID_INPUT."
#endif
#endif//#if (_QFID_GROUP_NUM>=4)
#if (_QFID_GROUP_NUM>=5)
	QFID_MODE_ID,
#endif
#if (_QFID_GROUP_NUM>=6)
	QFID_MODE_ID,
#endif
#if (_QFID_GROUP_NUM>=7)
	QFID_MODE_ID,
#endif
#if (_QFID_GROUP_NUM>=8)
	QFID_MODE_ID,
#endif
};

const U8 Tag_Num[]={
#if (_QFID_GROUP_NUM>=1)
#if		(_QFID_GRP0_RESPONSE==QFID_MODE_ID) 		&& 	(!_QFID_IS_IN_WINDOW(_QFID_GRP0_TAG_NUM,1,16))
#error "_QFID_GRP0_TAG_NUM only support 1~16 when _QFID_GRP0_RESPONSE select QFID_MODE_ID."
#elif	(_QFID_GRP0_RESPONSE==QFID_MODE_ID_INPUT) 	&& 	(!_QFID_IS_IN_WINDOW(_QFID_GRP0_TAG_NUM,1,8))
#error "_QFID_GRP0_TAG_NUM only support 1~8 when _QFID_GRP0_RESPONSE select QFID_MODE_ID_INPUT."
#else
	_QFID_GRP0_TAG_NUM,
#endif
#endif//#if (_QFID_GROUP_NUM>=1)
#if (_QFID_GROUP_NUM>=2)
#if		(_QFID_GRP1_RESPONSE==QFID_MODE_ID) 		&& 	(!_QFID_IS_IN_WINDOW(_QFID_GRP1_TAG_NUM,1,16))
#error "_QFID_GRP1_TAG_NUM only support 1~16 when _QFID_GRP1_RESPONSE select QFID_MODE_ID."
#elif	(_QFID_GRP1_RESPONSE==QFID_MODE_ID_INPUT) 	&& 	(!_QFID_IS_IN_WINDOW(_QFID_GRP1_TAG_NUM,1,8))
#error "_QFID_GRP1_TAG_NUM only support 1~8 when _QFID_GRP1_RESPONSE select QFID_MODE_ID_INPUT."
#else
	_QFID_GRP1_TAG_NUM,
#endif
#endif//#if (_QFID_GROUP_NUM>=2)
#if (_QFID_GROUP_NUM>=3)
#if		(_QFID_GRP2_RESPONSE==QFID_MODE_ID) 		&& 	(!_QFID_IS_IN_WINDOW(_QFID_GRP2_TAG_NUM,1,16))
#error "_QFID_GRP2_TAG_NUM only support 1~16 when _QFID_GRP2_RESPONSE select QFID_MODE_ID."
#elif	(_QFID_GRP2_RESPONSE==QFID_MODE_ID_INPUT) 	&& 	(!_QFID_IS_IN_WINDOW(_QFID_GRP2_TAG_NUM,1,8))
#error "_QFID_GRP2_TAG_NUM only support 1~8 when _QFID_GRP2_RESPONSE select QFID_MODE_ID_INPUT."
#else
	_QFID_GRP2_TAG_NUM,
#endif
#endif//#if (_QFID_GROUP_NUM>=3)
#if (_QFID_GROUP_NUM>=4)
#if		(_QFID_GRP3_RESPONSE==QFID_MODE_ID) 		&& 	(!_QFID_IS_IN_WINDOW(_QFID_GRP3_TAG_NUM,1,16))
#error "_QFID_GRP3_TAG_NUM only support 1~16 when _QFID_GRP3_RESPONSE select QFID_MODE_ID."
#elif	(_QFID_GRP3_RESPONSE==QFID_MODE_ID_INPUT) 	&& 	(!_QFID_IS_IN_WINDOW(_QFID_GRP3_TAG_NUM,1,8))
#error "_QFID_GRP3_TAG_NUM only support 1~8 when _QFID_GRP3_RESPONSE select QFID_MODE_ID_INPUT."
#else
	_QFID_GRP3_TAG_NUM,
#endif
#endif//#if (_QFID_GROUP_NUM>=4)
#if (_QFID_GROUP_NUM>=5)
#if	(_QFID_GRP4_RESPONSE==QFID_MODE_ID) && 	(!_QFID_IS_IN_WINDOW(_QFID_GRP4_TAG_NUM,1,16))
#error "_QFID_GRP4_TAG_NUM only support 1~16 when _QFID_GRP4_RESPONSE select QFID_MODE_ID."
#else
	_QFID_GRP4_TAG_NUM,
#endif
#endif
#if (_QFID_GROUP_NUM>=6)
#if	(_QFID_GRP5_RESPONSE==QFID_MODE_ID) && 	(!_QFID_IS_IN_WINDOW(_QFID_GRP5_TAG_NUM,1,16))
#error "_QFID_GRP5_TAG_NUM only support 1~16 when _QFID_GRP5_RESPONSE select QFID_MODE_ID."
#else
	_QFID_GRP5_TAG_NUM,
#endif
#endif
#if (_QFID_GROUP_NUM>=7)
#if	(_QFID_GRP6_RESPONSE==QFID_MODE_ID) && 	(!_QFID_IS_IN_WINDOW(_QFID_GRP6_TAG_NUM,1,16))
#error "_QFID_GRP6_TAG_NUM only support 1~16 when _QFID_GRP6_RESPONSE select QFID_MODE_ID."
#else
	_QFID_GRP6_TAG_NUM,
#endif
#endif
#if (_QFID_GROUP_NUM>=8)
#if	(_QFID_GRP7_RESPONSE==QFID_MODE_ID) && 	(!_QFID_IS_IN_WINDOW(_QFID_GRP7_TAG_NUM,1,16))
#error "_QFID_GRP7_TAG_NUM only support 1~16 when _QFID_GRP7_RESPONSE select QFID_MODE_ID."
#else
	_QFID_GRP7_TAG_NUM,
#endif
#endif
};

// IO define
#define GET_IO_QFID_RX			GPIO_Read(_QFID_RX_PORT,_QFID_RX_PIN)

// QFID Command define list
	#define CMD_RC_Tag			(0x00 | (_QFID_VID_PID<<2))		// CMD00 (10)	....RC (Roll Call)
	#define CMD_RC_bTag			(0x01 | (_QFID_VID_PID<<2))		// CMD01 (11)
	#define CMD_RC_Tag_IN		(0x02 | (_QFID_VID_PID<<2))		// CMD02 (12)
	#define CMD_RC_bTag_IN		(0x03 | (_QFID_VID_PID<<2)) 	// CMD03 (13)

#if _QFID_GROUP_NUM > 4
	#define CMD_RC_MulGrp		(0x16 | (_QFID_VID_PID<<5))		// CMD06 (16)
#else
	#define CMD_RC_MulGrp		(0x17 | (_QFID_VID_PID<<5))		// CMD07 (17)
#endif

	#define CMD_QFID_Out		(0x15 | (_QFID_VID_PID<<5))		// CMD05 (15)
	#define CMD_KEY_EN			0x34							// CMD14

// Timing define (Unit:62.5usec)
#if		(_QFID_TIMER==USE_TIMER0)
	#define	P_QFID_TIMER	TIMER0
#elif	(_QFID_TIMER==USE_TIMER1)
	#define	P_QFID_TIMER	TIMER1
#elif	(_QFID_TIMER==USE_TIMER2)
	#define	P_QFID_TIMER	TIMER2
#elif	(_QFID_TIMER==USE_TIMER_PWMA)
	#define	P_QFID_TIMER	PWMATIMER
#elif	(_QFID_TIMER==USE_TIMER_PWMB)
	#define	P_QFID_TIMER	PWMBTIMER
#elif	(_QFID_TIMER==USE_TIMER3 && _EF_SERIES)
	#define	P_QFID_TIMER	TIMER3
#elif	(_QFID_TIMER==USE_RTC)// Use RTC, P_QFID_TIMER ndef
#if		(body_is_4KHZ)
#error "When _QFID_TIMER use RTC, QFID can not support IC body without RTC_16K."
#endif//#if		(body_is_4KHZ)
#endif//#define	P_QFID_TIMER
#define QFID_RX_SAMPLE_RATE		(16384)		//Hz
#define QFID_RX_TIMER_TICK		(((OPTION_HIGH_FREQ_CLK+(QFID_RX_SAMPLE_RATE/2))/QFID_RX_SAMPLE_RATE)-1)
#define	QFID_MS_2_SAMPLES(ms)	(U16)((((ms)*QFID_RX_SAMPLE_RATE)+500)/1000)

#define T_PRE_CHG				QFID_MS_2_SAMPLES(10)			// 10ms
#define T_START					QFID_MS_2_SAMPLES(7)
#define T_ANT_STB				QFID_MS_2_SAMPLES(50.752)
#define T_WAIT_TAG				QFID_MS_2_SAMPLES(13)
#define T_SILENT				QFID_MS_2_SAMPLES(3)
#define T_TX_H_BIT_C			QFID_MS_2_SAMPLES(3)			// Reader to Tag, 3ms Carrier
#define T_TX_L_BIT_C			QFID_MS_2_SAMPLES(1)			// Reader to Tag, 1ms Carrier
#define T_TX_BIT_NC				QFID_MS_2_SAMPLES(3)			// Reader to Tag, No Carrier
#define	T_TX_L_5TH_BIT_NC		QFID_MS_2_SAMPLES(4)			// Reader to Tag, No Carrier

// Length/Size/Threshold define
#define QFID_BUF_SIZE			40								// 8 tags * 5bit or 16 tags * 2bit

#define QFID_RX_GRP_MAX_LMT		100
#define QFID_RX_DAT0_THDL		2//3  //312uS condition
#define QFID_RX_DAT0_THDH		6//5
#define QFID_RX_DAT1_THDL		7
#define QFID_RX_DAT1_THDH		9
#define QFID_RX_DAT_MAX_LMT		20

#if !_EF_SERIES
#if !defined(_QFID_IR_TX_OUTPUT)
#define _QFID_IR_TX_OUTPUT				USE_IR0_TX
#endif
#endif

typedef enum{
	_STS_PRE_CHARGE,
	_STS_TX_START,
	_STS_TX_CMD_GRP,
	_STS_TX_ANT_STB,
	_STS_RX_TAG,
#if (_QFID_VID_PID) || (_QFID_OUTPUT_DATA)
	_STS_WAIT_TAG_SLT,								// time of wait tag + silent
	_STS_TX_OUT_DAT,
#endif
	_STS_TX_TAIL,	
}QFidIsrSts;
static volatile struct {
	U32		Dat;
	U16		cnt_Level;
	U8		sts;//reference QFidIsrSts
	U8		flag_Level;
	U8		cnt_Bit;
	U8		flag;
	U8		tmp_H, tmp_L;
}QFidIsr;

typedef enum{
	_STS1_RX_PRE_COND,
	_STS1_RX_PARSER0,
	_STS1_RX_PARSER1,
	_STS1_RX_OVER,
}QFidRxSts;
static volatile struct {
	U8		sts;//reference QFidRxSts
	U8		cnt_Slot;
	U8 		cnt_Period_H;
	U8 		cnt_Period_L;
	U8		flag_StartCount;
	U8		flag_ChkPoint;
}QFidRx;

typedef enum{
	_OP_Grp,
	_OP_Tag,
#if	QFID_IS_USE_INPUT
	_OP_Tag_IN,
#endif
#if _QFID_OUTPUT_DATA
	_OP_Out,
#endif
#if _QFID_VID_PID
	_OP_Key,
#endif
}QFidParaStsOp;
static volatile struct {
	U16 	time_tail;
	U8		num_Slot;
	U8 		time_Slot;
	U8		bit_Slot;
	U8		size;
	U8		sts_op;//reference QFidParaStsOp
}QFidPara; //parameter

typedef enum{
#if _QFID_VID_PID
	_STS_VID_PID,
#endif
	_STS_SCAN_GRP,
	_STS_SCAN_TAG,
#if _QFID_OUTPUT_DATA
	_STS_OUTPUT_CTRL,
#endif
}QFidCtlSts;
static volatile struct {
	U8		sts;	//state machine for serviceloop, reference QFidIsrSts
}QFidCtl;


static volatile struct {
#if	QFID_IS_USE_INPUT
	U32		tagInput[_QFID_GROUP_NUM];
#endif
	U16		tagIdNow[_QFID_GROUP_NUM];
	U16		tagIdPrevDet[_QFID_GROUP_NUM];
	U16		tagIdPrevLev[_QFID_GROUP_NUM];
	U8		cntDet[_QFID_GROUP_NUM];
	U8		cntLev[_QFID_GROUP_NUM];
}QFidDbce;
volatile U8 qfid_Dbce_toggleCMD;//0 or 1

static volatile struct {
#if	QFID_IS_USE_INPUT
	U32		TagInput[_QFID_GROUP_NUM];							// Tag ID & Input information
#endif
	U16		TagID 	[_QFID_GROUP_NUM];							// Tag ID         information
}tmpInfo_QFID;//stable data

static volatile U8 flag_Frame_end=0; //0:Timer busy, 1:decode busy

//---
static volatile U8   Dat_QFID_CmdGrp;
static volatile U32  Dat_QFID_TxOutDat;							// support 11bit/20bit

static U8 QFidBuf_H[QFID_BUF_SIZE]; 								// 4/8/32/40
static U8 QFidBuf_L[QFID_BUF_SIZE];
static U8 idx_QFidBuf;

//---Individual status
static volatile	U8		Get_Grp_QFID	=0x00;	//get from decoder
static volatile U16  	Get_Tag_QFID	=0x0000;//get from decoder
#if	QFID_IS_USE_INPUT
static volatile U32		Get_TagIN_QFID 	=0;		//get from decoder
#endif

//---					
static volatile U8		QFIDState		=0;							// 0: Disable, 1: Enable Function

#define BitOpTab(t)		(1<<(t))

/* Function -------------------------------------------------------------------*/
void CB_QFID_OutData(void);

//------------------------------------------------------------------//
// QFID roll-call group
// Parameter:
//			NONE
// return value:
//          NONE
//------------------------------------------------------------------//
#if _QFID_GROUP_NUM>2
static void QFID_RollCall_Grp(void)
{
	Dat_QFID_CmdGrp =CMD_RC_MulGrp;

	QFidPara.num_Slot 		=_QFID_GROUP_NUM;
	QFidPara.bit_Slot 		= 1;
	QFidPara.size     		= QFidPara.num_Slot*QFidPara.bit_Slot;
	QFidPara.time_Slot		= QFID_MS_2_SAMPLES(6.592);		//6.592ms	
	QFidPara.sts_op	  		=_OP_Grp;
}
#endif
//------------------------------------------------------------------//
// QFID roll-call tag
// Parameter:
//          grp     : group (support all group[7:0])
// return value:
//     		NONE
//------------------------------------------------------------------//
static void QFID_RollCall_Tag(U8 grp)
{
	if(qfid_Dbce_toggleCMD==0){
		Dat_QFID_CmdGrp = (CMD_RC_Tag<<3) | grp;
	} else {
		Dat_QFID_CmdGrp = (CMD_RC_bTag<<3) | grp;
	}
	QFidPara.num_Slot 		= Tag_Num[grp];
	QFidPara.bit_Slot		=  2;  		//2bit in unit slot
	QFidPara.size     		= QFidPara.num_Slot*QFidPara.bit_Slot;	

	QFidPara.time_Slot		= QFID_MS_2_SAMPLES(6.592);		//6.592ms
	QFidPara.sts_op	  		=_OP_Tag;
}
//------------------------------------------------------------------//
// QFID roll-call tag, include 3bit input data information
// Parameter:
//          grp     : group (just support group[3:0])
// return value:
//          0:ok
//          others	: fail
//------------------------------------------------------------------//
#if	QFID_IS_USE_INPUT
static U8 QFID_RollCall_Tag_Input(U8 grp)
{
	if(qfid_Dbce_toggleCMD==0){
		Dat_QFID_CmdGrp = (CMD_RC_Tag_IN<<3) | grp;
	} else {
		Dat_QFID_CmdGrp = (CMD_RC_bTag_IN<<3) | grp;
	}
	QFidPara.num_Slot 		= Tag_Num[grp];
	QFidPara.bit_Slot 		=  5;  		//5bit in unit slot
	QFidPara.size     		= QFidPara.num_Slot*QFidPara.bit_Slot;	

	QFidPara.time_Slot		= QFID_MS_2_SAMPLES(13.183);		//13.183ms		
	QFidPara.sts_op	  		=_OP_Tag_IN;

	return 0;
}
#endif
//------------------------------------------------------------------//
// QFID send data to specific tag
// Parameter:
//          grp     : group	(support all group[7:0 ])
//			tag		: tag	(support all tag  [15:0])
//			out_Dat : 4bit output data
// return value:
//     		NONE
//------------------------------------------------------------------//
#if _QFID_OUTPUT_DATA
void QFID_Send_OutData(U8 grp, U8 tag, U8 out_Dat)
{
	Dat_QFID_CmdGrp   = CMD_QFID_Out;
	Dat_QFID_TxOutDat = (grp<<8) | (tag<<4) | out_Dat;

	QFidPara.bit_Slot 		=11;  		//11bit in unit slot (only one slot)
	QFidPara.time_tail		=QFID_MS_2_SAMPLES(40);		//40ms
	QFidPara.sts_op	  		=_OP_Out;
}
#endif
//------------------------------------------------------------------//
// QFID send VID/PID
// Parameter:
//          vid     : vender id
//			pid		: project id
// return value:
//     		NONE
//------------------------------------------------------------------//
#if _QFID_VID_PID
static void QFID_Send_Key(U16 vid, U16 pid)
{
	Dat_QFID_CmdGrp   = CMD_KEY_EN;
	Dat_QFID_TxOutDat = (vid<<10) | (pid);

	QFidPara.bit_Slot 		=20;  		//20bit in unit slot (only one slot)
	QFidPara.time_tail		=QFID_MS_2_SAMPLES(40);		//40ms	
	QFidPara.sts_op	  		=_OP_Key;
}
#endif

//------------------------------------------------------------------//
// Decode QFID frame
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
static void QFID_DecodeFrame(void)
{
    U8 i, sts, _grpIdx, _tagIdx, _bitIdx, _invertCmd;
#if	QFID_IS_USE_INPUT
	U8 _inputBitIdx;
#endif
	const U8 _ThdTab[4][2][2]=
	{
		{
			{QFID_RX_DAT0_THDL,QFID_RX_DAT0_THDH},
			{QFID_RX_DAT0_THDL,QFID_RX_DAT0_THDH},
		},
		{
			{QFID_RX_DAT0_THDL,QFID_RX_DAT0_THDH},
			{QFID_RX_DAT1_THDL,QFID_RX_DAT1_THDH},
		},
		{
			{QFID_RX_DAT1_THDL,QFID_RX_DAT1_THDH},
			{QFID_RX_DAT0_THDL,QFID_RX_DAT0_THDH},
		},
		{
			{QFID_RX_DAT1_THDL,QFID_RX_DAT1_THDH},
			{QFID_RX_DAT1_THDL,QFID_RX_DAT1_THDH},
		},
	};
	_invertCmd=(qfid_Dbce_toggleCMD==0)?0:1;//1:data invert
	switch((QFidParaStsOp)QFidPara.sts_op){
		case _OP_Grp:
			Get_Grp_QFID=0;
			for(i=0 ; i<QFidPara.num_Slot ; i++){
				_grpIdx=i;
				_bitIdx=i*QFidPara.bit_Slot;//num=grpNum, bit=1
				if(_QFID_IS_IN_WINDOW(QFidBuf_H[_bitIdx],QFID_RX_DAT1_THDL,QFID_RX_GRP_MAX_LMT)){
					Get_Grp_QFID = Get_Grp_QFID | (1<<_grpIdx);
				}
			}
			break;

		case _OP_Tag:
			Get_Tag_QFID=0;
			for(i=0 ; i<QFidPara.num_Slot ; i++){
				_tagIdx=i;
				_bitIdx=i*QFidPara.bit_Slot;//num=tagNum, bit=2
				if(_invertCmd==0){
					sts = (_tagIdx&0x3);	//get tag lowest 2 bit to chk data
				} else {
					sts = ((~_tagIdx)&0x3);	//get tag lowest 2 bit(invert) to chk data
				}
				if(		_QFID_IS_IN_WINDOW(QFidBuf_H[_bitIdx],	_ThdTab[sts][0][0],_ThdTab[sts][0][1])
					&&	_QFID_IS_IN_WINDOW(QFidBuf_L[_bitIdx],	_ThdTab[sts][0][0],_ThdTab[sts][0][1])
					&&	_QFID_IS_IN_WINDOW(QFidBuf_H[_bitIdx+1],_ThdTab[sts][1][0],_ThdTab[sts][1][1])
					&&	_QFID_IS_IN_WINDOW(QFidBuf_L[_bitIdx+1],_ThdTab[sts][1][0],_ThdTab[sts][1][1])){
					Get_Tag_QFID = Get_Tag_QFID | (1<<_tagIdx);
				}
			}
			break;
#if	QFID_IS_USE_INPUT
		case _OP_Tag_IN:
			Get_Tag_QFID=0;
			Get_TagIN_QFID=0;
			for(i=0 ; i<QFidPara.num_Slot ; i++){
				_tagIdx=i;
				_bitIdx=i*QFidPara.bit_Slot;//num=tagNum, bit=5
				_inputBitIdx=_tagIdx*4;
				if(_invertCmd==0){
					sts = (_tagIdx&0x3);	//get tag lowest 2 bit to chk data
				} else {
					sts = ((~_tagIdx)&0x3);	//get tag lowest 2 bit(invert) to chk data
				}
				if(		_QFID_IS_IN_WINDOW(QFidBuf_H[_bitIdx+3],	_ThdTab[sts][0][0],_ThdTab[sts][0][1])	
					&&	_QFID_IS_IN_WINDOW(QFidBuf_L[_bitIdx+3],	_ThdTab[sts][0][0],_ThdTab[sts][0][1])	
					&&	_QFID_IS_IN_WINDOW(QFidBuf_H[_bitIdx+4],	_ThdTab[sts][1][0],_ThdTab[sts][1][1])	
					&&	_QFID_IS_IN_WINDOW(QFidBuf_L[_bitIdx+4],	_ThdTab[sts][1][0],_ThdTab[sts][1][1])){
					Get_Tag_QFID = Get_Tag_QFID | (1<<_tagIdx);
					if(		_QFID_IS_IN_WINDOW(QFidBuf_H[_bitIdx+0],_ThdTab[2][_invertCmd][0],_ThdTab[2][_invertCmd][1])
						&&	_QFID_IS_IN_WINDOW(QFidBuf_L[_bitIdx+0],_ThdTab[2][_invertCmd][0],_ThdTab[2][_invertCmd][1])){
						Get_TagIN_QFID = Get_TagIN_QFID |  (1<<(_inputBitIdx+2));//LSB first
					}
					if(		_QFID_IS_IN_WINDOW(QFidBuf_H[_bitIdx+1],_ThdTab[2][_invertCmd][0],_ThdTab[2][_invertCmd][1])
						&&	_QFID_IS_IN_WINDOW(QFidBuf_L[_bitIdx+1],_ThdTab[2][_invertCmd][0],_ThdTab[2][_invertCmd][1])){
						Get_TagIN_QFID = Get_TagIN_QFID |  (1<<(_inputBitIdx+1));
					}
					if(		_QFID_IS_IN_WINDOW(QFidBuf_H[_bitIdx+2],_ThdTab[2][_invertCmd][0],_ThdTab[2][_invertCmd][1])
						&&	_QFID_IS_IN_WINDOW(QFidBuf_L[_bitIdx+2],_ThdTab[2][_invertCmd][0],_ThdTab[2][_invertCmd][1])){
						Get_TagIN_QFID = Get_TagIN_QFID |  (1<<(_inputBitIdx+0));
					}
				}
			}
			break;
#endif
		default:
			break;//decode nothing for output option
	}
}
//------------------------------------------------------------------//
// QFID IO Contrl
// Parameter:
//          sts : ENABLE / DISABLE (1/0)
// return value:
//          NONE
//------------------------------------------------------------------//
inline static void QFID_IOctrl(U8 sts)
{
#if 	_QFID_TX_IO_TYPE==QFID_TYPE_IR
	#if 	_QFID_CARRIER==QFID_FREQ_125K
		IR_Cmd(_QFID_IR_TX_OUTPUT,sts);
	#elif	_QFID_CARRIER==QFID_FREQ_530K
        #if _EF_SERIES
        GPIO_Write(
            s_IR_Resource[_QFID_IR_TX_OUTPUT].port
            ,s_IR_Resource[_QFID_IR_TX_OUTPUT].pin
            ,sts
        );
        #else
		GPIO_Write(GPIOA, 5 , sts);
        #endif
	#endif										
#elif	_QFID_TX_IO_TYPE==QFID_TYPE_PWM 
	PWM_Cmd(PWMA,PWMx0,PWM_START_LOW,sts); 
#endif	
}
//------------------------------------------------------------------//
// QFID Timer ISR
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void QFID_TimerIsr(void)
{
	if(QFIDState != ENABLE || flag_Frame_end != 0)return;
	
	U8 i;
	switch((QFidIsrSts)QFidIsr.sts){
		case _STS_PRE_CHARGE:
			if(++QFidIsr.cnt_Level >= T_PRE_CHG){
				#if DEBUG_IO
				GPIOA->Data ^=(1<<13);
				#endif
				QFID_IOctrl(DISABLE);
				memset((void *)&QFidIsr, 0, sizeof(QFidIsr));
				QFidIsr.sts =_STS_TX_START;
			}
			break;
		
		/* Start ---------------------------------------------------*/
		case _STS_TX_START:
			if(++QFidIsr.cnt_Level >= T_START){
				#if DEBUG_IO
					GPIOA->Data ^=(1<<13);
				#endif
				QFID_IOctrl(ENABLE);
				memset((void *)&QFidIsr, 0, sizeof(QFidIsr));
				QFidIsr.Dat = Dat_QFID_CmdGrp;
				QFidIsr.sts =_STS_TX_CMD_GRP;
			}
			break;
		/* Send command & group */
		case _STS_TX_CMD_GRP:
			if(QFidIsr.flag==0)	{	
				QFidIsr.flag=1;

				if(QFidIsr.cnt_Bit>=1){
					QFidIsr.Dat>>=1;
				}

				if((QFidIsr.Dat&0x01)==1) {
					QFidIsr.tmp_H=T_TX_H_BIT_C;
					QFidIsr.tmp_L=T_TX_BIT_NC;
				} else {
					QFidIsr.tmp_H=T_TX_L_BIT_C;
					QFidIsr.tmp_L=T_TX_BIT_NC;
				}

				if(QFidIsr.cnt_Bit==5){ 	// 5nd bit
					QFidIsr.tmp_L=T_TX_L_5TH_BIT_NC;
				}
		
				QFidIsr.cnt_Level=0;
				QFidIsr.flag_Level=1;
			}

			if(QFidIsr.flag_Level){ 		//process hi of bit
				if(++QFidIsr.cnt_Level >= QFidIsr.tmp_H){
					#if DEBUG_IO
					GPIOA->Data ^=(1<<13);
					#endif
					QFID_IOctrl(DISABLE);
					QFidIsr.cnt_Level=0;
					QFidIsr.flag_Level=0;
				}
			} else {					//process lo of bit
				if(++QFidIsr.cnt_Level >= QFidIsr.tmp_L){
					#if DEBUG_IO
						GPIOA->Data ^=(1<<13);
					#endif
					QFID_IOctrl(ENABLE);
					QFidIsr.flag=0;
					if(++QFidIsr.cnt_Bit>=6){
						QFidIsr.cnt_Bit=0;
						QFidIsr.cnt_Level=0;
						do {
#if (_QFID_VID_PID)
							if(QFidPara.sts_op==_OP_Key){
								QFidIsr.sts=_STS_WAIT_TAG_SLT;
								break;
							}
#endif
#if (_QFID_OUTPUT_DATA)
							if(QFidPara.sts_op==_OP_Out){
								QFidIsr.sts=_STS_WAIT_TAG_SLT;
								break;
							}
#endif
							QFidIsr.sts=_STS_TX_ANT_STB;
						} while(0);
					}
				}
			}
			break;

		/* Wait for the antenna to stabilize */
		case _STS_TX_ANT_STB:
			if(++QFidIsr.cnt_Level >= T_ANT_STB){
				QFidIsr.cnt_Level=0;
				QFidIsr.flag_Level=1;
				QFidIsr.cnt_Bit=0;

				memset((void *)&QFidRx, 0, sizeof(QFidRx));
				idx_QFidBuf=0;

				#if DEBUG_IO
					GPIOA->Data ^=(1<<13);
				#endif
				QFidIsr.sts=_STS_RX_TAG;
			}
			break;

		/* Receive Tags response by time slot */
		case _STS_RX_TAG:
			if(++QFidIsr.cnt_Level <= QFidPara.time_Slot){
				switch((QFidRxSts)QFidRx.sts){
					case _STS1_RX_PRE_COND:
						if(GET_IO_QFID_RX==0) {
							if(QFidPara.sts_op==_OP_Grp){
								QFidRx.sts=_STS1_RX_PARSER0;
							} else {
								QFidRx.sts=_STS1_RX_PARSER1;
							}
						} else {
							QFidRx.sts=_STS1_RX_OVER;
						}
						break;

					case _STS1_RX_PARSER0://for group (1end bit)
						if(GET_IO_QFID_RX==1 && QFidIsr.flag_Level==1){
							QFidRx.cnt_Period_H++;	
							QFidRx.flag_StartCount=1;
						} else if(GET_IO_QFID_RX==0 && QFidRx.flag_StartCount==1){
							QFidBuf_H[idx_QFidBuf]=QFidRx.cnt_Period_H;
							QFidRx.cnt_Period_H=0;
							idx_QFidBuf++;
							QFidIsr.flag_Level=1;
							QFidRx.flag_ChkPoint=1;
							QFidRx.sts=_STS1_RX_OVER;
						}
						break;

					case _STS1_RX_PARSER1://for tag (2/5bit + 1 end bit)
						if(GET_IO_QFID_RX==1 && QFidIsr.flag_Level==1){
							QFidRx.cnt_Period_H++;
							QFidRx.flag_StartCount=1;
						} else if(GET_IO_QFID_RX==0 && QFidRx.flag_StartCount==1){
							QFidBuf_H[idx_QFidBuf]=QFidRx.cnt_Period_H;
							QFidRx.cnt_Period_L++;
							QFidIsr.flag_Level=0;
						} else if(GET_IO_QFID_RX==1 && QFidIsr.flag_Level==0 ){  //end of bit
							QFidBuf_L[idx_QFidBuf]=QFidRx.cnt_Period_L;
							QFidRx.cnt_Period_H=0;
							QFidRx.cnt_Period_L=0;
							idx_QFidBuf++;
							QFidRx.cnt_Period_H++;
							QFidIsr.flag_Level=1;
							QFidRx.flag_ChkPoint=1;
							if(++QFidIsr.cnt_Bit >= QFidPara.bit_Slot){
								QFidRx.sts=_STS1_RX_OVER;
							}
						}
						break;

					case _STS1_RX_OVER:
						break;
				}
			} else {
				QFidIsr.cnt_Level=0;
				QFidRx.cnt_Slot++;
				#if DEBUG_IO
					GPIOA->Data ^=(1<<13);
				#endif
				// (no signal ack in this time slot || no all pass, can't get chkpoint)
				if(QFidRx.flag_StartCount==0 || QFidRx.flag_ChkPoint==0) {
					for(i=0 ; i<QFidPara.bit_Slot ; i++) {// 以上狀況，皆填0 !
						QFidBuf_H[idx_QFidBuf]=0;
						QFidBuf_L[idx_QFidBuf]=0;
						idx_QFidBuf++;	
					}
				}

				QFidIsr.flag_Level=1;
				QFidIsr.cnt_Bit=0;

				QFidRx.sts=0;
				QFidRx.cnt_Period_H=0;
				QFidRx.cnt_Period_L=0;
				QFidRx.flag_StartCount=0;
				QFidRx.flag_ChkPoint=0;
			}

			if(QFidRx.cnt_Slot >= QFidPara.num_Slot){
				flag_Frame_end=1;
				QFidIsr.sts=_STS_PRE_CHARGE; //(_STS_TX_TAIL)
			}
			break;

#if (_QFID_VID_PID) || (_QFID_OUTPUT_DATA)
		/* 13ms Wait tag + 3ms Silent */
		case _STS_WAIT_TAG_SLT:
			if(QFidIsr.flag==0){
				if(++QFidIsr.cnt_Level >= T_WAIT_TAG){
					#if DEBUG_IO
						GPIOA->Data ^=(1<<13);
					#endif
					QFID_IOctrl(DISABLE);
					QFidIsr.cnt_Level=0;
					QFidIsr.flag=1;
				}
			} else {
				if(++QFidIsr.cnt_Level >= T_SILENT){
					#if DEBUG_IO
						GPIOA->Data ^=(1<<13);
					#endif
					QFID_IOctrl(ENABLE);	
					QFidIsr.cnt_Level=0;
					QFidIsr.flag=0;
					QFidIsr.Dat = Dat_QFID_TxOutDat;
					QFidIsr.sts=_STS_TX_OUT_DAT;
				}
			}
			break;

		/* Reader tx to tag */
		case _STS_TX_OUT_DAT:
			if(QFidIsr.flag==0){
				QFidIsr.flag=1;
				if(QFidIsr.cnt_Bit>=1)
					QFidIsr.Dat>>=1;
				if((QFidIsr.Dat&0x01)==1){
					QFidIsr.tmp_H=T_TX_H_BIT_C;
					QFidIsr.tmp_L=T_TX_BIT_NC;
				} else {
					QFidIsr.tmp_H=T_TX_L_BIT_C;
					QFidIsr.tmp_L=T_TX_BIT_NC;
				}
				QFidIsr.cnt_Level=0;
				QFidIsr.flag_Level=1;
			}

			if(QFidIsr.flag_Level){ 		//process hi of bit
				if(++QFidIsr.cnt_Level >= QFidIsr.tmp_H){
					#if DEBUG_IO
						GPIOA->Data ^=(1<<13);
					#endif
					QFID_IOctrl(DISABLE);	
					QFidIsr.cnt_Level=0;
					QFidIsr.flag_Level=0;
				}
			} else {					//process lo of bit
				if(++QFidIsr.cnt_Level >= QFidIsr.tmp_L){
					#if DEBUG_IO
						GPIOA->Data ^=(1<<13);
					#endif
					QFID_IOctrl(ENABLE);
					QFidIsr.flag=0;
					if(++QFidIsr.cnt_Bit>=QFidPara.bit_Slot){
						QFidIsr.cnt_Bit=0;
						QFidIsr.cnt_Level=0;
						QFidIsr.sts=_STS_TX_TAIL;
					}
				}
			}
			break;
#endif

		/* Tail (charge tag) */
		case _STS_TX_TAIL:
			if(++QFidIsr.cnt_Level >= QFidPara.time_tail){
				QFidIsr.cnt_Level=0;
				#if DEBUG_IO
					GPIOA->Data ^=(1<<13);
				#endif
				flag_Frame_end=1;
				QFidIsr.sts=_STS_PRE_CHARGE;
			}
			break;
	}		
}
//------------------------------------------------------------------//
// QFID control state change
// Parameter:
//          _s:		next state(Ref. QFidCtlSts)
// return value:
//     		NONE
//------------------------------------------------------------------//
static void QFID_CtlStsChange(QFidCtlSts _s)
{
	U8 _grp;
	switch(_s) {
#if _QFID_VID_PID
		case _STS_VID_PID:
			QFidCtl.sts=_STS_VID_PID;
			QFID_Send_Key(_QFID_VID, _QFID_PID);
			break;
#endif
		case _STS_SCAN_GRP:
#if _QFID_GROUP_NUM>2
			QFidCtl.sts=_STS_SCAN_GRP;
			QFID_RollCall_Grp();
			break;
#elif _QFID_GROUP_NUM==2
			Get_Grp_QFID=0x3;//for tag scan
#elif _QFID_GROUP_NUM==1
			Get_Grp_QFID=0x1;//for tag scan
#endif
		case _STS_SCAN_TAG:
			QFidCtl.sts=_STS_SCAN_TAG;
			_grp = Get_LS1B_Position(Get_Grp_QFID);//warning: Get_Grp_QFID can't be zero
#if	QFID_IS_USE_INPUT
			if( QFID_Type[_grp] == QFID_MODE_ID_INPUT ) {
				QFID_RollCall_Tag_Input(_grp);
			} else {
				QFID_RollCall_Tag(_grp);
			}
#else
			QFID_RollCall_Tag(_grp);
#endif
			break;
		default:
			break;
	}
}
//------------------------------------------------------------------//
// QFID Function turn On / Off
// Parameter:
//          cmd     : ENABLE / DISABLE (1/0)
// return value:
//     		NONE
//------------------------------------------------------------------//
void QFID_Func(U8 cmd)
{
	cmd = (cmd==DISABLE)?DISABLE:ENABLE;//input protect
	QFIDState = DISABLE;//blocking TimerIsr
	if(cmd==ENABLE){
		memset((void *)&QFidIsr, 0, sizeof(QFidIsr));
		memset((void *)&QFidRx, 0, sizeof(QFidRx));
		memset((void *)&QFidCtl, 0, sizeof(QFidCtl));
		memset((void *)&QFidDbce, 0, sizeof(QFidDbce));
	#if _QFID_VID_PID
		QFID_CtlStsChange(_STS_VID_PID);//first step
	#else
		QFID_CtlStsChange(_STS_SCAN_GRP);//first step
	#endif
		flag_Frame_end = 0;
	#ifdef	P_QFID_TIMER
	#if _EF_SERIES
		TIMER_Init(P_QFID_TIMER, QFID_RX_TIMER_TICK);
	#else
		TIMER_Init(P_QFID_TIMER,TIMER_CLKDIV_1, QFID_RX_TIMER_TICK);
	#endif
	#endif
	}
		
	GPIO_Write(_QFID_EN_PORT, _QFID_EN_PIN, cmd);
	QFID_IOctrl(cmd);
	
	//---
	#ifdef	P_QFID_TIMER
	TIMER_Cmd(P_QFID_TIMER,cmd);
	#endif
	
	QFIDState = cmd;
}
//------------------------------------------------------------------//
// QFID Initial
// Parameter:
//          NONE
// return value:
//     		NONE
//------------------------------------------------------------------//
void QFID_Init(void)
{
	memset((void *)&tmpInfo_QFID, 0, sizeof(tmpInfo_QFID));
	
	#if DEBUG_IO
		GPIO_Init(GPIOA,  13, GPIO_OUTPUT_HIGH);
		GPIO_Init(GPIOA,  14, GPIO_OUTPUT_HIGH);
	#endif
	
	
	GPIO_Init(_QFID_EN_PORT , _QFID_EN_PIN, GPIO_OUTPUT_LOW);
    GPIO_Init(_QFID_RX_PORT , _QFID_RX_PIN, GPIO_INPUT_FLOAT);
    GPIO_WakeUpCmd(_QFID_RX_PORT, _QFID_RX_PIN, DISABLE);

	#if 	_QFID_TX_IO_TYPE==QFID_TYPE_IR
        #if _EF_SERIES
            #if 	_QFID_CARRIER==QFID_FREQ_125K
            SMU_PeriphClkCmd(SMU_PERIPH_TMR,ENABLE);
            IR_Init(_QFID_IR_TX_OUTPUT,IR_STOP_LOW,125000);
		    IR_Cmd(_QFID_IR_TX_OUTPUT,DISABLE);
            #elif   _QFID_CARRIER==QFID_FREQ_530K
            GPIO_Init(
                s_IR_Resource[_QFID_IR_TX_OUTPUT].port
                ,s_IR_Resource[_QFID_IR_TX_OUTPUT].pin
                ,GPIO_OUTPUT_LOW
            );
            #endif
        #else                                                                               //IR (PA5)
            #if 	_QFID_CARRIER==QFID_FREQ_125K
		    SMU_PeriphClkCmd(SMU_PERIPH_IR,ENABLE);
            IR_Init(_QFID_IR_TX_OUTPUT,IR_STOP_LOW,125000);
		    IR_Cmd(_QFID_IR_TX_OUTPUT,DISABLE);
            #elif   _QFID_CARRIER==QFID_FREQ_530K
            GPIO_Init(GPIOA , 5 , GPIO_OUTPUT_LOW);	
            #endif
        #endif
	#elif	_QFID_TX_IO_TYPE==QFID_TYPE_PWM 												//PWM-IO (PA8)
		SMU_PeriphClkCmd(SMU_PERIPH_PWM,ENABLE);
		GPIO_Init(GPIOA,PWMA0_PIN,GPIO_MODE_ALT);
		#if 	_QFID_CARRIER==QFID_FREQ_125K
		#if _EF_SERIES
			TIMER_Init(PWMATIMER,(OPTION_HIGH_FREQ_CLK/125000)-1);	
		#else
			TIMER_Init(PWMATIMER,TIMER_CLKDIV_1,((OPTION_HIGH_FREQ_CLK)/125000)-1);	
		#endif
			TIMER_Cmd(PWMATIMER,ENABLE);
		#elif	_QFID_CARRIER==QFID_FREQ_530K && OPTION_HIGH_FREQ_CLK!=12000000				//Fsys=12MHz not support
		#if _EF_SERIES
			TIMER_Init(PWMATIMER,(OPTION_HIGH_FREQ_CLK/530000)-1);	
		#else
			TIMER_Init(PWMATIMER,TIMER_CLKDIV_1,(OPTION_HIGH_FREQ_CLK/530000)-1);	
		#endif
			TIMER_Cmd(PWMATIMER,ENABLE);
		#endif		
		PWM_InitDuty(PWMA,PWMx0,PWM_START_LOW,50);
		PWM_Cmd(PWMA,PWMx0,PWM_START_LOW,DISABLE);
	#endif

	#ifdef	P_QFID_TIMER
		//SMU_PeriphClkCmd(SMU_PERIPH_TMR,ENABLE);//init by Timer process @ sys.c
		//SMU_PeriphClkCmd(SMU_PERIPH_PWM,ENABLE);//init by PWM process @ sys.c
		TIMER_Cmd(P_QFID_TIMER,DISABLE);
	#else	//RTC_16K
		//SMU_PeriphClkCmd(SMU_PERIPH_RTC,ENABLE);//init by TIMEBASE process @ sys.c
		//RTC_IntCmd(RTC_16KHZ,DISABLE);//init @ sys.c
	#endif
		QFID_Func(ENABLE);	
}
//------------------------------------------------------------------//
// QFID TagInput Update
// Parameter:
//     		NONE
// return value:
//     		NONE
//------------------------------------------------------------------//
#if	QFID_IS_USE_INPUT
void QFID_TagInputUpdate(void)
{
	U32 _tagInMask=0;
	U8	_grp,_tagid;
	//copy tag-input to tmpInfo_QFID
	for(_grp=0;_grp<_QFID_GROUP_NUM;++_grp) {
		if(QFID_Type[_grp]!=QFID_MODE_ID_INPUT) continue;
		//only exist Tag can update input data 
		for(_tagid=0;_tagid<Tag_Num[_grp];++_tagid) {
			if((QFidDbce.tagIdNow[_grp]&BitOpTab(_tagid))==0) continue;
			_tagInMask+=(0xF<<(4*_tagid));
		}
		QFidDbce.tagInput[_grp] &= _tagInMask;//keep data
		tmpInfo_QFID.TagInput[_grp] &= (~_tagInMask);//delete data
		tmpInfo_QFID.TagInput[_grp] |= QFidDbce.tagInput[_grp];//merge data
	}
}
#endif
//------------------------------------------------------------------//
// QFID TagID debounce
// Parameter:
//     		NONE
// return value:
//     		NONE
//------------------------------------------------------------------//
void QFID_TagIdDebounce(void)
{
	U16 _eventDetLev;
	U8	_grp,_tagid;
	//TagID detect debounce process
	for(_grp=0;_grp<_QFID_GROUP_NUM;++_grp){
		_eventDetLev = (QFidDbce.tagIdNow[_grp] ^ tmpInfo_QFID.TagID[_grp]) & QFidDbce.tagIdNow[_grp];
		if(_eventDetLev == 0){
			QFidDbce.tagIdPrevDet[_grp] = 0;
			QFidDbce.cntDet[_grp] = 0;
			continue;//next grp
		}
		if(_eventDetLev != QFidDbce.tagIdPrevDet[_grp]){
			QFidDbce.tagIdPrevDet[_grp] = _eventDetLev;
			QFidDbce.cntDet[_grp] = QFID_DETECT_DEBOUNCE;
			continue;//next grp
		}
		if(QFidDbce.cntDet[_grp] > 1){
			--QFidDbce.cntDet[_grp];
			continue;//next grp
		}
		QFidDbce.cntDet[_grp] = 0;//debounce end
		tmpInfo_QFID.TagID[_grp] ^= _eventDetLev;//Merge detect event to tmpInfo_QFID
		while(_eventDetLev!=0){
			_tagid = Get_LS1B_Position(_eventDetLev);
			CB_QfidFunc_Grp_Tag_detect(_grp,_tagid);
			_eventDetLev &= ~BitOpTab(_tagid);
		}
	}
	//TagID leave debounce process
	for(_grp=0;_grp<_QFID_GROUP_NUM;++_grp){
		_eventDetLev = (QFidDbce.tagIdNow[_grp] ^ tmpInfo_QFID.TagID[_grp]) & tmpInfo_QFID.TagID[_grp];
		if(_eventDetLev == 0){
			QFidDbce.tagIdPrevLev[_grp] = 0;
			QFidDbce.cntLev[_grp] = 0;
			continue;//next grp
		}
		if(_eventDetLev != QFidDbce.tagIdPrevLev[_grp]){
			QFidDbce.tagIdPrevLev[_grp] = _eventDetLev;
			QFidDbce.cntLev[_grp] = QFID_LEAVE_DEBOUNCE;
			continue;//next grp
		}
		if(QFidDbce.cntLev[_grp] > 1){
			--QFidDbce.cntLev[_grp];
			continue;
		}
		QFidDbce.cntLev[_grp] = 0;//debounce end
		tmpInfo_QFID.TagID[_grp] ^= _eventDetLev;//Merge leave event to tmpInfo_QFID
		while(_eventDetLev!=0){
			_tagid = Get_LS1B_Position(_eventDetLev);
			CB_QfidFunc_Grp_Tag_leave(_grp,_tagid);
			_eventDetLev &= ~BitOpTab(_tagid);
		}
	}
}
//------------------------------------------------------------------//
// QFID service loop
// Parameter:
//     		NONE
// return value:
//     		NONE
//------------------------------------------------------------------//
void QFID_ServiceLoop(void)
{
	if(QFIDState != ENABLE || flag_Frame_end == 0)return;
	
	U8 _grp;

	QFID_DecodeFrame();
	switch((QFidCtlSts)QFidCtl.sts) {//---Scan Groups---
#if _QFID_VID_PID
		case _STS_VID_PID:
			QFID_CtlStsChange(_STS_SCAN_GRP);//next step
			break;
#endif

#if _QFID_GROUP_NUM>2
		case _STS_SCAN_GRP:	
			if(Get_Grp_QFID>0) {
				for(_grp=0; _grp<_QFID_GROUP_NUM;++_grp) {
					if((Get_Grp_QFID&BitOpTab(_grp))!=0) continue;
					QFidDbce.tagIdNow[_grp]=0;//no tag id get
#if	QFID_IS_USE_INPUT
					if(QFID_Type[_grp]!=QFID_MODE_ID_INPUT) continue;
					QFidDbce.tagInput[_grp]=0;//no tag input get
#endif
				}
				QFID_CtlStsChange(_STS_SCAN_TAG);//next step
			} else {//Not detect any group
				memset((void *)QFidDbce.tagIdNow, 0, sizeof(QFidDbce.tagIdNow));
#if	QFID_IS_USE_INPUT
				memset((void *)QFidDbce.tagInput, 0, sizeof(QFidDbce.tagInput));
#endif
				QFID_TagIdDebounce();
#if _QFID_VID_PID
				QFID_CtlStsChange(_STS_VID_PID);//first step
#else
				QFID_CtlStsChange(_STS_SCAN_GRP);//first step
#endif
			}
		break;
#endif

		//Scan Tag for existing group (include debounce frame)- - - -
		case _STS_SCAN_TAG:	
			_grp = Get_LS1B_Position(Get_Grp_QFID);//warning: Get_Grp_QFID can't be zero
			QFidDbce.tagIdNow[_grp]=Get_Tag_QFID;
#if	QFID_IS_USE_INPUT
			if(QFID_Type[_grp]==QFID_MODE_ID_INPUT) {
				QFidDbce.tagInput[_grp]=Get_TagIN_QFID;	
			}
#endif
			//toggle cmd & next group
			Get_Grp_QFID &= ~(0x1<<_grp);
			if(Get_Grp_QFID == 0){
				qfid_Dbce_toggleCMD^=1;
#if	QFID_IS_USE_INPUT
				QFID_TagInputUpdate();
#endif
				QFID_TagIdDebounce();
#if _QFID_VID_PID
				QFID_CtlStsChange(_STS_VID_PID);//first step
#else
				QFID_CtlStsChange(_STS_SCAN_GRP);//first step
#endif
			} else {
				QFID_CtlStsChange(_STS_SCAN_TAG);//next tag of same grp
			}
			break;
		default:
			break;
	}
	flag_Frame_end = 0;
}	
//------------------------------------------------------------------//
// QFID get group information
//  - single group has no group concept, it return '0'.
// Parameter:
//          NONE
// return value:
//          group information (MSB=group7 ; LSB=group0)
//------------------------------------------------------------------//
U8 QFID_GetGroup(void)
{
	U8 cal_Grp=0, idx;
	for(idx=0;idx<_QFID_GROUP_NUM;++idx) {
		cal_Grp+=((tmpInfo_QFID.TagID[idx]!=0)<<idx);
	}
	return cal_Grp;
}
//------------------------------------------------------------------//
// QFID get tagid
// Parameter:
//          grp	: group no.	(0~7)
// return value:
//          tag information in the group (MSB=Tag15 ; LSB=Tag0)
//------------------------------------------------------------------//
U16 QFID_GetTagID(U8 grp)
{
	if(grp>=_QFID_GROUP_NUM)return 0;//Illegal input
	
	return tmpInfo_QFID.TagID[grp];
}
//------------------------------------------------------------------//
// QFID get tag input
// Parameter:
//          grp	  : group no. (0~3)
//			tagid : tag id	  (0~7)
// return value:
//          3bit input data for specific tag
//------------------------------------------------------------------//
U8  QFID_GetTagInput(U8 grp, U8 tagid)
{
#if	QFID_IS_USE_INPUT
	if(		grp>=_QFID_GROUP_NUM
		||	tagid>=Tag_Num[grp]
		||	QFID_Type[grp]!=QFID_MODE_ID_INPUT
		||	(tmpInfo_QFID.TagID[grp]&BitOpTab(tagid))==0
	)return 0;//Illegal input
	
	return (tmpInfo_QFID.TagInput[grp]>>(4*tagid))&0xf;
#else
	return 0;
#endif
}
//------------------------------------------------------------------//
// Get QFID status
// Parameter: 
//          NONE
// return value:
//          ENABLE(1)/DISABLE(0)
//------------------------------------------------------------------// 
U8 QFID_GetStatus(void)
{
    return  QFIDState;
}
//------------------------------------------------------------------//
// QFID output data
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
#if _QFID_OUTPUT_DATA
void CB_QFID_OutData(void)
{
    QFID_Send_OutData(0x0, 0x1, 0xB); //(grp, id, out_data)
}
#endif

#endif // end-off _QFID_MODULE
