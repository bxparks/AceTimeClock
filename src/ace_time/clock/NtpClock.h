/*
 * MIT License
 * Copyright (c) 2018 Brian T. Park
 */

#ifndef ACE_TIME_NTP_CLOCK_H
#define ACE_TIME_NTP_CLOCK_H

#if defined(ESP8266) || defined(ESP32) || defined(EPOXY_CORE_ESP8266)

#include <stdint.h>
#if defined(ESP8266) || defined(EPOXY_CORE_ESP8266)
  #include <ESP8266WiFi.h>
#else
  #include <WiFi.h>
#endif
#include <WiFiUdp.h>
#include "Clock.h"

#ifndef ACE_TIME_NTP_CLOCK_DEBUG
#define ACE_TIME_NTP_CLOCK_DEBUG 0
#endif

namespace ace_time {
namespace clock {

/**
 * A Clock that retrieves the time from an NTP server. This class has the
 * deficiency that the DNS name resolver WiFi.hostByName() is a blocking call.
 * So every now and then, it can take 5-6 seconds for the call to return,
 * blocking everything (e.g. display refresh, button clicks) until it times out.
 *
 * NTP seconds is an unsigned 32-bit integer offset from the NTP epoch of
 * 1900-01-01. It rolls over every 136 years, with the first rollover happening
 * just after 2036-02-07 06:28:15 UTC. Each NTP rollover enters into the next
 * NTP Era. I have seen documentation that says that NTP v4 packets contain the
 * NTP era number but I have yet to find information on how to extract that
 * number. So currently this class ignores the NTP Era from the NTP packet.
 * Instead, the NTP Era is automatically inferred to be the ones that completely
 * span the currently defined AceTime epoch range, as described next.
 *
 * The AceTime seconds is a signed 32-bit integer with a range of 136 years
 * centered around the `Epoch::currentEpochYear()`. This class converts NTP
 * seconds to AceTime seconds by mapping the the AceTime seconds range onto the
 * NTP seconds range, straddling any NTP era necessary to meet the AceTime
 * seconds range. For example, for the default AceTime v2 epoch of 2050, the NTP
 * seconds will be interpreted to be from 1982 to 2118, crossing from NTP era 0
 * to NTP era 1. If the AceTime epoch is changed to 2150, then the NTP seconds
 * will be interpreted to be from 2082 to 2218, crossing from NTP era 1 to NTP
 * era 2.
 *
 * Warning: If you are using an ESP8266, AND you are using the `analogRead()`
 * function, calling `analogRead()` too quickly will cause the WiFi connection
 * to disconnect after 5-10 seconds. Calling NtpClock::setup() will *not* fix
 * the disconnect. See https://github.com/esp8266/Arduino/issues/1634 and
 * https://github.com/esp8266/Arduino/issues/5083. The solution is to add a
 * slightly delay between calls to analogRead(). I don't know what the minimum
 * value should be, but using a 10ms delay (i.e. no faster than 100 times a
 * second) seems to work for me.
 *
 * Borrowed from
 * https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/NTPClient/NTPClient.ino
 * and
 * https://github.com/PaulStoffregen/Time/blob/master/examples/TimeNTP/TimeNTP.ino
 *
 * TODO: Create a version that uses a non-blocking DNS look up.
 */
class NtpClock: public Clock {
  public:
    /** Default NTP Server */
    static const char kNtpServerName[];

    /** Default port used for UDP packets. */
    static const uint16_t kLocalPort = 2390;

    /** Request time out milliseconds. */
    static const uint16_t kRequestTimeoutMillis = 1000;

    /** Deprecated. Use kRequestTimeoutMillis instead. */
    static const uint16_t kRequestTimeout = kRequestTimeoutMillis;

    /** Number of millis to wait during connect before timing out. */
    static const uint16_t kConnectTimeoutMillis = 10000;

    /**
     * Constructor.
     * @param server name of the NTP server (default us.pool.ntp.org)
     * @param localPort used by the UDP client (default 8888)
     * @param requestTimeout milliseconds for a request timeout (default 1000)
     */
    explicit NtpClock(
            const char* server = kNtpServerName,
            uint16_t localPort = kLocalPort,
            uint16_t requestTimeout = kRequestTimeoutMillis):
        mServer(server),
        mLocalPort(localPort),
        mRequestTimeout(requestTimeout) {}

    /**
     * Set up the WiFi connection using the given ssid and password, and
     * prepare the UDP connection. If the WiFi connection was set up elsewhere,
     * you can call the method with no arguments to bypass the WiFi setup.
     *
     * @param ssid wireless SSID (default nullptr)
     * @param password password of the SSID (default nullptr)
     * @param connectTimeoutMillis how long to wait for a WiFi connection
     *    (default 10000 ms)
     */
    void setup(
        const char* ssid = nullptr,
        const char* password = nullptr,
        uint16_t connectTimeoutMillis = kConnectTimeoutMillis);

    /** Return the name of the NTP server. */
    const char* getServer() const { return mServer; }

    /** Return true if setup() suceeded. */
    bool isSetup() const { return mIsSetUp; }

    acetime_t getNow() const override;

    void sendRequest() const override;

    bool isResponseReady() const override;

    acetime_t readResponse() const override;

    /**
     * Convert an NTP seconds to AceTime seconds relative to the current AceTime
     * epoch defined by `Epoch::currentEpochYear()`. Since NTP epoch is
     * 1900-01-01 and is due to rollover just after 2036-02-07 06:28:15, this
     * function automatically accounts for the rollover of NTP seconds by
     * shifting the NTP seconds into AceTime seconds using modulo 2^32 unsigned
     * operations.
     *
     * The result of this function is that it takes the entire 32-bit range of
     * the (signed) AceTime seconds (centered around the `currentEpochYear()`,
     * and overlays it on top of the (unsigned) NTP seconds time line (with
     * rollovers into new NTP eras every 2^32 seconds), and converts each NTP
     * seconds (regardless of its NTP era) into its corresonding AceTime
     * seconds.
     */
    static acetime_t convertNtpSecondsToAceTimeSeconds(uint32_t ntpSeconds);

  private:
    /** NTP time is in the first 48 bytes of message. */
    static const uint8_t kNtpPacketSize = 48;

    /**
     * Number of days between NTP epoch (1900-01-01T00:00:00Z) and
     * AceTime Epoch Converter epoch (2000-01-01T00:00:00Z).
     */
    static const int32_t kDaysToConverterEpochFromNtpEpoch = 36524;

    /** Send an NTP request to the time server at the given address. */
    void sendNtpPacket(const IPAddress& address) const;

  private:
    const char* const mServer;
    uint16_t const mLocalPort;
    uint16_t const mRequestTimeout;

    mutable WiFiUDP mUdp;
    // buffer to hold incoming & outgoing packets
    mutable uint8_t mPacketBuffer[kNtpPacketSize];
    bool mIsSetUp = false;
};

}
}

#endif

#endif
