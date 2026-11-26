# Next Steps

These items are deferred for future firmware and hardware revisions.

## Software

- Continue PMW3610 motion detection and reset retry. The current firmware
  uses SPI reset. The input hub already routes a hardware reset line for this
  work. The local PMW3610 driver checkout has a stash named
  `clicktaptoe-pmw-reset-retry-work`.

## Hardware

- Validate the input hub pedal-jack jumper revision with the v0.4.0 UF2 and
  normally closed pedals. Check that both pedals click as expected and each
  jack stays idle with its pedal unplugged.
- Integrate the XIAO battery access footprint in a future input hub hardware
  update. The footprint is in the input hub's KiCad library.
- Export the enclosure and wheel STEP files when the mechanical revision is
  complete.
