#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266httpUpdate.h>
#include "SimpleTimer.h"

const char* ssid = "********";
const char* password = "********";
const char* mqtt_server = "********";

const String chipId = String(ESP.getChipId());
const String TOPIC_ENDER3_RAW = "ender3/raw";

SimpleTimer timer;

WiFiClient espClient;
PubSubClient mqtt_client(espClient);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

   delay(10);
  // We start by connecting to a WiFi network
  Serial.print("M117 Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }


  Serial.print("M117 IP: ");
  Serial.println(WiFi.localIP());


  mqtt_client.setServer(mqtt_server, 1883);
  timer.setInterval(2000, handleMQTT);
  timer.setInterval(15500, updatePrintStatus);
}

void handleMQTT() {
  if (!mqtt_client.connected()) {
    reconnect();
  }
  mqtt_client.loop();
}

void updatePrintStatus() {
  Serial.println("M31");
  Serial.println("M27");
}

void reconnect() {
  int tries = 0;
  // Loop until we're reconnected
  while (!mqtt_client.connected() and tries < 1) {
    tries = tries + 1;
    Serial.println("M117 MQTT connecting...");
    // Attempt to connect
    if (mqtt_client.connect(chipId.c_str())) {
      Serial.println("M117 MQTT connected");
      mqtt_client.loop();
    } else {
      Serial.print("M117 failed, rc=");
      Serial.print(mqtt_client.state());
      delay(5000);
    }
  }
}

String raw;
void loop() {
  while(Serial.available()) {
    raw = Serial.readString();
    mqtt_client.publish(TOPIC_ENDER3_RAW.c_str(), raw.c_str());
  }
  timer.run();
}
