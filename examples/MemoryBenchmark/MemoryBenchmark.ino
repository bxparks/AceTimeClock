/*
 * Determine the size of various components of the AceTimeClock library.
 */

#include <stdint.h> // uint8_t
#include <Arduino.h>

// List of features of the AceTime library that we want to examine.
#define FEATURE_BASELINE 0
#define FEATURE_DS3231_CLOCK 1
#define FEATURE_SYSTEM_CLOCK_LOOP 2
#define FEATURE_SYSTEM_CLOCK_LOOP_AND_BASIC_TIME_ZONE 3
#define FEATURE_SYSTEM_CLOCK_LOOP_AND_EXTENDED_TIME_ZONE 4
#define FEATURE_SYSTEM_CLOCK_COROUTINE 5
#define FEATURE_SYSTEM_CLOCK_COROUTINE_AND_BASIC_TIME_ZONE 6
#define FEATURE_SYSTEM_CLOCK_COROUTINE_AND_EXTENDED_TIME_ZONE 7

// Select one of the FEATURE_* parameter and compile. Then look at the flash
// and RAM usage, compared to FEATURE_BASELINE usage to determine how much
// flash and RAM is consumed by the selected feature.
// NOTE: This line is modified by a 'sed' script in collect.sh. Be careful
// when modifying its format.
#define FEATURE 0

#if FEATURE != FEATURE_BASELINE
  #include <AceRoutine.h> // activates SystemClockCoroutine
  #include <AceTimeClock.h>
  #include <Wire.h> // TwoWire, Wire
  #include <AceWire.h> // TwoWireInterface
  using namespace ace_time;
  using namespace ace_time::clock;
  using namespace ace_time::hw;
#endif

// Set this variable to prevent the compiler optimizer from removing the code
// being tested when it determines that it does nothing.
volatile int guard;

// Use this instead of a constant to prevent the compiler from calculating
// certain values (e.g. toEpochSeconds()) at compile-time.
volatile int16_t year = 2019;

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
  guard = 0;
#elif FEATURE == FEATURE_DS3231_CLOCK
  using WireInterface = ace_wire::SimpleWireInterface;
  WireInterface wireInterface(SDA, SCL, 4 /* delayMicros */);
  DS3231Clock<WireInterface> dsClock(wireInterface);
  acetime_t now = dsClock.getNow();
  guard ^= now;

#elif FEATURE == FEATURE_SYSTEM_CLOCK_LOOP
  SystemClockLoop systemClock(nullptr, nullptr);
  systemClock.setup();
  systemClock.setNow(random(65000));
  acetime_t now = systemClock.getNow();
  guard ^= now;
#elif FEATURE == FEATURE_SYSTEM_CLOCK_LOOP_AND_BASIC_TIME_ZONE
  SystemClockLoop systemClock(nullptr, nullptr);
  systemClock.setup();
  systemClock.setNow(random(65000));
  acetime_t now = systemClock.getNow();
  BasicZoneProcessor processor;
  auto tz = TimeZone::forZoneInfo(&zonedb::kZoneAmerica_Los_Angeles,
      &processor);
  auto dt = ZonedDateTime::forEpochSeconds(now, tz);
  acetime_t epochSeconds = dt.toEpochSeconds();
  guard ^= epochSeconds;
#elif FEATURE == FEATURE_SYSTEM_CLOCK_LOOP_AND_EXTENDED_TIME_ZONE
  SystemClockLoop systemClock(nullptr, nullptr);
  systemClock.setup();
  systemClock.setNow(random(65000));
  acetime_t now = systemClock.getNow();
  ExtendedZoneProcessor processor;
  auto tz = TimeZone::forZoneInfo(&zonedbx::kZoneAmerica_Los_Angeles,
      &processor);
  auto dt = ZonedDateTime::forEpochSeconds(now, tz);
  acetime_t epochSeconds = dt.toEpochSeconds();
  guard ^= epochSeconds;

#elif FEATURE == FEATURE_SYSTEM_CLOCK_COROUTINE
  SystemClockCoroutine systemClock(nullptr, nullptr);
  systemClock.setup();
  systemClock.setNow(random(65000));
  systemClock.runCoroutine();
  acetime_t now = systemClock.getNow();
  guard ^= now;
#elif FEATURE == FEATURE_SYSTEM_CLOCK_COROUTINE_AND_BASIC_TIME_ZONE
  SystemClockCoroutine systemClock(nullptr, nullptr);
  systemClock.setup();
  systemClock.setNow(random(65000));
  systemClock.runCoroutine();
  acetime_t now = systemClock.getNow();
  BasicZoneProcessor processor;
  auto tz = TimeZone::forZoneInfo(&zonedb::kZoneAmerica_Los_Angeles,
      &processor);
  auto dt = ZonedDateTime::forEpochSeconds(now, tz);
  acetime_t epochSeconds = dt.toEpochSeconds();
  guard ^= epochSeconds;
#elif FEATURE == FEATURE_SYSTEM_CLOCK_COROUTINE_AND_EXTENDED_TIME_ZONE
  SystemClockCoroutine systemClock(nullptr, nullptr);
  systemClock.setup();
  systemClock.setNow(random(65000));
  systemClock.runCoroutine();
  acetime_t now = systemClock.getNow();
  ExtendedZoneProcessor processor;
  auto tz = TimeZone::forZoneInfo(&zonedbx::kZoneAmerica_Los_Angeles,
      &processor);
  auto dt = ZonedDateTime::forEpochSeconds(now, tz);
  acetime_t epochSeconds = dt.toEpochSeconds();
  guard ^= epochSeconds;
#else
  #error Unknown FEATURE
#endif
}

void loop() {
}
