# Next Steps

These items are intentionally deferred until after the PCB hardware work is
merged back to `main`.

## Software

- Continue the PMW3610 motion-detection and reset-retry work from the driver
  repository stash:

  ```sh
  cd /Users/michael/Documents/School/EMPOWER/zmk-pmw3610-driver
  git stash show -p stash@{0}
  ```

- The stash is currently:

  ```text
  stash@{0}: On clicktaptoe-hotplug-reset: clicktaptoe-pmw-reset-retry-work
  ```

- Re-apply it on a new branch from the updated `main` integration path when
  that work resumes:

  ```sh
  git stash apply stash@{0}
  ```

## Hardware

- Integrate the XIAO battery access footprint in a future input hub hardware
  update.
- The footprint is currently preserved in this repository's local stash:

  ```text
  stash@{0}: On pcb-hardware: XIAO battery access footprint
  ```

- It contains:

  ```text
  hardware/input-hub/kicad/input_hub.pretty/XIAO_BAT_SOLDER_ACCESS.kicad_mod
  ```
