#ifndef __SERIAL_HANDLER_
#define __SERIAL_HANDLER_

#include <stdint.h>
#include <packet.h>
#include "config.h"

#define SERIAL_BUFFERLEN    270
#define SERIAL_ESCAPE       'a'
#define SERIAL_START        'c'
#define SERIAL_END          'b'

#define SERIAL_INIT         'I'
#define SERIAL_RAWDATA      'r'

enum SerialStates {
    SERIAL_STATE_INIT,
    SERIAL_STATE_NORMAL
};

void serial_putenc(uint8_t * d, uint16_t n);
uint8_t serial_packetOut(struct packet_t * p);
uint8_t serial_packetIn(struct packet_t * p);
uint8_t serial_nextHeader(struct packet_t * p);
uint8_t serial_ready(void);
void serial_tick(void);
unsigned int readline( void );
void serial_setadr(uint8_t remote, uint8_t adr, uint8_t broadcast);
void serial_process(void);
uint16_t serial_rawlen(void);
uint8_t * serial_rawgetbuffer(void);
void serial_rawdone(void);

#endif
