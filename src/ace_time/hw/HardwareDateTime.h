/*
 * MIT License
 * Copyright (c) 2018 Brian T. Park
 */

#ifndef ACE_TIME_HW_DATE_TIME_H
#define ACE_TIME_HW_DATE_TIME_H

#include <stdint.h>
#include <Print.h> // Print

namespace ace_time {
namespace hw {

/**
 * The date (year, month, day) and time (hour, minute, second) fields supported
 * by the DS3231 RTC chip.
 */
class HardwareDateTime {
  public:
    /** Base year of the DS3231 chip. */
    static const int16_t kBaseYear = 2000;

    /** Print HardwareDateTime to 'printer'. */
    void printTo(Print& printer) const;

    /** [00, 99], year - 2000 */
    uint8_t year;

    /** [1, 12] */
    uint8_t month;

    /** [1, 31] */
    uint8_t day;

    /** [0, 23] */
    uint8_t hour;

    /** [0, 59] */
    uint8_t minute;

    /** [0, 59] */
    uint8_t second;

    /** [1, 7], interpretation undefined, increments every day */
    uint8_t dayOfWeek;
};

/**
 * Return true if two HardwareDateTime objects are equal. Optimized for small
 * changes in the less signficant fields, such as 'second' or 'minute'. The
 * dayOfWeek field must also match.
 */
inline bool operator==(const HardwareDateTime& a, const HardwareDateTime& b) {
  return a.second == b.second
      && a.minute == b.minute
      && a.hour == b.hour
      && a.day == b.day
      && a.month == b.month
      && a.year == b.year
      && a.dayOfWeek == b.dayOfWeek;
}

/** Return true if two HardwareDateTime objects are not equal. */
inline bool operator!=(const HardwareDateTime& a, const HardwareDateTime& b) {
  return ! (a == b);
}

}
}

#endif
