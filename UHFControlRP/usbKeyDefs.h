/*
 * usbKeyDefs.h
 *
 *  Created on: 07.09.2011
 *      Author: Wolfgang Engelhard
 *
 * Copyright: (c) 2011 by Wolfgang Engelhard
 * License: GNU GPL v2 (see License.txt)
 */

#ifndef USBKEYDEFS_H_
#define USBKEYDEFS_H_


typedef enum
{
	NOT_USED,
	KEY_OFF,
	KEY_MAIN,
	KEY_BOTH,
	KEY_ADF,
	KEY_MNL,
	KEY_PRESET,
	KEY_GUARD,
	KEY_CH_INC,
	KEY_CH_DEC
} keyCode_e;

#endif /* USBKEYDEFS_H_ */
