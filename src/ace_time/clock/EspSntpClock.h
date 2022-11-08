/*
 * MIT License
 * Copyright (c) 2022 Brian T. Park
 */

#ifndef ACE_TIME_ESP_SNTP_CLOCK_H
#define ACE_TIME_ESP_SNTP_CLOCK_H

#if defined(ESP8266) || defined(ESP32) || defined(EPOXY_CORE_ESP8266)

#include <time.h> // time()
#include <AceTime.h> // LocalDate
#include "Clock.h"

namespace ace_time {
namespace clock {

/**
 * An implementation of Clock that configures the built-in SNTP client on the
 * ESP8266 and ESP32 using the `configTime()` function using UTC timezone (no
 * STD or DST offset), and uses the C-library `time()` function as the reference
 * clock. Apparently the SNTP client synchronizes the `time()` every hour. This
 * class depends on the WiFi client being configured somewhere else.
 */
class EspSntpClock: public Clock {
  public:
    /** Default NTP server, "pool.ntp.org". */
    static const char kDefaultNtpServer[];

    /** Default time out for setup(). */
    static const uint32_t kDefaultTimeoutMillis = 15000;

    explicit EspSntpClock() {}

    /**
     * Setup the SNTP client. Assumes WiFi is already configured. This step can
     * be skipped if the SNTP client is configured somewhere else.
     *
     * @param ntpServer name of the NTP server, default "pool.ntp.org"
     * @param timeoutMillis number of millis to wait before returning if
     *    the SNTP does not stabilize, default 15000 (15 seconds)
     *
     * @returns true if the setup() was successful, false if the timeout
     *    occurred
     */
    bool setup(
        const char* ntpServer = kDefaultNtpServer,
        uint32_t timeoutMillis = kDefaultTimeoutMillis);

    /**
     * @copydoc Clock::getNow()
     *
     * Since `acetime_t` is a 32-bit integer, this method is valid if the
     * current SNTP time() is within about +/- 68 years of the current epoch
     * being used by the AceTime library, as defined by
     * `Epoch::currentEpochYear()`.
     */
    acetime_t getNow() const override {
      return time(nullptr)
        - Epoch::secondsToCurrentEpochFromUnixEpoch64();
    }
};

}
}

#endif

#endif
