#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "config.h" // WiFi + MQTT credentials + topics

//auto watering branch testing

// MQTT client (TLS)
WiFiClientSecure espClient;
PubSubClient client(espClient);

// -------- Pump pin and moisture sensor --------
const int pumpPin = 4;  // GPIO4 = D2 on many boards (change if needed)
const int sensorPin = A0;
// Relay logic: set true if your relay turns ON when GPIO is LOW (common)
const bool RELAY_ACTIVE_LOW = false;

// Pump timing (ms)
const unsigned long PUMP_MS = 2000;

//moisture reading timing
const unsigned long READING_DURATION = 360000;

// State
bool pumpRunning = false;
bool pendingIdle = false;
unsigned long doneMillisecs = 0;
unsigned long pumpStartMs = 0;
unsigned long lastMoistureReading = 0; //update after new reading
// ---------- Helpers ----------
void setPump(bool on) {
  if (RELAY_ACTIVE_LOW) {
    digitalWrite(pumpPin, on ? LOW : HIGH);
  } else {
    digitalWrite(pumpPin, on ? HIGH : LOW);
  }
}

void publishStatus(const char* msg) {
  client.publish(TOPIC_STATUS, msg, true); // retained
  Serial.print("STATUS -> ");
  Serial.println(msg);
}

void publishMoisture(const char* moisture){
  client.publish(TOPIC_MOISTURE, moisture, true); 
  Serial.print("MOISTURE = ");
  Serial.println(moisture);
}

void publishOnline(const char* msg) {
  client.publish(TOPIC_ONLINE, msg, true); // retained
  Serial.print("ONLINE -> ");
  Serial.println(msg);
}

// ---------- MQTT callback ----------
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  message.trim();
  message.toLowerCase();

  Serial.print("Message received [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  // Command topic
  if (String(topic) == MQTT_TOPIC_PUMP) {
    if (message == "on") {
      if (!pumpRunning) {
        pumpRunning = true;
        pumpStartMs = millis();
        setPump(true);
        publishStatus("RUNNING");
        Serial.println("Pump ON");
      } else {
        // Already running; optional: publish again
        publishStatus("RUNNING");
      }
    }
    else if (message == "off") {
      // Optional manual off command support
      pumpRunning = false;
      setPump(false);
      publishStatus("IDLE");
      Serial.println("Pump OFF (manual)");
    }
  }
}

// ---------- Connect / reconnect to MQTT ----------
void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT... ");

    // Unique client ID so HiveMQ doesn't kick you off for duplicate IDs
    String clientId = "ESP8266-" + String(ESP.getChipId()) + "-" + String(millis());

    // Last Will: if device disconnects unexpectedly, broker publishes OFFLINE (retained)
    bool ok = client.connect(
      clientId.c_str(),
      MQTT_USERNAME,
      MQTT_PASSWORD,
      TOPIC_ONLINE, // will topic
      1,            // QoS
      true,         // retained
      "OFFLINE"     // will payload
    );

    if (ok) {
      Serial.println("connected!");

      client.subscribe(MQTT_TOPIC_PUMP, 1);
      Serial.print("Subscribed to: ");
      Serial.println(MQTT_TOPIC_PUMP);

      publishOnline("ONLINE");
      publishStatus("IDLE");
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
  delay(50);

  pinMode(pumpPin, OUTPUT);
  setPump(false);

  // ---- WiFi ----
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("WiFi connected. IP: ");
  Serial.println(WiFi.localIP());

  // ---- MQTT ----
  espClient.setInsecure(); // skip TLS cert verification (OK for dev/testing)
  client.setServer(MQTT_HOST, MQTT_PORT);
  client.setCallback(callback);

  // Optional but helpful
  client.setKeepAlive(30);
  client.setSocketTimeout(10);
}

void loop() {

  //----WIFI RECONNECT LOGIC START----
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("WIFI Lost, reconnecting...");
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
    unsigned long start = millis();
    while(WiFi.status() != WL_CONNECTED && start < 10000){
      delay(200);
      Serial.print(".");
    }
    Serial.println("");
    return; //makes sure wifi reconnects before proceeding
  }
  //---WIFI RECONNECT LOGIC END---

  //Reconnect to mqtt broker if disconnected
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Non-blocking pump timer
  if (pumpRunning && (millis() - pumpStartMs >= PUMP_MS)) {
    pumpRunning = false;
    setPump(false);
    publishStatus("DONE");
    Serial.println("Pump OFF (timer done)");
    pendingIdle = true;
    doneMillisecs = millis();
  }

  //Non-blocking setting idle 
  if(pendingIdle && doneMillisecs >= 2000){
    publishStatus("IDLE");
    pendingIdle = false;

  }
  

  if(millis() - lastMoistureReading >= READING_DURATION){
    int raw = analogRead(sensorPin);
    int mappedVal = map(raw,1023,298,0,100); //1023 = dry (will convert to 0%) 298 = just watered (100%) moisture lvl
    int constrainVal = constrain(mappedVal,0,100); //makes sure value is within correct range..

    //better way to create the string for memory on the esp8266
    char payload[8];
    itoa(constrainVal, payload, 10);
    publishMoisture(payload);
    lastMoistureReading = millis();

  }
}
