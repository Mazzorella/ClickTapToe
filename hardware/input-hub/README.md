# Input Hub Hardware

This folder contains source files and notes for the ClickTapToe input hub PCB.

The input hub is the central electronics board. It hosts the XIAO nRF52840,
connects to the computer over USB, receives the external pedal TS jacks,
connects to the mouse pointer board over RJ45, and interfaces with the AS5600
scroll sensor.

## XIAO Pin Test Plan

This validation plan keeps the existing click pedals on `D0`/`D1`, uses the
AS5600 scroll sensor on the `D4`/`D5` I2C bus, and leaves the PMW3610 SPI
signals unchanged.

| XIAO pin | Signal plan | Firmware status |
| --- | --- | --- |
| `D0` | Left click pedal | Implemented |
| `D1` | Right click pedal | Implemented |
| `D2` | Future footswitch / TTS / spare input | Test direct GPIO input, emits play/pause |
| `D3` | Future footswitch / TTS / spare input | Test direct GPIO input, emits volume up |
| `D4` | AS5600 I2C bus | Implemented as `&xiao_i2c` |
| `D5` | AS5600 I2C bus | Implemented as `&xiao_i2c` |
| `D6` | PMW3610 `nRESET` | Implemented as active-low hardware reset |
| `D7` | PMW3610 `MOTION` | Implemented as PMW3610 interrupt |
| `D8` / `P1.13` | PMW3610 `SCLK` | Implemented |
| `D9` / `P1.14` | PMW3610 `nCS` | Implemented |
| `D10` / `P1.15` | PMW3610 `SDIO` | Implemented |

## Mouse Pointer RJ45 Pinout

The mouse pointer connector is an 8P8C modular jack used for ClickTapToe sensor
wiring. It is not Ethernet. The input hub RJ45 wiring must match the mouse
pointer board exactly.

| RJ45 pin | Signal | Input hub side |
| --- | --- | --- |
| 1 | `VIN` / `3V3` | 3.3 V sensor power |
| 2 | `GND` | Ground |
| 3 | `SCLK` | PMW3610 SPI clock |
| 4 | `SDIO` | PMW3610 bidirectional SPI data |
| 5 | `nCS` | PMW3610 chip select |
| 6 | `GND` | Ground |
| 7 | `MOTION` | PMW3610 motion interrupt |
| 8 | `nRESET` | PMW3610 reset routed to XIAO `D6` |

Twisted pair grouping:

| Pair | RJ45 pins | Signals |
| --- | --- | --- |
| Pair 1 | 1-2 | `VIN` / `3V3` + `GND` |
| Pair 2 | 3-6 | `SCLK` + `GND` |
| Pair 3 | 4-5 | `SDIO` + `nCS` |
| Pair 4 | 7-8 | `MOTION` + `nRESET` |

Series resistors for `SCLK`, `SDIO`, `nCS`, and `nRESET` belong on the input hub
side. The mouse pointer board only adds a local 100 ohm series resistor on
`MOTION`, and the input hub provides the `MOTION` pull-up to `3V3`.

`MOTION` is active-low and is handled by firmware on XIAO `D7`; the board also
has a 10k pull-up to `3V3` on that line.
`nRESET` is present in the cable on RJ45 pin 8 and is driven by firmware on XIAO
`D6` during PMW3610 initialization and retry.

## Planned Layout

- `kicad/` for KiCad project source files
- `kicad/production/` for generated Gerbers, drill files, BOMs, and placement files
