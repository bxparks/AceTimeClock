# AceTime Clock

[![AUnit Tests](https://github.com/bxparks/AceTimeClock/actions/workflows/aunit_tests.yml/badge.svg)](https://github.com/bxparks/AceTimeClock/actions/workflows/aunit_tests.yml)

The AceTimeClock provides Clock classes to retrieve and synchronize the real
time clock from different sources. The following sources are supported:

* the built-in `millis()` system clock
* an [NTP](https://en.wikipedia.org/wiki/Network_Time_Protocol) server
* a [DS3231 RTC](https://www.maximintegrated.com/en/products/analog/real-time-clocks/DS3231.html)
chip
* the generic STM32 RTC Clock
* the special STM32F1 RTC clock
* a Unix `time()` clock (when compiled using EpoxyDuino)

A special version of the `Clock` class called the `SystemClock` provides a
fast and accurate "epoch seconds" across all Arduino compatible systems. This
"epoch seconds" can be given to the Date, Time and TimeZone classes to retrieve
the current date and time in any desired timezone.

The companion [AceTime](https://github.com/bxparks/AceTime) library provides the
mechanism for converting the epoch seconds from AceTimeClock into human-readable
date and time in different time zones.

This library can be an alternative to the Arduino Time
(https://github.com/PaulStoffregen/Time) library.

**Version**: (2021-10-08, Initial split from AceTime)

**Changelog**: [CHANGELOG.md](CHANGELOG.md)

**See Also**:

* AceTime (https://github.com/bxparks/AceTime)
* AceTimeTools (https://github.com/bxparks/AceTimeTools)
* AceTimeValidation (https://github.com/bxparks/AceTimeValidation)
* AceTimePython (https://github.com/bxparks/AceTimePython)

## Table of Contents

* [Overview](#Overview)
    * [Clocks](#Clocks)
    * [MemoryUsage](#MemoryUsage)
    * [HelloSystemClock](#HelloSystemClock)
* [Installation](#Installation)
* [Documentation](#Documentation)
* [System Requirements](#SystemRequirements)
    * [Hardware](#Hardware)
    * [Tool Chain](#ToolChain)
    * [Operating System](#OperatingSystem)
* [License](#License)
* [Feedback and Support](#FeedbackAndSupport)
* [Authors](#Authors)

<a name="Overview"></a>
## Overview

This library provides Clock classes which provide access to more accurate
external time sources, and provide a fast access to an auto-updating "Epoch
Seconds" on all Arduino platforms. Initially, the classes of this library were
part of the [AceTime](https://github.com/bxparks/AceTime) library, but
eventually extracted into a separate library to manage the complexity of both
libraries.

The main purpose of the `Clock` class hierarchy is to provide a 32-bit signed
integer (`int32_t`) that represents the number of seconds since a fixed point in
the past called the "Epoch". Just like the AceTime library, the epoch is defined
to be 2000-01-01T00:00:00 UTC.

The documentation of these classes are given in
[docs/clock_system_clock.md](docs/clock_system_clock.md):

User-facing clock classes are in the `ace_time::clock` namespace for backwards
compatibility with the versions that lived in the AceTime library.

* `ace_time::clock::Clock`
    * `ace_time::clock::DS3231Clock`
    * `ace_time::clock::NtpClock`
    * `ace_time::clock::StmRtcClock`
    * `ace_time::clock::Stm32F1Clock`
    * `ace_time::clock::UnixClock`
    * `ace_time::clock::SystemClock`
        * `ace_time::clock::SystemClockCoroutine`
        * `ace_time::clock::SystemClockLoop`

Internal implementation classes are in the `ace_time::hw` namespace.

Different subclasses of `Clock` provide access to different hardware or network
devices that provide a source of time.

The `SystemClock` subclass is a special class whose purpose is to provide an
`epochSeconds` integer that increments by one every second. It then allows fast
access to this auto-incrementing integer. By fast, I mean that it should allow
sampling at least as fast as 10 times per second, but ideally much faster than
1000 times a second.

The `SystemClockRoutine` and `SystemClockLoop` classes provide a mechanism to
synchronize the `epochSeconds` of the `SystemClock` with a more accurate
`referenceClock` provided by one of the other `Clock` subclasses.

<a name="MemoryUsage"></a>
### Memory Usage

This library does not perform dynamic allocation of memory so that it can be
used in small microcontroller environments. In other words, it does not call the
`new` operator nor the `malloc()` function, and it does not use the Arduino
`String` class. Everything it needs is allocated statically at initialization
time.

An example of more complex application is the
WorldClock (https://github.com/bxparks/clocks/tree/master/WorldClock)
which has 3 OLED displays over SPI, 3 timezones using `BasicZoneProcessor`, a
`SystemClock` synchronized to a DS3231 chip on I2C, and 2 buttons with
debouncing and event dispatching provided by the AceButton
(https://github.com/bxparks/AceButton) library. This application consumes about
24 kB, well inside the 28 kB flash limit of a SparkFun Pro Micro controller.

<a name="HelloSystemClock"></a>
### HelloSystemClock

This is the example code for using the `SystemClock` taken from
[examples/HelloSystemClock](examples/HelloSystemClock).

```C++
#include <AceTime.h>
#include <AceTimeClock.h>

using namespace ace_time;
using namespace ace_time::clock;

// ZoneProcessor instances should be created statically at initialization time.
static BasicZoneProcessor pacificProcessor;

static SystemClockLoop systemClock(nullptr /*reference*/, nullptr /*backup*/);

void setup() {
  delay(1000);
  Serial.begin(115200);
  while (!Serial); // Wait until Serial is ready - Leonardo/Micro

  systemClock.setup();

  // Creating timezones is cheap, so we can create them on the fly as needed.
  auto pacificTz = TimeZone::forZoneInfo(&zonedb::kZoneAmerica_Los_Angeles,
      &pacificProcessor);

  // Set the SystemClock using these components.
  auto pacificTime = ZonedDateTime::forComponents(
      2019, 6, 17, 19, 50, 0, pacificTz);
  systemClock.setNow(pacificTime.toEpochSeconds());
}

void printCurrentTime() {
  acetime_t now = systemClock.getNow();

  // Create a time
  auto pacificTz = TimeZone::forZoneInfo(&zonedb::kZoneAmerica_Los_Angeles,
      &pacificProcessor);
  auto pacificTime = ZonedDateTime::forEpochSeconds(now, pacificTz);
  pacificTime.printTo(Serial);
  Serial.println();
}

// Do NOT use delay() here.
void loop() {
  static acetime_t prevNow = systemClock.getNow();
  systemClock.loop();
  acetime_t now = systemClock.getNow();
  if (now - prevNow >= 2) {
    printCurrentTime();
    prevNow = now;
  }
}
```

This will start by setting the SystemClock to 2019-06-17T19:50:00-07:00,
then printing the system time every 2 seconds:
```
2019-06-17T19:50:00-07:00[America/Los_Angeles]
2019-06-17T19:50:02-07:00[America/Los_Angeles]
2019-06-17T19:50:04-07:00[America/Los_Angeles]
...
```

<a name="Installation"></a>
## Installation

See [docs/installation.md](docs/installation.md).

<a name="Documentation"></a>
## Documentation

* [README.md](README.md)
    * this file
* Clocks and SystemClocks
    * See [docs/clock_system_clock.md](docs/clock_system_clock.md))
    * NTP Clock, DS3231 Clock, STM32 RTC Clock, STM32F1 Clock
    * SystemClock, SystemClockLoop, SystemClockCoroutine
* Doxygen docs
    * See [Doxygen docs](https://bxparks.github.io/AceTime/html) hosted on
      GitHub Pages
* Benchmarks
    * See [docs/benchmarks.md](docs/benchmarks.md) for CPU and memory usage
      benchmarks

<a name="SystemRequirements"></a>
## System Requirements

<a name="Hardware"></a>
### Hardware

This library has Tier 1 support on the following boards:

* Arduino Nano (16 MHz ATmega328P)
* SparkFun Pro Micro (16 MHz ATmega32U4)
* SAMD21 M0 Mini (48 MHz ARM Cortex-M0+)
* STM32 Blue Pill (STM32F103C8, 72 MHz ARM Cortex-M3)
* NodeMCU 1.0 (ESP-12E module, 80 MHz ESP8266)
* WeMos D1 Mini (ESP-12E module, 80 MHz ESP8266)
* ESP32 dev board (ESP-WROOM-32 module, 240 MHz dual core Tensilica LX6)
* Teensy 3.2 (96 MHz ARM Cortex-M4)

Tier 2 support can be expected on the following boards, mostly because I don't
test these as often:

* ATtiny85 (8 MHz ATtiny85)
* Arduino Pro Mini (16 MHz ATmega328P)
* Mini Mega 2560 (Arduino Mega 2560 compatible, 16 MHz ATmega2560)
* Teensy LC (48 MHz ARM Cortex-M0+)

The following boards are *not* supported:

* Any platform using the ArduinoCore-API
  (https://github.com/arduino/ArduinoCore-api), such as:
    * megaAVR (e.g. Nano Every)
    * SAMD21 boards w/ `arduino:samd` version >= 1.8.10 (e.g. Nano 33 IoT,
      MKRZero, etc)
    * Raspberry Pi Pico RP2040

<a name="ToolChain"></a>
### Tool Chain

This library was developed and tested using:

* [Arduino IDE 1.8.16](https://www.arduino.cc/en/Main/Software)
* [Arduino CLI 0.19.2](https://arduino.github.io/arduino-cli)
* [SpenceKonde ATTinyCore 1.5.2](https://github.com/SpenceKonde/ATTinyCore)
* [Arduino AVR Boards 1.8.3](https://github.com/arduino/ArduinoCore-avr)
* [Arduino SAMD Boards 1.8.9](https://github.com/arduino/ArduinoCore-samd)
* [SparkFun AVR Boards 1.1.13](https://github.com/sparkfun/Arduino_Boards)
* [SparkFun SAMD Boards 1.8.4](https://github.com/sparkfun/Arduino_Boards)
* [STM32duino 2.0.0](https://github.com/stm32duino/Arduino_Core_STM32)
* [ESP8266 Arduino 3.0.2](https://github.com/esp8266/Arduino)
* [ESP32 Arduino 1.0.6](https://github.com/espressif/arduino-esp32)
* [Teensyduino 1.55](https://www.pjrc.com/teensy/td_download.html)

This library is *not* compatible with:

* Any platform using the
  [ArduinoCore-API](https://github.com/arduino/ArduinoCore-api), for example:
    * [Arduino megaAVR](https://github.com/arduino/ArduinoCore-megaavr/)
    * [MegaCoreX](https://github.com/MCUdude/MegaCoreX)
    * [Arduino SAMD Boards >=1.8.10](https://github.com/arduino/ArduinoCore-samd)

It should work with [PlatformIO](https://platformio.org/) but I have
not tested it.

The library works on Linux or MacOS (using both g++ and clang++ compilers) using
the EpoxyDuino (https://github.com/bxparks/EpoxyDuino) emulation layer.

<a name="OperatingSystem"></a>
### Operating System

I use Ubuntu 18.04 and 20.04 for the vast majority of my development. I expect
that the library will work fine under MacOS and Windows, but I have not tested
them.

<a name="License"></a>
## License

[MIT License](https://opensource.org/licenses/MIT)

<a name="FeedbackAndSupport"></a>
## Feedback and Support

If you have any questions, comments and other support questions about how to
use this library, please use the
[GitHub Discussions](https://github.com/bxparks/AceTimeClock/discussions)
for this project. If you have bug reports or feature requests, please file a
ticket in [GitHub Issues](https://github.com/bxparks/AceTimeClock/issues).
I'd love to hear about how this software and its documentation can be improved.
I can't promise that I will incorporate everything, but I will give your ideas
serious consideration.

Please refrain from emailing me directly unless the content is sensitive. The
problem with email is that I cannot reference the email conversation when other
people ask similar questions later.

<a name="Authors"></a>
## Authors

* Created by Brian T. Park (brian@xparks.net).
* Support an existing WiFi connection in `NtpClock` by denis-stepanov@
  [#24](https://github.com/bxparks/AceTime/issues/24).
* Support for STM32RTC through the `ace_time::clock::StmRtcClock` class
  by Anatoli Arkhipenko (arkhipenko@)
  [#39](https://github.com/bxparks/AceTime/pull/39).
