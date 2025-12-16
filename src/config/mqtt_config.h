// MQTT configuration

#ifndef MQTT_CONFIG_H
#define MQTT_CONFIG_H

// MQTT broker settings
#define MQTT_BROKER "192.168.1.100"  // Change to your MQTT broker IP
#define MQTT_PORT 1883
#define MQTT_USERNAME ""  // Leave empty if no authentication
#define MQTT_PASSWORD ""  // Leave empty if no authentication

// Client ID
#define MQTT_CLIENT_ID "esp32-game"

// Topic prefix
#define MQTT_TOPIC_PREFIX "esp32-game"

// Reconnection interval (milliseconds)
#define MQTT_RECONNECT_INTERVAL_MS 5000

#endif // MQTT_CONFIG_H

