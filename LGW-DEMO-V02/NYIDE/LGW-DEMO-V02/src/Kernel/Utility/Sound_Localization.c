
#include <string.h>
#include "include.h"
#include "debug.h"
#include "nx1_lib.h"
#include "nx1_smu.h"
#include "nx1_reg.h"
#include "nx1_gpio.h"
#include "nx1_timer.h"
#include "nx1_intc.h"
#include "nx1_adc.h"
#include "nx1_spi.h"

#if _SL_MODULE
#define _SL_BUF_NOTREADY       		0
#define _SL_BUFA_READY          	1
#define _SL_BUFB_READY          	2
#define OFFSET0						0
#define OFFSET1						(OFFSET0+(_SL_BUF_SIZE)*1)
#define OFFSET2						(OFFSET1+(_SL_BUF_SIZE)*1)
#define OFFSET3						(OFFSET2+(_SL_BUF_SIZE)*1)
#define OFFSET4						(OFFSET3+(_SL_BUF_SIZE)*1)
#define OFFSET5						(OFFSET4+(_SL_BUF_SIZE)*1)

/*--------------------------------------------------------------------*/
#if _USE_HEAP_RAM
	static S16  *SLBuf=NULL;
	static SSLBuf1* SSL_Buf=NULL;
	static FFTctl*	FFT_Ctrl=NULL;
//------------------------------------------------------------------//
// Sound_AllocateMemory
// Description: if use Heap RAM, AllocateMemory Size
// Parameter: None 
//------------------------------------------------------------------// 	
	static void SL_AllocateMemory(void)
	{
		if(SLBuf==NULL)						
		{
			SLBuf=(S16*)MemAlloc(_SL_Base_RAM_SIZE*sizeof(S16));
		}	
		if(SSL_Buf==NULL)
		{
			SSL_Buf=(SSLBuf1*)MemAlloc(sizeof(SSLBuf1));
		}
		if(FFT_Ctrl==NULL)
		{
			FFT_Ctrl=(FFTctl*)MemAlloc(sizeof(FFTctl));
		}
	}
//------------------------------------------------------------------//
// Sound_Localization Memory Free
// Description: if use Heap RAM, Free Memory After Using
// Parameter: None		 
//------------------------------------------------------------------// 
	static void SL_FreeMemory(void)
	{
		if(SLBuf!=NULL)
		{
			MemFree(SLBuf);
			SLBuf=NULL;
		}
		if(SSL_Buf!=NULL)
		{
			MemFree(SSL_Buf);
			SSL_Buf=NULL;
		}
		if(FFT_Ctrl!=NULL)
		{
			MemFree(FFT_Ctrl);
			FFT_Ctrl=NULL;
		}
	}
#else 
	static S16  SLBuf[_SL_Base_RAM_SIZE];
	static SSLBuf1 _SSL_Buf;
	SSLBuf1* SSL_Buf = &_SSL_Buf;
	static FFTctl 	_FFTctl;
	FFTctl* FFT_Ctrl = &_FFTctl;
#endif
/* --------------------------------------------------------------------*/
static U8 _SL_BufReady;
static U8 _SL_BUF_Flag; 
static U16 _SL_BufIdx;  
static S16 *p_BufA0,*p_BufA1,*p_BufB0,*p_BufB1;
static S16 *buf0,*buf1;
#if _SL_MIC_NUM==3
static S16 *p_BufA2,*p_BufB2;
static S16 *buf2;
#endif
U8 SL_State;
S16 _SL_Degree_Divide=-1;
S16 SL_ServiceLoop(void);
void SL_Init(void);
void SL_Start(void);
void SL_Stop(void);
void SL_Degree_Divide(S16 degree);
#if _SL_MIC_NUM==3
void SL_AdcIsr(U8 size, U32 *pdest0_32, U32 *pdest1_32, U32 *pdest2_32);
extern void SoundLocationInit(const S16 *sslCtrl, const S16 * VadCtrl, SSLBuf1 *sslbuf, FFTctl*fftctl);
extern S16 SoundLocation(S16 *in1, S16 *in2, S16 *in3, SSLBuf1 *sslbuf, const S16 *dcRemove, const S16 *sslCtrl, FFTctl*fftctl);
#elif _SL_MIC_NUM==2
void SL_AdcIsr(U8 size, U32 *pdest0_32, U32 *pdest1_32);
extern void SoundLocation180Init(const S16 *sslCtrl, const S16 * VadCtrl, SSLBuf1 *sslbuf, FFTctl *fftctl);
extern S16 SoundLocation180(S16 *in1, S16 *in2, SSLBuf1 *sslbuf, const S16 *dcRemove, const S16 *sslCtrl, FFTctl *fftctl);
#endif

/* ALG Parameter-------------------------------------------------------*/    
const S16 SSLCtl[5] = {
     256,           //Insize
     6,            //AzimuthCandidate
     50,           //Max_AzimuthStatisticNumber
     1,            //Min_AzimuthStatisticNumber
     4,            //MaxDelayPoint
};

const S16 RTdVadCtl[8] = 
{
  /*Limitation*/
  	100,           	// (%),Max Avg Limit
  	4,           	// (%),Min Avg Limit
 /* Initialize*/
 	0,
 /* Start Detection */
  	13,         	 // Average history velocity, 
                                //Threshold更新速度: 0(固定閾值),1(更新慢)~32(更新快).
	3000,     		// Voice AVG threshold :0~32767 
   	0,           	// Start Detect condition : Voice  duration(frames) 
   	3000,       	// Start Detect condition : Voice max peak :0~32767 
     				/* End Detection */
    3,            	// End Detect condition : Voice duration(frames)  
};
//------------------------------------------------------------------//
// Sound_Localization Init
// Description: MFP Init, 1.HW PIN Disable, 2.GPIO PA1/PA2:ADC Ch, 3.SL_State=SL_OFF
// Parameter: None		
//------------------------------------------------------------------// 
void SL_Init(void)
{		  
    GPIO_Init(_SL_AMP_SD_CTL_PORT,_SL_AMP_SD_CTL_PIN,GPIO_MODE_OUT_LOW);   
    #if _EF_SERIES
    	GPIO_Init(GPIOA,ADC_CH1_PIN,GPIO_MODE_ALT3);
        #if _SL_MIC_NUM==3
    	GPIO_Init(GPIOA,ADC_CH2_PIN,GPIO_MODE_ALT3);
        #endif
    #else
    	GPIO_Init(GPIOA,ADC_CH1_PIN,GPIO_MODE_ALT);
        #if _SL_MIC_NUM==3
    	GPIO_Init(GPIOA,ADC_CH2_PIN,GPIO_MODE_ALT);
        #endif
    #endif   
	GPIO_Write(_SL_AMP_SD_CTL_PORT,_SL_AMP_SD_CTL_PIN,_SL_AMP_SD_CTL_ACTIVE);	// HW Init by GPIO 
	
	SL_State=SL_OFF;
	//SL_Start();
}	
//------------------------------------------------------------------//
// Sound_Localization Adc ISR
// Description: AB Buffer store Data
// Parameter: 
//          size: data-buffer fill size 
//      	pdest0_32: CH0 data-buffer address
//     	  	pdest1_32: CH1 data-buffer address
//      	pdest2_32: CH2 data-buffer address
//------------------------------------------------------------------// 
#if _SL_MIC_NUM==3
void SL_AdcIsr(U8 size, U32 *pdest0_32, U32 *pdest1_32, U32 *pdest2_32)
#elif _SL_MIC_NUM==2
void SL_AdcIsr(U8 size, U32 *pdest0_32, U32 *pdest1_32)
#endif
{    
   	U8  i;
   	S16 temp0,temp1;
	#if _SL_MIC_NUM==3
   	S16 temp2;
	#endif
   	
 	if(SL_State==SL_ON)
  	{
   		if(!_SL_BUF_Flag) 
   		{
   			buf0=p_BufA0+_SL_BufIdx;
   			buf1=p_BufA1+_SL_BufIdx;
		#if _SL_MIC_NUM==3
   			buf2=p_BufA2+_SL_BufIdx;
		#endif
   		}
   		else
   		{
   			buf0=p_BufB0+_SL_BufIdx;
   			buf1=p_BufB1+_SL_BufIdx;
		#if _SL_MIC_NUM==3
   			buf2=p_BufB2+_SL_BufIdx;
		#endif
   			
    	}	
    	for(i=0; i<size; i+=3)
    	{
    		temp0=((*pdest0_32++)-32768);
    		*buf0++=temp0;
    		temp1=((*pdest1_32++)-32768);
    		*buf1++=temp1;
		#if _SL_MIC_NUM==3
    		temp2=((*pdest2_32++)-32768);
    		*buf2++=temp2;
		#endif
    		_SL_BufIdx++;
    	}
    	if(_SL_BufIdx==_SL_BUF_SIZE)
    	{
    		if(!_SL_BUF_Flag)	_SL_BufReady=_SL_BUFA_READY;
        	else 				_SL_BufReady=_SL_BUFB_READY; 
        	
    		_SL_BUF_Flag^=1;
   			_SL_BufIdx=0;	
   	 	}  	
   }  
}
//------------------------------------------------------------------//
// Sound_Localization SV Loop
// Description: return Degree:-1~360 
// Parameter: None
//------------------------------------------------------------------// 
S16 SL_ServiceLoop(void)
{	
	if(SL_State!=SL_ON || _SL_BufReady==_SL_BUF_NOTREADY || AdcEnFlag !=0) return -1;
	
	S16 _SL_Degree=-1;
	if(_SL_BufReady==_SL_BUFA_READY)
	{
		_SL_BufReady=_SL_BUF_NOTREADY;
#if _SL_MIC_NUM==3
		_SL_Degree=SoundLocation(p_BufA1, p_BufA0, p_BufA2, SSL_Buf, DCRemove, SSLCtl, FFT_Ctrl);
#elif _SL_MIC_NUM==2
		_SL_Degree=SoundLocation180(p_BufA0, p_BufA1, SSL_Buf, DCRemove, SSLCtl, FFT_Ctrl);
#endif
	}	
	else if(_SL_BufReady==_SL_BUFB_READY)
	{
		_SL_BufReady=_SL_BUF_NOTREADY;
#if _SL_MIC_NUM==3
		_SL_Degree=SoundLocation(p_BufB1, p_BufB0, p_BufB2, SSL_Buf, DCRemove, SSLCtl, FFT_Ctrl);
#elif _SL_MIC_NUM==2
		_SL_Degree=SoundLocation180(p_BufB0, p_BufB1, SSL_Buf, DCRemove, SSLCtl, FFT_Ctrl);
#endif
	}
	SL_Degree_Divide(_SL_Degree);
	return _SL_Degree_Divide;
}
//------------------------------------------------------------------//
// Sound_Localization Start
// Description: 1.HW PIN Enable, 2.SL&ALG Init, 3.AllocateMemory, 4.SL_State=SL_INIT for ADC Init, 5.ADC Enable
// Parameter: 
//------------------------------------------------------------------// 
void SL_Start(void)
{
    if(SL_State!=SL_OFF) return;
    GPIO_Write(_SL_AMP_SD_CTL_PORT,_SL_AMP_SD_CTL_PIN,(~_SL_AMP_SD_CTL_ACTIVE));	//HW Enable
    
	_SL_BUF_Flag=0;
	_SL_BufIdx=0;
	_SL_BufReady=_SL_BUF_NOTREADY;

	#if _USE_HEAP_RAM
		SL_AllocateMemory();							// for p_Buf&ALG init
	#endif	
	memset((void*)SLBuf,0,(_SL_Base_RAM_SIZE*sizeof(S16)));
	memset((void*)SSL_Buf,0,sizeof(SSLBuf1));

	p_BufA0=(S16*)(SLBuf+OFFSET0);
	p_BufB0=(S16*)(SLBuf+OFFSET1);
	p_BufA1=(S16*)(SLBuf+OFFSET2);
	p_BufB1=(S16*)(SLBuf+OFFSET3);
    #if _SL_MIC_NUM==3
	p_BufA2=(S16*)(SLBuf+OFFSET4);
	p_BufB2=(S16*)(SLBuf+OFFSET5);
    #endif
	
    #if _SL_MIC_NUM==3
	SoundLocationInit(SSLCtl, RTdVadCtl, SSL_Buf, FFT_Ctrl);
    #elif _SL_MIC_NUM==2
	#if _EF_SERIES
		SSL_Buf->EFFlag = 1;
	#else
		SSL_Buf->EFFlag = 0;
	#endif
	SoundLocation180Init(SSLCtl, RTdVadCtl, SSL_Buf, FFT_Ctrl);
	#endif
	
	SL_State=SL_INIT;	
	#if _EF_SERIES
        #if _SL_MIC_NUM==3
		InitAdc(_SL_SampleRate*4); 
        #elif _SL_MIC_NUM==2
		InitAdc(_SL_SampleRate*2); 
        #endif
	#else
		InitAdc(_SL_SampleRate);
	#endif
	
	SL_State=SL_ON;
	ADC_Cmd(ENABLE);
}
//------------------------------------------------------------------//
// Sound_Localization Stop
// Description: 1.HW PIN Disable, 2.ADC OFF 3.FreeMemory
// Parameter: None			
//------------------------------------------------------------------// 
void SL_Stop(void)
{
	if(SL_State!=SL_ON)	return;	
	SL_State=SL_OFF;
	ADC_Cmd(DISABLE);
	#if _USE_HEAP_RAM
		SL_FreeMemory();
	#endif	
	GPIO_Write(_SL_AMP_SD_CTL_PORT,_SL_AMP_SD_CTL_PIN,_SL_AMP_SD_CTL_ACTIVE); //HW Disable
}
//------------------------------------------------------------------//
// Sound_Localization GetStatus
// Description: SL_State: 1.SL_OFF 2.SL_ON 3.SL_INIT, return
// Parameter: return SL_State 
//------------------------------------------------------------------// 
U8 SL_GetStatus()
{
	return SL_State;
}
//------------------------------------------------------------------//
// Sound_Localization Degree Divide
// Description: Degree Divide: 
// 3Mics: 30,60,120,150,180,210,240,270,300,330,360.
// 2Mics: 0,40,60,75,90,105,120,140,180.
// Parameter: return SL_State 
//------------------------------------------------------------------// 
void SL_Degree_Divide(S16 degree)
{
	if(degree > 359 || degree==-1)
	{
		_SL_Degree_Divide= -1;
	}
	else
	{
        #if _SL_MIC_NUM==3
		_SL_Degree_Divide = ((degree+15)/30)*30;//30,60,120,150,180,210,240,270,300,330,360.
        #elif _SL_MIC_NUM==2
		_SL_Degree_Divide = degree;//0,40,60,75,90,105,120,140,180.
        #endif
		if(_SL_Degree_Divide==360) _SL_Degree_Divide=0;
	}
}
#endif
//------------------------------------------------------------------//  
