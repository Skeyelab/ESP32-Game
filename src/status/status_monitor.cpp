// Status monitor implementation

#include "status_monitor.h"

static GameStatus currentStatus = {
  .gameName = "Unknown",
  .score = 0,
  .state = GAME_STATE_PLAYING,
  .leftPressed = false,
  .rightPressed = false,
  .actionPressed = false,
  .altPressed = false,
  .leds = {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}},
  .timestamp = 0,
  .hasChanged = false
};

static GameStatus previousStatus = currentStatus;

void status_monitor_init() {
  currentStatus = {
    .gameName = "Unknown",
    .score = 0,
    .state = GAME_STATE_PLAYING,
    .leftPressed = false,
    .rightPressed = false,
    .actionPressed = false,
    .altPressed = false,
    .leds = {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}},
    .timestamp = 0,
    .hasChanged = false
  };
  previousStatus = currentStatus;
}

void status_monitor_update_leds(const LEDColor* leds, int count) {
  int maxCount = (count > 8) ? 8 : count;

  // Always update LED state (LEDs change every frame)
  for (int i = 0; i < maxCount; i++) {
    currentStatus.leds[i] = leds[i];
  }

  // LEDs always trigger a change (for web server updates)
  currentStatus.hasChanged = true;
  currentStatus.timestamp = millis();
}

void status_monitor_update_game_name(const char* name) {
  if (name != nullptr && strcmp(currentStatus.gameName, name) != 0) {
    currentStatus.gameName = name;
    currentStatus.hasChanged = true;
  }
  currentStatus.timestamp = millis();
}

void status_monitor_update_score(uint32_t score) {
  if (currentStatus.score != score) {
    currentStatus.score = score;
    currentStatus.hasChanged = true;
  }
  currentStatus.timestamp = millis();
}

void status_monitor_update_state(GameState state) {
  if (currentStatus.state != state) {
    currentStatus.state = state;
    currentStatus.hasChanged = true;
  }
  currentStatus.timestamp = millis();
}

void status_monitor_update_input(bool left, bool right, bool action, bool alt) {
  bool changed = false;

  if (currentStatus.leftPressed != left) {
    currentStatus.leftPressed = left;
    changed = true;
  }
  if (currentStatus.rightPressed != right) {
    currentStatus.rightPressed = right;
    changed = true;
  }
  if (currentStatus.actionPressed != action) {
    currentStatus.actionPressed = action;
    changed = true;
  }
  if (currentStatus.altPressed != alt) {
    currentStatus.altPressed = alt;
    changed = true;
  }

  if (changed) {
    currentStatus.hasChanged = true;
  }
  currentStatus.timestamp = millis();
}

GameStatus status_monitor_get() {
  return currentStatus;
}

bool status_monitor_has_changed() {
  return currentStatus.hasChanged;
}

void status_monitor_clear_changed() {
  previousStatus = currentStatus;
  currentStatus.hasChanged = false;
}

