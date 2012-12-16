#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include <string.h>

#include "uart.h"
#include "leds.h"
#include "pinutils.h"
#include "config.h"

#define RS485_BITRATE   115200

uint8_t lampids[] = {0x22, 0x39, 0x2a};

void busputc(uint8_t c)
{
    UDR0 = c;
    leds_tx();
    while( !(UCSR0A & (1<<TXC0)) );
    UCSR0A |= (1<<TXC0);
    leds_txend();
}

void busputcenc(uint8_t c)
{
    busputc(c);
    if( c == '\\' )
        busputc(c);
}

uint8_t getInputs(void)
{
    //return 3;
    return PINC & 0x07;
}

void setLamp(uint8_t lamp, uint8_t r, uint8_t g, uint8_t b)
{
    busputc('\\');
    busputc('0');
    busputcenc(lamp);
    busputc('C');
    busputcenc(r);
    busputcenc(g);
    busputcenc(b);
    busputc('\\');
    busputc('1');
    _delay_us(1000);
}

void setLamps(uint8_t lamps)
{
    uint8_t i;
    for(i=0; i<3; i++){
        if( lamps & (1<<i) )
            setLamp(lampids[i],0,255,0);
        else
            setLamp(lampids[i],255,0,0);
     }
}

int main(void)
{
    leds_init();

    DDR_CONFIG_OUT(RS485_TX_PIN);
    DDR_CONFIG_OUT(RS485_NRX_PIN);

    PIN_SET(RS485_TX_PIN);
    PIN_SET(RS485_NRX_PIN);

    UBRR0 = UART_BAUD_SELECT(RS485_BITRATE,F_CPU);
    UCSR0B |= (1<<TXEN0) | (1<<RXEN0);
    volatile long l;
    while(0){
        setLamp(0x39,0,255,0);
        for(l=0; l<1000000; l++);
        setLamp(0x39,255,255,0);
        for(l=0; l<1000000; l++);
    }
    uint8_t inputs = getInputs();
    while(1){
        wdt_reset();
        uint8_t newInputs = getInputs();
        if( newInputs != inputs ){
            setLamps(newInputs);
        }
        inputs = newInputs;
    }
}




