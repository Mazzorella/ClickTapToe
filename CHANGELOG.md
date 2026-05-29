# Changelog

All notable changes to this project will be documented in this file.

The format is based on Keep a Changelog, and this project uses semantic versioning with tags in the form `vX.Y.Z`.

## [Unreleased]

## [0.3.1] - 2026-05-14

### Documentation

- Add contributing notes for commit style, branch policy, releases, and changelog updates.
- Standardize hardware board naming around the input hub and mouse pointer modules.

### Hardware

- Replace the old placeholder hardware folders with input-hub and mouse-pointer placeholders.
- Add a per-folder CERN-OHL-P v2 license placeholder for future input-hub hardware source.

## [0.3.0] - 2026-04-30

### Firmware

- Add PMW3610 pointing support on the XIAO SPI bus for the right-foot module.
- Add the external `badjeff/zmk-pmw3610-driver` module to the west manifest.
- Force the ClickTapToe build to USB HID mode with ZMK BLE disabled.
- Add a PMW3610 startup delay for the no-reset sensor wiring.

### Hardware

- Document the no-reset PMW3610 RJ45 pinout for the right-foot pointing module.
- Validate PMW3610 pointing over the RJ45 cable path.

## [0.2.0] - 2026-04-29

### Firmware

- AS5600 magnetic scroll wheel support for USB vertical scrolling.
- Documentation for the AS5600 scroll pin map and hardware test steps.

### Hardware

- No hardware design files changed.

## [0.1.0] - 2026-04-29

### Firmware

- Initial ClickTapToe ZMK shield for the Seeed Studio XIAO nRF52840 / XIAO BLE.
- USB mouse click support for left and right foot pedals.
- Hardware notes for known pedal GPIOs and reserved future inputs.
- Local `Makefile` wrapper for ZMK `west build`.
- GitHub Actions build workflow using the upstream ZMK user-config workflow.

### Hardware

- No hardware design files included yet.
