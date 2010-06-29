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

volatile uint8_t abus_buf0[ZBUS_BUFFER_LEN];
volatile uint8_t abus_buf1[ZBUS_BUFFER_LEN];
volatile uint8_t abus_len0 = 0;
volatile uint8_t abus_len1 = 0;
volatile uint8_t abus_full0 = 0;
volatile uint8_t abus_full1 = 0;

zbus_index_t zbus_rxfinish(uint8_t *buf) 
{
    if( abus_full0 ){
        memcpy(buf,abus_buf0,abus_len0);
        abus_full0 = 0;
        return 1;
    }
    if( abus_full1 ){
        memcpy(buf,abus_buf1,abus_len1);
        abus_full1 = 0;
        return 1;
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
    usart(UCSR,B) = _BV(usart(RXCIE)) | _BV(usart(RXEN));
    /* Default is reciever enabled*/
    PIN_CLEAR(RS485_TX_PIN);
    PIN_CLEAR(RS485_NRX_PIN);
}


SIGNAL(usart(USART,_RX_vect))
{
  static uint8_t escaped = 0;
  static uint8_t gotstart = 0;
  static uint8_t cur = 0;
  static uint8_t len = 0;
  static uint8_t *buf = 0;

  /* Ignore errors */
  if ((usart(UCSR,A) & _BV(usart(DOR))) || (usart(UCSR,A) & _BV(usart(FE)))) {
    uint8_t v = usart(UDR);
    (void) v;
    return; 
  }

  uint8_t data = usart(UDR);

  if(data == '\\' && escaped == 0){
    escaped = 1;
    return;
  }else if(escaped){
    escaped = 0;
    if(data == ZBUS_START){
        gotstart = 1;
        if( abus_full0 == 0 ){
            buf = abus_buf0;
            len = 0;
            cur = 0;
        }else if( abus_full1 == 0 ){
            buf = abus_buf1;
            len = 0;
            cur = 1;
        }else{
            gotstart = 0;
        }
        return;
    }else if(data == ZBUS_STOP){
        gotstart = 0;
        if( len ){
            if( cur == 0 ){
                abus_full0 = 1;
                abus_len0 = len;
            }else{
                abus_full1 = 1;
                abus_len1 = len;
            }
        }
        return;
    }
  }

  if( gotstart ){
      buf[len++] = data;
  }
}


#endif
