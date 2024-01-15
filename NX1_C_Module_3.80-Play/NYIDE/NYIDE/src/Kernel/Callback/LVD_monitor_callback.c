#include "include.h"
//#include "debug.h"
#if _SYS_LVD_MONITOR_MODULE
#if _QCODE_BUILD
#else
//------------------------------------------------------------------//
// Callback function when Vdd higher than LVD max level
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
static void CB_LVD_Monitor_MAX()
{

}
//------------------------------------------------------------------//
// Callback function when detect Vdd under 3.6V
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
static void CB_LVD_Monitor_3V6()
{

}
//------------------------------------------------------------------//
// Callback function when detect Vdd under 3.4V
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
static void CB_LVD_Monitor_3V4()
{

}
//------------------------------------------------------------------//
// Callback function when detect Vdd under 3.2V
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
static void CB_LVD_Monitor_3V2()
{

}

#if _EF_SERIES
//------------------------------------------------------------------//
// Callback function when detect Vdd under 3.0V(only NX1_EF)
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
static void CB_LVD_Monitor_3V0()
{

}
#endif

#if _EF_SERIES
//------------------------------------------------------------------//
// Callback function when detect Vdd under 2.8V(only NX1_EF)
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
static void CB_LVD_Monitor_2V8()
{

}
#endif

#if !_EF_SERIES
//------------------------------------------------------------------//
// Callback function when detect Vdd under 2.6V(only NX1_OTP)
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
static void CB_LVD_Monitor_2V6()
{

}
#endif

//------------------------------------------------------------------//
// Callback function when detect Vdd under 2.4V
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
static void CB_LVD_Monitor_2V4()
{

}

//------------------------------------------------------------------//
// Callback function when detect Vdd under 2.2V
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
static void CB_LVD_Monitor_2V2()
{

}

#if _EF_SERIES
//------------------------------------------------------------------//
// Callback function when detect Vdd under 2.0V(only NX1_EF)
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
static void CB_LVD_Monitor_2V0()
{

}
#endif


#if _EF_SERIES
static void (* const CB_funcTable[(_LVD_MONITOR_LEVEL+1)])(void) = {
    CB_LVD_Monitor_2V0,
    CB_LVD_Monitor_2V2,
    CB_LVD_Monitor_2V4,
    CB_LVD_Monitor_2V8,
    CB_LVD_Monitor_3V0,
    CB_LVD_Monitor_3V2,
    CB_LVD_Monitor_3V4,
    CB_LVD_Monitor_3V6,
    CB_LVD_Monitor_MAX,
};
#else
static void (* const CB_funcTable[(_LVD_MONITOR_LEVEL+1)])(void) = {
    CB_LVD_Monitor_2V2,
    CB_LVD_Monitor_2V4,
    CB_LVD_Monitor_2V6,
    CB_LVD_Monitor_3V2,
    CB_LVD_Monitor_3V4,
    CB_LVD_Monitor_3V6,
    CB_LVD_Monitor_MAX,
};
#endif
#endif//#if _QCODE_BUILD

//------------------------------------------------------------------//
// Callback function when LVD event happens
// Parameter: 
//          _lvdLevel: 	(0)not ready 1st scan
//						(1~(_LVD_MONITOR_LEVEL+1)) LVD level
// return value:
//          NONE
//------------------------------------------------------------------//
void CB_LVD_Monitor_Event(U8 _lvdLevel)
{
    //if(!_lvdLevel)return;
#if _QCODE_BUILD
	QC_LVD_setLevel(_lvdLevel);
#else
    CB_funcTable[(_lvdLevel-1)]();
#endif
}

#endif//#if _SYS_LVD_MONITOR_MODULE