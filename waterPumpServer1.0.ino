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

// ---------- Auto-watering config ----------
struct AutoConfig {
  bool enabled     = false;
  int  threshold   = 35;     // water if moisture < threshold
  int  durationMs  = 2000;   // pump run ms (you can override PUMP_MS with this)
  int  cooldownMin = 1440;   // 24h fixed
  int  hyst        = 10;     // latch resets when moisture >= threshold + hyst
  int  maxPerDay   = 3;      // optional (0 = unlimited)
};

AutoConfig cfg;

// ---------- Auto-watering state ----------
bool lowLatched = false;                 // stops re-trigger while still "low"
unsigned long lastWateredMs = 0;         // millis timestamp (note: resets on reboot)
int watersToday = 0;                     // optional
unsigned long dayWindowStartMs = 0;      // optional day counter start

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

void applyAutoConfig(const String& payload) {
  int start = 0;

  while (start < (int)payload.length()) {
    int end = payload.indexOf(';', start);
    if (end == -1) end = payload.length();

    String pair = payload.substring(start, end);
    int eq = pair.indexOf('=');

    if (eq > 0) {
      String key = pair.substring(0, eq);
      String val = pair.substring(eq + 1);
      key.trim(); val.trim();

      if (key == "enabled")        cfg.enabled = (val.toInt() == 1);
      else if (key == "threshold")   cfg.threshold = constrain(val.toInt(), 0, 100);
      else if (key == "durationMs")  cfg.durationMs = constrain(val.toInt(), 500, 30000);
      else if (key == "cooldownMin") cfg.cooldownMin = constrain(val.toInt(), 1, 7 * 24 * 60);
      else if (key == "hyst")        cfg.hyst = constrain(val.toInt(), 0, 50);
      else if (key == "maxPerDay")   cfg.maxPerDay = constrain(val.toInt(), 0, 20);
    }

    start = end + 1;
  }

  Serial.printf("AUTO CFG -> en=%d thr=%d dur=%d cdMin=%d hyst=%d max=%d\n",
                cfg.enabled, cfg.threshold, cfg.durationMs, cfg.cooldownMin, cfg.hyst, cfg.maxPerDay);


}

void startPumpAuto() {
  if (pumpRunning) return;

  pumpRunning = true;
  pumpStartMs = millis();
  setPump(true);
  publishStatus("RUNNING");
  Serial.println("Pump ON (auto)");

  // record watering time for cooldown
  lastWateredMs = millis();

  // optional daily limit window
  if (dayWindowStartMs == 0 || (millis() - dayWindowStartMs) > 24UL * 60UL * 60UL * 1000UL) {
    dayWindowStartMs = millis();
    watersToday = 0;
  }
  watersToday++;

  // latch immediately so it won't spam if moisture stays low
  lowLatched = true;

  // optional: publish last watered (retained)
  String lw = String(lastWateredMs);
}

void maybeAutoWater(int moisturePct) {
  if (!cfg.enabled) return;
  if (pumpRunning) return;

  // Reset latch once moisture recovers above threshold + hyst
  if (moisturePct >= (cfg.threshold + cfg.hyst)) {
    lowLatched = false;
    return;
  }

  // Only trigger if below threshold
  if (moisturePct >= cfg.threshold) return;

  // Don’t re-trigger while still low
  if (lowLatched) return;

  // Cooldown
  unsigned long cooldownMs = (unsigned long)cfg.cooldownMin * 60UL * 1000UL;
  if (lastWateredMs != 0 && (millis() - lastWateredMs) < cooldownMs) return;

  // Max per day (optional)
  if (cfg.maxPerDay > 0) {
    if (dayWindowStartMs == 0 || (millis() - dayWindowStartMs) > 24UL * 60UL * 60UL * 1000UL) {
      dayWindowStartMs = millis();
      watersToday = 0;
    }
    if (watersToday >= cfg.maxPerDay) return;
  }

  // All good -> water
  startPumpAuto();
}


// ---------- MQTT callback ----------
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  message.reserve(length + 1);
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  message.trim();

  Serial.print("Message received [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  String t = String(topic);

  // ---- Auto config topic ----
  if (t == TOPIC_AUTO_CONFIG) {
    applyAutoConfig(message);
    return;
  }

  // ---- Manual pump command topic ----
  if (t == MQTT_TOPIC_PUMP) {
    String cmd = message;
    cmd.toLowerCase();

    if (cmd == "on") {
      if (!pumpRunning) {
        pumpRunning = true;
        pumpStartMs = millis();
        setPump(true);
        publishStatus("RUNNING");
        Serial.println("Pump ON");
      } else {
        publishStatus("RUNNING");
      }
    } else if (cmd == "off") {
      pumpRunning = false;
      setPump(false);
      publishStatus("IDLE");
      Serial.println("Pump OFF (manual)");
    }
  }
}
//----MQTT callback End-----

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

      client.subscribe(TOPIC_AUTO_CONFIG, 1);
      Serial.print("Subscribed to: ");
      Serial.println(TOPIC_AUTO_CONFIG);

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
  if(pendingIdle && (millis() - doneMillisecs >= 2000)){
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
    maybeAutoWater(constrainVal);
    lastMoistureReading = millis();

  }
}
