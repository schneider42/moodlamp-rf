#ifndef __INTERFACES_H
#define __INTERFACES_H
#include <stdint.h>

#include "packet.h"
//#include "rf_handler.h"
#include "lib/rf12packet.h"
#include "serial_handler.h"
#include "rs485_handler.h"

#define IFACE_SERIAL        1
#define IFACE_RS485         2
#define IFACE_RF            3

//#define IFACE_NUM           1           //3
#define IFACE_LOCAL         0xFF        //local pseudo iface
#define IFACE_NONE          0xFE

struct interface{
    uint8_t     id;
    uint8_t(*packetOut)(struct packet_t * p);
    uint8_t(*packetIn)(struct packet_t * p);
    uint8_t(*nextHeader)(struct packet_t * p);
    uint8_t(*isReady)(void);
    void(*tick)(void);
    uint8_t dobroadcast;
    uint8_t getbroadcast;
    uint8_t metric;
    uint8_t enabled;
};

void interfaces_setEnabled(uint8_t iface, uint8_t enabled);
uint8_t interfaces_isEnabled(uint8_t iface);
uint8_t interfaces_getMetric(uint8_t iface);
uint8_t interfaces_packetOut(uint8_t id, struct packet_t * p);
void interfaces_tick(void);
uint8_t interfaces_gotPacket(struct packet_t * p);
uint8_t interfaces_getPacket(uint8_t iface, struct packet_t * p);
uint8_t interfaces_isReady(uint8_t iface);
uint8_t interfaces_broadcast(uint8_t originid, struct packet_t * p, uint8_t force);

extern struct interface ifaces[];
#endif
