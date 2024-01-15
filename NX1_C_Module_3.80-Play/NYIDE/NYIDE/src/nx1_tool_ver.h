﻿#line	2001	"nx1_config.h"
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)



//====================================================================================
// Tool Version Setting
//====================================================================================
#ifndef NYIDE_VER
	#define NYIDE_VER 								390
#endif
#ifndef SPI_ENCODER_VER
	#define SPI_ENCODER_VER 						170
#endif

#define SUPPORTED_NYIDE_VER							390
#define SUPPORTED_SPI_ENCODER_VER					170



//====================================================================================
// Tool Version Checking 
//====================================================================================
#ifdef _EF_SERIES
	#if _EF_SERIES
		#if (NYIDE_VER < 470) || (SPI_ENCODER_VER < 193)
			#define MSG_FUNC_UPDATE
		#endif
		#if (NYIDE_VER < 470) && (SUPPORTED_NYIDE_VER < 470)
			#undef SUPPORTED_NYIDE_VER
			#define SUPPORTED_NYIDE_VER				470
		#endif
		#if (SPI_ENCODER_VER < 193) && (SUPPORTED_SPI_ENCODER_VER < 193)
			#undef SUPPORTED_SPI_ENCODER_VER
			#define SUPPORTED_SPI_ENCODER_VER		193
		#endif
	#endif
#endif

#ifdef	_ACTION_MODULE
	#if _ACTION_MODULE == ENABLE
		#if (NYIDE_VER < 470) || (SPI_ENCODER_VER < 180)
			#define MSG_FUNC_UPDATE
		#endif
		#if (NYIDE_VER < 470) && (SUPPORTED_NYIDE_VER < 470)
			#undef SUPPORTED_NYIDE_VER
			#define SUPPORTED_NYIDE_VER				470
		#endif
		#if (SPI_ENCODER_VER < 180) && (SUPPORTED_SPI_ENCODER_VER < 180)
			#undef SUPPORTED_SPI_ENCODER_VER
			#define SUPPORTED_SPI_ENCODER_VER		180
		#endif
	#endif//#if _ACTION_MODULE == ENABLE
#endif//#ifdef	_ACTION_MODULE


#if _ADPCM_PLAY && _ADPCM_PLAY_REPEAT	//_ADPCM_QIO_MODULE
	#if (NYIDE_VER < 400) || (SPI_ENCODER_VER < 190)
		#define MSG_FUNC_UPDATE
	#endif
	#if (NYIDE_VER < 400) && (SUPPORTED_NYIDE_VER < 400)
		#undef SUPPORTED_NYIDE_VER
		#define SUPPORTED_NYIDE_VER					400
	#endif
	#if (SPI_ENCODER_VER < 190) && (SUPPORTED_SPI_ENCODER_VER < 190)
		#undef SUPPORTED_SPI_ENCODER_VER
		#define SUPPORTED_SPI_ENCODER_VER			190
	#endif
#endif

#if _ADPCM_PLAY	//Customize EQ filter
#if (_ADPCM_PLAY_BACK_EQ==ENABLE)	\
	||(_ADPCM_RECORD_PLAY_EQ==ENABLE && _ADPCM_RECORD_PLAY_TYPE!= TYPE1 && _ADPCM_EQ_TYPE==CUSTOMIZE)
	#if (NYIDE_VER < 421)
		#define MSG_FUNC_UPDATE
	#endif
	#if (NYIDE_VER < 421) && (SUPPORTED_NYIDE_VER < 421)
		#undef SUPPORTED_NYIDE_VER
		#define SUPPORTED_NYIDE_VER					421
	#endif
#endif
#endif

#if _ADPCM_CH2_PLAY	//Customize EQ filter
#if (_ADPCM_CH2_PLAY_BACK_EQ==ENABLE)
	#if (NYIDE_VER < 421)
		#define MSG_FUNC_UPDATE
	#endif
	#if (NYIDE_VER < 421) && (SUPPORTED_NYIDE_VER < 421)
		#undef SUPPORTED_NYIDE_VER
		#define SUPPORTED_NYIDE_VER					421
	#endif
#endif
#endif

#if _ADPCM_CH3_PLAY	//Customize EQ filter
#if (_ADPCM_CH3_PLAY_BACK_EQ==ENABLE)
	#if (NYIDE_VER < 421)
		#define MSG_FUNC_UPDATE
	#endif
	#if (NYIDE_VER < 421) && (SUPPORTED_NYIDE_VER < 421)
		#undef SUPPORTED_NYIDE_VER
		#define SUPPORTED_NYIDE_VER					421
	#endif
#endif
#endif

#if _ADPCM_CH4_PLAY	//Customize EQ filter
#if (_ADPCM_CH4_PLAY_BACK_EQ==ENABLE)
	#if (NYIDE_VER < 421)
		#define MSG_FUNC_UPDATE
	#endif
	#if (NYIDE_VER < 421) && (SUPPORTED_NYIDE_VER < 421)
		#undef SUPPORTED_NYIDE_VER
		#define SUPPORTED_NYIDE_VER					421
	#endif
#endif
#endif

#if _ADPCM_CH5_PLAY	//Customize EQ filter
#if (_ADPCM_CH5_PLAY_BACK_EQ==ENABLE)
	#if (NYIDE_VER < 421)
		#define MSG_FUNC_UPDATE
	#endif
	#if (NYIDE_VER < 421) && (SUPPORTED_NYIDE_VER < 421)
		#undef SUPPORTED_NYIDE_VER
		#define SUPPORTED_NYIDE_VER					421
	#endif
#endif
#endif

#if _ADPCM_CH6_PLAY	//Customize EQ filter
#if (_ADPCM_CH6_PLAY_BACK_EQ==ENABLE)
	#if (NYIDE_VER < 421)
		#define MSG_FUNC_UPDATE
	#endif
	#if (NYIDE_VER < 421) && (SUPPORTED_NYIDE_VER < 421)
		#undef SUPPORTED_NYIDE_VER
		#define SUPPORTED_NYIDE_VER					421
	#endif
#endif
#endif

#if _ADPCM_CH7_PLAY	//Customize EQ filter
#if (_ADPCM_CH7_PLAY_BACK_EQ==ENABLE)
	#if (NYIDE_VER < 421)
		#define MSG_FUNC_UPDATE
	#endif
	#if (NYIDE_VER < 421) && (SUPPORTED_NYIDE_VER < 421)
		#undef SUPPORTED_NYIDE_VER
		#define SUPPORTED_NYIDE_VER					421
	#endif
#endif
#endif

#if _ADPCM_CH8_PLAY	//Customize EQ filter
#if (_ADPCM_CH8_PLAY_BACK_EQ==ENABLE)
	#if (NYIDE_VER < 421)
		#define MSG_FUNC_UPDATE
	#endif
	#if (NYIDE_VER < 421) && (SUPPORTED_NYIDE_VER < 421)
		#undef SUPPORTED_NYIDE_VER
		#define SUPPORTED_NYIDE_VER					421
	#endif
#endif
#endif

#if _LEDSTR_MODULE
	#if (NYIDE_VER < 500)
		#define MSG_FUNC_UPDATE
	#endif
	#if (NYIDE_VER < 500) && (SUPPORTED_NYIDE_VER < 500)
		#undef SUPPORTED_NYIDE_VER
		#define SUPPORTED_NYIDE_VER					500
	#endif
#endif


#if (!_EF_SERIES && _SPI_MODULE==DISABLE && _GPIO_PB0_5_POWER==EXT_PWR)
	#if (NYIDE_VER < 472)
		#define MSG_FUNC_UPDATE
	#endif
	#if (NYIDE_VER < 472) && (SUPPORTED_NYIDE_VER < 472)
		#undef SUPPORTED_NYIDE_VER
		#define SUPPORTED_NYIDE_VER					472
	#endif
#endif

#if _MIDI_PLAY && _MIDI_INST_NOTEON
	#if (NYIDE_VER < 481)
		#define MSG_FUNC_UPDATE
	#endif
	#if (NYIDE_VER < 481) && (SUPPORTED_NYIDE_VER < 481)
		#undef SUPPORTED_NYIDE_VER
		#define SUPPORTED_NYIDE_VER					481
	#endif
#if _MIDI_INST_NOTEON_GLISS
	#if (NYIDE_VER < 493)
		#define MSG_FUNC_UPDATE
	#endif
	#if (NYIDE_VER < 493) && (SUPPORTED_NYIDE_VER < 493)
		#undef SUPPORTED_NYIDE_VER
		#define SUPPORTED_NYIDE_VER					493
	#endif
#endif	
#endif

// Old Version
#if (NYIDE_VER < 390) || (SPI_ENCODER_VER < 170)
	#define MSG_OLD_UPDATE
#endif



//====================================================================================
// Message Output 
//====================================================================================
#ifdef MSG_FUNC_UPDATE
	#if   SUPPORTED_NYIDE_VER < 100
		#define SHOW_NYIDE_VER_MAJOR	0
	#elif SUPPORTED_NYIDE_VER < 200
		#define SHOW_NYIDE_VER_MAJOR	1
	#elif SUPPORTED_NYIDE_VER < 300
		#define SHOW_NYIDE_VER_MAJOR	2
	#elif SUPPORTED_NYIDE_VER < 400
		#define SHOW_NYIDE_VER_MAJOR	3
	#elif SUPPORTED_NYIDE_VER < 500
		#define SHOW_NYIDE_VER_MAJOR	4
	#elif SUPPORTED_NYIDE_VER < 600
		#define SHOW_NYIDE_VER_MAJOR	5
	#elif SUPPORTED_NYIDE_VER < 700
		#define SHOW_NYIDE_VER_MAJOR	6
	#elif SUPPORTED_NYIDE_VER < 800
		#define SHOW_NYIDE_VER_MAJOR	7
	#elif SUPPORTED_NYIDE_VER < 900
		#define SHOW_NYIDE_VER_MAJOR	8
	#elif SUPPORTED_NYIDE_VER < 1000
		#define SHOW_NYIDE_VER_MAJOR	9
	#endif

	#if   (SUPPORTED_NYIDE_VER-100*SHOW_NYIDE_VER_MAJOR) < 10
		#define SHOW_NYIDE_VER_MINOR1	0
	#elif (SUPPORTED_NYIDE_VER-100*SHOW_NYIDE_VER_MAJOR) < 20
		#define SHOW_NYIDE_VER_MINOR1	1
	#elif (SUPPORTED_NYIDE_VER-100*SHOW_NYIDE_VER_MAJOR) < 30
		#define SHOW_NYIDE_VER_MINOR1	2
	#elif (SUPPORTED_NYIDE_VER-100*SHOW_NYIDE_VER_MAJOR) < 40
		#define SHOW_NYIDE_VER_MINOR1	3
	#elif (SUPPORTED_NYIDE_VER-100*SHOW_NYIDE_VER_MAJOR) < 50
		#define SHOW_NYIDE_VER_MINOR1	4
	#elif (SUPPORTED_NYIDE_VER-100*SHOW_NYIDE_VER_MAJOR) < 60
		#define SHOW_NYIDE_VER_MINOR1	5
	#elif (SUPPORTED_NYIDE_VER-100*SHOW_NYIDE_VER_MAJOR) < 70
		#define SHOW_NYIDE_VER_MINOR1	6
	#elif (SUPPORTED_NYIDE_VER-100*SHOW_NYIDE_VER_MAJOR) < 80
		#define SHOW_NYIDE_VER_MINOR1	7
	#elif (SUPPORTED_NYIDE_VER-100*SHOW_NYIDE_VER_MAJOR) < 90
		#define SHOW_NYIDE_VER_MINOR1	8
	#elif (SUPPORTED_NYIDE_VER-100*SHOW_NYIDE_VER_MAJOR) < 100
		#define SHOW_NYIDE_VER_MINOR1	9
	#endif

	#if   (SUPPORTED_NYIDE_VER-100*SHOW_NYIDE_VER_MAJOR-10*SHOW_NYIDE_VER_MINOR1) < 1
		#define SHOW_NYIDE_VER_MINOR2	0
	#elif (SUPPORTED_NYIDE_VER-100*SHOW_NYIDE_VER_MAJOR-10*SHOW_NYIDE_VER_MINOR1) < 2
		#define SHOW_NYIDE_VER_MINOR2	1
	#elif (SUPPORTED_NYIDE_VER-100*SHOW_NYIDE_VER_MAJOR-10*SHOW_NYIDE_VER_MINOR1) < 3
		#define SHOW_NYIDE_VER_MINOR2	2
	#elif (SUPPORTED_NYIDE_VER-100*SHOW_NYIDE_VER_MAJOR-10*SHOW_NYIDE_VER_MINOR1) < 4
		#define SHOW_NYIDE_VER_MINOR2	3
	#elif (SUPPORTED_NYIDE_VER-100*SHOW_NYIDE_VER_MAJOR-10*SHOW_NYIDE_VER_MINOR1) < 5
		#define SHOW_NYIDE_VER_MINOR2	4
	#elif (SUPPORTED_NYIDE_VER-100*SHOW_NYIDE_VER_MAJOR-10*SHOW_NYIDE_VER_MINOR1) < 6
		#define SHOW_NYIDE_VER_MINOR2	5
	#elif (SUPPORTED_NYIDE_VER-100*SHOW_NYIDE_VER_MAJOR-10*SHOW_NYIDE_VER_MINOR1) < 7
		#define SHOW_NYIDE_VER_MINOR2	6
	#elif (SUPPORTED_NYIDE_VER-100*SHOW_NYIDE_VER_MAJOR-10*SHOW_NYIDE_VER_MINOR1) < 8
		#define SHOW_NYIDE_VER_MINOR2	7
	#elif (SUPPORTED_NYIDE_VER-100*SHOW_NYIDE_VER_MAJOR-10*SHOW_NYIDE_VER_MINOR1) < 9
		#define SHOW_NYIDE_VER_MINOR2	8
	#elif (SUPPORTED_NYIDE_VER-100*SHOW_NYIDE_VER_MAJOR-10*SHOW_NYIDE_VER_MINOR1) < 10
		#define SHOW_NYIDE_VER_MINOR2	9
	#endif

	#if   SUPPORTED_SPI_ENCODER_VER < 100
		#define SHOW_SPI_ENCODER_VER_MAJOR	0
	#elif SUPPORTED_SPI_ENCODER_VER < 200
		#define SHOW_SPI_ENCODER_VER_MAJOR	1
	#elif SUPPORTED_SPI_ENCODER_VER < 300
		#define SHOW_SPI_ENCODER_VER_MAJOR	2
	#elif SUPPORTED_SPI_ENCODER_VER < 400
		#define SHOW_SPI_ENCODER_VER_MAJOR	3
	#elif SUPPORTED_SPI_ENCODER_VER < 500
		#define SHOW_SPI_ENCODER_VER_MAJOR	4
	#elif SUPPORTED_SPI_ENCODER_VER < 600
		#define SHOW_SPI_ENCODER_VER_MAJOR	5
	#elif SUPPORTED_SPI_ENCODER_VER < 700
		#define SHOW_SPI_ENCODER_VER_MAJOR	6
	#elif SUPPORTED_SPI_ENCODER_VER < 800
		#define SHOW_SPI_ENCODER_VER_MAJOR	7
	#elif SUPPORTED_SPI_ENCODER_VER < 900
		#define SHOW_SPI_ENCODER_VER_MAJOR	8
	#elif SUPPORTED_SPI_ENCODER_VER < 1000
		#define SHOW_SPI_ENCODER_VER_MAJOR	9
	#endif

	#if   (SUPPORTED_SPI_ENCODER_VER-100*SHOW_SPI_ENCODER_VER_MAJOR) < 10
		#define SHOW_SPI_ENCODER_VER_MINOR1	0
	#elif (SUPPORTED_SPI_ENCODER_VER-100*SHOW_SPI_ENCODER_VER_MAJOR) < 20
		#define SHOW_SPI_ENCODER_VER_MINOR1	1
	#elif (SUPPORTED_SPI_ENCODER_VER-100*SHOW_SPI_ENCODER_VER_MAJOR) < 30
		#define SHOW_SPI_ENCODER_VER_MINOR1	2
	#elif (SUPPORTED_SPI_ENCODER_VER-100*SHOW_SPI_ENCODER_VER_MAJOR) < 40
		#define SHOW_SPI_ENCODER_VER_MINOR1	3
	#elif (SUPPORTED_SPI_ENCODER_VER-100*SHOW_SPI_ENCODER_VER_MAJOR) < 50
		#define SHOW_SPI_ENCODER_VER_MINOR1	4
	#elif (SUPPORTED_SPI_ENCODER_VER-100*SHOW_SPI_ENCODER_VER_MAJOR) < 60
		#define SHOW_SPI_ENCODER_VER_MINOR1	5
	#elif (SUPPORTED_SPI_ENCODER_VER-100*SHOW_SPI_ENCODER_VER_MAJOR) < 70
		#define SHOW_SPI_ENCODER_VER_MINOR1	6
	#elif (SUPPORTED_SPI_ENCODER_VER-100*SHOW_SPI_ENCODER_VER_MAJOR) < 80
		#define SHOW_SPI_ENCODER_VER_MINOR1	7
	#elif (SUPPORTED_SPI_ENCODER_VER-100*SHOW_SPI_ENCODER_VER_MAJOR) < 90
		#define SHOW_SPI_ENCODER_VER_MINOR1	8
	#elif (SUPPORTED_SPI_ENCODER_VER-100*SHOW_SPI_ENCODER_VER_MAJOR) < 100
		#define SHOW_SPI_ENCODER_VER_MINOR1	9
	#endif

	#if   (SUPPORTED_SPI_ENCODER_VER-100*SHOW_SPI_ENCODER_VER_MAJOR-10*SHOW_SPI_ENCODER_VER_MINOR1) < 1
		#define SHOW_SPI_ENCODER_VER_MINOR2	0
	#elif (SUPPORTED_SPI_ENCODER_VER-100*SHOW_SPI_ENCODER_VER_MAJOR-10*SHOW_SPI_ENCODER_VER_MINOR1) < 2
		#define SHOW_SPI_ENCODER_VER_MINOR2	1
	#elif (SUPPORTED_SPI_ENCODER_VER-100*SHOW_SPI_ENCODER_VER_MAJOR-10*SHOW_SPI_ENCODER_VER_MINOR1) < 3
		#define SHOW_SPI_ENCODER_VER_MINOR2	2
	#elif (SUPPORTED_SPI_ENCODER_VER-100*SHOW_SPI_ENCODER_VER_MAJOR-10*SHOW_SPI_ENCODER_VER_MINOR1) < 4
		#define SHOW_SPI_ENCODER_VER_MINOR2	3
	#elif (SUPPORTED_SPI_ENCODER_VER-100*SHOW_SPI_ENCODER_VER_MAJOR-10*SHOW_SPI_ENCODER_VER_MINOR1) < 5
		#define SHOW_SPI_ENCODER_VER_MINOR2	4
	#elif (SUPPORTED_SPI_ENCODER_VER-100*SHOW_SPI_ENCODER_VER_MAJOR-10*SHOW_SPI_ENCODER_VER_MINOR1) < 6
		#define SHOW_SPI_ENCODER_VER_MINOR2	5
	#elif (SUPPORTED_SPI_ENCODER_VER-100*SHOW_SPI_ENCODER_VER_MAJOR-10*SHOW_SPI_ENCODER_VER_MINOR1) < 7
		#define SHOW_SPI_ENCODER_VER_MINOR2	6
	#elif (SUPPORTED_SPI_ENCODER_VER-100*SHOW_SPI_ENCODER_VER_MAJOR-10*SHOW_SPI_ENCODER_VER_MINOR1) < 8
		#define SHOW_SPI_ENCODER_VER_MINOR2	7
	#elif (SUPPORTED_SPI_ENCODER_VER-100*SHOW_SPI_ENCODER_VER_MAJOR-10*SHOW_SPI_ENCODER_VER_MINOR1) < 9
		#define SHOW_SPI_ENCODER_VER_MINOR2	8
	#elif (SUPPORTED_SPI_ENCODER_VER-100*SHOW_SPI_ENCODER_VER_MAJOR-10*SHOW_SPI_ENCODER_VER_MINOR1) < 10
		#define SHOW_SPI_ENCODER_VER_MINOR2	9
	#endif
	
	#if (SUPPORTED_NYIDE_VER > NYIDE_VER) && (SUPPORTED_SPI_ENCODER_VER > SPI_ENCODER_VER)
		#pragma message "Please update NYIDE to version " STR(SHOW_NYIDE_VER_MAJOR)"."STR(SHOW_NYIDE_VER_MINOR1)STR(SHOW_NYIDE_VER_MINOR2) " or newer, and update SPI_Encoder to version " STR(SHOW_SPI_ENCODER_VER_MAJOR)"."STR(SHOW_SPI_ENCODER_VER_MINOR1)STR(SHOW_SPI_ENCODER_VER_MINOR2) " or newer."
	#elif SUPPORTED_NYIDE_VER > NYIDE_VER
		#pragma message "Please update NYIDE to version " STR(SHOW_NYIDE_VER_MAJOR)"."STR(SHOW_NYIDE_VER_MINOR1)STR(SHOW_NYIDE_VER_MINOR2) " or newer."
	#elif SUPPORTED_SPI_ENCODER_VER > SPI_ENCODER_VER
		#pragma message "Please update SPI_Encoder to version " STR(SHOW_SPI_ENCODER_VER_MAJOR)"."STR(SHOW_SPI_ENCODER_VER_MINOR1)STR(SHOW_SPI_ENCODER_VER_MINOR2) " or newer."
	#endif
	#error "Tool Version Checking error."
#endif

#ifdef MSG_OLD_UPDATE
	#ifndef MSG_FUNC_UPDATE
		#pragma message "Old tool version. Please update NYIDE to version 3.90 or newer, and update SPI_Encoder to version 1.70 or newer."
		#error "Tool Version Checking error."
	#endif
#endif

