#define BLYNK_PRINT Serial

#include <SPI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_NeoPixel.h>

char auth[] = "3170a3fa3512404ca130316a607cbb13";
char ssid[] = "Kamache.net";
char pass[] = "Kam@ch3e";
#define PIN D8

bool isFirstConnect = true;

BLYNK_CONNECTED() {
  if (isFirstConnect) {
    Blynk.syncAll();
  }
}

Adafruit_NeoPixel strip = Adafruit_NeoPixel(12, PIN, NEO_GRB + NEO_KHZ800);

uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

BLYNK_WRITE(V0)
{
  int shift = param.asInt();
  if (shift > 0) {
    for (int i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, Wheel(shift & 255));
      strip.show();
    }
  } else {
    for (int i = 0; i < strip.numPixels(); i++)
    {
      strip.setPixelColor(i, 0, 0, 0);
      strip.show();
    }
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(D4, OUTPUT);
  digitalWrite(D4, HIGH);
  Blynk.begin(auth, ssid, pass);

  strip.begin();
  strip.show();
}

void loop()
{
  Blynk.run();
}

