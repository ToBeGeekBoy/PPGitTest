/* Includes ------------------------------------------------------------------*/

#include "include.h"
#include "nx1_dac.h"
#include "nx1_adc.h"
#include "debug.h"

#if _AUDIO_MODULE==1
extern S32 DacMixBuf[];

#if _ADPCM_CH2_PLAY
//------------------------------------------------------------------//
// Init Dac of CH2
// Parameter: 
//          chx         : DAC_CH0, DAC_CH1
//          sample_rate : sample rate
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_ADPCM_CH2_InitDac(CH_TypeDef *chx,U16 sample_rate,U8 upsample)
{    
    InitDac(chx,sample_rate,upsample);
}    
//------------------------------------------------------------------//
// Dac command of CH2
// Parameter: 
//          chx         : DAC_CH0, DAC_CH1
//          cmd         : ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_ADPCM_CH2_DacCmd(CH_TypeDef *chx,U8 cmd)
{    
    DAC_Cmd(chx,cmd);
}    
//------------------------------------------------------------------//
// Write Dac Fifo of CH2
// Parameter: 
//          size: size to write
//          psrc16: source pointer
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_ADPCM_CH2_WriteDac(U8 size,U16* psrc16)
{    
    U8 i;

    if(psrc16!=0)
    {
        for(i=0;i<size;i++)
        {
#if _AUDIO_MIX_CONTROL
            extern U8 AdpcmMixCtrl_CH2;
            DacMixBuf[i]+=(S32)((((S16)*psrc16)*AdpcmMixCtrl_CH2)>>2);
#else
            DacMixBuf[i]+=(S32)(S16)*psrc16;
#endif
            psrc16++;
        }   
    }   
}   

void CB_ADPCM_CH2_PlayStart(void)
{

}  
void CB_ADPCM_CH2_PlayEnd(void)
{

}    
      
#endif// _ADPCM_CH2_PLAY

#if _ADPCM_CH3_PLAY
//------------------------------------------------------------------//
// Init Dac of CH3
// Parameter: 
//          chx         : DAC_CH0, DAC_CH1
//          sample_rate : sample rate
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_ADPCM_CH3_InitDac(CH_TypeDef *chx,U16 sample_rate,U8 upsample)
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
void CB_ADPCM_CH3_DacCmd(CH_TypeDef *chx,U8 cmd)
{    
    DAC_Cmd(chx,cmd);
}    
//------------------------------------------------------------------//
// Write Dac Fifo of CH3
// Parameter: 
//          size: size to write
//          psrc16: source pointer
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_ADPCM_CH3_WriteDac(U8 size,U16* psrc16)
{    
    U8 i;

    if(psrc16!=0)
    {
        for(i=0;i<size;i++)
        {
#if _AUDIO_MIX_CONTROL
            extern U8 AdpcmMixCtrl_CH3;
            DacMixBuf[i]+=(S32)((((S16)*psrc16)*AdpcmMixCtrl_CH3)>>2);
#else
            DacMixBuf[i]+=(S32)(S16)*psrc16;
#endif
            psrc16++;
        }   
    }   
}   

void CB_ADPCM_CH3_PlayStart(void)
{

}  
void CB_ADPCM_CH3_PlayEnd(void)
{

}     
     
#endif //_ADPCM_CH3_PLAY

#if _ADPCM_CH4_PLAY
//------------------------------------------------------------------//
// Init Dac of CH4
// Parameter: 
//          chx         : DAC_CH0, DAC_CH1
//          sample_rate : sample rate
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_ADPCM_CH4_InitDac(CH_TypeDef *chx,U16 sample_rate,U8 upsample)
{    
    InitDac(chx,sample_rate,upsample);
}    
//------------------------------------------------------------------//
// Dac command of CH4
// Parameter: 
//          chx         : DAC_CH0, DAC_CH1
//          cmd         : ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_ADPCM_CH4_DacCmd(CH_TypeDef *chx,U8 cmd)
{    
    DAC_Cmd(chx,cmd);
}    
//------------------------------------------------------------------//
// Write Dac Fifo of CH4
// Parameter: 
//          size: size to write
//          psrc16: source pointer
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_ADPCM_CH4_WriteDac(U8 size,U16* psrc16)
{    
    U8 i;

    if(psrc16!=0)
    {
        for(i=0;i<size;i++)
        {
#if _AUDIO_MIX_CONTROL
            extern U8 AdpcmMixCtrl_CH4;
            DacMixBuf[i]+=(S32)((((S16)*psrc16)*AdpcmMixCtrl_CH4)>>2);
#else
            DacMixBuf[i]+=(S32)(S16)*psrc16;
#endif
            psrc16++;
        }   
    }   
}   

void CB_ADPCM_CH4_PlayStart(void)
{

}  
void CB_ADPCM_CH4_PlayEnd(void)
{

}    
      
#endif// _ADPCM_CH4_PLAY

#if _ADPCM_CH5_PLAY
//------------------------------------------------------------------//
// Init Dac of CH5
// Parameter: 
//          chx         : DAC_CH0, DAC_CH1
//          sample_rate : sample rate
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_ADPCM_CH5_InitDac(CH_TypeDef *chx,U16 sample_rate,U8 upsample)
{    
    InitDac(chx,sample_rate,upsample);
}    
//------------------------------------------------------------------//
// Dac command of CH5
// Parameter: 
//          chx         : DAC_CH0, DAC_CH1
//          cmd         : ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_ADPCM_CH5_DacCmd(CH_TypeDef *chx,U8 cmd)
{    
    DAC_Cmd(chx,cmd);
}    
//------------------------------------------------------------------//
// Write Dac Fifo of CH5
// Parameter: 
//          size: size to write
//          psrc16: source pointer
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_ADPCM_CH5_WriteDac(U8 size,U16* psrc16)
{    
    U8 i;

    if(psrc16!=0)
    {
        for(i=0;i<size;i++)
        {
#if _AUDIO_MIX_CONTROL
            extern U8 AdpcmMixCtrl_CH5;
            DacMixBuf[i]+=(S32)((((S16)*psrc16)*AdpcmMixCtrl_CH5)>>2);
#else
            DacMixBuf[i]+=(S32)(S16)*psrc16;
#endif
            psrc16++;
        }   
    }   
}   

void CB_ADPCM_CH5_PlayStart(void)
{

}  
void CB_ADPCM_CH5_PlayEnd(void)
{

}    
      
#endif// _ADPCM_CH5_PLAY

#if _ADPCM_CH6_PLAY
//------------------------------------------------------------------//
// Init Dac of CH6
// Parameter: 
//          chx         : DAC_CH0, DAC_CH1
//          sample_rate : sample rate
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_ADPCM_CH6_InitDac(CH_TypeDef *chx,U16 sample_rate,U8 upsample)
{    
    InitDac(chx,sample_rate,upsample);
}    
//------------------------------------------------------------------//
// Dac command of CH6
// Parameter: 
//          chx         : DAC_CH0, DAC_CH1
//          cmd         : ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_ADPCM_CH6_DacCmd(CH_TypeDef *chx,U8 cmd)
{    
    DAC_Cmd(chx,cmd);
}    
//------------------------------------------------------------------//
// Write Dac Fifo of CH6
// Parameter: 
//          size: size to write
//          psrc16: source pointer
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_ADPCM_CH6_WriteDac(U8 size,U16* psrc16)
{    
    U8 i;

    if(psrc16!=0)
    {
        for(i=0;i<size;i++)
        {
#if _AUDIO_MIX_CONTROL
            extern U8 AdpcmMixCtrl_CH6;
            DacMixBuf[i]+=(S32)((((S16)*psrc16)*AdpcmMixCtrl_CH6)>>2);
#else
            DacMixBuf[i]+=(S32)(S16)*psrc16;
#endif
            psrc16++;
        }   
    }   
}   

void CB_ADPCM_CH6_PlayStart(void)
{

}  
void CB_ADPCM_CH6_PlayEnd(void)
{

}    
      
#endif// _ADPCM_CH6_PLAY

#if _ADPCM_CH7_PLAY
//------------------------------------------------------------------//
// Init Dac of CH7
// Parameter: 
//          chx         : DAC_CH0, DAC_CH1
//          sample_rate : sample rate
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_ADPCM_CH7_InitDac(CH_TypeDef *chx,U16 sample_rate,U8 upsample)
{    
    InitDac(chx,sample_rate,upsample);
}    
//------------------------------------------------------------------//
// Dac command of CH7
// Parameter: 
//          chx         : DAC_CH0, DAC_CH1
//          cmd         : ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_ADPCM_CH7_DacCmd(CH_TypeDef *chx,U8 cmd)
{    
    DAC_Cmd(chx,cmd);
}    
//------------------------------------------------------------------//
// Write Dac Fifo of CH7
// Parameter: 
//          size: size to write
//          psrc16: source pointer
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_ADPCM_CH7_WriteDac(U8 size,U16* psrc16)
{    
    U8 i;

    if(psrc16!=0)
    {
        for(i=0;i<size;i++)
        {
#if _AUDIO_MIX_CONTROL
            extern U8 AdpcmMixCtrl_CH7;
            DacMixBuf[i]+=(S32)((((S16)*psrc16)*AdpcmMixCtrl_CH7)>>2);
#else
            DacMixBuf[i]+=(S32)(S16)*psrc16;
#endif
            psrc16++;
        }   
    }   
}   

void CB_ADPCM_CH7_PlayStart(void)
{

}  
void CB_ADPCM_CH7_PlayEnd(void)
{

}    
      
#endif// _ADPCM_CH7_PLAY

#if _ADPCM_CH8_PLAY
//------------------------------------------------------------------//
// Init Dac of CH8
// Parameter: 
//          chx         : DAC_CH0, DAC_CH1
//          sample_rate : sample rate
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_ADPCM_CH8_InitDac(CH_TypeDef *chx,U16 sample_rate,U8 upsample)
{    
    InitDac(chx,sample_rate,upsample);
}    
//------------------------------------------------------------------//
// Dac command of CH8
// Parameter: 
//          chx         : DAC_CH0, DAC_CH1
//          cmd         : ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_ADPCM_CH8_DacCmd(CH_TypeDef *chx,U8 cmd)
{    
    DAC_Cmd(chx,cmd);
}    
//------------------------------------------------------------------//
// Write Dac Fifo of CH8
// Parameter: 
//          size: size to write
//          psrc16: source pointer
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_ADPCM_CH8_WriteDac(U8 size,U16* psrc16)
{    
    U8 i;

    if(psrc16!=0)
    {
        for(i=0;i<size;i++)
        {
#if _AUDIO_MIX_CONTROL
            extern U8 AdpcmMixCtrl_CH8;
            DacMixBuf[i]+=(S32)((((S16)*psrc16)*AdpcmMixCtrl_CH8)>>2);
#else
            DacMixBuf[i]+=(S32)(S16)*psrc16;
#endif
            psrc16++;
        }   
    }   
}   

void CB_ADPCM_CH8_PlayStart(void)
{

}  
void CB_ADPCM_CH8_PlayEnd(void)
{

}    
      
#endif// _ADPCM_CH8_PLAY
#endif//_AUDIO_MODULE==1
