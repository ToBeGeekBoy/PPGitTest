/* Includes ------------------------------------------------------------------*/

#include "include.h"
#include "nx1_dac.h"
#include "nx1_adc.h"
#include "debug.h"
#include <string.h>

/* Defines -------------------------------------------------------------------*/
#define SUPPORT_MIDI_MARK 		DISABLE 		//ENABLE:1 DISABLE:0
#define SUPPORT_MIDI_SECTION	DISABLE			//ENABLE:1 DISABLE:0
#define SUPPORT_NoteOn_PITCH	DISABLE			//ENABLE:1 DISABLE:0

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
void CB_MIDI_InitDac(CH_TypeDef *chx,U16 sample_rate)
{    
    InitDac(chx,sample_rate,1);
}    
//------------------------------------------------------------------//
// Dac command
// Parameter: 
//          chx         : DAC_CH0, DAC_CH1
//          cmd         : ENABLE,DISABLE
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_MIDI_DacCmd(CH_TypeDef *chx,U8 cmd)
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
void CB_MIDI_WriteDac(U8 size,U16* psrc16)
{    
    U8 i;

    if(psrc16!=0)
    {
        for(i=0;i<size;i++)
        {
#if _AUDIO_MIX_CONTROL
            extern U8 MidiMixCtrl;
            DacMixBuf[i]+=(S32)((((S16)*psrc16)*MidiMixCtrl)>>2);
#else
            DacMixBuf[i]+=(S32)(S16)*psrc16;
#endif
            psrc16++;
        }   
    }   
}   



void CB_MIDI_PlayStart(void)
{
	
}  
void CB_MIDI_PlayEnd(void)
{
	
}          
void CB_MIDI_PlayPreprocess(void)
{

}

#if (_MIDI_INST_NOTEON&&_MIDI_INST_RECORD)
#if _QCODE_BUILD==0
U8 UserCtrl_Inst=0;
#endif
//------------------------------------------------------------------//
// Instrument Record Module - RecPlay NoteOn callback function
// Parameter: (from NX1 algorithm library)
//          pitch: decode data
//          vel: decode data 
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_InstNote_RecPlay_NoteOn(U8 pitch, U8 vel)
{
	/* Example:
		InstNote_On(UserCtrl_Inst, pitch, vel);	//1st parameter control by user.
	end Example */
#if _QCODE_BUILD
	QC_InstNote_RecPlay_NoteOn(pitch,vel);
#else
	InstNote_On(UserCtrl_Inst, pitch, vel);
#endif
}
//------------------------------------------------------------------//
// Instrument Record Module - RecPlay NoteOff callback function
// Parameter: (from NX1 algorithm library)
//          sustain_Pedal: (1) decode data (1:Pedal press ; 0:Pedal release or Not used pedal)
//						   (2) according to this parameter and the selected instrument, decide whether to execute "NoteOff" API.                
//          pitch: decode data
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_InstNote_RecPlay_NoteOff(U8 sustain_Pedal, U8 pitch) 
{	
	/* Example:
		InstNote_Off(UserCtrl_Inst, pitch);	//1st parameter control by user.
	end Example */
#if _QCODE_BUILD
	QC_InstNote_RecPlay_NoteOff(sustain_Pedal,pitch);
#else	
	InstNote_Off(UserCtrl_Inst, pitch);
#endif
}
//------------------------------------------------------------------//
// Instrument Record Module - Record over setting time callback function
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_InstNote_Rec_TrigTimeout(void)
{
#if _QCODE_BUILD
	QC_InstNote_Rec_SetTimeout();
#endif
}
#endif	//end: (_MIDI_INST_NOTEON&&_MIDI_INST_RECORD)

#if SUPPORT_MIDI_MARK
void func0(void){} 

//U32 M1;
void func1(void)
{
    //MIDI_Seq_Store(&M1);
}    
void func2(void)
{
}    
void func3(void)
{
}
void func4(void)
{
    //MIDI_Seq_Return(&M1);
}      
void (*MidiMarkFunc[MIDIMARK_FUNC_NUM])(void);
void (*MidiMarkFunc[MIDIMARK_FUNC_NUM])(void) = {func0, func1, func2, func3, func4};
#pragma weak MidiMarkFunc
#endif //SUPPORT_MIDI_MARK
//------------------------------------------------------------------//
// Callback fucntion when Midi Mark happens
// Parameter: 
//          id: mark id
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_MidiMark(U8 id){
#if _QCODE_BUILD
#if _MIDI_PLAY
    QC_MIDI_PushMark(id);
#endif
#else
#if SUPPORT_MIDI_MARK
	void (*func)(void);  

    if(id>=MIDIMARK_FUNC_NUM){
        return;
    }    
    func=MidiMarkFunc[id];
    
    if(func==0){
        return;
    }       
    func();
#endif
#endif
}

#if SUPPORT_MIDI_SECTION
void function_1(void){
    //Function 1
}    
void function_2(void){
    //Function 2
}    
void function_3(void){
    //Function 3
} 
void (*NoteOn_func)(void);
void (*MidiSectionFunc[MIDISECTION_FUNC_NUM])(void) = {function_1, function_2, function_3};  
#endif //SUPPORT_MIDI_SECTION
//------------------------------------------------------------------//
// Callback fucntion when Midi channel has Note event
// Parameter: 
//          id: mark id
// return value:
//          NONE
//------------------------------------------------------------------// 
void CB_MidiSection(U16 *NoteOn_Section)//20180420 Roy
{
#if _QCODE_BUILD
#if _MIDI_NOTEON_STATE
    extern U16 *MidiNoteOnEvent;
    MidiNoteOnEvent = NoteOn_Section;
#endif
#else
#if SUPPORT_MIDI_SECTION
	U8 i;
	for(i=0; i<MIDISECTION_FUNC_NUM; i++)
	{
		if(*NoteOn_Section & (1<<i))
		{
			NoteOn_func = MidiSectionFunc[i];
			if(NoteOn_func==0)
				return;
			NoteOn_func();
			*NoteOn_Section &=~((1<<i));
		}
	}
#endif
#endif
}

//------------------------------------------------------------------//
// Callback fucntion when Note event is assigned
// Parameter: 
//          *(NoteInfo+0) : midi channel
//          *(NoteInfo+1) : midi patch
//          *(NoteInfo+2) : midi pitch
// return value:
//          NONE
//------------------------------------------------------------------//
#if SUPPORT_NoteOn_PITCH 
void CB_GetPitch(U8 *NoteInfo)
{
	//dprintf("Current_channel=%d, _patch=%d, _pitch=%d\n\r",*(NoteInfo), *(NoteInfo+1), *(NoteInfo+2));
	
}
#endif //SUPPORT_NoteOn_PITCH

#endif	//end: _AUDIO_MODULE
