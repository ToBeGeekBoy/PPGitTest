#ifndef NX1_FMC_H
#define NX1_FMC_H

#include "nx1_reg.h"
#if		defined(P_FMC)
//////////////////////////////////////////////////////////////////////
// Constant Define
//////////////////////////////////////////////////////////////////////
#define MAIN_FLASH_SECTORSIZE		512

#define MAIN_FLASH_WORDPROGRAM		1
#define MAIN_FLASH_SECTORERASE		2
#define DATA_TRIGGER				0

#define DATA_MASK					0xFFFFFFFF
#define DATA_ALIGN					4


//////////////////////////////////////////////////////////////////////
// External Function
//////////////////////////////////////////////////////////////////////
void FMC_BurstErase_Sector(U32 addr, U32 len);
void FMC_BurstRead(U8 *buf, U32 addr, U16 len);
void FMC_BurstWrite(U8 *buf, U32 addr, U16 len);

#endif	//#if	defined(P_FMC)
#endif	//#ifndef NX1_SPI_H
