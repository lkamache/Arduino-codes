#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN D4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Kamache.net";
const char* password = "Kam@ch3e";
const char* mqtt_server = "10.0.0.254";

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
    digitalWrite(16, LOW);
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  digitalWrite(16, HIGH);
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    digitalWrite(16, LOW);
    // Attempt to connect
    if (client.connect("ESP8266Client_Laura", "home/laura/temperature", 2, 1, "0")) {
      Serial.println("connected");
      digitalWrite(16, HIGH);
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
  pinMode(16, OUTPUT);
  digitalWrite(16, HIGH);
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
    client.publish("home/laura/temperature", temperatureTemp);
    client.publish("home/laura/humidity", humidityTemp);
    client.publish("home/laura/heatindex", heatindexTemp);

    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print("Heat index: ");
    Serial.print(i);
    Serial.println(" *C ");

    digitalWrite(16, LOW);
    delay(30);
    digitalWrite(16, HIGH);
  }
}
