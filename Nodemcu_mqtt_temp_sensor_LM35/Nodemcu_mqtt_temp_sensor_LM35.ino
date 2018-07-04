#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Kamache.net";
const char* password = "Kam@ch3e";
const char* mqtt_server = "10.0.0.254";

const int LM35 = A0;
float temperatura;

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
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    digitalWrite(D4, LOW);
    // Attempt to connect
    if (client.connect("ESP8266Client", "LM35", 2, true, "OFFLINE")) {
      Serial.println("connected");
      digitalWrite(D4, LOW);
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
  pinMode(D4, OUTPUT);
  digitalWrite(D4, HIGH);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.connect("ESP8266Client");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis > interval) {

    //float millivolts = (analogRead(LM35) / 1024.0) * 3300;
    //float t = millivolts/10;
    float t = (float(analogRead(LM35)) * 3.3 / (1023)) / 0.01;
    static char temperatureTemp[7];
    dtostrf(t, 6, 2, temperatureTemp);

    if (isnan(t)) {
      return;
    }
    previousMillis = currentMillis;
    client.publish("LM35", temperatureTemp);

    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" *C ");

    digitalWrite(D4, LOW);
    delay(30);
    digitalWrite(D4, HIGH);
  }
}
