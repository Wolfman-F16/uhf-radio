/*
 * uhf.c
 *
 *  Created on: 16.04.2011
 *      Author: Wolfgang Engelhard
 *
 * Copyright: (c) 2011 by Wolfgang Engelhard
 * License: GNU GPL v2 (see License.txt)
 */

#include "uhf.h"
#include "key.h"
#include "serial.h"
#include "terminal.h"
#include "eeprom.h"
#include "debug.h"
#include "usbKeyDefs.h"
#include "usb.h"

/****************************   GLOBAL VARIABLES      ************************/
uhfKeyState_t uhfKeyState;
uhfOpMode_e uhfOpMode;
radio_t radio;

extern const uint8_t eepromMsg[];
extern uint8_t channelMemory;
/****************************   CONSTANTS             ************************/
#if DEBUG_LEVEL > 1
const uint8_t test_pressed[] PROGMEM = "TEST pressed\r\n\0";
const uint8_t adf_pressed[] PROGMEM = "ADF selected\r\n\0";
const uint8_t tone_pressed[] PROGMEM = "TONE selected\r\n\0";
const uint8_t squelch_pressed[] PROGMEM = "SQUELCH off\r\n\0";
const uint8_t squelch_depressed[] PROGMEM = "SQUELCH on\r\n\0";

const uint8_t guard_pressed[] PROGMEM = "GUARD selected\r\n\0";
const uint8_t both_pressed[] PROGMEM = "BOTH selected\r\n\0";
const uint8_t t_pressed[] PROGMEM = "\rT selected\r\n\0";
const uint8_t zeroize_pressed[] PROGMEM = "ZEROIZE selected\r\n\0";

const uint8_t preset_pressed[] PROGMEM = "\rPRESET selected\r\n\0";
const uint8_t main_pressed[] PROGMEM = "MAIN selected\r\n\0";
const uint8_t status_pressed[] PROGMEM = "STATUS pressed\r\n\0";
const uint8_t battery_pressed[] PROGMEM = "BATTERY power off\r\n\0";

const uint8_t mnl_pressed[] PROGMEM = "MNL selected\r\n\0";
const uint8_t off_pressed[] PROGMEM = "OFF selected\r\n\0";
const uint8_t load_pressed[] PROGMEM = "LOAD pressed\r\n\0";
const uint8_t backup_pressed[] PROGMEM = "UHF BACKUP selected\r\n\0";

#endif

/****************************   FUNCTION DECLARATIONS ************************/
uint8_t getAdditionalKeys(uint8_t);

/****************************   FUNCTION DEFINITION   ************************/
void processInputData(uint8_t keyStatus) {

#ifdef deactivated
  static uint8_t fmtTimerStarted = FALSE;
  //debug
  uint8_t dbgOnce = TRUE;
  // end debug
#endif
  uint8_t addKeyStatus = 0;

  // key press
  switch (currentKeyRow) {
  case COMMON_WIRE_00:
    switch (keyStatus) {
    case (1 << KEY1):
      radio.frequency[0] = '-';
      uhfControl.resend = TRUE;
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY2):
      radio.frequency[1] = '5';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_2_5);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY3):
      radio.frequency[2] = '5';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_3_5);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY4):
      radio.frequency[3] = '5';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_4_5);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    default:
      break;
    }
    break;
    /***** next common wire *****/
  case COMMON_WIRE_01:
    switch (keyStatus) {
    case (1 << KEY1):
      radio.frequency[0] = '-';
      uhfControl.resend = TRUE;

      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY2):
      radio.frequency[1] = '4';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_2_4);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY3):
      radio.frequency[2] = '4';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_3_4);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY4):
      radio.frequency[3] = '4';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_4_4);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    default:
      break;
    }
    break;
    /***** next common wire *****/
  case COMMON_WIRE_02:
    switch (keyStatus) {
    case (1 << KEY1):
      radio.frequency[0] = 'A';
      uhfControl.resend = TRUE;
      goActive();
      sendUsbMessage(KEY_1_4);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY2):
      radio.frequency[1] = '3';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_2_3);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY3):
      radio.frequency[2] = '3';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_3_3);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY4):
      radio.frequency[3] = '3';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_4_3);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY5):
      radio.frequency[4] = '7';
      radio.frequency[5] = '5';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_5_75);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    default:
      break;
    }
    break;
    /***** next common wire *****/
  case COMMON_WIRE_03:
    switch (keyStatus) {
    case (1 << KEY1):
      radio.frequency[0] = '3';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_1_3);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY2):
      radio.frequency[1] = '2';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_2_2);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY3):
      radio.frequency[2] = '2';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_3_2);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY4):
      radio.frequency[3] = '2';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_4_2);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY5):
      radio.frequency[4] = '5';
      radio.frequency[5] = '0';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_5_50);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY6):
      setManual();                      // set program state to MNL
      DBG_INFO_P(mnl_pressed);
      break;
    default:
      break;
    }
    break;
    /***** next common wire *****/
  case COMMON_WIRE_04:
    switch (keyStatus) {
    case (1 << KEY0):
      setToneT();
      DBG_INFO_P(t_pressed);
      break;
    case (1 << KEY1):
      radio.frequency[0] = '2';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_1_2);
      DBG_INFO(radio.frequency, 6);
      break;
    case (1 << KEY2):
      radio.frequency[1] = '1';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_2_1);
      DBG_INFO(radio.frequency, 6);
      break;
    case (1 << KEY3):
      radio.frequency[2] = '1';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_3_1);
      DBG_INFO(radio.frequency, 6);
      break;
    case (1 << KEY4):
      radio.frequency[3] = '1';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_4_1);
      DBG_INFO(radio.frequency, 6);
      break;
    case (1 << KEY5):
      radio.frequency[4] = '2';
      radio.frequency[5] = '5';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_5_25);
      DBG_INFO(radio.frequency, 6);
      break;
    case (1 << KEY6):
      setPreset();                      // set program state to PRESET
      uhfControl.resend = TRUE;
      DBG_INFO_P(preset_pressed);
      break;
    default:
      break;
    }
    break;
    /***** next common wire *****/
  case COMMON_WIRE_05:
    switch (keyStatus) {
    case (1 << KEY0):
      setToneTone();
      DBG_INFO_P(tone_pressed);
      break;
    case (1 << KEY1):
      radio.frequency[0] = '-';
      uhfControl.resend = TRUE;
      DBG_INFO(radio.frequency, 6);
      break;
    case (1 << KEY2):
      radio.frequency[1] = '0';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_2_0);
      DBG_INFO(radio.frequency, 6);
      break;
    case (1 << KEY3):
      radio.frequency[2] = '0';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_3_0);
      DBG_INFO(radio.frequency, 6);
      break;
    case (1 << KEY4):
      radio.frequency[3] = '0';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_4_0);
      DBG_INFO(radio.frequency, 6);
      break;
    case (1 << KEY5):
      radio.frequency[4] = '0';
      radio.frequency[5] = '0';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_5_00);
      DBG_INFO(radio.frequency, 6);
      break;
    case (1 << KEY6):
      setGuard();                       // set guard state incl. frequency
      uhfControl.resend = TRUE;         // update frequency
      DBG_INFO_P(guard_pressed);
      break;
    case (1 << KEY7):
      if (uhfControl.test != TRUE && uhfKeyState.OFF != MS_OFF) {
        setTest();
      }
      DBG_INFO_P(test_pressed);
      break;
    default:
      break;
    }
    // test depressed, doesn't work, because test flag is deleted
    // by reading value
    if (!(keyStatus & MASK_TEST) && uhfControl.test) {
      uhfControl.test = FALSE;
      uhfControl.resend = TRUE;
    }
    break;
    /***** next common wire *****/
  case COMMON_WIRE_06:
    switch (keyStatus) {
    case (1 << KEY1):
      radio.frequency[0] = '-';
      uhfControl.resend = TRUE;
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY2):
      radio.frequency[1] = '9';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_2_9);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY3):
      radio.frequency[2] = '9';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_3_9);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY4):
      radio.frequency[3] = '9';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_4_9);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    default:
      break;
    }
    break;
    /***** next common wire *****/
  case COMMON_WIRE_07:
    switch (keyStatus) {
    case (1 << KEY1):
      radio.frequency[0] = '-';
      uhfControl.resend = TRUE;
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY2):
      radio.frequency[1] = '8';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_2_8);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY3):
      radio.frequency[2] = '8';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_3_8);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY4):
      radio.frequency[3] = '8';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_4_8);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    default:
      break;
    }
    break;
    /***** next common wire *****/
  case COMMON_WIRE_08:
    switch (keyStatus) {
    case (1 << KEY1):
      radio.frequency[0] = '-';
      uhfControl.resend = TRUE;
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY2):
      radio.frequency[1] = '7';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_2_7);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY3):
      radio.frequency[2] = '7';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_3_7);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY4):
      radio.frequency[3] = '7';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_4_7);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    default:
      break;
    }
    break;
    /***** next common wire *****/
  case COMMON_WIRE_09:
    switch (keyStatus) {
    case (1 << KEY1):
      radio.frequency[0] = '-';
      uhfControl.resend = TRUE;
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY2):
      radio.frequency[1] = '6';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_2_6);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY3):
      radio.frequency[2] = '6';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_3_6);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    case (1 << KEY4):
      radio.frequency[3] = '6';
      uhfControl.resend = TRUE;
      sendUsbMessage(KEY_4_6);
      DBG_INFO((uint8_t*) radio.frequency, 6);
      break;
    default:
      break;
    }
    break;
  case COMMON_WIRE_10:
    switch (keyStatus) {
    case (1 << KEY0):
      setLoad();
      DBG_INFO_P(load_pressed);
      break;
    default:
      break;
    }
    break;
    /***** next common wire *****/
  case COMMON_WIRE_11:
    switch (keyStatus) {
    case (1 << KEY7):
      if (uhfKeyState.OFF == MS_OFF) {
        if (uhfKeyState.MODE != MS_ADF) { /* too many calls of setUhfOn() so */
          sendUsbMessage(KEY_MAIN); /* key stroke is separate here     */
          /* instead of within setUhfOn()    */
          sendUsbMessage(NOT_USED); /* only one key stroke should be   */
          /* sent, so release here           */
        }
        setUhfOn();
        uhfKeyState.OFF = MS_ON;
      }
      break;
    default:
      if (keyStatus) {
        sendUartString_n((uint8_t*) "\nc_w_11\t", 8);
        sendUartHex(keyStatus);
      }
      break;
    }
    // UHF Mode (sort of) depressed, doesn't work, because UHF mode flag is
    // deleted by reading value
    if (!(keyStatus & MASK_UHF_IO) && (uhfKeyState.OFF == MS_ON)) {
      uhfKeyState.OFF = MS_OFF;
      setUhfOff();
    }
    break;
    /***** next common wire *****/
  case COMMON_WIRE_12:
    switch (keyStatus) {
    case (1 << KEY7):
      if (uhfControl.squelch) {
        uhfControl.squelch = FALSE;
        DBG_INFO_P(squelch_depressed);
      }
      break;
    default:
      break;
    }
    // squelch depressed, doesn't work, because squelch flag is deleted by
    // reading value
    if (!(keyStatus & MASK_SQUELCH) && uhfControl.squelch != TRUE) {
      uhfControl.squelch = TRUE;
      DBG_INFO_P(squelch_pressed);
    }
    break;
    /***** next common wire *****/
  case COMMON_WIRE_13:
    switch (keyStatus) {
    case (1 << KEY0):
      DBG_INFO_P(adf_pressed);
      setAdf();
      break;
    case (1 << KEY6):
      DBG_INFO_P(main_pressed);
      setMain();
      break;
    case (1 << KEY7):
      DBG_INFO_P(both_pressed);
      setBoth();
      break;
    default:
      if (keyStatus) {
        sendUartString_n((uint8_t*) "\nc_w_13\t", 8);
        sendUartHex(keyStatus);
      }
      break;
    }
    break;
    /***** next common wire *****/
  case COMMON_WIRE_14:
    switch (keyStatus) {
    /* not used, redundant with commonwire5
     case (1 << KEY0):
     DBG_INFO_P((uint8_t*) guard_pressed);
     break;
     */
    default:
      break;
    }
    break;
    /***** next common wire *****/
  case COMMON_WIRE_15:
    switch (keyStatus & ~KEY_ADD_MASK) {
    // connected with battery and backup
    case (1 << KEY3):
      DBG_INFO_P(backup_pressed);
      break;
    case (1 << KEY4):
      DBG_INFO_P(battery_pressed);
      break;
    default:
      if (keyStatus) {
        sendUartString_n((uint8_t*) "\tc_w_15\t", 8);
        sendUartHex(keyStatus);
      }
      break;
    }
    break;
  default:
    break;
  }
  /* additional keys status, zeroize, as well as backup and battery */
  addKeyStatus = get_add_key_press(~KEY_ADD_MASK);
  switch (addKeyStatus) {
  case (1 << KEY3):
    DBG_INFO_P(zeroize_pressed);
    if (uhfKeyState.OFF != MS_OFF) {
      display(ERASE);
      eraseMWOD();
    }
    break;
  case (1 << KEY4):
    DBG_INFO_P(status_pressed);
    if (uhfKeyState.OFF != MS_OFF) {
      setStatus();
    }
    break;
  default:
    if (addKeyStatus) {
      sendUartString_n((uint8_t*) "\nc_w\t", 5);
      sendUartHex(addKeyStatus);
    }
    break;
  }

#ifdef deactivated
  if (uhfOpMode == FMTCHG) {
    if (keyStatus == 0 && addKeyStatus == 0) {
      if (fmtTimerStarted == FALSE) {
        //start timer at fmt load button
        fmtTimerStarted = TRUE;
        uhfControl.fmtTimeout = FALSE;
        DBG_WARN("\033[20;40f-fmt timer-  ",21);
        setT1Delay(MAX_TIME);
        dbgOnce = TRUE;
      } else {
        // timeout running, check for key activity
        if (uhfControl.fmtTimeout) {
          if(dbgOnce) {
            dbgOnce = FALSE;    // just to avoid repeating dbg display
            DBG_WARN("\033[20;40f>fmt timeOUT<",21);
          }
        }
      }
    } else {
      if(fmtTimerStarted && currentKeyRow < 14)
      dbgOnce = FALSE;
      fmtTimerStarted = FALSE;
      uhfControl.fmtTimeout = FALSE;
      uhfControl.resend = TRUE;
      DBG_WARN("\033[22;40f-fmt reset- ",21);
      if(keyStatus != 0x80)
      sendUartBitPattern(keyStatus);
      else {
        sendUartNewline();
        sendUartHex(currentKeyRow);
      }

    }
  }
#endif
}

/**
 * processes readChannel value
 */
void processChannel() {
  uint8_t keyStatus = 0;

  keyStatus = get_channel();

  if (keyStatus == MASK_CH_01) {
    DBG_INFO((uint8_t*) "CH01\n", 5);
    sendKeyStroke(1);
    radio.channel = 1;
    uhfControl.resend = TRUE;
  }
  if (keyStatus == MASK_CH_02) {
    DBG_INFO((uint8_t*) "CH02\n", 5);
    sendKeyStroke(1);
    radio.channel = 2;
    uhfControl.resend = TRUE;
  }
  if (keyStatus == MASK_CH_03) {
    DBG_INFO((uint8_t*) "CH03\n", 5);
    sendKeyStroke(2);
    radio.channel = 3;
    uhfControl.resend = TRUE;
  }
  if (keyStatus == MASK_CH_04) {
    DBG_INFO((uint8_t*) "CH04\n", 5);
    sendKeyStroke(3);
    radio.channel = 4;
    uhfControl.resend = TRUE;
  }
  if (keyStatus == MASK_CH_05) {
    DBG_INFO((uint8_t*) "CH05\n", 5);
    sendKeyStroke(4);
    radio.channel = 5;
    uhfControl.resend = TRUE;
  }
  if (keyStatus == MASK_CH_06) {
    DBG_INFO((uint8_t*) "CH06\n", 5);
    sendKeyStroke(5);
    radio.channel = 6;
    uhfControl.resend = TRUE;
  }
  if (keyStatus == MASK_CH_07) {
    DBG_INFO((uint8_t*) "CH07\n", 5);
    sendKeyStroke(6);
    radio.channel = 7;
    uhfControl.resend = TRUE;
  }
  if (keyStatus == MASK_CH_08) {
    DBG_INFO((uint8_t*) "CH08\n", 5);
    sendKeyStroke(7);
    radio.channel = 8;
    uhfControl.resend = TRUE;
  }
  if (keyStatus == MASK_CH_09) {
    DBG_INFO((uint8_t*) "CH09\n", 5);
    sendKeyStroke(8);
    radio.channel = 9;
    uhfControl.resend = TRUE;
  }
  if (keyStatus == MASK_CH_10) {
    DBG_INFO((uint8_t*) "CH10\n", 5);
    sendKeyStroke(9);
    radio.channel = 10;
    uhfControl.resend = TRUE;
  }
  if (keyStatus == MASK_CH_11) {
    DBG_INFO((uint8_t*) "CH11\n", 5);
    sendKeyStroke(10);
    radio.channel = 11;
    uhfControl.resend = TRUE;
  }
  if (keyStatus == MASK_CH_12) {
    DBG_INFO((uint8_t*) "CH12\n", 5);
    sendKeyStroke(11);
    radio.channel = 12;
    uhfControl.resend = TRUE;
  }
  if (keyStatus == MASK_CH_13) {
    DBG_INFO((uint8_t*) "CH13\n", 5);
    sendKeyStroke(12);
    radio.channel = 13;
    uhfControl.resend = TRUE;
  }
  if (keyStatus == MASK_CH_14) {
    DBG_INFO((uint8_t*) "CH14\n", 5);
    sendKeyStroke(13);
    radio.channel = 14;
    uhfControl.resend = TRUE;
  }
  if (keyStatus == MASK_CH_15) {
    DBG_INFO((uint8_t*) "CH15\n", 5);
    sendKeyStroke(14);
    radio.channel = 15;

    if (uhfOpMode == VEROP && uhfKeyState.FUSEL == FS_PRESET
        && uhfControl.invalidSWOD == 1) {
      uhfControl.invalidSWOD = FALSE;
      playTone(DOUBLE);
    }

    uhfControl.resend = TRUE;
  }
  if (keyStatus == MASK_CH_16) {
    DBG_INFO((uint8_t*) "CH16\n", 5);
    sendKeyStroke(15);
    radio.channel = 16;
    if (uhfOpMode == VEROP && uhfKeyState.FUSEL == FS_PRESET
        && uhfControl.invalidSWOD == 2) {
      playTone(BEEP);
      uhfControl.invalidSWOD = 1;
    }
    uhfControl.resend = TRUE;
  }
  if (keyStatus == MASK_CH_17) {
    DBG_INFO((uint8_t*) "CH17\n", 5);
    sendKeyStroke(16);
    radio.channel = 17;
    if (uhfOpMode == VEROP && uhfKeyState.FUSEL == FS_PRESET
        && uhfControl.invalidSWOD == 3) {
      playTone(BEEP);
      uhfControl.invalidSWOD = 2;
    }
    uhfControl.resend = TRUE;
  }
  if (keyStatus == MASK_CH_18) {
    DBG_INFO((uint8_t*) "CH18\n", 5);
    sendKeyStroke(17);
    radio.channel = 18;
    if (uhfOpMode == VEROP && uhfKeyState.FUSEL == FS_PRESET
        && uhfControl.invalidSWOD == 4) {
      playTone(BEEP);
      uhfControl.invalidSWOD = 3;
    }
    uhfControl.resend = TRUE;
  }
  if (keyStatus == MASK_CH_19) {
    DBG_INFO((uint8_t*) "CH19\n", 5);
    sendKeyStroke(18);
    radio.channel = 19;

    checkDom();
    if (uhfOpMode == VEROP && uhfKeyState.FUSEL == FS_PRESET
        && uhfControl.invalidSWOD == 5) {
      playTone(BEEP);
      uhfControl.invalidSWOD = 4;
    }

    uhfControl.ch20 = FALSE;
    uhfControl.resend = TRUE;
  }
  if (keyStatus == MASK_CH_20) {
    DBG_INFO((uint8_t*) "CH20\n", 5);
    sendKeyStroke(19);
    radio.channel = 20;
    uhfControl.ch20 = TRUE;

    if (uhfOpMode == VEROP && uhfKeyState.FUSEL == FS_PRESET) {
      uhfControl.invalidSWOD = 5;
      playTone(BEEP);
    }

    uhfControl.resend = TRUE;
  }
  if (radio.channel != 0) { /* backup radio channel */
    channelMemory = radio.channel;
  }
  if ((uhfKeyState.FUSEL == FS_MNL) || (uhfKeyState.FUSEL == FS_GUARD)) {
    radio.channel = 0; /* blank out channel for non-preset modes */
  }
}

void forcedReadFreq() {
  uint8_t temp = 0;
  uint8_t freqDigit = '-';

  for (currentKeyRow = COMMON_WIRE_00; currentKeyRow < COMMON_WIRE_10;
      currentKeyRow++) {
    MULTIPLEXPORT &= 0xF0;          // erase lower nibble (key rows)
    MULTIPLEXPORT |= currentKeyRow; // only works because pins 0 to 3 are used
    temp = 64;
    DBG_DELAY_8BIT(temp);

    temp = ~KEY_PIN;

    switch (currentKeyRow) {
    case COMMON_WIRE_00:
      freqDigit = '5';
      break;
    case COMMON_WIRE_01:
      freqDigit = '4';
      if (temp & (1 << KEY1)) {
        radio.frequency[0] = 'A';
      }
      break;
    case COMMON_WIRE_02:
      freqDigit = '3';
      if (temp & (1 << KEY1)) {
        radio.frequency[0] = freqDigit;
      }
      if (temp & (1 << KEY5)) {
        radio.frequency[4] = '7';
        radio.frequency[5] = '5';
      }
      break;
    case COMMON_WIRE_03:
      freqDigit = '2';
      if (temp & (1 << KEY1)) {
        radio.frequency[0] = freqDigit;
      }
      if (temp & (1 << KEY5)) {
        radio.frequency[4] = '5';
        radio.frequency[5] = '0';
      }
      break;
    case COMMON_WIRE_04:
      freqDigit = '1';
      if (temp & (1 << KEY5)) {
        radio.frequency[4] = '2';
        radio.frequency[5] = '5';
      }
      break;
    case COMMON_WIRE_05:
      freqDigit = '0';
      if (temp & (1 << KEY5)) {
        radio.frequency[4] = '0';
        radio.frequency[5] = '0';
      }
      break;
    case COMMON_WIRE_06:
      freqDigit = '9';
      break;
    case COMMON_WIRE_07:
      freqDigit = '8';
      break;
    case COMMON_WIRE_08:
      freqDigit = '7';
      break;
    case COMMON_WIRE_09:
      freqDigit = '6';
      break;
    default:
      freqDigit = '-';
      break;
    }

    if (temp & (1 << KEY2)) {
      radio.frequency[1] = freqDigit;
    }
    if (temp & (1 << KEY3)) {
      radio.frequency[2] = freqDigit;
    }
    if (temp & (1 << KEY4)) {
      radio.frequency[3] = freqDigit;
    }
    DBG_WARN(".\r", 2);
    uhfControl.resend = TRUE;
  }
}

/**
 * reads manually input states of input devices
 */
void forcedReadUhfState() {
  uint8_t temp = 0;

  for (currentKeyRow = COMMON_WIRE_13; currentKeyRow > COMMON_WIRE_02;
      currentKeyRow--) {
    MULTIPLEXPORT &= 0xF0;          // erase lower nibble (key rows)
    MULTIPLEXPORT |= currentKeyRow; // only works because pins 0 to 3 are used
    temp = 128;
    DBG_DELAY_8BIT(temp);

    temp = ~KEY_PIN;
    switch (currentKeyRow) {
    case (COMMON_WIRE_03):
      if (temp & (1 << KEY6)) {
        DBG_INFO("MNL\t",4);
        uhfKeyState.FUSEL = FS_MNL;
      }
      break;
    case (COMMON_WIRE_04):
      if (temp & (1 << KEY6)) {
        DBG_INFO("PRESET\t",7);
        uhfKeyState.FUSEL = FS_PRESET;
      }
      break;
    case (COMMON_WIRE_05):
      if (temp & (1 << KEY6)) {
        DBG_INFO("GRD\t",4);
        uhfKeyState.FUSEL = FS_GUARD;
      }
      break;
    case (COMMON_WIRE_10):
      readChannel();
      break;
    case (COMMON_WIRE_11):
      readKeys();
      currentKeyRow--;
      if (temp & (1 << KEY7)) {
        DBG_INFO("ON\t",3);
        key_press[currentKeyRow] |= (1 << KEY7);
        key_state[currentKeyRow] = ~temp;
        uhfKeyState.OFF = MS_ON;
      } else {
        DBG_INFO("OFF\t",4);
        uhfKeyState.OFF = MS_OFF;
        uhfControl.firstRun = FALSE;    // don't want to show test twice
      }
      break;
    case (COMMON_WIRE_12):
      readKeys();
      currentKeyRow--;
      if (temp & (1 << KEY7)) {
        uhfControl.squelch = FALSE;
        key_press[currentKeyRow] |= (1 << KEY7);
        key_state[currentKeyRow] = ~temp;
      } else {
        uhfControl.squelch = TRUE;
      }
      break;
    case (COMMON_WIRE_13):
      if (temp & (1 << KEY0)) {
        uhfKeyState.MODE = MS_ADF;
      }
      if (temp & (1 << KEY6)) {
        DBG_INFO("MAIN\t",5);
        uhfKeyState.MODE = MS_MAIN;
      }
      if (temp & (1 << KEY6)) {
        DBG_INFO("BOTH\t",5);
        uhfKeyState.MODE = MS_BOTH;
      }
      break;
    default:
      break;
    }
  }

  MULTIPLEXPORT &= 0xF0;

  if (uhfKeyState.FUSEL == FS_GUARD) {
    setGuard();
  }

}
