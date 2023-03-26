# Changelog

* Unreleased
* 1.2.4 (2023-03-26)
    * **Bug Fix**: Fix incorrect template parameter in the definition
      of `SystemClockCoroutine` which prevented it from being executed by
      the `CoroutineScheduler::loop()`.
    * Upgrade to AceTime v2.2.1.
    * Re-add support for SAMD21 using the Seeed Studio XIAO.
* 1.2.3 (2022-11-16)
    * When compiled under EpoxyDuino, implement stubs directly in
      `ace_time/hw/StmRtc.cpp` instead of relying on the `STM32RTC.{h,cpp}` from
      the `EpoxyMockSTM32RTC` library which mocks out the real STM32RTC library.
        * Eliminates the need to include `EpoxyMockSTM32RTC` in various
          downstream `Makefile` when using EpoxyDuino.
* 1.2.2 (2022-11-08)
    * Fix `NtpClock` to handle the `uint32_t` overflow of NTP seconds, with the
      first one occuring just after 2036-02-07 06:28:15 UTC.
    * Review and document the range of validity of the `getNow()` seconds of
      various `Clock` classes. See [Bugs and Limitations](README.md#Bugs).
    * Update various sections of README.md to account for the change of AceTime
      epoch from 2000-01-01 (v1) to 2050-01-01 (v2).
    * Upgrade tool chains
        * Arduino CLI from 0.27.1
        * STM32duino from 2.3.0
        * AVR Core from 1.8.4 to 1.8.5
        * ESP32 Core from 2.0.2 to 2.0.5
        * Teensyduino from 1.56 to 1.57
* 1.2.1 (2022-11-06)
    * Fix incorrect reference to
     `LocalDate::secondsToCurrentEpochFromUnixEpoch64` instead of
     `Epoch::secondsToCurrentEpochFromUnixEpoch64`.
    * Enable GitHub Actions for ESP8266 examples (HelloEspSntpClock,
      HelloNtpClock, HelloNtpClockLazy) to catch compiler errors.
        * Depends on EpoxyDuino 1.4.0, and hsaturn/EspMock v0.1.
* 1.2.0 (2022-11-04)
    * Replace `LocalDate::kEpochYear` with `HardwareDateTime::kBaseYear`
      since AceTime current epoch is no longer a constant and AceTime no longer
      uses a 2-digit year offset internally.
    * Replace `LocalDate::kSecondsSinceUnixEpoch` with
      `Epoch::secondsToCurrentEpochFromUnixEpoch64()` since AceTime current
      epoch is no longer constant.
    * There should be no visible breakage of the client-facing API.
* 1.1.0 (2022-03-28)
    * Simplify `StmRtcClock` and `StmRtc` classes to use
      `STM32RTC::getInstance()` directly.
        * Move configuration of the `STM32RTC` singleton to the `STM32RTC`
          object itself (e.g. configuring `LSE_CLOCK`, `HSE_CLOCK`).
        * Update [README.md documentation](README.md#StmRtcClockClass) to
          indicate that `STM32RTC` 1.2.0 fixes the bug which caused the date
          components to be lost upon power reset.
    * Incorporate `EpoxyMockSTM32RTC` mock library from EpoxyDuino to allow
      `StmRtcClock` to be compiled in GitHub Actions.
    * Add benchmarks for `StmRtcClock`, `Stm32F1Clock`, `NtpClock`, and
      `EspSntpClock` into [MemoryBenchmark](examples/MemoryBenchmark).
    * Simplify and refactor NTP examples to be more consistent.
        * `examples/HelloNtpClock`
        * `examples/HelloNtpClockLazy`
        * `examples/HelloEspSntpClock`
    * Upgrade tool chains
        * Arduino IDE from 1.8.16 to 1.8.19
        * Arduino CLI from 0.19.2 to 0.20.2
        * STM32duino from 2.0.0 to 2.2.0
        * AVR Core from 1.8.3 to 1.8.4
        * ESP32 Core from 1.0.6 to 2.0.2
        * Teensyduino from 1.55 to 1.56
    * Add initial supporting and testing of a STM32F411 dev board.
* 1.0.5 (2022-03-25)
    * Add a `uint16_t` template parameter to `CoroutineTemplate` for
      compatibility with AceRoutine 1.5.0.
* 1.0.4 (2022-01-18)
    * Add `EspSntpClock` class which configures the SNTP client on the ESP8266
      and ESP32 platforms, sets the `configTime()` timezone to UTC, and exposes
      the `time()` function through the `Clock` interface.
* 1.0.3 (2021-12-02)
    * Add `AceSorting` library to various Makefiles, a new dependency for
      AceTime 1.9.0.
    * Adding accidentally missing Baseline line in MemoryBenchmark.
* 1.0.2 (2021-10-19)
    * Add `examples/HelloDS3231Clock` to show how to configure and use the new
      `DS3231Clock<T>` template class.
    * Rename `examples/HelloSystemClock` to `examples/HelloSystemClockLoop` for
      consistency.
    * Update README.md sample codes to work with the new `DS3231Clock<T>`
      template class and the `<AceWire.h>` library.
    * Update MemoryBenchmark with 3 variations of `DS3231Clock`: TwoWire,
      SimpleWire, and SimpleWireFast.
        * Saves 1600-2200 bytes of flash on AVR by avoiding `<Wire.h>`.
* 1.0.1 (2021-10-16)
    * Add AceWire and AceRoutine to the `depends` parameter in
      `library.properties` so that the Library Manager installs them
      automatically for convenience.
    * Add `make -C examples/MemoryBenchmark epoxy` to GitHub actions.
* 1.0.0 (2021-10-15)
    * Extract from AceTime 1.7.5 to work with AceTime 1.8.0. See
      [Migrating to
      1.8](https://github.com/bxparks/AceTime/blob/develop/MIGRATING.md#MigratingToVersion180) for migration notes.
    * Merge `installation.md` and `docs/clock_system_clock.md` from AceTime
      library into a self-contained README.md, instead of information being
      scattered over multiple files.
* (2021-10-08)
    * Initial split from AceTime library.
