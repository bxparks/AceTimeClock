/*
 * MIT License
 * Copyright (c) 2020 Brian T. Park, Anatoli Arkhipenko
 */

#ifndef ACE_TIME_HW_STM_RTC_H
#define ACE_TIME_HW_STM_RTC_H

#if defined(ARDUINO_ARCH_STM32) || defined(EPOXY_DUINO)

namespace ace_time {
namespace hw {

class HardwareDateTime;

/**
 * A class that reads and writes HardwareDateTime from an STM32 RTC chip
 * using the [STM32RTC](https://github.com/stm32duino/STM32RTC) library. This
 * class is designed to access just enough features of the RTC chip to implement
 * the ace_time::StmRtcClock class. It is not meant to provide access to all the
 * features of the RTC chip.
 *
 * The STM32RTC library provides a singleton instance through the
 * `STM32RTC::getInstance()` method. Configuration of the STM32RTC can be
 * performed through this instance in the global `setup()` function.
 */
class StmRtc {
  public:
    /** Constructor. */
    explicit StmRtc() = default;

    /** Read the time into the HardwareDateTime object. */
    void readDateTime(HardwareDateTime* dateTime) const;

    /** Set the STM with the HardwareDateTime values. */
    void setDateTime(const HardwareDateTime& dateTime) const;

    /** Return true if the RTC is available and the time is set. */
    bool isTimeSet() const;
};

} // hw
} // ace_time

#endif //  #if defined(ARDUINO_ARCH_STM32) || defined(EPOXY_DUINO)
#endif //  #ifndef ACE_TIME_HW_STM_RTC_H
