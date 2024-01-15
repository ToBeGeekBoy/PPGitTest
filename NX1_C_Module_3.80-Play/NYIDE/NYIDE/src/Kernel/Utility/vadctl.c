#include "utility_define.h"
#include "debug.h"

#include <string.h>
//#define DEBUG_SD
#ifdef  DEBUG_SD
#define Dprintf dprintf
#else
#define Dprintf(...) 
#endif


#if _VR_VAD || _VR_VAD_TIMEOUT_EXTENSION

#define ABS(a) 					(((a) < 0) ? -(a) : (a))
#if !defined(DC_OFFSET_MINUS)
#define DC_OFFSET_MINUS			0
#endif

/* Global Word for Inline C-------------------------------------------------------*/
S16 g_vad_threshold;
S16 g_vad_active_count;
S16 g_vad_mute_count;
#if DC_OFFSET_MINUS
S16	g_vad_dc_offset;
#endif
/*--------------------------------------------------------------------------------*/

static volatile struct{
	U32 sound_ref;
    U16 count;
    U8  state;
#if _VR_VAD_TIMEOUT_EXTENSION
	U8 flag;
#endif
#if _VR_VAD
	U8 flag2;
#endif
#if _QCODE_BUILD && _VR_VAD
	U8 flag3;
#endif
}VadCtl;

#if _QCODE_BUILD && _VR_VAD
U8 VadCmd = DISABLE;
#endif
//------------------------------------------------------------------//
// VAD Get Threshold Parameter
// Parameter: 
//          NONE
// return value:
//          Threshold Setting(From XIP or InternalRom)
//------------------------------------------------------------------//
S16 VAD_GetThreshold(void)
{
#if _SPI_MODULE
    return XIP_VAD_Parameters(0);
#else
    return VAD_THRESHOLD;
#endif
}
//------------------------------------------------------------------//
// VAD Get Active Count Parameter
// Parameter: 
//          NONE
// return value:
//          Active Count Setting(From XIP or InternalRom)
//------------------------------------------------------------------//
S16 VAD_GetActiveCount(void)
{
#if _SPI_MODULE
    return XIP_VAD_Parameters(1);
#else
    return VAD_ACTIVE_COUNT;
#endif
}
//------------------------------------------------------------------//
// VAD Get Mute Count Parameter
// Parameter: 
//          NONE
// return value:
//          Mute Count Setting(From XIP or InternalRom)
//------------------------------------------------------------------//
S16 VAD_GetMuteCount(void)
{
#if _SPI_MODULE
    return XIP_VAD_Parameters(2);
#else
    return VAD_MUTE_COUNT;
#endif
}
//------------------------------------------------------------------//
// VAD Get DC Offset Parameter
// Parameter: 
//          NONE
// return value:
//          DC_Offset Setting(From XIP or InternalRom)
//------------------------------------------------------------------//
S16 VAD_GetDcOffset(void)
{
#if _SPI_MODULE
    return XIP_VAD_Parameters(3);
#else
    return DC_OFFSET;
#endif
}
//------------------------------------------------------------------//
// Voice activity detection process
// Parameter: 
//          audio_buf: audio buffer
//          samples: samples to process
// return value:
//          NONE
//------------------------------------------------------------------//     
void VadProcess(S16 *audio_buf,U16 samples)
{
    U16 i;
    U32 sum=0;

#if DC_OFFSET_MINUS
	U8 verBflag = isFDBverB();
#endif
    for(i=0;i<samples;i++){
	#if DC_OFFSET_MINUS
		S16 audio_temp = audio_buf[i];
		if(verBflag){
			if(AdcEnFlag)
				audio_temp = 0;
			else 
				audio_temp -= g_vad_dc_offset;
			audio_buf[i] = audio_temp;		// rewrite the data
		}
	#else
		S16 audio_temp = audio_buf[i];
	#endif
        sum=sum+ABS(audio_temp);
    }    
    sum=sum/samples;
	//dprintf("VAD sum = %d\r\n",sum);


    if(VadCtl.state==0)
    {
        if(sum>g_vad_threshold)
        {
        	Dprintf("sum = %d\r\n", sum);
			//VadCtl.sound_ref += sum;
			if(sum > VadCtl.sound_ref){
				VadCtl.sound_ref = sum;
			}
            VadCtl.count++;
        }
        else
        {
			VadCtl.sound_ref = 0;
            VadCtl.count=0;  
        }
		
        if(VadCtl.count>=g_vad_active_count)
        {			
			//VadCtl.sound_ref /= VadCtl.count;
			Dprintf("VAD sound_ref = %d\r\n", VadCtl.sound_ref);
			
            VadCtl.state=1;
            VadCtl.count=0;
		#if _VR_VAD_TIMEOUT_EXTENSION	
			VadCtl.flag = 1;
		#endif
		#if _VR_VAD
			VadCtl.flag2 = 1;
		#endif			
		#if _QCODE_BUILD && _VR_VAD
			VadCtl.flag3 = 1;
		#endif
			Dprintf("VAD Active\r\n");
        }
    }
    else
    {
        if(sum<VadCtl.sound_ref)
        {
            VadCtl.count++;
        }
        else
        {
			VadCtl.count=0;
		#if _VR_VAD_TIMEOUT_EXTENSION	
			VadCtl.flag = 2;
		#endif				
			Dprintf("VAD MuteReStart\r\n");
        }

        if(VadCtl.count>=g_vad_mute_count)
        {		   
  		    VadCtl.sound_ref = 0;
            VadCtl.state=0;
            VadCtl.count=0;
		#if _VR_VAD_TIMEOUT_EXTENSION	
			VadCtl.flag = 0;
		#endif
		#if _VR_VAD
			VadCtl.flag2 = 2;
		#endif	
			Dprintf("VAD Mute\r\n");
        }
    }	
}    
//------------------------------------------------------------------//
// Voice activity detection int
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//     
void VadInit(void)
{
    memset((void *)&VadCtl, 0, sizeof(VadCtl));   
#if _QCODE_BUILD==0
    g_vad_threshold 	= VAD_GetThreshold();
    g_vad_active_count  = VAD_GetActiveCount();
    g_vad_mute_count 	= VAD_GetMuteCount();
#if DC_OFFSET_MINUS
	g_vad_dc_offset 	= VAD_GetDcOffset();
#endif
#endif
	Dprintf("Init VAD\r\n");
}   
#if _VR_VAD_TIMEOUT_EXTENSION
//------------------------------------------------------------------//
// Get VAD status - for _VR_VAD_TIMEOUT_EXTENSION application
// Parameter: 
//          NONE
// return value:
//          0: mute state 
//          1: active state 
//          2: active state 
//------------------------------------------------------------------//   
U8 VR_GetVadSts(void)
{
	return VadCtl.flag;
}
#endif
#if _VR_VAD 
//------------------------------------------------------------------//
// Get VAD status2 - ONLY for VR_ServiceLoop()
// Parameter: 
//          NONE
// return value:
//			READ then CLEAR the flag
//          0: no action
//          1: active action
//          2: mute action
//------------------------------------------------------------------//    
U8 VR_GetVadSts2(void)
{
	U8 temp = VadCtl.flag2;
    if(temp){
        VadCtl.flag2 = 0;
        return temp;
    }else{
        return 0;
    }
}     
#endif
#if _QCODE_BUILD && _VR_VAD
//------------------------------------------------------------------//
// Get VAD status3 - for user check VAD status
// Parameter: 
//          NONE
// return value:
//			READ then CLEAR the flag
//          0: no action
//          1: active action
//------------------------------------------------------------------//    
U8 VR_GetVadSts3(void)
{
	U8 temp = VadCtl.flag3;
    VadCtl.flag3 = 0;
    return (VR_isVadEnable()==false)?0:temp;
}     
//------------------------------------------------------------------//
// Set VAD Cmd
// Parameter: 
//          cmd: ENABLE or DISABLE
// return value:
//			None
//------------------------------------------------------------------//
void VR_VadCmd(U8 cmd)
{
    VadCmd = (cmd==DISABLE)?DISABLE:ENABLE;
}
//------------------------------------------------------------------//
// Is VAD Enable?
// Parameter: 
//          NONE
// return value:
//			true / false
//------------------------------------------------------------------//
bool VR_isVadEnable(void)
{
    return (VadCmd==ENABLE);
}
#endif
//------------------------------------------------------------------//
// Set VAD Threshold
// Parameter: 
//          threshold: vad threshold value
// return value:
//          NONE
//------------------------------------------------------------------//  
void VadSetThreshold(U16 threshold)
{  
    g_vad_threshold = threshold;
}   
#endif
