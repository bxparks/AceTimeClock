/*
 * MIT License
 * Copyright (c) 2018 Brian T. Park
 */

#ifndef ACE_TIME_DS3231_CLOCK_H
#define ACE_TIME_DS3231_CLOCK_H

#include <stdint.h>
#include <AceTime.h>
#include "../hw/DS3231.h"
#include "../hw/HardwareDateTime.h"
#include "Clock.h"

namespace ace_time {
namespace clock {

/**
 * An implementation of Clock that uses a DS3231 RTC chip.
 *
 * @tparam T_WIREI type of the AceWire implementation to communicate over I2C
 */
template<typename T_WIREI>
class DS3231Clock : public Clock {
  public:
    /** Constructor. */
    explicit DS3231Clock(const T_WIREI& wireInterface) :
        mDS3231(wireInterface)
    {}

    /** Setup that currently does nothing. */
    void setup() {}

    acetime_t getNow() const override {
      hw::HardwareDateTime hardwareDateTime;
      mDS3231.readDateTime(&hardwareDateTime);
      return toDateTime(hardwareDateTime).toEpochSeconds();
    }
    
    void setNow(acetime_t epochSeconds) override {
      if (epochSeconds == kInvalidSeconds) return;

      LocalDateTime now = LocalDateTime::forEpochSeconds(epochSeconds);
      mDS3231.setDateTime(toHardwareDateTime(now));
    }

  private:
    /**
     * Convert the HardwareDateTime returned by the DS3231 chip to
     * the LocalDateTime object using UTC time zone.
     */
    static LocalDateTime toDateTime(const hw::HardwareDateTime& dt) {
      return LocalDateTime::forComponents(
          dt.year + hw::HardwareDateTime::kBaseYear,
          dt.month,
          dt.day,
          dt.hour,
          dt.minute,
          dt.second);
    }

    /**
     * Convert a LocalDateTime object to a HardwareDateTime object, ignoring
     * time zone. In practice, it will often be most convenient to store the
     * DS3231 as UTC time. Only 2 digits are supported by the year field in the
     * DS3231 so the year is assumed to be between 2000 and 2099.
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

  private:
    const hw::DS3231<T_WIREI> mDS3231;
};

}
}

#endif
