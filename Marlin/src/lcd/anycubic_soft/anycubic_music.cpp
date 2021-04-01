
#include "../../inc/MarlinConfigPre.h"

#if ALL(ANYCUBIC_SOFT_TFT_MODEL, ANYCUBIC_MUSIC, HAS_BUZZER)

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