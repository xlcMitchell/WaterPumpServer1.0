#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "config.h" // WiFi + MQTT credentials + pump pin + topic

// MQTT client
WiFiClientSecure espClient;     // Secure client for HiveMQ TLS
PubSubClient client(espClient);

// -------- Pump pin --------
const int pumpPin = 4;

// -------- Callback function --------
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Message received on topic ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(message);

  if (String(topic) == MQTT_TOPIC_PUMP) {
    if (message == "on") {
      // Turn pump on
      digitalWrite(pumpPin, HIGH);
      Serial.println("Pump ON");
      delay(2000);               // Pump runs for 2 seconds
      digitalWrite(pumpPin, LOW);
      Serial.println("Pump OFF");

      // Publish status
      client.publish(MQTT_TOPIC_PUMP, "DONE");
    }
  }
}

// -------- Connect / reconnect to MQTT --------
void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");
    if (client.connect("ESP8266Pump", MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("connected!");
      client.subscribe(MQTT_TOPIC_PUMP);
      Serial.print("Subscribed to: ");
      Serial.println(MQTT_TOPIC_PUMP);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retry in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW);

  // ---- WiFi ----
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());

  // ---- MQTT ----
  espClient.setInsecure(); // skip TLS certificate verification
  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();  // handle incoming messages
}