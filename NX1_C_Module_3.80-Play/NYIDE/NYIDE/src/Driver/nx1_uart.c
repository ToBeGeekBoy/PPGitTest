#include "nx1_uart.h"
#if		defined(P_UART)
#if _IC_BODY==0x11F22 //Fix IC bug
static U8	uart_txrx_state;     //0
#define	C_UART_NO_TX_RX		0
#define	C_UART_RX_FIRST		1
#define C_UART_TX_FIRST		2
#endif
//------------------------------------------------------------------//
// Uart Init
// Parameter: 
//          uartx : UART
//          baud_rate : UART_BAUDRATE_115200~UART_BAUDRATE_4800
// return value:
//          NONE
//------------------------------------------------------------------//    
void UART_Init(UART_TypeDef *uartx,U16 baud_rate)
{
#if	_IC_BODY==0x11F22
	if(baud_rate<5)
	{
		baud_rate = 5; //NX11FS22 can not set less than 5. Up to baudrate=800k.
	}
#endif
    uartx->BaudRate = baud_rate;
}
//------------------------------------------------------------------//
// Write single data to fifo
// Parameter: 
//          uartx:UART
//          data: data to send          
// return value:
//          NONE
//------------------------------------------------------------------//        
void UART_SendData(UART_TypeDef *uartx,U8 data)
{
#if _IC_BODY==0x11F22
	if(uart_txrx_state==C_UART_RX_FIRST)
	{
		volatile U32 uart_baudrate,uart_ctrl;
		
		uart_baudrate 	= uartx->BaudRate;
		uart_ctrl     	= uartx->Ctrl;
		P_SMU->FUNC_RST = C_Func_UART_RST;
	
		uartx->BaudRate = uart_baudrate;
		uartx->Ctrl     = uart_ctrl;
		uart_txrx_state = C_UART_TX_FIRST;
	}
	
#endif
    uartx->Data=data;
}    
//------------------------------------------------------------------//
// Enable or Disable Tx/Rx interrupt
// Parameter: 
//          uartx:UART
//          irq:UART_RXFIFO_NOEMPTY,UART_TX_DONE  
//          cmd:ENABLE,DISABLE         
// return value:
//          NONE
//------------------------------------------------------------------//        
void UART_IntCmd(UART_TypeDef *uartx,U8 irq,U8 cmd)
{
    if(cmd==ENABLE)
    {
        uartx->Ctrl|=irq;
    }
    else
    {
        uartx->Ctrl&=(~irq);
    }    
}    
//------------------------------------------------------------------//
// Read single data from fifo
// Parameter: 
//          uartx:UART       
// return value:
//          received data
//------------------------------------------------------------------//        
U8 UART_ReceiveData(UART_TypeDef *uartx)
{
#if _IC_BODY==0x11F22
	if(uart_txrx_state == C_UART_NO_TX_RX)
	{
		uart_txrx_state = C_UART_RX_FIRST;
	}
#endif
    return (uartx->Data&0xFF);
}    
//------------------------------------------------------------------//
// Wait TX finish
// Parameter: 
//          uartx:UART       
// return value:
//          None
//------------------------------------------------------------------//        
void UART_WaitTXDone(UART_TypeDef *uartx)
{
    do{        
#if _EF_SERIES
    }while (!(uartx->Flag & C_UART_Tx_Done_Flag));	
#else
    }while (!(uartx->Flag & C_UART_Tx_FIFO_Empty_Flag));
#endif
    //uartx->Flag = C_UART_Tx_FIFO_Empty_Flag;
}
//------------------------------------------------------------------//
// Clear TX Done Status
// Parameter: 
//          uartx : UART
//          sts   : clear flag
// return value:
//          None
//------------------------------------------------------------------//        
void UART_ClrTXDoneSts(UART_TypeDef *uartx)
{
#if _EF_SERIES
    uartx->Flag = C_UART_Tx_Done_Flag;
#else
    uartx->Flag = C_UART_Tx_FIFO_Empty_Flag;
#endif
}  
#if !EF11FS_SERIES
//------------------------------------------------------------------//
// Wait TX FIFO is not Full
// Parameter: 
//          uartx:UART       
// return value:
//          None
//------------------------------------------------------------------//        
void UART_WaitTXFIFOnotFull(UART_TypeDef *uartx)
{
    while (UART->Flag & UART_TXFIFO_FULL);
}
#endif
//------------------------------------------------------------------//
// Get Status
// Parameter: 
//          uartx:UART       
// return value:
//          FIFO status
//------------------------------------------------------------------//        
U8 UART_GetFIFOSts(UART_TypeDef *uartx)
{
    U8 sts;

    sts= uartx->Ctrl & uartx->Flag;
    return sts;
}  
#endif	//#if	defined(P_UART)