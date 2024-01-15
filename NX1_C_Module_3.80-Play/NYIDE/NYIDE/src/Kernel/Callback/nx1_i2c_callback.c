#include "nx1_i2c.h"
#include "include.h"
#include <string.h>
#include "_queueTemplate.h"
#if defined(P_I2C) && _I2C_MODULE && (_I2C_DEVICE_MODE == I2C_SLAVE)

#if defined(_I2C_SLAVE_TEST_QUEUE)
#if _I2C_SLAVE_TEST_QUEUE<4
#error "_I2C_SLAVE_TEST_QUEUE need greater then or equal to 4."
#endif
_queueDefine(U8,_I2C_SLAVE_TEST_QUEUE,I2C_SLAVE_TEST);
#endif
//----------------------------------------------------------------------------//
// I2C_Slave Hit Address
// Parameter: 
//            NONE
// return value:
//            NONE
//----------------------------------------------------------------------------//
void CB_I2C_SlaveHitAddress(void)
{

}
//----------------------------------------------------------------------------//
// I2C_Slave Transaction End
// Parameter: 
//            NONE
// return value:
//            NONE
//----------------------------------------------------------------------------//
void CB_I2C_SlaveTransactionEnd(void)
{

}
//----------------------------------------------------------------------------//
// I2C_Slave Write data to HW FIFO. (Memory -> I2C -> Bus)
// Parameter: 
//            data : push data by reference pointer
// return value:
//            0:     Success
//            other: Failure
//----------------------------------------------------------------------------//
S32 CB_I2C_SlaveWriteData(U8* data)
{
#if defined(_I2C_SLAVE_TEST_QUEUE)
    return (_queueGetData(I2C_SLAVE_TEST,data)!=EXIT_SUCCESS)?(-1):0;
#endif
#if _QCODE_BUILD
    return QC_I2C_SlaveWrite(data);
#endif
    return -1;
}

//----------------------------------------------------------------------------//
// I2C_Slave Read data from HW FIFO. (Bus -> I2C -> Memory)
// Parameter: 
//            data : get data by reference pointer
// return value:
//            0:     Success, ready to get next data.
//            other: Failure, can not get data anymore.
//----------------------------------------------------------------------------//
S32 CB_I2C_SlaveReadData(U8* data)
{
#if defined(_I2C_SLAVE_TEST_QUEUE)
    _queuePutData(I2C_SLAVE_TEST,data); //check full after put
    return (_queueIsFull(I2C_SLAVE_TEST))?(-1):0;
#endif
#if _QCODE_BUILD
    return QC_I2C_SlaveRead(data);
#endif
    return -1;
}

#endif