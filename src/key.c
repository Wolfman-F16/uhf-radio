/*
 * key.c
 *
 *  Based on work of Peter Dannegger
 *
 *  Created on: 09.10.2009
 *      Author: Wolfgang Engelhard
 *
 * Copyright: (c) 2011 by Wolfgang Engelhard
 * License: GNU GPL v2 (see License.txt)
 *
 *  circuit layout and port programming must be as follows
 *  input pull-up                   output
 *  DDRx = 0, PORTx = 1     -|>|-   DDRy = 1, PORTy = 0
 *  if more than 1 output row exists, only one PORT pin may be set low at one time
 *
 * NOTE:
 * to make this useful for momentary push buttons, deletion of button state must be prevented
 * so a pushed button always signals 1, while releasing stops signaling 1.
 * therefore, query of buttons must use a separate state variable
 */
#include <avr/interrupt.h>

#include "key.h"
#include "uhf.h"

/****************************   GLOBAL VARIABLES      ************************/
uint8_t key_state[NR_OF_KEYROWS];   // debounced and inverted key state:
                                    // bit = 1: key pressed
uint8_t key_press[NR_OF_KEYROWS];   // key press detect
static uint8_t key_rpt[NR_OF_KEYROWS];     // key long press and repeat
uint8_t currentKeyRow;              // current key row pulled low

static uint8_t key_add_state;
static uint8_t key_add_press;
static uint8_t key_add_rpt;

static uint8_t channel;
static uint8_t chan_state = 0;

keyState_t keyState;

/****************************   FUNCTION DEFINITIONS  ************************/

/*
 * check if a key has been pressed. Each pressed key is reported  only once
 */
uint8_t get_key_press(uint8_t key_mask) {
  cli();                                    // read and clear atomic !
  key_mask &= key_press[currentKeyRow];     // read key(s)
  key_press[currentKeyRow] ^= key_mask;     // clear key(s) with XOR operation
  // TEST, Battery and Backup are push&hold keys
  if ((key_mask & MASK_TEST) && (currentKeyRow == COMMON_WIRE_05)) {
    key_press[currentKeyRow] |= (key_mask & MASK_TEST);
  }
  if ((key_mask & MASK_SQUELCH) && (currentKeyRow == COMMON_WIRE_12)) {
    key_press[currentKeyRow] |= (key_mask & MASK_SQUELCH);
  }
  if ((key_mask & MASK_UHF_IO) && (currentKeyRow == COMMON_WIRE_11)) {
    key_press[currentKeyRow] |= (key_mask & MASK_UHF_IO);
  }
  sei();
  return key_mask;
}

/*
 * check if a key has been pressed on additional input port.
 */
uint8_t get_add_key_press(uint8_t key_mask) {
  key_mask = ~key_add_state & key_mask;
  cli();
  key_mask &= key_add_press;
  key_add_press ^= key_mask;

  if ((key_mask & MASK_BATTERY)) {
    //key_press[currentKeyRow] |= (key_mask & MASK_BATTERY);
  }
  if ((key_mask & MASK_BACKUP)) {
    //key_press[currentKeyRow] |= (key_mask & MASK_BACKUP);
  }

  sei();
  return key_mask;
}

/**
 * returns readChannel value (debounced)
 */
uint8_t get_channel() {
  static uint8_t prev_val = 0;
  uint8_t ch_val = ~chan_state & MASK_CHANNEL;

  if (ch_val != prev_val) {
    prev_val = ch_val;
  } else {
    ch_val = 0;
  }

  return ch_val;
}

/*
 check if a key has been pressed long enough such that the
 key repeat functionality kicks in. After a small setup delay
 the key is reported being pressed in subsequent calls
 to this function. This simulates the user repeatedly
 pressing and releasing the key.
 */
uint8_t get_key_rpt(uint8_t key_mask) {
  cli();                              // read and clear atomic !
  key_mask &= key_rpt[currentKeyRow]; // read key(s)
  key_rpt[currentKeyRow] ^= key_mask; // clear key(s)
  sei();
  return key_mask;
}

/**
 * returns 1 for pressed buttons or flipped switches. release is not signaled!
 * \return  normal key press data
 */

uint8_t get_key_short(uint8_t key_mask) {
  cli();                                // read key state and key press atomic!

  return get_key_press(~key_state[currentKeyRow] & key_mask);
}

/**
 * \return  repeated key press data
 */
uint8_t get_key_long(uint8_t key_mask) {
  return get_key_press(get_key_rpt(key_mask));
}

/*
 * reads key inputs
 */
void readKeys() {
  static uint8_t ct0[NR_OF_KEYROWS];
  static uint8_t ct1[NR_OF_KEYROWS];
  static uint8_t rpt[NR_OF_KEYROWS];
  uint8_t temp;

                                            // apply key mask to input
  if (currentKeyRow == COMMON_WIRE_15) {
    temp = KEY_MASK | KEY_ADD_PIN;
  } else {
    temp = KEY_MASK | KEY_PIN;              // key mask OR operation because of
  }                                         // inversion in next line

  temp = key_state[currentKeyRow] ^ temp;   // key changed ?
                                            // reset or count ct0
  ct0[currentKeyRow] = ~(ct0[currentKeyRow] & temp);
                                            // reset or count ct1
  ct1[currentKeyRow] = ct0[currentKeyRow] ^ (ct1[currentKeyRow] & temp);
  temp &= ct0[currentKeyRow] & ct1[currentKeyRow]; // count until roll over ?
  key_state[currentKeyRow] ^= temp;         // then toggle debounced state
  key_press[currentKeyRow] |= key_state[currentKeyRow] & temp;
                                            // 0->1: key press detect
  if ((key_state[currentKeyRow] & REPEAT_MASK) == 0) {  // check repeat function
    rpt[currentKeyRow] = REPEAT_START;      // start delay
  }
  if (--rpt[currentKeyRow] == 0) {
    rpt[currentKeyRow] = REPEAT_NEXT;       // repeat delay
    key_rpt[currentKeyRow] |= key_state[currentKeyRow] & REPEAT_MASK;
  }
  switchToNextKeyRow();
}

/*
 * reads key inputs of additional input port
 */
void readAdditionalKeys() {
  static uint8_t ct0;
  static uint8_t ct1;
  static uint8_t rpt;
  uint8_t temp = 0;

  if (currentKeyRow == KEY_ADD_ROW) {
                                        // apply key mask to input
    temp = KEY_ADD_MASK | KEY_ADD_PIN;  // key mask OR operation because of
                                        // inversion in next line
    temp = key_add_state ^ temp;        // key changed ?
                                        // reset or count ct0
    ct0 = ~(ct0 & temp);
                                        // reset or count ct1
    ct1 = ct0 ^ (ct1 & temp);
    temp &= ct0 & ct1;                  // count until roll over ?
    key_add_state ^= temp;              // then toggle debounced state
    key_add_press |= key_add_state & temp; // 0->1: key press detect

    if ((key_add_state & REPEAT_MASK) == 0) { // check repeat function
      rpt = REPEAT_START;               // start delay
    }
    if (--rpt == 0) {
      rpt = REPEAT_NEXT;                // repeat delay
      key_add_rpt |= key_add_state & REPEAT_MASK;
    }
  }
}

/**
 * reads readChannel frequency
 */
void readChannel() {
  uint8_t temp = 0;
  static uint8_t ct0 = 0;
  static uint8_t ct1 = 0;

  if (currentKeyRow == COMMON_WIRE_10) {
                                        // apply key mask to input
    temp = ~MASK_CHANNEL | KEY_PIN;     // key mask OR operation because of
                                        // inversion in next line
    temp = chan_state ^ temp;           // key changed ?
                                        // reset or count ct0
    ct0 = ~(ct0 & temp);
                                        // reset or count ct1
    ct1 = ct0 ^ (ct1 & temp);
    temp &= ct0 & ct1;                  // count until roll over ?
    chan_state ^= temp;                 // then toggle debounced state
    channel |= chan_state & temp;       // 0->1: key press detect
  }
}
