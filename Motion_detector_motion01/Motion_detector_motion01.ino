#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Kamache.net";
const char* password = "Kam@ch3e";
const char* mqtt_server = "10.0.0.254";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(16, OUTPUT);
  pinMode(10, INPUT);
  digitalWrite(16, HIGH);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
}

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

  digitalWrite(16, HIGH);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//void callback(char* topic, byte* payload, unsigned int length) {
//  Serial.print("Message arrived [");
//  Serial.print(topic);
//  Serial.print("] ");
//  for (int i = 0; i < length; i++) {
//    Serial.print((char)payload[i]);
//  }
//  Serial.println();
//
//  // Switch on the LED if an 1 was received as first character
//  if ((char)payload[0] == '1') {
//    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
//    // but actually the LED is on; this is because
//    // it is acive low on the ESP-01)
//  } else {
//    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
//  }
//
//}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client_motion01", "motion01/status", 2, 1, "OFFLINE")) {
      digitalWrite(16, HIGH);
      Serial.println("connected");
      client.publish("motion01/status", "ONLINE");
      // client.subscribe("inTopic");
    } else {
      digitalWrite(16, LOW);
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

  if (WiFi.status() != WL_CONNECTED) {
    setup_wifi();
  }

  client.loop();

  if (digitalRead(10) == HIGH) {
    Serial.println("Movimento detectado");
    digitalWrite(16, LOW);
    client.publish("motion01" , "1");
    delay(10000);
    digitalWrite(16, HIGH);
  }
}
