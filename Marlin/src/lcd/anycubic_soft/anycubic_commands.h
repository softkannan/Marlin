
#pragma once
#include "../../inc/MarlinConfigPre.h"

#if BOTH(ANYCUBIC_SOFT_TFT_MODEL, EXTENSIBLE_UI)

#if ENABLED(SDSUPPORT) && !defined(IFSD)
#define IFSD(A, B) (A)
#elif !defined(IFSD)
#define IFSD(A, B) (B)
#endif

#if HAS_FILAMENT_SENSOR
#define IFOOF(A, B) (A)
#else
#define IFOOF(A, B) (B)
#endif

#if ENABLED(ADVANCED_PAUSE_FEATURE)
#define IFADVPAUSE(A, B) (A)
#else
#define IFADVPAUSE(A, B) (B)
#endif

#ifdef ANYCUBIC_TFT_DEBUG
  #define ANYCUBIC_TFT_DEBUG_PRINT_PAIR(V...)            _SEP_N(NUM_ARGS(V),V)
  #define ANYCUBIC_TFT_DEBUG_PRINT(x)                    SERIAL_ECHO(x)
  #define ANYCUBIC_TFT_DEBUG_PRINT_PGM(x)                SERIAL_ECHOPGM(x)
  #define ANYCUBIC_TFT_DEBUG_PRINT_EOL()                 SERIAL_EOL()
  #define ANYCUBIC_TFT_DEBUG_PRINT_LNPGM(x)                           SERIAL_ECHOLNPGM(x)
  #define ANYCUBIC_TFT_DEBUG_PRINT_LNP(x)                             (serialprintPGM(x),serialprintPGM(PSTR("\r\n")))
#else
  #define ANYCUBIC_TFT_DEBUG_PRINT_PAIR(V...)
  #define ANYCUBIC_TFT_DEBUG_PRINT(x)
  #define ANYCUBIC_TFT_DEBUG_PRINT_PGM(x)
  #define ANYCUBIC_TFT_DEBUG_PRINT_EOL()
  #define ANYCUBIC_TFT_DEBUG_PRINT_LNPGM(x)
  #define ANYCUBIC_TFT_DEBUG_PRINT_LNP(x)
#endif

#define ENQUEUE_CMD_NOW_PGM(x)              queue.enqueue_now_P(PSTR(x))
#define ENQUEUE_CMD_NOW(x)                  queue.enqueue_one_now(x)
#define INJECT_CMD_NOW_PGM(x)               queue.inject_P(PSTR(x))
#define ENQUEUE_CMD_PGM(x)                  queue.enqueue_one_P(PSTR(x))

#define DIGIT(n) ('0' + (n))
#define DIGIMOD(n, f) DIGIT((n)/(f) % 10)
#define RJDIGIT(n, f) ((n) >= (f) ? DIGIMOD(n, f) : ' ')
#define MINUSOR(n, alt) (n >= 0 ? (alt) : (n = -n, '-'))

// TFT Buffer and other defs
#define ANYCUBIC_TFT_BAUDRATE           115200
#define ANYCUBIC_TFT_BUF_SIZE                4
#define ANYCUBIC_TFT_MAX_CMD_SIZE           96
#define ANYCUBIC_TFT_PAGE_SIZE               4 // TFT Lists 4 items per page
#define ANYCUBIC_TFT_PAGE_ITEM_START         4 // starting character of TFT selection list item in buffer
#define ANYCUBIC_MAX_SELECTED_DIR_LEN       30
#define ANYCUBIC_TFT_UPDATE_INTERVAL_MS     50 //ms
#define ANYCUBIC_TFT_UPDATE_LIST_DELAY_MS  250 //ms

//TFT Send commands
#define ANYCUBIC_TFT_TX_SD_CARD_INSERTED "J00" //Status: SD Card inserted
#define ANYCUBIC_TFT_TX_SD_CARD_REMOVED "J01" //Alert: No SD Card
#define ANYCUBIC_TFT_TX_SD_CARD_NOT_INSERTED "J02" //Alert: No SD Card
#define ANYCUBIC_TFT_TX_PRINT_VIA_HOST "J03" //Status: USB Host online
#define ANYCUBIC_TFT_TX_PRINT_RESUME "J04" //Status: Print from SD Card
#define ANYCUBIC_TFT_TX_PRINT_PAUSE "J05" //Alert: Command has been sent, wait for response
#define ANYCUBIC_TFT_TX_HOTEND_HEATING_START "J06" //Status: Nozzle heating
#define ANYCUBIC_TFT_TX_HOTEND_HEATING_END "J07" //Status: Nozzle heating done
#define ANYCUBIC_TFT_TX_HOTBED_HEATING_START "J08" //Status: Bed heating
#define ANYCUBIC_TFT_TX_HOTBED_HEATING_END "J09" //Status: Bed heating done
#define ANYCUBIC_TFT_TX_HOTEND_ABNORMAL "J10" //Alert: T1 Sensor abnormal
#define ANYCUBIC_TFT_TX_KILL "J11"
#define ANYCUBIC_TFT_TX_READY "J12" //Status: Ready
#define ANYCUBIC_TFT_TX_HOTEND_TEMP_LOW "J13" //Status: Low E0 Temp
#define ANYCUBIC_TFT_TX_PRINT_FINISHED "J14" //Alert: Print done
//#define ANYCUBIC_TFT_TX_FILAMENT_RUNOUT "J15" //Alert: Lack of filament //replaced by J23
#define ANYCUBIC_TFT_TX_PRINT_STOPPED "J16" //Switch Screen to Status: Stopped
#define ANYCUBIC_TFT_TX_MAINBOARD_RESET "J17" //Status: Stop
#define ANYCUBIC_TFT_TX_PRINT_PAUSE_REQ "J18" //Status: Pause
#define ANYCUBIC_TFT_TX_SD_CARD_OPEN_SUCCESS "J20" //Switch Screen to File Browser: Print avalible
#define ANYCUBIC_TFT_TX_SD_CARD_OPEN_FAILED "J21" //Switch Screen to File Browser: Print not avalible
#define ANYCUBIC_TFT_TX_FILAMENT_RUNOUT "J23" //Alert: Lack of filament
#define ANYCUBIC_TFT_TX_VERSION_INFO "J33 "
#define ANYCUBIC_TFT_TX_AUTO_SHUTDOWN_STATUS_OFF "J34 "
#define ANYCUBIC_TFT_TX_AUTO_SHUTDOWN_STATUS_ON "J35 "
#define ANYCUBIC_TFT_TX_SD_CARD_FILE_LIST_START "FN "
#define ANYCUBIC_TFT_TX_SD_CARD_FILE_LIST_END "END"
#define ANYCUBIC_TFT_TX_PRINT_SPEED "A20V "
#define ANYCUBIC_TFT_TX_PRINTING_TIME "A7V "
#define ANYCUBIC_TFT_TX_PRINTING_STATUS "A6V "
#define ANYCUBIC_TFT_TX_CURRENT_COORDINATES "A5V"
#define ANYCUBIC_TFT_TX_FAN_SPEED "A4V "
#define ANYCUBIC_TFT_TX_HOTBED_TARGET_TEMP "A3V "
#define ANYCUBIC_TFT_TX_HOTBED_TEMP "A2V "
#define ANYCUBIC_TFT_TX_HOTEND_TARGET_TEMP "A1V "
#define ANYCUBIC_TFT_TX_HOTEND_TEMP "A0V "

#define ANYCUBIC_TFT_RX_CODE_ENABLE 'O'
#define ANYCUBIC_TFT_RX_CODE_DISABLE 'C'
#define ANYCUBIC_TFT_RX_CODE_STATUS 'S'


// OnConfirm Message defs
#define ANYCUBIC_TFT_LOAD_FILAMENT_MSG              "Load Filament"
#define ANYCUBIC_TFT_PURGE_FILAMENT_MSG             "Filament Purge Running..."
#define ANYCUBIC_TFT_NOZZLE_PARKED_MSG              "Nozzle Parked"
#define ANYCUBIC_TFT_HEATER_TIMEOUT_MSG             "HeaterTimeout"
#define ANYCUBIC_TFT_REHEAT_FINISHED_MSG            "Reheat finished."
#define ANYCUBIC_TFT_M43_WAIT_MSG                   "M43 Wait Called"
#define ANYCUBIC_TFT_MMU2_EJECT_RECOVER             "MMU2 Eject Recover"
#define ANYCUBIC_TFT_USER_WAIT_MSG                   Language::MSG_USERWAIT

const char MENU_SPECIAL_MENU[] PROGMEM = "<Special_Menu>"; // while print is inprogress user cannot access special menu
const char MENU_SPECIAL_LEVEL_UP[] PROGMEM = "/..";
const char MENU_EXIT[] PROGMEM = "<Exit>";

#endif