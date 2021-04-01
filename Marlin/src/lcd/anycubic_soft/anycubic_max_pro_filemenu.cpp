#include "../../inc/MarlinConfigPre.h"

#if BOTH(ANYCUBIC_SOFT_TFT_MODEL, EXTENSIBLE_UI)

#include "../extui/ui_api.h"

#include "anycubic_max_pro_TFT.h"
#include "anycubic_generic_serial.h"
#include "anycubic_serial.h"
#include "anycubic_music.h"

extern AnycubicSerialClass AnycubicSerial;


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

const char MENU_FANON[] PROGMEM = "<Fan ON>";
const char MENU_FANOFF[] PROGMEM = "<Fan OFF>";
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

  ,MENU_FANON
  ,MENU_FANOFF
  ,MENU_DRYRUN
  #ifdef ANYCUBIC_TFT_DEBUG_MSG
  ,MENU_NEXT_MSG
  #endif
  ,MENU_EXIT
};

extern ExtUI::FileList fileList;


namespace ExtUI {

// Called when user touches refresh button
void AnycubicTFTClass::HandleSpecialMenu()
{
  STAY_ON_PAGE_RESET();

  if (STRCASECMP_P(_lastUserSelection, MENU_DRYRUN)==0) {
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("Special Menu:DryRun");
    ENQUEUE_CMD_NOW_PGM("M111 S8");
  }
  else if (STRCASECMP_P(_lastUserSelection, MENU_FANON)==0) {
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("Special Menu: Fan On");
    INJECT_CMD_NOW_PGM("M106 S255");
    STAY_ON_PAGE();
  }
  else if (STRCASECMP_P(_lastUserSelection, MENU_FANOFF)==0) {
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("Special Menu:Fan Off");
    INJECT_CMD_NOW_PGM("M107");
    STAY_ON_PAGE();
  }
  else if (STRCASECMP_P(_lastUserSelection,MENU_TUNE_HOTEND_PID)==0) {
    STAY_ON_PAGE();
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("Special Menu: Auto Tune Hotend PID");
    INJECT_CMD_NOW_PGM("M303 E0 S240 C15 U1");
  }
  else if (STRCASECMP_P(_lastUserSelection, MENU_TUNE_BED_PID)==0) {
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("Special Menu: Auto Tune Hotbed Pid");
    INJECT_CMD_NOW_PGM("M303 E-1 S75 C5 U1");
    STAY_ON_PAGE();
  }
  else if (STRCASECMP_P(_lastUserSelection, MENU_TUNE_ESTEP_CAL_PREP)==0) {
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("Special Menu: EStep Cal Prep");
    INJECT_CMD_NOW_PGM("G21\nG90\nM83\nG1 X130 Y100 Z190 F3600\nG1 E50 F600");
    STAY_ON_PAGE();
  }
  else if (STRCASECMP_P(_lastUserSelection, MENU_TUNE_ESTEP_CAL_PERFORM)==0) {
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("Special Menu: EStep Call Perform");
    INJECT_CMD_NOW_PGM("G21\nG90\nM83\nG1 X130 Y100 Z190 F3600\nG1 E100 F100");
    STAY_ON_PAGE();
  }
  else if (STRCASECMP_P(_lastUserSelection, MENU_SAVE_SETTINGS)==0) {
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("Special Menu: Save Settings");
    INJECT_CMD_NOW_PGM("M500");
#if USE_BEEPER
    buzzer.tone(105, 1108);
    buzzer.tone(210, 1661);
#endif
    STAY_ON_PAGE();
  }
  else if (STRCASECMP_P(_lastUserSelection, MENU_RESTORE_SETTINGS)==0) {
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("Special Menu: Restore Settings");
    INJECT_CMD_NOW_PGM("M501");
#if USE_BEEPER
    buzzer.tone(105, 1661);
    buzzer.tone(210, 1108);
#endif
    STAY_ON_PAGE();
  }
  else if (STRCASECMP_P(_lastUserSelection, MENU_FACTORY_RESET)==0) {
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("Special Menu: Factory Reset");
    INJECT_CMD_NOW_PGM("M502");
#if USE_BEEPER
    buzzer.tone(105, 1661);
    buzzer.tone(210, 1108);
#endif
    STAY_ON_PAGE();
  }
  else if (STRCASECMP_P(_lastUserSelection, MENU_COLD_FILAMENT_LOAD)==0) {
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("Special Menu: Cold Filament Load");
    INJECT_CMD_NOW_PGM("M302 P1\nM83\nG1 E5 F100\nM302 P0");
    STAY_ON_PAGE();
  }
  else if (STRCASECMP_P(_lastUserSelection, MENU_COLD_FILAMENT_UNLOAD)==0) {
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("Special Menu: Cold Filament Unload");
    INJECT_CMD_NOW_PGM("M302 P1\nM83\nG1 E-5 F100\nM302 P0");
    STAY_ON_PAGE();
  }
#ifdef PREVENT_COLD_EXTRUSION
  else if (STRCASECMP_P(_lastUserSelection, MENU_DISABLE_COLD_EXTRUSION)==0) {
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("Special Menu: Disable Cold Extrusion");
    INJECT_CMD_NOW_PGM("M302 P1");
    STAY_ON_PAGE();
  }
  else if (STRCASECMP_P(_lastUserSelection, MENU_ENABLE_COLD_EXTRUSION)==0) {
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("Special Menu: Enable Cold Extrusion");
    INJECT_CMD_NOW_PGM("M302 P0");
    STAY_ON_PAGE();
  }
#endif

#if ENABLED(MESH_BED_LEVELING) && ENABLED(PROBE_MANUALLY)
  else if (STRCASECMP_P(_selectedDirectory, MENU_START_MESH_LEVELING)==0) {
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("Special Menu: Start Mesh Leveling");
    INJECT_NOW_PGM("G29 S1");
    STAY_ON_PAGE();
  }
  else if (STRCASECMP_P(_selectedDirectory, MENU_NEXT_MESH_POINT)==0) {
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("Special Menu: Next Mesh Point");
    INJECT_NOW_PGM("G29 S2");
    STAY_ON_PAGE();
  }
#endif

  else if (STRCASECMP_P(_lastUserSelection, MENU_MOVE_TO_L1)==0) {
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("Special Menu: Move To L1");
    INJECT_CMD_NOW_PGM("G21\nG90\nG1 F9000\nG1 Z10\nG1 X20 Y20\nG1 F100 Z0\nG1 F9000");
    STAY_ON_PAGE();
  }
  else if (STRCASECMP_P(_lastUserSelection, MENU_MOVE_TO_L2)==0) {
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("Special Menu: Move To L2");
    INJECT_CMD_NOW_PGM("G21\nG90\nG1 F9000\nG1 Z10\nG1 X20 Y195\nG1 F100 Z0\nG1 F9000");
    STAY_ON_PAGE();
  }
  else if (STRCASECMP_P(_lastUserSelection, MENU_MOVE_TO_R1)==0) {
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("Special Menu: Move To R1");
    INJECT_CMD_NOW_PGM("G21\nG90\nG1 F9000\nG1 Z10\nG1 X252 Y10\nG1 F100 Z0\nG1 F9000");
    STAY_ON_PAGE();
  }
  else if (STRCASECMP_P(_lastUserSelection, MENU_MOVE_TO_R2)==0) {
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("Special Menu: Move To R2");
    INJECT_CMD_NOW_PGM("G21\nG90\nG1 F9000\nG1 Z10\nG1 X252 Y195\nG1 F100 Z0\nG1 F9000");
    STAY_ON_PAGE();
  }
  else if (STRCASECMP_P(_lastUserSelection, MENU_MOVE_TO_CENTER)==0) {
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("Special Menu: Move To Center");
    INJECT_CMD_NOW_PGM("G21\nG90\nG1 F9000\nG1 Z10\nG1 X136 Y107\nG1 F100 Z0\nG1 F9000");
    STAY_ON_PAGE();
  }
  else if (STRCASECMP_P(_lastUserSelection, MENU_MOVE_TO_BOTTOM)==0) {
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("Special Menu: Move to Z Bottom");
    INJECT_CMD_NOW_PGM("G21\nG90\nG1 F9000\nG1 Z190 F9000");
    STAY_ON_PAGE();
  }
#ifdef ANYCUBIC_TFT_DEBUG_MSG
  else if (STRCASECMP_P(_selectedDirectory, MENU_NEXT_MSG)==0) {
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("Special Menu: Move To Center");
    _lastMsgId++;
    _showMsg=true;
    STAY_ON_PAGE();
  }
#endif
}

void AnycubicTFTClass::BuildFileListItems(uint16_t  startIdx)
{
  if(_lastUserSelection[0] == '<' && IS_EXEC_CMD()) //Down or Up arrow click will not go into if block
  {
      HandleSpecialMenu(); // user selected special menu and want to perform action on in it
      EXEC_CMD_COMPLETED();
  }

  ANYCUBIC_SERIAL_PROTOCOLPGM("FN "); // Filelist start
  ANYCUBIC_SERIAL_ENTER();

  if (IS_SPECIAL_MENU()) {


    if(IS_STAY_ON_PAGE()) { startIdx=_lastShownPageIdx; }
    else { _lastShownPageIdx=startIdx; }
    STAY_ON_PAGE_SHOWN();

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
        ANYCUBIC_TFT_DEBUG_PRINT(idx);
        ANYCUBIC_TFT_DEBUG_PRINT_LNP((char*)pgm_read_ptr(&(menu_table[idx])));
    }
  }
#ifdef SDSUPPORT
  else if(ExtUI::isMediaInserted())
  {
    _lastShownPageIdx = 0;
    uint16_t idx=startIdx;
    uint16_t max_files;
    uint16_t dir_files=fileList.count();

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
        if(fileList.isAtRootDir()) {
          // Only display special menu on root level
          ANYCUBIC_TFT_SPECIAL_MENU(MENU_SPECIAL_MENU);
          ANYCUBIC_TFT_DEBUG_PRINT(idx);
          ANYCUBIC_TFT_DEBUG_PRINT_PGM(":");
          ANYCUBIC_TFT_DEBUG_PRINT_LNP(MENU_SPECIAL_MENU);
        } else {
          // on sub directory level display cdup menu
          ANYCUBIC_TFT_SPECIAL_MENU(MENU_SPECIAL_LEVEL_UP);
          ANYCUBIC_TFT_DEBUG_PRINT(idx);
          ANYCUBIC_TFT_DEBUG_PRINT_PGM(":");
          ANYCUBIC_TFT_DEBUG_PRINT_LNP(MENU_SPECIAL_LEVEL_UP);
        }
      } else {
        // Go back to one cnt to account special menu entry
        fileList.seek(idx-1);
        if(fileList.isDir()) {
          // prefix directory name with forward slash
          ANYCUBIC_SERIAL_PROTOCOLPGM("/");
          ANYCUBIC_SERIAL_PROTOCOLLN(fileList.shortFilename());
          ANYCUBIC_SERIAL_PROTOCOLPGM("/");
          ANYCUBIC_SERIAL_PROTOCOLLN(fileList.filename());

          ANYCUBIC_TFT_DEBUG_PRINT(idx);
          ANYCUBIC_TFT_DEBUG_PRINT_PGM(":");
          ANYCUBIC_TFT_DEBUG_PRINT_PGM("/");
          ANYCUBIC_TFT_DEBUG_PRINT(fileList.filename());
          ANYCUBIC_TFT_DEBUG_PRINT_EOL();
        } else {
          //display file names
          ANYCUBIC_SERIAL_PROTOCOLLN(fileList.shortFilename());
          ANYCUBIC_SERIAL_PROTOCOLLN(fileList.filename());

          ANYCUBIC_TFT_DEBUG_PRINT(idx);
          ANYCUBIC_TFT_DEBUG_PRINT_PGM(":");
          ANYCUBIC_TFT_DEBUG_PRINT(fileList.filename());
          ANYCUBIC_TFT_DEBUG_PRINT_EOL();
        }
      }
    }
  }
#endif
  else {
    //if no sd card is inserted then display only special menu
    ANYCUBIC_TFT_SPECIAL_MENU(MENU_SPECIAL_MENU);
    _lastShownPageIdx = 0;
  }

  ANYCUBIC_SERIAL_PROTOCOLPGM("END"); // Filelist stop
  ANYCUBIC_SERIAL_ENTER();
  ExtUI::delay_ms(ANYCUBIC_TFT_UPDATE_LIST_DELAY_MS); // prohibits double entries
}
}

#endif // EXTUI_EXAMPLE && EXTENSIBLE_UI