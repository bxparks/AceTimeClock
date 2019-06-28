/*
 * MIT License
 * Copyright (c) 2018 Brian T. Park
 */

#ifndef ACE_TIME_TIME_KEEPER_H
#define ACE_TIME_TIME_KEEPER_H

#include <stdint.h>
#include "TimeProvider.h"

namespace ace_time {
namespace clock {

/**
 * A TimeProvider whose time can be set by the end-user. For example, an RTC
 * chip.
 */
class TimeKeeper: public TimeProvider {
  public:
    /**
     * Set the time to the indicated seconds. Calling with a value of
     * kInvalidSeconds indicates an error condition, so the method should do
     * nothing.
     */
    virtual void setNow(acetime_t epochSeconds) = 0;
};

}
}

#endif
