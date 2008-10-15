#include <stdint.h>
#include <avr/io.h>
#include "config.h"
#include "fnordlicht.h"
#include "lib/uart.h"
#include "lib/rf12packet.h"
#include "lib/rf12.h"

#if SERIAL_UART
static char buffer[150];

unsigned char readline( void )
{
    static int fill = -1;
    static int escaped = 0;
    int  i = uart1_getc();
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
        if(fill >= 150)
            fill = 149;
    }
    return 0;
}
void serial_process(void)
{
    uint8_t c;
    if((c = readline())> 0){
        if(buffer[0] == 'P'){
            if(rf12packet_send(buffer[1],(unsigned char *)buffer+2,c-2)){
                uart1_puts("acsend errorab");
                while(1);
            }
        }else if(buffer[0] == 'B'){
            rf12packet_sendmc(buffer[1],(unsigned char *)buffer+2,c-2);
        }else if(buffer[0] == 'A'){
            rf12packet_init(buffer[1]);
        }else if(buffer[0] == 'S'){
            rf12packet_sniff = buffer[1];
        }else if(buffer[0] == 'R'){
/*            
            if(buffer[1]){
                raw = 1;
                rf12_checkcrc = 0;
            }else{
                raw = 0;
                rf12_checkcrc = 1;
            }
*/
            uart_puts("acDRaaw Doneab");
        }else if(buffer[0] == 'r'){
            rf12_allstop();
            rf12_txstart(buffer+1,c-1);
            //uart_puts("acDDoneab");
//            intransit = 1;
        }
    }
}
#endif
