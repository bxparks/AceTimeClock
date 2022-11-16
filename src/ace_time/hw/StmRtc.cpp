/*
 * MIT License
 * Copyright (c) 2020 Brian T. Park, Anatoli Arkhipenko
 */

#if defined(ARDUINO_ARCH_STM32) || defined(EPOXY_DUINO)

// Use the STM32RTC library if compiling under Stm32duino.
#if ! defined(EPOXY_DUINO)
  #include <STM32RTC.h>
#endif
#include "HardwareDateTime.h"
#include "StmRtc.h"

namespace ace_time {
namespace hw {

void StmRtc::readDateTime(HardwareDateTime* dateTime) const {
#if defined(EPOXY_DUINO)
  // Hardcode the date to 2000-01-01T00:00:00 under EpoxyDuino.
  dateTime->second = 0;
  dateTime->minute = 0;
  dateTime->hour = 0;
  dateTime->dayOfWeek = 6;
  dateTime->day = 1;
  dateTime->month = 1;
  dateTime->year = 0; // 2 digit year, so 0 means year 2000
#else
  STM32RTC& rtc = STM32RTC::getInstance();
  if (rtc.isTimeSet()) {
    dateTime->second = rtc.getSeconds();
    dateTime->minute = rtc.getMinutes();
    dateTime->hour = rtc.getHours();
    dateTime->dayOfWeek = rtc.getWeekDay();
    dateTime->day = rtc.getDay();
    dateTime->month = rtc.getMonth();
    dateTime->year = rtc.getYear();
  } else {
    dateTime->second = 0;
    dateTime->minute = 0;
    dateTime->hour = 0;

    // The STM32RTC weekday parameter ranges from (1-7).
    // AceTime uses ISO weekday numbers, where Monday=1.
    // The date 2000-01-01 was a Saturday=6, so this should be set to 6.
    //
    // AceTime does not actually use this field, because it derives the weekday
    // from the (year, month, day) tuple. But it seems prudent to set this
    // corectly in case something else in the system uses STM32RTC directly.
    dateTime->dayOfWeek = 6;

    dateTime->day = 1;
    dateTime->month = 1;
    dateTime->year = 0; // 2 digit year, so 0 means year 2000
  }
#endif
}

void StmRtc::setDateTime(const HardwareDateTime& dateTime) const {
#if defined(EPOXY_DUINO)
  (void) dateTime;
#else
  STM32RTC& rtc = STM32RTC::getInstance();
  rtc.setTime(dateTime.hour, dateTime.minute, dateTime.second);
  rtc.setDate(dateTime.dayOfWeek, dateTime.day, dateTime.month, dateTime.year);
#endif
}

bool StmRtc::isTimeSet() const {
#if defined(EPOXY_DUINO)
  return false;
#else
  STM32RTC& rtc = STM32RTC::getInstance();
  return rtc.isTimeSet();
#endif
}

} // hw
} // ace_time

#endif // #if defined(ARDUINO_ARCH_STM32) || defined(EPOXY_DUINO)
