/* vim:fdm=marker et ai
 * {{{
 *
 * Copyright (c) 2007 by Christian Dietrich <stettberger@dokucode.de>
 * Copyright (c) 2008 by Stefan Siegl <stesie@brokenpipe.de>
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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
//#include <util/delay.h>
#include "bit-macros.h"
//#include "../eeprom.h"
//#include "../config.h"
//#include "../syslog/syslog.h"
//#include "../crypto/encrypt-llh.h"
//#include "../net/zbus_raw_net.h"
#include "config.h"
#include "zbus.h"

#ifndef ZBUS_USE_USART
#define ZBUS_USE_USART 0 
#endif
#define USE_USART ZBUS_USE_USART 
#define BAUD ZBUS_BAUDRATE
#include "esusart.h"

/*
#include <util/setbaud.h>
*/

#define HAVE_ZBUS_RX_PIN
#define ZBUS_RX_PIN_PIN     PC3
#define ZBUS_RX_PIN_PORT    C

#if RS485_CTRL
/* We generate our own usart init module, for our usart port */
generate_usart_init()

static uint8_t send_escape_data = 0;
static uint8_t recv_escape_data = 0;
static uint8_t bus_blocked = 0;

static volatile zbus_index_t zbus_index;
static volatile zbus_index_t zbus_txlen;
static volatile zbus_index_t zbus_rxlen;

volatile uint8_t zbus_txbuf[ZBUS_BUFFER_LEN];
volatile uint8_t zbus_buf[ZBUS_BUFFER_LEN];
static void __zbus_txstart(void);
volatile uint8_t zbus_done = 0;

uint8_t zbus_ready(void)
{
    if(zbus_txlen != 0 || zbus_rxlen != 0)// || bus_blocked)
        return 0;
    return 1;
}

void
zbus_txstart(zbus_index_t size)
{
  // FIXME
//  if(zbus_txlen != 0 || zbus_rxlen != 0 || bus_blocked)
//    return;			/* rx or tx in action or
//				   new packet left in buffer
//                                   or somebody is talking on the line */


#ifdef ZBUS_RAW_SUPPORT
  if (!zbus_raw_conn->rport)
#endif
  {
#ifdef SKIPJACK_SUPPORT
    zbus_encrypt (zbus_buf, &size);

    if (!size){
      uip_buf_unlock ();
      // FIXME
      zbus_rxstart ();		/* destroy the packet and restart rx */
      return;
    }
#endif
  }
  zbus_txlen = size;

  if(bus_blocked)
    return;
  __zbus_txstart();
}

static void __zbus_txstart(void) {

  uint8_t sreg = SREG; cli();
  bus_blocked = 3;

  zbus_index = 0;
//  PIN_SET(ZBUS_RX_PIN);
  //uart1_puts("acDZTab");
  /* enable transmitter and receiver as well as their interrupts */
  usart(UCSR,B) = _BV(usart(TXCIE)) | _BV(usart(TXEN));

  /* Enable transmitter */
  PIN_SET(RS485_TX_PIN);
  PIN_SET(RS485_NRX_PIN);
  //PIN_SET(ZBUS_RXTX_PIN);

  /* reset tx interrupt flag */
  usart(UCSR,A) |= _BV(usart(TXC));

  /* Go! */
  SREG = sreg;

  /* Transmit Start sequence */
  send_escape_data = ZBUS_START;
  usart(UDR) = '\\';

#ifdef HAVE_ZBUS_TX_PIN
  PIN_SET(ZBUS_TX_PIN);
#endif
//  PORTC |= (1<<PC3);
  return;
}


void
zbus_rxstart (void)
{
  if(zbus_txlen > 0){
    return;
  }
//  uart1_puts("acDZRab");
//  PIN_CLEAR(ZBUS_RX_PIN);
  zbus_rxlen = 0;

  uint8_t sreg = SREG; cli();

  /* disable transmitter, enable receiver (and rx interrupt) */
  usart(UCSR,B) = _BV(usart(RXCIE)) | _BV(usart(RXEN));

  /* Default is reciever enabled*/
  PIN_CLEAR(RS485_TX_PIN);
  PIN_CLEAR(RS485_NRX_PIN);

  SREG = sreg;
}

void zbus_rxdone(void)
{
}


static void
zbus_rxstop (void)
{
  uint8_t sreg = SREG; cli();

  /* completely disable usart */
  usart(UCSR,B) = 0;

  SREG = sreg;
}


zbus_index_t
zbus_rxfinish(void) 
{
  if (zbus_rxlen != 0) {
#ifdef SKIPJACK_SUPPORT
#ifdef ZBUS_RAW_SUPPORT
    if (!zbus_raw_conn->rport)
#endif
    zbus_decrypt(zbus_buf, (zbus_index_t *) &zbus_rxlen);
    if(!zbus_rxlen) {
      zbus_rxstart ();
      uip_buf_unlock();
    }

#endif
    return zbus_rxlen;
  }
  return 0;
}

void
zbus_core_init(void)
{
    /* Initialize the usart module */
    usart_init();

    /* Enable RX/TX Swtich as Output */
    DDR_CONFIG_OUT(RS485_TX_PIN);
    DDR_CONFIG_OUT(RS485_NRX_PIN);
#ifdef HAVE_ZBUS_RX_PIN
    DDR_CONFIG_OUT(ZBUS_RX_PIN);
#endif
#ifdef HAVE_ZBUS_TX_PIN
    DDR_CONFIG_OUT(ZBUS_TX_PIN);
#endif
//DDRC |= (1<<PC3);
//PORTC |= (1<<PC3);
    /* clear the buffers */
    zbus_txlen = 0;
    zbus_rxlen = 0;
    zbus_index = 0;

/*#ifndef TEENSY_SUPPORT
    uint16_t s_usart_baudrate;
    eeprom_restore_int(usart_baudrate, &s_usart_baudrate);
    uint16_t ubrr = usart_baudrate(s_usart_baudrate);
    usart(UBRR,H) = HI8(ubrr);
    usart(UBRR,L) = LO8(ubrr);
#endif*/

    zbus_rxstart ();
}

void
zbus_core_periodic(void)
{
  static uint8_t t  = 30;
  if(t-- == 0){
    if(bus_blocked)
      if(--bus_blocked == 0 && zbus_txlen > 0)
;//        __zbus_txstart();
    t = 6;
  }
}

SIGNAL(usart(USART,_TX_vect))
{
  /* If there's a carry byte, send it! */
  if (send_escape_data) {
    usart(UDR) = send_escape_data;
    send_escape_data = 0;
  }

  /* Otherwise send data from send context, if any is left. */
  else if (zbus_txlen && zbus_index < zbus_txlen) {
    if (zbus_txbuf[zbus_index] == '\\') {
      /* We need to quote the character. */
      send_escape_data = zbus_txbuf[zbus_index];
      usart(UDR) = '\\';
    }
    else {
      /* No quoting needed, just send it. */
      usart(UDR) = zbus_txbuf[zbus_index];
    }

    zbus_index ++;
    bus_blocked = 3;
  }

  /* If send_ctx contains data, but every byte has been sent over the
     wires, send a stop condition. */
  else if (zbus_txlen) {
    zbus_txlen = 0;		/* mark buffer as empty. */
//    uip_buf_unlock();

    /* Generate the stop condition. */
    send_escape_data = ZBUS_STOP;
    usart(UDR) = '\\';
  }

  /* Nothing to do, disable transmitter and TX LED. */
  else {
    bus_blocked = 0;
#ifdef HAVE_ZBUS_TX_PIN
    PIN_CLEAR(ZBUS_TX_PIN);
#endif
//PORTC &= ~(1<<PC3);
    zbus_txlen = 0;
    zbus_rxstart ();
    zbus_done = 1;
  }
}

SIGNAL(usart(USART,_RX_vect))
{
//  PIN_SET(ZBUS_RX_PIN);
  /* Ignore errors */
  if ((usart(UCSR,A) & _BV(usart(DOR))) || (usart(UCSR,A) & _BV(usart(FE)))) {
    uint8_t v = usart(UDR);
    (void) v;
    return; 
  }
  uint8_t data = usart(UDR);


  /* Old data is not read by application, ignore message */
  if (zbus_rxlen != 0) return;

  /* Don't accept incoming message if we're sending and sharing
     send and receive buffer. */
  if (zbus_txlen != 0) return;

  if (recv_escape_data) {
    recv_escape_data = 0;

    if (data == ZBUS_START) {
//      if (uip_buf_lock())
//        return; /* lock of buffer failed, ignore packet */
      
      zbus_index = 0;
      bus_blocked = 3;

#ifdef HAVE_ZBUS_RX_PIN
      PIN_SET(ZBUS_RX_PIN);
#endif
    }

    else if (data == ZBUS_STOP) {
      /* Only if there was a start condition before */
      if (bus_blocked) {
	zbus_rxstop ();

#ifdef HAVE_ZBUS_RX_PIN
        PIN_CLEAR(ZBUS_RX_PIN);
#endif
	zbus_rxlen = zbus_index;
      }

      /* force bus free even if we didn't catch the start condition. */
      bus_blocked = 0;
      if(zbus_txlen > 0)
        __zbus_txstart();
    }

    else if (data == '\\')
      goto append_data;
  } 

  else if (data == '\\') 
    recv_escape_data = 1;

  else {
  append_data:
    /* Not enough space in buffer */
    if (zbus_index >= ZBUS_BUFFER_LEN)
      return;

    /* If bus is not blocked we aren't on an message */
    //if (!bus_blocked)
    //  return;
      
    zbus_buf[zbus_index] = data;
    zbus_index++;
  }
}

#endif
