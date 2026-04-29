# ClickTapToe Hardware Notes

This file tracks the hardware assumptions used by the firmware.

## Controller

- Controller: Seeed Studio XIAO nRF52840 / XIAO BLE
- ZMK board target: `xiao_ble//zmk`
- Connection mode: USB HID

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

## Planned Hardware

The full ClickTapToe hardware design also includes:

- An RJ45 connection from the center pedal module to the right-foot pointing module.
- A right-foot pointing module with a PMW3610 sensor.

These hardware pieces are intentionally not enabled in the current firmware.
