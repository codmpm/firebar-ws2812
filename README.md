Firebar WS2812
=
A quick Arduino hack for a buddy to get a WS2812 RGB fire animation for room ilumination. 
The animation is controlled with only two buttons. One for the speed of the flames and the other for the brightness. Nothing fancy.

Your choosen speed and brightness is stored into EEPROM. So the sketch will start with the last values after power loss.

Prerequisites
==
Please read the [Adafruit Neopixel Uberguide](https://learn.adafruit.com/adafruit-neopixel-uberguide/overview) before attaching Neopixels.

The data pin for the Neopixel-Strip is `D4`. Buttons are attached to pin `D3` (speed) and pin `D2` (brightness). Both buttons use the internal pull-up resistor so wire them to ground.

The ammount of pixels is set in `NUM_LEDS` inside the sketch. Be aware to supply enough power to drive your desired pixel count.

Credits
==

* Fire-Animation and sketch base: https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/#fire
* FastLED library: http://fastled.io/
* EEPROMex: https://github.com/thijse/Arduino-EEPROMEx

