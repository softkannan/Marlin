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

#if BOTH(ANYCUBIC_TFT_MODEL, EXTENSIBLE_UI)

#include "../extensible_ui/ui_api.h"

#include "Arduino.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../core/language.h"
#include "../../core/macros.h"
#include "../../core/serial.h"
#include "../../gcode/queue.h"
#include "../../feature/emergency_parser.h"
#include "../../feature/pause.h"
#include "../../inc/MarlinConfig.h"
#include "../../libs/buzzer.h"
#include "../../module/planner.h"
#include "../../module/printcounter.h"
#include "../../module/stepper.h"
#include "../../module/temperature.h"
#include "../../sd/cardreader.h"

#ifdef FILAMENT_RUNOUT_SENSOR
#include "../../feature/runout.h"
#endif

#ifdef ANYCUBIC_TFT_DEBUG
#include "../../core/debug_out.h"
#endif

#if ENABLED(SDSUPPORT)
  #define IFSD(A,B) (A)
#else
  #define IFSD(A,B) (B)
#endif

#if HAS_FILAMENT_SENSOR
  #define IFOOF(A,B) (A)
#else
  #define IFOOF(A,B) (B)
#endif

#if ENABLED(ADVANCED_PAUSE_FEATURE)
  #define IFADVPAUSE(A,B) (A)
#else
  #define IFADVPAUSE(A,B) (B)
#endif

#include "anycubic_max_pro_TFT.h"
#if ALL(ANYCUBIC_TFT_MODEL, EXTENSIBLE_UI, ANYCUBIC_GENERIC_SERIAL)
#include "anycubic_generic_serial.h"
extern AnycubicSerialClass AnycubicSerial;
#elif ALL(ANYCUBIC_TFT_MODEL, EXTENSIBLE_UI, ANYCUBIC_TFT_SERIAL)
#include "anycubic_serial.h"
extern AnycubicSerialClass AnycubicSerial;
#endif
#include "anycubic_music.h"

#if defined(__AVR__)
 #define STRCASECMP_P(A,B)  strcasecmp_P(A,B)
#elif defined(ANYCUBIC_GENERIC_SERIAL) && defined(TARGET_LPC1768)
  #include <string.h>
  #define STRCASECMP_P(A,B)  strcasecmp(A,B)
#else
   #error "anycubic TFT is not supported given target processor."
#endif


void (*softwareReset) (void)=0;

const char MENU_SPECIAL_MENU[] PROGMEM = "<Special_Menu>"; // while print is inprogress user cannot access special menu
const char MENU_SPECIAL_LEVEL_UP[] PROGMEM = "/..";
const char MENU_EXIT[] PROGMEM = "<Exit>";

const char MENU_MOVE_TO_L1[] PROGMEM = "<Move To L1>";
const char MENU_MOVE_TO_L2[] PROGMEM = "<Move To L2>";
const char MENU_MOVE_TO_R1[] PROGMEM = "<Move To R1>";
const char MENU_MOVE_TO_R2[] PROGMEM = "<Move To R2>";
const char MENU_MOVE_TO_CENTER[] PROGMEM = "<Move To Center>";
const char MENU_MOVE_TO_BOTTOM[] PROGMEM = "<Move Z To Bottom>";

#if ENABLED(MESH_BED_LEVELING) && ENABLED(PROBE_MANUALLY)
const char MENU_START_MESH_LEVELING[] PROGMEM = "<Start Mesh Leveling>";
const char MENU_NEXT_MESH_POINT[] PROGMEM = "<Next Mesh Point>";
#endif

#ifdef PREVENT_COLD_EXTRUSION
const char MENU_DISABLE_COLD_EXTRUSION[] PROGMEM = "<Disable Cold Extrusion>";
const char MENU_ENABLE_COLD_EXTRUSION[] PROGMEM = "<Enable Cold Extrusion>";
#endif

const char MENU_TUNE_HOTEND_PID[] PROGMEM = "<Tune Hotend PID>";
const char MENU_TUNE_BED_PID[] PROGMEM = "<Tune Bed PID>";
const char MENU_TUNE_ESTEP_CAL_PREP[] PROGMEM = "<Prep E-Step Cal>";
const char MENU_TUNE_ESTEP_CAL_PERFORM[] PROGMEM = "<Perform E-Step Cal>";
const char MENU_COLD_FILAMENT_LOAD[] PROGMEM = "<Cold Filament Load>";
const char MENU_COLD_FILAMENT_UNLOAD[] PROGMEM = "<Cold Filament Unload>";

const char MENU_RESTORE_SETTINGS[] PROGMEM = "<Restore Settings>";
const char MENU_SAVE_SETTINGS[] PROGMEM = "<Save Settings>";
const char MENU_FACTORY_RESET[] PROGMEM = "<Factory Reset>";

const char MENU_DRYRUN[] PROGMEM = "<DryRun>";
#ifdef ANYCUBIC_TFT_DEBUG_MSG
const char MENU_NEXT_MSG[] PROGMEM = "<Next Msg>";
#endif

const char * const menu_table[] PROGMEM =	   
{
   MENU_MOVE_TO_L1
  ,MENU_MOVE_TO_L2
  ,MENU_MOVE_TO_R1
  ,MENU_MOVE_TO_R2
  ,MENU_MOVE_TO_CENTER
  ,MENU_MOVE_TO_BOTTOM

#if ENABLED(MESH_BED_LEVELING) && ENABLED(PROBE_MANUALLY)
  ,MENU_START_MESH_LEVELING
  ,MENU_NEXT_MESH_POINT
#endif

  ,MENU_COLD_FILAMENT_LOAD
  ,MENU_COLD_FILAMENT_UNLOAD

#ifdef PREVENT_COLD_EXTRUSION
  ,MENU_DISABLE_COLD_EXTRUSION
  ,MENU_ENABLE_COLD_EXTRUSION
#endif

  ,MENU_TUNE_HOTEND_PID
  ,MENU_TUNE_BED_PID
  ,MENU_TUNE_ESTEP_CAL_PREP
  ,MENU_TUNE_ESTEP_CAL_PERFORM
  ,MENU_RESTORE_SETTINGS
  ,MENU_SAVE_SETTINGS
  ,MENU_FACTORY_RESET
  ,MENU_DRYRUN
  #ifdef ANYCUBIC_TFT_DEBUG_MSG
  ,MENU_NEXT_MSG
  #endif
  ,MENU_EXIT
};

namespace ExtUI {

AnycubicTFTClass::AnycubicTFTClass() {
}

void AnycubicTFTClass::Setup() {

#ifdef ANYCUBIC_MUSIC
  PowerOnMusic();
#endif

  AnycubicSerial.begin(115200);
  //ANYCUBIC_SERIAL_START();
  ANYCUBIC_SERIAL_PROTOCOLPGM("J17"); // J17 Main board reset
  ANYCUBIC_SERIAL_ENTER();
  delay(10);
  ANYCUBIC_SERIAL_PROTOCOLPGM("J12"); // J12 Ready
  ANYCUBIC_SERIAL_ENTER();

#ifdef ANYCUBIC_FILAMENT_RUNOUT
  setup_FilamentRunout();
#endif

#ifdef ANYCUBIC_POWERDOWN
  setup_PowerDownPin();
#endif

#ifdef ANYCUBIC_POWER_LOSS_RECOVERY
  setup_OutageTestPin();
#endif

  _selectedDirectory[0]=0;
  _isSpecialMenuActive=false;

  // Print optional message
  //SERIAL_ECHOPGM(MSG_MARLIN_AI3M);
  //SERIAL_CHAR(' ');
  //DEBUG_PRINT_LINE(CUSTOM_BUILD_VERSION);
  //DEBUG_PRINT_EOL();
  
#ifdef ANYCUBIC_STARTUP_CHIME
    buzzer.tone(250, 554); // C#5
    buzzer.tone(250, 740); // F#5
    buzzer.tone(250, 554); // C#5
    buzzer.tone(500, 831); // G#5
#endif
}

void AnycubicTFTClass::Idle() {

  if(_isRecursiveCall) { return; }

  _isRecursiveCall = true;

  CommandScan();

  #ifdef ANYCUBIC_ENDSTOP_BEEP
    EndstopBeep();
  #endif

  #ifdef ANYCUBIC_POWERDOWN  
    PowerDownScan();
  #endif

  #ifdef ANYCUBIC_FILAMENT_RUNOUT
    FilamentRunoutScan();
  #endif

  _isRecursiveCall = false;

}

void AnycubicTFTClass::WriteOutageEEPromData() {
  //int pos=E2END-256;

}

void AnycubicTFTClass::ReadOutageEEPromData() {
  //int pos=E2END-256;

}

void AnycubicTFTClass::KillTFT()
{
  ANYCUBIC_SERIAL_PROTOCOLPGM("J11"); // J11 Kill
  ANYCUBIC_SERIAL_ENTER();
  DEBUG_PRINT_LNPGM("TFT Serial Debug: Kill command... J11");
}

  void AnycubicTFTClass::onPrintTimerStarted()
  {
    _starttime=safe_millis();
  }
  void AnycubicTFTClass::onPrintTimerPaused()
  {

  }
  void AnycubicTFTClass::onPrintTimerStopped()
  {

  }

void AnycubicTFTClass::StartPrint() {
  _TFTState=ANYCUBIC_TFT_STATE_SDPRINT;
  if (!card.isFileOpen()) {
    ANYCUBIC_SERIAL_PROTOCOLPGM("J21"); // J21 Open failed
    ANYCUBIC_SERIAL_ENTER();
    DEBUG_PRINT_LNPGM("TFT Serial Debug: File open failed... J21");
  } 
  resumePrint();
  ANYCUBIC_SERIAL_CMD_SENDPGM("J04");// J04 printing form sd card now
  DEBUG_PRINT_LNPGM("DEBUG: Regular Start");
}
// Called by 
void AnycubicTFTClass::PausePrint() {
  _TFTState=ANYCUBIC_TFT_STATE_SDPAUSE_REQ;
  pausePrint(); // pause print regularly
  ANYCUBIC_SERIAL_CMD_SENDPGM("J18"); //J18 Pause success
  DEBUG_PRINT_LNPGM("DEBUG: Regular Pause");
}

void AnycubicTFTClass::StopPrint(){
  _TFTState=ANYCUBIC_TFT_STATE_SDSTOP;
  stopPrint();
  ANYCUBIC_SERIAL_CMD_SENDPGM("J16");// J16 stop print
  DEBUG_PRINT_LNPGM("DEBUG: Stopped and cleared");
}

inline uint8_t AnycubicTFTClass::isSDPrintingInProgress()
{
  return IFSD(card.flag.sdprinting, false);
}
//inline uint8_t AnycubicTFTClass::isSDPrintPaused()
//{
//  return IFSD(card.isPaused(),false);
//}
inline uint8_t AnycubicTFTClass::isPrinterIdle()
{
  return !commandsInQueue();
}

float AnycubicTFTClass::CodeValue()
{
  return (strtod(&_TFTcmdbuffer[_TFTbufindr][_TFTstrchr_pointer - _TFTcmdbuffer[_TFTbufindr] + 1], NULL));
}

uint8_t AnycubicTFTClass::CodeSeen(char code)
{
  _TFTstrchr_pointer = strchr(_TFTcmdbuffer[_TFTbufindr], code);
  return (_TFTstrchr_pointer != NULL); //Return True if a character was found
}

// Called when user touches refresh button
void AnycubicTFTClass::HandleSpecialMenu()
{
  if(STRCASECMP_P(_selectedDirectory, MENU_SPECIAL_MENU)==0) {
    _isSpecialMenuActive=true;
    _isLastUserActionRefresh = false;
  } 
  else if (STRCASECMP_P(_selectedDirectory,MENU_EXIT)==0) {
    _isSpecialMenuActive=false;
    _isLastUserActionRefresh = false;
  } 
  else if (STRCASECMP_P(_selectedDirectory, MENU_DRYRUN)==0) {
    _isSpecialMenuActive=false;
    _isLastUserActionRefresh = false;
    DEBUG_PRINT_LNPGM("Special Menu:DryRun");
    ENQUEUE_CMD_NOW_PGM("M111 S8");
  } 
  else if (STRCASECMP_P(_selectedDirectory,MENU_TUNE_HOTEND_PID)==0) {
    _isLastUserActionRefresh = true;
    DEBUG_PRINT_LNPGM("Special Menu: Auto Tune Hotend PID");
    INJECT_CMD_NOW_PGM("M303 E0 S240 C15 U1");
  } 
  else if (STRCASECMP_P(_selectedDirectory, MENU_TUNE_BED_PID)==0) {
    DEBUG_PRINT_LNPGM("Special Menu: Auto Tune Hotbed Pid");
    INJECT_CMD_NOW_PGM("M303 E-1 S75 C5 U1");
    _isLastUserActionRefresh = true;
  } 
  else if (STRCASECMP_P(_selectedDirectory, MENU_TUNE_ESTEP_CAL_PREP)==0) {
    DEBUG_PRINT_LNPGM("Special Menu: EStep Cal Prep");
    INJECT_CMD_NOW_PGM("G21\nG90\nM83\nG1 X130 Y100 Z190 F3600\nG1 E50 F600");
    _isLastUserActionRefresh = true;
  } 
  else if (STRCASECMP_P(_selectedDirectory, MENU_TUNE_ESTEP_CAL_PERFORM)==0) {
    DEBUG_PRINT_LNPGM("Special Menu: EStep Call Perform");
    INJECT_CMD_NOW_PGM("G21\nG90\nM83\nG1 X130 Y100 Z190 F3600\nG1 E100 F100");
    _isLastUserActionRefresh = true;
  }
  else if (STRCASECMP_P(_selectedDirectory, MENU_SAVE_SETTINGS)==0) {
    DEBUG_PRINT_LNPGM("Special Menu: Save Settings");
    INJECT_CMD_NOW_PGM("M500");
#if USE_BEEPER
    buzzer.tone(105, 1108);
    buzzer.tone(210, 1661);
#endif
    _isLastUserActionRefresh = true;
  } 
  else if (STRCASECMP_P(_selectedDirectory, MENU_RESTORE_SETTINGS)==0) {
    DEBUG_PRINT_LNPGM("Special Menu: Restore Settings");
    INJECT_CMD_NOW_PGM("M501");
#if USE_BEEPER
    buzzer.tone(105, 1661);
    buzzer.tone(210, 1108);
#endif
    _isLastUserActionRefresh = true;
  }
  else if (STRCASECMP_P(_selectedDirectory, MENU_FACTORY_RESET)==0) {
    DEBUG_PRINT_LNPGM("Special Menu: Factory Reset");
    INJECT_CMD_NOW_PGM("M502");
#if USE_BEEPER
    buzzer.tone(105, 1661);
    buzzer.tone(210, 1108);
#endif
    _isLastUserActionRefresh = true;
  }
  else if (STRCASECMP_P(_selectedDirectory, MENU_COLD_FILAMENT_LOAD)==0) {
    DEBUG_PRINT_LNPGM("Special Menu: Cold Filament Load");
    INJECT_CMD_NOW_PGM("M302 P1\nM83\nG1 E5 F100\nM302 P0");
    _isLastUserActionRefresh = true;
  }
  else if (STRCASECMP_P(_selectedDirectory, MENU_COLD_FILAMENT_UNLOAD)==0) {
    DEBUG_PRINT_LNPGM("Special Menu: Cold Filament Unload");
    INJECT_CMD_NOW_PGM("M302 P1\nM83\nG1 E-5 F100\nM302 P0");
    _isLastUserActionRefresh = true;
  }
#ifdef PREVENT_COLD_EXTRUSION
  else if (STRCASECMP_P(_selectedDirectory, MENU_DISABLE_COLD_EXTRUSION)==0) {
    DEBUG_PRINT_LNPGM("Special Menu: Disable Cold Extrusion");
    INJECT_CMD_NOW_PGM("M302 P1");
    _isLastUserActionRefresh = true;
  }
  else if (STRCASECMP_P(_selectedDirectory, MENU_ENABLE_COLD_EXTRUSION)==0) {
    DEBUG_PRINT_LNPGM("Special Menu: Enable Cold Extrusion");
    INJECT_CMD_NOW_PGM("M302 P0");
    _isLastUserActionRefresh = true;
  }
#endif 

#if ENABLED(MESH_BED_LEVELING) && ENABLED(PROBE_MANUALLY)
  else if (STRCASECMP_P(_selectedDirectory, MENU_START_MESH_LEVELING)==0) {
    DEBUG_PRINT_LNPGM("Special Menu: Start Mesh Leveling");
    INJECT_NOW_PGM("G29 S1");
    _isLastUserActionRefresh = true;
  }
  else if (STRCASECMP_P(_selectedDirectory, MENU_NEXT_MESH_POINT)==0) {
    DEBUG_PRINT_LNPGM("Special Menu: Next Mesh Point");
    INJECT_NOW_PGM("G29 S2");
    _isLastUserActionRefresh = true;
  } 
#endif

  else if (STRCASECMP_P(_selectedDirectory, MENU_MOVE_TO_L1)==0) {
    DEBUG_PRINT_LNPGM("Special Menu: Move To L1");
    INJECT_CMD_NOW_PGM("G21\nG90\nG1 F9000\nG1 Z10\nG1 X20 Y20\nG1 F100 Z0\nG1 F9000");
    _isLastUserActionRefresh = true;
  } 
  else if (STRCASECMP_P(_selectedDirectory, MENU_MOVE_TO_L2)==0) {
    DEBUG_PRINT_LNPGM("Special Menu: Move To L2");
    INJECT_CMD_NOW_PGM("G21\nG90\nG1 F9000\nG1 Z10\nG1 X20 Y195\nG1 F100 Z0\nG1 F9000");
    _isLastUserActionRefresh = true;
  } 
  else if (STRCASECMP_P(_selectedDirectory, MENU_MOVE_TO_R1)==0) {
    DEBUG_PRINT_LNPGM("Special Menu: Move To R1");
    INJECT_CMD_NOW_PGM("G21\nG90\nG1 F9000\nG1 Z10\nG1 X252 Y10\nG1 F100 Z0\nG1 F9000");
    _isLastUserActionRefresh = true;
  } 
  else if (STRCASECMP_P(_selectedDirectory, MENU_MOVE_TO_R2)==0) {
    DEBUG_PRINT_LNPGM("Special Menu: Move To R2");
    INJECT_CMD_NOW_PGM("G21\nG90\nG1 F9000\nG1 Z10\nG1 X252 Y195\nG1 F100 Z0\nG1 F9000");
    _isLastUserActionRefresh = true;
  } 
  else if (STRCASECMP_P(_selectedDirectory, MENU_MOVE_TO_CENTER)==0) {
    DEBUG_PRINT_LNPGM("Special Menu: Move To Center");
    INJECT_CMD_NOW_PGM("G21\nG90\nG1 F9000\nG1 Z10\nG1 X136 Y107\nG1 F100 Z0\nG1 F9000");
    _isLastUserActionRefresh = true;
  }
  else if (STRCASECMP_P(_selectedDirectory, MENU_MOVE_TO_BOTTOM)==0) {
    DEBUG_PRINT_LNPGM("Special Menu: Move to Z Bottom");
    INJECT_CMD_NOW_PGM("G21\nG90\nG1 F9000\nG1 Z190 F9000");
    _isLastUserActionRefresh = true;
  }
#ifdef ANYCUBIC_TFT_DEBUG_MSG
  else if (STRCASECMP_P(_selectedDirectory, MENU_NEXT_MSG)==0) {
    DEBUG_PRINT_LNPGM("Special Menu: Move To Center");
    _lastMsgId++;
    _showMsg=true;
    _isLastUserActionRefresh = true;
  }
#endif
}

void AnycubicTFTClass::BuildListItems(uint16_t  startIdx)
{
  if (_isSpecialMenuActive) {
    if(_isLastUserActionRefresh) { startIdx=_lastStartIdx; }
    else { _lastStartIdx=startIdx; } 

    uint16_t idx=startIdx;
    uint16_t max_files;
    uint16_t dir_files= sizeof(menu_table) / sizeof(char*);

    // clip number of display items to ANYCUBIC_TFT_PAGE_SIZE
    if(dir_files < ANYCUBIC_TFT_PAGE_SIZE) { max_files=dir_files; } 
    else {
      max_files=startIdx + ANYCUBIC_TFT_PAGE_SIZE;
      if(max_files > dir_files) { max_files=dir_files; }
    }

    for(idx=startIdx; idx < max_files; idx++)
    {
        //display file names
        ANYCUBIC_TFT_SPECIAL_MENU((char*)pgm_read_ptr(&(menu_table[idx])));
        DEBUG_PRINT(idx);
        DEBUG_PRINT_LNP((char*)pgm_read_ptr(&(menu_table[idx])));
    }
  }
#ifdef SDSUPPORT
  else if(card.isMounted() && !isSDPrintingInProgress())
  {
    _lastStartIdx = 0;
    uint16_t idx=startIdx;
    uint16_t max_files;
    uint16_t dir_files=card.countFilesInWorkDir();

    // clip number of display items to ANYCUBIC_TFT_PAGE_SIZE
    if(dir_files < ANYCUBIC_TFT_PAGE_SIZE) { max_files=dir_files + 1; } 
    else {
      max_files=startIdx + ANYCUBIC_TFT_PAGE_SIZE;
      if(max_files > dir_files) { max_files=dir_files + 1; }
    }

    for(idx=startIdx; idx < max_files; idx++)
    {
      if (idx==0) // Special Entry
      {
        if(strcmp(card.getWorkDirName(), "/") == 0) {
          // Only display special menu on root level
          ANYCUBIC_TFT_SPECIAL_MENU(MENU_SPECIAL_MENU);
          DEBUG_PRINT(idx);
          DEBUG_PRINT_LNP(MENU_SPECIAL_MENU);
        } else {
          // on sub directory level display cdup menu 
          ANYCUBIC_TFT_SPECIAL_MENU(MENU_SPECIAL_LEVEL_UP);
          DEBUG_PRINT(idx);
          DEBUG_PRINT_LNP(MENU_SPECIAL_LEVEL_UP);
        }
      } else {
        // Go back to one cnt to account special menu entry
        card.selectFileByIndex(idx-1);
        if(card.flag.filenameIsDir) {
          // prefix directory name with forward slash
          ANYCUBIC_SERIAL_PROTOCOLPGM("/");
          ANYCUBIC_SERIAL_PROTOCOLLN(card.filename);
          ANYCUBIC_SERIAL_PROTOCOLPGM("/");
          ANYCUBIC_SERIAL_PROTOCOLLN(card.longFilename);

          DEBUG_PRINT(idx);
          DEBUG_PRINT_LNPGM("/");
          DEBUG_PRINT(card.longFilename);
          DEBUG_PRINT_EOL();
        } else {
          //display file names
          ANYCUBIC_SERIAL_PROTOCOLLN(card.filename);
          ANYCUBIC_SERIAL_PROTOCOLLN(card.longFilename);

          DEBUG_PRINT(idx);
          DEBUG_PRINT(card.longFilename);
          DEBUG_PRINT_EOL();
        }
      }
    }
  }
#endif
  else {
    //if no sd card is inserted then display only special menu
    ANYCUBIC_TFT_SPECIAL_MENU(MENU_SPECIAL_MENU);
    _lastStartIdx = 0;
  }
}

void AnycubicTFTClass::CheckHeaterError()
{
  if ((thermalManager.degHotend(0) < 5) || (thermalManager.degHotend(0) > 290))
  {
    if (_heaterCheckCount > 60000)
    {
      _heaterCheckCount = 0;
      ANYCUBIC_SERIAL_PROTOCOLPGM("J10"); // J10 Hotend temperature abnormal
      ANYCUBIC_SERIAL_ENTER();
      DEBUG_PRINT_LNPGM("TFT Serial Debug: Hotend temperature abnormal... J20");
    }
    else
    {
      _heaterCheckCount++;
    }
  }
  else
  {
    _heaterCheckCount = 0;
  }
}

void AnycubicTFTClass::StateHandler()
{
  switch (_TFTState) {
    case ANYCUBIC_TFT_STATE_IDLE:
      if(isSDPrintingInProgress()) {  //means sd print is started by Gcode not from UI
        _TFTState=ANYCUBIC_TFT_STATE_SDPRINT;
        ANYCUBIC_SERIAL_CMD_SENDPGM("J04");// J04 printing form sd card now
        DEBUG_PRINT_LNPGM("StateHandler: Printing from sd card... J04");
        // --> Send print info to display... most probably print started via gcode
      }
      break;
    case ANYCUBIC_TFT_STATE_SDPRINT:
      // It seems that we are no longer printing ... pause or stopped, if it is paused then what kind of pause?
      if (isPrintingFromMediaPaused()) { // media print is paused
        if(IFOOF(getFilamentRunoutEnabled() && runout.filament_ran_out,false))
        {
          _TFTState=IFOOF(ANYCUBIC_TFT_STATE_SDPAUSE_OOF,IFADVPAUSE(ANYCUBIC_TFT_STATE_SDPAUSE_ADV,ANYCUBIC_TFT_STATE_SDPAUSE));
          ANYCUBIC_SERIAL_PROTOCOLPGM("J15"); //J15 FILAMENT LACK
          ANYCUBIC_SERIAL_ENTER();
          DEBUG_PRINT_LNPGM("StateHandler: Filament runout... J15");
        }
        else 
        {
          _TFTState= IFADVPAUSE(ANYCUBIC_TFT_STATE_SDPAUSE_ADV, ANYCUBIC_TFT_STATE_SDPAUSE);
          ANYCUBIC_SERIAL_PROTOCOLPGM("J18"); //J18 Pause success
          ANYCUBIC_SERIAL_ENTER();
          DEBUG_PRINT_LNPGM("StateHandler: Pause success... J18");
        }
      } else if (!isPrintingFromMedia()) { // file closed directly from print means print is completed
        OnPrintFinished();
      }
      break;
#if ENABLED(ADVANCED_PAUSE_FEATURE)
    case ANYCUBIC_TFT_STATE_SDPAUSE_ADV: // This condtion will hit only advance pause is enabled
      if(isSDPrintingInProgress())
      {
        _TFTState=ANYCUBIC_TFT_STATE_SDPRINT;
        ANYCUBIC_SERIAL_CMD_SENDPGM("J04");// J04 printing form sd card now
        DEBUG_PRINT_LNPGM("StateHandler: printing from sd card now... J04");
      }else if(!isPrintingFromMedia()) // if sd file is closed then print is stopped
      {
        _TFTState=ANYCUBIC_TFT_STATE_SDSTOP;
        ANYCUBIC_SERIAL_CMD_SENDPGM("J16");// J16 stop print
        DEBUG_PRINT_LNPGM("StateHandler: stop print... J16");
      }
      break;
#else
    case ANYCUBIC_TFT_STATE_SDPAUSE: // This condiotn will hit only when advance pause is disabled
      if(isSDPrintingInProgress())
      {
        _TFTState=ANYCUBIC_TFT_STATE_SDPRINT;
        ANYCUBIC_SERIAL_CMD_SENDPGM("J04");// J04 printing form sd card now
        DEBUG_PRINT_LNPGM("StateHandler: printing from sd card now... J04");
      }else if(!isPrintingFromMedia()) // if sd file is closed then print is stopped
      {
        _TFTState=ANYCUBIC_TFT_STATE_SDSTOP;
        ANYCUBIC_SERIAL_CMD_SENDPGM("J16");// J16 stop print
        DEBUG_PRINT_LNPGM("StateHandler: stop print... J16");
      }
      break;
#endif
#if HAS_FILAMENT_SENSOR
    case ANYCUBIC_TFT_STATE_SDPAUSE_OOF:
      if(isSDPrintingInProgress())
      {
        _TFTState=ANYCUBIC_TFT_STATE_SDPRINT;
        ANYCUBIC_SERIAL_CMD_SENDPGM("J04");// J04 printing from sd card now
        DEBUG_PRINT_LNPGM("StateHandler: printing from sd card now... J04");
      }else if(!isPrintingFromMedia()) // if sd file is closed then print is stopped
      {
        _TFTState=ANYCUBIC_TFT_STATE_SDSTOP;
        ANYCUBIC_SERIAL_CMD_SENDPGM("J16");// J16 stop print
        DEBUG_PRINT_LNPGM("StateHandler: stop print... J16");
      }
      break;
#endif
    case ANYCUBIC_TFT_STATE_SDPAUSE_REQ: // paused by user through UI
      if(isSDPrintingInProgress())
      {
        _TFTState=ANYCUBIC_TFT_STATE_SDPRINT;
        ANYCUBIC_SERIAL_CMD_SENDPGM("J04");// J04 printing from sd card now
        DEBUG_PRINT_LNPGM("StateHandler: printing from sd card now... J04");
      }else if(!isPrintingFromMedia()) // if sd file is closed then print is stopped
      {
        _TFTState=ANYCUBIC_TFT_STATE_SDSTOP;
        ANYCUBIC_SERIAL_CMD_SENDPGM("J16");// J16 stop print
        DEBUG_PRINT_LNPGM("StateHandler: stop print... J16");
      }
      break;
    case ANYCUBIC_TFT_STATE_SDSTOP:
        if(isPrinterIdle()) { //check file is closed and all the commands are processed
          // enter idle display state
          _TFTState=ANYCUBIC_TFT_STATE_IDLE;
        }
      break;
    default:
      break;
  }
}

#ifdef FILAMENT_RUNOUT_SENSOR
// Get Called only when printing is active and only one time per one occurance
// After this function is called M600 gcode is issued to pause the print
void AnycubicTFTClass::FilamentRunout()
{

}
#endif

void AnycubicTFTClass::GetCommandFromTFT()
{
  char *starpos = NULL;
  uint16_t tempVal = 0;
  while( AnycubicSerial.available() > 0  && _TFTbuflen < ANYCUBIC_TFT_BUF_SIZE)
  {
    _serial3_char = AnycubicSerial.read();
    if(_serial3_char == '\n' ||
    _serial3_char == '\r' ||
    _serial3_char == ':'  ||
    _serial3_count >= (ANYCUBIC_TFT_MAX_CMD_SIZE - 1) )
    {
      if(!_serial3_count) { //if empty line
#ifdef ANYCUBIC_TFT_COMMENT_ENABLED
        _TFTcomment_mode = false; //for new command
#endif
        return;
      }

      _TFTcmdbuffer[_TFTbufindw][_serial3_count] = 0; //terminate string

#ifdef ANYCUBIC_TFT_COMMENT_ENABLED
      // ANYCUBIC_SERIAL_PROTOCOL("OK:");
      // ANYCUBIC_SERIAL_PROTOCOLLN(TFTcmdbuffer[TFTbufindw]);
     if(!_TFTcomment_mode){
       _TFTcomment_mode = false; //for new command

       
       if(strchr(TFTcmdbuffer[TFTbufindw], 'N') != NULL)
       {
        TFTstrchr_pointer = strchr(TFTcmdbuffer[TFTbufindw], 'N');
        gcode_N = (strtol(&TFTcmdbuffer[TFTbufindw][TFTstrchr_pointer - TFTcmdbuffer[TFTbufindw] + 1], NULL, 10));
        if(gcode_N != gcode_LastN+1 && (strstr_P(TFTcmdbuffer[TFTbufindw], PSTR("M110")) == NULL) ) {
          ANYCUBIC_SERIAL_ERROR_START;
          //     ANYCUBIC_SERIAL_ERRORPGM(MSG_ERR_LINE_NO);
          //     ANYCUBIC_SERIAL_ERRORLN(gcode_LastN);
          NEWFlushSerialRequestResend();
          serial3_count = 0;
          return;
       }
  
       if(strchr(TFTcmdbuffer[TFTbufindw], '*') != NULL)
       {
          byte checksum = 0;
          byte count = 0;
          while(TFTcmdbuffer[TFTbufindw][count] != '*') checksum = checksum^TFTcmdbuffer[TFTbufindw][count++];
          TFTstrchr_pointer = strchr(TFTcmdbuffer[TFTbufindw], '*');
    
          if( (int)(strtod(&TFTcmdbuffer[TFTbufindw][TFTstrchr_pointer - TFTcmdbuffer[TFTbufindw] + 1], NULL)) != checksum) 
          {
              ANYCUBIC_SERIAL_ERROR_START;
              //ANYCUBIC_SERIAL_ERRORPGM(MSG_ERR_CHECKSUM_MISMATCH);
              //ANYCUBIC_SERIAL_ERRORLN(gcode_LastN);
              NEWFlushSerialRequestResend();

              ANYCUBIC_SERIAL_ERROR_START;
      //      ANYCUBIC_SERIAL_ERRORPGM(MSG_ERR_CHECKSUM_MISMATCH);
      //      ANYCUBIC_SERIAL_ERRORLN(gcode_LastN);
              NEWFlushSerialRequestResend();
              serial3_count = 0;
              return;
          }
          //if no errors, continue parsing
        }
        else
        {
            ANYCUBIC_SERIAL_ERROR_START;
        //  ANYCUBIC_SERIAL_ERRORPGM(MSG_ERR_NO_CHECKSUM);
        //  ANYCUBIC_SERIAL_ERRORLN(gcode_LastN);
          NEWFlushSerialRequestResend();
          serial3_count = 0;
          return;
        }  
       //gcode_LastN = gcode_N;
       //if no errors, continue parsing
       }
       else  // if we don't receive 'N' but still see '*'
       {
          if((strchr(TFTcmdbuffer[TFTbufindw], '*') != NULL))
          {
              ANYCUBIC_SERIAL_ERROR_START;
        //    ANYCUBIC_SERIAL_ERRORPGM(MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM);
        //    ANYCUBIC_SERIAL_ERRORLN(gcode_LastN);
              serial3_count = 0;
              return;
          }
       }
#endif

      if((strchr(_TFTcmdbuffer[_TFTbufindw], 'A') != NULL)) {
        int16_t a_command;
        _TFTstrchr_pointer = strchr(_TFTcmdbuffer[_TFTbufindw], 'A');
        a_command=((int)((strtod(&_TFTcmdbuffer[_TFTbufindw][_TFTstrchr_pointer - _TFTcmdbuffer[_TFTbufindw] + 1], NULL))));

        #ifdef ANYCUBIC_TFT_DEBUG
        if ((a_command>7) && (a_command != 20)) // No debugging of status polls, please!
        SERIAL_ECHOLNPAIR("TFT Serial Command: ", _TFTcmdbuffer[_TFTbufindw]);
        #endif

        switch(a_command) {

          case 0: //A0 GET HOTEND TEMP
            ANYCUBIC_SERIAL_PROTOCOLPGM("A0V ");
            ANYCUBIC_SERIAL_PROTOCOL(itostr3(int(thermalManager.degHotend(0) + 0.5)));
            ANYCUBIC_SERIAL_ENTER();
            break;
          case 1: //A1  GET HOTEND TARGET TEMP
            ANYCUBIC_SERIAL_PROTOCOLPGM("A1V ");
            ANYCUBIC_SERIAL_PROTOCOL(itostr3(int(thermalManager.degTargetHotend(0) + 0.5)));
            ANYCUBIC_SERIAL_ENTER();
            break;
          case 2: //A2 GET HOTBED TEMP
            ANYCUBIC_SERIAL_PROTOCOLPGM("A2V ");
            #if HAS_TEMP_CHAMBER
              if(_bedOrChamber) {
                _bedOrChamber = false;
                ANYCUBIC_SERIAL_PROTOCOL(itostr3(int(thermalManager.degBed() + 0.5)));
                _bedTargetTemp = uint8_t(thermalManager.degTargetBed() + 0.5);
              }
              else {
                _bedOrChamber = true;
                _bedTargetTemp = 0;
                ANYCUBIC_SERIAL_PROTOCOL(itostr3(int(thermalManager.degChamber() + 0.5)));
              }
            #else
              ANYCUBIC_SERIAL_PROTOCOL(itostr3(int(thermalManager.degBed() + 0.5)));
            #endif
            ANYCUBIC_SERIAL_ENTER();
            break;
          case 3: //A3 GET HOTBED TARGET TEMP
            ANYCUBIC_SERIAL_PROTOCOLPGM("A3V ");
            #if HAS_TEMP_CHAMBER
              ANYCUBIC_SERIAL_PROTOCOL(itostr2(_bedTargetTemp));
            #else
              ANYCUBIC_SERIAL_PROTOCOL(itostr3(int(thermalManager.degTargetBed() + 0.5)));
            #endif
            ANYCUBIC_SERIAL_ENTER();
            break;
          case 4://A4 GET FAN SPEED
            {
              unsigned int temp;

              temp = ((thermalManager.fan_speed[0] * 100) / 255);
              temp=constrain(temp,0,100);

              ANYCUBIC_SERIAL_PROTOCOLPGM("A4V ");
              ANYCUBIC_SERIAL_PROTOCOL(temp);
              ANYCUBIC_SERIAL_ENTER();
            }
            break;
          case 5:// A5 GET CURRENT COORDINATE
            ANYCUBIC_SERIAL_PROTOCOLPGM("A5V");
            ANYCUBIC_SERIAL_SPACE();
            ANYCUBIC_SERIAL_PROTOCOLPGM("X: ");
            ANYCUBIC_SERIAL_PROTOCOL(current_position[X_AXIS]);
            ANYCUBIC_SERIAL_SPACE();
            ANYCUBIC_SERIAL_PROTOCOLPGM("Y: ");
            ANYCUBIC_SERIAL_PROTOCOL(current_position[Y_AXIS]);
            ANYCUBIC_SERIAL_SPACE();
            ANYCUBIC_SERIAL_PROTOCOLPGM("Z: ");
            ANYCUBIC_SERIAL_PROTOCOL(current_position[Z_AXIS]);
            ANYCUBIC_SERIAL_SPACE();
            ANYCUBIC_SERIAL_ENTER();
            break;
          case 6: //A6 GET SD CARD PRINTING STATUS
            if(isSDPrintingInProgress()) {
              ANYCUBIC_SERIAL_PROTOCOLPGM("A6V ");
              ANYCUBIC_SERIAL_PROTOCOL(itostr3(card.percentDone())); // return print percent
            }
            else
            {
              ANYCUBIC_SERIAL_PROTOCOLPGM("A6V ---"); // return No Printing
            }
            ANYCUBIC_SERIAL_ENTER();
            break;
          case 7://A7 GET PRINTING TIME
            ANYCUBIC_SERIAL_PROTOCOLPGM("A7V ");
            if(_starttime != 0) // print time
            {
              uint16_t time = safe_millis()/60000 - _starttime/60000;
              ANYCUBIC_SERIAL_PROTOCOL(itostr2(time/60));
              ANYCUBIC_SERIAL_SPACE();
              ANYCUBIC_SERIAL_PROTOCOLPGM("H");
              ANYCUBIC_SERIAL_SPACE();
              ANYCUBIC_SERIAL_PROTOCOL(itostr2(time%60));
              ANYCUBIC_SERIAL_SPACE();
              ANYCUBIC_SERIAL_PROTOCOLPGM("M");
            }else{
              ANYCUBIC_SERIAL_SPACE();
              ANYCUBIC_SERIAL_PROTOCOLPGM("999:999");
            }
            ANYCUBIC_SERIAL_ENTER();
            break;
          case 8: // A8 GET SD LIST (at launch / back button press / down arrow / up arrow button press)
            DEBUG_PRINT_LNPGM("TFT Serial Debug:GET SD LIST... A8");
            _selectedDirectory[0]=0;
            if(IFSD(!IS_SD_INSERTED(), true))
            {
              ANYCUBIC_SERIAL_PROTOCOLPGM("J02"); // J02 No memory card
              ANYCUBIC_SERIAL_ENTER();
            }
            else
            {
              tempVal=0;
              if(CodeSeen('S')) { tempVal=CodeValue(); }

              ANYCUBIC_SERIAL_PROTOCOLPGM("FN "); // Filelist start
              ANYCUBIC_SERIAL_ENTER();
              BuildListItems(tempVal);
              ANYCUBIC_SERIAL_PROTOCOLPGM("END"); // Filelist stop
              ANYCUBIC_SERIAL_ENTER();
            }
            _isLastUserActionRefresh = false; // reset last user action
            break;
          case 9: // A9 pause sd print
              if(isSDPrintingInProgress())
              {
                PausePrint();
              }
            break;
          case 10: // A10 resume sd print
              if(isPrintingFromMedia())
              {
                StartPrint();
              }
            break;
          case 11: // A11 STOP SD PRINT
              StopPrint();
            break;
          case 12: // A12 kill
            //kill(PSTR(MSG_ERR_KILLED));
            break;
          case 13: // A13 SELECTION FILE (By toucing in touch pad / user makes selection by touching item in screen)
            {
              DEBUG_PRINT_LNPGM("TFT Serial Debug: SELECTION FILE... A13");
              starpos = (strchr(_TFTstrchr_pointer + ANYCUBIC_TFT_PAGE_ITEM_START,'*'));

              if (_TFTstrchr_pointer[ANYCUBIC_TFT_PAGE_ITEM_START] == '/') {
                strcpy(_selectedDirectory, _TFTstrchr_pointer + ANYCUBIC_TFT_PAGE_ITEM_START + 1); // Truncate first character on directory list item
                _lastStartIdx = 0;
              } else if (_TFTstrchr_pointer[ANYCUBIC_TFT_PAGE_ITEM_START] == '<') {  // Look for special character at begining of file name to detect user touched special menu
                // user touched special menu / it will not refresh list automatically, for that we have to rely on user explicitly pressing the refresh button
                strcpy(_selectedDirectory, _TFTstrchr_pointer + ANYCUBIC_TFT_PAGE_ITEM_START);
              } else {
                _selectedDirectory[0]=0;
                _lastStartIdx = 0;

                if(starpos!=NULL) { *(starpos-1)='\0'; }

                // bellow action will open the file in card and make current selected file ready for print
                card.openFileRead(_TFTstrchr_pointer + ANYCUBIC_TFT_PAGE_ITEM_START);
                if (card.isFileOpen()) {
                  ANYCUBIC_SERIAL_PROTOCOLPGM("J20"); // J20 Open successful
                  ANYCUBIC_SERIAL_ENTER();
                  DEBUG_PRINT_LNPGM("TFT Serial Debug: File open successful... J20");
                } else {
                  ANYCUBIC_SERIAL_PROTOCOLPGM("J21"); // J21 Open failed
                  ANYCUBIC_SERIAL_ENTER();
                  DEBUG_PRINT_LNPGM("TFT Serial Debug: File open failed... J21");
                }
              }
              ANYCUBIC_SERIAL_ENTER();
            }
            break;
          case 14: // A14 START PRINTING
              if(!commandsInQueue())
              {
                #ifdef ANYCUBIC_POWERDOWN  
                  _powerOFFflag= false;
                #endif
                StartPrint();
              }
            break;
          case 15: // A15 RESUMING FROM OUTAGE
            if(!commandsInQueue())
            {
              StartPrint();
              ANYCUBIC_SERIAL_SUCC_START;
            }
            ANYCUBIC_SERIAL_ENTER();
            break;
          case 16: // A16 set hotend temp
            {
              unsigned int tempvalue;
              if(CodeSeen('S'))
              {
                tempvalue=constrain(CodeValue(),0,275);
                thermalManager.setTargetHotend(tempvalue,0);
              }
              else if((CodeSeen('C'))&&(!planner.movesplanned()))
              {
                if((current_position[Z_AXIS]<10))
                ENQUEUE_CMD_NOW_PGM("G1 Z10"); //RASE Z AXIS
                tempvalue=constrain(CodeValue(),0,275);
                thermalManager.setTargetHotend(tempvalue,0);
              }
            }
            ANYCUBIC_SERIAL_ENTER();
            break;
          case 17:// A17 set heated bed temp
            {
              unsigned int tempbed;
              if(CodeSeen('S')) {tempbed=constrain(CodeValue(),0,110);
              {
                thermalManager.setTargetBed(tempbed);}
              }
            }
            ANYCUBIC_SERIAL_ENTER();
            break;
          case 18:// A18 set fan speed
            unsigned int temp;
            if (CodeSeen('S'))
            {
              temp=(CodeValue()*255/100);
              temp=constrain(temp,0,255);
              thermalManager.set_fan_speed(0, temp);
            }
            else { thermalManager.set_fan_speed(0, 255); }
            ANYCUBIC_SERIAL_ENTER();
            break;
          case 19: // A19 stop stepper drivers
            if(isPrinterIdle())
            {
              quickstop_stepper();
              disable_all_steppers();
              //disable_X();
              //disable_Y();
              //disable_Z();
              //disable_E0();
            }
            ANYCUBIC_SERIAL_ENTER();
            break;
          case 20:// A20 read printing speed
          {
            if(CodeSeen('S')) {
              feedrate_percentage=constrain(CodeValue(),40,999);
            }
            else{
              ANYCUBIC_SERIAL_PROTOCOLPGM("A20V ");
              ANYCUBIC_SERIAL_PROTOCOL(feedrate_percentage);
              ANYCUBIC_SERIAL_ENTER();
            }
          }
          break;
          case 21: // A21 all home
            if(isPrinterIdle())
            {
              if(CodeSeen('X')||CodeSeen('Y')||CodeSeen('Z'))
              {
                if(CodeSeen('X')) ENQUEUE_CMD_NOW_PGM("G28 X");
                if(CodeSeen('Y')) ENQUEUE_CMD_NOW_PGM("G28 Y");
                if(CodeSeen('Z')) ENQUEUE_CMD_NOW_PGM("G28 Z");
              }
              else if(CodeSeen('C')) { ENQUEUE_CMD_NOW_PGM("G28"); }
            }
            break;
          case 22: // A22 move X/Y/Z or extrude
            if(isPrinterIdle())
            {
              float coorvalue;
              unsigned int movespeed=0;
              char value[30];
              if(CodeSeen('F')) // Set feedrate
              movespeed = CodeValue();

              ENQUEUE_CMD_NOW_PGM("G91"); // relative coordinates

              if(CodeSeen('X')) // Move in X direction
              {
                coorvalue=CodeValue();
                if((coorvalue<=0.2)&&coorvalue>0) {sprintf_P(value,PSTR("G1 X0.1F%i"),movespeed);}
                else if((coorvalue<=-0.1)&&coorvalue>-1) {sprintf_P(value,PSTR("G1 X-0.1F%i"),movespeed);}
                else {sprintf_P(value,PSTR("G1 X%iF%i"),int(coorvalue),movespeed);}
                ENQUEUE_CMD_NOW(value);
              }
              else if(CodeSeen('Y')) // Move in Y direction
              {
                coorvalue=CodeValue();
                if((coorvalue<=0.2)&&coorvalue>0) {sprintf_P(value,PSTR("G1 Y0.1F%i"),movespeed);}
                else if((coorvalue<=-0.1)&&coorvalue>-1) {sprintf_P(value,PSTR("G1 Y-0.1F%i"),movespeed);}
                else {sprintf_P(value,PSTR("G1 Y%iF%i"),int(coorvalue),movespeed);}
                ENQUEUE_CMD_NOW(value);
              }
              else if(CodeSeen('Z')) // Move in Z direction
              {
                coorvalue=CodeValue();
                if((coorvalue<=0.2)&&coorvalue>0) {sprintf_P(value,PSTR("G1 Z0.1F%i"),movespeed);}
                else if((coorvalue<=-0.1)&&coorvalue>-1) {sprintf_P(value,PSTR("G1 Z-0.1F%i"),movespeed);}
                else {sprintf_P(value,PSTR("G1 Z%iF%i"),int(coorvalue),movespeed);}
                ENQUEUE_CMD_NOW(value);
              }
              else if(CodeSeen('E')) // Extrude
              {
                coorvalue=CodeValue();
                if((coorvalue<=0.2)&&coorvalue>0) {sprintf_P(value,PSTR("G1 E0.1F%i"),movespeed);}
                else if((coorvalue<=-0.1)&&coorvalue>-1) {sprintf_P(value,PSTR("G1 E-0.1F%i"),movespeed);}
                else {sprintf_P(value,PSTR("G1 E%iF500"),int(coorvalue)); }
                ENQUEUE_CMD_NOW(value);
              }
              ENQUEUE_CMD_NOW_PGM("G90"); // absolute coordinates
            }
            ANYCUBIC_SERIAL_ENTER();
            break;
          case 23: // A23 preheat pla
            if(isPrinterIdle())
            {
              if((current_position[Z_AXIS]<10)) ENQUEUE_CMD_NOW_PGM("G1 Z10"); // RAISE Z AXIS
              thermalManager.setTargetBed(60);
              thermalManager.setTargetHotend(210, 0);
              ANYCUBIC_SERIAL_SUCC_START;
              ANYCUBIC_SERIAL_ENTER();
            }
            break;
          case 24:// A24 preheat abs
            if(isPrinterIdle())
            {
              if((current_position[Z_AXIS]<10)) ENQUEUE_CMD_NOW_PGM("G1 Z10"); //RAISE Z AXIS
              thermalManager.setTargetBed(80);
              thermalManager.setTargetHotend(240, 0);

              ANYCUBIC_SERIAL_SUCC_START;
              ANYCUBIC_SERIAL_ENTER();
            }
            break;
          case 25: // A25 cool down
            if(isPrinterIdle())
            {
              thermalManager.setTargetHotend(0,0);
              thermalManager.setTargetBed(0);
              ANYCUBIC_SERIAL_PROTOCOLPGM("J12"); // J12 cool down
              ANYCUBIC_SERIAL_ENTER();
              DEBUG_PRINT_LNPGM("TFT Serial Debug: Cooling down... J12");
            }
            break;
          case 26: // A26 refresh SD (user touch refresh key), always TFT display will issue A8S command to get new list after refresh button press
          // On special menu case it will act like enter button
            DEBUG_PRINT_LNPGM("TFT Serial Debug: Refresh SD... A26");
            _isLastUserActionRefresh=false;
            if (_selectedDirectory[0]==0) { // Top Level user refresh
               IFSD(card.mount(),"");
            } else {
              if ((_selectedDirectory[0] == '.') && (_selectedDirectory[1] == '.')) {
                IFSD(card.cdup(),""); // User selected cdup menu item and want go up directory level
              } else {
                if (_selectedDirectory[0] == '<') {
                  HandleSpecialMenu(); // user selected special menu and want to perform action on in it
                } else {
                  IFSD(card.cd(_selectedDirectory),""); // user selected directory and navigate into the directory
                }
              }
            }
            _selectedDirectory[0]=0; // reset the selection
            break;
#if defined(SERVO_ENDSTOPS)
            case 27: // A27 servos angles  adjust
            //if((!_USBConnectFlag)&&(!card.sdprinting)) 
            if((!planner.movesplanned())&&(!TFTresumingflag))             
            {
            char value[30];
            planner.buffer_line(current_position[X_AXIS],current_position[Y_AXIS], 20, current_position[E_AXIS], 10, active_extruder);
            stepper.synchronize();
            ANYCUBIC_SERIAL_PROTOCOLPGM("A27V ");
            ANYCUBIC_SERIAL_PROTOCOLPGM("R ");
            ANYCUBIC_SERIAL_PROTOCOL(RiseAngles);
            ANYCUBIC_SERIAL_SPACE();
            ANYCUBIC_SERIAL_PROTOCOLPGM("F ");
            ANYCUBIC_SERIAL_PROTOCOL(FallAngles);
            ANYCUBIC_SERIAL_SPACE();                          
              if(TFTcode_seen('R'))
              {
                  RiseAngles=TFTcode_value();
                                        
              }
              if(TFTcode_seen('F'))
              {
                  FallAngles=TFTcode_value();

              }   
              if(TFTcode_seen('O')){ SaveMyServoAngles();delay(200);servos[0].detach();}                 
            }   
            ANYCUBIC_SERIAL_ENTER();
              break;
#endif
          case 28: // A28 filament test
            {
              if(CodeSeen('O'));
              else if(CodeSeen('C'));
            }
            ANYCUBIC_SERIAL_ENTER();
            break;
#ifdef ANYCUBIC_Z_OFFSET
            case 29: // A29 Z PROBE OFFESET SET
              {
                ANYCUBIC_SERIAL_PROTOCOLPGM("The past value:");
                ANYCUBIC_SERIAL_PROTOCOL(MY_Z_PROBE);                              
                if(CodeSeen('S'))
                {
                  MY_Z_PROBE_OFFSET_FROM_EXTRUDER=CodeValue();
                  SaveMyServoAngles(); 
                }
              }
              ANYCUBIC_SERIAL_SUCC_START;
              break;
            case 30: // A30 assist leveling, the original function was canceled
              if(CodeSeen('S')) {
                DEBUG_PRINT_LINE("TFT Entering level menue...");
              } else if(CodeSeen('O')) {
                DEBUG_PRINT_LINE("TFT Leveling started and movint to front left...");
                ENQUEUE_NOW("G91\nG1 Z10 F240\nG90\nG28\nG29\nG1 X20 Y20 F6000\nG1 Z0 F240");
              } else if(CodeSeen('T')) {
                DEBUG_PRINT_LINE("TFT Level checkpoint front right...");
                ENQUEUE_NOW("G1 Z5 F240\nG1 X190 Y20 F6000\nG1 Z0 F240");
              } else if(CodeSeen('C')) {
                DEBUG_PRINT_LINE("TFT Level checkpoint back right...");
                ENQUEUE_NOW("G1 Z5 F240\nG1 X190 Y190 F6000\nG1 Z0 F240");
              } else if(CodeSeen('Q')) {
                DEBUG_PRINT_LINE("TFT Level checkpoint back right...");
                ENQUEUE_NOW("G1 Z5 F240\nG1 X190 Y20 F6000\nG1 Z0 F240");
              } else if(CodeSeen('H')) {
                DEBUG_PRINT_LINE("TFT Level check no heating...");
                //ENQUEUE_NOW("... TBD ..."));
                ANYCUBIC_SERIAL_PROTOCOLPGM("J22"); // J22 Test print done
                ANYCUBIC_SERIAL_ENTER();
                DEBUG_PRINT_LINE("TFT Serial Debug: Leveling print test done... J22");
              } else if(CodeSeen('L')) {
                DEBUG_PRINT_LINE("TFT Level check heating...");
                //ENQUEUE_NOW("... TBD ..."));
                ANYCUBIC_SERIAL_PROTOCOLPGM("J22"); // J22 Test print done
                ANYCUBIC_SERIAL_ENTER();
                DEBUG_PRINT_LINE("TFT Serial Debug: Leveling print test with heating done... J22");
              }
              ANYCUBIC_SERIAL_SUCC_START;
              ANYCUBIC_SERIAL_ENTER();
              break;
#endif

#if HAS_BED_PROBE
            case 31: // A31 zoffset
              if((!planner.movesplanned())&&(TFTstate!=ANYCUBIC_TFT_STATE_SDPAUSE))
              {

                  char value[30];
                  char *s_zoffset;
                  //if((current_position[Z_AXIS]<10))
                  //  z_offset_auto_test();

                  if(CodeSeen('S')) {
                    ANYCUBIC_SERIAL_PROTOCOLPGM("A9V ");
                    ANYCUBIC_SERIAL_PROTOCOL(itostr3(int(zprobe_zoffset*100.00 + 0.5)));
                    ANYCUBIC_SERIAL_ENTER();
                    DEBUG_PRINT_ECHO("TFT sending current z-probe offset data... <");
                    DEBUG_PRINT_ECHO("A9V ");
                    DEBUG_PRINT_ECHO(itostr3(int(zprobe_zoffset*100.00 + 0.5)));
                    DEBUG_PRINT_LINE(">");
                  }
                  if(CodeSeen('D'))
                  {
                    s_zoffset=ftostr32(float(CodeValue())/100.0);
                    sprintf_P(value,PSTR("M851 Z"));
                    strcat(value,s_zoffset);
                    ENQUEUE_NOW_VAR(value); // Apply Z-Probe offset
                    ENQUEUE_NOW("M500")); // Save to EEPROM
                  }
                
              }
              ANYCUBIC_SERIAL_ENTER();
              break;
            case 32: // A32 clean leveling beep flag
              if(CodeSeen('S')) {
                DEBUG_PRINT_LINE("TFT Level saving data...");
                ENQUEUE_NOW("M500\nM420 S1\nG1 Z10 F240\nG1 X0 Y0 F6000");
                ANYCUBIC_SERIAL_SUCC_START;
                ANYCUBIC_SERIAL_ENTER();
              }
              break;
#endif
          case 33: // A33 get version info
            {
              ANYCUBIC_SERIAL_PROTOCOLPGM("J33 ");
              ANYCUBIC_SERIAL_PROTOCOLPGM(MSG_MY_VERSION);
              ANYCUBIC_SERIAL_ENTER();
            }
            break;
            case 40://a40 reset mainboard
            softwareReset();
            break;
#ifdef ANYCUBIC_POWERDOWN
          case 41://a41 continue button pressed
            {
              if(CodeSeen('O'))
              {
                _PrintdoneAndPowerOFF=true;
                break;
              }
              else if(CodeSeen('C'))
              {
                _PrintdoneAndPowerOFF=false;
                break;
              }  
              if(CodeSeen('S')) 
              {                                             
                if(_PrintdoneAndPowerOFF)
                {
                  ANYCUBIC_SERIAL_PROTOCOLPGM("J35 ");                     
                  ANYCUBIC_SERIAL_ENTER();                            
                }
                else  //didn't open print done and auto power off
                {
                  ANYCUBIC_SERIAL_PROTOCOLPGM("J34 ");                     
                  ANYCUBIC_SERIAL_ENTER();                               
                }
              }
            }
            break;
#endif
#if ENABLED(CASE_LIGHT_ENABLE)
          case 42://a42 light
            {
              if(CodeSeen('O'))
              {
                setCaseLightState(true);
                //ENQUEUE_NOW("M355 S1");
              }
              else  if(CodeSeen('C'))
              {
                setCaseLightState(false);
                //ENQUEUE_NOW("M355 S0");
              }
            }  
            ANYCUBIC_SERIAL_ENTER();
            break;
#endif
          default: 
            DEBUG_PRINT_PAIR("TFT Not Handled Command: ", _TFTcmdbuffer[_TFTbufindw]);
            DEBUG_PRINT_EOL();
            break;
        }
      }
#ifdef ANYCUBIC_TFT_DEBUG
      else if(strlen(_TFTcmdbuffer[_TFTbufindw]) > 0)
      {
        DEBUG_PRINT_PAIR("TFT Not Handled Command: ", _TFTcmdbuffer[_TFTbufindw]);
        DEBUG_PRINT_EOL();
      }
#endif
      _TFTbufindw = (_TFTbufindw + 1)%ANYCUBIC_TFT_BUF_SIZE;
      _TFTbuflen += 1;
      _serial3_count = 0; //clear buffer
    }
    else
    {
#ifdef ANYCUBIC_TFT_COMMENT_ENABLED
      if(serial3_char == ';') _TFTcomment_mode = true;
      if(!_TFTcomment_mode) TFTcmdbuffer[TFTbufindw][serial3_count++] = serial3_char;
#else
      _TFTcmdbuffer[_TFTbufindw][_serial3_count++] = _serial3_char;
#endif
    }
  }
}

void AnycubicTFTClass::CommandScan()
{
  #ifdef ANYCUBIC_TFT_DEBUG_MSG
  char value[6];
  #endif

  CheckHeaterError();
  #ifdef ANYCUBIC_SD_CHECK
  CheckSDCardChange();
  #endif
  StateHandler();

  if(_TFTbuflen<(ANYCUBIC_TFT_BUF_SIZE-1))
  GetCommandFromTFT();
  if(_TFTbuflen)
  {
    _TFTbuflen = (_TFTbuflen-1);
    _TFTbufindr = (_TFTbufindr + 1)%ANYCUBIC_TFT_BUF_SIZE;
  }

  static unsigned int Scancount=0;
  //    static unsigned long timeoutToStatus = 0;
  if((thermalManager.degHotend(0)<5)||((thermalManager.degHotend(0)>285)))   Scancount++;
  if(Scancount>16000){ Scancount=0;ANYCUBIC_SERIAL_PROTOCOLPGM("J10");ANYCUBIC_SERIAL_ENTER();}//T0 unnormal//2019.4.23

#ifdef ANYCUBIC_TFT_DEBUG_MSG
    if(_showMsg)
    {
      _showMsg = false;
      sprintf_P(value,PSTR("J%02d"),_lastMsgId);
      ANYCUBIC_SERIAL_PROTOCOLLN(value);
    }
#endif

}

void AnycubicTFTClass::HeatingStart()
{
  ANYCUBIC_SERIAL_PROTOCOLPGM("J06"); // J07 hotend heating start
  ANYCUBIC_SERIAL_ENTER();
  DEBUG_PRINT_LNPGM("TFT Serial Debug: Nozzle is heating... J06");
}

void AnycubicTFTClass::HeatingDone()
{
  ANYCUBIC_SERIAL_PROTOCOLPGM("J07"); // J07 hotend heating done
  ANYCUBIC_SERIAL_ENTER();
  DEBUG_PRINT_LNPGM("TFT Serial Debug: Nozzle heating is done... J07");
}

void AnycubicTFTClass::BedHeatingStart()
{
  ANYCUBIC_SERIAL_PROTOCOLPGM("J08"); // J08 hotbed heating start
  ANYCUBIC_SERIAL_ENTER();
  DEBUG_PRINT_LNPGM("TFT Serial Debug: Bed is heating... J08");
}

void AnycubicTFTClass::BedHeatingDone()
{
  ANYCUBIC_SERIAL_PROTOCOLPGM("J09"); // J09 hotbed heating done
  ANYCUBIC_SERIAL_ENTER();
  DEBUG_PRINT_LNPGM("TFT Serial Debug: Bed heating is done... J09");
}

void AnycubicTFTClass::ConfirmUser(const char * const msg)
{

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

  

#if defined(FILAMENT_RUNOUT_SENSOR)
  if(runout.filament_ran_out)
  {
    ANYCUBIC_SERIAL_PROTOCOLPGM("J05");// J05 pausing
    ANYCUBIC_SERIAL_ENTER();
    DEBUG_PRINT_LNPGM("TFT Serial Debug: SD print paused... J05");
  }
#endif
}

#ifdef ANYCUBIC_SD_CHECK
void AnycubicTFTClass::CheckSDCardChange()
{
  #ifdef SDSUPPORT
    if (_lastSDstatus != IS_SD_INSERTED())
    {
      _lastSDstatus = IS_SD_INSERTED();

      if (_lastSDstatus)
      {
        card.mount();
        ANYCUBIC_SERIAL_PROTOCOLPGM("J00"); // J00 SD Card inserted
        ANYCUBIC_SERIAL_ENTER();
        #ifdef ANYCUBIC_TFT_DEBUG
          SERIAL_ECHOLNPGM("TFT Serial Debug: SD card inserted... J00");
        #endif
      }
      else
      {
        ANYCUBIC_SERIAL_PROTOCOLPGM("J01"); // J01 SD Card removed
        ANYCUBIC_SERIAL_ENTER();
        #ifdef ANYCUBIC_TFT_DEBUG
          SERIAL_ECHOLNPGM("TFT Serial Debug: SD card removed... J01");
        #endif

      }
    }
  #endif
}
#endif

void AnycubicTFTClass::SDCardInserted()
{
  ANYCUBIC_SERIAL_PROTOCOLPGM("J00"); // J00 SD Card inserted
  ANYCUBIC_SERIAL_ENTER();
  DEBUG_PRINT_LNPGM("TFT Serial Debug: SD card inserted... J00");
  _TFTState = ANYCUBIC_TFT_STATE_IDLE;
}
void AnycubicTFTClass::SDCardRemoved()
{
  ANYCUBIC_SERIAL_PROTOCOLPGM("J01"); // J01 SD Card removed
  ANYCUBIC_SERIAL_ENTER();
  DEBUG_PRINT_LNPGM("TFT Serial Debug: SD card removed... J01");
  _TFTState = ANYCUBIC_TFT_STATE_IDLE;
}

void AnycubicTFTClass::CheckHeatingOn()
{
  //TODO: Update the logic that it only executes one time for given heat command
  if (thermalManager.isHeatingHotend(0))
  {
    ANYCUBIC_SERIAL_PROTOCOLPGM("J06");//heating
    ANYCUBIC_SERIAL_ENTER();
  }
  else
  {
    
    ANYCUBIC_SERIAL_PROTOCOLPGM("J07");//hotend heating done
    ANYCUBIC_SERIAL_ENTER();
  }

  if(thermalManager.isHeatingBed())
  {
    ANYCUBIC_SERIAL_PROTOCOLPGM("J08");//hotbed heating
    ANYCUBIC_SERIAL_ENTER();
  }
  else
  {
    ANYCUBIC_SERIAL_PROTOCOLPGM("J09");//hotbed heating done
    ANYCUBIC_SERIAL_ENTER();
  }
}

#ifdef ANYCUBIC_ENDSTOP_BEEP

void AnycubicTFTClass::mybeep(int beepP,int beepS)
{
    if (beepS > 0)
    {
      #if BEEPER_PIN > 0
        tone(BEEPER_PIN, beepS);
        delay(beepP);
        noTone(BEEPER_PIN);
      #elif defined(ULTRALCD)
        lcd_buzz(beepS, beepP);
      #elif defined(LCD_USE_I2C_BUZZER)
        lcd_buzz(beepP, beepS);
      #endif
    }
    else
    {
      delay(beepP);
    }
}

void AnycubicTFTClass::EndstopBeep()
{
  static char last_status=((READ(X_MIN_PIN)<<3)|(READ(Y_MIN_PIN)<<2)|(READ(Y_MAX_PIN)<<1)|READ(Z_MIN_PIN));
  static unsigned char now_status,counter=0;

  now_status=((READ(X_MIN_PIN)<<3)|(READ(Y_MIN_PIN)<<2)|(READ(Y_MAX_PIN)<<1)|READ(Z_MIN_PIN))&0xff;
  if(now_status<last_status)
  {
    counter++;
    if(counter>=1000){counter=0;mybeep(60,2000);last_status=now_status;}
  }
  else if(now_status!=last_status) {counter=0;last_status=now_status;}  
} 
#endif

#ifdef ANYCUBIC_FILAMENT_RUNOUT
void AnycubicTFTClass::setup_FilamentRunout()
{
    pinMode(FIL_RUNOUT_PIN,INPUT);
    WRITE(FIL_RUNOUT_PIN,HIGH);
     _delay_ms(50);
/*
    if(READ(FIL_RUNOUT_PIN)==true)
    {
      ANYCUBIC_SERIAL_PROTOCOLPGM("J15");//j15 FILAMENT LACK
      ANYCUBIC_SERIAL_ENTER();  
      //FilamentLack();//music    
    }
*/
    _filament_runout_last_status = READ(FIL_RUNOUT_PIN);
    _filament_runout_counter = 0;
}

void AnycubicTFTClass::FilamentRunoutScan()
{
  uint8_t now_status = READ(FIL_RUNOUT_PIN)&0xff;
  if(now_status > _filament_runout_last_status) {
    _filament_runout_counter++;
    if(_filament_runout_counter>=50000) // wait for some time for stability on reading
    {
      _filament_runout_counter=0;  
      if(isSDPrintingInProgress())
      {    
        ANYCUBIC_SERIAL_PROTOCOLPGM("J23");//j23 FILAMENT LACK with the prompt box don't disappear
        ANYCUBIC_SERIAL_ENTER();                       
        PausePrint();
      }
      //else if((isPrintingFromMediaPaused()==false))
      //{                         
      //      ANYCUBIC_SERIAL_PROTOCOLPGM("J15");//j15 FILAMENT LACK
      //      ANYCUBIC_SERIAL_ENTER();     
      //}
#ifdef ANYCUBIC_MUSIC
      FilamentLack();//music 
#endif
      _filament_runout_last_status=now_status;                      
    }    
  }
  else if(now_status != _filament_runout_last_status) { 
    _filament_runout_counter=0; _filament_runout_last_status=now_status;
  }    
} 

#endif

#ifdef USB_ONLINE_DETECT
void AnycubicTFTClass::USBOnLineTest()
{
    static long int temp=0;
    if(_USBConnectFlag==false)
    {
          if(_UsbOnLineFlag==true)
          {
            temp++;
            _UsbOnLineFlag=false;
            if(temp>1)
            {              
              _USBConnectFlag=true;
              ANYCUBIC_SERIAL_PROTOCOLPGM("J03");//usb connect
              ANYCUBIC_SERIAL_ENTER(); 
              temp=0;              
            }    
          }
    }
    else if(_USBConnectFlag==true)
    {
      if(_UsbOnLineFlag==false)
      {
        temp++;
        if(temp>50000) 
        {          
          _UsbOnLineFlag=false;
          _USBConnectFlag=false;
          ANYCUBIC_SERIAL_PROTOCOLPGM("J12");//ready
          ANYCUBIC_SERIAL_ENTER();
          temp=0;
        }
      }
      else {temp=0;_UsbOnLineFlag=false;}
    }      
}
#endif

#ifdef ANYCUBIC_POWERDOWN  
void AnycubicTFTClass::setup_PowerDownPin()
{
  _PrintdoneAndPowerOFF = false;
  SET_OUTPUT(POWER_OFF_PIN); 
  WRITE(POWER_OFF_PIN,HIGH);
}
void AnycubicTFTClass::PowerDownScan()
{
  if(_PrintdoneAndPowerOFF && _powerOFFflag && int(thermalManager.degHotend(0)) < 35)
  { 
    _powerOFFflag = false;
    PowerDown();
  }
}
void AnycubicTFTClass::PowerDown()
{
  for(unsigned char i=0;i<3;i++)
  {
    WRITE(POWER_OFF_PIN,LOW);
    delay(10);
    WRITE(POWER_OFF_PIN,HIGH);
    delay(10);
  } 
}
#endif

void AnycubicTFTClass::PlayTone(const uint16_t frequency, const uint16_t duration)
{
#if defined(BEEPER_PIN)
  int beepS = CodeSeen('S') ? int(CodeValue()) : 110;
  int beepP = CodeSeen('P') ? int(CodeValue()) : 1000;
  if (beepS > 0)
  {
      tone(BEEPER_PIN, beepS);
      delay(beepP);
      noTone(BEEPER_PIN);
  }
  else
  {
    delay(beepP);
  }
#endif
}

void AnycubicTFTClass::OnPrintFinished()
{
#ifdef ANYCUBIC_POWER_OUTTAGE_TEST 
    _PowerTestFlag=false;
    WRITE(OUTAGECON_PIN,LOW);
#endif
    _TFTState=ANYCUBIC_TFT_STATE_IDLE;
#ifdef ANYCUBIC_POWERDOWN
    _powerOFFflag=true;
#endif
    ANYCUBIC_SERIAL_PROTOCOLPGM("J14");// J14 print done
    ANYCUBIC_SERIAL_ENTER();
    DEBUG_PRINT_LNPGM("StateHandler: SD print done... J14");
}

#ifdef ANYCUBIC_POWER_OUTTAGE_TEST 
void AnycubicTFTClass::setup_OutageTestPin()
{
  pinMode(OUTAGETEST_PIN,INPUT);
//  WRITE(OUTAGETEST_PIN,HIGH);
  pinMode(OUTAGECON_PIN,OUTPUT);
  WRITE(OUTAGECON_PIN,LOW);
}
void AnycubicTFTClass::SimulatePowerOuttage()
{
      WRITE(OUTAGECON_PIN,HIGH); // Trigger Powerouttage
      if(runout.filament_ran_out)
      {                
        if(MYfeedrate_mm_s==0)MYfeedrate_mm_s=2000;
        card.setIndex(last_sd_position[0]);         
        _ResumingFlag=1;     
        fanSpeeds[0]=Max_ModelCooling;   //OPEN FAN0     
        fanSpeeds[0]=179;   //OPEN FAN0     
      }     
      if(1==READ(OUTAGETEST_PIN)) // Detect Power Outtage
      {
        _PowerTestFlag=true;
        DEBUG_PRINT_LINE("G5:PowerTestFlag=TRUE");
        attachInterrupt(PowerInt,PowerKill,CHANGE);     //INITIANAL SET             
      }    
}
void AnycubicTFTClass::PowerKill()
{
  //DEBUG_PRINT_LINE("int17 be called");
  Temp_Buf_Extuder_Temperature=thermalManager.degTargetHotend(0);
  Temp_Buf_Bed_Temperature=thermalManager.degTargetBed() ;
  if(_PowerTestFlag==true)
  {
    // MYfeedrate_mm_s=feedrate_mm_s;
    thermalManager.disable_all_heaters();
    disable_x();
    disable_y();
    disable_z();
    disable_e0();
    _PowerTestFlag=false;
    thermalManager.setTargetHotend(Temp_Buf_Extuder_Temperature,0); 
    thermalManager.setTargetBed(Temp_Buf_Bed_Temperature);
  }
}
#endif

} // ExtUI namespace close

ExtUI::AnycubicTFTClass AnycubicTFT;

#endif // EXTUI_EXAMPLE && EXTENSIBLE_UI