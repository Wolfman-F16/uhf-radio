/*
 * usb.c
 *
 *  Created on: 16.04.2011
 *      Author: Wolfgang Engelhard
 *
 * Copyright: (c) 2011 by Wolfgang Engelhard
 * License: GNU GPL v2 (see License.txt)
 */
#include <string.h>

#include "usb.h"
#include "usbconfig.h"
#include "oddebug.h"
#include "uhf.h"

/****************************   GLOBAL VARIABLES    **************************/
uint8_t isUsbPresent;

/****************************   LOCAL FUNCTIONS     **************************/
uint8_t transmitToDevice(usbWord_t, usbWord_t);

/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */

/**
 HID Report Descriptor as in original USB specification (HID Descriptor Tool)
 */
PROGMEM const char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] =
    { 0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
      0x09, 0x06,                    // USAGE (Keyboard)
      0xa1, 0x01,                    // COLLECTION (Application)
      0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
      0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
      0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
      0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
      0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
      0x75, 0x01,                    //   REPORT_SIZE (1)
      0x95, 0x08,                    //   REPORT_COUNT (8)
      0x81, 0x02,                    //   INPUT (Data,Var,Abs)
      0x95, 0x01,                    //   REPORT_COUNT (1)
      0x75, 0x08,                    //   REPORT_SIZE (8)
      0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
      0x95, 0x05,                    //   REPORT_COUNT (5)
      0x75, 0x01,                    //   REPORT_SIZE (1)
      0x05, 0x08,                    //   USAGE_PAGE (LEDs)
      0x19, 0x01,                    //   USAGE_MINIMUM (Num Lock)
      0x29, 0x05,                    //   USAGE_MAXIMUM (Kana)
      0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
      0x95, 0x01,                    //   REPORT_COUNT (1)
      0x75, 0x03,                    //   REPORT_SIZE (3)
      0x91, 0x03,                    //   OUTPUT (Cnst,Var,Abs)
      0x95, 0x06,                    //   REPORT_COUNT (6)
      0x75, 0x08,                    //   REPORT_SIZE (8)
      0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
      0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
      0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
      0x19, 0x00,           //   USAGE_MINIMUM (Reserved (no event indicated))
      0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
      0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
      0xc0                           // END_COLLECTION
    };

/****************************   FUNCTION DEFINITIONS  ************************/
void initUsb() {
  usbInit();
  isUsbPresent = 0;
}

/**
 * USB function setup
 */
uchar usbFunctionSetup(uchar data[8]) {
  usbRequest_t *rq = (void *) data;

  usbMsgPtr = reportBuffer;
  /* check for class request type */
  if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {
    DBG1(0x50, &rq->bRequest, 1); /* debug output: print our request */
    switch (rq->bRequest) {
    case USBRQ_HID_GET_REPORT:
      /* wValue: ReportType (highbyte), ReportID (lowbyte) */
      /* we only have one report type, so don't look at wValue */
      return sizeof(reportBuffer);

    case USBRQ_HID_SET_REPORT:
      if (rq->wLength.word == 1) { /* We expect one byte reports */
        return 0xFF; /* Call usbFunctionWrite with data */
      }
      break;
    case USBRQ_HID_GET_IDLE:
#if KEYREPEAT > 0
      usbMsgPtr = &idleRate;
      return 1;
#endif
      break;
    case USBRQ_HID_SET_IDLE:
#if KEYREPEAT > 0
      idleRate = rq->wValue.bytes[1];
#endif
      break;
    case USBRQ_HID_GET_PROTOCOL:
      if (rq->wValue.bytes[1] < 1) {
        protocolVer = rq->wValue.bytes[1];
      }
      break;
    case USBRQ_HID_SET_PROTOCOL:
      usbMsgPtr = &protocolVer;
      return 1;
    case USB_DATA_TX:
      return transmitToDevice(rq->wValue, rq->wIndex);

    default:
      break;
    }
  }
  return 0;
}

/**
 * writes data received by USB to UHF-Radio. Data definition is as follows:
 *  1.Byte 0 | 4L | 1M | 1L | 2M |    |    | 2L
 *  2.Byte 0 | 3M |    |    | 3L | 4M |    |  4
 *  3.Byte 0 | 5M | 5L | CM |    |    |    | CL
 *  with L = LSB, M = MSB, 1..5 = Frequency digit (5 contains 5th and 6th digit)
 *  C = Channel
 */
uint8_t transmitToDevice(usbWord_t value, usbWord_t index) {
  radio_t radioData;
  uint8_t arrData[3];

  arrData[0] = value.bytes[0];
  arrData[1] = value.bytes[1];
  arrData[2] = index.bytes[0];
  radioData = decompressRadioData(arrData);

  if(index.bytes[1] == 0xEE) {
    /* write to eeprom */
    writePresetFreq2eeprom(&radioData);
  } else {
    /* write to display */
    memcpy(&radio, &radioData, sizeof(radio_t));
    uhfControl.resend = 1;
  }
  return 4;
}
