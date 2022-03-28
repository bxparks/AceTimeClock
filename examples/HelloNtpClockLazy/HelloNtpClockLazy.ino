/*
 * A program to demonstrate the NtpClock where the NtpClock::setup() is used to
 * setup the WiFi stack for convenience.
 * Tested on ESP8266 and ESP32.
 *
 * Should print the following:
 *
 * Now Seconds: 701797848; Paris Time: 2022-03-28T17:50:48+02:00[Europe/Paris]
 * Now Seconds: 701797853; Paris Time: 2022-03-28T17:50:53+02:00[Europe/Paris]
 * ...
 */

#if !defined(ESP32) && !defined(ESP8266)
  #error This sketch works only for the ESP8266 and ESP32
#endif

#include <Arduino.h>
#include <AceTime.h>
#include <AceTimeClock.h>

using ace_time::acetime_t;
using ace_time::TimeZone;
using ace_time::BasicZoneProcessor;
using ace_time::ZonedDateTime;
using ace_time::zonedb::kZoneEurope_Paris;
using ace_time::clock::NtpClock;

// ESP32 does not define SERIAL_PORT_MONITOR
#ifndef SERIAL_PORT_MONITOR
#define SERIAL_PORT_MONITOR Serial
#endif

// Replace WIFI_SSID and WIFI_PASSWORD with your WiFi SSID and password. I have
// a wrapper script that replaces these with the correct values. You will have
// to replace them manually.
//
// WARNING: For security, do NOT commit your ssid and password into a public
// source repository.
static const char SSID[] = WIFI_SSID;
static const char PASSWORD[] = WIFI_PASSWORD;
static const unsigned long WIFI_TIMEOUT_MILLIS = 15000;

static BasicZoneProcessor parisProcessor;
static NtpClock ntpClock;

void setup() {
  delay(1000);
  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR); // Wait until Serial is ready - Leonardo/Micro
  SERIAL_PORT_MONITOR.println();

  // Tell NtpClock::setup() to configure the WiFi as well.
  ntpClock.setup(SSID, PASSWORD, WIFI_TIMEOUT_MILLIS);
  if (!ntpClock.isSetup()) {
    SERIAL_PORT_MONITOR.println(F("WiFi connection failed... try again."));
    return;
  }
}

void loop() {
  acetime_t nowSeconds = ntpClock.getNow();
  SERIAL_PORT_MONITOR.print(F("Now Seconds: "));
  SERIAL_PORT_MONITOR.print(nowSeconds);
  SERIAL_PORT_MONITOR.print("; ");

  auto parisTz = TimeZone::forZoneInfo(&kZoneEurope_Paris, &parisProcessor);
  auto parisTime = ZonedDateTime::forEpochSeconds(nowSeconds, parisTz);
  SERIAL_PORT_MONITOR.print(F("Paris Time: "));
  parisTime.printTo(SERIAL_PORT_MONITOR);
  SERIAL_PORT_MONITOR.println();

  delay(5000);
}
