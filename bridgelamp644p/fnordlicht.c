#include "config.h"

#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
#include <avr/wdt.h>

#include "common.h"
#include "fnordlicht.h"
#include "lib/uart.h"

#if SERIAL_UART
int uart_putc_file(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(uart_putc_file, NULL, _FDEV_SETUP_WRITE);
#endif
void setcolor(unsigned char adr, unsigned char r, unsigned char g, unsigned char b);
#if SERIAL_UART
static inline void check_serial_input(uint8_t data);
#endif

int uart_putc_file(char c, FILE *stream)
{
    uart1_putc(c);    
    return 0;
}

void jump_to_bootloader(void)
{
    wdt_enable(WDTO_30MS);
    while(1);
}

#if SERIAL_UART
/** process serial data received by uart */
void check_serial_input(uint8_t data)
/* {{{ */ {
    static uint8_t buffer[10];
    static int16_t fill = -1;
    static uint8_t escaped = 0;

    if(data == 0xAA){
        if(!escaped){
            escaped = 1;
            return;
        }
        escaped = 0;
    }else if(escaped){
        escaped = 0;
        if(data == 0x01){
            fill = 0;
            return;
        }
    }
    if(fill != -1){
        buffer[fill++] = data;
        if(fill >= 10)
            fill = -1;
    }
    uint8_t pos;
    if (buffer[0] == 0x01 && fill == 1) {  /* soft reset */

        jump_to_bootloader();
        
    } else if (buffer[0] == 0x02 && fill == 5) { /* set color */

        setcolor(buffer[1],buffer[2],buffer[3],buffer[4]);
        fill = -1;
        uart1_puts("acSDab");

    } else if (buffer[0] == 0x03 && fill == 6) { /* fade to color */

        for (pos = 0; pos < 3; pos++) {
        }

        fill = -1;
    }
    
} /* }}} */
#endif
/** main function
 */
void rs485_send( unsigned int data )
{
    /* Wait for empty transmit buffer */
    while ( !( UCSR0A & (1<<UDRE0)));
    /* Copy 9th bit to TXB8 */
    UCSR0B &= ~(1<<TXB80);
    if ( data & 0x0100 )
        UCSR0B |= (1<<TXB80);
    /* Put data into buffer, sends the data */
    UDR0 = data&0xFF;
}
void setcolor(unsigned char adr, unsigned char r, unsigned char g, unsigned char b)
{
    PORTA |= (1<<PA3);
    rs485_send(0x100|adr);
    rs485_send(2);
    rs485_send(r);
    rs485_send(g);
    rs485_send(b);
    PORTA &= ~(1<<PA3);
}

int main(void) {
    wdt_disable();
#if SERIAL_UART
    uart1_init( UART_BAUD_SELECT(UART_BAUDRATE,F_CPU));
    stdout = &mystdout;
#endif

#if RS485_CTRL
//    DDRD |= (1<<PD4)|(1<<PD5);
//    PORTD &= (1<<PD5);
//    PORTD |= (1<<PD4);
    /* init command bus */
    UCSR0A = _BV(MPCM0); /* enable multi-processor communication mode */
    UCSR0C = _BV(UCSZ00) | _BV(UCSZ01); /* 9 bit frame size */
    #define UART_UBRR 7 /* 115200 baud at 16mhz */
    UBRR0H = HIGH(UART_UBRR);
    UBRR0L = LOW(UART_UBRR);
    
    UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(UCSZ02); /* enable receiver and transmitter */
#endif

    sei();
    //while(1)
    printf("acDInit doneab");
    //char c = 0x55;
    //volatile long l;
    unsigned int i;
    DDRA |= (1<<PA3);
    
    while(1){
        i = uart1_getc();
        if(!(i & UART_NO_DATA)){
            check_serial_input(i&0xff);
        }
        //for(l=1;l<1000;l++);
        //setcolor(10,c++,c,c);
    }
}
