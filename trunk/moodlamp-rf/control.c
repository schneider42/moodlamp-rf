#include <stdint.h>
#include <string.h>
#include <avr/interrupt.h>
#include "config.h"
#include "fnordlicht.h"
#include "common.h"
#include "control.h"
#include "pwm.h"
#include "lib/rf12packet.h"
#include "packet.h"
#include "settings.h"
#include <string.h>
#include "interfaces.h"
uint16_t timeoutmax = 400;
uint32_t sleeptime=0;
uint32_t sleeptick=0;
uint16_t timeout = 0;
uint8_t serveradr = 0;

#define CONTROL_SEARCHMASTER         1
#define CONTROL_IDENTIFY             2
#define CONTROL_SETUPOK              3
uint8_t control_state = CONTROL_SEARCHMASTER;

void control_init(void)
{
    control_state = CONTROL_SEARCHMASTER;
}

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
    if(timeout)
        timeout = timeoutmax;
    if(global.state != STATE_PAUSE){
        global.oldstate = global.state;
        global.state = STATE_PAUSE;
        timeout = timeoutmax;
     }
}

void control_setServer(uint8_t s)
{
    serveradr = s;
    control_state = CONTROL_IDENTIFY;
}

void control_setupOK(void)
{
    control_state = CONTROL_SETUPOK;
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
    
    static unsigned int control_beacon = 500;
    if(control_beacon-- == 0){
        control_beacon = 500;
        if(control_state == CONTROL_SEARCHMASTER){
            p.flags = PACKET_BROADCAST;//don't know server yet
        }else{
            p.flags = 0;
        }
        p.dest = serveradr;     //0 if unknown
        p.src = packet_getAddress();        //put local address into src
        p.lasthop = packet_getAddress();
        p.iface = IFACE_LOCAL;
        if(control_state == CONTROL_SEARCHMASTER){  //request server adr
            p.data[0] = 'R';
            settings_readid(p.data+1);
            p.len = strlen((char*)p.data);
        }else if(control_state == CONTROL_IDENTIFY){
            p.data[0] = 'I';
            settings_readid(p.data+1);
            p.len = strlen((char*)p.data);
        }else{
//            p.flags = PACKET_BROADCAST;//don't know server yet
//            control_beacon = 100;
            p.len = 1;
            p.data[0] = 'B';
        }

        packet_packetOut(&p);        
//        control_beacon = 0;
    }

    if(timeout && --timeout == 0)
        global.state = global.oldstate;

}

