#ifndef __SERIAL_HANDLER_
#define __SERIAL_HANDLER_

#include <stdint.h>
#include <packet.h>

uint8_t serial_packetOut(struct packet_t * p);
uint8_t serial_packetIn(struct packet_t * p);
uint8_t serial_nextHeader(struct packet_t * p);
uint8_t serial_ready(void);
void serial_tick(void);
unsigned char readline( void );
void serial_setadr(uint8_t remote, uint8_t adr, uint8_t broadcast);
void serial_process(void);

#endif
