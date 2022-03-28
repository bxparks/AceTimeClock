/*
 * Determine the size of various components of the AceTimeClock library.
 */

#include <Arduino.h>

// List of features of the AceTime library that we want to examine.
#define FEATURE_BASELINE 0
#define FEATURE_DS3231_CLOCK_TWO_WIRE 1
#define FEATURE_DS3231_CLOCK_SIMPLE_WIRE 2
#define FEATURE_DS3231_CLOCK_SIMPLE_WIRE_FAST 3
#define FEATURE_NTP_CLOCK 4
#define FEATURE_ESP_SNTP_CLOCK 5
#define FEATURE_STM_RTC_CLOCK 6
#define FEATURE_STM32F1_CLOCK 7
#define FEATURE_SYSTEM_CLOCK_LOOP 8
#define FEATURE_SYSTEM_CLOCK_LOOP_AND_BASIC_TIME_ZONE 9
#define FEATURE_SYSTEM_CLOCK_LOOP_AND_EXTENDED_TIME_ZONE 10
#define FEATURE_SYSTEM_CLOCK_COROUTINE 11
#define FEATURE_SYSTEM_CLOCK_COROUTINE_AND_BASIC_TIME_ZONE 12
#define FEATURE_SYSTEM_CLOCK_COROUTINE_AND_EXTENDED_TIME_ZONE 13

// Select one of the FEATURE_* parameter and compile. Then look at the flash
// and RAM usage, compared to FEATURE_BASELINE usage to determine how much
// flash and RAM is consumed by the selected feature.
// NOTE: This line is modified by a 'sed' script in collect.sh. Be careful
// when modifying its format.
#define FEATURE 0

#if FEATURE != FEATURE_BASELINE
  #include <AceRoutine.h> // activates SystemClockCoroutine
  #include <AceTimeClock.h>
  using namespace ace_time;
  using namespace ace_time::clock;
#endif

#if FEATURE == FEATURE_DS3231_CLOCK_TWO_WIRE \
    || FEATURE == FEATURE_DS3231_CLOCK_SIMPLE_WIRE \
    || FEATURE == FEATURE_DS3231_CLOCK_SIMPLE_WIRE_FAST

  #include <AceWire.h> // TwoWireInterface, SimpleWireInterface, etc.
  #if FEATURE == FEATURE_DS3231_CLOCK_TWO_WIRE
    #include <Wire.h> // TwoWire, Wire
  #elif FEATURE == FEATURE_DS3231_CLOCK_SIMPLE_WIRE_FAST
    #include <digitalWriteFast.h>
    #include <ace_wire/SimpleWireFastInterface.h>
  #endif

#elif FEATURE == FEATURE_NTP_CLOCK || FEATURE == FEATURE_ESP_SNTP_CLOCK
  #if defined(ESP8266)
    #include <ESP8266WiFi.h>
  #elif defined(ESP32)
    #include <WiFi.h>
  #endif

#elif FEATURE == FEATURE_STM_RTC_CLOCK
  #if defined(EPOXY_DUINO)
    #include <EpoxyMockSTM32RTC.h>
  #elif defined(ARDUINO_ARCH_STM32)
    #include <STM32RTC.h>
  #endif
#endif

static const char SSID[] = "randomssid";
static const char PASSWORD[] = "randompassword";

// Set this variable to prevent the compiler optimizer from removing the code
// being tested when it determines that it does nothing.
volatile int guard;

// Use this instead of a constant to prevent the compiler from calculating
// certain values (e.g. toEpochSeconds()) at compile-time.
volatile int16_t year = 2019;

// Use this to prevent compiler optimization.
volatile uint32_t randomNow = 342348943;

// Configure static allocations, which will be tracked as static RAM usage.
#if FEATURE == FEATURE_DS3231_CLOCK_TWO_WIRE
  using WireInterface = ace_wire::TwoWireInterface<TwoWire>;
  WireInterface wireInterface(Wire);
  DS3231Clock<WireInterface> dsClock(wireInterface);

#elif FEATURE == FEATURE_DS3231_CLOCK_SIMPLE_WIRE
  static const uint8_t DELAY_MICROS = 4;
  using WireInterface = ace_wire::SimpleWireInterface;
  WireInterface wireInterface(SDA, SCL, DELAY_MICROS);
  DS3231Clock<WireInterface> dsClock(wireInterface);

#elif FEATURE == FEATURE_DS3231_CLOCK_SIMPLE_WIRE_FAST
  #if ! defined(ARDUINO_ARCH_AVR) && ! defined(EPOXY_DUINO)
    #error Unsupported FEATURE on this platform
  #endif

  static const uint8_t DELAY_MICROS = 4;
  using WireInterface = ace_wire::SimpleWireFastInterface<
      SDA, SCL, DELAY_MICROS>;
  WireInterface wireInterface;
  DS3231Clock<WireInterface> dsClock(wireInterface);

#elif FEATURE == FEATURE_NTP_CLOCK
  #if (defined(ESP8266) || defined(ESP32))
    NtpClock ntpClock;
  #endif

#elif FEATURE == FEATURE_ESP_SNTP_CLOCK
  #if defined(ESP8266) || defined(ESP32)
    EspSntpClock sntpClock;
  #endif

#elif FEATURE == FEATURE_STM_RTC_CLOCK
  #if defined(ARDUINO_ARCH_STM32) || defined(EPOXY_DUINO)
    StmRtcClock stmRtcClock;
  #endif

#elif FEATURE == FEATURE_STM32F1_CLOCK
  #if defined(ARDUINO_ARCH_STM32)
    Stm32F1Clock stmClock;
  #endif

#elif FEATURE == FEATURE_SYSTEM_CLOCK_LOOP
  SystemClockLoop systemClock(nullptr, nullptr);

#elif FEATURE == FEATURE_SYSTEM_CLOCK_LOOP_AND_BASIC_TIME_ZONE
  SystemClockLoop systemClock(nullptr, nullptr);

#elif FEATURE == FEATURE_SYSTEM_CLOCK_LOOP_AND_EXTENDED_TIME_ZONE
  SystemClockLoop systemClock(nullptr, nullptr);

#elif FEATURE == FEATURE_SYSTEM_CLOCK_COROUTINE
  SystemClockCoroutine systemClock(nullptr, nullptr);

#elif FEATURE == FEATURE_SYSTEM_CLOCK_COROUTINE_AND_BASIC_TIME_ZONE
  SystemClockCoroutine systemClock(nullptr, nullptr);

#elif FEATURE == FEATURE_SYSTEM_CLOCK_COROUTINE_AND_EXTENDED_TIME_ZONE
  SystemClockCoroutine systemClock(nullptr, nullptr);

#endif

// TeensyDuino seems to pull in malloc() and free() when a class with virtual
// functions is used polymorphically. This causes the memory consumption of
// FEATURE_BASELINE (which normally has no classes defined, so does not include
// malloc() and free()) to be artificially small which throws off the memory
// consumption calculations for all subsequent features. Let's define a
// throw-away class and call its method for all FEATURES, including BASELINE.
#if defined(TEENSYDUINO)
  class FooClass {
    public:
      virtual void doit() {
        guard = 0;
      }
  };

  FooClass* foo;
#endif

void setup() {
#if defined(TEENSYDUINO)
  // Force Teensy to bring in malloc(), free() and other things for virtual
  // dispatch.
  foo = new FooClass();
#endif

#if FEATURE == FEATURE_BASELINE
  guard = randomNow;

#elif FEATURE == FEATURE_DS3231_CLOCK_TWO_WIRE
  Wire.begin();
  wireInterface.begin();
  dsClock.setup();
  acetime_t now = dsClock.getNow();
  guard = now;

#elif FEATURE == FEATURE_DS3231_CLOCK_SIMPLE_WIRE
  wireInterface.begin();
  dsClock.setup();
  acetime_t now = dsClock.getNow();
  guard = now;

#elif FEATURE == FEATURE_DS3231_CLOCK_SIMPLE_WIRE_FAST
  #if ! defined(ARDUINO_ARCH_AVR) && ! defined(EPOXY_DUINO)
    #error Unsupported FEATURE on this platform
  #endif

  wireInterface.begin();
  dsClock.setup();
  acetime_t now = dsClock.getNow();
  guard = now;

#elif FEATURE == FEATURE_ESP_SNTP_CLOCK
  #if defined(ESP8266) || defined(ESP32)
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSWORD);
    sntpClock.setup();
    acetime_t now = sntpClock.getNow();
    guard = now;
  #elif defined(EPOXY_DUINO)
    // nothing
  #else
    #error Unsupported FEATURE on this platform
  #endif

#elif FEATURE == FEATURE_STM_RTC_CLOCK
  #if defined(ARDUINO_ARCH_STM32) || defined(EPOXY_DUINO)
    STM32RTC::getInstance().setClockSource(STM32RTC::HSE_CLOCK);
    STM32RTC::getInstance().begin(true /*reset*/);
    stmRtcClock.setup();
    acetime_t now = stmRtcClock.getNow();
    guard = now;
  #else
    #error Unsupported FEATURE on this platform
  #endif

#elif FEATURE == FEATURE_STM32F1_CLOCK
  #if defined(ARDUINO_ARCH_STM32)
    stmClock.setup();
    acetime_t now = stmClock.getNow();
    guard = now;
  #elif defined(EPOXY_DUINO)
    // nothing
  #else
    #error Unsupported FEATURE on this platform
  #endif

#elif FEATURE == FEATURE_NTP_CLOCK
  #if (defined(ESP8266) || defined(ESP32))
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSWORD);
    ntpClock.setup();
    acetime_t now = ntpClock.getNow();
    guard = now;
  #elif defined(EPOXY_DUINO)
    // nothing
  #else
    #error Unsupported FEATURE on this platform
  #endif

#elif FEATURE == FEATURE_ESP_SNTP_CLOCK
  #if defined(ESP8266) || defined(ESP32)
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSWORD);
    clock.setup();
    acetime_t now = ntpClock.getNow();
    guard = now;
  #else
    #error Unsupported FEATURE on this platform
  #endif

#elif FEATURE == FEATURE_SYSTEM_CLOCK_LOOP
  systemClock.setup();
  systemClock.setNow(randomNow);
  acetime_t now = systemClock.getNow();
  guard = now;

#elif FEATURE == FEATURE_SYSTEM_CLOCK_LOOP_AND_BASIC_TIME_ZONE
  systemClock.setup();
  systemClock.setNow(randomNow);
  acetime_t now = systemClock.getNow();
  BasicZoneProcessor processor;
  auto tz = TimeZone::forZoneInfo(&zonedb::kZoneAmerica_Los_Angeles,
      &processor);
  auto dt = ZonedDateTime::forEpochSeconds(now, tz);
  acetime_t epochSeconds = dt.toEpochSeconds();
  guard = epochSeconds;

#elif FEATURE == FEATURE_SYSTEM_CLOCK_LOOP_AND_EXTENDED_TIME_ZONE
  systemClock.setup();
  systemClock.setNow(randomNow);
  acetime_t now = systemClock.getNow();
  ExtendedZoneProcessor processor;
  auto tz = TimeZone::forZoneInfo(&zonedbx::kZoneAmerica_Los_Angeles,
      &processor);
  auto dt = ZonedDateTime::forEpochSeconds(now, tz);
  acetime_t epochSeconds = dt.toEpochSeconds();
  guard = epochSeconds;

#elif FEATURE == FEATURE_SYSTEM_CLOCK_COROUTINE
  systemClock.setup();
  systemClock.setNow(randomNow);
  systemClock.runCoroutine();
  acetime_t now = systemClock.getNow();
  guard = now;

#elif FEATURE == FEATURE_SYSTEM_CLOCK_COROUTINE_AND_BASIC_TIME_ZONE
  systemClock.setup();
  systemClock.setNow(randomNow);
  systemClock.runCoroutine();
  acetime_t now = systemClock.getNow();
  BasicZoneProcessor processor;
  auto tz = TimeZone::forZoneInfo(&zonedb::kZoneAmerica_Los_Angeles,
      &processor);
  auto dt = ZonedDateTime::forEpochSeconds(now, tz);
  acetime_t epochSeconds = dt.toEpochSeconds();
  guard = epochSeconds;

#elif FEATURE == FEATURE_SYSTEM_CLOCK_COROUTINE_AND_EXTENDED_TIME_ZONE
  systemClock.setup();
  systemClock.setNow(randomNow);
  systemClock.runCoroutine();
  acetime_t now = systemClock.getNow();
  ExtendedZoneProcessor processor;
  auto tz = TimeZone::forZoneInfo(&zonedbx::kZoneAmerica_Los_Angeles,
      &processor);
  auto dt = ZonedDateTime::forEpochSeconds(now, tz);
  acetime_t epochSeconds = dt.toEpochSeconds();
  guard = epochSeconds;

#else
  #error Unknown FEATURE
#endif
}

void loop() {
}
