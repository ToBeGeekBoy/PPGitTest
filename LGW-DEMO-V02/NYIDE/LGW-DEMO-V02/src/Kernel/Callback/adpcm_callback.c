/* Includes ------------------------------------------------------------------*/

#include "include.h"
#include "nx1_dac.h"
#include "nx1_adc.h"

#if _AUDIO_MODULE==1
extern S32 DacMixBuf[];
//------------------------------------------------------------------//
// Init Dac
// Parameter: 
//          chx         : DAC_CH0, DAC_CH1
//          sample_rate : sample rate
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_ADPCM_InitDac(CH_TypeDef *chx,U16 sample_rate,U8 upsample)
{    
    InitDac(chx,sample_rate,upsample);
}    
//------------------------------------------------------------------//
// Dac command
// Parameter: 
//          chx         : DAC_CH0, DAC_CH1
//          cmd         : ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_ADPCM_DacCmd(CH_TypeDef *chx,U8 cmd)
{    
    DAC_Cmd(chx,cmd);
}    
//------------------------------------------------------------------//
// Write Dac Fifo
// Parameter: 
//          size: size to write
//          psrc16: source pointer
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_ADPCM_WriteDac(U8 size,U16* psrc16)
{    
    U8 i;

    if(psrc16!=0)
    {
        for(i=0;i<size;i++)
        {
#if _AUDIO_MIX_CONTROL
            extern U8 AdpcmMixCtrl;
            DacMixBuf[i]+=(S32)((((S16)*psrc16)*AdpcmMixCtrl)>>2);
#else
            DacMixBuf[i]+=(S32)(S16)*psrc16;
#endif
            psrc16++;
        }   
    }   
}   
//------------------------------------------------------------------//
// Init ADC
// Parameter: 
//  sample_rate:sample rate
// return value:
//          NONE
//------------------------------------------------------------------//
void CB_ADPCM_InitAdc(U16 sample_rate)
{
    InitAdc(sample_rate);
}   
//------------------------------------------------------------------//
// ADC command
// Parameter: 
//          cmd:ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_ADPCM_AdcCmd(U8 cmd)
{    
#if	defined(P_ADC)
    ADC_Cmd(cmd);
#endif	//#if	defined(P_ADC)
}     
//------------------------------------------------------------------//
// Change dac sample rate
// Parameter:
//          chx         : DAC_CH0, DAC_CH1
//          sample_rate : sample rate
// return value:
//          NONE
//------------------------------------------------------------------//
void CB_ADPCM_ChangeSR(CH_TypeDef *chx,U16 sample_rate)
{ 
    SetSampleRate(chx,sample_rate);
}


void CB_ADPCM_PlayStart(void)
{
	
}  
void CB_ADPCM_PlayEnd(void)
{
	
}       
void CB_ADPCM_RecStart(void)
{
	
}  
void CB_ADPCM_RecEnd(void)
{
	
}   

#endif
