# ClickTapToe Hardware Notes

This file tracks the hardware assumptions used by the firmware.

## Controller

- Controller: Seeed Studio XIAO nRF52840 / XIAO BLE
- ZMK board target: `xiao_ble//zmk`
- Connection mode: USB HID; ZMK BLE disabled

## Current Pedal Inputs

The current firmware uses direct GPIO inputs. Pedals short the signal pin to `GND`, so the firmware enables internal pull-ups and treats the inputs as active-low.

| Function | XIAO pin | ZMK binding | Status |
| --- | --- | --- | --- |
| Left click pedal | `D0` | `&mkp LCLK` | Implemented |
| Right click pedal | `D1` | `&mkp RCLK` | Implemented |
| Third pedal jack | `TBD` | none | Reserved |

## Current Scroll Input

The current firmware supports vertical scroll with an EC11 encoder in the main module.

| Function | XIAO pin | Status |
| --- | --- | --- |
| EC11 channel A | `D2` | Implemented |
| EC11 channel B | `D3` | Implemented |
| EC11 common | `GND` | Implemented |
| EC11 push switch | unused | Reserved |

## Current Pointing Input

The current firmware supports PMW3610 pointing from the right-foot module over the XIAO SPI bus. The PMW3610 uses a bidirectional `SDIO` pin, so firmware maps both SPI MOSI and MISO to XIAO `D10`.

| Function | XIAO pin | PMW3610 pin | RJ45 pin / color | Status |
| --- | --- | --- | --- | --- |
| Sensor power | `3V3` | `VDD` / `VIN` | `7` / white-brown | Implemented |
| Sensor ground | `GND` | `GND` | `2` / orange, `6` / green, `8` / brown | Implemented |
| SPI clock | `D8` / `P1.13` | `SCK` | `1` / white-orange | Implemented |
| SPI data | `D10` / `P1.15` | `SDIO` | `3` / white-green | Implemented |
| Chip select | `D9` / `P1.14` | `NCS` | `4` / blue | Implemented |
| Motion interrupt | `D7` / `P1.12` | `MOT` | `5` / white-blue | Implemented |
| Sensor reset | none | `nRESET` | none | Not routed |

The right-foot pointing module connects through RJ45. The high-activity `SCK` and `SDIO` signals are paired with ground on their respective twisted pairs.

The PMW3610 orientation is corrected in firmware with `swap-xy` and `invert-y`.
Because `nRESET` is not routed, firmware adds an extra PMW3610 power-up delay before sensor initialization.
