#include <string.h>
#include "include.h"
#include "nx1_dac.h"

#if _IS_APPS_USE_DAC
    
#if _QCODE_BUILD
#else
const U8 _codecToDacMap[] = {//mapping by DAC_ASSIGNER_LIST, only support HW_CH0 / HW_CH1
    0,
    _SBC_PLAY_AUDIO_CH,
    _SBC_CH2_PLAY_AUDIO_CH,
    _ADPCM_PLAY_AUDIO_CH,
    _ADPCM_CH2_PLAY_AUDIO_CH,
    _CELP_PLAY_AUDIO_CH,
    _PCM_PLAY_AUDIO_CH,
    _MIDI_PLAY_AUDIO_CH,
    _ADPCM_CH3_PLAY_AUDIO_CH,
    _ADPCM_CH4_PLAY_AUDIO_CH,
    _PCM_CH2_PLAY_AUDIO_CH,
    _PCM_CH3_PLAY_AUDIO_CH,
    _SBC2_PLAY_AUDIO_CH,
    _SBC2_CH2_PLAY_AUDIO_CH,
    _RT_PLAY_AUDIO_CH,
    _WAVE_ID_AUDIO_CH,
    _ADPCM_CH5_PLAY_AUDIO_CH,
    _ADPCM_CH6_PLAY_AUDIO_CH,
    _ADPCM_CH7_PLAY_AUDIO_CH,
    _ADPCM_CH8_PLAY_AUDIO_CH,
};
#endif
SCOPE_TYPE DAC_ASSIGNER_MEM dacAssigner[_ASSIGNABLE_DAC_NUM];

//----------------------------------------------------------------------------//
// DAC auto assigner init
// Parameter: 
//          NONE
// return value:
//          NONE
//----------------------------------------------------------------------------//
void DacAssigner_Init(void)
{
	memset(dacAssigner,0,sizeof(dacAssigner));
}
//----------------------------------------------------------------------------//
// DAC auto assigner regist
// Parameter: 
//          _codec		:DAC_ASSIGNER_LIST
//			_sampleRate	:1~S32_MAX,or XXX_EXCLUSIVE
// return value:
//          DAC_CH		:0,1
//			-1			:No useable DAC CH
//----------------------------------------------------------------------------//
S8 DacAssigner_Regist(const DAC_ASSIGNER_LIST _codec,const  S32 _sampleRate)
{
    if(0==_sampleRate)return -1;
    S8 _dacCh;
    DacAssigner_unRegist(_codec);
#if _QCODE_BUILD
	for(_dacCh=0;_dacCh<_ASSIGNABLE_DAC_NUM;++_dacCh)
#else//C-Module
    for(_dacCh=_codecToDacMap[_codec];_dacCh<(_codecToDacMap[_codec]+1);++_dacCh)
#endif
    {
		if(_sampleRate==dacAssigner[_dacCh].s32_sampleRate){
			dacAssigner[_dacCh].u32_dacCodecMem |= (1<<_codec);
			return _dacCh;
		}
	}
#if _QCODE_BUILD
	for(_dacCh=0;_dacCh<_ASSIGNABLE_DAC_NUM;++_dacCh)
#else//C-Module
    for(_dacCh=_codecToDacMap[_codec];_dacCh<(_codecToDacMap[_codec]+1);++_dacCh)
#endif
    {
		if(0==dacAssigner[_dacCh].s32_sampleRate){
			dacAssigner[_dacCh].s32_sampleRate=_sampleRate;
			dacAssigner[_dacCh].u32_dacCodecMem |= (1<<_codec);
			return _dacCh;
		}
	}
	return -1;
}
//----------------------------------------------------------------------------//
// DAC auto assigner unregist
// Parameter: 
//          _codec		:DAC_ASSIGNER_LIST
// return value:
//          EXIT_SUCCESS
//			EXIT_FAILURE
//----------------------------------------------------------------------------//
U8 DacAssigner_unRegist(const DAC_ASSIGNER_LIST _codec)
{
	S8 _dacCh;
#if _QCODE_BUILD
	for(_dacCh=0;_dacCh<_ASSIGNABLE_DAC_NUM;++_dacCh)
#else//C-Module
    for(_dacCh=_codecToDacMap[_codec];_dacCh<(_codecToDacMap[_codec]+1);++_dacCh)
#endif
    {
		if(DacAssigner_isCodecUseDac(_codec,_dacCh)){
			dacAssigner[_dacCh].u32_dacCodecMem &= ~(1<<_codec);
			if(0==dacAssigner[_dacCh].u32_dacCodecMem){
				dacAssigner[_dacCh].s32_sampleRate=0;
			}
			return EXIT_SUCCESS;
		}
	}
	return EXIT_FAILURE;
}
//----------------------------------------------------------------------------//
// Find DAC CH by codec
// Parameter: 
//          _codec		:DAC_ASSIGNER_LIST
// return value:
//          DAC_CH		:0,1
//			-1			:No useable DAC CH
//----------------------------------------------------------------------------//
S8 DacAssigner_codecUseDacCh(const DAC_ASSIGNER_LIST _codec)
{
	S8 _dacCh;
#if _QCODE_BUILD
	for(_dacCh=0;_dacCh<_ASSIGNABLE_DAC_NUM;++_dacCh)
#else//C-Module
    for(_dacCh=_codecToDacMap[_codec];_dacCh<(_codecToDacMap[_codec]+1);++_dacCh)
#endif
    {
		if(DacAssigner_isCodecUseDac(_codec,_dacCh)){
			return _dacCh;
		}
	}
	return -1;
}
//----------------------------------------------------------------------------//
// Is codec use DAC
// Parameter: 
//          _codec		:DAC_ASSIGNER_LIST
//			_dacCh		:HW_CH0(0),HW_CH1(1)
// return value:
//          true/false
//----------------------------------------------------------------------------//
bool DacAssigner_isCodecUseDac(const DAC_ASSIGNER_LIST _codec,const S8 _dacCh)
{
	//if((_dacCh<0)||(_dacCh>=_ASSIGNABLE_DAC_NUM))return false;
    return ((dacAssigner[_dacCh].u32_dacCodecMem & (1<<_codec)) != 0);
}
//----------------------------------------------------------------------------//
// get regist info
// Parameter: 
//			_dacCh		:HW_CH0(0),HW_CH1(1)
// return value:
//          true/false
//----------------------------------------------------------------------------//
U32 DacAssigner_registInfo(const S8 _dacCh)
{
	//if((_dacCh<0)||(_dacCh>=_ASSIGNABLE_DAC_NUM))return false;
    return dacAssigner[_dacCh].u32_dacCodecMem;
}
//----------------------------------------------------------------------------//
// Is all dac regist empty
// Parameter: 
//          NONE
// return value:
//          true/false
//----------------------------------------------------------------------------//
bool DacAssigner_isEmpty()
{
	S8 _dacCh;
	for(_dacCh=0;_dacCh<_ASSIGNABLE_DAC_NUM;++_dacCh){
		if(dacAssigner[_dacCh].u32_dacCodecMem) return false;
	}
	return true;
}
//----------------------------------------------------------------------------//
// DAC fifo data number check
// Parameter: 
//			_dacCh		:HW_CH0(0),HW_CH1(1)
// return value:
//          EXIT_SUCCESS/EXIT_FAILURE
//----------------------------------------------------------------------------//
U8 DacAssigner_fifoDataNumberCheck(const S8 _dacCh)
{
	//if((_dacCh<0)||(_dacCh>=_ASSIGNABLE_DAC_NUM))return EXIT_FAILURE;
	if(dacAssigner[_dacCh].u32_dacCodecMem!=0){
		dacAssigner[_dacCh].u8_fifoDataMem=DAC_FIFO_LEN*2;
		dacAssigner[_dacCh].u8_dacDisableFlag=0;
	}else{
		if(dacAssigner[_dacCh].u8_fifoDataMem>(DAC_FIFO_LEN-DAC_FIFO_THD)){
			dacAssigner[_dacCh].u8_fifoDataMem-=(DAC_FIFO_LEN-DAC_FIFO_THD);
		}else{
			dacAssigner[_dacCh].u8_fifoDataMem=0;
			dacAssigner[_dacCh].u8_dacDisableFlag=1;
		}
	}
	return EXIT_SUCCESS;
}
//----------------------------------------------------------------------------//
// DacAssigner(For polling Idle DAC and disable it.)
// Parameter: 
//			NONE
// return value:
//          NONE
//----------------------------------------------------------------------------//
void DacAssigner_ServiceLoop(void)
{
	S8 _dacCh;
	GIE_DISABLE();
	for(_dacCh=0;_dacCh<_ASSIGNABLE_DAC_NUM;++_dacCh){
		if(   (dacAssigner[_dacCh].u8_dacDisableFlag!=0)//Condition 1
            &&(dacAssigner[_dacCh].u32_dacCodecMem==0)	//Condition 2
            &&(DacMixerRamp_IsIdle()==true)
        ) {
				dacAssigner[_dacCh].u8_dacDisableFlag=0;
				DAC_Cmd(pHwCtl[_dacCh].p_dac,DISABLE);
		}
	}
    GIE_ENABLE();
    
    do {
#if _DAC_MODE
        if(DacMixerRamp_IsDacModeUse()==true) break;
#endif
        if(DacMixerRamp_IsIdle()==true) break;
        DacMixerRamp_SetRampDown();
    } while(0);
}
#endif //#if _IS_APPS_USE_DAC