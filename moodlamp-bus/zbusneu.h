#ifndef _ZBUSNEU_H
#define _ZBUSNEU_H

#include <stdint.h>
#include "config.h"

#if RS485_CTRL

#define ZBUS_BUFFER_LEN    70

typedef uint16_t zbus_index_t;

enum ZBusEscapes {
  ZBUS_START = '0',
  ZBUS_STOP = '1',
};

void zbus_core_init(void);
void zbus_core_periodic(void);

uint8_t zbus_ready(void);
void zbus_txstart(zbus_index_t len);

void zbus_rxstart (void);

zbus_index_t zbus_rxfinish(uint8_t *buf); 
void zbus_process(void);

void zbus_rxdone(void);

extern volatile uint8_t zbus_txbuf[ZBUS_BUFFER_LEN];
extern volatile uint8_t zbus_buf[ZBUS_BUFFER_LEN];
extern volatile uint8_t zbus_done;
#endif /* ZBUS_SUPPORT */

#endif /* _ZBUS_H */
