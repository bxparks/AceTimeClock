/*
 * MIT License
 * Copyright (c) 2018 Brian T. Park
 */

#include <Arduino.h>
#include "NtpClock.h"

#if defined(ESP8266) || defined(ESP32) || defined(EPOXY_CORE_ESP8266)

// ESP32 does not define SERIAL_PORT_MONITOR
#ifndef SERIAL_PORT_MONITOR
#define SERIAL_PORT_MONITOR Serial
#endif

namespace ace_time {
namespace clock {

const char NtpClock::kNtpServerName[] = "us.pool.ntp.org";

void NtpClock::setup(
    const char* ssid,
    const char* password,
    uint16_t connectTimeoutMillis
) {
  if (ssid) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    uint16_t startMillis = millis();
    while (WiFi.status() != WL_CONNECTED) {
      uint16_t elapsedMillis = millis() - startMillis;
      if (elapsedMillis >= connectTimeoutMillis) {
      #if ACE_TIME_NTP_CLOCK_DEBUG >= 1
        SERIAL_PORT_MONITOR.println(F("NtpClock::setup(): failed"));
      #endif
        mIsSetUp = false;
        return;
      }

      delay(500);
    }
  }

  mUdp.begin(mLocalPort);

#if ACE_TIME_NTP_CLOCK_DEBUG >= 1
  SERIAL_PORT_MONITOR.print(F("NtpClock::setup(): connected to"));
  SERIAL_PORT_MONITOR.println(WiFi.localIP());
  #if defined(ESP8266)
    SERIAL_PORT_MONITOR.print(F("Local port: "));
    SERIAL_PORT_MONITOR.println(mUdp.localPort());
  #endif
#endif

  mIsSetUp = true;
}

acetime_t NtpClock::getNow() const {
  if (!mIsSetUp || WiFi.status() != WL_CONNECTED) return kInvalidSeconds;

  sendRequest();

  uint16_t startTime = millis();
  while ((uint16_t) (millis() - startTime) < mRequestTimeout) {
    if (isResponseReady()) {
      return readResponse();
    }
  }
  return kInvalidSeconds;
}

void NtpClock::sendRequest() const {
  if (!mIsSetUp) return;
  if (WiFi.status() != WL_CONNECTED) {
  #if ACE_TIME_NTP_CLOCK_DEBUG >= 1
    SERIAL_PORT_MONITOR.println(
        F("NtpClock::sendRequest(): not connected"));
  #endif
    return;
  }

  // discard any previously received packets
  while (mUdp.parsePacket() > 0) {}

  #if ACE_TIME_NTP_CLOCK_DEBUG >= 2
    SERIAL_PORT_MONITOR.println(F("NtpClock::sendRequest(): sending request"));
  #endif

  // Get a random server from the pool. Unfortunately, hostByName() is a
  // blocking call. So if the DNS resolver goes flaky, everything stops.
  //
  // TODO: Change to a non-blocking NTP library.
  // TODO: check return value of hostByName() for errors
  // When there is an error, the ntpServerIP seems to become "0.0.0.0".
  IPAddress ntpServerIP;
  WiFi.hostByName(mServer, ntpServerIP);
  sendNtpPacket(ntpServerIP);
}

bool NtpClock::isResponseReady() const {
#if ACE_TIME_NTP_CLOCK_DEBUG >= 3
  static uint8_t rateLimiter;
#endif

  if (!mIsSetUp) return false;
  if (WiFi.status() != WL_CONNECTED) {
  #if ACE_TIME_NTP_CLOCK_DEBUG >= 3
    if (++rateLimiter == 0) {
      SERIAL_PORT_MONITOR.print("F[256]");
    }
  #endif
    return false;
  }
  #if ACE_TIME_NTP_CLOCK_DEBUG >= 3
    if (++rateLimiter == 0) {
      SERIAL_PORT_MONITOR.print(".[256]");
    }
  #endif

  return mUdp.parsePacket() >= kNtpPacketSize;
}

acetime_t NtpClock::readResponse() const {
  if (!mIsSetUp) return kInvalidSeconds;
  if (WiFi.status() != WL_CONNECTED) {
  #if ACE_TIME_NTP_CLOCK_DEBUG >= 2
    SERIAL_PORT_MONITOR.println("NtpClock::readResponse(): not connected");
  #endif
    return kInvalidSeconds;
  }

  // read packet into the buffer
  mUdp.read(mPacketBuffer, kNtpPacketSize);

  // Extract the NTP seconds, an unsigned number of seconds since the NTP epoch
  // of 1900-01-01. The NTP timestamp is a 32:32 fixed point number (64-bits
  // total) starting at byte 40. The location is determined from the following
  // headers (see https://en.wikipedia.org/wiki/User_Datagram_Protocol):
  //
  //  * IP header - 12 bytes
  //  * UDP header - 8 bytes
  //  * NTP message packet
  //    (https://www.meinbergglobal.com/english/info/ntp-packet.htm):
  //    * flags - 4 bytes
  //    * Root delay - 4 bytes
  //    * Root dispersion - 4 bytes
  //    * Reference identifier - 4 bytes
  //    * Reference timestamp - 8 bytes (40th byte here)
  //      * whole seconds (unsigned 32 bits)
  //      * fractional seconds (unsigned 32 bits, in units of 1/2^32 seconds)
  //
  // The timestamp is stored in big-endian order.
  uint32_t ntpSeconds =  (uint32_t) mPacketBuffer[40] << 24;
  ntpSeconds |= (uint32_t) mPacketBuffer[41] << 16;
  ntpSeconds |= (uint32_t) mPacketBuffer[42] << 8;
  ntpSeconds |= (uint32_t) mPacketBuffer[43];

  // Convert to AceTime epoch (as defined by Epoch::currentEpochYear()).
  acetime_t epochSeconds = convertNtpSecondsToAceTimeSeconds(ntpSeconds);
  #if ACE_TIME_NTP_CLOCK_DEBUG >= 1
    SERIAL_PORT_MONITOR.print(F("NtpClock::readResponse(): ntpSeconds="));
    SERIAL_PORT_MONITOR.print(ntpSeconds);
    SERIAL_PORT_MONITOR.print(F("; epochSeconds="));
    SERIAL_PORT_MONITOR.println(epochSeconds);
  #endif

  return epochSeconds;
}

// NTP epoch is 1900-01-01. Unix epoch is 1970-01-01. GPS epoch is 1980-01-06.
// AceTime v2 epoch is 2050-01-01 by default  but is adjustable at runtime.
acetime_t NtpClock::convertNtpSecondsToAceTimeSeconds(uint32_t ntpSeconds) {
  // Sometimes the NTP packet is garage and contains 0. Mark that as invalid.
  // NOTE: Is this necessary? Let's comment it out for now.
  //if (ntpSeconds == 0) return kInvalidSeconds;

  // Shift the NTP seconds to AceTime seconds, using uint32_t operations,
  // which performs a shift using modulo 2^32 arithmetic. This maps the entire
  // 32-bit range of NTP seconds to the 32-bit AceTime seconds, automatically
  // accounting for NTP rollovers, for any AceTime currentEpochYear().
  int32_t daysToCurrentEpochFromNtpEpoch =
      Epoch::daysToCurrentEpochFromConverterEpoch()
      + kDaysToConverterEpochFromNtpEpoch;
  uint32_t secondsToCurrentEpochFromNtpEpoch = (uint32_t) 86400
      * (uint32_t) daysToCurrentEpochFromNtpEpoch;
  uint32_t epochSeconds = ntpSeconds - secondsToCurrentEpochFromNtpEpoch;

  // Cast the uint32_t to an int32_t, which has the effect of mapping the upper
  // half of the AceTime seconds 32-bit range to its lower half. In other words,
  // `if epochSeconds > INT32_MAX: epochSeconds -= 2^32`.
  return (int32_t) epochSeconds;
}

void NtpClock::sendNtpPacket(const IPAddress& address) const {
#if ACE_TIME_NTP_CLOCK_DEBUG >= 2
  uint16_t startTime = millis();
#endif

  // set all bytes in the buffer to 0
  memset(mPacketBuffer, 0, kNtpPacketSize);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  mPacketBuffer[0] = 0b11100011;   // LI, Version, Mode
  mPacketBuffer[1] = 0;     // Stratum, or type of clock
  mPacketBuffer[2] = 6;     // Polling Interval
  mPacketBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  mPacketBuffer[12] = 49;
  mPacketBuffer[13] = 0x4E;
  mPacketBuffer[14] = 49;
  mPacketBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  mUdp.beginPacket(address, 123); //NTP requests are to port 123
  mUdp.write(mPacketBuffer, kNtpPacketSize);
  mUdp.endPacket();

#if ACE_TIME_NTP_CLOCK_DEBUG >= 2
  SERIAL_PORT_MONITOR.print(F("NtpClock::sendNtpPacket(): "));
  SERIAL_PORT_MONITOR.print((unsigned) ((uint16_t) millis() - startTime));
  SERIAL_PORT_MONITOR.println(" ms");
#endif
}

} // clock
} // ace_time

#endif
