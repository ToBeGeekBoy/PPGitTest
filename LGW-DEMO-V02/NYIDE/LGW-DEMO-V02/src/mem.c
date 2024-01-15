#include "include.h"

#if _USE_HEAP_RAM
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * List of Predictive Memory Usage *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  Each algorithm HEAP Size
//====================================================================================
#define _SBC_PLAY_HEAP_RAM              SBC_CH1_PLAY_RAM        // SBC Play heap RAM size
#define _SBC_RECORD_HEAP_RAM            SBC_CH1_REC_RAM         // SBC Rec heap RAM size
#define _SBC_PITCH_SPEED_HEAP_RAM       SBC_CH1_PITCH_RAM       // SBC pitch/speed heap RAM size
#define _SBC_ROBOT2_HEAP_RAM            SBC_CH1_ROBOT2_RAM      // SBC Robot2 heap RAM size
#define _SBC_ROBOT3_HEAP_RAM            SBC_CH1_ROBOT3_RAM      // SBC Robot3 heap RAM size
#define _SBC_ROBOT4_HEAP_RAM            SBC_CH1_ROBOT4_RAM      // SBC Robot4 heap RAM size
#define _SBC_REVERB_HEAP_RAM            SBC_CH1_REVERB_RAM      // SBC Reverb heap RAM size
#define _SBC_ECHO_HEAP_RAM              SBC_CH1_ECHO_RAM        // SBC ECHO heap RAM size
#define _SBC_CH2_PLAY_HEAP_RAM          SBC_CH2_PLAY_RAM  		// SBC Play CH2 heap RAM size
#define _SBC_CH2_PITCH_SPEED_HEAP_RAM   SBC_CH2_PITCH_RAM       // SBC CH2 pitch/speed RAM size
#define _SBC_CH2_ROBOT2_HEAP_RAM       	SBC_CH2_ROBOT2_RAM      // SBC CH2 Robot2 heap RAM size
#define _SBC_CH2_ROBOT3_HEAP_RAM       	SBC_CH2_ROBOT3_RAM      // SBC CH2 Robot3 heap RAM size
#define _SBC_CH2_REVERB_HEAP_RAM        SBC_CH2_REVERB_RAM      // SBC CH2 Reverb heap RAM size
#define _SBC_CH2_ECHO_HEAP_RAM          SBC_CH2_ECHO_RAM        // SBC CH2 ECHO heap RAM size		
#define _SBC2_PLAY_HEAP_RAM         	SBC2_CH1_PLAY_RAM       // SBC-2 Play heap RAM size			
#define _SBC2_CH2_PLAY_HEAP_RAM         SBC2_CH2_PLAY_RAM       // SBC-2 CH2 Play heap RAM size		
#define _ADPCM_PLAY_HEAP_RAM            ADPCM_CH1_PLAY_RAM      // ADPCM Play heap RAM size
#define _ADPCM_RECORD_HEAP_RAM          ADPCM_CH1_REC_RAM       // ADPCM Rec heap RAM size
#define _ADPCM_PITCH_SPEED_HEAP_RAM     ADPCM_CH1_PITCH_RAM     // ADPCM pitch/speed RAM size
#define _ADPCM_SPEEDPITCHN_HEAP_RAM     ADPCM_CH1_SPN_RAM       // ADPCM SpeedPitchN RAM size
#define _ADPCM_ROBOT2_HEAP_RAM          ADPCM_CH1_ROBOT2_RAM    // ADPCM Robot2 RAM size
#define _ADPCM_ROBOT3_HEAP_RAM          ADPCM_CH1_ROBOT3_RAM    // ADPCM Robot3 RAM size
#define _ADPCM_ROBOT4_HEAP_RAM          ADPCM_CH1_ROBOT4_RAM    // ADPCM Robot4 RAM size
#define _ADPCM_REVERB_HEAP_RAM          ADPCM_CH1_REVERB_RAM    // ADPCM Reverb heap RAM size 
#define _ADPCM_ECHO_HEAP_RAM            ADPCM_CH1_ECHO_RAM      // ADPCM ECHO heap RAM size
#define _ADPCM_DARTH_HEAP_RAM           ADPCM_CH1_DARTH_RAM     // ADPCM DARTH heap RAM size
#define _ADPCM_ANIMAL_ROAR_HEAP_RAM     ADPCM_CH1_ANIMAL_ROAR_RAM // ADPCM Animal Roar heap RAM size
#define _ADPCM_CH2_PLAY_HEAP_RAM        ADPCM_CHX_PLAY_RAM      // ADPCM Play CH2~8 heap RAM size 
#define _CELP_PLAY_HEAP_RAM             CELP_PLAY_RAM           // CELP Play heap RAM size
#define _CELP_PITCH_SPEED_HEAP_RAM      CELP_PITCH_RAM          // CELP pitch/speed heap RAM size
#define _CELP_ROBOT2_RAM                CELP_ROBOT2_RAM         // CELP Robot2 heap RAM size
#define _CELP_ROBOT3_RAM                CELP_ROBOT3_RAM         // CELP Robot3 heap RAM size
#define _CELP_REVERB_RAM                CELP_REVERB_RAM         // CELP Reverb heap RAM size 
#define _CELP_ECHO_RAM                  CELP_ECHO_RAM           // CELP Echo heap RAM size 
#define _MIDI_PLAY_HEAP_RAM             ((MIDI_AUDIO_BUF_SIZE<<1)+(MIDI_RAM_ALLOCATE_SIZE<<2)) // MIDI heap RAM size
#define _PCM_PLAY_HEAP_RAM              PCM_CH1_PLAY_RAM        // PCM Play heap RAM size 
#define _PCM_RECORD_HEAP_RAM            PCM_CH1_REC_RAM         // PCM Rec heap RAM size
#define _PCM_PITCH_SPEED_HEAP_RAM       PCM_CH1_PITCH_RAM       // PCM pitch/speed heap RAM size
#define _PCM_ROBOT2_RAM                 PCM_CH1_ROBOT2_RAM      // PCM Robot2 heap RAM size
#define _PCM_ROBOT3_RAM                 PCM_CH1_ROBOT3_RAM      // PCM Robot3 heap RAM size
#define _PCM_REVERB_RAM                 PCM_CH1_REVERB_RAM      // PCM Reverb heap RAM size 
#define _PCM_ECHO_RAM                   PCM_CH1_ECHO_RAM        // PCM Echo heap RAM size 
#define _PCM_CH2_PLAY_HEAP_RAM          PCM_CH2_PLAY_RAM        // PCM Play CH2 heap RAM size 
#define _PCM_CH2_PITCH_SPEED_HEAP_RAM   PCM_CH2_PITCH_RAM       // PCM CH2 pitch/speed heap RAM size
#define _PCM_CH2_ROBOT2_RAM             PCM_CH2_ROBOT2_RAM      // PCM CH2 Robot2 heap RAM size
#define _PCM_CH2_ROBOT3_RAM             PCM_CH2_ROBOT3_RAM      // PCM CH2 Robot3 heap RAM size
#define _PCM_CH2_REVERB_RAM             PCM_CH2_REVERB_RAM      // PCM CH2 Reverb heap RAM size 
#define _PCM_CH2_ECHO_RAM               PCM_CH2_ECHO_RAM        // PCM CH2 Echo heap RAM size 
#define _PCM_CH3_PLAY_HEAP_RAM          PCM_CH3_PLAY_RAM        // PCM Play CH3 heap RAM size 
#define _PCM_CH3_PITCH_SPEED_HEAP_RAM   PCM_CH3_PITCH_RAM       // PCM CH3 pitch/speed heap RAM size
#define _PCM_CH3_ROBOT2_RAM             PCM_CH3_ROBOT2_RAM      // PCM CH3 Robot2 heap RAM size
#define _PCM_CH3_ROBOT3_RAM             PCM_CH3_ROBOT3_RAM      // PCM CH3 Robot3 heap RAM size
#define _PCM_CH3_REVERB_RAM             PCM_CH3_REVERB_RAM      // PCM CH3 Reverb heap RAM size 
#define _PCM_CH3_ECHO_RAM               PCM_CH3_ECHO_RAM        // PCM CH3 Echo heap RAM size 
#define _RT_PLAY_HEAP_RAM               RT_BASE_RAM				// Real time heap RAM size /w Howling
#define _RT_PITCH_HEAP_RAM              RT_PITCH_RAM            // Real time Pitch heap RAM size
#define _RT_ROBOT1_HEAP_RAM				RT_ROBOT1_RAM			// Real time Robot1 heap RAM size
#define _RT_ROBOT2_HEAP_RAM             RT_ROBOT2_RAM           // Real time Robot2 heap RAM size
#define _RT_ROBOT3_HEAP_RAM             RT_ROBOT3_RAM           // Real time Robot3 heap RAM size
#define _RT_ROBOT4_HEAP_RAM             RT_ROBOT4_RAM           // Real time Robot4 heap RAM size
#define _RT_REVERB_HEAP_RAM             RT_REVERB_RAM           // Real time Reverb heap RAM size 
#define _RT_ECHO_HEAP_RAM               RT_ECHO_RAM             // Real time Echo heap RAM size 
#define _RT_GHOST_HEAP_RAM				RT_GHOST_RAM			// Real time Ghost heap RAM size
#define _RT_DARTH_HEAP_RAM				RT_DARTH_RAM			// Real time Darth Vader heap RAM size
#define _RT_CHORUS_HEAP_RAM				RT_CHORUS_RAM			// Real time Chorus heap RAM size
#define _WID_PLAY_RAM					WID_PLAY_RAM			// WaveID heap RAM size
#define _VR_HEAP_RAM                    _VR_RAM_SIZE            // VR heap RAM size
#define _VR_SD_HEAP_RAM                 _VR_SD_RAM_SIZE         // VR Voice Tag heap RAM size
#define	_QIO_HEAP_RAM					QIO_READ_RAM			// QIO heap RAM size
#define	_VIO_HEAP_RAM					VIO_READ_RAM			// VIO heap RAM size
#define	_AUDIO_RECORD_HEAP_RAM			AUDIO_RECORD_REC_RAM	// AUDIO RECORD heap RAM size
#define	_PD_PLAY_RAM					PD_PLAY_RAM				// Pitch detection RAM size
#define	_SL_HEAP_RAM					_SL_RAM_SIZE			// SL heap RAM size
//====================================================================================
//  Each algorithm STACK Size
//====================================================================================
#define _SBC_PLAY_STACK_RAM             388	                    // SBC Play stack RAM size
#define _SBC_RECORD_STACK_RAM           396                     // SBC Rec stack RAM size
#define _SBC_ROBOT4_STACK_RAM         	400                     // SBC Robot4 stack RAM size
#define _SBC_CH2_PLAY_STACK_RAM         388                     // SBC Play CH2 stack RAM size
#define _SBC2_PLAY_STACK_RAM            448	                    // SBC-2 Play stack RAM size			
#define _SBC2_CH2_PLAY_STACK_RAM        448                     // SBC-2 Play CH2 stack RAM size		
#define _ADPCM_PLAY_STACK_RAM           ADPCM_PLAY_STACK_SIZE   // ADPCM Play stack RAM size
#define _ADPCM_RECORD_STACK_RAM         268                     // ADPCM Rec stack RAM size
#define _ADPCM_PITCH_HEAP_STACK_RAM     ADPCM_SP_STACK_SIZE 	// ADPCM pitch/speed stack RAM size
#define _ADPCM_SPEEDPITCHN_STACK_RAM    352				 	    // ADPCM SpeedPitchN stack RAM size
#define _ADPCM_ECHO_HEAP_STACK_RAM      352					 	// ADPCM ECHO stack RAM size
#define _ADPCM_ROBOT1_STACK_RAM         272                     // ADPCM Robot1 stack RAM size
#define _ADPCM_ROBOT2_STACK_RAM         288                     // ADPCM Robot2 stack RAM size
#define _ADPCM_ROBOT3_STACK_RAM         384                     // ADPCM Robot3 stack RAM size
#define _ADPCM_ROBOT4_STACK_RAM         320                     // ADPCM Robot4 stack RAM size
#define _ADPCM_REVERB_STACK_RAM			280						// ADPCM Reverb stack RAM size
#define _ADPCM_DARTH_STACK_RAM          316         			// ADPCM Darth stack RAM size
#define _ADPCM_ANIMAL_ROAR_STACK_RAM    ADPCM_ANIMAL_ROAR_STACK_SIZE // ADPCM Animal Roar stack RAM size
#define _ADPCM_CH2_PLAY_STACK_RAM       ADPCM_CH2_PLAY_STACK_SIZE // ADPCM Play CH2 stack RAM size  
#define _ADPCM_CHX_PLAY_STACK_RAM       ADPCM_CHX_PLAY_STACK_SIZE // ADPCM Play multi channels play simultaneously, stack RAM size  
#define _CELP_PLAY_STACK_RAM            432	                    // CELP Play stack RAM size
#define _MIDI_PLAY_STACK_RAM            316                     // MIDI Play stack RAM size
#define _PCM_PLAY_STACK_RAM             228                     // PCM Play stack RAM size
#define _PCM_RECORD_STACK_RAM           240                     // PCM Rec stack RAM size
#define _PCM_ROBOT3_STACK_RAM           304                     // PCM Robot3 stack RAM size
#define _PCM_CH2_PLAY_STACK_RAM         228                     // PCM Play CH2 stack RAM size
#define _PCM_CH2_ROBOT3_STACK_RAM       304                     // PCM CH2 Robot3 stack RAM size
#define _PCM_CH3_PLAY_STACK_RAM         228                     // PCM Play CH3 stack RAM size
#define _PCM_CH3_ROBOT3_STACK_RAM       304                     // PCM CH3 Robot3 stack RAM size
#define _RT_PLAY_STACK_RAM           	420                     // Real time stack RAM size(/w Howling)
#define _RT_PITCH_STACK_RAM           	436                     // Real time Pitch stack RAM size
#define _RT_ROBOT1_STACK_RAM            436                     // Real time Robot1 stack RAM size
#define _RT_ROBOT2_STACK_RAM            436                     // Real time Robot2 stack RAM size
#define _RT_ROBOT3_STACK_RAM            436                     // Real time Robot3 stack RAM size
#define _RT_ROBOT4_STACK_RAM            436                     // Real time Robot4 stack RAM size
#define _RT_REVERB_STACK_RAM            436                     // Real time Reverb stack RAM size
#define _RT_ECHO_STACK_RAM              436                     // Real time Echo stack RAM size
#define _RT_GHOST_STACK_RAM				460						// Real time Ghost stack RAM size
#define _RT_DARTH_STACK_RAM				464						// Real time Darth Vader stack RAM size
#define _RT_CHORUS_STACK_RAM			500						// Real time Chorus stack RAM size
#define _WID_PLAY_STACK_RAM				424						// WaveID stack RAM size
#define _VR_STACK_RAM                   VR_STACK_SIZE 		    // VR STACK RAM Size
#define	_QIO_STACK_RAM					96						// QIO stack RAM size
#define	_VIO_STACK_RAM					212						// VIO stack RAM size
#define	_AUDIO_RECORD_STACK_RAM			360						// AUDIO RECORD stack RAM size
#define	_PD_STACK_RAM					256						// Pitch detection stack RAM size
#define	_SL_STACK_RAM					_SL_STACK_SIZE			// SL stack RAM size
//====================================================================================
//  RAM Setting 
//  - User decide how many RAM size use simultaneously
//  - Please key in the largest RAM size scenario at the same time
//  - Please use copy/paste to avoid key in error
//  - Example:
//             Voice Recognition(VR), SBC_PLAY, ADPCM_PLAY, CELP_PLAY are used in a project
//             (1) When VR execute, the other algorithms do not work together.
//             (2) SBC_PLAY and ADPCM_PLAY execute at the same time.
//			   (3) When CELP_PLAY execute, the other algorithms do not work together.
//             By this example, user chooses the largest RAM size of these three scenarios.
//             If scenario(1) is largest, please define as following
//
//             #define _RAM_HEAP_SIZE   _VR_HEAP_RAM    //HEAP add all algorithms which play simultaneously.
//             #define _RAM_STACK_SIZE  _VR_STACK_SIZE  //Stack only define largest one.
//====================================================================================
#define _RAM_HEAP_SIZE                 _SBC_PLAY_HEAP_RAM
#define _RAM_STACK_SIZE                _SBC_PLAY_STACK_RAM

const U16 _stack_size = _RAM_STACK_SIZE;
#if _EF_SERIES && OPTION_IRAM
const U16 _ram_size   = OPTION_DLM_SIZE - 0x800;
#else
const U16 _ram_size   = OPTION_DLM_SIZE;
#endif
U8 __attribute__((section(".heap, \"aw\", @nobits!"))) use_ram[_RAM_HEAP_SIZE+_RAM_STACK_SIZE];

#if 0//debug use
U32 MEM_GetRamSize()
{
    dprintf("RAM_HEAP %d RAM_STACK %d \r\n",_RAM_HEAP_SIZE,_RAM_STACK_SIZE);
}
#endif
#endif

