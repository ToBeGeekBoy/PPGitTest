//==========================================================================
// File Name          : nx1_config.h
// Description        : NX1 C Module Setting
// Version            : V3.80
// Last modified date : 2023/08/31
//==========================================================================
#ifndef NX1_CONFIG_H
#define NX1_CONFIG_H
#include "nx1_def.h"
#ifndef  __ASSEMBLY__
#include "nx1_lib.h"
#endif

#include "user_storage_break_point.h"
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * IC Body Setting *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
#define _IC_BODY                    ((!OPTION_IC_BODY)?OPTION_IC_BODY_EXTD:OPTION_IC_BODY)
#define _BODY_VER                   OPTION_IC_VER       // Set by NYIDE
#define _EF_SERIES					((((OPTION_IC_BODY>>8)&0xF)==0xF)||(((OPTION_IC_BODY_EXTD>>20)&0xF)==0xF))
#define _C_VER						380					// C Module Version
#define _MODULE_VER                 23083100            // C Module Version



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * System Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  System Parameter Setting
//====================================================================================
#define _SYS_CLK                    OPTION_HIGH_FREQ_CLK// Set by NYIDE
#define _SYS_TIMEBASE               _NYIDE_TIMEBASE     // Set time base
#define _SYS_WDT_TIME               WDT_TIMEOUT_750ms   // Set watch dog time out
#define _SYS_SMU_CPU_CLK_DIV        CPU_CLK_DIV_1       // Set CPU divider
#define _SYS_SMU_LVD_VOLTAGE        LVD_VOLTAGE_2V2     // Set LVD voltage
#define _SYS_LVD_MONITOR_MODULE		DISABLE				// set LVD monitor module



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * AUDIO Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  AUDIO Parameter Setting
//====================================================================================
#define _AUDIO_MODULE               ENABLE              // Enable or Disable AUDIO Module
#define _DAC_CH0_TIMER              USE_TIMER0          // Set HW channel 0 timer
#define _DAC_CH1_TIMER              USE_TIMER1          // Set HW channel 1 timer
#define _DAC_UP_SAMPLE_POINT		16					// Set DAC Up Sample Point for NX1 EF series
#define _ADC_TIMER                  USE_TIMER2          // Set ADC timer
#define _PP_GAIN                    PP_GAIN_83          // Set default DAC push pull gain
#define _ROBOT_SOUND_COMPRESS       DISABLE             // Reduce Robot sound RAM size

//--------------------------------------------------------------------------------
// 使用自带的睡眠处理，为了解决睡眠时DAC不能正常Ramp_Down和唤醒时不能正常Ramp_Up问题
//--------------------------------------------------------------------------------
#define _AUDIO_IDLE_INTO_HALT		ENABLE				// Audio status = pause/stop, into Halt mode

#define _AUDIO_MULTI_CHANNEL        DISABLE             // If multi channels is played at the same HW channel, this setting need enable.
#define _AUDIO_SENTENCE				ENABLE              // Enable or Disable SPI_MODE Audio SentenceOrder Play
#define _AUDIO_VOLUME_CHANGE_SMOOTH ENABLE              // Enable or Disable volume change smooth
#define _AUDIO_VOLUME_SMOOTH_TIME   50                  // Volume change velocity between max to min or min to max. Unit is ms
#define _AUDIO_RECORD				DISABLE				// ENABLE or DISABLE audio record function (ADPCM Type)
#define _AUDIO_RECORD_AUDIO_CH      HW_CH0              // Select HW channel 0 or 1 ,please follow HW channel of audio play



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * DAC Mode *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  DAC MODE Parameter Setting
//====================================================================================
#define	_DAC_MODE					DISABLE				// ENABLE: DAC_MODE(only output to PP1) , DISABLE: PP_MODE
#define _AUDIO_RAMP_UP_TIME			8					// Select 1~n, default:8, unit:ms
#define _AUDIO_RAMP_DOWN_TIME		4					// Select 1~n, default:4, unit:ms
#define _AUDIO_AMP_SD_CTL			DISABLE				// ENABLE or DISABLE control shut down pin of AMP.
#define _AUDIO_AMP_SD_CTL_PORT		GPIOA				// Select PORT GPIOA/GPIOB/GPIOC(only NX1EV)
#define _AUDIO_AMP_SD_CTL_PIN		8					// Select PIN 0~15 for GPIOA & GPIOB, Select 0~7 for GPIOC(only NX1EV)
#define _AUDIO_AMP_SD_CTL_ACTIVE    ACTIVE_LOW			// Select shut down pin of AMP is ACTIVE_LOW or ACTIVE_HIGH
#define _AUDIO_AMP_WAKUP_TIME		30					// Select 1~n, default:30,Unit:ms, default value is follow Nyquest NY9A001A(@VDD=5.0V,SE mode,CB=0.1uF).
#define _AUDIO_AMP_SHUTDOWN_TIME	10					// Select 1~n, default:10,Unit:ms, default value is follow Nyquest NY9A001A(@VDD=5.0V,SE mode,CB=0.1uF).

//----------------------------------------------------------------------------//
// _PP_IDLE_INTO_HALT add by pp
//----------------------------------------------------------------------------//
#define _PP_IDLE_INTO_HALT_IC_BODY	((OPTION_IC_BODY == 0x11A22) || (OPTION_IC_BODY == 0x12A44))  // IC bodys of PP_IDL_INTO_HALT
#define _PP_IDLE_INTO_HALT			((DISABLE == _DAC_MODE) && _PP_IDLE_INTO_HALT_IC_BODY)		  // push-pull setting for Halt mode
#define _PP_GPIO_EN					ENABLE				                        				  // into Halt mode for GPIO toggle disable
#define _PP_GPIO_PORT   			GPIOA				                        				  // GPIOA~C
#define _PP_GPIO_PIN      			15				                            				  // 0~15

//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * SBC Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  SBC Parameter Setting
//====================================================================================
#define _SBC_PLAY                   ENABLE              // Enable or Disable playback
#define _SBC_SPEED_CHANGE           DISABLE             // Enable or Disable speed function
#define _SBC_PITCH_CHANGE           DISABLE             // Enable or Disable pitch function
#define _SBC_ROBOT1_SOUND           DISABLE             // Enable or Disable robot1 function
#define _SBC_ROBOT2_SOUND         	DISABLE             // Enable or Disable robot2 function
#define _SBC_ROBOT3_SOUND         	DISABLE             // Enable or Disable robot3 function
#define _SBC_REVERB_SOUND           DISABLE             // Enable or Disable reverb function
#define _SBC_ECHO_SOUND             DISABLE             // Enable or Disable echo function
#define	_SBC_QIO_MODULE				DISABLE				// Enable or Disable QIO function
#define _SBC_PLAY_AUDIO_CH          HW_CH0              // Select HW channel 0 or 1
#define _SBC_RECORD                 DISABLE             // Enable or Disable recorder
#define _SBC_RECORD_ERASING         DISABLE             // Enable or Disable real-time erasing function during Recording
#define _SBC_SECTOR_ERASE_MAXTIME	700					// Set SPI Flash sector erase time max spec, unit:msec. User must ensure the quality of SPI-Flash to execute real-time erasing function.
#define _SBC_PLAY_BACK_EQ			DISABLE				// Enable or Disable Equalizer when use play_back
#define _SBC_RECORD_PLAY_EQ		    DISABLE				// Enable or Disable Equalizer when use record_play
#define _SBC_RECORD_PLAY_TYPE		TYPE1				// Select DISABLE or TYPE1:DRC mode, or TYPE2:Digital gain control mode
#define _SBC_EQ_TYPE			    TYPE0               // Select TYPE0/TYPE1/CUSTOMIZE, only for recorded voice
#define _SBC_AUDIO_BUF_X3			DISABLE				// Enable or Disable SBC Audio has 3 Buffers
#define _SBC_AUTO_SAVE_TIME         DISABLE             // Enable or Disable saving the location of SBC playback. Data save in SPI Flash
#define _SBC_AUTO_SAVE_PERIOD       1000                // Set the period of auto save, unit:msec. This value must be larger than _SBC_SECTOR_ERASE_MAXTIME
#define _SBC_AUTO_SAVE_SR           16000               // Set SBC sample rate when auto save is running
//====================================================================================
//  Second SBC Parameter Setting
//====================================================================================
#define _SBC_CH2_PLAY               DISABLE             // Enable or Disable playback
#define _SBC_CH2_SPEED_CHANGE       DISABLE             // Enable or Disable speed function
#define _SBC_CH2_PITCH_CHANGE       DISABLE             // Enable or Disable pitch function
#define _SBC_CH2_ROBOT1_SOUND       DISABLE             // Enable or Disable robot1 function
#define _SBC_CH2_ROBOT2_SOUND     	DISABLE             // Enable or Disable robot2 function
#define _SBC_CH2_ROBOT3_SOUND     	DISABLE             // Enable or Disable robot3 function
#define _SBC_CH2_REVERB_SOUND       DISABLE             // Enable or Disable reverb function
#define _SBC_CH2_ECHO_SOUND         DISABLE             // Enable or Disable echo function
#define	_SBC_CH2_QIO_MODULE			DISABLE				// Enable or Disable QIO function
#define _SBC_CH2_PLAY_AUDIO_CH      HW_CH1              // Select HW channel 0 or 1
#define _SBC_CH2_PLAY_BACK_EQ		DISABLE				// Enable or Disable Equalizer when use play_back



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * SBC-2 Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  SBC-2 Parameter Setting
//====================================================================================
#define _SBC2_PLAY					DISABLE				// Enable or Disable playback
#define	_SBC2_QIO_MODULE			DISABLE				// Enable or Disable QIO function
#define _SBC2_PLAY_AUDIO_CH			HW_CH0				// Select HW channel 0 or 1
#define _SBC2_PLAY_BACK_EQ			DISABLE				// Enable or Disable Equalizer when use play_back
#define _SBC2_AUDIO_BUF_X3			DISABLE				// Enable or Disable SBC-2 Audio has 3 Buffers
//====================================================================================
//  Second SBC-2 Parameter Setting
//====================================================================================
#define _SBC2_CH2_PLAY				DISABLE				// Enable or Disable playback
#define	_SBC2_CH2_QIO_MODULE		DISABLE				// Enable or Disable QIO function
#define _SBC2_CH2_PLAY_AUDIO_CH		HW_CH1				// Select HW channel 0 or 1
#define _SBC2_CH2_PLAY_BACK_EQ		DISABLE				// Enable or Disable Equalizer when use play_back



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * ADPCM Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  ADPCM CH1 Decode flow select
//====================================================================================
#define _ADPCM_DECODE_FLOW_TYPE		TYPE0				// TYPE0: Sample base of decode flow, TYPE1: Frame base of decode flow,  Default: TYPE0
//====================================================================================
//  ADPCM Parameter Setting
//====================================================================================
#define _ADPCM_PLAY                 DISABLE             // Enable or Disable playback
#define _ADPCM_SPEED_CHANGE         DISABLE             // Enable or Disable speed function
#define _ADPCM_PITCH_CHANGE         DISABLE             // Enable or Disable pitch function
#define _ADPCM_SPEEDPITCH_HQ        DISABLE  		    // Enable or Disable SpeedPitch effect high sound quality
#define _ADPCM_SPEEDPITCH_N     	DISABLE             // Enable or Disable speed pitch function(no change sample rate)
#define _ADPCM_ROBOT1_SOUND         DISABLE             // Enable or Disable robot1 function
#define _ADPCM_ROBOT2_SOUND       	DISABLE             // Enable or Disable robot2 function
#define _ADPCM_ROBOT3_SOUND       	DISABLE             // Enable or Disable robot3 function
#define _ADPCM_ROBOT4_SOUND			DISABLE				// Enable or Disable robot4 function
#define _ADPCM_REVERB_SOUND         DISABLE             // Enable or Disable reverb function
#define _ADPCM_ECHO_SOUND           DISABLE             // Enable or Disable echo function
#define _ADPCM_ECHO_EFFECT			DELAY_320ms			// Select Total Delay time DELAY_320ms/DELAY_800ms/DELAY_1200ms/DELAY_USER_DEFINE
#define _ADPCM_ECHO_MAX_SAMPLE_RATE	SAMPLE_RATE_8000	// Set max sample rate (support 8000/10000/12000/16000Hz) for ECHO estimated resources
#define _ADPCM_ECHO_QUALITY			DISABLE				// Enable or Disable echo Advance QUALITY, default: Enable
#define _ADPCM_ECHO_UD_DELAY_TIME	DISABLE				// Set user define ECHO one delay time, select 50~1200ms,total delay time=_ADPCM_ECHO_UD_DELAY_TIME*_ADPCM_ECHO_UD_REPEAT_TIME, default: disable
#define _ADPCM_ECHO_UD_REPEAT_TIME	DISABLE				// Set user define ECHO repear time, support 1~6, default: disable
#define _ADPCM_DARTH_SOUND          DISABLE             // Enable or Disable darth vader function
#define _ADPCM_ANIMAL_ROAR			DISABLE             // Enable or Disable animal roar function
#define	_ADPCM_QIO_MODULE			DISABLE				// Enable or Disable QIO function
#define _ADPCM_PLAY_AUDIO_CH        HW_CH1              // Select HW channel 0 or 1
#define _ADPCM_PLAY_REPEAT			DISABLE			 	// Enable or Disable repeat function for playback or speed/pitch/robot3
#define _ADPCM_PLAY_SR_FOR_SBC_REC	DISABLE             // Set Sample Rate of playback for _SBC_RECORD_ERASING, MAX setting value is 24000Hz
#define _ADPCM_SW_UPSAMPLE			DISABLE             // Enable or Disable ADPCM upsample function
#define _ADPCM_RECORD               DISABLE             // Enable or Disable recorder
#define _ADPCM_RECORD_SAMPLE_RATE   SAMPLE_RATE_8000	// Set sample rate (support 8000/10000/12000/16000Hz)
#define _ADPCM_RECORD_ERASING       DISABLE             // Enable or Disable real-time erasing function during Recording
#define _ADPCM_SECTOR_ERASE_MAXTIME	700					// Set SPI Flash sector erase time max spec, unit:msec. User must ensure the quality of SPI-Flash to execute real-time erasing function.
#define _ADPCM_RECORD_SOUND_TRIG    DISABLE             // Enable or Disable sound trig record function
#define _ADPCM_PLAY_BACK_EQ			DISABLE				// Enable or Disable Equalizer when use play_back
#define _ADPCM_RECORD_PLAY_EQ		DISABLE				// Enable or Disable Equalizer when use record_play
#define _ADPCM_RECORD_PLAY_TYPE		DISABLE				// Select DISABLE or TYPE1:DRC mode, TYPE2:Digital gain control mode
#define _ADPCM_EQ_TYPE			    TYPE0            	// Select TYPE0/TYPE1/CUSTOMIZE, only for recorded voice



//====================================================================================
//  ADPCM CH2~8 Decode flow select
//====================================================================================
#define	_ADPCM_CHX_DECODE_FLOW_TYPE TYPE0				// TYPE0: Sample base of decode flow, TYPE1: Frame base of decode flow,  Default: TYPE0
//====================================================================================
//  Second ADPCM Parameter Setting
//====================================================================================
#define _ADPCM_CH2_PLAY             DISABLE             // Enable or Disable playback
#define	_ADPCM_CH2_QIO_MODULE		DISABLE				// Enable or Disable QIO function
#define _ADPCM_CH2_PLAY_AUDIO_CH    HW_CH0              // Select HW channel 0 or 1
#define _ADPCM_CH2_PLAY_BACK_EQ		DISABLE				// Enable or Disable Equalizer when use play_back
//====================================================================================
//  Third ADPCM Parameter Setting
//====================================================================================
#define _ADPCM_CH3_PLAY             DISABLE             // Enable or Disable playback
#define	_ADPCM_CH3_QIO_MODULE		DISABLE				// Enable or Disable QIO function
#define _ADPCM_CH3_PLAY_AUDIO_CH    HW_CH0              // Select HW channel 0 or 1
#define _ADPCM_CH3_PLAY_BACK_EQ		DISABLE				// Enable or Disable Equalizer when use play_back
//====================================================================================
//  Fourth ADPCM Parameter Setting
//====================================================================================
#define _ADPCM_CH4_PLAY             DISABLE             // Enable or Disable playback
#define	_ADPCM_CH4_QIO_MODULE		DISABLE				// Enable or Disable QIO function
#define _ADPCM_CH4_PLAY_AUDIO_CH    HW_CH0              // Select HW channel 0 or 1
#define _ADPCM_CH4_PLAY_BACK_EQ		DISABLE				// Enable or Disable Equalizer when use play_back
//====================================================================================
//  Fifth ADPCM Parameter Setting
//====================================================================================
#define _ADPCM_CH5_PLAY             DISABLE             // Enable or Disable playback
#define	_ADPCM_CH5_QIO_MODULE		DISABLE				// Enable or Disable QIO function
#define _ADPCM_CH5_PLAY_AUDIO_CH    HW_CH0              // Select HW channel 0 or 1
#define _ADPCM_CH5_PLAY_BACK_EQ		DISABLE				// Enable or Disable Equalizer when use play_back
//====================================================================================
//  Sixth ADPCM Parameter Setting
//====================================================================================
#define _ADPCM_CH6_PLAY             DISABLE             // Enable or Disable playback
#define	_ADPCM_CH6_QIO_MODULE		DISABLE				// Enable or Disable QIO function
#define _ADPCM_CH6_PLAY_AUDIO_CH    HW_CH0              // Select HW channel 0 or 1
#define _ADPCM_CH6_PLAY_BACK_EQ		DISABLE				// Enable or Disable Equalizer when use play_back
//====================================================================================
//  Seventh ADPCM Parameter Setting
//====================================================================================
#define _ADPCM_CH7_PLAY             DISABLE             // Enable or Disable playback
#define	_ADPCM_CH7_QIO_MODULE		DISABLE				// Enable or Disable QIO function
#define _ADPCM_CH7_PLAY_AUDIO_CH    HW_CH0              // Select HW channel 0 or 1
#define _ADPCM_CH7_PLAY_BACK_EQ		DISABLE				// Enable or Disable Equalizer when use play_back
//====================================================================================
//  Eighth ADPCM Parameter Setting
//====================================================================================
#define _ADPCM_CH8_PLAY             DISABLE             // Enable or Disable playback
#define	_ADPCM_CH8_QIO_MODULE		DISABLE				// Enable or Disable QIO function
#define _ADPCM_CH8_PLAY_AUDIO_CH    HW_CH0              // Select HW channel 0 or 1
#define _ADPCM_CH8_PLAY_BACK_EQ		DISABLE				// Enable or Disable Equalizer when use play_back



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * CELP Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  CELP Parameter Setting
//====================================================================================
#define _CELP_PLAY                  DISABLE             // Enable or Disable playback
#define _CELP_SPEED_CHANGE          DISABLE             // Enable or Disable speed function
#define _CELP_PITCH_CHANGE          DISABLE             // Enable or Disable pitch function
#define _CELP_ROBOT1_SOUND          DISABLE             // Enable or Disable robot1 function
#define _CELP_ROBOT2_SOUND        	DISABLE             // Enable or Disable robot2 function
#define _CELP_ROBOT3_SOUND        	DISABLE             // Enable or Disable robot3 function
#define _CELP_REVERB_SOUND          DISABLE             // Enable or Disable reverb function
#define _CELP_ECHO_SOUND            DISABLE             // Enable or Disable echo function
#define _CELP_PLAY_AUDIO_CH         HW_CH0              // Select HW channel 0 or 1



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * PCM Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  PCM Parameter Setting
//====================================================================================
#define _PCM_PLAY                   DISABLE             // Enable or Disable playback
#define _PCM_SPEED_CHANGE           DISABLE             // Enable or Disable speed function
#define _PCM_PITCH_CHANGE           DISABLE             // Enable or Disable pitch function
#define _PCM_ROBOT1_SOUND           DISABLE             // Enable or Disable robot1 function
#define _PCM_ROBOT2_SOUND         	DISABLE             // Enable or Disable robot2 function
#define _PCM_ROBOT3_SOUND         	DISABLE             // Enable or Disable robot3 function
#define _PCM_REVERB_SOUND           DISABLE             // Enable or Disable reverb function
#define _PCM_ECHO_SOUND             DISABLE             // Enable or Disable echo function
#define _PCM_PLAY_AUDIO_CH          HW_CH0              // Select HW channel 0 or 1
#define _PCM_RECORD                 DISABLE             // Enable or Disable recorder
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  Second PCM Parameter Setting
//====================================================================================
#define _PCM_CH2_PLAY               DISABLE             // Enable or Disable playback
#define _PCM_CH2_SPEED_CHANGE       DISABLE             // Enable or Disable speed function
#define _PCM_CH2_PITCH_CHANGE       DISABLE             // Enable or Disable pitch function
#define _PCM_CH2_ROBOT1_SOUND       DISABLE             // Enable or Disable robot1 function
#define _PCM_CH2_ROBOT2_SOUND     	DISABLE             // Enable or Disable robot2 function
#define _PCM_CH2_ROBOT3_SOUND     	DISABLE             // Enable or Disable robot3 function
#define _PCM_CH2_REVERB_SOUND       DISABLE             // Enable or Disable reverb function
#define _PCM_CH2_ECHO_SOUND         DISABLE             // Enable or Disable echo function
#define _PCM_CH2_PLAY_AUDIO_CH      HW_CH0              // Select HW channel 0 or 1
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  Third PCM Parameter Setting
//====================================================================================
#define _PCM_CH3_PLAY               DISABLE             // Enable or Disable playback
#define _PCM_CH3_SPEED_CHANGE       DISABLE             // Enable or Disable speed function
#define _PCM_CH3_PITCH_CHANGE       DISABLE             // Enable or Disable pitch function
#define _PCM_CH3_ROBOT1_SOUND       DISABLE             // Enable or Disable robot1 function
#define _PCM_CH3_ROBOT2_SOUND     	DISABLE             // Enable or Disable robot2 function
#define _PCM_CH3_ROBOT3_SOUND     	DISABLE             // Enable or Disable robot3 function
#define _PCM_CH3_REVERB_SOUND       DISABLE             // Enable or Disable reverb function
#define _PCM_CH3_ECHO_SOUND         DISABLE             // Enable or Disable echo function
#define _PCM_CH3_PLAY_AUDIO_CH      HW_CH0              // Select HW channel 0 or 1



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * Real Time Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  Real time Parameter Setting
//====================================================================================
#define _RT_PLAY                    DISABLE             // Enable or Disable real time play
#define _RT_PITCH_CHANGE            DISABLE             // Enable or Disable real time play (with pitch change function)
#define _RT_ECHO_SOUND              DISABLE             // Enable or Disable echo function
#define _RT_ECHO_EFFECT				DELAY_320ms			// Select DELAY_320ms/DELAY_800ms/DELAY_1200ms/DELAY_USER_DEFINE
#define _RT_ECHO_QUALITY			ENABLE				// Enable or Disable echo Advance QUALITY, default: Enable
#define _RT_ECHO_UD_DELAY_TIME		DISABLE				// Set user define ECHO delay time, support 50~1200ms,total delay time=_RT_ECHO_UD_DELAY_TIME*_RT_ECHO_UD_REPEAT_TIME, default: disable
#define _RT_ECHO_UD_REPEAT_TIME		DISABLE				// Set user define ECHO repear time, support 1~6, default: disable
#define _RT_ROBOT1_SOUND            DISABLE             // Enable or Disable robot1 function
#define _RT_ROBOT2_SOUND          	DISABLE             // Enable or Disable robot2 function
#define _RT_ROBOT3_SOUND          	DISABLE             // Enable or Disable robot3 function
#define _RT_ROBOT4_SOUND          	DISABLE             // Enable or Disable robot4 function
#define _RT_REVERB_SOUND            DISABLE             // Enable or Disable reverb function
#define _RT_GHOST_SOUND				DISABLE				// Enable or Disable ghost function
#define _RT_DARTH_SOUND				DISABLE				// Enable or Disable darth vader function
#define _RT_CHORUS_SOUND			DISABLE				// Enable or Disable chorus function
#define _RT_CHORUS_SOUND_DELAY1     DELAY_30ms			// Select DELAY_10ms / DELAY_20ms / DELAY_30ms
#define _RT_CHORUS_SOUND_DELAY2     DELAY_30ms			// Select DELAY_10ms / DELAY_20ms / DELAY_30ms
#define _RT_SAMPLE_RATE				SAMPLE_RATE_8000	// Set sample rate (support 8000/12000/16000Hz)
#define _RT_PLAY_AUDIO_CH           HW_CH1              // Select HW channel 0 or 1
#define _RT_DIGITAL_GAIN_CTRL	    DISABLE            	// Enable or Disable Digital gain control function
#define _RT_HOWLING_SUPPRESS		ENABLE            	// Enable or Disable Howling Suppress function, default: ENABLE



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * MIDI Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  MIDI Parameter Setting
//====================================================================================
#define _MIDI_PLAY                  DISABLE             // Enable or Disable playback
#define _MIDI_CH_NUM                QMIDI_CH_NUM        // Select the number of MIDI channels
#define _MIDI_TIMBRE_FORMAT         QMIDI_TIMBRE_FORMAT // Set by Q-MIDI
#define _MIDI_SAMPLE_RATE           QMIDI_SAMPLE_RATE   // Set sample rate
#define _MIDI_PLAY_AUDIO_CH         HW_CH0              // Select HW channel 0 or 1
#define _MIDI_NOTEON_STATE			DISABLE				// NoteOn Section
#define _MIDI_INST_NOTEON			DISABLE				// Instrument function
#define _MIDI_INST_NOTEON_TYPE		TYPE0				// Select TYPE0/TYPE1(Kick out the same pitch)
#define _MIDI_INST_NOTEON_MAXCH  	QMIDI_CH_NUM		// Instrument NoteOn max. channel
#define _MIDI_INST_NOTEON_GLISS		DISABLE				// Instrument NoteOn glissando function
#define _MIDI_INST_RECORD			DISABLE				// Instrument record function



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * WaveID Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  WaveID Parameter Setting
//====================================================================================
#define _WAVE_ID_MODULE             DISABLE         	// Enable or Disable WaveID Module
#define _WAVE_ID_AUDIO_CH           HW_CH0				// Select HW channel 0 or 1



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * Pitch detection Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  Pitch detection Parameter Setting
//====================================================================================
#define _PD_PLAY                    DISABLE          	// Enable or Disable Pitch detection function
#define _PD_TYPE					TYPE0				// TYPE0: Detect human voice;  TYPE1: Detect pure tone
#define _PD_TYPE1_DETECT_FREQ		TYPE2				// For _PD_TYPE=TYPE1, select detect frequency range. TYPE0: 31~2000, TYPE1: 1000~3000, TYPE2: 2000~4000



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * Sound detection Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  Sound detection Parameter Setting
//====================================================================================
#define _SOUND_DETECTION            DISABLE          	// Enable or Disable Sound detection function



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * VR Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  Voice Recognition Parameter Setting
//====================================================================================
#define _VR_FUNC                    DISABLE				// Enable or Disable Voice Recognition
#define _VR_TIMEOUT					5000				// Set Trigger Timeout, Unit is ms, or Set DISABLE
#define _VR_HIT_SCORE				DISABLE				// Enable or Disable the score of voice Command
#define _VR_VAD                 	DISABLE             // Enable or Disable Voice Activity Detection(VAD)
#define _VR_UNKNOWN_COMMAND			DISABLE				// Enable or Disable Unknown Command Detection, _VR_VAD should be set ENABLE
#define _VR_VAD_TIMEOUT_EXTENSION	DISABLE				// Enable or Disable Voice Activity Detection(VAD) for VR Timeout Extension
#define _VR_VOICE_TAG				DISABLE				// Enable or Disable Voice Tag function
#define _VR_VOICE_TAG_RSRC_INDEX	0					// Set the index number of Voice Tag Resource file
#define _VR_VOICE_PASSWORD			DISABLE				// Enable or Disable Voice Password function
#define _VR_VOICE_PASSWORD_INDEX	1					// Set the index number of Voice Password Resource file
#define _VR_GET_LOADING				DISABLE				// Enable or Disable to get the loading of Voice Recognition



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * QFID Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  QFID Parameter Setting
//====================================================================================
#define _QFID_MODULE				DISABLE				// Enable or Disable QFID Module
#define _QFID_TIMER					USE_TIMER1			// Set QFID timer
#define _QFID_EN_PORT				GPIOA				// Set QFID Enable port     (GPIOA/B/C)
#define _QFID_EN_PIN				1					// Set QFID Enable pin
#define _QFID_RX_PORT				GPIOA				// Set QFID RX port         (GPIOA/B/C)
#define _QFID_RX_PIN				7					// Set QFID RX pin
#define _QFID_TX_IO_TYPE			QFID_TYPE_IR		// Set QFID TX pin type     (QFID_TYPE_IR / QFID_TYPE_PWM)
#define _QFID_IR_TX_OUTPUT			USE_IR0_TX			// Select output(USE_IR0_TX/USE_IR1_TX/USE_IR2_TX/USE_IR3_TX)
#define _QFID_CARRIER				QFID_FREQ_125K		// Set Carrier freq. 125K or 530KHz
#define _QFID_VID_PID				DISABLE				// Enable or Disable VID/PID function
#define _QFID_VID					0x004				// Set Vender ID
#define _QFID_PID					0x234				// Set Project ID

#define _QFID_GROUP_NUM				1					// Set Group number 		(1~8)

#define _QFID_GRP0_RESPONSE         QFID_MODE_ID        // Set Group0 response type (QFID_MODE_ID / QFID_MODE_ID_INPUT)
#define _QFID_GRP1_RESPONSE         QFID_MODE_ID        // Set Group1 response type (QFID_MODE_ID / QFID_MODE_ID_INPUT)
#define _QFID_GRP2_RESPONSE         QFID_MODE_ID        // Set Group2 response type (QFID_MODE_ID / QFID_MODE_ID_INPUT)
#define _QFID_GRP3_RESPONSE         QFID_MODE_ID       	// Set Group3 response type (QFID_MODE_ID / QFID_MODE_ID_INPUT)

#define _QFID_GRP0_TAG_NUM          16                  // Set Group0 Tag number    (QFID_MODE_ID: 1~16 / QFID_MODE_ID_INPUT: 1~8, depend on _QFID_GRP0_RESPONSE)
#define _QFID_GRP1_TAG_NUM          16                  // Set Group1 Tag number    (QFID_MODE_ID: 1~16 / QFID_MODE_ID_INPUT: 1~8, depend on _QFID_GRP1_RESPONSE)
#define _QFID_GRP2_TAG_NUM          16                  // Set Group2 Tag number    (QFID_MODE_ID: 1~16 / QFID_MODE_ID_INPUT: 1~8, depend on _QFID_GRP2_RESPONSE)
#define _QFID_GRP3_TAG_NUM          16                  // Set Group3 Tag number    (QFID_MODE_ID: 1~16 / QFID_MODE_ID_INPUT: 1~8, depend on _QFID_GRP3_RESPONSE)
#define _QFID_GRP4_TAG_NUM          16                  // Set Group4 Tag number    (1~16)
#define _QFID_GRP5_TAG_NUM          16                  // Set Group5 Tag number    (1~16)
#define _QFID_GRP6_TAG_NUM          16                  // Set Group6 Tag number    (1~16)
#define _QFID_GRP7_TAG_NUM          16                  // Set Group7 Tag number    (1~16)

#define _QFID_DETECT_DEBOUNCE		1                   // Set Debounce 1~50 frames
#define _QFID_LEAVE_DEBOUNCE		1                   // Set Debounce 1~50 frames



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * DirectKey Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  DirectKey Parameter Setting
//====================================================================================
#define _DIRECTKEY_MODULE           DISABLE              // Enable or Disable DirectKey
#define _DIRECTKEY_PRESS_DB         PRESS_TIME_10ms     // Key press time
#define _DIRECTKEY_SCAN_TICK        SCAN_TICK_4ms       // Scan tick time
#define _DIRECTKEY_PULL_UP			DISABLE				// Enable or Disable internal PU
#define _DIRECTKEY_PA0				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PA1				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PA2				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PA3				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PA4				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PA5				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PA6				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PA7				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PA8				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PA9				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PA10				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PA11				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PA12				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PA13				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PA14				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PA15				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PB0				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PB1				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PB2				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PB3				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PB4				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PB5				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PB6				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PB7				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PB8				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PB9				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PB10				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PB11				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PB12				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PB13				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PB14				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PB15				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PC0				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PC1				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PC2				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PC3				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PC4				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PC5				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PC6				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PC7				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PD0				DISABLE				// Enable or Disable key
#define _DIRECTKEY_PD1				DISABLE				// Enable or Disable key



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * TouchKey Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  TouchKey Parameter Setting
//====================================================================================
#define _TOUCHKEY_MODULE			DISABLE				//Enable or Disable Touchkey
#define _TOUCHKEY_INTO_STANDBY		ENABLE				//Enable Touch into Standby Mode

#define C_TouchMaxPAD				TOUCH_MAX_8_PAD		//Set Touch Normal Max Pad number(1~16)
#define	C_TouchWakeUpMaxPAD			TOUCH_MAX_8_PAD		//Set Touch WakeUp Max Pad number(1~16)
#define C_TouchCALTime				TOUCH_CAL_2_SEC		//Set Touch Calibration Cycle Time when Normal Scan. Unit:1 Sec
#define C_TouchSleepCALTime			TOUCH_CAL_16_SEC	//Set Touch Calibration Cycle Time when Sleep Scan. Unit:1 Sec

#define C_TouchNormalFrame			TOUCH_NORMAL_8_MS	//Set Touch Normal Scan Cycle Time.(Default 8 ms) Unit:1 ms
#define C_TouchSleepFrame			TOUCH_SLEEP_50_MS	//Set Touch Sleep Scan Cycle Time. (Default 50 ms) Unit:1 ms

#define C_TouchEnforceTime			TOUCH_ENFORCE_40_SEC//Set Touch Enforce Calibration Time.(Default 40s) Unit:1 Sec
#define C_TouchWakeTime				TOUCH_WAKE_8_SEC	//Set Weak Time Count from Touch Release.(8~12) Unit:1 Sec

#define C_TouchDebounce				2					//Set Debounce frame.(0~7)

#define	C_PAD0_IN					TOUCH_PAD_IN_PA8	//Set PAD0 Input Touch pin use PortA.(0~15)
#define	C_PAD0_Out					TOUCH_PAD_OUT_PA9	//Set PAD0 Output Touch pin use PortA.(0~15)
#define	C_PAD1_IN					TOUCH_PAD_IN_PA10	//Set PAD1 Input Touch pin use PortA.(0~15)
#define	C_PAD1_Out					TOUCH_PAD_OUT_PA11	//Set PAD1 Output Touch pin use PortA.(0~15)
#define	C_PAD2_IN					TOUCH_PAD_IN_PA12	//Set PAD2 Input Touch pin use PortA.(0~15)
#define	C_PAD2_Out					TOUCH_PAD_OUT_PA13	//Set PAD2 Output Touch pin use PortA.(0~15)
#define	C_PAD3_IN					TOUCH_PAD_IN_PA14	//Set PAD3 Input Touch pin use PortA.(0~15)
#define	C_PAD3_Out					TOUCH_PAD_OUT_PA15	//Set PAD3 Output Touch pin use PortA.(0~15)
#define	C_PAD4_IN					TOUCH_PAD_IN_PA9	//Set PAD4 Input Touch pin use PortA.(0~15)
#define	C_PAD4_Out					TOUCH_PAD_OUT_PA8	//Set PAD4 Output Touch pin use PortA.(0~15)
#define	C_PAD5_IN					TOUCH_PAD_IN_PA11	//Set PAD5 Input Touch pin use PortA.(0~15)
#define	C_PAD5_Out					TOUCH_PAD_OUT_PA10	//Set PAD5 Output Touch pin use PortA.(0~15)
#define	C_PAD6_IN					TOUCH_PAD_IN_PA13	//Set PAD6 Input Touch pin use PortA.(0~15)
#define	C_PAD6_Out					TOUCH_PAD_OUT_PA12	//Set PAD6 Output Touch pin use PortA.(0~15)
#define	C_PAD7_IN					TOUCH_PAD_IN_PA15	//Set PAD7 Input Touch pin use PortA.(0~15)
#define	C_PAD7_Out					TOUCH_PAD_OUT_PA14	//Set PAD7 Output Touch pin use PortA.(0~15)
#define	C_PAD8_IN					TOUCH_PAD_IN_PA0	//Set PAD8 Input Touch pin use PortA.(0~15)
#define	C_PAD8_Out					TOUCH_PAD_OUT_PA1	//Set PAD8 Output Touch pin use PortA.(0~15)
#define	C_PAD9_IN					TOUCH_PAD_IN_PA2	//Set PAD9 Input Touch pin use PortA.(0~15)
#define	C_PAD9_Out					TOUCH_PAD_OUT_PA3	//Set PAD9 Output Touch pin use PortA.(0~15)
#define	C_PAD10_IN					TOUCH_PAD_IN_PA4	//Set PAD10 Input Touch pin use PortA.(0~15)
#define	C_PAD10_Out					TOUCH_PAD_OUT_PA5	//Set PAD10 Output Touch pin use PortA.(0~15)
#define	C_PAD11_IN					TOUCH_PAD_IN_PA6	//Set PAD11 Input Touch pin use PortA.(0~15)
#define	C_PAD11_Out					TOUCH_PAD_OUT_PA7	//Set PAD11 Output Touch pin use PortA.(0~15)
#define	C_PAD12_IN					TOUCH_PAD_IN_PA1	//Set PAD12 Input Touch pin use PortA.(0~15)
#define	C_PAD12_Out					TOUCH_PAD_OUT_PA0	//Set PAD12 Output Touch pin use PortA.(0~15)
#define	C_PAD13_IN					TOUCH_PAD_IN_PA3	//Set PAD13 Input Touch pin use PortA.(0~15)
#define	C_PAD13_Out					TOUCH_PAD_OUT_PA2	//Set PAD13 Output Touch pin use PortA.(0~15)
#define	C_PAD14_IN					TOUCH_PAD_IN_PA5	//Set PAD14 Input Touch pin use PortA.(0~15)
#define	C_PAD14_Out					TOUCH_PAD_OUT_PA4	//Set PAD14 Output Touch pin use PortA.(0~15)
#define	C_PAD15_IN					TOUCH_PAD_IN_PA7	//Set PAD15 Input Touch pin use PortA.(0~15)
#define	C_PAD15_Out					TOUCH_PAD_OUT_PA6	//Set PAD15 Output Touch pin use PortA.(0~15)
#define	C_PAD0_DiffCnt				600					//Set PAD0 Difference Count.
#define	C_PAD1_DiffCnt				600					//Set PAD1 Difference Count.
#define	C_PAD2_DiffCnt				600					//Set PAD2 Difference Count.
#define	C_PAD3_DiffCnt				600					//Set PAD3 Difference Count.
#define	C_PAD4_DiffCnt				600					//Set PAD4 Difference Count.
#define	C_PAD5_DiffCnt				600					//Set PAD5 Difference Count.
#define	C_PAD6_DiffCnt				600					//Set PAD6 Difference Count.
#define	C_PAD7_DiffCnt				600					//Set PAD7 Difference Count.
#define	C_PAD8_DiffCnt				600					//Set PAD8 Difference Count.
#define	C_PAD9_DiffCnt				600					//Set PAD9 Difference Count.
#define	C_PAD10_DiffCnt				600					//Set PAD10 Difference Count.
#define	C_PAD11_DiffCnt				600					//Set PAD11 Difference Count.
#define	C_PAD12_DiffCnt				600					//Set PAD12 Difference Count.
#define	C_PAD13_DiffCnt				600					//Set PAD13 Difference Count.
#define	C_PAD14_DiffCnt				600					//Set PAD14 Difference Count.
#define	C_PAD15_DiffCnt				600					//Set PAD15 Difference Count.



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * Timer Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  Timer Parameter Setting
//====================================================================================
#define _TIMER_MODULE               ENABLE              // Enable or Disable timer
#define _TIMER_TMR0_INT             DISABLE             // Enable or Disable TMR0 INT
#define _TIMER_TMR1_INT             DISABLE             // Enable or Disable TMR1 INT
#define _TIMER_TMR2_INT             DISABLE             // Enable or Disable TMR2 INT
#define _TIMER_TMR3_INT             DISABLE             // Enable or Disable TMR3 INT



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * UART Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  UART Parameter Setting
//====================================================================================
#define _UART_MODULE                DISABLE              // 0:DISABLE,1:ENABLE
#define _DEBUG_MODE					DISABLE              // 0:DISABLE,1:ENABLE. Use for Debug.
#define _UART_PORT					UART_PORT_PA67		 // UART_PORT_PA67: from PA6/PA7, UART_PORT_PD01: from PD0/PD1



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * GPIO Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  GPIO Parameter Setting
//====================================================================================
#define _GPIO_MODULE                ENABLE              // Enable or Disable GPIO
#define _GPIO_PA0_MODE              GPIO_PIN_OFF    	// Set PA0
#define _GPIO_PA1_MODE              GPIO_PIN_OFF    	// Set PA1
#define _GPIO_PA2_MODE              GPIO_PIN_OFF    	// Set PA2
#define _GPIO_PA3_MODE              GPIO_PIN_OFF    	// Set PA3
#define _GPIO_PA4_MODE              GPIO_PIN_OFF    	// Set PA4
#define _GPIO_PA5_MODE              GPIO_PIN_OFF    	// Set PA5
#define _GPIO_PA6_MODE              GPIO_PIN_OFF    	// Set PA6
#define _GPIO_PA7_MODE              GPIO_PIN_OFF    	// Set PA7
#define _GPIO_PA8_MODE              GPIO_PIN_OFF        // Set PA8
#define _GPIO_PA9_MODE              GPIO_PIN_OFF        // Set PA9
#define _GPIO_PA10_MODE             GPIO_PIN_OFF        // Set PA10
#define _GPIO_PA11_MODE             GPIO_PIN_OFF        // Set PA11
#define _GPIO_PA12_MODE             GPIO_PIN_OFF        // Set PA12
#define _GPIO_PA13_MODE             GPIO_PIN_OFF        // Set PA13
#define _GPIO_PA14_MODE             GPIO_PIN_OFF        // Set PA14
#define _GPIO_PA15_MODE             GPIO_PIN_OFF        // Set PA15
#define _GPIO_PB0_MODE              GPIO_PIN_OFF        // Set PB0
#define _GPIO_PB1_MODE              GPIO_PIN_OFF        // Set PB1
#define _GPIO_PB2_MODE              GPIO_PIN_OFF        // Set PB2
#define _GPIO_PB3_MODE              GPIO_PIN_OFF        // Set PB3
#define _GPIO_PB4_MODE              GPIO_PIN_OFF        // Set PB4
#define _GPIO_PB5_MODE              GPIO_PIN_OFF        // Set PB5
#define _GPIO_PB6_MODE              GPIO_PIN_OFF        // Set PB6
#define _GPIO_PB7_MODE              GPIO_PIN_OFF        // Set PB7
#define _GPIO_PB8_MODE              GPIO_PIN_OFF        // Set PB8
#define _GPIO_PB9_MODE              GPIO_PIN_OFF        // Set PB9
#define _GPIO_PB10_MODE             GPIO_PIN_OFF        // Set PB10
#define _GPIO_PB11_MODE             GPIO_PIN_OFF        // Set PB11
#define _GPIO_PB12_MODE             GPIO_PIN_OFF        // Set PB12
#define _GPIO_PB13_MODE             GPIO_PIN_OFF        // Set PB13
#define _GPIO_PB14_MODE             GPIO_PIN_OFF        // Set PB14
#define _GPIO_PB15_MODE             GPIO_PIN_OFF        // Set PB15
#define _GPIO_PC0_MODE              GPIO_PIN_OFF        // Set PC0
#define _GPIO_PC1_MODE              GPIO_PIN_OFF        // Set PC1
#define _GPIO_PC2_MODE              GPIO_PIN_OFF        // Set PC2
#define _GPIO_PC3_MODE              GPIO_PIN_OFF        // Set PC3
#define _GPIO_PC4_MODE              GPIO_PIN_OFF        // Set PC4
#define _GPIO_PC5_MODE              GPIO_PIN_OFF        // Set PC5
#define _GPIO_PC6_MODE              GPIO_PIN_OFF        // Set PC6
#define _GPIO_PC7_MODE              GPIO_PIN_OFF        // Set PC7
#define _GPIO_PD0_MODE              GPIO_PIN_OFF        // Set PD0
#define _GPIO_PD1_MODE              GPIO_PIN_OFF        // Set PD1
#define _GPIOA_INT					DISABLE				// Enable or Disable GPIOA INT
#define _GPIOB_INT					DISABLE				// Enable or Disable GPIOB INT
#define _GPIOC_INT					DISABLE				// Enable or Disable GPIOC INT
#define _EXIT0_INT					DISABLE				// Enable or Disable EXIT0 INT
#define _EXIT1_INT					DISABLE				// Enable or Disable EXIT1 INT
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  IO Wakeup Parameter Setting, Wakeup pin must also set input mode
//====================================================================================
#define _GPIO_WAKEUP                ENABLE              // Enable or Disable GPIO Wakeup
#define _GPIO_PA0_WAKEUP            DISABLE              // Enable or Disable PA0 Wakeup
#define _GPIO_PA1_WAKEUP            DISABLE              // Enable or Disable PA1 Wakeup
#define _GPIO_PA2_WAKEUP            DISABLE              // Enable or Disable PA2 Wakeup
#define _GPIO_PA3_WAKEUP            ENABLE              // Enable or Disable PA3 Wakeup
#define _GPIO_PA4_WAKEUP            ENABLE              // Enable or Disable PA4 Wakeup
#define _GPIO_PA5_WAKEUP            DISABLE              // Enable or Disable PA5 Wakeup
#define _GPIO_PA6_WAKEUP            DISABLE              // Enable or Disable PA6 Wakeup
#define _GPIO_PA7_WAKEUP            DISABLE              // Enable or Disable PA7 Wakeup
#define _GPIO_PA8_WAKEUP            DISABLE             // Enable or Disable PA8 Wakeup
#define _GPIO_PA9_WAKEUP            DISABLE             // Enable or Disable PA9 Wakeup
#define _GPIO_PA10_WAKEUP           DISABLE             // Enable or Disable PA10 Wakeup
#define _GPIO_PA11_WAKEUP           DISABLE             // Enable or Disable PA11 Wakeup
#define _GPIO_PA12_WAKEUP           DISABLE             // Enable or Disable PA12 Wakeup
#define _GPIO_PA13_WAKEUP           DISABLE             // Enable or Disable PA13 Wakeup
#define _GPIO_PA14_WAKEUP           DISABLE             // Enable or Disable PA14 Wakeup
#define _GPIO_PA15_WAKEUP           DISABLE             // Enable or Disable PA15 Wakeup
#define _GPIO_PB0_WAKEUP            DISABLE             // Enable or Disable PB0 Wakeup
#define _GPIO_PB1_WAKEUP            DISABLE             // Enable or Disable PB1 Wakeup
#define _GPIO_PB2_WAKEUP            DISABLE             // Enable or Disable PB2 Wakeup
#define _GPIO_PB3_WAKEUP            DISABLE             // Enable or Disable PB3 Wakeup
#define _GPIO_PB4_WAKEUP            DISABLE             // Enable or Disable PB4 Wakeup
#define _GPIO_PB5_WAKEUP            DISABLE             // Enable or Disable PB5 Wakeup
#define _GPIO_PB6_WAKEUP            DISABLE             // Enable or Disable PB6 Wakeup
#define _GPIO_PB7_WAKEUP            DISABLE             // Enable or Disable PB7 Wakeup
#define _GPIO_PB8_WAKEUP            DISABLE             // Enable or Disable PB8 Wakeup
#define _GPIO_PB9_WAKEUP            DISABLE             // Enable or Disable PB9 Wakeup
#define _GPIO_PB10_WAKEUP           DISABLE             // Enable or Disable PB10 Wakeup
#define _GPIO_PB11_WAKEUP           DISABLE             // Enable or Disable PB11 Wakeup
#define _GPIO_PB12_WAKEUP           DISABLE             // Enable or Disable PB12 Wakeup
#define _GPIO_PB13_WAKEUP           DISABLE             // Enable or Disable PB13 Wakeup
#define _GPIO_PB14_WAKEUP           DISABLE             // Enable or Disable PB14 Wakeup
#define _GPIO_PB15_WAKEUP           DISABLE             // Enable or Disable PB15 Wakeup
#define _GPIO_PC0_WAKEUP            DISABLE             // Enable or Disable PC0 Wakeup
#define _GPIO_PC1_WAKEUP            DISABLE             // Enable or Disable PC1 Wakeup
#define _GPIO_PC2_WAKEUP            DISABLE             // Enable or Disable PC2 Wakeup
#define _GPIO_PC3_WAKEUP            DISABLE             // Enable or Disable PC3 Wakeup
#define _GPIO_PC4_WAKEUP            DISABLE             // Enable or Disable PC4 Wakeup
#define _GPIO_PC5_WAKEUP            DISABLE             // Enable or Disable PC5 Wakeup
#define _GPIO_PC6_WAKEUP            DISABLE             // Enable or Disable PC6 Wakeup
#define _GPIO_PC7_WAKEUP            DISABLE             // Enable or Disable PC7 Wakeup
#define _GPIO_PD0_WAKEUP            DISABLE             // Enable or Disable PD0 Wakeup
#define _GPIO_PD1_WAKEUP            DISABLE             // Enable or Disable PD1 Wakeup



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * SPI Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  SPI0 Parameter Setting
//====================================================================================
#define _SPI_MODULE                 ENABLE              // Enable or Disable SPI0. Don't delete this line.
#if _EF_SERIES
	#define _SPI_ACCESS_MODE        (_VR_FUNC?SPI_1_4_4_MODE:SPI_1_1_1_MODE)	// For NX1 EF series, SPI0 data mode. Don't delete this line.
#else
	#define _SPI_ACCESS_MODE        (_VR_FUNC?SPI_1_4_4_MODE:SPI_1_1_1_MODE)      // For NX1 OTP series, SPI0 data mode.
#endif
#define _SPI_ADDR_BYTE              3					// SPI0 Address Byte(3 or 4)
#define _SPI_SECTOR_ERASE_MAX_TIME  750                 // SPI0 sector erase time max spec, unit:msec
#define _SPI_BLOCK_ERASE_MAX_TIME   5300                // SPI0 block erase time max spec, unit:msec
#define _SPI_INT					DISABLE				// Enable or Disable SPI0 INT
#define _SPI_KEEP_VDD_IN_SLEEP		DISABLE				// Keep SPI0_Vdd in sleep, only for NX1 OTP series.
#define _GPIO_PB0_5_POWER			EXT_PWR				// Select power source of PB[0:5] (INT_PWR or EXT_PWR), only for NX1 OTP series.
//====================================================================================
//  SPI1 Parameter Setting
//====================================================================================
#define _SPI1_MODULE 				DISABLE				// Enable or Disable SPI1. Don't delete this line.
#define _SPI1_USE_FLASH				DISABLE				// Enable or Disable SPI Flash
#define _SPI1_ACCESS_MODE           SPI_1_1_1_MODE_3WIRE// For NX1 EF series, SPI1 data mode. Don't delete this line.
#define _SPI1_ADDR_BYTE             3					// SPI1 Address Byte(3 or 4)
#define _SPI1_SECTOR_ERASE_MAX_TIME 750                 // SPI1 sector erase time max spec, unit:msec
#define _SPI1_BLOCK_ERASE_MAX_TIME  5300                // SPI1 block erase time max spec, unit:msec
#define _SPI1_INT					DISABLE				// Enable or Disable SPI1 INT
//====================================================================================
//  SPI EEPROM Emulation Parameter Setting
//====================================================================================
#define _SPI_EEPROM_MODULE          DISABLE             // Enable or Disable SPI EEPROM, it only supports SPI0
#define _SPI_EEPROM_RSRC_INDEX		0					// Set the index number of SPI EEPROM Resource file
#define _SPI_EEPROM_SIZE			4					// SPI EEPROM size, Unit:Kbytes. (only supports 4/8/16/32/64KB)
#define _SPI_EEPROM_RINGBUF_NO		10					// SPI EEPROM emulated buffer numbers. (2~100)



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * XIP Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  XIP Parameter Setting
//====================================================================================
#if _EF_SERIES
	#define _XIP_IO_MODE            ((_SPI_ACCESS_MODE==SPI_1_4_4_MODE)?XIP_4_IO_MODE:XIP_1_IO_MODE)       // For NX1 EF series, SPI data mode
#else
	#define _XIP_IO_MODE            ((_SPI_ACCESS_MODE==SPI_1_4_4_MODE)?XIP_4_IO_MODE:XIP_1_IO_MODE)       // For NX1 OTP series, SPI data mode
#endif



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * I2C Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  I2C Parameter Setting
//====================================================================================
#define _I2C_MODULE                 DISABLE             // Enable or Disable I2C
#define _I2C_DEVICE_MODE            I2C_MASTER          // I2C_MASTER,I2C_SLAVE
#define _I2C_CLOCK_SPEED            400000              // 100K~1M, Unit:Hz
#define _I2C_TIMEOUT_MS             30                  // 0(DISABLE), 1~30, Unit:ms
#define _I2C_SLAVE_ADDRESS_ID       0x61                // 0x0~0x7F(7Bit),0x0~0x3FF(10Bit)
#define _I2C_SLAVE_ADDRESS_Is10Bit  false               // true or false
#define _I2C_INT                    DISABLE             // Enable or Disable I2C INT



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * GPIO_I2C Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  GPIO_I2C Parameter Setting
//====================================================================================
#define _GPIO_I2C_MODULE			DISABLE				// Enable or Disable GPIO_I2C
#define _GPIO_I2C_IO				GPIOA				// GPIO Use Port
#define _GPIO_I2C_SDA				12					// GPIO Use Pin for SDA
#define _GPIO_I2C_SCL				13					// GPIO Use Pin for SCL

#define _EEPROM_24LC256_MODULE		DISABLE				// Enable or Disable EEPROM_24LC256



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * PWM-IO Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  PWM-IO Parameter Setting
//====================================================================================
#define _PWMA_MODULE                DISABLE             // Enable or Disable PWMA
#define _PWMB_MODULE                DISABLE             // Enable or Disable PWMB
#define _PWMA0_DUTY                 0xFF                // Unit is %. 0xFF is off
#define _PWMA1_DUTY                 0xFF                // Unit is %. 0xFF is off
#define _PWMA2_DUTY                 0xFF                // Unit is %. 0xFF is off
#define _PWMA3_DUTY                 0xFF                // Unit is %. 0xFF is off
#define _PWMB0_DUTY                 0xFF                // Unit is %. 0xFF is off
#define _PWMB1_DUTY                 0xFF                // Unit is %. 0xFF is off
#define _PWMB2_DUTY                 0xFF                // Unit is %. 0xFF is off
#define _PWMB3_DUTY                 0xFF                // Unit is %. 0xFF is off
#define _PWMA0_STATUS               START_HIGH          // START_HIGH or START_LOW
#define _PWMA1_STATUS               START_HIGH          // START_HIGH or START_LOW
#define _PWMA2_STATUS               START_HIGH          // START_HIGH or START_LOW
#define _PWMA3_STATUS               START_HIGH          // START_HIGH or START_LOW
#define _PWMB0_STATUS               START_HIGH          // START_HIGH or START_LOW
#define _PWMB1_STATUS               START_HIGH          // START_HIGH or START_LOW
#define _PWMB2_STATUS               START_HIGH          // START_HIGH or START_LOW
#define _PWMB3_STATUS               START_HIGH          // START_HIGH or START_LOW
#define _PWMA_INT                   DISABLE             // Enable or Disable PWMA INT
#define _PWMB_INT                   DISABLE             // Enable or Disable PWMB INT



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * CAPTURE Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  Capture Parameter Setting
//====================================================================================
#define _CAPTURE_MODULE             DISABLE             // Enable or Disable Capture
#define _CAPTURE_PAx                0                   // Set capture pin in PAx
#define _CAPTURE_INT                DISABLE             // Enable or Disable Capture INT
#define _CAPTURE_INT_MODE           CAP_RISING_FALLING  // Set Interrupt Mode



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * IR Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  IR Parameter Setting
//====================================================================================
#define _IR_MODULE                  DISABLE             // Enable or Disable IR Module
#define _IR_DATA_LEN				IR_DATA_LEN_16BIT	// IR TX&RX data length, unit:bit
#define _IR_CRC_LEN					IR_CRC_LEN_8BIT		// IR data CRC length, unit:bit
#define _IR_LOW_POWER_TYPE          DISABLE             // Select low power IR type(DISABLE/TYPE1/TYPE2, TYPE1:S/W, TYPE2:H/W)

#define _IR_TX_EN					DISABLE				// Enable or Disable IR_TX
#define _IR_TX_OUTPUT				USE_IR0_TX			// Select output(USE_IR0_TX/USE_IR1_TX/USE_IR2_TX/USE_IR3_TX)
#define _IR_CARRIER					IR_TX_CARRIER_HIGH	// TX Carry data signal level
#define _IR_FREQ					IR_TX_38K5HZ		// TX Carry data frequency(IR_TX_38K5HZ/IR_TX_55K5HZ)

#define _IR_RX_EN					DISABLE				// Enable or Disable IR_RX
#define _IR_RX_PORT					GPIOA				// Set IR_RX use port
#define _IR_RX_PIN					4					// Set IR_RX use pin
#define _IR_RX_WAKEUP				ENABLE				// Enable or Disable RX Wakeup
#define _IR_RX_TIMER                USE_RTC             // Select low power IR_RX scan timer(USE_RTC/USE_TIMERn, _IR_LOW_POWER_TYPE=TYPE1 is valid)
#define _IR_RX_EXT_INT              TRIG_EXT0          	// Select low power IR_RX data external interrupt pin(TRIG_EXT0/TRIG_EXT1)
#define _IR_RX_PWR_PORT             GPIOA               // Select low power IR_RX use port(_IR_LOW_POWER_TYPE=TYPE1 is valid)
#define _IR_RX_PWR_PIN              13                  // Select low power IR_RX use pin(_IR_LOW_POWER_TYPE=TYPE1 is valid)



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * Action Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  Action Parameter Setting
//====================================================================================
#define _ACTION_MODULE              DISABLE         	// Enable or Disable ACTION
#define _VIO_CH_NUM                 4               	// The number of action channel
#define _VIO_TIMER                  USE_RTC				// Set VIO timer.



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * SW PWM-IO Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  SW PWM-IO Parameter Setting
//====================================================================================
#define	_SW_PWM_MODULE				DISABLE				// Enable or Disable SW PWM-IO
#define	_SW_PWM_CH_NUM				4					// SW PWM-IO, QIO CH setting
#define	_SW_PWM_BUSY_LEVEL			SW_PWM_BUSY_LOW		// SW_PWM_BUSY_HIGH, SW_PWM_BUSY_LOW



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * LED Strip Setting *
//====================================================================================
#define	_LEDSTR_MODULE				DISABLE				// Enable or Disable LEDSTR
#define	_LEDSTR_TIMER				USE_TIMER2			// Set LEDSTR Timer
#define _LEDSTR_CH_NUM				1					// Set LEDSTR Channel number(1~16)
#define	_LEDSTR_CH0_PIN				GPIO_PA7			// Set LEDSTR Channel 0 IO Pin
#define	_LEDSTR_CH0_NUM				20					// Set LEDSTR Channel 0 number
#define	_LEDSTR_CH1_PIN				0xFF				// Set LEDSTR Channel 1 IO Pin
#define	_LEDSTR_CH1_NUM				20					// Set LEDSTR Channel 1 number
#define	_LEDSTR_CH2_PIN				0xFF				// Set LEDSTR Channel 2 IO Pin
#define	_LEDSTR_CH2_NUM				20					// Set LEDSTR Channel 2 number
#define	_LEDSTR_CH3_PIN				0xFF				// Set LEDSTR Channel 3 IO Pin
#define	_LEDSTR_CH3_NUM				20					// Set LEDSTR Channel 3 number
#define	_LEDSTR_CH4_PIN				0xFF				// Set LEDSTR Channel 4 IO Pin
#define	_LEDSTR_CH4_NUM				20					// Set LEDSTR Channel 4 number
#define	_LEDSTR_CH5_PIN				0xFF				// Set LEDSTR Channel 5 IO Pin
#define	_LEDSTR_CH5_NUM				20					// Set LEDSTR Channel 5 number
#define	_LEDSTR_CH6_PIN				0xFF				// Set LEDSTR Channel 6 IO Pin
#define	_LEDSTR_CH6_NUM				20					// Set LEDSTR Channel 6 number
#define	_LEDSTR_CH7_PIN				0xFF				// Set LEDSTR Channel 7 IO Pin
#define	_LEDSTR_CH7_NUM				20					// Set LEDSTR Channel 7 number
#define	_LEDSTR_CH8_PIN				0xFF				// Set LEDSTR Channel 8 IO Pin
#define	_LEDSTR_CH8_NUM				20					// Set LEDSTR Channel 8 number
#define	_LEDSTR_CH9_PIN				0xFF				// Set LEDSTR Channel 9 IO Pin
#define	_LEDSTR_CH9_NUM				20					// Set LEDSTR Channel 9 number
#define	_LEDSTR_CH10_PIN			0xFF				// Set LEDSTR Channel 10 IO Pin
#define	_LEDSTR_CH10_NUM			20					// Set LEDSTR Channel 10 number
#define	_LEDSTR_CH11_PIN			0xFF				// Set LEDSTR Channel 11 IO Pin
#define	_LEDSTR_CH11_NUM			20					// Set LEDSTR Channel 11 number
#define	_LEDSTR_CH12_PIN			0xFF				// Set LEDSTR Channel 12 IO Pin
#define	_LEDSTR_CH12_NUM			20					// Set LEDSTR Channel 12 number
#define	_LEDSTR_CH13_PIN			0xFF				// Set LEDSTR Channel 13 IO Pin
#define	_LEDSTR_CH13_NUM			20					// Set LEDSTR Channel 13 number
#define	_LEDSTR_CH14_PIN			0xFF				// Set LEDSTR Channel 14 IO Pin
#define	_LEDSTR_CH14_NUM			20					// Set LEDSTR Channel 14 number
#define	_LEDSTR_CH15_PIN			0xFF				// Set LEDSTR Channel 15 IO Pin
#define	_LEDSTR_CH15_NUM			20					// Set LEDSTR Channel 15 number

#define _LEDSTR_TIMING				25					// Set LEDSTR Timing 25/50/100 ms
#define _LEDSTR_CODE_0				1					// Set LEDSTR code 0 Transfer time 1:T0H+T0L=300+900 ns, 2:T0H+T0L=400+800 ns, 3:T0H+T0L=500+700 ns, 4:T0H+T0L=600+600 ns
#define _LEDSTR_CODE_1				4					// Set LEDSTR code 1 Transfer time 1:T1H+T1L=600+600 ns, 2:T1H+T1L=700+500 ns, 3:T1H+T1L=800+400 ns, 4:T1H+T1L=900+300 ns



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * Sync Play LED Strip Setting *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
#define	_LEDSTRn_MODULE				DISABLE				// Enable or Disable LED Multi strip
#define _LEDSTRn_IO_TOTAL_NUM		1					// Set LEDSTRn IO number
#define	_LEDSTRn_OUTPUT0			GPIO_PA7			// Set LEDSTRn Output0 Pin
#define	_LEDSTRn_OUTPUT1			0xFF				// Set LEDSTRn Output1 Pin
#define	_LEDSTRn_OUTPUT2			0xFF				// Set LEDSTRn Output2 Pin
#define	_LEDSTRn_OUTPUT3			0xFF				// Set LEDSTRn Output3 Pin
#define	_LEDSTRn_OUTPUT4			0xFF				// Set LEDSTRn Output4 Pin
#define	_LEDSTRn_OUTPUT5			0xFF				// Set LEDSTRn Output5 Pin
#define	_LEDSTRn_OUTPUT6			0xFF				// Set LEDSTRn Output6 Pin
#define	_LEDSTRn_OUTPUT7			0xFF				// Set LEDSTRn Output7 Pin
#define	_LEDSTRn_OUTPUT8			0xFF				// Set LEDSTRn Output8 Pin
#define	_LEDSTRn_OUTPUT9			0xFF				// Set LEDSTRn Output9 Pin
#define	_LEDSTRn_OUTPUT10			0xFF				// Set LEDSTRn Output10 Pin
#define	_LEDSTRn_OUTPUT11			0xFF				// Set LEDSTRn Output11 Pin
#define	_LEDSTRn_OUTPUT12			0xFF				// Set LEDSTRn Output12 Pin
#define	_LEDSTRn_OUTPUT13			0xFF				// Set LEDSTRn Output13 Pin
#define	_LEDSTRn_OUTPUT14			0xFF				// Set LEDSTRn Output14 Pin
#define	_LEDSTRn_OUTPUT15			0xFF				// Set LEDSTRn Output15 Pin
#define	_LEDSTRn_TIMER				USE_TIMER2			// Set LEDSTRn Timer
#define _LEDSTRn_MAX_NUM			30					// Set LEDSTRn MAX number
#define _LEDSTRn_TIMING				25					// Set LEDSTRn Timing 25/50/100 ms
#define _LEDSTRn_CODE_0				1					// Set LEDSTR code 0 Transfer time 1:T0H+T0L=300+900 ns, 2:T0H+T0L=400+800 ns, 3:T0H+T0L=500+700 ns, 4:T0H+T0L=600+600 ns
#define _LEDSTRn_CODE_1				4					// Set LEDSTR code 1 Transfer time 1:T1H+T1L=600+600 ns, 2:T1H+T1L=700+500 ns, 3:T1H+T1L=800+400 ns, 4:T1H+T1L=900+300 ns



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * Scrolling Text LED Strip Setting *
//====================================================================================
/////////////////////////////////////////////////////////////////////////////////////
#define	_LEDSTRm_MODULE				DISABLE				// Enable or Disable LED Multiple string, unequal length
#define _LEDSTRm_IO_TOTAL_NUM		8
#define	_LEDSTRm_OUTPUT0			GPIO_PA0			// Set LEDSTRm Output Pin
#define	_LEDSTRm_OUTPUT1			GPIO_PA1			// Set LEDSTRm Output Pin
#define	_LEDSTRm_OUTPUT2			GPIO_PA2			// Set LEDSTRm Output Pin
#define	_LEDSTRm_OUTPUT3			GPIO_PA3			// Set LEDSTRm Output Pin
#define	_LEDSTRm_OUTPUT4			GPIO_PA4			// Set LEDSTRm Output Pin
#define	_LEDSTRm_OUTPUT5			GPIO_PA5			// Set LEDSTRm Output Pin
#define	_LEDSTRm_OUTPUT6			GPIO_PA6			// Set LEDSTRm Output Pin
#define	_LEDSTRm_OUTPUT7			GPIO_PA7			// Set LEDSTRm Output Pin
#define	_LEDSTRm_OUTPUT8			0xFF				// Set LEDSTRm Output Pin
#define	_LEDSTRm_OUTPUT9			0xFF				// Set LEDSTRm Output Pin
#define	_LEDSTRm_OUTPUT10			0xFF				// Set LEDSTRm Output Pin
#define	_LEDSTRm_OUTPUT11			0xFF				// Set LEDSTRm Output Pin
#define	_LEDSTRm_OUTPUT12			0xFF				// Set LEDSTRm Output Pin
#define	_LEDSTRm_OUTPUT13			0xFF				// Set LEDSTRm Output Pin
#define	_LEDSTRm_OUTPUT14			0xFF				// Set LEDSTRm Output Pin
#define	_LEDSTRm_OUTPUT15			0xFF				// Set LEDSTRm Output Pin
#define	_LEDSTRm_TIMER				USE_TIMER2			// Set LEDSTR Timer

#define _LEDSTRm_MAX_NUM			256					// Set LEDSTR Max number
#define _LEDSTRm_TIMING				25					// Set LEDSTR Timing 25/50/100 ms
#define _LEDSTRm_CODE_0				1					// Set LEDSTR code 0 Transfer time 1:T0H+T0L=300+900 ns, 2:T0H+T0L=400+800 ns, 3:T0H+T0L=500+700 ns, 4:T0H+T0L=600+600 ns
#define _LEDSTRm_CODE_1				4					// Set LEDSTR code 1 Transfer time 1:T1H+T1L=600+600 ns, 2:T1H+T1L=700+500 ns, 3:T1H+T1L=800+400 ns, 4:T1H+T1L=900+300 ns



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * Sound_Localization Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  Sound_Localization Parameter Setting
//====================================================================================
#define	_SL_MODULE					DISABLE				// Enable or Disable
#define _SL_MIC_NUM                 3                   // Select MIC number (2 / 3)
#define	_SL_AMP_SD_CTL_PORT			GPIOA				// Select PORT GPIOA/GPIOB/GPIOC(only NX1EV) (Old naming is _SL_HWEnable_PORT)
#define	_SL_AMP_SD_CTL_PIN			15					// Select PIN 0,2~15 for GPIOA, 0~15 for GPIOB, 0~7 for GPIOC(only NX1EV) (Old naming is _SL_HWEnable_PIN)
#define	_SL_AMP_SD_CTL_ACTIVE		ACTIVE_LOW			// Select shut down pin of AMP is ACTIVE_LOW(default) or ACTIVE_HIGH, (Old naming is _SL_HWEnable_PIN_OUTPUT)



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * Storage Module *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  Storage Parameter Setting
//====================================================================================
#define _STORAGE_MODULE             DISABLE             // Enable or Disable Storage Module
#define _STORAGE_AUTO_SAVE          DISABLE             // Enable or Disable Auto Save Before Go Into Halt
#define	_STORAGE_MODE               SPI_MODE            // Set Storage Mode

#if USER_STORAGE_BREAK_POINT_MOUDLE
	#define	_STORAGE_BUF_SIZE	    USER_STORAGE_VAR_SIZE   // Set Total Variable RAM Size, unit:Bytes
	#define	_STORAGE_VAR_CNT	    USER_STORAGE_VAR_CNT    // Set Variable Amount
#else
	#define	_STORAGE_BUF_SIZE	    8             			// Set Total Variable RAM Size, unit:Bytes
	#define	_STORAGE_VAR_CNT	    2             			// Set Variable Amount
#endif

#define	_STORAGE_USER_DEF_IDX       0                   // Set the Index of User Defined
#define	_STORAGE_USER_DEF_AMOUNT    2                   // Set the Amount of User Defined, unit: 4kBytes for SpiFlash; 512Bytes for Embedded Flash
                                                        // The Min Amount is 2



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  * Software Setting *
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
#define _AUDIO_API_CH_NUM           4                   // SW channel number
#define _RANDOM_GEN                 DISABLE             // Enable or Disable
#define _USE_HEAP_RAM               DISABLE        		// Enable or Disable Heap Area
#define _SW_INT                     DISABLE				// Enable or Disable SWI INT and SWA INT



#include "nx1_tool_ver.h"
#include "nx1_config_check.h"
#endif //#ifndef NX1_CONFIG_H
