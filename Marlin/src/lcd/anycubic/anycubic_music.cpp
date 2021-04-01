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

#if BOTH(ANYCUBIC_SOFT_TFT_MODEL_OLD, ANYCUBIC_MUSIC)

#include "anycubic_music.h"
#include "../../libs/buzzer.h"
#include "Arduino.h"

namespace ExtUI {

const uint16_t FL[] PROGMEM=
{
	//NOTE_CS6,
	//NOTE_C6,
	//NOTE_DS6,

	NOTE_G7,
	NOTE_FS7,
	NOTE_A7,

	0,
};

const uint16_t timercount[] PROGMEM=
{
	10,10,10,8
};

const uint16_t printdone[] PROGMEM=
{
	NOTE_F7,
	NOTE_C7,
	NOTE_D7,
	NOTE_E7,
	NOTE_D7,
	NOTE_C7,
	NOTE_B6,
	0,
};

const uint16_t melody[] PROGMEM=
{
	NOTE_F7,
	NOTE_C7,
	NOTE_D7,
	NOTE_E7,
	NOTE_D7,
	NOTE_G7,
	NOTE_A7,
	0,
};

const uint16_t noteDurations[] PROGMEM=
{
  10,10,8,8,4,4,1,4,
};

void PowerOnMusic()
{
	for (int thisNote = 0; thisNote <8; thisNote++) {
		int noteDuration = 1000/ pgm_read_word(&noteDurations[thisNote]);
		tone(BEEPER_PIN, pgm_read_word(&melody[thisNote]),noteDuration);
		int pauseBetweenNotes = noteDuration * 2;
		delay(pauseBetweenNotes);
		noTone(8);
	}
}

void PowerOFFMusic()
{
	for (int thisNote = 0; thisNote <8; thisNote++) {
		int noteDuration = 1000/pgm_read_word(&noteDurations[thisNote]);
		tone(BEEPER_PIN, pgm_read_word(&printdone[thisNote]),noteDuration);
		int pauseBetweenNotes = noteDuration*2 ;
		delay(pauseBetweenNotes);
		noTone(8);
	}
}

void FilamentLack()
{
	for (int thisNote = 0; thisNote <4; thisNote++) {
		int noteDuration = 1000/pgm_read_word(&timercount[thisNote]);
		tone(BEEPER_PIN, pgm_read_word(&FL[thisNote]),noteDuration);
		int pauseBetweenNotes = noteDuration*2 ;
		delay(pauseBetweenNotes);
		noTone(8);
	}
}

}

#endif