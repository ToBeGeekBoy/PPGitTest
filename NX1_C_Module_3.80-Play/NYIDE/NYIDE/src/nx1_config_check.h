#line	4001	"nx1_config.h"



//====================================================================================
// Config Checking
//====================================================================================
#if _ADPCM_PLAY_SR_FOR_SBC_REC > 24000
	#error "_ADPCM_PLAY_SR_FOR_SBC_REC setting is higher than 24000Hz."
#endif

#if _ADPCM_RECORD && _ADPCM_RECORD_ERASING && (_ADPCM_RECORD_SAMPLE_RATE==SAMPLE_RATE_8000) && (_ADPCM_SECTOR_ERASE_MAXTIME > 700)
	#error "_ADPCM_SECTOR_ERASE_MAXTIME must be shorter than or equal to 700msec when setting _ADPCM_RECORD_SAMPLE_RATE as SAMPLE_RATE_8000."
#elif _ADPCM_RECORD && _ADPCM_RECORD_ERASING && (_ADPCM_RECORD_SAMPLE_RATE==SAMPLE_RATE_10000) && (_ADPCM_SECTOR_ERASE_MAXTIME > 560)
	#error "_ADPCM_SECTOR_ERASE_MAXTIME must be shorter than or equal to 560msec when setting _ADPCM_RECORD_SAMPLE_RATE as SAMPLE_RATE_10000."
#elif _ADPCM_RECORD && _ADPCM_RECORD_ERASING && (_ADPCM_RECORD_SAMPLE_RATE==SAMPLE_RATE_12000) && (_ADPCM_SECTOR_ERASE_MAXTIME > 470)
	#error "_ADPCM_SECTOR_ERASE_MAXTIME must be shorter than or equal to 470msec when setting _ADPCM_RECORD_SAMPLE_RATE as SAMPLE_RATE_12000."
#elif _ADPCM_RECORD && _ADPCM_RECORD_ERASING && (_ADPCM_RECORD_SAMPLE_RATE==SAMPLE_RATE_16000) && (_ADPCM_SECTOR_ERASE_MAXTIME > 350)
	#error "_ADPCM_SECTOR_ERASE_MAXTIME must be shorter than or equal to 350msec when setting _ADPCM_RECORD_SAMPLE_RATE as SAMPLE_RATE_16000."
#endif

#if (_ADPCM_ANIMAL_ROAR && ( _ADPCM_SPEED_CHANGE \
								|| _ADPCM_PITCH_CHANGE \
								|| _ADPCM_SPEEDPITCH_N \
								|| _ADPCM_ROBOT1_SOUND \
								|| _ADPCM_ROBOT2_SOUND \
								|| _ADPCM_ROBOT3_SOUND \
								|| _ADPCM_ROBOT4_SOUND \
								|| _ADPCM_REVERB_SOUND \
								|| _ADPCM_ECHO_SOUND \
								|| _ADPCM_DARTH_SOUND \
								|| _ADPCM_PLAY_REPEAT \
								|| _ADPCM_PLAY_SR_FOR_SBC_REC))
	#error "_ADPCM_ANIMAL_ROAR doesn't support any other sound effects."
#endif

#if	_ADPCM_QIO_MODULE
	#if _ADPCM_SPEED_CHANGE || _ADPCM_PITCH_CHANGE || _ADPCM_DARTH_SOUND || _ADPCM_ANIMAL_ROAR
	#error	"QIO does not support _ADPCM_SPEED_CHANGE, _ADPCM_PITCH_CHANGE, _ADPCM_DARTH_SOUND, _ADPCM_ANIMAL_ROAR"
	#endif
#endif

#if _ADPCM_SW_UPSAMPLE 
	#if((_IC_BODY!=0x11F20)&&(_IC_BODY!=0x11F21)&&(_IC_BODY!=0x11F22)&&(_IC_BODY!=0x11F23))
		#error "_ADPCM_SW_UPSAMPLE is only supported for NX11FS2x series."
	#endif
	#if(_ADPCM_PLAY && ( _ADPCM_SPEED_CHANGE \
						|| _ADPCM_PITCH_CHANGE \
						|| _ADPCM_DARTH_SOUND \
						|| _ADPCM_ECHO_SOUND \
						|| _ADPCM_ROBOT2_SOUND \
						|| _ADPCM_ROBOT3_SOUND \
						|| _ADPCM_ROBOT4_SOUND \
						|| _ADPCM_REVERB_SOUND \
						|| _ADPCM_PLAY_SR_FOR_SBC_REC \
						|| _ADPCM_SPEEDPITCH_N \
						|| _ADPCM_ANIMAL_ROAR \
						|| _ADPCM_QIO_MODULE))
		#error "_ADPCM_SW_UPSAMPLE doesn't support the selected sound effect or QIO."
	#endif
	#if (_ADPCM_DECODE_FLOW_TYPE==TYPE1)
		#error "_ADPCM_SW_UPSAMPLE doesn't support _ADPCM_DECODE_FLOW_TYPE TYPE1, please change to _ADPCM_DECODE_FLOW_TYPE TYPE0."
	#endif
#endif

#if _SBC_RECORD && _SBC_RECORD_ERASING && (_SBC_SECTOR_ERASE_MAXTIME > 700)	// 1 sector can support to 1800msec up, but SPI Flash sector erase time max spec is only 700~750msec.
	#error "_SBC_SECTOR_ERASE_MAXTIME must be shorter than or equal to 700msec."
#endif

#if ((_ADPCM_RECORD && _ADPCM_RECORD_ERASING && _ADPCM_RECORD_SOUND_TRIG) && _SOUND_DETECTION)
	#error " The sound detect function can only enable one from _ADPCM_RECORD_SOUND_TRIG or _SOUND_DETECTION"
#endif

#if _VR_PLATFORM==32
	#error	"VR 32bit platform is not supported right now."
#endif

#if _VR_PLATFORM==32 && (_VR_SAMPLE_RATE!=16000 || _VR_FRAME_RATE!=100)
	#error	"VR 32 bit platform only supports the settings _VR_SAMPLE_RATE=16000 and _VR_FRAME_RATE=100."
#endif

#if _VR_FUNC && _VR_VOICE_TAG && _VR_ENERGY_THRESHOLD
	#error "Voice Tag and Voice Password don't support CSMT Energy Threshold, please modify CSMT Energy Threshold setting as 0."
#endif

//====================================================================================
// Config Checking For IC Series
//====================================================================================
#if _EF_SERIES
	#if _GPIOA_INT
		#error "This IC Body doesn't support GPIOA INT."
	#endif
	#if _GPIOB_INT
		#error "This IC Body doesn't support GPIOB INT."
	#endif
	#if _GPIOC_INT
		#error "This IC Body doesn't support GPIOC INT."
	#endif
	
	//XIP
	#if ((_XIP_IO_MODE==XIP_4_IO_MODE) || (_XIP_IO_MODE==XIP_4_IO_MODE_114)) && (_SPI_ACCESS_MODE!=SPI_1_4_4_MODE)
		#error	"When _XIP_IO_MODE is set as XIP_4_IO_MODE or XIP_4_IO_MODE_114, first set _SPI_ACCESS_MODE as SPI_1_4_4_MODE."
	#endif
	#if (_XIP_IO_MODE==XIP_1_IO_MODE_3WIRE) && (_SPI_ACCESS_MODE!=SPI_1_1_1_MODE_3WIRE)
		#error	"When _XIP_IO_MODE is set as XIP_1_IO_MODE_3WIRE, first set _SPI_ACCESS_MODE as SPI_1_1_1_MODE_3WIRE."
	#endif
	#if ((_XIP_IO_MODE==XIP_1_IO_MODE) || (_XIP_IO_MODE==XIP_2_IO_MODE) || (_XIP_IO_MODE==XIP_2_IO_MODE_112)) && ((_SPI_ACCESS_MODE==SPI_1_4_4_MODE) || (_SPI_ACCESS_MODE==SPI_1_1_1_MODE_3WIRE))
		#error	"When _XIP_IO_MODE is set as XIP_1_IO_MODE, XIP_2_IO_MODE or XIP_2_IO_MODE_112, first set _SPI_ACCESS_MODE as SPI_1_1_1_MODE, SPI_1_2_2_MODE or SPI_1_1_2_MODE."
	#endif

	//VR
	#if ((_VR_FUNC==ENABLE) && (_VR_VOICE_TAG==ENABLE) && (_VR_STORAGE_TYPE==0))
		#error "VR Voice Tag does not support EF Mode right now."
	#endif

	//DAC UP SAMPLE POINT
	#if ((_DAC_UP_SAMPLE_POINT>32) || (_DAC_UP_SAMPLE_POINT<4) || (_DAC_UP_SAMPLE_POINT&(_DAC_UP_SAMPLE_POINT-1) != 0))
		#error "_DAC_UP_SAMPLE_POINT must be set as 4/8/16/32 points."
	#endif
    
#else 
	#if _DIRECTKEY_PD0
		#error "This IC Body doesn't support DirectKey PD0."
	#endif
	#if _DIRECTKEY_PD1
		#error "This IC Body doesn't support DirectKey PD1."
	#endif
	#if (_GPIO_PD0_MODE != GPIO_PIN_OFF)
		#error "This IC Body doesn't support GPIO PD0."
	#endif
	#if (_GPIO_PD1_MODE != GPIO_PIN_OFF)
		#error "This IC Body doesn't support GPIO PD1."
	#endif
	#if _TIMER_TMR3_INT
		#error "This IC Body doesn't support Timer3 INT."
	#endif
	#if _EXIT0_INT
		#error "This IC Body doesn't support EXIT0 INT."
	#endif
	#if _EXIT1_INT
		#error "This IC Body doesn't support EXIT1 INT."
	#endif
	
	#if (_UART_PORT==UART_PORT_PD01)
		#error "This IC Body doesn't support UART from PD0/PD1."
	#endif

	//XIP
	#if (_XIP_IO_MODE==XIP_4_IO_MODE) && (_SPI_ACCESS_MODE!=SPI_1_4_4_MODE)
		#error	"When _XIP_IO_MODE is set as XIP_4_IO_MODE, first set _SPI_ACCESS_MODE as SPI_1_4_4_MODE."
	#endif
	#if ((_XIP_IO_MODE==XIP_1_IO_MODE) || (_XIP_IO_MODE==XIP_2_IO_MODE)) && (_SPI_ACCESS_MODE==SPI_1_4_4_MODE)
		#error	"When _XIP_IO_MODE is set as XIP_1_IO_MODE or XIP_2_IO_MODE, first set _SPI_ACCESS_MODE as SPI_1_1_1_MODE, SPI_1_2_2_MODE or SPI_1_1_2_MODE."
	#endif
	#if (_XIP_IO_MODE==XIP_1_IO_MODE_3WIRE) || (_XIP_IO_MODE==XIP_2_IO_MODE_112) || (_XIP_IO_MODE==XIP_4_IO_MODE_114)
		#error "This IC Body doesn't support this _XIP_IO_MODE setting."
	#endif
	
    //Low power IR
    #if (_IR_MODULE && _IR_LOW_POWER_TYPE==TYPE2 && _IR_RX_EN)
        #error "This IC Body doesn't support TYPE2 on low power IR application."    
    #endif
#endif

//====================================================================================
// Config Checking For IC Body
//====================================================================================
#if	(_IC_BODY==0x12F51)||(_IC_BODY==0x12F61)
	#if (_IS_APPS_USE_ADC_MIC) \
	 && ((_DIRECTKEY_MODULE && (_DIRECTKEY_PC0 || _DIRECTKEY_PC1 || _DIRECTKEY_PC2 || _DIRECTKEY_PC3)) \
	 || (_GPIO_MODULE && ((_GPIO_PC0_MODE != GPIO_PIN_OFF) || (_GPIO_PC1_MODE != GPIO_PIN_OFF) || (_GPIO_PC2_MODE != GPIO_PIN_OFF) || (_GPIO_PC3_MODE != GPIO_PIN_OFF))))
		#error "Microphone applications and GPIO Port C settings are exclusive each other on NX12FS51/NX13FS51."
	#endif
	
	#if ((_DIRECTKEY_MODULE && (_DIRECTKEY_PA9 \
							|| _DIRECTKEY_PA10 \
							|| _DIRECTKEY_PA11 \
							|| _DIRECTKEY_PB6 \
							|| _DIRECTKEY_PB7 \
							|| _DIRECTKEY_PB8 \
							|| _DIRECTKEY_PB9 \
							|| _DIRECTKEY_PB10 \
							|| _DIRECTKEY_PB11 \
							|| _DIRECTKEY_PB12 \
							|| _DIRECTKEY_PB13 \
							|| _DIRECTKEY_PB14 \
							|| _DIRECTKEY_PB15 \
							|| _DIRECTKEY_PC4 \
							|| _DIRECTKEY_PC5 \
							|| _DIRECTKEY_PC6 \
							|| _DIRECTKEY_PC7)) \
	 || (_GPIO_MODULE && ((_GPIO_PA9_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PA10_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PA11_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB6_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB7_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB8_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB9_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB10_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB11_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB12_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB13_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB14_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB15_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC4_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC5_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC6_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC7_MODE != GPIO_PIN_OFF))))
		#error "Setting Error on pins that cannot be used by DirectKey Module or GPIO Module. (PA9~PA11, PB6~PB15, PC4~PC7)"
	#endif
    
    //Low power IR
    #if (_IR_MODULE && _IR_RX_EN)
        #if     (_IR_LOW_POWER_TYPE==TYPE1 && _IR_RX_TIMER!=USE_RTC)
            #error "This IC Body doesn't support timer as source on low power IR application."
        #elif   _IR_LOW_POWER_TYPE==TYPE2
            #error "This IC Body doesn't support TYPE2 on low power IR application."  
        #endif        
    #endif

#elif ((_IC_BODY==0x12FB0052)||(_IC_BODY==0x12FB0054)||(_IC_BODY==0x12FB0062)||(_IC_BODY==0x12FB0064))
	#if (_IS_APPS_USE_ADC_MIC) \
	 && ((_DIRECTKEY_MODULE && (_DIRECTKEY_PC0 || _DIRECTKEY_PC1 || _DIRECTKEY_PC2 || _DIRECTKEY_PC3)) \
	 || (_GPIO_MODULE && ((_GPIO_PC0_MODE != GPIO_PIN_OFF) || (_GPIO_PC1_MODE != GPIO_PIN_OFF) || (_GPIO_PC2_MODE != GPIO_PIN_OFF) || (_GPIO_PC3_MODE != GPIO_PIN_OFF))))
		#error "Microphone applications and GPIO Port C settings are exclusive each other on NX12FM5X/NX13FM5X/NX12FM6X/NX13FM6X."
	#endif

	#if (_SPI_MODULE && ((_SPI_ACCESS_MODE==SPI_1_4_4_MODE) || (_SPI_ACCESS_MODE==SPI_1_1_1_MODE_3WIRE)))
		#error "NX12FM5X/NX12FM6X/NX13FM5X/NX13FM6X don't support SPI_1_4_4_MODE and SPI_1_1_1_MODE_3WIRE."
	#endif

	#if ((_DIRECTKEY_MODULE && (_DIRECTKEY_PA9 \
							|| _DIRECTKEY_PA10 \
							|| _DIRECTKEY_PA11 \
							|| _DIRECTKEY_PB0 \
							|| _DIRECTKEY_PB1 \
							|| _DIRECTKEY_PB2 \
							|| _DIRECTKEY_PB3 \
							|| _DIRECTKEY_PB6 \
							|| _DIRECTKEY_PB7 \
							|| _DIRECTKEY_PB8 \
							|| _DIRECTKEY_PB9 \
							|| _DIRECTKEY_PB10 \
							|| _DIRECTKEY_PB11 \
							|| _DIRECTKEY_PB12 \
							|| _DIRECTKEY_PB13 \
							|| _DIRECTKEY_PB14 \
							|| _DIRECTKEY_PB15 \
							|| _DIRECTKEY_PC4 \
							|| _DIRECTKEY_PC5 \
							|| _DIRECTKEY_PC6 \
							|| _DIRECTKEY_PC7)) \
	 || (_GPIO_MODULE && ((_GPIO_PA9_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PA10_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PA11_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB0_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB1_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB2_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB3_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB6_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB7_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB8_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB9_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB10_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB11_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB12_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB13_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB14_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB15_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC4_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC5_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC6_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC7_MODE != GPIO_PIN_OFF))))
		#error "Setting Error on pins that cannot be used by DirectKey Module or GPIO Module. (PA9~PA11, PB0~PB3, PB6~PB15, PC4~PC7)"
	#endif
    
    //Low power IR
    #if (_IR_MODULE && _IR_RX_EN)
        #if     (_IR_LOW_POWER_TYPE==TYPE1 && _IR_RX_TIMER!=USE_RTC)
            #error "This IC Body doesn't support timer as source on low power IR application."
        #elif   _IR_LOW_POWER_TYPE==TYPE2
            #error "This IC Body doesn't support TYPE2 on low power IR application."  
        #endif        
    #endif

#elif (_IC_BODY==0x11F20)||(_IC_BODY==0x11F21)
	#if (	((_AUDIO_RECORD == ENABLE) && (_AUDIO_RECORD_AUDIO_CH == HW_CH1)) \
		||	((_SBC_PLAY == ENABLE) && (_SBC_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_SBC_CH2_PLAY == ENABLE) && (_SBC_CH2_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_SBC2_PLAY == ENABLE) && (_SBC2_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_SBC2_CH2_PLAY == ENABLE) && (_SBC2_CH2_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_ADPCM_PLAY == ENABLE) && (_ADPCM_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_ADPCM_CH2_PLAY == ENABLE) && (_ADPCM_CH2_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_ADPCM_CH3_PLAY == ENABLE) && (_ADPCM_CH3_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_ADPCM_CH4_PLAY == ENABLE) && (_ADPCM_CH4_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_ADPCM_CH5_PLAY == ENABLE) && (_ADPCM_CH5_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_ADPCM_CH6_PLAY == ENABLE) && (_ADPCM_CH6_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_ADPCM_CH7_PLAY == ENABLE) && (_ADPCM_CH7_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_ADPCM_CH8_PLAY == ENABLE) && (_ADPCM_CH8_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_CELP_PLAY == ENABLE) && (_CELP_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_PCM_PLAY == ENABLE) && (_PCM_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_PCM_CH2_PLAY == ENABLE) && (_PCM_CH2_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_PCM_CH3_PLAY == ENABLE) && (_PCM_CH3_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_RT_PLAY == ENABLE) && (_RT_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_MIDI_PLAY == ENABLE) && (_MIDI_PLAY_AUDIO_CH == HW_CH1)))
		#error "NX11FS20 and NX11FS21 only support HW channel 0."
	#endif

	#if (_DAC_MODE == ENABLE)
		#error "NX11FS20 and NX11FS21 only support PWM output mode."
	#endif

	#if ((_DIRECTKEY_MODULE && (_DIRECTKEY_PA0 \
							|| _DIRECTKEY_PA1 \
							|| _DIRECTKEY_PA4 \
							|| _DIRECTKEY_PA5 \
							|| _DIRECTKEY_PA6 \
							|| _DIRECTKEY_PA7 \
							|| _DIRECTKEY_PA9 \
							|| _DIRECTKEY_PA10 \
							|| _DIRECTKEY_PA11 \
							|| _DIRECTKEY_PA15 \
							|| _DIRECTKEY_PB4 \
							|| _DIRECTKEY_PB5 \
							|| _DIRECTKEY_PB6 \
							|| _DIRECTKEY_PB7 \
							|| _DIRECTKEY_PB8 \
							|| _DIRECTKEY_PB9 \
							|| _DIRECTKEY_PB10 \
							|| _DIRECTKEY_PB11 \
							|| _DIRECTKEY_PB12 \
							|| _DIRECTKEY_PB13 \
							|| _DIRECTKEY_PB14 \
							|| _DIRECTKEY_PB15 \
							|| _DIRECTKEY_PC0 \
							|| _DIRECTKEY_PC1 \
							|| _DIRECTKEY_PC2 \
							|| _DIRECTKEY_PC3 \
							|| _DIRECTKEY_PC4 \
							|| _DIRECTKEY_PC5 \
							|| _DIRECTKEY_PC6 \
							|| _DIRECTKEY_PC7)) \
	 || (_GPIO_MODULE && ((_GPIO_PA0_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PA1_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PA4_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PA5_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PA6_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PA7_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PA9_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PA10_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PA11_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PA15_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB4_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB5_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB6_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB7_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB8_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB9_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB10_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB11_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB12_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB13_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB14_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB15_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC0_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC1_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC2_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC3_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC4_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC5_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC6_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC7_MODE != GPIO_PIN_OFF))))
		#error "Setting Error on pins that cannot be used by DirectKey Module or GPIO Module. (PA0~PA1, PA4~PA7, PA9~PA11, PA15, PB4~PB15, PC0~PC7)"
	#endif

	#if (_SPI_MODULE && (_SPI_ACCESS_MODE==SPI_1_4_4_MODE))
		#error "NX11FS20 and NX11FS21 don't support SPI_1_4_4_MODE."
	#endif

	#if _PWMB_MODULE
		#error "NX11FS20 and NX11FS21 don't support PWMB Module."
	#endif

#elif ((_IC_BODY==0x11F22)||(_IC_BODY==0x11F23))
	#if (	((_AUDIO_RECORD == ENABLE) && (_AUDIO_RECORD_AUDIO_CH == HW_CH1)) \
		||	((_SBC_PLAY == ENABLE) && (_SBC_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_SBC_CH2_PLAY == ENABLE) && (_SBC_CH2_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_SBC2_PLAY == ENABLE) && (_SBC2_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_SBC2_CH2_PLAY == ENABLE) && (_SBC2_CH2_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_ADPCM_PLAY == ENABLE) && (_ADPCM_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_ADPCM_CH2_PLAY == ENABLE) && (_ADPCM_CH2_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_ADPCM_CH3_PLAY == ENABLE) && (_ADPCM_CH3_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_ADPCM_CH4_PLAY == ENABLE) && (_ADPCM_CH4_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_ADPCM_CH5_PLAY == ENABLE) && (_ADPCM_CH5_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_ADPCM_CH6_PLAY == ENABLE) && (_ADPCM_CH6_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_ADPCM_CH7_PLAY == ENABLE) && (_ADPCM_CH7_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_ADPCM_CH8_PLAY == ENABLE) && (_ADPCM_CH8_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_CELP_PLAY == ENABLE) && (_CELP_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_PCM_PLAY == ENABLE) && (_PCM_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_PCM_CH2_PLAY == ENABLE) && (_PCM_CH2_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_PCM_CH3_PLAY == ENABLE) && (_PCM_CH3_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_RT_PLAY == ENABLE) && (_RT_PLAY_AUDIO_CH == HW_CH1)) \
		||	((_MIDI_PLAY == ENABLE) && (_MIDI_PLAY_AUDIO_CH == HW_CH1)))
		#error "NX11FS22 and NX11FS23 only support HW channel 0."
	#endif

	#if ((_IC_BODY==0x11F22)&&(_DAC_MODE == ENABLE))
		#error "NX11FS22 only supports PWM output mode."
	#endif

	#if ((_DIRECTKEY_MODULE && (_DIRECTKEY_PA0 \
							|| _DIRECTKEY_PA9 \
							|| _DIRECTKEY_PA10 \
							|| _DIRECTKEY_PA11 \
							|| _DIRECTKEY_PB6 \
							|| _DIRECTKEY_PB7 \
							|| _DIRECTKEY_PB8 \
							|| _DIRECTKEY_PB9 \
							|| _DIRECTKEY_PB10 \
							|| _DIRECTKEY_PB11 \
							|| _DIRECTKEY_PB12 \
							|| _DIRECTKEY_PB13 \
							|| _DIRECTKEY_PB14 \
							|| _DIRECTKEY_PB15 \
							|| _DIRECTKEY_PC0 \
							|| _DIRECTKEY_PC1 \
							|| _DIRECTKEY_PC2 \
							|| _DIRECTKEY_PC3 \
							|| _DIRECTKEY_PC4 \
							|| _DIRECTKEY_PC5 \
							|| _DIRECTKEY_PC6 \
							|| _DIRECTKEY_PC7)) \
	 || (_GPIO_MODULE && ((_GPIO_PA0_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PA9_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PA10_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PA11_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB6_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB7_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB8_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB9_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB10_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB11_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB12_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB13_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB14_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB15_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC0_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC1_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC2_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC3_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC4_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC5_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC6_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC7_MODE != GPIO_PIN_OFF))))
		#error "Setting Error on pins that cannot be used by DirectKey Module or GPIO Module. (PA0, PA9~PA11, PB6~PB15, PC0~PC7)"
	#endif
    
    //Low power IR
    #if (_IC_BODY==0x11F23 && _IR_MODULE && _IR_RX_EN)
        #if     (_IR_LOW_POWER_TYPE==TYPE1 && _IR_RX_TIMER!=USE_RTC)
            #error "This IC Body doesn't support timer as source on low power IR application."
        #elif   _IR_LOW_POWER_TYPE==TYPE2
            #error "This IC Body doesn't support TYPE2 on low power IR application."  
        #endif        
    #endif    

#elif (_IC_BODY==0x12A64)
	#if ((_DIRECTKEY_MODULE && (_DIRECTKEY_PC0 \
							|| _DIRECTKEY_PC1 \
							|| _DIRECTKEY_PC2 \
							|| _DIRECTKEY_PC3 \
							|| _DIRECTKEY_PC4 \
							|| _DIRECTKEY_PC5 \
							|| _DIRECTKEY_PC6 \
							|| _DIRECTKEY_PC7)) \
	 || (_GPIO_MODULE && ((_GPIO_PC0_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC1_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC2_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC3_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC4_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC5_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC6_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC7_MODE != GPIO_PIN_OFF))))
		#error "Setting Error on pins that cannot be used by DirectKey Module or GPIO Module. (PC0~PC7)"
	#endif

#elif ((_IC_BODY==0x12B52)||(_IC_BODY==0x12B53)||(_IC_BODY==0x12B54)||(_IC_BODY==0x12B55))
	#if ((_DIRECTKEY_MODULE && (_DIRECTKEY_PA0 \
							|| _DIRECTKEY_PA1 \
							|| _DIRECTKEY_PB8 \
							|| _DIRECTKEY_PB9 \
							|| _DIRECTKEY_PB10 \
							|| _DIRECTKEY_PB11 \
							|| _DIRECTKEY_PB12 \
							|| _DIRECTKEY_PB13 \
							|| _DIRECTKEY_PB14 \
							|| _DIRECTKEY_PB15 \
							|| _DIRECTKEY_PC0 \
							|| _DIRECTKEY_PC1 \
							|| _DIRECTKEY_PC2 \
							|| _DIRECTKEY_PC3 \
							|| _DIRECTKEY_PC4 \
							|| _DIRECTKEY_PC5 \
							|| _DIRECTKEY_PC6 \
							|| _DIRECTKEY_PC7)) \
	 || (_GPIO_MODULE && ((_GPIO_PA0_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PA1_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB8_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB9_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB10_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB11_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB12_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB13_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB14_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB15_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC0_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC1_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC2_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC3_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC4_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC5_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC6_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC7_MODE != GPIO_PIN_OFF))))
		#error "Setting Error on pins that cannot be used by DirectKey Module or GPIO Module. (PA0~PA1, PB8~PB15, PC0~PC7)"
	#endif

	#if _SL_MODULE
		#error "_SL_MODULE can not support ADC scan without PA1."	
	#endif

#elif (_IC_BODY==0x12A44)
	#if ((_DIRECTKEY_MODULE && (_DIRECTKEY_PC0 \
							|| _DIRECTKEY_PC1 \
							|| _DIRECTKEY_PC2 \
							|| _DIRECTKEY_PC3 \
							|| _DIRECTKEY_PC4 \
							|| _DIRECTKEY_PC5 \
							|| _DIRECTKEY_PC6 \
							|| _DIRECTKEY_PC7)) \
	 || (_GPIO_MODULE && ((_GPIO_PC0_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC1_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC2_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC3_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC4_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC5_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC6_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC7_MODE != GPIO_PIN_OFF))))
		#error "Setting Error on pins that cannot be used by DirectKey Module or GPIO Module. (PC0~PC7)"
	#endif

#elif (_IC_BODY==0x11A22)
	#if ((_DIRECTKEY_MODULE && (_DIRECTKEY_PB8 \
							|| _DIRECTKEY_PB9 \
							|| _DIRECTKEY_PB10 \
							|| _DIRECTKEY_PB11 \
							|| _DIRECTKEY_PB12 \
							|| _DIRECTKEY_PB13 \
							|| _DIRECTKEY_PB14 \
							|| _DIRECTKEY_PB15 \
							|| _DIRECTKEY_PC0 \
							|| _DIRECTKEY_PC1 \
							|| _DIRECTKEY_PC2 \
							|| _DIRECTKEY_PC3 \
							|| _DIRECTKEY_PC4 \
							|| _DIRECTKEY_PC5 \
							|| _DIRECTKEY_PC6 \
							|| _DIRECTKEY_PC7)) \
	 || (_GPIO_MODULE && ((_GPIO_PB8_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB9_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB10_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB11_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB12_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB13_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB14_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB15_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC0_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC1_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC2_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC3_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC4_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC5_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC6_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC7_MODE != GPIO_PIN_OFF))))
		#error "Setting Error on pins that cannot be used by DirectKey Module or GPIO Module. (PB8~PB15, PC0~PC7)"
	#endif

#elif (_IC_BODY==0x11A21)
	#if (_SPI_MODULE && (_SPI_ACCESS_MODE==SPI_1_4_4_MODE))
		#error "NX11P21A doesn't support SPI_1_4_4_MODE."
	#endif

	#if ((_DIRECTKEY_MODULE && (_DIRECTKEY_PA12 \
							|| _DIRECTKEY_PA13 \
							|| _DIRECTKEY_PB4 \
							|| _DIRECTKEY_PB5 \
							|| _DIRECTKEY_PB6 \
							|| _DIRECTKEY_PB7 \
							|| _DIRECTKEY_PB8 \
							|| _DIRECTKEY_PB9 \
							|| _DIRECTKEY_PB10 \
							|| _DIRECTKEY_PB11 \
							|| _DIRECTKEY_PB12 \
							|| _DIRECTKEY_PB13 \
							|| _DIRECTKEY_PB14 \
							|| _DIRECTKEY_PB15 \
							|| _DIRECTKEY_PC0 \
							|| _DIRECTKEY_PC1 \
							|| _DIRECTKEY_PC2 \
							|| _DIRECTKEY_PC3 \
							|| _DIRECTKEY_PC4 \
							|| _DIRECTKEY_PC5 \
							|| _DIRECTKEY_PC6 \
							|| _DIRECTKEY_PC7)) \
	 || (_GPIO_MODULE && ((_GPIO_PA12_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PA13_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB4_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB5_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB6_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB7_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB8_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB9_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB10_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB11_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB12_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB13_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB14_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB15_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC0_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC1_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC2_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC3_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC4_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC5_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC6_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC7_MODE != GPIO_PIN_OFF))))
		#error "Setting Error on pins that cannot be used by DirectKey Module or GPIO Module. (PA12~PA13, PB4~PB15, PC0~PC7)"
	#endif

#elif ((_IC_BODY==0x11B25)||(_IC_BODY==0x11B24)||(_IC_BODY==0x11B23)||(_IC_BODY==0x11B22)||(_IC_BODY==0x11C22)||(_IC_BODY==0x11C21))
	#if (_SPI_MODULE && (_SPI_ACCESS_MODE==SPI_1_4_4_MODE))
		#error "NX11M2XA/NX11S2XA doesn't support SPI_1_4_4_MODE."
	#endif
	
	#if ((_DIRECTKEY_MODULE && (_DIRECTKEY_PA12 \
							|| _DIRECTKEY_PA13 \
							|| _DIRECTKEY_PA14 \
							|| _DIRECTKEY_PA15 \
							|| _DIRECTKEY_PB6 \
							|| _DIRECTKEY_PB7 \
							|| _DIRECTKEY_PB8 \
							|| _DIRECTKEY_PB9 \
							|| _DIRECTKEY_PB10 \
							|| _DIRECTKEY_PB11 \
							|| _DIRECTKEY_PB12 \
							|| _DIRECTKEY_PB13 \
							|| _DIRECTKEY_PB14 \
							|| _DIRECTKEY_PB15 \
							|| _DIRECTKEY_PC0 \
							|| _DIRECTKEY_PC1 \
							|| _DIRECTKEY_PC2 \
							|| _DIRECTKEY_PC3 \
							|| _DIRECTKEY_PC4 \
							|| _DIRECTKEY_PC5 \
							|| _DIRECTKEY_PC6 \
							|| _DIRECTKEY_PC7)) \
	 || (_GPIO_MODULE && ((_GPIO_PA12_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PA13_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PA14_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PA15_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB6_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB7_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB8_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB9_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB10_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB11_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB12_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB13_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB14_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PB15_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC0_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC1_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC2_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC3_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC4_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC5_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC6_MODE != GPIO_PIN_OFF) \
						|| (_GPIO_PC7_MODE != GPIO_PIN_OFF))))
		#error "Setting Error on pins that cannot be used by DirectKey Module or GPIO Module. (PA12~PA15, PB6~PB15, PC0~PC7)"
	#endif

	//Low power IR
    #if (_IR_MODULE && _IR_LOW_POWER_TYPE && _IR_RX_EN)
        #error "This IC Body doesn't support low power IR application."    
    #endif

#endif
#if (_IC_BODY==0x11C22)
	#if _STORAGE_MODULE
		#error "NX11S2XA doesn't support _STORAGE_MODULE."
	#endif
#endif