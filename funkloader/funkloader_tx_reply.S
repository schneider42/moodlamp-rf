/*
 * Copyright(C) 2008 Stefan Siegl <stesie@brokenpipe.de>
 *
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

#define  __SFR_OFFSET 0
#include <avr/io.h>

#include "pinconfig.h"

	.file	"funkloader_tx_reply.S"
	.text
	
.global funkloader_tx_reply
	.type	funkloader_tx_reply, @function
funkloader_tx_reply:
#ifdef STATUS_LED_TX
	sbi	STATUS_LED_PORT, STATUS_LED_TX
#endif
	
	ldi     r24, 0x38
	ldi     r25, 0x82
	rcall	rfm12_trans	; tx on

	ldi	r25, 0xB8	; tx data byte
	
	;; send preamble: AA, AA, AA, 2D, D4
	ldi	r24, 0xAA
	rcall	rfm12_wait_tx
	rcall	rfm12_wait_tx
	rcall	rfm12_wait_tx

	ldi	r24, 0x2D
	rcall	rfm12_wait_tx
	
	ldi	r24, 0xD4
	rcall	rfm12_wait_tx

	;; packet length
	ldi     r24, 2
	rcall	rfm12_wait_tx

	;; first data byte
	lds	r24, funkloader_buf
	rcall 	rfm12_wait_tx

	;; second data byte
	mov	r24, r23
	rcall	rfm12_wait_tx

	;; postamble: AA, AA
	ldi	r24, 0xAA
	rcall	rfm12_wait_tx
	rcall	rfm12_wait_tx

	ldi     r24, 0x08
	ldi     r25, 0x82
	rcall	rfm12_trans	; tx off

#ifdef STATUS_LED_TX
	cbi	STATUS_LED_PORT, STATUS_LED_TX
#endif

	ret
	.size	funkloader_tx_reply, .-funkloader_tx_reply

