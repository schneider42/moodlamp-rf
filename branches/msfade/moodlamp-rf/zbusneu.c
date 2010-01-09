#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "bit-macros.h"
#include "config.h"
#include "zbusneu.h"
#include "pinutils.h"

#ifndef ZBUS_USE_USART
#define ZBUS_USE_USART 0 
#endif
#define USE_USART ZBUS_USE_USART 
#define BAUD ZBUS_BAUDRATE
#include "esusart.h"

/*
#include <util/setbaud.h>
*/
#if RS485_CTRL
/* We generate our own usart init module, for our usart port */
generate_usart_init()

static volatile zbus_index_t zbus_index;
static volatile zbus_index_t zbus_txlen;
static volatile zbus_index_t zbus_rxlen;

volatile uint8_t zbus_txbuf[ZBUS_BUFFER_LEN];
volatile uint8_t zbus_buf[ZBUS_BUFFER_LEN];
volatile uint8_t zbus_done = 0;
volatile uint8_t zbus_inrx = 0;
volatile uint8_t zbus_rxfinished = 0;
volatile uint8_t zbus_txfinished = 0;
volatile uint8_t zbus_intx = 0;

uint8_t zbus_ready(void)
{
//    if(zbus_inrx)
//        return 0;
    if(zbus_intx)
        return 0;
    return 1;
}

void
zbus_txstart(zbus_index_t size)
{
    if(zbus_intx)
        return;
    zbus_txlen = size;
    if(zbus_inrx)
        return;
    zbus_intx = 1;

    zbus_txlen = size;
    uint8_t sreg = SREG; cli();
    usart(UCSR,B) = _BV(usart(TXCIE)) | _BV(usart(TXEN));
    PIN_SET(RS485_TX_PIN);
    PIN_SET(RS485_NRX_PIN);
    usart(UCSR,A) |= _BV(usart(TXC));
    SREG = sreg;
    usart(UDR) = '\\';
}


void zbus_rxdone(void)
{    
    zbus_rxfinished = 0;
}

void
zbus_rxstart (void)
{
    if(zbus_rxfinished)
        return;
    if(zbus_inrx)
        return;
    if(zbus_intx)
        return;

    uint8_t sreg = SREG; cli();

    /* disable transmitter, enable receiver (and rx interrupt) */
    usart(UCSR,B) = _BV(usart(RXCIE)) | _BV(usart(RXEN));

    /* Default is reciever enabled*/
    PIN_CLEAR(RS485_TX_PIN);
    PIN_CLEAR(RS485_NRX_PIN);
    
    zbus_inrx = 0;
    zbus_rxlen = 0;
    zbus_rxfinished = 0;

    SREG = sreg;
}


static void
zbus_rxstop (void)
{
    uint8_t sreg = SREG; cli();
    /* completely disable usart */
    usart(UCSR,B) = 0;
    SREG = sreg;
}


zbus_index_t zbus_rxfinish(void) 
{
    if(zbus_rxlen != 0 && zbus_rxfinished){
        return zbus_rxlen;
    }
    return 0;
}

void
zbus_core_init(void)
{
    /* Initialize the usart module */
    usart_init();

    /* Enable RX/TX Swtich as Output */
    DDR_CONFIG_OUT(RS485_TX_PIN);
    DDR_CONFIG_OUT(RS485_NRX_PIN);
    
    /* clear the buffers */
    zbus_txlen = 0;
    zbus_rxlen = 0;
    zbus_index = 0;

    zbus_rxstart ();
}

void
zbus_core_periodic(void)
{
    if(zbus_txlen && zbus_intx == 0 && zbus_inrx == 0)
        zbus_txstart(zbus_txlen);
    if(zbus_txlen == 0 && zbus_rxfinished == 0)
        zbus_rxstart();
}

SIGNAL(usart(USART,_TX_vect))
{
    static uint8_t escaped = 0;
    static uint16_t p = 0;
    if(zbus_intx == 1){
        usart(UDR) = ZBUS_START;
        zbus_intx++;
        p = 0;
    }else if(zbus_intx == 2){
        if(escaped){
            usart(UDR) = escaped;
            escaped = 0;
        }else if(zbus_txbuf[p] == '\\'){
            usart(UDR) = '\\';
            escaped = '\\';
            return;
        }else{
            usart(UDR) = zbus_txbuf[p];
        }
        if(++p == zbus_txlen){
            zbus_intx++;
        }
    }else if(zbus_intx == 3){
        usart(UDR) = '\\';
        zbus_intx++;
    }else if(zbus_intx == 4){
        usart(UDR) = ZBUS_STOP;
        zbus_intx++;
    }else if(zbus_intx == 5){
        zbus_intx = 0;
        zbus_txfinished = 1;
        zbus_txlen = 0;
        zbus_done = 1;
        zbus_rxstart();
    }
}

SIGNAL(usart(USART,_RX_vect))
{
  static uint8_t escaped = 0;
  static uint8_t gotstart = 0;
//  PIN_SET(ZBUS_RX_PIN);
  /* Ignore errors */
  if ((usart(UCSR,A) & _BV(usart(DOR))) || (usart(UCSR,A) & _BV(usart(FE)))) {
    uint8_t v = usart(UDR);
    (void) v;
    return; 
  }
  uint8_t data = usart(UDR);
  zbus_inrx = 1;
  if(data == '\\' && escaped == 0){
    escaped = 1;
    return;
  }else if(escaped){
    escaped = 0;
    if(data == ZBUS_START){
        gotstart = 1;
        zbus_rxlen = 0;
        return;
    }else if(data == ZBUS_STOP){
        zbus_inrx = 0;
        if(zbus_rxlen)
            zbus_rxfinished = 1;
        zbus_rxstop();
        return;
    }
  }
  zbus_buf[zbus_rxlen++] = data;

}


#endif
