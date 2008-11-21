#include <avr/io.h>
#include "config.h"
#include "fnordlicht.h"
#include "common.h"
#include "rs485.h"
#include "pwm.h"
#include "control.h"
#include "uart.h"

#if RS485_CTRL

void rs485_init(void)
{
    RS485_TX_DDR |= (1<<RS485_TX_PIN);
    RS485_RX_DDR |= (1<<RS485_NRX_PIN);

    RS485_RX_PORT &= ~(1<<RS485_NRX_PIN);
    RS485_TX_PORT &= ~(1<<RS485_TX_PIN);
    
    /* init command bus */
    UCSR0A = _BV(MPCM0); /* enable multi-processor communication mode */
    UCSR0C = _BV(UCSZ00) | _BV(UCSZ01); /* 9 bit frame size */

    #define UART_UBRR 4         //230400 bei 18mhz 
    UBRR0H = HIGH(UART_UBRR);
    UBRR0L = LOW(UART_UBRR);

    UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(UCSZ02); /* enable receiver and transmitter */
}

void rs485_putc(uint8_t c)
{
    UDR0 = c;
}

uint16_t rs485_getc(void)
{
    volatile uint8_t v;
    if ((UCSR0A & (1<<DOR0 || UCSR1A & (1<<FE0)))) {
        v = UDR0;
        return UART_NO_DATA;
    }
    return UDR0;
}

#endif

