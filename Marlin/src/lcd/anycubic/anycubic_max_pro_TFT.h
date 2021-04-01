/*
 AnycubicTFT.h  --- Support for Anycubic i3 Mega TFT
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

#pragma once

#include <stdio.h>
#include "../../inc/MarlinConfig.h"

#if BOTH(ANYCUBIC_SOFT_TFT_MODEL_OLD, EXTENSIBLE_UI)
#if HAS_BUZZER
  #include "../../libs/buzzer.h"
#endif

#define ANYCUBIC_TFT_STATE_IDLE           0  // Printer Not doing anything / connected to USB
#define ANYCUBIC_TFT_STATE_SDPRINT        1  // media priting
#define ANYCUBIC_TFT_STATE_SDPAUSE_REQ    3  // paused by user (explicit using TFT, can resume from TFT)
#if HAS_FILAMENT_SENSOR
#define ANYCUBIC_TFT_STATE_SDPAUSE_OOF    4  // Out of filament pause (via TFT user cannot resume, user has to go through default marlin steps to resume print)
#endif
#if ENABLED(ADVANCED_PAUSE_FEATURE)
#define ANYCUBIC_TFT_STATE_SDPAUSE_ADV    5  // filament change pause / paused by gcode (via TFT user cannot resume, user has to go through default marlin steps to resume print)
#else
#define ANYCUBIC_TFT_STATE_SDPAUSE        2  // paused by gcode (can resume from TFT)
#endif
#define ANYCUBIC_TFT_STATE_SDSTOP         6  // stopped / aborted by user (explicit using TFT)

#define ANYCUBIC_TFT_LOAD_FILAMENT_MSG              "Load Filament"
#define ANYCUBIC_TFT_PURGE_FILAMENT_MSG             "Filament Purge Running..."
#define ANYCUBIC_TFT_NOZZLE_PARKED_MSG              "Nozzle Parked"
#define ANYCUBIC_TFT_HEATER_TIMEOUT_MSG             "HeaterTimeout"
#define ANYCUBIC_TFT_REHEAT_FINISHED_MSG            "Reheat finished."
#define ANYCUBIC_TFT_M43_WAIT_MSG                   "M43 Wait Called"
#define ANYCUBIC_TFT_USER_WAIT_MSG                   Language::MSG_USERWAIT

//#define ANYCUBIC_TFT_SPECIAL_MENU(x)                (AnycubicSerial.AnycubicSerialprintPGM(PSTR(x "\r\n")),AnycubicSerial.AnycubicSerialprintPGM(PSTR(x "\r\n")))
#define ANYCUBIC_TFT_SPECIAL_MENU(x)                (AnycubicSerial.AnycubicSerialprintPGM(x),AnycubicSerial.AnycubicSerialprintPGM(PSTR("\r\n")),AnycubicSerial.AnycubicSerialprintPGM(x), AnycubicSerial.AnycubicSerialprintPGM(PSTR("\r\n")))

#define MSG_MY_VERSION                "V116"

#define ANYCUBIC_TFT_BUF_SIZE              4
#define ANYCUBIC_TFT_MAX_CMD_SIZE         96
#define ANYCUBIC_TFT_PAGE_SIZE             4 // TFT Lists 4 items per page
#define ANYCUBIC_TFT_PAGE_ITEM_START       4 // starting character of TFT selection list item in buffer
#define ANYCUBIC_MAX_SELECTED_DIR_LEN     30

#ifdef ANYCUBIC_TFT_DEBUG
  #define DEBUG_PRINT_PAIR(V...)            _SEP_N(NUM_ARGS(V),V)
  #define DEBUG_PRINT(x)                    SERIAL_ECHO(x)
  #define DEBUG_PRINT_PGM(x)                SERIAL_ECHO_P(x)
  #define DEBUG_PRINT_EOL()                 SERIAL_EOL()
  #define DEBUG_PRINT_LNPGM(x)              SERIAL_ECHOLNPGM(x)
  #define DEBUG_PRINT_LNP(x)                (serialprintPGM(x),serialprintPGM(PSTR("\r\n")))
#else
  #define DEBUG_PRINT_PAIR(V...)
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINT_PGM(x)
  #define DEBUG_PRINT_EOL()
  #define DEBUG_PRINT_LNPGM(x)
  #define DEBUG_PRINT_LNP(x)
#endif

#define ENQUEUE_CMD_NOW_PGM(x)              queue.enqueue_now_P(PSTR(x))
#define ENQUEUE_CMD_NOW(x)                  queue.enqueue_one_now(x)
#define INJECT_CMD_NOW_PGM(x)               queue.inject_P(PSTR(x))
#define ENQUEUE_CMD_PGM(x)                  queue.enqueue_one_P(PSTR(x))

#define DIGIT(n) ('0' + (n))
#define DIGIMOD(n, f) DIGIT((n)/(f) % 10)
#define RJDIGIT(n, f) ((n) >= (f) ? DIGIMOD(n, f) : ' ')
#define MINUSOR(n, alt) (n >= 0 ? (alt) : (n = -n, '-'))

namespace ExtUI {

class AnycubicTFTClass {

public:

  AnycubicTFTClass();
  void Setup();
  void Idle();
  void CommandScan();
  void KillTFT();
  void SDCardInserted();
  void SDCardRemoved();
  void PlayTone(const uint16_t frequency, const uint16_t duration);
  void ConfirmUser(const char * const msg);

  void onPrintTimerStarted();
  void onPrintTimerPaused();
  void onPrintTimerStopped();

#if defined(FILAMENT_RUNOUT_SENSOR)
  void FilamentRunout();
#endif

#ifdef ANYCUBIC_ENDSTOP_BEEP
  void EndstopBeep();
#endif

private:

  char _TFTState=ANYCUBIC_TFT_STATE_IDLE;
  char  _TFTcmdbuffer[ANYCUBIC_TFT_BUF_SIZE][ANYCUBIC_TFT_MAX_CMD_SIZE];
  int   _TFTbuflen=0;
  int   _TFTbufindr = 0;
  int   _TFTbufindw = 0;
  char  _serial3_char;
  int   _serial3_count = 0;
  char *_TFTstrchr_pointer;
#ifdef ANYCUBIC_TFT_COMMENT_ENABLED
  uint8_t _TFTcomment_mode = false;
#endif

  uint64_t _starttime=0;
  uint64_t _stoptime=0;

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

  void WriteOutageEEPromData();
  void ReadOutageEEPromData();

  float CodeValue();
  uint8_t CodeSeen(char);

  void StartPrint();
  void PausePrint();
  void StopPrint();
  void OnPrintFinished();

  uint8_t isSDPrintingInProgress();
  uint8_t isPrinterIdle();

  void StateHandler();
  void GetCommandFromTFT();

  uint16_t _heaterCheckCount=0;
  void CheckHeaterError();

  void CheckHeatingOn();

#ifdef ANYCUBIC_SD_CHECK
  uint8_t _lastSDstatus;
  void CheckSDCardChange();
#endif

#ifdef ANYCUBIC_TFT_DEBUG_MSG
  uint8_t _lastMsgId=0xFF;
  uint8_t _showMsg = false;
#endif

  void BedHeatingStart();
  void BedHeatingDone();
  void HeatingDone();
  void HeatingStart();

  char      _selectedDirectory[ANYCUBIC_MAX_SELECTED_DIR_LEN];
  uint8_t   _isSpecialMenuActive=false;
  uint8_t   _isRecursiveCall=false;
  uint16_t  _lastStartIdx=0;
  uint8_t   _isLastUserActionRefresh=false;
  void BuildListItems(uint16_t  showPageNo);
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
  void mybeep(int beepP,int beepS);
#endif

#ifdef ANYCUBIC_POWER_LOSS_RECOVERY
  uint8_t _PowerTestFlag = false;
  int PowerInt= 6;//
  void SimulatePowerOuttage();
  void setup_OutageTestPin();
  void PowerKill();
#endif

#ifdef USB_ONLINE_DETECT
  // To Add this support we ned to modify the "gcode_M105" function, "if(!card.sdprinting)UsbOnLineFlag=true; // if receive m105 from pc,meant usb on line"
  uint8_t _UsbOnLineFlag=false;
  uint8_t _USBConnectFlag=false;
  void USBOnLineTest();
#endif

};

}

extern ExtUI::AnycubicTFTClass AnycubicTFT;

#endif