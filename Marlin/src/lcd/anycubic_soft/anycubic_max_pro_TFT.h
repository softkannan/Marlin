
#pragma once

#include "../../inc/MarlinConfigPre.h"

#if BOTH(ANYCUBIC_SOFT_TFT_MODEL, EXTENSIBLE_UI)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Arduino.h"
#include <inttypes.h>

#include "../extui/ui_api.h"
#include "../../inc/MarlinConfig.h"

#include "../../core/language.h"
#include "../../core/macros.h"
#include "../../core/serial.h"
#include "../../gcode/queue.h"
//#include "../../feature/emergency_parser.h"
#include "../../feature/pause.h"
#include "../../libs/buzzer.h"
#include "../../module/planner.h"
#include "../../module/printcounter.h"
#include "../../module/stepper.h"
#include "../../module/temperature.h"
#include "../../sd/cardreader.h"

#if HAS_BUZZER
  #include "../../libs/buzzer.h"
#endif

#ifdef FILAMENT_RUNOUT_SENSOR
#include "../../feature/runout.h"
#endif

#ifdef ANYCUBIC_TFT_DEBUG
#include "../../core/debug_out.h"
#endif

#if defined(__AVR__)
 #define STRCASECMP_P(A,B)  strcasecmp_P(A,B)
#elif defined(ANYCUBIC_GENERIC_SERIAL) && defined(TARGET_LPC1768)
  #include <string.h>
  #define STRCASECMP_P(A,B)  strcasecmp(A,B)
#else
   #error "anycubic TFT is not supported given target processor."
#endif

#include "anycubic_commands.h"

//#define ANYCUBIC_TFT_SPECIAL_MENU(x)                (AnycubicSerial.AnycubicSerialprintPGM(PSTR(x "\r\n")),AnycubicSerial.AnycubicSerialprintPGM(PSTR(x "\r\n")))
#define ANYCUBIC_TFT_SPECIAL_MENU(x)                (AnycubicSerial.AnycubicSerialprintPGM(x),AnycubicSerial.AnycubicSerialprintPGM(PSTR("\r\n")),AnycubicSerial.AnycubicSerialprintPGM(x), AnycubicSerial.AnycubicSerialprintPGM(PSTR("\r\n")))

#define MSG_MY_VERSION                "V116"

#define PREHEAT_PLA_TEMP_HOTEND 210
#define PREHEAT_PLA_TEMP_BED 60

#define PREHEAT_ABS_TEMP_HOTEND 240
#define PREHEAT_ABS_TEMP_BED 80

namespace ExtUI {

class AnycubicTFTClass {

public:

  AnycubicTFTClass();
  void Setup();
  void Idle();

private:
  void IdleInternal();
  void CommandScan();
  void HandleTFTCommands(int16_t a_command);
  void GetCommandFromTFT();

public:
  void KillTFT();
  void SDCardInserted();
  void SDCardRemoved();

  void onStatusChanged(const char * const msg);
  void onUserConfirmRequired(const char * const msg);
  void onPidTuning(const result_t rst);

  void onPrintTimerStarted();
  void onPrintTimerPaused();
  void onPrintTimerStopped();

#if defined(FILAMENT_RUNOUT_SENSOR)
  void FilamentRunout();
#endif



private:

  char  _TFTcmdbuffer[ANYCUBIC_TFT_BUF_SIZE][ANYCUBIC_TFT_MAX_CMD_SIZE];
  int   _TFTbuflen = 0;
  int   _TFTbufindr = 0;
  int   _TFTbufindw = 0;
  char  _serial3_char = 0;
  int   _serial3_count = 0;
  char *_TFTstrchr_pointer = NULL;

#if TEMP_SENSOR_CHAMBER
  uint8_t _bedOrChamber = false;
  uint8_t _bedTargetTemp = 0;
#endif

#if defined(FILAMENT_RUNOUT_SENSOR)
#endif

#ifdef POWER_LOSS_RECOVERY
  uint8_t _FlagResumFromOutage=0;
  struct OutageDataStruct {
    uint8_t OutageDataVersion;
    uint8_t OutageFlag;
    float last_position[XYZE];
    float last_bed_temp;
    float last_hotend_temp;
    long lastSDposition;
  } _OutageData;
#endif

  char _conv[8];

  char *itostr2(const uint8_t &x)
  {
    //sprintf(conv,"%5.1f",x);
    int xx=x;
    _conv[0]=(xx/10)%10+'0';
    _conv[1]=(xx)%10+'0';
    _conv[2]=0;
    return _conv;
  }

  char* itostr3(const int x) {
    int xx = x;
    _conv[4] = MINUSOR(xx, RJDIGIT(xx, 100));
    _conv[5] = RJDIGIT(xx, 10);
    _conv[6] = DIGIMOD(xx, 1);
    return &_conv[4];
  }

  // Convert signed float to fixed-length string with 023.45 / -23.45 format
  char *ftostr32(const float &x) {
    long xx = x * 100;
    _conv[1] = MINUSOR(xx, DIGIMOD(xx, 10000));
    _conv[2] = DIGIMOD(xx, 1000);
    _conv[3] = DIGIMOD(xx, 100);
    _conv[4] = '.';
    _conv[5] = DIGIMOD(xx, 10);
    _conv[6] = DIGIMOD(xx, 1);
    return &_conv[1];
  }

  float CodeValue();
  uint8_t CodeSeen(char);


  void StartSDPrint();
  void ResumeSDPrint();
  void ResumePrintFromOutage();
  void PauseSDPrint();
  void StopSDPrint();
  uint8_t isWaitingForUserConfirm();
  void waitForUserConfirm();


  uint16_t _heaterCheckCount=0;
  void CheckHeaterError();

public:
  void PlayTone(const uint16_t duration,const uint16_t frequency);
private:
  void playInfoTone();
  void playSuccessTone();
  void playErrorTone();

#ifdef ANYCUBIC_TFT_DEBUG_MSG
  uint8_t _lastMsgId=0xFF;
  uint8_t _showMsg = false;
#endif

  void BedHeatingStart();
  void BedHeatingDone();
  void HeatingDone();
  void HeatingStart();

  union __attribute__((packed))
  {
      uint8_t   status;
      struct __attribute__((packed))
      {
        uint8_t RequestStayOnPage:1;         // 0000 0001
        uint8_t ShowLastPage:1;              // 0000 0010
        uint8_t SpecialMenuActive:1;         // 0000 0100
        uint8_t RecursiveCall:1;             // 0000 1000
        uint8_t RequestExecuteCmd:1;         // 0001 0000
        uint8_t Reserved3:1;                 // 0010 0000
        uint8_t Reserved4:1;                 // 0100 0000
        uint8_t Reserved5:1;                 // 1000 0000
      } flag;
  }m_mStatus;

  #define REQUEST_EXEC_CMD() m_mStatus.flag.RequestExecuteCmd = 1
  #define EXEC_CMD_COMPLETED() m_mStatus.flag.RequestExecuteCmd = 0
  #define IS_EXEC_CMD() m_mStatus.flag.RequestExecuteCmd

  #define STAY_ON_PAGE()  m_mStatus.flag.RequestStayOnPage = 1, m_mStatus.flag.ShowLastPage = 1
  #define STAY_ON_PAGE_RESET()  m_mStatus.flag.RequestStayOnPage = 0, m_mStatus.flag.ShowLastPage = 0
  #define IS_STAY_ON_PAGE()  m_mStatus.flag.RequestStayOnPage && m_mStatus.flag.ShowLastPage
  #define STAY_ON_PAGE_SHOWN() m_mStatus.flag.ShowLastPage = 0
  #define STAY_ON_PAGE_SHOW_AGAIN() m_mStatus.flag.ShowLastPage = 1

  #define ENTER_SPECIAL_MENU() m_mStatus.flag.SpecialMenuActive = 1
  #define EXIT_SPECIAL_MENU() m_mStatus.flag.SpecialMenuActive = 0
  #define IS_SPECIAL_MENU() m_mStatus.flag.SpecialMenuActive

  char      _lastUserSelection[ANYCUBIC_MAX_SELECTED_DIR_LEN];
  char _selectedFilename[LONG_FILENAME_LENGTH];
  char _selectedDirectory[LONG_FILENAME_LENGTH];
  uint16_t  _lastShownPageIdx=0;

  void BuildFileListItems(uint16_t  showPageNo);
  void HandleSpecialMenu();

#ifdef ANYCUBIC_POWERDOWN
  void setup_PowerDownPin();
  void PowerDown();
  void PowerDownScan();
  uint8_t _PrintdoneAndPowerOFF = false;
  uint8_t _powerOFFflag= false;
#endif

#ifdef ANYCUBIC_FILAMENT_RUNOUT
  uint8_t   _filament_runout_last_status = 0;
  uint16_t _filament_runout_counter = 0;
  void setup_FilamentRunout();
  void FilamentRunoutScan();
#endif

#ifdef ANYCUBIC_ENDSTOP_BEEP
  void EndstopBeep();
#endif

};

}

extern ExtUI::AnycubicTFTClass AnycubicTFT;

#endif