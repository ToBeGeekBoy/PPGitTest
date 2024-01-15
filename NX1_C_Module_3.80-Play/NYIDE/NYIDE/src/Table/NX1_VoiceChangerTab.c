#include "nx1_lib.h"
#include "NX1_VoiceChangerTab.h"

#if VOICE_CHANGER_ADPCM_USED
#if _ADPCM_PLAY && _ADPCM_RECORD && _ADPCM_RECORD_PLAY_TYPE==TYPE2
#if		VC_ADPCM_DGC_TYPE==TYPE0
const S16 ADPCMDigiAmpArray[14] = {15, 200, VC_ADPCM_DGC_MAX_VOL, 4096, 4, 150, 55, 1966, VC_ADPCM_DGC_VAD_FUNC, VC_ADPCM_DGC_VAD_THRES, 1, 10, 100, 200};
const U16 ADPCMVADArray[9] = {100, 3, 0, 2, 10, 2, 0, 30, 5};
const S16 ADPCMDigiAmpSet[2] = {32, 0};

#elif	VC_ADPCM_DGC_TYPE==TYPE1
const S16 ADPCMDigiAmpArray[14] = {15, 200, VC_ADPCM_DGC_MAX_VOL, 8192, 8, 180, 55, 1966, VC_ADPCM_DGC_VAD_FUNC, VC_ADPCM_DGC_VAD_THRES, 2, 15, 100, 250};
const U16 ADPCMVADArray[9] = {100, 5, 0, 32, 10, 2, 0, 35, 4};
const S16 ADPCMDigiAmpSet[2] = {32, 0};

#elif	VC_ADPCM_DGC_TYPE==TYPE2
const S16 ADPCMDigiAmpArray[14] = {30, 200, VC_ADPCM_DGC_MAX_VOL, 4096, 7, 200, 55, 1966, VC_ADPCM_DGC_VAD_FUNC, VC_ADPCM_DGC_VAD_THRES, 1, 15, 100, 180};
const U16 ADPCMVADArray[9] = {100, 3, 0, 7, 10, 1, 150, 30, 5};
const S16 ADPCMDigiAmpSet[2] = {32, 0};

#else
#error "VC_ADPCM_DGC_TYPE setting error."
#endif
#endif
#endif

#if VOICE_CHANGER_RT_USED 
#if _RT_PLAY && _RT_DIGITAL_GAIN_CTRL
#if		VC_RT_DGC_TYPE==TYPE0
const S16 RTDigiAmpArray[14] = {15, 200, VC_RT_DGC_MAX_VOL, 4096, 4, 150, 55, 1966, VC_RT_DGC_VAD_FUNC, VC_RT_DGC_VAD_THRES, 1, 10, 100, 200};
const U16 RTVadArray[9] = {100, 3, 0, 2, 10, 2, 0, 30, 5};
const S16 RTDigiAmpSet[2] = {32, 0};

#elif	VC_RT_DGC_TYPE==TYPE1
const S16 RTDigiAmpArray[14] = {15, 200, VC_RT_DGC_MAX_VOL, 8192, 8, 180, 55, 1966, VC_RT_DGC_VAD_FUNC, VC_RT_DGC_VAD_THRES, 2, 15, 100, 250};
const U16 RTVadArray[9] = {100, 5, 0, 32, 10, 2, 0, 35, 4};
const S16 RTDigiAmpSet[2] = {32, 0};

#elif	VC_RT_DGC_TYPE==TYPE2
const S16 RTDigiAmpArray[14] = {30, 200, VC_RT_DGC_MAX_VOL, 4096, 7, 200, 55, 1966, VC_RT_DGC_VAD_FUNC, VC_RT_DGC_VAD_THRES, 1, 15, 100, 180};
const U16 RTVadArray[9] = {100, 3, 0, 7, 10, 1, 150, 30, 5};
const S16 RTDigiAmpSet[2] = {32, 0};

#else
#error "VC_RT_DGC_TYPE setting error."
#endif
#endif
#endif