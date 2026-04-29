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

## Planned Hardware

The full ClickTapToe hardware design also includes:

- A large scroll wheel coupled to an EC11 encoder in the main module.
- An RJ45 connection from the center pedal module to the right-foot pointing module.
- A right-foot pointing module with a PMW3610 sensor.

These hardware pieces are intentionally not enabled in the current firmware baseline.
