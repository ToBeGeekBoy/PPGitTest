#ifndef NX1_GPIO_H
#define NX1_GPIO_H

#include "nx1_reg.h"

//////////////////////////////////////////////////////////////////////
// Constant Define
//////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------//
//  Function   : GPIO_Init, GPIO_Read, GPIO_Write, GPIO_Toggle, GPIO_WritePort, 
//               GPIO_WritePort, GPIO_ReadPort,GPIO_ReadMode, GPIO_WriteMode, 
//               GPIO_SetCurrent, GPIO_SetPullMode, GPIO_WakeUpCmd
//  Parameter  : gpiox
//------------------------------------------------------------------//
#define GPIOA                       P_PORTA
#define GPIOB                       P_PORTB
#define GPIOC                       P_PORTC
#if _EF_SERIES
#define GPIOD                       P_PORTD
#endif
//------------------------------------------------------------------//
//  Function   : GPIO_Init
//  Parameter  : mode
//------------------------------------------------------------------//
// GPIO Input/Output and function setting
#define GPIO_MODE_IN_PU             GPIO_INPUT_PULLHIGH
#define GPIO_MODE_IN_FLOAT          GPIO_INPUT_FLOAT     
#define GPIO_MODE_OUT_HI			GPIO_OUTPUT_HIGH
#define GPIO_MODE_OUT_LOW			GPIO_OUTPUT_LOW
#define GPIO_MODE_ALT               GPIO_OUTPUT_ALT
#define GPIO_MODE_ALT2              GPIO_OUTPUT_ALT2
#if _EF_SERIES
#define GPIO_MODE_ALT3              GPIO_OUTPUT_ALT3
#define GPIO_MODE_ALT_OFFSET		3
#endif
#define GPIO_ALT2_OFFSET            16

//------------------------------------------------------------------//
//  Function   : GPIO_SetPullMode
//  Parameter  : mode
//------------------------------------------------------------------//
#define GPIO_STRONG_PULL            0x1
#define GPIO_WEAK_PULL              0x0    

//------------------------------------------------------------------//
//  Function   : GPIO_SetCurrent
//  Parameter  : setting
//------------------------------------------------------------------//
#define GPIO_SINK_NORMAL            0x00
#define GPIO_SINK_LARGE             0x02
#define GPIO_CC_NORMAL              0x01
#define GPIO_CC_LARGE               0x03
#define GPIO_CURRENT_CC             0x1             // Set, when GPIO_CC_NORMAL or GPIO_CC_LARGE
#define GPIO_CC_PIN_NUM             8               // PA8~PA15 with CC MODE
#define GPIO_LARGE_CURRENT          0x2             // Set, when GPIO_SINK_LARGE or GPIO_CC_LARGE

//------------------------------------------------------------------//
//  Function   : GPIO_ExtTrigInit
//  Parameter  : trig_src, trig_mode
//------------------------------------------------------------------//
#define TRIG_EXT0					0x0
#define TRIG_EXT1					0x1
#define GPIO_EXT_RISING             C_EXT_INT_Trig_Rising
#define GPIO_EXT_FALLING            C_EXT_INT_Trig_Falling
#define GPIO_EXT_RISING_FALLING     C_EXT_INT_Trig_Edge

//------------------------------------------------------------------//
//  Function   : GPIO_GetPort, GPIO_GetPin
//  Parameter  : IO_information
//------------------------------------------------------------------//
#define GPIO_PIN_MASK				0x0F
#define GPIO_PORT_MASK				0xF0
#define GPIO_PORTA					(0<<4)
#define GPIO_PORTB					(1<<4)
#define GPIO_PORTC					(2<<4)
#if _EF_SERIES
#define GPIO_PORTD					(3<<4)
#endif
#define GPIO_PA0					(GPIO_PORTA|0)
#define GPIO_PA1					(GPIO_PORTA|1)
#define GPIO_PA2					(GPIO_PORTA|2)
#define GPIO_PA3					(GPIO_PORTA|3)
#define GPIO_PA4					(GPIO_PORTA|4)
#define GPIO_PA5					(GPIO_PORTA|5)
#define GPIO_PA6					(GPIO_PORTA|6)
#define GPIO_PA7					(GPIO_PORTA|7)
#define GPIO_PA8					(GPIO_PORTA|8)
#define GPIO_PA9					(GPIO_PORTA|9)
#define GPIO_PA10					(GPIO_PORTA|10)
#define GPIO_PA11					(GPIO_PORTA|11)
#define GPIO_PA12					(GPIO_PORTA|12)
#define GPIO_PA13					(GPIO_PORTA|13)
#define GPIO_PA14					(GPIO_PORTA|14)
#define GPIO_PA15					(GPIO_PORTA|15)
#define GPIO_PB0					(GPIO_PORTB|0)
#define GPIO_PB1					(GPIO_PORTB|1)
#define GPIO_PB2					(GPIO_PORTB|2)
#define GPIO_PB3					(GPIO_PORTB|3)
#define GPIO_PB4					(GPIO_PORTB|4)
#define GPIO_PB5					(GPIO_PORTB|5)
#define GPIO_PB6					(GPIO_PORTB|6)
#define GPIO_PB7					(GPIO_PORTB|7)
#define GPIO_PB8					(GPIO_PORTB|8)
#define GPIO_PB9					(GPIO_PORTB|9)
#define GPIO_PB10					(GPIO_PORTB|10)
#define GPIO_PB11					(GPIO_PORTB|11)
#define GPIO_PB12					(GPIO_PORTB|12)
#define GPIO_PB13					(GPIO_PORTB|13)
#define GPIO_PB14					(GPIO_PORTB|14)
#define GPIO_PB15					(GPIO_PORTB|15)
#define GPIO_PC0					(GPIO_PORTC|0)
#define GPIO_PC1					(GPIO_PORTC|1)
#define GPIO_PC2					(GPIO_PORTC|2)
#define GPIO_PC3					(GPIO_PORTC|3)
#define GPIO_PC4					(GPIO_PORTC|4)
#define GPIO_PC5					(GPIO_PORTC|5)
#define GPIO_PC6					(GPIO_PORTC|6)
#define GPIO_PC7					(GPIO_PORTC|7)
#if _EF_SERIES
#define GPIO_PD0					(GPIO_PORTD|0)
#define GPIO_PD1					(GPIO_PORTD|1)
#endif
extern  GPIO_TypeDef*   const   GPIO_PortList[];

#if _EF_SERIES
#define GPIO_MAX					GPIO_PD1
#else
#define GPIO_MAX					GPIO_PC7
#endif

//////////////////////////////////////////////////////////////////////
// External Function
//////////////////////////////////////////////////////////////////////

extern void GPIO_Init(GPIO_TypeDef *gpiox,U8 pin,U8 mode);
extern U8   GPIO_Read(GPIO_TypeDef *gpiox,U8 pin);
extern void GPIO_Write(GPIO_TypeDef *gpiox,U8 pin,U8 value) ;
extern void GPIO_Toggle(GPIO_TypeDef *gpiox,U8 pin);
extern void GPIO_WritePort(GPIO_TypeDef *gpiox,U16 value);
extern U16  GPIO_ReadPort(GPIO_TypeDef *gpiox);
extern U16  GPIO_ReadPortMode(GPIO_TypeDef *gpiox);
extern void GPIO_WritePortMode(GPIO_TypeDef *gpiox,U16 mode);
extern U8   GPIO_ReadMode(GPIO_TypeDef *gpiox,U8 pin);
extern void GPIO_WriteMode(GPIO_TypeDef *gpiox,U8 pin,U8 mode);
extern void GPIO_SetCurrent(GPIO_TypeDef *gpiox,U8 pin,U8 setting);
extern void GPIO_ExtTrigInit(U8 trig_src, U8 trig_mode);
extern void GPIO_ExtIntCmd(U8 trig_src, U8 cmd);
extern void GPIO_SetPullMode(GPIO_TypeDef *gpiox,U8 part,U8 mode);
extern void GPIO_WakeUpCmd(GPIO_TypeDef *gpiox,U8 pin,U8 cmd);
extern void GPIO_ExtInt_ReEn(void);
#define     GPIO_GetPortIdx(a)      (((a)&GPIO_PORT_MASK)>>4)
#define		GPIO_GetPort(a)			((GPIO_TypeDef*)GPIO_PortList[GPIO_GetPortIdx(a)])
#define 	GPIO_GetPin(a)			((a)&GPIO_PIN_MASK)




  
#endif //#ifndef NX1_GPIO_H
