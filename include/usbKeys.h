/*
 * keys.h
 *
 *  Created on: 07.09.2011
 *      Author: Wolfgang Engelhard
 *
 * Update 29.Apr.2019:
 *  fixed redundant key definitions
 *  matched key definitions with BMS 4.34 Pitbuilder QWERTY layout (BMS Full Layout w Placeholders (US Int))
 *
 * Copyright: (c) 2011 by Wolfgang Engelhard
 * License: GNU GPL v2 (see License.txt)
 */

#ifndef KEYS_H_
#define KEYS_H_

#include <avr/pgmspace.h>
#include <inttypes.h>

#define NUM_KEYS    58

static const uint8_t    keyReport[NUM_KEYS +1][2] PROGMEM = {
          {0 , 0},
/*  01 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT                , KEY_ENTER},   //+ mode off
/*  02 */ {MOD_ALT_LEFT | MOD_CTRL_LEFT                 , KEY_ENTER},   //+ mode main
/*  03 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT | MOD_CTRL_LEFT, KEY_ENTER},   //+ mode both
/*  04 */ {MOD_SHIFT_LEFT | MOD_CTRL_LEFT               , KEY_Z},       //- mode adf
/*  05 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT | MOD_CTRL_LEFT, KEY_X},       //+ fu mnl
/*  06 */ {MOD_SHIFT_LEFT | MOD_CTRL_LEFT               , KEY_C},       //+ fu preset
/*  07 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT                , KEY_C},       //+ fu guard
/*  08 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT                , KEY_S},       //+ ch inc
/*  09 */ {MOD_ALT_LEFT | MOD_CTRL_LEFT                 , KEY_S},       //+ ch dec
/*  10 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT                , KEY_D},       //+ Freq 1 2
/*  11 */ {MOD_ALT_LEFT | MOD_CTRL_LEFT                 , KEY_D},       //+ Freq 1 3
/*  12 */ {MOD_SHIFT_LEFT | MOD_CTRL_LEFT               , KEY_D},       //+ Freq 1 4
/*  13 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT | MOD_CTRL_LEFT, KEY_D},       //+ Freq 2 0
/*  14 */ {MOD_SHIFT_LEFT | MOD_CTRL_LEFT               , KEY_F},       //+ Freq 2 1
/*  15 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT                , KEY_F},       //+ Freq 2 2
/*  16 */ {MOD_ALT_LEFT | MOD_CTRL_LEFT                 , KEY_F},       //+ Freq 2 3
/*  17 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT | MOD_CTRL_LEFT, KEY_F},       //+ Freq 2 4
/*  18 */ {MOD_SHIFT_LEFT | MOD_CTRL_LEFT               , KEY_G},       //+ Freq 2 5
/*  19 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT                , KEY_G},       //+ Freq 2 6
/*  20 */ {MOD_ALT_LEFT | MOD_CTRL_LEFT                 , KEY_G},       //+ Freq 2 7
/*  21 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT | MOD_CTRL_LEFT, KEY_G},       //+ Freq 2 8
/*  22 */ {MOD_SHIFT_LEFT | MOD_CTRL_LEFT               , KEY_H},       //+ Freq 2 9
/*  23 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT                , KEY_H},       //+ Freq 3 0
/*  24 */ {MOD_ALT_LEFT | MOD_CTRL_LEFT                 , KEY_H},       //+ Freq 3 1
/*  25 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT | MOD_CTRL_LEFT, KEY_H},       //+ Freq 3 2
/*  26 */ {MOD_SHIFT_LEFT | MOD_CTRL_LEFT               , KEY_J},       //+ Freq 3 3
/*  27 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT                , KEY_J},       //+ Freq 3 4
/*  28 */ {MOD_ALT_LEFT | MOD_CTRL_LEFT                 , KEY_J},       //+ Freq 3 5
/*  29 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT | MOD_CTRL_LEFT, KEY_J},       //+ Freq 3 6
/*  30 */ {MOD_SHIFT_LEFT | MOD_CTRL_LEFT               , KEY_K},       //+ Freq 3 7
/*  31 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT                , KEY_K},       //+ Freq 3 8
/*  32 */ {MOD_ALT_LEFT | MOD_CTRL_LEFT                 , KEY_K},       //+ Freq 3 9
/*  33 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT | MOD_CTRL_LEFT, KEY_K},       //+ Freq 4 0
/*  34 */ {MOD_SHIFT_LEFT | MOD_CTRL_LEFT               , KEY_L},       //+ Freq 4 1
/*  35 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT                , KEY_L},       //+ Freq 4 2
/*  36 */ {MOD_ALT_LEFT | MOD_CTRL_LEFT                 , KEY_L},       //+ Freq 4 3
/*  37 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT | MOD_CTRL_LEFT, KEY_L},       //+ Freq 4 4
/*  38 */ {MOD_SHIFT_LEFT | MOD_CTRL_LEFT               , KEY_SEMICOL}, //+ Freq 4 5
/*  39 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT                , KEY_SEMICOL}, //+ Freq 4 6
/*  40 */ {MOD_ALT_LEFT | MOD_CTRL_LEFT                 , KEY_SEMICOL}, //+ Freq 4 7
/*  41 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT | MOD_CTRL_LEFT, KEY_SEMICOL}, //+ Freq 4 8
/*  42 */ {MOD_SHIFT_LEFT | MOD_CTRL_LEFT               , KEY_APOSTR},  //+ Freq 4 9
/*  43 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT                , KEY_APOSTR},  //+ Freq 5 00
/*  44 */ {MOD_ALT_LEFT | MOD_CTRL_LEFT                 , KEY_APOSTR},  //+ Freq 5 25
/*  45 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT | MOD_CTRL_LEFT, KEY_APOSTR},  //+ Freq 5 50
/*  46 */ {MOD_SHIFT_LEFT | MOD_CTRL_LEFT               , KEY_ENTER},   //+ Freq 5 75
/*  47 */ {MOD_SHIFT_LEFT | MOD_CTRL_LEFT               , KEY_S},       //- Load
/*  48 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT | MOD_CTRL_LEFT, KEY_S},       //- Status
/*  49 */ {MOD_ALT_LEFT | MOD_CTRL_LEFT                 , KEY_A},       //- Zero On
/*  50 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT | MOD_CTRL_LEFT, KEY_A},       //- Zero Off
/*  51 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT                , KEY_X},       //- Squelch On
/*  52 */ {MOD_ALT_LEFT | MOD_CTRL_LEFT                 , KEY_X},       //- Squelch Off
/*  53 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT                , KEY_Z},       //- Tone T
/*  54 */ {MOD_ALT_LEFT | MOD_CTRL_LEFT                 , KEY_Z},       //- Tone Tone
/*  55 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT | MOD_CTRL_LEFT, KEY_Z},       //+ Chan Vol Inc
/*  56 */ {MOD_SHIFT_LEFT | MOD_CTRL_LEFT               , KEY_X},       //+ Chan Vol Dec
/*  57 */ {MOD_SHIFT_LEFT | MOD_ALT_LEFT                , KEY_DOT}      // Test Display
          /* +: supported by BMS                                   */
          /* -: placeholder by BMS                                 */
          /* TODO BMS: channel does not support discrete signaling */
};

#endif /* KEYS_H_ */
