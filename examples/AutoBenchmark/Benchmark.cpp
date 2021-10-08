/*
 * Benchmarks for various classes of AceTimeClock.
 */

#include <stdint.h>
#include <Arduino.h>
#include <AceTimeClock.h>
#include "Benchmark.h"

using namespace ace_time;

#if defined(ARDUINO_ARCH_AVR)
const uint32_t COUNT = 1000;
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
const uint32_t COUNT = 50000;
#else
// A generic Arduino board that we have not looked at.
const uint32_t COUNT = 10000;
#endif

const uint32_t MILLIS_TO_NANO_PER_ITERATION = ((uint32_t) 1000000 / COUNT);

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

// Type declaration of the Lambda.
typedef void (*Lambda)();

// A small helper that runs the given lamba expression in a loop
// and returns how long it took.
unsigned long runLambda(Lambda lambda) {
  yield();
  uint32_t count = COUNT;
  unsigned long startMillis = millis();
  while (count--) {
    lambda();
  }
  unsigned long elapsedMillis = millis() - startMillis;
  yield();
  return elapsedMillis;
}

// Given total elapsed time in millis, print micros per iteration as a floating
// point number (without using floating point operations).
//
// Sometimes, the elapsedMillis is negative. This happens on some benchmarks on
// higher powered CPUs where the thing being measured is so quickly executed
// that the empty loop overhead can take a longer. Print "-0.000" if that
// occurs.
static void printMicrosPerIteration(long elapsedMillis) {
  if (elapsedMillis < 0) {
    SERIAL_PORT_MONITOR.print(F("  -0.000"));
    return;
  }
  unsigned long nanos = elapsedMillis * MILLIS_TO_NANO_PER_ITERATION;
  uint16_t whole = nanos / 1000;
  uint16_t frac = nanos % 1000;
  SERIAL_PORT_MONITOR.print(' ');
  SERIAL_PORT_MONITOR.print(whole);
  SERIAL_PORT_MONITOR.print('.');
  ace_common::printPad3To(SERIAL_PORT_MONITOR, frac, '0');
}

void runBenchmarks() {
}
