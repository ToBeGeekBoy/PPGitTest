#include "include.h"
#include "nx1_gpio.h"
#include "nx1_ir.h"
#include "nx1_smu.h"
#include "nx1_rtc.h"
#include "nx1_timer.h"
#include "nx1_pwm.h"
#include <string.h>

#if (_IR_DATA_LEN==IR_DATA_LEN_4BIT)&&(_IR_CRC_LEN==IR_CRC_LEN_8BIT)
#error "It is not available use IR_DATA_LEN_4BIT with IR_CRC_LEN_8BIT."
#endif

/* Defines -------------------------------------------------------------------*/
#if body_is_4KHZ //4k 
#define IR_TX_COUNT         2
#define IR_RX_COUNT         1
#else
#define IR_TX_COUNT         8
#define IR_RX_COUNT         4
#endif
/*ir Rx state*/
#define IR_RX_STATE_IDLE        1
#define IR_RX_STATE_HEADER_LOW  2
#define IR_RX_STATE_HEADER_HIGH 3
#define IR_RX_STATE_DATA_LOW    4
#define IR_RX_STATE_DATA_HIGH   5
#define IR_RX_STATE_ENDCODE     6

#define IR_RX_TIMEOUT           250   //ticks

#if !_IR_LOW_POWER_TYPE
#define IR_RX_HEADER0_TICK_MIN  16    //Header      
#define IR_RX_HEADER0_TICK_MAX  32
#define IR_RX_HEADER1_TICK_MIN  1
#define IR_RX_HEADER1_TICK_MAX  4
#define IR_RX_DATA1_0_TICK_MIN  1     //Data Logic 1
#define IR_RX_DATA1_0_TICK_MAX  4
#define IR_RX_DATA1_1_TICK_MIN  4
#define IR_RX_DATA1_1_TICK_MAX  8
#define IR_RX_DATA0_0_TICK_MIN  4     //Data Logic 0
#define IR_RX_DATA0_0_TICK_MAX  8
#define IR_RX_DATA0_1_TICK_MIN  1
#define IR_RX_DATA0_1_TICK_MAX  4
#define IR_RX_ENDCODE_TICK_MIN  8     //End Code
#define IR_RX_ENDCODE_TICK_MAX  16

/*ir Tx state*/
#define IR_TX_HEADER0_TICK      12
#define IR_TX_HEADER1_TICK      1
#define IR_TX_DATA0_0_TICK      3
#define IR_TX_DATA0_1_TICK      1
#define IR_TX_DATA1_0_TICK      1
#define IR_TX_DATA1_1_TICK      3
#define IR_TX_ENDCODE_TICK      (6+1)

#else //(LoPwr) 
#define IR_RX_HEADER0_TICK_MIN  4    //Header      
#define IR_RX_HEADER0_TICK_MAX  12
#define IR_RX_HEADER1_TICK_MIN  1
#define IR_RX_HEADER1_TICK_MAX  4
#define IR_RX_DATA1_0_TICK_MIN  1     //Data Logic 1
#define IR_RX_DATA1_0_TICK_MAX  4
#define IR_RX_DATA1_1_TICK_MIN  1
#define IR_RX_DATA1_1_TICK_MAX  4
#define IR_RX_DATA0_0_TICK_MIN  4     //Data Logic 0
#define IR_RX_DATA0_0_TICK_MAX  8
#define IR_RX_DATA0_1_TICK_MIN  1
#define IR_RX_DATA0_1_TICK_MAX  4
#define IR_RX_ENDCODE_TICK_MIN  4     //End Code
#define IR_RX_ENDCODE_TICK_MAX  8

/*ir Tx state*/
#define IR_TX_HEADER0_TICK      5
#define IR_TX_HEADER1_TICK      1
#define IR_TX_DATA0_0_TICK      3
#define IR_TX_DATA0_1_TICK      1
#define IR_TX_DATA1_0_TICK      1
#define IR_TX_DATA1_1_TICK      1
#define IR_TX_ENDCODE_TICK      (3+1)

#undef  _IR_RX_PORT
#define _IR_RX_PORT             GPIOA
#undef  _IR_RX_PIN
#if     _IR_LOW_POWER_TYPE==TYPE1
#if     _EF_SERIES && _IR_RX_EXT_INT==TRIG_EXT0
    #define _IR_RX_PIN          2
#elif   _EF_SERIES && _IR_RX_EXT_INT==TRIG_EXT1
    #define _IR_RX_PIN          3
#elif   !_EF_SERIES && _IR_RX_EXT_INT==TRIG_EXT0
    #define _IR_RX_PIN          15
#elif   !_EF_SERIES && _IR_RX_EXT_INT==TRIG_EXT1
#if (_IC_BODY==0x11A21 || _IC_BODY==0x11A22 || _IC_BODY==0x12A44)
#error "This IC Body doesn't support EXT1_INT on low power IR application."  
#endif
    #define _IR_RX_PIN          14
#endif
#elif   _IR_LOW_POWER_TYPE==TYPE2
#define _IR_RX_PIN              2
#undef  _IR_RX_EXT_INT
#define _IR_RX_EXT_INT          TRIG_EXT0
#undef  _IR_RX_PWR_PIN
#define _IR_RX_PWR_PIN          12     
#ifndef _IR_RX_MASK_FUNC
#define _IR_RX_MASK_FUNC        15
#endif 
#endif

#if (_IR_RX_TIMER==USE_TIMER0 || _IR_RX_TIMER==USE_TIMER1 || _IR_RX_TIMER==USE_TIMER2 || _IR_RX_TIMER==USE_TIMER3)
#define _IR_RX_USE_TIMER        (_IR_LOW_POWER_TYPE==TYPE2? DISABLE : ENABLE)    
#endif
#if _IR_RX_TIMER==USE_TIMER0
#define P_IR_RX_TIMER           (TIMER0)
#elif _IR_RX_TIMER==USE_TIMER1
#define P_IR_RX_TIMER           (TIMER1)
#elif _IR_RX_TIMER==USE_TIMER2
#define P_IR_RX_TIMER           (TIMER2)
#elif _IR_RX_TIMER==USE_TIMER3
#define P_IR_RX_TIMER           (TIMER3)
#endif

#if _IR_LOW_POWER_TYPE==TYPE1 && _IR_RX_TIMER==USE_RTC
#define BASE_USEC       (1000000/1024.0) //RTC 1KHz
#define TIME2CNT(us)    ((U16)((us/BASE_USEC)+0.5))
#define _TIME2CNT(us)   (TIME2CNT(us)==0? 1: TIME2CNT(us))
#else
#define BASE_USEC       (1000000/32768.0) //ILRC    
#define TIME2CNT(us)    ((U16)((us/BASE_USEC)+0.5))
#define _TIME2CNT(us)   (TIME2CNT(us)<=1? 1: TIME2CNT(us)-1)
#endif
#endif


#if _IR_CARRIER
#define IR_BUSY_LEVEL           IR_STOP_LOW
#else
#define IR_BUSY_LEVEL           IR_STOP_HIGH
#endif

/*#if _IR_RX_PORT==0
#define    IR_GPIO_PORT         P_PORTA
#elif   _IR_RX_PORT==1
#define    IR_GPIO_PORT         P_PORTB
#elif   _IR_RX_PORT==2
#define    IR_GPIO_PORT         P_PORTC
#endif*/
#define    IR_GPIO_PORT         _IR_RX_PORT

#define IR_TX_STATE_IDLE            0
#define IR_TX_STATE_START           1
#define IR_TX_STATE_HEADER_0        2
#define IR_TX_STATE_HEADER_1        3
#define IR_TX_STATE_DATA0_0         4
#define IR_TX_STATE_DATA0_1         5
#define IR_TX_STATE_DATA1_0         6
#define IR_TX_STATE_DATA1_1         7
#define IR_TX_STATE_ENDCODE         8

#if !_EF_SERIES
#if !defined(_IR_TX_OUTPUT)
#define _IR_TX_OUTPUT				USE_IR0_TX
#endif
#endif

#ifndef _IR_RX_IDLE_LEVEL
#define _IR_RX_IDLE_LEVEL    1
#endif
/* Static Variables ----------------------------------------------------------*/
typedef struct IR_TRANSMIT{
    U32 data;
    U32 tx_data;
	U8  state;
    U8  tick;
    U8  bitcnt;
    U8  tx_bitcnt;
    U8  tx_bit;
}IR_TRANSMIT_T;
#if _IR_TX_EN&&_IR_MODULE
static IR_TRANSMIT_T ir_tx;
static U8 IRTxCount;
#endif

//struct ir_receive{
typedef struct IR_RECEIVE{
	U8 state;
    U8 ticks;
    U8 time_duration[2];
}IR_RECEIVE_T;
#if _IR_RX_EN&&_IR_MODULE
static U32 IR_RawData,IR_RxData;
static IR_RECEIVE_T IR_Receive_Data;
static U8 IR_Rec_Enable;
static U8 IR_Data_Available;
static U8 IR_Rx_Count;
//static U8 g_ir_sleep;
static U8 IRRxCount;
#endif

typedef struct IR_RECEIVER2
{
    U16 cnt_StbPeriod;      //T3
    U16 cnt_DetectPeriod;   //T2-T2a
    U16 cnt_MaskPeriod;     //T2a
    U8  autoIntoSTB;
    U8	buf_IR_RxEn;
}IR_RECEIVE2_T;
typedef enum
{
    sts_IDLE,
    sts_INTO_STB_SCAN,
    sts_WAKEUP,
}IR_RECEIVE2_STS_T;
#if (_IR_MODULE && _IR_LOW_POWER_TYPE && _IR_RX_EN)
static IR_RECEIVE2_T        (IR_LoPwr_RxCtl);
static IR_RECEIVE2_STS_T    (IR_LoPwr_RxSts);
#endif

#if _IR_TX_EN&&_IR_MODULE
//------------------------------------------------------------------//
// Get TX Data bits
// Parameter: 
//          NONE
// return value:
//          1: transmitting bits
//          0: Final bit
//------------------------------------------------------------------// 
static U8 GetTxData(void)
{
    if(!ir_tx.tx_bitcnt)
    {
#if _IR_CRC_LEN        
        ir_tx.state = IR_TX_STATE_ENDCODE;
        ir_tx.tick  = IR_TX_ENDCODE_TICK;
#else
        //IR_IdleLevel(_IR_CARRIER);
        if(ir_tx.bitcnt)
        {
            ir_tx.state = IR_TX_STATE_START;
        }
        else
        {
            ir_tx.state = IR_TX_STATE_IDLE;
        }
#endif
        //dprintf("GetTxData 11\r\n");
        return 0;
    }
    
    ir_tx.tx_bitcnt--;
    ir_tx.tx_bit = (ir_tx.tx_data>>ir_tx.tx_bitcnt)&0x1;
    if(ir_tx.tx_bit)
    {
        ir_tx.state = IR_TX_STATE_DATA1_0;
        ir_tx.tick  = IR_TX_DATA1_0_TICK;
    }
    else
    {
        ir_tx.state = IR_TX_STATE_DATA0_0;
        ir_tx.tick  = IR_TX_DATA0_0_TICK;
    }
    //dprintf("GetTxData b %d d %d\r\n",ir_tx.tx_bit,ir_tx.tx_data);
    return 1;
    
}

//------------------------------------------------------------------//
// Get IR Tx status
// Parameter: 
//          NONE
// return value:
//          !0: IR working, 0: Idle
//------------------------------------------------------------------// 
U8 IR_GetTxStatus(void)
{
    return  ir_tx.state;
}

//------------------------------------------------------------------//
// IR TX Process
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
static void IR_TxProcess(void)
{
    switch(ir_tx.state)
    {
        case    IR_TX_STATE_IDLE:
            break;
        case    IR_TX_STATE_START:
            ir_tx.state     = IR_TX_STATE_HEADER_0;
            ir_tx.tick      = IR_TX_HEADER0_TICK;
            ir_tx.tx_data   = ir_tx.data;
            ir_tx.tx_bitcnt = ir_tx.bitcnt;
            ir_tx.bitcnt    = 0;
            break;
        case    IR_TX_STATE_HEADER_0:
            IR_Cmd(_IR_TX_OUTPUT,ENABLE);
            ir_tx.tick--;
            if(!ir_tx.tick)
            {
                ir_tx.state = IR_TX_STATE_HEADER_1;
                ir_tx.tick  = IR_TX_HEADER1_TICK;
            }
            break;
        case    IR_TX_STATE_HEADER_1:
            IR_Cmd(_IR_TX_OUTPUT,DISABLE);
            //IR_IdleLevel(_IR_CARRIER);
            ir_tx.tick--;
            if(!ir_tx.tick)
            {
                GetTxData();
            }
            break;
        case    IR_TX_STATE_DATA0_0:
            IR_Cmd(_IR_TX_OUTPUT,ENABLE);
            ir_tx.tick--;
            if(!ir_tx.tick)
            {
                ir_tx.state = IR_TX_STATE_DATA0_1;
                ir_tx.tick  = IR_TX_DATA0_1_TICK;
            }
            break;
        case    IR_TX_STATE_DATA0_1:
            IR_Cmd(_IR_TX_OUTPUT,DISABLE);
            //IR_IdleLevel(_IR_CARRIER);
            ir_tx.tick--;
            if(!ir_tx.tick)
            {
                GetTxData();
                //dprintf("D0 1\r\n");
            }
            break;
        case    IR_TX_STATE_DATA1_0:
            IR_Cmd(_IR_TX_OUTPUT,ENABLE);
            ir_tx.tick--;
            if(!ir_tx.tick)
            {
                ir_tx.state = IR_TX_STATE_DATA1_1;
                ir_tx.tick  = IR_TX_DATA1_1_TICK;
            }
            break;
        case    IR_TX_STATE_DATA1_1:
            IR_Cmd(_IR_TX_OUTPUT,DISABLE);
            //IR_IdleLevel(_IR_CARRIER);
            ir_tx.tick--;
            if(!ir_tx.tick)
            {
                GetTxData();
            }
            break;
#if _IR_CRC_LEN             
        case    IR_TX_STATE_ENDCODE:
            IR_Cmd(_IR_TX_OUTPUT,ENABLE);
            ir_tx.tick--;
            if(!ir_tx.tick)
            {
                IR_Cmd(_IR_TX_OUTPUT,DISABLE);
                //IR_IdleLevel(_IR_CARRIER);
                if(ir_tx.bitcnt)
                {
                    ir_tx.state = IR_TX_STATE_START;
                }
                else
                {
                    ir_tx.state = IR_TX_STATE_IDLE;
                }
            }
            break;
#endif            
        default:
            break;
    }
}

//------------------------------------------------------------------//
// IR TX Init
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void IR_TxInit(void)
{
	memset((void *)&ir_tx, 0, sizeof(ir_tx));
#if _EF_SERIES
    SMU_PeriphClkCmd(SMU_PERIPH_TMR,ENABLE);
#else
    SMU_PeriphClkCmd(SMU_PERIPH_IR,ENABLE);
#endif
#if _IR_FREQ == 1 //38.5k
    IR_Init(_IR_TX_OUTPUT,IR_BUSY_LEVEL,38500);
#else             //55.5k
    IR_Init(_IR_TX_OUTPUT,IR_BUSY_LEVEL,55500);
#endif
    IRTxCount= IR_TX_COUNT;
}

//------------------------------------------------------------------//
// IR TX Send Command
// Parameter: 
//          data: Tx send data
// return value:
//          NONE
//------------------------------------------------------------------// 
void IR_TxData(U16 data) // pin, bits pull outside
{
    U8 state;

#if _IR_DATA_LEN==4
    data &=0xF;
#elif _IR_DATA_LEN==8
    data &=0xFF;
#elif _IR_DATA_LEN==12
    data &=0xFFF;
#elif _IR_DATA_LEN==16
    data &=0xFFFF;
#endif
#if _IR_CRC_LEN==0
    ir_tx.data = data;
#elif   _IR_CRC_LEN==2 
    ir_tx.data = (data<<_IR_CRC_LEN)| ((~data)&0x3);
#elif   _IR_CRC_LEN==4 
    ir_tx.data = (data<<_IR_CRC_LEN)| ((~data)&0xf);
#elif   _IR_CRC_LEN==8 
    ir_tx.data = (data<<_IR_CRC_LEN)| ((~data)&0xff);
#endif
    ir_tx.bitcnt=_IR_DATA_LEN+_IR_CRC_LEN;
    //dprintf("cmd 0x%x with crc 0x%x state %d\r\n",ir_tx.data,data,ir_tx.state);
    //dprintf("bitcnt %d tx_bitcnt%d\r\n",ir_tx.bitcnt,ir_tx.tx_bitcnt);

    state = ir_tx.state;
    if(state==IR_TX_STATE_IDLE)
    {
        ir_tx.state = IR_TX_STATE_START;
    }
}
#endif

#if _IR_RX_EN&&_IR_MODULE
//------------------------------------------------------------------//
// Turn on/off IR Rx Send 
// Parameter: 
//          cmd: 1: enable, 0:disable
// return value:
//          NONE
//------------------------------------------------------------------// 
void IR_RxCmd(U8 cmd)
{
	if (cmd)
	{
		IR_Rec_Enable = 1;
        IR_RxData = 0;
        GPIO_Init(IR_GPIO_PORT,_IR_RX_PIN,GPIO_MODE_IN_PU);
	}
	else
	{
		IR_Rec_Enable = 0;
        IR_Data_Available = 0;
        IR_Receive_Data.state = IR_RX_STATE_IDLE;
	}
}

//------------------------------------------------------------------//
// Check IR Rx data available
// Parameter: 
//          NONE
// return value:
//          return : 0: no data, 1:with data
//------------------------------------------------------------------// 
U8 IR_CheckRxData(void)
{

    if(IR_Data_Available)
    {
        IR_Data_Available = 0;
        return 1;
    }
    else 
    {
        return 0;
    }
}
//------------------------------------------------------------------//
// Get IR Rx data
// Parameter: 
//          NONE
// return value:
//          return : IR rx data
//------------------------------------------------------------------//
U16 IR_GetRxData(void)
{
	return IR_RxData;
}
//------------------------------------------------------------------//
// Init IR Rx
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void IR_RxInit(void)
{
    GPIO_Init(IR_GPIO_PORT,_IR_RX_PIN,GPIO_MODE_IN_PU);
#if _IR_RX_WAKEUP
    IR_GPIO_PORT->Wakeup_Mask |= (1<<_IR_RX_PIN);
#else
	IR_GPIO_PORT->Wakeup_Mask &= ~(1<<_IR_RX_PIN);
#endif
	IR_Receive_Data.state = IR_RX_STATE_IDLE;
	IR_Rec_Enable=1;
    IR_Data_Available=0;
    IR_RxData = 0;
    IRRxCount=IR_RX_COUNT;
}
//------------------------------------------------------------------//
// IR rx process
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
static void IR_RxProcess(void)
{

    //extern void ClrSleepTimer(void);
	U32 status;

    if(!IR_Rec_Enable)    
    {
        return;
    }
	IR_Receive_Data.ticks++;
	//status = GetIrPinStatus(1);
	status = GPIO_Read(IR_GPIO_PORT, _IR_RX_PIN) ^ (!_IR_RX_IDLE_LEVEL);
	
	
	switch (IR_Receive_Data.state) 
    {
		case IR_RX_STATE_IDLE:
	  		if (!status) 
            {
				//IR_Receive_Data.slot = 0;
				//IR_Receive_Data.time_duration[IR_Receive_Data.slot++] = IR_Receive_Data.ticks;  //time_duration dummy
				IR_Receive_Data.ticks = 0;
				IR_Receive_Data.state = IR_RX_STATE_HEADER_LOW;
                IR_RawData = 0;
                IR_Rx_Count = 0;
                //ClrSleepTimer();
	  		}
	  		break;
		case IR_RX_STATE_HEADER_LOW:
	  		if (status) 
            {
				if(!(IR_Receive_Data.ticks >= IR_RX_HEADER0_TICK_MIN && IR_Receive_Data.ticks <= IR_RX_HEADER0_TICK_MAX))    // 0 ticks
                {                               
    			  	IR_Receive_Data.state = IR_RX_STATE_IDLE;
    				break;
    			}
    			IR_Receive_Data.ticks = 0;
    			IR_Receive_Data.state = IR_RX_STATE_HEADER_HIGH;
			}
			break;
		case IR_RX_STATE_HEADER_HIGH:
			if (!status) 
            {
			    if(!(IR_Receive_Data.ticks >= IR_RX_HEADER1_TICK_MIN && IR_Receive_Data.ticks <= IR_RX_HEADER1_TICK_MAX)) // 1 ticks
                {
                	IR_Receive_Data.state = IR_RX_STATE_IDLE;
					break;
				}
    			IR_Receive_Data.ticks = 0;
    			//IR_Receive_Data.slot = 0;
    			IR_Receive_Data.state = IR_RX_STATE_DATA_LOW;
                //ClrSleepTimer();
			}
			break;
		case IR_RX_STATE_DATA_LOW:
			if (status) 
            {
				IR_Receive_Data.time_duration[0] = IR_Receive_Data.ticks;    //0 ticks

				if (!(IR_Receive_Data.time_duration[0] >= IR_RX_DATA1_0_TICK_MIN && IR_Receive_Data.time_duration[0] <= IR_RX_DATA0_0_TICK_MAX))  
                { 
					IR_Receive_Data.state = IR_RX_STATE_IDLE;
					break;
				}
    			
    			IR_Receive_Data.ticks = 0;
    			IR_Receive_Data.state = IR_RX_STATE_DATA_HIGH;
		  	}
			break;
		case IR_RX_STATE_DATA_HIGH:
#if _IR_CRC_LEN==0            
            if(IR_Rx_Count== (_IR_DATA_LEN-1)) //Final Bit
            {
                if(!status)
                {

                    IR_Receive_Data.state = IR_RX_STATE_IDLE;
                    break;
                }
                else if( (IR_Receive_Data.ticks>=IR_RX_DATA1_1_TICK_MIN) &&
                    (IR_Receive_Data.time_duration[0] >= IR_RX_DATA1_0_TICK_MIN && IR_Receive_Data.time_duration[0] <= IR_RX_DATA1_0_TICK_MAX))                   //Data1, Low:1~4, High:>=4
                {
                    IR_RawData = (IR_RawData << 1) | 1;
                    IR_Data_Available = 1;
#if _IR_DATA_LEN == 4
                    IR_RxData = IR_RawData&0xF;
#elif _IR_DATA_LEN == 8
                    IR_RxData = IR_RawData&0xFF;
#elif _IR_DATA_LEN == 12
                    IR_RxData = IR_RawData&0xFFF;
#elif _IR_DATA_LEN == 16
                    IR_RxData = IR_RawData&0xFFFF;
#endif                    
                    IR_Receive_Data.state = IR_RX_STATE_IDLE;
                  
                }
                else if( (IR_Receive_Data.ticks>=IR_RX_DATA0_1_TICK_MIN) &&
                    (IR_Receive_Data.time_duration[0] >= IR_RX_DATA0_0_TICK_MIN && IR_Receive_Data.time_duration[0] <= IR_RX_DATA0_0_TICK_MAX))                   //Data0, Low:4~8, High:>=1
                {
                    IR_RawData<<= 1;
                    IR_Data_Available = 1;
#if _IR_DATA_LEN == 4
                    IR_RxData = IR_RawData&0xF;
#elif _IR_DATA_LEN == 8
                    IR_RxData = IR_RawData&0xFF;
#elif _IR_DATA_LEN == 12
                    IR_RxData = IR_RawData&0xFFF;
#elif _IR_DATA_LEN == 16
                    IR_RxData = IR_RawData&0xFFFF;
#endif                     
                    IR_Receive_Data.state = IR_RX_STATE_IDLE;

                }
                          
            }
            else if(!status)
#else                
			if (!status) 
#endif                
            {
				//Dprintf("number of %d data ====", IR_Rx_Count);
				IR_Receive_Data.time_duration[1] = IR_Receive_Data.ticks;            //1 ticks

				if ( (IR_Receive_Data.time_duration[0] >= IR_RX_DATA1_0_TICK_MIN && IR_Receive_Data.time_duration[0] <= IR_RX_DATA1_0_TICK_MAX) &&                //Data1, Low:1~4, High:4~8
                     (IR_Receive_Data.time_duration[1] >= IR_RX_DATA1_1_TICK_MIN && IR_Receive_Data.time_duration[1] <= IR_RX_DATA1_1_TICK_MAX)) 
                {
  		   			IR_RawData = (IR_RawData << 1) | 1;
				} 
				else if ( (IR_Receive_Data.time_duration[0] >= IR_RX_DATA0_0_TICK_MIN && IR_Receive_Data.time_duration[0] <= IR_RX_DATA0_0_TICK_MAX) &&           //Data0, High:4~8, Low:1~4
                          (IR_Receive_Data.time_duration[1] >= IR_RX_DATA0_1_TICK_MIN && IR_Receive_Data.time_duration[1] <= IR_RX_DATA0_1_TICK_MAX))
                {
  		   			IR_RawData <<= 1;
				}
				else
                {
                    //dprintf("Error Package\r\n");
					IR_Receive_Data.state = IR_RX_STATE_IDLE;
					break;
				}
			    IR_Rx_Count++;
#if _IR_CRC_LEN
    		    if(IR_Rx_Count == (_IR_DATA_LEN+_IR_CRC_LEN))   //_IR_DATA_LEN
    		    {
                    U8 i;
                    U8 rxd,rxdinv;

                    // CRC check
                    for(i=0;i<_IR_CRC_LEN;i++)
                    {
                        rxd    = (IR_RawData>>(i+_IR_CRC_LEN)) &0x01;
                        rxdinv = (IR_RawData>>i) &0x01;
                        if(rxd==rxdinv)
                        {
                            i=0;
                            break;
                        }
                    }
                    if(!i)
                    {
                        IR_Receive_Data.state = IR_RX_STATE_IDLE;
                        break;
                    }
                    else
                    {
                        IR_RawData >>= _IR_CRC_LEN;
                        IR_Receive_Data.state = IR_RX_STATE_ENDCODE;
                    }
                    
    		    }

				else
#endif
				{
			        IR_Receive_Data.state = IR_RX_STATE_DATA_LOW;
				}
                IR_Receive_Data.ticks = 0;
			}
            
			break;




            
#if _IR_CRC_LEN             
        case IR_RX_STATE_ENDCODE:
            if(status)
            {

                if(IR_Receive_Data.ticks>=IR_RX_ENDCODE_TICK_MIN && IR_Receive_Data.ticks<=IR_RX_ENDCODE_TICK_MAX) // 0 tick
                {
#if _IR_DATA_LEN == 4
                    IR_RxData = IR_RawData&0xF;
#elif _IR_DATA_LEN == 8
                    IR_RxData = IR_RawData&0xFF;
#elif _IR_DATA_LEN == 12
                    IR_RxData = IR_RawData&0xFFF;
#elif _IR_DATA_LEN == 16
                    IR_RxData = IR_RawData&0xFFFF;
#endif                     
        		    IR_Data_Available = 1;
                }
                IR_Receive_Data.state = IR_RX_STATE_IDLE;
            }
            break;
#endif            
	}
    
    if(IR_Receive_Data.state!=IR_RX_STATE_IDLE)
    {
        //g_ir_sleep = 1;
        if(IR_Receive_Data.ticks>IR_RX_TIMEOUT)
        {
            IR_Receive_Data.state = IR_RX_STATE_IDLE;
        }
    }
    
}
//------------------------------------------------------------------//
// IR rx start monitor and check if command input
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void IR_MonitorSleep(void)
{
    //g_ir_sleep = 0;
}

//------------------------------------------------------------------//
// Get the result of IR rx monitor 
// Parameter: 
//          NONE
// return value:
//          0:RX still working
//			1:RX is IDLE state, can into sleep
//------------------------------------------------------------------//
U8 IR_CheckRxSleep(void)
{
    //return g_ir_sleep;
    return ((IR_Receive_Data.state==IR_RX_STATE_IDLE) ? 1 : 0);
}

#if _IR_LOW_POWER_TYPE

#if _IR_RX_USE_TIMER
#if _EF_SERIES
#define TMR_INIT0	(C_TMR_CLK_Src_LoCLK | C_TMR_Auto_Reload_En)
#else
#define TMR_INIT0	(C_TMR_CLK_Src_LoCLK | C_TMR_CLK_Div_1 | C_TMR_Auto_Reload_En)
#endif
#define TMR_INIT1	(TMR_INIT0| C_TMR_En)
//------------------------------------------------------------------//
// IR rx(LoPwr) timer setup
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
static inline void _IR_LoPwr_RxTimerInit(U16 counter)
{
    P_IR_RX_TIMER->Ctrl = TMR_INIT0;
    P_IR_RX_TIMER->Data = counter;
    P_IR_RX_TIMER->Ctrl = TMR_INIT1;
	P_IR_RX_TIMER->Flag = 1;    
}
#endif
//------------------------------------------------------------------//
// IR rx(LoPwr) timing setup
// Parameter:
//          T3_us:  Set standby mode period
//          T2_us:  Set enable receiver period
//          T2a_us: Set mask period within T2 (for receiver stable)
// return value:
//          0:ok
//			others:fail
//------------------------------------------------------------------//
U8 IR_LoPwr_RxTimerSet(U32 T3_us, U32 T2_us, U32 T2a_us)
{
    if( (IR_LoPwr_RxSts == sts_INTO_STB_SCAN)   \
     || (!T3_us || !T2_us || !T2a_us)           \
     || !(T2a_us<T2_us)                         \
    )
    {
        return (1);
    }

    U16 cnt3  = _TIME2CNT( _IR_LOPWR_RX_STB_TIME);
    U16 cnt2  = _TIME2CNT( _IR_LOPWR_RX_ENPWR_TIME);
    U16 cnt2a = _TIME2CNT( _IR_LOPWR_RX_MASK_TIME);
    
    cnt2 = ((cnt2==cnt2a)? (cnt2+1) : (cnt2));

#if _IR_LOW_POWER_TYPE==TYPE1
    IR_LoPwr_RxCtl.cnt_StbPeriod    = cnt3;
    IR_LoPwr_RxCtl.cnt_DetectPeriod = cnt2-cnt2a;
    IR_LoPwr_RxCtl.cnt_MaskPeriod   = cnt2a;
#elif _IR_LOW_POWER_TYPE==TYPE2
    PWMA->TMR_Ctrl = (PWMA_CLK_Src_LoCLK) | (PWMA_IRWK_MASK_DIN_ToHi);
    U16 cnt1 = (cnt3+cnt2+1);
    PWMA->TMR_Data = cnt1;                                                  //T1(Scan period) = T3+T2
    PWMA->IO0_Duty = ((cnt1-cnt2+1) <<16) | PWM_START_LOW  | C_PWM_OUT_En;  //EnPwr
    PWMA->IO3_Duty = ((cnt1-cnt2a+1)<<16) | PWM_START_HIGH | C_PWM_OUT_En;  //Mask
#endif

    return (0);
}
//------------------------------------------------------------------//
// Turn on/off IR rx(LoPwr) auto inot standby mode scan
// Parameter: 
//          cmd: ENABLE/DISABLE
// return value:
//          NONE
//------------------------------------------------------------------//
void IR_LoPwr_RxAutoScan(U8 cmd)
{
    IR_LoPwr_RxCtl.autoIntoSTB = cmd;    
}
//------------------------------------------------------------------//
// IR rx(LoPwr) IO setup
// Parameter:
//          sts: sts_IDLE/sts_INTO_STB_SCAN/sts_WAKEUP
// return value:
//          NONE
//------------------------------------------------------------------//
static void IR_LoPwr_RxIOInit(U8 sts)
{
    switch(sts)
    {
        case sts_IDLE:
        case sts_WAKEUP:
                                GPIO_ExtIntCmd(_IR_RX_EXT_INT, DISABLE);
                                GPIO_Init(_IR_RX_PWR_PORT, _IR_RX_PWR_PIN, GPIO_MODE_OUT_LOW);  //enable receiver
                                GPIO_Init(_IR_RX_PORT, _IR_RX_PIN, GPIO_MODE_IN_PU);
                                GPIO_WakeUpCmd(_IR_RX_PORT, _IR_RX_PIN, _IR_RX_WAKEUP);         //follow nx1_config.h setting        
                                break;

        case sts_INTO_STB_SCAN:
#if _IR_LOW_POWER_TYPE==TYPE1
                                GPIO_ExtTrigInit(_IR_RX_EXT_INT, GPIO_EXT_RISING_FALLING);
                                GPIO_Init(_IR_RX_PWR_PORT, _IR_RX_PWR_PIN, GPIO_MODE_OUT_HI);   //disable receiver
                                GPIO_Init(_IR_RX_PORT, _IR_RX_PIN, GPIO_MODE_OUT_HI);
                                GPIO_WakeUpCmd(_IR_RX_PORT, _IR_RX_PIN, DISABLE);
#elif _IR_LOW_POWER_TYPE==TYPE2
                                GPIO_ExtTrigInit(_IR_RX_EXT_INT, GPIO_EXT_FALLING);
                                GPIO_Init(_IR_RX_PWR_PORT, _IR_RX_PWR_PIN, GPIO_MODE_ALT);      //PA12 MFP
                                GPIO_Init(_IR_RX_PWR_PORT, _IR_RX_MASK_FUNC, GPIO_MODE_ALT);    //PA15 MFP
                                GPIO_Init(_IR_RX_PORT, _IR_RX_PIN, GPIO_MODE_ALT2);             //PA2 MFP
                                GPIO_WakeUpCmd(_IR_RX_PORT, _IR_RX_PIN, DISABLE);                             
#endif
                                break;             
    }
    IR_LoPwr_RxSts = sts; //update status
}
//------------------------------------------------------------------//
// IR rx(LoPwr) init
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void IR_LoPwr_RxInit(void)
{
    memset((void *)&IR_LoPwr_RxCtl, 0, sizeof(IR_LoPwr_RxCtl));
#if _IR_LOW_POWER_TYPE==TYPE2
    SMU_PeriphClkCmd(SMU_PERIPH_PWM,ENABLE);  
#endif    
    IR_LoPwr_RxIOInit(sts_IDLE);
    IR_LoPwr_RxTimerSet(_IR_LOPWR_RX_STB_TIME, _IR_LOPWR_RX_ENPWR_TIME, _IR_LOPWR_RX_MASK_TIME);    
    IR_LoPwr_RxAutoScan(ENABLE);
}
//------------------------------------------------------------------//
// Turn on/off IR rx(LoPwr) standby mode scan 
// Parameter:
//          cmd: ENABLE/DISABLE
// return value:
//          NONE
//------------------------------------------------------------------//
void IR_LoPwr_RxCmd(U8 cmd)
{
    if(cmd && IR_LoPwr_RxSts!=sts_INTO_STB_SCAN)
    {
        IR_LoPwr_RxCtl.buf_IR_RxEn = IR_Rec_Enable;
        IR_RxCmd(DISABLE);
        IR_LoPwr_RxIOInit(sts_INTO_STB_SCAN);
    }
    else if (!cmd)
    {
        IR_LoPwr_RxIOInit(sts_IDLE);
        IR_RxCmd(IR_LoPwr_RxCtl.buf_IR_RxEn);
    }
}
//------------------------------------------------------------------//
// IR rx(LoPwr) init before standby mode
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void IR_LoPwr_RxInitBeforeStandby(void)
{
#if _IR_LOW_POWER_TYPE==TYPE1
#if     _IR_RX_TIMER==USE_RTC
    P_RTC->CLR=0x3ca5;
    P_RTC->Flag=P_RTC->Flag;
#elif   _IR_RX_USE_TIMER
    _IR_LoPwr_RxTimerInit(IR_LoPwr_RxCtl.cnt_StbPeriod);
#endif 
#elif _IR_LOW_POWER_TYPE==TYPE2
    PWMA->TMR_Ctrl |= C_PWM_En;
    PWMA->TMR_Flag = 1;
    P_PORT_EXT->EXT_INT_Flag = (1<<_IR_RX_EXT_INT);
#endif
}
//------------------------------------------------------------------//
// IR rx(LoPwr) init after standby mode
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void IR_LoPwr_RxInitAfterStandby(void)
{
#if     _IR_LOW_POWER_TYPE==TYPE1 
#if _IR_RX_USE_TIMER
    P_IR_RX_TIMER->Ctrl &= ~C_TMR_En;
#endif
#elif   _IR_LOW_POWER_TYPE==TYPE2
    PWMA->TMR_Ctrl &= ~C_PWM_En;
#endif    
}
//------------------------------------------------------------------//
// Get IR rx(LoPwr) sleep status
// Parameter:
//          NONE
// return value:
//          0: can't sleep
//          1: sleep
//------------------------------------------------------------------//
U8 IR_LoPwr_RxGetSleepFlag(void)
{
    if(IR_LoPwr_RxSts==sts_INTO_STB_SCAN)
    {
        return 1;       
    }
    else if(IR_LoPwr_RxSts==sts_WAKEUP && IR_LoPwr_RxCtl.autoIntoSTB)
    {
        IR_LoPwr_RxCmd(ENABLE);
        return 1;
    }
    else
    {
        return 0;
    }
}
#if _IR_LOW_POWER_TYPE==TYPE1
//------------------------------------------------------------------//
// IR rx(LoPwr) data pin set as external interrupt
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
static inline void _IR_LoPwr_DatPin_ExtInt(void)
{
#if _EF_SERIES
		_IR_RX_PORT->MFP &= (~(3<<(_IR_RX_PIN*2)));
		_IR_RX_PORT->MFP |= ((GPIO_MODE_ALT2-GPIO_MODE_ALT_OFFSET)<<(_IR_RX_PIN*2));
#else
#if _IR_RX_EXT_INT==TRIG_EXT0
        _IR_RX_PORT->MFP |= (1<<_IR_RX_PIN);
#elif _IR_RX_EXT_INT==TRIG_EXT1
        _IR_RX_PORT->MFP &= (~(1<<_IR_RX_PIN));
        _IR_RX_PORT->MFP |= (1<<(_IR_RX_PIN+GPIO_ALT2_OFFSET));    
#endif
#endif   
}
//------------------------------------------------------------------//
// IR rx(LoPwr) RTC init
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
static inline void _IR_LoPwr_RTC_Init(U8 freq)
{
	P_RTC->INT_Ctrl = freq;
	P_RTC->CLR=0x3ca5;
	P_RTC->Flag=P_RTC->Flag;    
}
//------------------------------------------------------------------//
// IR rx(LoPwr) External interrupt on/off
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
static inline void _IR_LoPwr_ExtInt(U8 cmd)
{
    P_PORT_EXT->EXT_INT_Flag |= (1<<_IR_RX_EXT_INT);
    P_PORT_EXT->EXT_INT_Ctrl &= ~(1<<_IR_RX_EXT_INT);
    if(cmd)
        P_PORT_EXT->EXT_INT_Ctrl |= (cmd<<_IR_RX_EXT_INT);   
}
//------------------------------------------------------------------//
// IR rx(LoPwr) data pin set as output hi
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
static inline void _IR_LoPwr_DatPin_OutHi(void)
{
    _IR_RX_PORT->Data |= (1<<_IR_RX_PIN);
#if _EF_SERIES
    _IR_RX_PORT->MFP &= ~(3<<(_IR_RX_PIN*2));
#else
    _IR_RX_PORT->MFP &= ~(1<<_IR_RX_PIN);
#endif
    _IR_RX_PORT->DIR &= ~(1<<_IR_RX_PIN);    
}
#endif
//------------------------------------------------------------------//
// IR rx(LoPwr) detect process
// Parameter:
//          NONE
// return value:
//          0:keep processing
//          1:wakeup
//------------------------------------------------------------------//
U8 IR_LoPwr_RxProcess(void)
{
    if(IR_LoPwr_RxSts==sts_IDLE) return 1;

#if     _IR_LOW_POWER_TYPE==TYPE1   
    U8 cnt_trigger=0;
    
    _IR_RX_PWR_PORT->Data &=~(1<<_IR_RX_PWR_PIN);   //enable receiver
    
    _IR_LoPwr_DatPin_ExtInt();                      //data pin switch to ext_int

#if     _IR_RX_TIMER==USE_RTC
    U8 freq = P_RTC->INT_Ctrl;
    _IR_LoPwr_RTC_Init(RTC_1KHZ);
    
    U8 sts=0, cnt=0;
    U8 _tick = IR_LoPwr_RxCtl.cnt_MaskPeriod;
    do {
        SMU_Sleep(C_SLP_Standby_Mode);

        if(SYS_IsWakeupPinChange()==true) return 1;

        if(P_RTC->Flag & C_RTC_1KHz_Flag)
        {
            cnt++;
            P_RTC->Flag = P_RTC->Flag;  
        }
        if((P_PORT_EXT->EXT_INT_Flag & (1<<_IR_RX_EXT_INT)) && sts)
        {
            cnt_trigger++;
            P_PORT_EXT->EXT_INT_Flag = (1<<_IR_RX_EXT_INT);
        }
        if(cnt == _tick)
        {
            if(!sts)
            {
                sts=1;
                _tick = IR_LoPwr_RxCtl.cnt_DetectPeriod;
                cnt=0;
                _IR_LoPwr_ExtInt(ENABLE);
            }
            else
                break;
        }
    }while(1);
        
#elif   _IR_RX_USE_TIMER
    _IR_LoPwr_RxTimerInit(IR_LoPwr_RxCtl.cnt_MaskPeriod);
    SMU_Sleep(C_SLP_Standby_Mode);
    if(SYS_IsWakeupPinChange()==true) return 1;
    _IR_LoPwr_RxTimerInit(IR_LoPwr_RxCtl.cnt_DetectPeriod);
    _IR_LoPwr_ExtInt(ENABLE);
    do {
        SMU_Sleep(C_SLP_Standby_Mode);
        if(SYS_IsWakeupPinChange()==true) return 1;
        if(P_PORT_EXT->EXT_INT_Flag & (1<<_IR_RX_EXT_INT))
        {
            cnt_trigger++;
            P_PORT_EXT->EXT_INT_Flag = (1<<_IR_RX_EXT_INT);
        }        
    }while(!P_IR_RX_TIMER->Flag);
#endif

    _IR_LoPwr_ExtInt(DISABLE);
    _IR_RX_PWR_PORT->Data |= (1<<_IR_RX_PWR_PIN);   //disable receiver
    
    if(cnt_trigger<1)
    {
        _IR_LoPwr_DatPin_OutHi();
#if _IR_RX_TIMER == USE_RTC
        _IR_LoPwr_RTC_Init(freq);
#elif   _IR_RX_USE_TIMER
        _IR_LoPwr_RxTimerInit(IR_LoPwr_RxCtl.cnt_StbPeriod);
#endif        
    }
    else
    {
        IR_LoPwr_RxIOInit(sts_WAKEUP);
        IR_RxCmd(ENABLE);
    }

#elif   _IR_LOW_POWER_TYPE==TYPE2
    IR_LoPwr_RxIOInit(sts_WAKEUP);
    IR_RxCmd(ENABLE);
#endif
    
    return ( IR_LoPwr_RxSts==sts_WAKEUP? 1:0 );
}

#endif //_IR_LOW_POWER_TYPE
#endif

#if(_IR_MODULE)&&(_IR_TX_EN||_IR_RX_EN)
//------------------------------------------------------------------//
// IR Timer ISR
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void IR_TimerIsr(void)
{
#if _IR_TX_EN  
        IRTxCount--; //0.5ms
        if(!IRTxCount)
        {
            IRTxCount= IR_TX_COUNT; //4K or 16K
            IR_TxProcess();
        }
#endif   

#if _IR_RX_EN == 1
        IRRxCount--;
        if(!IRRxCount)
        {
            IRRxCount=IR_RX_COUNT;
            IR_RxProcess();
        }
#endif 
}
#endif