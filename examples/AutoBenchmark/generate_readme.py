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
samd21_results = check_output(
    "./generate_table.awk < samd21.txt", shell=True, text=True)
stm32_results = check_output(
    "./generate_table.awk < stm32.txt", shell=True, text=True)
esp8266_results = check_output(
    "./generate_table.awk < esp8266.txt", shell=True, text=True)
esp32_results = check_output(
    "./generate_table.awk < esp32.txt", shell=True, text=True)
teensy32_results = check_output(
    "./generate_table.awk < teensy32.txt", shell=True, text=True)

print(f"""\
# Auto Benchmark

Here are the results from `AutoBenchmark.ino` for various boards.

**Version**: AceTimeClock v1.2.4

**DO NOT EDIT**: This file was auto-generated using `make README.md`.

## Dependencies

This program depends on the following libraries:

* [AceTime](https://github.com/bxparks/AceTime)
* [AceTimeClock](https://github.com/bxparks/AceTimeClock)
* [AceCommon](https://github.com/bxparks/AceCommon)
* [AceRoutine](https://github.com/bxparks/AceRoutine)

## How to Generate

This requires the [AUniter](https://github.com/bxparks/AUniter) script
to execute the Arduino IDE programmatically.

The `Makefile` has rules to generate the `*.txt` results file for several
microcontrollers that I usually support, but the `$ make benchmarks` command
does not work very well because the USB port of the microcontroller is a
dynamically changing parameter. I created a semi-automated way of collect the
`*.txt` files:

1. Connect the microcontroller to the serial port. I usually do this through a
USB hub with individually controlled switch.
2. Type `$ auniter ports` to determine its `/dev/ttyXXX` port number (e.g.
`/dev/ttyUSB0` or `/dev/ttyACM0`).
3. If the port is `USB0` or `ACM0`, type `$ make nano.txt`, etc.
4. Switch off the old microontroller.
5. Go to Step 1 and repeat for each microcontroller.

The `generate_table.awk` program reads one of `*.txt` files and prints out an
ASCII table that can be directly embedded into this README.md file. For example
the following command produces the table in the Nano section below:

```
$ ./generate_table.awk < nano.txt
```

Fortunately, we no longer need to run `generate_table.awk` for each `*.txt`
file. The process has been automated using the `generate_readme.py` script which
will be invoked by the following command:
```
$ make README.md
```

The CPU times below are given in microseconds.

## CPU Time Changes

**v1.0.0**

* Initial extraction from AceTime library.

**v1.0.2**

* Regenerate all benchmarks to get correct numbers. Previous numbers were all
  identical. I probably added them as placeholders, then forgot to regenerate.

**v1.1.0**

* Upgrade tool chains
    * Arduino IDE from 1.8.16 to 1.8.19
    * Arduino CLI from 0.19.2 to 0.20.2
    * STM32duino from 2.0.0 to 2.2.0
    * AVR Core from 1.8.3 to 1.8.4
    * ESP32 Core from 1.0.6 to 2.0.2
    * Teensyduino from 1.55 to 1.56
* Add EmptyLoop entry and subtract from SystemClockLoop.
    * The results for ESP8266 is surprisingly slow, slower than the AVR. Not
      sure why.

**v1.2.2**

* Upgrade tool chains
    * Arduino CLI to 0.27.1
    * STM32duino to 2.3.0
    * AVR Core from 1.8.4 to 1.8.5
    * ESP32 Core from 2.0.2 to 2.0.5
    * Teensyduino from 1.56 to 1.57
* Upgrade to AceTime v2.0
    * Update `NtpClock` to use AceTime `Epoch::currentEpochYear()`
* No significant changes to CPU time.

**v1.2.4**

* Upgrade tool chains
    * Arduino CLI to 0.31.0
    * STM32duino to 2.4.0
    * AVR Core to 1.8.6
    * ESP32 Core to 2.0.7
* Upgrade to AceTime v2.2

## Arduino Nano

* 16MHz ATmega328P
* Arduino IDE 1.8.19, Arduino CLI 0.31.0
* Arduino AVR Boards 1.8.6

```
{nano_results}
```

## Sparkfun Pro Micro

* 16 MHz ATmega32U4
* Arduino IDE 1.8.19, Arduino CLI 0.31.0
* SparkFun AVR Boards 1.1.13

```
{micro_results}
```

## Seeed Studio XIAO SAMD21

* SAMD21, 48 MHz ARM Cortex-M0+
* Arduino IDE 1.8.19, Arduino CLI 0.31.0
* Seeeduino 1.8.3

```
{samd21_results}
```

## STM32 Blue Pill

* STM32F103C8, 72 MHz ARM Cortex-M3
* Arduino IDE 1.8.19, Arduino CLI 0.31.0
* STM32duino 2.4.0

```
{stm32_results}
```

## ESP8266

* NodeMCU 1.0 clone, 80MHz ESP8266
* Arduino IDE 1.8.19, Arduino CLI 0.31.0
* ESP8266 Boards 3.0.2

```
{esp8266_results}
```

## ESP32

* ESP32-01 Dev Board, 240 MHz Tensilica LX6
* Arduino IDE 1.8.19, Arduino CLI 0.31.0
* ESP32 Boards 2.0.7

```
{esp32_results}
```

## Teensy 3.2

* 96 MHz ARM Cortex-M4
* Arduino IDE 1.8.19, Arduino CLI 0.31.0
* Teensyduino 1.57
* Compiler options: "Faster"

```
{teensy32_results}
```
""")
