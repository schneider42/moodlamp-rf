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

	.file	"rfm12_trans.S"
	.text


.global rfm12_trans
	.type	rfm12_trans, @function
rfm12_trans:
	rcall	rfm12_trans_bh
	in	r24, SPDR	; read lower reply byte
	clr	r25

	sbi	SPI_CS_RFM12_PORT, SPI_CS_RFM12
	
	ret
	.size	rfm12_trans, .-rfm12_trans


	;; .global rfm12_trans_bh
	.type	rfm12_trans_bh, @function
rfm12_trans_bh:
	sbi     SPCR, SPR0	; slow spi communication down
	cbi 	SPI_CS_RFM12_PORT, SPI_CS_RFM12

	/* send high byte first */
	out 	SPDR, r25

.L1:
	sbis 	SPSR, SPIF
	rjmp	.L1

	; in	r25, SPDR

	/* send low byte */
	out 	SPDR, r24

.L2:
	sbis 	SPSR, SPIF
	rjmp	.L2
	ret
	.size	rfm12_trans_bh, .-rfm12_trans_bh



.global rfm12_wait_tx
	.type	rfm12_wait_tx, @function
rfm12_wait_tx:
	/* Wait for the nIRQ pin to become low. */
.L0:
	sbic	INT_PORT, INT_PIN
	rjmp	.L0

	rcall	rfm12_trans_bh

	sbi	SPI_CS_RFM12_PORT, SPI_CS_RFM12
	
	ret
	.size	rfm12_wait_tx, .-rfm12_wait_tx
