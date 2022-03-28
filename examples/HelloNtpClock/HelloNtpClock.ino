/*
 * A program to demonstrate the use of NtpClock using 2 instances. One instance
 * calls the setup() method with the WiFi SSID and password. The other instance
 * calls setup() with no arguments, which assumes that the WiFi connection was
 * setup by something else.
 *
 * Tested on ESP8266 and ESP32.
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

// Replace WIFI_SSID and WIFI_PASSWORD with your WiFi SSID and password.
// (I have a wrapper script that replaces these with the the correct values.
// You will have to replace them manually.)
// WARNING: For security, do NOT commit your ssid and password into a public
// source repository.
static const char SSID[] = WIFI_SSID;
static const char PASSWORD[] = WIFI_PASSWORD;

static BasicZoneProcessor parisProcessor;
static NtpClock ntpClock;
static NtpClock ntpClock2;

void setup() {
  delay(1000);
  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR); // Wait until Serial is ready - Leonardo/Micro
  SERIAL_PORT_MONITOR.println();

  auto parisTz = TimeZone::forZoneInfo(&kZoneEurope_Paris, &parisProcessor);

  // Example of setting up clock without Wi-Fi configured in advance by
  // passing the SSID and PASSWORD into setup().
  SERIAL_PORT_MONITOR.println(F("==== clock 1: Wi-Fi configured by NtpClock"));
  ntpClock.setup(SSID, PASSWORD);
  if (!ntpClock.isSetup()) {
    SERIAL_PORT_MONITOR.println(F("WiFi connection failed... try again."));
    return;
  }
  {
    acetime_t nowSeconds = ntpClock.getNow();
    SERIAL_PORT_MONITOR.print(F("Now Seconds: "));
    SERIAL_PORT_MONITOR.println(nowSeconds);

    auto parisTime = ZonedDateTime::forEpochSeconds(nowSeconds, parisTz);
    SERIAL_PORT_MONITOR.print(F("Paris Time: "));
    parisTime.printTo(SERIAL_PORT_MONITOR);
    SERIAL_PORT_MONITOR.println();
  }

  // Example of setting up clock with Wi-Fi already configured in advance by
  // calling the no-argument setup().
  SERIAL_PORT_MONITOR.println(F("==== clock 2: Wi-Fi configured in advance"));
  ntpClock2.setup();
  if (!ntpClock2.isSetup()) {
    SERIAL_PORT_MONITOR.println(F("Something went wrong."));
    return;
  }
  {
    acetime_t nowSeconds = ntpClock2.getNow();
    SERIAL_PORT_MONITOR.print(F("Now Seconds: "));
    SERIAL_PORT_MONITOR.println(nowSeconds);

    auto parisTime = ZonedDateTime::forEpochSeconds(nowSeconds, parisTz);
    SERIAL_PORT_MONITOR.print(F("Paris Time: "));
    parisTime.printTo(SERIAL_PORT_MONITOR);
    SERIAL_PORT_MONITOR.println();
  }

  SERIAL_PORT_MONITOR.println(F("Done!"));
}

void loop() {}
