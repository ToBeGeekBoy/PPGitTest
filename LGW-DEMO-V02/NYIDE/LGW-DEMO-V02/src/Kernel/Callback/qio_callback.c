#include "include.h"
#if _QIO_MODULE
//------------------------------------------------------------------//
// Callback fucntion when QIO Mark happens
// Parameter: 
//          id: mark id from 1~255
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_QIOMark(U8 id){
	// User defined code.
	// Code MUST be short and streamlined, or CPU performance will be degraded.
#if _QCODE_BUILD
	QC_QIO_PushMark(id);
#endif
	
}
#endif