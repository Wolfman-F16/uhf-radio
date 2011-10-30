/*
 * uhfInit.c
 *
 *  Created on: 26.04.2011
 *      Author: Wolfgang Engelhard
 *
 * Copyright: (c) 2011 by Wolfgang Engelhard
 * License: GNU GPL v2 (see License.txt)
 */
#include <avr/pgmspace.h>
#include <string.h>

#include "uhf.h"
#include "eeprom.h"
#include "key.h"
#include "debug.h"
#include "serial.h"

/****************************   FLASH CONSTANTS       ************************/
const uint8_t verop[] PROGMEM = "VER/OP";
const uint8_t mload[] PROGMEM = "M-LOAD";
const uint8_t erase[] PROGMEM = "ERASE ";
const uint8_t fmtchg[]PROGMEM = "FMTCHG";
const uint8_t off[] PROGMEM = "      ";

const uint8_t *uhfStatusStrings[] PROGMEM
= { verop, mload, erase, fmtchg, off };

/****************************   EEPROM CONSTANTS      ************************/


const uint8_t eePresetFrequencies[MAX_CHANNELS][FREQ_ARRAY_SIZE] EEMEM = {
		{ '2', '8', '4', '1', '0', '0' },
		{ '2', '9', '2', '5', '0', '0' },
		{ '2', '6', '4', '5', '0', '0' },
		{ '2', '3', '7', '9', '0', '0' },
		{ '3', '7', '9', '9', '0', '0' },
		{ '3', '6', '3', '1', '5', '0' },
		{ '3', '7', '7', '1', '0', '0' },
		{ '3', '6', '0', '6', '0', '0' },
		{ '2', '7', '3', '5', '7', '5' },
		{ '3', '4', '9', '7', '2', '5' },
		{ '2', '8', '8', '1', '7', '5' },
		{ '2', '5', '7', '8', '0', '0' },
		{ '2', '7', '2', '6', '0', '0' },
		{ '2', '9', '5', '7', '0', '0' },
		{ '2', '8', '7', '4', '0', '0' },
		{ '2', '9', '7', '6', '0', '0' },
		{ '3', '1', '4', '3', '0', '0' },
		{ '3', '5', '9', '1', '0', '0' },
		{ '3', '7', '6', '0', '0', '0' },
		{ '3', '0', '0', '0', '7', '5' } };

const uint8_t eeFmtNets[MAX_FMT][FREQ_ARRAY_SIZE] EEMEM = {
		{ '2', '7', '9', '7', '5', '0' },
		{ '2', '8', '4', '1', '5', '0' },
		{ '2', '8', '9', '0', '5', '0' },
		{ '2', '9', '3', '5', '5', '0' },
		{ '2', '9', '8', '6', '5', '0' },
		{ '3', '0', '3', '2', '7', '5' },
		{ '3', '0', '8', '7', '5', '0' },
		{ '3', '1', '4', '4', '5', '0' },
		{ '2', '5', '7', '2', '5', '0' },
		{ '2', '6', '2', '4', '5', '0' },
		{ '2', '6', '7', '8', '5', '0' },
		{ '2', '7', '1', '9', '5', '0' },
		{ '2', '3', '9', '9', '5', '0' },
		{ '2', '5', '2', '9', '2', '5' },
		{ '2', '2', '5', '1', '5', '0' },
		{ '2', '3', '5', '0', '5', '0' } };


/****************************   GLOBAL VARIABLES      ************************/
uint8_t	currDayOfMonth[] = {0x55,0x55};	// make sure it's outside valid range of 01..31
extern const uint8_t eepromMsg[] PROGMEM;

/****************************   VOLATILE VARIABLES    ************************/
volatile uhfControl_t uhfControl;

/****************************   FUNCTION DEFINITIONS  ************************/
/**
 * initialize variables to startup values.
 */
void initVariables() {
	uint8_t i = 0;
	// spiRxValue
	//	spiRxValue = 0;

	// keyState
	keyState.dirtyFlag = FALSE;
	keyState.readKeys = TRUE;
	// uhfControl
	uhfControl.resend = TRUE;	// TODO check...
	uhfControl.test = FALSE;
	uhfControl.squelch = TRUE;
	uhfControl.firstRun = TRUE;
	uhfControl.invalidSWOD = 5;

	// copy fmt nets from eeprom to sram
	for(i = 0; i < MAX_FMT; i++) {
		eeprom_read_block(uhfFMT.ch[i], eeFmtNets[i], FREQ_ARRAY_SIZE);
	}

	// radio
	forcedReadUhfState();

	uhfOpMode = EEPReadByte(&eeUhfState);
	if (uhfOpMode > 4) {	// magic number 4: max number of uhfOpMode enums
		EEPWriteByte(&(eeUhfState),VEROP);
		DBG_WARN_P(eepromMsg);
	}
}
