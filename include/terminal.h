/*
 * terminal.h
 *
 *  Created on: 17.04.2011
 *      Author: Wolfgang Engelhard
 *
 * Copyright: (c) 2011 by Wolfgang Engelhard
 * License: GNU GPL v2 (see License.txt)
 */

#ifndef TERMINAL_H_
#define TERMINAL_H_

#include <avr/pgmspace.h>

/****************************   DEFINITIONS           ************************/
#define EXT_TERMINAL_SUPPORT  1

// special ASCII codes
#define CLEAR_PAGE    12
#define DEL_RIGHT     32
#define BACKSPACE     127 //backspace#define CURSOR_LEFT   8
#define CURSOR_UP     141
#define CURSOR RIGHT  28
#define CURSOR_DOWN   132
#define CR_LF         133
#define COPYRIGHT     169
#define CORNER_TOP_RIGHT 172
#define LINE_TOP 173
// next equals cursor right, so check what's right
#define ANSI_ESC       0x1B

#if EXT_TERMINAL_SUPPORT > 0

#define TERM_RESET          sendUartString_P((uint8_t*)sReset)
#define TERM_HIDE_CURSOR    sendUartString_P((uint8_t*) sHide_cursor)
#define TERM_SHOW_CURSOR    sendUartString_P((uint8_t*) sShow_cursor)
#define TERM_NORMAL         sendUartString_P((uint8_t*) sNormal)
#define TERM_BOLD           sendUartString_P((uint8_t*) sBold)
#define TERM_CLEAR_TILL_END sendUartString_P((uint8_t*) sClearTillEnd)

/****************************   GLOBAL VARIABLES      ************************/
extern const uint8_t sHide_cursor[];
extern const uint8_t sShow_cursor[];
extern const uint8_t sReset[];
extern const uint8_t sHome[];
extern const uint8_t sBold[];
extern const uint8_t sNormal[];
extern const uint8_t sClearTillEnd[];

/****************************   FUNCTION DEFINITIONS  ************************/
void setTermGreen(void);
void resetTerm(void);
void storeCursor(void);
void restoreCursor(void);
#endif

#endif /* TERMINAL_H_ */
