#include <avr/io.h>
#include "config.h"
#include "fnordlicht.h"
#include "common.h"
#include "rs485.h"
#include "pwm.h"
#include "control.h"

#if RS485_CTRL

void rs485_init(void)
{
    DDRD |= (1<<PD4)|(1<<PD5);
    PORTD &= ~(1<<PD4);
    PORTD &= ~(1<<PD5);
    /* init command bus */
    UCSR0A = _BV(MPCM0); /* enable multi-processor communication mode */
    UCSR0C = _BV(UCSZ00) | _BV(UCSZ01); /* 9 bit frame size */

    #define UART_UBRR 6 
    UBRR0H = HIGH(UART_UBRR);
    UBRR0L = LOW(UART_UBRR);

    UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(UCSZ02); /* enable receiver and transmitter */
}

#endif

