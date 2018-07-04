#include <Adafruit_CircuitPlayground.h>

void setup() {
  CircuitPlayground.begin();
  CircuitPlayground.setBrightness(255);
}

void vermelho() {
  for (int r = 0; r <= 9; r++) {
    CircuitPlayground.setPixelColor(r, 255, 0, 0);
  }
}

void azul() {
  for (int b = 0; b <= 9; b++) {
    CircuitPlayground.setPixelColor(b, 0, 0, 255);
  }
}

void loop() {
  if (CircuitPlayground.slideSwitch()) {
    for (int x = 0; x < 3; x++) {
      vermelho();
      delay(50);
      CircuitPlayground.clearPixels();
      delay(50);
    }
    delay(300);
    for (int x = 0; x < 3; x++) {
      azul();
      delay(50);
      CircuitPlayground.clearPixels();
      delay(50);
    }
    delay(300);
  }
  if (!CircuitPlayground.slideSwitch()) {
    for (int g = 0; g <= 9; g++) {
      CircuitPlayground.setPixelColor(g, 0, 255, 0);
    }
    delay(50);
    CircuitPlayground.clearPixels();
    delay(50);
  }
}
