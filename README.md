# ClickTapToe

ClickTapToe is a custom foot-operated mouse for people who need to reduce repetitive hand and arm movement. The goal is to move common mouse actions away from the hands by using foot pedals for clicking, a large foot-driven scroll wheel, and a separate mouse pointer module.

This repository contains the firmware, hardware notes, and project documentation for ClickTapToe. The current firmware target is a Seeed Studio XIAO nRF52840 / XIAO BLE running as a USB HID mouse with ZMK. Bluetooth is disabled for the ClickTapToe firmware build.

The [hub assembly photo guide](https://mazzorella.github.io/ClickTapToe/docs/hub-assembly.html)
shows how to assemble the scroll wheel and hub, wire the sensor, and connect the
finished slipper pointer. The enclosure and wheel STEP files are being revised.
The [scroll wheel maintenance guide](https://mazzorella.github.io/ClickTapToe/docs/maintenance.html)
shows how to remove, clean, adjust, and reinstall the wheel.

## Current Status

The [v0.4.0 release](https://github.com/Mazzorella/ClickTapToe/tree/v0.4.0)
includes both custom circuit boards and the USB firmware tested with them.
Get the [matching manufacturing files](https://mazzorella.github.io/ClickTapToe/docs/hardware.html#files)
before ordering boards. The newer input hub jumper revision on `main` is
awaiting hardware validation.

The current firmware supports USB mouse clicks from external pedals, USB vertical scrolling from the input hub scroll wheel, and pointer movement from the mouse pointer module:

- Left click on the first pedal input
- Right click on the second pedal input
- Vertical scroll from the AS5600 magnetic angle sensor connected to the input hub scroll wheel
- Pointer movement from the PMW3610 sensor in the mouse pointer module
- `D2` and `D3` remain spare/test inputs and are not exposed as production pedal jacks on the current input hub PCB

PMW3610 pointing support is active for the current firmware target.

## Hardware Overview

ClickTapToe is built from two custom electronics modules plus external pedals:

- The input hub contains the XIAO nRF52840 controller, USB connection, external pedal TS jacks, RJ45 pointer connection, and a large foot-operated scroll wheel coupled to an AS5600 magnetic angle sensor.
- The mouse pointer module is shaped like a slipper, houses a PMW3610 pointing sensor, and connects to the input hub with a short straight-through RJ45 cable. The port carries sensor signals and power between the two units.
- The pedals are premade external pedals connected to the input hub so higher-quality parts can be used and the project is easier to reproduce.

See [HARDWARE.md](HARDWARE.md) for the current pin map and hardware notes.
Visit the [ClickTapToe project site](https://mazzorella.github.io/ClickTapToe/)
for an overview, or open the
[hardware page](https://mazzorella.github.io/ClickTapToe/docs/hardware.html)
for interactive board previews, source files, and manufacturing downloads.

| Board | Source | KiCanvas preview |
| --- | --- | --- |
| Input hub | [v0.4.0 KiCad files](https://github.com/Mazzorella/ClickTapToe/tree/v0.4.0/hardware/input-hub/kicad) | [Open preview](https://mazzorella.github.io/ClickTapToe/hardware/input-hub/kicanvas.html) |
| Mouse pointer | [v0.4.0 KiCad files](https://github.com/Mazzorella/ClickTapToe/tree/v0.4.0/hardware/mouse-pointer/kicad) | [Open preview](https://mazzorella.github.io/ClickTapToe/hardware/mouse-pointer/kicanvas.html) |

## Repository Layout

- `.github/workflows/build.yml` runs the upstream ZMK user-config build workflow.
- `build.yaml` defines the firmware build matrix.
- `config/west.yml` tracks upstream ZMK and imports ZMK's west manifest.
- `zephyr/module.yml` declares this repository as the `zmk-keyboard-clicktaptoe` module and exposes its board, devicetree binding, Kconfig, and driver files.
- `boards/shields/clicktaptoe/` contains the ClickTapToe shield definition, overlay, metadata, and default keymap.
- `Makefile` wraps the local `west build` command for Docker/devcontainer use.
- `hardware/` contains the input hub and mouse pointer KiCad source files.
- `docs/` contains the hub assembly guide, photos, and project documentation.

## Build Target

The active firmware target is:

```yaml
board: xiao_ble//zmk
shield: clicktaptoe
artifact-name: clicktaptoe_xiao_usb_zmk
```

## Flashing

Use the [tested v0.4.0 UF2](https://raw.githubusercontent.com/Mazzorella/ClickTapToe/v0.4.0/firmware/clicktaptoe_xiao_usb_zmk.uf2)
to flash the release firmware. To build your own copy, use GitHub Actions or a
local ZMK toolchain.

1. Put the XIAO BLE into bootloader mode so it mounts as a UF2 drive.
2. Copy `clicktaptoe_xiao_usb_zmk.uf2` onto the mounted drive.
3. After the board reboots, connect it to the host over USB.

## Hardware Test

Use a safe desktop target for click testing, such as an empty Finder window or browser page.

- Plug in normally closed pedals. Press and release the left and right pedals and check that each produces the matching click.
- For a bare board GPIO test, hold `D0` or `D1` at `GND` to simulate an idle pedal. Open that connection to simulate a press, then reconnect it to simulate a release.
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

Clone the PMW3610 driver module where the `Makefile` can find it. Check out the
revision pinned in [config/west.yml](config/west.yml) so the driver matches the
one used by GitHub Actions:

```sh
git clone https://github.com/badjeff/zmk-pmw3610-driver /workspaces/zmk-pmw3610-driver
git -C /workspaces/zmk-pmw3610-driver checkout --detach 44b4a76b74d293a93cec4ccb7e04cb8d29c10f93
```

After that, run the build commands from this repository:

```sh
make build
```

To build and copy the generated UF2 into this repository's `firmware/` directory:

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
