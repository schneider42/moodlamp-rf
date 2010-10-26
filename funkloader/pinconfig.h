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

#ifndef PINCONFIG_H
#define PINCONFIG_H

#define SPI_CS_RFM12_PORT PORTB
#define SPI_CS_RFM12_DDR  DDRB
#define SPI_CS_RFM12      PB4

#define SPI_DDR           DDRB
#define SPI_MOSI          PB5
#define SPI_MISO          PB6
#define SPI_SCK           PB7

#define INT_PORT          PIND
#define INT_PIN           PD2

#define baud 19200
/* If you don't want to have status LEDs, simply comment out these */
#define STATUS_LED_PORT   PORTA
#define STATUS_LED_DDR    DDRA
#define STATUS_LED_TX     PA0
#define STATUS_LED_RX     PA1



#endif
