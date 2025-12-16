// WiFi configuration
// Set your WiFi credentials here or use WiFiManager for runtime configuration

#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

// WiFi credentials
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// AP mode fallback (if WiFi connection fails)
#define AP_SSID "ESP32-Game"
#define AP_PASSWORD ""

// Connection timeout (milliseconds)
#define WIFI_CONNECT_TIMEOUT_MS 30000

#endif // WIFI_CONFIG_H

