# Input Hub Hardware

This folder on `main` contains the input hub jumper revision, pending hardware
validation. To build the tested v0.4.0 board, use the
[v0.4.0 KiCad source](https://github.com/Mazzorella/ClickTapToe/tree/v0.4.0/hardware/input-hub/kicad)
and [matching manufacturing files](https://mazzorella.github.io/ClickTapToe/docs/hardware.html#files).

The input hub is the central electronics board. It hosts the XIAO nRF52840,
connects to the computer over USB, receives the external pedal TS jacks,
connects to the mouse pointer board over RJ45, and interfaces with the AS5600
scroll sensor.

See the [hub assembly photo guide](https://mazzorella.github.io/ClickTapToe/docs/hub-assembly.html)
for mounting the board, scroll wheel, and sensor in the enclosure. See the
[hardware notes](../../HARDWARE.md#current-pedal-inputs) for the current pedal
jumper settings.

## XIAO Pin Map

The USB firmware uses `D0` and `D1` for the click pedals, `D4` and `D5` for
the AS5600 scroll sensor, and SPI for the PMW3610 pointer.

| XIAO pin | Signal | Firmware status |
| --- | --- | --- |
| `D0` | Left click pedal | Implemented |
| `D1` | Right click pedal | Implemented |
| `D2` | Future footswitch / TTS / spare input | Test direct GPIO input, emits play/pause |
| `D3` | Future footswitch / TTS / spare input | Test direct GPIO input, emits volume up |
| `D4` | AS5600 I2C bus | Implemented as `&xiao_i2c` |
| `D5` | AS5600 I2C bus | Implemented as `&xiao_i2c` |
| `D6` | PMW3610 `nRESET` | Routed on the board. Firmware uses SPI reset. |
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
`nRESET` is routed from RJ45 pin 8 to XIAO `D6`. The current firmware initializes
the PMW3610 through SPI reset.

## Online Review

Open the [input hub KiCanvas viewer](https://mazzorella.github.io/ClickTapToe/hardware/input-hub/kicanvas.html)
to inspect the tested v0.4.0 schematic and PCB in a browser.

## Jumper revision files

- [KiCad project source](kicad/)
- [Fabrication and assembly files](kicad/production/), including Gerbers, drill
  files, the bill of materials, and the placement file.
