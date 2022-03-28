/*
 * A program to demonstrate the NtpClock class where the WiFi stack is
 * configured manually before calling NtpClock::setup().
 * Tested on ESP8266 and ESP32.
 *
 * Should print the following:
 *
 * Connecting to WiFi.............. Done.
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
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
#else
  #include <WiFi.h>
#endif

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
// Number of millis to wait for WiFi connection before doing a software reboot.
static const unsigned long REBOOT_TIMEOUT_MILLIS = 15000;

static BasicZoneProcessor parisProcessor;
static NtpClock ntpClock;

void setupWiFi(
    const char* ssid,
    const char* password,
    unsigned long rebootTimeoutMillis)
{
  SERIAL_PORT_MONITOR.print(F("Connecting to WiFi"));
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  unsigned long startMillis = millis();
  while (true) {
    SERIAL_PORT_MONITOR.print('.');
    if (WiFi.status() == WL_CONNECTED) {
      SERIAL_PORT_MONITOR.println(F(" Done."));
      break;
    }

    unsigned long nowMillis = millis();
    if ((unsigned long) (nowMillis - startMillis) >= rebootTimeoutMillis) {
    #if defined(ESP8266)
      SERIAL_PORT_MONITOR.println(F("FAILED! Rebooting.."));
      delay(1000);
      ESP.reset();
    #elif defined(ESP32)
      SERIAL_PORT_MONITOR.println(F("FAILED! Rebooting.."));
      delay(1000);
      ESP.restart();
    #else
      SERIAL_PORT_MONITOR.println(
          F("FAILED! But cannot reboot.. continuing.."));
      delay(1000);
      startMillis = nowMillis;
    #endif
    }

    delay(500);
  }
}

void setup() {
  delay(1000);
  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR); // Wait until Serial is ready - Leonardo/Micro
  SERIAL_PORT_MONITOR.println();

  setupWiFi(SSID, PASSWORD, REBOOT_TIMEOUT_MILLIS);
  ntpClock.setup();
  if (!ntpClock.isSetup()) {
    SERIAL_PORT_MONITOR.println(F("Something went wrong."));
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
