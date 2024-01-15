#ifndef NX1_I2C_H
#define NX1_I2C_H

#include "nx1_reg.h"
#if     defined(P_I2C)
//////////////////////////////////////////////////////////////////////
// Constant Define
//////////////////////////////////////////////////////////////////////
#define  I2C_TRANS_SUCCESS          (0)
#define  I2C_FINISH                 I2C_TRANS_SUCCESS
#define  I2C_ERR_ARB_LOSE           (-1)
#define  I2C_ERR_NACK               (-2)
#define  I2C_ERR_TIMEOUT            (-3)
#define  I2C_ERR_OTHER              (-127)
typedef enum {
    I2C_STANDARD_MODE=0,
    I2C_FAST_MODE,
    I2C_FAST_MODE_PLUS,
    num_I2C_SPEED_MODE
}I2C_SPEED_MODE;

//////////////////////////////////////////////////////////////////////
// External Function
//////////////////////////////////////////////////////////////////////
extern U8   I2C_GetFifoSize(void);
extern void I2C_SetInterruptSource(U32 _src);
extern U32  I2C_GetInterruptSource(void);
extern void I2C_Int_ReEn(void);
extern bool I2C_IsSdaHigh(void);
extern bool I2C_IsSclHigh(void);
extern bool I2C_IsGenCall(void);
extern bool I2C_IsBusBusy(void);
extern bool I2C_IsAck(void);
extern bool I2C_IsCmpl(void);
extern bool I2C_IsByteRecv(void);
extern bool I2C_IsByteTrans(void);
extern bool I2C_IsStart(void);
extern bool I2C_IsStop(void);
extern bool I2C_IsArbLose(void);
extern bool I2C_IsAddrHit(void);
extern bool I2C_IsFifoHalf(void);
extern bool I2C_IsFifoFull(void);
extern bool I2C_IsFifoEmpty(void);
extern void I2C_ClrAllSts(void);
extern void I2C_ClrCmplSts(void);
extern void I2C_ClrByteRecvSts(void);
extern void I2C_ClrByteTransSts(void);
extern void I2C_ClrStartSts(void);
extern void I2C_ClrStopSts(void);
extern void I2C_ClrArbLoseSts(void);
extern void I2C_ClrAddrHitSts(void);
extern void I2C_SetAddr(const U32 _addr, const bool _isAddr10bit);
extern void I2C_PushData(const U8 _data);
extern U8   I2C_GetData(void);
extern void I2C_SetControlMode(U32 _phase, U32 _dataCnt, U32 _dir);
extern U8   I2C_GetDataCnt(void);
extern void I2C_ClrDataCnt(void);
extern bool I2C_IsDirTx(void);
extern void I2C_IssueDataTransaction(void);
extern bool I2C_IsDataTransaction(void);
extern void I2C_RespondACK(void);
extern void I2C_RespondNACK(void);
extern void I2C_ClrFifo(void);
extern void I2C_RstController(void);
extern U8   I2C_SetClock(U32 _freq, I2C_SPEED_MODE _mode);
extern void I2C_SetDeviceMode(U32 mode);
extern bool I2C_IsMasterMode(void);
extern void I2C_Cmd(U8 cmd);

#if _I2C_MODULE
extern void I2C_Init(void);
#if _I2C_DEVICE_MODE == I2C_MASTER
extern void I2C_ErrorHandler(void);
extern S32  I2C_Start(void);
extern S32  I2C_StartAddr(U32 _deviceAddr, bool _isAddr10bit, U32 _dir);
extern S32  I2C_Stop(void);
extern S32  I2C_Write(void *pData);
extern S32  I2C_Read(void *pData, bool _isAck);
extern S32  I2C_WriteMultiByte_EEPROM(U16 _deviceAddr,U32 _memAddr,U8 _memAddrLen,U8 *pWrBuf,U32 _wrlen);
extern S32  I2C_ReadMultiByte_EEPROM( U16 _deviceAddr,U32 _memAddr,U8 _memAddrLen,U8 *pRdBuf,U32 _rdlen);
extern S32  I2C_WriteMultiByte(U16 addr, U16 len, U8* data); // old API
extern S32  I2C_ReadMultiByte(U16 addr, U16 len, U8* data); // old API
#else
extern void I2C_SlaveServiceLoop(void);
extern void CB_I2C_SlaveHitAddress(void);
extern void CB_I2C_SlaveTransactionEnd(void);
extern S32  CB_I2C_SlaveWriteData(U8* data);
extern S32  CB_I2C_SlaveReadData(U8* data);
#endif
#endif

#endif//#if     defined(P_I2C)
#endif//#ifndef NX1_I2C_H
