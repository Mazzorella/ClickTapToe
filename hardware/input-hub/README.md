# Input Hub Hardware

This folder contains source files and notes for the ClickTapToe input hub PCB.

The input hub is the central electronics board. It hosts the XIAO nRF52840,
connects to the computer over USB, receives the external pedal TS jacks,
connects to the mouse pointer board over RJ45, and interfaces with the EC11
scroll encoder.

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
| 8 | `nRESET` | PMW3610 reset |

Twisted pair grouping:

| Pair | RJ45 pins | Signals |
| --- | --- | --- |
| Pair 1 | 1-2 | `VIN` / `3V3` + `GND` |
| Pair 2 | 3-6 | `SCLK` + `GND` |
| Pair 3 | 4-5 | `SDIO` + `nCS` |
| Pair 4 | 7-8 | `MOTION` + `nRESET` |

Series resistors for `SCLK`, `SDIO`, `nCS`, and `nRESET` belong on the input hub
side. The mouse pointer board only adds a local 100 ohm series resistor on
`MOTION`.

## Planned Layout

- `kicad/` for KiCad project source files
- `production/` for generated Gerbers, drill files, BOMs, and placement files
