#include <stdint.h>
#include <avr/io.h>
#include <string.h>
#include "config.h"
#include "fnordlicht.h"
#include "interfaces.h"
#include "cache.h"
#include "control.h"
#include "zbusneu.h"

uint8_t lasttxsrc = 0;
uint8_t rs485_doreply = 0;

void rs485_init(void)
{
    zbus_core_init();
}

uint8_t rs485_packetOut(struct packet_t * p)
{
    if(p->flags & PACKET_REPLYDONE){
        p->flags ^= PACKET_REPLYDONE;
        rs485_doreply = 1;
        lasttxsrc = p->src;
    }
    memcpy((char*)zbus_txbuf,p,p->len + PACKET_HEADERLEN);
    zbus_txstart(p->len + PACKET_HEADERLEN);
    return 0;
}

uint8_t rs485_packetIn(struct packet_t * p)
{
    if(zbus_done && rs485_doreply){
        rs485_nextHeader(p);
        rs485_doreply = 0;
        zbus_done = 0;
        return 0;
    }
    if(zbus_rxfinish() == 0)
        return 1;
    memcpy(p,(char*)zbus_buf,zbus_rxfinish());
    zbus_rxdone();
    zbus_rxstart();
    return 0;
}

uint8_t rs485_nextHeader(struct packet_t * p)
{
    if(zbus_done && rs485_doreply){
        p->len = 0;
        p->dest = lasttxsrc;
        p->src = packet_getAddress();
        p->lasthop =  packet_getAddress();
        p->nexthop = packet_getAddress();
        p->flags = PACKET_DONE;
//        p->iface = IFACE_NONE;
//        zbus_done = 0;
        //uart1_puts("acDRDab");
        return 1;
    }
    if(zbus_rxfinish() < PACKET_HEADERLEN)
        return 0;
    memcpy(p,(char*)zbus_buf,PACKET_HEADERLEN);
    return 1;
}

uint8_t rs485_ready(void)
{
    return zbus_ready();
}

void rs485_tick(void)
{
    zbus_core_periodic();
}

void rs485_process(void)
{
}
