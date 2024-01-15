//==========================================================================
// File Name          : nx1_def.h
// Description        : Load from NYIDE. Produce automatically by NYIDE.
// Last modified date : 2016/10/20
//==========================================================================
#ifndef NX1_DEF
#define NX1_DEF



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  Option Setting from NYIDE
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
// IC BODY
#define OPTION_IC_BODY              0x11A21          	
#define OPTION_IC_BODY_EXTD         0x00000000          	
// BODY Version
#define OPTION_IC_VER               0           // 0: A, 1: B 
// ROM Size
#define OPTION_IC_SIZE              0x8000     // Unit is byte
// RAM Size
#define OPTION_DLM_SIZE             0x1000      // Unit is byte
// High Clock Source
#define OPTION_HIGH_CLK_SRC         0      		// 0: Internal, 1: External 
// High Frequency Clock
#define OPTION_HIGH_FREQ_CLK        16000000    // Unit is Hz 
// Low Clock Source
#define OPTION_LOW_CLK_SRC          0       	// 0: Internal, 1: External
// VDD Voltage
#define OPTION_VDD_VOLTAGE          0      		// 0: 4.5V, 1:3.0V
// SPI0 Voltage
#define OPTION_SPI0_VOLTAGE         0     		// 0: 3.3V, 1:1.8V
// Intruction RAM
#define OPTION_IRAM                 0     		// Inst. RAM 0:Disable, 1:Enable
// Speaker Impedance
#define OPTION_SPEAKER_IMPEDANCE    8     		// Unit:ohms (4/8/16)



//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  MIDI Setting from Q-MIDI
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
// Channel Number
#define QMIDI_CH_NUM                16     		// Unit is channel.
// Timbre Format
#define QMIDI_TIMBRE_FORMAT         1			// 1: PCM, 2: ULAW
// Sample Rate
#define QMIDI_SAMPLE_RATE           32000  		// Unit is Hz
// Storage Type
#define QMIDI_STORAGE_TYPE			0	
// Pitch Shift
#define QMIDI_PITCH_SHIFT           0
// Pitch Event


//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  Cyberon VR Setting
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
// VR Storage Type
#define _VR_STORAGE_TYPE            0           // 0: OTP, 1: SPI
// VR Sample Rate
#define _VR_SAMPLE_RATE             0
// VR Frame Rate
#define _VR_FRAME_RATE              0
// VR RAM Usage
#define _VR_ENGINE_RAM_SIZE         0
// VR SD RAM Usage
#define _VR_SD_ENGINE_RAM_SIZE      0
// VR Platform
#define _VR_PLATFORM                16
// VR Energy Threshold
#define _VR_ENERGY_THRESHOLD        0

//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  Action Setting from Resource Manager of NYIDE
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
// Action timebase
#define _NYIDE_TIMEBASE             250         // Unit is us
#define _NYIDE_FRAMERATE            80          // Unit is Hz, 20~150

//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  SPI Flash Security Settings
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
#define N25Q_FLASH_PAIRING          0           // 1: Enable, 0: Disable
#define N25Q_PAIRING_CODE_0         0x00000000  // From SPI_Encoder
#define N25Q_PAIRING_CODE_1         0x00000000  // From SPI_Encoder
#define N25Q_SERIES_ONLY            0           // 1: Enable, 0: Disable
#define N25Q_ANTI_COPY              0           // 1: Enable, 0: Disable

//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  Common OTP Settings
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
#define _XIP_COC                    DISABLE

//////////////////////////////////////////////////////////////////////////////////////
//====================================================================================
//  Tool Version
//====================================================================================
//////////////////////////////////////////////////////////////////////////////////////
#define NYIDE_VER                   520
#define SPI_ENCODER_VER             202

#endif //#ifndef NX1_DEF
