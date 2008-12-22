#include <avr/io.h>
#include <string.h>
#include "config.h"
#include "common.h"
#include "fnordlicht.h"
#include "cmd_handler.h"
#include "lib/rf12.h"
#include "lib/rf12packet.h"
#include "pwm.h"
#include "static_scripts.h"
#include "settings.h"
#include "rf_handler.h"
#include "control.h"
#include "lib/uart.h"
#include "rf12config.h"
#include "avr/interrupt.h"
#include "packet.h"

void rf_init(void)
{
    volatile unsigned long l;

    for(l=0;l<10000;l++);
/*#ifdef RF12DEBUGBIN
    sei();
    printf("acDinit rfab");
#endif*/
    if(!(PINC & (1<<PC3)))
        return;

    rf12_init();
    rf12_setfreq(RF12FREQ(434.32));
    rf12_setbandwidth(4, 1, 4);     // 200kHz Bandbreite, -6dB Verstärkung, DRSSI threshold: -79dBm
    rf12_setbaud(19200);
    rf12_setpower(0, 6);            // 1mW Ausgangangsleistung, 120kHz Frequenzshift
    rf12packet_init(0);
    //volatile uint32_t tmp;
    //for(tmp=0;tmp<100000;tmp++);
/*#ifdef RF12DEBUGBIN
    printf("acDInit doneab");
#endif*/

}

