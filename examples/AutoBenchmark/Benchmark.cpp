/*
 * Benchmarks for various classes of AceTimeClock.
 */

#include <stdint.h>
#include <Arduino.h>
#include <AceCommon.h> // printUint32AsFloat3To()
#include <AceTimeClock.h>
#include <ace_time/testing/FakeClock.h>
#include "Benchmark.h"

using ace_time::clock::SystemClockLoop;
using ace_time::testing::FakeClock;

#if defined(ARDUINO_ARCH_AVR)
const uint32_t COUNT = 5000;
#elif defined(ARDUINO_ARCH_SAMD)
const uint32_t COUNT = 5000;
#elif defined(ARDUINO_ARCH_STM32)
const uint32_t COUNT = 10000;
#elif defined(ESP8266)
const uint32_t COUNT = 10000;
#elif defined(ESP32)
const uint32_t COUNT = 20000;
#elif defined(TEENSYDUINO)
const uint32_t COUNT = 20000;
#elif defined(EPOXY_DUINO)
// Linux or MacOS
const uint32_t COUNT = 100000;
#else
// A generic Arduino board that we have not looked at.
const uint32_t COUNT = 10000;
#endif

// The compiler is extremelly good about removing code that does nothing. This
// volatile variable is used to create side-effects that prevent the compiler
// from optimizing out the code that's being tested. Each disableOptimization()
// method should perform 6 XOR operations to cancel each other out when
// subtracted.
volatile uint8_t guard;

void disableOptimization(uint32_t value) {
  guard ^= value & 0xff;
  guard ^= (value >> 8) & 0xff;
  guard ^= (value >> 16) & 0xff;
  guard ^= (value >> 24) & 0xff;
}

// Print nanos as micros with 3 decimal places.
static void printMicrosPerIteration(uint32_t nanos) {
  ace_common::printUint32AsFloat3To(SERIAL_PORT_MONITOR, nanos);
}

//-----------------------------------------------------------------------------

FakeClock fakeClock;

SystemClockLoop systemClockLoop(&fakeClock, nullptr);

/**
 * Call SystemClockLoop::loop() COUNT number of times and return the total
 * number of micros elapsed.
 */
uint32_t runSystemClockLoop() {
  fakeClock.isResponseReady(true);

  yield();
  uint32_t count = COUNT;
  uint16_t internal = 0;
  uint32_t startMicros = micros();
  while (count--) {
    // Simulate the FakeClock incrementing by 1 second every 1000 iterations.
    // It's much faster than real time, but should be good enough to get a rough
    // estimate of how much long SystemClockLoop::loop() takes.
    if (internal == 1000) {
      uint32_t now = fakeClock.getNow();
      disableOptimization(now);
      fakeClock.setNow(now + 1);
      internal = 0;
    }
    internal++;

    systemClockLoop.loop();
  }
  uint32_t elapsedMicros = micros() - startMicros;
  yield();

  return elapsedMicros;
}

void runBenchmarks() {
  uint32_t elapsedNanos = runSystemClockLoop() * 1000 / COUNT;
  SERIAL_PORT_MONITOR.print(F("SystemClockLoop::loop() "));
  printMicrosPerIteration(elapsedNanos);
  SERIAL_PORT_MONITOR.println();
}
