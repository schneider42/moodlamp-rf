#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include <stdlib.h>

#include "global.h"
#include "lib/rf12.h"
#include "lib/rf12packet.h"
#include "lib/uart.h"

//Rx
static FILE mystdout = FDEV_SETUP_STREAM(uart_putc_file, NULL, _FDEV_SETUP_WRITE);

static char buffer[40];

unsigned char readline( void )
{
    static int fill = -1;
    static int escaped = 0;
    int  i = uart_getc();
    char data;
    if ( i & UART_NO_DATA ){
        return 0;
    }
    data = i&0xFF;

    if(data == 'a'){
        if(!escaped){
            escaped = 1;
            return 0;
        }
        escaped = 0;
    }else if(escaped){
        escaped = 0;
        if(data == 'c'){
            fill = 0;
            return 0;
        }else if( data == 'b'){
            return fill;
        }
    }
    if(fill != -1){
        buffer[fill++] = data;
        if(fill >= 40)
            fill = 39;
    }
    return 0;
}

int main(void)
{
    uart_init( UART_BAUD_SELECT(115200,F_CPU));
    //CLKPR = 0;
    sei();
//    uart_puts("hello world\n");
//    while(1);
    stdout = &mystdout;
    rf12_init();				// ein paar Register setzen (z.B. CLK auf 10MHz)
    printf("acDThis is 3ab");
//while(1);
    rf12_setfreq(RF12FREQ(434.32));		// Sende/Empfangsfrequenz auf 433,92MHz einstellen
    rf12_setbandwidth(4, 1, 4);		// 200kHz Bandbreite, -6dB Verstärkung, DRSSI threshold: -79dBm 
    rf12_setbaud(19200);			// 19200 baud
    rf12_setpower(0, 6);			// 1mW Ausgangangsleistung, 120kHz Frequenzshift
    rf12packet_init(2);
    printf("acDInit doneab");

    unsigned int c = 0;
    unsigned char raw = 1;
	//unsigned char ret;
    unsigned char packet[50];
    //unsigned int i = 0;
	for (;;)
	{
        _delay_ms(1);                                                   //generate 1khz clock
        if(!raw)                    //raw mode
            rf12packet_tick();
        else{
            rf12_rxstart();
            unsigned char ret = rf12_rxfinish(packet);
            if(ret != 255 && ret != 254 && ret != 0){
                uart_puts("acR");
                for(c=0;c<ret;c++){
                    uart_putc(packet[c]);
                    if(packet[c] == 'a'){
                        uart_putc(packet[c]);
                    }
                }
                uart_puts("ab");   
                rf12_rxstart();
            }
            if(ret == 0){
                uart_puts("accrcab");
            }
        }
        if((c = readline())> 0){
            if(buffer[0] == 'P'){
                rf12packet_send(buffer[1],(unsigned char *)buffer+2,c-2);
            }else if(buffer[0] == 'B'){
                rf12packet_sendmc(buffer[1],(unsigned char *)buffer+2,c-2);
            }else if(buffer[0] == 'A'){
                rf12packet_init(buffer[1]);
            }else if(buffer[0] == 'S'){
                rf12packet_sniff = buffer[1];
            }else if(buffer[0] == 'R'){
                if(buffer[1]){
                    raw = 1;
                    rf12_checkcrc = 0;
                }else{
                    raw = 0;
                    rf12_checkcrc = 0;
                }
                uart_puts("acDDoneab");
            }else if(buffer[0] == 'r'){
               rf12_allstop();
               rf12_txstart(buffer+1,c-1);
               uart_puts("acDDoneab");
            }

            //sprintf((char *)buf,"P3=%u\n",c);
            //rf12packet_send(1,buf,strlen((char*)buf));
        }
        if(rf12packet_status & RF12PACKET_NEWDATA){
            rf12packet_status ^= RF12PACKET_NEWDATA;
            uart_puts("ac");
            for(c=0;c<rf12packet_datalen;c++){
                uart_putc(rf12packet_data[c]);
                if(rf12packet_data[c] == 'a'){
                    uart_putc(rf12packet_data[c]);
              }
            }
            uart_puts("ab");
        }
        if(rf12packet_status & RF12PACKET_TIMEOUT){
            rf12packet_status ^= RF12PACKET_TIMEOUT;
            uart_puts("acSTab");
        }
        if(rf12packet_status & RF12PACKET_PACKETDONE){
            rf12packet_status ^= RF12PACKET_PACKETDONE;
            uart_puts("acSDab");
        }
        /*if(i++ == 100){
            i=0;
            sprintf((char *)buf,"P3=%u\n",c);
            if( rf12packet_send(1,buf,strlen((char*)buf)) == 0){
                c++;
            }
        }*/
/*        unsigned char s = ((rf12_trans(0x0000)>>8) & (1<<0));         //get clock recovery
        if(s!=r){
            //UART_Tx_Str("Change\n",7);
            UART_Tx_Str(test, sprintf(test, "ret: %u\r\n", s));
            r  = s;
        }
*/
    }

}
