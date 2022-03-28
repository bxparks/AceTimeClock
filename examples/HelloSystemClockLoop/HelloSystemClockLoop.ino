/*
 * A program to demonstrate the use of SystemClockLoop. It should print the
 * following on the SERIAL_PORT_MONITOR port every 2 seconds:
 *
 *   2019-06-17T19:50:00-07:00[America/Los_Angeles]
 *   2019-06-17T19:50:02-07:00[America/Los_Angeles]
 *   2019-06-17T19:50:04-07:00[America/Los_Angeles]
 *   ...
 */

#include <Arduino.h>
#include <AceTimeClock.h>

using ace_time::acetime_t;
using ace_time::ZonedDateTime;
using ace_time::TimeZone;
using ace_time::BasicZoneProcessor;
using ace_time::zonedb::kZoneAmerica_Los_Angeles;
using ace_time::clock::SystemClockLoop;

// ESP32 does not define SERIAL_PORT_MONITOR
#ifndef SERIAL_PORT_MONITOR
#define SERIAL_PORT_MONITOR Serial
#endif

// ZoneProcessor instance should be created statically at initialization time.
static BasicZoneProcessor pacificProcessor;

static SystemClockLoop systemClock(nullptr /*reference*/, nullptr /*backup*/);

void printCurrentTime() {
  acetime_t now = systemClock.getNow();

  // Create Pacific Time and print.
  auto pacificTz = TimeZone::forZoneInfo(&kZoneAmerica_Los_Angeles,
      &pacificProcessor);
  auto pacificTime = ZonedDateTime::forEpochSeconds(now, pacificTz);
  pacificTime.printTo(SERIAL_PORT_MONITOR);
  SERIAL_PORT_MONITOR.println();
}

//-----------------------------------------------------------------------------

void setup() {
#if ! defined(EPOXY_DUINO)
  delay(1000);
#endif
  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR); // Wait until ready - Leonardo/Micro

  systemClock.setup();

  // Creating timezones is cheap, so we can create them on the fly as needed.
  auto pacificTz = TimeZone::forZoneInfo(&kZoneAmerica_Los_Angeles,
      &pacificProcessor);

  // Set the SystemClock using these components.
  auto pacificTime = ZonedDateTime::forComponents(
      2019, 6, 17, 19, 50, 0, pacificTz);
  systemClock.setNow(pacificTime.toEpochSeconds());
}

// Do NOT use delay(), it breaks systemClock.loop()
void loop() {
  static acetime_t prevNow = systemClock.getNow();
  systemClock.loop();
  acetime_t now = systemClock.getNow();
  if (now - prevNow >= 2) {
    printCurrentTime();
    prevNow = now;
  }
}
