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
#include "cmd_handler.h"
#include "leds.h"

//#include "settings.h"

#if SERIAL_UART
char buffer[SERIAL_BUFFERLEN];
uint16_t packet = 0;
uint8_t remote = 0;

void serial_putcenc(uint8_t c)
{
    if(c == SERIAL_ESCAPE)
        uart1_putc(SERIAL_ESCAPE);
    uart1_putc(c);
}

void serial_putsenc(char * s)
{
    while(*s){
        if(*s == SERIAL_ESCAPE)
            uart1_putc(SERIAL_ESCAPE);
        uart1_putc(*s++);
    }
}

void serial_putenc(uint8_t * d, uint16_t n)
{
    uint16_t i;
    for(i=0;i<n;i++){
        if(*d == SERIAL_ESCAPE)
            uart1_putc(SERIAL_ESCAPE);
        uart1_putc(*d++);
    }
}

inline void serial_putStart(void)
{
    uart1_putc(SERIAL_ESCAPE);
    uart1_putc(SERIAL_START);
}

inline void serial_putStop(void)
{
    uart1_putc(SERIAL_ESCAPE);
    uart1_putc(SERIAL_END);
}

void serial_sendFrames(char * s)
{
    serial_putStart();
    serial_putsenc(s);
    serial_putStop();
}

void serial_sendFramec(uint8_t s)
{
    serial_putStart();
    serial_putcenc(s);
    serial_putStop();
}

uint8_t serial_packetOut(struct packet_t * p)
{
    leds_tx();
    if(p->flags & PACKET_DONE){         //these are special flags
        serial_sendFrames("SD");           //for the host system
        //return 0;                     //they don't contain any data
    }
    if(p->flags & PACKET_TIMEOUT){
        serial_sendFrames("ST");
    }
    if(p->len == 0)             //hm somehow uggly
        return 0;
    serial_putStart();
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
    serial_putStop();
    leds_txend();
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
}


void serial_sendid(void)
{
    //uart1_puts("acB");
//    settings_readid(p.data);
    
}

void serial_tick(void)
{
    static uint8_t state = SERIAL_STATE_INIT;
    if(state == SERIAL_STATE_INIT){
        serial_sendFramec(SERIAL_INIT);
        state = SERIAL_STATE_NORMAL;
    } 
}

uint16_t serial_rawlen(void)
{
    return packet;
}

uint8_t * serial_rawgetbuffer(void)
{
    return (uint8_t *) buffer + 1;
}

void serial_rawdone(void)
{
    packet = 0;
}

void serial_process(void)
{
    if(packet)
        return;
    uint16_t c = readline();
    if(c){
        if(buffer[0] == SERIAL_INIT){
            serial_setadr(buffer[1],buffer[2],buffer[3]);
            //serial_sendid();
        }else if(buffer[0] == CMD_RAW){
            cmd_handler(CMD_RAW,(uint8_t *)buffer+1,NULL);
        }else if(buffer[0] == SERIAL_RAWDATA){
            packet = c-1;
        }else{
            packet = c;
        }
    } 
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
    leds_rx();
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
            leds_rxend();
            return fill;
        }
    }
    buffer[fill++] = data;
    if(fill >= SERIAL_BUFFERLEN)
        fill = SERIAL_BUFFERLEN - 1;
    return 0;
}

#endif
