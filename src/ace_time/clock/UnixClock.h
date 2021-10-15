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

    acetime_t getNow() const override {
      return time(nullptr) - LocalDate::kSecondsSinceUnixEpoch;
    }
};

}
}

#endif

#endif
