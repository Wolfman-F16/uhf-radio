/*
 * main.h
 *
 *  Created on: 16.04.2011
 *      Author: Wolfgang Engelhard
 *
 * Copyright: (c) 2011 by Wolfgang Engelhard
 * License: GNU GPL v2 (see License.txt)
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <avr/io.h>

/****************************   DEFINITIONS           ************************/
#define TRUE    1
#define FALSE   0

#define STATUS_LED_PORT PORTD
#define MULTIPLEXPORT   PORTB

#define PIN_INHIBIT     PORTA7
#define PIN_OUTPUT_0    PORTA6
#define PIN_WAVEOUT     PORTD5
#define PIN_LED         PORTD7
#define PIN_UART_TX     PORTD1
#define PIN_TRIGGER     PORTD2
#define SPI_MOSI        PORTB5
#define SPI_SCK         PORTB7
#define SPI_SS          PORTB4

/****************************   INLINE FUNCTIONS      ************************/
static inline void setStatusLedOff(void) {
  STATUS_LED_PORT |= (1 << PIN_LED);
}

static inline void setStatusLedOn(void) {
  STATUS_LED_PORT &= ~(1 << PIN_LED);
}

#endif /* MAIN_H_ */
