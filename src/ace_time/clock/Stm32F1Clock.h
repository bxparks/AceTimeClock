/*
 * MIT License
 * Copyright (c) 2021 Brian T. Park
 */

#ifndef ACE_TIME_STM32_F1_CLOCK_H
#define ACE_TIME_STM32_F1_CLOCK_H

// For EpoxyDuino, this class is simply stubbed out for testing purposes.
#if defined(STM32F1xx) || defined(EPOXY_DUINO)

#include <stdint.h>
#if ! defined(EPOXY_DUINO)
  #include "../hw/Stm32F1Rtc.h"
#endif
#include "Clock.h"

namespace ace_time {
namespace clock {

/**
 * An implementation of `Clock` that is specialized for the LSE_CLOCK (Low Speed
 * External clock) on the STM32F1 RTC chip. Normally, the LSE_CLOCK requires an
 * additional external 32.768 kHz crystal, but the popular "Blue Pill" dev board
 * already includes this extenal crystal on pins C14 and C15. **Warning**: For
 * the highest clock accuracy, those pins should not be attached anything else,
 * not even the male header pins. The header pins will add too much stray
 * capacitance to the oscillator circuit, and cause the clock to run too slow. I
 * have seen the clock run as much as 10% too slow with the male header pins
 * attached. If you hold a finger to those pins, it adds so much capacitance
 * that the LSE_CLOCK will appear to just stop.
 *
 * There are 3 possible RTC clocks on the STM32F1 (HSI_CLOCK, LSI_CLOCK, and
 * LSE_CLOCK). But the LSE_CLOCK is special because it keeps updating the RTC on
 * the STM32F1 through a reset or power loss, as long as a battery is attached
 * to VBat. The battery could be a 3V CR2032 coin battery, or 2AA rechargeable
 * or non-rechargeable battery, or it could be a super capacitor.
 *
 * This class uses the `Stm32F1Rtc` helper class to write directly to the RTC
 * registers on the STM32F1, bypassing the generic `STM32RTC` library
 * (https://github.com/stm32duino/STM32RTC) which would normally be used for
 * other STM32 microcontrollers. The generic `STM32RTC` library
 * has a bug on the STM32F1 where it preserves only the time fields in the RTC
 * registers, saving the date fields on SRAM which is lost upon reset (See
 * https://github.com/stm32duino/STM32RTC/issues/29 and
 * https://github.com/stm32duino/STM32RTC/issues/32). The problem is caused in
 * the low-level HAL (hardware abstraction layer) of the STM32F1 chip, because
 * unlike other STM32 processors which stores the time and date fields as
 * separate fields, the RTC on the STM32F1 is just a simple 32-bit counter
 * (split across 2 registeres, RTC_CNTH and RTC_CNTL) that increments once a
 * second.
 *
 * It turns out that for the purposes of AceTime and the `SystemClock`, a 32-bit
 * counter is sufficient to support all of its functionality. In particular, the
 * 32-bit counter is sufficient to allow AceTime to retain both date and time
 * fields through a power reset. So the `Stm32F1Rtc` class is a narrowly
 * targeted HAL whose only purpose is to read from and write to the 32-bit RTC
 * counter on the STM32F1 chip. It bypasses the entire generic RTC and HAL
 * layers provided by the STM32duino framework.
 *
 * AceTime v2 allows the epoch year of the library to be adjustable by the
 * client application, from the year 2000 until the year 10000. The use of a
 * simple 32-bit counter in the STM32F1 works to its advantage because it makes
 * it allows it work for any the epoch year selected in the AceTime library.
 * This is in contrast to the other STM32 processors which use a 2-digit year
 * offset from the year 2000. The RTC of those processors will break in the year
 * 2100 unless a software fix can be created to work around that limitation.
 *
 * The `Stm32F1Rtc` class uses one additional register, the Backup DR1 register,
 * which holds a single status bit to indicate whether or not the underlying RTC
 * counter has been initialized to a valid time. The selection of the `DR1`
 * register, instead of any of the other 9-10 backup registers, is currently
 * hardcoded in the `RTC_INIT_REG` macro in `Stm32F1Rtc.h`. If that causes a
 * conflict with something else, let me know, because this is fixable. We can
 * make that a configurable parameter in the Stm32F1Rtc::begin() method.
 */
class Stm32F1Clock: public Clock {
  public:
    explicit Stm32F1Clock() {}

    /** Configure the clock. */
    void setup() {
    #if ! defined(EPOXY_DUINO)
      mStm32F1Rtc.begin();
    #endif
    }

    acetime_t getNow() const override {
    #if defined(EPOXY_DUINO)
      return mEpochSeconds;
    #else
      return mStm32F1Rtc.getTime();
    #endif
    }

    void setNow(acetime_t epochSeconds) override {
      if (epochSeconds == kInvalidSeconds) return;
    #if defined(EPOXY_DUINO)
      mEpochSeconds = epochSeconds;
    #else
      mStm32F1Rtc.setTime(epochSeconds);
    #endif
    }

  private:
  #if defined(EPOXY_DUINO)
    uint32_t mEpochSeconds;
  #else
    mutable hw::Stm32F1Rtc mStm32F1Rtc;
  #endif
};

} // clock
} // ace_time

#endif // #if defined(STM32F1xx) || defined(EPOXY_DUINO)

#endif // #ifndef ACE_TIME_STM32F1_CLOCK_H
