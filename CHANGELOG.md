# Changelog

* Unreleased
* v1.1.0 (2022-03-28)
    * Simplify `StmRtcClock` and `StmRtc` classes to use
      `STM32RTC::getInstance()` directly.
        * Move configuration of the `STM32RTC` singleton to the `STM32RTC`
          object itself (e.g. configuring `LSE_CLOCK`, `HSE_CLOCK`).
        * Update [README.md documentation](README.md#StmRtcClockClass) to
          indicate that `STM32RTC` v1.2.0 fixes the bug which caused the date
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
* v1.0.5 (2022-03-25)
    * Add a `uint16_t` template parameter to `CoroutineTemplate` for
      compatibility with AceRoutine v1.5.0.
* v1.0.4 (2022-01-18)
    * Add `EspSntpClock` class which configures the SNTP client on the ESP8266
      and ESP32 platforms, sets the `configTime()` timezone to UTC, and exposes
      the `time()` function through the `Clock` interface.
* v1.0.3 (2021-12-02)
    * Add `AceSorting` library to various Makefiles, a new dependency for
      AceTime v1.9.0.
    * Adding accidentally missing Baseline line in MemoryBenchmark.
* v1.0.2 (2021-10-19)
    * Add `examples/HelloDS3231Clock` to show how to configure and use the new
      `DS3231Clock<T>` template class.
    * Rename `examples/HelloSystemClock` to `examples/HelloSystemClockLoop` for
      consistency.
    * Update README.md sample codes to work with the new `DS3231Clock<T>`
      template class and the `<AceWire.h>` library.
    * Update MemoryBenchmark with 3 variations of `DS3231Clock`: TwoWire,
      SimpleWire, and SimpleWireFast.
        * Saves 1600-2200 bytes of flash on AVR by avoiding `<Wire.h>`.
* v1.0.1 (2021-10-16)
    * Add AceWire and AceRoutine to the `depends` parameter in
      `library.properties` so that the Library Manager installs them
      automatically for convenience.
    * Add `make -C examples/MemoryBenchmark epoxy` to GitHub actions.
* v1.0.0 (2021-10-15)
    * Extract from AceTime v1.7.5 to work with AceTime v1.8.0. See
      [Migrating to
      v1.8](https://github.com/bxparks/AceTime/blob/develop/MIGRATING.md#MigratingToVersion180) for migration notes.
    * Merge `installation.md` and `docs/clock_system_clock.md` from AceTime
      library into a self-contained README.md, instead of information being
      scattered over multiple files.
* (2021-10-08)
    * Initial split from AceTime library.
