/*
 * Determine the size of various components of the AceTimeClock library.
 */

#include <Arduino.h>

// List of features of the AceTime library that we want to examine.
#define FEATURE_BASELINE 0
#define FEATURE_DS3231_CLOCK_TWO_WIRE 1
#define FEATURE_DS3231_CLOCK_SIMPLE_WIRE 2
#define FEATURE_DS3231_CLOCK_SIMPLE_WIRE_FAST 3
#define FEATURE_SYSTEM_CLOCK_LOOP 4
#define FEATURE_SYSTEM_CLOCK_LOOP_AND_BASIC_TIME_ZONE 5
#define FEATURE_SYSTEM_CLOCK_LOOP_AND_EXTENDED_TIME_ZONE 6
#define FEATURE_SYSTEM_CLOCK_COROUTINE 7
#define FEATURE_SYSTEM_CLOCK_COROUTINE_AND_BASIC_TIME_ZONE 8
#define FEATURE_SYSTEM_CLOCK_COROUTINE_AND_EXTENDED_TIME_ZONE 9

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
#endif

#if FEATURE == FEATURE_DS3231_CLOCK_TWO_WIRE
  #include <Wire.h> // TwoWire, Wire
#endif

#if FEATURE == FEATURE_DS3231_CLOCK_SIMPLE_WIRE_FAST
  #include <digitalWriteFast.h>
  #include <ace_wire/SimpleWireFastInterface.h>
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

#elif FEATURE == FEATURE_DS3231_CLOCK_TWO_WIRE
  using WireInterface = ace_wire::TwoWireInterface<TwoWire>;
  WireInterface wireInterface(Wire);
  DS3231Clock<WireInterface> dsClock(wireInterface);
  acetime_t now = dsClock.getNow();
  guard ^= now;

#elif FEATURE == FEATURE_DS3231_CLOCK_SIMPLE_WIRE
  static const uint8_t DELAY_MICROS = 4;
  using WireInterface = ace_wire::SimpleWireInterface;
  WireInterface wireInterface(SDA, SCL, DELAY_MICROS);
  DS3231Clock<WireInterface> dsClock(wireInterface);
  acetime_t now = dsClock.getNow();
  guard ^= now;

#elif FEATURE == FEATURE_DS3231_CLOCK_SIMPLE_WIRE_FAST
  #if ! defined(ARDUINO_ARCH_AVR) && ! defined(EPOXY_DUINO)
    #error Unsupported FEATURE on this platform
  #endif

  static const uint8_t DELAY_MICROS = 4;
  using WireInterface = ace_wire::SimpleWireFastInterface<
      SDA, SCL, DELAY_MICROS>;
  WireInterface wireInterface;
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
