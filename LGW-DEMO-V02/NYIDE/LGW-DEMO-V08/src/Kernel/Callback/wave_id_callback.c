/* Includes ------------------------------------------------------------------*/
#include <string.h>
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
void CB_WaveID_InitDac(CH_TypeDef *chx,U32 sample_rate)
{
	SetNoiseFilter(chx,ENABLE);				//force upsample enable
    InitDac(chx,sample_rate,ENABLE);
}    
//------------------------------------------------------------------//
// Dac command
// Parameter: 
//          chx         : DAC_CH0, DAC_CH1
//          cmd         : ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_WaveID_DacCmd(CH_TypeDef *chx,U8 cmd)
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
void CB_WaveID_WriteDac(U8 size,S16 *psrc16)
{
	if(psrc16==NULL) return;
    U8 i;
    for(i=0;i<size;i++)
    {
        DacMixBuf[i] += *psrc16;
        psrc16++;
    }   
}   
//------------------------------------------------------------------//
// Init ADC
// Parameter: 
//  sample_rate:sample rate
// return value:
//          NONE
//------------------------------------------------------------------//
void CB_WaveID_InitAdc(U16 sample_rate)
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
void CB_WaveID_AdcCmd(U8 cmd)
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
void CB_WaveID_ChangeSR(CH_TypeDef *chx,U16 sample_rate)
{ 
    SetSampleRate(chx,sample_rate);
}

      

#endif
