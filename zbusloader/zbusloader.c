/*
 * Copyright(C) 2007,2008 Stefan Siegl <stesie@brokenpipe.de>
 * Copyright(C) 2008 Christian Dietrich <stettberger@dokucode.de>

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */

#include <avr/pgmspace.h>
#include <avr/boot.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "pinconfig.h"
#include <avr/wdt.h>

#define noinline __attribute__((noinline))
#define naked    __attribute__((naked))

/* We need to store magic byte + page-number + a whole page + crc */
#define BUFSZ (SPM_PAGESIZE + 3)
unsigned char zbusloader_buf[BUFSZ];
unsigned char zbusloader_tx_buf[2];

#define MAGIC_FLASH_PAGE 0x23
#define MAGIC_LAUNCH_APP 0x42
#define ZBUS_UBRR 9


static void timer_init (void)
{
  /* select clk/256 prescaler,
     at 8 MHz this means 31250 ticks per seconds, i.e. total timeout
     of 2.09 seconds. */
    TCCR1B = _BV (CS12);
    /* enable overflow interrupt of Timer 1 */
    TIMSK1 = _BV (TOIE1);
    sei ();
}

void zbus_init(void)
{
    /* set baud rate */
    UBRR0H = ZBUS_UBRR >> 8;
    UBRR0L = ZBUS_UBRR;

    ZBUS_TX_DDR |= _BV(ZBUS_TX_PIN);
    ZBUS_TX_PORT &= ~_BV(ZBUS_TX_PIN);
    
    ZBUS_nRX_DDR |= _BV(ZBUS_nRX_PIN);
    ZBUS_nRX_PORT &= ~_BV(ZBUS_nRX_PIN);

    /* set mode: 8 bits, 1 stop, no parity, asynchronous usart
       and Set URSEL so we write UCSRC and not UBRRH */
    UCSR0C = _BV(UCSZ00) | _BV(UCSZ01); //| _BV(URSEL);
    /* Enable the RX interrupt and receiver and transmitter */
    UCSR0B |= _BV(TXEN0) | _BV(RXEN0); 

}


static void flash_page (void)
{
//#if SPM_PAGESIZE < 256
//  uint8_t i;
//#else
    uint16_t j;
//#endif

    uint16_t page = zbusloader_buf[1] * SPM_PAGESIZE;

    eeprom_busy_wait();

    boot_page_erase(page);
    boot_spm_busy_wait();

    for(j = 0; j < SPM_PAGESIZE; j += 2) {
        /* Set up little-endian word. */
        uint16_t w = zbusloader_buf[2 + j];
        w += zbusloader_buf[3 + j] << 8;    
        boot_page_fill (page + j, w);
    }

    boot_page_write (page);
    boot_spm_busy_wait();

    /* Reenable RWW-section again. */
    boot_rww_enable ();
}

volatile uint16_t i = 0x17;

void zbusloader_rx ()
{
    uint8_t last = 0;
    uint8_t current;
    uint8_t started = 0;
    for(i=0;i<BUFSZ;i++)
        zbusloader_buf[i] = 0xAA;
    i = 0;

    while (1) {
        /* While an byte is recieved */
        while ( !(UCSR0A & _BV(RXC0)) );
        if ((UCSR0A & _BV(DOR0)) || (UCSR0A & _BV(FE0))) {
	        current = UDR0;
	        continue;
        }
        current = UDR0;

        if (last == '\\') {
	        if (current == '0') {
	            started = 1;
#ifdef STATUS_LED_RX
STATUS_LED_PORT |= _BV (STATUS_LED_RX);
#endif
	        } else if (current == '1'){ 
	            break;
            } else { 
	            goto append_data;
            }
        } else {
	        if (current == '\\') goto save_current;
append_data:
	        if(started == 0) goto save_current;
	        
            zbusloader_buf[i++] = current;
        }
save_current:
        if (last == '\\')
            last = 0;
        else
            last = current;
    }
#ifdef STATUS_LED_RX
STATUS_LED_PORT &= ~_BV (STATUS_LED_RX);
#endif
  if (i <= BUFSZ){
    return;
  }
  zbusloader_buf[0] = 0;
}

void zbus_send_byte(uint8_t data)
{
    UDR0 = data;
    while ( !( UCSR0A & _BV(TXC0)) );
    UCSR0A |= _BV(TXC0);
}

void zbusloader_tx_reply(void) 
{
#ifdef STATUS_LED_TX
STATUS_LED_PORT |= _BV (STATUS_LED_TX);
#endif
    ZBUS_TX_PORT |= _BV(ZBUS_TX_PIN);
    ZBUS_nRX_PORT |= _BV(ZBUS_nRX_PIN);

    /* Start Conditon */
    zbus_send_byte('\\');
    zbus_send_byte('0');
    /* byte one */
    zbus_send_byte(zbusloader_tx_buf[0]);
    /* byte two: can be \ */
    if (zbusloader_tx_buf[1] == '\\') {
        zbus_send_byte('\\');
    }
    zbus_send_byte(zbusloader_tx_buf[1]);
    /* Stop Condition */
    zbus_send_byte('\\');
    zbus_send_byte('1');

    ZBUS_TX_PORT &= ~_BV(ZBUS_TX_PIN);
    ZBUS_nRX_PORT &= ~_BV(ZBUS_nRX_PIN);

#ifdef STATUS_LED_TX
STATUS_LED_PORT &= ~_BV (STATUS_LED_TX);
#endif
}


static void crc_update (unsigned char *crc, uint8_t data)
{
    for (uint8_t j = 0; j < 8; j ++){
        if ((*crc ^ data) & 1)
	        *crc = (*crc >> 1) ^ 0x8c;
        else
            *crc = (*crc >> 1);
        data = data >> 1;
    }
}


static uint8_t crc_check (void)
{
    unsigned char crc_chk = 0;
    unsigned char *ptr = zbusloader_buf + 2;

    for (uint16_t j = 0; j < SPM_PAGESIZE; j ++)
        crc_update (&crc_chk, *(ptr ++));

    /* subtract one from the other, this is far cheaper than comparation */
    crc_chk -= *ptr;
    return crc_chk;
}

naked void
zbusloader_main (void)
//int main(void)
{
//    wdt_disable();
    timer_init ();
    zbus_init();
//    cli();
    UCSR0A |= _BV(TXC0);
#ifdef STATUS_LED_RX
    STATUS_LED_DDR |= _BV (STATUS_LED_RX);
#endif

#ifdef STATUS_LED_TX
    STATUS_LED_DDR |= _BV (STATUS_LED_TX);
#endif
//    ZBUS_nRX_DDR |= _BV(ZBUS_TX_PIN);
//    ZBUS_nRX_PORT ^= _BV(ZBUS_TX_PIN);

//    STATUS_LED_PORT ^= _BV (STATUS_LED_TX);    
//    zbusloader_tx_reply ();
//    while(1);
    while(1){
        /* try to receive a packet */
        zbusloader_rx ();

        /* check packet validity */
       if (zbusloader_buf[0] == MAGIC_LAUNCH_APP) {
            _delay_us(50);
            zbusloader_tx_buf[0] = 0x42;
            zbusloader_tx_buf[1] = 0;
            zbusloader_tx_reply ();
            break;
        }
        /*zbusloader_tx_buf[0] = zbusloader_buf[0];
        zbusloader_tx_buf[1] = zbusloader_buf[BUFSZ-1];
        zbusloader_tx_reply ();

        zbusloader_tx_buf[0] = (i>>8)&0xFF;
        zbusloader_tx_buf[1] = i&0xff;
        zbusloader_tx_reply ();*/

        if (zbusloader_buf[0] != MAGIC_FLASH_PAGE)
	        continue;		/* unknown magic, ignore. */

        if (crc_check ())
	        continue;		/* crc invalid */

        /* clear global interrupt flag, so timer interrupt cannot
            call the application any longer. */
        cli ();

        /* flash page */
        flash_page ();

        /* transmit reply */
        zbusloader_tx_buf[0] = 0x23;
        zbusloader_tx_buf[1] = zbusloader_buf[BUFSZ - 1];
        zbusloader_tx_reply ();
    }
  
    /* leave here, thusly jump into application now */
    __asm volatile ("ret");
    STATUS_LED_PORT |= _BV (STATUS_LED_TX);
}

