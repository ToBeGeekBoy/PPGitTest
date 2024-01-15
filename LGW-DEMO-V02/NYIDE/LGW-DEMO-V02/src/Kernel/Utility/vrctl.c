#include "include.h"
#include "nx1_adc.h"
#include "nx1_gpio.h"
#include "nx1_spi.h"
#include "nx1_fmc.h"
#include "nx1_wdt.h"
#include "VRCmdTab.h"
#include "utility_define.h"
#include "debug.h"
#include <string.h>

//#define DEBUG_SD
#ifdef  DEBUG_SD
#define Dprintf dprintf
#else
#define Dprintf(...) 
#endif

#if _VR_FUNC 

#define DC_REMOVE						1

#define BUF0_READY          			0
#define BUF1_READY          			1
#define BUF_NOTREADY       				2
#define BUF0_OFFSET        				0    
#define BUF1_OFFSET         			(BUF0_OFFSET+(BUF_SIZE*2))
#define ENGINE_BUF_OFFSET   			(BUF1_OFFSET+(BUF_SIZE*2))

#define MAX_TIME						300
#define MAX_COMBO_NUM					3							// restricted by the third party API

#define CSPOTTER_SUCCESS				(0)
#define CSPOTTER_ERR_SDKError			(-2000)
#define CSPOTTER_ERR_NeedMoreSample		(CSPOTTER_ERR_SDKError - 9)

#if _VR_STORAGE_TYPE
#define UTTR_BUF_SIZE					0x1000
#define UDR_VR_MODE						SPI_MODE
#define UDR_VR_READ(a,b,c)				SPI_BurstRead(a, b, c)
#else
#define UTTR_BUF_SIZE					0x2000
#define UDR_VR_MODE						EF_MODE
#define UDR_VR_READ(a,b,c)				memcpy(a,(void*)b, c);
#endif

static S16 *Buf0,*Buf1;
static U8 BufToggle;
static U16 BufIdx;
static U8 *CyBaseAddr, **ModelAddr;
static U8 numGroup;
static U8 VRStateRam[VR_STATE_SIZE] = {0};
static volatile U8 BufReady;
#if _VR_GET_LOADING
static U8 loadingInterval, adcFifoSize, loadingCount, loadingCountMax;
static U16 loadingPercentage;
#endif

U32 hCSpotter;
#if _VR_VOICE_TAG
U32 *hCSpotterSD;
U8* SDModel;
#if _VR_VOICE_PASSWORD
U8* SVModel;
S32 SVscore, SVTagId;
#endif
#if _VR_VOICE_PASSWORD
static U8 numTrainingUttr = 1;
#else
static U8 numTrainingUttr = 3;
#endif
#endif
U8 VRState;
U8 VR_FuncType = VR_SI_Func;
U8 VR_GroupState = 0;

#if	_VR_GET_SCORE
S32 CmdId[10]={-100,-100,-100,-100,-100,-100,-100,-100,-100,-100};
S32 Score[10]={-100,-100,-100,-100,-100,-100,-100,-100,-100,-100};
#endif
#if	_VR_HIT_SCORE
S8 VR_HitScore;
#endif
#if _QCODE_BUILD
S8 VR_HitId = -1;
#endif
#if DC_REMOVE
static S32 RunFilterBuf[8];
#endif
/* Global Word for Inline C-------------------------------------------------------*/
#if _VR_VAD
U16 g_vr_id_hold_time;
U16 g_vr_mute_hold_time;
static U32 time_VadActive, time_VadMute;
#endif
/*--------------------------------------------------------------------------------*/


/* Dynamic Allocate RAM-------------------------------------------------------*/
#if _USE_HEAP_RAM
static U8 *VRRam=NULL;
#else
static U8 VRRam[_VR_RAM_SIZE];
#endif

/* Extern Functions ----------------------------------------------------------*/
extern void CB_VR_ApiPresetting(void);
extern S8 CB_VR_VoiceTag_CreateOrDelete(U8 input, U8 action);
extern void CB_VR_TrainTag_PromptAction_BeforeTrainingStart(void);
extern void CB_VR_TrainTag_PromptAction_BeforeSayingTag(U8 index);
extern void CB_VR_TrainTag_BeforeTrainWord(void);
extern void CB_VR_TrainTag_AfterTrainWord(void);
#if _QCODE_BUILD
extern void CB_VR_TrainTag_AudioActionPlayWaiting(void);
#endif

#if _VR_VAD
//------------------------------------------------------------------//
// VR_VAD Get ID Hold Time Parameter
// Parameter: 
//          NONE
// return value:
//          ID Hold Time Setting(From XIP or InternalRom)
//------------------------------------------------------------------//
U16 VR_VAD_GetIdHoldTime(void)
{
#if _SPI_MODULE
    return XIP_VR_VAD_Parameters(0);
#else
    return VR_ID_HOLD_TIME;
#endif
}
//------------------------------------------------------------------//
// VR_VAD Get Mute Hold Time Parameter
// Parameter: 
//          NONE
// return value:
//          Mute Hold Time Setting(From XIP or InternalRom)
//------------------------------------------------------------------//
U16 VR_VAD_GetMuteHoldTime(void)
{
#if _SPI_MODULE
    return XIP_VR_VAD_Parameters(1);
#else
    return VR_MUTE_HOLD_TIME;
#endif
}
#endif //#if _VR_VAD

#if _QCODE_BUILD && _VR_HIT_SCORE
//------------------------------------------------------------------//
// Get hit Score
// Parameter:
//          None
// return value:
//          return hit ID
//------------------------------------------------------------------//
S8 VR_GetHitScore(void)
{
    S8 temp = VR_HitScore;
    VR_HitScore = -1;
    return (VRState==VR_ON)?temp:-1;
}
#endif
#if _QCODE_BUILD
//------------------------------------------------------------------//
// Set hit ID
// Parameter:
//          hit ID
// return value:
//          None
//------------------------------------------------------------------//
void VR_SetHitId(S8 id)
{
    VR_HitId = id;
}
//------------------------------------------------------------------//
// Get hit ID
// Parameter:
//          None
// return value:
//          return hit ID
//------------------------------------------------------------------//
S8  VR_GetHitId(void)
{
    S8 temp = VR_HitId;
    VR_HitId = -1;
    return (VRState==VR_ON)?temp:-1;
}
#endif
//------------------------------------------------------------------//
// VR Get Ram Size Parameter
// Parameter: 
//          NONE
// return value:
//          RAM_SIZE Setting(From XIP or InternalRom)
//------------------------------------------------------------------//
U16 VR_GetRamSize(void)
{
#if _SPI_MODULE && _XIP_COC && _VR_FUNC && _VR_STORAGE_TYPE
    return COC_VR_RamSize();
#else
    return _VR_RAM_SIZE;
#endif
}

//------------------------------------------------------------------//
// VR Get Engine Ram Size Parameter
// Parameter: 
//          NONE
// return value:
//          ENGINE_RAM_SIZE Setting(From XIP or InternalRom)
//------------------------------------------------------------------//
U16 VR_GetEngineRamSize(void)
{
#if _SPI_MODULE && _XIP_COC && _VR_FUNC && _VR_STORAGE_TYPE
    return COC_VR_EngineRamSize();
#else
    return _VR_ENGINE_RAM_SIZE;
#endif
}

//------------------------------------------------------------------//
// VR Get Timeout Parameter
// Parameter: 
//          NONE
// return value:
//          TIMEOUT Setting(From XIP or InternalRom)
//------------------------------------------------------------------//
U32 VR_GetTimeOut(void)
{
#if _SPI_MODULE && _XIP_COC && _VR_FUNC && _VR_STORAGE_TYPE
    return COC_VR_TimeOut();
#else
    return _VR_TIMEOUT;
#endif
}

//------------------------------------------------------------------//
// Allocate VR memory
// Parameter: 
//          func: Choose Allocate function
// return value:
//          NONE
//------------------------------------------------------------------// 
#if _USE_HEAP_RAM
static void VR_AllocateMemory(U16 func)
{
    if(VRRam==NULL && (func&MEMORY_ALLOCATE_VR))    //VRRam
    {
		if(VR_FuncType == VR_SI_Func)
			VRRam = (U8 *)MemAlloc(VR_GetRamSize());
	#if _VR_VOICE_TAG
		else if(VR_FuncType == VR_SD_Func)
			VRRam = (U8 *)MemAlloc(_VR_SD_RAM_SIZE);
	#endif
    }
    //else
    //	dprintf("Allocate twice. Ignore...\r\n");
}
//------------------------------------------------------------------//
// Free VR memory
// Parameter: 
//          func: Choose Allocate function
// return value:
//          NONE
//------------------------------------------------------------------// 
static void VR_FreeMemory(U16 func)
{
    if(VRRam!=NULL && (func&MEMORY_ALLOCATE_VR))    
    {
        MemFree(VRRam);
        VRRam=NULL;
        //dprintf("VR Free Mem\r\n");
    }
    //else
    //{
    //    dprintf("VR Already Free\r\n");
    //}
}
#endif


#if _VR_STORAGE_TYPE
static U32 SpiReadAddr = 0;
static U32 SpiBuf;
#endif

U32 SPIMgr_Read(U8 *dest, U8 *src, U32 nSize)
{
#if _VR_STORAGE_TYPE    
	if (((U32)src == SpiReadAddr) && (nSize == 4)){
		*(U32 *)dest = SpiBuf;
	}
	else {
	#if _VR_VOICE_TAG
		if (VR_FuncType == VR_SD_Func){
			P_WDT->CLR = C_WDT_CLR_Data;
		}
	#endif
		
#define spiReadEZ	0
#if spiReadEZ == 1		
		SPI_FastReadStart((U32)src);
#else
		SPI0_CS_PORT->Data&=~(1<<SPI0_CS_PIN);
		
		SPI_TypeDef *SPI = P_SPI0;
		
	#if (_SPI_ACCESS_MODE==SPI_1_1_1_MODE) || (_EF_SERIES && (_SPI_ACCESS_MODE==SPI_1_1_1_MODE_3WIRE))
		SPI->Data=SPI_READ_CMD;
	#elif _SPI_ACCESS_MODE==SPI_1_2_2_MODE    
		SPI->Data=SPI_2READ_CMD;
	#elif _SPI_ACCESS_MODE==SPI_1_1_2_MODE
		SPI->Data=SPI_DUAL_READ_CMD;
	#elif _SPI_ACCESS_MODE==SPI_1_4_4_MODE
		SPI->Data=SPI_4READ_CMD;
	#endif		
		
		SPI->Ctrl &= (~(C_SPI_FIFO_Length | C_SPI_DataMode));
		SPI->Ctrl |= C_SPI_Tx_Start | SPI_DATA_1;
		
		do{
		}while(SPI->Ctrl&C_SPI_Tx_Busy);
	#if _SPI_ADDR_BYTE==3
		SPI->Data=(((U32)src&0xFF)<<16)|((U32)src&0xFF00)|((U32)src>>16);
	#elif _SPI_ADDR_BYTE==4
		SPI->Data=(((U32)src&0x000000FF)<<24)|(((U32)src&0x0000FF00)<<8)|(((U32)src&0x00FF0000)>>8)|(((U32)src&0xFF000000)>>24);
	#else
		#error "_SPI_ADDR_BYTE setting error"
	#endif
	#if (_SPI_ADDR_BYTE==3 && _SPI_ACCESS_MODE==SPI_1_4_4_MODE) || (_SPI_ADDR_BYTE==4 && (_SPI_ACCESS_MODE==SPI_1_2_2_MODE || _SPI_ACCESS_MODE==SPI_1_1_2_MODE || _SPI_ACCESS_MODE==SPI_1_4_4_MODE))
		SPI->Data=0;
	#endif
		SPI->Ctrl &= (~(C_SPI_FIFO_Length | C_SPI_DataMode));
	#if (_SPI_ACCESS_MODE==SPI_1_1_1_MODE) || (_EF_SERIES && (_SPI_ACCESS_MODE==SPI_1_1_1_MODE_3WIRE))
		SPI->Ctrl |= (C_SPI_Tx_Start |((_SPI_ADDR_BYTE+0-1)<<SPI_DATA_NUM_OFFSET)| SPI_DATA_1); 
	#elif _SPI_ACCESS_MODE==SPI_1_2_2_MODE    
		SPI->Ctrl |= (C_SPI_Tx_Start |((_SPI_ADDR_BYTE+1-1)<<SPI_DATA_NUM_OFFSET)| SPI_DATA_2); 
	#elif _SPI_ACCESS_MODE==SPI_1_1_2_MODE
		SPI->Ctrl |= (C_SPI_Tx_Start |((_SPI_ADDR_BYTE+1-1)<<SPI_DATA_NUM_OFFSET)| SPI_DATA_1); 
	#elif _SPI_ACCESS_MODE==SPI_1_4_4_MODE
		SPI->Ctrl |= (C_SPI_Tx_Start |((_SPI_ADDR_BYTE+3-1)<<SPI_DATA_NUM_OFFSET)| SPI_DATA_4); 
	#endif		

		do{
		}while(SPI->Ctrl&C_SPI_Tx_Busy);
		
		SPI->Ctrl &= (~(C_SPI_DataMode));
	#if (_SPI_ACCESS_MODE==SPI_1_1_1_MODE) || (_EF_SERIES && (_SPI_ACCESS_MODE==SPI_1_1_1_MODE_3WIRE))
		SPI->Ctrl |= SPI_DATA_1;
	#elif _SPI_ACCESS_MODE==SPI_1_2_2_MODE    
		SPI->Ctrl |= SPI_DATA_2;
	#elif _SPI_ACCESS_MODE==SPI_1_1_2_MODE
		SPI->Ctrl |= SPI_DATA_2;
	#elif _SPI_ACCESS_MODE==SPI_1_4_4_MODE
		SPI->Ctrl |= SPI_DATA_4;
	#endif		
#endif
		SPI_FastRead((U32 *)dest, nSize);
#if spiReadEZ == 1	
		GPIO_Write(SPI0_CS_PORT, SPI0_CS_PIN, 1); 
#else
		SPI0_CS_PORT->Data|=(1<<SPI0_CS_PIN);
#endif
		
		if (nSize == 4) {
			SpiReadAddr = (U32)src;
			SpiBuf = *(U32 *)dest;
		}
	}
#endif
    return 0;
}

U32 SPIMgr_ReadNoWait(U8 *dest, U8 *src, U32 nSize)
{
#if _VR_STORAGE_TYPE   
	SPIMgr_Read(dest, src, nSize);
#endif
    return 0;
}

U32 SPIMgr_Erase(U8 *dest, U32 nSize)
{
#if _VR_STORAGE_TYPE 
	//dprintf("E 0x%x size %d\r\n",(U32)dest, nSize);
	SPI_BurstErase_Sector((U32)dest, nSize);
#endif
	return 0;
}

U32 SPIMgr_Write(U8 *dest, U8 *src, U32 nSize)
{
#if _VR_STORAGE_TYPE 
	//dprintf("Write 0x%x size %d\r\n",(U32)dest, nSize);
	SPI_BurstWrite(src, (U32)dest, nSize);
#endif
	return 0;
}

U32 DataFlash_Erase(U8 *lpbyDest, U32 nSize)
{
#if	defined(P_FMC)
	//dprintf("E 0x%x size %d\r\n",(U32)lpbyDest, nSize);
	FMC_BurstErase_Sector((U32)lpbyDest, nSize);
#endif
	return 0;
}

U32 DataFlash_Write(U8 *lpbyDest, U8 *lpbySrc, U32 nSize)
{
#if	defined(P_FMC)
	//dprintf("Write 0x%x size %d\r\n",(U32)lpbyDest, nSize);
	FMC_BurstWrite(lpbySrc, (U32)lpbyDest, nSize);
#endif
	return 0;
}
//------------------------------------------------------------------//
// Adc ISR
// Parameter: 
//          size: data-buffer fill size 
//       pdest32: data-buffer address
// return value:
//          NONE
//------------------------------------------------------------------// 
void VR_AdcIsr(U8 size, U32 *pdest32){
    U8 i;
    S16 temp,*buf;
    
    if(VRState==VR_ON){
        if(!BufToggle){
            buf=Buf0+BufIdx;
        }else{
            buf=Buf1+BufIdx;
        }        
        
        for(i=0;i<size;i++){
            temp=((*pdest32++)-32768);//ADC data is unsigned integer. convert it to signed integer.
            
            *buf++=temp;
        }   
        BufIdx+= size;
        
        if(BufIdx==BUF_SIZE){
            if(BufToggle){
                BufReady=BUF1_READY;
            }else{
                BufReady=BUF0_READY;
            }        
            BufToggle^=1;
            BufIdx=0;
        }    
	#if _VR_GET_LOADING
		if(loadingInterval){
			adcFifoSize = size;
			loadingCount++;
		}
	#endif
    }
}
//------------------------------------------------------------------//
// Set command model
// Parameter: 
//          cybase: cybase address
//   		command_model: model array address
// return value:
//          NONE
//------------------------------------------------------------------//     
static void VR_Init(U8 *cybase, U8 **command_model){
    U32 n;
	S32 nErr;
	U8 i = 0;
	U8 *modelplus[MAX_COMBO_NUM];

	U16 engineramsize = VR_GetEngineRamSize();

	for(i = 0; i <numGroup; i++)
	{
		modelplus[i] = *(command_model + i);
	}

#if _VR_VOICE_TAG && _VR_VOICE_PASSWORD
	if((modelplus[numGroup-1] == SDModel) && VR_GetNumWord(SVModel))
	{
		n = CSPOTTER_X_GetMemoryUsage_MultiWithSpeaker(cybase, modelplus, numGroup, MAX_TIME, SVModel);
		//dprintf("SV Mem= %d\r\n", n);
	} else 
#endif
	{
		n = CSPOTTER_X_GetMemoryUsage_Multi(cybase, modelplus, numGroup, MAX_TIME);
		//dprintf("SI Mem= %d\r\n", n);
	}
	if(n>engineramsize){
		while(1);
	}    	
	hCSpotter = (U32)CSPOTTER_X_Init_Multi(cybase, modelplus, numGroup, MAX_TIME, (U8 *)(VRRam+ENGINE_BUF_OFFSET), engineramsize, (U8 *)(VRStateRam), VR_STATE_SIZE, &nErr);

#if	_VR_GET_SCORE
	CSPOTTER_X_SetEnableNBest((HANDLE)hCSpotter, ENABLE);
#endif

	CSPOTTER_X_Reset((HANDLE)hCSpotter);
}    
//------------------------------------------------------------------//
// Voice Recogniztion start
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
static void VR_Start_(void){   
    U8 sts;
    
    if(VRState==VR_ON){
        VR_Stop();        
    }    
    
	sts = AUDIO_GetStatus_All();

    if(sts != STATUS_STOP){
        VRState=VR_HALT;
        return;
    }        
#if _USE_HEAP_RAM
	else
	{
		AUDIO_ServiceLoop();
	}
    VR_AllocateMemory(MEMORY_ALLOCATE_VR);
#endif
    Buf0=(S16 *)(VRRam+BUF0_OFFSET);
    Buf1=(S16 *)(VRRam+BUF1_OFFSET);
    
    BufIdx=0;
    BufToggle=0;
    BufReady=BUF_NOTREADY;
    
    InitAdc(_VR_SAMPLE_RATE);

#if DC_REMOVE
	memset((void *)RunFilterBuf, 0, 8<<2);
#endif

#if _QCODE_BUILD && _VR_VAD
    VadInit();
    time_VadActive = 0;
	time_VadMute = 0;
#else
#if _VR_VAD || _VR_VAD_TIMEOUT_EXTENSION
    VadInit();
#if _VR_VAD
	time_VadActive = 0;
	time_VadMute = 0;
    g_vr_id_hold_time 	= VR_VAD_GetIdHoldTime();
    g_vr_mute_hold_time	= VR_VAD_GetMuteHoldTime();
#endif	
#endif
#endif
#if _VR_GET_LOADING
	loadingCountMax = 0;
	loadingCount = 0;
#endif

    VR_Init(CyBaseAddr,ModelAddr);

	CB_VR_ApiPresetting();
	
    VRState=VR_ON;
    ADC_Cmd(ENABLE);
}   
//------------------------------------------------------------------//
// Voice Recogniztion start
// Parameter: 
//          cybase:cybase table address
//          model:model array address
// return value:
//          NONE
//------------------------------------------------------------------//     
void VR_Start(U8 *cybase, U8 **model){
	CyBaseAddr = cybase;
    ModelAddr = model;

    VR_Start_();    
}
#if _QCODE_BUILD
//------------------------------------------------------------------//
// Voice Recogniztion start
// Parameter: 
//          cybase:cybase table address
//          model:model array address
// return value:
//          NONE
//------------------------------------------------------------------//     
void VR_SetModel(U8 *cybase, U8 **model){

	CyBaseAddr = cybase;
    ModelAddr = model;
    if(VRState==VR_ON)
    {
        VR_Halt();
    }
#if _VR_HIT_SCORE
    VR_HitScore = -1;
#endif
}
#endif
//------------------------------------------------------------------//
// Voice Recogniztion halt
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
void VR_Halt(void){

    if((VRState==VR_ON) && (VR_FuncType == VR_SI_Func)){	// bypass VR_SD_Func
		VR_ApiRelease(VR_SI_Func);
	#if _USE_HEAP_RAM
		VR_FreeMemory(MEMORY_ALLOCATE_VR);   
	#endif					
        ADC_Cmd(DISABLE);
        VRState=VR_HALT;
    }
}    
//------------------------------------------------------------------//
// Voice Recogniztion stop
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
void VR_Stop(void){
    
    if(VRState==VR_ON){    
		VR_ApiRelease(VR_SI_Func);
	#if _USE_HEAP_RAM
        VR_FreeMemory(MEMORY_ALLOCATE_VR);   
	#endif    		
        ADC_Cmd(DISABLE);
    }
    VRState=VR_OFF;
}  

//------------------------------------------------------------------//
// VR Service loop
// Parameter: 
//          NONE
// return value:
//          -1: no command
//			-2: unknown command when _VR_VAD and _VR_UNKNOWN_COMMAND are set ENABLE
//      others: command index  
//------------------------------------------------------------------//     
S8 VR_ServiceLoop(void){
#if _VR_VAD
	S32 sts;
	static S8 id=-1;
	static U32 time_getId = 0;
#if	_VR_HIT_SCORE
	static S8 tempHitScore;
#endif	 
#else
    S32 sts;
    S8 id;
#endif
	
    if(VRState==VR_ON){
        sts=1;
      	//GPIOA->Data |= (1<<8);
        if(BufReady==BUF0_READY)
        {
		#if _VR_GET_LOADING
			loadingInterval = 1;
			loadingCount = 0;
		#endif
            BufReady=BUF_NOTREADY;
		#if DC_REMOVE
			U16 i;
			for(i=0;i<BUF_SIZE;i++)//unit word
			{
				RunFilter(&Buf0[i], RunFilterBuf, DCRemove);
			}
		#endif
            //process buf0 here
		#if _VR_VAD || _VR_VAD_TIMEOUT_EXTENSION
            VadProcess(Buf0,BUF_SIZE);
		#endif
            sts=CSPOTTER_X_AddSample((HANDLE)hCSpotter, Buf0, BUF_SIZE);			
        }
        else if(BufReady==BUF1_READY)
        {
		#if _VR_GET_LOADING
			loadingInterval = 1;
			loadingCount = 0;
		#endif
            BufReady=BUF_NOTREADY;
		#if DC_REMOVE
			U16 i;
			for(i=0;i<BUF_SIZE;i++)//unit word
			{
				RunFilter(&Buf1[i], RunFilterBuf, DCRemove);
			}
		#endif
            //process buf1 here
		#if _VR_VAD || _VR_VAD_TIMEOUT_EXTENSION
            VadProcess(Buf1,BUF_SIZE);	
		#endif
            sts=CSPOTTER_X_AddSample((HANDLE)hCSpotter, Buf1, BUF_SIZE);				
        } 
	#if _VR_GET_LOADING
		loadingInterval = 0;
		if(loadingCount>loadingCountMax){
			loadingCountMax = loadingCount;
			loadingPercentage = loadingCountMax*adcFifoSize*100/BUF_SIZE;
		}
	#endif
        //GPIOA->Data &= ~(1<<8);

	#if _VR_VAD		
		if(sts==0){
			id = CSPOTTER_X_GetResult((HANDLE)hCSpotter);
		#if _VR_VOICE_TAG && _VR_VOICE_PASSWORD
			SVTagId = CSpotter16_GetSpeakerResult((HANDLE)hCSpotter, &SVscore);
			if(SVTagId>=0) {
				SVTagId = CSPOTTERSD_X_GetTagID(SVModel, SVTagId);
			}else{
				SVTagId = -1;
				SVscore = -1;
			}
		#endif
		#if	_VR_GET_SCORE
			CSPOTTER_X_GetNBestScore((HANDLE)hCSpotter, CmdId, Score, 10);
		#endif	
		#if	_VR_HIT_SCORE
			tempHitScore = CSPOTTER_X_GetResultScore((HANDLE)hCSpotter);
		#endif	
			CSPOTTER_X_Reset((HANDLE)hCSpotter);
			
			time_getId = SysTick;
		#if _QCODE_BUILD
            if(VR_isVadEnable()==false)
            {
                S8 tempId = id;
                VR_SetHitId(id);
		#if	_VR_HIT_SCORE
                VR_HitScore = tempHitScore;
		#endif	
			    Dprintf("VR id = %d VAD off\r\n", id);
                id = -1;
                return tempId;;
            }
		#endif
			Dprintf("VR id = %d, Time = %d\r\n", id, SysTick);
		}
	
		if((SysTick - time_getId) > g_vr_id_hold_time){
		#if	_VR_HIT_SCORE
			tempHitScore = -1;
		#endif
			id = -1;
		}	
	
		U8 VadSts = VR_GetVadSts2();								// VAD action: 00, 10, 11, 00...
		if(VadSts == 1){
			time_VadActive = SysTick;
			time_VadMute = 0;
			Dprintf("VR VAD Active, Time = %d\r\n", SysTick);
		}else if(VadSts == 2){
			time_VadMute = SysTick;
			Dprintf("VR VAD Mute, Time = %d\r\n", SysTick);	
		}
		
		if(time_VadActive) {
		#if	_VR_HIT_SCORE && _QCODE_BUILD==0
			VR_HitScore = -1;
		#endif			
			if(id >= 0) {
				time_VadActive = 0;
				time_VadMute = 0;
			#if	_VR_HIT_SCORE
				VR_HitScore = tempHitScore;
			#endif
				S8 tempId = id;
			#if _QCODE_BUILD
                VR_SetHitId(id);
			#endif
				id = -1;
				return tempId;
			}else if(time_VadMute && (SysTick - time_VadMute) > g_vr_mute_hold_time) {		
				time_VadActive = 0;	
				time_VadMute = 0;		
				Dprintf("VR VAD Timeout..., Time = %d\r\n", SysTick);
			#if _VR_UNKNOWN_COMMAND	
				return -2;
			#endif
			}
			return -1;
		}else{
		#if	_VR_HIT_SCORE && _QCODE_BUILD==0
			VR_HitScore = -1;
		#endif
			return -1;
		}
		
	#else 
        if(sts==0){
            id = CSPOTTER_X_GetResult((HANDLE)hCSpotter);
		#if _VR_VOICE_TAG && _VR_VOICE_PASSWORD
			SVTagId = CSpotter16_GetSpeakerResult((HANDLE)hCSpotter, &SVscore);
			if(SVTagId>=0) {
				SVTagId = CSPOTTERSD_X_GetTagID(SVModel, SVTagId);
			}else{
				SVTagId = -1;
				SVscore = -1;
			}
		#endif
		#if _QCODE_BUILD
            VR_SetHitId(id);
		#endif
		#if	_VR_GET_SCORE
			CSPOTTER_X_GetNBestScore((HANDLE)hCSpotter, CmdId, Score, 10);
		#endif	
		#if	_VR_HIT_SCORE
			VR_HitScore = CSPOTTER_X_GetResultScore((HANDLE)hCSpotter);
		#endif
			CSPOTTER_X_Reset((HANDLE)hCSpotter);
            return id;	
			
        }else{
		#if	_VR_HIT_SCORE && _QCODE_BUILD==0
			VR_HitScore = -1;
		#endif
            return -1;
        }
	#endif
		
    }else if(VRState==VR_HALT){

		sts = AUDIO_GetStatus_All();	
        if(sts == STATUS_STOP){
            VR_Start_();
        }    
    }  
    return -1;  
}

//------------------------------------------------------------------//
// Set GroupState and Number of Groups
// Parameter: 
//          groupState: VR_TRIGGER_STATE, VR_CMD_STATE_1, VR_CMD_STATE_2, ... VR_CMD_STATE_10
//          numModel: number of groups, value is 1 ~ MAX_COMBO_NUM
// return value:
//          NONE
//------------------------------------------------------------------//  
void VR_SetGroupState(U8 groupState, U8 numModel)
{
	VR_GroupState = groupState;
	if (numModel > MAX_COMBO_NUM){
		numModel = MAX_COMBO_NUM;
	}	
	numGroup = numModel;
}

//------------------------------------------------------------------//
// Get the number of commands from the model
// Parameter: 
//          model_addr:cybase table address
// return value:
//          0: no any command
//			>0: number of commands 
//------------------------------------------------------------------//  
U8 VR_GetNumWord(U8* model_addr)
{
	S32 nNum = CSPOTTER_X_GetNumWord(model_addr);
	if (nNum > 0)
		return nNum;
	else 
		return 0;
}

//------------------------------------------------------------------//
// Release the resource of Cyberon API
// Parameter: 
//          vrfunctype: VR function, VR_SI_Func or VR_SD_Func
// return value:
//          NONE
//------------------------------------------------------------------//  
void VR_ApiRelease(U8 vrfunctype)
{
	if ((vrfunctype == VR_SI_Func) && (hCSpotter != (U32)NULL)) {
		CSPOTTER_X_Release((HANDLE)hCSpotter);
		hCSpotter = (U32)NULL;
	}
#if _VR_VOICE_TAG
	else if ((vrfunctype == VR_SD_Func) && (hCSpotterSD != NULL)) {
		CSpotterSD16_Release(hCSpotterSD);
		hCSpotterSD = NULL;
	}
#endif		
		
}

#if _VR_VOICE_TAG
//------------------------------------------------------------------//
// Delete a voice tag from the model
// Parameter: 
//          lpbySDModel: SD model address
//          nModelSize: SD model size
//          nTagID: the voice tad ID
// return value:
//          0: success
//		   	<0: error code
//------------------------------------------------------------------// 
S32 DoVR_DelTag(U8* lpbySDModel, U32 nModelSize, U8 nTagID) {
	
	U32 nSize;
	S32 nErr;

	if ((nErr = CSPOTTERSD_X_DeleteWord(hCSpotterSD, lpbySDModel, nModelSize, nTagID, &nSize)) != CSPOTTER_SUCCESS )
		return nErr;
	//dprintf("nSize= %d\r\n", nSize);
	return nSize;
}

//------------------------------------------------------------------//
// Train a voice tag into the model
// Parameter: 
//          lpbySDModel: SD model address
//          nModelSize: SD model size
//			lpbySDBuffer: SD training Buffer address
//			nSDBufSize: SD training Buffer size
//          nTagID: the voice tad ID
//			nTimeOut: time out timer
// return value:
//		   	<0: error code
//     		>0: total voice tag size
//------------------------------------------------------------------// 
#if _VR_VOICE_PASSWORD
S32 DoVR_TrainTag(U8* lpbySDModel, U32 nModelSize, U8* lpbySDBuffer, U32 nSDBufSize, U8* lpbySVModel, U8 nTagID, U16 nTimeOut) {
#else
S32 DoVR_TrainTag(U8* lpbySDModel, U32 nModelSize, U8* lpbySDBuffer, U32 nSDBufSize, U8 nTagID, U16 nTimeOut) {	
#endif
	
	S32 nErr = 0;
	U32 nWait, nSize;
	U8 i;

	InitAdc(_VR_SAMPLE_RATE);
	ADC_Cmd(ENABLE);

#if _QCODE_BUILD==0
	CB_VR_TrainTag_PromptAction_BeforeTrainingStart();
#endif
	
	while (AdcEnFlag)
	{
		AUDIO_ServiceLoop_Rightnow();	// wait ADC stable
	}
	
	// Start training
	for (i = 0; i < numTrainingUttr; i++) {

		CB_VR_TrainTag_PromptAction_BeforeSayingTag(i);
	
		if ((nErr = CSPOTTERSD_X_AddUttrStart(hCSpotterSD, i, lpbySDBuffer + (i * UTTR_BUF_SIZE), UTTR_BUF_SIZE)) != CSPOTTER_SUCCESS)
		{
			return nErr;
		}	

		Buf0=(S16 *)(VRRam+BUF0_OFFSET);
		Buf1=(S16 *)(VRRam+BUF1_OFFSET);
		BufIdx = 0;
		BufToggle = 0;
		BufReady = BUF_NOTREADY;
		VRState=VR_ON;
	#if DC_REMOVE
		memset((void *)RunFilterBuf, 0, 8<<2);
	#endif
		
		nWait = 0;
		while (1) {
			
			if (BufReady == BUF_NOTREADY) {
				continue;
			}
			
			else if (BufReady == BUF0_READY) {
			#if DC_REMOVE
				U16 i;
				for(i=0;i<BUF_SIZE;i++)//unit word
				{
					RunFilter(&Buf0[i], RunFilterBuf, DCRemove);
				}
			#endif
				BufReady = BUF_NOTREADY;
				//process buf0 here
				if ((nErr = CSPOTTERSD_X_AddSample(hCSpotterSD, Buf0, BUF_SIZE)) != CSPOTTER_ERR_NeedMoreSample) {
					break;
				}
			}
			
			else if (BufReady == BUF1_READY) {
			#if DC_REMOVE
				U16 i;
				for(i=0;i<BUF_SIZE;i++)//unit word
				{
					RunFilter(&Buf1[i], RunFilterBuf, DCRemove);
				}
			#endif
				BufReady = BUF_NOTREADY;
				//process buf1 here
				if ((nErr = CSPOTTERSD_X_AddSample(hCSpotterSD, Buf1, BUF_SIZE)) != CSPOTTER_ERR_NeedMoreSample) {
					break;
				}
			}

			nWait++;
			if (nWait >= nTimeOut)
			{           
				return nErr;
			}
			
			WDT_Clear();
		}
		//dprintf("nWait = %d\r\n", nWait);

		if ((nErr = CSPOTTERSD_X_AddUttrEnd(hCSpotterSD)) != CSPOTTER_SUCCESS)
		{          
			return nErr;
		}
		
		//U32 nStart, nEnd;
		//CSpotterSD16_GetUttrEPD(hCSpotterSD, &nStart, &nEnd);
		//dprintf("Uttr %d: %d msec\r\n", i, (nEnd-nStart)/16);
		
	#if TRAINING_DIFF_CHECK
		if ((i > 0))
		{
			S32 temp = CSPOTTERSD_X_DifferenceCheckWithRejLevel(hCSpotterSD, -10);
			if (temp != CSPOTTER_SUCCESS)
			{
				i--; // try again after for loop i++
				// different check action
			}
		}
	#endif
	}

	CB_VR_TrainTag_BeforeTrainWord();
#if _VR_VOICE_PASSWORD	
	if ((nErr = CSPOTTERSD_X_TrainWordForSpeaker(hCSpotterSD, lpbySVModel, 0x2000, PASSWORD_ID1, &nSize)) != CSPOTTER_SUCCESS)	// 8K and 4K buffer
	{
#if _QCODE_BUILD
		CB_VR_TrainTag_AfterTrainWord();
#endif
		return nErr;
	}
#endif
	if ((nErr = CSPOTTERSD_X_TrainWord(hCSpotterSD, lpbySDModel, nModelSize, nTagID, 1, &nSize)) != CSPOTTER_SUCCESS)
	{
#if _QCODE_BUILD
		CB_VR_TrainTag_AfterTrainWord();
#endif
		return nErr;
	}
	CB_VR_TrainTag_AfterTrainWord();

	return nSize;
}

//------------------------------------------------------------------//
// Set the number of training utterances
// Parameter: 
//			numUttr: the the number of training utterances, value is 1 ~ 3
// return value:
//          NONE
//------------------------------------------------------------------//  
void VR_SetNumTrainingUttr(U8 numUttr) 
{
	if(numUttr > 3){
		numUttr = 3;	
	}else if (numUttr < 1){
		numUttr = 1;
	}
	numTrainingUttr = numUttr;
}

//------------------------------------------------------------------//
// Flow of voice tag
// Parameter: 
//			input: Tag input data
//			action: =0 Delete MODEL; =1 Creat ID; =2 Delete ID;
// return value:
//          DO_NOTHING or nReturnCode
//------------------------------------------------------------------// 
S8 VR_Train(U8 input, U8 action) {
	
	S32 nErr;
#if _QCODE_BUILD==0
    if(AUDIO_GetStatus_All() != STATUS_STOP) return DO_NOTHING;
#endif
	VR_ApiRelease(VR_SI_Func);
	VR_FuncType = VR_SD_Func;
	
	U8* lpbyBaseModel = (U8*)VR_GroupAddr(0);		// MUSET choose 1 cybase file
    
	// Training_Engine initial
/*	U32 n = CSPOTTERSD_X_GetMemoryUsage(lpbyBaseModel, _VR_VOICE_TAG&_VR_VOICE_PASSWORD);
	dprintf("Training Mem:%d\r\n", n);
	if (n > _VR_SD_ENGINE_RAM_SIZE)
		while(1); */
	
#if _USE_HEAP_RAM	
	VR_FreeMemory(MEMORY_ALLOCATE_VR); 
#if _QCODE_BUILD
    CB_VR_TrainTag_AudioActionPlayWaiting();
#endif
	VR_AllocateMemory(MEMORY_ALLOCATE_VR);
#endif
	
	hCSpotterSD = CSPOTTERSD_X_Init(lpbyBaseModel, (VRRam+ENGINE_BUF_OFFSET), _VR_SD_ENGINE_RAM_SIZE, &nErr);

	S8 nReturnCode = CB_VR_VoiceTag_CreateOrDelete(input, action);

    SYS_ClrSleepTime();

	VR_ApiRelease(VR_SD_Func);
	VR_FuncType = VR_SI_Func;
#if _USE_HEAP_RAM	
	VR_FreeMemory(MEMORY_ALLOCATE_VR);
#endif
 	ADC_Cmd(DISABLE);
    VRState=VR_OFF;

	return nReturnCode;
}
//------------------------------------------------------------------//
// Creat a voice tag. If it exist, it will be deleted first.
// Parameter: 
//			TAGID: Tag input data
// return value:
//          DO_NOTHING or nReturnCode
//------------------------------------------------------------------// 
S8 VR_VoiceTag_CreatID(U8 TAGID)
{
	return VR_Train(TAGID, TRAIN_CREATID);
}
//------------------------------------------------------------------//
// Delete a voice tag
// Parameter: 
//			TAGID: Tag input data
// return value:
//          DO_NOTHING or nReturnCode
//------------------------------------------------------------------// 
S8 VR_VoiceTag_DeleteID(U8 TAGID)
{
	return VR_Train(TAGID, TRAIN_DELETEID);
}
//------------------------------------------------------------------//
// Delete voice tag model space
// Parameter: 
//          NONE
// return value:
//          DO_NOTHING or nReturnCode
//------------------------------------------------------------------// 
S8 VR_VoiceTag_DeleteMODEL(void)
{
	return VR_Train(0, TRAIN_DELETEMODEL);      		   
}
#endif
//------------------------------------------------------------------//
// Get VR group address from internal ROM or SPI Flash
// Parameter: 
//          groupindex: 0 = cybase; 
//          			others = VR group 
// return value:
//          0: error address
//			>0: VR group address 
//------------------------------------------------------------------//  
U32 VR_GroupAddr(U8 groupindex)
{
	return VR_CVRGroupAddr(0, groupindex);
}
//------------------------------------------------------------------//
// Get VR group address of CVRs from internal ROM or SPI Flash
// Parameter: 
//			CVRindex:   0 ~ CVR index
//          groupindex: 0 = cybase; 
//          			others = VR group 
// return value:
//          0: error address
//			>0: VR group address 
//------------------------------------------------------------------// 
U32 VR_CVRGroupAddr(U16 CVRindex, U8 groupindex)
{
#ifndef NYIDE_VER
#define NYIDE_VER	500		// supports UDR VR from NYIDE 5.00
#endif
#if _VR_STORAGE_TYPE || (_EF_SERIES && (_VR_STORAGE_TYPE == 0) && (NYIDE_VER > 492))
	U8 tempbuf[4];
	U8 numberOfBin;
	U16 i;
	U32 ref_addr, start_addr;

	ref_addr = UserDefinedData_GetAddressUseType(CVRindex, kUD_CVR, UDR_VR_MODE);
	UDR_VR_READ(tempbuf, ref_addr, 4);
	numberOfBin = tempbuf[0];	//|(tempbuf[1]<<8)|(tempbuf[2]<<16)|(tempbuf[3]<<24); 
	
	start_addr = ref_addr + (numberOfBin+1)*4;
	
	if(groupindex < numberOfBin) {
		if(groupindex == 0) {
			return start_addr;
		} else {
			U32 temp = 0;
			for(i = 1; i <= groupindex; i++) {
				UDR_VR_READ(tempbuf, ref_addr + i*4, 4);
				temp += (tempbuf[0]|(tempbuf[1]<<8)|(tempbuf[2]<<16)|(tempbuf[3]<<24)); 
			}
			return temp + start_addr;
		}		
	}
	return 0;
#else
	return *((U32*)(*(VrFileList+CVRindex))+groupindex);
#endif
}

#if _VR_GET_LOADING
//------------------------------------------------------------------//
// Get VR loading percentage dynamically
// Parameter: 
//          None
// return value: VR loading percentage, loading > 100% is not recommend
//------------------------------------------------------------------//  
U16 VR_GetLoading(void)
{
	return loadingPercentage;
}
#endif
#endif
