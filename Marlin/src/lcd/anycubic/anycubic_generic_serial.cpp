/*
  AnycubicSerial.cpp  --- Support for Anycubic i3 Mega TFT serial connection
  Created by Christian Hopp on 09.12.17.

  Original file:
  HardwareSerial.cpp - Hardware serial library for Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Modified 23 November 2006 by David A. Mellis
  Modified 28 September 2010 by Mark Sproul
  Modified 14 August 2012 by Alarus
*/

#include "../../inc/MarlinConfig.h"

#if ALL(ANYCUBIC_SOFT_TFT_MODEL_OLD, EXTENSIBLE_UI, ANYCUBIC_GENERIC_SERIAL)

#include "anycubic_generic_serial.h"

#if defined(__AVR__)
  #ifndef INTERNAL_SERIAL_PORT
    #error "anycubic TFT INTERNAL_SERIAL_PORT must be defined."
  #endif
#elif defined(ANYCUBIC_GENERIC_SERIAL) && defined(TARGET_LPC1768)
  #ifdef SERIAL_PORT_2
    #error "anycubic TFT uses SERIAL_PORT_2 exclusively."
  #else
    #include "../../HAL_LPC1768/MarlinSerial.h"
    MarlinSerial MSerial(LPC_UART0);
    extern "C" void UART0_IRQHandler() {
      MSerial.IRQHandler();
    }
  #endif
#else
   #error "anycubic TFT is not supported given target processor."
#endif

// Constructors ////////////////////////////////////////////////////////////////

AnycubicSerialClass::AnycubicSerialClass() {
}

// Public Methods //////////////////////////////////////////////////////////////
STREAM_SIZE_T AnycubicSerialClass::available(void) {
  return TFT_SERIAL.available();
}

STREAM_INT16_T AnycubicSerialClass::peek(void) {
  return TFT_SERIAL.peek();
}

STREAM_INT16_T AnycubicSerialClass::read(void) {
  return TFT_SERIAL.read();
}

void AnycubicSerialClass::flush() {
  TFT_SERIAL.flush();
}

size_t AnycubicSerialClass::write(uint8_t c) {
  TFT_SERIAL.write(c);
  return 1;
}

AnycubicSerialClass::operator bool() {
	return true;
}

AnycubicSerialClass AnycubicSerial;

#endif // whole file
