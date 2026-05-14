# Mouse Pointer Hardware

This folder contains source files and notes for the ClickTapToe mouse pointer
PCB.

The mouse pointer board contains the optical pointing sensor and connects to the
input hub over RJ45. The initial KiCad source for this board is derived from the
PMW3610 breakout by `siderakb/pmw3610-pcb`; RJ45-specific board changes are
tracked separately from the imported baseline.

## Planned Layout

- `kicad/` for KiCad project source files
- `production/` for generated Gerbers, drill files, BOMs, and placement files
