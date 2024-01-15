#include "include.h"
#include "nx1_spi.h"
#include "nx1_adc.h"

/* Extern Functions ----------------------------------------------------------*/
#if _VR_VOICE_PASSWORD
extern S32 DoVR_TrainTag(U8* lpbySDModel, U32 nModelSize, U8* lpbySDBuffer, U32 nSDBufSize, U8* lpbySVModel, U8 nTagID, U16 nTimeOut);
#else
extern S32 DoVR_TrainTag(U8* lpbySDModel, U32 nModelSize, U8* lpbySDBuffer, U32 nSDBufSize, U8 nTagID, U16 nTimeOut);
#endif
extern S32 DoVR_DelTag(U8* lpbySDModel, U32 nModelSize, U8 nTagID);
extern void CB_EraseData(U8 mode, U32 addr,U32 size);


#if _VR_FUNC
//------------------------------------------------------------------//
// VR API Preset before VR working
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//  
void CB_VR_ApiPresetting(void)
{
	// VR API setting, user defined
	
	
	
	
	
	// Default setting
	#if _VR_VOICE_TAG
	if (VR_GetNumWord(SDModel)) {									// Does Voice Tag exist?
		// if (VR_GroupState == VR_TRIGGER_STATE) {
		// 	CSpotter16_SetResponseTime((HANDLE)hCSpotter, 10);
		// }
		
		#if _VR_VOICE_PASSWORD
		if (VR_GetNumWord(SVModel)) {
			CSPOTTER_X_SetSpeakerModel((HANDLE)hCSpotter, SVModel);
			// CSpotter16_SetSpeakerIdentLevel((HANDLE)hCSpotter, 0);
		}
		#endif		
	}
	#endif	
}
#if _VR_VOICE_TAG
#ifndef SD_MODEL_SIZE
#define SD_MODEL_SIZE				    0x7000		// 8 Voice_Tag Size
#endif
#ifndef TRAIN_BUF_SIZE
#define TRAIN_BUF_SIZE				    0x3000
#endif
#define TIMEOUT							7000 //ms
#if _VR_FRAME_RATE == 625
#define TIMEOUT_COUNT	    			(TIMEOUT/16)      
#elif _VR_FRAME_RATE == 100
#define TIMEOUT_COUNT	    			(TIMEOUT/10)
#else
#error "incorrect _VR_FRAME_RATE settings."
#endif
#if _VR_STORAGE_TYPE
#define MODE_VR							SPI_MODE
#else
#define MODE_VR							EF_MODE
#endif
//------------------------------------------------------------------//
// VR Voice-Tag create/delete control flow
// Parameter: 
//       input: Tag input data
//		 action:  =1 Create ID,=2 Delete ID,=0 Delete Module
// return value:
//      Result: DO_NOTHING, TRAIN_TAG_OK, TRAIN_TAG_FAIL, DELETE_TAG_OK, DELETE_TAG_FAIL, DELETE_SD_MODEL_OK, DELETE_TAG_NoTAG
//------------------------------------------------------------------//  
S8 CB_VR_VoiceTag_CreateOrDelete(U8 input, U8 action)
{
	// VR VoiceTag control flow, user defined
	// Default control flow

	U8 nTagID = input;
	U8* lpbySDBuffer = SDModel + SD_MODEL_SIZE;
	//======================================================
#if _VR_VOICE_PASSWORD	
	CB_EraseData(MODE_VR, (U32)SDModel, SD_MODEL_SIZE);
	CB_EraseData(MODE_VR, (U32)SVModel, 0x2000);

	switch(action)
	{
		case TRAIN_DELETEMODEL:		// Delete MODEL
		case TRAIN_DELETEID:		// Delete Tag ID
			if(action == TRAIN_DELETEMODEL)
				return DELETE_SD_MODEL_OK;
			else
				return DELETE_TAG_OK;
			break;
		case TRAIN_CREATID:			// Creat Tag ID 		
			if (DoVR_TrainTag(SDModel, SD_MODEL_SIZE, lpbySDBuffer, TRAIN_BUF_SIZE, SVModel, nTagID, TIMEOUT_COUNT) < 0) 
			{
				return TRAIN_TAG_FAIL;
			}
			else 
			{
				return TRAIN_TAG_OK;
			}
			break;
	}
#else
	S32 nID = CSPOTTERSD_X_GetCmdID(SDModel, nTagID);
	switch(action)
	{
		case TRAIN_DELETEMODEL:		// Delete MODEL
			CB_EraseData(MODE_VR, (U32)SDModel, SD_MODEL_SIZE);
			return DELETE_SD_MODEL_OK;
			break;
		case TRAIN_CREATID:			// Creat Tag ID 		
			if (nID >= 0) 
			{	// Delete first
				if (DoVR_DelTag(SDModel, SD_MODEL_SIZE, nTagID) < 0) 
				{
					return DELETE_TAG_FAIL;
				} 		
			}
			if (DoVR_TrainTag(SDModel, SD_MODEL_SIZE, lpbySDBuffer, TRAIN_BUF_SIZE, nTagID, TIMEOUT_COUNT) < 0) 
			{
				return TRAIN_TAG_FAIL;
			}
			else 
			{
				return TRAIN_TAG_OK;
			}
			break;
		case TRAIN_DELETEID:		// Delete Tag ID
			if (nID < 0) return DELETE_TAG_NoTAG;
			
			if (DoVR_DelTag(SDModel, SD_MODEL_SIZE, nTagID) < 0) 
			{
				return DELETE_TAG_FAIL;
			} 
			else
			{
				return DELETE_TAG_OK;
			}	
			break;

	}
#endif
	return DO_NOTHING;
	
}
//------------------------------------------------------------------//
// VR Voice-Tag prompt action before training start
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//  
void CB_VR_TrainTag_PromptAction_BeforeTrainingStart(void)
{
	// Prompt Action before training flow
	AUDIO_Play(AUDIO_CH0, 0, SPI_MODE);
	AUDIO_ServiceLoop_Rightnow();			// MUST call AUDIO_ServiceLoop_Rightnow(), if audio is played on CB_VR_TrainTag_XXXXX function
	
}
//------------------------------------------------------------------//
// VR Voice-Tag prompt action before saying tag
// Parameter: 
//          index: get the index number 0, 1, and 2 automatically
// return value:
//          NONE
//------------------------------------------------------------------//  
void CB_VR_TrainTag_PromptAction_BeforeSayingTag(U8 index)
{
#if _QCODE_BUILD
#if _VOICETAG_BEFORERECORD
    QC_DIRECT_CALLBACK(QC_VoiceTag_BeforeRecord);
#endif
#else
	// Prompt Action before saying a voice-tag
	switch(index) {
	default:
		break;
	case 0:
	case 1:
	case 2:
		AUDIO_Play(AUDIO_CH0, 1, SPI_MODE);
		AUDIO_ServiceLoop_Rightnow();		// MUST call AUDIO_ServiceLoop_Rightnow(), if audio is played on CB_VR_TrainTag_XXXXX function
		break;
	}
#endif
	
}
//------------------------------------------------------------------//
// VR Voice-Tag call back before the training API
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//  
void CB_VR_TrainTag_BeforeTrainWord(void)
{
 	ADC_Cmd(DISABLE);
    VRState=VR_OFF;
#if _QCODE_BUILD
#if _VOICETAG_BEFORETRAINING
    QC_DIRECT_CALLBACK(QC_VoiceTag_BeforeTraining);
#endif
#else
	AUDIO_Play(AUDIO_CH0, 6, SPI_MODE);
	AUDIO_ServiceLoop_Rightnow();		// MUST call AUDIO_ServiceLoop_Rightnow(), if audio is played on CB_VR_TrainTag_XXXXX function
#endif	
}
//------------------------------------------------------------------//
// VR Voice-Tag call back after the training API
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_VR_TrainTag_AfterTrainWord(void)
{

}
#if _QCODE_BUILD
//------------------------------------------------------------------//
// VR Voice-Tag call back for AudioActionPlayWaiting
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_VR_TrainTag_AudioActionPlayWaiting(void)
{
    do {
#if _AUDIO_MODULE
        bool _isAudioBusy = ({
            U8 ch;
            for(ch=0;ch<_AUDIO_API_CH_NUM;++ch) {
                if(AUDIO_GetStatus(ch)==STATUS_PLAYING) break;
            }
            (ch!=_AUDIO_API_CH_NUM);
        });
#endif
#if _ACTION_MODULE
        bool _isActionBusy = ({
            U8 ch;
            for(ch=0;ch<_VIO_CH_NUM;++ch) {
                if(ACTION_GetIOStatus(_AUDIO_API_CH_NUM+ch)==STATUS_PLAYING) break;
            }
            (ch!=_VIO_CH_NUM);
        });
#endif
        AUDIO_ServiceLoop();
        UTILITY_ServiceLoop();
        SYS_ClrSleepTime();
        SYS_ServiceLoop();
#if _AUDIO_MODULE
        if(_isAudioBusy) continue;
#endif
#if _ACTION_MODULE
        if(_isActionBusy) continue;
#endif
        break;
    } while(1);
}
#endif	//#if _QCODE_BUILD
#endif	//#if _VR_VOICE_TAG
#endif	//#if _VR_FUNC