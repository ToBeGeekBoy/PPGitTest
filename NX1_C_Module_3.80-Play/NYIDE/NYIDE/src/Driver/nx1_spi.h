#ifndef NX1_SPI_H
#define NX1_SPI_H

#include "nx1_reg.h"

//////////////////////////////////////////////////////////////////////
// Constant Define
//////////////////////////////////////////////////////////////////////
#if _EF_SERIES
#define SPI0_CS_PORT				GPIOB
#define SPI0_CS_PIN					1
#define SPI1_CS_PORT				GPIOA
#define SPI1_CS_PIN					12
#define SPI1_LDOSPI0_EN             ((_SPI1_ACCESS_MODE==SPI_1_1_1_MODE)||(_SPI1_ACCESS_MODE==SPI_1_1_1_MODE_3WIRE)||(_SPI1_ACCESS_MODE==SPI_1_2_2_MODE)||(_SPI1_ACCESS_MODE==SPI_1_1_2_MODE))
#else
#define SPI0_CS_PORT				GPIOB
#define SPI0_CS_PIN					0
#define SPI1_CS_PORT				GPIOB
#define SPI1_CS_PIN					8
#endif

//------------------------------------------------------------------//
//  Function   : SPI_SetBusMode
//  Parameter  : cmd_bus_mode, addr_bus_mode, data_bus_mode
//------------------------------------------------------------------//
#if _EF_SERIES && (_SPI_ACCESS_MODE==SPI_1_1_1_MODE_3WIRE)
#define SPI_DATA_1                  C_SPI_DataMode_1IO_3Wire
#else
#define SPI_DATA_1                  C_SPI_DataMode_1IO
#endif
#define SPI_DATA_2                  C_SPI_DataMode_2IO
#define SPI_DATA_4                  C_SPI_DataMode_4IO

#if _EF_SERIES
#if _SPI1_ACCESS_MODE==SPI_1_1_1_MODE_3WIRE
#define SPI1_DATA_1                 C_SPI_DataMode_1IO_3Wire
#else
#define SPI1_DATA_1                 C_SPI_DataMode_1IO
#endif
#define SPI1_DATA_2                 C_SPI_DataMode_2IO
#endif
                                    
//------------------------------------------------------------------//
//  Function   : SPI_Init
//  Parameter  : clk_divisor, _polarity, _sampling edge
//------------------------------------------------------------------//
#define SPI_CLKDIV_1                C_SPI_CLK_Div_1
#define SPI_CLKDIV_2                C_SPI_CLK_Div_2
#define SPI_CLKDIV_4                C_SPI_CLK_Div_4
#define SPI_CLKDIV_8                C_SPI_CLK_Div_8
#define SPI_CLKDIV_16               C_SPI_CLK_Div_16
#define SPI_CLKDIV_32               C_SPI_CLK_Div_32
#define SPI_CLKDIV_64               C_SPI_CLK_Div_64
#define SPI_CLKDIV_128              C_SPI_CLK_Div_128

#define SPI_Pol_High				C_SPI_Pol_High
#define SPI_Pol_Low					C_SPI_Pol_Low
#define SPI_Phase_1st_Edge			C_SPI_Phase_1st_Edge
#define SPI_Phase_2nd_Edge			C_SPI_Phase_2nd_Edge

//------------------------------------------------------------------//
//  Function   : None
//  Parameter  : SPI Size
//------------------------------------------------------------------//                                    
#if EF11FS_SERIES
#define SPI_FIFO_SIZE               0x4
#else
#define SPI_FIFO_SIZE               0x10
#endif
#define SPI1_FIFO_SIZE              0x08                                   
#define SPI_SECTOR_SIZE             0x1000
#define SPI_BLOCK_SIZE              0x10000
#define SPI_PAGE_SIZE               0x100

//------------------------------------------------------------------//
//  Function   : None
//  Parameter  : SPI address byte, reg. offset
//------------------------------------------------------------------// 
#define SPI_ADDR_BYTE               3                           
#define SPI_DATA_NUM_OFFSET         16

//------------------------------------------------------------------//
//  Function   : SPI_WriteStsReg
//  Parameter  : data
//------------------------------------------------------------------//                            
#define SPI_QE_BIT                  0x02

//------------------------------------------------------------------//
//  Function   : SPI_SendCmdAddr, SPI_SendCmd, SPI1_SendCmd, SPI1_SendCmdAddr
//  Parameter  : cmd
//------------------------------------------------------------------//                            
#define SPI_READ_CMD                0x03
#define SPI_PAGE_PRG_CMD            0x02
#define SPI_FAST_READ_CMD           0x80
#define SPI_2READ_CMD               0xBB
#define SPI_4READ_CMD               0xEB
#define SPI_QUAD_READ_CMD           0x6B
#define SPI_DUAL_READ_CMD           0x3B
#define SPI_WREN_CMD                0x06
#define SPI_WRDI_CMD                0x04
#define SPI_SECTOR_ERASE_CMD        0x20
#define SPI_BLOCK_ERASE_CMD         0xD8
#define SPI_CHIP_ERASE_CMD          0xC7
#define SPI_READ_STS_REG1_CMD       0x5
#define SPI_READ_STS_REG2_CMD       0x35
#define SPI_READ_STS_REG3_CMD       0x15 
#define SPI_WRITE_STS_REG1_CMD      0x1
#define SPI_WRITE_STS_REG2_CMD      0x31
#define SPI_WRITE_STS_REG3_CMD      0x11
#define SPI_READ_DEVID_CMD          0x90 
#define SPI_READ_MFID_DEVID_CMD     0x9F
#define SPI_EXIT_QPI_MODE_CMD       0xff
#define SPI_RESETEN_CMD             0x66
#define SPI_RESET_CMD               0x99
#define SPI_ENTER_4BYTE_ADDR_CMD    0xB7
#define SPI_EXIT_4BYTE_ADDR_CMD     0xE9
#define SPI_POWER_DOWN_CMD			0xB9
#define SPI_RELEASE_POWER_DOWN_CMD	0xAB


//------------------------------------------------------------------//
//  SPI Flash Manufacturer ID
//------------------------------------------------------------------//  
#define SPI_VENDOR_WINBOND	 		0xEF
#define SPI_VENDOR_GD				0xC8
#define SPI_VENDOR_MXIC     		0xC2
#define SPI_VENDOR_XMC	     		0x20
#define SPI_N25Q_20H				0x20
#define SPI_N25Q_1CH				0x1C
#define SPI_PUYA					0x85

//////////////////////////////////////////////////////////////////////
// External Function
//////////////////////////////////////////////////////////////////////

extern void SPI_Init(U32 divide,U8 pol, U8 pha);
extern void SPI_SetClockDivisor(U32 divide);
extern void SPI_SetClockPolarity(U32 pol);
extern void SPI_SetClockPhase(U32 pha);
extern void SPI_SendCmdAddr(U8 cmd,U32 addr,U8 dummy);
extern void SPI_SendData(U32 *data,U8 len);
extern void SPI_ReceiveData(void *buf,U16 len);
extern void SPI_SendCmd(U8 cmd);
extern void SPI_ByteCmd(U8 cmd);
extern void SPI_WriteFlashSet(U8 cmd, U32* p_Data, U8 len);
extern void SPI_ReadFlashSet(U8 cmd, U32* p_Data, U8 len);
extern void SPI_WRENCmd(U8 cmd);
extern void SPI_SetBusMode(U8 cmd_bus_mode,U8 addr_bus_mode,U8 data_bus_mode);
extern void SPI_SectorErase(U32 addr);
extern void SPI_BlockErase(U32 addr);
extern void SPI_ChipErase(void);
extern void SPI_BurstWrite(U8 *buf,U32 addr,U16 len);
extern void SPI_BurstErase(U32 addr,U32 len);
extern void SPI_BurstErase_Sector(U32 addr,U32 len);
extern void SPI_BurstRead(U8 *buf,U32 addr,U16 len);
extern void SPI_StartSectorErase(U32 addr);
extern void SPI_StartBlockErase(U32 addr);
extern void SPI_StartChipErase(void);
extern U8   SPI_CheckBusyFlag(void);
extern S8   SPI_WaitEraseDone(void);
extern void SPI_QECmd(U8 cmd);
extern void SPI_4ByteAddrCmd(U8 cmd);
extern void SPI_Reset(void);
extern void SPI_WriteStsReg(U8 reg,U32 data);
extern void SPI_WriteStsReg2(U32 data);
extern U32  SPI_ReadStsReg(U8 reg);
extern void SPI_XipAddrMode(U8 addr_mode);
extern void SPI_XipIoMode(U8 io_mode);
extern void SPI_XipRom2Bank(U8 bank);
extern U32 SPI_GetVendorID(void);
extern U8	SPI_4ByteAddressEnable(const U32 _JEDEC_ID);
extern U8	SPI_4ByteAddressDisable(const U32 _JEDEC_ID);
extern bool SPI_Is4ByteAddressSet(const U32 _JEDEC_ID);
extern U8	SPI_QEBitEnable(const U32 _JEDEC_ID);
extern U8	SPI_QEBitDisable(const U32 _JEDEC_ID);
extern bool SPI_IsQEBitSet(const U32 _JEDEC_ID);
extern void SPI_WRSR(U8 reg,U32 data, U8 len);
extern U32  SPI_RDSR(U8 reg);
extern void SPI_CS_On(void);
extern void SPI_CS_Off(void);
extern bool SPI_isBusBusy(void);
extern void SPI_TX(U32 _data);
extern U32  SPI_RX(void);
extern void SPI_WRD(U32 _addr, U32 _data);
extern U32  SPI_RDD(U32 _addr);
extern void SPI_WREN(void);
extern void SPI_WRDIS(void);
extern U32  SPI_RDID(void);
extern void SPI_CE(void);
extern void SPI_SE(U32 _addr);
extern void SPI_BE(U32 _addr);
extern void SPI_DP(void);
extern U32  SPI_RDP(void);
extern U32  SPI_GetAddr(U16 _index);
extern void SPI_FastReadStart(U32 addr);
extern void SPI_FastRead(U32 *buf,U32 len);
extern void SPI_FastReadStop(void);

#if	defined(P_SPI1)
extern void SPI1_Init(U32 divide, U8 pol, U8 pha);
extern void SPI1_SetClockDivisor(U32 divide);
extern void SPI1_SetClockPolarity(U32 pol);
extern void SPI1_SetClockPhase(U32 pha);
extern void SPI1_SendData(U32 *data,U8 len);
extern void SPI1_ReceiveData(void *buf,U16 len);
extern void SPI1_SendCmdAddr(U8 cmd,U32 addr,U8 dummy);
extern void SPI1_SendCmd(U8 cmd);
extern void SPI1_ByteCmd(U8 cmd);
extern void SPI1_WriteFlashSet(U8 cmd, U32* p_Data, U8 len);
extern void SPI1_ReadFlashSet(U8 cmd, U32* p_Data, U8 len);
extern void SPI1_WRENCmd(U8 cmd);
extern void SPI1_SetBusMode(U8 cmd_bus_mode,U8 addr_bus_mode,U8 data_bus_mode);
extern void SPI1_SectorErase(U32 addr);
extern void SPI1_BlockErase(U32 addr);
extern void SPI1_ChipErase(void);
extern void SPI1_Reset(void);
extern void SPI1_4ByteAddrCmd(U8 cmd);
extern void SPI1_BurstRead(U8 *buf,U32 addr,U16 len);
extern void SPI1_BurstErase(U32 addr,U32 len);
extern void SPI1_BurstErase_Sector(U32 addr,U32 len);
extern void SPI1_StartSectorErase(U32 addr);
extern void SPI1_StartBlockErase(U32 addr);
extern void SPI1_StartChipErase(void);
extern U8 SPI1_CheckBusyFlag(void);
extern void SPI1_BurstWrite(U8 *buf,U32 addr,U16 len);
extern void SPI1_WriteStsReg(U8 reg,U32 data);
extern void SPI1_WriteStsReg2(U32 data);
extern U32 SPI1_ReadStsReg(U8 reg);
extern U32 SPI1_GetVendorID(void);
extern U8	SPI1_4ByteAddressEnable(const U32 _JEDEC_ID);
extern U8	SPI1_4ByteAddressDisable(const U32 _JEDEC_ID);
extern bool SPI1_Is4ByteAddressSet(const U32 _JEDEC_ID);
extern void SPI1_WRSR(U8 reg,U32 data, U8 len);
extern U32  SPI1_RDSR(U8 reg);
extern void SPI1_CS_On(void);
extern void SPI1_CS_Off(void);
extern bool SPI1_isBusBusy(void);
extern void SPI1_TX(U32 _data);
extern U32  SPI1_RX(void);
extern void SPI1_WRD(U32 _addr, U32 _data);
extern U32  SPI1_RDD(U32 _addr);
extern void SPI1_WREN(void);
extern void SPI1_WRDIS(void);
extern U32  SPI1_RDID(void);
extern void SPI1_CE(void);
extern void SPI1_SE(U32 _addr);
extern void SPI1_BE(U32 _addr);
extern void SPI1_DP(void);
extern U32  SPI1_RDP(void);
extern U32  SPI1_GetAddr(U16 _index);
extern void SPI1_FastReadStart(U32 addr);
extern void SPI1_FastReadStop(void);
#endif	//#if	defined(P_SPI1)

#endif //#ifndef NX1_SPI_H
