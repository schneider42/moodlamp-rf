#include <stdint.h>
#include <string.h>
#include <avr/interrupt.h>
#include "config.h"
#include "fnordlicht.h"
#include "common.h"
#include "control.h"
#include "pwm.h"
#include "lib/rf12packet.h"

#if ROLE==ROLE_MASTER
    unsigned int initadr = 0;
#elif ROLE==ROLE_SLAVE
    unsigned int initadr = 1;
#endif

uint16_t timeoutmax = 200;
uint32_t sleeptime=0;
uint32_t sleeptick=0;
uint16_t timeout = 0;

void control_setColor(uint8_t r, uint8_t g, uint8_t b)
{
cli();
    global_pwm.channels[0].brightness = r;
//    global_pwm.channels[0].target_brightness = r;
    global_pwm.channels[1].brightness = g;
//    global_pwm.channels[1].target_brightness = g;
    global_pwm.channels[2].brightness = b;
//    global_pwm.channels[2].target_brightness = b;
sei();
control_setTimeout();

}

void control_fade(uint8_t r, uint8_t g, uint8_t b, uint16_t speed)
{
    uint8_t pos;
    global_pwm.channels[0].target_brightness = r;
    global_pwm.channels[1].target_brightness = g;
    global_pwm.channels[2].target_brightness = b;
    for(pos = 0; pos < 3; pos++){
        global_pwm.channels[pos].speed_h = speed << 8;
        global_pwm.channels[pos].speed_l = speed & 0xFF;
    }
    control_setTimeout();

}
void control_setTimeout(void)
{
    timeout = timeoutmax;
    global.state = STATE_PAUSE;
}

void control_tick(void)
{
    switch(global.state){
        case STATE_RUNNING:
            global.flags.running = 1;
        break;
        case STATE_PAUSE:
            global.flags.running = 0;
        break;
        case STATE_ENTERSTANDBY:
            global_pwm.olddim = global_pwm.dim;
            global_pwm.dim = 0;
            global.flags.running = 0;
            global.state = STATE_STANDBY;
        case STATE_STANDBY:                 //will be left by rc5_handler
        break;
        case STATE_LEAVESTANDBY:
            global_pwm.dim = global_pwm.olddim;
            global.flags.running = 1;
            global.state = global.oldstate; //STATE_RUNNING;
        break;
        case STATE_ENTERSLEEP:
            sleeptime = 0;
            sleeptick = SLEEP_TIME/global_pwm.dim; //Calculate dim steps
            global_pwm.olddim = global_pwm.dim;
            global.state = STATE_SLEEP;
        break;
        case STATE_SLEEP:
            sleeptime++;
            if(sleeptime == sleeptick){
                sleeptime = 0;
                global_pwm.dim--;
                if(global_pwm.dim ==0){
                    global.state = STATE_STANDBY;
                    global.flags.running = 0;
                }
            }
        break;
    }
#if ROLE==ROLE_SLAVE
    static unsigned int beacon = 0;
    if(initadr == 0 && beacon++ >= 500){
        strcpy((char *)rf12packet_data,"B");
        if(rf12packet_send(2,
                            rf12packet_data,
                            strlen((char *)rf12packet_data)) == 0){
            beacon = 0;
        }
    }
    if(initadr == 1){
        rf12packet_setadr(0);
        if(global.uuid[0] == 0){
            strcpy((char *)rf12packet_data, "ID?");
            if(rf12packet_send(2,rf12packet_data,3) == 0)
                initadr = 2;
        }else{
            strcpy((char *)rf12packet_data, "ID=");
            memcpy(rf12packet_data+3,(char *)global.uuid,16);
            if(rf12packet_send(2,rf12packet_data,19) == 0)
                initadr = 2;
        }
    }

    if(initadr > 1 && initadr++ > 500)
        initadr = 1;
#endif
    if(timeout && --timeout == 0)
        global.state = STATE_RUNNING;

}

void control_gotAddress(void)
{
    initadr = 0;
}
