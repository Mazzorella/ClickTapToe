# Mouse Pointer Hardware

This folder contains source files and notes for the ClickTapToe mouse pointer
PCB.

The mouse pointer board contains the optical pointing sensor and connects to the
input hub over RJ45.

The initial KiCad source for this board is derived from the PMW3610 breakout by
`siderakb/pmw3610-pcb`. The current ClickTapToe revision preserves the original
PMW3610 optical/lens/LDO section while adding the project-specific RJ45 cable
interface.

## RJ45 Pinout

The connector is an 8P8C modular jack used for ClickTapToe sensor wiring. It is
not Ethernet.

| RJ45 pin | Signal | Notes |
| --- | --- | --- |
| 1 | `VIN` / `3V3` | Sensor board input power |
| 2 | `GND` | Ground return |
| 3 | `SCLK` | PMW3610 SPI clock |
| 4 | `SDIO` | PMW3610 bidirectional SPI data |
| 5 | `nCS` | PMW3610 chip select |
| 6 | `GND` | Ground return paired with `SCLK` |
| 7 | `MOTION` | PMW3610 motion output after local series resistor |
| 8 | `nRESET` | Reset from the input hub through a 0 ohm link |

Twisted pair grouping:

| Pair | RJ45 pins | Signals |
| --- | --- | --- |
| Pair 1 | 1-2 | `VIN` / `3V3` + `GND` |
| Pair 2 | 3-6 | `SCLK` + `GND` |
| Pair 3 | 4-5 | `SDIO` + `nCS` |
| Pair 4 | 7-8 | `MOTION` + `nRESET` |

## Board Configuration

- Default logic configuration is 3.3 V: `JP1` is a 0 ohm link from `VIN` to
  `VDDIO` and is installed by default.
- Alternate 1.8 V logic is available by populating `JP2`, a 0 ohm link from
  `+1V8` to `VDDIO`. `JP2` is DNP by default.
- `JP3` is a 0 ohm reset link from RJ45 `nRESET` to PMW3610 `nRESET` and is
  installed by default.
- `R3` is a 100 ohm series resistor on `MOTION`, placed near the PMW3610 side
  before the signal leaves the board.
- `R2` is a 100k optional `nCS` pull-up to `VDDIO` and is DNP by default.
- `J1` remains as a debug/test header on the PMW3610-side signals.

The RJ45 connector is unshielded. ESD protection is planned as a separate layout
pass near the RJ45 connector.

## Planned Layout

- `kicad/` for KiCad project source files
- `production/` for generated Gerbers, drill files, BOMs, and placement files
