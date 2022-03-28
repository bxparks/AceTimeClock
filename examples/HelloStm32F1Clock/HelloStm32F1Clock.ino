/*
 * A program to demonstrate the use of the Stm32F1Clock class. It should print
 * the following on the SERIAL_PORT_MONITOR port every 2 seconds:
 *
 *   2022-03-22T10:11:00
 *   2022-03-22T10:11:02
 *   2022-03-22T10:11:04
 *   ...
 *
 * On EpoxyDuino however, the Stf32F1Clock class is stubbed out, so the clock
 * does not actually increment. This program prints the following, which is good
 * enough for continuous integration:
 *
 *   2022-03-27T10:11:00
 *   2022-03-27T10:11:00
 *   ...
 */

#include <Arduino.h>
#include <AceTimeClock.h> // Stm32F1Clock

using ace_time::acetime_t;
using ace_time::LocalDateTime;
using ace_time::clock::Stm32F1Clock;

// ESP32 does not define SERIAL_PORT_MONITOR
#ifndef SERIAL_PORT_MONITOR
#define SERIAL_PORT_MONITOR Serial
#endif

//-----------------------------------------------------------------------------

Stm32F1Clock stmClock;

void printCurrentTime() {
  acetime_t now = stmClock.getNow();
  LocalDateTime ldt = LocalDateTime::forEpochSeconds(now);
  ldt.printTo(SERIAL_PORT_MONITOR);
  SERIAL_PORT_MONITOR.println();
}

//-----------------------------------------------------------------------------

void setup() {
#if ! defined(EPOXY_DUINO)
  delay(1000);
#endif
  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR); // Wait until ready - Leonardo/Micro
#if defined(EPOXY_DUINO)
  SERIAL_PORT_MONITOR.setLineModeUnix();
  enableTerminalEcho();
#endif

  stmClock.setup();
  LocalDateTime ldt = LocalDateTime::forComponents(2022, 3, 27, 10, 11, 0);
  stmClock.setNow(ldt.toEpochSeconds());
}

// Do NOT use delay() here.
void loop() {
  static uint16_t prevMillis;
  uint16_t nowMillis = millis();
  if ((uint16_t) (nowMillis - prevMillis) >= 2000) {
    printCurrentTime();
    prevMillis = nowMillis;
  }
}
