#include <stdint.h>
#include <avr/io.h>
#include "config.h"
#include "fnordlicht.h"
#include "lib/uart.h"
#include "serial_handler.h"
#include "interfaces.h"
#include "cache.h"
#include <string.h>
#include "control.h"

void rs485_init(void)
{

}

uint8_t rs485_packetOut(struct packet_t * p)
{
    return 0;
}

uint8_t rs485_packetIn(struct packet_t * p)
{
    return 0;
}

uint8_t rs485_nextHeader(struct packet_t * p)
{
    return 1;
}

uint8_t rs485_ready(void)
{
    return 1;
}


void rs485_tick(void)
{
}

void rs485_process(void)
{
}
