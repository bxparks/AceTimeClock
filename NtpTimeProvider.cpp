/*
 * MIT License
 * Copyright (c) 2018 Brian T. Park
 */

#include <Arduino.h>
#include "../common/flash.h"
#include "NtpTimeProvider.h"

#if defined(ESP8266) || defined(ESP32)

namespace ace_time {
namespace clock {

const char NtpTimeProvider::kNtpServerName[] = "us.pool.ntp.org";

// Moved from. h to .cpp because F() strings in inline contexts under ESP8266
// causes problems with other F() strings.
void NtpTimeProvider::setup(const char* ssid, const char* password) {
  uint16_t startMillis = millis();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    uint16_t elapsedMillis = millis() - startMillis;
    if (elapsedMillis >= kConnectTimeoutMillis) {
      mIsSetUp = false;
      return;
    }

    delay(500);
  }

  mUdp.begin(mLocalPort);

#if ACE_TIME_NTP_TIME_PROVIDER_DEBUG == 1
  #if defined(ESP8266)
    SERIAL_PORT_MONITOR.print(F("Local port: "));
    SERIAL_PORT_MONITOR.println(mUdp.localPort());
  #endif
#endif

  mIsSetUp = true;
}

}
}

#endif
