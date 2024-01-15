#ifndef _UTILITY_DEFINE_H
#define _UTILITY_DEFINE_H
/* Includes ------------------------------------------------------------------*/
#include "include.h"

// NX1_FDB Ver.B DC_offset
#define DC_OFFSET				1500	// each NX1_FDB Ver.B has a different DC offset
										// different PGA Gain setting will result the different DC offset
										// user can update this value by measuring the real DC offset to minimize the DC_OFFSET effect

// VR VAD setting for user
#define VAD_THRESHOLD			800
#define VAD_ACTIVE_TIME		    80		// unit: ms
#define VAD_MUTE_TIME      		650 	// unit: ms
// VR VAD setting for user
#define VR_ID_HOLD_TIME			1000	// unit: ms
#define VR_MUTE_HOLD_TIME		800		// unit: ms


// Sound Detection setting for user
#if !defined(SOUND_DETECTION_SAMPLE_RATE)
#define SOUND_DETECTION_SAMPLE_RATE					SAMPLE_RATE_8000	// Set sample rate (support 8000/10000/12000/16000Hz)	
#endif
#if		!defined(SOUND_DETECTION_HIGH_THRESHOLD)	\
	&&	!defined(SOUND_DETECTION_LOW_THRESHOLD)		\
	&&	!defined(SOUND_DETECTION_RECORD_HEAD_TIME)	\
	&&	!defined(SOUND_DETECTION_ACTIVE_TIME)		\
	&&	!defined(SOUND_DETECTION_MUTE_TIME)			//Setting from nx1_config when use Q-Code
#define SOUND_DETECTION_HIGH_THRESHOLD				300		
#define SOUND_DETECTION_LOW_THRESHOLD				300		
#define SOUND_DETECTION_ACTIVE_TIME					80		// unit: ms
#define SOUND_DETECTION_MUTE_TIME					650		// unit: ms
#define SOUND_DETECTION_RECORD_HEAD_TIME			150		// unit: ms, only for ADPCM sound trig record function
#endif



/* Internal define -----------------------------------------------------------*/
#if _VR_FUNC && (_VR_VAD || _VR_VAD_TIMEOUT_EXTENSION)
#if _VR_FRAME_RATE == 625
#define VAD_ACTIVE_COUNT    	(VAD_ACTIVE_TIME/16)      
#define VAD_MUTE_COUNT     		(VAD_MUTE_TIME/16)  
#elif _VR_FRAME_RATE == 100
#define VAD_ACTIVE_COUNT    	(VAD_ACTIVE_TIME/10)      
#define VAD_MUTE_COUNT     		(VAD_MUTE_TIME/10)
#else
#error "incorrect _VR_FRAME_RATE settings."
#endif
#endif

#define SOUND_DETECTION_ACTIVE_COUNT				(SOUND_DETECTION_ACTIVE_TIME/16)
#define SOUND_DETECTION_MUTE_COUNT					(SOUND_DETECTION_MUTE_TIME/16)
#if _ADPCM_RECORD && _ADPCM_RECORD_ERASING && _ADPCM_RECORD_SOUND_TRIG
#if SOUND_DETECTION_RECORD_HEAD_TIME > _ADPCM_SECTOR_ERASE_MAXTIME
#error "SOUND_DETECTION_RECORD_HEAD_TIME must be shorter than or equal to _ADPCM_SECTOR_ERASE_MAXTIME."
#endif
#else//_SOUND_DETECTION only
//#define PROCESS_SAMPLES_USER_DEFINED
#ifdef PROCESS_SAMPLES_USER_DEFINED
#define SOUND_DETECTION_PROCESS_SAMPLES	  			32	// User defined
#undef SOUND_DETECTION_ACTIVE_COUNT
#undef SOUND_DETECTION_MUTE_COUNT
#define SOUND_DETECTION_ACTIVE_COUNT				(SOUND_DETECTION_ACTIVE_TIME*SOUND_DETECTION_SAMPLE_RATE/(SOUND_DETECTION_PROCESS_SAMPLES*1000))
#define SOUND_DETECTION_MUTE_COUNT					(SOUND_DETECTION_MUTE_TIME*SOUND_DETECTION_SAMPLE_RATE/(SOUND_DETECTION_PROCESS_SAMPLES*1000))
#endif
#endif
#if _PD_PLAY && _SOUND_DETECTION
#if (_PD_SampleRate!=SOUND_DETECTION_SAMPLE_RATE)
#error "When pitch detect and sound detect use together, the SOUND_DETECTION_SAMPLE_RATE needs to follow _PD_SampleRate."
#endif
#endif
#endif