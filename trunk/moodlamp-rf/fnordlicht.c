/* vim:fdm=marker ts=4 et ai
 * {{{
 *         moodlamp-rf - fnordlicht firmware next generation
 *
 *    for additional information please
 *    see http://blinkenlichts.net/
 *    and http://koeln.ccc.de/prozesse/running/fnordlicht
 *
 * This is a modified version of the fnordlicht
 * (c) by Alexander Neumann <alexander@bumpern.de>
 *     Lars Noschinski <lars@public.noschinski.de>
 *
 * Modifications done by:
 * Kiu
 * Mazzoo
 * Tobias Schneider(schneider@blinkenlichts.net)
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
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
#include <avr/wdt.h>

#include "common.h"
#include "fnordlicht.h"
#include "pwm.h"
#include "lib/uart.h"

#include "rs485_handler.h"
#include "zbus.h"
#include "raw.h"

#include "cmd_handler.h"
#include "control.h"
#if RC5_DECODER
#include "rc5.h"
#include "rc5_handler.h"
#endif
#include "lib/rf12.h"
#include "lib/rf12packet.h"
#include "rf_handler.h"
#if STATIC_SCRIPTS
#include "static_scripts.h"
#endif
#include "settings.h"
#include "control.h"
#include "serial_handler.h"
#include "packet.h"

#if SERIAL_UART
int uart_putc_file(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(uart_putc_file, NULL, _FDEV_SETUP_WRITE);
#endif

/* structs */
volatile struct global_t global = {{0, 0}};
/* prototypes */

#if SERIAL_UART
int uart_putc_file(char c, FILE *stream)
{
    uart1_putc(c);    
    return 0;
}
#endif

void jump_to_bootloader(void)
{
    //uart1_puts("acDRab");
    //cli();
    wdt_enable(WDTO_30MS);
    while(1);
}

unsigned int random_seed __attribute__ ((section (".noinit")));

/** main function
 */
int main(void) {
//    SPCR &= ~(1<<SPE);
//    TIMSK &= ~(1<<TOIE1);
    wdt_disable();
    /* Clear WDRF in MCUSR */
    MCUSR &= ~(1<<WDRF);
    /* Write logical one to WDCE and WDE */
    /* Keep old prescaler setting to prevent unintentional time-out */
    WDTCSR |= (1<<WDCE) | (1<<WDE);
    /* Turn off WDT */
    WDTCSR = 0x00;

    //volatile long l;
//    for(l=0;l<1000000;l++);
    DDRC &= ~(1<<PC3);
    PORTC |= (1<<PC3);

    init_pwm();
#if SERIAL_UART
    uart1_init( UART_BAUD_SELECT(UART_BAUDRATE,F_CPU));
    stdout = &mystdout;
#endif

#if RC5_DECODER
    rc5_init();
#endif

#if STATIC_SCRIPTS
    init_script_threads();
#endif
    settings_read();

    control_init();

#if RS485_CTRL
    rs485_init();
    zbus_core_init();
#endif
    rf_init();
    packet_init(0,0);

    srandom(random_seed);
    random_seed = random();
    /* enable interrupts globally */
    sei();
//    global.state = STATE_RUNNING;
//    global.state = STATE_PAUSE;
//    global.flags.running = 0;
    while (1) {
        if(packetbase > 32){
            packetbase = 0;
            if(global.flags.rawmode == 0)
                packet_tick();
            else
                raw_tick();
        }

        if(global.flags.timebase){
            control_tick();
            global.flags.timebase=0;
        }
        /* after the last pwm timeslot, rebuild the timeslot table */
        /*
        if (global.flags.last_pulse) {
            global.flags.last_pulse = 0;
            if(global.flags.running)
                update_pwm_timeslots();
        }
        */

        /* at the beginning of each pwm cycle, call the fading engine and
         * execute all script threads */
        if (global.flags.new_cycle) {
            global.flags.new_cycle = 0;
            if(control_faderunning)
                update_brightness();

            if(global.flags.running){
#if STATIC_SCRIPTS
                execute_script_threads();
#endif
            }
            continue;
        }
#if RC5_DECODER
        rc5_handler();
#endif
#if RS485_CTRL
        rs485_process();
#endif
#if SERIAL_UART
        serial_process();
#endif
    }
}
