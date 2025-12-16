// MQTT client implementation

#include "mqtt_client.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "../config/mqtt_config.h"
#include <ArduinoJson.h>

static WiFiClient wifiClient;
static PubSubClient* mqttClient = nullptr;
static bool mqttConnected = false;
static uint32_t lastReconnectAttempt = 0;

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  // Handle incoming messages (optional - for remote control)
  // For now, just log them
  Serial.print("MQTT message received on topic: ");
  Serial.println(topic);
}

bool mqtt_reconnect() {
  if (mqttClient == nullptr) {
    return false;
  }

  Serial.print("Attempting MQTT connection...");

  String clientId = String(MQTT_CLIENT_ID) + "-" + String(random(0xffff), HEX);

  bool connected = false;
  if (strlen(MQTT_USERNAME) > 0) {
    connected = mqttClient->connect(clientId.c_str(), MQTT_USERNAME, MQTT_PASSWORD);
  } else {
    connected = mqttClient->connect(clientId.c_str());
  }

  if (connected) {
    Serial.println(" connected");
    mqttConnected = true;

    // Subscribe to command topic (optional)
    String cmdTopic = String(MQTT_TOPIC_PREFIX) + "/command";
    mqttClient->subscribe(cmdTopic.c_str());
  } else {
    Serial.print(" failed, rc=");
    Serial.println(mqttClient->state());
    mqttConnected = false;
  }

  return connected;
}

void mqtt_client_init() {
  if (mqttClient != nullptr) {
    return;
  }

  mqttClient = new PubSubClient(wifiClient);
  mqttClient->setCallback(mqtt_callback);
  mqttConnected = false;
}

bool mqtt_client_connect(const char* broker, uint16_t port, const char* username, const char* password, const char* clientId) {
  if (mqttClient == nullptr) {
    mqtt_client_init();
  }

  mqttClient->setServer(broker, port);
  return mqtt_reconnect();
}

void mqtt_client_update() {
  if (mqttClient == nullptr) {
    return;
  }

  if (!mqttClient->connected()) {
    mqttConnected = false;
    uint32_t now = millis();
    if (now - lastReconnectAttempt > MQTT_RECONNECT_INTERVAL_MS) {
      lastReconnectAttempt = now;
      mqtt_reconnect();
    }
  } else {
    mqttClient->loop();
  }
}

void mqtt_client_publish_status(const char* json) {
  if (mqttClient == nullptr || !mqttConnected) {
    return;
  }

  String topic = String(MQTT_TOPIC_PREFIX) + "/status";
  mqttClient->publish(topic.c_str(), json);
}

void mqtt_client_publish_score(uint32_t score) {
  if (mqttClient == nullptr || !mqttConnected) {
    return;
  }

  String topic = String(MQTT_TOPIC_PREFIX) + "/score";
  String payload = String(score);
  mqttClient->publish(topic.c_str(), payload.c_str());
}

void mqtt_client_publish_game_state(uint8_t state) {
  if (mqttClient == nullptr || !mqttConnected) {
    return;
  }

  String topic = String(MQTT_TOPIC_PREFIX) + "/game-state";
  String payload = String(state);
  mqttClient->publish(topic.c_str(), payload.c_str());
}

void mqtt_client_publish_input(bool left, bool right, bool action, bool alt) {
  if (mqttClient == nullptr || !mqttConnected) {
    return;
  }

  StaticJsonDocument<128> doc;
  doc["left"] = left;
  doc["right"] = right;
  doc["action"] = action;
  doc["alt"] = alt;
  doc["timestamp"] = millis();

  String topic = String(MQTT_TOPIC_PREFIX) + "/input";
  String payload;
  serializeJson(doc, payload);
  mqttClient->publish(topic.c_str(), payload.c_str());
}

bool mqtt_client_is_connected() {
  return mqttConnected && mqttClient != nullptr && mqttClient->connected();
}

