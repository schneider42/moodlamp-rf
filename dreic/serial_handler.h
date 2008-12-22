#ifndef __SERIAL_HANDLER_
#define __SERIAL_HANDLER_

#include <stdint.h>
void serial_putenc(uint8_t * d, uint16_t n);
uint16_t serial_readline(void);
unsigned int readline(void);

extern uint8_t serial_buffer[270];

#endif
