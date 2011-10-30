/*
 * debug.h
 *
 *  Created on: Apr 17, 2011
 *      Author: Wolfgang Engelhard
 *
 * Copyright: (c) 2011 by Wolfgang Engelhard
 * License: GNU GPL v2 (see License.txt)
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#define DEBUG_LEVEL 1


#if DEBUG_LEVEL > 0
#   define  DBG(data)       debug(data)
#   define  DBG_WARN(data, len) sendUartString_n((uint8_t*)data, len)
#   define  DBG_DELAY_8BIT(value) for(;value > 0; value--){ asm("nop"); }
#   define  DBG_WARN_P(data)     sendUartString_P((uint8_t*)data)

extern const uint8_t s_baud_error[];
#else
#   define  DBG_WARN(data, len)
#   define  DBG(data)
#   define  DBG_P(data)
#endif

#if DEBUG_LEVEL > 1
#   define  DBG_INFO(data, len) sendUartString_n(data, len)
#   define  DBG_INFO_P(data) sendUartString_P((uint8_t*)data)
#else
#   define  DBG_INFO(data, len)
#   define  DBG_INFO_P(data)
#endif

#if DEBUG_LEVEL > 0

typedef struct {
  unsigned keyPoll: 1;
  unsigned pwm: 1;
  unsigned pollChannel: 1;
  unsigned readChannel: 1;
} debugFlag_t;


/****************************   GLOBAL VARIABLES    **************************/
extern debugFlag_t       debugFlag;

/****************************   FUNCTION DECLARATIONS ************************/
extern void sendUartString_n(uint8_t *data, uint8_t len);

extern void debug(uint8_t data);
#endif

#endif /* DEBUG_H_ */
