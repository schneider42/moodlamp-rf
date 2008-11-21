#ifndef __RS485_HANDLER_
#define __RS485_HANDLER_

#include <stdint.h>
#include <packet.h>

void rs485_init(void);
uint8_t rs485_packetOut(struct packet_t * p);
uint8_t rs485_packetIn(struct packet_t * p);
uint8_t rs485_nextHeader(struct packet_t * p);
uint8_t rs485_ready(void);
void rs485_tick(void);
void rs485_process(void);

#endif
