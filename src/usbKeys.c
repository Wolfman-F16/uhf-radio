/*
 * usbKeys.c
 *
 *  Created on: 14.10.2011
 *      Author: Wolfgang Engelhard
 *
 * Copyright: (c) 2011 by Wolfgang Engelhard
 * License: GNU GPL v2 (see License.txt)
 */

#include "usb.h"
#include "main.h"
#include "uhf.h"
#include "usbKeyDefs.h"

/*
 * send keystroke via USB
 */
void sendUsbMessage(uint8_t data) {
  static uint8_t lastData = 0;

  if (uhfControl.squelch == TRUE || isUsbPresent == FALSE) {
    // mute key strokes
    return;
  }

  if (data != lastData) {
    setStatusLedOn();
    lastData = data;
    /* detection if USB connected done in main loop */
    while (!usbInterruptIsReady()) {
      ;
    }
    /* use last key and not current key status in order to avoid lost
     changes in key status. */
    buildReport(lastData);
    usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
    setStatusLedOff();
  }
}

/**
 * \param previous channel number -1 relative to current
 */
void sendKeyStroke(uint8_t previous) {
  if (radio.channel == previous) {
    sendUsbMessage(KEY_CH_INC);
  } else {
    sendUsbMessage(KEY_CH_DEC);
  }
}
