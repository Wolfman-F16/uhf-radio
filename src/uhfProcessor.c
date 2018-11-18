/*
 * uhfProcessor.c
 *
 *  Created on: 19.04.2011
 *      Author: Wolfgang Engelhard
 *
 * Copyright: (c) 2011 by Wolfgang Engelhard
 * License: GNU GPL v2 (see License.txt)
 */
#include <avr/pgmspace.h>
#include <string.h>

#include "main.h"
#include "uhf.h"
#include "uhfVersion.h"
#include "debug.h"
#include "key.h"
#include "timer.h"
#include "serial.h"
#include "eeprom.h"
#include "spi.h"
#include "usbKeyDefs.h"
#include "usb.h"

/****************************   FLASH CONSTANTS       ************************/
const uint8_t sendChanPre[] PROGMEM = "\033[5;34f\0";
const uint8_t sendFreqPre[] PROGMEM = "\033[6;30f\0";
const uint8_t eepromMsg[] PROGMEM = "\033[15;20f\033[2Kwriting to eeprom...\0";
const uint8_t eraseMWodMsg[] PROGMEM = "\033[12;20f\033[2Kerasing M-WOD\0";
const uint8_t toneReceived[] PROGMEM = "\033[11;20f\033[2KTone received\0";

const uint8_t veropFreq[] PROGMEM = "220000";
const uint8_t guardFreq[] PROGMEM = "243000";
const uint8_t toneFreq[] PROGMEM = "310425";
const uint8_t testFreq[] PROGMEM = "888888";

/****************************   GLOBAL VARIABLES      ************************/
uhf_wod_t uhfMWOD[MAX_MWOD];
uhf_fmt_t uhfFMT;
uint8_t currMWodIndex = 0;
uint8_t channelMemory;

/****************************   FUNCTION DEFINITIONS  ************************/
/*
 * return readChannel as string
 */
uint8_t* getChannelString() {
  static uint8_t channelString[] = { 0, 0 };
  uint8_t tmpChannel = radio.channel;

  if (radio.channel == 0) {
    channelString[0] = ' ';
    channelString[1] = ' ';
  } else {
    channelString[1] = '0' + tmpChannel % 10;
    tmpChannel /= 10;
    channelString[0] = '0' + tmpChannel;
  }
  return &(channelString[0]);
}

/*
 * return readChannel as string
 */
uint8_t* getFrequencyString() {
  return &(radio.frequency[0]);
}

void eraseMWOD() {
  uint8_t i = 0;
  /* erase mwod
   * play a tone?
   * erase mwod day to invalidate complete mwod set
   */
  for (i = 0; i < MAX_MWOD; i++) {
    uhfMWOD[i].ch_14[0] = 0x55;
    uhfMWOD[i].ch_14[1] = 0x55;
  }
  DBG_WARN_P(eraseMWodMsg);
}

/*
 * sets manual state for function selector.
 */
void setManual() {
  sendUsbMessage(KEY_MNL);          /* not supported by BMS 4.32 */
  uhfKeyState.FUSEL = FS_MNL;
  radio.channel = 0;                /* blank channel display */
  forcedReadFreq();
}

/*
 * sets preset state for function selector.
 */
void setPreset() {
  sendUsbMessage(KEY_PRESET);
  uhfKeyState.FUSEL = FS_PRESET;
  readChannel();
  radio.channel = channelMemory;
}

/*
 * sets guard state for function selector.
 */
void setGuard() {
  sendUsbMessage(KEY_GUARD);
  uhfKeyState.FUSEL = FS_GUARD;
  radio.channel = 0;                /* blank channel display */
                                    /* set guard frequency */
  memcpy_P(radio.frequency, guardFreq, FREQ_ARRAY_SIZE);
}

/**
 * sets main mode
 */
void setMain() {
  if (uhfKeyState.MODE == MS_BOTH) {
    sendUsbMessage(KEY_MAIN);
  }
  uhfKeyState.MODE = MS_MAIN;
}

/**
 * sets both mode
 */
void setBoth() {
  sendUsbMessage(KEY_BOTH);
  uhfKeyState.MODE = MS_BOTH;
}

/**
 * sets adf mode. this turns display off.
 */
void setAdf() {
  uhfKeyState.MODE = MS_ADF;
}

/**
 * turns display off
 */
void setUhfOff() {
  sendUsbMessage(KEY_OFF);
  TCCR1B = 0;
  display(DISP_OFF);
}

/**
 * turns display on.
 * if adf was previous state, then display is just turned on, otherwise there is a test sequence.
 */
void setUhfOn() {
  forcedReadFreq();
  if (uhfKeyState.MODE != MS_ADF) {
    setTest();
    /* disable inputs */
    setT1Delay(ONE_SEC);        /* 1 second init test */
    while (uhfControl.test) {
      ;
    }
  } else {
    uhfControl.resend = TRUE;
  }
}

/**
 * find tone
 */
void setToneT() {
  uint8_t i = 0;

  if (uhfOpMode == VEROP) {
    if (memcmp_P((uint8_t*) &radio.frequency, toneFreq, FREQ_ARRAY_SIZE) == 0) {
      /* if tone is received play beep
         maybe add a random delay? within 10 sec
       */
      for (i = 0; i < 0xFF; i++)
        asm("nop");
      playTone(BEEP);
      uhfControl.gotTone = TRUE;
    } else {
      uhfControl.gotTone = FALSE;
    }
    if (uhfKeyState.MODE == MS_ADF) {
      sendData((uint8_t*) chVersion, (uint8_t*) freqVersion);
    }
  }
}

/**
 *
 */
void setToneTone() {
  if (uhfKeyState.FUSEL == FS_MNL) {
    switch (uhfOpMode) {
    case MLOAD: {
      switch (radio.channel) {
      case 1:
        currDayOfMonth[0] = radio.frequency[1];
        currDayOfMonth[1] = radio.frequency[2];
        DBG_INFO((currDayOfMonth[0]),1);
        DBG_INFO((currDayOfMonth[1]),1);
        playTone(WAVE);
        break;
      case 14:
        memcpy(&(uhfMWOD[currMWodIndex].ch_14[0]), &(radio.frequency[1]),
            MAX_DOM);
        playTone(BEEP);
        currMWodIndex++;
        if (currMWodIndex >= MAX_MWOD) {
          currMWodIndex = 0;
        }
        playTone(PULSE);
        uhfControl.audioCycle = 2;
        break;
      case 15:
        memcpy(&(uhfMWOD[currMWodIndex].ch_15[0]), &(radio.frequency[0]),
            FREQ_ARRAY_SIZE);
        playTone(BEEP);
        break;
      case 16:
        memcpy(&(uhfMWOD[currMWodIndex].ch_16[0]), &(radio.frequency[0]),
            FREQ_ARRAY_SIZE);
        playTone(BEEP);
        break;
      case 17:
        memcpy(&(uhfMWOD[currMWodIndex].ch_17[0]), &(radio.frequency[0]),
            FREQ_ARRAY_SIZE);
        playTone(BEEP);
        break;
      case 18:
        memcpy(&(uhfMWOD[currMWodIndex].ch_18[0]), &(radio.frequency[0]),
            FREQ_ARRAY_SIZE);
        playTone(BEEP);
        break;
      case 19:
        memcpy(&(uhfMWOD[currMWodIndex].ch_19[0]), &(radio.frequency[0]),
            FREQ_ARRAY_SIZE);
        playTone(BEEP);
        break;
      case 20:
        memcpy(&(uhfMWOD[currMWodIndex].ch_20[0]), &(radio.frequency[0]),
            FREQ_ARRAY_SIZE);
        playTone(BEEP);
        break;
      default:
        break;
      }
    }
      break;
    case ERASE:
      eraseMWOD();
      break;
    case FMTCHG:
      /* arc-164.pdf p. 82 */
      /* check for ch 20 - 5 */
      if (radio.channel > 4) {
        memcpy(uhfFMT.ch[MAX_CHANNELS - radio.channel], &radio.frequency,
            FREQ_ARRAY_SIZE);
        eeprom_write_block(&(radio.frequency[0]),
                           (uint8_t*)&(eeFmtNets[MAX_CHANNELS - radio.channel][0]),
                           FREQ_ARRAY_SIZE);
        DBG_WARN_P(eepromMsg);
        playTone(BEEP);
      }
      break;
    case VEROP:
      if (uhfControl.gotTone) {
        playTone(TOD);
      } else {
        playTone(BEEP);
      }
      break;
    default:
      break;
    }
  }
  if (uhfKeyState.FUSEL == FS_PRESET) {
    if (uhfOpMode == FMTCHG) {
      /* nothing to implement found yet */
    }
  }
}

/**
 * load button functions
 */
void setLoad() {
  if (memcmp_P(&(radio.frequency[0]), veropFreq, 4) == 0) {
    if (uhfKeyState.FUSEL == FS_PRESET && radio.channel == 20) {
      /* verop */
      if (radio.frequency[4] == '0') {
        uhfOpMode = VEROP;
      }
      /* mload */
      if (radio.frequency[4] == '2') {
        uhfOpMode = MLOAD;
      }
      /* erase */
      if (radio.frequency[4] == '5') {
        uhfOpMode = ERASE;
      }
      /* fmtchg */
      if (radio.frequency[4] == '7') {
        uhfOpMode = FMTCHG;
      }
      display(uhfOpMode);
    }
  } else {
    if (uhfOpMode == VEROP && uhfKeyState.FUSEL == FS_PRESET) {
      writePresetFreq2eeprom(&radio);
      DBG_WARN_P(eepromMsg);
    }
    /* illegal op. ch 20 is reserved for mwod
       but channel 20 holds training/common net!? Apache SWOD loading states,
       you program ch 20 with training net freq 300.0xx
     */
  }
}

/**
 * op mode is shown with ch 20 selected. if preset is set and radio is in
 * normal mode (verop) then status loads frequency for selected channel
 */
void setStatus() {
  if (uhfKeyState.FUSEL == FS_PRESET && uhfOpMode == VEROP) {
    if (radio.frequency[0] == 'A') {
      if (radio.channel > 4) {
        memcpy(&(radio.frequency[0]),
               &(uhfFMT.ch[MAX_CHANNELS - radio.channel][0]),
               FREQ_ARRAY_SIZE);
      }
    } else {
      eeprom_read_block(&(radio.frequency[0]),
                        &(eePresetFrequencies[radio.channel - 1][0]),
                        FREQ_ARRAY_SIZE);
    }
  }
  if (radio.channel == 20) {
    display(uhfOpMode);
  } else {
    uhfControl.resend = TRUE;
  }
}

/**
 * sends test display sequence via SPI
 */
void setTest() {
  uint8_t testString[] = "888888";
  if (uhfControl.test != TRUE) {
    uhfControl.test = TRUE;
#if DEBUG_LEVEL > 0
    sendUartString_P((uint8_t*) sendChanPre);
    sendUartString_n(testString, CHAN_ARRAY_SIZE + 1);
    sendUartString_P((uint8_t*) sendFreqPre);
    sendUartString_n(testString, FREQ_ARRAY_SIZE);
#endif
#if _SPI_EN_ > 0
    startSpiTransmission();
    sendSpiByte(SPI_SOF);
    sendSpiString(testString, CHAN_ARRAY_SIZE + 1);
    sendSpiString(testString, FREQ_ARRAY_SIZE);
    sendSpiByte(SPI_EOF);
    endSpiTransmission();
#endif
  }
}

/*
 * selects display TEXT from uhfStatusStrings and sends it
 * in a message package via SPI
 */
void display(uhfOpMode_e pMode) {
  uint8_t chanString[] = { ' ', ' ' };
  uint8_t freqString[] = { ' ', ' ', ' ', ' ', ' ', ' ' };
  PGM_P stringPointer;

  if (pMode != DISP_OFF) {
    chanString[0] = getChannelString()[0];
    chanString[1] = getChannelString()[1];
  }

  memcpy_P(&stringPointer, &(uhfStatusStrings[pMode]), sizeof(PGM_P));
  strcpy_P((char*) freqString, stringPointer);

  sendData(chanString, freqString);

  if (pMode != DISP_OFF) {
    setT1Delay(MAX_TIME); /* limit display duration */
  }
}

/*
 * sends data to SPI interface. if debug is enabled, it also sends data to
 * USART.
 */
void sendData(uint8_t* channel, uint8_t* frequency) {
#if DEBUG_LEVEL > 0
  sendUartString_P((uint8_t*) sendChanPre);
  sendUartString_n(channel, CHAN_ARRAY_SIZE + 1);
  sendUartString_P((uint8_t*) sendFreqPre);
  sendUartString_n(frequency, FREQ_ARRAY_SIZE);
#endif
#if _SPI_EN_ > 0
  /* SPI transmission */
  startSpiTransmission();
  sendSpiByte(SPI_SOF);
  sendSpiString(channel, CHAN_ARRAY_SIZE + 1);
  sendSpiString(frequency, FREQ_ARRAY_SIZE);
  sendSpiByte(SPI_EOF);
  endSpiTransmission();
#endif
}

/**
 *
 */
void playTone(tone_e audioType) {
  switch (audioType) {
  case BEEP:
    setT1Pwm(FREQ_1KHZ);
    enablePWM1A();
    setT2Delay(ONE_SEC);
    uhfControl.audio = BEEP;
    break;
  case DOUBLE:
    setT1Pwm(FREQ_1KHZ);
    enablePWM1A();
    setT2Delay(ONE_SEC);
    uhfControl.audio = DOUBLE;
    uhfControl.audioCycle = 2;
    break;
  case WAVE:
    setT1Pwm(FREQ_500HZ);
    enablePWM1A();
    setT2Delay(ONE_SEC);
    uhfControl.audio = WAVE;
    uhfControl.audioCycle = 3;
    /* have to set in isr of t2 option to repeat this with 0.5 khz
       max duration is 5 sec? */
    break;
  case PULSE:
    setT1Pwm(FREQ_1KHZ);
    enablePWM1A();
    setT2Delay(HALF_SEC);
    uhfControl.audio = PULSE;
    uhfControl.audioCycle = 4;
    /* should be one sec and one sec pause for 5 sec total?
       p.77 hints, that pulse is beep, beep, beep */
    break;
  case TOD:
    setT1Pwm(FREQ_1666Hz);
    enablePWM1A();
    setT2Delay(ONE_SEC);
    uhfControl.audio = TOD;
    break;
  case WARN:
    setT1Pwm(FREQ_1KHZ);
    enablePWM1A();
    setT2Delay(FOUR_SECS);
    uhfControl.audio = BEEP;
    break;
  default:
    disablePWM1A();
    break;
  }
}

/**
 * check word of day exist for day of month
 */
void checkDom() {
  uint8_t i = 0;

  if (uhfOpMode
      == VEROP&& uhfKeyState.FUSEL == FS_MNL && uhfControl.ch20 == TRUE) {
    for (i = 0; i < MAX_MWOD; i++) {
      if (radio.frequency[1] == uhfMWOD[i].ch_14[0]
          && radio.frequency[2] == uhfMWOD[i].ch_14[1]) {
        playTone(BEEP);
        break;
      }
      DBG_WARN("\r\nChecked ", 10);
      DBG_WARN(&uhfMWOD[i].ch_14[0], 1);
      DBG_WARN(&uhfMWOD[i].ch_14[1], 1);
    }
  }
}

/**
 * pulse if no net entered for net number, or net number ends with 75
 * beep for 4sec (steady warning tone) if no tod or no wod (dom check failed)
 *
 */
void goActive() {
  uint8_t commonNet[FREQ_ARRAY_SIZE];
  uint8_t match = 0;
  uint8_t netNumber = 0;

  if (uhfOpMode != VEROP) {
    radio.frequency[0] = '3';
    return;
  }

  if (!uhfControl.gotTone) {
    /* wod or tod invalid (steady) */
    playTone(WARN);
    DBG_WARN("X tone", 6);
    return;
  }
  /* active net ending with 50 or 75 not allowed (Apache HQ IIa) */
  if (radio.frequency[4] > '2') {
    playTone(PULSE);
    DBG_WARN("X >.25", 6);
    return;
  }
  if (memcmp(&(radio.frequency[0]), "A0", 2)) {
    playTone(PULSE);
    DBG_WARN("X >15", 5);
    return;
  }
  netNumber = ((radio.frequency[2] - 0x30) * 10) + (radio.frequency[3] - 0x30);
  /* check for training or combat mode net */
  eeprom_read_block(&(commonNet[0]),
                    &(eePresetFrequencies[MAX_CHANNELS - 1][0]),
                    FREQ_ARRAY_SIZE);
  match = memcmp(&(commonNet[0]), "3000", 4);
  if (match) {
    /* combat mode
       classified ? */
  } else {
    /* training mode */
    if (radio.frequency[4] == '0') {
      /* small range hop (Axx.x00) */
      if (netNumber < 5) {
        /* selected net is one of presetChannel 19 to 15 (SWOD). */
        /* There is always a frequency for SWOD, but maybe not valid */
        if (uhfControl.invalidSWOD) {
          playTone(WARN);
          DBG_WARN("X SWOD", 6);
        }
        /* active, no audio */
        return;
      }
    } else {
      /* fmt net range hop (Axx.x25) */
      if (netNumber < 16) {
        /* active, no audio (only permissible, because FMT-NETs are */
        /* loaded non-volatile in eeprom) */
        return;
      }
    }
    playTone(PULSE); /* error */
  }
}
