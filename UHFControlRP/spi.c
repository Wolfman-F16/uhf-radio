/*
 * spi.c
 *
 *  Created on: 07.10.2009
 *      Author: Wolfgang Engelhard
 *
 * Copyright: (c) 2011 by Wolfgang Engelhard
 * License: GNU GPL v2 (see License.txt)
 */
#include <avr/interrupt.h>
#include <avr/io.h>

#include "spi.h"
#include "main.h"

/****************************   VOLATILE VARIABLES    ************************/
volatile uint8_t spiRxValue;

/****************************   FUNCTIONS DEFINITIONS ************************/
/*
 * initialises Serial Peripheral Interface
 * Set to master with clock rate / 4
 */
void initSpi()
{
#if _MASTER_SPI_ > 0
#if _FAST_SPI_ > 0
  SPCR = (1 << SPE | 1 << MSTR);                            // clock rate / 4
#else
  SPCR = (1 << SPE | 1 << MSTR | 1 << SPR1 | 1 << SPR0);    // clock rate / 128
#endif
#else
  SPCR = (1 << SPE | 1 << SPIE);
#endif
}

/*
 * sends a byte via SPI
 */
void sendSpiByte(uint8_t cData)
{
  /* Start transmission */
  SPDR = cData;
#if _MASTER_SPI_ > 0
  /* Wait for transmission complete */
  while (!(SPSR & (1 << SPIF)))
    ;
#endif
}

#if _MASTER_SPI_ > 0
void sendSpiString(uint8_t *string, uint8_t length)
{
  for(int i = 0; i < length; i++) {
    sendSpiByte(*string);
    string++;
  }
}

void startSpiTransmission()
{
  PORTB &= ~(1 << SPI_SS);
  asm volatile ("nop");
}

void endSpiTransmission()
{
  asm volatile ("nop");
  PORTB |= (1 << SPI_SS);
}
#endif

/**************************** Interrupt Service Routine     ******************/
ISR (SPI_STC_vect)
{
  spiRxValue = SPDR;
}
