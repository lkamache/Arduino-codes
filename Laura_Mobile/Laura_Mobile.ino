#include <Adafruit_CircuitPlayground.h>
#include <Wire.h>
#include <SPI.h>

int r;
int g;
int b;
int p;
bool slideSwitch;

uint8_t pixeln = 0;

void setup() {
  CircuitPlayground.begin();
  CircuitPlayground.setBrightness(128);
}

void loop() {
  slideSwitch = CircuitPlayground.slideSwitch();

  if (slideSwitch) {
    CircuitPlayground.clearPixels();
    r = random(1, 255);
    g = random(1, 255);
    b = random(1, 255);
    p = random(0, 9);
    CircuitPlayground.setPixelColor(p, r, g, b);
    delay(2000);
  } else {
    for (p = 0; p < 10; p++) {
      CircuitPlayground.setPixelColor(p, 255, 255, 255);
    }
  }
}
