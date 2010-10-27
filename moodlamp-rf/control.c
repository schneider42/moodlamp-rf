#include <stdint.h>
#include <string.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>

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
#include <avr/wdt.h>
#include "adc.h"
#include "cmd_handler.h"
#include "scripts.h"
#include <avr/sleep.h>

uint16_t timeoutmax = 400;
uint32_t sleeptime=0;
uint32_t sleeptick=0;
uint16_t timeout = 0;
uint8_t  serveradr = 0;
uint16_t control_beacontime = 1000;
uint8_t control_faderunning = 0;
uint16_t time = 0;

#define CONTROL_SEARCHMASTER         1
#define CONTROL_IDENTIFY             2
#define CONTROL_SETUPOK              3
uint8_t control_state = CONTROL_SEARCHMASTER;

void control_init(void)
{
    control_state = CONTROL_SEARCHMASTER;
    control_faderunning = global.flags.running;
}

void control_setColor(uint8_t r, uint8_t g, uint8_t b)
{
cli();
    global_pwm.channels[0].brightness = r;
    global_pwm.channels[0].target_brightness = r;
    global_pwm.channels[1].brightness = g;
    global_pwm.channels[1].target_brightness = g;
    global_pwm.channels[2].brightness = b;
    global_pwm.channels[2].target_brightness = b;
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
        global_pwm.channels[pos].speed_h = speed >> 8;
        global_pwm.channels[pos].speed_l = speed & 0xFF;
    }
    control_setTimeout();
}

void control_fadems(uint8_t r, uint8_t g, uint8_t b, uint16_t time)
{
    uint8_t pos;
    uint8_t max = 0;
    uint8_t tmp;

    global_pwm.channels[0].target_brightness = r;
    global_pwm.channels[1].target_brightness = g;
    global_pwm.channels[2].target_brightness = b;

    for( pos = 0; pos < 3; pos++){
        tmp = abs(global_pwm.channels[pos].brightness - global_pwm.channels[pos].target_brightness);
        if( tmp > max ){
            max = tmp;
        }
    }
    //speed = steps/tick
    //time = steps / (speed * ticks_per_time)
    //speed = steps * 1000 / ( time * 144 );
    uint32_t lsteps = max * 1024L * 256L; //good enough
    uint16_t speed = lsteps / (time * 144L);
    
    for(pos = 0; pos < 3; pos++){
        global_pwm.channels[pos].speed_h = speed >> 8;
        global_pwm.channels[pos].speed_l = speed & 0xFF;
    }
  //control_setTimeout();           //setting state manually since without STATE_REMOTE scripts will
    global.state = STATE_REMOTE;    //inerfere with fadems. Because of this state has to be reset also
    global.oldstate = STATE_REMOTE; //manually when launching script. see => cmd_handler.c
                                    //TODO: add STATE_REMOTE handling to state-machine
}

void control_fademsalt(uint8_t r, uint8_t g, uint8_t b, uint16_t time)
{
    uint8_t pos;
    global_pwm.channels[0].target_brightness = r;
    global_pwm.channels[1].target_brightness = g;
    global_pwm.channels[2].target_brightness = b;

    for(pos = 0; pos < 3; pos++){
        //about 1000 cycles per channel
        //PORTC |= (1<<PC0) | (1<<PC1);
        uint8_t a = global_pwm.channels[pos].brightness;
        uint8_t b = global_pwm.channels[pos].target_brightness;
        if( a == b ){
            PORTC &= ~((1<<PC1) | (1<<PC0));
            continue;
        }
        uint8_t dist = abs(a-b);

        //PORTC &= ~(1<<PC1);
        uint32_t lsteps = dist * 1024L * 256L; //good enough
        uint16_t speed = lsteps / (time * 144L);
        global_pwm.channels[pos].speed_h = speed >> 8;
        global_pwm.channels[pos].speed_l = speed & 0xFF;
        //PORTC &= ~((1<<PC0));
    }

    control_setTimeout();

}
void control_setTimeout(void)
{
    if(timeout)
        timeout = timeoutmax;
    if(global.state != STATE_PAUSE && global.state != STATE_REMOTE){
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

void control_standby(uint16_t wait)
{
    time = wait;
    global.state = STATE_ENTERSTANDBY;
}

void control_lowbat(void)
{
    if( global.state != STATE_ENTERPOWERDOWN  && global.state != STATE_LOWBAT){
        cmd_setscript(&memory_handler_flash, (uint16_t) &red_blink);
        global.state = STATE_LOWBAT;
    }
}

void control_tick(void)
{
    switch(global.state){
        case STATE_REMOTE:
            control_faderunning = 1;
            global.flags.running = 0;
        break;
        case STATE_RUNNING:
            control_faderunning = 1;
            global.flags.running = 1;
        break;
        case STATE_PAUSE:
            control_faderunning = 0;
            global.flags.running = 0;
        break;
        case STATE_ENTERSTANDBY:
            if( time-- == 0){
                global_pwm.olddim = global_pwm.dim;
                global_pwm.dim = 0;
                global.flags.running = 0;
                global.state = STATE_STANDBY;
            }
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
        case STATE_LOWBAT:
            time = 3000;
            global.state = STATE_ENTERPOWERDOWN;
        break;
        case STATE_ENTERPOWERDOWN:
            if( time-- == 0){
                //cli();
                PORTA = 0;
                set_sleep_mode(SLEEP_MODE_PWR_DOWN);
                while(1)
                    sleep_mode();
            }
        break;
    }
    
    static unsigned int control_beacon = 1000;
    if(control_beacontime !=  0 && control_beacon-- == 0 ){
        control_beacon = control_beacontime;
        
        uint16_t bat = adc_getChannel(6);
        if( bat < ADC_MINBATIDLE ){
            //global.flags.lowbat = 1;
        }

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
            if( global.config >= 30){      //use the adc
                sprintf((char *)p.data,"V=%u",bat);
                p.len = strlen((char *)p.data);
            }else{                  //no adc yet
                p.len = 1;
                p.data[0] = 'B';
            }
        }

        packet_packetOut(&p);        
//        control_beacon = 0;
    }

    if(timeout && --timeout == 0)
        global.state = global.oldstate;

}

void control_selfassign(void)
{
    uint8_t adr = idbuf[0];
    wdt_reset();
    main_reset = 0;
    if(control_state == CONTROL_SETUPOK)
        return;
    packet_setAddress(adr,adr);
    control_setServer(1);
    control_setupOK();
}
