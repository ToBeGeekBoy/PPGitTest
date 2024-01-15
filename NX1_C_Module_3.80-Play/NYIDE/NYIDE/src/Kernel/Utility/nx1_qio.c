#include <string.h>
#include "include.h"
#include "nx1_lib.h"
#include "nx1_spi.h"
#include "nx1_gpio.h"
#include "_queueTemplate.h"

#define		Type_QIO_Sync		(0x02)
#define		Type_QIO_Spec		(0x03)
#define		Type_QIO_Data		(0x01 | 0x80)
#define		Type_QIO_Data_wEnd	(0x01)
#define		Type_QIO_Mark		(0x00 | 0x80)
#define		Type_QIO_Mark_wEnd	(0x00)
#define		Type_QIO_End		(0x04)

#define		min(a,b)			(((a)<(b))?(a):(b))

#if _QIO_MODULE

#ifndef QIO_MARK_FIFOSIZE
#define QIO_MARK_FIFOSIZE   8
#endif
_queueDefine(U8,QIO_MARK_FIFOSIZE,QIO_MARK);

_queueDefine(U32,QIO_EVENT_FIFOSIZE,QIO_EVENT);

typedef struct
{
	U16 u16_OffsetSample;
	U8  u8_DataType;
	U8  u8_MarkID;
}S_QIO_Event __attribute__((aligned(4)));//FIFO use U32 to reduce process time.

typedef struct
{
	U32 u32_QIO_StartAddr;
	U32 u32_QIO_CurrAddr;
	U32 u32_QIO_DelayStopTick;
	U16 u16_OffsetSample;
	U8  u8_QIO_Version;
	U8  u8_QIO_Option;//bool
	U8  u8_QIO_GetDataFlag;//bool
	U8  u8_QIO_ReGetDataFlag;//bool
	U8  u8_QIO_DelayStopFlag;
	U8  u8_QIO_StorageMode;
	U8  u8_QIO_AudioResurce;
}S_QIO_CTRL;
S_QIO_CTRL				s_QIO_CTRL;

typedef	struct
{
    S_QIO_Event sEvent;
	U16         u16_OffsetSample;
	U8          u8_wEventFlag;
}S_QIO_DacIsrCTRL;
S_QIO_DacIsrCTRL		s_QIO_DacIsrCTRL;

#if QIO_FrameFlag_FIFOSIZE!=0
static  void    QIO_FrameFlag_FIFO_Init();
#endif

#if _QSW_PWM_MODULE
static  void    QSW_PWM_CMD_Init();
static  bool    QSW_PWM_CMD_FIFO_Is_Empty();
static  bool    QSW_PWM_CMD_FIFO_Is_Full();
static  bool    QSW_PWM_CMD_FIFO_Is_UnLock_END();
static  U8      QSW_PWM_CMD_FIFO_UnLock();
static  U8      QSW_PWM_CMD_FIFO_Get(U32 *data);
static  U8	    QSW_PWM_CMD_FIFO_Put(U32 *data);
static  U8      QSW_PWM_Duty_Set_wLock(U8 _CH, U8 _Duty, U16 _DiffMs);
#endif
//------------------------------------------------------------------//
// QIO Use Pin Set
// Parameter: 
//          _PinTab:	QIO Pin set table (GPIO_PA0 ~ GPIO_PC7)...Reference nx1_gpio.h
//			_PinTabSize:Table size(Pin number)
// return value:
//          EXIT_SUCCESS(0):Set success
//			EXIT_FAILURE(1):Set error
//------------------------------------------------------------------//
#if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)
U8	QIO_PinSet(const U8 _PinTab[][_QIO_PIN_DATA_SIZE], U8 _PinTabSize)
#else
U8	QIO_PinSet(const U8 _PinTab[], U8 _PinTabSize)
#endif//#if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)
{
	//set pin by pin table
	if(!_PinTab) return EXIT_FAILURE;//error setting
#if _QSW_PWM_MODULE
    U8 _CH = 0;
#if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)
	if(_PinTabSize > _SW_PWM_CH_NUM*_QIO_PIN_DATA_SIZE) {
		_PinTabSize = _SW_PWM_CH_NUM*_QIO_PIN_DATA_SIZE;
	}
	_PinTabSize /= _QIO_PIN_DATA_SIZE;
	for(_CH = 0 ; _CH < _PinTabSize ; _CH++) {
		_SW_PWM_CH_Set(_CH,_PinTab[_CH][0],_PinTab[_CH][1]);
		SW_PWM_CH_Start(_CH);
	}
#else
	if(_PinTabSize > _SW_PWM_CH_NUM) {
		_PinTabSize = _SW_PWM_CH_NUM;
	}
	for(_CH = 0 ; _CH < _PinTabSize ; _CH++) {
		_SW_PWM_CH_Set(_CH,_PinTab[_CH]);
        SW_PWM_CH_Start(_CH);
	}
#endif//#if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)
#endif
	return EXIT_SUCCESS;
}

//------------------------------------------------------------------//
// QIO Use Pin Set__by channel
// Parameter: 
//          _CH:		QIO Channel(0 ~ _SW_PWM_CH_NUM-1)
//			_Pin:		GPIO(GPIO_PA0 ~ GPIO_PC7)...Reference nx1_gpio.h
// return value:
//          EXIT_SUCCESS(0):Set success
//			EXIT_FAILURE(1):Set error
//------------------------------------------------------------------//
#if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)
U8	QIO_ChPinSet(U8 _CH, U8 _Pin, U8 _BusyLevel)
#else
U8	QIO_ChPinSet(U8 _CH, U8 _Pin)
#endif//#if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)
{
#if _QSW_PWM_MODULE
    if(_CH >= _SW_PWM_CH_NUM) return EXIT_FAILURE;
#if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)
	_SW_PWM_CH_Set(_CH,_Pin,_BusyLevel);//(CH,Pin,Busy,Init)
#else
	_SW_PWM_CH_Set(_CH,_Pin);
#endif
    SW_PWM_CH_Start(_CH);
#endif
	return EXIT_SUCCESS;
}

//------------------------------------------------------------------//
// QIO Data decode
// Parameter: 
//          _type:
//          _len:
//          pData:
// return value:
//          NONE
//------------------------------------------------------------------//
static void _decode(U8 _type, U8 _len, U8* pData)
{
    S_QIO_Event _tempEvent;
    switch(_type) {
        case Type_QIO_Spec:
            if(_len == 1) {
                s_QIO_CTRL.u8_QIO_Version = pData[0];
            }
            break;
        case Type_QIO_Sync:
            if(_len == 2) {
                s_QIO_CTRL.u16_OffsetSample = (U16)((pData[1]<<8) + pData[0]);
            }
            break;
#if _QSW_PWM_MODULE
        case Type_QIO_Data:
            if(_len == 4) {
                if(pData[0]<=_SW_PWM_CH_NUM) {
                    QSW_PWM_Duty_Set_wLock(
                        (pData[0]-1)
                        ,(pData[3])
                        ,(((U16)pData[2]<<8)+(U16)pData[1])
                    );//(Ch,Duty,DFms)
                    _tempEvent=(S_QIO_Event){
                        .u16_OffsetSample=s_QIO_CTRL.u16_OffsetSample,
                        .u8_DataType=Type_QIO_Data,
                        .u8_MarkID=0,
                    };
                    _queuePutData(QIO_EVENT,(U32*)&_tempEvent);
                }
            }
            break;
#endif
        case Type_QIO_Data_wEnd:
#if _QSW_PWM_MODULE
            if(_len == 4) {
                if(pData[0]<=_SW_PWM_CH_NUM) {
                    QSW_PWM_Duty_Set_wLock(
                        (pData[0]-1)
                        ,(pData[3])
                        ,(((U16)pData[2]<<8)+(U16)pData[1])
                    );//(Ch,Duty,DFms)
                    _tempEvent=(S_QIO_Event){
                        .u16_OffsetSample=s_QIO_CTRL.u16_OffsetSample,
                        .u8_DataType=Type_QIO_Data_wEnd,
                        .u8_MarkID=0,
                    };
                } else {
                    _tempEvent=(S_QIO_Event){
                        .u16_OffsetSample=s_QIO_CTRL.u16_OffsetSample,
                        .u8_DataType=Type_QIO_Mark_wEnd,
                        .u8_MarkID=0,
                    };//dummy data use Mark0
                }
                _queuePutData(QIO_EVENT,(U32*)&_tempEvent);
            }
#else
            _tempEvent=(S_QIO_Event){
                .u16_OffsetSample=s_QIO_CTRL.u16_OffsetSample,
                .u8_DataType=Type_QIO_Mark_wEnd,
                .u8_MarkID=0,
            };//dummy data use Mark0
            _queuePutData(QIO_EVENT,(U32*)&_tempEvent);
#endif
            break;
        case Type_QIO_Mark:
            if(_len == 1) {
                _tempEvent=(S_QIO_Event){
                    .u16_OffsetSample=s_QIO_CTRL.u16_OffsetSample,
                    .u8_DataType=Type_QIO_Mark,
                    .u8_MarkID=pData[0],
                };
                _queuePutData(QIO_EVENT,(U32*)&_tempEvent);
            }
            break;
        case Type_QIO_Mark_wEnd:
            if(_len == 1) {
                _tempEvent=(S_QIO_Event){
                    .u16_OffsetSample=s_QIO_CTRL.u16_OffsetSample,
                    .u8_DataType=Type_QIO_Mark_wEnd,
                    .u8_MarkID=pData[0],
                };
                _queuePutData(QIO_EVENT,(U32*)&_tempEvent);
            }
            break;
        case Type_QIO_End:
            if(_len == 4) {
                s_QIO_CTRL.u32_QIO_CurrAddr = s_QIO_CTRL.u32_QIO_StartAddr;
                s_QIO_CTRL.u8_QIO_GetDataFlag = DISABLE;
            }
            break;
        default:
            break;
    }
}

//------------------------------------------------------------------//
// QIO Access data memory Start
// Parameter: 
//          _addr
//          _mode
// return value:
//          EXIT_SUCCESS(0):get data success
//          EXIT_FAILURE(1):get data error
//------------------------------------------------------------------//
static U8 _accessMemStart(U32 _addr, U8 _mode)
{
    switch(_mode) {
        default: //OTP_MODE,EF_UD_MODE
            break; // do nothing
#if _SPI_MODULE
        case SPI_MODE:
            SPI_FastReadStart(_addr);
            break;
#endif
#if (_SPI1_MODULE && _SPI1_USE_FLASH)
        case SPI1_MODE:
            SPI1_FastReadStart(_addr);
            break;
#endif
    }
    return EXIT_SUCCESS;
}

//------------------------------------------------------------------//
// QIO Access data memory
// Parameter: 
//          pData
//          _addr
//          _len
//          _mode
// return value:
//          EXIT_SUCCESS(0):get data success
//          EXIT_FAILURE(1):get data error
//------------------------------------------------------------------//
static U8 _accessMem(U8* pData, U32 _addr, U8 _len, U8 _mode)
{
    //if(!pData) return EXIT_FAILURE;
    switch(_mode) {
        default:
            break;
        case    OTP_MODE:
#if _EF_SERIES
        case    EF_UD_MODE:
#endif
            memcpy(pData,(void*)_addr,_len);
            break;
#if _SPI_MODULE
        case    SPI_MODE:
            SPI_ReceiveData(pData,_len);
            break;
#endif
#if _SPI1_MODULE&&_SPI1_USE_FLASH
        case    SPI1_MODE:
            SPI1_ReceiveData(pData,_len);
            break;
#endif
    }
    return EXIT_SUCCESS;
}

//------------------------------------------------------------------//
// QIO Access data memory Stop
// Parameter: 
//          _addr
//          _mode
// return value:
//          EXIT_SUCCESS(0):get data success
//          EXIT_FAILURE(1):get data error
//------------------------------------------------------------------//
static U8 _accessMemStop(U8 _mode)
{
    switch(_mode) {
        default: //OTP_MODE,EF_UD_MODE
            break; // do nothing
#if _SPI_MODULE
        case SPI_MODE:
            SPI_FastReadStop();
            break;
#endif
#if (_SPI1_MODULE && _SPI1_USE_FLASH)
        case SPI1_MODE:
            SPI1_FastReadStop();
            break;
#endif
    }
    return EXIT_SUCCESS;
}
//------------------------------------------------------------------//
// QIO is queue full
// Parameter: 
//          NONE
// return value:
//          true / false
//------------------------------------------------------------------//
static bool _isQueueFull()
{
    if(_queueIsFull(QIO_EVENT)) return true;
#if _QSW_PWM_MODULE
    if(QSW_PWM_CMD_FIFO_Is_Full()) return true;
#endif
    return false;
}

#if !defined(QIO_GetDataSize)
#define QIO_GetDataSize             6
#endif
//------------------------------------------------------------------//
// QIO Get storage data
// Parameter: 
//          NONE
// return value:
//          EXIT_SUCCESS(0):get data success
//          EXIT_FAILURE(1):get data error
//------------------------------------------------------------------//
static U8 _getStorageData()
{
    if(!s_QIO_CTRL.u8_QIO_GetDataFlag) {
        if(!s_QIO_CTRL.u8_QIO_ReGetDataFlag) return EXIT_SUCCESS;
        s_QIO_CTRL.u8_QIO_ReGetDataFlag = DISABLE;
        s_QIO_CTRL.u8_QIO_GetDataFlag = ENABLE;
    }
    
    if(_isQueueFull()) return EXIT_SUCCESS;
    //start access mem
    _accessMemStart(s_QIO_CTRL.u32_QIO_CurrAddr, s_QIO_CTRL.u8_QIO_StorageMode);
    
    //get type and len
    U8  pData[QIO_GetDataSize];
    _accessMem(
        pData,
        s_QIO_CTRL.u32_QIO_CurrAddr,
        2,
        s_QIO_CTRL.u8_QIO_StorageMode
    );
    U8 _type = pData[0], _len = pData[1];
    
    do {
        _accessMem(
            pData,
            (s_QIO_CTRL.u32_QIO_CurrAddr+2), //offset 2B, skip current type and len
            (_len+2), //get current data and next type,len
            s_QIO_CTRL.u8_QIO_StorageMode
        );
        s_QIO_CTRL.u32_QIO_CurrAddr += (_len+2);
        _decode(_type,_len,pData);
        
        if(_type==Type_QIO_End) break;//exit point1
        if(_isQueueFull()) break;//exit point2
        _type=pData[_len];  //next type
        _len=pData[_len+1]; //next len
    } while(1);
    
    //stop access mem
    _accessMemStop(s_QIO_CTRL.u8_QIO_StorageMode);//end access mem
    return EXIT_SUCCESS;
}

//------------------------------------------------------------------//
// QIO Control data initial
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
static void	_ctrlInit()
{
	memset(&s_QIO_CTRL,0,sizeof(S_QIO_CTRL));	//QIO_CTRL initial
	memset(&s_QIO_DacIsrCTRL,0,sizeof(S_QIO_DacIsrCTRL));	//QIO_CTRL initial
}

//------------------------------------------------------------------//
// QIO DacIsr
// Parameter: 
//          _AudioResource:	
//					QIO_USE_SBC_CH1,
//					QIO_USE_SBC_CH2,
//					QIO_USE_SBC2_CH1
//					QIO_USE_SBC2_CH2
//					QIO_USE_ADPCM_CH1,
//					QIO_USE_ADPCM_CH2,
//					QIO_USE_ADPCM_CH3,
//					QIO_USE_ADPCM_CH4
//			_EventFlag:		QIO sample Flag
// return value:
//          EXIT_SUCCESS(0):service success
//			EXIT_FAILURE(1):service error
//------------------------------------------------------------------//
U8	QIO_DacIsr(U8 _AudioResource, U8 _EventFlag)
{
	if(s_QIO_CTRL.u8_QIO_AudioResurce != _AudioResource) return EXIT_FAILURE;
	if(!s_QIO_CTRL.u8_QIO_Option) return EXIT_FAILURE;
	
	if(_EventFlag) {//restart frame
		if(_queueGetData(QIO_EVENT,(U32*)&s_QIO_DacIsrCTRL.sEvent)!=EXIT_SUCCESS) {//get first event from fifo
			s_QIO_DacIsrCTRL.u8_wEventFlag = DISABLE;
			return EXIT_FAILURE;//no data
		}
		s_QIO_DacIsrCTRL.u16_OffsetSample = 0;
		s_QIO_DacIsrCTRL.u8_wEventFlag = ENABLE;
	}
	
	if(s_QIO_DacIsrCTRL.u8_wEventFlag) {
		while(s_QIO_DacIsrCTRL.u16_OffsetSample >= s_QIO_DacIsrCTRL.sEvent.u16_OffsetSample) {
			switch(s_QIO_DacIsrCTRL.sEvent.u8_DataType) {
#if _QSW_PWM_MODULE
				case Type_QIO_Data:
					QSW_PWM_CMD_FIFO_UnLock();
					if(_queueGetData(QIO_EVENT,(U32*)&s_QIO_DacIsrCTRL.sEvent)!=EXIT_SUCCESS) {//get next event from fifo
						s_QIO_DacIsrCTRL.u8_wEventFlag = DISABLE;
						return EXIT_FAILURE;
					}
					break;
				case Type_QIO_Data_wEnd:
					QSW_PWM_CMD_FIFO_UnLock();
					s_QIO_DacIsrCTRL.u8_wEventFlag = DISABLE;//end of frame
					return EXIT_SUCCESS;
					break;
#endif
				case Type_QIO_Mark:
					_queuePutData(QIO_MARK,&s_QIO_DacIsrCTRL.sEvent.u8_MarkID);
                    if(_queueGetData(QIO_EVENT,(U32*)&s_QIO_DacIsrCTRL.sEvent)!=EXIT_SUCCESS) {//get next event from fifo
						s_QIO_DacIsrCTRL.u8_wEventFlag = DISABLE;
						return EXIT_FAILURE;
					}
					break;
				case Type_QIO_Mark_wEnd:
					if(s_QIO_DacIsrCTRL.sEvent.u8_MarkID) {//dummy data = Mark 0
						_queuePutData(QIO_MARK,&s_QIO_DacIsrCTRL.sEvent.u8_MarkID);
					}
					s_QIO_DacIsrCTRL.u8_wEventFlag = DISABLE;//end of frame
					return EXIT_SUCCESS;
					break;
				default:
					break;
			}
		}
		s_QIO_DacIsrCTRL.u16_OffsetSample++;
	}
	return EXIT_SUCCESS;
}

//------------------------------------------------------------------//
// QIO Serviceloop
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void	QIO_ServiceLoop()
{
	U8 _data;
	while(_queueGetData(QIO_MARK,&_data)==EXIT_SUCCESS) {//_data from 1~255
		CB_QIOMark(_data);
	}
	
	if(!s_QIO_CTRL.u8_QIO_Option) return;
	
	if(s_QIO_CTRL.u8_QIO_DelayStopFlag) {//QIO_DelayStopENABLE or QIO_DelayStopENwPinNonBusy
		if((SysTick - s_QIO_CTRL.u32_QIO_DelayStopTick) > 20) {//delay 20ms stop QIO
			QIO_Stop_woResource();
			if(s_QIO_CTRL.u8_QIO_DelayStopFlag == QIO_DelayStopENwPinNonBusy) {
#if _QSW_PWM_MODULE
				SW_PWM_ForcePinNonBusy();
#endif
			}
			s_QIO_CTRL.u8_QIO_DelayStopFlag = QIO_DelayStopDISABLE;
		}
	}
	_getStorageData();//Put data to FIFO
}

//------------------------------------------------------------------//
// QIO Play
// Parameter: 
//          _AudioResource:	
//					QIO_USE_SBC_CH1,
//					QIO_USE_SBC_CH2,
//					QIO_USE_SBC2_CH1
//					QIO_USE_SBC2_CH2
//					QIO_USE_ADPCM_CH1,
//					QIO_USE_ADPCM_CH2,
//					QIO_USE_ADPCM_CH3,
//					QIO_USE_ADPCM_CH4
//			_StorageMode:	SPI_MODE/SPI1_MODE/OTP_MODE/EF_MODE/EF_UD_MODE
//			_StartAddr:		QIO Data start address(StartAddr+HEADER_LEN+pheader->nyq_offset)
// return value:
//          NONE
//------------------------------------------------------------------//
void	QIO_Play(U8 _AudioResource, U8 _StorageMode, U32 _StartAddr)
{
	if(s_QIO_CTRL.u8_QIO_AudioResurce != QIO_RESOURCE_IDLE) {
		QIO_Stop_woResource();
	}
	
	//Initial FIFO
	GIE_DISABLE();
	_ctrlInit();
#if _QSW_PWM_MODULE
	QSW_PWM_CMD_Init();
	SW_PWM_ChDataInit();
#endif
    _queueReset(QIO_MARK);
    _queueReset(QIO_EVENT);
#if QIO_FrameFlag_FIFOSIZE!=0
	QIO_FrameFlag_FIFO_Init();
#endif
	GIE_ENABLE();
	
	s_QIO_CTRL.u8_QIO_StorageMode = _StorageMode;
	s_QIO_CTRL.u32_QIO_StartAddr = _StartAddr;
	s_QIO_CTRL.u32_QIO_CurrAddr = _StartAddr;
	s_QIO_CTRL.u8_QIO_AudioResurce = _AudioResource;
	s_QIO_CTRL.u8_QIO_GetDataFlag = ENABLE;

	//Initial SW PWM-IO module
#if _QSW_PWM_MODULE
    U8 _CH;
	for(_CH = 0; _CH < _SW_PWM_CH_NUM; _CH++) {
		SW_PWM_CH_Start(_CH);
	}
#endif
	
	_getStorageData();//Put data to FIFO	
#if _QSW_PWM_MODULE
	SW_PWM_Start();
#endif
	s_QIO_CTRL.u8_QIO_Option = ENABLE;
}

//------------------------------------------------------------------//
// QIO Stop woResource
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void	QIO_Stop_woResource()
{
	s_QIO_CTRL.u8_QIO_AudioResurce = QIO_RESOURCE_IDLE;
	s_QIO_CTRL.u8_QIO_Option = DISABLE;
	s_QIO_CTRL.u8_QIO_GetDataFlag = DISABLE;
#if _QSW_PWM_MODULE
	SW_PWM_Stop();
#endif
}

//------------------------------------------------------------------//
// QIO Delay Stop
// Parameter: 
//          _AudioResource:	
//					QIO_USE_SBC_CH1,
//					QIO_USE_SBC_CH2,
//					QIO_USE_SBC2_CH1
//					QIO_USE_SBC2_CH2
//					QIO_USE_ADPCM_CH1,
//					QIO_USE_ADPCM_CH2,
//					QIO_USE_ADPCM_CH3,
//					QIO_USE_ADPCM_CH4
//			_ForcePinNonBusy: DISABLE/ENABLE
// return value:
//          NONE
//------------------------------------------------------------------//
void	QIO_DelayStop(U8 _AudioResource , U8 _ForcePinNonBusy)
{
	if(s_QIO_CTRL.u8_QIO_AudioResurce != _AudioResource) return;

	s_QIO_CTRL.u32_QIO_DelayStopTick = SysTick;
	if(_ForcePinNonBusy) {
		s_QIO_CTRL.u8_QIO_DelayStopFlag=QIO_DelayStopENwPinNonBusy;
	} else {
		s_QIO_CTRL.u8_QIO_DelayStopFlag=QIO_DelayStopENABLE;
	}
}

//------------------------------------------------------------------//
// QIO Stop
// Parameter: 
//          _AudioResource:	
//					QIO_USE_SBC_CH1,
//					QIO_USE_SBC_CH2,
//					QIO_USE_SBC2_CH1
//					QIO_USE_SBC2_CH2
//					QIO_USE_ADPCM_CH1,
//					QIO_USE_ADPCM_CH2,
//					QIO_USE_ADPCM_CH3,
//					QIO_USE_ADPCM_CH4
//			_ForcePinNonBusy: DISABLE/ENABLE
// return value:
//          NONE
//------------------------------------------------------------------//
void	QIO_Stop(U8 _AudioResource, U8 _ForcePinNonBusy)
{
	if(s_QIO_CTRL.u8_QIO_AudioResurce != _AudioResource) return;

	QIO_Stop_woResource();
#if _QSW_PWM_MODULE
	if(_ForcePinNonBusy) {
		SW_PWM_ForcePinNonBusy();
	}
#endif
}

//------------------------------------------------------------------//
// QIO Pause
// Parameter: 
//          _AudioResource:	
//					QIO_USE_SBC_CH1,
//					QIO_USE_SBC_CH2,
//					QIO_USE_SBC2_CH1
//					QIO_USE_SBC2_CH2
//					QIO_USE_ADPCM_CH1,
//					QIO_USE_ADPCM_CH2,
//					QIO_USE_ADPCM_CH3,
//					QIO_USE_ADPCM_CH4
// return value:
//          NONE
//------------------------------------------------------------------//
void	QIO_Pause(U8 _AudioResource)
{
	if(s_QIO_CTRL.u8_QIO_AudioResurce != _AudioResource)return;

#if _QSW_PWM_MODULE
	SW_PWM_Pause();
#endif
}

//------------------------------------------------------------------//
// QIO Resume
// Parameter: 
//          _AudioResource:	
//					QIO_USE_SBC_CH1,
//					QIO_USE_SBC_CH2,
//					QIO_USE_SBC2_CH1
//					QIO_USE_SBC2_CH2
//					QIO_USE_ADPCM_CH1,
//					QIO_USE_ADPCM_CH2,
//					QIO_USE_ADPCM_CH3,
//					QIO_USE_ADPCM_CH4
// return value:
//          NONE
//------------------------------------------------------------------//
void	QIO_Resume(U8 _AudioResource)
{
	if(s_QIO_CTRL.u8_QIO_AudioResurce != _AudioResource) return;

#if _QSW_PWM_MODULE
	SW_PWM_Resume();
#endif
}

//------------------------------------------------------------------//
// QIO Reset Get storage data flag
// Parameter: 
//          _AudioResource:	
//					QIO_USE_SBC_CH1,
//					QIO_USE_SBC_CH2,
//					QIO_USE_SBC2_CH1
//					QIO_USE_SBC2_CH2
//					QIO_USE_ADPCM_CH1,
//					QIO_USE_ADPCM_CH2,
//					QIO_USE_ADPCM_CH3,
//					QIO_USE_ADPCM_CH4
// return value:
//          EXIT_SUCCESS(0):set data success
//			EXIT_FAILURE(1):set data error
//------------------------------------------------------------------//
U8	QIO_ReGetStorageDataSet(U8 _AudioResource)
{
	if(s_QIO_CTRL.u8_QIO_AudioResurce != _AudioResource) return EXIT_FAILURE;
	
	s_QIO_CTRL.u8_QIO_ReGetDataFlag = ENABLE;
	_getStorageData();//Put data to FIFO	
	return EXIT_SUCCESS;
}

#if QIO_FrameFlag_FIFOSIZE!=0
_queueDefine(U8,QIO_FrameFlag_FIFOSIZE,QIO_FrameQueue);
typedef struct
{
    U8  rBitIdx;
    U8  rData;
    U8  wBitIdx;
    U8  wData;
}S_QIO_FrameBitQueue;
S_QIO_FrameBitQueue    s_QIO_FrameBitQueue;
//------------------------------------------------------------------//
// QIO frame flag initial
// Parameter:
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
static void QIO_FrameFlag_FIFO_Init()
{
    _queueReset(QIO_FrameQueue);
    memset(&s_QIO_FrameBitQueue,0,sizeof(S_QIO_FrameBitQueue));
}

//------------------------------------------------------------------//
// QIO frame flag FIFO bit put
// Parameter:
//          _AudioResource:
//                  QIO_USE_SBC_CH1,
//                  QIO_USE_SBC_CH2,
//                  QIO_USE_SBC2_CH1
//                  QIO_USE_SBC2_CH2
//                  QIO_USE_ADPCM_CH1,
//                  QIO_USE_ADPCM_CH2,
//                  QIO_USE_ADPCM_CH3,
//                  QIO_USE_ADPCM_CH4
//          U8 *data
// return value:
//          EXIT_FAILURE(1):FIFO bit put failure
//          EXIT_SUCCESS(0):FIFO bit put success
//------------------------------------------------------------------//
U8  QIO_FrameFlag_FIFO_BitPut(U8 _AudioResource, U8 *data)
{
    if(s_QIO_CTRL.u8_QIO_AudioResurce != _AudioResource) return EXIT_FAILURE;

    S_QIO_FrameBitQueue *pBitfifo = &s_QIO_FrameBitQueue;
    U8 _mask = (1<<pBitfifo->wBitIdx);
    U8 _data = pBitfifo->wData;
    pBitfifo->wData = (*data > 0) ? (_data|_mask) : (_data&(~_mask));

    pBitfifo->wBitIdx=(pBitfifo->wBitIdx+1)%8;
    if(!pBitfifo->wBitIdx) {
        return _queuePutData(QIO_FrameQueue, &pBitfifo->wData);
    }
    return EXIT_SUCCESS;
}

//------------------------------------------------------------------//
// QIO frame flag FIFO reset put
// Parameter:
//          _AudioResource:
//                  QIO_USE_SBC_CH1,
//                  QIO_USE_SBC_CH2,
//                  QIO_USE_SBC2_CH1
//                  QIO_USE_SBC2_CH2
//                  QIO_USE_ADPCM_CH1,
//                  QIO_USE_ADPCM_CH2,
//                  QIO_USE_ADPCM_CH3,
//                  QIO_USE_ADPCM_CH4
// return value:
//          EXIT_FAILURE(1):FIFO reset put failure
//          EXIT_SUCCESS(0):FIFO reset put success
//------------------------------------------------------------------// 
U8  QIO_FrameFlag_FIFO_RstPut(U8 _AudioResource)
{
    if(s_QIO_CTRL.u8_QIO_AudioResurce != _AudioResource) return EXIT_FAILURE;

    S_QIO_FrameBitQueue *pBitfifo = &s_QIO_FrameBitQueue;
    if(pBitfifo->wBitIdx) { //if have data
        pBitfifo->wBitIdx = 0;
        return _queuePutData(QIO_FrameQueue, &pBitfifo->wData);
    }
    return EXIT_FAILURE;    //no data
}

//------------------------------------------------------------------//
// QIO frame flag FIFO bit get
// Parameter:
//          _AudioResource:
//                  QIO_USE_SBC_CH1,
//                  QIO_USE_SBC_CH2,
//                  QIO_USE_SBC2_CH1
//                  QIO_USE_SBC2_CH2
//                  QIO_USE_ADPCM_CH1,
//                  QIO_USE_ADPCM_CH2,
//                  QIO_USE_ADPCM_CH3,
//                  QIO_USE_ADPCM_CH4
//          U8 *data
// return value:
//          EXIT_FAILURE(1):FIFO bit get failure
//          EXIT_SUCCESS(0):FIFO bit get success
//------------------------------------------------------------------//
U8  QIO_FrameFlag_FIFO_BitGet(U8 _AudioResource, U8 *data)
{
    if(s_QIO_CTRL.u8_QIO_AudioResurce != _AudioResource) return EXIT_FAILURE;

    S_QIO_FrameBitQueue *pBitfifo = &s_QIO_FrameBitQueue;
    if(!pBitfifo->rBitIdx) {    //idx=0? get new data
        if(_queueGetData(QIO_FrameQueue, &pBitfifo->rData)!=EXIT_SUCCESS) {
            return EXIT_FAILURE;//read error
        }
    }

    U8 _mask = (1<<pBitfifo->rBitIdx);
    U8 _data = pBitfifo->rData;
    *data = ((_data & _mask) > 0);  //true(1) or false(0)

    pBitfifo->rBitIdx=(pBitfifo->rBitIdx+1)%8;
    return EXIT_SUCCESS;
}

//------------------------------------------------------------------//
// QIO frame flag FIFO reset get
// Parameter:
//          _AudioResource:
//                  QIO_USE_SBC_CH1,
//                  QIO_USE_SBC_CH2,
//                  QIO_USE_SBC2_CH1
//                  QIO_USE_SBC2_CH2
//                  QIO_USE_ADPCM_CH1,
//                  QIO_USE_ADPCM_CH2,
//                  QIO_USE_ADPCM_CH3,
//                  QIO_USE_ADPCM_CH4
// return value:
//          EXIT_FAILURE(1):FIFO reset get failure
//          EXIT_SUCCESS(0):FIFO reset get success
//------------------------------------------------------------------//
U8  QIO_FrameFlag_FIFO_RstGet(U8 _AudioResource)
{
    if(s_QIO_CTRL.u8_QIO_AudioResurce != _AudioResource) return EXIT_FAILURE;

    S_QIO_FrameBitQueue *pBitfifo = &s_QIO_FrameBitQueue;
    if(pBitfifo->rBitIdx) {//if have data
        pBitfifo->rBitIdx = 0;
        return EXIT_SUCCESS;//reset success
    }
    return EXIT_FAILURE;//does not need reset
}
#endif//#if QIO_FrameFlag_FIFOSIZE!=0

#if _QSW_PWM_MODULE
#if (QIO_EVENT_FIFOSIZE&(QIO_EVENT_FIFOSIZE-1))!=0
#error "QIO_EVENT_FIFOSIZE only support power of 2.(16,32...etc)"
#endif
#if QIO_EVENT_FIFOSIZE>128
#error "QIO_EVENT_FIFOSIZE max is 128"
#endif
#define QSW_PWM_CMD_FIFOSIZE    (QIO_EVENT_FIFOSIZE)
#define QSW_PWM_CMD_FIFOMASK    (QSW_PWM_CMD_FIFOSIZE-1)
typedef struct 
{
    U8  pr;//read pt
    U8  pl;//lock pt
    U8  pw;//write pt
    U32 buf[QSW_PWM_CMD_FIFOSIZE];//FIFO use U32 to reduce process time.
}S_QSW_PWM_CMD_FIFO;
S_QSW_PWM_CMD_FIFO  s_QSW_PWM_CMD_FIFO;
//------------------------------------------------------------------//
// SW PWM-IO CMD FIFO Initial
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
static  void    QSW_PWM_CMD_Init()
{
    S_QSW_PWM_CMD_FIFO *pfifo = &s_QSW_PWM_CMD_FIFO;
    pfifo->pr = pfifo->pl = pfifo->pw = 0;
}

//------------------------------------------------------------------//
// SW PWM-IO CMD FIFO empty check
// Parameter: 
//          NONE
// return value:
//          true(1):empty
//          false(0):not empty
//------------------------------------------------------------------//
static bool QSW_PWM_CMD_FIFO_Is_Empty()
{
    S_QSW_PWM_CMD_FIFO *pfifo = &s_QSW_PWM_CMD_FIFO;
    return (pfifo->pr == pfifo->pl);
}

//------------------------------------------------------------------//
// SW PWM-IO CMD FIFO full check
// Parameter: 
//          NONE
// return value:
//          true(1):full
//          false(0):not full
//------------------------------------------------------------------//
static bool QSW_PWM_CMD_FIFO_Is_Full()
{
    S_QSW_PWM_CMD_FIFO *pfifo = &s_QSW_PWM_CMD_FIFO;
    return pfifo->pw==(pfifo->pr^(QSW_PWM_CMD_FIFOMASK+1));
}

//------------------------------------------------------------------//
// SW PWM-IO CMD FIFO unlock end check
// Parameter: 
//          NONE
// return value:
//          true(1):unlock end
//          false(0):not unlock end
//------------------------------------------------------------------//
static bool QSW_PWM_CMD_FIFO_Is_UnLock_END()
{
    S_QSW_PWM_CMD_FIFO *pfifo = &s_QSW_PWM_CMD_FIFO;
    return (pfifo->pl == pfifo->pw);
}

//------------------------------------------------------------------//
// SW PWM-IO CMD FIFO unlock
// Parameter: 
//          NONE
// return value:
//          EXIT_FAILURE(1):FIFO unlock failure
//          EXIT_SUCCESS(0):FIFO unlock success
//------------------------------------------------------------------//
static U8   QSW_PWM_CMD_FIFO_UnLock()
{
    if(QSW_PWM_CMD_FIFO_Is_UnLock_END()) return EXIT_FAILURE;
    S_QSW_PWM_CMD_FIFO *pfifo = &s_QSW_PWM_CMD_FIFO;
    pfifo->pl=(pfifo->pl+1)&((2*(QSW_PWM_CMD_FIFOMASK+1))-1);
    return EXIT_SUCCESS;
}

//------------------------------------------------------------------//
// SW PWM-IO CMD FIFO get
// Parameter: 
//          U32 *data
// return value:
//          EXIT_FAILURE(1):FIFO get failure
//          EXIT_SUCCESS(0):FIFO get success
//------------------------------------------------------------------//
static U8   QSW_PWM_CMD_FIFO_Get(U32 *data)//interface to SW_PWM
{
    if(QSW_PWM_CMD_FIFO_Is_Empty()) return EXIT_FAILURE;//||(data== NULL)
    S_QSW_PWM_CMD_FIFO *pfifo = &s_QSW_PWM_CMD_FIFO;
    *data = pfifo->buf[pfifo->pr&QSW_PWM_CMD_FIFOMASK];
    pfifo->pr=(pfifo->pr+1)&((2*(QSW_PWM_CMD_FIFOMASK+1))-1);
    return EXIT_SUCCESS;
}

//------------------------------------------------------------------//
// SW PWM-IO CMD FIFO put
// Parameter:
//          U32 *data
// return value:
//          EXIT_FAILURE(1):FIFO put failure
//          EXIT_SUCCESS(0):FIFO put success
//------------------------------------------------------------------//
static U8   QSW_PWM_CMD_FIFO_Put(U32 *data)
{
    if(QSW_PWM_CMD_FIFO_Is_Full()) return EXIT_FAILURE;//||(data== NULL)
    S_QSW_PWM_CMD_FIFO *pfifo = &s_QSW_PWM_CMD_FIFO;
    pfifo->buf[pfifo->pw&QSW_PWM_CMD_FIFOMASK]= *data;
    pfifo->pw=(pfifo->pw+1)&((2*(QSW_PWM_CMD_FIFOMASK+1))-1);
    return EXIT_SUCCESS;
}

//------------------------------------------------------------------//
// Set SW PWM-IO duty with FIFO Lock
// Parameter:
//          _CH:        SW PWM-IO Channel(0 ~ _SW_PWM_CH_NUM-1)
//          _Duty:      SW PWM-IO Duty(0 ~ 255)
//          _DiffFrame: SW PWM-IO Diff ms of Duty Change(0 ~ 65535ms)
// return value:
//          EXIT_SUCCESS(0):FIFO Put success
//          EXIT_FAILURE(1):FIFO Put error
//------------------------------------------------------------------//
static U8   QSW_PWM_Duty_Set_wLock(U8 _CH, U8 _Duty, U16 _DiffMs)
{
    _DiffMs = SW_PWM_Ms2Frame(_DiffMs);
    S_SW_PWM_CMD_TYPE _tempCmd =
    {
        .u8_CMD_ch=_CH,
        .u8_CMD_TargetDutyNum=_Duty,
        .u16_CMD_DiffFrame=_DiffMs,
    };
    return  QSW_PWM_CMD_FIFO_Put((U32*)&_tempCmd);
}
#endif//#if _QSW_PWM_MODULE
//------------------------------------------------------------------//
// SW PWM-IO CMD FIFO get(for PWM Callback interface)
// Parameter:
//          U32 *data
// return value:
//          EXIT_FAILURE(1):FIFO get failure
//          EXIT_SUCCESS(0):FIFO get success
//------------------------------------------------------------------//
U8  QIO_SW_PWM_GetCmd(U32 *cmd)
{
#if _QSW_PWM_MODULE
    return QSW_PWM_CMD_FIFO_Get(cmd);
#else
    return EXIT_FAILURE;
#endif
}
#endif//#if _QIO_MODULE