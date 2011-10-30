/*
 * timer.h
 *
 *  Created on: 04.10.2009
 *      Author: Wolfgang Engelhard
 *
 * Copyright: (c) 2011 by Wolfgang Engelhard
 * License: GNU GPL v2 (see License.txt)
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <avr/io.h>

/****************************   GLOBAL VARIABLES    **************************/
typedef enum 
{
  MAX_TIME = 0x0000,    // 4.19 seconds at 16 MHz and prescaler 1024
  FOUR_SECS = 0xF425,
  ONE_SEC = 0x3D09,
  HALF_SEC = 0x1E84
} time_e;

typedef enum 
{
	FREQ_1666Hz = 0x0145, // is a 1667 Hz frequency
	FREQ_1KHZ = 0x0290,		// is a 1020 Hz frequency
	FREQ_500HZ = 0x0530

} freq_e;

/****************************   FUNCTION DECLARATIONS ************************/
void initTimer(void);
void setT1Delay(time_e);
void setT1Pwm(freq_e);
void setT2Delay(time_e);

/****************************   INLINE FUNCTIONS    **************************/
/*
 * Enables PWM output OCR1A.
 */
static inline void enablePWM1A()
{
  TCCR1A |= (1 << COM1A0);	// toggle PWM pin OCR1A
}

/*
 * Disables PWM output OCR1A
 *  // check if port must be set to low manually
 */
static inline void disablePWM1A()
{
  TCCR1A &= ~(1 << COM1A0);
}

/* timer 0
 * for input device check and ADC
 * interrupt every 1 ms based on 16 MHz requires prescaler 64
   TCCR0: FOC0|WGM00|COM01|COM00|WGM01|CS02|CS01|CS00
   TIMSK: NA|NA|NA|NA|NA|NA|OCIE0|TOIE0
 */
static inline void initTimer0()
{
  TCCR0 = (1 << CS01 );//| 1 << CS00);      // divide by 64
  TIMSK |= (1 << TOIE0);                // enable timer interrupt
}

/* timer 1(a) for tone PWM
  1.020 kHz = 16 MHz / 8 prescaler / 0x07A8
  TCNT1 : 0xCC00, same for ICR1, used as TOP value provider
  COM1A11:0 > 0b10, COM1B11:0 > 00 fast PWM, set 1 at TOP
  FOC1A:B > 0                     Disable forced compare
  WGM13:0 > 0b0110                Use OCR1 as compare value register
  CS12:0  > 0b010                 Clock source prescaling 8
  TICIE1  > 0                     ISR on input capture flag: top value reached
  TCCR1A: COM1A1|COM1A0|COM1B1|COM1B0|FOC1A|FOC1B|WGM11|WGM10
  TCCR1B: ICNC1|ICES1|res|WGM13|WGM12|CS12|CS11|CS10
  TIMSK:  NA|NA|TICIE1|OCIE1A|OCIE1B|TOIE1|NA|NA
 */
static inline void initTimer1()
{
  TIMSK &= ~(1 << TOIE1);
}

/* timer 2
 * used to control audio signal output duration (1 second)
   TCCR0: FOC2|WGM20|COM21|COM20|WGM21|CS22|CS21|CS20
   TIMSK: OCIE2|TOIE2|NA|NA|NA|NA|NA|NA
 */
static inline void initTimer2()
{
  // divide by 1024
  // results in interrupt every 1/61 of a second.
}


#endif /* TIMER_H_ */
