/* vim:fdm=marker ts=4 et ai
 * {{{
 *         moodlamp - fnordlicht firmware next generation
 *
 *    for additional information please
 *    see http://blinkenlichts.net/
 *    and http://koeln.ccc.de/prozesse/running/fnordlicht
 *
 * This is a modified version of the fnordlicht
 * (c) by Alexander Neumann <alexander@bumpern.de>
 *     Lars Noschinski <lars@public.noschinski.de>
 *
 * Modifications done by Tobias Schneider(schneider@blinkenlichts.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * For more information on the GPL, please go to:
 * http://www.gnu.org/copyleft/gpl.html
 }}} */

/* includes */
#include "config.h"

#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "fnordlicht.h"
#include "pwm.h"
#include "uart.h"
#include "i2c.h"

#if RC5_DECODER
#include "rc5.h"
#include "rc5_handler.h"
#endif
#include "lib/rf12.h"
#include "lib/rf12packet.h"

#if STATIC_SCRIPTS
/* include static scripts */
#include "static_scripts.h"
//#include "testscript.h"
#endif

#include "settings.h"
int uart_putc_file(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(uart_putc_file, NULL, _FDEV_SETUP_WRITE);

/* structs */
volatile struct global_t global = {{0, 0}};

/* prototypes */
void (*jump_to_bootloader)(void) = (void *)0xc00;
static inline void init_output(void);

#if SERIAL_UART
static inline void check_serial_input(uint8_t data);
#endif

int uart_putc_file(char c, FILE *stream)
{
    uart_putc(c);    
    return 0;
}
/** init output channels */
void init_output(void) { /* {{{ */
    /* set all channels high -> leds off */
    //PORTC = _BV(PC0) | _BV(PC1) | _BV(PC2);
    PORTA = 0;
    /* configure PB0-PB2 as outputs */
    DDRA = _BV(PC0) | _BV(PC1) | _BV(PC2);
    //while(1);
}

/* }}} */

#if SERIAL_UART
/** process serial data received by uart */
void check_serial_input(uint8_t data)
/* {{{ */ {
    static uint8_t buffer[10];
    static int16_t fill = -1;
    static uint8_t escaped = 0;

    if(data == 0xAA){
        if(!escaped){
            escaped = 1;
            return;
        }
        escaped = 0;
    }else if(escaped){
        escaped = 0;
        if(data == 0x01){
            fill = 0;
            return;
        }
    }
    if(fill != -1){
        buffer[fill++] = data;
        if(fill >= 10)
            fill = -1;
    }
    uint8_t pos;
    if (buffer[0] == 0x01 && fill == 1) {  /* soft reset */

        jump_to_bootloader();
        
    } else if (buffer[0] == 0x02 && fill == 4) { /* set color */

        for ( pos = 0; pos < 3; pos++) {
            global_pwm.channels[pos].target_brightness = buffer[pos + 1];
            global_pwm.channels[pos].brightness = buffer[pos + 1];
        }

        fill = -1;

    } else if (buffer[0] == 0x03 && fill == 6) { /* fade to color */

        for (pos = 0; pos < 3; pos++) {
            global_pwm.channels[pos].speed_h = buffer[1];
            global_pwm.channels[pos].speed_l = buffer[2];
            global_pwm.channels[pos].target_brightness = buffer[pos + 3];
        }

        fill = -1;
    }
    
} /* }}} */
#endif
/** main function
 */
int main(void) {
    uint32_t sleeptime=0;
    uint32_t sleeptick=0;
    init_output();
    init_pwm();

#if SERIAL_UART
    init_uart();
//    uart_puts("Welcome to fnordlicht");
#endif

#if RC5_DECODER
    //init_rc5();
#if defined(__AVR_ATmega324P__) ||  defined(__AVR_ATmega644P__)
    TCCR0B = 1<<CS02;			//divide by 256
    TIMSK0 |= 1<<TOIE0;			//enable timer interrupt
#else
    TCCR0 = 1<<CS02;			//divide by 256
    //TIMSK0 |= 1<<TOIE0;			//enable timer interrupt
    TIMSK |= 1<<TOIE0;			//enable timer interrupt
#endif

#endif

#if I2C
    init_i2c();
#endif

#if STATIC_SCRIPTS
    init_script_threads();
#endif

    settings_read();

#if I2C_MASTER
    i2c_global.send_messages[0].command.size = 4;
    i2c_global.send_messages[0].command.code = COMMAND_SET_COLOR;
    i2c_global.send_messages[0].command.set_color_parameters.colors[0] = 0x10;
    i2c_global.send_messages[0].command.set_color_parameters.colors[1] = 0x10;
    i2c_global.send_messages[0].command.set_color_parameters.colors[2] = 0x10;

    i2c_global.send_messages_count = 1;
#endif

#if RS485_CTRL
    /* init command bus */
    UCSR0A = _BV(MPCM0); /* enable multi-processor communication mode */
    UCSR0C = _BV(UCSZ00) | _BV(UCSZ01); /* 9 bit frame size */

    #define UART_UBRR 8 /* 115200 baud at 16mhz */
    UBRR0H = HIGH(UART_UBRR);
    UBRR0L = LOW(UART_UBRR);

    UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(UCSZ02); /* enable receiver and transmitter */
#endif

    stdout = &mystdout;
    rf12_init();				// ein paar Register setzen (z.B. CLK auf 10MHz)
#ifdef RF12DEBUGBIN
    printf("acDThis is 3ab");
#endif
//while(1);
    rf12_setfreq(RF12FREQ(434.32));		// Sende/Empfangsfrequenz auf 433,92MHz einstellen
    rf12_setbandwidth(4, 1, 4);		// 200kHz Bandbreite, -6dB Verstärkung, DRSSI threshold: -79dBm 
    rf12_setbaud(19200);			// 19200 baud
    rf12_setpower(0, 6);			// 1mW Ausgangangsleistung, 120kHz Frequenzshift
    rf12packet_init(3);
#ifdef RF12DEBUGBIN
    printf("acDInit doneab");
#endif
    /* enable interrupts globally */
    sei();

    while (1) {
        //if(global.flags.rfm12base){
        if(rfm12base > 32){
            //global.flags.rfm12base = 0;
            rfm12base = 0;
            //uart_puts("acDtab");
            rf12packet_tick();
            if(rf12packet_status & RF12PACKET_NEWDATA){
                rf12packet_status ^= RF12PACKET_NEWDATA;
                /*uart_puts("ac");
                uint8_t c;
                for(c=0;c<rf12packet_datalen;c++){
                    uart_putc(rf12packet_data[c]);
                    if(rf12packet_data[c] == 'a'){
                        uart_putc(rf12packet_data[c]);
                    }
                }
                uart_puts("ab");*/
                unsigned char sender = rf12packet_data[3];
                if(rf12packet_data[4] == 'V'){
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
                }else if(rf12packet_data[4] == 'C'){
                    global_pwm.channels[0].brightness = rf12packet_data[5];
                    global_pwm.channels[1].brightness = rf12packet_data[6];
                    global_pwm.channels[2].brightness = rf12packet_data[7];
                }else if(rf12packet_data[4] == 'D'){
                    global_pwm.dim = rf12packet_data[5];
                }else if(rf12packet_data[4] == 'S'){
                    //printf("acD%dab",global.state);
                    global.state = rf12packet_data[5];
                    //printf("acD%dab",global.state);
                }else if(rf12packet_data[4] == 'G'){
                    rf12packet_data[0] = global.state;
                    rf12packet_data[1] = script_threads[0].speed_adjustment;
                    rf12packet_data[2] = global_pwm.dim;
                    rf12packet_send(sender,rf12packet_data,3);
                }else if(rf12packet_data[4]> 0 && rf12packet_data[4] < 10){
                    rc5_handler(RC5_ADDRESS, rf12packet_data[4]);
                }else if(rf12packet_data[4] == 's'){
                    script_threads[0].speed_adjustment = rf12packet_data[5];
                }
            }
        }
        if(global.flags.timebase){
            static unsigned int beacon = 0;
            if(beacon++ >= 3000){
                strcpy((char *)rf12packet_data,"B");
                if(rf12packet_send(2,
                                   rf12packet_data,
                                   strlen((char *)rf12packet_data)) == 0){
                    beacon = 0;
                }
            }
            /* State machine covering basic functionality*/
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
            global.flags.timebase=0;
        }
        /* after the last pwm timeslot, rebuild the timeslot table */
        if (global.flags.last_pulse) {
            global.flags.last_pulse = 0;
            //if(global.flags.running)
                update_pwm_timeslots();
        }


        /* at the beginning of each pwm cycle, call the fading engine and
         * execute all script threads */
        if (global.flags.new_cycle) {
            global.flags.new_cycle = 0;
            if(global.flags.running)
                update_brightness();
#if STATIC_SCRIPTS
            if(global.flags.running) 
                execute_script_threads();
#endif
            continue;
        }

#if 0               //gives problems when the ir receiver is connected to rxd
if SERIAL_UART
        /* check if we received something via uart */
        if (fifo_fill(&global_uart.rx_fifo) > 0) {
            check_serial_input(fifo_load(&global_uart.rx_fifo));
            continue;
        }
#endif

//settings.power =0;
#if RC5_DECODER
    cli();
    uint16_t rc5d =  rc5_data;			// read two bytes from interrupt !
    rc5_data = 0;
    sei();
    
    if(checkRC5(rc5d)){
        uint8_t rc5adr = rc5d >> 6 & 0x1F;
        uint8_t rc5cmd = (rc5d & 0x3F) | (~rc5d >> 7 & 0x40);
        //rc5adr = RC5_ADDRESS;
        if(global.state != STATE_STANDBY || rc5cmd == RC5_POWER || rc5cmd == RC5_RECORD)   //filter codes during standby
            rc5_handler(rc5adr,rc5cmd);
    }

#endif

#if RS485_CTRL
        if (UCSR0A & _BV(RXC0)) {

            uint8_t address = UCSR0B & _BV(RXB80); /* read nineth bit, zero if data, one if address */
            uint8_t data = UDR0;
            static uint8_t buffer[8];
            static uint8_t fill = 0;

            if (UCSR0A & _BV(MPCM0) || address) { /* if MPCM mode is still active, or ninth bit set, this is an address packet */

                /* check if we are ment */
                if (data == 0 || data == RS485_ADDRESS) {

                    /* remove MPCM flag and reset buffer fill counter */
                    UCSR0A &= ~_BV(MPCM0);
                    fill = 0;

                    continue;

                } else {/* turn on MPCM */

                    UCSR0A |= _BV(MPCM0);
                    continue;

                }
            }

            /* else this is a data packet, put data into buffer */
            buffer[fill++] = data;

            if (buffer[0] == 0x01) {  /* soft reset */

                jump_to_bootloader();

            } else if (buffer[0] == 0x02 && fill == 4) { /* set color */

                for (uint8_t pos = 0; pos < 3; pos++) {
                    global_pwm.channels[pos].target_brightness = buffer[pos + 1];
                    global_pwm.channels[pos].brightness = buffer[pos + 1];
                }

                UCSR0A |= _BV(MPCM0); /* return to MPCM mode */

            } else if (buffer[0] == 0x03 && fill == 6) { /* fade to color */

                for (uint8_t pos = 0; pos < 3; pos++) {
                    global_pwm.channels[pos].speed_h = buffer[1];
                    global_pwm.channels[pos].speed_l = buffer[2];
                    global_pwm.channels[pos].target_brightness = buffer[pos + 3];
                }

                UCSR0A |= _BV(MPCM0); /* return to MPCM mode */
            }

        }
#endif

#if I2C_MASTER
        i2c_master_check_queue();
#endif
    }
}
