#include "include.h"
#include "nx1_spi.h"
#include "nx1_wdt.h"
//#include "debug.h"
#if _EF_SERIES
#include "UserDefinedTab.h"
#endif

#ifndef _PROGRAM_CHECKSUM
#define _PROGRAM_CHECKSUM 1
#endif

#if _PROGRAM_CHECKSUM
const U16 program_checksum_val __attribute__ ((section (".prog_ck")));

// symbol export by linker. (see link script)
extern const U8 __text_section_start[];
extern const U8 __text_section_end[];
extern const U16 __program_checksum_addr[];

#if _EF_SERIES
extern const U8 __flashrw_section_start[];
extern const U8 __flashrw_section_end[];
#endif

#define CHECKSUM_BUF_SIZE		16

//------------------------------------------------------------------//
// Code Check Sum
// CAUTION: Maybe return fail checksum result when using Debug Mode on NX1 OTP series
// Parameter: 
//          NONE
// return value:
//          return: 1: Check pass 0: Check sum fail
//------------------------------------------------------------------//
U8 ProgChecksum(void)
{
    U16 ck_current = 0;
    const U8 *ptr = __text_section_start;
    
    while(ptr < __text_section_end)
    {
        ck_current += *ptr;
        ptr++;
    }
    return (ck_current == __program_checksum_addr[0]);
}
//------------------------------------------------------------------//
// SPI internal Check Sum Calculate
// Parameter: 
//          checkType: 1:SPI Data,  0: XIP
//			  offset1: address offset 1
// return value:
//          return: checksum value
//                  0xFFFFFFFF is no content.
//------------------------------------------------------------------//
U32 Checksum_Calculate(U8 checkType, U8 offset1)
{
    U8 tempbuf[CHECKSUM_BUF_SIZE], j, mod;
    U32 addr, length, i, checksum = 0;

	SPI_BurstRead(tempbuf,SPI_RESERVED_SIZE-offset1,4);
    length=tempbuf[0]|(tempbuf[1]<<8)|(tempbuf[2]<<16)|(tempbuf[3]<<24); 

    SPI_BurstRead(tempbuf,SPI_RESERVED_SIZE-(offset1-4),4);
    addr=tempbuf[0]|(tempbuf[1]<<8)|(tempbuf[2]<<16)|(tempbuf[3]<<24); 	

//	dprintf("length=%x, addr=%x\r\n", length, addr);
	
	for (i = 0; i < (length/CHECKSUM_BUF_SIZE); i++) {	
		SPI_BurstRead(tempbuf, addr + i*CHECKSUM_BUF_SIZE, CHECKSUM_BUF_SIZE);
		for (j = 0; j < CHECKSUM_BUF_SIZE; j++) {
			checksum += tempbuf[j];
		}
		WDT_Clear();
	}
	
	mod = length % CHECKSUM_BUF_SIZE;
	SPI_BurstRead(tempbuf, addr + i*CHECKSUM_BUF_SIZE, mod);
	for (j = 0; j < mod; j++) {
		checksum += tempbuf[j];
	}		


	if (checkType) {
		SPI_BurstRead(tempbuf, addr + (length-16) , 4);  // sum Tail section
		for (j = 0; j < 4; j++) {
			checksum -= tempbuf[j];
		}		
	}
	
	checksum &= 0xFFFFFF;	
//	dprintf("0.%x\r\n", checksum);
	return checksum;
}
//------------------------------------------------------------------//
// SPI internal Check Sum
// Parameter: 
//          checkType: 1:SPI Data,  0: XIP
//			  offset1: address offset 1
//			  offset2: address offset 2
// return value:
//          return: 1: Check pass,  0: Check sum fail
//------------------------------------------------------------------//
static U8 SPI_Checksum(U8 checkType, U8 offset1, U8 offset2)
{
    U8  tempbuf[CHECKSUM_BUF_SIZE];
    U32 i, checksum = 0;
	
	checksum = Checksum_Calculate(checkType,offset1);
//	dprintf("checksum %x, checkType %x\r\n", checksum, checkType);
	
	SPI_BurstRead(tempbuf,SPI_RESERVED_SIZE-offset2,3);
    i=tempbuf[0]|(tempbuf[1]<<8)|(tempbuf[2]<<16); 	

    SYS_ClrSleepTime();

//	dprintf("[CM]%x vs [NYIDE]%x \r\n", checksum,i);
	if (i != checksum) 
		return 0;
	
	return 1;
}
//------------------------------------------------------------------//
// SPI Check Sum
// Parameter: 
//          NONE
// return value:
//          return: 1: Check pass, 0: Check sum fail
//------------------------------------------------------------------//
U8 SPI_ProgChecksum(void)
{
	if (SPI_Checksum(1, 8, 48))					// SPI data checksum
		if (SPI_Checksum(0, 16, 51))			// XIP checksum
			return 1;
		
	return 0;
}
#endif // ifdef _PROGRAM_CHECKSUM
