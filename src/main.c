/*
 * main.c
 *
 *  Created on: 16.04.2011
 *      Author: Wolfgang Engelhard
 *
 * Copyright: (c) 2011 by Wolfgang Engelhard
 * License: GNU GPL v2 (see License.txt)
 */

#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "main.h"
#include "uhf.h"
#include "key.h"
#include "spi.h"
#include "serial.h"
#include "timer.h"
#include "debug.h"
#include "terminal.h"
#include "usb.h"
#include "usbKeyDefs.h"

/****************************   FLASH CONSTANTS       ************************/
const uint8_t sReady[] PROGMEM =
"\033c\nAN/ARC-164 UHF IIa Airborne Radio ready...\r\n\033[s\0";

/****************************   FUNCTION DECLARATIONS ************************/
/**
 * Port pin assignment for UHF Control Board RP v1.0
 * PORTA
 * 0: Volume ADC (ADC0)
 * 1..2 N/C
 * 3: Input 9
 * 4: Input 8
 * 5: N/C
 * 6: Output 0
 * 7: Inhibit IC 154 (inhibit on H)
 * PORTB
 * 0: A
 * 1: B
 * 2: C
 * 3: D
 * 4: SPI SS
 * 5: SPI MOSI
 * 6: SPI MISO
 * 7: SPI CLK
 * PORTC
 * 0..7: Input 0..7
 * PORTD
 * 0: RX
 * 1: TX
 * 2: Trigger (INT0)
 * 3: USB_IN
 * 4: USB_OUT
 * 5: Tone_waveout (OC1A)
 * 6: N/C
 * 7: Status LED (OC2)
 */
void initIO() {

  DDRA = (1 << PIN_INHIBIT | 1 << PIN_OUTPUT_0);
  PORTA = (1 << PIN_INHIBIT | 1 << INPUT8 | 1 << INPUT9);

  DDRB = (1 << SPI_MOSI | 1 << SPI_SCK | 1 << SPI_SS);
  DDRB |= 0x0F;             // IC 154 A..D
  PORTB &= ~(1 << SPI_SS); 	// pull low to indicate presence of control board

  // enable internal pull up
  PORTC = 0xFF;

  setStatusLedOn();
  DDRD = (1 << PIN_UART_TX | 1 << PIN_WAVEOUT | 1 << PIN_LED);
  PORTD = (1 << USB_CFG_DMINUS_BIT | 1 << USB_CFG_DPLUS_BIT);
  DDRD |= (1 << USB_CFG_DMINUS_BIT | 1 << USB_CFG_DPLUS_BIT);
  /* USB reset of a single ended device needs both lines pulled high for at least 10 ms */
  _delay_ms(15);
  /* remove USB reset condition */
  DDRD = (1 << PIN_UART_TX | 1 << PIN_WAVEOUT | 1 << PIN_LED);
  PORTD = (1 << USB_CFG_DMINUS_BIT); /* USB low speed device must hold D- (only) high (shouldn't be necessary since hw does pull up)*/
  // status led is on
  setStatusLedOff();
}

/**
 * enable multiplexer IC 74 HC 154
 */
void initMultiplexer() {
  // active when low
  PORTA &= ~(1 << PIN_INHIBIT); 	// set multiplexer device active
  PORTA &= ~(1 << PIN_OUTPUT_0); 	// output for additional keys set low
}

/**
 * main function
 */
int main(void) {
  uint8_t row = 0;
  uint8_t row2 = 0;

  wdt_enable(WDTO_2S);

  initIO();

  if (initSerial()) {
    TCCR2 |= (1 << WGM20 | 1 << COM21 | 1 << COM20);             /* error */
    OCR2 = 16;                                    /* show by LED blinking */
    for (;;) {
      ;
    }
  }

  initSpi();

  initTimer();

  initMultiplexer();

  initUsb();

  set_sleep_mode(SLEEP_MODE_IDLE);

#if DEBUG_LEVEL > 0
  debugFlag.keyPoll = FALSE;
  debugFlag.pwm = FALSE;
  debugFlag.pollChannel = FALSE;
  debugFlag.readChannel = FALSE;
#endif

  sendUartString_P((uint8_t*) sReady);
  storeCursor();

  PORTD &= ~(1 << USB_CFG_DMINUS_BIT); /* reset D- to default state */
  setStatusLedOn();
  sei();

  // set default values
  initVariables();

  for (;;) {

    wdt_reset();
    usbPoll();

    /* USB SECTION -------------------------------------------------*/
    /* releases key press */
    sendUsbMessage(NOT_USED);
    /* USB SECTION -------------------------------------------------*/

    if (serialStatus.rx) {
      serialStatus.rx = 0;
      DBG(rxValue);
    }

#if DEBUG_LEVEL > 0
    if (debugFlag.keyPoll) {

      if (row++ > 15) {
        row = 0;
        restoreCursor();
      } else {
        sendUartBitPattern(PINC);
        if (row == KEY_ADD_ROW + 1) {
          sendUartByte('\t');
          sendUartBitPattern(PINA);
        }
      }
      sendUartHex(row);
      sendUartByte(' ');
      PORTB &= 0xF0;
      PORTB |= row;
      PORTA &= 0x18;
      PORTA |= row2;
      setStatusLedOn();
    } else
#endif

    // query Keys
    if (keyState.readKeys) {
      readChannel();
      readKeys();
      readAdditionalKeys();
    }

    //setState();
    processChannel();
    processInputData(get_key_short(ALL_KEYS));

    if (uhfControl.resend && (uhfControl.test != TRUE)) {
      if (uhfKeyState.FUSEL == FS_GUARD) {
        setGuard();
      }

      uhfControl.resend = FALSE;
      if (uhfKeyState.OFF != MS_OFF) {
        if (uhfControl.firstRun) {
          uhfControl.firstRun = FALSE;
          setUhfOn();
        }
#ifdef deactivated
        if(uhfOpMode == FMTCHG && uhfControl.fmtTimeout) {
          sendData(getChannelString(), (uint8_t*)"FMTCHG");
        } else
#endif
        sendData(getChannelString(), getFrequencyString());
      }
    }

    if (usbInterruptIsReady()) {
      isUsbPresent = TRUE;
    }

  } /* endless loop */

  return 0;
}

