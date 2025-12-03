Test Checklist
Test ID	Description	How to Test	Expected Result	Done
T1	MQTT Connection	Power on ESP and watch Serial Monitor	Shows “Connected to MQTT” and publishes "online"	[ ]
T2	Pump Command	Publish "plant1" to plant/system/water	Pump runs for 2 seconds, publishes "success"	[ ]
T3	Soil Moisture Data	Subscribe to plant/soil/plant1	ESP publishes moisture readings	[ ]
T4	Reconnection	Disconnect and reconnect WiFi	ESP reconnects automatically to WiFi + MQTT	[ ]
T5	Invalid Command Handling	Send "banana" to plant/system/water	ESP prints “Unknown plant”, pump does not run	[ ]
T6	MQTT Explorer Test	Use MQTT Explorer to publish & subscribe	All topics behave consistently with HiveMQ Web Client	[ ]
T7	Status Topic	Subscribe to plant/system/status	Receives "online" on boot & "offline" if device resets	[ ]
T8	Pump Safety Check	Send rapid multiple “plant1” commands	ESP still only runs pump once per request	[ ]