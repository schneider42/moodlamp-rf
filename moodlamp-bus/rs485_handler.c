#include <stdint.h>
#include <avr/io.h>
#include <string.h>
#include "config.h"
#include "fnordlicht.h"
#include "control.h"
#include "packet.h"
#include "zbusneu.h"
uint8_t lasttxsrc = 0;
uint8_t rs485_doreply = 0;

void rs485_init(void)
{
    zbus_core_init();
}

uint8_t rs485_packetOut(struct packet_t * p)
{
    return 0;
}

uint8_t rs485_packetIn(struct packet_t * p)
{
    return zbus_rxfinish((uint8_t *)p);
}

void rs485_tick(void)
{
}

void rs485_process(void)
{
}
