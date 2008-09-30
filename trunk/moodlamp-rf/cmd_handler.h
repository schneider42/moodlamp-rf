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
#ifndef __CMD_HANDLER_H_
#define __CMD_HANDLER_H_

#define CMD_POWER               0
#define CMD_BRIGHTNESS_UP       1
#define CMD_BRIGHTNESS_DOWN     2
#define CMD_FASTER              3
#define CMD_SLOWER              4
#define CMD_FULL_BRIGHTNESS     5
#define CMD_ZERO_BRIGHTNESS     6
#define CMD_PAUSE               7
#define CMD_SAVE                8
#define CMD_SLEEP               9
#define CMD_COLOR_UP            10
#define CMD_COLOR_DOWN          11
#define CMD_RED                 12
#define CMD_GREEN               13
#define CMD_BLUE                14
#define CMD_COLOR_FULL          15
#define CMD_COLOR_ZERO          16
#define CMD_SET_SCRIPT          17
#define CMD_GET_VERSION         'V'

uint8_t cmd_interpret(uint8_t * cmd, uint8_t * result);
uint8_t cmd_handler(uint8_t cmd, uint8_t * param, uint8_t * result);
#endif
