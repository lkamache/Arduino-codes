#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SFE_MicroOLED.h>
#include <PubSubClient.h>

#define PIN_RESET 255
#define DC_JUMPER 0

const char* ssid = "IoT BT";
const char* password = "wWbNtAfxKkgk3yrNharYpxQK5arYPh8r";
const char* mqtt_server = "io.adafruit.com";
const char* mqtt_feed = "lkamache/feeds/senhadodia";
const char* mqtt_user = "lkamache";
const char* mqtt_pass = "1d9ebf2479f84a2892ecd522f2487613";
bool mqtt_use_auth = 1;
String senhadodia;

WiFiClient espClient;
PubSubClient client(espClient);

MicroOLED oled(PIN_RESET, DC_JUMPER);

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

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  senhadodia = "";
  Serial.print("Senha do dia: ");
  for (int i = 0; i < length; i++) {
    senhadodia += ((char)payload[i]);
  }
  Serial.println(senhadodia);
  escrevesenha();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    if (mqtt_use_auth == 1) {
      if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
        Serial.println("connected");
        client.subscribe(mqtt_feed);
      } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        delay(5000);
      }
    } else {
      if (client.connect(clientId.c_str())) {
        Serial.println("connected");
        client.subscribe(mqtt_feed);
      } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        delay(5000);
      }
    }
  }
}

void escrevesenha() {

  oled.clear(PAGE);
  oled.setFontType(0);
  oled.setCursor(0, 0);
  oled.println(" Bodytech");
  oled.println(" --------");
  oled.println();
  oled.println();
  oled.println(senhadodia);
  oled.display();
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  oled.begin();
  oled.clear(PAGE);
  oled.setFontType(0);
  oled.setCursor(0, 0);
  oled.println(" Bodytech");
  oled.println(" --------");
  oled.println();
  oled.println();
  oled.println(senhadodia);
  oled.display();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
