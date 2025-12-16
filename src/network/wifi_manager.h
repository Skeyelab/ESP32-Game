// WiFi connection manager

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>

// WiFi connection states
enum WiFiState {
  WIFI_STATE_DISCONNECTED,
  WIFI_STATE_CONNECTING,
  WIFI_STATE_CONNECTED,
  WIFI_STATE_AP_MODE
};

// Initialize WiFi manager
void wifi_manager_init();

// Connect to WiFi (blocking, with timeout)
bool wifi_manager_connect(const char* ssid, const char* password, uint32_t timeout_ms);

// Start AP mode
void wifi_manager_start_ap(const char* ssid, const char* password);

// Get current WiFi state
WiFiState wifi_manager_get_state();

// Get IP address (returns empty string if not connected)
String wifi_manager_get_ip();

// Update (call in loop for non-blocking operations)
void wifi_manager_update();

// Check if connected
bool wifi_manager_is_connected();

#endif // WIFI_MANAGER_H

