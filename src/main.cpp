#include <Arduino.h>
#include <FastLED.h>
#include "input/touch_input.h"
#include "games/game_manager.h"

// Enable networking (comment out to disable)
#define ENABLE_NETWORKING

#ifdef ENABLE_NETWORKING
#include "status/status_monitor.h"
#include "network/wifi_manager.h"
#include "network/web_server.h"
#include "network/mqtt_client.h"
#include "config/wifi_config.h"
#include "config/mqtt_config.h"
#include <ArduinoJson.h>
#include <FastLED.h>  // For CRGB access
#endif

#define LED_PIN     16
#define NUM_LEDS    8
#define BRIGHTNESS  10
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(115200);
  delay(200);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  touch_input_init();

  // Initialize game manager (loads saved game from EEPROM)
  game_manager_init();

#ifdef ENABLE_NETWORKING
  // Initialize status monitor
  status_monitor_init();

  // Initialize WiFi - start in AP mode (self-hosted server)
  wifi_manager_init();
  Serial.println("Starting AP mode (self-hosted server)...");
  wifi_manager_start_ap(AP_SSID, AP_PASSWORD);
  Serial.print("AP started! IP: ");
  Serial.println(wifi_manager_get_ip());

  // Initialize web server
  web_server_init();

  // MQTT is disabled when using AP mode (no internet connection)
  // Uncomment below if you want to enable MQTT with a different network setup
  // mqtt_client_init();
  // if (mqtt_client_connect(MQTT_BROKER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD, MQTT_CLIENT_ID)) {
  //   Serial.println("MQTT connected!");
  // } else {
  //   Serial.println("MQTT connection failed (will retry)");
  // }
#endif

  // Setup the current game (loaded from EEPROM or default)
  game_manager_setup();
}

void loop() {
  static uint32_t last = millis();
  uint32_t now = millis();
  uint32_t dt = now - last;
  last = now;

  touch_input_update();

#ifdef ENABLE_NETWORKING
  // Update network services
  wifi_manager_update();
  web_server_update();
  // MQTT disabled in AP mode (no internet connection)
  // mqtt_client_update();

  // Update status monitor with input state
  InputState input = touch_input_get();
  status_monitor_update_input(
    input.left.pressed,
    input.right.pressed,
    input.action.pressed,
    input.alt.pressed
  );
#endif

  game_manager_loop(dt);

#ifdef ENABLE_NETWORKING
  // Update status monitor with LED state AFTER game_loop (so we capture the rendered state)
  LEDColor ledColors[8];
  for (int i = 0; i < NUM_LEDS; i++) {
    ledColors[i].r = leds[i].r;
    ledColors[i].g = leds[i].g;
    ledColors[i].b = leds[i].b;
  }
  status_monitor_update_leds(ledColors, NUM_LEDS);

  // Always send status updates (LEDs change every frame, not just on status changes)
  GameStatus status = status_monitor_get();

  // Publish full status as JSON (increased size for LED array)
  StaticJsonDocument<768> doc;
  doc["gameName"] = status.gameName;
  doc["score"] = status.score;
  doc["state"] = status.state;
  doc["leftPressed"] = status.leftPressed;
  doc["rightPressed"] = status.rightPressed;
  doc["actionPressed"] = status.actionPressed;
  doc["altPressed"] = status.altPressed;
  doc["timestamp"] = status.timestamp;

    // Add LED array - ensure all 8 LEDs are included
    JsonArray ledsArray = doc.createNestedArray("leds");
    for (int i = 0; i < 8; i++) {
      JsonObject led = ledsArray.createNestedObject();
      led["r"] = (int)status.leds[i].r;
      led["g"] = (int)status.leds[i].g;
      led["b"] = (int)status.leds[i].b;
    }

  // MQTT publishing disabled in AP mode (no internet connection)
  // Status is available via web server at /status endpoint
  if (status_monitor_has_changed()) {
    // Status changes are automatically available via web server
    status_monitor_clear_changed();
  }
#endif
}

// All games are compiled separately by PlatformIO
// Game manager uses wrapper functions (game_XX_setup, game_XX_loop) to call them
