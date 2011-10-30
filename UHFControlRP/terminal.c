/*
 * terminal.c
 *
 *  Created on: 17.04.2011
 *      Author: Wolfgang Engelhard
 *
 * Copyright: (c) 2011 by Wolfgang Engelhard
 * License: GNU GPL v2 (see License.txt)
 */

#include "terminal.h"
#include "serial.h"

#if EXT_TERMINAL_SUPPORT > 0

/****************************   FLASH CONSTANTS       ************************/
const uint8_t sHide_cursor[] PROGMEM = "\033[?25l\0";
const uint8_t sShow_cursor[] PROGMEM = "\033[?25h\0";
const uint8_t sReset[] PROGMEM = "\033c\033[s";//"\033[2J\033[H\033[?25h\0";
const uint8_t sHome[] PROGMEM = "\033[H\0";
const uint8_t sGreen[] PROGMEM = "\033[32m\0";
const uint8_t sBold[] PROGMEM = "\033[1m\0";
const uint8_t sNormal[] PROGMEM = "\033[0m\0";
const uint8_t sClearTillEnd[] PROGMEM = "\033[J";

/****************************   FUNCTION DECLARATIONS ************************/
void setTermGreen()
{
  sendUartString_P((uint8_t*) sGreen);
}

void storeCursor()
{
  sendUartByte(ANSI_ESC);
  sendUartByte('[');
  sendUartByte('s');
}

void restoreCursor()
{
  sendUartByte(ANSI_ESC);
  sendUartByte('[');
  sendUartByte('u');
}

#endif
