
#pragma once

#include "../../inc/MarlinConfigPre.h"

#if ALL(ANYCUBIC_SOFT_TFT_MODEL, EXTENSIBLE_UI, ANYCUBIC_GENERIC_SERIAL)

#include "Stream.h"
#include "../../inc/MarlinConfig.h"

#if defined(__AVR__)
  #define TFT_SERIAL internalSerial
  #define STREAM_SIZE_T int
  #define STREAM_INT16_T int
#elif defined(ANYCUBIC_GENERIC_SERIAL) && defined(TARGET_LPC1768)
  #define STREAM_SIZE_T size_t
  #define STREAM_INT16_T int16_t
  #define TFT_SERIAL MSerial
#else
   #error "anycubic TFT is not supported given target processor."
#endif

class AnycubicSerialClass : public Stream
{
  public:
    AnycubicSerialClass();
    inline void begin(unsigned long baud) {
      TFT_SERIAL.begin(baud);
    }
    inline void end() {
      TFT_SERIAL.end();
    }
    virtual STREAM_SIZE_T available(void);
    virtual STREAM_INT16_T peek(void);
    virtual STREAM_INT16_T read(void);
    virtual void flush(void);
    virtual size_t write(uint8_t);
    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }
    inline void AnycubicSerialprintPGM(const char *str) {
      uint8_t ch=pgm_read_byte(str);
      while(ch)
      {
        write(ch);
        ch=pgm_read_byte(++str);
      }
    }
    using Print::write; // pull in write(str) and write(buf, size) from Print
    operator bool();
};

#define ANYCUBIC_SERIAL_PROTOCOL(x) (AnycubicSerial.print(x))
#define ANYCUBIC_SERIAL_PROTOCOL_F(x,y) (AnycubicSerial.print(x,y))
#define ANYCUBIC_SERIAL_PROTOCOLPGM(x) (AnycubicSerial.AnycubicSerialprintPGM(PSTR(x)))
#define ANYCUBIC_SERIAL_(x) (AnycubicSerial.print(x),AnycubicSerial.write('\n'))
#define ANYCUBIC_SERIAL_PROTOCOLLN(x) (AnycubicSerial.print(x),AnycubicSerial.AnycubicSerialprintPGM(PSTR("\r\n")))
#define ANYCUBIC_SERIAL_PROTOCOLLNPGM(x) (AnycubicSerial.AnycubicSerialprintPGM(PSTR(x "\r\n")))

#define ANYCUBIC_SERIAL_START() (AnycubicSerial.AnycubicSerialprintPGM(PSTR("\r\n"))
#define ANYCUBIC_SERIAL_CMD_SENDPGM(x) (AnycubicSerial.AnycubicSerialprintPGM(PSTR(x "\r\n")))
#define ANYCUBIC_SERIAL_ENTER() (AnycubicSerial.AnycubicSerialprintPGM(PSTR("\r\n")))
#define ANYCUBIC_SERIAL_SPACE() (AnycubicSerial.AnycubicSerialprintPGM(PSTR(" ")))

const char newErr[] PROGMEM ="ERR ";
const char newSucc[] PROGMEM ="OK";

#define ANYCUBIC_SERIAL_ERROR_START (AnycubicSerial.AnycubicSerialprintPGM(newErr))
#define ANYCUBIC_SERIAL_ERROR(x) ANYCUBIC_SERIAL_PROTOCOL(x)
#define ANYCUBIC_SERIAL_ERRORPGM(x) ANYCUBIC_SERIAL_PROTOCOLPGM(x)
#define ANYCUBIC_SERIAL_ERRORLN(x) ANYCUBIC_SERIAL_PROTOCOLLN(x)
#define ANYCUBIC_SERIAL_ERRORLNPGM(x) ANYCUBIC_SERIAL_PROTOCOLLNPGM(x)

//##define ANYCUBIC_SERIAL_ECHO_START (AnycubicSerialprintPGM(newSucc))
#define ANYCUBIC_SERIAL_ECHOLN(x) ANYCUBIC_SERIAL_PROTOCOLLN(x)
#define ANYCUBIC_SERIAL_SUCC_START (AnycubicSerial.AnycubicSerialprintPGM(newSucc))
#define ANYCUBIC_SERIAL_ECHOPAIR(name,value) (serial_echopair_P(PSTR(name),(value)))
#define ANYCUBIC_SERIAL_ECHOPGM(x) ANYCUBIC_SERIAL_PROTOCOLPGM(x)
#define ANYCUBIC_SERIAL_ECHO(x) ANYCUBIC_SERIAL_PROTOCOL(x)

#endif