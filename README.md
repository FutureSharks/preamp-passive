# Preamp - Passive

This is the Arduino code and PCB layouts for a HiFi passive preamplifier. It has the following features:

  * Volume control and input selection via encoder and Apple remote.
  * Uses a 256 step R2R relayed based attenuator.
  * Completely passive.
  * Only 5V power required.
  * NeoPixel shows volume level with colour.
  * Sleep mode with low power consumption: 0.9W.

Full details can be found here:
http://www.diyaudio.com/forums/analog-line-level/275446-passive-preamp-arduino-based-remote-control-relay-r2r-input-selection.html

## Hardware

  * MCP23S17: Controlled via SPI. This is the R2R relayed attenuator and controls the volume.
  * MCP23S08: Controlled via SPI. For the input selector and mute.
  * Adafruit NeoPixel: Controlled via a single-wire control protocol.
  * IR sensor: Standard 38Khz type.
  * Bourns EM14 Rotary Optical Encoder with switch

PCB and schematics are provided in EAGLE PCB format.

The IRremote library is from here:
https://github.com/z3t0/Arduino-IRremote

Adafruit NeoPixel library is from here:
https://github.com/adafruit/Adafruit_NeoPixel

## Photos

![Connection diagram](../master/images/PassivePreamp_bb.jpg?raw=true)

![Attenuator schematic](../master/images/attenuator-schematic.jpg?raw=true)

![Input selector schematic](../master/images/input-selector-schematic.jpg?raw=true)

![Completed preamp GIF](../master/images/hPBWdx8.gif?raw=true)

![Completed preamp 1](../master/images/IMG_0036.JPG?raw=true)

![Completed preamp 2](../master/images/IMG_0037.JPG?raw=true)

![Completed preamp 3](../master/images/IMG_0038.JPG?raw=true)

![Completed preamp 4](../master/images/IMG_0041.JPG?raw=true)

![Completed preamp 4](../master/images/IMG_0042.JPG?raw=true)
