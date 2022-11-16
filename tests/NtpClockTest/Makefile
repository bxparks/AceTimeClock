# See https://github.com/bxparks/EpoxyDuino for documentation about this
# Makefile to compile and run Arduino programs natively on Linux or MacOS.

APP_NAME := NtpClockTest
ARDUINO_LIBS := AUnit AceCommon AceSorting AceTime AceTimeClock ESP8266WiFi
ARDUINO_LIB_DIRS := $(abspath ../../..) $(abspath ../../../EspMock/libraries)
EPOXY_CORE := EPOXY_CORE_ESP8266
include ../../../EpoxyDuino/EpoxyDuino.mk
