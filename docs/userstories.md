# User Stories - Plant Watering App

| ID | User Story | Acceptance Criteria | Done |
|----|------------|-------------------|------|
| US01 | As a user, I want to turn the water pump on from my phone so that I can water my plant remotely even when I’m not on the same Wi-Fi network | - Press button in app triggers MQTT message to pump  <br> - Pump runs for set duration (e.g., 2 seconds) | ⬜ |
| US02 | As a user, I want a simple app on my phone so that I can water my plant without physically interacting with the pump | - Main screen has a “Water Plant” button  <br> - Button triggers MQTT message  <br> - App provides visual feedback that command was sent | ⬜ |
| US03 | As a user, I want to know whether the watering was successful so that I can be confident my plant has been watered | - ESP publishes a response message `"DONE"` after watering  <br> - App subscribes to MQTT status topic  <br> - App shows notification or status indicator confirming pump finished | ⬜ |
