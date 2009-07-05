#include <avr/io.h>
#include "leds.h"
#include "config.h"
#include "pinutils.h"

void leds_init(void)
{
    DDR_CONFIG_OUT(LED1); 
    DDR_CONFIG_OUT(LED2); 
    DDR_CONFIG_OUT(LED3); 
}

inline void leds_rx(void)
{
    PIN_SET(LED2);
}


inline void leds_rxend(void)
{
    PIN_CLEAR(LED2);
}

inline void leds_adc(void)
{
//    PIN_SET(LED1);
}


inline void leds_adcend(void)
{
//    PIN_CLEAR(LED1);
}

inline void leds_tx(void)
{
    PIN_SET(LED3);
}


inline void leds_txend(void)
{
    PIN_CLEAR(LED3);
}
