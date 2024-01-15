#include "nx1_gpio.h"
#include "include.h"

//////////////////////////////////////////////////////////////////////
//  Defines
//////////////////////////////////////////////////////////////////////
#define GPIO_PORT_OFFSET            16

GPIO_TypeDef*   const   GPIO_PortList[] = 	{
												GPIOA,
												GPIOB,
												GPIOC,
											#if _EF_SERIES
												GPIOD,
											#endif									 
											};

//------------------------------------------------------------------//
// GPIO pin init
// Parameter: 
//          gpiox : gpio group, GPIOA,GPIOB,GPIOC
//          pin   : pin number(0~15)
//          mode  : 
//                  GPIO_MODE_IN_PU
//                  GPIO_MODE_IN_FLOAT
//					GPIO_MODE_OUT_HI
//					GPIO_MODE_OUT_LOW
//                  GPIO_MODE_ALT
//                  GPIO_MODE_ALT2  (Switch GPIOA4~7,14 to ADC CH4~7,TRIG_EXT1)
// return value:
//          NONE
//------------------------------------------------------------------//    
void GPIO_Init(GPIO_TypeDef *gpiox,U8 pin,U8 mode)
{
    GIE_DISABLE();
    if(mode==GPIO_MODE_IN_PU)
    {
	#if _EF_SERIES
		gpiox->MFP &= (~(3<<(pin*2)));	
	#else
        gpiox->MFP &= (~(1<<pin));
	#endif
        gpiox->DIR  |= (1<<pin);
        gpiox->CFG &= (~(1<<pin));
    }
    else if(mode==GPIO_MODE_IN_FLOAT)
    {
	#if _EF_SERIES
		gpiox->MFP &= (~(3<<(pin*2)));	
	#else
        gpiox->MFP &= (~(1<<pin));
	#endif
        gpiox->DIR  |= (1<<pin);
        gpiox->CFG |= (1<<pin);
    }
    else if((mode==GPIO_MODE_OUT_HI) || (mode==GPIO_MODE_OUT_LOW))
    {
		if(mode==GPIO_MODE_OUT_HI)
		{
			gpiox->Data |= (1<<pin);
		}
		else
		{
			gpiox->Data &= (~(1<<pin));
		}
	#if _EF_SERIES
		gpiox->MFP &= (~(3<<(pin*2)));	
	#else
        gpiox->MFP &= (~(1<<pin));
	#endif
        gpiox->DIR  &= (~(1<<pin));
    }
#if _EF_SERIES	
	else if((mode==GPIO_MODE_ALT) || (mode==GPIO_MODE_ALT2) || (mode==GPIO_MODE_ALT3))
    {
		gpiox->MFP &= (~(3<<(pin*2)));
		gpiox->MFP |= ((mode-GPIO_MODE_ALT_OFFSET)<<(pin*2));
	}
#else
    else if(mode==GPIO_MODE_ALT)
    {
        gpiox->MFP |= (1<<pin);
    }
    else if(mode==GPIO_MODE_ALT2 && gpiox==P_PORTA)
    {
        gpiox->MFP &= (~(1<<pin));
        pin+=GPIO_ALT2_OFFSET;
        gpiox->MFP |= (1<<pin);
    }
#endif
    GIE_ENABLE();
}
//------------------------------------------------------------------//
// Read specific gpio pin
// Parameter: 
//          gpiox : gpio group, GPIOA,GPIOB,GPIOC
//          pin   : pin number(0~15)
// return value:
//          pin value
//------------------------------------------------------------------//    
U8 GPIO_Read(GPIO_TypeDef *gpiox,U8 pin)
{
    return ((gpiox->PAD>>pin)&0x1);
}      
//------------------------------------------------------------------//
// Write specific gpio pin
// Parameter: 
//          gpiox : gpio group, GPIOA,GPIOB,GPIOC
//          pin   : pin number(0~15)
//          value : BIT_SET,BIT_UNSET
// return value:
//          NONE
//------------------------------------------------------------------//    
void GPIO_Write(GPIO_TypeDef *gpiox,U8 pin,U8 value) 
{
    GIE_DISABLE();
 	if(value&0x1)
    {
        gpiox->Data |= (1<<pin);
    }
    else
    {
        gpiox->Data &= (~(1<<pin));
    }
    GIE_ENABLE();
}        
//------------------------------------------------------------------//
// Toggle specific gpio pin
// Parameter: 
//          gpiox : gpio group, GPIOA,GPIOB,GPIOC
//          pin   : pin number(0~15)
// return value:
//          NONE
//------------------------------------------------------------------//    
void GPIO_Toggle(GPIO_TypeDef *gpiox,U8 pin) 
{
    GIE_DISABLE();
    gpiox->Data ^= (1<<pin);
    GIE_ENABLE();
}        
//------------------------------------------------------------------//
// Write gpio port
// Parameter: 
//          gpiox:gpio group, GPIOA,GPIOB,GPIOC
//          value:value to be written to the output data register
// return value:
//          NONE
//------------------------------------------------------------------//    
void GPIO_WritePort(GPIO_TypeDef *gpiox,U16 value) 
{
    GIE_DISABLE();    
    gpiox->Data=value;
    GIE_ENABLE();    
}   
//------------------------------------------------------------------//
// Read gpio port
// Parameter: 
//          gpiox:gpio group, GPIOA,GPIOB,GPIOC
// return value:
//          port value
//------------------------------------------------------------------//    
U16 GPIO_ReadPort(GPIO_TypeDef *gpiox) 
{
    return (gpiox->PAD);
}   
//------------------------------------------------------------------//
// Read port mode
// Parameter: 
//          gpiox:gpio group, GPIOA,GPIOB,GPIOC
// return value:
//          mode:
//                1 : Input mode for each pin
//                0 : Output mode for each pin
//------------------------------------------------------------------//    
U16 GPIO_ReadPortMode(GPIO_TypeDef *gpiox)
{
    return (gpiox->DIR); 
}
//------------------------------------------------------------------//
// Write port mode
// Parameter: 
//          gpiox:gpio group, GPIOA,GPIOB,GPIOC
//          mode:
//                1 : Input mode for each pin
//                0 : Output mode for each pin
// return value:
//          NONE
//------------------------------------------------------------------//    
void GPIO_WritePortMode(GPIO_TypeDef *gpiox,U16 mode)
{
    GIE_DISABLE();
    gpiox->DIR = mode;
    switch((const U32)gpiox) {
#if defined(_GPIOA_PULLHIGH_RESISTOR)
        case (const U32)GPIOA:
            gpiox->CFG &= (~_GPIOA_PULLHIGH_RESISTOR);
            break;
#endif
#if defined(_GPIOB_PULLHIGH_RESISTOR)
        case (const U32)GPIOB:
            gpiox->CFG &= (~_GPIOB_PULLHIGH_RESISTOR);
            break;
#endif
#if defined(_GPIOC_PULLHIGH_RESISTOR)
        case (const U32)GPIOC:
            gpiox->CFG &= (~_GPIOC_PULLHIGH_RESISTOR);
            break;
#endif
#if _EF_SERIES && defined(_GPIOD_PULLHIGH_RESISTOR)
        case (const U32)GPIOD:
            gpiox->CFG &= (~_GPIOD_PULLHIGH_RESISTOR);
            break;
#endif
        default:
            break;
    }
    GIE_ENABLE();
}
//------------------------------------------------------------------//
// Read mode
// Parameter: 
//          gpiox : gpio group, GPIOA,GPIOB,GPIOC
//          pin   : pin number(0~15)
// return value:
//          mode
//------------------------------------------------------------------//    
U8 GPIO_ReadMode(GPIO_TypeDef *gpiox,U8 pin)
{
    return ((gpiox->DIR>>pin)&0x1);
}
//------------------------------------------------------------------//
// Write pin mode
// Parameter: 
//          gpiox : gpio group, GPIOA,GPIOB,GPIOC
//          pin   : pin number(0~15)
//          mode  :
//                  1:input, 0:output
// return value:
//          NONE
//------------------------------------------------------------------//    
void GPIO_WriteMode(GPIO_TypeDef *gpiox,U8 pin,U8 mode)
{
    GIE_DISABLE();
    if(mode)
    {
        gpiox->DIR |= (1<<pin);
        switch((const U32)gpiox) {
#if defined(_GPIOA_PULLHIGH_RESISTOR)
            case (const U32)GPIOA:
                gpiox->CFG &= (~(_GPIOA_PULLHIGH_RESISTOR & (1<<pin)));
                break;
#endif
#if defined(_GPIOB_PULLHIGH_RESISTOR)
            case (const U32)GPIOB:
                gpiox->CFG &= (~(_GPIOB_PULLHIGH_RESISTOR & (1<<pin)));
                break;
#endif
#if defined(_GPIOC_PULLHIGH_RESISTOR)
            case (const U32)GPIOC:
                gpiox->CFG &= (~(_GPIOC_PULLHIGH_RESISTOR & (1<<pin)));
                break;
#endif
#if _EF_SERIES && defined(_GPIOD_PULLHIGH_RESISTOR)
            case (const U32)GPIOD:
                gpiox->CFG &= (~(_GPIOD_PULLHIGH_RESISTOR & (1<<pin)));
                break;
#endif
            default:
                break;
        }
    }
    else
    {
        gpiox->DIR &= (~(1<<pin));
    }     
    GIE_ENABLE();
}
//------------------------------------------------------------------//
// Set pin current
// Parameter: 
//          gpiox   : gpio group, GPIOA,GPIOB,GPIOC
//          pin     : pin number(0~15)
//          setting :
//                     GPIO_SINK_NORMAL
//                     GPIO_SINK_LARGE
//                     GPIO_CC_NORMAL(PA8~15 only)
//                     GPIO_CC_LARGE(PA8~15 only)
// return value:
//          void
//------------------------------------------------------------------//    
void GPIO_SetCurrent(GPIO_TypeDef *gpiox,U8 pin,U8 setting)
{
    pin += GPIO_PORT_OFFSET;
#if !_EF_SERIES
    //Set PA8~PA15 for Constant Sink Current 
    if(setting&GPIO_CURRENT_CC && gpiox==P_PORTA && pin>=(GPIO_CC_PIN_NUM+GPIO_PORT_OFFSET))
    {
    	P_SMU->PWR_Ctrl |= C_CSC_Bias_En;  //SMU power control
        gpiox->CFG &= (~(1<<pin));
    }
    else
    {
        gpiox->CFG |= (1<<pin);
    }    
#endif
    
#if EF11FS_SERIES
    pin = pin/24 + GPIO_PORT_OFFSET;    // normal/large sink set by bit 16 for PA0~PA7, bit 17 for PA8~PA15 
#endif
    if(setting&GPIO_LARGE_CURRENT)
    {
        gpiox->Wakeup_Mask &= (~(1<<pin));
    }
    else
    {
        gpiox->Wakeup_Mask |= (1<<pin);
    }    
}    
//------------------------------------------------------------------//
// Enable or Disable wakeup pin
// Parameter: 
//          gpiox : gpio group, GPIOA,GPIOB,GPIOC
//          pin   : pin number(0~15)
//          cmd   : ENABLE,DISABLE
// return value:
//          void
//------------------------------------------------------------------//    
void GPIO_WakeUpCmd(GPIO_TypeDef *gpiox,U8 pin,U8 cmd)
{
    if(cmd)
    {
        gpiox->Wakeup_Mask |= (1<<pin);
    }
    else
    {
        gpiox->Wakeup_Mask &= (~(1<<pin));
    }    
}   
//------------------------------------------------------------------//
// Set pull-resistor mode
// Parameter: 
//          gpiox : GPIOA,GPIOB,GPIOC
//          part  : 0:low part, 1:hight part
//          mode  : GPIO_STRONG_PULL,GPIO_WEAK_PULL
// return value:
//          void
//------------------------------------------------------------------//    
void GPIO_SetPullMode(GPIO_TypeDef *gpiox,U8 part,U8 mode)
{
    U8 pullmode;
    if(gpiox==GPIOA)
    {
        if(part)
        {
            //PORTA High
            P_PORTA->PULL_HIGH_CFG &= (~C_PULL_HIGH_PAH);   
            pullmode = (mode==GPIO_STRONG_PULL)? C_PULL_HIGH_PAH_100K : C_PULL_HIGH_PAH_1M;
            P_PORTA->PULL_HIGH_CFG |= pullmode;
        }
        else
        {
            //PORTA Low
            P_PORTA->PULL_HIGH_CFG &= (~C_PULL_HIGH_PAL);   
            pullmode = (mode==GPIO_STRONG_PULL)? C_PULL_HIGH_PAL_100K : C_PULL_HIGH_PAL_1M;
            P_PORTA->PULL_HIGH_CFG |= pullmode;
        }    
    }
    else if(gpiox==GPIOB)
    {
        if(part)
        {
            //PORTB High
            P_PORTA->PULL_HIGH_CFG &= (~C_PULL_HIGH_PBH);   
            pullmode = (mode==GPIO_STRONG_PULL)? C_PULL_HIGH_PBH_100K : C_PULL_HIGH_PBH_1M;
            P_PORTA->PULL_HIGH_CFG |= pullmode;            
        }
        else
        {
            //PORTB Low
            P_PORTA->PULL_HIGH_CFG &= (~C_PULL_HIGH_PBL);   
            pullmode = (mode==GPIO_STRONG_PULL)? C_PULL_HIGH_PBL_100K : C_PULL_HIGH_PBL_1M;
            P_PORTA->PULL_HIGH_CFG |= pullmode;  
        }    
    }
    else if(gpiox==GPIOC)
    {
        //PORTC
        P_PORTA->PULL_HIGH_CFG &= (~C_PULL_HIGH_PCL);   
        pullmode = (mode==GPIO_STRONG_PULL)? C_PULL_HIGH_PCL_100K : C_PULL_HIGH_PCL_1M;
        P_PORTA->PULL_HIGH_CFG |= pullmode;
    }    
#if _EF_SERIES
    else if(gpiox==GPIOD)
    {
        //PORTD
        P_PORTD->PULL_HIGH_CFG &= (~C_PULL_HIGH_PDL);   
        pullmode = (mode==GPIO_STRONG_PULL)? C_PULL_HIGH_PDL_100K : C_PULL_HIGH_PDL_1M;
        P_PORTD->PULL_HIGH_CFG |= pullmode;
    }  
#endif
}    
//------------------------------------------------------------------//
// Init external trigger
// Parameter: 
//			trig_src: TRIG_EXT0
//					  TRIG_EXT1 
//          trig_mode:
//                    GPIO_EXT_RISING
//                    GPIO_EXT_FALLING
//                    GPIO_EXT_RISING_FALLING
// return value:
//          void
//------------------------------------------------------------------//    
void GPIO_ExtTrigInit(U8 trig_src, U8 trig_mode)
{
	if (trig_src) 
	{
		P_PORT_EXT->EXT_INT_Trig &= ~(3<<2);
		P_PORT_EXT->EXT_INT_Trig |= (trig_mode<<2);  
	}
	else
	{
		P_PORT_EXT->EXT_INT_Trig &= ~(3);
		P_PORT_EXT->EXT_INT_Trig |= trig_mode;  
	}
}    
//------------------------------------------------------------------//
// Enable or Disable external interrupt
// Parameter: 
//			trig_src: TRIG_EXT0
//					  TRIG_EXT1
//          cmd:ENABLE,DISABLE
// return value:
//          void
//------------------------------------------------------------------//    
void GPIO_ExtIntCmd(U8 trig_src, U8 cmd)
{
    if (trig_src)
    {
		P_PORT_EXT->EXT_INT_Flag |= (1<<1);
        P_PORT_EXT->EXT_INT_Ctrl &= ~(1<<1);
		P_PORT_EXT->EXT_INT_Ctrl |= (cmd<<1);
    }
    else
    {
		P_PORT_EXT->EXT_INT_Flag |= (1);
        P_PORT_EXT->EXT_INT_Ctrl &= ~(1);
		P_PORT_EXT->EXT_INT_Ctrl |= cmd;
    }      
}    

//------------------------------------------------------------------//
// Re-Enable external interrupt
// Parameter: 
//          None      
// return value:
//          None
//------------------------------------------------------------------// 
void GPIO_ExtInt_ReEn(void)
{
	U8 ExtInt_temp = P_PORT_EXT->EXT_INT_Ctrl;
	P_PORT_EXT->EXT_INT_Ctrl = DISABLE;
	P_PORT_EXT->EXT_INT_Ctrl = ExtInt_temp;
}