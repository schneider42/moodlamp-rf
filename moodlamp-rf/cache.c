#include "cache.h"
#include "packet.h"
#include "stdint.h"
#include "interfaces.h"

uint8_t table[256][2];      //2bit interface desc. per address

void cache_init(void)
{
    uint8_t i;
    for(i=0;i<64;i++){
        table[i][0] = IFACE_NONE;
        table[i][1] = 0;
    }
    //table[2] = IFACE_SERIAL;
    //table[1] = IFACE_LOCAL;

}

uint8_t cache_getSrcIface(struct packet_t * p)
{
    return table[p->lasthop][0];
}

uint8_t cache_getDestIface(struct packet_t *p)
{
    return table[p->nexthop][0];
}

void cache_input(struct packet_t *p, uint8_t interface)
{
    uint8_t s = p->src;
//    table[s/4] &= ~(0x3<<(2*(s%4)));
//    table[s/4] |= (interface<<(2*(s%4)));
//    uart1_puts("acDCab");
    if(s != packet_getAddress() && s != 0){
        table[s][0] = interface;
        table[s][1] = p->lasthop;
    }
    if(p->lasthop != packet_getAddress() && p->lasthop != 0){
        table[p->lasthop][0] = interface;
        table[p->lasthop][1] = p->lasthop;
    }
}

uint8_t cache_getNextHop(struct packet_t * p)
{
    return table[p->dest][1];
}

void cache_set(uint8_t adr, uint8_t iface)
{
    table[adr][0] = iface;
    table[adr][1] = adr;
}
