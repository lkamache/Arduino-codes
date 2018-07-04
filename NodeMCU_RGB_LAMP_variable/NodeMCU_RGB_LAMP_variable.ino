#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN D2
//#define LM35PIN A0

Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRB + NEO_KHZ800);

const char* ssid = "Kamache.net";
const char* password = "Kam@ch3e";
const char* mqtt_server = "10.0.0.254";

char colormode = 0;
String Jambalaya;

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
  strip.setBrightness(255);
  strip.begin();
  strip.show();
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void rainbow(uint8_t wait) {
  uint16_t i, j;
  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  colormode = (char)payload[0];
  Jambalaya = (char)payload;
//  Serial.print("Color mode = ");
//  Serial.println(colormode);
  Serial.print("Hex Code = ");
  Serial.println(Jambalaya);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("rgblamp", "rgblamp", 2, 1, "")) {
      Serial.println("connected");
      //client.publish("rgblamp", "0");
      client.subscribe("rgblamp");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (colormode == '0') { // desliga
    colorWipe(strip.Color(0, 0, 0), 300);
  }
  if (colormode == 'r') { // red
    colorWipe(strip.Color(255, 0, 0), 300);
  }
  if (colormode == 'g') { // green
    colorWipe(strip.Color(0, 255, 0), 300);
  }
  if (colormode == 'b') { // blue
    colorWipe(strip.Color(0, 0, 255), 300);
  }
  if (colormode == 'o') { // orange
    colorWipe(strip.Color(255, 140, 0), 300);
  }
  if (colormode == 'v') { // violet
    colorWipe(strip.Color(255, 0, 80), 300);
  }
  if (colormode == 'y') { // yellow
    colorWipe(strip.Color(255, 255, 0), 300);
  }
  if (colormode == 'c') { // cyan
    colorWipe(strip.Color(0, 255, 255), 300);
  }
  if (colormode == '1') { // rainbow
    rainbow(50);
  }
  if (colormode == 'w') { // white
    colorWipe(strip.Color(255, 255, 255), 300);
  }
}
