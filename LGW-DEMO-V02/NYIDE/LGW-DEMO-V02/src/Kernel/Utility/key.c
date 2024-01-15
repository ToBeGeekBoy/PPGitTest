/* Includes ------------------------------------------------------------------*/
#include "key.h"
/* Defines -------------------------------------------------------------------*/
//in key.h
/* Static Variables ----------------------------------------------------------*/
#if _DIRECTKEY_MODULE && DIRECTKEY_PORT_NUM
SCOPE_TYPE	S_DKeyCtrl      sDKeyCtrl;
SCOPE_TYPE 	K_DKeyInPara  kDKeyInPara[DIRECTKEY_PORT_NUM] = {
#if (GPIOA_MASK>0)
    {
        .port               =GPIOA,
        .mask               =GPIOA_MASK,
        .busyDefine         =GPIOA_BUSY_DEFINE,
#if GPIOA_SHORT_DB_MASK
        .shortDebounceMask  =GPIOA_SHORT_DB_MASK,
#endif
		.portLable			=DKEY_PORTA,
    },
#endif
#if (GPIOB_MASK>0)
    {
        .port               =GPIOB,
        .mask               =GPIOB_MASK,
        .busyDefine         =GPIOB_BUSY_DEFINE,
#if GPIOB_SHORT_DB_MASK
        .shortDebounceMask  =GPIOB_SHORT_DB_MASK,
#endif
		.portLable			=DKEY_PORTB,
    },
#endif
#if (GPIOC_MASK>0)
    {
        .port               =GPIOC,
        .mask               =GPIOC_MASK,
        .busyDefine         =GPIOC_BUSY_DEFINE,
#if GPIOC_SHORT_DB_MASK
        .shortDebounceMask  =GPIOC_SHORT_DB_MASK,
#endif
		.portLable			=DKEY_PORTC,
    },
#endif
#if (GPIOD_MASK>0)
    {
        .port               =GPIOD,
        .mask               =GPIOD_MASK,
        .busyDefine         =GPIOD_BUSY_DEFINE,
#if GPIOD_SHORT_DB_MASK
        .shortDebounceMask  =GPIOD_SHORT_DB_MASK,
#endif
		.portLable			=DKEY_PORTD,
    },
#endif
};
#endif

//------------------------------------------------------------------//
// Move key events to key queue by port
// Parameter: 
//			keyEvent
//          keyEventVar
//          portLable
// return value:
//          NONE
//------------------------------------------------------------------// 
#if _DIRECTKEY_MODULE && DIRECTKEY_PORT_NUM
static void KeyEventToQueueByPort(U8 keyEvent, U16 *keyEventVar, U8 portLable) {
	while(*keyEventVar) {
		if(sDKeyCtrl.qBufCnt>=DKEY_QUEUE_LEN) 
			break;
		U8 j = Get_LS1B_Position(*keyEventVar);
		sDKeyCtrl.qBuf[sDKeyCtrl.qWIndex] = ((keyEvent<<6) | (portLable<<4) | j);
		sDKeyCtrl.qWIndex++;
		if(sDKeyCtrl.qWIndex==DKEY_QUEUE_LEN) {
			sDKeyCtrl.qWIndex = 0;
		}
		sDKeyCtrl.qBufCnt++;
		*keyEventVar &= ~(1<<j);
	}
}
#endif
//------------------------------------------------------------------//
// Scan keys by Port
// Parameter: 
//          pPort
//          pPara
// return value:
//          NONE
//------------------------------------------------------------------// 
#if _DIRECTKEY_MODULE && DIRECTKEY_PORT_NUM
static void KeyScanByPort(S_DKeyPortCtrl *pPort, K_DKeyInPara *pPara) {
    U16 _data = ~(pPara->port->PAD);
    _data ^= (pPara->busyDefine);
    _data &= (pPara->mask) & (pPara->port->DIR);
#if DEBOUNCE_CNT>0
#if GPIOA_SHORT_DB_MASK||GPIOB_SHORT_DB_MASK||GPIOC_SHORT_DB_MASK||GPIOD_SHORT_DB_MASK
    U16 _sdData;
    if(pPara->shortDebounceMask) {
        _sdData = _data^(pPort->prev);
        _sdData &= (pPara->shortDebounceMask);
        pPort->pressEvent |= _sdData & _data;
        pPort->releaseEvent |= _sdData & (pPort->prev);
        pPort->prev ^= _sdData;
        pPort->checked ^= _sdData;
        
        KeyEventToQueueByPort(DKEY_RELEASE, &(pPort->releaseEvent), pPara->portLable);
        KeyEventToQueueByPort(DKEY_PRESS, &(pPort->pressEvent), pPara->portLable);
    }
#endif
	U16 _peekPrev = pPort->prev;
	pPort->prev = _data;
    if(_data == pPort->checked) {
        pPort->debounceCount = 0;
        return;
    }
    if(_data != _peekPrev){
		pPort->debounceCount = DEBOUNCE_CNT;//!!!
		return;
	}
    if(pPort->debounceCount > 1){
		--(pPort->debounceCount);
		return;
	}
	pPort->debounceCount = 0;
#else
    if(_data == pPort->checked) return;
#endif
    U16 _diff = _data^(pPort->checked);
	pPort->pressEvent |= _diff&_data;
	pPort->releaseEvent |= _diff&(pPort->checked);
	pPort->checked=_data;

	KeyEventToQueueByPort(DKEY_RELEASE, &(pPort->releaseEvent), pPara->portLable);
	KeyEventToQueueByPort(DKEY_PRESS, &(pPort->pressEvent), pPara->portLable);
}
#endif
//------------------------------------------------------------------//
// Scan keys
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//  
#if _DIRECTKEY_MODULE && DIRECTKEY_PORT_NUM
static void KeyScan(void){
    U8 i;
    for(i=0;i<DIRECTKEY_PORT_NUM;++i) {
        KeyScanByPort(&(sDKeyCtrl.sIn[i]), &kDKeyInPara[i]);
    }
}
#endif
//------------------------------------------------------------------//
// KeyScan Debounce state Checking
// Parameter: 
//          NONE
// return value:
//          1: debounce state
//			0: non-debounce state
//------------------------------------------------------------------// 
U8 KeyScan_DebounceCheck(void){
#if _DIRECTKEY_MODULE && DIRECTKEY_PORT_NUM
#if DEBOUNCE_CNT>0
    U8 i;
    for(i=0;i<DIRECTKEY_PORT_NUM;++i) {
        if(sDKeyCtrl.sIn[i].debounceCount) return true;
    }
#endif
#endif
	return false;
}
//------------------------------------------------------------------//
// return KeyScan Checked data
// Parameter: 
//          portIndex: 0=GPIOA, 1=GPIOB, 2=GPIOC
// return value:
//          the checked data of KeyScan
//------------------------------------------------------------------// 
U16 KeyScan_CheckedData(const U8 portIndex){
#if _DIRECTKEY_MODULE && DIRECTKEY_PORT_NUM
    S_DKeyPortCtrl *pPort=sDKeyCtrl.sIn;
    K_DKeyInPara *pPara=kDKeyInPara;
#if GPIOA_MASK
	if(portIndex == 0)
		return ~((pPort[kDKeyInPortA].checked)^(pPara[kDKeyInPortA].busyDefine));
#endif
#if GPIOB_MASK
	if(portIndex == 1)
		return ~((pPort[kDKeyInPortB].checked)^(pPara[kDKeyInPortB].busyDefine));
#endif
#if GPIOC_MASK
	if(portIndex == 2)
		return ~((pPort[kDKeyInPortC].checked)^(pPara[kDKeyInPortC].busyDefine));
#endif
#if GPIOD_MASK
	if(portIndex == 3)
		return ~((pPort[kDKeyInPortD].checked)^(pPara[kDKeyInPortD].busyDefine));
#endif
#endif
	return 0xFFFF;
}
//------------------------------------------------------------------//
// return KeyScan Used Pin
// Parameter: 
//          portIndex: 0=GPIOA, 1=GPIOB, 2=GPIOC
// return value:
//          the Use Pin of KeyScan
//------------------------------------------------------------------// 
U16 KeyScan_UsedPin(const U8 portIndex){
#if _DIRECTKEY_MODULE && DIRECTKEY_PORT_NUM
    K_DKeyInPara *pPara=kDKeyInPara;
#if GPIOA_MASK
	if(portIndex == 0)
		return (pPara[kDKeyInPortA].mask) & (pPara[kDKeyInPortA].port->DIR);
#endif
#if GPIOB_MASK
	if(portIndex == 1)
		return (pPara[kDKeyInPortB].mask) & (pPara[kDKeyInPortB].port->DIR);
#endif
#if GPIOC_MASK
	if(portIndex == 2)
		return (pPara[kDKeyInPortC].mask) & (pPara[kDKeyInPortC].port->DIR);
#endif
#if GPIOD_MASK
	if(portIndex == 3)
		return (pPara[kDKeyInPortD].mask) & (pPara[kDKeyInPortD].port->DIR);
#endif
#endif
	return 0;
}
//------------------------------------------------------------------//
// check power on trigger
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void DIRECTKEY_PowerOnTriggerCheck(void){
#ifdef	_POWER_ON_TRIGGER_DIRECTKEY
#if _POWER_ON_TRIGGER_DIRECTKEY==DISABLE
#if _DIRECTKEY_MODULE && DIRECTKEY_PORT_NUM
    U16 _data;
    U8 i;
    for(i=0;i<DIRECTKEY_PORT_NUM;++i) {
        _data = ~((kDKeyInPara[i].port)->PAD);
        _data ^= (kDKeyInPara[i].busyDefine);
        _data &= (kDKeyInPara[i].mask) & (kDKeyInPara[i].port->DIR);
#if DEBOUNCE_CNT>0
        sDKeyCtrl.sIn[i].prev=_data;
#endif
        sDKeyCtrl.sIn[i].checked=_data;
    }
#endif
#endif//#if _POWER_ON_TRIGGER_DIRECTKEY==DISABLE
#endif//#ifdef	_POWER_ON_TRIGGER_DIRECTKEY
}
//------------------------------------------------------------------//
// Reset KeyScan variables
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void DIRECTKEY_Reset(void){
#if _DIRECTKEY_MODULE && DIRECTKEY_PORT_NUM
	sDKeyCtrl.getKeySemaphore = 1;
    memset((void*)sDKeyCtrl.sIn,0,sizeof(sDKeyCtrl.sIn));
	sDKeyCtrl.getKeySemaphore = 0;
#endif
}
//------------------------------------------------------------------//
// Init KeyScan variables & register
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void DIRECTKEY_Init(void){
	GIE_DISABLE();
#if GPIOA_MASK&&_DIRECTKEY_MODULE
#if _EF_SERIES
	GPIOA->MFP &= ~GPIOA_MFP_MASK;
#else
	GPIOA->MFP &= ~GPIOA_MASK;													// Set I/O mode
#endif
	GPIOA->DIR |= GPIOA_MASK;													// Set Input mode
#ifdef _DIRECTKEY_PULL_UP
#if _DIRECTKEY_PULL_UP==ENABLE
	GPIOA->CFG &= ~GPIOA_MASK;													// Set pull up
#if GPIOA_MASK>>8
	GPIOA->PULL_HIGH_CFG |= (1<<1);												// Set strong PU
#endif
#if GPIOA_MASK&0xFF
	GPIOA->PULL_HIGH_CFG |= (1<<0);												// Set strong PU
#endif
#else//_DIRECTKEY_PULL_UP==DISABLE
	GPIOA->CFG |= GPIOA_MASK;													// Unset pull up
#endif
#endif//#ifdef _DIRECTKEY_PULL_UP
#endif
#if GPIOB_MASK&&_DIRECTKEY_MODULE
#if _EF_SERIES
	GPIOB->MFP &= ~GPIOB_MFP_MASK;
#else
	GPIOB->MFP &= ~GPIOB_MASK;													// Set I/O mode
#endif
	GPIOB->DIR |= GPIOB_MASK;													// Set Input mode
#ifdef _DIRECTKEY_PULL_UP
#if _DIRECTKEY_PULL_UP==ENABLE
	GPIOB->CFG &= ~GPIOB_MASK;													// Set pull up
#if GPIOB_MASK>>8
	GPIOA->PULL_HIGH_CFG |= (1<<3);												// Set strong PU
#endif
#if GPIOB_MASK&0xFF
	GPIOA->PULL_HIGH_CFG |= (1<<2);												// Set strong PU
#endif
#else//_DIRECTKEY_PULL_UP==DISABLE
	GPIOB->CFG |= GPIOB_MASK;													// Unset pull up
#endif
#endif//#ifdef _DIRECTKEY_PULL_UP
#endif
#if GPIOC_MASK&&_DIRECTKEY_MODULE
#if _EF_SERIES
	GPIOC->MFP &= ~GPIOC_MFP_MASK;
#else
	GPIOC->MFP &= ~GPIOC_MASK;													// Set I/O mode
#endif
	GPIOC->DIR |= GPIOC_MASK;													// Set Input mode
#ifdef _DIRECTKEY_PULL_UP
#if _DIRECTKEY_PULL_UP==ENABLE
	GPIOC->CFG &= ~GPIOC_MASK;													// Set pull up
	GPIOA->PULL_HIGH_CFG |= (1<<4);												// Set strong PU
#else//_DIRECTKEY_PULL_UP==DISABLE
	GPIOC->CFG |= GPIOC_MASK;													// Unset pull up
#endif
#endif//#ifdef _DIRECTKEY_PULL_UP
#endif
#if GPIOD_MASK&&_DIRECTKEY_MODULE
	GPIOD->MFP &= ~GPIOD_MFP_MASK;
	GPIOD->DIR |= GPIOD_MASK;													// Set Input mode
#ifdef _DIRECTKEY_PULL_UP
#if _DIRECTKEY_PULL_UP==ENABLE
	GPIOD->CFG &= ~GPIOD_MASK;													// Set pull up
	GPIOA->PULL_HIGH_CFG |= (1<<5);												// Set strong PU
#else//_DIRECTKEY_PULL_UP==DISABLE
	GPIOD->CFG |= GPIOD_MASK;													// Unset pull up
#endif
#endif//#ifdef _DIRECTKEY_PULL_UP
#endif

	DIRECTKEY_Reset();
	DIRECTKEY_PowerOnTriggerCheck();
	DIRECTKEY_ScanCmd(ENABLE);
	GIE_ENABLE();
}
//------------------------------------------------------------------//
// Directkey Timer Isr(Trigger by ms timer interrupt)
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void DIRECTKEY_TimerIsr(){
#if _DIRECTKEY_MODULE && DIRECTKEY_PORT_NUM
    if(sDKeyCtrl.getKeySemaphore || !sDKeyCtrl.KeyScanCmd)return;
#if KEY_SCAN_TICK>1
	if(sDKeyCtrl.KeyScanCount>1) {
		--sDKeyCtrl.KeyScanCount;// KeyScanCount = KEY_SCAN_TICK~1
		return;
	}
	sDKeyCtrl.KeyScanCount = KEY_SCAN_TICK;
#endif
	KeyScan();
#endif
}
//------------------------------------------------------------------//
// Directkey Scan Commend
// Parameter: 
//          DISABLE/ENABLE
// return value:
//          NONE
//------------------------------------------------------------------// 
void DIRECTKEY_ScanCmd(U8 _cmd){
#if _DIRECTKEY_MODULE && DIRECTKEY_PORT_NUM
	sDKeyCtrl.KeyScanCmd = (_cmd)?ENABLE:DISABLE;
#endif
}
//------------------------------------------------------------------//
// Get KeyCode
// Parameter: 
//          NONE
// return value:
//          KeyCode
//------------------------------------------------------------------//    
U8 DIRECTKEY_GetKey(void){
#if _DIRECTKEY_MODULE && DIRECTKEY_PORT_NUM
	sDKeyCtrl.getKeySemaphore = 1;
	U8 keyCode;
	if(sDKeyCtrl.qBufCnt) {
		keyCode = sDKeyCtrl.qBuf[sDKeyCtrl.qRIndex];
		sDKeyCtrl.qRIndex++;
		if(sDKeyCtrl.qRIndex==DKEY_QUEUE_LEN) {
			sDKeyCtrl.qRIndex = 0;
		}
		sDKeyCtrl.qBufCnt--;
	}
	else {
		keyCode = 0;
	}

    U8 i;
    for(i=0;i<DIRECTKEY_PORT_NUM;++i) {
		KeyEventToQueueByPort(DKEY_RELEASE, &(sDKeyCtrl.sIn[i].releaseEvent), kDKeyInPara[i].portLable);
		KeyEventToQueueByPort(DKEY_PRESS, &(sDKeyCtrl.sIn[i].pressEvent), kDKeyInPara[i].portLable);
	}
	sDKeyCtrl.getKeySemaphore = 0;

	return keyCode;
#else
	return 0;
#endif
}
//------------------------------------------------------------------//
// Peek KeyCode
// Parameter: 
//          NONE
// return value:
//          KeyCode
//------------------------------------------------------------------//    
U8 DIRECTKEY_PeekKey(void){
#if _DIRECTKEY_MODULE && DIRECTKEY_PORT_NUM
	sDKeyCtrl.getKeySemaphore = 1;
	U8 keyCode;
	if(sDKeyCtrl.qBufCnt) {
		keyCode = sDKeyCtrl.qBuf[sDKeyCtrl.qRIndex];
	}
	else {
		keyCode = 0;
	}
	sDKeyCtrl.getKeySemaphore = 0;

	return keyCode;
#else
    return 0;
#endif
}
//------------------------------------------------------------------//
// Is key queue empty?
// Parameter: 
//          NONE
// return value:
//          true / false
//------------------------------------------------------------------//  
bool DIRECTKEY_IsQueueEmpty(void)
{
#if _DIRECTKEY_MODULE && DIRECTKEY_PORT_NUM
    return (sDKeyCtrl.qBufCnt==0);
#else
    return true;
#endif
}