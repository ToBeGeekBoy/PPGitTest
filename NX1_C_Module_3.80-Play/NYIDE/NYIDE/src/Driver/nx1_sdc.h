#ifndef NX1_SDC_H
#define NX1_SDC_H

#include "nx1_reg.h"
#if		defined(P_SDC)
//////////////////////////////////////////////////////////////////////
// Constant Define
//////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------//
//  Function   : SDCardBusy, SDCardReadBlock, SDCardWriteBlock, SDCardInit
//  Parameter  : Return Value
//------------------------------------------------------------------//
#define SD_OK                       0
#define SD_ERROR                    1
#define SD_BUSY                     1



//////////////////////////////////////////////////////////////////////
// External Function
//////////////////////////////////////////////////////////////////////

extern U8 SDCardBusy(void);   
extern U8 SDCardReadBlock(U32* buf,U32 block);
extern U8 SDCardWriteBlock(U32* buf,U32 block);
extern U8 SDCardInit(void);
extern U8 SDC_CheckWP(void);
extern U8 SDC_CheckCard(void);



#endif	//#if	defined(P_SDC)

#endif //#ifndef NX1_SDC_H
