#line 2 "NtpClockTest.ino"

#include <AUnit.h>
#include <AceTimeClock.h>

using namespace aunit;
using ace_time::LocalDate;
using ace_time::clock::NtpClock;

static const int64_t kSecondsTo1900From1970 = -2208988800;
static const int64_t kSecondsTo2000From1970 = 946684800;
static const int64_t kSecondsTo2050From1970 = 2524608000;
static const int64_t kSecondsTo2100From1970 = 4102444800;

// Largest NTP seconds value for NTP Era 0, expressed as seconds from 1970.
// 2,085,978,495 = 2036-02-07 06:28:15
static const int64_t kSecondsToNtpEra0MaxFrom1970 =
    (UINT32_MAX) + kSecondsTo1900From1970;

test(NtpClockTest, convertNtpSecondsToAceTimeSeconds) {
   // NTP epoch of 0 is sometimes returned by invalid packets, so NtpClock
   // returns kInvalidEpochSeconds to indicate an error. Unfortunately, 0 also
   // corresponds to the first value of the new NTP era after rollover in the
   // year 2036, in other words, 0 also represents 2036-02-07 06:28:16. The
   // AceTimeClock library will ignore that one second for sync'ing purposes.
   //
   // NOTE: Commented out pending whether this check is required.
   // assertEqual(
   //   LocalDate::kInvalidEpochSeconds,
   //   NtpClock::convertNtpSecondsToAceTimeSeconds(0));

   // NTP seconds of UINT32_MAX is interpreted be in NTP era 0 and represents
   // 2036-02-07 06:28:15.
   assertEqual(
      (int32_t) (kSecondsToNtpEra0MaxFrom1970 - kSecondsTo2050From1970),
      NtpClock::convertNtpSecondsToAceTimeSeconds(UINT32_MAX));

   // NTP seconds of 1 is interpreted be in NTP era 1, after rolling over in
   // 2036, and represents 2036-02-07 06:28:17.
   assertEqual(
      (int32_t) (kSecondsToNtpEra0MaxFrom1970 - kSecondsTo2050From1970 + 2),
      NtpClock::convertNtpSecondsToAceTimeSeconds(1));

   // 2000-01-01 (AceTime v1 epoch) is interpreted to be in NTP era 0,
   // before the NTP roll over in 2036.
   assertEqual(
      (int32_t) (kSecondsTo2000From1970 - kSecondsTo2050From1970),
      NtpClock::convertNtpSecondsToAceTimeSeconds((uint32_t)
          (kSecondsTo2000From1970 - kSecondsTo1900From1970)));

  // 2050-01-01 (AceTime v2 epoch) is interpreted to be in NTP era 1, since it
  // is after 2036.
  assertEqual((int32_t) 0,
      NtpClock::convertNtpSecondsToAceTimeSeconds((uint32_t)
          (kSecondsTo2050From1970 - kSecondsTo1900From1970)));

  // 2100-01-01 (50 years past v2 epoch) is interpreted to in NTP Era 1.
  assertEqual((int32_t) (kSecondsTo2100From1970 - kSecondsTo2050From1970),
      NtpClock::convertNtpSecondsToAceTimeSeconds((uint32_t)
          (kSecondsTo2100From1970 - kSecondsTo1900From1970)));
}

//---------------------------------------------------------------------------

void setup() {
#if ! defined(EPOXY_DUINO)
  delay(1000); // wait to prevent garbage on SERIAL_PORT_MONITOR
#endif
  SERIAL_PORT_MONITOR.begin(115200);
  while (!SERIAL_PORT_MONITOR); // Leonardo/Micro
}

void loop() {
  TestRunner::run();
}
