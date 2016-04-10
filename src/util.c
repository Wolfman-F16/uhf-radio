/*
 * util.c
 *
 *  Created on: Apr 10, 2016
 *      Author: Wolfgang Engelhard
 *
 * Copyright: (c) 2016 by Wolfgang Engelhard
 * License: GNU GPL v2 (see License.txt)
 */
#include "eeprom.h"
#include "uhf.h"

/*
 * Decompresses radio data. Converts frequency data to string array.
 * Populates radio_t structure.
 */
radio_t decompressRadioData(uint8_t bytes[3]) {
  radio_t radioData;
  uint8_t temp = 0;

  temp = (bytes[0] & 0x30) >> 4;    // 1. digit masked
  if (temp == 1) {
    temp |= 0x40;
  } else {
    temp |= ASCII_OFFSET;
  }
  radioData.frequency[0] = temp;              // special handling 'A'

  temp = bytes[0] & 0x0F;           // 2.digit
  radioData.frequency[1] = temp | ASCII_OFFSET;

  temp = bytes[1] & 0x78;           // 3. digit
  radioData.frequency[2] = (temp >> 3) | ASCII_OFFSET;

  temp = (bytes[1] << 1) & 0x0E;
  temp |= (bytes[0] & 0x40) >> 6;   // 4. digit
  radioData.frequency[3] = temp | ASCII_OFFSET;

  temp = (bytes[2] & 0x60);
  switch (temp) {
  case 0x00:
    radioData.frequency[4] = '0';
    radioData.frequency[5] = '0';
    break;
  case 0x20:
    radioData.frequency[4] = '2';
    radioData.frequency[5] = '5';
    break;
  case 0x40:
    radioData.frequency[4] = '5';
    radioData.frequency[5] = '0';
    break;
  case 0x60:
    radioData.frequency[4] = '7';
    radioData.frequency[5] = '5';
    break;
  default:
    break;
  }

  radioData.channel = bytes[2] & 0x1F;

  return radioData;
}

/*
 * writes preset frequency to EEPROM.
 * channel provides array index
 */
uint8_t writePresetFreq2eeprom(radio_t *pRadioData) {

  if(pRadioData == 0) {
    return 1;
  }

  eeprom_write_block(
      &(pRadioData->frequency[0]),
      (uint8_t*) &(eePresetFrequencies[pRadioData->channel -1][0]),
      FREQ_ARRAY_SIZE);

  return 0;
}
