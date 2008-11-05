#include <stdint.h>
#include <avr/io.h>
#include "config.h"
#include "fnordlicht.h"
#include "lib/uart.h"
#include "serial_handler.h"
#include "interfaces.h"
#include "cache.h"
#include <string.h>
#include "control.h"
//#include "settings.h"

#if SERIAL_UART
static char buffer[150];
uint8_t packet = 0;
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

void serial_putenc(uint8_t * d, uint8_t n)
{
    uint8_t i;
    for(i=0;i<n;i++){
        if(*d == 'a')
            uart1_putc('a');
        uart1_putc(*d++);
    }
}

uint8_t serial_packetOut(struct packet_t * p)
{
    if(p->flags & PACKET_DONE){
        uart1_puts("acSDab");
        //return 0;
    }
    if(p->flags & PACKET_TIMEOUT){
        uart1_puts("acSTab");
    }
    if(p->len == 0)             //hm somehow uggly
        return 0;
    uart1_puts("ac");
    if(p->flags & PACKET_BROADCAST)
        serial_putcenc('B');
    else
        serial_putcenc('P');
    serial_putcenc(0);
    serial_putcenc(p->dest);
    serial_putcenc(p->src);
    /*uint8_t c;
    for(c=0;c< p->len;c++){
        uart1_putc(p->data[c]);
        if(p->data[c] == 'a'){
            uart1_putc('a');
        }
    }*/
    serial_putenc(p->data,p->len);
    uart1_puts("ab");
    return 0;
}

uint8_t serial_packetIn(struct packet_t * p)
{
    if(!packet)
        return 1;
    //uart1_puts("acDnab");
    serial_nextHeader(p);
    memcpy(p->data,buffer+2,p->len);
    packet = 0;
    return 0;
}

uint8_t serial_nextHeader(struct packet_t * p)
{
    if(!packet)
        return 0;
    if(buffer[0] == 'P' || buffer[0] == 'B'){
        p->src = remote;
        p->dest = buffer[1];
        p->lasthop = remote;
        p->nexthop = packet_getAddress();
        p->flags = PACKET_REPLYDONE;
        if(buffer[0] == 'B'){
            p->flags |= PACKET_BROADCAST;
        }
        p->len = packet-2;
        //uart1_puts("acDsgab");
        return 1;
    }else{
        packet = 0;
    }
    return 0;
    
}

uint8_t serial_ready(void)
{
    return 1;
}

void serial_setadr(uint8_t remadr, uint8_t adr, uint8_t broadcast){
    control_init();
    packet_init(adr,broadcast);
    remote = remadr;
//    cache_set(remote, IFACE_SERIAL);
//    control_setserver(remote);
}

/*void serial_send(char * s)
{
    uint8_t c;
    uint8_t len = strlen(s);
    for(c=0;c< p->len;c++){
        uart1_putc(s);
        if(p->data[c] == 'a'){
            uart1_putc('a');
        }
    }
}*/

void serial_sendid(void)
{
    //uart1_puts("acB");
//    settings_readid(p.data);
    
}

void serial_tick(void)
{
    static uint8_t state = 0;
    if(state == 0){
        uart1_puts("acIab");
        state = 1;
    }

   
}

void serial_process(void)
{
    if(packet)
        return;
    uint8_t c = readline();
    if(c){
        if(buffer[0] == 'I'){
            serial_setadr(buffer[1],buffer[2],buffer[3]);
            //serial_sendid();
        }//else if(buffer[0] == 'B');
        else{
            packet = c;
        }
    } 
}
unsigned char readline( void )
{
    static int fill = 0;
    static uint8_t escaped = 0;
    int  i = uart1_getc();
    uint8_t data;

    if ( i & UART_NO_DATA ){
        return 0;
    }
    data = i&0xFF;

    if(data == 'a'){
        if(!escaped){
            escaped = 1;
            return 0;
        }
        escaped = 0;
    }else if(escaped){
        escaped = 0;
        if(data == 'c'){
            fill = 0;
            return 0;
        }else if( data == 'b'){
            return fill;
        }
    }
    //if(fill != -1){
        buffer[fill++] = data;
        if(fill >= 150)
            fill = 149;
    //}
    return 0;
}

#endif
