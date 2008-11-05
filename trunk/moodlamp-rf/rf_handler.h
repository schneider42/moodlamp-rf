#ifndef __RF_HANDLER_H__
#define __RF_HANDLER_H__
#include "packet.h"

/*uint8_t rf_packetOut(struct packet * p);
uint8_t rf_packetIn(struct packet * p);
uint8_t rf_nextHeader(struct packet * p);
uint8_t rf_ready(void);*/
void rf_init(void);
void rf_tick(void);
//void rf_sendUUID(void);
#endif
