#include "nx1_i2c.h"
#include "include.h"
#include "nx1_smu.h"
#include "nx1_gpio.h"
#include <string.h>

#if defined(P_I2C)
//----------------------------------------------------------------------------//
// I2C Get fifo size(in Byte)
// Parameter:
//            NONE
// return value:
//            4(fix)
//----------------------------------------------------------------------------//
U8   I2C_GetFifoSize(void) { //(M/S)
    return 4; //(0x1<<(P_I2C->FIFO_Ctrl+1))
}

//----------------------------------------------------------------------------//
// I2C Set interrupt source
// Parameter:
//            _src  :   C_I2C_CMPL_IEn (M/S)
//                      C_I2C_BYTE_Rx_IEn (M/S)
//                      C_I2C_BYTE_Tx_IEn (M/S)
//                      C_I2C_START_IEn (M/S)
//                      C_I2C_STOP_IEn (M/S)
//                      C_I2C_ARB_LOSE_IEn (M)
//                      C_I2C_ADDR_HIT_IEn (M/S)
//                      C_I2C_FIFO_HALF_IEn (M/S)
//                      C_I2C_FIFO_FULL_IEn (M/S)
//                      C_I2C_FIFO_EMPTY_IEn (M/S)
// return value:
//            NONE
//----------------------------------------------------------------------------//
void I2C_SetInterruptSource(U32 _src) { //(M/S)
    P_I2C->INT_Ctrl = 0;//reEn
    P_I2C->INT_Ctrl = _src;
}

//----------------------------------------------------------------------------//
// I2C Get interrupt source
// Parameter:
//            NONE
// return value:
//            INT_Ctrl(Ref. I2C_SetInterruptSource)
//----------------------------------------------------------------------------//
U32  I2C_GetInterruptSource(void) { //(M/S)
    return P_I2C->INT_Ctrl;
}

//------------------------------------------------------------------//
// Re-Enable I2C interrupt
// Parameter: 
//          None      
// return value:
//          None
//------------------------------------------------------------------// 
void I2C_Int_ReEn(void)
{
    U16 I2C_Int_temp = P_I2C->INT_Ctrl;
    P_I2C->INT_Ctrl = DISABLE;
    P_I2C->INT_Ctrl = I2C_Int_temp;
}

//----------------------------------------------------------------------------//
// I2C is SDA Hi?
// Parameter:
//            NONE
// return value:
//            true / false
//----------------------------------------------------------------------------//
bool I2C_IsSdaHigh(void) { //(M/S)
    return (P_I2C->Status & C_I2C_SDA_Status)>0;
}

//----------------------------------------------------------------------------//
// I2C is SCL Hi?
// Parameter:
//            NONE
// return value:
//            true / false
//----------------------------------------------------------------------------//
bool I2C_IsSclHigh(void) { //(M/S)
    return (P_I2C->Status & C_I2C_SCL_Status)>0;
}

//----------------------------------------------------------------------------//
// I2C is General Call? (Slave only)
// Parameter:
//            NONE
// return value:
//            true / false
//----------------------------------------------------------------------------//
bool I2C_IsGenCall(void) { //(S)
    return (P_I2C->Status & C_I2C_Gen_Call)>0;
}

//----------------------------------------------------------------------------//
// I2C is Bus Busy?
// Parameter:
//            NONE
// return value:
//            true / false
//----------------------------------------------------------------------------//
bool I2C_IsBusBusy(void) { //(M/S)
    return (P_I2C->Status & C_I2C_Busy)>0;
}

//----------------------------------------------------------------------------//
// I2C is last read/write ACK?
// Parameter:
//            NONE
// return value:
//            true / false
//----------------------------------------------------------------------------//
bool I2C_IsAck(void) { //(M/S)
    return (P_I2C->Status & C_I2C_ACK)>0;
}

//----------------------------------------------------------------------------//
// I2C is Transaction Completion?
// Parameter:
//            NONE
// return value:
//            true / false
//----------------------------------------------------------------------------//
bool I2C_IsCmpl(void) { //(M/S)
    return (P_I2C->Status & C_I2C_CMPL_Flag)>0;
}

//----------------------------------------------------------------------------//
// I2C is a Byte Received?
// Parameter:
//            NONE
// return value:
//            true / false
//----------------------------------------------------------------------------//
bool I2C_IsByteRecv(void) { //(M/S)
    return (P_I2C->Status & C_I2C_BYTE_Rx_Flag)>0;
}

//----------------------------------------------------------------------------//
// I2C is a Byte Transmitted?
// Parameter:
//            NONE
// return value:
//            true / false
//----------------------------------------------------------------------------//
bool I2C_IsByteTrans(void) { //(M/S)
    return (P_I2C->Status & C_I2C_BYTE_Tx_Flag)>0;
}

//----------------------------------------------------------------------------//
// I2C is Start condition transmitted / received?
// Parameter:
//            NONE
// return value:
//            true / false
//----------------------------------------------------------------------------//
bool I2C_IsStart(void) { //(M/S)
    return (P_I2C->Status & C_I2C_START_Flag)>0;
}

//----------------------------------------------------------------------------//
// I2C is Stop condition transmitted / received?
// Parameter:
//            NONE
// return value:
//            true / false
//----------------------------------------------------------------------------//
bool I2C_IsStop(void) { //(M/S)
    return (P_I2C->Status & C_I2C_STOP_Flag)>0;
}

//----------------------------------------------------------------------------//
// I2C is lose bus arbitration?(Master only)
// Parameter:
//            NONE
// return value:
//            true / false
//----------------------------------------------------------------------------//
bool I2C_IsArbLose(void) { //(M)
    return (P_I2C->Status & C_I2C_ARB_LOSE_Flag)>0;
}

//----------------------------------------------------------------------------//
// I2C is Address Hit
// Parameter:
//            NONE
// return value:
//            true / false
//----------------------------------------------------------------------------//
bool I2C_IsAddrHit(void) { //(M/S)
    return (P_I2C->Status & C_I2C_ADDR_HIT_Flag)>0;
}

//----------------------------------------------------------------------------//
// I2C is FIFO half
// Parameter:
//            NONE
// return value:
//            true / false
//----------------------------------------------------------------------------//
bool I2C_IsFifoHalf(void) { //(M/S)
    return (P_I2C->Status & C_I2C_FIFO_HALF_Flag)>0;
}

//----------------------------------------------------------------------------//
// I2C is FIFO full
// Parameter:
//            NONE
// return value:
//            true / false
//----------------------------------------------------------------------------//
bool I2C_IsFifoFull(void) { //(M/S)
    return (P_I2C->Status & C_I2C_FIFO_FULL_Flag)>0;
}

//----------------------------------------------------------------------------//
// I2C is FIFO empty
// Parameter:
//            NONE
// return value:
//            true / false
//----------------------------------------------------------------------------//
bool I2C_IsFifoEmpty(void) { //(M/S)
    return (P_I2C->Status & C_I2C_FIFO_EMPTY_Flag)>0;
}

//----------------------------------------------------------------------------//
// I2C get all status
// Parameter:
//            NONE
// return value:
//            NONE
//----------------------------------------------------------------------------//
U32  I2C_GetAllSts(void) { //(M/S)
    return P_I2C->Status;
}

//----------------------------------------------------------------------------//
// I2C clear all status
// Parameter:
//            NONE
// return value:
//            NONE
//----------------------------------------------------------------------------//
void I2C_ClrAllSts(void) { //(M/S)
    P_I2C->Status = P_I2C->Status;
}

//----------------------------------------------------------------------------//
// I2C clear Completion status
// Parameter:
//            NONE
// return value:
//            NONE
//----------------------------------------------------------------------------//
void I2C_ClrCmplSts(void) { //(M/S)
    P_I2C->Status = C_I2C_CMPL_Flag; //W1C
}

//----------------------------------------------------------------------------//
// I2C clear Byte Received status
// Parameter:
//            NONE
// return value:
//            NONE
//----------------------------------------------------------------------------//
void I2C_ClrByteRecvSts(void) { //(M/S)
    P_I2C->Status = C_I2C_BYTE_Rx_Flag; //W1C
}

//----------------------------------------------------------------------------//
// I2C clear Byte Transmitted status
// Parameter:
//            NONE
// return value:
//            NONE
//----------------------------------------------------------------------------//
void I2C_ClrByteTransSts(void) { //(M/S)
    P_I2C->Status = C_I2C_BYTE_Tx_Flag; //W1C
}

//----------------------------------------------------------------------------//
// I2C clear Start Condition status
// Parameter:
//            NONE
// return value:
//            NONE
//----------------------------------------------------------------------------//
void I2C_ClrStartSts(void) { //(M/S)
    P_I2C->Status = C_I2C_START_Flag; //W1C
}

//----------------------------------------------------------------------------//
// I2C clear Stop Condition status
// Parameter:
//            NONE
// return value:
//            NONE
//----------------------------------------------------------------------------//
void I2C_ClrStopSts(void) { //(M/S)
    P_I2C->Status = C_I2C_STOP_Flag; //W1C
}

//----------------------------------------------------------------------------//
// I2C clear Arbitration Lose status (Master only)
// Parameter:
//            NONE
// return value:
//            NONE
//----------------------------------------------------------------------------//
void I2C_ClrArbLoseSts(void) { //(M)
    P_I2C->Status = C_I2C_ARB_LOSE_Flag; //W1C
}

//----------------------------------------------------------------------------//
// I2C clear Address Hit status
// Parameter:
//            NONE
// return value:
//            NONE
//----------------------------------------------------------------------------//
void I2C_ClrAddrHitSts(void) { //(M/S)
    P_I2C->Status = C_I2C_ADDR_HIT_Flag; //W1C
}

//----------------------------------------------------------------------------//
// I2C clear Address Hit status
// Parameter:
//            _addr: 0x0~0x7F(7 bits) / 0x0~0x3FF(10 bits)
//            _isAddr10bit: true / false
// return value:
//            NONE
//----------------------------------------------------------------------------//
void I2C_SetAddr(const U32 _addr, const bool _isAddr10bit) { //(M/S)
    if(!_isAddr10bit) {
        P_I2C->Ctrl = (P_I2C->Ctrl & ~C_I2C_Addr_Mode) | C_I2C_Addr_Mode_7bit;
        P_I2C->Addr = _addr&((1<<7)-1);
    } else {
        P_I2C->Ctrl = (P_I2C->Ctrl & ~C_I2C_Addr_Mode) | C_I2C_Addr_Mode_10bit;
        P_I2C->Addr = _addr&((1<<10)-1);
    }
}

//----------------------------------------------------------------------------//
// I2C push byte to HW FIFO
// Parameter:
//            _data: Byte data
// return value:
//            NONE
//----------------------------------------------------------------------------//
void I2C_PushData(const U8 _data) { //(M/S)
    P_I2C->Data = _data;
}

//----------------------------------------------------------------------------//
// I2C get byte to HW FIFO
// Parameter:
//            NONE
// return value:
//            _data: Byte data
//----------------------------------------------------------------------------//
U8   I2C_GetData(void) { //(M/S)
    return P_I2C->Data;
}

//----------------------------------------------------------------------------//
// I2C Set Control Mode (Master only)
// Parameter:
//            _phase: C_I2C_PHA_Start / C_I2C_PHA_Addr / C_I2C_PHA_Data / C_I2C_PHA_Stop
//            _dataCnt: 1~256, over 256 will saturate at 256.
//            _dir: I2C_DIR_TX / I2C_DIR_RX
// return value:
//            NONE
//----------------------------------------------------------------------------//
void I2C_SetControlMode(U32 _phase, U32 _dataCnt, U32 _dir) { //(M)
    _phase&=(C_I2C_PHA_Start|C_I2C_PHA_Addr|C_I2C_PHA_Data|C_I2C_PHA_Stop);
    _dataCnt = (_dataCnt>=256)?0:_dataCnt;
    _dir = (_dir==I2C_DIR_TX)?C_I2C_Master_Dir_Tx:C_I2C_Master_Dir_Rx;
    P_I2C->Mode = _phase+_dir+_dataCnt;
}

//----------------------------------------------------------------------------//
// I2C Get Data Count
// Parameter:
//            NONE
// return value:
//            0~255(0 mean 256)
//            -> TransMitted/Received data count(SLAVE increase/ MASTER decrease)
//----------------------------------------------------------------------------//
U8   I2C_GetDataCnt(void) { //(M/S)
    return (P_I2C->Mode)&C_I2C_Data_Count;
}

//----------------------------------------------------------------------------//
// I2C Clear Data Count (Slave only)
// Parameter:
//            NONE
// return value:
//            NONE
//----------------------------------------------------------------------------//
void I2C_ClrDataCnt(void) { //(S)
    P_I2C->Mode=P_I2C->Mode & ~C_I2C_Data_Count;
}

//----------------------------------------------------------------------------//
// I2C is dir TX?
// Parameter:
//            NONE
// return value:
//            true / false
//----------------------------------------------------------------------------//
bool I2C_IsDirTx(void) { //(M/S)
    if(I2C_IsMasterMode()) {
        return !((P_I2C->Mode & C_I2C_Master_Dir_Rx)>0);
    } else {
        return ((P_I2C->Mode & C_I2C_Slave_Dir_Tx)>0);
    }
}

//----------------------------------------------------------------------------//
// I2C Issue Data Transaction (Master only)
// Parameter:
//            NONE
// return value:
//            NONE
//----------------------------------------------------------------------------//
void I2C_IssueDataTransaction(void) { //(M)
    P_I2C->Cmd = C_I2C_CMD_Issue;
}

//----------------------------------------------------------------------------//
// I2C Is Transacting? (Master only)
// Parameter:
//            NONE
// return value:
//            true / false
//----------------------------------------------------------------------------//
bool I2C_IsDataTransaction(void) { //(M)
    return (P_I2C->Cmd & C_I2C_CMD_Issue) >0;
}

//----------------------------------------------------------------------------//
// I2C Respond ACK(Must disable auto-ack)
// Parameter:
//            NONE
// return value:
//            NONE
//----------------------------------------------------------------------------//
void I2C_RespondACK(void) { //(M/S)
    P_I2C->Cmd = C_I2C_CMD_Resp_ACK;
}

//----------------------------------------------------------------------------//
// I2C Respond NACK(Must disable auto-ack)
// Parameter:
//            NONE
// return value:
//            NONE
//----------------------------------------------------------------------------//
void I2C_RespondNACK(void) { //(M/S)
    P_I2C->Cmd = C_I2C_CMD_Resp_NACK;
}

//----------------------------------------------------------------------------//
// I2C Clear FIFO
// Parameter:
//            NONE
// return value:
//            NONE
//----------------------------------------------------------------------------//
void I2C_ClrFifo(void) { //(M/S)
    P_I2C->Cmd = C_I2C_CMD_CLR_FIFO;
}

//----------------------------------------------------------------------------//
// I2C Clear All Controller(FIFO / Status / Current Transaction)
// Parameter:
//            NONE
// return value:
//            NONE
//----------------------------------------------------------------------------//
void I2C_RstController(void) { //(M/S)
    P_I2C->Cmd = C_I2C_CMD_Reset;
}

//====================  P_I2C->Ctrl     ====================//
//nx1_i2c.c
#define CLK_IN_US       (OPTION_HIGH_FREQ_CLK/1000000)
#define RD_ns2clk(ns)   (( (ns)*CLK_IN_US)     /1000)//round down
#define RU_ns2clk(ns)   ((((ns)*CLK_IN_US)+999)/1000)//round up
#define clk2fq(clk)     ((OPTION_HIGH_FREQ_CLK+((clk)>>1))/(clk))

typedef const struct {
    U16     LO_ns;      //fix
    U16     HI_ns;      //fix
    U16     HDDAT_ns;   //setable
    U16     SUDAT_ns;   //setable
    U8      SP_ns;      //setable
    bool    LH_isRatio2;//setable
}I2C_SPEC;

#define _I2C_MIN_FREQ   ((OPTION_HIGH_FREQ_CLK<=12000000)?11651:\
                         (OPTION_HIGH_FREQ_CLK<=16000000)?15534:\
                         (OPTION_HIGH_FREQ_CLK<=24000000)?23256:\
                         (OPTION_HIGH_FREQ_CLK<=32000000)?31008:\
                         (OPTION_HIGH_FREQ_CLK<=40000000)?38685:\
                         (OPTION_HIGH_FREQ_CLK<=48000000)?46422:100000)

//----------------------------------------------------------------------------//
// I2C SetClockSpeed
// Parameter:
//            _freq:  _I2C_MIN_FREQ ~ 1M, unit(Hz)
//            _mode:  I2C_STANDARD_MODE(support ~100KHz)
//                    I2C_FAST_MODE(support ~400KHz)
//                    I2C_FAST_MODE_PLUS(support ~1MHz)
//                    0xFF(auto slect by _freq)
// return value:
//            EXIT_SUCCESS / EXIT_FAILURE
//----------------------------------------------------------------------------//
U8   I2C_SetClock(U32 _freq, I2C_SPEED_MODE _mode) { //(M/S)
    static const I2C_SPEC i2cPara[num_I2C_SPEED_MODE] = {
        {   //STANDARD_MODE
            .HDDAT_ns    = 300,     // 0~3450 in Spec
            .SUDAT_ns    = 250,     // 250~   in Spec
            .SP_ns       = 50,      // 0~     in Spec
            .LH_isRatio2 = false,   // true or false(default)
            .LO_ns       = 4700,    // 4700~  in Spec
            .HI_ns       = 4000,    // 4000~  in Spec
        },
        {   //FAST_MODE
            .HDDAT_ns    = 300,     // 0~900  in Spec
            .SUDAT_ns    = 100,     // 100~   in Spec
            .SP_ns       = 50,      // 0~50   in Spec
            .LH_isRatio2 = true,    // true(default) or false
            .LO_ns       = 1300,    // 1300~  in Spec
            .HI_ns       = 600,     // 600~   in Spec
        },
        {   //FAST_MODE_PLUS
            .HDDAT_ns    = 0,       // 0~450  in Spec
            .SUDAT_ns    = 50,      // 50~    in Spec
            .SP_ns       = 50,      // 0~50   in Spec
            .LH_isRatio2 = true,    // true(default) or false
            .LO_ns       = 500,     // 500~   in Spec
            .HI_ns       = 260,     // 260~   in Spec
        },
    };
    if(_freq>1000000) return EXIT_FAILURE;
    if(_mode==I2C_STANDARD_MODE && _freq>100000) return EXIT_FAILURE;
    if(_mode==I2C_FAST_MODE && _freq>400000) return EXIT_FAILURE;
    
    _freq = (_freq<_I2C_MIN_FREQ)?_I2C_MIN_FREQ:_freq;
    if(_mode>=num_I2C_SPEED_MODE) {
        _mode =  (_freq<=100000)?I2C_STANDARD_MODE:
                 (_freq<=400000)?I2C_FAST_MODE:
                 I2C_FAST_MODE_PLUS;
    }
    const I2C_SPEC *pPara = &(i2cPara[_mode]);
    
    U32 _sp = RD_ns2clk(pPara->SP_ns);
    if(_sp > 0x7) return EXIT_FAILURE;
    U32 _realSp=_sp+4;
    
    U32 _hdd = RD_ns2clk(pPara->HDDAT_ns);
    _hdd = _hdd>_realSp?(_hdd-_realSp):0;
    _hdd = (!_hdd)?1:_hdd;
    if(_hdd > 0x1F) return EXIT_FAILURE;
    
    U32 _sud = RU_ns2clk(pPara->SUDAT_ns);
    // _sud = _sud>_realSp?(_sud-_realSp):0; // without correct for ATCIIC100 Slave ACK bug
    if(_sud > 0x1F) return EXIT_FAILURE;
    
    U32 _lhRatio = (pPara->LH_isRatio2)?true:false;
    
    U32 _hi = OPTION_HIGH_FREQ_CLK;
    U32 _temp = (2*_realSp)*_freq;
    _hi = (_hi>_temp)?(_hi-_temp):0;
    _temp = _freq * (_lhRatio+2);
    _hi = (_hi+_temp-1)/_temp;
    if(_hi > 0x1FF || _hi<=_sp) return EXIT_FAILURE;
    
    U32 _realHi = (_hi +_realSp);
    U32 _realLo = _realHi + _lhRatio*_hi;
    if(_realLo < RD_ns2clk(pPara->LO_ns)) return EXIT_FAILURE;
    if(_realHi < RD_ns2clk(pPara->HI_ns)) return EXIT_FAILURE;

    // dprintf("Success:FQ(%d),SP(%d),HDD(%d),SUD(%d),HI(%d),LO(%d),RealFQ(%d)\r\n",
        // _freq, _sp, _hdd, _sud, _realHi, _realLo, clk2fq(_realLo+_realHi) );
    U32 _set =   ((_sud&0x1F) <<24)
                |((_sp &0x7)  <<21)
                |((_hdd&0x1F) <<16)
                |((_lhRatio)  <<13)
                |((_hi &0x1FF)<< 4);
    P_I2C->Ctrl = ((P_I2C->Ctrl&~((0x1FFF<<16)|(0x3FF<<4)))|_set);
    return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------//
// I2C Set Master Mode
// Parameter:
//            mode: I2C_MASTER / I2C_SLAVE
// return value:
//            NONE
//----------------------------------------------------------------------------//
void I2C_SetDeviceMode(U32 mode) {
    if(mode == I2C_MASTER) {
        P_I2C->Ctrl= (P_I2C->Ctrl&~C_I2C_Mode)|C_I2C_Mode_Master;
    } else {
        P_I2C->Ctrl= (P_I2C->Ctrl&~C_I2C_Mode)|C_I2C_Mode_Slave;
    }
}

//----------------------------------------------------------------------------//
// I2C is Master mode?
// Parameter:
//            NONE
// return value:
//            true / false
//----------------------------------------------------------------------------//
bool I2C_IsMasterMode(void) {
    return (P_I2C->Ctrl & C_I2C_Mode_Master)>0;
}

//----------------------------------------------------------------------------//
// I2C Cmd
// Parameter:
//            cmd:  ENABLE / DISABLE
// return value:
//            NONE
//----------------------------------------------------------------------------//
void I2C_Cmd(U8 cmd) {
    if(cmd==DISABLE) {
        P_I2C->Ctrl&=~C_I2C_En;
    } else {
        P_I2C->Ctrl|=C_I2C_En;
    }
}

#if _I2C_MODULE
//----------------------------------------------------------------------------//
// I2C Init
// Parameter:
//            NONE
// return value:
//            NONE
//----------------------------------------------------------------------------//
void I2C_Init(void) {
    SMU_PeriphClkCmd(SMU_PERIPH_I2C,ENABLE);
    GPIO_Init(GPIOA,I2C_SDA_MTF_PIN,GPIO_MODE_ALT);
#if _EF_SERIES
    GPIO_Init(GPIOA,I2C_SCL_MTF_PIN,GPIO_MODE_ALT);
#endif
    I2C_SetDeviceMode(_I2C_DEVICE_MODE);
#if _I2C_DEVICE_MODE == I2C_MASTER
    I2C_SetInterruptSource(0);
#else
    I2C_SetAddr(_I2C_SLAVE_ADDRESS_ID, _I2C_SLAVE_ADDRESS_Is10Bit);
    I2C_SetInterruptSource(C_I2C_BYTE_Rx_IEn);//default disable Auto-Ack
#endif
#ifndef _I2C_CLOCK_SPEED
#define _I2C_CLOCK_SPEED _I2C_MIN_FREQ
#endif
    if(I2C_SetClock(_I2C_CLOCK_SPEED,0xFF)==EXIT_SUCCESS) {
        I2C_Cmd(ENABLE);
    }
}

#if _I2C_DEVICE_MODE == I2C_MASTER
//----------------------------------------------------------------------------//
// Wait I2C Transmission Completion(support START / ADDR / TX / STOP)
// Parameter:
//            NONE
// return value:
//            I2C_TRANS_SUCCESS, I2C_ERR_ARB_LOSE, ...
//----------------------------------------------------------------------------//
static S32 _waitComplete(void)
{
#if _I2C_TIMEOUT_MS>0
    U32 timeChk = SysTick;
#endif
    S32 ret;
    while(1) {
        if( I2C_IsCmpl() ) {
            ret = I2C_TRANS_SUCCESS;
            break;
        }
        if( I2C_IsArbLose() ) {
            ret = I2C_ERR_ARB_LOSE;
            break;
        }
#if _I2C_TIMEOUT_MS>0
        if((SysTick-timeChk)>(_I2C_TIMEOUT_MS+1)) {
            ret = I2C_ERR_TIMEOUT;
            break;
        }
#endif
    }
    return ret;
}

//----------------------------------------------------------------------------//
// Wait I2C Byte Received(support RX)
// Parameter:
//            NONE
// return value:
//            I2C_TRANS_SUCCESS, I2C_ERR_ARB_LOSE, ...
//----------------------------------------------------------------------------//
static S32 _waitByteRecv(void)
{
#if _I2C_TIMEOUT_MS>0
    U32 timeChk = SysTick;
#endif
    S32 ret;
    while(1) {
        if( I2C_IsByteRecv() ) {
            ret = I2C_TRANS_SUCCESS;
            break;
        }
        if( I2C_IsArbLose() ) {
            ret = I2C_ERR_ARB_LOSE;
            break;
        }
#if _I2C_TIMEOUT_MS>0
        if((SysTick-timeChk)>(_I2C_TIMEOUT_MS+1)) {
            ret = I2C_ERR_TIMEOUT;
            break;
        }
#endif
    }
    return ret;
}

//----------------------------------------------------------------------------//
// Error Handler(Force Reset all controller)
// Parameter:
//            NONE
// return value:
//            NONE
//----------------------------------------------------------------------------//
void I2C_ErrorHandler(void) {
    I2C_SetInterruptSource(0);
    I2C_RstController();
}

//----------------------------------------------------------------------------//
// I2C Set START condition
// Parameter:
//            NONE
// return value:
//            I2C_TRANS_SUCCESS, I2C_ERR_ARB_LOSE, ...
//----------------------------------------------------------------------------//
S32 I2C_Start(void) { //(M)
    I2C_SetControlMode( C_I2C_PHA_Start, 0 , I2C_DIR_TX);
    I2C_ClrCmplSts();
    I2C_ClrArbLoseSts();
    I2C_IssueDataTransaction();
    S32 ret = _waitComplete();
    return ret;
}

//----------------------------------------------------------------------------//
// I2C Set START condition with Addr
// Parameter:
//            _deviceAddr: 0x0~0x3FF
//            _isAddr10bit: true / false (Address format)
//            _dir: I2C_DIR_TX / I2C_DIR_RX
// return value:
//            I2C_TRANS_SUCCESS, I2C_ERR_ARB_LOSE, ...
//----------------------------------------------------------------------------//
S32 I2C_StartAddr(U32 _deviceAddr, bool _isAddr10bit, U32 _dir) { //(M)
    I2C_SetAddr(_deviceAddr, _isAddr10bit);
    I2C_SetControlMode( C_I2C_PHA_Start|C_I2C_PHA_Addr, 0 , _dir);
    I2C_ClrCmplSts();
    I2C_ClrArbLoseSts();
    I2C_IssueDataTransaction();
    S32 ret = _waitComplete();
    ret = (ret==I2C_TRANS_SUCCESS && !I2C_IsAddrHit())?I2C_ERR_NACK:ret; //chk hitAddr
    return ret;
}

//----------------------------------------------------------------------------//
// I2C Set STOP condition
// Parameter:
//            NONE
// return value:
//            I2C_TRANS_SUCCESS, I2C_ERR_ARB_LOSE, ...
//----------------------------------------------------------------------------//
S32 I2C_Stop(void) { //(M)
    if( I2C_IsByteRecv() ) {
        U8 _temp;
        I2C_Read(&_temp,false);
    }
    I2C_SetInterruptSource(0);
    I2C_SetControlMode(C_I2C_PHA_Stop, 0, I2C_DIR_TX);
    I2C_ClrCmplSts();
    I2C_ClrArbLoseSts();
    I2C_IssueDataTransaction();
    S32 ret = _waitComplete();
    I2C_RstController();
    return ret;
}

//----------------------------------------------------------------------------//
// I2C Write data
// Parameter:
//            *pData: Data pointer
// return value:
//            I2C_TRANS_SUCCESS, I2C_ERR_ARB_LOSE, ...
//----------------------------------------------------------------------------//
S32 I2C_Write(void *pData) { //(M)
    if(pData==NULL) return I2C_ERR_OTHER;
    
    U8 *data = (U8*)pData;
    I2C_PushData(*data);
    I2C_SetControlMode(C_I2C_PHA_Data, 1, I2C_DIR_TX);
    I2C_ClrCmplSts();
    I2C_ClrArbLoseSts();
    I2C_IssueDataTransaction();
    S32 ret = _waitComplete();//wait cmpl&&ACK / timeout
    ret = (ret==I2C_TRANS_SUCCESS && !I2C_IsAck()) ? I2C_ERR_NACK : ret; //chk hitAddr
    I2C_ClrFifo();
    I2C_ClrByteTransSts();
    return ret;
}

//----------------------------------------------------------------------------//
// I2C Read data
// Parameter:
//            *pData: Data pointer
//            _isAck: true / false , after read data
// return value:
//            I2C_TRANS_SUCCESS, I2C_ERR_ARB_LOSE, ...
//----------------------------------------------------------------------------//
S32 I2C_Read(void *pData, bool _isAck) { //(M)
    if(pData==NULL) return I2C_ERR_OTHER;
    
    U8 *data = (U8*)pData;
    S32 ret;//wait ByteRecv / timeout
    if( !I2C_IsByteRecv() ) { //RX Init
        I2C_SetInterruptSource(C_I2C_BYTE_Rx_IEn);//disable Auto-Ack
        I2C_SetControlMode(C_I2C_PHA_Data, 0, I2C_DIR_RX);
        I2C_ClrByteRecvSts();
        I2C_ClrArbLoseSts();
        I2C_IssueDataTransaction();
        ret = _waitByteRecv();//wait ByteRecv / timeout
        if(ret!=I2C_TRANS_SUCCESS) return ret;
    }
    
    *data = I2C_GetData();
    if(_isAck) {
        I2C_ClrByteRecvSts();
        I2C_ClrArbLoseSts();
        I2C_RespondACK();
        ret = _waitByteRecv();//wait ByteRecv / timeout
    } else {
        I2C_ClrByteRecvSts();
        I2C_ClrCmplSts();
        I2C_ClrArbLoseSts();
        I2C_RespondNACK();
        ret = _waitComplete();//wait Complete / timeout
        I2C_SetInterruptSource(0);
    }
    return ret;
}

//----------------------------------------------------------------------------//
// I2C Write EEPROM
// Parameter:
//            _deviceAddr: EEPROM Address, only support 7-bits
//            _memAddr: EEPROM memory address
//            _memAddrLen: 0~4, 0 will discard _memAddr
//            *pWrBuf: Data pointer
//            _wrlen: 1~n
// return value:
//            I2C_TRANS_SUCCESS, I2C_ERR_ARB_LOSE, ...
//----------------------------------------------------------------------------//
S32 I2C_WriteMultiByte_EEPROM(U16 _deviceAddr,U32 _memAddr,U8 _memAddrLen,U8 *pWrBuf,U32 _wrlen) {
    if(_memAddrLen>4 || _wrlen==0 || pWrBuf==NULL) return I2C_ERR_OTHER;
    
    S32 ret = I2C_StartAddr(_deviceAddr,false,I2C_DIR_TX);
    if(ret != I2C_TRANS_SUCCESS) {
        I2C_ErrorHandler();
        return ret;
    }
    
    _memAddr = __builtin_bswap32(_memAddr)>>((4-_memAddrLen)*8);
    while(_memAddrLen--) {
        ret = I2C_Write(&_memAddr);
        if(ret != I2C_TRANS_SUCCESS) {
            I2C_ErrorHandler();
            return ret;
        }
        _memAddr>>=8;
    }
    
    do {
        ret = I2C_Write(pWrBuf);
        if(ret != I2C_TRANS_SUCCESS) {
            I2C_ErrorHandler();
            return ret;
        }
        pWrBuf++;
    } while(--_wrlen);
    
    ret = I2C_Stop();
    return ret;
}

//----------------------------------------------------------------------------//
// I2C Write EEPROM
// Parameter:
//            _deviceAddr: EEPROM Address, only support 7-bits
//            _memAddr: EEPROM memory address
//            _memAddrLen: 0~4, 0 will discard _memAddr
//            *pRdBuf: Data pointer
//            _rdlen: 1~n
// return value:
//            I2C_TRANS_SUCCESS, I2C_ERR_ARB_LOSE, ...
//----------------------------------------------------------------------------//
S32 I2C_ReadMultiByte_EEPROM(U16 _deviceAddr,U32 _memAddr,U8 _memAddrLen,U8 *pRdBuf,U32 _rdlen) {
    if(_memAddrLen>4 || _rdlen==0 || pRdBuf==NULL) return I2C_ERR_OTHER;
    
    S32 ret = I2C_StartAddr(_deviceAddr,false,I2C_DIR_TX);
    if(ret != I2C_TRANS_SUCCESS) {
        I2C_ErrorHandler();
        return ret;
    }
    
    _memAddr = __builtin_bswap32(_memAddr)>>((4-_memAddrLen)*8);
    while(_memAddrLen--) {
        ret = I2C_Write(&_memAddr);
        if(ret != I2C_TRANS_SUCCESS) {
            I2C_ErrorHandler();
            return ret;
        }
        _memAddr>>=8;
    }
    
    ret = I2C_StartAddr(_deviceAddr,false,I2C_DIR_RX); //reStart RX
    if(ret != I2C_TRANS_SUCCESS) {
        I2C_ErrorHandler();
        return ret;
    }
    
    do {
        --_rdlen;
        ret = I2C_Read(pRdBuf,(_rdlen>0));
        if(ret != I2C_TRANS_SUCCESS) {
            I2C_ErrorHandler();
            return ret;
        }
        pRdBuf++;
    } while(_rdlen);
    
    ret = I2C_Stop();
    return ret;
}

#ifndef _I2C_MASTER_ADDRESS
#define _I2C_MASTER_ADDRESS 0x50 //compact with old API
#endif
#ifndef _I2C_ADDRESS_LENGTH
#define _I2C_ADDRESS_LENGTH TWO_BYTE //compact with old API
#endif
//----------------------------------------------------------------------------//
// Write Multiple byte(old API)
// Parameter: 
//            addr : 
//            len  : 1~Page Write Buffer of EEPROM
//            data : write data
// return value:
//          I2C_TRANS_SUCCESS, I2C_ERR_ARB_LOSE, ...
//----------------------------------------------------------------------------//
S32 I2C_WriteMultiByte(U16 addr, U16 len, U8* data)
{
    return I2C_WriteMultiByte_EEPROM(_I2C_MASTER_ADDRESS,addr,_I2C_ADDRESS_LENGTH,data,len);
}
//----------------------------------------------------------------------------//
// Read Multiple byte(old API)
// Parameter: 
//            addr : 
//            len  : 1~256
//            data : read data
// return value:
//          I2C_TRANS_SUCCESS, I2C_ERR_ARB_LOSE, ...
//----------------------------------------------------------------------------//
S32 I2C_ReadMultiByte(U16 addr, U16 len, U8* data)
{
    return I2C_ReadMultiByte_EEPROM(_I2C_MASTER_ADDRESS,addr,_I2C_ADDRESS_LENGTH,data,len);
}
#undef  _I2C_MASTER_ADDRESS
#undef  _I2C_ADDRESS_LENGTH

#else//I2C_SLAVE
//----------------------------------------------------------------------------//
// Slave mode Write Byte
// Parameter: 
//          NONE
// return value:
//          NONE
//----------------------------------------------------------------------------//
static void I2C_SlaveWrite(void)
{
    U8 data=0xFF;
    if( CB_I2C_SlaveWriteData(&data)!=EXIT_SUCCESS ) {
        I2C_PushData(0xFF);
    } else {
        I2C_PushData(data);
    }
}
//----------------------------------------------------------------------------//
// Slave mode Read Byte
// Parameter: 
//          NONE
// return value:
//          NONE
//----------------------------------------------------------------------------//
static void I2C_SlaveRead(void)
{
    U8 data = I2C_GetData();
    if( CB_I2C_SlaveReadData(&data)!=EXIT_SUCCESS ) {
        I2C_RespondNACK();
    } else {
        I2C_RespondACK();
    }
}

enum {
    kI2C_BUS_STOP=0,
    kI2C_BUS_READ,
    kI2C_BUS_WRITE,
    kI2C_BUS_RESET,
    num_I2C_SLAVE_STATE
} mI2C_SlaveState = kI2C_BUS_STOP;
//----------------------------------------------------------------------------//
// Slave mode serviceloop
// Parameter: 
//          NONE
// return value:
//          NONE
//----------------------------------------------------------------------------//
void I2C_SlaveServiceLoop(void)
{
    volatile U32 sts = I2C_GetAllSts(); //read once to avoid race condition
    switch(mI2C_SlaveState) {
        case kI2C_BUS_STOP:
            if( (sts & C_I2C_ADDR_HIT_Flag)>0 ) {
                CB_I2C_SlaveHitAddress();
                if( I2C_IsDirTx() ) {
                    mI2C_SlaveState = kI2C_BUS_WRITE;
                    I2C_SlaveWrite(); // 1st byte write
                } else {
                    mI2C_SlaveState = kI2C_BUS_READ;
                }
            }
            break;
        case kI2C_BUS_WRITE:
            if( (sts & C_I2C_STOP_Flag)>0 ) {
                if( (sts & C_I2C_CMPL_Flag)>0 ) mI2C_SlaveState = kI2C_BUS_RESET;
                break;
            }
            if( (sts & C_I2C_BYTE_Tx_Flag)>0 && (sts & C_I2C_ACK)>0 ) {
                I2C_ClrByteTransSts();
                I2C_SlaveWrite();
            }
            break;
        case kI2C_BUS_READ:
            if( (sts & C_I2C_STOP_Flag)>0 ) {
                if( (sts & C_I2C_CMPL_Flag)>0 ) mI2C_SlaveState = kI2C_BUS_RESET;
                break;
            }
            if( (sts & C_I2C_BYTE_Rx_Flag)>0 ) {
                I2C_ClrByteRecvSts();
                I2C_SlaveRead();
            }
            break;
        case kI2C_BUS_RESET:
        default:
            mI2C_SlaveState = kI2C_BUS_STOP;
            CB_I2C_SlaveTransactionEnd();
            I2C_ClrFifo();
            I2C_ClrAllSts();
            break;
    }
}
#endif
#endif //_I2C_MODULE
#endif //defined(P_I2C)