/* vim:fdm=marker ts=4 et ai
 * {{{
 *         fnordlicht firmware next generation
 *
 *    for additional information please
 *    see http://koeln.ccc.de/prozesse/running/fnordlicht
 *
 * (c) by Alexander Neumann <alexander@bumpern.de>
 *     Lars Noschinski <lars@public.noschinski.de>
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

#ifndef __FNORDLICHT_H_
#define __FNORDLICHT_H_
/* global flag(=bit) structure */
struct flags_t {
    /* set by pwm interrupt after burst, signals the beginning of a new pwm
     * cycle to the main loop. */
    uint8_t new_cycle:1;
    /* set by pwm interrupt after last interrupt in the current cycle, signals
     * the main loop to rebuild the pwm timslot table */
    uint8_t last_pulse:1;
    uint8_t timebase:1;
    uint8_t running:1;
    uint8_t rfm12base:1;
};

#define STATE_RUNNING       0
#define STATE_STANDBY       1
#define STATE_PAUSE         2
#define STATE_ENTERSTANDBY  3
#define STATE_LEAVESTANDBY  4
#define STATE_ENTERSLEEP    5
#define STATE_SLEEP         6

#define SLEEP_TIME          (180 * 125)        //125 equals 1 second
//#define STATE_

struct global_t {
    struct flags_t flags;
    uint8_t state;
    uint8_t oldstate;
    uint8_t uuid[16];
};

extern volatile struct global_t global;

void jump_to_bootloader(void);

#endif
