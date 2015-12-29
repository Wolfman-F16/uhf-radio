/*
 * uhf.h
 *
 *  Created on: 16.04.2011
 *      Author: Wolfgang Engelhard
 *
 * Copyright: (c) 2011 by Wolfgang Engelhard
 * License: GNU GPL v2 (see License.txt)
 */

#ifndef UHF_H_
#define UHF_H_

#include <inttypes.h>

#include "main.h"
#include "timer.h"

/****************************   DEFINITIONS           ************************/
#define ASCII_OFFSET    0x30

#define INPUT0          PC0
#define INPUT1          PC1
#define INPUT2          PC2
#define INPUT3          PC3
#define INPUT4          PC4
#define INPUT5          PC5
#define INPUT6          PC6
#define INPUT7          PC7
#define INPUT8          PA4
#define INPUT9          PA3

#define COMMON_WIRE_00   0
#define COMMON_WIRE_01   1
#define COMMON_WIRE_02   2
#define COMMON_WIRE_03   3
#define COMMON_WIRE_04   4
#define COMMON_WIRE_05   5
#define COMMON_WIRE_06   6
#define COMMON_WIRE_07   7
#define COMMON_WIRE_08   8
#define COMMON_WIRE_09   9
#define COMMON_WIRE_10  10
#define COMMON_WIRE_11  11
#define COMMON_WIRE_12  12
#define COMMON_WIRE_13  13
#define COMMON_WIRE_14  14
#define COMMON_WIRE_15  15

#define KEY_DDR         DDRC
#define KEY_PORT        PORTC
#define KEY_PIN         PINC
#define KEY_ADD_PIN     PINA
#define KEY0            0
#define KEY1            1
#define KEY2            2
#define KEY3            3
#define KEY4            4
#define KEY5            5
#define KEY6            6
#define KEY7            7

#define ALL_KEYS        (1<<KEY0 | 1<<KEY1 | 1<<KEY2 | 1<<KEY3 | \
                         1<<KEY4 | 1<<KEY5 | 1<<KEY6 | 1<<KEY7 )

#define NR_OF_KEYROWS   16

#define KEY_MASK        ~ALL_KEYS               // key mask must be inverted#define KEY_ADD_MASK    ~(1<<KEY3 | 1<<KEY4)
#define KEY_ADD_ROW     COMMON_WIRE_05
#define MASK_TEST       0b10000000              // masks test button#define MASK_SQUELCH    0b10000000              // masks squelch switch#define MASK_STATUS     0b00010000              // masks status button#define MASK_UHF_IO     0b10000000
#define MASK_BATTERY    0b00000010
#define MASK_BACKUP     0b00000001

#define REPEAT_MASK     (1<<KEY0)               // repeat: key0 (test)#define REPEAT_START    50                      // after 500ms#define REPEAT_NEXT     20                      // every 200ms#define MASK_CH_01      0b00000010
#define MASK_CH_02      0b00000110
#define MASK_CH_03      0b00000100
#define MASK_CH_04      0b00001100
#define MASK_CH_05      0b00001110
#define MASK_CH_06      0b00001010
#define MASK_CH_07      0b00001000
#define MASK_CH_08      0b00011000
#define MASK_CH_09      0b00011010
#define MASK_CH_10      0b00011110
#define MASK_CH_11      0b00011100
#define MASK_CH_12      0b00010100
#define MASK_CH_13      0b00010110
#define MASK_CH_14      0b00010010
#define MASK_CH_15      0b00010000
#define MASK_CH_16      0b00110000
#define MASK_CH_17      0b00110010
#define MASK_CH_18      0b00110110
#define MASK_CH_19      0b00110100
#define MASK_CH_20      0b00111100
#define MASK_CHANNEL    (MASK_CH_20 | MASK_CH_01)

#define MAX_CHANNELS    20
#define CHAN_ARRAY_SIZE 1
#define FREQ_ARRAY_SIZE 6

#define  SPI_SOF        0x1A
#define  SPI_EOF        0xA5

#define MAX_MWOD    6
#define MAX_FMT     16
#define MAX_DOM     2

#define eeUhfState eePresetFrequencies[MAX_CHANNELS][FREQ_ARRAY_SIZE]
#define eeMnlFreq  eePresetFrequencies[MAX_CHANNELS]

/****************************   Structures      *******************************/
/**
 * display content. channel is stored as hex, frequency as ASCII string.
 */
typedef struct {
  uint8_t channel;
  uint8_t frequency[FREQ_ARRAY_SIZE];
} radio_t;

/**
 * MPG switch status
 */
typedef struct {
  unsigned FUSEL :3;
  unsigned OFF :1;
  unsigned MODE :3;
  unsigned BACKUP :1;
} uhfKeyState_t;

/**
 * uhf control structure
 */
typedef struct {
  unsigned audio :3;
  unsigned audioCycle :3;

  unsigned test :1;
  unsigned squelch :1;

  unsigned invalidSWOD :3;
  unsigned gotTone :1;
  unsigned ch20 :1;

  unsigned resend :1;
  unsigned firstRun :1;
  unsigned fmtTimeout :1;
} uhfControl_t;

/**
 * MWOD structure used for ring buffer
 */
typedef struct {
  uint8_t ch_20[FREQ_ARRAY_SIZE];
  uint8_t ch_19[FREQ_ARRAY_SIZE];
  uint8_t ch_18[FREQ_ARRAY_SIZE];
  uint8_t ch_17[FREQ_ARRAY_SIZE];

  uint8_t ch_16[FREQ_ARRAY_SIZE];
  uint8_t ch_15[FREQ_ARRAY_SIZE];
  uint8_t ch_14[2];
} uhf_wod_t;

/**
 * fmt change
 */
typedef struct {
  uint8_t ch[MAX_FMT][FREQ_ARRAY_SIZE];
} uhf_fmt_t;

/****************************   ENUMS           *******************************/
typedef enum {
  VEROP, MLOAD, ERASE, FMTCHG, DISP_OFF
} uhfOpMode_e;

typedef enum {
  BEEP, WAVE, PULSE, TOD, DOUBLE, WARN
} tone_e;

typedef enum {
  FS_MNL, FS_PRESET, FS_GUARD
} functionSelector_e;

enum modeSelector_e {
  MS_OFF, MS_ADF, MS_MAIN, MS_BOTH
};

enum offSelector_e {
  MS_ON = 1
};

/****************************   GLOBAL VARIABLES ******************************/
// extern eeprom variables
extern const uint8_t eePresetFrequencies[MAX_CHANNELS][FREQ_ARRAY_SIZE];
// extern variables
extern uhfKeyState_t uhfKeyState;
extern uhfOpMode_e uhfOpMode;
extern const uint8_t* const uhfStatusStrings[];
extern uhf_wod_t uhfMWOD[];
extern uint8_t currMWodIndex;
extern uhf_wod_t uhfSWOD;
extern uhf_fmt_t uhfFMT;
extern const uint8_t eeFmtNets[MAX_FMT][FREQ_ARRAY_SIZE];
extern uint8_t currDayOfMonth[];        // current day of month
extern radio_t radio;
extern uint8_t isUsbPresent;

// ISR values
extern volatile uhfControl_t uhfControl;
static volatile uint8_t upperByte;

/****************************   FUNCTION DECLARATIONS *************************/
void processInputData(uint8_t);
void processChannel(void);
void display(uhfOpMode_e);
void playTone(tone_e);

void initVariables(void);
void sendData(uint8_t*, uint8_t*);

void eraseMWOD(void);

void setManual(void);
void setPreset(void);
void setGuard(void);

void setTest(void);
void setLoad(void);
void setStatus(void);

void setToneT(void);
void setToneTone(void);

void setMain(void);
void setBoth(void);
void setAdf(void);
void setUhfOn(void);
void setUhfOff(void);

void goActive(void);

void checkDom(void);

void forcedReadUhfState(void);
void forcedReadFreq(void);

uint8_t* getChannelString(void);
uint8_t* getFrequencyString(void);

/****************************   Inline Functions      *************************/
static inline void isrTimer1() {
  // not used during audio output
  TCCR1B = 0;
  uhfControl.test = FALSE;
  uhfControl.resend = TRUE;
#ifdef deactivated
  uhfControl.fmtTimeout = TRUE;
#endif
}

static inline void isrTimer2() {

  if (upperByte > 0) {
    upperByte--;
  } else {
    // test type of tone to be played
    // if beep then this
    if (uhfControl.audio == BEEP) {
      TCCR2 = 0;
      disablePWM1A();
    }
    /* TOD */
    if (uhfControl.audio == TOD) {
      setT1Pwm(FREQ_1KHZ);
      enablePWM1A();
      setT2Delay(ONE_SEC);
      uhfControl.audio = BEEP;
    }
    /* wave  */
    if (uhfControl.audio == WAVE) {
      switch (uhfControl.audioCycle) {
      case 3:
      case 1:
        setT1Pwm(FREQ_1KHZ);
        enablePWM1A();
        setT2Delay(ONE_SEC);
        uhfControl.audioCycle--;
        break;
      case 2:
        setT1Pwm(FREQ_500HZ);
        enablePWM1A();
        setT2Delay(ONE_SEC);
        uhfControl.audioCycle--;
        break;
      default:
        TCCR2 = 0;
        disablePWM1A();
        break;

      }
    }
    /* double beep */
    if (uhfControl.audio == DOUBLE) {
      switch (uhfControl.audioCycle) {
      case 2:
        disablePWM1A();
        setT2Delay(ONE_SEC);
        uhfControl.audioCycle--;
        break;
      case 1:
        setT1Pwm(FREQ_1KHZ);
        enablePWM1A();
        setT2Delay(ONE_SEC);
        uhfControl.audioCycle--;
        break;
      default:
        TCCR2 = 0;
        disablePWM1A();
        break;
      }
    }
    /* pulse */
    if (uhfControl.audio == PULSE) {
      switch (uhfControl.audioCycle) {
      case 4:
      case 2:
        // no sound for one sec
        disablePWM1A();
        setT2Delay(HALF_SEC);
        uhfControl.audioCycle--;
        break;
      case 3:
      case 1:
        setT1Pwm(FREQ_1KHZ);
        enablePWM1A();
        setT2Delay(HALF_SEC);
        uhfControl.audioCycle--;
        break;
      default:
        TCCR2 = 0;
        disablePWM1A();
        break;
      }
    }
  }
}

static inline void isrAdc() {
  ;
}

#endif /* UHF_H_ */
