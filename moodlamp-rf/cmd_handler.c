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
#include <string.h>

#include "common.h"
#include "fnordlicht.h"
#include "pwm.h"


#include "cmd_handler.h"

#if STATIC_SCRIPTS
#include "static_scripts.h"
#include "scripts.h"
#endif
#include "settings.h"


#if 0
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

void cmd_dark(void)
{
    global_pwm.channels[0].brightness =  global_pwm.channels[0].target_brightness = 0;
    global_pwm.channels[1].brightness =  global_pwm.channels[1].target_brightness = 0;
    global_pwm.channels[2].brightness =  global_pwm.channels[2].target_brightness = 0;
}

void cmd_setscript(void (*execute)(struct thread_t *current_thread), uint16_t position)
{
    script_threads[0].handler.execute = execute;
    script_threads[0].handler.position = position;
    script_threads[0].flags.disabled = 0;
    script_threads[0].handler_stack_offset = 0;
    cmd_dark(); 
}

uint8_t cmd_interpret(uint8_t * cmd, uint8_t * result)
{
    return cmd_handler(cmd[0],cmd+1,result);
}

uint8_t cmd_handler(uint8_t cmd, uint8_t * param, uint8_t * result)
{
   if(cmd == CMD_BRIGHTNESS_DOWN){
        if(global_pwm.dim > 0){
            global_pwm.dim--;
        }
    }else if(cmd == CMD_BRIGHTNESS_UP){
        if(global_pwm.dim < 255){
            global_pwm.dim++;
        }
    }else if(cmd == CMD_FULL_BRIGHTNESS){
        global_pwm.dim=255;
    }else if(cmd == CMD_ZERO_BRIGHTNESS){
        global_pwm.dim=0;
    }else if(cmd == CMD_POWER){
        if(global.state != STATE_STANDBY){
            if(global.state == STATE_SLEEP)
                global.oldstate = STATE_RUNNING;
            else
                global.oldstate = global.state;
            global.state = STATE_ENTERSTANDBY;
        }else{
            global.state = STATE_LEAVESTANDBY;
        }
    }else if(cmd == CMD_SET_SCRIPT){
        if(sizeof(global_playlist)/(sizeof(struct playlist_t)) > param[0])
                cmd_setscript(
                    global_playlist[param[0]].execute, global_playlist[param[0]].position);
    }else if(cmd == CMD_FASTER){
        if(script_threads[0].speed_adjustment < 8){
            script_threads[0].speed_adjustment++;
            respeed(&script_threads[0]);
            //update_brightness();
            //execute_script_threads();
        }

    }else if(cmd == CMD_SLOWER){
        if(script_threads[0].speed_adjustment > -8){
            script_threads[0].speed_adjustment--;
            respeed(&script_threads[0]);
            //update_brightness();
            //execute_script_threads();
        }
    }else if(cmd == CMD_PAUSE){
        if(global.state != STATE_PAUSE)
            global.state = STATE_PAUSE;
        else
            global.state = STATE_RUNNING;
    }else if(cmd == CMD_SAVE){
        settings_save();
    }else if(cmd == CMD_SLEEP){
        global.state = STATE_ENTERSLEEP;
    }else if(cmd == CMD_RED){
        global_pwm.channel_modifier = 0;
    }else if(cmd == CMD_GREEN){
        global_pwm.channel_modifier = 1;
    }else if(cmd == CMD_BLUE){
        global_pwm.channel_modifier = 2;
    }else if(cmd == CMD_COLOR_UP){
        global_pwm.channels[global_pwm.channel_modifier].brightness++;
    }else if(cmd == CMD_COLOR_DOWN){
        global_pwm.channels[global_pwm.channel_modifier].brightness--;
    }else if(cmd == CMD_COLOR_FULL){
        global_pwm.channels[global_pwm.channel_modifier].brightness=255;
    }else if(cmd == CMD_COLOR_ZERO){
        global_pwm.channels[global_pwm.channel_modifier].brightness=0;
    }else if(cmd == CMD_GET_VERSION){
        strcpy((char *) result,"D=");
        strcat((char *) result,__DATE__);
        return strlen((char *)result);
    }

    return 0;
}

