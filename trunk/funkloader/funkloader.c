/*
 * Copyright(C) Benedikt K.
 * Copyright(C) Juergen Eckert
 * Copyright(C) 2007 Ulrich Radig <mail@ulrichradig.de>
 * Copyright(C) 2007 Jochen Roessner <jochen@lugrot.de>
 * Copyright(C) 2007,2008 Stefan Siegl <stesie@brokenpipe.de>

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

#define noinline __attribute__((noinline))
#define naked    __attribute__((naked))

extern unsigned short rfm12_trans (unsigned short);
extern unsigned char  rfm12_wait_read (void);
extern void funkloader_tx_reply (void);

/* We need to store magic byte + page-number + a whole page + crc */
#define BUFSZ (SPM_PAGESIZE + 3)
unsigned char funkloader_buf[BUFSZ];

#define MAGIC_FLASH_PAGE 0x23
#define MAGIC_LAUNCH_APP 0x42

static void
timer_init (void)
{
  /* select clk/256 prescaler,
     at 8 MHz this means 31250 ticks per seconds, i.e. total timeout
     of 2.09 seconds. */
  TCCR1B = _BV (CS12);

  /* enable overflow interrupt of Timer 1 */
  TIMSK = _BV (TOIE1);

//  sei ();
}

static void
spi_init (void)
{
  /* configure MOSI, SCK, lines as outputs */
  SPI_DDR |= _BV(SPI_MOSI);
  SPI_DDR |= _BV(SPI_SCK);
  SPI_DDR &= ~_BV(SPI_MISO);

  /* initialize spi link to rfm12 module */
  SPI_CS_RFM12_DDR |= _BV(SPI_CS_RFM12);

  /* Enable the pullup */
  SPI_CS_RFM12_PORT |= _BV(SPI_CS_RFM12);

  /* enable spi, set master and clock modes (f/2) */
  SPCR = _BV(SPE) | _BV(MSTR);
  SPSR = _BV(SPI2X);
}

void rf12_setbandwidth(unsigned char bandwidth, unsigned char gain, unsigned char drssi)
{
    rfm12_trans(0x9400|((bandwidth&7)<<5)|((gain&3)<<3)|(drssi&7));
}

void rf12_setpower(unsigned char power, unsigned char mod)
{
    rfm12_trans(0x9800|(power&7)|((mod&15)<<4));
}

static void
rfm12_init (void)
{
  /* Wait for POR to finish. */
  while (INT_PORT & _BV (INT_PIN));

  rfm12_trans (0xC0E0);		/* AVR CLK: 10MHz */
  rfm12_trans (0x80D7);		/* Enable FIFO */
  rfm12_trans (0xC2AB);		/* Data Filter: internal */
  rfm12_trans (0xCA81);		/* Set FIFO mode */
  rfm12_trans (0xE000);		/* disable wakeuptimer */
  rfm12_trans (0xC800);		/* disable low duty cycle */
  rfm12_trans (0xC4F7);	        /* autotuning: -10kHz...+7,5kHz */
  rfm12_trans (0x0000);

//  rfm12_trans (0xa620);		/* rfm12_setfreq(RFM12FREQ(433.92)) */
  rfm12_trans(0xa6bf);
//  rfm12_trans (0x94ac);		/* rfm12_setbandwidth(5, 1, 4); */
    rf12_setbandwidth(4, 1, 4);
//  rfm12_trans (0xc627);		/* rfm12_setbaud(8620); */

#if baud<5400                  // Baudrate= 344827,58621/(R+1)/(1+CS*7)
    rfm12_trans(0xC680|((43104/baud)-1));
#else
    rfm12_trans(0xC600|((344828UL/baud)-1));
#endif

//  rfm12_trans (0x9820);		/* rfm12_setpower(0, 2); */
    rf12_setpower(0, 6);
}


static void
flash_page (void)
{
#if SPM_PAGESIZE < 256
  uint8_t i;
#else
  uint16_t i;
#endif

  uint16_t page = funkloader_buf[1] * SPM_PAGESIZE;

  eeprom_busy_wait();

  boot_page_erase(page);
  boot_spm_busy_wait();

  for(i = 0; i < SPM_PAGESIZE; i += 2) {
    /* Set up little-endian word. */
    uint16_t w = funkloader_buf[2 + i];
    w += funkloader_buf[3 + i] << 8;
        
    boot_page_fill (page + i, w);
  }

  boot_page_write (page);
  boot_spm_busy_wait();

  /* Reenable RWW-section again. */
  boot_rww_enable ();
}


static void
funkloader_rx ()
{
  rfm12_trans(0x82C8);		/* RX on */
  rfm12_trans(0xCA81);		/* set FIFO mode */
  rfm12_trans(0xCA83);		/* enable FIFO */

  uint8_t i = rfm12_wait_read ();
  uint8_t *ptr = funkloader_buf;
  *ptr = 0; 

#ifdef STATUS_LED_RX
  STATUS_LED_PORT |= _BV (STATUS_LED_RX);
#endif

  if (i > BUFSZ)
    goto out;

  while (i --)
    *(ptr ++) = rfm12_wait_read ();

 out:
  rfm12_trans(0x8208);		/* RX off */

#ifdef STATUS_LED_RX
  STATUS_LED_PORT &= ~_BV (STATUS_LED_RX);
#endif
}


static void
crc_update (unsigned char *crc, uint8_t data)
{
  for (uint8_t j = 0; j < 8; j ++)
    {
      if ((*crc ^ data) & 1)
	*crc = (*crc >> 1) ^ 0x8c;
      else
        *crc = (*crc >> 1);

      data = data >> 1;
    }
}


static uint8_t
crc_check (void)
{
  unsigned char crc_chk = 0;
  unsigned char *ptr = funkloader_buf + 2;

  for (uint8_t i = 0; i < SPM_PAGESIZE; i ++)
    crc_update (&crc_chk, *(ptr ++));

  /* subtract one from the other, this is far cheaper than comparation */
  crc_chk -= *ptr;
  return crc_chk;
}

naked void
funkloader_main (void)
{
  timer_init ();
  spi_init ();
  rfm12_init ();

#ifdef STATUS_LED_RX
  STATUS_LED_DDR |= _BV (STATUS_LED_RX);
#endif

#ifdef STATUS_LED_TX
  STATUS_LED_DDR |= _BV (STATUS_LED_TX);
#endif
STATUS_LED_PORT &= ~_BV (STATUS_LED_RX);
STATUS_LED_PORT &= ~_BV (STATUS_LED_TX);

//funkloader_buf[0] = MAGIC_LAUNCH_APP;
funkloader_buf[0] = 'H';
__asm volatile ("eor     r23, r23");
funkloader_tx_reply ();

  for (;;) 
    {
      /* try to receive a packet */
      funkloader_rx ();

      /* check packet validity */
      if (funkloader_buf[0] == MAGIC_LAUNCH_APP) {
//        STATUS_LED_PORT |= _BV (STATUS_LED_TX);
//        while(1);
        __asm volatile ("eor     r23, r23");
        funkloader_tx_reply (); /* reply: 0x42 0x00 */
	break;
      }

      if (funkloader_buf[0] != MAGIC_FLASH_PAGE)
	continue;		/* unknown magic, ignore. */

      if (crc_check ())
        continue;		/* crc invalid */

      /* clear global interrupt flag, so timer interrupt cannot
         call the application any longer. */
      cli ();

      /* flash page */
      flash_page ();

      /* transmit reply */
      __asm volatile ("lds     r23, funkloader_buf + %0 + 2"
                      :
		      : "i" (SPM_PAGESIZE));

      funkloader_tx_reply ();
    }
  
  /* leave here, thusly jump into application now */
  __asm volatile ("ret");
}

