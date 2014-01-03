# clockblock
v1.0

Available for purchase at [clockblocknola.com](http://www.clockblocknola.com).

## firmware
Firmware is written in C++ and compiled in GCC with AVR modifications. If you're using a Mac, install [CrossPack for AVR](http://www.obdev.at/products/crosspack/index.html) for all the necessary tools. Otherwise, do some Googling.

The clockblock uses an ATTiny167 with 16KB of flash and 512B of RAM. At compilation, the application takes about 6KB of program and 30B of RAM. Currently firmware must be flashed onto the board with an ISP, but hardware supports USB bootloading*. An Arduino-compatible bootloader is in the works.

*v1.0 hardware has a trace that must be cut and replaced with a capacitor for this to work. See [issue #6](https://github.com/wileycousins/clockblock/issues/6).

## hardware
The PCBs are designed in [KiCad](https://launchpad.net/kicad). There are gerber files in the repo, but your best bet is to generate them yourself from KiCad. Components are from [these libraries](https://github.com/mcous/kicad-lib), but you shouldn't have to install them to view the schematic and board files if you don't want to.

The boards are almost entirely surface mount, so solder-paste and a reflow tool of some sort (hot-air gun, toaster, hot-plate) are recommended. If anyone manages this with just an iron, I'd love to see pictures, because that's awesome. Shameless self promotion: I know about this pretty good [toaster oven control board for solder reflow](https://github.com/mcous/reflow).
