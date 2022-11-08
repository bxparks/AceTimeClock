/*
 * MIT License
 * Copyright (c) 2018 Brian T. Park
 */

#ifndef ACE_TIME_UNIX_CLOCK_H
#define ACE_TIME_UNIX_CLOCK_H

#if defined(EPOXY_DUINO)

#include <time.h> // time()
#include <AceTime.h> // LocalDate
#include "Clock.h"

namespace ace_time {
namespace clock {

/**
 * An implementation of Clock that works on Unix using EpoxyDuino.
 */
class UnixClock: public Clock {
  public:
    explicit UnixClock() {}

    /** Setup function that currently does nothing. */
    void setup() {}

    /**
     * @copydoc Clock::getNow()
     *
     * Since `acetime_t` is a 32-bit integer, this method is valid if the
     * current Unix time() is within about +/- 68 years of the current epoch
     * being used by the AceTime library, as defined by
     * `Epoch::currentEpochYear()`.
     */
    acetime_t getNow() const override {
      return time(nullptr) - Epoch::secondsToCurrentEpochFromUnixEpoch64();
    }
};

}
}

#endif

#endif
