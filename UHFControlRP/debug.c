/*
 * debug.c
 *
 *  Created on: Apr 17, 2011
 *      Author: Wolfgang Engelhard
 *
 * Copyright: (c) 2011 by Wolfgang Engelhard
 * License: GNU GPL v2 (see License.txt)
 */

#include <inttypes.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "debug.h"
#include "serial.h"
#include "timer.h"
#include "terminal.h"
#include "uhf.h"
#include "key.h"

/******************    FLASH CONSTANTS                ************************/
#if DEBUG_LEVEL > 0

const uint8_t s_debug_en[] PROGMEM = "\tDebug enabled: \0";

const uint8_t help[] PROGMEM = "HELP for terminal\r\n\n\0";
const uint8_t help_a[] PROGMEM = "a\tshow readout for additional keys\n\0";
const uint8_t
		help_0_9[] PROGMEM
				=
				"0\tPortC readout for all Multiplex channels\r\n1..9\tPortC readout for selected Multiplex channel\r\n\0";
const uint8_t help_timer[] PROGMEM = "T\tTimer Registers\n\0";
const uint8_t help_audio[] PROGMEM = "p\tplay beep\r\nP\tplay pulse\r\nW\tplay wave\r\n\0";
const uint8_t help_spi[] PROGMEM = "S\tSPI register\n\0";
const uint8_t help_dom[] PROGMEM = "d\tDay of Month\n\0";
const uint8_t help_cursor[] PROGMEM = "c\tHide cursor\r\nC\tShow cursor\r\n\0";
const uint8_t help_ocr[] PROGMEM = ">\tincrease audio freq.\r\n<\tdecrease audio freq.\r\n\0";
const uint8_t help_TAB[] PROGMEM = "TAB\tToggle PortC readout\n\0";
const uint8_t help_BS[] PROGMEM = "BS\tReset terminal\r\n\0";
const uint8_t help_baud[] PROGMEM = "b\tcheck baud error\r\n\0";
const uint8_t help_monitor[] PROGMEM = "m\tread channel\r\nM\tmonitor channel\r\n\0";
const uint8_t help_mwod[] PROGMEM = "f\tshow loaded MWOD\r\n\0";

#if DEBUG_LEVEL > 1
// info messages
#endif
#endif

/****************************   GLOBAL VARIABLES      ************************/
debugFlag_t debugFlag;

/****************************   FUNCTION DEFINITIONS  ************************/
void debug(uint8_t data) {
	uint8_t i = 0;
	uint8_t k = 255;
	uint16_t temp = 0;

	switch (data) {
	case '0':
		restoreCursor();
		sendUartString_n((uint8_t*) "\033[J\033[33m", 8);
		sendUartString_n((uint8_t*) "PORTC\r\n", 7);

		for (i = 0; i < 16; i++) {
			PORTB &= 0xF0;
			PORTB |= i;
			DBG_DELAY_8BIT(k);
			sendUartHex(i);
			sendUartByte('>');
			sendUartBitPattern(PINC);
		}
		TERM_NORMAL;
		restoreCursor();
		break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		PORTB &= 0xF0;
		PORTB |= (data - 0x30);
		sendUartString_n((uint8_t*) ": ", 2);
		sendUartBitPattern(PINC);
		restoreCursor();
		break;
	case 'd':
		DBG_WARN("\rDOM ",4);
		sendUartByte(currDayOfMonth[0]);
		sendUartByte(currDayOfMonth[1]);
		break;
	case 'f':
		// TODO display all 6 dimensions of array
		sendUartString_n((uint8_t*)"\r\nMWOD\r\n",8);
		sendUartString_n(uhfMWOD[0].ch_20,FREQ_ARRAY_SIZE);
		sendUartNewline();
		sendUartString_n(uhfMWOD[0].ch_19,FREQ_ARRAY_SIZE);
		sendUartNewline();
		sendUartString_n(uhfMWOD[0].ch_18,FREQ_ARRAY_SIZE);
		sendUartNewline();
		sendUartString_n(uhfMWOD[0].ch_17,FREQ_ARRAY_SIZE);
		sendUartNewline();
		sendUartString_n(uhfMWOD[0].ch_16,FREQ_ARRAY_SIZE);
		sendUartNewline();
		sendUartString_n(uhfMWOD[0].ch_15,FREQ_ARRAY_SIZE);
		sendUartNewline();
		sendUartString_n(uhfMWOD[0].ch_14,2);
		sendUartString_n((uint8_t*)"\t\t@",3);
		sendUartByte(currMWodIndex+0x30);
		break;
	case 'n':
		sendUartString_n((uint8_t*)"\r\nFMT-NET\r\n",11);
		for(i=0; i < MAX_FMT; i++) {
			sendUartString_n(uhfFMT.ch[i], FREQ_ARRAY_SIZE);
			sendUartNewline();
		}
		break;
	case 'u':
		sendUartString_n((uint8_t*)"\r\nUHF Control\n",14);
		TERM_CLEAR_TILL_END;
		if(uhfControl.firstRun) {
			sendUartString_n((uint8_t*)"first run\r\n",11);
		}
		if(uhfControl.gotTone) {
			sendUartString_n((uint8_t*)"Tone recvd\r\n",12);
		}
		if(uhfControl.invalidSWOD == FALSE) {
			sendUartString_n((uint8_t*)"SWOD valid\r\n",12);
		}
		if(uhfKeyState.OFF == MS_OFF) {
			sendUartString_n((uint8_t*)"UHF off\r\n",10);
		}
		sendUartNewline();
		break;
	case 'o':
		uhfKeyState.MODE = MS_ADF;
		setUhfOn();
		uhfKeyState.OFF = MS_ON;
		forcedReadFreq();
		break;
	case 'p':
		playTone(BEEP);
		break;
	case 'P':
		playTone(PULSE);
		break;
	case 'w':
		playTone(WAVE);
		break;
	case 't':
		playTone(TOD);
		break;
	case '>':
		i = SREG;
		cli();
		OCR1A = OCR1A-0x10;
		SREG = i;
		break;
	case '<':
		i = SREG;
		cli();
		OCR1A = OCR1A+0x10;
		SREG = i;
		break;
	case 'T':
		TERM_RESET;
		TERM_BOLD;
		sendUartString_n((uint8_t*) "TCNT0\n", 6);
		TERM_NORMAL;
		sendUartBitPattern(TCNT0);
		TERM_BOLD;
		sendUartString_n((uint8_t*) "TCCR0\n", 6);
		TERM_NORMAL;
		sendUartBitPattern(TCCR0);
		TERM_BOLD;
		sendUartString_n((uint8_t*) "\nOCR1A\n", 7);
		TERM_NORMAL;
		temp = OCR1A;
		sendUartBitPattern(temp >> 8);
		sendUartBitPattern(temp & 0x00ff);
		TERM_BOLD;
		sendUartString_n((uint8_t*) "TCNT1\n", 6);
		TERM_NORMAL;
		temp = TCNT1;
		sendUartBitPattern(temp >> 8);
		sendUartBitPattern(temp & 0x00ff);
		TERM_BOLD;
		sendUartString_n((uint8_t*) "TCCR1A\n", 7);
		TERM_NORMAL;
		temp = TCCR1A;
		sendUartBitPattern(temp);
		TERM_BOLD;
		sendUartString_n((uint8_t*) "TCCR1B\n", 7);
		TERM_NORMAL;
		temp = TCCR1B;
		sendUartBitPattern(temp);
		TERM_BOLD;
		sendUartString_n((uint8_t*) "\nTCNT2\n", 7);
		TERM_NORMAL;
		sendUartBitPattern(TCNT2);
		TERM_BOLD;
		sendUartString_n((uint8_t*) "TCCR2\n", 6);
		TERM_NORMAL;
		sendUartBitPattern(TCCR2);

		restoreCursor();
		sendUartByte(CURSOR_LEFT);
		break;
	case 'S':
		restoreCursor();
		TERM_CLEAR_TILL_END;
		sendUartString_n((uint8_t*)"\nSPCR\n",5);
		sendUartBitPattern(SPCR);
		restoreCursor();
		sendUartByte(CURSOR_LEFT);
		break;
	case '+':
		SPDR = SPI_SOF;
		break;
	case 'a':
		restoreCursor();
		TERM_CLEAR_TILL_END;
		sendUartBitPattern(DDRA);
		sendUartBitPattern(PORTA);
		sendUartHex(get_add_key_press(ALL_KEYS)); // all input pins
		sendUartString_n((uint8_t*) "I ", 3);
		sendUartBitPattern(PINA);
		break;
	case 'c':
		TERM_HIDE_CURSOR;
		break;
	case 'C':
		TERM_SHOW_CURSOR; //show cursor
		break;
	case 0x09:
		debugFlag.keyPoll ^= 1;
		debugFlag.pollChannel = 0;
		debugFlag.readChannel = 0;
		TERM_HIDE_CURSOR;
		break;
	case 127:
		TERM_RESET;
		break;
	case 'm':		// only works in debug level 2
		debugFlag.readChannel ^= 1;
		break;
	case 'M': 	// only works in debug level 2
		debugFlag.pollChannel ^= 1;
		break;
	case 'b':
		checkBaudError();
		break;
	case 'h':
	default:
		debugFlag.keyPoll = 0;
		debugFlag.pollChannel = 0;
		debugFlag.readChannel = 0;
		restoreCursor();
		TERM_CLEAR_TILL_END;
		sendUartString_P((uint8_t*) help);
		sendUartString_P((uint8_t*) help_0_9);
		sendUartString_P((uint8_t*) help_a);
		sendUartString_P((uint8_t*) help_ocr);
		sendUartString_P((uint8_t*) help_audio);
		sendUartString_P((uint8_t*) help_timer);
		sendUartString_P((uint8_t*) help_spi);
		sendUartString_P((uint8_t*) help_cursor);
		sendUartString_P((uint8_t*) help_TAB);
		sendUartString_P((uint8_t*) help_BS);
		sendUartString_P((uint8_t*) help_monitor);
		sendUartString_P((uint8_t*) help_baud);
		sendUartString_P((uint8_t*) help_dom);
		sendUartString_P((uint8_t*) help_mwod);

		break;
	}
}
