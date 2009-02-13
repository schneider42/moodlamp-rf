#include "cache.h"
#include "packet.h"
#include "stdint.h"
#include "interfaces.h"

struct cache_entry {
    uint8_t iface;
    uint8_t nexthop;
    uint8_t metric;
    uint8_t timeout;
};

//uint8_t table[256][4];      //[host] = (interface, nexthop, metric, timeout)

struct cache_entry cache[256];

#define NO_METRIC       255
void cache_init(void)
{
    uint16_t i;
    for(i=0;i<256;i++){
        cache[i].iface = IFACE_NONE;
        cache[i].nexthop = 0;
        cache[i].metric = NO_METRIC;
        cache[i].timeout = 0;
    }
}

uint8_t cache_getSrcIface(struct packet_t * p)
{
    return cache[p->lasthop].iface;
}

uint8_t cache_getDestIface(struct packet_t *p)
{
    return cache[p->nexthop].iface;
}

void cache_input(struct packet_t *p, uint8_t interface)
{
    uint8_t s = p->src;
    uint8_t metric = interfaces_getMetric(interface);
    
    if(s != packet_getAddress() && s != 0 && metric <= cache[s].metric){
        cache[s].iface = interface;
        cache[s].nexthop = p->lasthop;
        cache[s].metric = metric;
        cache[s].timeout = 60;
    }

    if(p->lasthop != packet_getAddress() && p->lasthop != 0 && metric <= cache[p->lasthop].metric){
        cache[p->lasthop].iface = interface;
        cache[p->lasthop].nexthop = p->lasthop;
        cache[p->lasthop].metric = metric;
        cache[p->lasthop].timeout = 60;
    }
}

uint8_t cache_getNextHop(struct packet_t * p)
{
    return cache[p->dest].nexthop;
}

void cache_set(uint8_t adr, uint8_t iface)
{
    cache[adr].iface = iface;
    cache[adr].nexthop = adr;
}

void cache_tick(void)
{
    static uint16_t t = 1000;
    uint8_t i;
    if(!t--){
        t = 1000;
        for(i=0;i<254;i++){
            if( !(cache[i].timeout--)){
                cache[i].metric = NO_METRIC;
            }
        }
    }
}
