## Initial ESP8266 MQTT Testing Plan

### Instructions
1. Power on the ESP8266 and open the Serial Monitor in Arduino IDE.  
2. Check that Wi-Fi connects properly:
   - Serial Monitor should show:  
     ```
     Connecting to WiFi...
     WiFi connected. IP: 192.168.x.x
     ```
3. Open HiveMQ Web Client (or MQTT Explorer).  
4. Subscribe to the status topic (e.g., `plant/pump/status`) to see responses.  
5. Publish `"ON"` to the command topic (`plant/pump/on`) to test the pump.  
6. Observe Serial Monitor and HiveMQ for debug messages and `"DONE"` confirmation.

---

| Test Case | Topic | Payload | Expected Result | Actual Result | Pass/Fail |
|-----------|-------|---------|----------------|---------------|-----------|
| Wi-Fi connection | N/A | N/A | ESP connects to Wi-Fi; Serial Monitor shows IP | TBD | TBD |
| MQTT broker connection | N/A | N/A | ESP connects to HiveMQ broker; subscribes to topic | TBD | TBD |
| Send pump ON message | `plant/pump/on` | `"ON"` | Pump runs for 2 seconds; ESP publishes `"DONE"` | TBD | TBD |
| Rapid ON requests | `plant/pump/on` | `"ON"` repeatedly | Pump triggers correctly each time without errors | TBD | TBD |
| Invalid payload | `plant/pump/on` | `"INVALID"` | Pump state unchanged; no `"DONE"` published | TBD | TBD |
| MQTT disconnect/reconnect | N/A | N/A | ESP reconnects automatically and resubscribes | TBD | TBD |
| Wi-Fi disconnect/reconnect | N/A | N/A | ESP reconnects to Wi-Fi automatically; resumes MQTT connection | TBD | TBD |
