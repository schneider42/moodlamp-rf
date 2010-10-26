#include "packet.h"
#include "interfaces.h"
#include "cmd_handler.h"
#include "fnordlicht.h"
#include "config.h"
#include "stdint.h"
#include "cache.h"
#include "stdlib.h"

uint8_t locladr = 0;
uint8_t broadcastadr = 0;
struct packet_t p;

uint8_t packet_getAddress(void)
{
    return locladr;
}

uint8_t packet_getBroadcast(void)
{
    return broadcastadr;
}

uint8_t packet_isOwnAddress(struct packet_t * p)
{
    return (p->nexthop == locladr) ? 1 : 0;
}

uint8_t packet_isOwnBroadcast(struct packet_t * p)
{
    return (p->dest == broadcastadr) ? 1 : 0;
}

void packet_init(uint8_t address, uint8_t broadcast)
{
    locladr = address;
    broadcastadr = broadcast;
    cache_init();
    cache_set(locladr, IFACE_LOCAL);
}

void packet_setAddress(uint8_t address, uint8_t broadcast)
{
    locladr = address;
    broadcastadr = broadcast;
    cache_set(locladr, IFACE_LOCAL);
}

void packet_packetIn(struct packet_t * p, uint8_t interface)
{
    uint8_t r;
    //uart1_puts("acDkab");
    cache_input(p,interface);

    if(p->flags & PACKET_BROADCAST){
        packet_packetOut(p);
        if(p->dest == broadcastadr || p->dest == 0)
            cmd_interpret(p->data,NULL);
        if( p->flags & PACKET_REPLYDONE ){
            if(p->flags & PACKET_REPLYDONE){
                //p->flags ^= PACKET_REPLYDONE;
                p->flags = PACKET_DONE;
            }
            p->len = 0;
            p->dest = p->src;
            p->src = locladr;
            packet_packetOut(p);
        }
        return;

    }

    if(p->dest == locladr){
        /*uart1_puts("acDi");
        uint8_t tmp;
        for(tmp=0;tmp<p->len;tmp++)
            uart1_putc(p->data[tmp]);
        uart1_puts("ab");*/

        r = cmd_interpret(p->data,p->data);
        if(r || (p->flags & PACKET_REPLYDONE)){
            if(p->flags & PACKET_REPLYDONE){
                //p->flags ^= PACKET_REPLYDONE;
                p->flags = PACKET_DONE;
            }
            p->len = r;
            p->dest = p->src;
            p->lasthop = locladr;
            p->src = locladr;
            packet_packetOut(p);
        }
    }else if(p->nexthop == locladr){
        packet_packetOut(p);
    }
}

void packet_tick(void)
{
    uint8_t src;
    interfaces_tick();
    cache_tick();
    if((src = interfaces_gotPacket(&p)) != IFACE_NONE){
        //uart1_puts("acDGab");
        uint8_t iface = cache_getDestIface(&p);
        if(interfaces_isReady(iface)){
            //uart1_puts("acDgab");
            interfaces_getPacket(src,&p);
            packet_packetIn(&p,src);
        }
    }
                                                                            
    //for interfaces do getpacket(&struct packt *)
    //sollte vom iface dierekt ueber getpacket geloest weren //for interfaces do isdone('');

}

void packet_packetOut(struct packet_t * p)
{
    if(global.flags.rawmode)
        return;

    if( p->flags & PACKET_BROADCAST){
    uint8_t iface;
        if(p->iface != IFACE_LOCAL)
             iface = cache_getSrcIface(p);
        else
            iface = IFACE_LOCAL;
        //if( iface == 1)
        //    return;
        p->lasthop = locladr;
        p->nexthop = p->dest;
        /*uart1_puts("acDB");
        uart1_putc(iface);
        serial_putenc(p,p->len+PACKET_HEADERLEN);
        uart1_puts("ab");*/

        if(p->iface == IFACE_LOCAL/*p->src == locladr && p->src != 0*/)  //hm
            interfaces_broadcast(iface,p,1);            //force
        else
            interfaces_broadcast(iface,p,0);
        //uart1_puts("acDEab");
        return;
    }

    p->lasthop = locladr;
    p->nexthop = cache_getNextHop(p);
    uint8_t iface = cache_getDestIface(p);
    //printf("acDP%c%cab",p->dest,p->nexthop);
    /*uart1_puts("acDP");
    uart1_putc(iface);
    serial_putenc(p,p->len+PACKET_HEADERLEN);
    uart1_puts("ab");*/
    interfaces_packetOut(iface,p);
}



