// MQTT client for status publishing

#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <Arduino.h>

// Initialize MQTT client
void mqtt_client_init();

// Connect to MQTT broker
bool mqtt_client_connect(const char* broker, uint16_t port, const char* username, const char* password, const char* clientId);

// Update (call in loop)
void mqtt_client_update();

// Publish status update
void mqtt_client_publish_status(const char* json);

// Publish score update
void mqtt_client_publish_score(uint32_t score);

// Publish game state update
void mqtt_client_publish_game_state(uint8_t state);

// Publish input state update
void mqtt_client_publish_input(bool left, bool right, bool action, bool alt);

// Check if connected
bool mqtt_client_is_connected();

#endif // MQTT_CLIENT_H

