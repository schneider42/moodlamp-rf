/* vim:fdm=marker ts=4 et ai
 * {{{
 *         fnordlicht firmware next generation
 *
 *    for additional information please
 *    see http://koeln.ccc.de/prozesse/running/fnordlicht
 *
 * (c) by Alexander Neumann <alexander@bumpern.de>
 *     Lars Noschinski <lars@public.noschinski.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
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

#ifndef _FNORDLICHT_CONFIG_H
#define _FNORDLICHT_CONFIG_H

#define BOARD_0_1       0       //old moodlamp design
#define BOARD_RF_0_1    1       //proto rf board
#define BOARD_RF_0_2    2       //current rf board

#ifndef BOARD
#define BOARD   BOARD_RF_0_2
#endif

#ifndef F_CPU
    #define F_CPU 18432000UL
#endif

#ifndef LED_PORT
#define LED_PORT PORTA
#endif

#ifndef LED_PORT_DDR
#define LED_PORT_DDR DDRA
#endif

#ifndef LED_PORT_INVERT
#define LED_PORT_INVERT 0
#endif

/* color <-> channel assignment */
#define CHANNEL_RED     0
#define CHANNEL_GREEN   1
#define CHANNEL_BLUE    2

#define RC5_DECODER 1
#define RS485_CTRL  1
#define SCRIPT_SPEED_CONTROL 1
#define STATIC_SCRIPTS 1
#define USB_CTRL 1
#define UART_BAUDRATE 230400
#define SERIAL_UART 1

/* debug defines */
#ifndef DEBUG
#define DEBUG 0
#endif

/* include the script intpreter per default */
#ifndef STATIC_SCRIPTS
#define STATIC_SCRIPTS 1
#endif

/* include uart support per default */
#ifndef SERIAL_UART
#define SERIAL_UART 0
#endif

/* disable at keyboard decoder per default (EXPERIMENTAL) */
/* ATTENTION: THIS IS EXPERIMENTAL AND DOES NOT WORK ATM! */
#ifndef AT_KEYBOARD
#define AT_KEYBOARD 0
#endif

/* disable rc5-decoder per default */
#ifndef RC5_DECODER
#define RC5_DECODER 1
#endif

/* disable scripts speed control per default */
#ifndef SCRIPT_SPEED_CONTROL
#define SCRIPT_SPEED_CONTROL 1
#endif

/* fifo size must be a power of 2 and below 128 */
//#define UART_FIFO_SIZE 32
#ifndef UART_BAUDRATE
#define UART_BAUDRATE 19200
#endif


/* enable this if you want to control a fnordlicht via RS485 */
#if RS485_CTRL

#define TEENSY_SUPPORT

#define RS485_TX_PIN_PORT   C
#define RS485_TX_PIN_PIN    PC4

#define RS485_NRX_PIN_PORT  C
#define RS485_NRX_PIN_PIN   PC5

#define ZBUS_BAUDRATE 115200
/*#define RS485_TX_DDR    DDRD
#define RS485_RX_DDR    DDRD
#define RS485_RX_PORT   PORTD
#define RS485_TX_PORT   PORTD

#define RS485_TX_PIN    PD4
#define RS485_NRX_PIN   PD5*/

#endif

/*#if (RS485_CTRL == 1) && (SERIAL_UART == 1) && !(ROLE == ROLE_MASTER)
#error "RS485_CTRL and SERIAL_UART are mutually exclusive!"
#endif*/


#include <avr/version.h>

/* check for avr-libc version */
#if __AVR_LIBC_VERSION__ < 10602UL
#error newer libc version (>= 1.6.2) needed!
#endif

/* check if cpu speed is defined */
#ifndef F_CPU
#error please define F_CPU!
#endif

/* check if this cpu is supported */
#if !(defined(__AVR_ATmega8__) || defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega16__) || defined(__AVR_ATmega324P__)  || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega32__))
#error "this cpu isn't supported yet!"
#endif

/* cpu specific configuration registers */
#if defined(__AVR_ATmega8__)
/* {{{ */
#define _ATMEGA8

#define _TIMSK_TIMER1 TIMSK
#define _UCSRB_UART0 UCSRB
#define _UDRIE_UART0 UDRIE
#define _TXEN_UART0 TXEN
#define _RXEN_UART0 RXEN
#define _RXCIE_UART0 RXCIE
#define _UBRRH_UART0 UBRRH
#define _UBRRL_UART0 UBRRL
#define _UCSRC_UART0 UCSRC
#define _UCSZ0_UART0 UCSZ0
#define _UCSZ1_UART0 UCSZ1
#define _SIG_UART_RECV_UART0 SIG_UART_RECV
#define _SIG_UART_DATA_UART0 SIG_UART_DATA
#define _UDR_UART0 UDR
#define UCSR0A UCSRA
#define UCSR0C UCSRC
#define MPCM0 MPCM
#define UCSZ00 UCSZ0
#define UCSZ01 UCSZ1
#define UCSZ02 UCSZ2
#define UBRR0H UBRRH
#define UBRR0L UBRRL
#define UCSR0B UCSRB
#define RXEN0 RXEN
#define TXEN0 TXEN
#define RXC0 RXC
#define RXB80 RXB8
#define UDR0 UDR

/* }}} */
#elif defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__)
/* {{{ */
#define _ATMEGA16

#define _EICRA MCUCR
#define _EIMSK GICR
#define _TIMSK_TIMER1 TIMSK
#define _UCSRB_UART0 UCSRB
#define _UDRIE_UART0 UDRIE
#define _TXEN_UART0 TXEN
#define _RXEN_UART0 RXEN
#define _RXCIE_UART0 RXCIE
#define _UBRRH_UART0 UBRRH
#define _UBRRL_UART0 UBRRL
#define _UCSRC_UART0 UCSRC
#define _UCSZ0_UART0 UCSZ0
#define _UCSZ1_UART0 UCSZ1
#define _SIG_UART_RECV_UART0 SIG_UART_RECV
#define _SIG_UART_DATA_UART0 SIG_UART_DATA
#define _UDR_UART0 UDR
#define UCSR0A UCSRA
#define UCSR0C UCSRC
#define MPCM0 MPCM
#define UCSZ00 UCSZ0
#define UCSZ01 UCSZ1
#define UCSZ02 UCSZ2
#define UBRR0H UBRRH
#define UBRR0L UBRRL
#define UCSR0B UCSRB
#define RXEN0 RXEN
#define TXEN0 TXEN
#define RXC0 RXC
#define RXB80 RXB8
#define UDR0 UDR

/* }}} */

#elif defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__)|| defined(__AVR_ATmega324P__) || defined(__AVR_ATmega644P__)
/* {{{ */
#define _ATMEGA88
#define _EICRA EICRA
#define _EIMSK EIMSK
#define _TIMSK_TIMER1 TIMSK1
#define _UCSRB_UART0 UCSR0B
#define _UDRIE_UART0 UDRIE0
#define _TXEN_UART0 TXEN0
#define _RXEN_UART0 RXEN0
#define _RXCIE_UART0 RXCIE0
#define _UBRRH_UART0 UBRR0H
#define _UBRRL_UART0 UBRR0L
#define _UCSRC_UART0 UCSR0C
#define _UCSZ0_UART0 UCSZ00
#define _UCSZ1_UART0 UCSZ01
#define _SIG_UART_RECV_UART0 SIG_USART_RECV
#define _SIG_UART_DATA_UART0 SIG_USART_DATA
#define _UDR_UART0 UDR0
/* }}} */
#endif


#endif /* _FNORDLICHT_CONFIG_H */
