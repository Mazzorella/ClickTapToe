# ClickTapToe Hardware Notes

This file tracks the hardware assumptions used by the firmware.

## Controller

- Controller: Seeed Studio XIAO nRF52840 / XIAO BLE
- ZMK board target: `xiao_ble//zmk`
- Connection mode: USB HID; ZMK BLE disabled

## Current Pedal Inputs

The current firmware uses direct GPIO inputs. Yamaha-style pedals short the
signal pin to `GND` at rest and open when pressed, so the firmware enables
internal pull-ups and treats the inputs as active-high.

Use normally closed pedals with the v0.4.0 USB firmware. Plug both pedals in
before connecting the hub to the computer, since an open jack reads as a
pressed pedal on the tested board.

The input hub revision on `main` adds optional 0 ohm jumpers `JP1` and `JP2`
from the pedal jacks' tip-normal contacts to `GND`. These are designed to keep
unplugged jacks idle with normally closed pedals. The change is awaiting
hardware validation. A normally open pedal setup leaves the jumpers
unpopulated and requires matching active-low firmware.

Known jack normal contacts:

| Jack type | Routed signal contact | Normal contact to optionally ground |
| --- | --- | --- |
| 1/4 inch Amphenol `ACJM-IH` | `T` / tip | `TN` / tip-normal |
| 3.5 mm `PJ-307` | Pin 5 | Pin 4 |

| Function | XIAO pin | ZMK binding | Status |
| --- | --- | --- | --- |
| Left click pedal | `D0` | `&mkp LCLK` | Implemented |
| Right click pedal | `D1` | `&mkp RCLK` | Implemented |
| Future footswitch / TTS / spare input | `D2` | `&kp C_PLAY_PAUSE` | Test direct GPIO input |
| Future footswitch / TTS / spare input | `D3` | `&kp C_VOL_UP` | Test direct GPIO input |

## Current Scroll Input

The current firmware supports vertical scroll with an AS5600 magnetic angle
sensor in the input hub. The scroll wheel spins freely with a diametric magnet
mounted on the wheel shaft near the sensor.

| Function | XIAO pin | Status |
| --- | --- | --- |
| AS5600 I2C bus | `D4` | Implemented as `&xiao_i2c` |
| AS5600 I2C bus | `D5` | Implemented as `&xiao_i2c` |
| AS5600 `VCC` | `3V3` | Board routed |
| AS5600 `GND` | `GND` | Board routed |

The AS5600 connects to `J5` on the hub using a STEMMA QT / Qwiic cable with
4-pin JST-SH plugs at both ends. The connector pitch is 1.0 mm.

| J5 pin | Signal |
| --- | --- |
| 1 | Ground |
| 2 | 3.3 V power |
| 3 | SDA |
| 4 | SCL |

The AS5600 uses fixed I2C address `0x36`. The current firmware sets
`triggers-per-rotation = <24>` for the scroll wheel.

## Current Pointing Input

The current firmware supports PMW3610 pointing from the mouse pointer module over the XIAO SPI bus. The PMW3610 uses a bidirectional `SDIO` pin, so firmware maps both SPI MOSI and MISO to XIAO `D10`.

| Function | XIAO pin | PMW3610 pin | RJ45 pin / color | Status |
| --- | --- | --- | --- | --- |
| Sensor power | `3V3` | `VIN` / `VDDIO` default | `1` / white-orange | Board routed |
| Sensor ground | `GND` | `GND` | `2` / orange, `6` / green | Board routed |
| SPI clock | `D8` / `P1.13` | `SCLK` | `3` / white-green | Board routed |
| SPI data | `D10` / `P1.15` | `SDIO` | `4` / blue | Board routed |
| Chip select | `D9` / `P1.14` | `nCS` | `5` / white-blue | Board routed |
| Motion interrupt | `D7` / `P1.12` | `MOTION` | `7` / white-brown | Board routed |
| Sensor reset | `D6` | `nRESET` | `8` / brown | Board routed; current firmware uses SPI/software reset |

The mouse pointer module connects to the input hub through an 8P8C modular jack
used as a ClickTapToe sensor cable, not Ethernet. The cable pairing is:

| Pair | RJ45 pins | Signals |
| --- | --- | --- |
| Pair 1 | 1-2 | `VIN` / `3V3` + `GND` |
| Pair 2 | 3-6 | `SCLK` + `GND` |
| Pair 3 | 4-5 | `SDIO` + `nCS` |
| Pair 4 | 7-8 | `MOTION` + `nRESET` |

The PMW3610 orientation is corrected in firmware with `swap-xy` and `invert-y`.
The mouse pointer board routes `nRESET` over RJ45, and the input hub connects it
to XIAO `D6`. The input hub also provides a 10k pull-up to `3V3` on the
active-low `MOTION` interrupt line. This validated firmware build keeps the
original PMW3610 driver behavior and relies on SPI/software reset; D6 is routed
for a later hardware-reset recovery update.
