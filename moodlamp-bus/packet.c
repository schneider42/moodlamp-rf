#include "packet.h"
#include "cmd_handler.h"
#include "fnordlicht.h"
#include "config.h"
#include "stdint.h"
#include "stdlib.h"
//#include "lib/uart.h"

uint8_t localadr = 0;
uint8_t broadcastadr = 0;
struct packet_t p;

uint8_t packet_getAddress(void)
{
    return localadr;
}

uint8_t packet_getBroadcast(void)
{
    return broadcastadr;
}

#define ADR_FRONT   1
#define ADR_BACK    2
#define ADR_FRONT1  3
#define ADR_FRONT2  4
#define ADR_FRONT3  5
#define ADR_BACK1   6
#define ADR_BACK2   7
#define ADR_BACK3   8
#define ADR_FB1     9
#define ADR_FB2     10
#define ADR_FB3     11

#define CMASK       0xF0
#define SIDEMASK    0x08

#define FRONT       0x08
#define BACK        0x00

#define C1          0x10
#define C2          0x20
#define C3          0x30

uint8_t packet_isDreicBroadcast(uint8_t adr)
{
    if( adr == 0 )
        return 1;
    if( adr == localadr )
        return 1;
    if( adr == ADR_FRONT && (localadr & SIDEMASK) == FRONT )
        return 1;
    if( adr == ADR_BACK  && (localadr & SIDEMASK) == BACK )
        return 1;

    if( adr == ADR_FRONT1 && (localadr & SIDEMASK) == FRONT && (localadr & CMASK) == C1)
        return 1;
    if( adr == ADR_FRONT2 && (localadr & SIDEMASK) == FRONT && (localadr & CMASK) == C2)
        return 1;
    if( adr == ADR_FRONT3 && (localadr & SIDEMASK) == FRONT && (localadr & CMASK) == C3)
        return 1;

    if( adr == ADR_BACK1 && (localadr & SIDEMASK) == BACK && (localadr & CMASK) == C1)
        return 1;
    if( adr == ADR_BACK2 && (localadr & SIDEMASK) == BACK && (localadr & CMASK) == C2)
        return 1;
    if( adr == ADR_BACK3 && (localadr & SIDEMASK) == BACK && (localadr & CMASK) == C3)
        return 1;

    if( adr == ADR_FB1 && (localadr & CMASK) == C1 )
        return 1;
    if( adr == ADR_FB2 && (localadr & CMASK) == C2 )
        return 1;
    if( adr == ADR_FB3 && (localadr & CMASK) == C3 )
        return 1;
    return 0;
}

void packet_init(uint8_t address, uint8_t broadcast)
{
    localadr = address;
    broadcastadr = broadcast;
}

void packet_setAddress(uint8_t address, uint8_t broadcast)
{
    localadr = address;
    broadcastadr = broadcast;
}

void packet_tick(void)
{
    //zbus_core_periodic();
    if( rs485_packetIn(&p) )
        if( packet_isDreicBroadcast(p.dest) )
            cmd_interpret(p.data,NULL);
}

