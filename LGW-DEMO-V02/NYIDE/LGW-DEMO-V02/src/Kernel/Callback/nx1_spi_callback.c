/* Includes ------------------------------------------------------------------*/
#include "include.h"


//------------------------------------------------------------------//
// SPI0 block erase timeout
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void CB_SPI0_BlockErase_Timeout(void)
{ 
	// User defined code.
#if _QCODE_BUILD && _SPI_MODULE && _SPI_ERASE_TIMEOUT_PATH
    QC_DIRECT_CALLBACK(QC_SpiEraseTimeoutPath);
#endif
}
//------------------------------------------------------------------//
// SPI0 sector erase timeout
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void CB_SPI0_SectorErase_Timeout(void)
{ 
	// User defined code.
#if _QCODE_BUILD && _SPI_MODULE && _SPI_ERASE_TIMEOUT_PATH
    QC_DIRECT_CALLBACK(QC_SpiEraseTimeoutPath);
#endif
}
//------------------------------------------------------------------//
// SPI1 block erase timeout
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void CB_SPI1_BlockErase_Timeout(void)
{ 
	// User defined code.
#if _QCODE_BUILD && _SPI1_MODULE && _SPI1_USE_FLASH && _SPI_ERASE_TIMEOUT_PATH
    QC_DIRECT_CALLBACK(QC_SpiEraseTimeoutPath);
#endif
}
//------------------------------------------------------------------//
// SPI1 sector erase timeout
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void CB_SPI1_SectorErase_Timeout(void)
{ 
	// User defined code.
#if _QCODE_BUILD && _SPI1_MODULE && _SPI1_USE_FLASH && _SPI_ERASE_TIMEOUT_PATH
    QC_DIRECT_CALLBACK(QC_SpiEraseTimeoutPath);
#endif
}

