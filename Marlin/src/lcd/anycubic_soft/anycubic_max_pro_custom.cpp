#include "../../inc/MarlinConfigPre.h"

#if BOTH(ANYCUBIC_SOFT_TFT_MODEL, EXTENSIBLE_UI)

#include "../extui/ui_api.h"

#include "anycubic_max_pro_TFT.h"
#include "anycubic_generic_serial.h"
#include "anycubic_serial.h"
#include "anycubic_music.h"

extern AnycubicSerialClass AnycubicSerial;

namespace ExtUI {

  void AnycubicTFTClass::KillTFT()
  {
    ANYCUBIC_SERIAL_PROTOCOLPGM("J11"); // J11 Kill
    ANYCUBIC_SERIAL_ENTER();
    ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("TFT Serial Debug: Kill command... J11");
  }

#ifdef ANYCUBIC_POWERDOWN
void AnycubicTFTClass::setup_PowerDownPin()
{
  _PrintdoneAndPowerOFF = false;
  SET_OUTPUT(POWER_OFF_PIN);
  WRITE(POWER_OFF_PIN,HIGH);
}
void AnycubicTFTClass::PowerDownScan()
{
  if(_PrintdoneAndPowerOFF && _powerOFFflag && int(ExtUI::getActualTemp_celsius(ExtUI::extruder_t::E0)) < 35)
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

void AnycubicTFTClass::PlayTone(const uint16_t duration,const uint16_t frequency)
{
  #if HAS_BUZZER
    buzzer.tone(duration, frequency);
  #endif

// #if defined(BEEPER_PIN)
//   int beepS = CodeSeen('S') ? int(CodeValue()) : 110;
//   int beepP = CodeSeen('P') ? int(CodeValue()) : 1000;
//   if (beepS > 0)
//   {
//       tone(BEEPER_PIN, beepS);
//       delay(beepP);
//       noTone(BEEPER_PIN);
//   }
//   else
//   {
//     delay(beepP);
//   }
// #endif

}

void AnycubicTFTClass::playInfoTone()
{
    PlayTone(100, NOTE_C5);
}

void AnycubicTFTClass::playSuccessTone()
{
    PlayTone(250, NOTE_C5);
    PlayTone(500, NOTE_G5);
}

void AnycubicTFTClass::playErrorTone()
{
    PlayTone(250, NOTE_G5);
    PlayTone(250, NOTE_C5);
    PlayTone(500, NOTE_G5);
}


void AnycubicTFTClass::CheckHeaterError()
{
  if ((int(ExtUI::getActualTemp_celsius(ExtUI::extruder_t::E0)) < 5) || (int(ExtUI::getActualTemp_celsius(ExtUI::extruder_t::E0)) > 290))
  {
    if (_heaterCheckCount > 60000)
    {
      _heaterCheckCount = 0;
      ANYCUBIC_SERIAL_PROTOCOLPGM("J10"); // J10 Hotend temperature abnormal
      ANYCUBIC_SERIAL_ENTER();
      ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("TFT Serial Debug: Hotend temperature abnormal... J20");
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

void AnycubicTFTClass::HeatingStart()
{
  ANYCUBIC_SERIAL_PROTOCOLPGM("J06"); // J07 hotend heating start
  ANYCUBIC_SERIAL_ENTER();
  ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("TFT Serial Debug: Nozzle is heating... J06");
}

void AnycubicTFTClass::HeatingDone()
{
  ANYCUBIC_SERIAL_PROTOCOLPGM("J07"); // J07 hotend heating done
  ANYCUBIC_SERIAL_ENTER();
  ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("TFT Serial Debug: Nozzle heating is done... J07");
}

void AnycubicTFTClass::BedHeatingStart()
{
  ANYCUBIC_SERIAL_PROTOCOLPGM("J08"); // J08 hotbed heating start
  ANYCUBIC_SERIAL_ENTER();
  ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("TFT Serial Debug: Bed is heating... J08");
}

void AnycubicTFTClass::BedHeatingDone()
{
  ANYCUBIC_SERIAL_PROTOCOLPGM("J09"); // J09 hotbed heating done
  ANYCUBIC_SERIAL_ENTER();
  ANYCUBIC_TFT_DEBUG_PRINT_LNPGM("TFT Serial Debug: Bed heating is done... J09");
}

#ifdef ANYCUBIC_ENDSTOP_BEEP
void AnycubicTFTClass::EndstopBeep()
{
  static char last_status=((READ(X_MIN_PIN)<<3)|(READ(Y_MIN_PIN)<<2)|(READ(Y_MAX_PIN)<<1)|READ(Z_MIN_PIN));
  static unsigned char now_status,counter=0;

  now_status=((READ(X_MIN_PIN)<<3)|(READ(Y_MIN_PIN)<<2)|(READ(Y_MAX_PIN)<<1)|READ(Z_MIN_PIN))&0xff;
  if(now_status<last_status)
  {
    counter++;
    if(counter>=1000){counter=0; PlayTone(60,2000);last_status=now_status;}
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

} // ExtUI namespace close

#endif // EXTUI_EXAMPLE && EXTENSIBLE_UI