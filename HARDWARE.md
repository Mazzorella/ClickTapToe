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
| Future footswitch / TTS / spare input | `D2` | `&kp C_PLAY_PAUSE` | Test direct GPIO input |
| Future footswitch / TTS / spare input | `D3` | `&kp C_VOL_UP` | Test direct GPIO input |

## Current Scroll Input

The current firmware supports vertical scroll with a Bourns
`PEC11R-4215F-S0024` encoder in the input hub. This encoder has 24 detents and
24 pulses per full rotation.

| Function | XIAO pin | Status |
| --- | --- | --- |
| Scroll option bus / EC11 channel A | `D4` | Implemented as EC11 GPIO |
| Scroll option bus / EC11 channel B | `D5` | Implemented as EC11 GPIO |
| EC11 common | `GND` | Implemented |
| EC11 push switch | unused | Reserved |

The scroll option bus uses `D4`/`D5` for the EC11 validation build. These pins
are also reserved for a possible AS5600 I2C scroll option in a later hardware
revision.

ZMK is configured with `steps = <96>` for the EC11 quadrature transition count
and `triggers-per-rotation = <24>` so the scroll behavior triggers once per
detent.

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
| Sensor reset | `D6` | `nRESET` | `8` / brown | Reserved, not wired in firmware |

The mouse pointer module connects to the input hub through an 8P8C modular jack
used as a ClickTapToe sensor cable, not Ethernet. The cable pairing is:

| Pair | RJ45 pins | Signals |
| --- | --- | --- |
| Pair 1 | 1-2 | `VIN` / `3V3` + `GND` |
| Pair 2 | 3-6 | `SCLK` + `GND` |
| Pair 3 | 4-5 | `SDIO` + `nCS` |
| Pair 4 | 7-8 | `MOTION` + `nRESET` |

The PMW3610 orientation is corrected in firmware with `swap-xy` and `invert-y`.
The mouse pointer board routes `nRESET` over RJ45, and the input hub reserves
XIAO `D6` for it. The current PMW3610 firmware binding does not expose a clean
hardware reset GPIO, so reset remains unwired in firmware for this test plan.
