/*
 * MIT License
 * Copyright (c) 2018 Brian T. Park
 */

#ifndef ACE_TIME_SYSTEM_CLOCK_SYNC_COROUTINE_H
#define ACE_TIME_SYSTEM_CLOCK_SYNC_COROUTINE_H

#include <stdint.h>
#include "../common/TimingStats.h"
#include "SystemClock.h"

class SystemClockSyncCoroutineTest;

namespace ace_time {
namespace clock {

/**
 * A coroutine that syncs the SystemClock with its mSyncTimeProvider. The
 * call to mSyncTimeProvider is asynchronous, which is helpful when the
 * syncTimeProvider issues a network request to an NTP server.
 *
 * Initially, the class attempts to sync with its syncTimeProvider every
 * initialSyncPeriodSeconds. If the request fails, then it retries with an
 * exponential backoff (doubling the delay every iteration), until the sync
 * period becomes greater than syncPeriodSeconds, then the delay is set
 * permanently to syncPeriodSeconds.
 */
class SystemClockSyncCoroutine: public ace_routine::Coroutine {
  public:
    /**
     * Constructor.
     *
     * @param systemClock the system time keeper to sync up
     * @param syncPeriodSeconds seconds between normal sync attempts
     *    (default 3600)
     * @param initialSyncPeriodSeconds seconds between sync attempts when
     *    the systemClock is not initialized (default 5)
     * @param requestTimeoutMillis number of milliseconds before the request to
     *    syncTimeProvider times out
     * @param timingStats internal statistics
     */
    explicit SystemClockSyncCoroutine(SystemClock& systemClock,
        uint16_t syncPeriodSeconds = 3600,
        uint16_t initialSyncPeriodSeconds = 5,
        uint16_t requestTimeoutMillis = 1000,
        common::TimingStats* timingStats = nullptr):
      mSystemClock(systemClock),
      mSyncPeriodSeconds(syncPeriodSeconds),
      mRequestTimeoutMillis(requestTimeoutMillis),
      mTimingStats(timingStats),
      mCurrentSyncPeriodSeconds(initialSyncPeriodSeconds) {}

    /**
     * @copydoc Coroutine::runCoroutine()
     *
     * The CoroutineScheduler will use this method if enabled. Don't forget to
     * call setupCoroutine() (inherited from Coroutine) in the global setup() to
     * register this coroutine into the CoroutineScheduler.
     */
    int runCoroutine() override {
      if (mSystemClock.mSyncTimeProvider == nullptr) return 0;

      COROUTINE_LOOP() {
        // Send request
        mSystemClock.mSyncTimeProvider->sendRequest();
        mRequestStartTime = millis();

        // Wait for request
        while (true) {
          if (mSystemClock.mSyncTimeProvider->isResponseReady()) {
            mRequestStatus = kStatusOk;
            break;
          }

          {
            // Local variable waitTime must be scoped with {} so that the goto
            // in COROUTINE_LOOP() skip past it in clang++. g++ seems to be
            // fine without it.
            uint16_t waitTime = millis() - mRequestStartTime;
            if (waitTime >= mRequestTimeoutMillis) {
              mRequestStatus = kStatusTimedOut;
              break;
            }
          }

          COROUTINE_YIELD();
        }

        // Process the response
        if (mRequestStatus == kStatusOk) {
          acetime_t nowSeconds =
              mSystemClock.mSyncTimeProvider->readResponse();
          uint16_t elapsedTime = millis() - mRequestStartTime;
          if (mTimingStats != nullptr) {
            mTimingStats->update(elapsedTime);
          }
          mSystemClock.sync(nowSeconds);
          mCurrentSyncPeriodSeconds = mSyncPeriodSeconds;
        }

        COROUTINE_DELAY_SECONDS(mDelayLoopCounter, mCurrentSyncPeriodSeconds);

        // Determine the retry delay time based on success or failure. If
        // failure, retry with exponential backoff, until the delay becomes
        // mSyncPeriodSeconds.
        if (mRequestStatus == kStatusTimedOut) {
          if (mCurrentSyncPeriodSeconds >= mSyncPeriodSeconds / 2) {
            mCurrentSyncPeriodSeconds = mSyncPeriodSeconds;
          } else {
            mCurrentSyncPeriodSeconds *= 2;
          }
        }
      }
    }

  private:
    friend class ::SystemClockSyncCoroutineTest;

    // disable copy constructor and assignment operator
    SystemClockSyncCoroutine(const SystemClockSyncCoroutine&) = delete;
    SystemClockSyncCoroutine& operator=(const SystemClockSyncCoroutine&) =
        delete;

    static const uint8_t kStatusOk = 0;
    static const uint8_t kStatusTimedOut = 1;

    SystemClock& mSystemClock;
    uint16_t const mSyncPeriodSeconds;
    uint16_t const mRequestTimeoutMillis;
    common::TimingStats* const mTimingStats;

    uint16_t mRequestStartTime;
    uint16_t mCurrentSyncPeriodSeconds;
    uint8_t mRequestStatus;
    uint16_t mDelayLoopCounter;
};

}
}

#endif
