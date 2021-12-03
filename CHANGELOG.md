# Changelog

* Unreleased
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
