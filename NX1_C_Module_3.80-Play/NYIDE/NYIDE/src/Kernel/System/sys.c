/* Includes ------------------------------------------------------------------*/
#include "include.h"
#include "nx1_lib.h"
#include "nx1_smu.h"
#include "nx1_rtc.h"
#include "nx1_spi.h"
#include "nx1_uart.h"
#include "nx1_timer.h"
#include "nx1_intc.h"
#include "nx1_gpio.h"
#include "nx1_wdt.h"
#include "nx1_pwm.h"
#include "nx1_ir.h"
#include "nx1_dac.h"
#include "nx1_adc.h"
#include "nx1_i2c.h"
#include "TouchTab.h"
#include "debug.h"
#include "user_main.h"
/* Defines -------------------------------------------------------------------*/
#if (_SYS_TIMEBASE!=TIMEBASE_250us) \
    &&(_SYS_TIMEBASE!=TIMEBASE_62us)\
    &&(_SYS_TIMEBASE!=TIMEBASE_1000us)
#error _SYS_TIMEBASE undefined
#endif

#if !(body_is_4KHZ) \
    &&  (   (_SYS_TIMEBASE==TIMEBASE_250us) \
        ||  (_SYS_TIMEBASE==TIMEBASE_62us) \
        ||  (_IR_MODULE&&_IR_RX_EN) \
        ||  (_IR_MODULE&&_IR_TX_EN) \
        ||  (_QFID_MODULE && (_QFID_TIMER == USE_RTC)) \
        ||  (_QCODE_BUILD && _RTC_16384Hz_PATH) \
        )
#ifndef NORMAL_USE_RTC_16384HZ
#define NORMAL_USE_RTC_16384HZ
#endif
#endif

#if body_is_4KHZ \
    &&  (   (_SYS_TIMEBASE==TIMEBASE_250us) \
        ||  (_IR_MODULE&&_IR_RX_EN) \
        ||  (_IR_MODULE&&_IR_TX_EN) \
        ||  (_QCODE_BUILD && _RTC_4096Hz_PATH) \
        )
#ifndef NORMAL_USE_RTC_4096HZ
#define NORMAL_USE_RTC_4096HZ
#endif
#endif

#if     (_SYS_TIMEBASE==TIMEBASE_1000us) \
    ||  (_QCODE_BUILD && _RTC_1024Hz_PATH)
#ifndef NORMAL_USE_RTC_1024HZ
#define NORMAL_USE_RTC_1024HZ
#endif
#endif

#if     (_TOUCHKEY_MODULE && C_TouchSleepFrame<500) \
    ||  (_QCODE_BUILD && _RTC_64Hz_PATH)
#ifndef NORMAL_USE_RTC_64HZ
#define NORMAL_USE_RTC_64HZ
#endif
#endif

#if     (_TOUCHKEY_MODULE && C_TouchSleepFrame>=500) \
    ||  (_QCODE_BUILD && _RTC_2Hz_PATH) \
    ||  (_QCODE_BUILD && _500ms_PATH)
#ifndef NORMAL_USE_RTC_2HZ
#define NORMAL_USE_RTC_2HZ
#endif
#endif

#if (_TOUCHKEY_MODULE && _TOUCHKEY_INTO_STANDBY && C_TouchSleepFrame<500)
#ifndef SLEEP_USE_RTC_64HZ
#define SLEEP_USE_RTC_64HZ
#endif
#endif

#if (_TOUCHKEY_MODULE && _TOUCHKEY_INTO_STANDBY && C_TouchSleepFrame>=500) \
    ||  (body_MultiTrgLdo) \
    ||  (_QCODE_BUILD && _SLEEP_MODE_TYPE==1)
#ifndef SLEEP_USE_RTC_2HZ
#define SLEEP_USE_RTC_2HZ
#endif
#endif

#if     _IR_LOW_POWER_TYPE==TYPE1
#if _IR_RX_TIMER==USE_RTC
	#ifndef SLEEP_USE_RTC_2HZ
	#define SLEEP_USE_RTC_2HZ
	#endif
#elif _IR_RX_TIMER==USE_TIMER0
	#ifndef SLEEP_USE_TIMER0
	#define SLEEP_USE_TIMER0
	#endif
#elif _IR_RX_TIMER==USE_TIMER1
	#ifndef SLEEP_USE_TIMER1
	#define SLEEP_USE_TIMER1
	#endif
#elif _IR_RX_TIMER==USE_TIMER2
	#ifndef SLEEP_USE_TIMER2
	#define SLEEP_USE_TIMER2
	#endif
#elif _IR_RX_TIMER==USE_TIMER3
	#ifndef SLEEP_USE_TIMER3
	#define SLEEP_USE_TIMER3
	#endif
#endif
#elif   _IR_LOW_POWER_TYPE==TYPE2
	#ifndef SLEEP_USE_EXT0_INT
	#define SLEEP_USE_EXT0_INT
	#endif
#endif

const U8 NORMAL_RTC_SET = ( 0
#if !body_is_4KHZ && defined(NORMAL_USE_RTC_16384HZ)
        |   RTC_16KHZ
#endif
#if body_is_4KHZ && defined(NORMAL_USE_RTC_4096HZ)
        |   RTC_4KHZ
#endif
#if defined(NORMAL_USE_RTC_1024HZ)
        |   RTC_1KHZ
#endif
#if defined(NORMAL_USE_RTC_64HZ)
        |   RTC_64HZ
#endif
#if defined(NORMAL_USE_RTC_2HZ)
        |   RTC_2HZ
#endif
    );

const U8 SLEEP_RTC_SET = ( 0
#if defined(SLEEP_USE_RTC_64HZ)
        |   RTC_64HZ
#endif
#if defined(SLEEP_USE_RTC_2HZ)
        |   RTC_2HZ
#endif
    );

const U32 SLEEP_CLEAR_IRQ_PENDING = ( 0
#if defined(SLEEP_USE_RTC_64HZ) || defined(SLEEP_USE_RTC_2HZ)
		|	IRQ_BIT_RTC
#endif
#if defined(SLEEP_USE_TIMER0)
		|	IRQ_BIT_TMR0
#endif
#if defined(SLEEP_USE_TIMER1)
		|	IRQ_BIT_TMR1
#endif
#if defined(SLEEP_USE_TIMER2)
		|	IRQ_BIT_TMR2
#endif
#if defined(SLEEP_USE_TIMER3)
		|	IRQ_BIT_TMR3
#endif
#if defined(SLEEP_USE_EXT0_INT)
		|	IRQ_BIT_EXIT0
#endif
    );

#define RANDOM_AI                   41
#define RANDOM_CI                   7

#define DELAY_SLEEP_TIME			250//ms

#define GPIOA_INPUT_PULLHIGH		(((_GPIO_PA0_MODE == GPIO_INPUT_PULLHIGH)<<0) | ((_GPIO_PA1_MODE == GPIO_INPUT_PULLHIGH)<<1) | ((_GPIO_PA2_MODE == GPIO_INPUT_PULLHIGH)<<2) | ((_GPIO_PA3_MODE == GPIO_INPUT_PULLHIGH)<<3) | \
									((_GPIO_PA4_MODE == GPIO_INPUT_PULLHIGH)<<4) | ((_GPIO_PA5_MODE == GPIO_INPUT_PULLHIGH)<<5) | ((_GPIO_PA6_MODE == GPIO_INPUT_PULLHIGH)<<6) | ((_GPIO_PA7_MODE == GPIO_INPUT_PULLHIGH)<<7) | \
									((_GPIO_PA8_MODE == GPIO_INPUT_PULLHIGH)<<8) | ((_GPIO_PA9_MODE == GPIO_INPUT_PULLHIGH)<<9) | ((_GPIO_PA10_MODE == GPIO_INPUT_PULLHIGH)<<10) | ((_GPIO_PA11_MODE == GPIO_INPUT_PULLHIGH)<<11) | \
									((_GPIO_PA12_MODE == GPIO_INPUT_PULLHIGH)<<12) | ((_GPIO_PA13_MODE == GPIO_INPUT_PULLHIGH)<<13) | ((_GPIO_PA14_MODE == GPIO_INPUT_PULLHIGH)<<14) | ((_GPIO_PA15_MODE == GPIO_INPUT_PULLHIGH)<<15))
#define GPIOA_INPUT_FLOAT			(((_GPIO_PA0_MODE == GPIO_INPUT_FLOAT)<<0) | ((_GPIO_PA1_MODE == GPIO_INPUT_FLOAT)<<1) | ((_GPIO_PA2_MODE == GPIO_INPUT_FLOAT)<<2) | ((_GPIO_PA3_MODE == GPIO_INPUT_FLOAT)<<3) | \
									((_GPIO_PA4_MODE == GPIO_INPUT_FLOAT)<<4) | ((_GPIO_PA5_MODE == GPIO_INPUT_FLOAT)<<5) | ((_GPIO_PA6_MODE == GPIO_INPUT_FLOAT)<<6) | ((_GPIO_PA7_MODE == GPIO_INPUT_FLOAT)<<7) | \
									((_GPIO_PA8_MODE == GPIO_INPUT_FLOAT)<<8) | ((_GPIO_PA9_MODE == GPIO_INPUT_FLOAT)<<9) | ((_GPIO_PA10_MODE == GPIO_INPUT_FLOAT)<<10) | ((_GPIO_PA11_MODE == GPIO_INPUT_FLOAT)<<11) | \
									((_GPIO_PA12_MODE == GPIO_INPUT_FLOAT)<<12) | ((_GPIO_PA13_MODE == GPIO_INPUT_FLOAT)<<13) | ((_GPIO_PA14_MODE == GPIO_INPUT_FLOAT)<<14) | ((_GPIO_PA15_MODE == GPIO_INPUT_FLOAT)<<15))
#define GPIOA_OUTPUT_HIGH			(((_GPIO_PA0_MODE == GPIO_OUTPUT_HIGH)<<0) | ((_GPIO_PA1_MODE == GPIO_OUTPUT_HIGH)<<1) | ((_GPIO_PA2_MODE == GPIO_OUTPUT_HIGH)<<2) | ((_GPIO_PA3_MODE == GPIO_OUTPUT_HIGH)<<3) | \
									((_GPIO_PA4_MODE == GPIO_OUTPUT_HIGH)<<4) | ((_GPIO_PA5_MODE == GPIO_OUTPUT_HIGH)<<5) | ((_GPIO_PA6_MODE == GPIO_OUTPUT_HIGH)<<6) | ((_GPIO_PA7_MODE == GPIO_OUTPUT_HIGH)<<7) | \
									((_GPIO_PA8_MODE == GPIO_OUTPUT_HIGH)<<8) | ((_GPIO_PA9_MODE == GPIO_OUTPUT_HIGH)<<9) | ((_GPIO_PA10_MODE == GPIO_OUTPUT_HIGH)<<10) | ((_GPIO_PA11_MODE == GPIO_OUTPUT_HIGH)<<11) | \
									((_GPIO_PA12_MODE == GPIO_OUTPUT_HIGH)<<12) | ((_GPIO_PA13_MODE == GPIO_OUTPUT_HIGH)<<13) | ((_GPIO_PA14_MODE == GPIO_OUTPUT_HIGH)<<14) | ((_GPIO_PA15_MODE == GPIO_OUTPUT_HIGH)<<15))
#define GPIOA_OUTPUT_LOW			(((_GPIO_PA0_MODE == GPIO_OUTPUT_LOW)<<0) | ((_GPIO_PA1_MODE == GPIO_OUTPUT_LOW)<<1) | ((_GPIO_PA2_MODE == GPIO_OUTPUT_LOW)<<2) | ((_GPIO_PA3_MODE == GPIO_OUTPUT_LOW)<<3) | \
									((_GPIO_PA4_MODE == GPIO_OUTPUT_LOW)<<4) | ((_GPIO_PA5_MODE == GPIO_OUTPUT_LOW)<<5) | ((_GPIO_PA6_MODE == GPIO_OUTPUT_LOW)<<6) | ((_GPIO_PA7_MODE == GPIO_OUTPUT_LOW)<<7) | \
									((_GPIO_PA8_MODE == GPIO_OUTPUT_LOW)<<8) | ((_GPIO_PA9_MODE == GPIO_OUTPUT_LOW)<<9) | ((_GPIO_PA10_MODE == GPIO_OUTPUT_LOW)<<10) | ((_GPIO_PA11_MODE == GPIO_OUTPUT_LOW)<<11) | \
									((_GPIO_PA12_MODE == GPIO_OUTPUT_LOW)<<12) | ((_GPIO_PA13_MODE == GPIO_OUTPUT_LOW)<<13) | ((_GPIO_PA14_MODE == GPIO_OUTPUT_LOW)<<14) | ((_GPIO_PA15_MODE == GPIO_OUTPUT_LOW)<<15))



#if _EF_SERIES
#define GPIOA_OUTPUT_ALT			(((_GPIO_PA0_MODE == GPIO_OUTPUT_ALT)?(1<<0):(0<<0)) | ((_GPIO_PA1_MODE == GPIO_OUTPUT_ALT)?(1<<2):(0<<2)) | ((_GPIO_PA2_MODE == GPIO_OUTPUT_ALT)?(1<<4):(0<<4)) | ((_GPIO_PA3_MODE == GPIO_OUTPUT_ALT)?(1<<6):(0<<6)) | \
									((_GPIO_PA4_MODE == GPIO_OUTPUT_ALT)?(1<<8):(0<<8)) | ((_GPIO_PA5_MODE == GPIO_OUTPUT_ALT)?(1<<10):(0<<10)) | ((_GPIO_PA6_MODE == GPIO_OUTPUT_ALT)?(1<<12):(0<<12)) | ((_GPIO_PA7_MODE == GPIO_OUTPUT_ALT)?(1<<14):(0<<14)) | \
									((_GPIO_PA12_MODE == GPIO_OUTPUT_ALT)?(1<<24):(0<<24)) | ((_GPIO_PA13_MODE == GPIO_OUTPUT_ALT)?(1<<26):(0<<26)) | ((_GPIO_PA14_MODE == GPIO_OUTPUT_ALT)?(1<<28):(0<<28)) | ((_GPIO_PA15_MODE == GPIO_OUTPUT_ALT)?(1<<30):(0<<30)))
#define GPIOA_OUTPUT_ALT2			(((_GPIO_PA2_MODE == GPIO_OUTPUT_ALT2)?(2<<4):(0<<4)) | ((_GPIO_PA3_MODE == GPIO_OUTPUT_ALT2)?(2<<6):(0<<6)) | ((_GPIO_PA12_MODE == GPIO_OUTPUT_ALT2)?(2<<24):(0<<24)) |  ((_GPIO_PA15_MODE == GPIO_OUTPUT_ALT2)?(2<<30):(0<<30)))
#define GPIOA_OUTPUT_ALT3			(((_GPIO_PA0_MODE == GPIO_OUTPUT_ALT3)?(3<<0):(0<<0)) | ((_GPIO_PA1_MODE == GPIO_OUTPUT_ALT3)?(3<<2):(0<<2)) | ((_GPIO_PA2_MODE == GPIO_OUTPUT_ALT3)?(3<<4):(0<<4)) | ((_GPIO_PA3_MODE == GPIO_OUTPUT_ALT3)?(3<<6):(0<<6)) | \
									((_GPIO_PA4_MODE == GPIO_OUTPUT_ALT3)?(3<<8):(0<<8)) | ((_GPIO_PA5_MODE == GPIO_OUTPUT_ALT3)?(3<<10):(0<<10)) | ((_GPIO_PA6_MODE == GPIO_OUTPUT_ALT3)?(3<<12):(0<<12)) | ((_GPIO_PA7_MODE == GPIO_OUTPUT_ALT3)?(3<<14):(0<<14)))
#else
#define GPIOA_OUTPUT_ALT			(((_GPIO_PA0_MODE == GPIO_OUTPUT_ALT)<<0) | ((_GPIO_PA1_MODE == GPIO_OUTPUT_ALT)<<1) | ((_GPIO_PA2_MODE == GPIO_OUTPUT_ALT)<<2) | ((_GPIO_PA3_MODE == GPIO_OUTPUT_ALT)<<3) | \
									((_GPIO_PA4_MODE == GPIO_OUTPUT_ALT)<<4) | ((_GPIO_PA5_MODE == GPIO_OUTPUT_ALT)<<5) | ((_GPIO_PA6_MODE == GPIO_OUTPUT_ALT)<<6) | ((_GPIO_PA7_MODE == GPIO_OUTPUT_ALT)<<7) | \
									((_GPIO_PA8_MODE == GPIO_OUTPUT_ALT)<<8) | ((_GPIO_PA9_MODE == GPIO_OUTPUT_ALT)<<9) | ((_GPIO_PA10_MODE == GPIO_OUTPUT_ALT)<<10) | ((_GPIO_PA11_MODE == GPIO_OUTPUT_ALT)<<11) | \
									((_GPIO_PA12_MODE == GPIO_OUTPUT_ALT)<<12) | ((_GPIO_PA13_MODE == GPIO_OUTPUT_ALT)<<13) | ((_GPIO_PA14_MODE == GPIO_OUTPUT_ALT)<<14) | ((_GPIO_PA15_MODE == GPIO_OUTPUT_ALT)<<15))
#define GPIOA_OUTPUT_ALT2			(((_GPIO_PA4_MODE == GPIO_OUTPUT_ALT2)<<20) | ((_GPIO_PA5_MODE == GPIO_OUTPUT_ALT2)<<21) | ((_GPIO_PA6_MODE == GPIO_OUTPUT_ALT2)<<22) | ((_GPIO_PA7_MODE == GPIO_OUTPUT_ALT2)<<23) | \
                                    ((_GPIO_PA14_MODE == GPIO_OUTPUT_ALT2)<<30))
#endif

#define GPIOB_INPUT_PULLHIGH		(((_GPIO_PB0_MODE == GPIO_INPUT_PULLHIGH)<<0) | ((_GPIO_PB1_MODE == GPIO_INPUT_PULLHIGH)<<1) | ((_GPIO_PB2_MODE == GPIO_INPUT_PULLHIGH)<<2) | ((_GPIO_PB3_MODE == GPIO_INPUT_PULLHIGH)<<3) | \
									((_GPIO_PB4_MODE == GPIO_INPUT_PULLHIGH)<<4) | ((_GPIO_PB5_MODE == GPIO_INPUT_PULLHIGH)<<5) | ((_GPIO_PB6_MODE == GPIO_INPUT_PULLHIGH)<<6) | ((_GPIO_PB7_MODE == GPIO_INPUT_PULLHIGH)<<7) | \
									((_GPIO_PB8_MODE == GPIO_INPUT_PULLHIGH)<<8) | ((_GPIO_PB9_MODE == GPIO_INPUT_PULLHIGH)<<9) | ((_GPIO_PB10_MODE == GPIO_INPUT_PULLHIGH)<<10) | ((_GPIO_PB11_MODE == GPIO_INPUT_PULLHIGH)<<11) | \
									((_GPIO_PB12_MODE == GPIO_INPUT_PULLHIGH)<<12) | ((_GPIO_PB13_MODE == GPIO_INPUT_PULLHIGH)<<13) | ((_GPIO_PB14_MODE == GPIO_INPUT_PULLHIGH)<<14) | ((_GPIO_PB15_MODE == GPIO_INPUT_PULLHIGH)<<15))
#define GPIOB_INPUT_FLOAT			(((_GPIO_PB0_MODE == GPIO_INPUT_FLOAT)<<0) | ((_GPIO_PB1_MODE == GPIO_INPUT_FLOAT)<<1) | ((_GPIO_PB2_MODE == GPIO_INPUT_FLOAT)<<2) | ((_GPIO_PB3_MODE == GPIO_INPUT_FLOAT)<<3) | \
									((_GPIO_PB4_MODE == GPIO_INPUT_FLOAT)<<4) | ((_GPIO_PB5_MODE == GPIO_INPUT_FLOAT)<<5) | ((_GPIO_PB6_MODE == GPIO_INPUT_FLOAT)<<6) | ((_GPIO_PB7_MODE == GPIO_INPUT_FLOAT)<<7) | \
									((_GPIO_PB8_MODE == GPIO_INPUT_FLOAT)<<8) | ((_GPIO_PB9_MODE == GPIO_INPUT_FLOAT)<<9) | ((_GPIO_PB10_MODE == GPIO_INPUT_FLOAT)<<10) | ((_GPIO_PB11_MODE == GPIO_INPUT_FLOAT)<<11) | \
									((_GPIO_PB12_MODE == GPIO_INPUT_FLOAT)<<12) | ((_GPIO_PB13_MODE == GPIO_INPUT_FLOAT)<<13) | ((_GPIO_PB14_MODE == GPIO_INPUT_FLOAT)<<14) | ((_GPIO_PB15_MODE == GPIO_INPUT_FLOAT)<<15))
#define GPIOB_OUTPUT_HIGH			(((_GPIO_PB0_MODE == GPIO_OUTPUT_HIGH)<<0) | ((_GPIO_PB1_MODE == GPIO_OUTPUT_HIGH)<<1) | ((_GPIO_PB2_MODE == GPIO_OUTPUT_HIGH)<<2) | ((_GPIO_PB3_MODE == GPIO_OUTPUT_HIGH)<<3) | \
									((_GPIO_PB4_MODE == GPIO_OUTPUT_HIGH)<<4) | ((_GPIO_PB5_MODE == GPIO_OUTPUT_HIGH)<<5) | ((_GPIO_PB6_MODE == GPIO_OUTPUT_HIGH)<<6) | ((_GPIO_PB7_MODE == GPIO_OUTPUT_HIGH)<<7) | \
									((_GPIO_PB8_MODE == GPIO_OUTPUT_HIGH)<<8) | ((_GPIO_PB9_MODE == GPIO_OUTPUT_HIGH)<<9) | ((_GPIO_PB10_MODE == GPIO_OUTPUT_HIGH)<<10) | ((_GPIO_PB11_MODE == GPIO_OUTPUT_HIGH)<<11) | \
									((_GPIO_PB12_MODE == GPIO_OUTPUT_HIGH)<<12) | ((_GPIO_PB13_MODE == GPIO_OUTPUT_HIGH)<<13) | ((_GPIO_PB14_MODE == GPIO_OUTPUT_HIGH)<<14) | ((_GPIO_PB15_MODE == GPIO_OUTPUT_HIGH)<<15))
#define GPIOB_OUTPUT_LOW			(((_GPIO_PB0_MODE == GPIO_OUTPUT_LOW)<<0) | ((_GPIO_PB1_MODE == GPIO_OUTPUT_LOW)<<1) | ((_GPIO_PB2_MODE == GPIO_OUTPUT_LOW)<<2) | ((_GPIO_PB3_MODE == GPIO_OUTPUT_LOW)<<3) | \
									((_GPIO_PB4_MODE == GPIO_OUTPUT_LOW)<<4) | ((_GPIO_PB5_MODE == GPIO_OUTPUT_LOW)<<5) | ((_GPIO_PB6_MODE == GPIO_OUTPUT_LOW)<<6) | ((_GPIO_PB7_MODE == GPIO_OUTPUT_LOW)<<7) | \
									((_GPIO_PB8_MODE == GPIO_OUTPUT_LOW)<<8) | ((_GPIO_PB9_MODE == GPIO_OUTPUT_LOW)<<9) | ((_GPIO_PB10_MODE == GPIO_OUTPUT_LOW)<<10) | ((_GPIO_PB11_MODE == GPIO_OUTPUT_LOW)<<11) | \
									((_GPIO_PB12_MODE == GPIO_OUTPUT_LOW)<<12) | ((_GPIO_PB13_MODE == GPIO_OUTPUT_LOW)<<13) | ((_GPIO_PB14_MODE == GPIO_OUTPUT_LOW)<<14) | ((_GPIO_PB15_MODE == GPIO_OUTPUT_LOW)<<15))





#if _EF_SERIES
#define GPIOB_OUTPUT_ALT			(((_GPIO_PB0_MODE == GPIO_OUTPUT_ALT)?(1<<0):(0<<0)) | ((_GPIO_PB1_MODE == GPIO_OUTPUT_ALT)?(1<<2):(0<<2)) | ((_GPIO_PB2_MODE == GPIO_OUTPUT_ALT)?(1<<4):(0<<4)) | ((_GPIO_PB3_MODE == GPIO_OUTPUT_ALT)?(1<<6):(0<<6)) | \
									((_GPIO_PB4_MODE == GPIO_OUTPUT_ALT)?(1<<8):(0<<8)) | ((_GPIO_PB5_MODE == GPIO_OUTPUT_ALT)?(1<<10):(0<<10)))
#define GPIOB_OUTPUT_ALT2			(((_GPIO_PB0_MODE == GPIO_OUTPUT_ALT2)?(2<<0):(0<<0)) | ((_GPIO_PB1_MODE == GPIO_OUTPUT_ALT2)?(2<<2):(0<<2)) | ((_GPIO_PB4_MODE == GPIO_OUTPUT_ALT2)?(2<<8):(0<<8)))
#define GPIOB_OUTPUT_ALT3			(((_GPIO_PB4_MODE == GPIO_OUTPUT_ALT3)?(3<<8):(0<<8)))
#else
#define GPIOB_OUTPUT_ALT			(((_GPIO_PB0_MODE == GPIO_OUTPUT_ALT)<<0) | ((_GPIO_PB1_MODE == GPIO_OUTPUT_ALT)<<1) | ((_GPIO_PB2_MODE == GPIO_OUTPUT_ALT)<<2) | ((_GPIO_PB3_MODE == GPIO_OUTPUT_ALT)<<3) | \
									((_GPIO_PB4_MODE == GPIO_OUTPUT_ALT)<<4) | ((_GPIO_PB5_MODE == GPIO_OUTPUT_ALT)<<5) | ((_GPIO_PB6_MODE == GPIO_OUTPUT_ALT)<<6) | ((_GPIO_PB7_MODE == GPIO_OUTPUT_ALT)<<7) | \
									((_GPIO_PB8_MODE == GPIO_OUTPUT_ALT)<<8) | ((_GPIO_PB9_MODE == GPIO_OUTPUT_ALT)<<9) | ((_GPIO_PB10_MODE == GPIO_OUTPUT_ALT)<<10) | ((_GPIO_PB11_MODE == GPIO_OUTPUT_ALT)<<11) | \
									((_GPIO_PB12_MODE == GPIO_OUTPUT_ALT)<<12) | ((_GPIO_PB13_MODE == GPIO_OUTPUT_ALT)<<13) | ((_GPIO_PB14_MODE == GPIO_OUTPUT_ALT)<<14) | ((_GPIO_PB15_MODE == GPIO_OUTPUT_ALT)<<15))
#endif

#define GPIOC_INPUT_PULLHIGH		(((_GPIO_PC0_MODE == GPIO_INPUT_PULLHIGH)<<0) | ((_GPIO_PC1_MODE == GPIO_INPUT_PULLHIGH)<<1) | ((_GPIO_PC2_MODE == GPIO_INPUT_PULLHIGH)<<2) | ((_GPIO_PC3_MODE == GPIO_INPUT_PULLHIGH)<<3) | \
									((_GPIO_PC4_MODE == GPIO_INPUT_PULLHIGH)<<4) | ((_GPIO_PC5_MODE == GPIO_INPUT_PULLHIGH)<<5) | ((_GPIO_PC6_MODE == GPIO_INPUT_PULLHIGH)<<6) | ((_GPIO_PC7_MODE == GPIO_INPUT_PULLHIGH)<<7))
#define GPIOC_INPUT_FLOAT			(((_GPIO_PC0_MODE == GPIO_INPUT_FLOAT)<<0) | ((_GPIO_PC1_MODE == GPIO_INPUT_FLOAT)<<1) | ((_GPIO_PC2_MODE == GPIO_INPUT_FLOAT)<<2) | ((_GPIO_PC3_MODE == GPIO_INPUT_FLOAT)<<3) | \
									((_GPIO_PC4_MODE == GPIO_INPUT_FLOAT)<<4) | ((_GPIO_PC5_MODE == GPIO_INPUT_FLOAT)<<5) | ((_GPIO_PC6_MODE == GPIO_INPUT_FLOAT)<<6) | ((_GPIO_PC7_MODE == GPIO_INPUT_FLOAT)<<7))
#define GPIOC_OUTPUT_HIGH			(((_GPIO_PC0_MODE == GPIO_OUTPUT_HIGH)<<0) | ((_GPIO_PC1_MODE == GPIO_OUTPUT_HIGH)<<1) | ((_GPIO_PC2_MODE == GPIO_OUTPUT_HIGH)<<2) | ((_GPIO_PC3_MODE == GPIO_OUTPUT_HIGH)<<3) | \
									((_GPIO_PC4_MODE == GPIO_OUTPUT_HIGH)<<4) | ((_GPIO_PC5_MODE == GPIO_OUTPUT_HIGH)<<5) | ((_GPIO_PC6_MODE == GPIO_OUTPUT_HIGH)<<6) | ((_GPIO_PC7_MODE == GPIO_OUTPUT_HIGH)<<7))
#define GPIOC_OUTPUT_LOW			(((_GPIO_PC0_MODE == GPIO_OUTPUT_LOW)<<0) | ((_GPIO_PC1_MODE == GPIO_OUTPUT_LOW)<<1) | ((_GPIO_PC2_MODE == GPIO_OUTPUT_LOW)<<2) | ((_GPIO_PC3_MODE == GPIO_OUTPUT_LOW)<<3) | \
									((_GPIO_PC4_MODE == GPIO_OUTPUT_LOW)<<4) | ((_GPIO_PC5_MODE == GPIO_OUTPUT_LOW)<<5) | ((_GPIO_PC6_MODE == GPIO_OUTPUT_LOW)<<6) | ((_GPIO_PC7_MODE == GPIO_OUTPUT_LOW)<<7))



#if _EF_SERIES
#define GPIOC_OUTPUT_ALT			(((_GPIO_PC0_MODE == GPIO_OUTPUT_ALT)?(1<<0):(0<<0)) | ((_GPIO_PC1_MODE == GPIO_OUTPUT_ALT)?(1<<2):(0<<2)))
#define GPIOC_OUTPUT_ALT3			(((_GPIO_PC0_MODE == GPIO_OUTPUT_ALT3)?(3<<0):(0<<0)) | ((_GPIO_PC1_MODE == GPIO_OUTPUT_ALT3)?(3<<2):(0<<2)) | ((_GPIO_PC2_MODE == GPIO_OUTPUT_ALT3)?(3<<4):(0<<4)) | ((_GPIO_PC3_MODE == GPIO_OUTPUT_ALT3)?(3<<6):(0<<6)))

#define GPIOD_INPUT_PULLHIGH		(((_GPIO_PD0_MODE == GPIO_INPUT_PULLHIGH)<<0) | ((_GPIO_PD1_MODE == GPIO_INPUT_PULLHIGH)<<1))
#define GPIOD_INPUT_FLOAT			(((_GPIO_PD0_MODE == GPIO_INPUT_FLOAT)<<0) | ((_GPIO_PD1_MODE == GPIO_INPUT_FLOAT)<<1))
#define GPIOD_OUTPUT_HIGH			(((_GPIO_PD0_MODE == GPIO_OUTPUT_HIGH)<<0) | ((_GPIO_PD1_MODE == GPIO_OUTPUT_HIGH)<<1))
#define GPIOD_OUTPUT_LOW			(((_GPIO_PD0_MODE == GPIO_OUTPUT_LOW)<<0) | ((_GPIO_PD1_MODE == GPIO_OUTPUT_LOW)<<1))
#define GPIOD_OUTPUT_ALT3			(((_GPIO_PD0_MODE == GPIO_OUTPUT_ALT3)?(3<<0):(0<<0)) | ((_GPIO_PD1_MODE == GPIO_OUTPUT_ALT3)?(3<<2):(0<<2)))
#else
#define GPIOC_OUTPUT_ALT			(((_GPIO_PC0_MODE == GPIO_OUTPUT_ALT)<<0) | ((_GPIO_PC1_MODE == GPIO_OUTPUT_ALT)<<1) | ((_GPIO_PC2_MODE == GPIO_OUTPUT_ALT)<<2) | ((_GPIO_PC3_MODE == GPIO_OUTPUT_ALT)<<3) | \
									((_GPIO_PC4_MODE == GPIO_OUTPUT_ALT)<<4) | ((_GPIO_PC5_MODE == GPIO_OUTPUT_ALT)<<5) | ((_GPIO_PC6_MODE == GPIO_OUTPUT_ALT)<<6) | ((_GPIO_PC7_MODE == GPIO_OUTPUT_ALT)<<7))
#endif

#define GPIOA_WAKEUPMASK			((_GPIO_PA0_WAKEUP<<0)|(_GPIO_PA1_WAKEUP<<1)|(_GPIO_PA2_WAKEUP<<2)|(_GPIO_PA3_WAKEUP<<3)|(_GPIO_PA4_WAKEUP<<4)|(_GPIO_PA5_WAKEUP<<5)|(_GPIO_PA6_WAKEUP<<6)|(_GPIO_PA7_WAKEUP<<7) | \
									(_GPIO_PA8_WAKEUP<<8)|(_GPIO_PA9_WAKEUP<<9)|(_GPIO_PA10_WAKEUP<<10)|(_GPIO_PA11_WAKEUP<<11)|(_GPIO_PA12_WAKEUP<<12)|(_GPIO_PA13_WAKEUP<<13)|(_GPIO_PA14_WAKEUP<<14)|(_GPIO_PA15_WAKEUP<<15))
#define GPIOB_WAKEUPMASK 			((_GPIO_PB0_WAKEUP<<0)|(_GPIO_PB1_WAKEUP<<1)|(_GPIO_PB2_WAKEUP<<2)|(_GPIO_PB3_WAKEUP<<3)|(_GPIO_PB4_WAKEUP<<4)|(_GPIO_PB5_WAKEUP<<5)|(_GPIO_PB6_WAKEUP<<6)|(_GPIO_PB7_WAKEUP<<7) | \
									(_GPIO_PB8_WAKEUP<<8)|(_GPIO_PB9_WAKEUP<<9)|(_GPIO_PB10_WAKEUP<<10)|(_GPIO_PB11_WAKEUP<<11)|(_GPIO_PB12_WAKEUP<<12)|(_GPIO_PB13_WAKEUP<<13)|(_GPIO_PB14_WAKEUP<<14)|(_GPIO_PB15_WAKEUP<<15))
#define GPIOC_WAKEUPMASK 			((_GPIO_PC0_WAKEUP<<0)|(_GPIO_PC1_WAKEUP<<1)|(_GPIO_PC2_WAKEUP<<2)|(_GPIO_PC3_WAKEUP<<3)|(_GPIO_PC4_WAKEUP<<4)|(_GPIO_PC5_WAKEUP<<5)|(_GPIO_PC6_WAKEUP<<6)|(_GPIO_PC7_WAKEUP<<7))
#if _EF_SERIES
#define GPIOD_WAKEUPMASK 			((_GPIO_PD0_WAKEUP<<0)|(_GPIO_PD1_WAKEUP<<1))
#endif

/* Static Variables ----------------------------------------------------------*/
static const U8 LibVer[]="NX1-V3.80";

#if _RANDOM_GEN
static U32 Getrandom_x;
#endif

extern U8 _end;//bss end

//Flag for ADC setup
#if _IS_APPS_USE_ADC_MIC
volatile U8	AdcEnFlag;
volatile U16 AdcEnCnt;
#endif

#if _AUDIO_IDLE_INTO_HALT || (_TOUCHKEY_MODULE && _TOUCHKEY_INTO_STANDBY)
static U32 sleep_time_init;//time base of music_play
U8 SleepFlag = 1;
#endif

#if _USE_HEAP_RAM
volatile U16 _stack_size;
volatile U16 _ram_size;
#endif


/* Global Variables ----------------------------------------------------------*/

void Set_WakeUp_Port(void);
void wait_FlashStable_15ms(void);
void release_PowerDown_ms(void);

#if body_MultiTrgLdo
//------------------------------------------------------------------//
// SYS_MULTI_TRG_LDO
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SYS_MULTI_TRG_LDO()
{
#define STB_CNT_NUMBER  4 // 4: 2 second
	static S8 cnt_500ms=0;
    if(STB_CNT_NUMBER == ++cnt_500ms)
    {
        cnt_500ms = 0;
        // Switch to slow mode
        P_SMU->CLK_Ctrl = _SYS_SMU_CPU_CLK_DIV | SMU_SYSCLK_FROM_LOCLK | C_HiCLK_En;
        P_SMU->CLK_Ctrl = _SYS_SMU_CPU_CLK_DIV | SMU_SYSCLK_FROM_LOCLK;  //disable IHRC

        // Switch to High Clock and Disable SPI0_LDO
        P_SMU->CLK_Ctrl = _SYS_SMU_CPU_CLK_DIV | SMU_SYSCLK_FROM_LOCLK | C_HiCLK_En; //Enable  IHRC

        // Switch to normal mode
        P_SMU->CLK_Ctrl = _SYS_SMU_CPU_CLK_DIV | SMU_SYSCLK_FROM_HICLK | C_HiCLK_En;
    }
#undef STB_CNT_NUMBER
}
#endif
//------------------------------------------------------------------//
// SYS_SOFT_ENABLE_LDO
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SYS_SOFT_ENABLE_LDO()
{
	U32	u32_CLK_Ctrl_Temp1,u32_CLK_Ctrl_Temp2,u32_PWR_Ctrl_Temp1;

	u32_CLK_Ctrl_Temp1 = P_SMU->CLK_Ctrl&(~SMU_SYSCLKDIV_1);	//SMU_SYSCLKDIV_128
	u32_PWR_Ctrl_Temp1 = P_SMU->PWR_Ctrl | C_LDOSPI0_En;
	u32_CLK_Ctrl_Temp2 = P_SMU->CLK_Ctrl|SMU_SYSCLKDIV_1;

	GIE_DISABLE();
	P_SMU->CLK_Ctrl = u32_CLK_Ctrl_Temp1;//SMU_CpuClock(_SYS_SMU_CPU_CLK_DIV,SMU_SYSCLK_FROM_LOCLK);
    P_SMU->PWR_Ctrl = u32_PWR_Ctrl_Temp1;//SMU_LDO33Cmd(ENABLE);
    __asm__("nop\t\n");
    P_SMU->CLK_Ctrl = u32_CLK_Ctrl_Temp2;//SMU_CpuClock(_SYS_SMU_CPU_CLK_DIV,SMU_SYSCLK_FROM_HICLK);
	GIE_ENABLE();
}
//------------------------------------------------------------------//
// SYS_NX1F_SPI_MODE_INTO_HALT
// Parameter:
//          cmd:  ENABLE:  SPI0/1 pins set to GPIO mode
//				  DISABLE: SPI0/1 pins set to SPI mode
// return value:
//          NONE
//------------------------------------------------------------------//
void SYS_NX1F_SPI_MODE_INTO_HALT(U8 cmd)
{
#if (_SPI_MODULE&&_EF_SERIES) || (_SPI1_MODULE&&_EF_SERIES)
	if(cmd){
#if _SPI_MODULE&&_EF_SERIES
    GPIO_Init(GPIOB,1,GPIO_OUTPUT_HIGH); //CS,PB1
#if _SPI_ACCESS_MODE==SPI_1_1_1_MODE_3WIRE
#if (_GPIO_PB0_MODE == GPIO_PIN_OFF)
    GPIO_Init(GPIOB,0,GPIO_OUTPUT_HIGH); //MISO,PB0
#endif
    GPIO_Init(GPIOB,3,GPIO_OUTPUT_HIGH); //MOSI,PB3
    GPIO_Init(GPIOB,2,GPIO_OUTPUT_HIGH); //CLK,PB2
#else
    GPIO_Init(GPIOB,0,GPIO_OUTPUT_HIGH); //MISO,PB0
    GPIO_Init(GPIOB,3,GPIO_OUTPUT_HIGH); //MOSI,PB3
    GPIO_Init(GPIOB,2,GPIO_OUTPUT_HIGH); //CLK,PB2
#endif
#if (_GPIO_PB4_MODE == GPIO_PIN_OFF) && (_GPIO_PB5_MODE == GPIO_PIN_OFF)
	GPIO_Init(GPIOB,4,GPIO_OUTPUT_HIGH); //IO2,PB4
    GPIO_Init(GPIOB,5,GPIO_OUTPUT_HIGH); //IO3,PB5;
#endif
#endif

#if  _SPI1_MODULE&&_EF_SERIES
	GPIO_Init(GPIOA, 12, GPIO_OUTPUT_HIGH); //CS,PA12
#if _SPI1_ACCESS_MODE==SPI_1_1_1_MODE_3WIRE
#if (_GPIO_PA15_MODE == GPIO_PIN_OFF)
	GPIO_Init(GPIOA, 15, GPIO_OUTPUT_HIGH); //MISO,PA15
#endif
    GPIO_Init(GPIOA, 14, GPIO_OUTPUT_HIGH); //MOSI,PA14
    GPIO_Init(GPIOA, 13, GPIO_OUTPUT_HIGH); //CLK,PA13
#else
	GPIO_Init(GPIOA, 15, GPIO_OUTPUT_HIGH); //MISO,PA15
	GPIO_Init(GPIOA, 14, GPIO_OUTPUT_HIGH); //MOSI,PA14
    GPIO_Init(GPIOA, 13, GPIO_OUTPUT_HIGH); //CLK,PA13
#endif
#endif
	}
	else
	{

#if _SPI_MODULE&&_EF_SERIES
    GPIO_Init(GPIOB,SPI0_CSB_MTF_PIN,GPIO_MODE_ALT2);
#if _SPI_ACCESS_MODE==SPI_1_1_1_MODE_3WIRE
#if (_GPIO_PB0_MODE == GPIO_PIN_OFF)
    GPIO_Init(GPIOB,SPI0_MISO_MTF_PIN,GPIO_OUTPUT_LOW);
#endif
#else
    GPIO_Init(GPIOB,SPI0_MISO_MTF_PIN,GPIO_MODE_ALT2);
#endif
#if (_GPIO_PB4_MODE == GPIO_PIN_OFF) && (_GPIO_PB5_MODE == GPIO_PIN_OFF)
	GPIO_Init(GPIOB,SPI0_IO2_MTF_PIN,GPIO_MODE_ALT2);
#endif

    SMU_PeriphReset(SMU_PERIPH_SPI);
#if OPTION_HIGH_FREQ_CLK > 32000000
	SPI_Init(SPI_CLKDIV_2, SPI_Pol_Low, SPI_Phase_1st_Edge);
#else
    SPI_Init(SPI_CLKDIV_1, SPI_Pol_Low, SPI_Phase_1st_Edge);
#endif
#endif

#if  _SPI1_MODULE&&_EF_SERIES
#if _SPI_MODULE==DISABLE
	SMU_PeriphReset(SMU_PERIPH_SPI);
#endif

	GPIO_Init(GPIOA, SPI1_CSB_MTF_PIN, GPIO_MODE_ALT2);
#if _SPI1_ACCESS_MODE==SPI_1_1_1_MODE_3WIRE
#if (_GPIO_PA15_MODE == GPIO_PIN_OFF)
    GPIO_Init(GPIOA, SPI1_MISO_MTF_PIN, GPIO_OUTPUT_LOW);
#endif
#else
	GPIO_Init(GPIOA, SPI1_MISO_MTF_PIN, GPIO_MODE_ALT2);
#endif

#if OPTION_HIGH_FREQ_CLK > 32000000
	SPI1_Init(SPI_CLKDIV_2, SPI_Pol_Low, SPI_Phase_1st_Edge);
#else
    SPI1_Init(SPI_CLKDIV_1, SPI_Pol_Low, SPI_Phase_1st_Edge);
#endif
#endif	// #if _SPI1_MODULE&&_EF_SERIES
	}
#endif	// #if (_SPI_MODULE&&_EF_SERIES) || (_SPI1_MODULE&&_EF_SERIES)
}
//------------------------------------------------------------------//
// PWM-IO Init in sys.c
// Parameter:
//          none
// return value:
//          NONE
//------------------------------------------------------------------//
void SYS_PWMInit(void)
{
    SMU_PeriphClkCmd(SMU_PERIPH_PWM,ENABLE);

	//Each Module setting
#if (_VIO_TIMER==USE_TIMER_PWMA && _ACTION_MODULE) || (_QFID_TIMER==USE_TIMER_PWMA&&_QFID_MODULE)
    TIMER_IntCmd(PWMATIMER,ENABLE);
    INTC_IrqCmd(IRQ_BIT_PWMA,ENABLE);
#endif
#if (_VIO_TIMER==USE_TIMER_PWMB && _ACTION_MODULE) || (_QFID_TIMER==USE_TIMER_PWMB&&_QFID_MODULE)
    TIMER_IntCmd(PWMBTIMER,ENABLE);
    INTC_IrqCmd(IRQ_BIT_PWMB,ENABLE);
#endif

#if _PWMA_MODULE
#if _EF_SERIES
    TIMER_Init(PWMATIMER,65535);
#else
    TIMER_Init(PWMATIMER,TIMER_CLKDIV_1,65535);
#endif
    TIMER_Cmd(PWMATIMER,ENABLE);
#if _PWMA_INT
    TIMER_IntCmd(PWMATIMER,ENABLE);
    INTC_IrqCmd(IRQ_BIT_PWMA,ENABLE);
#endif
#endif

#if _PWMB_MODULE
#if _EF_SERIES
	TIMER_Init(PWMBTIMER,65535);
#else
    TIMER_Init(PWMBTIMER,TIMER_CLKDIV_1,65535);
#endif
    TIMER_Cmd(PWMBTIMER,ENABLE);
#if _PWMB_INT
    TIMER_IntCmd(PWMBTIMER,ENABLE);
    INTC_IrqCmd(IRQ_BIT_PWMB,ENABLE);
#endif
#endif

#if _CAPTURE_MODULE
    TIMER_Init(PWMATIMER,TIMER_CLKDIV_32,999);
    TIMER_Cmd(PWMATIMER,ENABLE);
    PWM_CapturePinSelect(_CAPTURE_PAx);
#if _CAPTURE_INT
    PWM_CaptureIntCmd(_CAPTURE_INT_MODE,ENABLE);
    INTC_IrqCmd(IRQ_BIT_CAPTURE,ENABLE);
#endif
#endif

    //Each IO output setting
#if  _PWMA_MODULE
#if _PWMA0_DUTY!=0xFF
    GPIO_Init(GPIOA,PWMA0_PIN,GPIO_MODE_ALT);
#if (_PWMA0_STATUS==START_HIGH)
	PWM_InitDuty(PWMA,PWMx0,PWM_START_HIGH,_PWMA0_DUTY);
#else
	PWM_InitDuty(PWMA,PWMx0,PWM_START_LOW,_PWMA0_DUTY);
#endif
#endif
#if _PWMA1_DUTY!=0xFF
    GPIO_Init(GPIOA,PWMA1_PIN,GPIO_MODE_ALT);
#if (_PWMA1_STATUS==START_HIGH)
	PWM_InitDuty(PWMA,PWMx1,PWM_START_HIGH,_PWMA1_DUTY);
#else
	PWM_InitDuty(PWMA,PWMx1,PWM_START_LOW,_PWMA1_DUTY);
#endif
#endif
#if _PWMA2_DUTY!=0xFF
    GPIO_Init(GPIOA,PWMA2_PIN,GPIO_MODE_ALT);
#if (_PWMA2_STATUS==START_HIGH)
	PWM_InitDuty(PWMA,PWMx2,PWM_START_HIGH,_PWMA2_DUTY);
#else
	PWM_InitDuty(PWMA,PWMx2,PWM_START_LOW,_PWMA2_DUTY);
#endif
#endif
#if _PWMA3_DUTY!=0xFF
    GPIO_Init(GPIOA,PWMA3_PIN,GPIO_MODE_ALT);
#if (_PWMA3_STATUS==START_HIGH)
	PWM_InitDuty(PWMA,PWMx3,PWM_START_HIGH,_PWMA3_DUTY);
#else
	PWM_InitDuty(PWMA,PWMx3,PWM_START_LOW,_PWMA3_DUTY);
#endif
#endif
#endif
#if _PWMB_MODULE
#if _PWMB0_DUTY!=0xFF
    GPIO_Init(GPIOB,PWMB0_PIN,GPIO_MODE_ALT);
#if (_PWMB0_STATUS==START_HIGH)
	PWM_InitDuty(PWMB,PWMx0,PWM_START_HIGH,_PWMB0_DUTY);
#else
	PWM_InitDuty(PWMB,PWMx0,PWM_START_LOW,_PWMB0_DUTY);
#endif
#endif
#if _PWMB1_DUTY!=0xFF
    GPIO_Init(GPIOB,PWMB1_PIN,GPIO_MODE_ALT);
#if (_PWMB1_STATUS==START_HIGH)
	PWM_InitDuty(PWMB,PWMx1,PWM_START_HIGH,_PWMB1_DUTY);
#else
	PWM_InitDuty(PWMB,PWMx1,PWM_START_LOW,_PWMB1_DUTY);
#endif
#endif
#if _PWMB2_DUTY!=0xFF
    GPIO_Init(GPIOB,PWMB2_PIN,GPIO_MODE_ALT);
#if (_PWMB2_STATUS==START_HIGH)
	PWM_InitDuty(PWMB,PWMx2,PWM_START_HIGH,_PWMB2_DUTY);
#else
	PWM_InitDuty(PWMB,PWMx2,PWM_START_LOW,_PWMB2_DUTY);
#endif
#endif
#if _PWMB3_DUTY!=0xFF
    GPIO_Init(GPIOB,PWMB3_PIN,GPIO_MODE_ALT);
#if (_PWMB3_STATUS==START_HIGH)
	PWM_InitDuty(PWMB,PWMx3,PWM_START_HIGH,_PWMB3_DUTY);
#else
	PWM_InitDuty(PWMB,PWMx3,PWM_START_LOW,_PWMB3_DUTY);
#endif
#endif
#endif
}
//------------------------------------------------------------------//
// Timer Init in sys.c
// Parameter:
//          none
// return value:
//          NONE
//------------------------------------------------------------------//
void SYS_TIMERInit(void)
{
	SMU_PeriphClkCmd(SMU_PERIPH_TMR,ENABLE);
#if _TIMER_TMR0_INT || (_VIO_TIMER==USE_TIMER0 && _ACTION_MODULE) || (_QFID_TIMER==USE_TIMER0 && _QFID_MODULE)
    TIMER_IntCmd(TIMER0,ENABLE);
    INTC_IrqCmd(IRQ_BIT_TMR0,ENABLE);
#endif
#if _TIMER_TMR1_INT || (_VIO_TIMER==USE_TIMER1 && _ACTION_MODULE) || (_QFID_TIMER==USE_TIMER1 && _QFID_MODULE)
    TIMER_IntCmd(TIMER1,ENABLE);
    INTC_IrqCmd(IRQ_BIT_TMR1,ENABLE);
#endif
#if _TIMER_TMR2_INT || (_VIO_TIMER==USE_TIMER2 && _ACTION_MODULE) || (_QFID_TIMER==USE_TIMER2 && _QFID_MODULE)
    TIMER_IntCmd(TIMER2,ENABLE);
    INTC_IrqCmd(IRQ_BIT_TMR2,ENABLE);
#endif
#if _EF_SERIES
#if _TIMER_TMR3_INT || (_VIO_TIMER==USE_TIMER3 && _ACTION_MODULE) || (_QFID_TIMER==USE_TIMER3 && _QFID_MODULE)
    TIMER_IntCmd(TIMER3,ENABLE);
    INTC_IrqCmd(IRQ_BIT_TMR3,ENABLE);
#endif
#endif
}
//------------------------------------------------------------------//
// GPIO Init in sys.c
// Parameter:
//          none
// return value:
//          NONE
//------------------------------------------------------------------//
void SYS_GPIOInit(void)
{
#if _QCODE_BUILD==0
	//pull up all GPIO
	P_PORTA->CFG &= ~0xFFFF;
	P_PORTB->CFG &= ~0xFFFF;
	P_PORTC->CFG &= ~0xFF;
#if _EF_SERIES
#if (_IS_APPS_USE_ADC_MIC) \
	|| ((_DIRECTKEY_MODULE && (_DIRECTKEY_PC0 || _DIRECTKEY_PC1 || _DIRECTKEY_PC2 || _DIRECTKEY_PC3)) \
	|| (_GPIO_MODULE && ((_GPIO_PC0_MODE != GPIO_PIN_OFF) || (_GPIO_PC1_MODE != GPIO_PIN_OFF) || (_GPIO_PC2_MODE != GPIO_PIN_OFF) || (_GPIO_PC3_MODE != GPIO_PIN_OFF))))
	// ADC or GPIO conditions
#else
	GPIOC->DIR = 0;
	GPIOC->Data = 0;
#endif
	P_PORTD->CFG &= ~0x3;
#endif
#endif

	// Default Setting is input-pullhigh
#if GPIOA_OUTPUT_HIGH || GPIOA_OUTPUT_LOW
	P_PORTA->Data = ~GPIOA_OUTPUT_LOW;
	P_PORTA->DIR = ~(GPIOA_OUTPUT_HIGH | GPIOA_OUTPUT_LOW);
#endif
#if GPIOA_INPUT_FLOAT
	P_PORTA->CFG |= GPIOA_INPUT_FLOAT;
#endif
#if _EF_SERIES
#if GPIOA_OUTPUT_ALT || GPIOA_OUTPUT_ALT2 || GPIOA_OUTPUT_ALT3
	P_PORTA->MFP = GPIOA_OUTPUT_ALT | GPIOA_OUTPUT_ALT2 | GPIOA_OUTPUT_ALT3;
#endif
#else
#if GPIOA_OUTPUT_ALT || GPIOA_OUTPUT_ALT2
	P_PORTA->MFP = GPIOA_OUTPUT_ALT | GPIOA_OUTPUT_ALT2;
#endif
#endif

#if GPIOB_OUTPUT_HIGH || GPIOB_OUTPUT_LOW
	P_PORTB->Data = ~GPIOB_OUTPUT_LOW;
	P_PORTB->DIR = ~(GPIOB_OUTPUT_HIGH | GPIOB_OUTPUT_LOW);
#endif
#if GPIOB_INPUT_FLOAT
	P_PORTB->CFG |= GPIOB_INPUT_FLOAT;
#endif
#if _EF_SERIES
#if GPIOB_OUTPUT_ALT || GPIOB_OUTPUT_ALT2 || GPIOB_OUTPUT_ALT3
	P_PORTB->MFP = GPIOB_OUTPUT_ALT | GPIOB_OUTPUT_ALT2 | GPIOB_OUTPUT_ALT3;
#endif
#else
#if GPIOB_OUTPUT_ALT
	P_PORTB->MFP = GPIOB_OUTPUT_ALT;
#endif
#endif

#if GPIOC_OUTPUT_HIGH || GPIOC_OUTPUT_LOW
	P_PORTC->Data = ~GPIOC_OUTPUT_LOW;
	P_PORTC->DIR = ~(GPIOC_OUTPUT_HIGH | GPIOC_OUTPUT_LOW);
#endif
#if GPIOC_INPUT_FLOAT
	P_PORTC->CFG |= GPIOC_INPUT_FLOAT;
#endif
#if _EF_SERIES
#if GPIOC_OUTPUT_ALT || GPIOC_OUTPUT_ALT3
	P_PORTC->MFP = GPIOC_OUTPUT_ALT | GPIOC_OUTPUT_ALT3;
#endif
#else
#if GPIOC_OUTPUT_ALT
	P_PORTC->MFP = GPIOC_OUTPUT_ALT;
#endif
#endif

#if _EF_SERIES
#if GPIOD_OUTPUT_HIGH || GPIOD_OUTPUT_LOW
	P_PORTD->Data = ~GPIOD_OUTPUT_LOW;
	P_PORTD->DIR = ~(GPIOD_OUTPUT_HIGH | GPIOD_OUTPUT_LOW);
#endif
#if GPIOD_INPUT_FLOAT
	P_PORTD->CFG |= GPIOD_INPUT_FLOAT;
#endif
#if GPIOD_OUTPUT_ALT3
	P_PORTD->MFP = GPIOD_OUTPUT_ALT3;
#endif
#endif

#if defined(_GPIO_PULLHIGH_RESISTOR)
    GPIOA->PULL_HIGH_CFG = _GPIO_PULLHIGH_RESISTOR;
#endif
    // Normal/Large
#if defined(_GPIOA_SINK_MODE)
    GPIOA->Wakeup_Mask = (GPIOA->Wakeup_Mask & ~(0xFFFF<<16)) | (_GPIOA_SINK_MODE<<16);
#endif
#if defined(_GPIOB_SINK_MODE)
    GPIOB->Wakeup_Mask = (GPIOB->Wakeup_Mask & ~(0xFFFF<<16)) | (_GPIOB_SINK_MODE<<16);
#endif
#if defined(_GPIOC_SINK_MODE)
    GPIOC->Wakeup_Mask = (GPIOC->Wakeup_Mask & ~(0xFFFF<<16)) | (_GPIOC_SINK_MODE<<16);
#endif
#if _EF_SERIES && defined(_GPIOD_SINK_MODE)
    GPIOD->Wakeup_Mask = (GPIOD->Wakeup_Mask & ~(0xFFFF<<16)) | (_GPIOD_SINK_MODE<<16);
#endif
    // Pull High Resistor
#if defined(_GPIOA_PULLHIGH_RESISTOR)
    GPIOA->CFG &= (~_GPIOA_PULLHIGH_RESISTOR);
#endif
#if defined(_GPIOB_PULLHIGH_RESISTOR)
    GPIOB->CFG &= (~_GPIOB_PULLHIGH_RESISTOR);
#endif
#if defined(_GPIOC_PULLHIGH_RESISTOR)
    GPIOC->CFG &= (~_GPIOC_PULLHIGH_RESISTOR);
#endif
#if _EF_SERIES && defined(_GPIOD_PULLHIGH_RESISTOR)
    GPIOD->CFG &= (~_GPIOD_PULLHIGH_RESISTOR);
#endif
    // Constant Sink(PA8~PA15)
#if !_EF_SERIES && defined(_GPIOA_CONSTANT_SINK) && (_GPIOA_CONSTANT_SINK!=0xFF)
    GPIOA->CFG = (GPIOA->CFG & ~(0xFF<<24)) | (_GPIOA_CONSTANT_SINK<<24);
    SMU_DCMCmd(ENABLE);
#endif

#if !_EF_SERIES
#if _GPIOA_INT
	INTC_IrqCmd(IRQ_BIT_GPIOA,ENABLE);
#endif
#if _GPIOB_INT
	INTC_IrqCmd(IRQ_BIT_GPIOB,ENABLE);
#endif
#if _GPIOC_INT
	INTC_IrqCmd(IRQ_BIT_GPIOC,ENABLE);
#endif
#else
#if _EXIT0_INT
	INTC_IrqCmd(IRQ_BIT_EXIT0, ENABLE);
#endif
#if _EXIT1_INT
	INTC_IrqCmd(IRQ_BIT_EXIT1, ENABLE);
#endif
#endif
}
//------------------------------------------------------------------//
// Module initialization
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SYS_ModuleInit(void)
{

#if (_SPI_ACCESS_MODE==SPI_1_4_4_MODE) && _SPI_MODULE
    U32 id;
    U8  mf_id;
    U16 device_id;
#endif

    //Default Initial
    SMU_PeriphClkCmd(0xFFFFFFFF,DISABLE);
    INTC_IrqCmd(0xFFFFFFFF,DISABLE);
    INTC_ClrPending(0xFFFFFFFF);
    INTC_SetDefaultPriority();

    SMU_PeriphClkCmd(SMU_PERIPH_GPIO,ENABLE);
    //**** IP Selection by nx1_config.h ****//
    //Module Initial
#if _GPIO_MODULE
    SYS_GPIOInit();
#if _GPIO_WAKEUP
	Set_WakeUp_Port();
#endif
#endif

//--------------------------------------------------------------------------
// for PP current,2023-09-14添加
//--------------------------------------------------------------------------
#if (_PP_IDLE_INTO_HALT&&_PP_GPIO_EN)
	GPIO_Init(_PP_GPIO_PORT, _PP_GPIO_PIN, GPIO_INPUT_FLOAT);
#endif

#if _TIMER_MODULE || _AUDIO_MODULE || _PWMA_MODULE || _PWMB_MODULE 									\
	|| (_ACTION_MODULE&&(_VIO_TIMER==USE_TIMER0||_VIO_TIMER==USE_TIMER1||_VIO_TIMER==USE_TIMER2||_VIO_TIMER==USE_TIMER3))	\
	|| (_QFID_MODULE&&(_QFID_TIMER==USE_TIMER0||_QFID_TIMER==USE_TIMER1||_QFID_TIMER==USE_TIMER2||_QFID_TIMER==USE_TIMER3))
    SYS_TIMERInit();
#endif

#if (_SYS_TIMEBASE!=0) || _RTC_MODULE
    SMU_PeriphClkCmd(SMU_PERIPH_RTC,ENABLE);
#endif

#if _EF_SERIES
	SMU_CpuClock(_SYS_SMU_CPU_CLK_DIV);
#else
    SMU_CpuClock(_SYS_SMU_CPU_CLK_DIV,SMU_SYSCLK_FROM_HICLK);
#endif

#if 	_SYS_SMU_LVD_VOLTAGE==LVD_VOLTAGE_2V0 && _EF_SERIES
	SMU_LVDCmd(ENABLE,SMU_LVD_2_0V);
#elif	_SYS_SMU_LVD_VOLTAGE==LVD_VOLTAGE_2V2
	SMU_LVDCmd(ENABLE,SMU_LVD_2_2V);
#elif	_SYS_SMU_LVD_VOLTAGE==LVD_VOLTAGE_2V4
	SMU_LVDCmd(ENABLE,SMU_LVD_2_4V);
#elif	_SYS_SMU_LVD_VOLTAGE==LVD_VOLTAGE_2V6 && !_EF_SERIES
	SMU_LVDCmd(ENABLE,SMU_LVD_2_6V);
#elif	_SYS_SMU_LVD_VOLTAGE==LVD_VOLTAGE_2V8 && _EF_SERIES
	SMU_LVDCmd(ENABLE,SMU_LVD_2_8V);
#elif	_SYS_SMU_LVD_VOLTAGE==LVD_VOLTAGE_3V0 && _EF_SERIES
	SMU_LVDCmd(ENABLE,SMU_LVD_3_0V);
#elif	_SYS_SMU_LVD_VOLTAGE==LVD_VOLTAGE_3V2
	SMU_LVDCmd(ENABLE,SMU_LVD_3_2V);
#elif	_SYS_SMU_LVD_VOLTAGE==LVD_VOLTAGE_3V4
	SMU_LVDCmd(ENABLE,SMU_LVD_3_4V);
#elif	_SYS_SMU_LVD_VOLTAGE==LVD_VOLTAGE_3V6
	SMU_LVDCmd(ENABLE,SMU_LVD_3_6V);
#elif 	_SYS_SMU_LVD_VOLTAGE==LVD_VOLTAGE_OFF
	SMU_LVDCmd(DISABLE,0);
#else
	#error "LVD setting failed."
#endif//LVD

#if _SPI_MODULE
    SMU_PeriphClkCmd(SMU_PERIPH_SPI,ENABLE);
#if _EF_SERIES
    GPIO_Init(GPIOB,SPI0_CSB_MTF_PIN,GPIO_MODE_ALT2);
#if _SPI_ACCESS_MODE==SPI_1_1_1_MODE_3WIRE
#if (_GPIO_PB0_MODE == GPIO_PIN_OFF)
    GPIO_Init(GPIOB,SPI0_MISO_MTF_PIN,GPIO_OUTPUT_LOW);
#endif
#else
    GPIO_Init(GPIOB,SPI0_MISO_MTF_PIN,GPIO_MODE_ALT2);
#endif
#else
    GPIO_Init(GPIOB,SPI0_CEN_MTF_PIN,GPIO_MODE_ALT);
    GPIO_Init(GPIOB,SPI0_SCLK_MTF_PIN,GPIO_MODE_ALT);
#endif
#if (_GPIO_PB4_MODE == GPIO_PIN_OFF) && (_GPIO_PB5_MODE == GPIO_PIN_OFF)
#if _EF_SERIES
	GPIO_Init(GPIOB,SPI0_IO2_MTF_PIN,GPIO_MODE_ALT2);
#else
    GPIO_Init(GPIOB,SPI0_IO2_MTF_PIN,GPIO_MODE_ALT);
#endif
#endif

	SYS_SOFT_ENABLE_LDO();

    SMU_PeriphReset(SMU_PERIPH_SPI);
#if OPTION_HIGH_FREQ_CLK > 32000000
	SPI_Init(SPI_CLKDIV_2, SPI_Pol_Low, SPI_Phase_1st_Edge);
#else
    SPI_Init(SPI_CLKDIV_1, SPI_Pol_Low, SPI_Phase_1st_Edge);
#endif
    //Add 15ms delay for booting Flash safety.
	wait_FlashStable_15ms();

#else
#if (!_EF_SERIES && _GPIO_PB0_5_POWER==INT_PWR)
	SYS_SOFT_ENABLE_LDO();
#endif
#endif

#if  _SPI1_MODULE
	SMU_PeriphClkCmd(SMU_PERIPH_SPI,ENABLE);
#if _SPI_MODULE==DISABLE
	SMU_PeriphReset(SMU_PERIPH_SPI);
#endif
#if _EF_SERIES
	GPIO_Init(GPIOA, SPI1_CSB_MTF_PIN, GPIO_MODE_ALT2);
#if _SPI1_ACCESS_MODE==SPI_1_1_1_MODE_3WIRE
#if (_GPIO_PA15_MODE == GPIO_PIN_OFF)
    GPIO_Init(GPIOA, SPI1_MISO_MTF_PIN, GPIO_OUTPUT_LOW);
#endif
#else
	GPIO_Init(GPIOA, SPI1_MISO_MTF_PIN, GPIO_MODE_ALT2);
#endif
#else
	GPIO_Init(GPIOB, 8, GPIO_OUTPUT_HIGH);	//spi1 cs pin
	GPIO_Init(GPIOB, SPI1_SCLK_MTF_PIN, GPIO_MODE_ALT);
#endif
#if _EF_SERIES && (_SPI_MODULE==DISABLE) && SPI1_LDOSPI0_EN
	SYS_SOFT_ENABLE_LDO();
#endif
#if OPTION_HIGH_FREQ_CLK > 32000000
	SPI1_Init(SPI_CLKDIV_2, SPI_Pol_Low, SPI_Phase_1st_Edge);
#else
    SPI1_Init(SPI_CLKDIV_1, SPI_Pol_Low, SPI_Phase_1st_Edge);
#endif
#endif

#if !(body_none_ADC)
#if (_IS_APPS_USE_ADC_MIC || _IS_ADC_INPUT)
    SMU_PeriphClkCmd(SMU_PERIPH_ADC,ENABLE);
    SMU_PeriphReset(SMU_PERIPH_ADC);
#if _EF_SERIES
	SMU_ALDOCmd(ENABLE);
#if _IS_APPS_USE_ADC_MIC
	GPIOC->MFP = 0xFF;
#endif
#endif
#endif
#endif

#if _AUDIO_MODULE
	SMU_PeriphClkCmd(SMU_PERIPH_DAC,ENABLE);
    AudioCtlInit();
    AUDIO_Init();
#endif

#if _IS_APPS_USE_DAC
	DacAssigner_Init();
#endif

#if _IS_APPS_USE_DAC
    DacMixerRamp_Init();
    DacMixerRamp_SetDacModeUse((_DAC_MODE==ENABLE));
#endif

#if _DIRECTKEY_MODULE
    DIRECTKEY_Init();
#endif

    SysTickInit();
    RTC_IntCmd(NORMAL_RTC_SET,ENABLE);
    INTC_IrqCmd(IRQ_BIT_RTC,ENABLE);

#if _RANDOM_GEN
    Getrandom_x = RandomSeed();
#endif

#if _I2C_MODULE
    I2C_Init();
#endif
#if _I2C_INT
    SMU_PeriphClkCmd(SMU_PERIPH_I2C,ENABLE);
    INTC_IrqCmd(IRQ_BIT_I2C,ENABLE);
#endif

#if _GPIO_I2C_MODULE
	GPIO_I2C_Init();
#endif

#if _IR_MODULE
#if !(_IR_TX_EN||_IR_RX_EN)
#if _EF_SERIES
    SMU_PeriphClkCmd(SMU_PERIPH_TMR,ENABLE);
#else
    SMU_PeriphClkCmd(SMU_PERIPH_IR,ENABLE);
    IR_Init(USE_IR0_TX,IR_STOP_LOW,38500);
#endif
#endif
#if _IR_TX_EN
	IR_TxInit();
#endif
#if _IR_RX_EN
	IR_RxInit();
#if _IR_LOW_POWER_TYPE
    IR_LoPwr_RxInit();
#endif
#endif
#endif

#if _PWMA_MODULE || _PWMB_MODULE || _CAPTURE_MODULE 								\
	|| (_ACTION_MODULE&&(_VIO_TIMER==USE_TIMER_PWMA||_VIO_TIMER==USE_TIMER_PWMB))	\
	|| (_QFID_MODULE&&(_QFID_TIMER==USE_TIMER_PWMA||_QFID_TIMER==USE_TIMER_PWMB))
    SYS_PWMInit();
#endif

#if _UART_MODULE
	SMU_PeriphClkCmd(SMU_PERIPH_UART,ENABLE);
#if defined(_UART_PORT) && _UART_PORT==UART_PORT_PD01
    GPIO_Init(GPIOD,0,GPIO_MODE_ALT3); //PD0 TX
    GPIO_Init(GPIOD,1,GPIO_MODE_ALT3); //PD1 RX
#else
    GPIO_Init(GPIOA,6,GPIO_MODE_ALT); //PA6 TX
    GPIO_Init(GPIOA,7,GPIO_MODE_ALT); //PA7 RX
#endif
#if _DEBUG_MODE
    UartCtlInit(UART_BAUDRATE_115200);
#endif
#endif

#if _ACTION_MODULE
	ACTION_Init();
#endif

#if  _QFID_MODULE
	QFID_Init();
#endif

#if _TOUCHKEY_MODULE
	Touch_Initial();
#endif

    GIE_ENABLE();

#if _SPI_MODULE || (_SPI1_MODULE && _SPI1_USE_FLASH)
    //delay for SPI Flash stable
    Delayms(15);
    // SPI Flash release deep power down mode
#if _SPI_MODULE
    SPI_ByteCmd(SPI_RELEASE_POWER_DOWN_CMD);
#endif
#if _SPI1_MODULE && _SPI1_USE_FLASH
	SPI1_ByteCmd(SPI_RELEASE_POWER_DOWN_CMD);
#endif//#if _SPI1_MODULE && _SPI1_USE_FLASH
	release_PowerDown_ms();
    // SPI Flash Software reset
#if _SPI_MODULE
    SPI_ByteCmd(SPI_RESETEN_CMD);
    SPI_ByteCmd(SPI_RESET_CMD);
#endif
#if _SPI1_MODULE && _SPI1_USE_FLASH
    SPI1_ByteCmd(SPI_RESETEN_CMD);
    SPI1_ByteCmd(SPI_RESET_CMD);
#endif//#if _SPI1_MODULE && _SPI1_USE_FLASH
    Delayms(1);
    // Toggle CS for Reset SPI Flash
#if _SPI_MODULE
	GPIO_Write(SPI0_CS_PORT, SPI0_CS_PIN, 0);
#endif
#if _SPI1_MODULE && _SPI1_USE_FLASH
	GPIO_Write(SPI1_CS_PORT, SPI1_CS_PIN, 0);
#endif//#if _SPI1_MODULE && _SPI1_USE_FLASH
    Delayms(1);
#if _SPI_MODULE
	GPIO_Write(SPI0_CS_PORT, SPI0_CS_PIN, 1);
#endif
#if _SPI1_MODULE && _SPI1_USE_FLASH
	GPIO_Write(SPI1_CS_PORT, SPI1_CS_PIN, 1);
#endif//#if _SPI1_MODULE && _SPI1_USE_FLASH
    Delayms(5);

#if _SPI_MODULE
#if _SPI_ACCESS_MODE==SPI_1_4_4_MODE
	id = SPI_GetVendorID();
	mf_id=id&0xff;
	device_id=id>>8;
	if(SPI_QEBitEnable(id)==EXIT_SUCCESS)
	{
	}
    else if(mf_id == SPI_VENDOR_WINBOND || mf_id == SPI_N25Q_20H)
    {
    	if(SPI_ReadStsReg(2)!=SPI_QE_BIT) //winbond
    	{
    		if(device_id==0x1740)
    		{
    			SPI_WriteStsReg2(0x200); //winbond w25q64fv
    		}
    		else
    		{
    			SPI_WriteStsReg(2,SPI_QE_BIT);
    		}
    		Delayms(15);
    	}
    	//Check
        //dprintf("sts 0x%x id 0x%x winbon\r\n",SPI_ReadStsReg(2),id);
        //dprintf("s1 %x s2 %x s3 %x\r\n",SPI_ReadStsReg(1),SPI_ReadStsReg(2),SPI_ReadStsReg(3));
    }
    else if(mf_id == SPI_VENDOR_MXIC)
    {
    	//SPI_WriteStsReg2(0x0000);
    	if(SPI_ReadStsReg(1)!=0x40) //MXIC
    	{
    		SPI_WriteStsReg2(0x0040);
            Delayms(15);
    	}
    	//Check
    	//dprintf("MXIC sts 0x%x id 0x%x\r\n",SPI_ReadStsReg(1),id);
    }
    else if(mf_id == SPI_PUYA)
    {
			SPI_WriteStsReg2(0x200);
			Delayms(15);
    }
    else if(mf_id == SPI_N25Q_1CH)
    {
			// Quad mode is enable
			// do nothing
    }
    else
    {
#if _QCODE_BUILD && _QC_SPI_SET_QE==0
    	while(1) WDT_Clear();
#endif
    }
#endif//#if _SPI_ACCESS_MODE==SPI_1_4_4_MODE
#if	_SPI_ADDR_BYTE==4
	if(SPI_4ByteAddressEnable(SPI_GetVendorID())!=EXIT_SUCCESS)
	{
		while(1)
		{
			WDT_Clear();
		}
	}
#endif//#if	_SPI_ADDR_BYTE==4
#endif//#if _SPI_MODULE

#if (_SPI1_ADDR_BYTE==4) && (_SPI1_MODULE && _SPI1_USE_FLASH)
	if(SPI1_4ByteAddressEnable(SPI1_GetVendorID())!=EXIT_SUCCESS)
	{
		while(1)
		{
			WDT_Clear();
		}
	}
#endif//#if (_SPI1_ADDR_BYTE==4) && (_SPI1_MODULE && _SPI1_USE_FLASH)

#if _QCODE_BUILD && _BEFORE_POWER_ON_PATH==1
    QC_DIRECT_CALLBACK(QcPath_before_poweron);
#endif

#if (_XIP_IO_MODE==XIP_1_IO_MODE)           \
    || (_XIP_IO_MODE==XIP_2_IO_MODE)        \
    || (_XIP_IO_MODE==XIP_4_IO_MODE)        \
    || (_XIP_IO_MODE==XIP_1_IO_MODE_3WIRE)  \
    || (_XIP_IO_MODE==XIP_2_IO_MODE_112)    \
    || (_XIP_IO_MODE==XIP_4_IO_MODE_114)
    SPI_XipIoMode(_XIP_IO_MODE);
#endif
#if _SPI_MODULE
	SPI_XipRom2Bank(1); //bank_1
#if		_SPI_ADDR_BYTE==4
	SPI_XipAddrMode(C_SPI_Addr_Mode_4Byte);
#elif	_SPI_ADDR_BYTE==3
	SPI_XipAddrMode(C_SPI_Addr_Mode_3Byte);
#else
#endif
#if _SPI_EEPROM_MODULE
	SPI_EEPROM_Init(_SPI_EEPROM_RSRC_INDEX, _SPI_EEPROM_SIZE, _SPI_EEPROM_RINGBUF_NO);
#endif
#endif

#if _SPI_INT
	INTC_IrqCmd(IRQ_BIT_SPI0,ENABLE);
#endif
#endif

#if _SPI1_MODULE
#if _SPI1_INT
	INTC_IrqCmd(IRQ_BIT_SPI1,ENABLE);
#endif
#endif

#if _SYS_WDT_TIME==1
    WDT_Init(WDT_TIME_188ms);
#else
    WDT_Init(WDT_TIME_750ms);
#endif

#if _MIDI_INST_NOTEON==ENABLE
	//INST_NoteOn_INIT();//NOTEON SPI ENABLE
#endif

#if _SW_INT
	INTC_IrqCmd(IRQ_BIT_SWI,ENABLE);
	INTC_IrqCmd(IRQ_BIT_SWA,ENABLE);
#endif
#if _LEDSTR_MODULE
	LEDSTR_Init();
#endif

#if _LEDSTRn_MODULE
	LEDSTRn_Init();
#endif

#if _LEDSTRm_MODULE
	LEDSTRm_Init();
#endif

#if _SL_MODULE
	SL_Init();
#endif
// Delay for ADC voltage stable.
#if _IS_APPS_USE_ADC_MIC
//100ms
#if _EF_SERIES
#if OPTION_HIGH_FREQ_CLK==32000000
	DelayLoop(275);
#else//48M
	DelayLoop(430);
#endif
#else
#if OPTION_HIGH_FREQ_CLK==24000000
	DelayLoop(280);
#elif OPTION_HIGH_FREQ_CLK==16000000
	DelayLoop(180);
#elif OPTION_HIGH_FREQ_CLK==12000000
	DelayLoop(140);
#else//32M
	DelayLoop(360);
#endif
#endif
#endif

#if _SYS_LVD_MONITOR_MODULE
    LVD_Monitor_Init();
#endif

#if		(_SW_PWM_MODULE)
	SW_PWM_Init();
#endif

#if _STORAGE_MODULE
#if _QCODE_BUILD
    QC_STORAGE_Init();
#else
	//自动恢复保存的变量值
    STORAGE_Init(_STORAGE_MODE,_STORAGE_USER_DEF_IDX);
#if _SBC_AUTO_SAVE_TIME //&& _SBC_SEEK
	//底层SBC算法设置定义的变量地址
    SBC_StorageSetVar();
#endif
#endif
#endif

#if _EF_SERIES
	extern U16 PWMCurrentFactor;
	PWMCurrentFactor = NX1EF_getPWMCurrentFactor();
#endif
}
//------------------------------------------------------------------//
// System initialization
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SYS_Init(void)
{
    SYS_ModuleInit();
    DelayLoop2(50);
    WDT_Clear();
#if _QCODE_BUILD
    QcodeInit();
#endif
}
//------------------------------------------------------------------//
// System clear sleep downcount time
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SYS_ClrSleepTime(void)
{
#if _AUDIO_IDLE_INTO_HALT || (_TOUCHKEY_MODULE && _TOUCHKEY_INTO_STANDBY) || (_IR_MODULE && _IR_LOW_POWER_TYPE && _IR_RX_EN)
#if _TOUCHKEY_MODULE
	Touch_SleepTime(C_TouchWakeTime);
#endif
	sleep_time_init = SysTick;
#endif
}
//------------------------------------------------------------------//
// System service loop
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SYS_ServiceLoop(void){
	#if _AUDIO_IDLE_INTO_HALT || (_TOUCHKEY_MODULE && _TOUCHKEY_INTO_STANDBY) || (_IR_MODULE && _IR_LOW_POWER_TYPE && _IR_RX_EN)
    	if(CB_SYS_isAppBusy()==true)
		{
			SYS_ClrSleepTime();
		}

		//----------------------------------------------------------------------------//
        // 用户 空闲检测 add by pp 2023-09-14 14:32:20 Fri
        //----------------------------------------------------------------------------//
        if(STATUS_WORKING == user_system_get_status_for_sleep())
		{
			SYS_ClrSleepTime();
		}

		//->Halt mode
		if(((SysTick - sleep_time_init) > DELAY_SLEEP_TIME)
            && CB_SYS_isSleepable()
	    ){
			//----------------------------------------------------------------------------//
            // 用户 睡眠前处理
            //----------------------------------------------------------------------------//
            user_system_off_process();

			SYS_Sleep();
			sleep_time_init = SysTick;							//initial time base

			//----------------------------------------------------------------------------//
            // 用户 唤醒后处理
            //----------------------------------------------------------------------------//
			user_system_wakeup_process();
		}

	#endif

    WDT_Clear();
}

//------------------------------------------------------------------//
// System into standby mode
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
#if defined(SLEEP_USE_RTC_64HZ) || defined(SLEEP_USE_RTC_2HZ) \
||  defined(SLEEP_USE_TIMER0) || defined(SLEEP_USE_TIMER1) || defined(SLEEP_USE_TIMER2) || defined(SLEEP_USE_TIMER3) \
||  defined(SLEEP_USE_EXT0_INT)
void SYS_IntoStandby()
{
    volatile U32 _intcCmdTemp = __nds32__mfsr(NDS32_SR_INT_MASK2);
    INTC_IrqCmd(0xFFFFFFFF,DISABLE);//Stop All interrupt, but can wakeup by RTC
#if defined(SLEEP_USE_RTC_64HZ) || defined(SLEEP_USE_RTC_2HZ)
    RTC_IntCmd(SLEEP_RTC_SET,ENABLE);
#endif
#if defined(SLEEP_USE_TIMER0)
    TIMER_IntCmd(TIMER0, ENABLE);
#endif
#if defined(SLEEP_USE_TIMER1)
    TIMER_IntCmd(TIMER1, ENABLE);
#endif
#if defined(SLEEP_USE_TIMER2)
    TIMER_IntCmd(TIMER2, ENABLE);
#endif
#if defined(SLEEP_USE_TIMER3)
    TIMER_IntCmd(TIMER3, ENABLE);
#endif
#if defined(SLEEP_USE_EXT0_INT)
	GPIO_ExtIntCmd(TRIG_EXT0, ENABLE);
#endif

    //init before sleep
#if _TOUCHKEY_MODULE && _TOUCHKEY_INTO_STANDBY
    Touch_InitBeforeStandby();
#endif
#if (_IR_MODULE && _IR_LOW_POWER_TYPE && _IR_RX_EN)
    IR_LoPwr_RxInitBeforeStandby();
#endif
    do {
        SMU_Sleep(SMU_STANDBY_MODE);
        INTC_ClrPending(SLEEP_CLEAR_IRQ_PENDING);

#if defined(SLEEP_USE_RTC_64HZ) || defined(SLEEP_USE_RTC_2HZ)
        U8 _rtcFlag=P_RTC->Flag;
        P_RTC->Flag = 0xf;       // clear rtc flag
        RTC_Int_ReEn();
#endif
        bool isWakeup = false;
        if(SYS_IsWakeupPinChange()==true) {
            isWakeup=true;
        }
#if defined(SLEEP_USE_RTC_64HZ)
        if(_rtcFlag & RTC_64HZ) {
#if _TOUCHKEY_MODULE && _TOUCHKEY_INTO_STANDBY && C_TouchSleepFrame<500
            if(Touch_ProcessInStandby()!=0) {
                isWakeup=true;
            }
#endif
        }
#endif
#if defined(SLEEP_USE_RTC_2HZ)
        if(_rtcFlag & RTC_2HZ) {
#if body_MultiTrgLdo
            SYS_MULTI_TRG_LDO();
#endif
#if _QCODE_BUILD && _RTC_2Hz_PATH && _SLEEP_MODE_TYPE==1
            QC_ResetEnforceWakeup();
            QC_DIRECT_CALLBACK(QC_Rtc2HzPath);
            if(QC_IsEnforceWakeup()==true) {
                isWakeup=true;
            }
#endif
#if _TOUCHKEY_MODULE && _TOUCHKEY_INTO_STANDBY && C_TouchSleepFrame>=500
            if(Touch_ProcessInStandby()!=0) {
                isWakeup=true;
            }
#endif
#if (_IR_MODULE && _IR_LOW_POWER_TYPE==TYPE1 && _IR_RX_EN)
            if(IR_LoPwr_RxProcess()) {
            	isWakeup=true;
            }
#endif
        }
#endif
#if defined(SLEEP_USE_TIMER0)
    if(TIMER0->Flag) {
        TIMER0->Flag=1;
#if (_IR_MODULE && _IR_LOW_POWER_TYPE==TYPE1 && _IR_RX_EN)
        if(IR_LoPwr_RxProcess()) {
            isWakeup=true;
        }
#endif
    }
#endif
#if defined(SLEEP_USE_TIMER1)
    if(TIMER1->Flag) {
        TIMER1->Flag=1;
#if (_IR_MODULE && _IR_LOW_POWER_TYPE==TYPE1 && _IR_RX_EN)
        if(IR_LoPwr_RxProcess()) {
            isWakeup=true;
        }
#endif
    }
#endif
#if defined(SLEEP_USE_TIMER2)
    if(TIMER2->Flag) {
        TIMER2->Flag=1;
#if (_IR_MODULE && _IR_LOW_POWER_TYPE==TYPE1 && _IR_RX_EN)
        if(IR_LoPwr_RxProcess()) {
            isWakeup=true;
        }
#endif
    }
#endif
#if defined(SLEEP_USE_TIMER3)
    if(TIMER3->Flag) {
        TIMER3->Flag=1;
#if (_IR_MODULE && _IR_LOW_POWER_TYPE==TYPE1 && _IR_RX_EN)
        if(IR_LoPwr_RxProcess()) {
            isWakeup=true;
        }
#endif
    }
#endif
#if defined(SLEEP_USE_EXT0_INT)
	if(P_PORT_EXT->EXT_INT_Flag & (1<<TRIG_EXT0)) {
		P_PORT_EXT->EXT_INT_Flag = (1<<TRIG_EXT0);
#if (_IR_MODULE && _IR_LOW_POWER_TYPE==TYPE2 && _IR_RX_EN)
        if(IR_LoPwr_RxProcess()) {
            isWakeup=true;
        }
#endif
	}
#endif

        if(isWakeup) break;//go to wakeup
    } while(1);
    //init after wakeup
#if _TOUCHKEY_MODULE && _TOUCHKEY_INTO_STANDBY
    Touch_InitAfterStandby();
#endif
#if (_IR_MODULE && _IR_LOW_POWER_TYPE && _IR_RX_EN)
    IR_LoPwr_RxInitAfterStandby();
#endif
    RTC_IntCmd(0xF,DISABLE);//close all RTC
    INTC_IrqCmd(_intcCmdTemp,ENABLE);
}
#endif
//------------------------------------------------------------------//
// System Sleep
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SYS_Sleep(void)
{
#if _STORAGE_MODULE && _STORAGE_AUTO_SAVE
    STORAGE_Save(_STORAGE_MODE);
#endif

#if _SYS_LVD_MONITOR_MODULE
    LVD_Monitor_Sleep();
#endif

#if _SW_PWM_MODULE
	SW_PWM_Sleep();
#endif

#if _ACTION_MODULE
    ACTION_Sleep();
#endif

//--------------------------------------------------------------------------
// for PP current,2023-09-14添加
//--------------------------------------------------------------------------
#if _PP_IDLE_INTO_HALT
	U8 DAC_trig_status = DAC_Trig_Timer_disable();
#else
	DAC_Trig_Timer_disable();
#endif

#if _IR_MODULE&&_IR_RX_EN
	if(!IR_CheckRxSleep())
	{
		goto BYPASS_FLOW;
	}
#endif

#if _DIRECTKEY_MODULE
	if(KeyScan_DebounceCheck()){
		goto BYPASS_FLOW;
	}
#if _GPIO_WAKEUP
#if GPIOA_WAKEUPMASK
	P_PORTA->Data = (KeyScan_CheckedData(0) & KeyScan_UsedPin(0)) | (P_PORTA->PAD & (~KeyScan_UsedPin(0)));
#endif
#if GPIOB_WAKEUPMASK
	P_PORTB->Data = (KeyScan_CheckedData(1) & KeyScan_UsedPin(1)) | (P_PORTB->PAD & (~KeyScan_UsedPin(1)));
#endif
#if GPIOC_WAKEUPMASK
	P_PORTC->Data = (KeyScan_CheckedData(2) & KeyScan_UsedPin(2)) | (P_PORTC->PAD & (~KeyScan_UsedPin(2)));
#endif
#if _EF_SERIES
#if GPIOD_WAKEUPMASK
	P_PORTD->Data = (KeyScan_CheckedData(3) & KeyScan_UsedPin(3)) | (P_PORTD->PAD & (~KeyScan_UsedPin(3)));
#endif
#endif
#endif

#else
#if _GPIO_WAKEUP
#if GPIOA_WAKEUPMASK
	P_PORTA->Data = P_PORTA->PAD;
#endif
#if GPIOB_WAKEUPMASK
	P_PORTB->Data = P_PORTB->PAD;
#endif
#if GPIOC_WAKEUPMASK
	P_PORTC->Data = P_PORTC->PAD;
#endif
#if _EF_SERIES
#if GPIOD_WAKEUPMASK
	P_PORTD->Data = P_PORTD->PAD;
#endif
#endif
#endif
#endif

//--------------------------------------------------------------------------
// for PP current,2023-09-14添加
//--------------------------------------------------------------------------
#if _PP_IDLE_INTO_HALT
	// if(DAC_trig_status!=DAC_trig_None)
	{
		PP_Enter_Halt(DAC_trig_status,ENABLE);
		SMU_PeriphClkCmd(SMU_PERIPH_DAC,DISABLE);
	}
#endif

    //ADC FIFO convert disable
#if !(body_none_ADC)
    U8 ADC_fifo_ctrl_save = (P_ADC->FIFO_Ctrl) & (C_ADC_CNV_Done_IEn | C_ADC_FIFO_Level_IEn | C_ADC_FIFO_Empty_IEn | C_ADC_FIFO_Full_IEn);
    P_ADC->FIFO_Ctrl &= ~(C_ADC_CNV_Done_IEn | C_ADC_FIFO_Level_IEn | C_ADC_FIFO_Empty_IEn | C_ADC_FIFO_Full_IEn);
#if _EF_SERIES
#if (_IS_APPS_USE_ADC_MIC||_IS_ADC_INPUT)
	SMU_ALDOCmd(DISABLE);
#endif
#if (_IS_APPS_USE_ADC_MIC)
	GPIOC->MFP = 0;
	GPIOC->DIR = 0;
	GPIOC->Data = 0;
#endif
#endif
#endif

#if _QCODE_BUILD && _SLEEP_PATH
    QC_DIRECT_CALLBACK(QC_SleepPath);
#endif

	SYS_SpiPowerOff();
	SYS_NX1F_SPI_MODE_INTO_HALT(ENABLE);
	//close RTC
    RTC_IntCmd(0xF,DISABLE);//close all RTC

///////////////////////////////////////////////////////////////////////////////////////
	//----------------------------------------------------------------------------//
	// 再次睡眠的状态检测，看是否满足睡眠条件，2023-09-14添加
	// 注：
	//    为了防止：通信时，刚进入刚函数，然后接收到了一个通信码(中断接收)，没有被处理，然后就睡眠了。
	//    所以，通信时，需要添加，一定是通信码被处理后并且没有正在通信，才可以睡眠。
	//----------------------------------------------------------------------------//
	// standby or halt mode
	if(STATUS_IDLE == user_system_get_status_for_sleep())
	{
		#if (defined(SLEEP_USE_RTC_64HZ) || defined(SLEEP_USE_RTC_2HZ) \
		||  defined(SLEEP_USE_TIMER0) || defined(SLEEP_USE_TIMER1) || defined(SLEEP_USE_TIMER2) || defined(SLEEP_USE_TIMER3) \
		||  defined(SLEEP_USE_EXT0_INT))
				SYS_IntoStandby();// into standby mode
		#else
				SMU_Sleep(C_SLP_Halt_Mode);// into halt mode
		#endif
	}
///////////////////////////////////////////////////////////////////////////////////////

#if _QCODE_BUILD && _WAKEUP_PATH
    QC_DIRECT_CALLBACK(QC_WakeupPath);
#endif

    //RTC wake_up
    RTC_IntCmd(NORMAL_RTC_SET,ENABLE);
	SYS_NX1F_SPI_MODE_INTO_HALT(DISABLE);
	SYS_SpiPowerOn();

//--------------------------------------------------------------------------
// for PP current,2023-09-14添加
//--------------------------------------------------------------------------
#if _PP_IDLE_INTO_HALT
	// if(DAC_trig_status!=DAC_trig_None)
	{
		PP_Enter_Halt(DAC_trig_status,DISABLE);
		SMU_PeriphClkCmd(SMU_PERIPH_DAC,ENABLE);
	}
#endif

    //ADC wake up
#if !(body_none_ADC)
#if _EF_SERIES
#if (_IS_APPS_USE_ADC_MIC)
	GPIOC->MFP = 0xFF;
#endif
#if (_IS_APPS_USE_ADC_MIC || _IS_ADC_INPUT)
	SMU_ALDOCmd(ENABLE);
#endif
#endif
    P_ADC->FIFO_Ctrl |= ADC_fifo_ctrl_save;
#endif

#if (_IR_MODULE&&_IR_RX_EN) \
    || _DIRECTKEY_MODULE
BYPASS_FLOW:;
#endif

#if _ACTION_MODULE
    ACTION_Wakeup();
#endif

#if _SW_PWM_MODULE
	SW_PWM_WakeUp();
#endif

#if _SYS_LVD_MONITOR_MODULE
    LVD_Monitor_WakeUp();
#endif
}
//------------------------------------------------------------------//
// Get rolling code
// Parameter:
//          NONE
// return value:
//          return rolling code
//------------------------------------------------------------------//
U32 SYS_GetRollingCode(void)
{
	U32	rolling_code;
#if _EF_SERIES
	rolling_code = NX1EF_getRollingCode();	// 3 Bytes
#else
	rolling_code = *P_ROLLING_CODE;
#endif

	return rolling_code;
}
//------------------------------------------------------------------//
// Set Wake up pin
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void Set_WakeUp_Port(void)
{
#if GPIOA_WAKEUPMASK
    // Config Port A Wakeup
	P_PORTA->Wakeup_Mask |= GPIOA_WAKEUPMASK;
#endif
#if GPIOB_WAKEUPMASK
    // Config Port B Wakeup
	P_PORTB->Wakeup_Mask |= GPIOB_WAKEUPMASK;
#endif
#if GPIOC_WAKEUPMASK
    // Config Port C Wakeup
	P_PORTC->Wakeup_Mask |= GPIOC_WAKEUPMASK;
#endif
#if _EF_SERIES
#if GPIOD_WAKEUPMASK
    // Config Port D Wakeup
	P_PORTD->Wakeup_Mask |= GPIOD_WAKEUPMASK;
#endif
#endif
}
//------------------------------------------------------------------//
// wait 1 ms approximately
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void release_PowerDown_ms(void)
{
#if _EF_SERIES
#if OPTION_HIGH_FREQ_CLK==32000000
	DelayLoop(3);
#else//48M
	DelayLoop(5);
#endif
#else
#if OPTION_HIGH_FREQ_CLK==24000000
		DelayLoop(3);
#elif OPTION_HIGH_FREQ_CLK==16000000
		DelayLoop(2);
#elif OPTION_HIGH_FREQ_CLK==12000000
		DelayLoop(2);
#else//32M
		DelayLoop(4);
#endif
#endif
}
//------------------------------------------------------------------//
// wait 15 ms approximately
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void wait_FlashStable_15ms(void)
{
#if _EF_SERIES
#if OPTION_HIGH_FREQ_CLK==32000000
	DelayLoop(44);
#else//48M
	DelayLoop(66);
#endif
#else
#if OPTION_HIGH_FREQ_CLK==24000000
	DelayLoop(42);
#elif OPTION_HIGH_FREQ_CLK==16000000
	DelayLoop(27);
#elif OPTION_HIGH_FREQ_CLK==12000000
	DelayLoop(21);
#else//32M
	DelayLoop(54);
#endif
#endif
}
//------------------------------------------------------------------//
// ENABLE or Disable PP enter halt mode flow
// Parameter:
//			DAC_status: 0:DAC_CH no select, 1:DAC_CH0, 2:DAC:CH1
//          cmd:		ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------//
void PP_Enter_Halt(U8 DAC_status,U8 cmd)
{
    // U16 fifo_data0,fifo_data1;


    if(cmd==ENABLE)
    {
			//--------------------------------------------------------------------------
			// for PP current,2023-09-14添加
			// PP disable flow before enter halt mode
			//--------------------------------------------------------------------------
			#if (_PP_IDLE_INTO_HALT&&_PP_GPIO_EN)
				_PP_GPIO_PORT->MFP &= (~(1<<_PP_GPIO_PIN));
				_PP_GPIO_PORT->DIR &= (~(1<<_PP_GPIO_PIN));
			#endif

		#if 0
			if(DAC_status&1)
			{
				#if _DAC_CH0_TIMER==USE_TIMER0
            		TIMER_IntCmd(TIMER0,DISABLE);
      			#elif _DAC_CH0_TIMER==USE_TIMER1
      				TIMER_IntCmd(TIMER1,DISABLE);
      			#else
      				TIMER_IntCmd(TIMER2,DISABLE);
      			#endif

				P_DAC->CH0.CH_FIFO |= C_DAC_FIFO_Reset;
				P_DAC->CH0.CH_Ctrl &= (~C_DAC_Trig_Timer_En);
				P_DAC->CH0.CH_Ctrl &= (~(C_DAC_Trig|C_DAC_Up_Sample_En));
				P_DAC->CH0.CH_Ctrl |= DAC_MANUAL_TRIG;
			}
			if(DAC_status&2)
			{
				#if _DAC_CH1_TIMER==USE_TIMER0
            		TIMER_IntCmd(TIMER0,DISABLE);
      			#elif _DAC_CH1_TIMER==USE_TIMER1
      				TIMER_IntCmd(TIMER1,DISABLE);
      			#else
      				TIMER_IntCmd(TIMER2,DISABLE);
      			#endif

				P_DAC->CH1.CH_FIFO |= C_DAC_FIFO_Reset;
				P_DAC->CH1.CH_Ctrl &= (~C_DAC_Trig_Timer_En);
				P_DAC->CH1.CH_Ctrl &= (~(C_DAC_Trig|C_DAC_Up_Sample_En));
				P_DAC->CH1.CH_Ctrl |= DAC_MANUAL_TRIG;
			}

			//
			DAC_VoltageDacCmd(ENABLE);

			//no Ramp down
			//###################
//			P_DAC->CH0.CH_Data=0x8000;
			//###################

			//Ramp down
			//###################
			if(DAC_status&1)
			{
				fifo_data0=0;
        		P_DAC->CH0.CH_Data=fifo_data0;
        	}
        	fifo_data1=0;
        	P_DAC->CH1.CH_Data=fifo_data1;
        	while(1)
        	{
            	fifo_data1--;
            	P_DAC->CH1.CH_Data=fifo_data1;
            	if(DAC_status&1)
				{
            		fifo_data0--;
            		P_DAC->CH0.CH_Data=fifo_data0;
            		if(fifo_data0==0x8000 && fifo_data1==0x8000)
            		{
                	break;
            		}
            	}
				else
				{
					if(fifo_data1==0x8000)
                 	{
                	break;
            		}
            	}
            	DelayLoop2(1);
        	}
			//###################

			//Delay 1ms
			#if OPTION_HIGH_FREQ_CLK==24000000
				DelayLoop(2);
			#elif OPTION_HIGH_FREQ_CLK==16000000
				DelayLoop(1);
			#elif OPTION_HIGH_FREQ_CLK==12000000
				DelayLoop(1);
			#else//32M
				DelayLoop(2);
			#endif

		#if _EF_SERIES
			DAC_PWMCmd(DISABLE);
		#else
			DAC_PPCmd(DISABLE);
		#endif
	#endif
			//--------------------------------------------------------------------------
			// for PP current,2023-09-14添加
			// GPIO set to output high
			//--------------------------------------------------------------------------
			#if (_PP_IDLE_INTO_HALT&&_PP_GPIO_EN)
				_PP_GPIO_PORT->Data |= (1<<_PP_GPIO_PIN);
			#endif

			//Delay ~12ms
			#if OPTION_HIGH_FREQ_CLK==24000000
				DelayLoop(60);
			#elif OPTION_HIGH_FREQ_CLK==16000000
				DelayLoop(40);
			#elif OPTION_HIGH_FREQ_CLK==12000000
				DelayLoop(40);
			#else//32M
				DelayLoop(80);
			#endif

			//--------------------------------------------------------------------------
			// for PP current,2023-09-14添加
			// GPIO set to output low
			//--------------------------------------------------------------------------
			#if (_PP_IDLE_INTO_HALT&&_PP_GPIO_EN)
				_PP_GPIO_PORT->Data &= (~(1<<_PP_GPIO_PIN));
    		#endif
    }
    else
    {
            //--------------------------------------------------------------------------
			// for PP current,2023-09-14添加
			// GPIO set to floating
			//--------------------------------------------------------------------------
			#if (_PP_IDLE_INTO_HALT&&_PP_GPIO_EN)
            	_PP_GPIO_PORT->MFP &= (~(1<<_PP_GPIO_PIN));
        		_PP_GPIO_PORT->DIR  |= (1<<_PP_GPIO_PIN);
        		_PP_GPIO_PORT->CFG |= (1<<_PP_GPIO_PIN);
            #endif

		#if 0
            if(DAC_status&1)
            {
            	P_DAC->CH0.CH_Ctrl &= (~C_DAC_Trig);

            	#if _DAC_CH0_TIMER==USE_TIMER0
            		P_DAC->CH0.CH_Ctrl |= DAC_TIMER0_TRIG;
            		TIMER_IntCmd(TIMER0,ENABLE);
      			#elif _DAC_CH0_TIMER==USE_TIMER1
      				P_DAC->CH0.CH_Ctrl |= DAC_TIMER1_TRIG;
      				TIMER_IntCmd(TIMER1,ENABLE);
      			#else
      				P_DAC->CH0.CH_Ctrl |= DAC_TIMER2_TRIG;
      				TIMER_IntCmd(TIMER2,ENABLE);
      			#endif
      			P_DAC->CH0.CH_Ctrl |= C_DAC_Trig_Timer_En;
      			P_DAC->CH0.CH_Ctrl |= C_DAC_Up_Sample_En;
      		}
      		if(DAC_status&2)
      		{
      			P_DAC->CH1.CH_Ctrl &= (~C_DAC_Trig);

      			#if _DAC_CH1_TIMER==USE_TIMER0
            		P_DAC->CH1.CH_Ctrl |= DAC_TIMER0_TRIG;
            		TIMER_IntCmd(TIMER0,ENABLE);
      			#elif _DAC_CH1_TIMER==USE_TIMER1
      				P_DAC->CH1.CH_Ctrl |= DAC_TIMER1_TRIG;
      				TIMER_IntCmd(TIMER1,ENABLE);
      			#else
      				P_DAC->CH1.CH_Ctrl |= DAC_TIMER2_TRIG;
      				TIMER_IntCmd(TIMER2,ENABLE);
      			#endif
      		    P_DAC->CH1.CH_Ctrl |= C_DAC_Trig_Timer_En;
      			P_DAC->CH1.CH_Ctrl |= C_DAC_Up_Sample_En;
			}
		#endif
    }
}
//------------------------------------------------------------------//
// Tune off spi power and init flash
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SYS_SpiPowerOff(void)
{
    //SPI flash power-down
#if _SPI_MODULE
    SPI_ByteCmd(SPI_POWER_DOWN_CMD);
#endif
#if _SPI1_MODULE && _SPI1_USE_FLASH
	SPI1_ByteCmd(SPI_POWER_DOWN_CMD);
#endif//#if _SPI1_MODULE && _SPI1_USE_FLASH

#if 	(body_disable_ldo) \
	&& 	(!body_MultiTrgLdo) \
    &&  !(_QCODE_BUILD && _IC_BODY==0x11A21 && _SLEEP_MODE_TYPE==1) \
	&& 	((_EF_SERIES && (_SPI_MODULE || (_SPI1_MODULE && SPI1_LDOSPI0_EN))) || (!_EF_SERIES && (SPI_KEEP_VDD_IN_SLEEP==DISABLE))) \
	&& 	((_EF_SERIES && (_SPI_MODULE || (_SPI1_MODULE && SPI1_LDOSPI0_EN))) || (!_EF_SERIES && (_SPI_MODULE || _GPIO_PB0_5_POWER==INT_PWR)))

    SMU_LDO33Cmd(DISABLE);	//disable LDOSPI0
#endif
}
//------------------------------------------------------------------//
// Tune on spi power and init flash
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void SYS_SpiPowerOn(void)
{
#if 	(body_disable_ldo) \
	&& 	(!body_MultiTrgLdo) \
    &&  !(_QCODE_BUILD && _IC_BODY==0x11A21 && _SLEEP_MODE_TYPE==1) \
	&& 	((_EF_SERIES && (_SPI_MODULE || (_SPI1_MODULE && SPI1_LDOSPI0_EN))) || (!_EF_SERIES && (SPI_KEEP_VDD_IN_SLEEP==DISABLE))) \
	&& 	((_EF_SERIES && (_SPI_MODULE || (_SPI1_MODULE && SPI1_LDOSPI0_EN))) || (!_EF_SERIES && (_SPI_MODULE || _GPIO_PB0_5_POWER==INT_PWR)))

	SYS_SOFT_ENABLE_LDO();	//Enable LDOSPI0
#endif

    //SPI flash wake up
#if _SPI_MODULE || (_SPI1_MODULE && _SPI1_USE_FLASH)
	wait_FlashStable_15ms();//15ms delay for Flash ready
#if _SPI_MODULE
    SPI_ByteCmd(SPI_RELEASE_POWER_DOWN_CMD);
#endif
#if _SPI1_MODULE && _SPI1_USE_FLASH
	SPI1_ByteCmd(SPI_RELEASE_POWER_DOWN_CMD);
#endif//#if _SPI1_MODULE && _SPI1_USE_FLASH
    release_PowerDown_ms();
#if	_SPI_MODULE && _SPI_ADDR_BYTE==4
	SPI_4ByteAddressEnable(SPI_GetVendorID());
#endif//#if	_SPI_MODULE && _SPI_ADDR_BYTE==4
#if (_SPI1_MODULE && _SPI1_USE_FLASH) && _SPI1_ADDR_BYTE==4
	SPI1_4ByteAddressEnable(SPI1_GetVendorID());
#endif//#if (_SPI1_MODULE && _SPI1_USE_FLASH) && _SPI1_ADDR_BYTE==4
#endif//#if _SPI_MODULE || (_SPI1_MODULE && _SPI1_USE_FLASH)
}
//------------------------------------------------------------------//
// Get the SPI-Flash address offset
// Parameter:
//          NONE
// return value:
//          SPI-Flash address offset
//------------------------------------------------------------------//
U32 SpiOffset(void)
{
    U8 tempbuf[4];
    U32 start_addr;

    SPI_BurstRead(tempbuf,SPI_RESERVED_SIZE-4,4);
    start_addr=tempbuf[0]|(tempbuf[1]<<8)|(tempbuf[2]<<16)|(tempbuf[3]<<24);

	return start_addr;
}
#if _SPI1_MODULE && _SPI1_USE_FLASH
//------------------------------------------------------------------//
// Get the SPI1-Flash address offset
// Parameter:
//          NONE
// return value:
//          SPI1-Flash address offset
//------------------------------------------------------------------//
U32 Spi1_Offset(void)
{
    U8 tempbuf[4];
    U32 start_addr;

    SPI1_BurstRead(tempbuf,SPI_RESERVED_SIZE-4,4);
    start_addr=tempbuf[0]|(tempbuf[1]<<8)|(tempbuf[2]<<16)|(tempbuf[3]<<24);

	return start_addr;
}
#endif
//------------------------------------------------------------------//
// Generate random
// Parameter:
//          totalNumber: 2 ~ 4294967295
// return value:
//          Random value = 0 ~ (totalNumber-1)
//------------------------------------------------------------------//
#if _RANDOM_GEN
U32 Random(U32 totalNumber)
{
	U32 Getrandom_y;

	if (totalNumber > 1) Getrandom_y = (RANDOM_AI*Getrandom_x+RANDOM_CI)%totalNumber;
	else Getrandom_y = (Getrandom_x & 1);

	if ((Getrandom_x & 0xFF)>50) Getrandom_x += (RANDOM_AI*SysTick+RANDOM_CI);
	else Getrandom_x += SysTick;

	return Getrandom_y;
}
U32 RandomSeed(void)
{
    U8 i;
    U32 seed = 0;
	volatile U32 count;

#if _EF_SERIES
	TIMER0->Ctrl  = (C_TMR_CLK_Src_LoCLK | C_TMR_Auto_Reload_En);
#else
	TIMER0->Ctrl  = (C_TMR_CLK_Src_LoCLK | TIMER_CLKDIV_1 | C_TMR_Auto_Reload_En);
#endif
	TIMER0->Data = 10;

	for (i = 0; i <10; i++) {
		count = 0;
		TIMER_Cmd(TIMER0,ENABLE);
		do {
			count++;
		} while (TIMER0->Data >> 15);
		seed += ((seed^count)<<(10-i));

		Delayms(1);
	}
	TIMER_Cmd(TIMER0,DISABLE);
	return seed;
}
#endif

//------------------------------------------------------------------//
// Is wakeup pin change?(For touch wakeup)
// Parameter:
//          NONE
// return value:
//          true	:Pin is change
//			false	:Pin is not change
//------------------------------------------------------------------//
bool SYS_IsWakeupPinChange(void)
{
#if _GPIO_WAKEUP
#if GPIOA_WAKEUPMASK
	if(((P_PORTA->Data^P_PORTA->PAD)&(U16)P_PORTA->Wakeup_Mask)!=0)return true;
#endif
#if GPIOB_WAKEUPMASK
	if(((P_PORTB->Data^P_PORTB->PAD)&(U16)P_PORTB->Wakeup_Mask)!=0)return true;
#endif
#if GPIOC_WAKEUPMASK
	if(((P_PORTC->Data^P_PORTC->PAD)&(U16)P_PORTC->Wakeup_Mask)!=0)return true;
#endif
#if _EF_SERIES
#if GPIOD_WAKEUPMASK
	if(((P_PORTD->Data^P_PORTD->PAD)&(U16)P_PORTD->Wakeup_Mask)!=0)return true;
#endif
#endif
#endif
	return false;
}

//------------------------------------------------------------------//
// Output 1KHz wave(For test use)
// Parameter:
//          gpiox : gpio group, GPIOA,GPIOB,GPIOC,GPIOD
//          pin   : pin number(0~15)
//          ms    : wave duration time(0:infinite,1~15000:available)
// return value:
//          true	:Pin is change
//			false	:Pin is not change
//------------------------------------------------------------------//
void __attribute__((optimize("O0"),aligned(4))) SYS_OUTPUT_1KHz(GPIO_TypeDef *gpiox, U8 pin, U32 ms){

#define _DELAY_CNT_SETTING	(((OPTION_HIGH_FREQ_CLK/2000)-40)/10)//40+10*n

	if(ms > 15000) return;//illegal
	U32 _delayCnt;
	U16 _gpioBitUse = (0x1<<pin);

	//init
	GPIO_Init(gpiox,pin,GPIO_MODE_OUT_HI);
	ms *= 2;

	GIE_DISABLE();
	if(!ms){

		while(1) {
			__asm__("nop\t\n");
			_delayCnt = _DELAY_CNT_SETTING;
			__asm__("nop\t\n");
			while(_delayCnt--) {
				__asm__("nop\t\n");
#if _EF_SERIES
#else
				__asm__("nop\t\n");
				__asm__("nop\t\n");
				__asm__("nop\t\n");
#endif
			}
			__asm__("nop\t\n");
#if _EF_SERIES
#else
			__asm__("nop\t\n");
			__asm__("nop\t\n");
			__asm__("nop\t\n");
			__asm__("nop\t\n");
			__asm__("nop\t\n");
			__asm__("nop\t\n");
			__asm__("nop\t\n");
			__asm__("nop\t\n");
			__asm__("nop\t\n");
			__asm__("nop\t\n");
			__asm__("nop\t\n");
			__asm__("nop\t\n");
			__asm__("nop\t\n");
#endif
			P_WDT->CLR = C_WDT_CLR_Data;
			gpiox->Data ^= _gpioBitUse;
		}
	} else {
		while(ms--) {
			__asm__("nop\t\n");
			_delayCnt = _DELAY_CNT_SETTING;
			while(_delayCnt--) {
				__asm__("nop\t\n");
#if _EF_SERIES
#else
				__asm__("nop\t\n");
				__asm__("nop\t\n");
				__asm__("nop\t\n");
#endif
			}
			__asm__("nop16\t\n");
#if _EF_SERIES
#else
			__asm__("nop\t\n");
			__asm__("nop\t\n");
			__asm__("nop\t\n");
			__asm__("nop\t\n");
			__asm__("nop\t\n");
			__asm__("nop\t\n");
			__asm__("nop\t\n");
			__asm__("nop\t\n");
			__asm__("nop\t\n");
			__asm__("nop\t\n");
#endif
			P_WDT->CLR = C_WDT_CLR_Data;
			gpiox->Data ^= _gpioBitUse;
		}
	}
	GIE_ENABLE();
}
