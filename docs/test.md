### Communication and Network Testing

| Test ID | Test Description | Test Steps | Expected Result | Actual Result | Status |
|-------|------------------|-----------|-----------------|---------------|--------|
| T01 | ESP connects to Wi-Fi (LAN) | Power ESP on within home Wi-Fi network | ESP connects and obtains IP address | Connected successfully, IP assigned | [x] |
| T02 | MQTT broker connection | ESP attempts to connect to MQTT broker | MQTT connection established | Connected to HiveMQ broker | [x] |
| T03 | MQTT topic subscription | ESP subscribes to `plant/pump/on` topic | Subscription successful | Subscribed successfully | [x] |
| T04 | MQTT message receive (LAN) | Publish `on` to `plant/pump/on` from same network | ESP receives message | Message received correctly | [x] |
| T05 | MQTT message receive (Mobile Data) | Publish `on` to `plant/pump/on` via mobile network | ESP receives message | Message received correctly | [x] |
| T06 | Cross-network MQTT messaging | ESP on Wi-Fi, client on mobile data | Message received without delay | Successful cross-network delivery | [x] |

### Notes
- Relay hardware was not connected during this test phase.
- Testing focused on Wi-Fi connectivity and MQTT messaging.
- Relay and pump activation tests will be added in a later phase.