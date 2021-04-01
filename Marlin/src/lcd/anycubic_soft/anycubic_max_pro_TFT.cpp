#include "../../inc/MarlinConfigPre.h"

#if BOTH(ANYCUBIC_SOFT_TFT_MODEL, EXTENSIBLE_UI)

#include "../extui/ui_api.h"
#include "anycubic_max_pro_TFT.h"
#include "anycubic_generic_serial.h"
#include "anycubic_serial.h"
#include "anycubic_music.h"

extern AnycubicSerialClass AnycubicSerial;
ExtUI::FileList fileList;

void (*softwareReset)(void) = 0;

namespace ExtUI
{
  AnycubicTFTClass::AnycubicTFTClass()
  {
  }

  void AnycubicTFTClass::Setup()
  {

    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("TFT Setup Start");

    _TFTbuflen = 0;
    _TFTbufindr = 0;
    _TFTbufindw = 0;
    _serial3_char = 0;
    _serial3_count = 0;
    _TFTstrchr_pointer = NULL;

#ifdef ANYCUBIC_MUSIC
    PowerOnMusic();
#endif

    AnycubicSerial.begin(ANYCUBIC_TFT_BAUDRATE);
    //ANYCUBIC_SERIAL_START();
    ANYCUBIC_SERIAL_PROTOCOLPGM("J17"); // J17 Main board reset
    ANYCUBIC_SERIAL_ENTER();
    ExtUI::delay_ms(10);
    ANYCUBIC_SERIAL_PROTOCOLPGM("J12"); // J12 Ready
    ANYCUBIC_SERIAL_ENTER();

#ifdef ANYCUBIC_FILAMENT_RUNOUT
    setup_FilamentRunout();
#endif

#ifdef ANYCUBIC_POWERDOWN
    setup_PowerDownPin();
#endif

    m_mStatus.status = 0;
    _lastShownPageIdx = 0;
    _lastUserSelection[0] = 0;
    _selectedFilename[0] = '\0';
    _selectedDirectory[0] = '\0';

    // Print optional message
    //SERIAL_ECHOPGM(MSG_MARLIN_AI3M);
    //SERIAL_CHAR(' ');
    //DEBUG_PRINT_LINE(CUSTOM_BUILD_VERSION);
    //ANYCUBIC_TFT_DEBUG_PRINT_EOL();

#if ENABLED(CASE_LIGHT_ENABLE)
    ExtUI::setCaseLightState(false);
#endif

#ifdef ANYCUBIC_STARTUP_CHIME
    playTone(250, NOTE_C5);
    playTone(250, NOTE_F5);
    playTone(250, NOTE_C5);
    playTone(500, NOTE_G5);
#endif

    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("TFT Setup Completed");
  }

  //Having delay has issues with receiving commands from TFT display

  // void AnycubicTFTClass::Idle()
  // {
  //   const millis_t ms = millis();
  //   static millis_t nextUpdateCheckMs = 0;

  //   if (ELAPSED(ms, nextUpdateCheckMs)) {
  //     nextUpdateCheckMs = ms + ANYCUBIC_TFT_UPDATE_INTERVAL_MS;
  //     IdleInternal();
  //   }
  // }

  void AnycubicTFTClass::Idle()
  {
    IdleInternal();
  }
  void AnycubicTFTClass::IdleInternal()
  {

    if (m_mStatus.flag.RecursiveCall)
    {
      return;
    }

    m_mStatus.flag.RecursiveCall = 1;

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

    m_mStatus.flag.RecursiveCall = 0;
  }

  void AnycubicTFTClass::CommandScan()
  {

#ifdef ANYCUBIC_TFT_DEBUG_MSG
    char value[6];
#endif

    CheckHeaterError();

    //Receive TFT Commands Start
    if (_TFTbuflen < (ANYCUBIC_TFT_BUF_SIZE - 1))
    {
      GetCommandFromTFT();
    }

    if (_TFTbuflen)
    {
      _TFTbuflen = (_TFTbuflen - 1);
      _TFTbufindr = (_TFTbufindr + 1) % ANYCUBIC_TFT_BUF_SIZE;
    }
    //Receive TFT Commands End

#ifdef ANYCUBIC_TFT_DEBUG_MSG
    if (_showMsg)
    {
      _showMsg = false;
      sprintf_P(value, PSTR("J%02d"), _lastMsgId);
      ANYCUBIC_SERIAL_PROTOCOLLN(value);
    }
#endif
  }

  void AnycubicTFTClass::GetCommandFromTFT()
  {
    while (AnycubicSerial.available() > 0 && _TFTbuflen < ANYCUBIC_TFT_BUF_SIZE)
    {
      _serial3_char = AnycubicSerial.read();
      if (_serial3_char == '\n' ||
          _serial3_char == '\r' ||
          _serial3_char == ':' ||
          _serial3_count >= (ANYCUBIC_TFT_MAX_CMD_SIZE - 1))
      {
        if (!_serial3_count)
        { //if empty line
          return;
        }

        _TFTcmdbuffer[_TFTbufindw][_serial3_count] = 0; //terminate string

        {
          if ((strchr(_TFTcmdbuffer[_TFTbufindw], 'A') != NULL))
          {
            int16_t a_command;
            _TFTstrchr_pointer = strchr(_TFTcmdbuffer[_TFTbufindw], 'A');
            a_command = ((int)((strtod(&_TFTcmdbuffer[_TFTbufindw][_TFTstrchr_pointer - _TFTcmdbuffer[_TFTbufindw] + 1], NULL))));

#ifdef ANYCUBIC_TFT_DEBUG
            if ((a_command > 7) && (a_command != 20)) // No debugging of status polls, please!
              SERIAL_ECHOLNPAIR("TFT Serial Command: ", _TFTcmdbuffer[_TFTbufindw]);
#endif
            HandleTFTCommands(a_command);
          }
#ifdef ANYCUBIC_TFT_DEBUG
          else if (strlen(_TFTcmdbuffer[_TFTbufindw]) > 0)
          {
            ANYCUBIC_TFT_DEBUG_PRINT_PAIR("TFT Not STD Command: ", _TFTcmdbuffer[_TFTbufindw]);
            ANYCUBIC_TFT_DEBUG_PRINT_EOL();
          }
#endif
          _TFTbufindw = (_TFTbufindw + 1) % ANYCUBIC_TFT_BUF_SIZE;
          _TFTbuflen += 1;
        }
        _serial3_count = 0; //clear buffer
      }
      else
      {
        _TFTcmdbuffer[_TFTbufindw][_serial3_count++] = _serial3_char;
      }
    }
  }

  void AnycubicTFTClass::HandleTFTCommands(int16_t a_command)
  {
    char *starpos = NULL;
    uint16_t tempVal = 0;
    switch (a_command)
    {

    case 0: //A0 GET HOTEND TEMP
      ANYCUBIC_SERIAL_PROTOCOLPGM("A0V ");
      ANYCUBIC_SERIAL_PROTOCOL(itostr3(int(ExtUI::getActualTemp_celsius(ExtUI::extruder_t::E0))));
      ANYCUBIC_SERIAL_ENTER();
      break;
    case 1: //A1  GET HOTEND TARGET TEMP
      ANYCUBIC_SERIAL_PROTOCOLPGM("A1V ");
      ANYCUBIC_SERIAL_PROTOCOL(itostr3(int(ExtUI::getTargetTemp_celsius(ExtUI::extruder_t::E0))));
      ANYCUBIC_SERIAL_ENTER();
      break;
    case 2: //A2 GET HOTBED TEMP
      ANYCUBIC_SERIAL_PROTOCOLPGM("A2V ");
#if TEMP_SENSOR_CHAMBER
      if (_bedOrChamber)
      {
        _bedOrChamber = false;
        ANYCUBIC_SERIAL_PROTOCOL(itostr3(int(ExtUI::getActualTemp_celsius(ExtUI::heater_t::BED))));
        _bedTargetTemp = uint8_t(ExtUI::getTargetTemp_celsius(ExtUI::heater_t::BED));
      }
      else
      {
        _bedOrChamber = true;
        _bedTargetTemp = 0;
        //ANYCUBIC_SERIAL_PROTOCOL(itostr3(int(ExtUI::getActualTemp_celsius(ExtUI::heater_t::CHAMBER))));
        ANYCUBIC_SERIAL_PROTOCOL(itostr3(int(thermalManager.degChamber() + 0.5)));
      }
#else
      ANYCUBIC_SERIAL_PROTOCOL(itostr3(int(ExtUI::getActualTemp_celsius(ExtUI::heater_t::BED))));
#endif
      ANYCUBIC_SERIAL_ENTER();
      break;
    case 3: //A3 GET HOTBED TARGET TEMP
      ANYCUBIC_SERIAL_PROTOCOLPGM("A3V ");
#if TEMP_SENSOR_CHAMBER
      ANYCUBIC_SERIAL_PROTOCOL(itostr2(_bedTargetTemp));
#else
      ANYCUBIC_SERIAL_PROTOCOL(itostr3(int(ExtUI::getTargetTemp_celsius(ExtUI::heater_t::BED))));
#endif
      ANYCUBIC_SERIAL_ENTER();
      break;
    case 4: //A4 GET FAN SPEED
      ANYCUBIC_SERIAL_PROTOCOLPGM("A4V ");
      ANYCUBIC_SERIAL_PROTOCOL(int(ExtUI::getActualFan_percent(ExtUI::fan_t::FAN0)));
      ANYCUBIC_SERIAL_ENTER();
      break;
    case 5: // A5 GET CURRENT COORDINATE
      ANYCUBIC_SERIAL_PROTOCOLPGM("A5V");
      ANYCUBIC_SERIAL_SPACE();
      ANYCUBIC_SERIAL_PROTOCOLPGM("X: ");
      ANYCUBIC_SERIAL_PROTOCOL(ExtUI::getAxisPosition_mm(ExtUI::axis_t::X));
      ANYCUBIC_SERIAL_SPACE();
      ANYCUBIC_SERIAL_PROTOCOLPGM("Y: ");
      ANYCUBIC_SERIAL_PROTOCOL(ExtUI::getAxisPosition_mm(ExtUI::axis_t::Y));
      ANYCUBIC_SERIAL_SPACE();
      ANYCUBIC_SERIAL_PROTOCOLPGM("Z: ");
      ANYCUBIC_SERIAL_PROTOCOL(ExtUI::getAxisPosition_mm(ExtUI::axis_t::Z));
      ANYCUBIC_SERIAL_SPACE();
      ANYCUBIC_SERIAL_ENTER();
      break;
    case 6: //A6 GET SD CARD PRINTING STATUS
      if (ExtUI::isPrinting())
      {
        ANYCUBIC_SERIAL_PROTOCOLPGM("A6V ");
        ANYCUBIC_SERIAL_PROTOCOL(itostr3(ExtUI::getProgress_percent())); // return print percent
      }
      else
      {
        ANYCUBIC_SERIAL_PROTOCOLPGM("A6V ---"); // return No Printing
      }
      ANYCUBIC_SERIAL_ENTER();
      break;
    case 7: //A7 GET PRINTING TIME
    {
      ANYCUBIC_SERIAL_PROTOCOLPGM("A7V ");
      duration_t elapsed = print_job_timer.duration();
      if (elapsed.second() != 0) // print time
      {
        ANYCUBIC_SERIAL_PROTOCOL(itostr2(elapsed.hour()));
        ANYCUBIC_SERIAL_SPACE();
        ANYCUBIC_SERIAL_PROTOCOLPGM("H");
        ANYCUBIC_SERIAL_SPACE();
        ANYCUBIC_SERIAL_PROTOCOL(itostr2(elapsed.minute() - (elapsed.hour() * 60)));
        ANYCUBIC_SERIAL_SPACE();
        ANYCUBIC_SERIAL_PROTOCOLPGM("M");
      }
      else
      {
        ANYCUBIC_SERIAL_SPACE();
        ANYCUBIC_SERIAL_PROTOCOLPGM("999:999");
      }
      ANYCUBIC_SERIAL_ENTER();
    }
    break;
    case 8: // A8 GET SD LIST (at launch / back button press / down arrow / up arrow button press / after selection refresh button press)
      ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("TFT Serial Debug:GET SD LIST... A8");
      tempVal = 0;
      if (CodeSeen('S'))
      {
        tempVal = CodeValue();
      }
      BuildFileListItems(tempVal);
      break;
    case 9: // A9 pause sd print
      if (isWaitingForUserConfirm())
      {                                     //workaround for confirm user
        ANYCUBIC_SERIAL_PROTOCOLPGM("J18"); //Status: Pause
        ANYCUBIC_SERIAL_ENTER();
      }
      else
      {
        PauseSDPrint();
      }
      break;
    case 10: // A10 resume sd print
      if (isWaitingForUserConfirm())
      { //workaround for confirm user
        playSuccessTone();
        ExtUI::setUserConfirmed();
      }
      else
      {
        ResumeSDPrint();
      }
      break;
    case 11: // A11 STOP SD PRINT
      StopSDPrint();
      break;
    case 12: // A12 kill
      //kill(PSTR(MSG_ERR_KILLED));
      {
        ANYCUBIC_SERIAL_CMD_SENDPGM("J11");
        ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("TFT Serial Debug: Kill command... J11");
#ifdef ANYCUBIC_POWERDOWN
        PowerDown();
#endif
      }
      break;
    case 13: // A13 SELECTION FILE (By toucing in touch pad / user makes selection by touching item in screen)
    {
      ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("TFT Serial Debug: SELECTION FILE... A13");
      starpos = (strchr(_TFTstrchr_pointer + ANYCUBIC_TFT_PAGE_ITEM_START, '*'));
      _lastUserSelection[0] = 0;
      if (_TFTstrchr_pointer[ANYCUBIC_TFT_PAGE_ITEM_START] == '/' &&
          _TFTstrchr_pointer[ANYCUBIC_TFT_PAGE_ITEM_START + 1] == '.' &&
          _TFTstrchr_pointer[ANYCUBIC_TFT_PAGE_ITEM_START + 2] == '.')
      { //dir up
        ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("Dir UP");
        fileList.upDir();
      }
      else if (_TFTstrchr_pointer[ANYCUBIC_TFT_PAGE_ITEM_START] == '/')
      {
        _selectedDirectory[0] = 0;
        strcpy(_selectedDirectory, _TFTstrchr_pointer + ANYCUBIC_TFT_PAGE_ITEM_START + 1); // Truncate first character on directory list item
        ANYCUBIC_TFT_DEBUG_PRINT_PGM("Dir Selection:");
        ANYCUBIC_TFT_DEBUG_PRINT(_selectedDirectory);
        ANYCUBIC_TFT_DEBUG_PRINT_EOL();
        fileList.changeDir(_selectedDirectory);
        ANYCUBIC_SERIAL_ECHOLN(_selectedDirectory);
      }
      else if (_TFTstrchr_pointer[ANYCUBIC_TFT_PAGE_ITEM_START] == '<')
      { // Look for special character at begining of file name to detect user touched special menu
        // user touched special menu / it will not refresh list automatically, for that we have to rely on user explicitly pressing the refresh button
        strcpy(_lastUserSelection, _TFTstrchr_pointer + ANYCUBIC_TFT_PAGE_ITEM_START);
        if (strcasecmp_P(_lastUserSelection, MENU_SPECIAL_MENU) == 0)
        {
          // This Enters the special menu, user must press exit to exit
          ENTER_SPECIAL_MENU();
          STAY_ON_PAGE_RESET();
          _lastShownPageIdx = 0;
          _lastUserSelection[0] = 0;
        }
        else if (STRCASECMP_P(_lastUserSelection, MENU_EXIT) == 0)
        {
          EXIT_SPECIAL_MENU();
          STAY_ON_PAGE_RESET();
          _lastShownPageIdx = 0;
          _lastUserSelection[0] = 0;
        }
        else
        {
          REQUEST_EXEC_CMD();
        }
        ANYCUBIC_SERIAL_ECHOLN(_lastUserSelection);
        //HandleSpecialMenu(); // user selected special menu and want to perform action on in it
        ANYCUBIC_TFT_DEBUG_PRINT_PGM("SpecialMenu Selection:");
        ANYCUBIC_TFT_DEBUG_PRINT(_lastUserSelection);
        ANYCUBIC_TFT_DEBUG_PRINT_EOL();
      }
      else
      {
        _selectedFilename[0] = 0;
        if (starpos != NULL)
        {
          *(starpos - 1) = '\0';
        }
        strcpy(_selectedFilename, _TFTstrchr_pointer + ANYCUBIC_TFT_PAGE_ITEM_START);
        // bellow action will open the file in card and make current selected file ready for print
        card.openFileRead(_selectedFilename);
        ANYCUBIC_SERIAL_ECHOLN(_selectedFilename);
        if (card.isFileOpen())
        {
          ANYCUBIC_SERIAL_PROTOCOLPGM("J20"); // J20 Open successful, Switch Screen to File Browser: Print avalible
          ANYCUBIC_SERIAL_ENTER();
          ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("TFT Serial Debug: File open successful... J20");
        }
        else
        {
          ANYCUBIC_SERIAL_PROTOCOLPGM("J21"); // J21 Open failed, Switch Screen to File Browser: Print not avalible
          ANYCUBIC_SERIAL_ENTER();
          ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("TFT Serial Debug: File open failed... J21");
        }
      }
      ANYCUBIC_SERIAL_ENTER();
    }
    break;
    case 14: // A14 START PRINTING
      StartSDPrint();
      break;
    case 15: // A15 RESUMING FROM OUTAGE / After file is selected then user hits resume button instead of print button
      ResumePrintFromOutage();
      break;
    case 16: // A16 set hotend temp
      if (!ExtUI::isPrinting() && !ExtUI::isMoving())
      {
        unsigned int tempvalue;
        if (CodeSeen('S'))
        {
          tempvalue = constrain(CodeValue(), 0, 275);
          ExtUI::setTargetTemp_celsius(float(tempvalue), ExtUI::extruder_t::E0);
        }
        else if ((CodeSeen('C')) && (!planner.movesplanned()))
        {
          if (ExtUI::getAxisPosition_mm(ExtUI::axis_t::Z) < 10)
          {
            ENQUEUE_CMD_NOW_PGM("G1 Z10"); //RASE Z AXIS
          }
          tempvalue = constrain(CodeValue(), 0, 275);
          ExtUI::setTargetTemp_celsius(float(tempvalue), ExtUI::extruder_t::E0);
        }
      }
      ANYCUBIC_SERIAL_ENTER();
      break;
    case 17: // A17 set heated bed temp
      if (!ExtUI::isPrinting() && !ExtUI::isMoving())
      {
        unsigned int tempbed;
        if (CodeSeen('S'))
        {
          tempbed = constrain(CodeValue(), 0, 110);
          ExtUI::setTargetTemp_celsius(float(tempbed), ExtUI::heater_t::BED);
        }
      }
      ANYCUBIC_SERIAL_ENTER();
      break;
    case 18: // A18 set fan speed
      if (!ExtUI::isPrinting() && !ExtUI::isMoving())
      {
        if (CodeSeen('S'))
        {
          ExtUI::setTargetFan_percent(float(CodeValue()), ExtUI::fan_t::FAN0);
        }
        else
        {
          ExtUI::setTargetFan_percent(float(100), ExtUI::fan_t::FAN0);
        }
      }
      ANYCUBIC_SERIAL_ENTER();
      break;
    case 19: // A19 stop stepper drivers
      if (!ExtUI::isPrinting() && !ExtUI::isMoving())
      {
        quickstop_stepper();
        disable_all_steppers();
      }
      ANYCUBIC_SERIAL_ENTER();
      break;
    case 20: // A20 read printing speed
    {
      if (CodeSeen('S'))
      {
        ExtUI::setFeedrate_percent(float(constrain(CodeValue(), 40, 999)));
      }
      else
      {
        ANYCUBIC_SERIAL_PROTOCOLPGM("A20V ");
        ANYCUBIC_SERIAL_PROTOCOL(int(ExtUI::getFeedrate_percent()));
        ANYCUBIC_SERIAL_ENTER();
      }
    }
    break;
    case 21: // A21 all home
      if (!ExtUI::isPrinting() && !ExtUI::isMoving())
      {
        if (CodeSeen('X') || CodeSeen('Y') || CodeSeen('Z'))
        {
          if (CodeSeen('X'))
            ENQUEUE_CMD_NOW_PGM("G28 X");
          if (CodeSeen('Y'))
            ENQUEUE_CMD_NOW_PGM("G28 Y");
          if (CodeSeen('Z'))
            ENQUEUE_CMD_NOW_PGM("G28 Z");
        }
        else if (CodeSeen('C'))
        {
          ENQUEUE_CMD_NOW_PGM("G28");
        }
      }
      break;
    case 22: // A22 move X/Y/Z or extrude
      if (!ExtUI::isPrinting() && !ExtUI::isMoving())
      {
        float coorvalue;
        unsigned int movespeed = 0;
        char value[30];
        if (CodeSeen('F')) // Set feedrate
          movespeed = CodeValue();

        ENQUEUE_CMD_NOW_PGM("G91"); // relative coordinates

        if (CodeSeen('X')) // Move in X direction
        {
          coorvalue = CodeValue();
          movespeed = int(ExtUI::getAxisMaxFeedrate_mm_s(ExtUI::axis_t::X) * 60 / 3000 * movespeed);
          if ((coorvalue <= 0.2) && coorvalue > 0)
          {
            sprintf_P(value, PSTR("G1 X0.1F%i"), movespeed);
          }
          else if ((coorvalue <= -0.1) && coorvalue > -1)
          {
            sprintf_P(value, PSTR("G1 X-0.1F%i"), movespeed);
          }
          else
          {
            sprintf_P(value, PSTR("G1 X%iF%i"), int(coorvalue), movespeed);
          }
          ENQUEUE_CMD_NOW(value);
        }
        else if (CodeSeen('Y')) // Move in Y direction
        {
          coorvalue = CodeValue();
          movespeed = int(ExtUI::getAxisMaxFeedrate_mm_s(ExtUI::axis_t::Y) * 60 / 3000 * movespeed);
          if ((coorvalue <= 0.2) && coorvalue > 0)
          {
            sprintf_P(value, PSTR("G1 Y0.1F%i"), movespeed);
          }
          else if ((coorvalue <= -0.1) && coorvalue > -1)
          {
            sprintf_P(value, PSTR("G1 Y-0.1F%i"), movespeed);
          }
          else
          {
            sprintf_P(value, PSTR("G1 Y%iF%i"), int(coorvalue), movespeed);
          }
          ENQUEUE_CMD_NOW(value);
        }
        else if (CodeSeen('Z')) // Move in Z direction
        {
          coorvalue = CodeValue();
          movespeed = int(ExtUI::getAxisMaxFeedrate_mm_s(ExtUI::axis_t::Z) * 60 / 3000 * movespeed);
          if ((coorvalue <= 0.2) && coorvalue > 0)
          {
            sprintf_P(value, PSTR("G1 Z0.1F%i"), movespeed);
          }
          else if ((coorvalue <= -0.1) && coorvalue > -1)
          {
            sprintf_P(value, PSTR("G1 Z-0.1F%i"), movespeed);
          }
          else
          {
            sprintf_P(value, PSTR("G1 Z%iF%i"), int(coorvalue), movespeed);
          }
          ENQUEUE_CMD_NOW(value);
        }
        else if (CodeSeen('E')) // Extrude
        {
          coorvalue = CodeValue();
          if ((coorvalue <= 0.2) && coorvalue > 0)
          {
            sprintf_P(value, PSTR("G1 E0.1F%i"), movespeed);
          }
          else if ((coorvalue <= -0.1) && coorvalue > -1)
          {
            sprintf_P(value, PSTR("G1 E-0.1F%i"), movespeed);
          }
          else
          {
            sprintf_P(value, PSTR("G1 E%iF500"), int(coorvalue));
          }
          ENQUEUE_CMD_NOW(value);
        }
        ENQUEUE_CMD_NOW_PGM("G90"); // absolute coordinates
      }
      ANYCUBIC_SERIAL_ENTER();
      break;
    case 23: // A23 preheat pla
      if (!ExtUI::isPrinting() && !ExtUI::isMoving())
      {
        if (ExtUI::getAxisPosition_mm(ExtUI::axis_t::Z) < 10)
          ENQUEUE_CMD_NOW_PGM("G1 Z10"); // RAISE Z AXIS
        ExtUI::setTargetTemp_celsius(float(PREHEAT_PLA_TEMP_HOTEND), ExtUI::extruder_t::E0);
        ExtUI::setTargetTemp_celsius(float(PREHEAT_PLA_TEMP_BED), ExtUI::heater_t::BED);
        ANYCUBIC_SERIAL_SUCC_START;
        ANYCUBIC_SERIAL_ENTER();
      }
      break;
    case 24: // A24 preheat abs
      if (!ExtUI::isPrinting() && !ExtUI::isMoving())
      {
        if (ExtUI::getAxisPosition_mm(ExtUI::axis_t::Z) < 10)
          ENQUEUE_CMD_NOW_PGM("G1 Z10"); //RAISE Z AXIS

        ExtUI::setTargetTemp_celsius(float(PREHEAT_ABS_TEMP_HOTEND), ExtUI::extruder_t::E0);
        ExtUI::setTargetTemp_celsius(float(PREHEAT_ABS_TEMP_BED), ExtUI::heater_t::BED);

        ANYCUBIC_SERIAL_SUCC_START;
        ANYCUBIC_SERIAL_ENTER();
      }
      break;
    case 25: // A25 cool down
      if (!ExtUI::isPrinting() && !ExtUI::isMoving())
      {
        ExtUI::setTargetTemp_celsius(float(0), ExtUI::extruder_t::E0);
        ExtUI::setTargetTemp_celsius(float(0), ExtUI::heater_t::BED);

        if (ExtUI::getAxisPosition_mm(ExtUI::axis_t::Z) < 10)
          ENQUEUE_CMD_NOW_PGM("G1 Z10"); //RAISE Z AXIS

        ANYCUBIC_SERIAL_PROTOCOLPGM("J12"); // J12 cool down
        ANYCUBIC_SERIAL_ENTER();
        ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("TFT Serial Debug: Cooling down... J12");
      }
      break;
    case 26: // A26 refresh SD (user touch refresh key), always TFT display will issue A8S command to get new list after refresh button press
             // On special menu case it will act like enter button
      ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("TFT Serial Debug: Refresh SD... A26");
      if (_lastUserSelection[0] == '<')
      { // Top Level user refresh
        REQUEST_EXEC_CMD();
      }
      else if (_lastUserSelection[0] == 0)
      {
        //IFSD(card.mount(), "");
      }
      ANYCUBIC_SERIAL_ENTER();
      //ExtUI::delay_ms(ANYCUBIC_TFT_UPDATE_LIST_DELAY_MS); // prohibits double entries
      break;
    case 27: // A27 servos angles  adjust
      break;
    case 28: // A28 filament test
    {
      if (CodeSeen('O'))
        ;
      else if (CodeSeen('C'))
        ;
    }
      ANYCUBIC_SERIAL_ENTER();
      break;
    case 29: // A29 Z PROBE OFFESET SET
      break;
    case 30: // A30 assist leveling, the original function was canceled
      break;
    case 31: // A31 zoffset
      break;
    case 32: // A32 clean leveling beep flag
      break;
    case 33: // A33 get version info
    {
      ANYCUBIC_SERIAL_PROTOCOLPGM("J33 ");
      ANYCUBIC_SERIAL_PROTOCOLPGM(MSG_MY_VERSION);
      ANYCUBIC_SERIAL_ENTER();
    }
    break;
    case 40: //a40 reset mainboard
      softwareReset();
      break;
#ifdef ANYCUBIC_POWERDOWN
    case 41: //A41 continue button pressed
    {
      if (CodeSeen('O'))
      {
        _PrintdoneAndPowerOFF = true;
        break;
      }
      else if (CodeSeen('C'))
      {
        _PrintdoneAndPowerOFF = false;
        break;
      }
      if (CodeSeen('S'))
      {
        if (_PrintdoneAndPowerOFF)
        {
          ANYCUBIC_SERIAL_PROTOCOLPGM("J35 ");
          ANYCUBIC_SERIAL_ENTER();
        }
        else //didn't open print done and auto power off
        {
          ANYCUBIC_SERIAL_PROTOCOLPGM("J34 ");
          ANYCUBIC_SERIAL_ENTER();
        }
      }
    }
    break;
#endif
#if ENABLED(CASE_LIGHT_ENABLE)
    case 42: //A42 light
    {
      if(ExtUI::getCaseLightState())
      {
        ExtUI::setCaseLightState(false);
      }
      else
      {
        ExtUI::setCaseLightState(true);
      }
      // if (CodeSeen('O'))
      // {
      //   ExtUI::setCaseLightState(true);
      // }
      // else if (CodeSeen('C'))
      // {
      //   ExtUI::setCaseLightState(false);
      // }
    }
      ANYCUBIC_SERIAL_ENTER();
      break;
#endif
    default:
      ANYCUBIC_TFT_DEBUG_PRINT_PAIR("TFT Not Handled Command: ", _TFTcmdbuffer[_TFTbufindw]);
      ANYCUBIC_TFT_DEBUG_PRINT_EOL();
      break;
    }
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

} // namespace ExtUI

ExtUI::AnycubicTFTClass AnycubicTFT;

#endif // EXTUI_EXAMPLE && EXTENSIBLE_UI