#include "TouchTab.h"
#if _TOUCHKEY_MODULE
typedef struct {
	U32 	u32BGValue;
	U32 	u32CrrntValue;
	U32 	u32BGValueSum;
	U8 		u8TouchFlag;
	U8 		u8BGStabTimes;
	U8 		u8BGFailTimes;
	U8 		u8BGFail2Times;
	U8 		u8BGLevelFlag;
	U8 		u8StabStatus;
}STouchPAD;
volatile STouchPAD TouchPAD[C_TouchMaxPAD];

typedef const struct {
	U32 u32PresThreshold[4];
    U32 u32RelsThreshold[4];
    U8  u8PresDebounceTimes;
    U8  u8RelsDebounceTimes;
    U8  u8InputPin;
    U8  u8OutputPin;
} STouchPadPara;
STouchPadPara TouchPadPara[C_TouchMaxPAD] = 
{
#if C_TouchMaxPAD>=1//TOUCH_PAD0
	{
        .u32PresThreshold       = { (C_PAD0_DiffCnt*0.6),
                                    (C_PAD0_DiffCnt*0.7),
                                    (C_PAD0_DiffCnt*0.8),
                                    (C_PAD0_DiffCnt*0.9), },
        .u32RelsThreshold       = { (C_PAD0_DiffCnt*0.4),
                                    (C_PAD0_DiffCnt*0.3),
                                    (C_PAD0_DiffCnt*0.2),
                                    (C_PAD0_DiffCnt*0.1), },
        .u8PresDebounceTimes    = C_TouchDebounce,
        .u8RelsDebounceTimes    = C_TouchDebounce,
        .u8InputPin             = C_PAD0_IN,
        .u8OutputPin            = C_PAD0_Out,
	},
#endif
#if C_TouchMaxPAD>=2//TOUCH_PAD1
	{
        .u32PresThreshold       = { (C_PAD1_DiffCnt*0.6),
                                    (C_PAD1_DiffCnt*0.7),
                                    (C_PAD1_DiffCnt*0.8),
                                    (C_PAD1_DiffCnt*0.9), },
        .u32RelsThreshold       = { (C_PAD1_DiffCnt*0.4),
                                    (C_PAD1_DiffCnt*0.3),
                                    (C_PAD1_DiffCnt*0.2),
                                    (C_PAD1_DiffCnt*0.1), },
        .u8PresDebounceTimes    = C_TouchDebounce,
        .u8RelsDebounceTimes    = C_TouchDebounce,
        .u8InputPin             = C_PAD1_IN,
        .u8OutputPin            = C_PAD1_Out,
	},
#endif
#if C_TouchMaxPAD>=3//TOUCH_PAD2
	{
        .u32PresThreshold       = { (C_PAD2_DiffCnt*0.6),
                                    (C_PAD2_DiffCnt*0.7),
                                    (C_PAD2_DiffCnt*0.8),
                                    (C_PAD2_DiffCnt*0.9), },
        .u32RelsThreshold       = { (C_PAD2_DiffCnt*0.4),
                                    (C_PAD2_DiffCnt*0.3),
                                    (C_PAD2_DiffCnt*0.2),
                                    (C_PAD2_DiffCnt*0.1), },
        .u8PresDebounceTimes    = C_TouchDebounce,
        .u8RelsDebounceTimes    = C_TouchDebounce,
        .u8InputPin             = C_PAD2_IN,
        .u8OutputPin            = C_PAD2_Out,
	},
#endif
#if C_TouchMaxPAD>=4//TOUCH_PAD3
	{
        .u32PresThreshold       = { (C_PAD3_DiffCnt*0.6),
                                    (C_PAD3_DiffCnt*0.7),
                                    (C_PAD3_DiffCnt*0.8),
                                    (C_PAD3_DiffCnt*0.9), },
        .u32RelsThreshold       = { (C_PAD3_DiffCnt*0.4),
                                    (C_PAD3_DiffCnt*0.3),
                                    (C_PAD3_DiffCnt*0.2),
                                    (C_PAD3_DiffCnt*0.1), },
        .u8PresDebounceTimes    = C_TouchDebounce,
        .u8RelsDebounceTimes    = C_TouchDebounce,
        .u8InputPin             = C_PAD3_IN,
        .u8OutputPin            = C_PAD3_Out,
	},
#endif
#if C_TouchMaxPAD>=5//TOUCH_PAD4
	{
        .u32PresThreshold       = { (C_PAD4_DiffCnt*0.6),
                                    (C_PAD4_DiffCnt*0.7),
                                    (C_PAD4_DiffCnt*0.8),
                                    (C_PAD4_DiffCnt*0.9), },
        .u32RelsThreshold       = { (C_PAD4_DiffCnt*0.4),
                                    (C_PAD4_DiffCnt*0.3),
                                    (C_PAD4_DiffCnt*0.2),
                                    (C_PAD4_DiffCnt*0.1), },
        .u8PresDebounceTimes    = C_TouchDebounce,
        .u8RelsDebounceTimes    = C_TouchDebounce,
        .u8InputPin             = C_PAD4_IN,
        .u8OutputPin            = C_PAD4_Out,
	},
#endif
#if C_TouchMaxPAD>=6//TOUCH_PAD5
	{
        .u32PresThreshold       = { (C_PAD5_DiffCnt*0.6),
                                    (C_PAD5_DiffCnt*0.7),
                                    (C_PAD5_DiffCnt*0.8),
                                    (C_PAD5_DiffCnt*0.9), },
        .u32RelsThreshold       = { (C_PAD5_DiffCnt*0.4),
                                    (C_PAD5_DiffCnt*0.3),
                                    (C_PAD5_DiffCnt*0.2),
                                    (C_PAD5_DiffCnt*0.1), },
        .u8PresDebounceTimes    = C_TouchDebounce,
        .u8RelsDebounceTimes    = C_TouchDebounce,
        .u8InputPin             = C_PAD5_IN,
        .u8OutputPin            = C_PAD5_Out,
	},
#endif
#if C_TouchMaxPAD>=7//TOUCH_PAD6
	{
        .u32PresThreshold       = { (C_PAD6_DiffCnt*0.6),
                                    (C_PAD6_DiffCnt*0.7),
                                    (C_PAD6_DiffCnt*0.8),
                                    (C_PAD6_DiffCnt*0.9), },
        .u32RelsThreshold       = { (C_PAD6_DiffCnt*0.4),
                                    (C_PAD6_DiffCnt*0.3),
                                    (C_PAD6_DiffCnt*0.2),
                                    (C_PAD6_DiffCnt*0.1), },
        .u8PresDebounceTimes    = C_TouchDebounce,
        .u8RelsDebounceTimes    = C_TouchDebounce,
        .u8InputPin             = C_PAD6_IN,
        .u8OutputPin            = C_PAD6_Out,
	},
#endif
#if C_TouchMaxPAD>=8//TOUCH_PAD7
	{
        .u32PresThreshold       = { (C_PAD7_DiffCnt*0.6),
                                    (C_PAD7_DiffCnt*0.7),
                                    (C_PAD7_DiffCnt*0.8),
                                    (C_PAD7_DiffCnt*0.9), },
        .u32RelsThreshold       = { (C_PAD7_DiffCnt*0.4),
                                    (C_PAD7_DiffCnt*0.3),
                                    (C_PAD7_DiffCnt*0.2),
                                    (C_PAD7_DiffCnt*0.1), },
        .u8PresDebounceTimes    = C_TouchDebounce,
        .u8RelsDebounceTimes    = C_TouchDebounce,
        .u8InputPin             = C_PAD7_IN,
        .u8OutputPin            = C_PAD7_Out,
	},
#endif
#if C_TouchMaxPAD>=9//TOUCH_PAD8
	{
        .u32PresThreshold       = { (C_PAD8_DiffCnt*0.6),
                                    (C_PAD8_DiffCnt*0.7),
                                    (C_PAD8_DiffCnt*0.8),
                                    (C_PAD8_DiffCnt*0.9), },
        .u32RelsThreshold       = { (C_PAD8_DiffCnt*0.4),
                                    (C_PAD8_DiffCnt*0.3),
                                    (C_PAD8_DiffCnt*0.2),
                                    (C_PAD8_DiffCnt*0.1), },
        .u8PresDebounceTimes    = C_TouchDebounce,
        .u8RelsDebounceTimes    = C_TouchDebounce,
        .u8InputPin             = C_PAD8_IN,
        .u8OutputPin            = C_PAD8_Out,
	},
#endif
#if C_TouchMaxPAD>=10//TOUCH_PAD9
	{
        .u32PresThreshold       = { (C_PAD9_DiffCnt*0.6),
                                    (C_PAD9_DiffCnt*0.7),
                                    (C_PAD9_DiffCnt*0.8),
                                    (C_PAD9_DiffCnt*0.9), },
        .u32RelsThreshold       = { (C_PAD9_DiffCnt*0.4),
                                    (C_PAD9_DiffCnt*0.3),
                                    (C_PAD9_DiffCnt*0.2),
                                    (C_PAD9_DiffCnt*0.1), },
        .u8PresDebounceTimes    = C_TouchDebounce,
        .u8RelsDebounceTimes    = C_TouchDebounce,
        .u8InputPin             = C_PAD9_IN,
        .u8OutputPin            = C_PAD9_Out,
	},
#endif
#if C_TouchMaxPAD>=11//TOUCH_PAD10
	{
        .u32PresThreshold       = { (C_PAD10_DiffCnt*0.6),
                                    (C_PAD10_DiffCnt*0.7),
                                    (C_PAD10_DiffCnt*0.8),
                                    (C_PAD10_DiffCnt*0.9), },
        .u32RelsThreshold       = { (C_PAD10_DiffCnt*0.4),
                                    (C_PAD10_DiffCnt*0.3),
                                    (C_PAD10_DiffCnt*0.2),
                                    (C_PAD10_DiffCnt*0.1), },
        .u8PresDebounceTimes    = C_TouchDebounce,
        .u8RelsDebounceTimes    = C_TouchDebounce,
        .u8InputPin             = C_PAD10_IN,
        .u8OutputPin            = C_PAD10_Out,
	},
#endif
#if C_TouchMaxPAD>=12//TOUCH_PAD11
	{
        .u32PresThreshold       = { (C_PAD11_DiffCnt*0.6),
                                    (C_PAD11_DiffCnt*0.7),
                                    (C_PAD11_DiffCnt*0.8),
                                    (C_PAD11_DiffCnt*0.9), },
        .u32RelsThreshold       = { (C_PAD11_DiffCnt*0.4),
                                    (C_PAD11_DiffCnt*0.3),
                                    (C_PAD11_DiffCnt*0.2),
                                    (C_PAD11_DiffCnt*0.1), },
        .u8PresDebounceTimes    = C_TouchDebounce,
        .u8RelsDebounceTimes    = C_TouchDebounce,
        .u8InputPin             = C_PAD11_IN,
        .u8OutputPin            = C_PAD11_Out,
	},
#endif
#if C_TouchMaxPAD>=13//TOUCH_PAD12
	{
        .u32PresThreshold       = { (C_PAD12_DiffCnt*0.6),
                                    (C_PAD12_DiffCnt*0.7),
                                    (C_PAD12_DiffCnt*0.8),
                                    (C_PAD12_DiffCnt*0.9), },
        .u32RelsThreshold       = { (C_PAD12_DiffCnt*0.4),
                                    (C_PAD12_DiffCnt*0.3),
                                    (C_PAD12_DiffCnt*0.2),
                                    (C_PAD12_DiffCnt*0.1), },
        .u8PresDebounceTimes    = C_TouchDebounce,
        .u8RelsDebounceTimes    = C_TouchDebounce,
        .u8InputPin             = C_PAD12_IN,
        .u8OutputPin            = C_PAD12_Out,
	},
#endif
#if C_TouchMaxPAD>=14//TOUCH_PAD13
	{
        .u32PresThreshold       = { (C_PAD13_DiffCnt*0.6),
                                    (C_PAD13_DiffCnt*0.7),
                                    (C_PAD13_DiffCnt*0.8),
                                    (C_PAD13_DiffCnt*0.9), },
        .u32RelsThreshold       = { (C_PAD13_DiffCnt*0.4),
                                    (C_PAD13_DiffCnt*0.3),
                                    (C_PAD13_DiffCnt*0.2),
                                    (C_PAD13_DiffCnt*0.1), },
        .u8PresDebounceTimes    = C_TouchDebounce,
        .u8RelsDebounceTimes    = C_TouchDebounce,
        .u8InputPin             = C_PAD13_IN,
        .u8OutputPin            = C_PAD13_Out,
	},
#endif
#if C_TouchMaxPAD>=15//TOUCH_PAD14
	{
        .u32PresThreshold       = { (C_PAD14_DiffCnt*0.6),
                                    (C_PAD14_DiffCnt*0.7),
                                    (C_PAD14_DiffCnt*0.8),
                                    (C_PAD14_DiffCnt*0.9), },
        .u32RelsThreshold       = { (C_PAD14_DiffCnt*0.4),
                                    (C_PAD14_DiffCnt*0.3),
                                    (C_PAD14_DiffCnt*0.2),
                                    (C_PAD14_DiffCnt*0.1), },
        .u8PresDebounceTimes    = C_TouchDebounce,
        .u8RelsDebounceTimes    = C_TouchDebounce,
        .u8InputPin             = C_PAD14_IN,
        .u8OutputPin            = C_PAD14_Out,
	},
#endif
#if C_TouchMaxPAD>=16//TOUCH_PAD15
	{
        .u32PresThreshold       = { (C_PAD15_DiffCnt*0.6),
                                    (C_PAD15_DiffCnt*0.7),
                                    (C_PAD15_DiffCnt*0.8),
                                    (C_PAD15_DiffCnt*0.9), },
        .u32RelsThreshold       = { (C_PAD15_DiffCnt*0.4),
                                    (C_PAD15_DiffCnt*0.3),
                                    (C_PAD15_DiffCnt*0.2),
                                    (C_PAD15_DiffCnt*0.1), },
        .u8PresDebounceTimes    = C_TouchDebounce,
        .u8RelsDebounceTimes    = C_TouchDebounce,
        .u8InputPin             = C_PAD15_IN,
        .u8OutputPin            = C_PAD15_Out,
	}
#endif
};

#if C_TouchSleepFrame>=500
#define TouchRtcSrc (2)
#else
#define TouchRtcSrc (64)
#endif

typedef const struct {
	U8  u8BgCalTimes;
    U8  u8BgLevelTimes;
    U8  u8BgThreshold0Idx;
    U8  u8BgThreshold1Idx;
    U8  u8BgStabTimesIdx;
    U8  u8BgFailTimesIdx;
    U8  u8BgFailMaxCount;
    U8  u8IndexPadCount;
    U8  u8IndexMaxConvertTimes;
    U8  u8IndexNormalCalTime;
    U8  u8IndexSleepCalTime;
    U8  u8IndexNormalFrame;
    U8  u8IndexSleepFrame;
    U8  u8Rtc1sSource;
    U8  u8CaptureSourceInUs;
} STouchCtlPara;
STouchCtlPara TouchCtlPara = 
{
    .u8BgCalTimes               =   3,
    .u8BgLevelTimes             =   5,
    .u8BgThreshold0Idx          =   10,
    .u8BgThreshold1Idx          =   50,
    .u8BgStabTimesIdx           =   16,
    .u8BgFailTimesIdx           =   2,
    .u8BgFailMaxCount           =   125,
    .u8IndexPadCount            =   C_TouchMaxPAD,
    .u8IndexMaxConvertTimes     =   16,
    .u8IndexNormalCalTime       =   C_TouchCALTime,
    .u8IndexSleepCalTime        =   C_TouchSleepCALTime,
    .u8IndexNormalFrame         =   (((C_TouchNormalFrame/C_TouchMaxPAD)>=1)?(C_TouchNormalFrame/C_TouchMaxPAD):1),
    .u8IndexSleepFrame          =   (((C_TouchSleepFrame/(1000/TouchRtcSrc)>=1))?(C_TouchSleepFrame/(1000/TouchRtcSrc)):1),
    .u8Rtc1sSource              =   TouchRtcSrc,
    .u8CaptureSourceInUs        =   (OPTION_HIGH_FREQ_CLK/1000000),
};
#endif