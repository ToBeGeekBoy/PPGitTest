#include "include.h"
#if _ACTION_MODULE
//------------------------------------------------------------------//
// Callback fucntion when Action Mark happens
// Parameter: 
//          id: mark id from 1~255
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_ActionMark(U8 id){
	// User defined code.
	// Code MUST be short and streamlined, or CPU performance will be degraded.
#if _QCODE_BUILD
	QC_Action_PushMark(id);
#endif

}
#endif
