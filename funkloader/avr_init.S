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

	.file	"avr_init.S"
	.text
	.section        .init3,"ax",@progbits
	
.global avr_init
	.type	avr_init, @function
avr_init:
	;; clear r1
	eor	r1, r1

	;; initialize stack pointer
	ldi	r28, lo8(RAMEND)
	ldi	r29, hi8(RAMEND)
	out	SPH, r29
	out	SPL, r28

	;; clear SREG
	out	SREG, r1

	;; call main func
	rcall	funkloader_main
	nop			; fall through into application call

	;; timer1 overflow vector
	ldi	r16, _BV (IVCE)
	out	GICR, r16	; enable change reset vector

	out	GICR, r1	; change reset vector to application

	;; disable timer1 overflow interrupt, so the application 
	;; may call `sei' safely
	out	TIMSK, r1

	;; jump to application reset vector
	mov	r30, r1
	mov	r31, r1
	icall

	.size	avr_init, .-avr_init
