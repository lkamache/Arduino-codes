#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <WiFiManager.h>
#include <Ticker.h>
Ticker ticker;
int alarmStatus = 3;

const char* mqtt_server = "10.0.0.254";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void tick()
{
  int state = digitalRead(BUILTIN_LED);
  digitalWrite(BUILTIN_LED, !state);
}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
  ticker.attach(0.2, tick);
}

void setup() {
  Serial.begin(115200);

  pinMode(BUILTIN_LED, OUTPUT);
  ticker.attach(0.6, tick);

  WiFiManager wifiManager;
  //  wifiManager.resetSettings();
  wifiManager.setAPCallback(configModeCallback);

  if (!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    ESP.reset();
    delay(1000);
  }

  Serial.println("connected...yeey :)");
  ticker.detach();
  digitalWrite(BUILTIN_LED, HIGH);

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void callback(char* topic, byte* payload, unsigned int length) {
  //  Serial.print("Message arrived [");
  //  Serial.print(topic);
  //  Serial.print("] ");
  //  for (int i = 0; i < length; i++) {
  //    Serial.print((char)payload[i]);
  //  }
  //  Serial.println();

  if ((char)payload[0] == '0') {
    Serial.println("Home");
    alarmStatus = 0;
    digitalWrite(BUILTIN_LED, LOW);
    client.publish("alarmReturn", "0");
  }
  if ((char)payload[0] == '1') {
    Serial.println("Away");
    alarmStatus = 1;
    digitalWrite(BUILTIN_LED, LOW);
    client.publish("alarmReturn", "1");
  }
  if ((char)payload[0] == '2') {
    Serial.println("Night");
    alarmStatus = 2;
    digitalWrite(BUILTIN_LED, LOW);
    client.publish("alarmReturn", "2");
  }
  if ((char)payload[0] == '3') {
    Serial.println("Off");
    alarmStatus = 3;
    digitalWrite(BUILTIN_LED, HIGH);
    client.publish("alarmReturn", "3");
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      client.subscribe("alarmSystem");
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
}
