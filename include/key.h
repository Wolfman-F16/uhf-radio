/*
 * key.h
 *
 *	Based on work of Peter Dannegger
 *
 *  Created on: 09.10.2009
 *      Author: Wolfgang Engelhard
 *
 * Copyright: (c) 2011 by Wolfgang Engelhard
 * License: GNU GPL v2 (see License.txt)
 *
 */

#ifndef KEY_H_
#define KEY_H_

#include <avr/io.h>
#include <stdint.h>

#include "uhf.h"
#include "main.h"

/****************************   STRUCTURES            ************************/
typedef struct {
  unsigned dirtyFlag :1;
  unsigned readKeys :1;
} keyState_t;

/****************************   GLOBAL VARIABLES      ************************/
extern uint8_t key_state[];     // debounced and inverted key state:
                                // bit = 1: key pressed
extern uint8_t key_press[];     // key press detect
extern uint8_t key_rpt[];       // key long press and repeat
extern uint8_t currentKeyRow;

extern uint8_t channel;

extern keyState_t keyState;

/****************************   FUNCTION DECLARATIONS ************************/
uint8_t get_key_long(uint8_t key_mask);
uint8_t get_key_short(uint8_t key_mask);
uint8_t get_key_rpt(uint8_t key_mask);
uint8_t get_key_press(uint8_t key_mask);
uint8_t get_add_key_press(uint8_t key_mask);
uint8_t get_channel();

void readKeys(void);
void readAdditionalKeys(void);
void readChannel(void);

/****************************   INLINE FUNCTIONS      ************************/
/**
 * switches to next key row
 */
static inline void switchToNextKeyRow() {
  currentKeyRow++;
  if (currentKeyRow > COMMON_WIRE_15) {
                                    //reset to zero
    currentKeyRow = COMMON_WIRE_00;
  }
                                    // pull current row low
  MULTIPLEXPORT &= 0xF0;            // erase lower nibble (key rows)
  MULTIPLEXPORT |= currentKeyRow;   // only works because pins 0 to 3 are used
                                    // and upper nibble is not changed by operation
}

/****************************     ISR FUNCTIONS       ************************/
/**
 * interrupt service routine
 */
static inline void isrKey() {
  keyState.readKeys = TRUE;
}
#endif /* KEY_H_ */
