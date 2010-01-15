#include "raw.h"
#include "stdint.h"
#include <avr/io.h>
#include "fnordlicht.h"
#include "config.h"
#include "zbus.h"
#include "serial_handler.h"
#include "string.h"
#include "lib/uart.h"

void raw_tick(void)
{
//    uart1_puts("acDRawtickab");
    zbus_core_periodic();
    uint16_t len = serial_rawlen();
    if(len){
        if(zbus_ready()){
            memcpy((char*)zbus_txbuf, serial_rawgetbuffer(), len);
            zbus_txstart(len);
            serial_rawdone();
        }
    }
    len = zbus_rxfinish();
    if(len){
        uart1_puts("acR");
        serial_putenc((unsigned char *)zbus_buf, len);
        uart1_puts("ab");
        zbus_rxdone();
        zbus_rxstart();
    }

}
