/*
 * A program to demonstrate the use of the StmRtcClock class. It should print
 * the following on the SERIAL_PORT_MONITOR port every 2 seconds:
 *
 *   2022-03-22T10:11:00
 *   2022-03-22T10:11:02
 *   2022-03-22T10:11:04
 *   ...
 */

#include <Arduino.h>
#include <AceTimeClock.h> // StmRtcClock
#if defined(EPOXY_DUINO)
  #include <EpoxyMockSTM32RTC.h>
#else
  #include <STM32RTC.h>
#endif

using ace_time::acetime_t;
using ace_time::LocalDateTime;
using ace_time::clock::StmRtcClock;

// ESP32 does not define SERIAL_PORT_MONITOR
#ifndef SERIAL_PORT_MONITOR
#define SERIAL_PORT_MONITOR Serial
#endif

//-----------------------------------------------------------------------------

StmRtcClock stmRtcClock;

void printCurrentTime() {
  acetime_t now = stmRtcClock.getNow();
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

  // Choose from:
  //    * STM32RTC::LSI_CLOCK (low speed internal)
  //    * STM32RTC::LSE_CLOCK (low speed external)
  //    * STM32RTC::HSE_CLOCK (high speed external)
  //    * (STM32RTC::HSI_CLOCK is not defined, don't know why)
  STM32RTC::getInstance().setClockSource(STM32RTC::HSE_CLOCK);

  // Reset clock on the first run. Subsequent runs should set this false.
  STM32RTC::getInstance().begin(true /*reset*/);

  stmRtcClock.setup();
  LocalDateTime ldt = LocalDateTime::forComponents(2022, 3, 27, 10, 11, 0);
  stmRtcClock.setNow(ldt.toEpochSeconds());
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
