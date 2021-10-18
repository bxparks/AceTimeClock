/*
 * A program to demonstrate the use of the DS3231Clock. It should print the
 * following on the SERIAL_PORT_MONITOR port every 2 seconds:
 *
 *   2021-10-18T10:28:00
 *   2021-10-18T10:28:02
 *   2021-10-18T10:28:04
 *   ...
 */

#include <Arduino.h>
#include <AceTimeClock.h>
#include <AceWire.h>
#include <Wire.h> // TwoWire, Wire

using namespace ace_time;
using namespace ace_time::clock;

// ESP32 does not define SERIAL_PORT_MONITOR
#ifndef SERIAL_PORT_MONITOR
#define SERIAL_PORT_MONITOR Serial
#endif

//-----------------------------------------------------------------------------

using WireInterface = ace_wire::TwoWireInterface<TwoWire>;
WireInterface wireInterface(Wire);
DS3231Clock<WireInterface> dsClock(wireInterface);

void printCurrentTime() {
  acetime_t now = dsClock.getNow();
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

  Wire.begin();
  dsClock.setup();
  LocalDateTime ldt = LocalDateTime::forComponents(2021, 10, 18, 10, 28, 0);
  dsClock.setNow(ldt.toEpochSeconds());
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
