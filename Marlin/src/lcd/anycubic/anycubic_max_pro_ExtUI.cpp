/*
  AnycubicTFT.cpp  --- Support for Anycubic i3 Mega TFT
  Created by Christian Hopp on 09.12.17.

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
*/

#include "../../inc/MarlinConfig.h"

#if BOTH(ANYCUBIC_SOFT_TFT_MODEL_OLD, EXTENSIBLE_UI)

#include "../extui/ui_api.h"

#include "../../sd/cardreader.h"
#include "../../module/temperature.h"
#include "../../core/language.h"
#include "../../module/stepper.h"
#include "../../module/motion.h"
#include "../../libs/duration_t.h"
#include "../../module/printcounter.h"
#include "../../gcode/queue.h"

#ifdef FILAMENT_RUNOUT_SENSOR
#include "../feature/runout.h"
#endif

#ifdef ANYCUBIC_TFT_DEBUG
#include "../../core/debug_out.h"
#endif

#include "anycubic_max_pro_TFT.h"

// To implement a new UI, complete the functions below and
// read or update Marlin's state using the methods in the
// ExtUI methods in "../ui_api.h"
//
// Although it may be possible to access other state
// variables from Marlin, using the API here possibly
// helps ensure future compatibility.

namespace ExtUI {
  void onStartup() {
    /* Initialize the display module here. The following
     * routines are available for access to the GPIO pins:
     *
     *   SET_OUTPUT(pin)
     *   SET_INPUT_PULLUP(pin)
     *   SET_INPUT(pin)
     *   WRITE(pin,value)
     *   READ(pin)
     */
    AnycubicTFT.Setup();
  }
  void onIdle() {
    AnycubicTFT.Idle();
  }
  void onPrinterKilled(PGM_P const error, PGM_P const component) { AnycubicTFT.KillTFT(); }
  void onMediaInserted() { AnycubicTFT.SDCardInserted(); };
  void onMediaError() { AnycubicTFT.SDCardRemoved(); };
  void onMediaRemoved() { AnycubicTFT.SDCardRemoved(); };
  void onPlayTone(const uint16_t frequency, const uint16_t duration) { AnycubicTFT.PlayTone(frequency,duration); }
  void onPrintTimerStarted() { AnycubicTFT.onPrintTimerStarted(); }
  void onPrintTimerPaused() { AnycubicTFT.onPrintTimerPaused(); }
  void onPrintTimerStopped() { AnycubicTFT.onPrintTimerStopped(); }

  void onFilamentRunout(const extruder_t extruder) {
#ifdef FILAMENT_RUNOUT_SENSOR
    AnycubicTFT.FilamentRunout();
#endif
  }

  void onUserConfirmRequired(const char * const msg) {

    DEBUG_PRINT_PGM(PSTR("Confirm:"));
    DEBUG_PRINT(msg);
    DEBUG_PRINT_EOL();

    AnycubicTFT.ConfirmUser(msg);
  }

  //void onUserConfirmRequired_P(PGM_P const pstr) { }

  void onStatusChanged(const char * const msg) {

  }

  void onFactoryReset() {

  }

  void onStoreSettings(char *buff) {
    // This is called when saving to EEPROM (i.e. M500). If the ExtUI needs
    // permanent data to be stored, it can write up to eeprom_data_size bytes
    // into buff.

    // Example:
    //  static_assert(sizeof(myDataStruct) <= ExtUI::eeprom_data_size);
    //  memcpy(buff, &myDataStruct, sizeof(myDataStruct));
  }

  void onLoadSettings(const char *buff) {
    // This is called while loading settings from EEPROM. If the ExtUI
    // needs to retrieve data, it should copy up to eeprom_data_size bytes
    // from buff

    // Example:
    //  static_assert(sizeof(myDataStruct) <= ExtUI::eeprom_data_size);
    //  memcpy(&myDataStruct, buff, sizeof(myDataStruct));
  }

  void onConfigurationStoreWritten(bool success) {
    // This is called after the entire EEPROM has been written,
    // whether successful or not.
  }

  void onConfigurationStoreRead(bool success) {
    // This is called after the entire EEPROM has been read,
    // whether successful or not.
  }

  void onMeshUpdate(const int8_t xpos, const int8_t ypos, const float zval) {
    // This is called when any mesh points are updated
  }

  void OnPidTuning(const result_t rst) {

  }
  #if ENABLED(POWER_LOSS_RECOVERY)
  void OnPowerLossResume() {

  }
  #endif
}

#endif // EXTUI_EXAMPLE && EXTENSIBLE_UI