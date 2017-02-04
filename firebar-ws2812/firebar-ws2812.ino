/*
Copyright (c) 2017 Patrik Mayer - patrik.mayer@codm.de

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "FastLED.h"
#include <EEPROMex.h>

#define NUM_LEDS 30
CRGB leds[NUM_LEDS];
#define PIN 4

#define BTN_SPEED 3
#define BTN_BRIGHT 2

uint8_t speed[] = {2, 4, 8, 12, 15, 20, 25, 30, 40, 50, 70, 100};
uint8_t bright[] = {3 , 10, 25, 50, 70, 80, 90, 100};

uint8_t buttonStateSpeed;
uint8_t buttonStateBright;

uint8_t defaultSpeed = 5;
uint8_t defaultBright = 3;

uint8_t lastButtonStateSpeed = HIGH;
uint8_t lastButtonStateBright = HIGH;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 40;

#define eepromMemoryBase 0
int configAddress = 0;
struct deviceConfigStruct {
  uint8_t speed;
  uint8_t bright;
} deviceConfig = {
  NULL, 
  NULL 
};


void setup() {

  Serial.begin(9600);
  Serial.println("ready...");

  EEPROM.setMemPool(eepromMemoryBase, EEPROMSizeUno); 
  configAddress  = EEPROM.getAddress(sizeof(deviceConfigStruct)); // Size of config object
  loadConfig();  
  delay(10);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  pinMode(BTN_SPEED, INPUT);
  digitalWrite(BTN_SPEED, HIGH);

  pinMode(BTN_BRIGHT, INPUT);
  digitalWrite(BTN_BRIGHT, HIGH);

  FastLED.addLeds<WS2811, PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
}

void loop() {

  uint8_t readingSpeed = digitalRead(BTN_SPEED);
  uint8_t readingBright = digitalRead(BTN_BRIGHT);

  if (readingSpeed != lastButtonStateSpeed) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {

    if (readingSpeed != buttonStateSpeed) {
      buttonStateSpeed = readingSpeed;

      if (buttonStateSpeed == LOW) {
        deviceConfig.speed++;
        if (deviceConfig.speed >= sizeof(speed)) {
          deviceConfig.speed = 0;
        }

        Serial.print("new speed: ");
        Serial.println(speed[deviceConfig.speed]);

        saveConfig();
        blinkShort();

      }
    }


    if (readingBright != buttonStateBright) {
      buttonStateBright = readingBright;

      if (buttonStateBright == LOW) {
        deviceConfig.bright++;
        if (deviceConfig.bright >= sizeof(bright)) {
          deviceConfig.bright = 0;
        }

        Serial.print("new brightness: ");
        Serial.println(bright[deviceConfig.bright]);
        
        saveConfig();
        blinkShort();

      }
    }

  }

  lastButtonStateSpeed = readingSpeed;
  lastButtonStateBright = readingBright;

  Fire(55, 120, speed[deviceConfig.speed]);
}

void blinkShort() {
  digitalWrite(LED_BUILTIN, LOW);
  delay(120);
  digitalWrite(LED_BUILTIN, HIGH);
}

void showStrip() {
  FastLED.show();
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
  leds[Pixel].r = red;
  leds[Pixel].g = green;
  leds[Pixel].b = blue;
}

void setAll(byte red, byte green, byte blue) {
  for (int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}



void Fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[NUM_LEDS];
  int cooldown;

  // Step 1.  Cool down every cell a little
  for ( int i = 0; i < NUM_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);

    if (cooldown > heat[i]) {
      heat[i] = 0;
    } else {
      heat[i] = heat[i] - cooldown;
    }
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if ( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160, 255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for ( int j = 0; j < NUM_LEDS; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  showStrip();
  delay(SpeedDelay);

}


void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature / 255.0) * 191);

  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252

  //dimm to the desired brightness
  float mod = bright[deviceConfig.bright] / 100.0;

  // figure out which third of the spectrum we're in:
  if ( t192 > 0x80) {                    // hottest
    setPixel(Pixel, 255 * mod, 255 * mod, heatramp * mod);
  } else if ( t192 > 0x40 ) {            // middle
    setPixel(Pixel, 255 * mod, heatramp * mod, 0);
  } else {                               // coolest
    setPixel(Pixel, heatramp * mod, 0, 0);
  }
}

bool loadConfig() {
  EEPROM.readBlock(configAddress, deviceConfig);
}

void saveConfig() {
  EEPROM.writeBlock(configAddress, deviceConfig);
}

