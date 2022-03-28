/*
 * A program to determine how long it takes to execute certain features of the
 * AceTimeClock classes.
 */

#include <Arduino.h>
#include <AceRoutine.h> // activate SystemClock coroutines
#include <AceTimeClock.h>
#include <AceWire.h> // SimpleWireInterface
#include "Benchmark.h"

using namespace ace_time::clock;
using ace_wire::SimpleWireInterface;

// ESP32 does not define SERIAL_PORT_MONITOR
#ifndef SERIAL_PORT_MONITOR
#define SERIAL_PORT_MONITOR Serial
#endif

void setup() {
#if ! defined(EPOXY_DUINO)
  delay(1000);
#endif

  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR); // Wait until ready - Leonardo/Micro

  SERIAL_PORT_MONITOR.println(F("SIZEOF"));

  SERIAL_PORT_MONITOR.print(F("sizeof(DS3231Clock): "));
  SERIAL_PORT_MONITOR.println(sizeof(DS3231Clock<SimpleWireInterface>));

#if ! defined(EPOXY_DUINO)
  #if defined(ESP8266) || defined(ESP32)
    SERIAL_PORT_MONITOR.print(F("sizeof(NtpClock): "));
    SERIAL_PORT_MONITOR.println(sizeof(NtpClock));

    SERIAL_PORT_MONITOR.print(F("sizeof(EspSntpClock): "));
    SERIAL_PORT_MONITOR.println(sizeof(EspSntpClock));

  #elif defined(ARDUINO_ARCH_STM32)
    SERIAL_PORT_MONITOR.print(F("sizeof(StmRtcClock): "));
    SERIAL_PORT_MONITOR.println(sizeof(StmRtcClock));

    SERIAL_PORT_MONITOR.print(F("sizeof(Stm32F1Clock): "));
    SERIAL_PORT_MONITOR.println(sizeof(Stm32F1Clock));

  #endif
#endif

  SERIAL_PORT_MONITOR.print(F("sizeof(SystemClock): "));
  SERIAL_PORT_MONITOR.println(sizeof(SystemClock));

  SERIAL_PORT_MONITOR.print(F("sizeof(SystemClockLoop): "));
  SERIAL_PORT_MONITOR.println(sizeof(SystemClockLoop));

  SERIAL_PORT_MONITOR.print(F("sizeof(SystemClockCoroutine): "));
  SERIAL_PORT_MONITOR.println(sizeof(SystemClockCoroutine));

  SERIAL_PORT_MONITOR.println(F("BENCHMARKS"));
  runBenchmarks();
  SERIAL_PORT_MONITOR.println(F("END"));

#if defined(EPOXY_DUINO)
  exit(0);
#endif
}

void loop() {
}
