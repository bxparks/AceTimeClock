/*
 * A program to determine how long it takes to execute some of the more complex
 * methods of ZonedDateTime and LocalDate.
 *
 * This should compile on all microcontrollers supported by the Arduino IDE.
 */

#include <Arduino.h>
#include <AceRoutine.h> // activate SystemClock coroutines
#include <AceTimeClock.h>
#include <AceWire.h> // SimpleWireInterface
#include "Benchmark.h"

using namespace ace_time::clock;
using ace_wire::SimpleWireInterface;

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
