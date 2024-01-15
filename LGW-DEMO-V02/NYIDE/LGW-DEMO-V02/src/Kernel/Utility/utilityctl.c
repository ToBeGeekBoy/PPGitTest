/* Includes ------------------------------------------------------------------*/
#include "include.h"
#include "nx1_lib.h"
#include "nx1_i2c.h"

//------------------------------------------------------------------//
// Utility service loop
// Parameter: 
//          NONE
// return value:
//          NONE
//------------------------------------------------------------------//
void UTILITY_ServiceLoop(void)
{
#if _AUDIO_SENTENCE
	Sentence_ServiceLoop();
#endif

#if _ACTION_MODULE
	ACTION_ServiceLoop();
#endif

#if	_QFID_MODULE
	QFID_ServiceLoop();
#endif

#if _TOUCHKEY_MODULE
	TOUCH_ServiceLoop();
#endif

#if _LEDSTR_MODULE
    LEDSTR_ServiceLoop();
#endif  

#if _LEDSTRn_MODULE
    LEDSTRn_ServiceLoop();
#endif  

#if _LEDSTRm_MODULE
    LEDSTRm_ServiceLoop();
#endif  

#if _QIO_MODULE
	QIO_ServiceLoop();
#endif//#if	_QIO_MODULE

#if _PD_PLAY
	PD_ServiceLoop();
#endif

#if _SOUND_DETECTION
	SOUND_DETECTION_ServiceLoop();
#endif

#if _SYS_LVD_MONITOR_MODULE
    LVD_Monitor_ServiceLoop();
#endif

#if _QCODE_BUILD==0
#if _I2C_MODULE && (_I2C_DEVICE_MODE == I2C_SLAVE)
    I2C_SlaveServiceLoop();
#endif
#endif

#if _QCODE_BUILD
    QC_ServiceLoop();
#endif

}
