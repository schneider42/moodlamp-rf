/* vim:fdm=marker ts=4 et ai
 * {{{
 *
 * Copyright (c) 2007 by Christian Dietrich <stettberger@dokucode.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * For more information on the GPL, please go to:
 * http://www.gnu.org/copyleft/gpl.html
 }}} */

#ifndef _ZBUS_H
#define _ZBUS_H

#include <stdint.h>
#include "config.h"
//#include "../uip/uip.h"

#if RS485_CTRL

//#define ZBUS_RXTX_PIN_PORT  C
//#define ZBUS_RXTX_PIN_PIN   PC0

#define ZBUS_BUFFER_LEN    270
//#define ZBUS_BUFFER_LEN    150

//#define zbus_buf           (uip_buf + ZBUS_BRIDGE_OFFSET)

#if 0
#ifdef TEENSY_SUPPORT
#  if ZBUS_BUFFER_LEN > 254
#    error "modify code or shrink (shared) uIP buffer."
#  endif
typedef uint8_t zbus_index_t;
#else   /* TEENSY_SUPPORT */
typedef uint16_t zbus_index_t;
#endif	/* not TEENSY_SUPPORT */
#endif

typedef uint16_t zbus_index_t;

enum ZBusEscapes {
  ZBUS_START = '0',
  ZBUS_STOP = '1',
};

void zbus_core_init(void);
void zbus_core_periodic(void);

uint8_t zbus_ready(void);
void zbus_txstart(zbus_index_t len);

void zbus_rxstart (void);
zbus_index_t zbus_rxfinish(void);

void zbus_process(void);

extern volatile uint8_t zbus_txbuf[ZBUS_BUFFER_LEN];
extern volatile uint8_t zbus_buf[ZBUS_BUFFER_LEN];
extern volatile uint8_t zbus_done;
void zbus_rxdone(void);

#endif /* ZBUS_SUPPORT */

#endif /* _ZBUS_H */
