/*
 * A program to demonstrate the use of EspSntpClock.
 * Tested on ESP8266 and ESP32.
 *
 * Should print the following:
 *
 * Connecting to WiFi........ Done.
 * Configuring SNTP.. Done.
 * Now Seconds: 701798545; Paris Time: 2022-03-28T18:02:25+02:00[Europe/Paris]
 * Now Seconds: 701798550; Paris Time: 2022-03-28T18:02:30+02:00[Europe/Paris]
 * ...
 */

#if !defined(ESP32) && !defined(ESP8266) && !defined(EPOXY_CORE_ESP8266)
  #error This sketch works only for the ESP8266 and ESP32
#endif

#include <Arduino.h>
#include <AceTime.h>
#include <AceTimeClock.h>
#if defined(ESP8266) || defined(EPOXY_CORE_ESP8266)
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

// Define your WiFi SSID and password.
// 1) The Arduino IDE has no support for defining C preprocessor macros, so you
// have to manually define them here. (WARNING: For security, do NOT commit your
// ssid and password into a public source repository.)
// 2) The auniter.sh script from the AUniter project allows the WIFI_SSID and
// WIFI_PASSWORD macros to be defined on the command line or from a config file.
#if defined(AUNITER)
static const char SSID[] = WIFI_SSID;
static const char PASSWORD[] = WIFI_PASSWORD;
#else
static const char SSID[] = "your wifi ssid";
static const char PASSWORD[] = "your wifi passord";
#endif

// Number of millis to wait for WiFi connection before doing a software reboot.
static const unsigned long REBOOT_TIMEOUT_MILLIS = 15000;

static BasicZoneProcessor parisProcessor;
static EspSntpClock sntpClock;

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
  sntpClock.setup();
}

void loop() {
  acetime_t nowSeconds = sntpClock.getNow();
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
