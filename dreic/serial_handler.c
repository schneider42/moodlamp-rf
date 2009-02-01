#include <stdint.h>
#include <avr/io.h>
#include "lib/uart.h"
#include "serial_handler.h"
#include <string.h>

char buffer[SERIAL_BUFFERLEN];
uint8_t serial_buffer[SERIAL_BUFFERLEN];

uint16_t packet = 0;
uint8_t remote = 0;

void serial_putcenc(uint8_t c)
{
    if(c == 'a')
        uart1_putc('a');
    uart1_putc(c);
}

void serial_putsenc(uint8_t * s)
{
    while(*s){
        if(*s == 'a')
            uart1_putc('a');
        uart1_putc(*s++);
    }
}

void serial_putenc(uint8_t * d, uint16_t n)
{
    uint16_t i;
    for(i=0;i<n;i++){
        if(*d == 'a')
            uart1_putc('a');
        uart1_putc(*d++);
    }
}

uint16_t serial_readline(void)
{
    uint16_t l;
    l = readline();
    if(l){
        memcpy(serial_buffer,buffer,l);
    }
    return l;
}

unsigned int readline( void )
{
    static int fill = 0;
    static uint8_t escaped = 0;
    int  i = uart1_getc();
    uint8_t data;

    if ( i & UART_NO_DATA ){
        return 0;
    }
    data = i&0xFF;

    if(data == SERIAL_ESCAPE){
        if(!escaped){
            escaped = 1;
            return 0;
        }
        escaped = 0;
    }else if(escaped){
        escaped = 0;
        if(data == SERIAL_START){
            fill = 0;
            return 0;
        }else if( data == SERIAL_END){
            return fill;
        }
    }
    //if(fill != -1){
        buffer[fill++] = data;
        if(fill >= SERIAL_BUFFERLEN)
            fill = SERIAL_BUFFERLEN -1;
    //}
    return 0;
}

