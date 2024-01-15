#include "include.h"
#include "nx1_adc.h"


// User defined ----------------------------------------------------//
#define AGC_SETTING				ENABLE								// ENABLE AGC is recommended strongly on VR application
#define PRE_GAIN				C_ADC_MIC_AMP_Gain_15X
#define PGA_GAIN				12
#define RT_PITCH_PRE_GAIN		C_ADC_MIC_AMP_Gain_15X
#if VOICE_CHANGER_RT_USED
#define RT_PITCH_PGA_GAIN		(VC_RT_PGA_GAIN)
#else
#define RT_PITCH_PGA_GAIN		0
#endif

//------------------------------------------------------------------//
#if ((_IC_BODY == 0x12A44) && (AGC_SETTING == ENABLE))
#undef  AGC_SETTING
#define AGC_SETTING				DISABLE
#endif

#if _IS_APPS_USE_ADC_MIC 
extern U8 RT_PlayFlag;

#if _SPI_MODULE
const U8 XIP_ADCGain_ParametersTable[] _XIP_RODATA = {PRE_GAIN, PGA_GAIN};
#if _RT_PLAY
const U8 XIP_RTpitchChange_ADCGain_ParametersTable[] _XIP_RODATA = {RT_PITCH_PRE_GAIN, RT_PITCH_PGA_GAIN};
#endif
#endif

//------------------------------------------------------------------//
// ADC Get PreGain Parameter
// Parameter: 
//          NONE
// return value:
//          PreGain Setting(From XIP or InternalRom)
//------------------------------------------------------------------//
U8 ADC_GetPreGainParameter(void)
{
#if _SPI_MODULE
    return XIP_ADCGain_Parameters(0);
#else
    return PRE_GAIN;
#endif
}
//------------------------------------------------------------------//
// ADC Get PGA Gain Parameter
// Parameter: 
//          NONE
// return value:
//          PGA Gain Setting(From XIP or InternalRom)
//------------------------------------------------------------------//
U8 ADC_GetPgaGainParameter(void)
{
#if _SPI_MODULE
    return XIP_ADCGain_Parameters(1);
#else
    return PGA_GAIN;
#endif
}
#if _RT_PLAY
//------------------------------------------------------------------//
// RT Get PreGain Parameter
// Parameter: 
//          NONE
// return value:
//          PreGain Setting(From XIP or InternalRom)
//------------------------------------------------------------------//
U8 RT_GetPreGainParameter(void)
{
#if _SPI_MODULE
    return XIP_RTpitchChange_ADCGain_Parameters(0);
#else
    return RT_PITCH_PRE_GAIN;
#endif
}
//------------------------------------------------------------------//
// RT Get PGA Gain Parameter
// Parameter: 
//          NONE
// return value:
//          PGA Gain Setting(From XIP or InternalRom)
//------------------------------------------------------------------//
U8 RT_GetPgaGainParameter(void)
{
#if _SPI_MODULE
    return XIP_RTpitchChange_ADCGain_Parameters(1);
#else
    return RT_PITCH_PGA_GAIN;
#endif
}
#endif
//------------------------------------------------------------------//
// Init Adc Flow 
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void CB_InitAdc(void)
{
#if _QCODE_BUILD
	ADC_SetGain(QC_GetPgaGain());
	ADC_AgCmd(QC_GetAgcCmd());
#else
    do {
	#if _RT_PLAY
        if (RT_PlayFlag) {
            ADC_MicPreGain(RT_GetPreGainParameter());
            ADC_SetGain(RT_GetPgaGainParameter());
            break;
        }
	#endif		
		ADC_MicPreGain(ADC_GetPreGainParameter());
		ADC_SetGain(ADC_GetPgaGainParameter());	
    } while(0);
	ADC_AgCmd(AGC_SETTING);
#endif
	
	// User defined code.	
	
}
#endif
