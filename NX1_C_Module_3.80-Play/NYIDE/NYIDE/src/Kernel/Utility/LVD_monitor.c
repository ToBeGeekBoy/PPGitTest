#include "include.h"
#include "nx1_smu.h"
//#include "debug.h"
#if _SYS_LVD_MONITOR_MODULE

#ifndef _LVD_MONITOR_BOUNCING_TIMES
#define _LVD_MONITOR_BOUNCING_TIMES 50//0~1000
#endif

#ifndef _LVD_MONITOR_POWER_ON_INIT
#define _LVD_MONITOR_POWER_ON_INIT  ENABLE
#endif

typedef struct{
#if _LVD_MONITOR_BOUNCING_TIMES>0
    U16 u16BouncingCount;
    U8  u8PreviousLevel;
#endif
    U8  u8StableLevel;
    U8  u8TempSysTick;
    U8  u8WaitState;//0:wait , 1:sample LVD
}S_lvdMonitor;

#if _EF_SERIES
static const U16 kLVD_VoltageTab[_LVD_MONITOR_LEVEL] = {
    C_LVD_2V0,
    C_LVD_2V2,
    C_LVD_2V4,
    C_LVD_2V8,
    C_LVD_3V0,
    C_LVD_3V2,
    C_LVD_3V4,
    C_LVD_3V6,
};
#else
static const U16 kLVD_VoltageTab[_LVD_MONITOR_LEVEL] = {
    C_LVD_2V2,
    C_LVD_2V4,
    C_LVD_2V6,
    C_LVD_3V2,
    C_LVD_3V4,
    C_LVD_3V6,
};
#endif

static S_lvdMonitor s_ctrl;

//------------------------------------------------------------------//
// LVD scan process
// Parameter: 
//          NONE
// return value:
//          0:no result
//          1~(_LVD_MONITOR_LEVEL+1)
//------------------------------------------------------------------//
static U8  _scan()
{
    static U8 _scanLevel = 0;
    U8 _sampleLevel=0;
	//step 1. check flag and set next voltage
    if(SMU_GetLVDSts()) {
        _sampleLevel = _scanLevel;
    } else {
        ++_scanLevel;
        if(_scanLevel >= _LVD_MONITOR_LEVEL) {
            _sampleLevel = _LVD_MONITOR_LEVEL;  //max level
        } else {
            SMU_LVDCmd(ENABLE,kLVD_VoltageTab[_scanLevel]);
            return 0;//no result, scan next level
        }
    }
    _scanLevel = 0;
    SMU_LVDCmd(ENABLE,kLVD_VoltageTab[0]);//reset min level
    
    ++_sampleLevel;//remap to 1~(_LVD_MONITOR_LEVEL+1), compact with NY5Q
    
    return _sampleLevel;
}

//------------------------------------------------------------------//
// LVD Monitor init function
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void LVD_Monitor_Init()
{
    //Init LVD lowest setting
    SMU_LVDCmd(ENABLE,kLVD_VoltageTab[0]);
    
    //force scan, get init value
    U8 _sampleLevel = 0;
#if _LVD_MONITOR_POWER_ON_INIT
    do {
        _sampleLevel = _scan();
    } while(!_sampleLevel);
#endif
    
    //Init variable
    s_ctrl = (S_lvdMonitor){
#if _LVD_MONITOR_BOUNCING_TIMES>0
        .u16BouncingCount = 0,
        .u8PreviousLevel = _sampleLevel,
#endif
        .u8StableLevel = _sampleLevel,
        .u8TempSysTick = (SysTick&0xFF),
        .u8WaitState = 0,
    };
    
#if _LVD_MONITOR_POWER_ON_INIT
    CB_LVD_Monitor_Event(_sampleLevel);
#endif
}

//------------------------------------------------------------------//
// LVD Monitor serviceloop
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void LVD_Monitor_ServiceLoop()
{
    U8 _holdSysTick,_sampleLevel;
#if _LVD_MONITOR_BOUNCING_TIMES>0
    U8 _peekPrev;
#endif
    switch(s_ctrl.u8WaitState) {
        case 0:
            _holdSysTick = SysTick&0xFF;//sample hold
            if(_holdSysTick==s_ctrl.u8TempSysTick) break;
            ++s_ctrl.u8TempSysTick;
            s_ctrl.u8WaitState=1;//fallthrough
        case 1:
            //step 1. check flag and set next voltage
            _sampleLevel = _scan();
            if(!_sampleLevel) break;//no result
            s_ctrl.u8WaitState=0;//LVD sample success
            
            //step 2. if get volgate -> debouncing
#if _LVD_MONITOR_BOUNCING_TIMES>0
            _peekPrev = s_ctrl.u8PreviousLevel;
            s_ctrl.u8PreviousLevel = _sampleLevel;
            if(_sampleLevel==s_ctrl.u8StableLevel) {
                s_ctrl.u16BouncingCount = 0;
                break;
            }
            if(_sampleLevel!=_peekPrev) {
                s_ctrl.u16BouncingCount = _LVD_MONITOR_BOUNCING_TIMES;
                break;
            }
            if(s_ctrl.u16BouncingCount>1) {
                --s_ctrl.u16BouncingCount;
                break;
            }
            s_ctrl.u16BouncingCount=0;
#else
            if(_sampleLevel == s_ctrl.u8StableLevel) break; 
#endif
            s_ctrl.u8StableLevel = _sampleLevel;
            //dprintf("LVD_Level:%d\r\n",_sampleLevel);
            
            //step 3. if debounce checked -> event output to callback
            CB_LVD_Monitor_Event(_sampleLevel);
            break;
        default:
            break;
    }
}

//------------------------------------------------------------------//
// LVD Monitor Sleep process(init scan state)
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void LVD_Monitor_Sleep()
{
    U8 _sampleLevel=0;
    do {
        _sampleLevel=_scan();
    } while(!_sampleLevel);
#if _LVD_MONITOR_BOUNCING_TIMES>0
    s_ctrl.u16BouncingCount=0;
    s_ctrl.u8PreviousLevel=s_ctrl.u8StableLevel;
#endif
    s_ctrl.u8WaitState=0;
}

//------------------------------------------------------------------//
// LVD Monitor Wakeup(Force Scan LVD wo/debounce)
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void LVD_Monitor_WakeUp()
{
    U8 _sampleLevel=0;
    do {
        _sampleLevel=_scan();
    } while(!_sampleLevel);
    
    if(_sampleLevel!=s_ctrl.u8StableLevel) {
        s_ctrl.u8StableLevel=_sampleLevel;
#if _LVD_MONITOR_BOUNCING_TIMES>0
        s_ctrl.u8PreviousLevel=s_ctrl.u8StableLevel;
#endif
        CB_LVD_Monitor_Event(_sampleLevel);
    }
}

#endif//#if _SYS_LVD_MONITOR_MODULE