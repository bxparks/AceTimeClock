# See https://github.com/bxparks/EpoxyDuino for documentation about this
# Makefile to compile and run Arduino programs natively on Linux or MacOS.

APP_NAME := HelloNtpClock
ARDUINO_LIBS := AceCommon AceSorting AceTime AceTimeClock AceRoutine ESP8266WiFi
ARDUINO_LIB_DIRS := $(abspath ../../..) $(abspath ../../../EspMock/libraries)
EPOXY_CORE := EPOXY_CORE_ESP8266
MORE_CLEAN := more_clean
include ../../../EpoxyDuino/EpoxyDuino.mk
