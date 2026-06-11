# ClickTapToe

ClickTapToe is a custom foot-operated mouse for people who need to reduce repetitive hand and arm movement. The goal is to move common mouse actions away from the hands by using foot pedals for clicking, a large foot-driven scroll wheel, and a separate mouse pointer module.

This repository contains the firmware, hardware notes, and project documentation for ClickTapToe. The current firmware target is a Seeed Studio XIAO nRF52840 / XIAO BLE running as a USB HID mouse with ZMK. Bluetooth is disabled for the ClickTapToe firmware build.

## Current Status

The current firmware supports USB mouse clicks from external pedals, USB vertical scrolling from the input hub scroll wheel, and pointer movement from the mouse pointer module:

- Left click on the first pedal input
- Right click on the second pedal input
- Vertical scroll from the AS5600 magnetic angle sensor connected to the input hub scroll wheel
- Pointer movement from the PMW3610 sensor in the mouse pointer module
- `D2` and `D3` remain spare/test inputs and are not exposed as production pedal jacks on the current input hub PCB

PMW3610 pointing support is included in the v0.3.0 firmware work.

## Hardware Overview

ClickTapToe is built from two custom electronics modules plus external pedals:

- The input hub contains the XIAO nRF52840 controller, USB connection, external pedal TS jacks, RJ45 pointer connection, and a large foot-operated scroll wheel coupled to an AS5600 magnetic angle sensor.
- The mouse pointer module is shaped like a slipper, houses a PMW3610 pointing sensor, and connects to the input hub over RJ45 with a short Ethernet cable.
- The pedals are premade external pedals connected to the input hub so higher-quality parts can be used and the project is easier to reproduce.

See [HARDWARE.md](HARDWARE.md) for the current pin map and hardware notes.

| Board | Source | KiCanvas preview |
| --- | --- | --- |
| Input hub | [hardware/input-hub/](hardware/input-hub/) | [Open preview](https://mazzorella.github.io/ClickTapToe/hardware/input-hub/kicanvas.html) |
| Mouse pointer | [hardware/mouse-pointer/](hardware/mouse-pointer/) | [Open preview](https://mazzorella.github.io/ClickTapToe/hardware/mouse-pointer/kicanvas.html) |

## Repository Layout

- `.github/workflows/build.yml` runs the upstream ZMK user-config build workflow.
- `build.yaml` defines the firmware build matrix.
- `config/west.yml` tracks upstream ZMK and imports ZMK's west manifest.
- `zephyr/module.yml` declares this repository as the `zmk-keyboard-clicktaptoe` module and exposes its board, devicetree binding, Kconfig, and driver files.
- `boards/shields/clicktaptoe/` contains the ClickTapToe shield definition, overlay, metadata, and default keymap.
- `Makefile` wraps the local `west build` command for Docker/devcontainer use.
- `hardware/` contains the input hub and mouse pointer KiCad source files.
- `docs/` is reserved for assembly, usage, and release documentation.

## Build Target

The active firmware target is:

```yaml
board: xiao_ble//zmk
shield: clicktaptoe
artifact-name: clicktaptoe_xiao_usb_zmk
```

## Flashing

1. Build the firmware with GitHub Actions or a local ZMK toolchain.
2. Put the XIAO BLE into bootloader mode so it mounts as a UF2 drive.
3. Copy the generated `clicktaptoe_xiao_usb_zmk.uf2` file onto the mounted drive.
4. After the board reboots, connect it to the host over USB.

## Hardware Test

Use a safe desktop target for click testing, such as an empty Finder window or browser page.

- Short physical `D0` to `GND`; the host should receive a left click.
- Short physical `D1` to `GND`; the host should receive a right click.
- Rotate the AS5600 scroll wheel; the host should receive vertical scroll events.
- Move the mouse pointer module over a surface; the host pointer should move smoothly in the same direction.
- Test the PMW3610 once through the RJ45 cable path; pointer response should match the direct breadboard wiring test.
- Leave the AS5600 magnet away from the sensor once as a negative test; the host should not scroll continuously.
- Verify `D2` and `D3` only as spare/test GPIOs if they are wired for bring-up.

If macOS does not immediately recognize the updated HID device, unplug and reconnect the XIAO BLE.
If an old Bluetooth name appears after flashing, remove the cached Bluetooth entry from the host. This firmware build disables ZMK BLE.
If the PMW3610 does not move the pointer after flashing, power-cycle the XIAO once. The input hub physically routes PMW3610 `nRESET` on XIAO `D6` / RJ45 pin 8, but the current firmware still relies on SPI/software reset and does not drive that reset line yet.

## Local Build Notes

This repository is both a ZMK config repository and a ZMK module. Local builds should make the repository visible to ZMK as the config path and as an extra module so the custom `clicktaptoe` shield can be discovered.

The preferred local workflow is to build from a ZMK Docker/devcontainer environment and use this repository's `Makefile`.

One common local layout is to keep this repository next to a ZMK firmware checkout:

```text
projects/
  zmk/
  zmk-pmw3610-driver/
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

Clone the PMW3610 driver module where the `Makefile` can find it. In the default devcontainer layout:

```sh
git clone https://github.com/badjeff/zmk-pmw3610-driver /workspaces/zmk-pmw3610-driver
```

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
firmware/clicktaptoe_xiao_usb_zmk.uf2
```

Build outputs are written under this repository's ignored `build/` directory,
not under ZMK's `app/` tree.

By default, the `Makefile` expects ZMK's app directory to be available at `../zmk/app`, which matches the sibling directory layout shown above. Override `ZMK_APP` if your checkout path differs:

```sh
make uf2 ZMK_APP=/path/to/zmk/app
```

The wrapped `west` command has this shape:

```sh
west build -d "/path/to/this-repo/build/clicktaptoe" -b "xiao_ble//zmk" -- \
  -DSHIELD=clicktaptoe \
  -DZMK_CONFIG="/path/to/this-repo/config" \
  -DZMK_EXTRA_MODULES="/path/to/this-repo;/path/to/zmk-pmw3610-driver"
```

By default, the `Makefile` expects the PMW3610 driver module at `../zmk-pmw3610-driver`. Override `PMW3610_MODULE` if your checkout path differs:

```sh
make uf2 PMW3610_MODULE=/path/to/zmk-pmw3610-driver
```
