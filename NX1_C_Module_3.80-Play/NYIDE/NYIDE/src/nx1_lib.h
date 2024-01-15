//==========================================================================
// File Name          : nx1_lib.h
// Description        : NX1 library
// Last modified date : 2018/08/31
//==========================================================================
#ifndef NX1_LIB_H
#define NX1_LIB_H


//====================================================================================
//  Type Define
//====================================================================================
typedef unsigned char       U8,u8;
typedef unsigned short      U16,u16;
typedef unsigned int        U32,u32;
typedef unsigned long long  U64,u64;
typedef signed char         S8,s8;
typedef signed short        S16,s16;
typedef signed int          S32,s32;
typedef signed long long    S64,s64;
typedef unsigned char       bool;
typedef enum {
    R_OK = 0
} RESULT;

typedef struct OKONParameter{
	U8 trig_mode;
	U8 patch;
	U8 pitch;
	U8 velocity;
}OKONParameter;
typedef struct TeachInput{
    U8	Reset;
	U8	NextNote;
	U8  MStart;
	U8  MStop;
}TeachInput;

typedef struct TeachOutput{
    U8	Instrument;
	U8	Note;
	U8	NextFlag;
	U8  NextGroup;

}TeachOutput;

typedef struct{
    U32 *pVar_table; // (len | address)
    U32 cur_addr;    // Storage current address
    U32 start_addr;  // Storage start address
    U32 end_addr;    // Storage end address
    U32 block_cnt;
    U16 erase_size;  // Storage erase size
    U16 var_cnt;
    U16 block_size;  // total addr+data size
}STORAGE_T;

typedef struct{
    U32 erased_addr;
    U8 state;
}STORAGE_NONBLOCK_ERASE_T;

//====================================================================================
//  Constant Define
//====================================================================================
// Time base
#define TIMEBASE_OFF                0
#define	TIMEBASE_62us				62
#define TIMEBASE_250us              250
#define TIMEBASE_1000us             1000

// WDT timeout
#define WDT_TIMEOUT_750ms           0
#define WDT_TIMEOUT_188ms           1

// CPU clock divider
#define CPU_CLK_DIV_1               (7<<5)
#define CPU_CLK_DIV_2               (6<<5)
#define CPU_CLK_DIV_4               (5<<5)
#define CPU_CLK_DIV_8               (4<<5)

// LVD voltage
#define LVD_VOLTAGE_OFF             0xFF
#define LVD_VOLTAGE_2V0             1
#define LVD_VOLTAGE_2V2             2
#define LVD_VOLTAGE_2V4             3
#define LVD_VOLTAGE_2V6             4
#define LVD_VOLTAGE_2V8             5
#define LVD_VOLTAGE_3V0             6
#define LVD_VOLTAGE_3V2             7
#define LVD_VOLTAGE_3V4             8
#define LVD_VOLTAGE_3V6             9

// HW channel ID
#define HW_CH0                      0
#define HW_CH1                      1

// AUDIO channel ID
#define AUDIO_CH0                   0
#define AUDIO_CH1                   1
#define AUDIO_CH2                   2
#define AUDIO_CH3                   3
#define AUDIO_CH4                   4
#define AUDIO_CH5                   5
#define AUDIO_CH6                   6
#define AUDIO_CH7                   7

// ACTION channel ID
#define ACTION_CH0                  _AUDIO_API_CH_NUM
#define ACTION_CH1                  (_AUDIO_API_CH_NUM+1)
#define ACTION_CH2                  (_AUDIO_API_CH_NUM+2)
#define ACTION_CH3                  (_AUDIO_API_CH_NUM+3)
#define ACTION_CH4                  (_AUDIO_API_CH_NUM+4)
#define ACTION_CH5                  (_AUDIO_API_CH_NUM+5)
#define ACTION_CH6                  (_AUDIO_API_CH_NUM+6)
#define ACTION_CH7                  (_AUDIO_API_CH_NUM+7)
#define ACTION_CH8                  (_AUDIO_API_CH_NUM+8)
#define ACTION_CH9                  (_AUDIO_API_CH_NUM+9)
#define ACTION_CH10                 (_AUDIO_API_CH_NUM+10)
#define ACTION_CH11                 (_AUDIO_API_CH_NUM+11)
#define ACTION_CH12                 (_AUDIO_API_CH_NUM+12)
#define ACTION_CH13                 (_AUDIO_API_CH_NUM+13)
#define ACTION_CH14                 (_AUDIO_API_CH_NUM+14)
#define ACTION_CH15                 (_AUDIO_API_CH_NUM+15)
#define ACTION_CH16                 (_AUDIO_API_CH_NUM+16)
#define ACTION_CH17                 (_AUDIO_API_CH_NUM+17)
#define ACTION_CH18                 (_AUDIO_API_CH_NUM+18)
#define ACTION_CH19                 (_AUDIO_API_CH_NUM+19)
#define ACTION_All_CH				(_AUDIO_API_CH_NUM+200)

// ADC/DAC Timer
#define USE_TIMER0                  0
#define USE_TIMER1                  1
#define USE_TIMER2                  2
#define USE_RTC						3
#define USE_TIMER_PWMA				4
#define USE_TIMER_PWMB				5
#define USE_TIMER3                  6

// DAC PP gain
#define PP_GAIN_40					0
#define PP_GAIN_45					1
#define PP_GAIN_50                  2
#define PP_GAIN_55					3
#define PP_GAIN_60					4
#define PP_GAIN_66					5
#define PP_GAIN_71					6
#define PP_GAIN_77					7
#define PP_GAIN_83                  8
#define PP_GAIN_88					9
#define PP_GAIN_94					10
#define PP_GAIN_100                 11
#define PP_GAIN_107					12
#define PP_GAIN_114					13
#define PP_GAIN_122					14
#define PP_GAIN_130					15

// Ramp setting
#define RAMP_NONE                   0
#define RAMP_UP                     1
#define RAMP_DOWN                   2
#define RAMP_UP_DOWN                3

// Channel Volume Setting
#define CH_VOL_0                    0
#define CH_VOL_1                    1
#define CH_VOL_2                    2
#define CH_VOL_3                    3
#define CH_VOL_4                    4
#define CH_VOL_5                    5
#define CH_VOL_6                    6
#define CH_VOL_7                    7
#define CH_VOL_8                    8
#define CH_VOL_9                    9
#define CH_VOL_10                   10
#define CH_VOL_11                   11
#define CH_VOL_12                   12
#define CH_VOL_13                   13
#define CH_VOL_14                   14
#define CH_VOL_15                   15

// Speed index
#define SPEED_N12                   (-12)
#define SPEED_N11                   (-11)
#define SPEED_N10                   (-10)
#define SPEED_N9                    (-9)
#define SPEED_N8                    (-8)
#define SPEED_N7                    (-7)
#define SPEED_N6                    (-6)
#define SPEED_N5                    (-5)
#define SPEED_N4                    (-4)
#define SPEED_N3                    (-3)
#define SPEED_N2                    (-2)
#define SPEED_N1                    (-1)
#define SPEED_P0                    0
#define SPEED_P1                    1
#define SPEED_P2                    2
#define SPEED_P3                    3
#define SPEED_P4                    4
#define SPEED_P5                    5
#define SPEED_P6                    6
#define SPEED_P7                    7
#define SPEED_P8                    8
#define SPEED_P9                    9
#define SPEED_P10                   10
#define SPEED_P11                   11
#define SPEED_P12                   12

// Pitch index
#define PITCH_N12                   (-12)
#define PITCH_N11                   (-11)
#define PITCH_N10                   (-10)
#define PITCH_N9                    (-9)
#define PITCH_N8                    (-8)
#define PITCH_N7                    (-7)
#define PITCH_N6                    (-6)
#define PITCH_N5                    (-5)
#define PITCH_N4                    (-4)
#define PITCH_N3                    (-3)
#define PITCH_N2                    (-2)
#define PITCH_N1                    (-1)
#define PITCH_P0                    0
#define PITCH_P1                    1
#define PITCH_P2                    2
#define PITCH_P3                    3
#define PITCH_P4                    4
#define PITCH_P5                    5
#define PITCH_P6                    6
#define PITCH_P7                    7
#define PITCH_P8                    8
#define PITCH_P9                    9
#define PITCH_P10                   10
#define PITCH_P11                   11
#define PITCH_P12                   12

// Robot sound index
#define ROBOT_0                     0
#define ROBOT_1                     1
#define ROBOT_2                     2
#define ROBOT_3                     3
#define ROBOT_4                     4
#define ROBOT_5                     5
#define ROBOT_6                     6
#define ROBOT_7                     7
#define ROBOT_8                     8
#define ROBOT_9                     9
#define ROBOT_10                    10
#define ROBOT_11                    11
#define ROBOT_12                    12
#define ROBOT_13                    13
#define ROBOT_14                    14
#define ROBOT_15                    15

// Sample rate
#define SAMPLE_RATE_48000           48000
#define SAMPLE_RATE_44100           44100
#define SAMPLE_RATE_32000           32000
#define SAMPLE_RATE_24000			24000
#define SAMPLE_RATE_22050           22050
#define SAMPLE_RATE_16000           16000
#define SAMPLE_RATE_12000           12000
#define SAMPLE_RATE_10000           10000
#define SAMPLE_RATE_8000            8000

// Codec
#define CODEC_MIDI                  0
#define CODEC_SBC                   1
#define CODEC_ADPCM                 2
#define CODEC_CELP                  3
#define CODEC_PCM                   4
#define CODEC_ACTION				5
#define CODEC_SBC2_E                6
#define CODEC_SBC_E                 7
#define CODEC_SBC2					8
#define CODEC_NOT_CODEC_MARK		0xFE
#define CODEC_NOT_CODEC             0xFF

// Audio status
#define STATUS_STOP                 0
#define STATUS_PLAYING              1
#define STATUS_PAUSE                2
#define STATUS_RECORDING            3
#define STATUS_MUTE					4
#define STATUS_CLEAR				5
#define STATUS_RESUME               6       //Q-Code use
#define STATUS_AUDIO_RECORDING      8
#define STATUS_UPDATE               0xFF    //Q-Code use

// Record Erase
#define C_RECORD_ERASE_IDLE         0
#define C_RECORD_ERASE_START        1
#define C_RECORD_ERASE_RUN          2
#define C_RECORD_ERASE_DONE         3
#define C_RECORD_ERASE_WAIT         4

// Record Playback active algorithm channel
#define C_RECORD_RECORD_SBC         (1<<0)
#define C_RECORD_RECORD_ADPCM       (1<<2)
#define C_WHOLE_CHANNEL_COMPLETE    0x0

//TYPE number
#define TYPE0			0
#define TYPE1			1
#define TYPE2			2
#define TYPE3			3
#define CUSTOMIZE		0xFF

// ID of address mode
#define ADDRESS_MODE                0x80000000

// Storage mode
#define SPI_MODE                    0x00
#define OTP_MODE                    0x02
#define EF_MODE                    	0x02
#define MIDI_MODE                   0x04
#define SPI1_MODE					0x08
#define EF_UD_MODE					0x10

// SPI mode
#define SPI_1_1_1_MODE              0
#define SPI_1_2_2_MODE              1
#define SPI_1_4_4_MODE              2
#define SPI_1_1_2_MODE              3
#define SPI_1_1_1_MODE_3WIRE        4

// XIP mode
#define XIP_1_IO_MODE               0
#define XIP_2_IO_MODE               2
#define XIP_4_IO_MODE               3
#define XIP_1_IO_MODE_3WIRE         1
#define XIP_2_IO_MODE_112           4
#define XIP_4_IO_MODE_114           5

#define _XIP						__attribute__ ((noinline)) __attribute__ ((section(".xip_code")))
#define _XIP_RODATA 				__attribute__ ((section(".xip_rodata")))

// I2C
#define ONE_BYTE                    1
#define TWO_BYTE                    2
#define I2C_MASTER                  0
#define I2C_SLAVE                   1
#define I2C_DIR_TX                  0
#define I2C_DIR_RX                  1

// GPIO_I2C
#define GPIO_I2C_WRITE_MODE			0x00
#define GPIO_I2C_READ_MODE			0x01

// Action
#define ACTION_PORTA                (0<<4)
#define ACTION_PORTB                (1<<4)
#define ACTION_PORTC                (2<<4)
#define ACTION_REPEAT               1
#define ACTION_NO_REPEAT            0
#define ACTION_BUSY_HIGH            1
#define ACTION_BUSY_LOW             0

// Software PWM
#define SW_PWM_BUSY_woDEFINE		2
#define SW_PWM_BUSY_HIGH			1
#define SW_PWM_BUSY_LOW				0

//QFID Reader
#define QFID_FREQ_125K				0
#define QFID_FREQ_530K				1
#define QFID_TYPE_IR				0
#define QFID_TYPE_PWM				1
#define QFID_MODE_ID                0
#define QFID_MODE_ID_INPUT          1

// Direct Key
#define PRESS_TIME_10ms             10
#define SCAN_TICK_4ms               4
// Direct Key
#define DKEY_PRESS					1
#define DKEY_RELEASE				2
#define DKEY_PORTA					0
#define DKEY_PORTB					1
#define DKEY_PORTC					2
#define DKEY_PORTD					3
#define PA0_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTA<<4) | 0)
#define PA0_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTA<<4) | 0)
#define PA1_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTA<<4) | 1)
#define PA1_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTA<<4) | 1)
#define PA2_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTA<<4) | 2)
#define PA2_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTA<<4) | 2)
#define PA3_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTA<<4) | 3)
#define PA3_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTA<<4) | 3)
#define PA4_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTA<<4) | 4)
#define PA4_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTA<<4) | 4)
#define PA5_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTA<<4) | 5)
#define PA5_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTA<<4) | 5)
#define PA6_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTA<<4) | 6)
#define PA6_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTA<<4) | 6)
#define PA7_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTA<<4) | 7)
#define PA7_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTA<<4) | 7)
#define PA8_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTA<<4) | 8)
#define PA8_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTA<<4) | 8)
#define PA9_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTA<<4) | 9)
#define PA9_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTA<<4) | 9)
#define PA10_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTA<<4) | 10)
#define PA10_RELEASE				((DKEY_RELEASE<<6)	| (DKEY_PORTA<<4) | 10)
#define PA11_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTA<<4) | 11)
#define PA11_RELEASE				((DKEY_RELEASE<<6)	| (DKEY_PORTA<<4) | 11)
#define PA12_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTA<<4) | 12)
#define PA12_RELEASE				((DKEY_RELEASE<<6)	| (DKEY_PORTA<<4) | 12)
#define PA13_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTA<<4) | 13)
#define PA13_RELEASE				((DKEY_RELEASE<<6)	| (DKEY_PORTA<<4) | 13)
#define PA14_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTA<<4) | 14)
#define PA14_RELEASE				((DKEY_RELEASE<<6)	| (DKEY_PORTA<<4) | 14)
#define PA15_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTA<<4) | 15)
#define PA15_RELEASE				((DKEY_RELEASE<<6)	| (DKEY_PORTA<<4) | 15)
#define PB0_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTB<<4) | 0)
#define PB0_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTB<<4) | 0)
#define PB1_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTB<<4) | 1)
#define PB1_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTB<<4) | 1)
#define PB2_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTB<<4) | 2)
#define PB2_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTB<<4) | 2)
#define PB3_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTB<<4) | 3)
#define PB3_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTB<<4) | 3)
#define PB4_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTB<<4) | 4)
#define PB4_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTB<<4) | 4)
#define PB5_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTB<<4) | 5)
#define PB5_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTB<<4) | 5)
#define PB6_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTB<<4) | 6)
#define PB6_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTB<<4) | 6)
#define PB7_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTB<<4) | 7)
#define PB7_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTB<<4) | 7)
#define PB8_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTB<<4) | 8)
#define PB8_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTB<<4) | 8)
#define PB9_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTB<<4) | 9)
#define PB9_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTB<<4) | 9)
#define PB10_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTB<<4) | 10)
#define PB10_RELEASE				((DKEY_RELEASE<<6)	| (DKEY_PORTB<<4) | 10)
#define PB11_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTB<<4) | 11)
#define PB11_RELEASE				((DKEY_RELEASE<<6)	| (DKEY_PORTB<<4) | 11)
#define PB12_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTB<<4) | 12)
#define PB12_RELEASE				((DKEY_RELEASE<<6)	| (DKEY_PORTB<<4) | 12)
#define PB13_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTB<<4) | 13)
#define PB13_RELEASE				((DKEY_RELEASE<<6)	| (DKEY_PORTB<<4) | 13)
#define PB14_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTB<<4) | 14)
#define PB14_RELEASE				((DKEY_RELEASE<<6)	| (DKEY_PORTB<<4) | 14)
#define PB15_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTB<<4) | 15)
#define PB15_RELEASE				((DKEY_RELEASE<<6)	| (DKEY_PORTB<<4) | 15)
#define PC0_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTC<<4) | 0)
#define PC0_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTC<<4) | 0)
#define PC1_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTC<<4) | 1)
#define PC1_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTC<<4) | 1)
#define PC2_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTC<<4) | 2)
#define PC2_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTC<<4) | 2)
#define PC3_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTC<<4) | 3)
#define PC3_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTC<<4) | 3)
#define PC4_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTC<<4) | 4)
#define PC4_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTC<<4) | 4)
#define PC5_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTC<<4) | 5)
#define PC5_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTC<<4) | 5)
#define PC6_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTC<<4) | 6)
#define PC6_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTC<<4) | 6)
#define PC7_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTC<<4) | 7)
#define PC7_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTC<<4) | 7)
#define PD0_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTD<<4) | 0)
#define PD0_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTD<<4) | 0)
#define PD1_PRESS					((DKEY_PRESS<<6) 	| (DKEY_PORTD<<4) | 1)
#define PD1_RELEASE					((DKEY_RELEASE<<6)	| (DKEY_PORTD<<4) | 1)
#define _IS_DKEY_PRESS(a)           ((((a)>>0x6)&0x3)==DKEY_PRESS)
#define _IS_DKEY_RELEASE(a)         ((((a)>>0x6)&0x3)==DKEY_RELEASE)
#define _GET_DKEY_PORT(a)           (((a)>>0x4)&0x3)
#define _GET_DKEY_PIN(a)            ((a)&0xF)

// Turn on or off
#define ENABLE                      1
#define DISABLE                     0

// Boolean true or false
#define true                        1
#define false                       0

// Process exit status
#define EXIT_FAILURE                1
#define EXIT_SUCCESS                0

// Bit Set
#define BIT_SET                     1
#define BIT_UNSET                   0

// GPIO Setting
#define GPIO_INPUT_PULLHIGH         0
#define GPIO_INPUT_FLOAT            1
#define GPIO_OUTPUT_HIGH			2
#define GPIO_OUTPUT_LOW		        3
#define GPIO_OUTPUT_ALT             4
#define GPIO_OUTPUT_ALT2            5
#define GPIO_OUTPUT_ALT3            6
#define GPIO_PIN_OFF                0xFF

// UART MFP
#define	UART_PORT_PA67				0
#define	UART_PORT_PD01				1

// Capture Setting
#define CAP_RISING                  (1<<5)
#define CAP_FALLING                 (1<<4)
#define CAP_RISING_FALLING          (CAP_RISING | CAP_FALLING)

// VR Setting
#define VR_CMD_STATE_1 				1
#define VR_CMD_STATE_2 				2
#define VR_CMD_STATE_3 				3
#define VR_CMD_STATE_4 				4
#define VR_CMD_STATE_5 				5
#define VR_CMD_STATE_6 				6
#define VR_CMD_STATE_7 				7
#define VR_CMD_STATE_8 				8
#define VR_CMD_STATE_9 				9
#define VR_CMD_STATE_10 			10
#define VR_TRIGGER_STATE   			20
#define VR_TRIGGER_STATE_2   		21
#define VR_TRIGGER_STATE_3   		22
#define VR_TRIGGER_STATE_4   		23

#define VR_OFF   					0
#define VR_ON               		1
#define VR_HALT             		2

#define VR_SI_Func					0
#define VR_SD_Func					1

#define TAGID1						51
#define TAGID2						52
#define TAGID3						53
#define TAGID4						54
#define TAGID5						55
#define TAGID6						56
#define TAGID7						57
#define TAGID8						58
#define PASSWORD_ID1				201

#define TRAIN_DELETEMODEL			0
#define TRAIN_CREATID				1
#define TRAIN_DELETEID				2

#define DO_NOTHING					0
#define TRAIN_TAG_OK				1
#define TRAIN_TAG_FAIL				(-1)
#define DELETE_TAG_OK				2
#define DELETE_TAG_FAIL				(-2)
#define DELETE_TAG_NoTAG			(-3)
#define DELETE_SD_MODEL_OK			3

// Touch Setting
#define TOUCH_PAD_IN_PA0			0
#define TOUCH_PAD_OUT_PA0			0
#define TOUCH_PAD_IN_PA1			1
#define TOUCH_PAD_OUT_PA1			1
#define TOUCH_PAD_IN_PA2			2
#define TOUCH_PAD_OUT_PA2			2
#define TOUCH_PAD_IN_PA3			3
#define TOUCH_PAD_OUT_PA3			3
#define TOUCH_PAD_IN_PA4			4
#define TOUCH_PAD_OUT_PA4			4
#define TOUCH_PAD_IN_PA5			5
#define TOUCH_PAD_OUT_PA5			5
#define TOUCH_PAD_IN_PA6			6
#define TOUCH_PAD_OUT_PA6			6
#define TOUCH_PAD_IN_PA7			7
#define TOUCH_PAD_OUT_PA7			7
#define TOUCH_PAD_IN_PA8			8
#define TOUCH_PAD_OUT_PA8			8
#define TOUCH_PAD_IN_PA9			9
#define TOUCH_PAD_OUT_PA9			9
#define TOUCH_PAD_IN_PA10			10
#define TOUCH_PAD_OUT_PA10			10
#define TOUCH_PAD_IN_PA11			11
#define TOUCH_PAD_OUT_PA11			11
#define TOUCH_PAD_IN_PA12			12
#define TOUCH_PAD_OUT_PA12			12
#define TOUCH_PAD_IN_PA13			13
#define TOUCH_PAD_OUT_PA13			13
#define TOUCH_PAD_IN_PA14			14
#define TOUCH_PAD_OUT_PA14			14
#define TOUCH_PAD_IN_PA15			15
#define TOUCH_PAD_OUT_PA15			15

#define TOUCH_MAX_1_PAD				1
#define TOUCH_MAX_2_PAD				2
#define TOUCH_MAX_3_PAD				3
#define TOUCH_MAX_4_PAD				4
#define TOUCH_MAX_5_PAD				5
#define TOUCH_MAX_6_PAD				6
#define TOUCH_MAX_7_PAD				7
#define TOUCH_MAX_8_PAD				8
#define TOUCH_MAX_9_PAD				9
#define TOUCH_MAX_10_PAD			10
#define TOUCH_MAX_11_PAD			11
#define TOUCH_MAX_12_PAD			12
#define TOUCH_MAX_13_PAD			13
#define TOUCH_MAX_14_PAD			14
#define TOUCH_MAX_15_PAD			15
#define TOUCH_MAX_16_PAD			16

#define TOUCH_CAL_1_SEC				1
#define TOUCH_CAL_2_SEC				2
#define TOUCH_CAL_3_SEC				3
#define TOUCH_CAL_4_SEC				4
#define TOUCH_CAL_5_SEC				5
#define TOUCH_CAL_6_SEC				6
#define TOUCH_CAL_7_SEC				7
#define TOUCH_CAL_8_SEC				8
#define TOUCH_CAL_9_SEC				9
#define TOUCH_CAL_10_SEC			10
#define TOUCH_CAL_11_SEC			11
#define TOUCH_CAL_12_SEC			12
#define TOUCH_CAL_13_SEC			13
#define TOUCH_CAL_14_SEC			14
#define TOUCH_CAL_15_SEC			15
#define TOUCH_CAL_16_SEC			16

#define TOUCH_PAD0					0
#define TOUCH_PAD1					1
#define TOUCH_PAD2					2
#define TOUCH_PAD3					3
#define TOUCH_PAD4					4
#define TOUCH_PAD5					5
#define TOUCH_PAD6					6
#define TOUCH_PAD7					7
#define TOUCH_PAD8					8
#define TOUCH_PAD9					9
#define TOUCH_PAD10					10
#define TOUCH_PAD11					11
#define TOUCH_PAD12					12
#define TOUCH_PAD13					13
#define TOUCH_PAD14					14
#define TOUCH_PAD15					15

#define TOUCH_NORMAL_8_MS			8
#define TOUCH_NORMAL_16_MS			16

#define TOUCH_SLEEP_50_MS			50

#define TOUCH_ENFORCE_40_SEC		40

#define TOUCH_WAKE_8_SEC			8
#define TOUCH_WAKE_9_SEC			9
#define TOUCH_WAKE_10_SEC			10
#define TOUCH_WAKE_11_SEC			11
#define TOUCH_WAKE_12_SEC			12

#define TOUCH_SENSITIVE_LV0			0
#define TOUCH_SENSITIVE_LV1			1
#define TOUCH_SENSITIVE_LV2			2
#define TOUCH_SENSITIVE_LV3			3

#define TOUCH_PORTA					(0<<4)
#define TOUCH_PORTB 				(1<<4)
#define TOUCH_PORTC					(2<<4)

// IR Setting
#define IR_TX_CARRIER_LOW			0
#define IR_TX_CARRIER_HIGH			1
#define IR_TX_55K5HZ				0
#define IR_TX_38K5HZ				1
#define IR_DATA_LEN_16BIT			16
#define IR_DATA_LEN_12BIT			12
#define IR_DATA_LEN_8BIT			8
#define IR_DATA_LEN_4BIT			4
#define IR_CRC_LEN_8BIT				8
#define IR_CRC_LEN_4BIT				4
#define IR_CRC_LEN_2BIT				2
#define IR_NO_CRC					0


#define DELAY_320ms					320
#define DELAY_800ms					800
#define DELAY_1200ms				1200
#define DELAY_USER_DEFINE			1


//Chrous use
#define DELAY_10ms				    10
#define DELAY_20ms				    20
#define DELAY_30ms				    30

//Power source
#define INT_PWR						1
#define EXT_PWR						2

//Audio Mix Control
#define MIX_CTRL_0                  0
#define MIX_CTRL_25                 1
#define MIX_CTRL_50                 2
#define MIX_CTRL_75                 3
#define MIX_CTRL_100                4

//SBC AUTO SAVE
#define C_FLASH_ERASE_IDLE          0
#define C_FLASH_ERASE_START         1
#define C_FLASH_ERASE_GOING         2

// LEDSTR channel ID
#define LEDSTR_CH0                   0
#define LEDSTR_CH1                   1
#define LEDSTR_CH2                   2
#define LEDSTR_CH3                   3
#define LEDSTR_CH4                   4
#define LEDSTR_CH5                   5
#define LEDSTR_CH6                   6
#define LEDSTR_CH7                   7
#define LEDSTR_CH8                   8
#define LEDSTR_CH9                   9
#define LEDSTR_CH10                  10
#define LEDSTR_CH11                  11
#define LEDSTR_CH12                  12
#define LEDSTR_CH13                  13
#define LEDSTR_CH14                  14
#define LEDSTR_CH15                  15

//====================================================================================
//  Include
//====================================================================================
#include "nx1_config.h"
#if !defined(_QCODE_BUILD)
#define _QCODE_BUILD    0
#endif
#if _QCODE_BUILD
#undef  _SYS_TIMEBASE
#define _SYS_TIMEBASE   _NYIDE_TIMEBASE
#endif
#if !defined(_LEDSTRn_TIMER)
#define _LEDSTRn_TIMER _LEDSTR_TIMER
#endif
#if !defined(_LEDSTRn_TIMING)
#define _LEDSTRn_TIMING _LEDSTR_TIMING
#endif
#if !defined(_LEDSTRm_TIMING)
#define _LEDSTRm_TIMING _LEDSTR_TIMING
#endif

#if !defined(_IR_LOPWR_RX_STB_TIME)
#define _IR_LOPWR_RX_STB_TIME   (500000)    //T3_us
#endif
#if !defined(_IR_LOPWR_RX_ENPWR_TIME)
#define _IR_LOPWR_RX_ENPWR_TIME (4000)      //T2_us
#endif
#if !defined(_IR_LOPWR_RX_MASK_TIME)
#define _IR_LOPWR_RX_MASK_TIME  (1000)      //T2a_us
#endif
//====================================================================================
//  External Reference
//====================================================================================
extern volatile U32 SysTick;//ms
extern void SysTickInit(void);
extern void TIMEBASE_ServiceLoop(void);
extern U8 DIRECTKEY_GetKey(void);
extern U8 DIRECTKEY_PeekKey(void);
extern bool DIRECTKEY_IsQueueEmpty(void);
extern void DIRECTKEY_Reset(void);
extern void DIRECTKEY_ScanCmd(U8 _cmd);

extern void MIDI_ServiceLoop(void);
extern void MIDI_Init(void);
extern void MIDI_SetVolume(U8 vol);
extern void MIDI_SetMixCtrl(U8 mixCtrl);
extern U8   MIDI_GetStatus(void);
extern U8 	MIDI_GetStatus_All(void);
extern void MIDI_Stop(void);
extern void INST_MIDI_STOP(void);
extern U8 NoteEnv_GetStatus(U8 chx, U8 inst, U8 pitch);
extern void MIDI_Resume(void);
extern void MIDI_Pause(void);
extern U8   MIDI_Play(U32 index,U8 mode);
extern void MIDI_DacIsr(U8 size);

extern void MIDI_Seq_Store(U32* AddrPointer);
extern void MIDI_Seq_Return(U32* AddrPointer);
extern U8 Get_Pitch(U8 Clear_Pitch);	//Clear_Pitch:1->clear pitch, 0->reserve pitch
//------MIDI Tempo------//
extern void MIDI_SetTempo(U8 tempo);		//set absolute tempo (1~255)
extern void MIDI_SetTempo_Rel(S8 percent);	//set relative tempo (±%)
extern U8 MIDI_GetTempo(void);				//get tempo
extern void MIDI_ResetTempo(void);			//reest tempo
//---------------------//
extern void Change_PitchBendRatio(U8 Ch, U16 PitchBendRatio);//U8 Ch(value:1~16) U16 PitchBendRatio(value:1~65535,Default:16384)
extern void Change_VolCtl(U8 Ch, U8 VolCtl);//U8 Ch(value:1~16) U8 VolCtl(value:0~127,Default:127)
extern void Change_PatchIdx(U8 Ch, U8 PatchIdx);//U8 Ch(value:1~16) U8 Change_PatchIdx(value:0~128,Ref to Q-MIDI Index,EX:1=>Bright_Grand_Piano,128=>Percussion)
extern void MIDI_MuteCh(U8 ch, U8 cmd);
extern void MIDI_SetChVolume(U8 ch, U8 vol);
extern U8 MIDI_GetChVolume(U8 ch);
extern void MIDI_SetPitch(S8 semitone);
extern void MIDI_MaskCh(U8 ch, U8 cmd);
extern void MIDI_MaskPercu(U8 ch10_percu_index, U8 cmd);
extern void MIDI_SetMainChannel(U16 chx);
extern U16 MIDI_MainCh;
#if _MIDI_INST_NOTEON
#define MIDI_STOP 			9
#define NOTEON_EVENT_MODE	(129)
extern void Inst_NoteOn_init(void);
extern U8 InstNote_On(U8 Instrument,U8 Pitch,U8 Velocity);//Pitch:Pitch Number(EX:c1=24,Db1=25,C2=36)  Instrument: Instrument Number, Ref.to Q-MIDI(1:Bright_Grand Piano, 128:Percussions) Velocity:(0~127)
extern U8 InstNote_Off(U8 Instrument,U8 Pitch);//Pitch:Pitch Number(EX:c1=24,Db1=25,C2=36)  Instrument: Instrument Number, Ref.to Q-MIDI(1:Bright_Grand Piano, 128:Percussions)
extern void InstNote_SustainPedal(U8 cmd);
extern void InstNote_AllOff(void);
extern void InstNote_AllOff2(void);
extern void NoteOn_INST_Vol(U8 volume);//volume(value:0~255)
extern void NoteOn_MIDI_Vol(U8 volume);//volume(value:0~255)
extern U8 GetCodec(U32 index,U8 mode);
extern U8 NoteOn_GetStatus(void);	   //STATUS_PLAYING:1,STATUS_STOP:0
extern U8 NoteOn_Enable_Flag;
extern U16 NoteOn_CH_BusyFlag;
extern void NoteOn_Stop(void);			//NoteOn Stop
extern U8 MIDI_BusyFlag;
extern U8 Inst_MIDI_GetStatus(void);		//STATUS_PLAYING:1,STATUS_STOP:0
//--------OKON-------//
#define OKON_TRIG_STD	0
#define OKON_TRIG_INST	1
#define	OKON_PRESS		OKON_press();
#define OKON_RELEASE	OKON_release();
#define OKON_Reset 		Next_flag
extern U8 onekey_press;//key be pressed
extern U8 onekey_enable;//one_key_one_note function enable
extern U8 Next_flag;
extern OKONParameter	MIDI_OKON;
extern void OneKey_Play(U32 index,U8 mode);//similar to playMIDI
extern void OKON_press(void);
extern void OKON_release(void);
extern void OneKey_Func(U8 cmd, U8 trig);
extern U8 OneKey_GetStatus(void);
//-------------------//


//----Learning mode-----//
#define MIDI_LearnInput		MIDI_TeachInput
#define MIDI_LearnOutput	MIDI_TeachOutput
#define MIDILearn_enable	teach_enable
#define NoteReturn			Reset

extern void MIDI_Learn_Play(U32 index,U8 mode);
extern void	MIDIL_NoteReturn(void);
extern void MIDIL_NextNote(void);
extern void MIDIL_MStart(U8 MarkStart);
extern void MIDIL_MStop(U8 MarkStop);
extern U8 MIDIL_GetNote(void);
extern U8 MIDIL_NoteUpdate(void);
extern U8 MIDIL_GroupCheck(void);
extern void MIDIL_GroupProcessed(void);

extern TeachInput MIDI_TeachInput;
extern TeachOutput MIDI_TeachOutput;
extern U8 teach_enable;


#define MIDIL_FLAG_DISABLE		0
#define MIDIL_FLAG_ENABLE		1
#define MIDIL_GROUP_PROCESSED	2
//-------------------//


//------MIDI pMidiCtlState set-----//
extern void Change_InstPitchBendRatio(U16 PitchBendRatio);//U16 PitchBendRatio(value:1~65535,Default:16384)
extern void Change_InstVibrato(U8 vibrato_coef);//U8 vibrato_coef(value:0~127,Default:0,disable)
//--------------------------------//
extern U8 InstNote_Gliss(U8 note, S8 semitone, U16 tick);

extern void Set_ShortInst_HoldTick(U8 tick);//U8 tick(value:0~255,Default:0,disable, unit:4mS)
extern void Set_LongInst_PlayTick(S16 tick);//S16 tick(vaule:0~32767,Default:0, unlimited play, unit:4mS)


#if _MIDI_INST_RECORD
extern U8 InstNote_Record(U32 index,U8 mode,U16 trigTout,U32 size);
extern void InstNote_Rec_Stop(void);
extern U8 InstNote_RecPlay(U32 index,U8 mode);
extern void InstNote_RecPlay_Stop(void);
extern U8 InstNote_Rec_GetStatus(void) ;
extern void InstNote_RecordPlay_ServiceLoop(void);

extern U8 instNote_Rec_Init(U32 index,U8 mode,U16 trigTout,U32 size);
extern U8 instNote_RecPlay_Init(U32 index, U8 mode);
extern void instNote_RecPlay_Stop(void);
extern void instNote_Rec_Stop(void);
extern void instNote_On_RecData(U8 opt, U8 vel, U32 RecTime);
extern void instNote_Off_RecData(U8 sPedal, U8 pitch, U32 RecTime);
#endif //end: _MIDI_INST_RECORD

#endif

extern U8	SBC_PlayRepeatFlag;
extern void SBC_ServiceLoop(void);
extern void SBC_Init(void);
extern void SBC_SetVolume(U8 vol);
extern U8   SBC_GetStatus(void);
extern void SBC_Stop(void);
extern void SBC_Resume(void);
extern void SBC_Pause(void);
extern U8   SBC_Play(U32 index,U8 mod);
extern void SBC_DacIsr(U8 size);
extern void SBC_AdcIsr(U8 size, U32 *pdest32);
extern void SBC_SetEQFilter(U8 index);
extern void SBC_SetSpeed(S8 index);
extern void SBC_SetPitch(S8 index);
extern void SBC_SetRobot1Sound(U8 cmd,U8 index);
extern void SBC_SetRobot2Sound(U8 cmd,U8 type, U8 thres);
extern void SBC_SetRobot3Sound(U8 cmd,U8 type, U8 thres);
extern void SBC_SetRobot4Sound(U8 cmd);
extern void SBC_SetReverbSound(U8 cmd,U8 index);
extern void SBC_SetEchoSound(U8 cmd,U8 index);
extern void SBC_SetMixCtrl(U8 mixCtrl);
extern U8   SBC_Record(U32 index,U8 mode,U32 size);
extern void SBC_StartRecordErase(U32 address, U32 length, U16 chan);
extern void SBC_SkipTailSetting(U16 ms);
extern void SBC_Seek(void);
extern void SBC_StorageSetVar(void);

extern void SBC_CH2_ServiceLoop(void);
extern void SBC_CH2_Init(void);
extern void SBC_CH2_SetVolume(U8 vol);
extern U8   SBC_CH2_GetStatus(void);
extern void SBC_CH2_Stop(void);
extern void SBC_CH2_Resume(void);
extern void SBC_CH2_Pause(void);
extern U8   SBC_CH2_Play(U32 index,U8 mode);
extern void SBC_CH2_DacIsr(U8 size);
extern void SBC_CH2_SetEQFilter(U8 index);
extern void SBC_CH2_SetSpeed(S8 index);
extern void SBC_CH2_SetPitch(S8 index);
extern void SBC_CH2_SetRobot1Sound(U8 cmd,U8 index);
extern void SBC_CH2_SetRobot2Sound(U8 cmd,U8 type, U8 thres);
extern void SBC_CH2_SetRobot3Sound(U8 cmd,U8 type, U8 thres);
extern void SBC_CH2_SetReverbSound(U8 cmd,U8 index);
extern void SBC_CH2_SetEchoSound(U8 cmd,U8 index);
extern void SBC_CH2_SetMixCtrl(U8 mixCtrl);

extern U8	SBC2_PlayRepeatFlag;
extern void SBC2_ServiceLoop(void);
extern void SBC2_Init(void);
extern void SBC2_SetVolume(U8 vol);
extern U8   SBC2_GetStatus(void);
extern void SBC2_Stop(void);
extern void SBC2_Resume(void);
extern void SBC2_Pause(void);
extern U8   SBC2_Play(U32 index,U8 mod);
extern void SBC2_DacIsr(U8 size);
extern void SBC2_SetEQFilter(U8 index);
extern void SBC2_SetMixCtrl(U8 mixCtrl);

extern void SBC2_CH2_ServiceLoop(void);
extern void SBC2_CH2_Init(void);
extern void SBC2_CH2_SetVolume(U8 vol);
extern U8   SBC2_CH2_GetStatus(void);
extern void SBC2_CH2_Stop(void);
extern void SBC2_CH2_Resume(void);
extern void SBC2_CH2_Pause(void);
extern U8   SBC2_CH2_Play(U32 index,U8 mode);
extern void SBC2_CH2_DacIsr(U8 size);
extern void SBC2_CH2_SetEQFilter(U8 index);
extern void SBC2_CH2_SetMixCtrl(U8 mixCtrl);

extern U8   ADPCM_PlayRepeatFlag;
extern void ADPCM_ServiceLoop(void);
extern void ADPCM_Init(void);
extern void ADPCM_SetVolume(U8 vol);
extern U8   ADPCM_GetStatus(void);
extern void ADPCM_Stop(void);
extern void ADPCM_Resume(void);
extern void ADPCM_Pause(void);
extern U8   ADPCM_Play(U32 index,U8 mode);
extern void ADPCM_DacIsr(U8 size,S16 *buf);
extern void ADPCM_AdcIsr(U8 size, U32 *pdest32);
extern void ADPCM_SetEQFilter(U8 index);
extern void ADPCM_SetSpeed(S8 index);
extern void ADPCM_SetPitch(S8 index);
extern void ADPCM_SetSpeedPitch(S8 speed_index, S8 pitch_index);
extern void ADPCM_SetSpeedPitchN(S8 speedpitch_index);
extern void ADPCM_SetRobot1Sound(U8 cmd,U8 index);
extern void ADPCM_SetRobot2Sound(U8 cmd,U8 type, U8 thres);
extern void ADPCM_SetRobot3Sound(U8 cmd,U8 type, U8 thres);
extern void ADPCM_SetRobot4Sound(U8 cmd);
extern void ADPCM_SetReverbSound(U8 cmd,U8 index);
extern void ADPCM_SetEchoSound(U8 cmd);
extern void ADPCM_SetDarthSound(U8 Cmd, S8 PitchIndex, U8 CarrierType);
extern void ADPCM_SetAnimalRoar(U8 cmd, S8 speed_index, S8 pitch_index, U8 reverb_index);
extern void ADPCM_SetMixCtrl(U8 mixCtrl);
extern U8   ADPCM_Record(U32 index,U8 mode,U16 sample_rate,U32 size);
extern void ADPCM_StartRecordErase(U32 address, U32 length, U16 chan);
extern void	ADPCM_SkipTailSetting(U16 ms);
extern void ADPCM_SW_UpsampleCmd(U8 cmd, U8 upsampleFactor);

extern void ADPCM_CH2_ServiceLoop(void);
extern void ADPCM_CH2_Init(void);
extern void ADPCM_CH2_SetVolume(U8 vol);
extern U8   ADPCM_CH2_GetStatus(void);
extern void ADPCM_CH2_Stop(void);
extern void ADPCM_CH2_Resume(void);
extern void ADPCM_CH2_Pause(void);
extern U8   ADPCM_CH2_Play(U32 index,U8 mode);
extern void ADPCM_CH2_DacIsr(U8 size,S16 *buf);
extern void ADPCM_CH2_SetEQFilter(U8 index);
extern void ADPCM_CH2_SetMixCtrl(U8 mixCtrl);

extern void ADPCM_CH3_ServiceLoop(void);
extern void ADPCM_CH3_Init(void);
extern void ADPCM_CH3_SetVolume(U8 vol);
extern U8   ADPCM_CH3_GetStatus(void);
extern void ADPCM_CH3_Stop(void);
extern void ADPCM_CH3_Resume(void);
extern void ADPCM_CH3_Pause(void);
extern U8   ADPCM_CH3_Play(U32 index,U8 mode);
extern void ADPCM_CH3_DacIsr(U8 size,S16 *buf);
extern void ADPCM_CH3_SetEQFilter(U8 index);
extern void ADPCM_CH3_SetMixCtrl(U8 mixCtrl);

extern void ADPCM_CH4_ServiceLoop(void);
extern void ADPCM_CH4_Init(void);
extern void ADPCM_CH4_SetVolume(U8 vol);
extern U8   ADPCM_CH4_GetStatus(void);
extern void ADPCM_CH4_Stop(void);
extern void ADPCM_CH4_Resume(void);
extern void ADPCM_CH4_Pause(void);
extern U8   ADPCM_CH4_Play(U32 index,U8 mode);
extern void ADPCM_CH4_DacIsr(U8 size,S16 *buf);
extern void ADPCM_CH4_SetEQFilter(U8 index);
extern void ADPCM_CH4_SetMixCtrl(U8 mixCtrl);

extern void ADPCM_CH5_ServiceLoop(void);
extern void ADPCM_CH5_Init(void);
extern void ADPCM_CH5_SetVolume(U8 vol);
extern U8   ADPCM_CH5_GetStatus(void);
extern void ADPCM_CH5_Stop(void);
extern void ADPCM_CH5_Resume(void);
extern void ADPCM_CH5_Pause(void);
extern U8   ADPCM_CH5_Play(U32 index,U8 mode);
extern void ADPCM_CH5_DacIsr(U8 size,S16 *buf);
extern void ADPCM_CH5_SetEQFilter(U8 index);
extern void ADPCM_CH5_SetMixCtrl(U8 mixCtrl);

extern void ADPCM_CH6_ServiceLoop(void);
extern void ADPCM_CH6_Init(void);
extern void ADPCM_CH6_SetVolume(U8 vol);
extern U8   ADPCM_CH6_GetStatus(void);
extern void ADPCM_CH6_Stop(void);
extern void ADPCM_CH6_Resume(void);
extern void ADPCM_CH6_Pause(void);
extern U8   ADPCM_CH6_Play(U32 index,U8 mode);
extern void ADPCM_CH6_DacIsr(U8 size,S16 *buf);
extern void ADPCM_CH6_SetEQFilter(U8 index);
extern void ADPCM_CH6_SetMixCtrl(U8 mixCtrl);

extern void ADPCM_CH7_ServiceLoop(void);
extern void ADPCM_CH7_Init(void);
extern void ADPCM_CH7_SetVolume(U8 vol);
extern U8   ADPCM_CH7_GetStatus(void);
extern void ADPCM_CH7_Stop(void);
extern void ADPCM_CH7_Resume(void);
extern void ADPCM_CH7_Pause(void);
extern U8   ADPCM_CH7_Play(U32 index,U8 mode);
extern void ADPCM_CH7_DacIsr(U8 size,S16 *buf);
extern void ADPCM_CH7_SetEQFilter(U8 index);
extern void ADPCM_CH7_SetMixCtrl(U8 mixCtrl);

extern void ADPCM_CH8_ServiceLoop(void);
extern void ADPCM_CH8_Init(void);
extern void ADPCM_CH8_SetVolume(U8 vol);
extern U8   ADPCM_CH8_GetStatus(void);
extern void ADPCM_CH8_Stop(void);
extern void ADPCM_CH8_Resume(void);
extern void ADPCM_CH8_Pause(void);
extern U8   ADPCM_CH8_Play(U32 index,U8 mode);
extern void ADPCM_CH8_DacIsr(U8 size,S16 *buf);
extern void ADPCM_CH8_SetEQFilter(U8 index);
extern void ADPCM_CH8_SetMixCtrl(U8 mixCtrl);

extern void VR_Start(U8 *cybase, U8 **model);
extern void VR_Halt(void);
extern void VR_Stop(void);
extern S8   VR_ServiceLoop(void);
extern void VR_AdcIsr(U8 size, U32 *pdest32);
extern U32  VR_GroupAddr(U8 groupindex);
extern U32  VR_CVRGroupAddr(U16 CVRindex, U8 groupindex);
extern void VR_SetGroupState(U8 groupState, U8 numModel);
extern void VR_ApiRelease(U8 vrfunctype);
extern U8   VR_GetNumWord(U8* sd_addr);
extern S8   VR_VoiceTag_CreatID(U8 TAGID);
extern S8   VR_VoiceTag_DeleteID(U8 TAGID);
extern S8   VR_VoiceTag_DeleteMODEL();
extern void VR_SetNumTrainingUttr(U8 numUttr);
extern S8 	VR_HitScore;
extern U16 	VR_GetLoading(void);
extern U16  VR_GetRamSize(void);
extern U16  VR_GetEngineRamSize(void);
extern U32  VR_GetTimeOut(void);
extern U16  VR_VAD_GetIdHoldTime(void);
extern U16  VR_VAD_GetMuteHoldTime(void);
#if _QCODE_BUILD
extern void VR_SetModel(U8 *cybase, U8 **model);
extern S8   VR_GetHitScore(void);
extern S8   VR_GetHitId(void);
#endif

extern void VadInit(void);
extern void VadProcess(S16 *audio_buf,U16 samples);
extern U8   VR_GetVadSts(void);
extern U8   VR_GetVadSts2(void);
extern void VadSetThreshold(U16 threshold);
extern S16  VAD_GetThreshold(void);
extern S16  VAD_GetActiveCount(void);
extern S16  VAD_GetMuteCount(void);
extern S16  VAD_GetDcOffset(void);
#if _QCODE_BUILD
extern U8   VR_GetVadSts3(void);
extern void VR_VadCmd(U8 cmd);
extern bool VR_isVadEnable(void);
#endif

extern void SOUND_DETECTION_Start(void);
extern void SOUND_DETECTION_Stop(void);
extern void SOUND_DETECTION_AdcIsr(U8 size, U32 *pdest32);
extern void SOUND_DETECTION_Init(void);
extern void SOUND_DETECTION_Process(S16 *audio_buf,U16 samples);
extern U8   SOUND_DETECTION_GetSts(void);
extern U8  	SOUND_DETECTION_GetStatus(void);
extern void SOUND_DETECTION_ServiceLoop(void);

extern void AUDIO_ServiceLoop(void);
extern void AUDIO_ServiceLoop_Rightnow(void);
extern void Sentence_ServiceLoop(void);
extern void Sentence_ACTION_IO_Setting(U8 ch, U8 port, U8 busy);
extern void Sentence_Stop(U8 ch);
extern U16  AUDIO_GetSpiFileCount(void);
extern U16  AUDIO_GetSpi1FileCount(void);
extern void AUDIO_Init(void);
extern void AUDIO_SetChVolume(U8 ch,U8 vol);
extern void AUDIO_SetMixerVolume(U8 vol);
extern U8   AUDIO_GetMixerVolume(void);
extern void AUDIO_SetPPGain(U8 gain);
extern U8   AUDIO_GetPPGain(void);
extern U8   AUDIO_GetStatus(U8 ch);
extern U8   AUDIO_GetStatus_All(void);
extern void AUDIO_Stop(U8 ch);
extern void AUDIO_Resume(U8 ch);
extern void AUDIO_Pause(U8 ch);
extern U8   AUDIO_Play(U8 ch,U32 index,U8 mode);
extern U8   AUDIO_Play_AutoSelect(U8 ch, U32 index, U8 mode, U8 codec);
extern void AUDIO_SetEQFilter(U8 ch, U8 index);
extern void AUDIO_SetSpeed(U8 ch,S8 index);
extern void AUDIO_SetPitch(U8 ch,S8 index);
extern void AUDIO_SetRobot1Sound(U8 ch,U8 cmd,U8 index);
extern void AUDIO_SetRobot2Sound(U8 ch, U8 cmd,U8 type, U8 thres);
extern void AUDIO_SetRobot3Sound(U8 ch, U8 cmd,U8 type, U8 thres);
extern void AUDIO_SetRobot4Sound(U8 ch, U8 cmd);
extern void AUDIO_SetReverbSound(U8 ch, U8 cmd,U8 index);
extern void AUDIO_SetEchoSound(U8 ch, U8 cmd,U8 index);
extern void AUDIO_SetDarthSound(U8 ch, U8 Cmd, S8 PitchIndex, U8 CarrierType);
extern void AUDIO_SetSpeedPitchN(U8 ch, S8 speedpitch_index);
extern void AUDIO_SetMixCtrl(U8 ch, U8 mixCtrl);
extern void AUDIO_SetAnimalRoar(U8 ch, U8 cmd, S8 speed_index, S8 pitch_index, U8 reverb_index);
extern bool AUDIO_IsRecorded(U32 addr, U8 mode);

extern void AUDIO_Hw0_SetMixingChanNum(U8 num);
extern void AUDIO_Hw1_SetMixingChanNum(U8 num);
extern U8   AUDIO_GetAudioSwChan(U8 codec);
extern void SetUpsample(U8 cmd);
extern U8 GetUpsampleStatus(void);

extern void CELP_ServiceLoop(void);
extern void CELP_Init(void);
extern void CELP_SetVolume(U8 vol);
extern U8   CELP_GetStatus(void);
extern void CELP_Stop(void);
extern void CELP_Resume(void);
extern void CELP_Pause(void);
extern U8   CELP_Play(U32 index,U8 mode);
extern void CELP_DacIsr(U8 size);
extern void CELP_SetSpeed(S8 index);
extern void CELP_SetPitch(S8 index);
extern void CELP_SetRobot1Sound(U8 cmd,U8 index);
extern void CELP_SetRobot2Sound(U8 cmd,U8 type, U8 thres);
extern void CELP_SetRobot3Sound(U8 cmd,U8 type, U8 thres);
extern void CELP_SetReverbSound(U8 cmd,U8 index);
extern void CELP_SetEchoSound(U8 cmd,U8 index);
extern void CELP_SetMixCtrl(U8 mixCtrl);

extern void PCM_ServiceLoop(void);
extern void PCM_Init(void);
extern void PCM_SetVolume(U8 vol);
extern U8   PCM_GetStatus(void);
extern void PCM_Stop(void);
extern void PCM_Resume(void);
extern void PCM_Pause(void);
extern U8   PCM_Play(U32 index,U8 mode);
extern void PCM_DacIsr(U8 size);
extern void PCM_AdcIsr(U8 size, U32 *pdest32);
extern void PCM_SetSpeed(S8 index);
extern void PCM_SetPitch(S8 index);
extern void PCM_SetRobot1Sound(U8 cmd,U8 index);
extern void PCM_SetRobot2Sound(U8 cmd,U8 type, U8 thres);
extern void PCM_SetRobot3Sound(U8 cmd,U8 type, U8 thres);
extern void PCM_SetReverbSound(U8 cmd,U8 index);
extern void PCM_SetEchoSound(U8 cmd,U8 index);
extern void PCM_SetMixCtrl(U8 mixCtrl);
extern U8   PCM_Record(U32 index,U8 mode,U16 sample_rate,U32 size);
extern void PCM_SkipTailSetting(U16 ms);

extern void PCM_CH2_ServiceLoop(void);
extern void PCM_CH2_Init(void);
extern void PCM_CH2_SetVolume(U8 vol);
extern U8   PCM_CH2_GetStatus(void);
extern void PCM_CH2_Stop(void);
extern void PCM_CH2_Resume(void);
extern void PCM_CH2_Pause(void);
extern U8   PCM_CH2_Play(U32 index,U8 mode);
extern void PCM_CH2_DacIsr(U8 size);
extern void PCM_CH2_SetSpeed(S8 index);
extern void PCM_CH2_SetPitch(S8 index);
extern void PCM_CH2_SetRobot1Sound(U8 cmd,U8 index);
extern void PCM_CH2_SetRobot2Sound(U8 cmd,U8 type, U8 thres);
extern void PCM_CH2_SetRobot3Sound(U8 cmd,U8 type, U8 thres);
extern void PCM_CH2_SetReverbSound(U8 cmd,U8 index);
extern void PCM_CH2_SetEchoSound(U8 cmd,U8 index);
extern void PCM_CH2_SetMixCtrl(U8 mixCtrl);
extern U8   PCM_CH2_Record(U32 index,U8 mode,U16 sample_rate,U32 size);

extern void PCM_CH3_ServiceLoop(void);
extern void PCM_CH3_Init(void);
extern void PCM_CH3_SetVolume(U8 vol);
extern U8   PCM_CH3_GetStatus(void);
extern void PCM_CH3_Stop(void);
extern void PCM_CH3_Resume(void);
extern void PCM_CH3_Pause(void);
extern U8   PCM_CH3_Play(U32 index,U8 mode);
extern void PCM_CH3_DacIsr(U8 size);
extern void PCM_CH3_SetSpeed(S8 index);
extern void PCM_CH3_SetPitch(S8 index);
extern void PCM_CH3_SetRobot1Sound(U8 cmd,U8 index);
extern void PCM_CH3_SetRobot2Sound(U8 cmd,U8 type, U8 thres);
extern void PCM_CH3_SetRobot3Sound(U8 cmd,U8 type, U8 thres);
extern void PCM_CH3_SetReverbSound(U8 cmd,U8 index);
extern void PCM_CH3_SetEchoSound(U8 cmd,U8 index);
extern void PCM_CH3_SetMixCtrl(U8 mixCtrl);
extern U8   PCM_CH3_Record(U32 index,U8 mode,U16 sample_rate,U32 size);

extern void RT_ServiceLoop(void);
extern void RT_Init(void);
extern void RT_DacIsr(U8 size);
extern void RT_AdcIsr(U8 size, U32 *pdest32);
extern U8   RT_Play(void);
extern void RT_Stop(void);
extern U8 	RT_GetStatus(void);
extern void RT_SetPitch(S8 index);
extern void RT_SetRobot1Sound(U8 cmd,U8 index);
extern void RT_SetRobot2Sound(U8 cmd,U8 type, U8 thres);
extern void RT_SetRobot3Sound(U8 cmd,U8 type, U8 thres);
extern void RT_SetRobot4Sound(U8 cmd);
extern void RT_SetReverbSound(U8 cmd,U8 index);
extern void RT_SetEchoSound(U8 cmd);
extern void RT_SetGhostSound(U8 Cmd, S8 PitchIndex, U8 EnvelopeType);
extern void RT_SetDarthSound(U8 Cmd, S8 PitchIndex, U8 CarrierType);
extern void RT_SetVolume(U8 vol);
extern U8   RT_GetVolume(void);
extern void RT_SetChorusSound(U8 Cmd, S8 PitchIndex,U8 vol_1,U8 vol_2,U8 vol_3, U8 gain);
extern void RT_SetChorusVolume(U8 vol_1,U8 vol_2,U8 vol_3, U8 gain);
//----------------------------------------------
// RT Howling Suppress
#if _RT_PLAY && _RT_HOWLING_SUPPRESS
#define Howling_Suppress_OFF   		0
#define Howling_Suppress_ON         1
#define Howling_Suppress_INIT       2

extern U8	RT_Howling_Suppress_GetStatus(void);
extern void RT_Howling_Suppress_Init(void);
extern void RT_Howling_Suppress_ALG_Limit(void);
extern void RT_Howling_Suppress_Function(U8 cmd);
extern void RT_Howling_Suppress_Switch(void);
extern void RT_Howling_Suppress_ServiceLoop(S16 *Audio_buf_ptr);
#endif
//----------------------------------------------

extern void CB_MidiMark(U8 id);
extern void CB_GetPitch(U8 *NoteInfo);

extern U32  Random(U32 mi);
extern U32  RandomSeed(void);
extern void Delayms(U32 ms);

extern U8   ACTION_PlayIO(U8 ch,const U32 file,U8 port,U8 repeat,U8 busy,U8 mode);
extern void ACTION_StopIO(U8 ch);
extern void ACTION_ResumeIO(U8 ch);
extern void ACTION_PauseIO(U8 ch);
extern U8   ACTION_GetIOStatus(U8 ch);
extern void ACTION_ServiceLoop(void);
extern void ACTION_Init(void);
extern void ACTION_Sleep(void);
extern void ACTION_Wakeup(void);
extern bool ACTION_IsSpiAction(U8 _CH);
extern void CB_ActionMark(U8 id);

extern void INST_NoteOn_INIT(void);
extern void SYS_Init(void);
extern void SYS_ServiceLoop(void);
extern void SYS_ClrSleepTime(void);
extern void SYS_Sleep(void);
extern bool SYS_IsWakeupPinChange(void);
extern U32  SYS_GetRollingCode(void);
extern void PP_Enter_Halt(U8 DAC_status,U8 cmd);
extern void SYS_SpiPowerOff(void);
extern void SYS_SpiPowerOn(void);
extern bool CB_SYS_isAppBusy(void);
extern bool CB_SYS_isSleepable(void);
extern U32  SpiOffset(void);
extern U32  Spi1_Offset(void);
extern U8   SleepFlag;
extern volatile U8	AdcEnFlag;
extern volatile U16 AdcEnCnt;

extern void UTILITY_ServiceLoop(void);

typedef enum{
	kUD_USER_DEFINED=0,
	kUD_CVR=1,
	kUD_RECORD=2,
	kUD_VOICE_TAG=3,
	kUD_LED_STRING=4,
    kUD_STORAGE_MODULE=5,
	kUD_ACTION=252,
	kUD_MIDI=253,
	kUD_VOICE=254,
}UDR_APP_TYPE;
extern U16 RESOURCE_EF_GetResourceCnt(void);
extern U32 RESOURCE_EF_GetAddress(U16 idx);
extern U8 RESOURCE_EF_GetType(U16 idx);
extern U16 RESOURCE_GetResourceCnt(void);
extern U32 RESOURCE_GetAddress(U16 idx);
extern U8 RESOURCE_GetType(U16 idx);
extern U16 RESOURCE1_GetResourceCnt(void);
extern U32 RESOURCE1_GetAddress(U16 idx);
extern U8 RESOURCE1_GetType(U16 idx);
extern void UserDefinedData_ResetNonVolatilePara(U8 _mode);
extern U16 UserDefinedData_GetCount(U8 _mode);
extern U32 UserDefinedData_GetAddress(U16 _idx, U8 _mode);
extern U8  UserDefinedData_GetType(U16 _idx, U8 _mode);
extern U16 UserDefinedData_GetAlignment(U16 _idx, U8 _mode);
extern U32 UserDefinedData_GetLength(U16 _idx, U8 _mode);
extern U16 UserDefinedData_GetIndexUseType(U16 _idx, UDR_APP_TYPE _type, U8 _mode);
extern U32 UserDefinedData_GetAddressUseType(U16 _idx, UDR_APP_TYPE _type, U8 _mode);

extern void QFID_ServiceLoop(void);
extern void QFID_Func(U8 cmd);
extern U8   QFID_GetGroup(void);
extern U16  QFID_GetTagID(U8 grp);
extern U8   QFID_GetTagInput(U8 grp, U8 tagid);

extern void WaveID_SendCMD(U8 cmd);
extern S8 WaveID_ReceiveCMD(void);
extern U8 WaveID_GetStatus(void);
extern void WaveID_Start(void);
extern void WaveID_Stop(void);
extern void WaveID_Init(void);
extern void WaveID_AdcIsr(U8 size, U32 *pdest32);

extern void LEDSTR_Init_IO(U8 pin);
extern void LEDSTR_Init(void);
extern U8 LEDSTR_Play(U8 Channel, U32 index, U8 mode);
//extern void LEDSTR_Replace_Color(U32 *buf,U16 ID ,U8 R ,U8 G ,U8 B);
extern void LEDSTR_Stop(U8 Channel);
extern void LEDSTR_Clear(U8 Channel);
extern void LEDSTR_ServiceLoop(void);
extern U8 LEDSTR_GetStatus(U8 Channel);
extern void LEDSTR_Isr(void);
extern void LEDSTR_SetBrightness(U8 Brightness);

extern void LEDSTRn_Init(void);
extern U8 LEDSTRn_Play(U32 index, U8 mode);
extern void LEDSTRn_Stop(void);
extern void LEDSTRn_Clear(void);
extern void LEDSTRn_ServiceLoop(void);
extern U8 LEDSTRn_GetStatus(void);
extern void LEDSTRn_Isr(void);
extern void LEDSTRn_SetBrightness(U8 Brightness);

extern void LEDSTRm_Init(void);
extern U8 LEDSTRm_Play(U32 index, U8 mode);
extern void LEDSTRm_Stop(void);
extern void LEDSTRm_Clear(void);
extern void LEDSTRm_ServiceLoop(void);
extern U8 LEDSTRm_GetStatus(void);
extern void LEDSTRm_Isr(void);
extern void LEDSTRm_SetBrightness(U8 Brightness);

//_Sound Location lib
#define SL_OFF   					0
#define SL_ON               		1
#define SL_INIT             		2
extern U8	SL_GetStatus(void);
extern void SL_Init(void);
extern void SL_Start(void);
extern void SL_Stop(void);
extern S16 SL_ServiceLoop(void);
#if _SL_MIC_NUM==3
extern void SL_AdcIsr(U8 size, U32 *pdest0_32, U32 *pdest1_32, U32 *pdest2_32);
#elif   _SL_MIC_NUM==2
extern void SL_AdcIsr(U8 size, U32 *pdest0_32, U32 *pdest1_32);
#else
#error "Wrong _SL_MIC_NUM setting.(only 2 or 3)"
#endif

extern U8 ProgChecksum(void);
extern U8 SPI_ProgChecksum(void);
extern U32 Checksum_Calculate(U8 checkType, U8 offset1);

extern U8 isFDBverB(void);

extern const U32  PlayList[];

extern void Touch_SetEnforceTime(U8 EnforceTime);				//	Enforce calibration downcount time function
extern void Touch_ScanKey(void);								//	Touch scan process
extern void Touch_SetAutoCalibration(U8 u8PADCount);			//	Auto calibration
extern void Touch_SetEnforceCalibration (U8 u8PADCount);		//	Enforce calibration
extern void Touch_SetSensitivityLevel(U8 u8PADCount,U8 Level);	//	Set pad sensitive ,level(0~3,default0)
extern void Touch_SleepTime(U8 sleeptime);						//	Set sleep downcount time
extern void Touch_SetEnforceWakeup();							//	Set enforce wakeup
extern void	Touch_SetWakeUpMaxPAD(U8 u8MaxPAD);					//	Set wake up PAD numbers.
extern U32 	Touch_GetTouchValue(U8 u8PADCount);					//	return Touch PAD Count
extern U32 	Touch_GetBKGroundValue(U8 u8PADCount);				//	return Touch Background Count
extern U8 	Touch_GetSleepFlag(void);							//	Go to sleep available flag
extern U8 	Touch_GetPADStatus();								// =1 Touch PAD check Fail
extern U8   Touch_GetPADInput(U8 u8PADCount);
extern U8   Touch_GetPADOutput(U8 u8PADCount);


extern void GPIO_I2C_Init();
extern void GPIO_I2C_Start();
extern void GPIO_I2C_RepeatStart();
extern void GPIO_I2C_Stop();
extern void GPIO_I2C_Send_NoACK();
extern void GPIO_I2C_Send_ACK();
extern void GPIO_I2C_Write(U8 U8_write_data);
extern U8 	GPIO_I2C_Read();
extern U8 	GPIO_I2C_Check_ACK();
extern U8 	User_24LC256_Write(U8 I2C_Addr, U16 EEPROM_Addr, U8 *Buf);
extern U8 	User_24LC256_Write_nBytes(U8 I2C_Addr, U16 EEPROM_Addr, U8 Len, U8 *Buf);
extern U8 	User_24LC256_Read(U8 I2C_Addr, U16 EEPROM_Addr, U8 *Buf);
extern U8 	User_24LC256_Read_nBytes(U8 I2C_Addr, U16 EEPROM_Addr, U8 Len, U8 *Buf);

extern void SPI_EEPROM_Init(U8 idx, U8 eeprom_size, U8 eeprom_ringbuf_no);
extern void SPI_EEPROM_Selective_Read(U8 *buf, U16 eeprom_addr, U16 eeprom_len);
extern void SPI_EEPROM_Sequential_Read(U8 *buf);
extern void SPI_EEPROM_nByte_Write(U8 *buf, U16 eeprom_addr, U16 eeprom_len);

extern U8	IR_GetTxStatus(void);
extern void IR_TxInit(void);
extern void IR_TxData(U16 data);
extern void IR_RxCmd(U8 cmd);
extern U8	IR_CheckRxData(void);
extern U16	IR_GetRxData(void);
extern void IR_RxInit(void);
extern U8	IR_CheckRxSleep(void);
extern void IR_TimerIsr(void);
extern U8	IR_LoPwr_RxGetSleepFlag(void);
extern void IR_LoPwr_RxInit(void);
extern void IR_LoPwr_RxInitBeforeStandby(void);
extern void IR_LoPwr_RxInitAfterStandby(void);
extern U8 	IR_LoPwr_RxProcess(void);
extern void IR_LoPwr_RxCmd(U8 cmd);
extern void IR_LoPwr_RxAutoScan(U8 cmd);
extern U8  	IR_LoPwr_RxTimerSet(U32 T3_us, U32 T2_us, U32 T2a_us);

//QIO Function
extern	void	CB_QIOMark(U8 id);
#if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)
#define	_QIO_PIN_DATA_SIZE	2
extern	U8		QIO_PinSet(const U8 _PinTab[][_QIO_PIN_DATA_SIZE], U8 _PinTabSize);
extern	U8		QIO_ChPinSet(U8 _CH, U8 _Pin, U8 _BusyLevel);
#else
extern	U8		QIO_PinSet(const U8 _PinTab[], U8 _PinTabSize);
extern	U8		QIO_ChPinSet(U8 _CH, U8 _Pin);
#endif//#if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)

//SW PWM-IO Function
extern	void	SW_PWM_Start();
extern	void	SW_PWM_Stop();
extern	void	SW_PWM_Pause();
extern	void	SW_PWM_Resume();
#if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)
extern	U8		SW_PWM_CH_Set(U8 _CH, U8 _Pin, U8 _BusyLevel, U16 _InitDuty);
extern	U8		_SW_PWM_CH_Set(U8 _CH, U8 _Pin, U8 _BusyLevel);
#else
extern	U8		SW_PWM_CH_Set(U8 _CH, U8 _Pin, U16 _InitDuty);
extern	U8		_SW_PWM_CH_Set(U8 _CH, U8 _Pin);
#endif//#if	(_SW_PWM_BUSY_LEVEL==SW_PWM_BUSY_woDEFINE)
extern	U8		SW_PWM_CH_Stop(U8 _CH);
extern	U8		SW_PWM_CH_Start(U8 _CH);
extern	U8		SW_PWM_Duty_Set(U8 _CH, U16 _Duty);
extern	U16		SW_PWM_CH_GetDuty(U8 _CH);
extern	U16		SW_PWM_Percent2U16(U8 _Percent);

//nx1_math.c
extern	U8		Get_LS1B_Position(U32 _value);
extern	U8		Get_HS1B_Position(U32 _value);

//_DAC_MODE, compact witch 2nd version interface
#define dacRampUp()             DacMixerRamp_SetRampUp()
#define dacRampDn()             DacMixerRamp_SetRampDown()
#define DAC_AUDIO_Play(a,b,c)   AUDIO_Play(a,b,c)
#define DAC_SBC_Play(a,b)       SBC_Play(a,b)
#define DAC_SBC_CH2_Play(a,b)   SBC_CH2_Play(a,b)
#define DAC_SBC2_Play(a,b)      SBC2_Play(a,b)
#define DAC_SBC2_CH2_Play(a,b)  SBC2_CH2_Play(a,b)
#define DAC_ADPCM_Play(a,b)     ADPCM_Play(a,b)
#define DAC_ADPCM_CH2_Play(a,b) ADPCM_CH2_Play(a,b)
#define DAC_ADPCM_CH3_Play(a,b) ADPCM_CH3_Play(a,b)
#define DAC_ADPCM_CH4_Play(a,b) ADPCM_CH4_Play(a,b)
#define DAC_ADPCM_CH5_Play(a,b) ADPCM_CH5_Play(a,b)
#define DAC_ADPCM_CH6_Play(a,b) ADPCM_CH6_Play(a,b)
#define DAC_ADPCM_CH7_Play(a,b) ADPCM_CH7_Play(a,b)
#define DAC_ADPCM_CH8_Play(a,b) ADPCM_CH8_Play(a,b)
#define DAC_CELP_Play(a,b)      CELP_Play(a,b)
#define DAC_MIDI_Play(a,b)      MIDI_Play(a,b)
#define DAC_InstNote_On(a,b,c)  InstNote_On(a,b,c)
#define DAC_InstNote_Off(a,b)   InstNote_Off(a,b)
#define DAC_PCM_Play(a,b)       PCM_Play(a,b)
#define DAC_PCM_CH2_Play(a,b)   PCM_CH2_Play(a,b)
#define DAC_PCM_CH3_Play(a,b)   PCM_CH3_Play(a,b)
#define DAC_RT_Play()           RT_Play()
#define DAC_WaveID_SendCMD(a)   WaveID_SendCMD(a)

//_AUDIO_RECORD
extern void AudioRecord_Init(U32 addr,U16 sample_rate,U32 size);
extern void AudioRecord_Start(U32 addr,U16 sample_rate,U32 size);
extern void AudioRecord_Stop(void);
extern U8 	AudioRecord_GetStatus(void);
extern void AudioRecord_DacIsr(U8 size, S32 *pdest32);
extern void AudioRecord_ServiceLoop(void);

extern U8  NX1EF_getPwmCurrent();
extern U32 NX1EF_getRollingCode();
extern U16 NX1EF_getPWMCurrentFactor();

extern void STORAGE_ClearOldData(STORAGE_T *pStorage, U8 mode);
extern void STORAGE_LibRecovery(STORAGE_T *pStorage, U32 index, U32 sector_amount, U8 mode);
extern S8   STORAGE_LibSetVar(STORAGE_T *pStorage, U8 *buf, U8 len);
extern void STORAGE_LibSave(STORAGE_T *pStorage, U8 mode);
extern void STORAGE_LibSave2(STORAGE_T *pStorage, U8 mode);
extern void STORAGE_Init(U8 mode,U32 index);
extern S8   STORAGE_SetVar(U8 *buf, U8 len);
extern S8   STORAGE_Save(U8 mode);
extern void STORAGE_NonBlockSectorErase_Init(void);
extern S32  STORAGE_GetNonBlockSectorAddr(void);
extern void STORAGE_NonBlockSectorEraseDone(void);

extern S8 SbcRamEBufCtrl_SetEraseSt(void);
extern S8 SBC_AutoSave_Reset(U8 mode);
extern S8 SBC_AutoSave_GetEraseState(void);

#if _EF_SERIES
#define _ADC_INPUT_MASK (((_GPIO_PA0_MODE==GPIO_OUTPUT_ALT3)<<0)\
                        |((_GPIO_PA1_MODE==GPIO_OUTPUT_ALT3)<<1)\
                        |((_GPIO_PA2_MODE==GPIO_OUTPUT_ALT3)<<2)\
                        |((_GPIO_PA3_MODE==GPIO_OUTPUT_ALT3)<<3)\
                        |((_GPIO_PA4_MODE==GPIO_OUTPUT_ALT3)<<4)\
                        |((_GPIO_PA5_MODE==GPIO_OUTPUT_ALT3)<<5)\
                        |((_GPIO_PA6_MODE==GPIO_OUTPUT_ALT3)<<6)\
                        |((_GPIO_PA7_MODE==GPIO_OUTPUT_ALT3)<<7))
#else
#define _ADC_INPUT_MASK (((_GPIO_PA0_MODE==GPIO_OUTPUT_ALT )<<0)\
                        |((_GPIO_PA1_MODE==GPIO_OUTPUT_ALT )<<1)\
                        |((_GPIO_PA2_MODE==GPIO_OUTPUT_ALT )<<2)\
                        |((_GPIO_PA3_MODE==GPIO_OUTPUT_ALT )<<3)\
                        |((_GPIO_PA4_MODE==GPIO_OUTPUT_ALT2)<<4)\
                        |((_GPIO_PA5_MODE==GPIO_OUTPUT_ALT2)<<5)\
                        |((_GPIO_PA6_MODE==GPIO_OUTPUT_ALT2)<<6)\
                        |((_GPIO_PA7_MODE==GPIO_OUTPUT_ALT2)<<7))
#endif
#define _IS_ADC_INPUT   (_ADC_INPUT_MASK>0)
extern U16 ADC_INPUT_GetValue(U8 _ch);

#endif //#ifndef NX1_LIB_H
