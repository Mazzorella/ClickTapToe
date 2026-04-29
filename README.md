# ClickTapToe

ClickTapToe is a custom foot-operated mouse for people who need to reduce repetitive hand and arm movement. The goal is to move common mouse actions away from the hands by using foot pedals for clicking, a large foot-driven scroll wheel, and a separate right-foot pointing module.

This repository contains the firmware, hardware notes, and project documentation for ClickTapToe. The current firmware target is a Seeed Studio XIAO nRF52840 / XIAO BLE running as a USB HID mouse with ZMK.

## Current Status

The current firmware supports USB mouse clicks from the center pedal module:

- Left click on the first pedal input
- Right click on the second pedal input
- The third pedal jack is reserved and currently unassigned

Scroll wheel and PMW3610 pointing support are part of the hardware design but are not included in this initial firmware baseline.

## Hardware Overview

ClickTapToe is built from multiple modules:

- The main module contains the XIAO nRF52840 controller and a large foot-operated scroll wheel coupled to an EC11 encoder.
- The center pedal module has three TS jacks for simple foot pedals used as mouse buttons.
- The right-foot pointing module is shaped like a slipper and houses a PMW3610 pointing sensor.
- The center pedal module and pointing module connect over RJ45 with a short Ethernet cable.

See [HARDWARE.md](HARDWARE.md) for the current pin map and hardware notes.

## Repository Layout

- `.github/workflows/build.yml` runs the upstream ZMK user-config build workflow.
- `build.yaml` defines the firmware build matrix.
- `config/west.yml` tracks upstream ZMK and imports ZMK's west manifest.
- `zephyr/module.yml` declares this repository as the `zmk-keyboard-clicktaptoe` module and exposes the top-level `boards/` directory.
- `boards/shields/clicktaptoe/` contains the ClickTapToe shield definition, overlay, metadata, and default keymap.
- `Makefile` wraps the local `west build` command for Docker/devcontainer use.
- `hardware/main-module/` is reserved for the main controller and scroll wheel PCB.
- `hardware/center-module/` is reserved for the center pedal and RJ45 PCB.
- `docs/` is reserved for assembly, usage, and release documentation.

## Build Target

The active firmware target is:

```yaml
board: xiao_ble//zmk
shield: clicktaptoe
artifact-name: clicktaptoe_xiao_ble_zmk
```

## Flashing

1. Build the firmware with GitHub Actions or a local ZMK toolchain.
2. Put the XIAO BLE into bootloader mode so it mounts as a UF2 drive.
3. Copy the generated `clicktaptoe_xiao_ble_zmk.uf2` file onto the mounted drive.
4. After the board reboots, connect it to the host over USB.

## Hardware Test

Use a safe desktop target for click testing, such as an empty Finder window or browser page.

- Short physical `D0` to `GND`; the host should receive a left click.
- Short physical `D1` to `GND`; the host should receive a right click.
- Leave the third pedal jack disconnected until its GPIO pin is finalized.

If macOS does not immediately recognize the updated HID device, unplug and reconnect the XIAO BLE.

## Local Build Notes

This repository is both a ZMK config repository and a ZMK module. Local builds should make the repository visible to ZMK as the config path and as an extra module so the custom `clicktaptoe` shield can be discovered.

The preferred local workflow is to build from a ZMK Docker/devcontainer environment and use this repository's `Makefile`.

One common local layout is to keep this repository next to a ZMK firmware checkout:

```text
projects/
  zmk/
  ClickTapToe/
```

Open the ZMK firmware checkout in VS Code:

```sh
code /path/to/projects/zmk
```

Then use **Dev Containers: Reopen in Container**. If using a devcontainer, bind mount this repository into the container and run the build commands from the ClickTapToe repository directory.

On the first devcontainer launch, initialize the Zephyr workspace from the ZMK checkout:

```sh
west init -l app/
west update
```

Restart the devcontainer after `west update` completes.

After that, run the build commands from this repository:

```sh
make build
```

To build and copy the generated UF2 into this repository's ignored `firmware/` directory:

```sh
make uf2
```

The copied firmware path is:

```sh
firmware/clicktaptoe_xiao_ble_zmk.uf2
```

By default, the `Makefile` expects ZMK's app directory to be available at `../zmk/app`, which matches the sibling directory layout shown above. Override `ZMK_APP` if your checkout path differs:

```sh
make uf2 ZMK_APP=/path/to/zmk/app
```

The wrapped `west` command has this shape:

```sh
west build -d build/clicktaptoe -b "xiao_ble//zmk" -- \
  -DSHIELD=clicktaptoe \
  -DZMK_CONFIG="/path/to/this-repo/config" \
  -DZMK_EXTRA_MODULES="/path/to/this-repo"
```
