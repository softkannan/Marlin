#include "../../inc/MarlinConfigPre.h"

#if BOTH(ANYCUBIC_SOFT_TFT_MODEL, EXTENSIBLE_UI)

#include "../extui/ui_api.h"

#include "anycubic_max_pro_TFT.h"
#include "anycubic_generic_serial.h"
#include "anycubic_serial.h"
#include "anycubic_music.h"

extern AnycubicSerialClass AnycubicSerial;
extern ExtUI::FileList fileList;

namespace ExtUI {

  void AnycubicTFTClass::onPrintTimerStarted()
  {
  #ifdef ANYCUBIC_POWERDOWN
          _powerOFFflag = false;
  #endif

#if ENABLED(CASE_LIGHT_ENABLE)
    ExtUI::setCaseLightState(true);
#endif
  }
  void AnycubicTFTClass::onPrintTimerPaused()
  {

  }
  void AnycubicTFTClass::onPrintTimerStopped()
  {
#ifdef ANYCUBIC_POWER_OUTTAGE_TEST
    _PowerTestFlag=false;
    WRITE(OUTAGECON_PIN,LOW);
#endif
#ifdef ANYCUBIC_POWERDOWN
    _powerOFFflag=true;
#endif
    ANYCUBIC_SERIAL_PROTOCOLPGM("J14");// J14 print done
    ANYCUBIC_SERIAL_ENTER();
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("StateHandler: SD print done... J14");

#if ENABLED(CASE_LIGHT_ENABLE)
    ExtUI::setCaseLightState(false);
#endif
  }

void AnycubicTFTClass::StartSDPrint() {

  #ifdef ANYCUBIC_POWERDOWN
          _powerOFFflag = false;
  #endif

  #ifdef SDSUPPORT
    if(ExtUI::isMediaInserted() && !ExtUI::isMoving()) {
      if (!card.isFileOpen()) {
        ANYCUBIC_SERIAL_PROTOCOLPGM("J21"); // J21 Open failed
        ANYCUBIC_SERIAL_ENTER();
        ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("TFT Serial Debug: File open failed... J21");
        return;
      }
      card.startFileprint();
      ANYCUBIC_SERIAL_CMD_SENDPGM("J04");// J04 printing form sd card now
      ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("DEBUG: Regular Start");
    }
  #endif
}
void AnycubicTFTClass::ResumeSDPrint() {
  #ifdef SDSUPPORT
  if(ExtUI::isPrintingFromMediaPaused() || card.isFileOpen())
  {
    ExtUI::resumePrint();
    ANYCUBIC_SERIAL_CMD_SENDPGM("J04");// J04 printing form sd card now
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("DEBUG: Regular Start");
  }
  #endif
}
void AnycubicTFTClass::ResumePrintFromOutage()
{
 #ifdef SDSUPPORT
  if(ExtUI::isPrintingFromMediaPaused() || card.isFileOpen())
  {
    ExtUI::resumePrint();
    ANYCUBIC_SERIAL_CMD_SENDPGM("J04");// J04 printing form sd card now
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("DEBUG: Regular Start");
  }
  #endif
}
// Called by
void AnycubicTFTClass::PauseSDPrint() {
  #ifdef SDSUPPORT
  ANYCUBIC_SERIAL_CMD_SENDPGM("J18"); //J18 Pause success
  ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("DEBUG: Regular Pause");
  if(ExtUI::isPrintingFromMedia())
  {
    ExtUI::pausePrint(); // pause print regularly
  }
  #endif
}

void AnycubicTFTClass::StopSDPrint(){
  #ifdef SDSUPPORT
  if(ExtUI::isPrintingFromMedia() || ExtUI::isPrintingFromMediaPaused()) {
      ExtUI::stopPrint();
      ANYCUBIC_SERIAL_CMD_SENDPGM("J16");// J16 stop print
      ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("DEBUG: Stopped and cleared");
  }
  #endif
}

#ifdef FILAMENT_RUNOUT_SENSOR
// Get Called only when printing is active and only one time per one occurance
// After this function is called M600 gcode is issued to pause the print
void AnycubicTFTClass::FilamentRunout()
{
  playErrorTone();
  ANYCUBIC_SERIAL_PROTOCOLPGM("J15"); //J15 FILAMENT LACK
  ANYCUBIC_SERIAL_ENTER();
  DEBUG_ECHOLNPAIR("TFT Serial Debug: Filament runout... J15");
}
#endif

uint8_t AnycubicTFTClass::isWaitingForUserConfirm()
{
  #if HAS_RESUME_CONTINUE
    return wait_for_user;
  #else
    return false;
  #endif
}

void AnycubicTFTClass::waitForUserConfirm()
{
  #if HAS_RESUME_CONTINUE
    playSuccessTone();
    ANYCUBIC_SERIAL_PROTOCOLPGM("J05"); //Alert: Command has been sent, wait for response
    ANYCUBIC_SERIAL_ENTER();
    DEBUG_ECHOLNPAIR("TFT Serial Debug: waitForUserConfirm: ", DWIN_TFT_TX_PRINT_PAUSE);
    ANYCUBIC_SERIAL_PROTOCOLPGM("J18"); //Status: Pause, Brings up Pause / Resume button screen
    ANYCUBIC_SERIAL_ENTER();
    DEBUG_ECHOLNPAIR("TFT Serial Debug: waitForUserConfirm: ", DWIN_TFT_TX_PRINT_PAUSE_REQ);
  #endif
}

void AnycubicTFTClass::onUserConfirmRequired(const char * const msg)
{

  //  #if ENABLED(ANYCUBIC_LCD_DEBUG)
  //   SERIAL_ECHOLNPAIR("TFT Serial Debug: OnUserConfirmRequired triggered... ", msg);
  // #endif

  // #if ENABLED(SDSUPPORT)
  //   /**
  //    * Need to handle the process of following states
  //    * "Nozzle Parked"
  //    * "Load Filament"
  //    * "Filament Purging..."
  //    * "HeaterTimeout"
  //    * "Reheat finished."
  //    *
  //    * NOTE:  The only way to handle these states is strcmp_P with the msg unfortunately (very expensive)
  //    */
  //   if (strcmp_P(msg, PSTR("Nozzle Parked")) == 0) {
  //     mediaPrintingState = AMPRINTSTATE_PAUSED;
  //     mediaPauseState    = AMPAUSESTATE_PARKED;
  //     // enable continue button
  //     SENDLINE_DBG_PGM("J18", "TFT Serial Debug: UserConfirm SD print paused done... J18");
  //   }
  //   else if (strcmp_P(msg, PSTR("Load Filament")) == 0) {
  //     mediaPrintingState = AMPRINTSTATE_PAUSED;
  //     mediaPauseState    = AMPAUSESTATE_FILAMENT_OUT;
  //     // enable continue button
  //     SENDLINE_DBG_PGM("J18", "TFT Serial Debug: UserConfirm Filament is out... J18");
  //     SENDLINE_DBG_PGM("J23", "TFT Serial Debug: UserConfirm Blocking filament prompt... J23");
  //   }
  //   else if (strcmp_P(msg, PSTR("Filament Purging...")) == 0) {
  //     mediaPrintingState = AMPRINTSTATE_PAUSED;
  //     mediaPauseState    = AMPAUSESTATE_PARKING;
  //     // TODO: JBA I don't think J05 just disables the continue button, i think it injects a rogue M25. So taking this out
  //     // disable continue button
  //     // SENDLINE_DBG_PGM("J05", "TFT Serial Debug: UserConfirm SD Filament Purging... J05"); // J05 printing pause

  //     // enable continue button
  //     SENDLINE_DBG_PGM("J18", "TFT Serial Debug: UserConfirm Filament is purging... J18");
  //   }
  //   else if (strcmp_P(msg, PSTR("HeaterTimeout")) == 0) {
  //     mediaPrintingState = AMPRINTSTATE_PAUSED;
  //     mediaPauseState    = AMPAUSESTATE_HEATER_TIMEOUT;
  //     // enable continue button
  //     SENDLINE_DBG_PGM("J18", "TFT Serial Debug: UserConfirm SD Heater timeout... J18");
  //   }
  //   else if (strcmp_P(msg, PSTR("Reheat finished.")) == 0) {
  //     mediaPrintingState = AMPRINTSTATE_PAUSED;
  //     mediaPauseState    = AMPAUSESTATE_REHEAT_FINISHED;
  //     // enable continue button
  //     SENDLINE_DBG_PGM("J18", "TFT Serial Debug: UserConfirm SD Reheat done... J18");
  //   }
  // #endif

  #if HAS_RESUME_CONTINUE
    if(msg) {
      if(strcasecmp_P(msg, PSTR(ANYCUBIC_TFT_HEATER_TIMEOUT_MSG)) == 0) {
        ANYCUBIC_SERIAL_PROTOCOLPGM("J06"); //Status: Nozzle heating
        ANYCUBIC_SERIAL_ENTER();
        waitForUserConfirm();
      } else if(strcasecmp_P(msg, PSTR(ANYCUBIC_TFT_REHEAT_FINISHED_MSG)) == 0) {
        ANYCUBIC_SERIAL_PROTOCOLPGM("J07"); //Status: Nozzle heating done
        ANYCUBIC_SERIAL_ENTER();
        waitForUserConfirm();
      } else if(strcasecmp_P(msg, PSTR(ANYCUBIC_TFT_LOAD_FILAMENT_MSG)) == 0) {
        waitForUserConfirm();
      } else if(strcasecmp_P(msg, PSTR(ANYCUBIC_TFT_PURGE_FILAMENT_MSG)) == 0) {
        waitForUserConfirm();
      } else if(strcasecmp_P(msg, PSTR(ANYCUBIC_TFT_NOZZLE_PARKED_MSG)) == 0) {
        waitForUserConfirm();
      } else if(strcasecmp_P(msg, PSTR(ANYCUBIC_TFT_MMU2_EJECT_RECOVER)) == 0) {
        waitForUserConfirm();
      } else if(strcasecmp_P(msg, PSTR(ANYCUBIC_TFT_M43_WAIT_MSG)) == 0) {
        waitForUserConfirm();
      }
#if defined(FILAMENT_RUNOUT_SENSOR)
      else if(runout.filament_ran_out) {
        waitForUserConfirm();
      }
#endif
      else {
        ExtUI::setUserConfirmed();
      }
      DEBUG_ECHOLNPAIR("TFT Serial Debug: userConfirmRequired: ", msg);
    }
  #endif

  //currently there is no best way to implement on stock display firmware, has to think about more
  // call this method once user confirms the filament load
    //setUserConfirmed();

  //ANYCUBIC_SERIAL_ERROR_START;
  //ANYCUBIC_SERIAL_PROTOCOL(msg);
  //ANYCUBIC_SERIAL_ENTER();

  //if(STRCASECMP_P(msg,PSTR(ANYCUBIC_TFT_NOZZLE_PARKED_MSG)) == 0) {
    //ANYCUBIC_SERIAL_PROTOCOLPGM("J23");//j23 FILAMENT LACK with the prompt box don't disappear
    //ANYCUBIC_SERIAL_ENTER();
  //}
  //else {
    //// call this method once user confirms the filament load
    //setUserConfirmed();
  //}

}

void AnycubicTFTClass::onStatusChanged(const char * const msg)
{
  if(msg) {
    if(strcasecmp_P(msg, PSTR("Reheating...")) == 0) {
      ANYCUBIC_SERIAL_PROTOCOLPGM("J06"); //Status: Nozzle heating
      ANYCUBIC_SERIAL_ENTER();
    }
    DEBUG_ECHOLNPAIR("TFT Serial Debug: statusChanged: ", msg);
  }
}

void AnycubicTFTClass::SDCardInserted()
{
  if(ExtUI::isMediaInserted())
  {
    ANYCUBIC_SERIAL_PROTOCOLPGM("J00"); // J00 SD Card inserted
    ANYCUBIC_SERIAL_ENTER();
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("TFT Serial Debug: SD card inserted... J00");
  }
}
void AnycubicTFTClass::SDCardRemoved()
{
  if(!ExtUI::isMediaInserted())
  {
    ANYCUBIC_SERIAL_PROTOCOLPGM("J01"); // J01 SD Card removed
    ANYCUBIC_SERIAL_ENTER();
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("TFT Serial Debug: SD card removed... J01");
  }
}

void AnycubicTFTClass::onPidTuning(const ExtUI::result_t rst)
{
  #if HAS_PID_HEATING
    playSuccessTone();
  #endif
}

} // ExtUI namespace close

#endif // EXTUI_EXAMPLE && EXTENSIBLE_UI