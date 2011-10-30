/*
 * serial.c
 *
 *  Created on: 04.10.2009
 *      Author: Wolfgang Engelhard
 *
 * Copyright: (c) 2011 by Wolfgang Engelhard
 * License: GNU GPL v2 (see License.txt)
 */
#ifndef F_CPU
#error F_CPU not defined
#endif

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdint.h>

#include "main.h"
#include "serial.h"

/****************************   GLOBAL VARIABLES      ************************/
volatile uint8_t rxValue = 0;
volatile serialStatus_t serialStatus;

/****************************   LOCAL VARIABLES       ************************/
const uint8_t s_baud_error[] PROGMEM =
"\033[JWarning: Baud rate outside reliable range!\n\0";

/****************************   FUNCTION DECLARATIONS ************************/
/*
 * Functions
 */
int8_t initSerial() {
	int8_t errCode = checkBaudError();

	if (errCode != 0) {
		if (errCode > 1) {
			sendUartString_P((uint8_t*) s_baud_error);
		} else {
			return errCode;
		}
	}

	UINT16_T baud;
	baud.i16 = (F_CPU / (UART_BAUD_RATE * 16L) - 1);
	UBRR0H = baud.i8h;
	UBRR0L = baud.i8l;

	UCSR0C = (1 << URSEL | 1 << UCSZ01 | 1 << UCSZ00);
	UCSR0B = (1 << TXEN0 | 1 << RXEN0 | 1 << RXCIE0);

	return errCode;
}

/**
 * sends byte
 * \param[in] byte byte to send
 */
void sendUartByte(uint8_t byte) {
	while (!(UCSR0A & (1 << UDRE0)))
		; // wait till byte is out of buffer
	UDR0 = byte;
#if UART_POWER_SAVE > 0
	// next lines necessary when using power save sleep modes
	while( !(UCSR0A & (1 << TXC0)));
	UCSR0A |= (1 << TXC0);
	while( !(UCSR0A & (1 << UDRE0)) ); // wait till byte is out of buffer
#endif
}

/*
 * sends string
 * \param[in] string string to send
 */
void sendUartString_P(uint8_t *string) {
	uint8_t character;

	while ((character = pgm_read_byte(string))) {
		sendUartByte(character);
		string++;
	}
}

/*
 * sends string with specific length
 * \param[in] string string to send
 * \param[in] length string length
 */
void sendUartString_n(uint8_t *string, uint8_t length) {
	int i = 0;

	for (i = 0; i < length; i++) {
		sendUartByte(*(string + i));
	}
}

/**
 * sends bit pattern of byte
 * \param[in] byte byte to send as bit pattern
 */
void sendUartBitPattern(uint8_t byte) {
	if (byte & 0x80)
		sendUartByte('1');
	else
		sendUartByte('0');
	if (byte & 0x40)
		sendUartByte('1');
	else
		sendUartByte('0');
	if (byte & 0x20)
		sendUartByte('1');
	else
		sendUartByte('0');
	if (byte & 0x10)
		sendUartByte('1');
	else
		sendUartByte('0');
	if (byte & 0x08)
		sendUartByte('1');
	else
		sendUartByte('0');
	if (byte & 0x04)
		sendUartByte('1');
	else
		sendUartByte('0');
	if (byte & 0x02)
		sendUartByte('1');
	else
		sendUartByte('0');
	if (byte & 0x01)
		sendUartByte('1');
	else
		sendUartByte('0');

	sendUartNewline();
}

/**
 * helper function for sendUartHex
 * \param[in] hex nibble to convert to ASCII
 */
uint8_t hexAscii(uint8_t hex) {
	hex &= 0xf;
	if (hex >= 10)
		hex += 'a' - (uint8_t) 10 - '0';
	hex += '0';
	return hex;
}

/**
 * sends byte as hex number
 * \param[in] byte byte to send as hex
 */
void sendUartHex(uint8_t byte) {
	//sendUartByte('0');
	//sendUartByte('x');
	sendUartByte(hexAscii(byte >> 4));
	sendUartByte(hexAscii(byte));
}

#if UART_NUMBER_EN > 0
/**
 * sends byte as decimal number
 */
void sendUartDez(uint8_t byte)
{
	channelString[1] = '0' + tmpChannel % 10;
	tmpChannel /= 10;
	channelString[0] = '0' + tmpChannel;
}
#endif

/*
 * sends carriage return and line feed
 */
void sendUartNewline() {
	UDR0 = '\r';
	while (!(UCSR0A & (1 << UDRE0)))
		; // wait till byte is out of buffer
	UDR0 = '\n';
	while (!(UCSR0A & (1 << UDRE0)))
		; // wait till byte is out of buffer
}

/*
 * checks baud rate error
 * \retVal 0  baud rate error acceptable
 * \retVal 1  baud rate error above 2 percent
 */
int checkBaudError() {
#if serial_debug
	UINT16_T baud;
	int16_t UBRR_VAL = ((F_CPU + UART_BAUD_RATE * 8) / (UART_BAUD_RATE * 16) - 1);
#endif

	int16_t BAUD_ERROR = ((REAL_BAUD_RES * 1000) / UART_BAUD_RATE - 1000);
	// max. +/-10 per mill error
	if ((BAUD_ERROR > 20) || (BAUD_ERROR < -20)) {
		return -1; // error magic number
	}
	if ((BAUD_ERROR > 10) || (BAUD_ERROR < -10)) {
		return 1; // error magic number
	}
#if serial_debug
	sendUartString_n("\033[2;1f\033[JTEST\r\n", 15);
	sendUartString_n("\t0x", 3);
	sendUartHex(UBRR0H);
	sendUartHex(UBRR0L);
	sendUartString_n("\n\rUBRR\r\n", 8);
	baud.i16 = UBRR_VAL;
	sendUartBitPattern(baud.i8h);
	sendUartBitPattern(baud.i8l);
	sendUartString_n("\t0x", 3);
	sendUartHex(baud.i8h);
	sendUartHex(baud.i8l);
	sendUartString_n("\n\rREAL\r\n", 8);
	baud.i16 = REAL_BAUD_RES;
	sendUartBitPattern(baud.i8h);
	sendUartBitPattern(baud.i8l);
	sendUartString_n("\t0x", 3);
	sendUartHex(baud.i8h);
	sendUartHex(baud.i8l);
	sendUartString_n("\n\rError\r\n", 9);
	baud.i16 = BAUD_ERROR;
	sendUartBitPattern(baud.i8h);
	sendUartBitPattern(baud.i8l);
	sendUartString_n("\t0x", 3);
	sendUartHex(baud.i8h);
	sendUartHex(baud.i8l);
	sendUartString_n("\n\rF_CPU[kHz]\r\n", 14);
	baud.i16 = F_CPU / 1000;
	sendUartBitPattern(baud.i8h);
	sendUartBitPattern(baud.i8l);
	sendUartString_n("\t0x", 3);
	sendUartHex(baud.i8h);
	sendUartHex(baud.i8l);
	sendUartString_n("\n\rBAUD_RATE\r\n", 13);
	baud.i16 = UART_BAUD_RATE;
	sendUartBitPattern(baud.i8h);
	sendUartBitPattern(baud.i8l);
	sendUartString_n("\t0x", 3);
	sendUartHex(baud.i8h);
	sendUartHex(baud.i8l);
#endif

	return 0;
}

/**************************** Interrupt Service Routine     ******************/
ISR (USART_RX_vect)
{
	rxValue = UDR0;
	serialStatus.rx = TRUE;
	// pings back received value
	UDR0 = rxValue;
}
