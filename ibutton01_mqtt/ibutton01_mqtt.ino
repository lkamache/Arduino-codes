#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid     = "Kamache.net";
const char* password = "Kam@ch3e";
const char* mqtt_server = "10.0.0.254";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println();
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

  client.setServer(mqtt_server, 1883);
  if (!client.connected()) {
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    client.connect(clientId.c_str());
    client.publish("ibutton01", "toggle");
  }
  Serial.println("Going to sleep");
  ESP.deepSleep(0);
}

void loop() {

}

