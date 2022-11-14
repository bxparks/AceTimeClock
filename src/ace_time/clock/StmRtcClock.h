/*
 * MIT License
 * Copyright (c) 2020 Brian T. Park, Anatoli Arkhipenko
 *
 * Requires https://github.com/stm32duino/STM32RTC
 */

#ifndef ACE_TIME_STM_RTC_CLOCK_H
#define ACE_TIME_STM_RTC_CLOCK_H

#if defined(ARDUINO_ARCH_STM32) || defined(EPOXY_DUINO)

#include <stdint.h>
#include <AceTime.h> // LocalDateTime
#include "../hw/StmRtc.h"
#include "../hw/HardwareDateTime.h"
#include "Clock.h"

namespace ace_time {
namespace clock {

/**
 * An implementation of Clock that uses an STM32 RTC chip using the
 * [STM32RTC](https://github.com/stm32duino/STM32RTC) library. The STM32RTC
 * singleton object should be configured using `STM32RTC::getInstance()` in the
 * global `setup()` function.
 *
 * AceTime v2 allows the epoch year of the library to be adjustable by the
 * client application, from the year 2000 until the year 10000. Unfortunately,
 * the STM32 hardware supported by this class (through the dependent STM32RTC
 * library) uses a 2-digit year offset from the year 2000. This will break in
 * the year 2100 unless a software fix can be created to work around that
 * limitation.
 *
 * Ironically, the STM32F1 chip has a more limited RTC functionality in the
 * hardware but that limitation actually works to its advantage so that the F1
 * can be fully compatible with AceTime to its limit of the year 10000 if
 * Stm32F1Clock class is used instead of this class. (See notes in
 * Stm32F1Clock.h).
 */
class StmRtcClock: public Clock {
  public:
    /** Constructor. */
    explicit StmRtcClock() = default;

    /** Setup does nothing. Defined for API consistency with other Clocks. */
    void setup() {}

    acetime_t getNow() const override {
      hw::HardwareDateTime hardwareDateTime;
      mStmRtc.readDateTime(&hardwareDateTime);
      return toDateTime(hardwareDateTime).toEpochSeconds();
    }

    void setNow(acetime_t epochSeconds) override {
      if (epochSeconds == kInvalidSeconds) return;

      LocalDateTime now = LocalDateTime::forEpochSeconds(epochSeconds);
      mStmRtc.setDateTime(toHardwareDateTime(now));
    }

    /** Return true if the RTC is available and the time is set. */
    bool isTimeSet() const {
      return mStmRtc.isTimeSet();
    }

  private:
    /**
     * Convert the HardwareDateTime returned by the STM RTC chip to
     * the LocalDateTime object using UTC time zone.
     */
    static LocalDateTime toDateTime(const hw::HardwareDateTime& dt) {
      return LocalDateTime::forComponents(
          dt.year + hw::HardwareDateTime::kBaseYear, dt.month, dt.day,
          dt.hour, dt.minute, dt.second);
    }

    /**
     * Convert a LocalDateTime object to a HardwareDateTime object, ignoring
     * time zone. In practice, it will often be most convenient to store
     * as UTC time. Only 2 digits are supported by the year field.
     */
    static hw::HardwareDateTime toHardwareDateTime(const LocalDateTime& dt) {
      return hw::HardwareDateTime{
          (uint8_t) (dt.year() - hw::HardwareDateTime::kBaseYear),
          dt.month(),
          dt.day(),
          dt.hour(),
          dt.minute(),
          dt.second(),
          dt.dayOfWeek()};
    }

    hw::StmRtc mStmRtc;
};

} // hw
} // ace_time

#endif // #if defined(ARDUINO_ARCH_STM32) || defined(EPOXY_DUINO)
#endif // #ifndef ACE_TIME_STM_RTC_CLOCK_H
