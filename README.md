# Preamp V1

This is the Arduino code to run a HiFi passive preamplifier. It has the following features:

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
  * NeoPixel: Controlled via a single-wire control protocol.
  * IR sensor: Standard 38Khz module.

The IRremote library is from here:
https://github.com/z3t0/Arduino-IRremote

Adafruit NeoPixel library is from here:
https://github.com/adafruit/Adafruit_NeoPixel

## Photos

![Connection diagram](../master/images/PassivePreamp_bb.jpg?raw=true)

![Completed preamp 1](../master/images/IMG_0036.JPG?raw=true)

![Completed preamp 2](../master/images/IMG_0041.JPG?raw=true)
