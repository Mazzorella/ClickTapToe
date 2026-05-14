# Mouse Pointer Hardware

This folder contains source files and notes for the ClickTapToe mouse pointer
PCB.

The mouse pointer board contains the optical pointing sensor and connects to the
input hub over RJ45.

The initial KiCad source for this board is derived from the PMW3610 breakout by
`siderakb/pmw3610-pcb`. The first hardware commit is only the imported and
renamed baseline. RJ45-specific schematic, layout, and routing changes will come
in later commits so the design diff stays reviewable.

Final RJ45 pinout, board validation status, and production notes are TBD.

## Planned Layout

- `kicad/` for KiCad project source files
- `production/` for generated Gerbers, drill files, BOMs, and placement files
