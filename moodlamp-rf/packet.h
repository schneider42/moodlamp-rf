#ifndef __PACKET_H_
#define __PACKET_H_

#include "stdint.h"


#define PACKET_BROADCAST            1
#define PACKET_REPLYDONE            2       //request for done after processing
#define PACKET_DONE                 4       //done flag
#define PACKET_TIMEOUT              8

#define PACKET_MAXDATA              150
struct packet_t {
    uint8_t src;
    uint8_t dest;
    uint8_t lasthop;
    uint8_t nexthop;
    uint8_t flags;
    uint8_t iface;
    uint8_t len;
    uint8_t data[PACKET_MAXDATA];
};

#define PACKET_HEADERLEN            (sizeof(struct packet_t)-PACKET_MAXDATA)


extern struct packet_t p;
//extern uint8_t localadr;
extern uint8_t broadcastadr;
//extern uint8_t serveradr;

uint8_t packet_getAddress(void);
uint8_t packet_getBroadcast(void);
uint8_t packet_isOwnAddress(struct packet_t * p);
uint8_t packet_isOwnBroadcast(struct packet_t * p);

void packet_setAddress(uint8_t address, uint8_t broadcast);
void packet_init(uint8_t address, uint8_t broadcast);
void packet_packetIn(struct packet_t * p, uint8_t interface);
void packet_tick(void);
void packet_packetOut(struct packet_t * p);

#endif
