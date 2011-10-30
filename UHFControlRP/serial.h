/*
 * serial.h
 *
 *  This file is for the ATMega88 and newer devices.
 *
 *  Created on: 04.10.2009
 *      Author: Wolfgang Engelhard
 *
 * Copyright: (c) 2011 by Wolfgang Engelhard
 * License: GNU GPL v2 (see License.txt)
 */

#ifndef SERIAL_H_
#define SERIAL_H_

#include <inttypes.h>

/****************************   DEFINITIONS           ************************/
#define UART_BAUD_RATE  (uint16_t)38400
#define UART_POWER_SAVE         0
#define UART_NUMBER_EN          0

// port to ATMega16
#define USART_RX_vect USART_RXC_vect
#define UBRR0H        UBRRH
#define UBRR0L        UBRRL
#define UDRE0         UDRE
#define UCSR0A        UCSRA
#define UCSR0C        UCSRC
#define UCSR0B        UCSRB
#define UCSZ00        UCSZ0
#define UCSZ01        UCSZ1
#define TXEN0         TXEN
#define RXEN0         RXEN
#define RXCIE0        RXCIE
#define TXC0          TXC
#define UDR0          UDR

#define UBRR_RESULT		(F_CPU / (UART_BAUD_RATE * 16L) -1)
#define REAL_BAUD_RES (F_CPU / (16 * (UBRR_RESULT + 1)))

/****************************   STRUCTURES      *******************************/
typedef struct {
  unsigned rx:1;

} serialStatus_t;

typedef union {
  uint16_t i16;
  struct {
    uint8_t i8l;
    uint8_t i8h;
  };
} UINT16_T;

/****************************   GLOBAL VARIABLES    **************************/
extern volatile uint8_t         rxValue;
extern volatile serialStatus_t  serialStatus;

/****************************   FUNCTION DEFINITIONS  ************************/
int8_t initSerial(void);
void sendUartByte(uint8_t );
void sendUartBitPattern(uint8_t );
void sendUartString_P(uint8_t*);
void sendUartString_n(uint8_t *, uint8_t );
void sendUartHex(uint8_t);
void sendUartNewline(void);
#if UART_NUMBER_EN > 0
void sendUartDez(uint8_t);
#endif
int checkBaudError(void);

#endif /* SERIAL_H_ */
