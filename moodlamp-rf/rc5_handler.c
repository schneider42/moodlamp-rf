/* vim:fdm=marker ts=4 et ai
 * {{{
 *         moodlamp-ng - fnordlicht firmware next generation
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

#include "config.h"

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "common.h"
#include "fnordlicht.h"
#include "pwm.h"

#if RC5_DECODER
#include "rc5.h"
#include "rc5_handler.h"
#endif

#if STATIC_SCRIPTS
#include "static_scripts.h"
#include "scripts.h"
#endif
#if SERIAL_UART
#include "uart.h"
#define DEBUG	0
#endif
#include "settings.h"

#if RC5_DECODER

#if DEBUG
void printbyte(uint8_t c)
{
    uint8_t i = c/100;
    uart_putc(i+'0');
    c-=i*100;
    i=c/10;
    uart_putc(i+'0');
    c-=i*10;
    uart_putc(c+'0');
}
#endif

void dark(void)
{
    global_pwm.channels[0].brightness =  global_pwm.channels[0].target_brightness = 0;
    global_pwm.channels[1].brightness =  global_pwm.channels[1].target_brightness = 0;
    global_pwm.channels[2].brightness =  global_pwm.channels[2].target_brightness = 0;
}

void rc5_setscript(void (*execute)(struct thread_t *current_thread), uint16_t position)
{
    script_threads[0].handler.execute = execute;
    script_threads[0].handler.position = position;
    script_threads[0].flags.disabled = 0;
    script_threads[0].handler_stack_offset = 0;
    dark(); 
}

uint8_t rc5_handler(uint8_t rc5adr, uint8_t rc5cmd)
{
#ifndef RC5_USEANY
    if(rc5adr != RC5_ADDRESS)
        return 1;
#endif
    if(rc5cmd == KEY_BRIGHTNESS_DOWN){
        if(global_pwm.dim > 0){
            global_pwm.dim--;
#if DEBUG
            uart_puts("dim=");
            printbyte(global_pwm.dim);
            uart_puts("\r\n");
#endif
        }
    }else if(rc5cmd == KEY_BRIGHTNESS_UP){
        if(global_pwm.dim < 255){
            global_pwm.dim++;
#if DEBUG
            uart_puts("dim=");
            printbyte(global_pwm.dim);
            uart_puts("\r\n");
#endif
        }
    }else if(rc5cmd == KEY_FULL_BRIGHTNESS){
        global_pwm.dim=255;
    }else if(rc5cmd == KEY_ZERO_BRIGHTNESS){
        global_pwm.dim=0;
    }
    else if(rc5cmd == KEY_POWER){
        if(global.state != STATE_STANDBY){
            global.oldstate = global.state;
            global.state = STATE_ENTERSTANDBY;
        }else{
            global.state = STATE_LEAVESTANDBY;
        }
    }else if(rc5cmd > 0 && rc5cmd < 10){
        if(sizeof(global_playlist)/(sizeof(struct playlist_t)) >= rc5cmd)
                rc5_setscript(
                    global_playlist[rc5cmd-1].execute, global_playlist[rc5cmd-1].position);
    }/*else if(rc5cmd == KEY_SCRIPT1){
        //script_threads[0].handler.execute = &memory_handler_flash;
        //script_threads[0].handler.position = (uint16_t) &green_flash;
        //script_threads[0].flags.disabled = 0;
        //script_threads[0].handler_stack_offset = 0;
        //dark();
        rc5_setscript(&memory_handler_flash, (uint16_t) &green_flash);
        if(sizeof(global_playlist)/(sizeof(struct playlist_t)) > 0)
        rc5_setscript(global_playlist[0].execute, global_playlist[0].position);
    }else if(rc5cmd == KEY_SCRIPT2){
        script_threads[0].handler.execute = &memory_handler_flash;
        script_threads[0].handler.position = (uint16_t) &blinken;
        script_threads[0].flags.disabled = 0;
        script_threads[0].handler_stack_offset = 0; 
        dark();
    }else if(rc5cmd == KEY_SCRIPT3){
        script_threads[0].handler.execute = &memory_handler_flash;
        script_threads[0].handler.position = (uint16_t) &colorchange_red;
        script_threads[0].flags.disabled = 0;
        script_threads[0].handler_stack_offset = 0;
        dark();
    }else if(rc5cmd == KEY_SCRIPT4){
        script_threads[0].handler.execute = &memory_handler_flash;
        script_threads[0].handler.position = (uint16_t) &colorchange_red_blue;
        script_threads[0].flags.disabled = 0;
        script_threads[0].handler_stack_offset = 0; 
        dark();
    }else if(rc5cmd == KEY_SCRIPT5){
        script_threads[0].handler.execute = &memory_handler_flash;
        script_threads[0].handler.position = (uint16_t) &green_blink;
        script_threads[0].flags.disabled = 0;
        script_threads[0].handler_stack_offset = 0; 
        dark();
    }else if(rc5cmd == KEY_SCRIPT6){
        script_threads[0].handler.execute = &memory_handler_flash;
        script_threads[0].handler.position = (uint16_t) &police;
        script_threads[0].flags.disabled = 0;
        script_threads[0].handler_stack_offset = 0; 
        dark();
    }*/else if(rc5cmd == KEY_FASTER){
        if(script_threads[0].speed_adjustment < 8){
            script_threads[0].speed_adjustment++;
            respeed(&script_threads[0]);
            //update_brightness();
            //execute_script_threads();
        }

    }else if(rc5cmd == KEY_SLOWER){
        if(script_threads[0].speed_adjustment > -8){
            script_threads[0].speed_adjustment--;
            respeed(&script_threads[0]);
            //update_brightness();
            //execute_script_threads();
        }
    }else if(rc5cmd == KEY_PAUSE){
        /*if(global.flags.running){
            global.flags.running = 0;
        }else{
            global.flags.running = 1;
        }*/
        if(global.state != STATE_PAUSE)
            global.state = STATE_PAUSE;
        else
            global.state = STATE_RUNNING;
    }else if(rc5cmd == KEY_SAVE){
        settings_save();
    }else if(rc5cmd == KEY_SLEEP){
        global.state = STATE_ENTERSLEEP;
    }else if(rc5cmd == KEY_RED){
        global_pwm.channel_modifier = 0;
    }else if(rc5cmd == KEY_GREEN){
        global_pwm.channel_modifier = 1;
    }else if(rc5cmd == KEY_BLUE){
        global_pwm.channel_modifier = 2;
    }else if(rc5cmd == KEY_COLOR_UP){
        global_pwm.channels[global_pwm.channel_modifier].brightness++;
    }else if(rc5cmd == KEY_COLOR_DOWN){
        global_pwm.channels[global_pwm.channel_modifier].brightness--;
    }else if(rc5cmd == KEY_COLOR_FULL){
        global_pwm.channels[global_pwm.channel_modifier].brightness=255;
    }else if(rc5cmd == KEY_COLOR_ZERO){
        global_pwm.channels[global_pwm.channel_modifier].brightness=0;
    }

    return 0;
}
#endif

