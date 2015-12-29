/*
 * timer.c
 *
 *  Created on: 03.10.2009
 *      Author: Wolfgang Engelhard
 *
 * Copyright: (c) 2011 by Wolfgang Engelhard
 * License: GNU GPL v2 (see License.txt)
 */
#include <avr/interrupt.h>
#include <avr/io.h>

#include "timer.h"
#include "uhf.h"
#include "key.h"

/****************************   FUNCTION DECLARATIONS ************************/
/*
 * Functions
 */
void initTimer() {
  initTimer0(); // key
  initTimer1(); // tone
  initTimer2(); // led
}

/**
 * configures timer 1 to overflow in specified time
 * 16 MHz / 1024 / TCNT1 = overflows per second
 * -> max time: 4.19 sec (0x0000)
 * -> 1 sec (0x3D09)
 */
void setT1Delay(time_e tcntValue) {
  uint8_t sreg;

  TIMSK &= ~(1 << TOIE1 | 1 << OCIE1A);
  TCCR1A = 0x00;                    // normal timer operation
  TCCR1B = 0x00;                    // stop timer
  /* atomic write */
  sreg = SREG;
  cli();
  TCNT1 = 0x0000;
  OCR1A = tcntValue;                //  default to 4.19 seconds max time
  SREG = sreg;
  TCCR1B = (1 << WGM12 | 1 << CS10 | 1 << CS12); // prescaler 1024 and activate timer
                                    // clear overflow flag, just in case
  TIFR |= (1 << TOV1 | 1 << OCF1A);
                                    // activate timer 1 overflow interrupt
  TIMSK |= (1 << OCIE1A);
}

void setT1Pwm(freq_e freqValue) {
  uint8_t sreg;

  TCCR1B = 0x00;                    // disable timer 1
  TCNT1 = 0x00;                     //  default to 4.19 seconds max time
                                    // clear overflow flag, just in case
  TIFR |= (1 << TOV1);
  /* PWM mode 9: Phase & freq correct, OCR1 is TOP reference , required prescaler: 8*/
  TCCR1A = (1 << WGM10);
  TCCR1B = (1 << WGM13 | 1 << CS11);
  TIMSK &= ~(1 << TOIE1 | 1 << OCIE1A); // no overflow interrupt needed when PWM
  /* atomic write */
  sreg = SREG;
  cli();
  OCR1A = freqValue;
  SREG = sreg;
}

void setT2Delay(time_e tcntValue) {

  TCNT2 = tcntValue & 0x00FF;
  upperByte = tcntValue >> 8;

  TIFR |= (1 << TOV2);
  TCCR2 = (1 << CS22 | 1 << CS21 | 1 << CS20);  // 1024
  TIMSK |= (1 << TOIE2);
}

/**************************** INTERRUPT SERVICE ROUTINES   *******************/
/*
 * Timer 0.
 * calls isrADC function. at 2nd execution it
 * calls isrKey function.
 */
ISR (TIMER0_OVF_vect) {
  static uint8_t extTimerCounter = 0;

  if (--extTimerCounter == 0) {
    extTimerCounter = 2;
    isrKey();
  } else {
    isrAdc();
  }
}

/*
 * Timer 1. ISR not active during tone play.
 */
ISR (TIMER1_COMPA_vect) {
  isrTimer1();
}

/*
 * Timer 2.
 */
ISR (TIMER2_OVF_vect) {
  isrTimer2();
}
