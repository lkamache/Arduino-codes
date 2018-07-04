#include <Adafruit_CircuitPlayground.h>
#include <Wire.h>
#include <SPI.h>

void setup() {
  Serial.begin(9600);
  Serial.println("Circuit Playground test!");

  CircuitPlayground.begin();
  CircuitPlayground.setBrightness(255);

}

void goRed() {
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 255, 0, 0);
    delay(50);
    //CircuitPlayground.clearPixels();
  }
}

void goGreen() {
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 0, 255, 0);
    delay(50);
    //CircuitPlayground.clearPixels();
  }
}

void goBlue() {
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 0, 0, 255);
    delay(50);
    //CircuitPlayground.clearPixels();
  }
}

void goViolet() {
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 255, 0, 127);
    delay(50);
    //CircuitPlayground.clearPixels();
  }
}

void goYellow() {
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 255, 255, 0);
    delay(50);
    //CircuitPlayground.clearPixels();
  }
}

void goWhite() {
  for (int i = 0; i < 10; i++) {
    CircuitPlayground.setPixelColor(i, 255, 255, 255);
    delay(50);
    //CircuitPlayground.clearPixels();
  }
}

void loop() {
  Serial.println(CircuitPlayground.temperature());
  goRed();
  delay(10000);
  goGreen();
  delay(5000);
  goYellow();
  delay(2000);
  //  goWhite();
}
