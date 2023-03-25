#line 2 "SystemClockTestCoroutine.ino"

#include <AUnitVerbose.h>
#include <AceRoutine.h> // enable SystemClockCoroutine
#include <AceTimeClock.h>
#include <AceCommon.h> // PrintStr

#define SYSTEM_CLOCK_TEST_DEBUG 0

using namespace aunit;
using namespace ace_time;
using namespace ace_time::clock;

// Create an instance and insert into its internal linked list.
SystemClockCoroutine sysClock(nullptr, nullptr);

// Check that the CoroutineScheduler is able to see the SystemClockCoroutine.
// Previously, there was a bug that prevented this.
test(SystemClockCoroutineAppearsInScheduler) {
  sysClock.setName("sysClock");
  ace_routine::CoroutineScheduler::setup();

  ace_common::PrintStr<100> str;
  ace_routine::CoroutineScheduler::list(str);
  assertEqual("Coroutine sysClock; status: Yielding\r\n", str.cstr());
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
