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

#define UART_BAUDRATE   230400


volatile uint8_t timebase = 0;
struct packet_t outpacket;
uint8_t sendpacket = 0;
uint8_t senddone = 0;
ISR(TIMER0_OVF_vect)
{
    static uint16_t tick = 0;
    tick++;
    if(tick > 2){
        tick = 0;
        timebase =1;
    }
}

void preparePacket(uint8_t adr, struct packet_t * p)
{
    p->src = 1;
    p->dest = adr;
    p->lasthop = 1;
    p->nexthop = adr;
    p->flags = (1<<PACKET_BROADCAST);
    p->iface = 1;
    p->len = 0;
}

void colorPacket(struct packet_t * p, uint8_t adr, uint8_t r, uint8_t g, uint8_t b)
{
    preparePacket(adr, p);
    p->len = 4;
    p->data[0] = 'C';
    p->data[1] = r;
    p->data[2] = g;
    p->data[3] = b;

}
void fadePacket(struct packet_t * p, uint8_t adr, uint8_t r, uint8_t g, uint8_t b, uint16_t speed)
{
    preparePacket(adr, p);
    p->len = 6;
    p->data[0] = 'F';
    p->data[1] = r;
    p->data[2] = g;
    p->data[3] = b;
    p->data[4] = speed >> 8;
    p->data[5] = speed & 0xff;
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

uint8_t fromHex(uint8_t * str)
{
    if(str[0] == '#')
        str++;
    return (str[0]-'A')* 16 + (str[1] - 'A');
}

void cmd(uint8_t * cmd)
{
    uint8_t adr,r,g,b,speed;
    switch(cmd[0]){
        case 'C':
            adr = fromHex(cmd+1);
            r = fromHex(cmd+4);
            g = fromHex(cmd+7);
            b = fromHex(cmd+10);
            colorPacket(&outpacket,adr,r,g,b);
            sendpacket = 1;
            senddone = 1;
        break;
        case 'F':
            adr = fromHex(cmd+1);
            r = fromHex(cmd+4);
            g = fromHex(cmd+7);
            b = fromHex(cmd+10);
            speed = (fromHex(cmd+13)<<8) + fromHex(cmd+15);
            fadePacket(&outpacket,adr,r,g,b,speed);
            sendpacket = 1;
            senddone = 1;
        break;
    }
   
}

int main(void)
{
    uint16_t ms1000 = 32000;
    uint16_t ms1    = 32;
    uint8_t maintenace = 0;
    uint16_t len = 0;
    uart1_init( UART_BAUD_SELECT(UART_BAUDRATE,F_CPU));
    zbus_core_init();

    TCCR0B = 1<<CS02;           //divide by 256
    TIMSK0 |= 1<<TOIE0;         //enable timer interrupt

   
    while(1){
        if(readline()){
            cmd(serial_buffer);
        }
        len = zbus_rxfinish();
        if(len){
            zbus_rxdone();
            zbus_rxstart();
        }
        if(timebase){
            timebase = 0;
            if(--ms1 == 0){
                ms1 = 32;
                zbus_core_periodic();
            }
            if(sendpacket){
                if(sendPacket(&outpacket)){
                    sendpacket = 0;
                    if(senddone){
                        senddone = 0;
                        uart1_puts("D");
                    }
                }
            }
            if(--ms1000 == 0){
                ms1000 = 32000;
                if(maintenace == 0 && sendpacket == 0){
                    maintenace = 1;
                    selfassignPacket(&outpacket);
                    sendpacket = 1;
                    senddone = 0;
                }else if(maintenace ==1 && sendpacket == 0){
                    maintenace = 0;
                    statePacket(&outpacket);
                    sendpacket = 1;
                    senddone = 0;
                }
            }
        }
    }

    return 0;
}
