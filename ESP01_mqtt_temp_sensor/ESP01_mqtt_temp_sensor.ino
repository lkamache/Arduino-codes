#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "IoT BT";
const char* password = "wWbNtAfxKkgk3yrNharYpxQK5arYPh8r";
const char* mqtt_server = "10.3.0.226";

long previousMillis = 0;
long interval = 10000;

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(1, LOW);
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  digitalWrite(1, HIGH);
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    digitalWrite(1, LOW);
    // Attempt to connect
    if (client.connect("ESP01Client_BT", "esp01/temperature", 2, 1, "0")) {
      Serial.println("connected");
      digitalWrite(1, HIGH);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(1, OUTPUT);
  digitalWrite(1, HIGH);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  dht.begin();
  //client.connect("ESP8266Client", "home/bedroom/temperature", 2, 1, "0");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis > interval) {
    float h = dht.readHumidity();
    static char humidityTemp[7];
    dtostrf(h, 6, 2, humidityTemp);

    float t = dht.readTemperature();
    static char temperatureTemp[7];
    dtostrf(t, 6, 2, temperatureTemp);

    float i = dht.computeHeatIndex(t, h, false);
    static char heatindexTemp[7];
    dtostrf(i, 6, 2, heatindexTemp);

    if (isnan(h) || isnan(t) || isnan(i)) {
      return;
    }
    previousMillis = currentMillis;
    client.publish("esp01/temperature", temperatureTemp);
    client.publish("esp01/humidity", humidityTemp);
    client.publish("esp01/heatindex", heatindexTemp);

    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print("Heat index: ");
    Serial.print(i);
    Serial.println(" *C ");

    digitalWrite(1, LOW);
    delay(30);
    digitalWrite(1, HIGH);
  }
}
