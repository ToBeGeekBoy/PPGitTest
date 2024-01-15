#ifndef NX1_VOICECHANERTAB_H
#define NX1_VOICECHANERTAB_H

#include "nx1_lib.h"


//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * ADPCM Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
#define VOICE_CHANGER_ADPCM_USED            	DISABLE

#define VC_ADPCM_AGC_FUNC                   	DISABLE
#define VC_ADPCM_AGC_TYPE						TYPE0
#if 	VC_ADPCM_AGC_TYPE==TYPE0
#define VC_ADPCM_AGC_TARGET_RMS             	MINUS_5DB
#define VC_ADPCM_AGC_ALPHA                  	16384
#define VC_ADPCM_AGC_BETA                   	1024
#define VC_ADPCM_AGC_LIMITER                	30000
#elif	VC_ADPCM_AGC_TYPE==TYPE1
#define VC_ADPCM_AGC_TARGET_RMS             	MINUS_12DB
#define VC_ADPCM_AGC_ALPHA                  	1024
#define VC_ADPCM_AGC_BETA                   	8192
#define VC_ADPCM_AGC_LIMITER                	30000
#elif	VC_ADPCM_AGC_TYPE==TYPE2
#define VC_ADPCM_AGC_TARGET_RMS             	MINUS_12DB
#define VC_ADPCM_AGC_ALPHA                  	2048
#define VC_ADPCM_AGC_BETA                   	1024
#define VC_ADPCM_AGC_LIMITER                	30000
#else
#error "VC_ADPCM_AGC_TYPE setting error."
#endif

#define VC_ADPCM_NOISE_GATE_FUNC            	DISABLE
#define VC_ADPCM_NOISE_GATE_TYPE				TYPE0
#if		VC_ADPCM_NOISE_GATE_TYPE==TYPE0
#define VC_ADPCM_NG_ON_OFF_SAMPLE_16K       	160
#define VC_ADPCM_NG_ATTACK_SAMPLE_16K       	80
#define VC_ADPCM_NG_RELEASE_SAMPLE_16K      	1600
#define VC_ADPCM_NG_ON_OFF_SAMPLE_8K        	80
#define VC_ADPCM_NG_ATTACK_SAMPLE_8K        	40
#define VC_ADPCM_NG_RELEASE_SAMPLE_8K       	800
#elif	VC_ADPCM_NOISE_GATE_TYPE==TYPE1
#define VC_ADPCM_NG_ON_OFF_SAMPLE_16K       	160
#define VC_ADPCM_NG_ATTACK_SAMPLE_16K       	80
#define VC_ADPCM_NG_RELEASE_SAMPLE_16K      	3200
#define VC_ADPCM_NG_ON_OFF_SAMPLE_8K        	80
#define VC_ADPCM_NG_ATTACK_SAMPLE_8K        	40
#define VC_ADPCM_NG_RELEASE_SAMPLE_8K       	1600
#elif	VC_ADPCM_NOISE_GATE_TYPE==TYPE2
#define VC_ADPCM_NG_ON_OFF_SAMPLE_16K       	160
#define VC_ADPCM_NG_ATTACK_SAMPLE_16K       	240
#define VC_ADPCM_NG_RELEASE_SAMPLE_16K      	1600
#define VC_ADPCM_NG_ON_OFF_SAMPLE_8K        	80
#define VC_ADPCM_NG_ATTACK_SAMPLE_8K        	120
#define VC_ADPCM_NG_RELEASE_SAMPLE_8K       	800
#elif	VC_ADPCM_NOISE_GATE_TYPE==TYPE3
#define VC_ADPCM_NG_ON_OFF_SAMPLE_16K       	160
#define VC_ADPCM_NG_ATTACK_SAMPLE_16K       	32
#define VC_ADPCM_NG_RELEASE_SAMPLE_16K      	2400
#define VC_ADPCM_NG_ON_OFF_SAMPLE_8K        	80
#define VC_ADPCM_NG_ATTACK_SAMPLE_8K        	16
#define VC_ADPCM_NG_RELEASE_SAMPLE_8K       	1200
#else
#error "VC_ADPCM_NOISE_GATE_TYPE setting error."
#endif

#define	VC_ADPCM_DGC_TYPE						TYPE0
#define VC_ADPCM_DGC_MAX_VOL                    100
#define VC_ADPCM_DGC_VAD_FUNC                   1         
#define VC_ADPCM_DGC_VAD_THRES                  3000

#if VOICE_CHANGER_ADPCM_USED
extern const S16 ADPCMDigiAmpArray[14];
extern const U16 ADPCMVADArray[9];
extern const S16 ADPCMDigiAmpSet[2];
#endif

//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * Real Time Module * 
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
#define VOICE_CHANGER_RT_USED               	DISABLE

#define VC_RT_AGC_FUNC                      	DISABLE
#define VC_RT_AGC_TYPE							TYPE0
#if 	VC_RT_AGC_TYPE==TYPE0
#define VC_RT_AGC_TARGET_RMS                	MINUS_7DB
#define VC_RT_AGC_ALPHA                     	4096
#define VC_RT_AGC_BETA                      	1024

#elif 	VC_RT_AGC_TYPE==TYPE1
#define VC_RT_AGC_TARGET_RMS                	MINUS_5DB
#define VC_RT_AGC_ALPHA                     	16384
#define VC_RT_AGC_BETA                      	2048

#elif 	VC_RT_AGC_TYPE==TYPE2
#define VC_RT_AGC_TARGET_RMS                	MINUS_9DB
#define VC_RT_AGC_ALPHA                     	16384
#define VC_RT_AGC_BETA                      	1024

#else
#error "VC_RT_AGC_TYPE setting error."
#endif

#define VC_RT_NOISE_GATE_FUNC               	DISABLE
#define VC_RT_NOISE_GATE_TYPE					TYPE0
#if		VC_RT_NOISE_GATE_TYPE==TYPE0
#define VC_RT_NG_ON_OFF_SAMPLE_16K          	160
#define VC_RT_NG_ATTACK_SAMPLE_16K          	80
#define VC_RT_NG_RELEASE_SAMPLE_16K         	1600
#define VC_RT_NG_ON_OFF_SAMPLE_8K           	80
#define VC_RT_NG_ATTACK_SAMPLE_8K           	40
#define VC_RT_NG_RELEASE_SAMPLE_8K          	800
#elif	VC_RT_NOISE_GATE_TYPE==TYPE1
#define VC_RT_NG_ON_OFF_SAMPLE_16K          	160
#define VC_RT_NG_ATTACK_SAMPLE_16K          	80
#define VC_RT_NG_RELEASE_SAMPLE_16K         	3200
#define VC_RT_NG_ON_OFF_SAMPLE_8K           	80
#define VC_RT_NG_ATTACK_SAMPLE_8K           	40
#define VC_RT_NG_RELEASE_SAMPLE_8K          	1600
#elif	VC_RT_NOISE_GATE_TYPE==TYPE2
#define VC_RT_NG_ON_OFF_SAMPLE_16K          	160
#define VC_RT_NG_ATTACK_SAMPLE_16K          	240
#define VC_RT_NG_RELEASE_SAMPLE_16K         	1600
#define VC_RT_NG_ON_OFF_SAMPLE_8K           	80
#define VC_RT_NG_ATTACK_SAMPLE_8K           	120
#define VC_RT_NG_RELEASE_SAMPLE_8K          	800
#elif	VC_RT_NOISE_GATE_TYPE==TYPE3
#define VC_RT_NG_ON_OFF_SAMPLE_16K          	160
#define VC_RT_NG_ATTACK_SAMPLE_16K          	32
#define VC_RT_NG_RELEASE_SAMPLE_16K         	2400
#define VC_RT_NG_ON_OFF_SAMPLE_8K           	80
#define VC_RT_NG_ATTACK_SAMPLE_8K           	16
#define VC_RT_NG_RELEASE_SAMPLE_8K          	1200
#else
#error "VC_RT_NOISE_GATE_TYPE setting error."
#endif

#define VC_RT_ECHO_CANCEL_FUNC              	DISABLE
#define VC_RT_ECHO_CANCEL_THRES					2000

#define	VC_RT_DGC_TYPE							TYPE0
#define VC_RT_DGC_MAX_VOL                       100
#define VC_RT_DGC_VAD_FUNC                      1         
#define VC_RT_DGC_VAD_THRES                     3000

#define VC_RT_PGA_GAIN                          0

#if VOICE_CHANGER_RT_USED
extern const S16 RTDigiAmpArray[14];
extern const U16 RTVadArray[9];
extern const S16 RTDigiAmpSet[2];
#endif

#endif