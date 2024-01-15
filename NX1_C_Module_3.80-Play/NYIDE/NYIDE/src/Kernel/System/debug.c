#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "include.h"
#include "nx1_uart.h"
#include "nx1_intc.h"
#include "debug.h"
#include "nx1_wdt.h"
#include "nx1_gpio.h"

#if _UART_MODULE && _DEBUG_MODE

#if _DEBUG_PRINTF==1
static U8 DebugBuf[128];

static U8 TxBlock;

static volatile U8 StartSending;

#define UART_TX_BUF_SIZE    512

static U8  UartTxBuf[UART_TX_BUF_SIZE];
static volatile struct{
    U8  *buf;           //data buffer
    U16  rd_idx;        //current buffer read index
    U16  wr_idx;        //current buffer write index
}UartTxHd;

static int print(char **out, const char *format, va_list args );

#undef putchar

inline int putchar(int c)
{

    U16 temp;
	//c = toupper(c);

	/* tx fifo full */
    
    if(TxBlock){	
        //if(StartSending){
            //UART_WaitTXDone(P_UART);  
	#if _EF_SERIES
        while (StartSending&&(!(P_UART->Flag & C_UART_Tx_Done_Flag)) );
	#else
        while (StartSending&&(!(P_UART->Flag & C_UART_Tx_FIFO_Empty_Flag)) );
	#endif
        //}
        UART_ClrTXDoneSts(P_UART);
        StartSending=1;
        UART_SendData(P_UART,c); 
        return c;
    }

    temp=UartTxHd.wr_idx;
    temp++;
    if(temp==UART_TX_BUF_SIZE){
        temp=0;
    }
   // while(UartTxHd.rd_idx==temp);    
    if(temp!=UartTxHd.rd_idx){
        UartTxHd.buf[UartTxHd.wr_idx]= c;
        UartTxHd.wr_idx=temp;    
    }

    return c;
}

void nds_write(const unsigned char *buf, int size)
{
	int    i,data;
	//while(1);
	for (i = 0; i < size; i++)
		putchar(buf[i]);

	GIE_DISABLE();  	
    if(!StartSending){
	    if(UartTxHd.rd_idx!=UartTxHd.wr_idx){
            data=UartTxHd.buf[UartTxHd.rd_idx];
            StartSending=1;
            UART_SendData(P_UART,data);
            UartTxHd.rd_idx++;
            if(UartTxHd.rd_idx==UART_TX_BUF_SIZE){
                UartTxHd.rd_idx=0;
            }
        }                                
    }
    GIE_ENABLE(); 
 
}

void DebugPrintf(const char *format, ...)
{
    U32 temp;
 
    va_list args;
        
    va_start( args, format );
//    
//    vsprintf((char *)DebugBuf, format, args );
//    
    temp= (U32)DebugBuf;
    print((char **) &temp, format, args );
    
    va_end(args);
    
    //puts((char *)DebugBuf);
    
    nds_write(DebugBuf,strlen((char *)DebugBuf));
}
void DebugPrintfS(char *str)
{
    TxBlock=1;
    DebugPrintf("%s",str);
}

static void printchar(char **str, int c)
{
	//extern int putchar(int c);
	
	if (str) {
		**str = c;
		++(*str);
	}
	//else (void)putchar(c);
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

static int prints(char **out, const char *string, int width, int pad)
{
	register int pc = 0, padchar = ' ';

	if (width > 0) {
		register int len = 0;
		register const char *ptr;
		for (ptr = string; *ptr; ++ptr) ++len;
		if (len >= width) width = 0;
		else width -= len;
		if (pad & PAD_ZERO) padchar = '0';
	}
	if (!(pad & PAD_RIGHT)) {
		for ( ; width > 0; --width) {
			printchar (out, padchar);
			++pc;
		}
	}
	for ( ; *string ; ++string) {
		printchar (out, *string);
		++pc;
	}
	for ( ; width > 0; --width) {
		printchar (out, padchar);
		++pc;
	}

	return pc;
}

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12

static int printi(char **out, int i, int b, int sg, int width, int pad, int letbase)
{
	char print_buf[PRINT_BUF_LEN];
	register char *s;
	register int t, neg = 0, pc = 0;
	register unsigned int u = i;

	if (i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return prints (out, print_buf, width, pad);
	}

	if (sg && b == 10 && i < 0) {
		neg = 1;
		u = -i;
	}

	s = print_buf + PRINT_BUF_LEN-1;
	*s = '\0';

	while (u) {
		t = u % b;
		if( t >= 10 )
			t += letbase - '0' - 10;
		*--s = t + '0';
		u /= b;
	}

	if (neg) {
		if( width && (pad & PAD_ZERO) ) {
			printchar (out, '-');
			++pc;
			--width;
		}
		else {
			*--s = '-';
		}
	}

	return pc + prints (out, s, width, pad);
}

static int print(char **out, const char *format, va_list args )
{
	register int width, pad;
	register int pc = 0;
	char scr[2];

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			width = pad = 0;
			if (*format == '\0') break;
			if (*format == '%') goto out;
			if (*format == '-') {
				++format;
				pad = PAD_RIGHT;
			}
			while (*format == '0') {
				++format;
				pad |= PAD_ZERO;
			}
			for ( ; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}
			if( *format == 's' ) {
				register char *s = (char *)va_arg( args, int );
				pc += prints (out, s?s:"(null)", width, pad);
				continue;
			}
			if( *format == 'd' ) {
				pc += printi (out, va_arg( args, int ), 10, 1, width, pad, 'a');
				continue;
			}
			if( *format == 'x' ) {
				pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'X' ) {
				pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'A');
				continue;
			}
			if( *format == 'u' ) {
				pc += printi (out, va_arg( args, int ), 10, 0, width, pad, 'a');
				continue;
			}
			if( *format == 'c' ) {
				/* char are converted to int then pushed on the stack */
				scr[0] = (char)va_arg( args, int );
				scr[1] = '\0';
				pc += prints (out, scr, width, pad);
				continue;
			}
		}
		else {
		out:
			printchar (out, *format);
			++pc;
		}
	}
	if (out) **out = '\0';
	va_end( args );
	return pc;
}
#elif _DEBUG_PRINTF==2

static volatile U8 StartSending;
void DebugPrintfS(char *str)
{
    U8 i,len;
    
    len=strlen((char *)str);
    for(i=0;i<len;i++){
        UART_WaitTXDone(P_UART);
        UART_ClrTXDoneSts(P_UART);
        UART_SendData(P_UART,str[i]); 
    }    
}
#endif


#if _DEBUG_RX   
#define UART_RX_BUF_SIZE    4
static U8  UartRxBuf[UART_RX_BUF_SIZE];
static volatile struct{
    U8  *buf;           //data buffer
    U8  rd_idx;         //current buffer read index
    U8  wr_idx;         //current buffer write index
}UartRxHd;
#endif

//------------------------------------------------------------------//
// Uart Debug Init
// Parameter: 
//          baud_rate:baud rate
// return value:
//          NONE
//------------------------------------------------------------------//    
void UartCtlInit(U16 baud_rate){


#if _DEBUG_PRINTF==1   
    StartSending=0;
    
#ifdef DEBUG_BLOCKING
    TxBlock=1;
#else
    TxBlock=1;
#endif

#if _DEBUG_RX
    UartRxHd.wr_idx=0;  
    UartRxHd.rd_idx=0;   
    UartRxHd.buf=UartRxBuf; 
#endif    
    UartTxHd.wr_idx=0;  
    UartTxHd.rd_idx=0;   
    UartTxHd.buf=UartTxBuf; 
        
    UART_Init(P_UART,baud_rate);
    UART_IntCmd(P_UART,UART_TX_DONE,ENABLE);
#if _DEBUG_RX
    UART_IntCmd(P_UART,UART_RXFIFO_NOEMPTY,ENABLE);
#endif
    INTC_IrqCmd(IRQ_BIT_UART,ENABLE);

    
#elif   _DEBUG_PRINTF==2 

#if _DEBUG_RX
    UartRxHd.wr_idx=0;  
    UartRxHd.rd_idx=0;   
    UartRxHd.buf=UartRxBuf;
#endif    
    
    UART_Init(P_UART,baud_rate);
#if _DEBUG_RX
    UART_IntCmd(P_UART,UART_RXFIFO_NOEMPTY,ENABLE);
    INTC_IrqCmd(IRQ_BIT_P_UART,ENABLE);
#endif

#endif
}    
//------------------------------------------------------------------//
// Uart ISR
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//    
void UART_ISR(void){
#if _DEBUG_PRINTF==1    
    U8 data,sts;

    sts = UART_GetFIFOSts(P_UART);

    if(sts & UART_TX_DONE){
        UART_ClrTXDoneSts(P_UART);
        StartSending=0;
        if(UartTxHd.rd_idx!=UartTxHd.wr_idx){
            data=UartTxHd.buf[UartTxHd.rd_idx];
            StartSending=1;
            UART_SendData(P_UART,data);
            UartTxHd.rd_idx++;
            if(UartTxHd.rd_idx==UART_TX_BUF_SIZE){
                UartTxHd.rd_idx=0;
            }
        }            
    }   
#elif  _DEBUG_PRINTF==2 
    U8 data,sts;

    sts = UART_GetFIFOSts(P_UART);

    if(sts & UART_TX_DONE){
        UART_ClrTXDoneSts(P_UART);
        StartSending=0;
    }        
#endif   
    
#if _DEBUG_RX&&_DEBUG_PRINTF    
    if(sts & UART_RXFIFO_NOEMPTY){
       
    	data= UART_ReceiveData(P_UART);
    	// dprintf("%x\r\n",data);   
        UartRxHd.buf[UartRxHd.wr_idx]=data;
        UartRxHd.wr_idx++;
        if(UartRxHd.wr_idx==UART_RX_BUF_SIZE){
            UartRxHd.wr_idx=0;
        }    
    }  
#endif     
}    

//------------------------------------------------------------------//
// Get KeyCode
// Parameter: 
//          NONE
// return value:
//          0: no key 
//          others: keycode
//------------------------------------------------------------------//    
U8 DebugGetKey(void){  
#if _DEBUG_RX&&_DEBUG_PRINTF
    U8 data;
    if(UartRxHd.wr_idx==UartRxHd.rd_idx){
        return 0;
    }else{
        
        data=UartRxHd.buf[UartRxHd.rd_idx++];
       
        if(UartRxHd.rd_idx==UART_RX_BUF_SIZE){
            UartRxHd.rd_idx=0;
        }        
        return data;
    }
#else
    return 0;
#endif      
}
#endif
