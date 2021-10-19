#!/usr/bin/python3
#
# Python script that regenerates the README.md from the embedded template. Uses
# ./generate_table.awk to regenerate the ASCII tables from the various *.txt
# files.

from subprocess import check_output

nano_results = check_output(
    "./generate_table.awk < nano.txt", shell=True, text=True)
micro_results = check_output(
    "./generate_table.awk < micro.txt", shell=True, text=True)
samd_results = check_output(
    "./generate_table.awk < samd.txt", shell=True, text=True)
stm32_results = check_output(
    "./generate_table.awk < stm32.txt", shell=True, text=True)
esp8266_results = check_output(
    "./generate_table.awk < esp8266.txt", shell=True, text=True)
esp32_results = check_output(
    "./generate_table.awk < esp32.txt", shell=True, text=True)
teensy32_results = check_output(
    "./generate_table.awk < teensy32.txt", shell=True, text=True)

print(f"""\
# Memory Benchmark

The `MemoryBenchmark.ino` was compiled with each `FEATURE_*` and the flash
memory and static RAM sizes were recorded. The `FEATURE_BASELINE` selection is
the baseline, and its memory usage  numbers are subtracted from the subsequent
`FEATURE_*` memory usage.

**Version**: AceTimeClock v1.0.2

**DO NOT EDIT**: This file was auto-generated using `make README.md`.

## How to Regenerate

To regenerate this README.md:

```
$ make clean_benchmarks
$ make benchmarks
$ make README.md
```

The `make benchmarks` target uses `collect.sh` script which calls `auniter.sh`
(https://github.com/bxparks/AUniter) to invoke the Arduino IDE programmatically.
It produces a `*.txt` file with the flash and ram usage information (e.g.
`nano.txt`). It now takes about 16 minutes to generate the `*.txt` files on my
quad-core Intel Core i7-3840QM CPU @ 2.80GHz laptop.

The `make README.md` command calls the `generated_readme.py` Python script which
generates this `README.md` file. The ASCII tables below are generated by the
`generate_table.awk` script, which takes each `*.txt` file and converts it to an
ASCII table.

## Library Size Changes

In v1.0.0:
* Initial split from AceTime v1.7.5 for v1.8.0.
* Convert `DS3231.h` into a template class for `<AceWire.h>`, replacing direct
  dependency on `<Wire.h>`.
    * Just including the `<Wire.h>` header causes flash memory to be consumed,
      even if `Wire` object is never used.
    * Saves 1000-1500 bytes of flash on AVR processors.
    * Saves 500 bytes of flash on SAMD.
    * Saves 4000 bytes of flash STM32.
    * Saves 500 bytes of flash on ESP8266.
    * Saves 3000-4000 bytes of flash on ESP32.
    * Saves 2500 bytes of flash on Teensy 3.2.
* Add benchmark for `DS3231Clock` separately from `SytemClock`.
* Add benchmarks for `SystemClockCoroutine` separately from `SystemClockLoop`.
* Rename `SystemClock` label to `SystemClockLoop`.

In v1.0.2:
* Fix bad merge in README.md.
* Create benchmarks for 3 variations of `DS3231Clock`: TwoWire, SimpleWire, and
  SimpleWireFast.
    * Make sure `<Wire.h>` is *not* included when using SimpleWireInterface and
      SimpleWireFastInterface.
    * Flash consumption reduced by 1600 bytes using SimpleWireInterface and
      2200 bytes using `SimpleWireInterface` on AVR.
    * Saves 800-8800 bytes of flash on 32-bit processors.

## Arduino Nano

* 16MHz ATmega328P
* Arduino IDE 1.8.16, Arduino CLI 0.19.2
* Arduino AVR Boards 1.8.3

```
{nano_results}
```

## Sparkfun Pro Micro

* 16 MHz ATmega32U4
* Arduino IDE 1.8.16, Arduino CLI 0.19.2
* SparkFun AVR Boards 1.1.13

```
{micro_results}
```

## SAMD21 M0 Mini

* 48 MHz ARM Cortex-M0+
* Arduino IDE 1.8.16, Arduino CLI 0.19.2
* Sparkfun SAMD Boards 1.8.4

```
{samd_results}
```

(SAMD compiler does not produce RAM usage numbers.)

## STM32 Blue Pill

* STM32F103C8, 72 MHz ARM Cortex-M3
* Arduino IDE 1.8.16, Arduino CLI 0.19.2
* STM32duino 2.0.0

```
{stm32_results}
```

An entry of `-1` indicates that the memory usage exceeded the maximum of the
microcontroller and the compiler did not generate the desired information.

## ESP8266

* NodeMCU 1.0, 80MHz ESP8266
* Arduino IDE 1.8.16, Arduino CLI 0.19.2
* ESP8266 Boards 3.0.2

```
{esp8266_results}
```

## ESP32

* ESP32-01 Dev Board, 240 MHz Tensilica LX6
* Arduino IDE 1.8.16, Arduino CLI 0.19.2
* ESP32 Boards 1.0.6

```
{esp32_results}
```

RAM usage remains constant as more objects are created, which indicates that an
initial pool of a certain minimum size is created regardless of the actual RAM
usage by objects.

## Teensy 3.2

* 96 MHz ARM Cortex-M4
* Arduino IDE 1.8.16, Arduino CLI 0.19.2
* Teensyduino 1.55

```
{teensy32_results}
```
""")
