// WiFi manager implementation

#include "wifi_manager.h"
#include <WiFi.h>
#include "../config/wifi_config.h"

static WiFiState wifiState = WIFI_STATE_DISCONNECTED;
static uint32_t connectStartTime = 0;

void wifi_manager_init() {
  // Start in AP mode by default (self-hosted server)
  WiFi.mode(WIFI_AP);
  wifiState = WIFI_STATE_DISCONNECTED;
}

bool wifi_manager_connect(const char* ssid, const char* password, uint32_t timeout_ms) {
  if (wifiState == WIFI_STATE_CONNECTED) {
    return true;
  }

  wifiState = WIFI_STATE_CONNECTING;
  connectStartTime = millis();

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - connectStartTime > timeout_ms) {
      wifiState = WIFI_STATE_DISCONNECTED;
      return false;
    }
    delay(100);
  }

  wifiState = WIFI_STATE_CONNECTED;
  return true;
}

void wifi_manager_start_ap(const char* ssid, const char* password) {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  wifiState = WIFI_STATE_AP_MODE;
}

WiFiState wifi_manager_get_state() {
  return wifiState;
}

String wifi_manager_get_ip() {
  if (wifiState == WIFI_STATE_CONNECTED) {
    return WiFi.localIP().toString();
  } else if (wifiState == WIFI_STATE_AP_MODE) {
    return WiFi.softAPIP().toString();
  }
  return String("");
}

void wifi_manager_update() {
  if (wifiState == WIFI_STATE_CONNECTING) {
    if (WiFi.status() == WL_CONNECTED) {
      wifiState = WIFI_STATE_CONNECTED;
    } else if (millis() - connectStartTime > WIFI_CONNECT_TIMEOUT_MS) {
      wifiState = WIFI_STATE_DISCONNECTED;
    }
  } else if (wifiState == WIFI_STATE_CONNECTED) {
    if (WiFi.status() != WL_CONNECTED) {
      wifiState = WIFI_STATE_DISCONNECTED;
    }
  }
}

bool wifi_manager_is_connected() {
  return wifiState == WIFI_STATE_CONNECTED || wifiState == WIFI_STATE_AP_MODE;
}

