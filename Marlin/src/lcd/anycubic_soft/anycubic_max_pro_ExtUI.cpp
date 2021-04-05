#include "../../inc/MarlinConfigPre.h"

#if BOTH(ANYCUBIC_SOFT_TFT_MODEL, EXTENSIBLE_UI)

#include "../extui/ui_api.h"
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
  void onPlayTone(const uint16_t frequency, const uint16_t duration) { AnycubicTFT.PlayTone(duration, frequency); }
  void onPrintTimerStarted() { AnycubicTFT.onPrintTimerStarted(); }
  void onPrintTimerPaused() { AnycubicTFT.onPrintTimerPaused(); }
  void onPrintTimerStopped() { AnycubicTFT.onPrintTimerStopped(); }

  void onHomingStart() {}
  void onHomingComplete() {}
  void onPrintFinished() {}

  void onFilamentRunout(const extruder_t extruder) {
#ifdef FILAMENT_RUNOUT_SENSOR
    AnycubicTFT.FilamentRunout();
#endif
  }

  void onUserConfirmRequired(const char * const msg) {
    AnycubicTFT.onUserConfirmRequired(msg);
  }

  void onStatusChanged(const char * const msg) {
    AnycubicTFT.onStatusChanged(msg);
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

  #if HAS_MESH
    void onMeshUpdate(const int8_t xpos, const int8_t ypos, const float zval) {
    void onMeshLevelingStart() {}

    void onMeshUpdate(const int8_t xpos, const int8_t ypos, const float &zval) {
      // Called when any mesh points are updated
    }
  #endif

  #if HAS_PID_HEATING
    void onPidTuning(const result_t rst) {
      AnycubicTFT.onPidTuning(rst);
    }
  #endif

  #if ENABLED(POWER_LOSS_RECOVERY)
    void onPowerLossResume() {
      // Called on resume from power-loss
    }
  #endif

  void onSteppersDisabled() {}
  void onSteppersEnabled()  {}
}

#endif // EXTUI_EXAMPLE && EXTENSIBLE_UI