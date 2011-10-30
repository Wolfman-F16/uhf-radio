/*
 * keys.h
 *
 *  Created on: 07.09.2011
 *      Author: Wolfgang Engelhard
 *
 * Copyright: (c) 2011 by Wolfgang Engelhard
 * License: GNU GPL v2 (see License.txt)
 */

#ifndef KEYS_H_
#define KEYS_H_

#include <avr/pgmspace.h>
#include <inttypes.h>

#define NUM_KEYS    10

static const uint8_t    keyReport[NUM_KEYS +1][2] PROGMEM = {
          {0 , 0},
/*  01 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT | MOD_CTRL_LEFT, KEY_F4}, // mode off
/*  02 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT | MOD_CTRL_LEFT, KEY_F5}, // mode main
/*  03 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT | MOD_CTRL_LEFT, KEY_F6}, // mode both
/*  04 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT | MOD_CTRL_LEFT, KEY_F7}, // mode adf
/*  05 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT | MOD_CTRL_LEFT, KEY_F8}, // fu mnl
/*  06 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT | MOD_CTRL_LEFT, KEY_F9}, // fu preset
/*  07 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT | MOD_CTRL_LEFT, KEY_F10},// fu guard
/*  08 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT | MOD_CTRL_LEFT, KEY_F11},// ch inc
/*  09 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT | MOD_CTRL_LEFT, KEY_F12} // ch dec
};

#endif /* KEYS_H_ */
