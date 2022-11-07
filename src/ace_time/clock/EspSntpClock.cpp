#include <Arduino.h>
#include "EspSntpClock.h"

#if defined(ESP8266) || defined(ESP32) || defined(EPOXY_CORE_ESP8266)

namespace ace_time {
namespace clock {

// Value of time_t for 2000-01-01 00:00:00, used to detect invalid SNTP
// responses.
static const time_t EPOCH_2000_01_01 = 946684800;

const char EspSntpClock::kDefaultNtpServer[] = "pool.ntp.org";

bool EspSntpClock::setup(const char* ntpServer, uint32_t timeoutMillis) {
  Serial.print(F("Configuring SNTP"));

  // Use UTC timezone with no STD offset and no DST offset.
  configTime(0 /*timezone*/, 0 /*dst_sec*/, ntpServer);

  // Wait until SNTP stabilizes by ignoring values before year 2000.
  uint32_t startMillis = millis();
  while (true) {
    Serial.print('.'); // each '.' represents an attempt
    time_t now = time(nullptr);
    if (now >= EPOCH_2000_01_01) {
      Serial.println(F(" Done."));
      return true;
    }

    uint32_t elapsedMillis = millis() - startMillis;
    if (elapsedMillis >= timeoutMillis) {
      Serial.println(F(" Failed!"));
      return false;
    }

    delay(500);
  }
}

}
}

#endif
