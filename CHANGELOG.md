# Changelog

All notable changes to this project will be documented in this file.

The format is based on Keep a Changelog, and this project uses semantic versioning with tags in the form `vX.Y.Z`.

## [Unreleased]

### Firmware

- Restore the validated USB HID ClickTapToe firmware configuration for the
  working input hub PCB.
- Add AS5600 magnetic scroll sensor support for the input hub scroll wheel.
- Add XIAO onboard status LED activity reporting for bring-up feedback.

### Hardware

- Add ClickTapToe-maintained KiCad source for the mouse pointer PCB.
- Add KiCad source, JLC assembly preparation, production files, 3D models, and
  KiCanvas previews for the input hub PCB.
- Add RJ45 wiring between the input hub and mouse pointer module, including
  PMW3610 power, SPI, motion interrupt, and reset routing.
- Add ESD protection, alternate pedal jack footprints, updated connector
  layout, silkscreen artwork, and production output refinements.
- Document the next input hub pedal jack normal-contact grounding improvement.

### Documentation

- Add the public hardware viewer page with board images, embedded KiCanvas
  previews, fabrication downloads, a combined electronics BOM, assembly
  guidance, and the RJ45 pinout.
- Add follow-up notes for the deferred PMW3610 reset-retry driver stash and the
  stashed XIAO battery access footprint.

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
