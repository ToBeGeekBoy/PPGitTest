//////////////////////////////////////////////////////////////////////
#ifndef _NX1_DEBUG
#define _NX1_DEBUG

#include "nx1_reg.h"

//////////////////////////////////////////////////////////////////////
// Constant Define
//////////////////////////////////////////////////////////////////////
            
#define _DEBUG_PRINTF               1              // 0:DISABLE,1:ENABLE
#define _DEBUG_RX                   0               // 0:DISABLE,1:ENABLE

#define UART_TX_PIN                 6
#define UART_RX_PIN                 7

#if _DEBUG_PRINTF==1 && _UART_MODULE && _DEBUG_MODE
#define dprintf  DebugPrintf
#define dprintfS DebugPrintf
#else
#define dprintf(...)
#define dprintfS(...)
#endif


//////////////////////////////////////////////////////////////////////
// External Function
//////////////////////////////////////////////////////////////////////
extern void DebugPrintf(const char *format, ...);
extern void DebugPrintfS(char *str);
extern void UartCtlInit(U16 baud_rate);





#endif
