/*
 * A program to demonstrate the use of EspSntpClock.
 *
 * Tested on ESP8266 and ESP32.
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
using ace_time::clock::EspSntpClock;

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

// Number of millis to wait for WiFi connection before doing a software reboot.
static const unsigned long REBOOT_TIMEOUT_MILLIS = 15000;

static BasicZoneProcessor parisProcessor;
static EspSntpClock sntpClock;

void setupWiFi() {
  Serial.print(F("Connecting to WiFi"));
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startMillis = millis();
  while (true) {
    Serial.print('.');
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println(F(" Done."));
      break;
    }

    unsigned long nowMillis = millis();
    if ((unsigned long) (nowMillis - startMillis) >= REBOOT_TIMEOUT_MILLIS) {
    #if defined(ESP8266)
      Serial.println(F("FAILED! Rebooting.."));
      delay(1000);
      ESP.reset();
    #elif defined(ESP32)
      Serial.println(F("FAILED! Rebooting.."));
      delay(1000);
      ESP.restart();
    #else
      Serial.println(F("FAILED! But cannot reboot.. continuing.."));
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

  setupWiFi();
  sntpClock.setup();

  acetime_t nowSeconds = sntpClock.getNow();
  SERIAL_PORT_MONITOR.print(F("Now Seconds: "));
  SERIAL_PORT_MONITOR.println(nowSeconds);

  auto parisTz = TimeZone::forZoneInfo(&kZoneEurope_Paris, &parisProcessor);
  auto parisTime = ZonedDateTime::forEpochSeconds(nowSeconds, parisTz);
  SERIAL_PORT_MONITOR.print(F("Paris Time: "));
  parisTime.printTo(SERIAL_PORT_MONITOR);
  SERIAL_PORT_MONITOR.println();

  SERIAL_PORT_MONITOR.println(F("Done!"));
}

void loop() {}
