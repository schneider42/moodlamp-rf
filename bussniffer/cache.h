#ifndef __CACHE_H_
#define __CACHE_H_

#include "packet.h"
void cache_init(void);
uint8_t cache_getSrcIface(struct packet_t * p);
uint8_t cache_getDestIface(struct packet_t *p);
void cache_input(struct packet_t *p, uint8_t interface);
void cache_set(uint8_t adr, uint8_t iface);
uint8_t cache_getNextHop(struct packet_t * p);
void cache_tick(void);
#endif
