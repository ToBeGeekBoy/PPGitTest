#include "include.h"
#if _SW_PWM_MODULE
//------------------------------------------------------------------//
// Callback fucntion when QIO Mark happens
// Parameter: 
//          id: mark id from 1~255
// return value:
//          NONE
//------------------------------------------------------------------// 
U8 CB_SW_PWM_GetCmd(U32* cmd) {
#if _QIO_MODULE
    return QIO_SW_PWM_GetCmd(cmd);
#else
    return EXIT_FAILURE;
#endif
}
#endif