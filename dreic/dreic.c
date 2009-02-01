#include <stdlib.h>
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <string.h>

#include "lib/uart.h"
#include "serial_handler.h"
#include "zbusneu.h"
#include "packet.h"
#include "cmd_handler.h"
#include "fnordlicht.h"

#define UART_BAUDRATE   115200
#define DREIC_COLOR     'C'
#define DREIC_FADE      'F'

#define DREIC_DONE      "D"

#define PRE_500MS      (6000)
//((F_CPU/512) / 2)
#define PRE_1MS         (PRE_500MS / 500)


volatile uint8_t timebase = 0;
struct packet_t outpacket;
uint8_t sendpacket = 0;
uint8_t debug = 0;

ISR(TIMER0_OVF_vect)
{
    static uint16_t tick = 0;
    TCNT0 = -2;
    tick++;
    if(tick > 2){
        tick = 0;
        timebase = 1;
    }
}

void preparePacket(uint8_t adr, struct packet_t * p)
{
    p->src = 1;
    p->dest = adr;
    p->lasthop = 1;
    p->nexthop = adr;
    p->flags = PACKET_BROADCAST;
    p->iface = 1;
    p->len = 0;
}

void colorPacket(struct packet_t * p, uint8_t adr, uint8_t r, uint8_t g, uint8_t b)
{
    preparePacket(adr, p);
    p->len = 5;
    p->data[0] = CMD_SET_COLOR;
    p->data[1] = r;
    p->data[2] = g;
    p->data[3] = b;
    p->data[4] = 'd';


}
void fadePacket(struct packet_t * p, uint8_t adr, uint8_t r, uint8_t g, uint8_t b, uint16_t speed)
{
    preparePacket(adr, p);
    p->len = 7;
    p->data[0] = CMD_FADE;
    p->data[1] = r;
    p->data[2] = g;
    p->data[3] = b;
    p->data[4] = speed >> 8;
    p->data[5] = speed & 0xff;
    p->data[6] = 'd';

}

void selfassignPacket(struct packet_t * p)
{
    preparePacket(0,p);
    p->len = 1;
    p->data[0] = CMD_STANDALONE;
}

void statePacket(struct packet_t * p)
{
    preparePacket(0,p);
    p->len = 2;
    p->data[0] = CMD_SET_STATE;
    p->data[1] = STATE_REMOTE;
}

uint8_t sendPacket(struct packet_t * p)
{
    if(zbus_ready()){
        memcpy((char*)zbus_txbuf, (char*) p, p->len +PACKET_HEADERLEN);
        zbus_txstart(p->len +PACKET_HEADERLEN);
        return 1;
    }
    return 0;
}

uint8_t fromDigit(uint8_t d)
{
    if(d >= '0' && d <= '9')
        return d-'0';
    return d-'A'+10;
}
uint8_t fromHex(uint8_t * str)
{
    if(debug > 2){
        uart1_puts("From hex:");uart1_putc(str[0]);uart1_putc(str[1]);uart1_putc(str[2]);uart1_puts("\r\n");
    }
    if(str[0] == '#')
        str++;
    return fromDigit(str[0])* 16 + fromDigit(str[1]);
}

void cmd(uint8_t * cmd)
{
    uint8_t adr,r,g,b;
    uint16_t speed;
    if(debug){
        uart1_puts("cmd=");uart1_putc(cmd[0]);uart1_puts("\r\n");
    }
    switch(cmd[0]){
        case DREIC_COLOR:
            if(debug > 1)
            uart1_puts("Got set color\r\n");
            adr = fromHex(cmd+1);
            r = fromHex(cmd+4);
            g = fromHex(cmd+7);
            b = fromHex(cmd+9);
            colorPacket(&outpacket,adr,r,g,b);
            sendpacket = 1;
        break;
        case DREIC_FADE:
            adr = fromHex(cmd+1);
            r = fromHex(cmd+4);
            g = fromHex(cmd+7);
            b = fromHex(cmd+9);
            speed = (fromHex(cmd+11)<<8) + fromHex(cmd+13);
            fadePacket(&outpacket,adr,r,g,b,speed);
            sendpacket = 1;
        break;
        case 'D':
            debug = cmd[1]-'0';
        break;
    }
   
}

int main(void)
{
    uint16_t ms500 = PRE_500MS;   //360000;
    uint16_t ms1    = PRE_1MS;      //36
    uint8_t maintenace = 0;
    uint16_t len = 0;
    uint8_t block = 0;
    uint8_t sendmaintenace = 0;
    uint8_t gotcmd = 0;
    uart1_init( UART_BAUD_SELECT(UART_BAUDRATE,F_CPU));
#ifdef USART_USE_0
    uart_init( UART_BAUD_SELECT(UART_BAUDRATE,F_CPU));
#endif

#ifndef USART_USE_0
    zbus_core_init();
#else
    PORTC |= (1<<PC4);
#endif

    TCCR0B = 1<<CS02;           //divide by 256
    TIMSK0 |= 1<<TOIE0;         //enable timer interrupt

    sei();
#ifdef USART_USE_0
    uart_puts("123456789");
    while(1){
        uint16_t i = uart1_getc();
        if(!(i & UART_NO_DATA)){    
            uart_putc(i&0xFF);
            if(i == 'b')
                uart1_putc('D');
        }
    }
#endif

    uart1_putc('D');

    if(debug)
    uart1_puts("reset\r\n");
   
    while(1){
        if(serial_readline()){
            if(debug > 1)
            uart1_puts("readline\r\n");
            gotcmd = 1;
//            ms500 = PRE_500MS;
        }

        if(gotcmd && !sendpacket && !sendmaintenace){
           if(debug > 1)
           uart1_puts("cmd\r\n");
           gotcmd = 0;
           cmd(serial_buffer);
        }

        len = zbus_rxfinish();
        if(len){
            if(debug)
            uart1_puts("Eingang auf zbus\r\n");
            zbus_rxdone();
            zbus_rxstart();
        }

        if(timebase){
            timebase = 0;
            if(--ms1 == 0){
                ms1 = PRE_1MS;
#ifndef USART_USE_0
//                zbus_core_periodic();
#endif
                if(block)
                    block--;
            }

            if(sendpacket &&  !block){
                if(debug > 1)
                uart1_puts("send\r\n");
                if(sendPacket(&outpacket)){
                    if(debug > 1)
                    uart1_puts("sent\r\n");
                    sendpacket = 0;
                    block = 4;
                    uart1_puts(DREIC_DONE);
                }
            }
            if(sendmaintenace && !block){
                if(sendPacket(&outpacket)){
                    block = 4;
                    sendmaintenace = 0;
                }else if(debug > 2){
                    //uart1_puts("maintenace packet failed\r\n");
                }
            }
            if(--ms500 == 0){
                ms500 = PRE_500MS;
                if(debug > 1)
                uart1_puts("mark\r\n");
#ifdef USART_USE_0
                uart_puts("\\0Hello\\1");
#endif
                if(maintenace == 0 && sendpacket == 0){
                    maintenace = 1;
                    selfassignPacket(&outpacket);
#ifndef USART_USE_0
                    sendmaintenace = 1;
#endif
                }else if(maintenace ==1 && sendpacket == 0){
                    maintenace = 0;
                    statePacket(&outpacket);
#ifndef USART_USE_0
                    sendmaintenace = 1;
#endif
                }
            }
        }
    }

    return 0;
}
