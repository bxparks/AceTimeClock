/*
 * MIT License
 * Copyright (c) 2021 Brian T. Park
 */

/**
 * @mainpage AceTimeClock Library
 *
 * This is the Doxygen documentation for the
 * <a href="https://github.com/bxparks/AceTimeClock">AceTimeClock Library</a>.
 *
 * Click on the "Classes" menu above to see the list of classes.
 *
 * Click on the "Files" menu above to see the list of header files.
 */

#ifndef ACE_TIME_CLOCK_ACE_TIME_CLOCK_H
#define ACE_TIME_CLOCK_ACE_TIME_CLOCK_H

// Blacklist boards using new Arduino API due to incompatibilities. This
// currently includes all megaAVR boards and SAMD21 boards using arduino::samd
// >= 1.8.10. Boards using arduino:samd <= 1.8.9 or SparkFun:samd are fine.
#if defined(ARDUINO_ARCH_MEGAAVR)
#error MegaAVR not supported, https://github.com/bxparks/AceTime/issues/44

#elif defined(ARDUINO_ARCH_SAMD) && defined(ARDUINO_API_VERSION)
#error SAMD21 with arduino:samd >= 1.8.10 not supported, https://github.com/bxparks/AceTime/issues/45

#elif defined(ARDUINO_API_VERSION)
#error Platforms using ArduinoCore-API not supported
#endif

#include "ace_time/clock/Clock.h"
#include "ace_time/clock/NtpClock.h"
#include "ace_time/clock/DS3231Clock.h"
#include "ace_time/clock/UnixClock.h"
#include "ace_time/clock/SystemClock.h"
#include "ace_time/clock/SystemClockLoop.h"
#include "ace_time/clock/SystemClockCoroutine.h"

#if defined(ARDUINO_ARCH_STM32)
#include "ace_time/clock/StmRtcClock.h"
#include "ace_time/clock/Stm32F1Clock.h"
#endif // #if defined(ARDUINO_ARCH_STM32)


// Version format: xxyyzz == "xx.yy.zz"
#define ACE_TIME_CLOCK_VERSION 10002
#define ACE_TIME_CLOCK_VERSION_STRING "1.0.2"

#endif
