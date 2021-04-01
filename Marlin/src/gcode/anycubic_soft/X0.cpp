/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "../gcode.h"
#include "../../module/planner.h"

#include "../../lcd/anycubic_soft/anycubic_max_pro_TFT.h"
#include "../../lcd/anycubic_soft/anycubic_generic_serial.h"
#include "../../lcd/anycubic_soft/anycubic_serial.h"

extern AnycubicSerialClass AnycubicSerial;

void GcodeSuite::X0() {

  ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("X0 Command Handler");

  if (parser.seenval('J'))
  {
      ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("J Command Handler");
      auto cmdID = parser.value_string();
      AnycubicSerial.print('J');
      AnycubicSerial.println(cmdID);
  }

}
