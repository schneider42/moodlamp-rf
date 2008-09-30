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

void rs485_process(void)
{
    if (UCSR0A & _BV(RXC0)) {

        uint8_t address = UCSR0B & _BV(RXB80); /* read nineth bit, zero if data, one if address */
        uint8_t data = UDR0;
        static uint8_t buffer[8];
        static uint8_t fill = 0;
//        uint8_t pos;
        if (UCSR0A & _BV(MPCM0) || address) { /* if MPCM mode is still active, or ninth bit set, this is an address packet */

            /* check if we are ment */
            if (data == 0 || data == RS485_ADDRESS) {

                /* remove MPCM flag and reset buffer fill counter */
                UCSR0A &= ~_BV(MPCM0);
                fill = 0;
            } else {/* turn on MPCM */

                UCSR0A |= _BV(MPCM0);
            }
        }else{

            /* else this is a data packet, put data into buffer */
            buffer[fill++] = data;

            if (buffer[0] == 0x01) {  /* soft reset */
                jump_to_bootloader();
            } else if (buffer[0] == 0x02 && fill == 4) { /* set color */
                control_setColor(buffer[1],buffer[2],buffer[3]);
                UCSR0A |= _BV(MPCM0); /* return to MPCM mode */
            } else if (buffer[0] == 0x03 && fill == 6) { /* fade to color */
                control_fade((buffer[1]<<8)+buffer[2],buffer[3],buffer[4],buffer[5]);
                UCSR0A |= _BV(MPCM0); /* return to MPCM mode */
            }
        }
    }
}
#endif

