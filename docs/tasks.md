# Smart Plant Watering System – Agile Task List

This task list follows an **Agile / iterative approach**. Work is organised into **iterations (sprints)**, each producing a testable outcome. Tasks are only marked complete when verified.

---

## Sprint 0 – Discovery & Foundations
**Goal:** Define scope, tools, and success criteria

- [x] Define project goal (remote plant watering)
- [x] Choose ESP8266 platform
- [x] Choose MQTT for communication
- [x] Choose HiveMQ Cloud as broker
- [x] Decide on Android app as control interface
- [x] Create GitHub repository
- [x] Create initial README and other documentation

**Done when:** Architecture decisions documented

---

## Sprint 1 – Hardware v1 (Initial PCB Design)
**Goal:** Create first working hardware design

- [x] Select ESP8266 dev board
- [x] Design custom PCB in EasyEDA
- [x] Place headers and power circuitry
- [x] Order PCB (v1)
- [x] Discover ESP8266 pin spacing issue (25.1 mm vs 25.4 mm)
- [x] Document PCB design mistake and lesson learned

**Done when:** PCB v1 reviewed and issues identified

---

## Sprint 2 – Hardware v2 (PCB Redesign)
**Goal:** Correct PCB and prepare for reliable assembly

- [x] Re-measure ESP8266 header spacing correctly (25.4 mm)
- [x] Update PCB footprint with correct spacing
- [x] Review hole sizes and tolerances
- [x] Update silkscreen and pin labels
- [x] Order revised PCB (v2)
- [ ] Assemble revised PCB
- [ ] Solder components
- [ ] Power-on test (no load)

**Done when:** PCB v2 powers correctly

---

##  Sprint 3 – MQTT Broker Setup
**Goal:** Establish reliable cloud communication

- [x] Create HiveMQ Cloud cluster
- [x] Configure TLS broker connection
- [x] Understand auto-generated credentials
- [x] Create credentials for Android app
- [x] Rotate credentials after exposure
- [x] Secure credentials using .gitignore
- [x] Verify publish/subscribe using HiveMQ Web Client
- [x] Learn MQTT topic wildcard rules

**Done when:** Messages visible in Web Client

---

##  Sprint 4 – Android App (MQTT Publisher)
**Goal:** Publish MQTT commands from Android

- [x] Create clean Android project
- [x] Evaluate MQTT libraries for Android
- [x] Select Paho MQTT client
- [x] Add minimal dependency only
- [x] Add INTERNET permission
- [x] Implement MQTTHelper class
- [x] Handle async MQTT connection
- [x] Generate unique MQTT client ID per session
- [x] Publish command from button press
- [x] Verify Android → HiveMQ publishing

**Done when:** Android publishes reliably to broker

---

##  Sprint 5 – ESP8266 Firmware
**Goal:** React to MQTT commands and control pump

- [x] Connect ESP8266 to Wi-Fi
- [x] Connect ESP8266 to HiveMQ Cloud (TLS)
- [x] Subscribe to command topic (`plant/pump/on`)
- [ ] Parse incoming payloads
- [ ] Drive relay / pump ON
- [ ] Implement timed auto-OFF logic
- [ ] Publish pump state (`plant/pump/state`)
- [x] Add reconnect handling

**Done when:** ESP reacts correctly to MQTT commands

---

##  Sprint 6 – End-to-End Integration
**Goal:** Full system operation

- [ ] Android publishes ON command
- [ ] ESP receives command
- [ ] Pump activates
- [ ] Pump auto-shuts off after timeout
- [ ] State message published back to broker
- [ ] Observe state in HiveMQ Web Client

**Done when:** Complete Android → Pump → State loop works

---

##  Sprint 7 – Testing & Validation
**Goal:** Prove reliability and safety

- [ ] Test repeated watering commands
- [ ] Test broker disconnect recovery
- [ ] Test power cycling ESP8266
- [ ] Test Android app restart behaviour
- [ ] Record test results in table

**Done when:** System behaves predictably under failure

---

##  Sprint 8 – Documentation & Portfolio Polish
**Goal:** Make project portfolio-ready

- [] Document lessons learned
- [ ] Add system block diagram
- [ ] Document MQTT topic structure
- [ ] Add test evidence screenshots
- [ ] Clean up README
- [ ] Final review for public GitHub


-

