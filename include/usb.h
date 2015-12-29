/*
 * usb.h
 *
 *  Created on: 16.04.2011
 *      Author: Wolfgang Engelhard
 *
 * Copyright: (c) 2011 by Wolfgang Engelhard
 * License: GNU GPL v2 (see License.txt)
 *
 * Features intended for USB:
 * - Interface with Teamspeak. Frequency and Common Net as ways to separate communication.
 *   Eventually Teamspeak channel (frequency) hopping.
 * - Interface with Falcon 4.2 (Falcon 4.0, Falcon 4.1 Allied Force, Falcon 4.2 OpenFalcon)
 *   Submit channel, frequency, volume and button/switch states.
 */

#ifndef USB_H_
#define USB_H_

#include "usbconfig.h"
#include "usbdrv.h"             // uchar
/****************************   DEFINITIONS         **************************/
/* Keyboard usage values, see usb.org's HID-usage-tables document, chapter
 * 10 Keyboard/Keypad Page for more codes.
 */
#define MOD_CTRL_LEFT       (1<<0)
#define MOD_SHIFT_LEFT      (1<<1)
#define MOD_ALT_LEFT        (1<<2)
#define MOD_GUI_LEFT        (1<<3)
#define MOD_CONTROL_RIGHT   (1<<4)
#define MOD_SHIFT_RIGHT     (1<<5)
#define MOD_ALT_RIGHT       (1<<6)
#define MOD_GUI_RIGHT       (1<<7)

#define KEY_A                    4
#define KEY_B                    5
#define KEY_C                    6
#define KEY_D                    7
#define KEY_E                    8
#define KEY_F                    9
#define KEY_G                   10
#define KEY_H                   11
#define KEY_I                   12
#define KEY_J                   13
#define KEY_K                   14
#define KEY_L                   15
#define KEY_M                   16
#define KEY_N                   17
#define KEY_O                   18
#define KEY_P                   19
#define KEY_Q                   20
#define KEY_R                   21
#define KEY_S                   22
#define KEY_T                   23
#define KEY_U                   24
#define KEY_V                   25
#define KEY_W                   26
#define KEY_X                   27
#define KEY_Y                   28
#define KEY_Z                   29
#define KEY_1                   30
#define KEY_2                   31
#define KEY_3                   32
#define KEY_4                   33
#define KEY_5                   34
#define KEY_6                   35
#define KEY_7                   36
#define KEY_8                   37
#define KEY_9                   38
#define KEY_0                   39

#define KEY_ENTER               40
#define KEY_ESC                 41
#define KEY_BACKSPACE           42
#define KEY_TAB                 43
#define KEY_SPACE               44
#define KEY_MINUS               45
#define KEY_EQUAL               46
#define KEY_OBRACKET            47
#define KEY_CBRACKET            48
#define KEY_B_SLASH             49
#define KEY_DDDD                50
#define KEY_SEMICOL             51
#define KEY_APOSTR              52
#define KEY_TILDE               53
#define KEY_COMMA               54
#define KEY_DOT                 55
#define KEY_SLASH               56
#define KEY_CAPS                57

#define KEY_F1                  58
#define KEY_F2                  59
#define KEY_F3                  60
#define KEY_F4                  61
#define KEY_F5                  62
#define KEY_F6                  63
#define KEY_F7                  64
#define KEY_F8                  65
#define KEY_F9                  66
#define KEY_F10                 67
#define KEY_F11                 68
#define KEY_F12                 69

#define KEY_PRTSCR              70
#define KEY_SCROLL              71
#define KEY_PAUSE               72
#define KEY_INSERT              73
#define KEY_HOME                74
#define KEY_PGUP                75
#define KEY_DEL                 76
#define KEY_END                 77
#define KEY_PGDN                78
#define KEY_RARROW              79
#define KEY_LARROW              80
#define KEY_DARROW              81
#define KEY_UARROW              82
#define PAD_NUM                 83
#define PAD_DIV                 84
#define PAD_MULT                85
#define PAD_MINUS               86
#define PAD_PLUS                87
#define PAD_ENTER               88
#define PAD_1                   89
#define PAD_2                   90
#define PAD_3                   91
#define PAD_4                   92
#define PAD_5                   93
#define PAD_6                   94
#define PAD_7                   95
#define PAD_8                   96
#define PAD_9                   97
#define PAD_0                   98
#define PAD_DOT                 99
#define KEY_CCCC               100
#define PAD_COMMA              133

#define REPORT_BUFFER_SIZE       8

/****************************   GLOBAL VARIABLES    **************************/
static uchar reportBuffer[REPORT_BUFFER_SIZE]; /* buffer for HID reports */
static uchar protocolVer = 1;                  /* 0 is the boot protocol,*/
                                               /* 1 is report protocol   */
extern const char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH];
extern uint8_t isUsbPresent;

/****************************   FUNCTION DECLARATIONS ************************/
uchar usbFunctionSetup(uchar[]);
uchar usbFunctionWrite(uchar *, uchar);
void  initUsb(void);
void  sendUsbMessage(uint8_t);
void  sendKeyStroke(uint8_t);

/****************************   STATIC FUNCTIONS      ************************/
#include "usbKeys.h"
static void buildReport(uint8_t key) {
  reportBuffer[0] = pgm_read_byte(&(keyReport[key][0]));
  reportBuffer[2] = pgm_read_byte(&(keyReport[key][1]));
}
#endif /* USB_H_ */
