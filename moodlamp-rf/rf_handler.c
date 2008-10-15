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

void rf_init(void)
{
    volatile unsigned long l;
//    PORTD &= ~(1<<PD6);     //todo: remove fet
//    DDRD |= (1<<PD6);

    for(l=0;l<10000;l++);

#ifdef RF12DEBUGBIN
    printf("acDThis is moodlamp-rfab");
#endif
    rf12_init();
    rf12_setfreq(RF12FREQ(434.32));
    rf12_setbandwidth(4, 1, 4);     // 200kHz Bandbreite, -6dB Verstärkung, DRSSI threshold: -79dBm
    rf12_setbaud(19200);
    rf12_setpower(0, 6);            // 1mW Ausgangangsleistung, 120kHz Frequenzshift
#if ROLE == ROLE_MASTER
    rf12packet_init(2);
#elif ROLE == ROLE_SLAVE
    rf12packet_init(0);
#endif
    //volatile uint32_t tmp;
    //for(tmp=0;tmp<100000;tmp++);
#ifdef RF12DEBUGBIN
    printf("acDInit doneab");
#endif

}

void rf_tick(void){
    rf12packet_tick();

#if ROLE == ROLE_SLAVE
    if(rf12packet_status & RF12PACKET_NEWDATA){
        rf12packet_status ^= RF12PACKET_NEWDATA;
        
        unsigned char sender = rf12packet_data[3];
/*        printf("got packet from %d: ",sender);
        uint8_t i;
        for(i=0;i<rf12packet_datalen;i++){
            if(rf12packet_data[i] < 0x20)
                printf("0x%x ",rf12packet_data[i]);
            else
                printf("%c ",rf12packet_data[i]);
        }
        printf("\r\n");
        return;
*/
        /*if(rf12packet_data[4] == 'V'){
            //sprintf((char *)rf12packet_data,"F=%s T=%s D=%s",
            //    __FILE__,__TIME__,__DATE__);
            //strcpy((char *)rf12packet_data, "F=");
            //strcat((char *)rf12packet_data, __FILE__);
            //strcat((char *)rf12packet_data," T=");
            //strcat((char *)rf12packet_data,__TIME__);
            //strcat((char *)rf12packet_data," D=");
            
            strcpy((char *)rf12packet_data,"D=");
            strcat((char *)rf12packet_data,__DATE__);
            rf12packet_send(sender,rf12packet_data,
                            strlen((char *)rf12packet_data));
        }else*/ if(rf12packet_data[4] == 'C'){
            /*global_pwm.channels[0].brightness = rf12packet_data[5];
            global_pwm.channels[1].brightness = rf12packet_data[6];
            global_pwm.channels[2].brightness = rf12packet_data[7];
            control_setTimeout();*/
            control_setColor(rf12packet_data[5],rf12packet_data[6],rf12packet_data[7]);
        }else if(rf12packet_data[4] == 'D'){
            global_pwm.dim = rf12packet_data[5];
        }else if(rf12packet_data[4] == 'S'){
            global.state = rf12packet_data[5];
        }/*else if(rf12packet_data[4] == 'G'){
            rf12packet_data[0] = global.state;
            rf12packet_data[1] = script_threads[0].speed_adjustment;
            rf12packet_data[2] = global_pwm.dim;
            rf12packet_send(sender,rf12packet_data,3);
        }*/else if(rf12packet_data[4]> 0 && rf12packet_data[4] < 10){
            cmd_handler(CMD_SET_SCRIPT, rf12packet_data+4, NULL);
        }else if(rf12packet_data[4] == 's'){
            script_threads[0].speed_adjustment = rf12packet_data[5];
        }else if(rf12packet_data[4] == 'R'){
            jump_to_bootloader();
        }else if(rf12packet_data[4] == 'I' && 
                    rf12packet_data[5] == 'D' &&
                    rf12packet_data[6] == '='){
            memcpy((char *)global.uuid, (char *)rf12packet_data+7,16);
            settings_save();
        }else if(rf12packet_data[4] == 'I' && 
                    rf12packet_data[5] == 'D' &&
                    rf12packet_data[6] == '?'){
                    rf_sendUUID(); 
        }else if(rf12packet_data[4] == 'A' &&
                rf12packet_data[5] == 'D' &&
                rf12packet_data[6] == 'R' &&
                rf12packet_data[7] == '='){
            if(memcmp((char *)rf12packet_data+9,
                        (char *)global.uuid,16) == 0){
                rf12packet_setadr(rf12packet_data[8]);
                strcpy((char *)rf12packet_data,"D="__DATE__);
                //strcat((char *)rf12packet_data,__DATE__);
                rf12packet_send(sender,rf12packet_data,
                                strlen((char *)rf12packet_data));
                //initadr = 0;
            }
        }else if(rf12packet_data[4] == 'O' &&
                rf12packet_data[5] == 'K'){
                control_gotAddress();
        }else{
            uint8_t r = cmd_interpret(rf12packet_data+4,rf12packet_data);
            if(r){
                rf12packet_send(sender,rf12packet_data,r);
            }
        }
    }
#endif
#if ROLE == ROLE_MASTER
    if(rf12packet_status & RF12PACKET_NEWDATA){
            rf12packet_status ^= RF12PACKET_NEWDATA;
            uart1_puts("ac");
            uint8_t c;
            for(c=0;c<rf12packet_datalen;c++){
                uart1_putc(rf12packet_data[c]);
                if(rf12packet_data[c] == 'a'){
                    uart1_putc(rf12packet_data[c]);
              }
            }
            uart1_puts("ab");
        }
        if(rf12packet_status & RF12PACKET_TIMEOUT){
            rf12packet_status ^= RF12PACKET_TIMEOUT;
            uart1_puts("acSTab");
        }
        if(rf12packet_status & RF12PACKET_PACKETDONE){
            rf12packet_status ^= RF12PACKET_PACKETDONE;
            uart1_puts("acSDab");
        }
#endif
    
}
void rf_sendUUID(void)
{
    strcpy((char *)rf12packet_data, "ID=");
    memcpy(rf12packet_data+3,(char *)global.uuid,16);
    rf12packet_send(2,rf12packet_data,19);
}
