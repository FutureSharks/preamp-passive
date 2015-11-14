# Preamp - Passive

This is the Arduino code, PCB layouts and 3D drawings for a HiFi passive preamplifier. It has the following features:

  * Volume control and input selection via encoder and Apple remote.
  * Uses a 256 step R2R relayed based attenuator.
  * Completely passive.
  * Only 5V power required.
  * [Adafruit NeoPixel](http://www.adafruit.com/category/168) shows volume level with colour.
  * Sleep mode with low power consumption: 0.9W.

Full build details can be found here:
http://www.diyaudio.com/forums/analog-line-level/275446-passive-preamp-arduino-based-remote-control-relay-r2r-input-selection.html

## Hardware

  * [MCP23S17](http://www.microchip.com/wwwproducts/Devices.aspx?product=MCP23S17): Controlled via SPI. This is the R2R relayed attenuator and controls the volume.
  * [MCP23S08](http://www.microchip.com/wwwproducts/Devices.aspx?product=MCP23S08): Controlled via SPI. For the input selector and mute.
  * [Adafruit NeoPixel](http://www.adafruit.com/category/168): Controlled via a single-wire control protocol.
  * IR sensor: Standard 38Khz type.
  * [Bourns EM14](https://www.bourns.com/pdfs/em14.pdf) Rotary Optical Encoder with switch.
  * [Modushop Galaxy Maggiorato GX187](http://www.modu.it/galaxyeng.html) with custom machined front and rear.

PCB and schematics are provided in EAGLE PCB format.

Front and rear panel 3D drawings are in AutoCAD format.

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
