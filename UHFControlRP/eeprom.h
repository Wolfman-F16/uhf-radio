/*
 * eeprom.h
 *
 *  Created on: 26.04.2011
 *      Author: Wolfgang Engelhard
 *
 * Copyright: (c) 2011 by Wolfgang Engelhard
 * License: GNU GPL v2 (see License.txt)
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#include <inttypes.h>
#include <avr/eeprom.h>

#ifndef EEMEM
#define EEMEM  __attribute__ ((section (".eeprom")))
#endif
/****************************   FUNCTION DECLARATIONS ************************/
#define   EEPReadByte(addr)         eeprom_read_byte((uint8_t *)addr)
#define   EEPWriteByte(addr, val)   eeprom_write_byte((uint8_t *)addr, val)

#endif /* EEPROM_H_ */
