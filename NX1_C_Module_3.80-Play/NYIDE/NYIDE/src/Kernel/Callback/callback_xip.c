#include "include.h"


extern const U8 XIP_ADCGain_ParametersTable[];
U8 XIP_ADCGain_Parameters(U8 index)
{
	return XIP_ADCGain_ParametersTable[index];
}

#if _RT_PLAY
extern const U8 XIP_RTpitchChange_ADCGain_ParametersTable[];
U8 XIP_RTpitchChange_ADCGain_Parameters(U8 index)
{
	return XIP_RTpitchChange_ADCGain_ParametersTable[index];
}
#endif