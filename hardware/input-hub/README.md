# Input Hub Hardware

This folder contains source files and notes for the ClickTapToe input hub PCB.

The input hub is the central electronics board. It hosts the XIAO nRF52840,
connects to the computer over USB, receives the external pedal TS jacks,
connects to the mouse pointer board over RJ45, and interfaces with the EC11
scroll encoder.

## Planned Layout

- `kicad/` for KiCad project source files
- `production/` for generated Gerbers, drill files, BOMs, and placement files
