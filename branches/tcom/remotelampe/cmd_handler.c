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
#include <stdio.h>

#include "common.h"
#include "fnordlicht.h"
#include "pwm.h"
#include "cmd_handler.h"

#if STATIC_SCRIPTS
#include "static_scripts.h"
#include "scripts.h"
#endif
#include "settings.h"
#include "control.h"
#include "packet.h"
#include "adc.h"

void cmd_dark(void)
{
    global_pwm.channels[0].brightness =
                global_pwm.channels[0].target_brightness = 0;
    global_pwm.channels[1].brightness =
                global_pwm.channels[1].target_brightness = 0;
    global_pwm.channels[2].brightness =
                global_pwm.channels[2].target_brightness = 0;
}


void cmd_setscript(void (*execute)(struct thread_t *current_thread), uint16_t position)
{
    script_threads[0].handler.execute = execute;
    script_threads[0].handler.position = position;
    script_threads[0].flags.disabled = 0;
    script_threads[0].handler_stack_offset = 0;
    cmd_dark(); 
}

void cmd_setscriptNum(uint8_t index){
    cmd_setscript(global_playlist[index].execute, global_playlist[index].position);
}

uint8_t cmd_interpret(uint8_t * cmd, uint8_t * result)
{
    return cmd_handler(cmd[0],cmd+1,result);
}

uint8_t cmd_handler(uint8_t cmd, uint8_t * param, uint8_t * result)
{
    p.src = 0;
    p.dest = 0;
    p.lasthop = 0;
    p.nexthop = 0;
    p.flags = PACKET_BROADCAST;
    if( param == NULL )
        p.len = 1;
    else
        p.len = 4;

    p.data[0] = cmd;
    if( param != NULL ){
        p.data[1] = param[0];
        p.data[2] = param[1];
        p.data[3] = param[2];
    }
    rf12packet_packetOut(&p);
    return 0;
}

