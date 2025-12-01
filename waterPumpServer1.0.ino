#include <ESP8266WiFi.h>
#include "config.h" //file that stores wifi credentials
//THIS IS THE LATEST SERVER AS OF 2/12/2025
//DOUBLE CHECK PUMP INPUT 

// GPIO pins
const int pumpPin = 4;   // Pump


WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(pumpPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(pumpPin, LOW);
  digitalWrite(ledPin, LOW);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String request = client.readStringUntil('\r'); // read HTTP request line
    client.flush();

    // If pump request received, turn it on, wait 2 seconds, then turn it off
    if (request.indexOf("/pump/on") != -1) {
      digitalWrite(pumpPin, HIGH);
      delay(2000);               // Pump runs for 2 seconds
      digitalWrite(pumpPin, LOW);
    
    }

    // Respond with pump status (0 = off, 1 = just ran)
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();
    client.println("{\"pump\":1}");
    delay(10);
    client.stop();
  }
}
