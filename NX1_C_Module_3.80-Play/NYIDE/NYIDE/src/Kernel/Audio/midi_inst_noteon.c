#include "string.h"
#include "include.h"
#include "nx1_lib.h"

#if _MIDI_INST_NOTEON

#ifndef _MIDI_INST_NOTEON_MAXCH
#define _MIDI_INST_NOTEON_MAXCH	_MIDI_CH_NUM
#endif
#if _MIDI_INST_NOTEON_MAXCH>_MIDI_CH_NUM
#error "The NoteOn max. channel setting is over the number of MIDI channels."
#elif _MIDI_INST_NOTEON_MAXCH<=0
#error "The NoteOn max. channel setting error. (1~_MIDI_CH_NUM)"
#endif
#if _SPI1_MODULE && _SPI1_USE_FLASH
#warning "The NoteOn isn't supported on SPI1 Flash."
#endif

#ifndef _MIDI_INST_NOTEON_TYPE
#define _MIDI_INST_NOTEON_TYPE	(TYPE1)
#endif

#define _NOTEON_EVENT_SIZE		(16)
#if		(_MIDI_INST_NOTEON_TYPE==TYPE0 || _MIDI_INST_NOTEON_TYPE==TYPE1)
#define _STS_EMPTY				(0xFFFF)
#elif 	(_MIDI_INST_NOTEON_TYPE==TYPE2 || _MIDI_INST_NOTEON_TYPE==TYPE3)
#define _STS_EMPTY				(0xFF)
#define _MAX_SN					(100)
#if _MIDI_INST_RECORD
#error "_MIDI_INST_RECORD not support this type."
#endif
#else
#error "_MIDI_INST_NOTEON_TYPE setting error."
#endif

enum{
	i_Inst=0,
	i_Pitch,
	i_Velocity,
	i_FwCh=2,
#if 	(_MIDI_INST_NOTEON_TYPE==TYPE0 || _MIDI_INST_NOTEON_TYPE==TYPE1)	
	i_Mark=3,
#elif 	(_MIDI_INST_NOTEON_TYPE==TYPE2 || _MIDI_INST_NOTEON_TYPE==TYPE3)
	i_Sn=3,
#endif	
}NoteOn_Info;

U8 noteOn_EventSize = _NOTEON_EVENT_SIZE;

U8 noteOn_EventPw, noteOn_EventPr;
#if		(_MIDI_INST_NOTEON_TYPE==TYPE2 || _MIDI_INST_NOTEON_TYPE==TYPE3)
U8 noteOn_EventSn;
#endif
U8 noteOn_EventBuff[_NOTEON_EVENT_SIZE][4];				/*** [][0]:Inst, [][1]:Pitch, [][2]:Velocity, [][3]:Mark/Serial number ***/ 
U8 *noteOn_EventPtr = noteOn_EventBuff[0];

U8 noteOff_EventNum;
#if		(_MIDI_INST_NOTEON_TYPE==TYPE0 || _MIDI_INST_NOTEON_TYPE==TYPE1)
U16 noteOff_EventBuff[_NOTEON_EVENT_SIZE];				/*** []:Inst & Pitch ***/
U16 *noteOff_EventPtr = noteOff_EventBuff;
#elif	(_MIDI_INST_NOTEON_TYPE==TYPE2 || _MIDI_INST_NOTEON_TYPE==TYPE3)
U8 noteOff_EventBuff[_NOTEON_EVENT_SIZE];				/*** []:serial number ***/
U8 *noteOff_EventPtr = noteOff_EventBuff;
#endif

U8 noteOn_PlayNum;
U8 noteOn_PlayBuff[_MIDI_INST_NOTEON_MAXCH][4];			/*** [][0]:Inst, [][1]:Pitch, [][2]:Assign FwCh, [][3]:Mark/Serial number ***/ 
U8 *noteOn_PlayPtr = noteOn_PlayBuff[0];

#if _MIDI_INST_NOTEON_GLISS
U8 noteOn_GlissNum;
U8 noteOn_Gliss_EventBuff[_MIDI_INST_NOTEON_MAXCH][4];	/*** [][0]:note, [][1]:semitone, [][2]:tick_H, [][3]:tick_L ***/
U8 *noteOn_Gliss_EventPtr = noteOn_Gliss_EventBuff[0];

U32 noteOn_GlissBuff1[_MIDI_INST_NOTEON_MAXCH][2];		/*** [][0]:Current, [][1]:Step ***/
U32 *noteOn_GlissPtr1 = noteOn_GlissBuff1[0];
U16 noteOn_GlissBuff2[_MIDI_INST_NOTEON_MAXCH][2];		/*** [][0]:Tick_Cnt, [][1]:TargetIndex ***/
U16 *noteOn_GlissPtr2 = noteOn_GlissBuff2[0];
#endif

static U8 chk_NoteOff_event(U16 dat);
//void _debug_Buff(U8 dat);

U8 InstNoteOffFlag=1;		//default:1, NoteOff API will be executed.

//------------------------------------------------------------------//
// Instrument NoteOn
// Parameter: 
//			Instrument: Instrument Number, Ref.to Q-MIDI(1:Bright_Grand Piano, 128:Percussions) 
//          Pitch:Pitch Number(EX:c1=24,Db1=25,C2=36)
//			Velocity:(0~127)
// return value:
//			0             : fifo full
//          1    	      : ok						@ _MIDI_INST_NOTEON_TYPE = TYPE0/1
//          1~(_MAX_SN-1) : event serial number		@ _MIDI_INST_NOTEON_TYPE = TYPE2/3
//------------------------------------------------------------------// 
U8 InstNote_On(U8 Instrument,U8 Pitch,U8 Velocity)
{
	INST_NoteOn_INIT();
	
	if( (noteOn_EventPw+1)%_NOTEON_EVENT_SIZE == noteOn_EventPr ) {
		//dprintf("\x1b[91m!!! KeyOn wr fifo fail, full!!!\x1b[m\n\r");
		return (0);
	}

	noteOn_EventBuff[noteOn_EventPw][i_Inst] 		= Instrument;
	noteOn_EventBuff[noteOn_EventPw][i_Pitch] 		= Pitch;
	noteOn_EventBuff[noteOn_EventPw][i_Velocity] 	= Velocity;
#if (_MIDI_INST_NOTEON_TYPE==TYPE2 || _MIDI_INST_NOTEON_TYPE==TYPE3)	
	if(noteOn_EventSn==_MAX_SN) noteOn_EventSn=1;
	noteOn_EventBuff[noteOn_EventPw][i_Sn] 			= noteOn_EventSn;
#endif

#if _MIDI_INST_RECORD
	instNote_On_RecData(Pitch, Velocity, SysTick);
#endif

	noteOn_EventPw = (noteOn_EventPw+1)%_NOTEON_EVENT_SIZE;
	//dprintf("key On = %d\n\r",Pitch);
	//_debug_Buff(255);
	
#if		(_MIDI_INST_NOTEON_TYPE==TYPE2 || _MIDI_INST_NOTEON_TYPE==TYPE3)
	return (noteOn_EventSn++);
#else
	return (1);
#endif	
}


//------------------------------------------------------------------//
// Instrument NoteOff
// Parameter: 
//			Instrument: Instrument Number, Ref.to Q-MIDI(1:Bright_Grand Piano, 128:Percussions) 
//          Pitch: pitch Number(EX:c1=24,c2=25)
// return value:
//			0 : fail
//			1 : ok
//------------------------------------------------------------------// 
U8 InstNote_Off(U8 Instrument,U8 Pitch)
{
	if(!InstNoteOffFlag) return (1);
	
	U8 i=0;

#if 	(_MIDI_INST_NOTEON_TYPE==TYPE0 || _MIDI_INST_NOTEON_TYPE==TYPE1)	
	U16 tmp = (Pitch<<8) | Instrument;
#elif 	(_MIDI_INST_NOTEON_TYPE==TYPE2 || _MIDI_INST_NOTEON_TYPE==TYPE3)
	U8 tmp = Instrument; /*** serial number ***/

	if(tmp==0) {
		//dprintf("\x1b[91m!!! KeyOff sn error = 0 !!!\x1b[m\n\r");
		return (0);
	}
#endif

	if(chk_NoteOff_event(tmp)==0) {
		//dprintf("\x1b[91m!!! keyoff invalid = %x!!!\x1b[m\n\r",tmp);
		return (0);
	}
	
	do {
		if(*(noteOff_EventPtr+i) == _STS_EMPTY)
		{
			*(noteOff_EventPtr+i) = tmp;
			noteOff_EventNum++;
			//dprintf("key Off = %d, %d\n\r",Pitch, noteOff_EventNum);
			//_debug_Buff(255);
#if _MIDI_INST_RECORD
			instNote_Off_RecData(0, Pitch, SysTick);
#endif												
			return (1);			
		}
	} while(++i < _NOTEON_EVENT_SIZE);
	
	//dprintf("\x1b[91m!!! KeyOff wr fifo fail, full!!!\x1b[m\n\r");
	return (0);
}

//------------------------------------------------------------------//
// Check if NoteOff event is valid
// Parameter: 
//			dat : Instrument & Pitch @ _MIDI_INST_NOTEON_TYPE = TYPE0
//                Pitch				 @ _MIDI_INST_NOTEON_TYPE = TYPE1
//                Serial number		 @ _MIDI_INST_NOTEON_TYPE = TYPE2/3
// return value:
//          1 : valid
//			0 : invalid
//------------------------------------------------------------------//
static U8 chk_NoteOff_event(U16 dat)
{
	U8 i;
#if 	(_MIDI_INST_NOTEON_TYPE==TYPE0 || _MIDI_INST_NOTEON_TYPE==TYPE1)		
	U8 H_dat = dat>>8;
#endif
	U8 L_dat = (U8)dat;	
	
	/*** chk NoteOn playing buff ***/
	for(i=0; i<_MIDI_INST_NOTEON_MAXCH; i++) {	
#if 	(_MIDI_INST_NOTEON_TYPE==TYPE0)
		if(H_dat == noteOn_PlayBuff[i][i_Pitch] && L_dat == noteOn_PlayBuff[i][i_Inst] && noteOn_PlayBuff[i][i_Mark]==0)
		{
			noteOn_PlayBuff[i][i_Mark]=1;
#elif   (_MIDI_INST_NOTEON_TYPE==TYPE1)
		if( 	(L_dat==128 && H_dat == noteOn_PlayBuff[i][i_Pitch] && L_dat == noteOn_PlayBuff[i][i_Inst] && noteOn_PlayBuff[i][i_Mark]==0) \
			||	(L_dat!=128 && H_dat == noteOn_PlayBuff[i][i_Pitch] && noteOn_PlayBuff[i][i_Mark]==0) \
		)
		{
			noteOn_PlayBuff[i][i_Mark]=1;
#elif 	(_MIDI_INST_NOTEON_TYPE==TYPE2 || _MIDI_INST_NOTEON_TYPE==TYPE3)
		if(L_dat == noteOn_PlayBuff[i][i_Sn])
		{
#endif
			return (1);
		}
	}
	
	/*** chk NoteOn event buff ***/
	for(i=0; i<_NOTEON_EVENT_SIZE; i++) {
#if 	(_MIDI_INST_NOTEON_TYPE==TYPE0)
		if(H_dat == noteOn_EventBuff[i][i_Pitch] && L_dat == noteOn_EventBuff[i][i_Inst] && noteOn_EventBuff[i][i_Mark]==0)
		{
			noteOn_EventBuff[i][i_Mark]=1;
#elif   (_MIDI_INST_NOTEON_TYPE==TYPE1)			
		if( 	(L_dat==128 && H_dat == noteOn_EventBuff[i][i_Pitch] && L_dat == noteOn_EventBuff[i][i_Inst] && noteOn_EventBuff[i][i_Mark]==0) \
			||	(L_dat!=128 && H_dat == noteOn_EventBuff[i][i_Pitch] && noteOn_EventBuff[i][i_Mark]==0) \
		)
		{
			noteOn_EventBuff[i][i_Mark]=1;			
#elif 	(_MIDI_INST_NOTEON_TYPE==TYPE2 || _MIDI_INST_NOTEON_TYPE==TYPE3)
		if(L_dat == noteOn_EventBuff[i][i_Sn])
		{
#endif
			return (1);
		}
	}
	
	return (0);
}

//------------------------------------------------------------------//
// Instrument All NoteOff
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void InstNote_AllOff(void)	//Fast
{
	InstNote_On(0xFF, 0xFF, 0xFF); 
}
void InstNote_AllOff2(void)	//Normal
{
	InstNote_On(0xFE, 0xFF, 0xFF); 
}

//------------------------------------------------------------------//
// Instrument Sustaining Pedal control
// Parameter: 
//          cmd: 1(Pedal On), 0(Pedal Off)
// return value:
//          NONE
//------------------------------------------------------------------// 
void InstNote_SustainPedal(U8 cmd)
{
	if(cmd) {					//Pedal On (down)
		InstNoteOffFlag=0;
	}
	else {						//Pedal Off (released)
		InstNoteOffFlag=1;
		InstNote_AllOff2();		
	}
}

//------------------------------------------------------------------//
// Instrument initial
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------// 
void Inst_NoteOn_init(void)
{
	noteOn_EventPw=0;
	noteOn_EventPr=0;
#if (_MIDI_INST_NOTEON_TYPE==TYPE2 || _MIDI_INST_NOTEON_TYPE==TYPE3)		
	noteOn_EventSn=1;
#endif
	memset((void *)noteOn_EventBuff, 0x00, sizeof(noteOn_EventBuff)); 
	
	noteOff_EventNum=0; 
	memset((void *)noteOff_EventBuff, _STS_EMPTY, sizeof(noteOff_EventBuff)); 
	
	noteOn_PlayNum=0;
	memset((void *)noteOn_PlayBuff, 0xFF, sizeof(noteOn_PlayBuff));
	
#if _MIDI_INST_NOTEON_GLISS
	noteOn_GlissNum=0;
	memset((void *)noteOn_Gliss_EventBuff, 0x00, sizeof(noteOn_Gliss_EventBuff));
	memset((void *)noteOn_GlissBuff1, 0, sizeof(noteOn_GlissBuff1));
	memset((void *)noteOn_GlissBuff2, 0, sizeof(noteOn_GlissBuff2)); 
	
	U8 i;
	for(i=0; i<_MIDI_INST_NOTEON_MAXCH; i++)
	{
		noteOn_GlissBuff1[i][0]=(16384<<9);	//current
	}
#endif	
}

//------------------------------------------------------------------//
// Set MIDI song volume
// Parameter: 
//          volume: 0~255, 255(default), 0(mute)
// return value:
//          NONE
//------------------------------------------------------------------// 
void NoteOn_MIDI_Vol(U8 volume)
{
	NoteOn_MidiVol = volume;
}

//------------------------------------------------------------------//
// Set NoteOn volume
// Parameter: 
//          volume: 0~255, 255(default), 0(mute)
// return value:
//          NONE
//------------------------------------------------------------------// 
void NoteOn_INST_Vol(U8 volume)
{
	NoteOn_InstVol = volume;
}

#if _MIDI_INST_RECORD
//------------------------------------------------------------------//
// Instrument Record Module - Start record 
// Parameter: 
//          index: absolute address
//          mode: SPI_MODE
//          trigTout: set timeout after triggering recording (Unit:msec)
//          size: record size	
// return value:
//          0:ok
//          others:fail
//------------------------------------------------------------------// 
U8 InstNote_Record(U32 index,U8 mode,U16 trigTout,U32 size)
{
	if(mode!=SPI_MODE)
		return 1;
	
	CB_InstNote_Rec_PreSetting();
	instNote_Rec_Init(index,mode,trigTout,size);
	
	return 0;
}
//------------------------------------------------------------------//
// Instrument Record Module - Stop record
// Parameter: 
//          None
// return value:
//          None
//------------------------------------------------------------------// 
void InstNote_Rec_Stop(void)
{
	instNote_Rec_Stop();
}
//------------------------------------------------------------------//
// Instrument Record Module - Start play record
// Parameter: 
//          index: absolute address
//          mode: SPI_MODE
// return value:
//          0:ok
//          others:fail
//------------------------------------------------------------------// 
U8 InstNote_RecPlay(U32 index,U8 mode)
{
	U8 sts=0;
	
	if(mode!=SPI_MODE)
		return 1;
	
	INST_NoteOn_INIT(); 
	
	sts=instNote_RecPlay_Init(index,mode);
	
	return sts;
}
//------------------------------------------------------------------//
// Instrument Record Module - Stop play record
// Parameter: 
//          None
// return value:
//          None
//------------------------------------------------------------------// 
void InstNote_RecPlay_Stop(void)
{
	instNote_RecPlay_Stop();
	InstNote_AllOff();
	while(NoteOn_GetStatus()){
		AUDIO_ServiceLoop();
	}		
}
#endif //end: _MIDI_INST_RECORD

#if 0
#include "debug.h"
void _debug_Buff(U8 dat)
{
	U8 x, y;
	U32 *u32Ptr;
	
	dprintf("\n\r\t{0x%X} {%d} {%d}\n\r", NoteOn_CH_BusyFlag, noteOn_PlayNum, noteOff_EventNum);
	if(dat==0 || dat==255) {	/*** NoteOn event buff ***/		
		dprintf("\t==>0x Sn, Vel, Pitch, Inst\n\r");
		u32Ptr = (U32*) noteOn_EventPtr;
		for(y=0; y<_NOTEON_EVENT_SIZE; y++) {
			dprintf("\t 0x%08X\n\r", *(u32Ptr+y));
		}
	}
	if(dat==1 || dat==255) {	/*** NoteOff event buff ***/
		dprintf("\t==>0x Off_event\n\r");
		for(x=0; x<_NOTEON_EVENT_SIZE; x++) {
			dprintf("\t %X", noteOff_EventBuff[x]);
		}
		dprintf("\n\r");
	}	
	if(dat==2 || dat==255) {
		dprintf("\t==>0x Sn, Chx, Pitch, Inst\n\r");
		u32Ptr = (U32*) noteOn_PlayPtr;
		for(y=0; y<_MIDI_INST_NOTEON_MAXCH; y++) {
			dprintf("\t 0x%08X\n\r", *(u32Ptr+y));
		}		
	}
	dprintf("\n\r");
}
#endif

#endif //end: _MIDI_INST_NOTEON