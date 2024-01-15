#include "include.h"
#include "utility_define.h"


#if _SPI_MODULE && _XIP_COC && _VR_FUNC && _VR_STORAGE_TYPE
U16 COC_VR_RamSize(void)
{
	return _VR_RAM_SIZE;
}
U16 COC_VR_EngineRamSize(void)
{
	return _VR_ENGINE_RAM_SIZE;
}
U32 COC_VR_TimeOut(void)
{
	return _VR_TIMEOUT;
}
#endif


#if _SPI_MODULE && _VR_FUNC  && (_VR_VAD || _VR_VAD_TIMEOUT_EXTENSION)
const S16 XIP_VAD_ParametersTable[] _XIP_RODATA = {VAD_THRESHOLD, VAD_ACTIVE_COUNT, VAD_MUTE_COUNT, DC_OFFSET};
S16 XIP_VAD_Parameters(U8 index)
{
	return XIP_VAD_ParametersTable[index];
}
#if _VR_VAD
const U16 XIP_VR_VAD_ParametersTable[] _XIP_RODATA = {VR_ID_HOLD_TIME, VR_MUTE_HOLD_TIME};
U16 XIP_VR_VAD_Parameters(U8 index)
{
	return XIP_VR_VAD_ParametersTable[index];
}
#endif
#endif


#if _SPI_MODULE
const S16 XIP_SOUND_DETECTION_ParametersTable[] _XIP_RODATA = {SOUND_DETECTION_HIGH_THRESHOLD, SOUND_DETECTION_LOW_THRESHOLD, SOUND_DETECTION_ACTIVE_COUNT, SOUND_DETECTION_MUTE_COUNT, DC_OFFSET};
S16 XIP_SOUND_DETECTION_Parameters(U8 index)
{
	return XIP_SOUND_DETECTION_ParametersTable[index];
}
#endif