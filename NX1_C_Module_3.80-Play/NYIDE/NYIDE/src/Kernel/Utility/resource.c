/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "include.h"
#include "nx1_spi.h"
#if _EF_SERIES
#include "UserDefinedTab.h"
#endif

/* Defines -------------------------------------------------------------------*/
#define RESOURCE_DATA_TABLE_OFFSET  10//Ref. base information w/SPI_Data_Format
#define _IDX_TO_TYPE(a)             (2+0+(a)*5)//input 0~N
#define _IDX_TO_ADDR(a)             (2+1+(a)*5)//input 0~N
#define _IDX_TO_ALIGN(a)            (4+0+(a)*6)//input 0~N
#define _IDX_TO_LEN(a)              (4+2+(a)*6)//input 0~N
/* Static Variable -----------------------------------------------------------*/
#if defined(_USER_DEFINED_NONVOLATILE_PARAMETER)
typedef struct {
    U32 base;
    U32 offset;
    U16 count;
} _nonvolatile_para_t;
#if _SPI_MODULE
    static _nonvolatile_para_t _spi0Para = {
        .base = 0,
        .offset = 0,
        .count = 0,
    };
#endif
#if (_SPI1_MODULE && _SPI1_USE_FLASH)
    static _nonvolatile_para_t _spi1Para = {
        .base = 0,
        .offset = 0,
        .count = 0,
    };
#endif
#endif//#if defined(_USER_DEFINED_NONVOLATILE_PARAMETER)

#if (_SPI1_MODULE && _SPI1_USE_FLASH)\
    ||_SPI_MODULE\
    ||_EF_SERIES
#if defined(_USER_DEFINED_NONVOLATILE_PARAMETER)
static U32 _getNonvolatileOffset(U8 _mode)
{
    switch(_mode) {
#if _SPI_MODULE
        //case SPI_UD_MODE:
        case SPI_MODE:
            return _spi0Para.offset;
#endif
#if (_SPI1_MODULE && _SPI1_USE_FLASH)
        //case SPI1_UD_MODE:
        case SPI1_MODE:
            return _spi1Para.offset;
#endif
        default:
            return 0;
    }
}
static void _setNonvolatileOffset(U32 _offset, U8 _mode)
{
    switch(_mode) {
#if _SPI_MODULE
        //case SPI_UD_MODE:
        case SPI_MODE:
            _spi0Para.offset = _offset;
            break;
#endif
#if (_SPI1_MODULE && _SPI1_USE_FLASH)
        //case SPI1_UD_MODE:
        case SPI1_MODE:
            _spi1Para.offset = _offset;
            break;
#endif
        default:
            break;
    }
}
static U32 _getNonvolatileBase(U8 _mode)
{
    switch(_mode) {
#if _SPI_MODULE
        //case SPI_UD_MODE:
        case SPI_MODE:
            return _spi0Para.base;
#endif
#if (_SPI1_MODULE && _SPI1_USE_FLASH)
        //case SPI1_UD_MODE:
        case SPI1_MODE:
            return _spi1Para.base;
#endif
        default:
            return 0;
    }
}
static void _setNonvolatileBase(U32 _base, U8 _mode)
{
    switch(_mode) {
#if _SPI_MODULE
        //case SPI_UD_MODE:
        case SPI_MODE:
            _spi0Para.base = _base;
            break;
#endif
#if (_SPI1_MODULE && _SPI1_USE_FLASH)
        //case SPI1_UD_MODE:
        case SPI1_MODE:
            _spi1Para.base = _base;
            break;
#endif
        default:
            break;
    }
}
static U16 _getNonvolatileCount(U8 _mode)
{
    switch(_mode) {
#if _SPI_MODULE
        //case SPI_UD_MODE:
        case SPI_MODE:
            return _spi0Para.count;
#endif
#if (_SPI1_MODULE && _SPI1_USE_FLASH)
        //case SPI1_UD_MODE:
        case SPI1_MODE:
            return _spi1Para.count;
#endif
        default:
            return 0;
    }
}
static void _setNonvolatileCount(U16 _count, U8 _mode)
{
    switch(_mode) {
#if _SPI_MODULE
        //case SPI_UD_MODE:
        case SPI_MODE:
            _spi0Para.count = _count;
            break;
#endif
#if (_SPI1_MODULE && _SPI1_USE_FLASH)
        //case SPI1_UD_MODE:
        case SPI1_MODE:
            _spi1Para.count = _count;
            break;
#endif
        default:
            break;
    }
}
#endif//#if defined(_USER_DEFINED_NONVOLATILE_PARAMETER)
//------------------------------------------------------------------//
// Check accessible by _mode
// Parameter: 
//          _mode:  SPI_MODE / SPI1_MODE / EF_MODE/ EF_UD_MODE
// return value:
//          true/false
//------------------------------------------------------------------//
static bool _isAccessible(U8 _mode)
{
#if !defined(_USER_DEFINED_ACCESSIBLE_CHECK)
#define _USER_DEFINED_ACCESSIBLE_CHECK  ENABLE
#endif
#if _USER_DEFINED_ACCESSIBLE_CHECK
    switch(_mode) {
#if _EF_SERIES
        case EF_UD_MODE:
        case EF_MODE:
            return true;
#endif
#if _SPI_MODULE
        //case SPI_UD_MODE:
        case SPI_MODE:
            return !(SPI_isBusBusy() || SPI_CheckBusyFlag());
#endif
#if (_SPI1_MODULE && _SPI1_USE_FLASH)
        //case SPI1_UD_MODE:
        case SPI1_MODE:
            return !(SPI1_isBusBusy() || SPI1_CheckBusyFlag());
#endif
        default:
            return false;
    }
#else
    return true;
#endif
}
//------------------------------------------------------------------//
// MemoryAccess by _mode
// Parameter: 
//          _mode:  SPI_MODE / SPI1_MODE / EF_MODE/ EF_UD_MODE
// return value:
//          _buf
//------------------------------------------------------------------//
static void _accessMem(U8* _buf, U32 _addr, U32 _size, U8 _mode)
{
    switch(_mode) {
#if _EF_SERIES
        case EF_UD_MODE:
        case EF_MODE:
            memcpy((void*)_buf,(void*)_addr,_size);
            break;
#endif
#if _SPI_MODULE
        //case SPI_UD_MODE:
        case SPI_MODE:
            SPI_BurstRead(_buf,_addr,_size);
            break;
#endif
#if (_SPI1_MODULE && _SPI1_USE_FLASH)
        //case SPI1_UD_MODE:
        case SPI1_MODE:
            SPI1_BurstRead(_buf,_addr,_size);
            break;
#endif
        default:
            //memset((void*)_buf,(void*)0,_size);
            break;//error
    }
}
//------------------------------------------------------------------//
// Get Info offset from start address
// Parameter: 
//          _baseAddr:base address
//          _mode:  SPI_MODE / SPI1_MODE / EF_MODE/ EF_UD_MODE
// return value:
//          return: offset from base
//------------------------------------------------------------------//
static U32 _getResourceOffset(U32 _baseAddr, U8 _mode)
{
    U32 _tempbuf=0;
    U32 _resourceOffset=0;
    switch(_mode) {
#if _EF_SERIES
        case EF_UD_MODE:
        case EF_MODE:
            _tempbuf=((U32*)_baseAddr)[2];
            if(_tempbuf && (~_tempbuf)) {//not 0 or 0xffffffff
                _resourceOffset=(((U32*)_baseAddr)[1]==_tempbuf)?0:((U32*)_baseAddr)[1];
            }
            break;
#endif
#if (_SPI1_MODULE && _SPI1_USE_FLASH)\
    ||_SPI_MODULE
        //case SPI_UD_MODE:
        case SPI_MODE:
        //case SPI1_UD_MODE:
        case SPI1_MODE:
#if defined(_USER_DEFINED_NONVOLATILE_PARAMETER)
            _resourceOffset=_getNonvolatileOffset(_mode);
            if(_resourceOffset!=0) break;
#endif
            _accessMem((U8*)&_tempbuf,_baseAddr+RESOURCE_DATA_TABLE_OFFSET,sizeof(_tempbuf),_mode);
            if(_tempbuf) {
                _resourceOffset=_tempbuf;
            }
#if defined(_USER_DEFINED_NONVOLATILE_PARAMETER)
            _setNonvolatileOffset(_resourceOffset,_mode);
#endif
            break;
#endif
        default:
            break;
    }
    return _resourceOffset;
}
//------------------------------------------------------------------//
// Get Base address
// Parameter: 
//          _mode:  SPI_MODE / SPI1_MODE / EF_MODE/ EF_UD_MODE
// return value:
//          return: address, failure return 0
//------------------------------------------------------------------//
static U32 _getBaseAddr(U8 _mode)
{
    U32 _addr=0;
    switch(_mode) {
#if _EF_SERIES
        case EF_UD_MODE:
        case EF_MODE:
            _addr=(U32)UPDATEABLE_RESOURCES_DATA;//must exist when use EF series
            break;
#endif
#if _SPI_MODULE
        //case SPI_UD_MODE:
        case SPI_MODE:
#if defined(_USER_DEFINED_NONVOLATILE_PARAMETER)
            _addr=_getNonvolatileBase(_mode);
            if(_addr!=0) break;
#endif
            _addr=SpiOffset();
#if defined(_USER_DEFINED_NONVOLATILE_PARAMETER)
            _setNonvolatileBase(_addr,_mode);
#endif
            break;
#endif
#if (_SPI1_MODULE && _SPI1_USE_FLASH)
        //case SPI1_UD_MODE:
        case SPI1_MODE:
#if defined(_USER_DEFINED_NONVOLATILE_PARAMETER)
            _addr=_getNonvolatileBase(_mode);
            if(_addr!=0) break;
#endif
            _addr=Spi1_Offset();
#if defined(_USER_DEFINED_NONVOLATILE_PARAMETER)
            _setNonvolatileBase(_addr,_mode);
#endif
            break;
#endif
        default:
            break;
    }
    return _addr;
}
#endif
//------------------------------------------------------------------//
// Reset nonVolatile parameter
// Parameter: 
//          _mode:  SPI_MODE / SPI1_MODE / EF_MODE/ EF_UD_MODE
// return value:
//          None
//------------------------------------------------------------------//
void UserDefinedData_ResetNonVolatilePara(U8 _mode)
{
    switch(_mode) {
#if defined(_USER_DEFINED_NONVOLATILE_PARAMETER)
#if _SPI_MODULE
        //case SPI_UD_MODE:
        case SPI_MODE:
            memset(&_spi0Para,0,sizeof(_spi0Para));
            break;
#endif
#if (_SPI1_MODULE && _SPI1_USE_FLASH)
        //case SPI1_UD_MODE:
        case SPI1_MODE:
            memset(&_spi1Para,0,sizeof(_spi1Para));
            break;
#endif
#endif
        default: //do nothing
            break;
    }
}
//------------------------------------------------------------------//
// Get Resource Count
// Parameter: 
//          _mode:  SPI_MODE / SPI1_MODE / EF_MODE/ EF_UD_MODE
// return value:
//          return: the number of resource count
//------------------------------------------------------------------//
U16 UserDefinedData_GetCount(U8 _mode)
{
#if (_SPI1_MODULE && _SPI1_USE_FLASH)\
    ||_SPI_MODULE\
    ||_EF_SERIES
#if defined(_USER_DEFINED_NONVOLATILE_PARAMETER)
    U16 _nonvolatileCnt = _getNonvolatileCount(_mode);
    if(_nonvolatileCnt!=0) return _nonvolatileCnt;
#endif
    if(_isAccessible(_mode)==false) return 0;
    U32 _baseAddr=_getBaseAddr(_mode);
    if(!_baseAddr) return 0;
    U32 _infoAddr=_baseAddr+_getResourceOffset(_baseAddr,_mode);
    if(_baseAddr==_infoAddr)return 0;
    
    U16 _resourceCnt=0;
    _accessMem((U8*)&_resourceCnt,_infoAddr,sizeof(_resourceCnt),_mode);
#if defined(_USER_DEFINED_NONVOLATILE_PARAMETER)
    _setNonvolatileCount(_resourceCnt,_mode);
#endif
    return _resourceCnt;
#else
    return 0;
#endif
}
//------------------------------------------------------------------//
// Get Resource Address
// Parameter: 
//          _idx:   resource index, it is from 0 ~ UserDefinedData_GetCount()-1
//          _mode:  SPI_MODE / SPI1_MODE / EF_MODE/ EF_UD_MODE
// return value:
//          return: the address of resource count, 0 is fail data
//------------------------------------------------------------------//
U32 UserDefinedData_GetAddress(U16 _idx, U8 _mode)
{
#if (_SPI1_MODULE && _SPI1_USE_FLASH)\
    ||_SPI_MODULE\
    ||_EF_SERIES
    if(_isAccessible(_mode)==false) return 0;
    U16 _cnt=UserDefinedData_GetCount(_mode);
    if(_idx>=_cnt) return 0;
    U32 _baseAddr=_getBaseAddr(_mode);
    U32 _infoAddr=_baseAddr+_getResourceOffset(_baseAddr,_mode);
    
    U32 _resourceAddr=0;
    _accessMem((U8*)&_resourceAddr,(_infoAddr+_IDX_TO_ADDR(_idx)),sizeof(_resourceAddr),_mode);
    return (_baseAddr+_resourceAddr);
#else
    return 0;
#endif
}
//------------------------------------------------------------------//
// Get Resource Type
// Parameter: 
//          _mode:  SPI_MODE / SPI1_MODE / EF_MODE/ EF_UD_MODE
//          _idx:   resource index, it is from 0 ~ UserDefinedData_GetCount()-1
// return value:
//          return: resource type
//                  0(UserDefine)
//                  1(CVR for SPI Mode)
//                  2(Record for Q-Code)
//                  3(VoiceTrigger for Q-Code)
//                  4(LED String for Q-Code)
//                  5~251(Reserved)
//                  252(EF_Action)
//                  253(EF_MIDI)
//                  254(EF_Voice)
//                  255(Reserved)
//------------------------------------------------------------------//
U8  UserDefinedData_GetType(U16 _idx, U8 _mode)
{
#if (_SPI1_MODULE && _SPI1_USE_FLASH)\
    ||_SPI_MODULE\
    ||_EF_SERIES
    if(_isAccessible(_mode)==false) return 0xFF;
    U16 _cnt=UserDefinedData_GetCount(_mode);
    if(_idx>=_cnt) return 0xFF;
    U32 _baseAddr=_getBaseAddr(_mode);
    U32 _infoAddr=_baseAddr+_getResourceOffset(_baseAddr,_mode);
    
    U8 _resourceType=0xFF;
    U32 _addr=(_infoAddr+_IDX_TO_TYPE(_idx));
    _accessMem((U8*)&_resourceType,_addr,sizeof(_resourceType),_mode);
    return _resourceType;
#else
    return 0xFF;
#endif
}

//------------------------------------------------------------------//
// Get Resource Alignment
// Parameter: 
//          _mode:  SPI_MODE / SPI1_MODE / EF_MODE/ EF_UD_MODE
//          _idx:   resource index, it is from 0 ~ UserDefinedData_GetCount()-1
// return value:
//          return: the Alignment of resource count, 
//                  0x0     (Alignment 1 Byte)
//                  ...
//                  0x1FF   (Alignment 512 Byte)
//                  ...
//                  0xFFF   (Alignment 4096 Byte)
//                  ...
//                  0xFFFF  (Alignment 65536 Byte)
//------------------------------------------------------------------//
U16 UserDefinedData_GetAlignment(U16 _idx, U8 _mode)
{
#if (_SPI1_MODULE && _SPI1_USE_FLASH)\
    ||_SPI_MODULE\
    ||_EF_SERIES
    if(_isAccessible(_mode)==false) return 0;
    U16 _cnt=UserDefinedData_GetCount(_mode);
    if(_idx>=_cnt) return 0;
    
    U32 _addr = UserDefinedData_GetAddress(_idx,_mode);
    if(!_addr) return 0;
    U16 _nextIdx = _idx+1;
    U32 _nextAddr = (_nextIdx<_cnt) ? UserDefinedData_GetAddress(_nextIdx,_mode):
                                     (UserDefinedData_GetLength(_idx,_mode)+_addr);
    if(!_nextAddr) return 0;
    
    _addr = _addr&(-_addr);
    _nextAddr = _nextAddr&(-_nextAddr);
    _addr = (_addr<_nextAddr)?_addr:_nextAddr; //get min
    return (U16)(_addr-1);
#else
    return 0;
#endif
}

//------------------------------------------------------------------//
// Get Resource Section Length (not data length)
// Parameter: 
//          _mode:  SPI_MODE / SPI1_MODE / EF_MODE/ EF_UD_MODE
//          _idx:   resource index, it is from 0 ~ UserDefinedData_GetCount()-1
// return value:
//          return: the Length of resource count, 0 is fail data
//------------------------------------------------------------------//
U32 UserDefinedData_GetLength(U16 _idx, U8 _mode)
{
#if (_SPI1_MODULE && _SPI1_USE_FLASH)\
    ||_SPI_MODULE\
    ||_EF_SERIES
    if(_isAccessible(_mode)==false) return 0;
    U16 _cnt=UserDefinedData_GetCount(_mode);
    if(_idx>=_cnt) return 0;
    U32 _baseAddr=_getBaseAddr(_mode);
    U32 _infoAddr=_baseAddr+_getResourceOffset(_baseAddr,_mode);
    
    U32 _resourceLen=0;
    U16 _nextIdx = _idx+1;
    if(_nextIdx<_cnt) {
        _resourceLen  = UserDefinedData_GetAddress(_nextIdx,_mode);
        _resourceLen -= UserDefinedData_GetAddress(_idx,_mode);
    } else { //Last section
        U32 _addr=(_infoAddr+_IDX_TO_TYPE(_cnt));//base on _IDX_TO_TYPE(_cnt)
        _accessMem((U8*)&_resourceLen,_addr,sizeof(_resourceLen),_mode);
    }
    return _resourceLen;
#else
    return 0;
#endif
}

//------------------------------------------------------------------//
// Get Resource Address use type, we will find the "Nth" type data.
// Parameter: 
//          _idx:   resource index, it is from 0 ~ UserDefinedData_GetCount()-1
//          _mode:  SPI_MODE / SPI1_MODE / EF_MODE/ EF_UD_MODE
//          _type:  ref.UDR_APP_TYPE
// return value:
//          return: the index of resource count, 0xFFFF is fail data
//------------------------------------------------------------------//
U16 UserDefinedData_GetIndexUseType(U16 _idx, UDR_APP_TYPE _type, U8 _mode)
{
#if (_SPI1_MODULE && _SPI1_USE_FLASH)\
    ||_SPI_MODULE\
    ||_EF_SERIES
    if(_isAccessible(_mode)==false) return 0xffff;
    U16 _cnt=UserDefinedData_GetCount(_mode);
    if(_idx>=_cnt) return 0xffff;
    U32 _baseAddr=_getBaseAddr(_mode);
    U32 _infoAddr=_baseAddr+_getResourceOffset(_baseAddr,_mode);
    U8 _tempType=0xFF;
#if 1 //defined(_USER_DEFINED_PARSE_TYPE_USE_BINARY_SEARCH)
    // step.1 binary search
    U32 _left = 0,_right = _cnt-1,_mid;
    while(_left<_right) {
        _mid = (_left+_right)/2;
        _accessMem((U8*)&_tempType,(_infoAddr+_IDX_TO_TYPE(_mid)),sizeof(_tempType),_mode);
        if(_tempType<_type) {
            _left = _mid+1;
        } else {
            _right = _mid;
        }
    }
    // step.2 shift _idx from _left and check type
    _left+=_idx;
    if(_left>=_cnt) return 0xffff;
    _accessMem((U8*)&_tempType,(_infoAddr+_IDX_TO_TYPE(_left)),sizeof(_tempType),_mode);
    return (_tempType!=_type)?0xffff:(U16)_left;
#else
    U32 _resourceIndex=0xffff;
    U16 i;
    for(i=0;i<_cnt;++i) {
        _accessMem((U8*)&_tempType,(_infoAddr+_IDX_TO_TYPE(i)),sizeof(_tempType),_mode);
        if(_tempType==_type) {
            if(!_idx) {
                _resourceIndex = i;
                break;
            }
            --_idx;
        }
    }
    return _resourceIndex;
#endif
#else
    return 0xffff;
#endif
}
//------------------------------------------------------------------//
// Get Resource Address use type, we will find the "Nth" type data.
// Parameter: 
//          _idx:   resource index, it is from 0 ~ UserDefinedData_GetCount()-1
//          _mode:  SPI_MODE / SPI1_MODE / EF_MODE/ EF_UD_MODE
//          _type:  ref.UDR_APP_TYPE
// return value:
//          return: the address of resource count, 0 is fail data
//------------------------------------------------------------------//
U32 UserDefinedData_GetAddressUseType(U16 _idx, UDR_APP_TYPE _type, U8 _mode)
{
#if (_SPI1_MODULE && _SPI1_USE_FLASH)\
    ||_SPI_MODULE\
    ||_EF_SERIES
    if(_isAccessible(_mode)==false) return 0;
    _idx=UserDefinedData_GetIndexUseType(_idx,_type,_mode);
    return UserDefinedData_GetAddress(_idx,_mode);
#else
    return 0;
#endif
}

//--------------------compact old interface--------------------//
//------------------------------------------------------------------//
// Get Resource Count(for EF)
// Parameter: 
//          NONE
// return value:
//          return: the number of resource count
//------------------------------------------------------------------//
U16 RESOURCE_EF_GetResourceCnt(void)
{
#if _EF_SERIES
    return UserDefinedData_GetCount(EF_MODE);
#else
    return 0;
#endif
}
//------------------------------------------------------------------//
// Get Resource Address(for EF)
// Parameter: 
//          idx: resource index, it is from 0.
// return value:
//          return: the address of resource count, 0 is fail data
//------------------------------------------------------------------//
U32 RESOURCE_EF_GetAddress(U16 _idx)
{
#if _EF_SERIES
    return UserDefinedData_GetAddress(_idx, EF_MODE);
#else
    return 0;
#endif
}
//------------------------------------------------------------------//
// Get Resource type(for EF)
// Parameter: 
//          idx: resource index, it is from 0.
// return value:
//          return: resource type
//                  0(UserDefine)
//                  1(CVR for SPI Mode)
//                  2(Record for Q-Code)
//                  3(VoiceTrigger for Q-Code)
//                  4(LED String for Q-Code)
//------------------------------------------------------------------//
U8 RESOURCE_EF_GetType(U16 _idx)
{
#if _EF_SERIES
    return UserDefinedData_GetType(_idx,EF_MODE);
#else
    return 0xFF;
#endif
}


//------------------------------------------------------------------//
// Get Resource Count
// Parameter: 
//          NONE
// return value:
//          return: the number of resource count
//------------------------------------------------------------------//
U16 RESOURCE_GetResourceCnt(void)
{
#if _SPI_MODULE 
    return UserDefinedData_GetCount(SPI_MODE);
#else
    return 0;
#endif
}   
//------------------------------------------------------------------//
// Get Resource Address
// Parameter: 
//          idx: resource index, it is from 0.
// return value:
//          return: the address of resource count
//------------------------------------------------------------------//
U32 RESOURCE_GetAddress(U16 _idx)
{
#if _SPI_MODULE 
    return UserDefinedData_GetAddress(_idx, SPI_MODE);
#else
    return 0;
#endif
} 

//------------------------------------------------------------------//
// Get Resource type
// Parameter: 
//          idx: resource index, it is from 0.
// return value:
//          return: resource type
//------------------------------------------------------------------//
U8 RESOURCE_GetType(U16 _idx)
{
#if _SPI_MODULE 
    return UserDefinedData_GetType(_idx,SPI_MODE);
#else
    return 0xFF;
#endif
}


//------------------------------------------------------------------//
// Get Resource1 Count
// Parameter: 
//          NONE
// return value:
//          return: the number of resource count
//------------------------------------------------------------------//
U16 RESOURCE1_GetResourceCnt(void)
{
#if _SPI1_MODULE && _SPI1_USE_FLASH
    return UserDefinedData_GetCount(SPI1_MODE);
#else
    return 0;
#endif
}   
//------------------------------------------------------------------//
// Get Resource1 Address
// Parameter: 
//          idx: resource index, it is from 0.
// return value:
//          return: the address of resource count
//------------------------------------------------------------------//
U32 RESOURCE1_GetAddress(U16 _idx)
{
#if _SPI1_MODULE && _SPI1_USE_FLASH
    return UserDefinedData_GetAddress(_idx, SPI1_MODE);
#else
    return 0;
#endif
} 

//------------------------------------------------------------------//
// Get Resource1 type
// Parameter: 
//          idx: resource index, it is from 0.
// return value:
//          return: resource type
//------------------------------------------------------------------//
U8 RESOURCE1_GetType(U16 _idx)
{
#if _SPI1_MODULE && _SPI1_USE_FLASH
    return UserDefinedData_GetType(_idx,SPI1_MODE);
#else
    return 0xFF;
#endif
}

