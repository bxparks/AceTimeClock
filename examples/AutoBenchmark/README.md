# Auto Benchmark

Here are the results from `AutoBenchmark.ino` for various boards.

**Version**: AceTimeClock v1.0.0

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

v1.0.0:
* Initial extraction from AceTime library.

## Arduino Nano

* 16MHz ATmega328P
* Arduino IDE 1.8.16, Arduino CLI 0.19.2
* Arduino AVR Boards 1.8.3

```
Sizes of Objects:
sizeof(clock::DS3231Clock): 3
sizeof(clock::SystemClock): 28
sizeof(clock::SystemClockLoop): 41
sizeof(clock::SystemClockCoroutine): 52

CPU:
+--------------------------------------------------+----------+
| Method                                           |   micros |
|--------------------------------------------------+----------|
| SystemClockLoop                                  |   19.564 |
+--------------------------------------------------+----------+

```

## Sparkfun Pro Micro

* 16 MHz ATmega32U4
* Arduino IDE 1.8.16, Arduino CLI 0.19.2
* SparkFun AVR Boards 1.1.13

```
Sizes of Objects:
sizeof(clock::DS3231Clock): 3
sizeof(clock::SystemClock): 28
sizeof(clock::SystemClockLoop): 41
sizeof(clock::SystemClockCoroutine): 52

CPU:
+--------------------------------------------------+----------+
| Method                                           |   micros |
|--------------------------------------------------+----------|
| SystemClockLoop                                  |   19.564 |
+--------------------------------------------------+----------+

```

## SAMD21 M0 Mini

* 48 MHz ARM Cortex-M0+
* Arduino IDE 1.8.16, Arduino CLI 0.19.2
* Sparkfun SAMD Core 1.8.4

```
Sizes of Objects:
sizeof(clock::DS3231Clock): 8
sizeof(clock::SystemClock): 36
sizeof(clock::SystemClockLoop): 52
sizeof(clock::SystemClockCoroutine): 72

CPU:
+--------------------------------------------------+----------+
| Method                                           |   micros |
|--------------------------------------------------+----------|
| SystemClockLoop                                  |   19.564 |
+--------------------------------------------------+----------+

```

## STM32 Blue Pill

* STM32F103C8, 72 MHz ARM Cortex-M3
* Arduino IDE 1.8.16, Arduino CLI 0.19.2
* STM32duino 2.0.0

```
Sizes of Objects:
sizeof(clock::DS3231Clock): 8
sizeof(clock::SystemClock): 36
sizeof(clock::SystemClockLoop): 52
sizeof(clock::SystemClockCoroutine): 72

CPU:
+--------------------------------------------------+----------+
| Method                                           |   micros |
|--------------------------------------------------+----------|
| SystemClockLoop                                  |   19.564 |
+--------------------------------------------------+----------+

```

## ESP8266

* NodeMCU 1.0 clone, 80MHz ESP8266
* Arduino IDE 1.8.16, Arduino CLI 0.19.2
* ESP8266 Boards 3.0.2

```
Sizes of Objects:
sizeof(clock::DS3231Clock): 8
sizeof(clock::NtpClock): 92
sizeof(clock::SystemClock): 36
sizeof(clock::SystemClockLoop): 52
sizeof(clock::SystemClockCoroutine): 72

CPU:
+--------------------------------------------------+----------+
| Method                                           |   micros |
|--------------------------------------------------+----------|
| SystemClockLoop                                  |   19.564 |
+--------------------------------------------------+----------+

```

## ESP32

* ESP32-01 Dev Board, 240 MHz Tensilica LX6
* Arduino IDE 1.8.16, Arduino CLI 0.19.2
* ESP32 Boards 1.0.6

```
Sizes of Objects:
sizeof(clock::DS3231Clock): 8
sizeof(clock::NtpClock): 116
sizeof(clock::SystemClock): 36
sizeof(clock::SystemClockLoop): 52
sizeof(clock::SystemClockCoroutine): 72

CPU:
+--------------------------------------------------+----------+
| Method                                           |   micros |
|--------------------------------------------------+----------|
| SystemClockLoop                                  |   19.564 |
+--------------------------------------------------+----------+

```

Note: Once the benchmark of the function under test becomes smaller than the
duration of an empty loop, the numbers become unreliable.

## Teensy 3.2

* 96 MHz ARM Cortex-M4
* Arduino IDE 1.8.16, Arduino CLI 0.19.2
* Teensyduino 1.55
* Compiler options: "Faster"

```
Sizes of Objects:
sizeof(clock::DS3231Clock): 8
sizeof(clock::SystemClock): 36
sizeof(clock::SystemClockLoop): 52
sizeof(clock::SystemClockCoroutine): 72

CPU:
+--------------------------------------------------+----------+
| Method                                           |   micros |
|--------------------------------------------------+----------|
| SystemClockLoop                                  |   19.564 |
+--------------------------------------------------+----------+

```

