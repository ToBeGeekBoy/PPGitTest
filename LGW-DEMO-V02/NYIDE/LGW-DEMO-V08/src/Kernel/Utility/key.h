#ifndef _DIRECTKEY_H
#define _DIRECTKEY_H
#include "include.h"
#include "nx1_gpio.h"
#include <string.h>
#define GPIOA_MASK                  ((_DIRECTKEY_PA15<<15)  \
                                    |(_DIRECTKEY_PA14<<14)  \
                                    |(_DIRECTKEY_PA13<<13)  \
                                    |(_DIRECTKEY_PA12<<12)  \
                                    |(_DIRECTKEY_PA11<<11)  \
                                    |(_DIRECTKEY_PA10<<10)  \
                                    |(_DIRECTKEY_PA9<<9)    \
                                    |(_DIRECTKEY_PA8<<8)    \
                                    |(_DIRECTKEY_PA7<<7)    \
                                    |(_DIRECTKEY_PA6<<6)    \
                                    |(_DIRECTKEY_PA5<<5)    \
                                    |(_DIRECTKEY_PA4<<4)    \
                                    |(_DIRECTKEY_PA3<<3)    \
                                    |(_DIRECTKEY_PA2<<2)    \
                                    |(_DIRECTKEY_PA1<<1)    \
                                    |_DIRECTKEY_PA0)
#define GPIOB_MASK                  ((_DIRECTKEY_PB15<<15)  \
                                    |(_DIRECTKEY_PB14<<14)  \
                                    |(_DIRECTKEY_PB13<<13)  \
                                    |(_DIRECTKEY_PB12<<12)  \
                                    |(_DIRECTKEY_PB11<<11)  \
                                    |(_DIRECTKEY_PB10<<10)  \
                                    |(_DIRECTKEY_PB9<<9)    \
                                    |(_DIRECTKEY_PB8<<8)    \
                                    |(_DIRECTKEY_PB7<<7)    \
                                    |(_DIRECTKEY_PB6<<6)    \
                                    |(_DIRECTKEY_PB5<<5)    \
                                    |(_DIRECTKEY_PB4<<4)    \
                                    |(_DIRECTKEY_PB3<<3)    \
                                    |(_DIRECTKEY_PB2<<2)    \
                                    |(_DIRECTKEY_PB1<<1)    \
                                    |_DIRECTKEY_PB0)
#define GPIOC_MASK                  ((_DIRECTKEY_PC7<<7)    \
                                    |(_DIRECTKEY_PC6<<6)    \
                                    |(_DIRECTKEY_PC5<<5)    \
                                    |(_DIRECTKEY_PC4<<4)    \
                                    |(_DIRECTKEY_PC3<<3)    \
                                    |(_DIRECTKEY_PC2<<2)    \
                                    |(_DIRECTKEY_PC1<<1)    \
                                    |_DIRECTKEY_PC0)
#define GPIOD_MASK                  ((_DIRECTKEY_PD1<<1)    \
                                    |_DIRECTKEY_PD0)

#define GPIOA_MFP_MASK              (((_DIRECTKEY_PA15*3)<<30)  \
                                    |((_DIRECTKEY_PA14*3)<<28)  \
                                    |((_DIRECTKEY_PA13*3)<<26)  \
                                    |((_DIRECTKEY_PA12*3)<<24)  \
                                    |((_DIRECTKEY_PA7*3)<<14)   \
                                    |((_DIRECTKEY_PA6*3)<<12)   \
                                    |((_DIRECTKEY_PA5*3)<<10)   \
                                    |((_DIRECTKEY_PA4*3)<<8)    \
                                    |((_DIRECTKEY_PA3*3)<<6)    \
                                    |((_DIRECTKEY_PA2*3)<<4)    \
                                    |((_DIRECTKEY_PA1*3)<<2)    \
                                    |((_DIRECTKEY_PA0*3)<<0))
#define GPIOB_MFP_MASK              (((_DIRECTKEY_PB5*3)<<10)   \
                                    |((_DIRECTKEY_PB4*3)<<8)    \
                                    |((_DIRECTKEY_PB3*3)<<6)    \
                                    |((_DIRECTKEY_PB2*3)<<4)    \
                                    |((_DIRECTKEY_PB1*3)<<2)    \
                                    |((_DIRECTKEY_PB0*3)<<0))
#define GPIOC_MFP_MASK              (((_DIRECTKEY_PC3*3)<<6)    \
                                    |((_DIRECTKEY_PC2*3)<<4)    \
                                    |((_DIRECTKEY_PC1*3)<<2)    \
                                    |((_DIRECTKEY_PC0*3)<<0))
#define GPIOD_MFP_MASK              (((_DIRECTKEY_PD1*3)<<2)    \
                                    |((_DIRECTKEY_PD0*3)<<0))
									 
#ifdef	_DIRECTKEY_SHORT_DB_PA0
#define	DIRECTKEY_SHORT_DB_PA0		(1<<0)
#else
#define	DIRECTKEY_SHORT_DB_PA0		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PA1
#define	DIRECTKEY_SHORT_DB_PA1		(1<<1)
#else
#define	DIRECTKEY_SHORT_DB_PA1		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PA2
#define	DIRECTKEY_SHORT_DB_PA2		(1<<2)
#else
#define	DIRECTKEY_SHORT_DB_PA2		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PA3
#define	DIRECTKEY_SHORT_DB_PA3		(1<<3)
#else
#define	DIRECTKEY_SHORT_DB_PA3		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PA4
#define	DIRECTKEY_SHORT_DB_PA4		(1<<4)
#else
#define	DIRECTKEY_SHORT_DB_PA4		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PA5
#define	DIRECTKEY_SHORT_DB_PA5		(1<<5)
#else
#define	DIRECTKEY_SHORT_DB_PA5		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PA6
#define	DIRECTKEY_SHORT_DB_PA6		(1<<6)
#else
#define	DIRECTKEY_SHORT_DB_PA6		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PA7
#define	DIRECTKEY_SHORT_DB_PA7		(1<<7)
#else
#define	DIRECTKEY_SHORT_DB_PA7		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PA8
#define	DIRECTKEY_SHORT_DB_PA8		(1<<8)
#else
#define	DIRECTKEY_SHORT_DB_PA8		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PA9
#define	DIRECTKEY_SHORT_DB_PA9		(1<<9)
#else
#define	DIRECTKEY_SHORT_DB_PA9		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PA10
#define	DIRECTKEY_SHORT_DB_PA10		(1<<10)
#else
#define	DIRECTKEY_SHORT_DB_PA10		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PA11
#define	DIRECTKEY_SHORT_DB_PA11		(1<<11)
#else
#define	DIRECTKEY_SHORT_DB_PA11		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PA12
#define	DIRECTKEY_SHORT_DB_PA12		(1<<12)
#else
#define	DIRECTKEY_SHORT_DB_PA12		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PA13
#define	DIRECTKEY_SHORT_DB_PA13		(1<<13)
#else
#define	DIRECTKEY_SHORT_DB_PA13		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PA14
#define	DIRECTKEY_SHORT_DB_PA14		(1<<14)
#else
#define	DIRECTKEY_SHORT_DB_PA14		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PA15
#define	DIRECTKEY_SHORT_DB_PA15		(1<<15)
#else
#define	DIRECTKEY_SHORT_DB_PA15		(0)
#endif
#define GPIOA_SHORT_DB_MASK			(	((DIRECTKEY_SHORT_DB_PA0)	\
										|(DIRECTKEY_SHORT_DB_PA1)	\
										|(DIRECTKEY_SHORT_DB_PA2)	\
										|(DIRECTKEY_SHORT_DB_PA3)	\
										|(DIRECTKEY_SHORT_DB_PA4)	\
										|(DIRECTKEY_SHORT_DB_PA5)	\
										|(DIRECTKEY_SHORT_DB_PA6)	\
										|(DIRECTKEY_SHORT_DB_PA7)	\
										|(DIRECTKEY_SHORT_DB_PA8)	\
										|(DIRECTKEY_SHORT_DB_PA9)	\
										|(DIRECTKEY_SHORT_DB_PA10)	\
										|(DIRECTKEY_SHORT_DB_PA11)	\
										|(DIRECTKEY_SHORT_DB_PA12)	\
										|(DIRECTKEY_SHORT_DB_PA13)	\
										|(DIRECTKEY_SHORT_DB_PA14)	\
										|(DIRECTKEY_SHORT_DB_PA15))	\
									&GPIOA_MASK)
#ifdef	_DIRECTKEY_SHORT_DB_PB0
#define	DIRECTKEY_SHORT_DB_PB0		(1<<0)
#else
#define	DIRECTKEY_SHORT_DB_PB0		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PB1
#define	DIRECTKEY_SHORT_DB_PB1		(1<<1)
#else
#define	DIRECTKEY_SHORT_DB_PB1		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PB2
#define	DIRECTKEY_SHORT_DB_PB2		(1<<2)
#else
#define	DIRECTKEY_SHORT_DB_PB2		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PB3
#define	DIRECTKEY_SHORT_DB_PB3		(1<<3)
#else
#define	DIRECTKEY_SHORT_DB_PB3		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PB4
#define	DIRECTKEY_SHORT_DB_PB4		(1<<4)
#else
#define	DIRECTKEY_SHORT_DB_PB4		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PB5
#define	DIRECTKEY_SHORT_DB_PB5		(1<<5)
#else
#define	DIRECTKEY_SHORT_DB_PB5		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PB6
#define	DIRECTKEY_SHORT_DB_PB6		(1<<6)
#else
#define	DIRECTKEY_SHORT_DB_PB6		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PB7
#define	DIRECTKEY_SHORT_DB_PB7		(1<<7)
#else
#define	DIRECTKEY_SHORT_DB_PB7		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PB8
#define	DIRECTKEY_SHORT_DB_PB8		(1<<8)
#else
#define	DIRECTKEY_SHORT_DB_PB8		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PB9
#define	DIRECTKEY_SHORT_DB_PB9		(1<<9)
#else
#define	DIRECTKEY_SHORT_DB_PB9		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PB10
#define	DIRECTKEY_SHORT_DB_PB10		(1<<10)
#else
#define	DIRECTKEY_SHORT_DB_PB10		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PB11
#define	DIRECTKEY_SHORT_DB_PB11		(1<<11)
#else
#define	DIRECTKEY_SHORT_DB_PB11		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PB12
#define	DIRECTKEY_SHORT_DB_PB12		(1<<12)
#else
#define	DIRECTKEY_SHORT_DB_PB12		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PB13
#define	DIRECTKEY_SHORT_DB_PB13		(1<<13)
#else
#define	DIRECTKEY_SHORT_DB_PB13		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PB14
#define	DIRECTKEY_SHORT_DB_PB14		(1<<14)
#else
#define	DIRECTKEY_SHORT_DB_PB14		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PB15
#define	DIRECTKEY_SHORT_DB_PB15		(1<<15)
#else
#define	DIRECTKEY_SHORT_DB_PB15		(0)
#endif
#define GPIOB_SHORT_DB_MASK			(	((DIRECTKEY_SHORT_DB_PB0)	\
										|(DIRECTKEY_SHORT_DB_PB1)	\
										|(DIRECTKEY_SHORT_DB_PB2)	\
										|(DIRECTKEY_SHORT_DB_PB3)	\
										|(DIRECTKEY_SHORT_DB_PB4)	\
										|(DIRECTKEY_SHORT_DB_PB5)	\
										|(DIRECTKEY_SHORT_DB_PB6)	\
										|(DIRECTKEY_SHORT_DB_PB7)	\
										|(DIRECTKEY_SHORT_DB_PB8)	\
										|(DIRECTKEY_SHORT_DB_PB9)	\
										|(DIRECTKEY_SHORT_DB_PB10)	\
										|(DIRECTKEY_SHORT_DB_PB11)	\
										|(DIRECTKEY_SHORT_DB_PB12)	\
										|(DIRECTKEY_SHORT_DB_PB13)	\
										|(DIRECTKEY_SHORT_DB_PB14)	\
										|(DIRECTKEY_SHORT_DB_PB15))	\
									&GPIOB_MASK)
#ifdef	_DIRECTKEY_SHORT_DB_PC0
#define	DIRECTKEY_SHORT_DB_PC0		(1<<0)
#else
#define	DIRECTKEY_SHORT_DB_PC0		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PC1
#define	DIRECTKEY_SHORT_DB_PC1		(1<<1)
#else
#define	DIRECTKEY_SHORT_DB_PC1		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PC2
#define	DIRECTKEY_SHORT_DB_PC2		(1<<2)
#else
#define	DIRECTKEY_SHORT_DB_PC2		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PC3
#define	DIRECTKEY_SHORT_DB_PC3		(1<<3)
#else
#define	DIRECTKEY_SHORT_DB_PC3		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PC4
#define	DIRECTKEY_SHORT_DB_PC4		(1<<4)
#else
#define	DIRECTKEY_SHORT_DB_PC4		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PC5
#define	DIRECTKEY_SHORT_DB_PC5		(1<<5)
#else
#define	DIRECTKEY_SHORT_DB_PC5		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PC6
#define	DIRECTKEY_SHORT_DB_PC6		(1<<6)
#else
#define	DIRECTKEY_SHORT_DB_PC6		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PC7
#define	DIRECTKEY_SHORT_DB_PC7		(1<<7)
#else
#define	DIRECTKEY_SHORT_DB_PC7		(0)
#endif
#define GPIOC_SHORT_DB_MASK			(	((DIRECTKEY_SHORT_DB_PC0)	\
										|(DIRECTKEY_SHORT_DB_PC1)	\
										|(DIRECTKEY_SHORT_DB_PC2)	\
										|(DIRECTKEY_SHORT_DB_PC3)	\
										|(DIRECTKEY_SHORT_DB_PC4)	\
										|(DIRECTKEY_SHORT_DB_PC5)	\
										|(DIRECTKEY_SHORT_DB_PC6)	\
										|(DIRECTKEY_SHORT_DB_PC7))	\
									&GPIOC_MASK)
#ifdef	_DIRECTKEY_SHORT_DB_PD0
#define	DIRECTKEY_SHORT_DB_PD0		(1<<0)
#else
#define	DIRECTKEY_SHORT_DB_PD0		(0)
#endif
#ifdef	_DIRECTKEY_SHORT_DB_PD1
#define	DIRECTKEY_SHORT_DB_PD1		(1<<1)
#else
#define	DIRECTKEY_SHORT_DB_PD1		(0)
#endif
#define GPIOD_SHORT_DB_MASK			(	((DIRECTKEY_SHORT_DB_PD0)	\
										|(DIRECTKEY_SHORT_DB_PD1))	\
									&GPIOD_MASK)
#ifndef _GPIOA_BUSY_DEFINE
#define GPIOA_BUSY_DEFINE	(0x0000)
#else
#define GPIOA_BUSY_DEFINE	(_GPIOA_BUSY_DEFINE)
#endif
#ifndef _GPIOB_BUSY_DEFINE
#define GPIOB_BUSY_DEFINE	(0x0000)
#else
#define GPIOB_BUSY_DEFINE	(_GPIOB_BUSY_DEFINE)
#endif
#ifndef _GPIOC_BUSY_DEFINE
#define GPIOC_BUSY_DEFINE	(0x0000)
#else
#define GPIOC_BUSY_DEFINE	(_GPIOC_BUSY_DEFINE)
#endif
#ifndef _GPIOD_BUSY_DEFINE
#define GPIOD_BUSY_DEFINE	(0x0000)
#else
#define GPIOD_BUSY_DEFINE	(_GPIOD_BUSY_DEFINE)
#endif

#define DIRECTKEY_PORT_NUM  ((GPIOA_MASK>0)+(GPIOB_MASK>0)+(GPIOC_MASK>0)+(GPIOD_MASK>0))

#define DEBOUNCE_TIME               _DIRECTKEY_PRESS_DB
#define KEY_SCAN_TICK               _DIRECTKEY_SCAN_TICK
#if (_DIRECTKEY_SCAN_TICK<=0)
#error	"_DIRECTKEY_SCAN_TICK must be ≧ 1"
#endif
#if (_DIRECTKEY_PRESS_DB<0)
#error	"_DIRECTKEY_PRESS_DB must be ≧ 0"
#endif
#define DEBOUNCE_CNT				((DEBOUNCE_TIME+KEY_SCAN_TICK-1)/KEY_SCAN_TICK)	//Round Up

typedef enum {
#if (GPIOA_MASK>0)
    kDKeyInPortA,
#endif
#if (GPIOB_MASK>0)
    kDKeyInPortB,
#endif
#if (GPIOC_MASK>0)
    kDKeyInPortC,
#endif
#if (GPIOD_MASK>0)
    kDKeyInPortD,
#endif
    kDKeyInPortNum,         //=(DIRECTKEY_PORT_NUM)
}M_DKEY_OUT_PORT;

typedef PARA_TYPE struct {
    GPIO_TypeDef   *port;//GPIOA~GPIOD
    U16             mask;
	U16             busyDefine;
#if GPIOA_SHORT_DB_MASK||GPIOB_SHORT_DB_MASK||GPIOC_SHORT_DB_MASK||GPIOD_SHORT_DB_MASK
    U16             shortDebounceMask;//0 = debounce end, other = in debouncing
#endif
    U8              portLable;
}K_DKeyInPara;

typedef struct {
    U16     debounceCount;//0 = debounce end, other = in debouncing
	U16     checked;
    U16     pressEvent;
    U16     releaseEvent;
#if DEBOUNCE_CNT>0
    U16     prev;
#endif
}S_DKeyPortCtrl;

#ifndef	DKEY_QUEUE_LEN
#define DKEY_QUEUE_LEN              16//max 255
#endif

typedef struct {
                S_DKeyPortCtrl      sIn[DIRECTKEY_PORT_NUM];
    volatile    U8                  getKeySemaphore;
    volatile    U8                  KeyScanCmd;
#if KEY_SCAN_TICK>1
                U8                  KeyScanCount;
#endif
    volatile    U8                  qBuf[DKEY_QUEUE_LEN];
    volatile    U8                  qBufCnt;
    volatile    U8                  qWIndex;
    volatile    U8                  qRIndex;
}S_DKeyCtrl;
#endif