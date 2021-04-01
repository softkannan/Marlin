
#include "../../inc/MarlinConfigPre.h"

#if ALL(ANYCUBIC_SOFT_TFT_MODEL, EXTENSIBLE_UI, ANYCUBIC_GENERIC_SERIAL)

#include "anycubic_generic_serial.h"

#if defined(__AVR__)
  #ifndef INTERNAL_SERIAL_PORT
    #error "anycubic TFT INTERNAL_SERIAL_PORT must be defined."
  #endif
#elif defined(ANYCUBIC_GENERIC_SERIAL) && defined(TARGET_LPC1768)
  #ifdef SERIAL_PORT_2
    #error "anycubic TFT uses SERIAL_PORT_2 exclusively."
  #else
    #include "../../HAL_LPC1768/MarlinSerial.h"
    MarlinSerial MSerial(LPC_UART0);
    extern "C" void UART0_IRQHandler() {
      MSerial.IRQHandler();
    }
  #endif
#else
   #error "anycubic TFT is not supported given target processor."
#endif

// Constructors ////////////////////////////////////////////////////////////////

AnycubicSerialClass::AnycubicSerialClass() {
}

// Public Methods //////////////////////////////////////////////////////////////
STREAM_SIZE_T AnycubicSerialClass::available(void) {
  return TFT_SERIAL.available();
}

STREAM_INT16_T AnycubicSerialClass::peek(void) {
  return TFT_SERIAL.peek();
}

STREAM_INT16_T AnycubicSerialClass::read(void) {
  return TFT_SERIAL.read();
}

void AnycubicSerialClass::flush() {
  TFT_SERIAL.flush();
}

size_t AnycubicSerialClass::write(uint8_t c) {
  TFT_SERIAL.write(c);
  return 1;
}

AnycubicSerialClass::operator bool() {
	return true;
}

AnycubicSerialClass AnycubicSerial;

#endif // whole file
