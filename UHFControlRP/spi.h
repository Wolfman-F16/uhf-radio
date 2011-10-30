/*
 * spi.h
 *
 *  Created on: 07.10.2009
 *      Author: Wolfgang Engelhard
 *
 * Copyright: (c) 2011 by Wolfgang Engelhard
 * License: GNU GPL v2 (see License.txt)
 */

#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>

/****************************   DEFINITIONS         **************************/
#define _SPI_EN_				1
#define _MASTER_SPI_    1
#define _FAST_SPI_      0

/****************************   GLOBAL VARIABLES    **************************/
extern volatile uint8_t spiRxValue;

/****************************   FUNCTION DECLARATIONS ************************/
void initSpi(void);
void sendSpiByte(uint8_t );
#if _MASTER_SPI_ > 0
void sendSpiString(uint8_t *, uint8_t );
void startSpiTransmission(void);
void endSpiTransmission(void);
#endif

#endif /* SPI_H_ */
