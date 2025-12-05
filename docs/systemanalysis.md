# System Analysis – SmartPump IoT Project

## 1. Project Overview
The SmartPump system allows a user to remotely activate a water pump connected to an ESP8266 microcontroller. 
Control is done through an Android application which communicates via MQTT.

## 2. System Goals
- Remotely trigger water pump 
- Pump runs for a set time then turns off
- Display pump status in real time on Android app
- Support communication from different networks (outside LAN)


## 3. Technologies Chosen
### 3.1 Hardware
- ESP8266 (NodeMCU)
- Relay or MOSFET driver for pump
- Water pump module

### 3.2 Software
- **MQTT** (HiveMQ Cloud free tier)
- ESP8266 Arduino framework
- Android Studio (Java)
- Paho MQTT Client for Android

### 3.3 Why MQTT?
- Lightweight communication protocol
- Designed for IoT
- Supports real-time status updates
- Works outside local network


## 4. System Architecture
- Android app publishes messages to `pump/control`
- ESP8266 subscribes to `pump/control`
- ESP8266 operates pump and publishes status to `pump/status`
- Android app subscribes to `pump/status`



## 5. Requirements

### 5.1 Functional Requirements
- User can press “Pump On” button in app
- ESP8266 receives MQTT message and activates pump
- Pump auto-shuts off after a fixed duration
- App shows real-time status (ON/OFF)
- App reconnects to MQTT on network change

### 5.2 Non-Functional Requirements
- Secure MQTT credentials storage
- Low latency communication (<500ms)
- Reliable reconnection logic
- Minimal power consumption for ESP8266

## 6. Future Enhancements
- Add soil moisture sensor live readings
- Add scheduling (watering timer)
- Add notification when pump runs
- Add history logging with charts