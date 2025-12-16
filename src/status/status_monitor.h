// Status monitoring abstraction
// Tracks game status and only reports changes

#ifndef STATUS_MONITOR_H
#define STATUS_MONITOR_H

#include <Arduino.h>
#include <stdint.h>

// Game state enumeration
enum GameState {
  GAME_STATE_PLAYING,
  GAME_STATE_GAME_OVER,
  GAME_STATE_WON,
  GAME_STATE_PAUSED
};

// LED color structure
struct LEDColor {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

// Status structure
struct GameStatus {
  const char* gameName;
  uint32_t score;
  GameState state;
  bool leftPressed;
  bool rightPressed;
  bool actionPressed;
  bool altPressed;
  LEDColor leds[8];  // LED strip state
  uint32_t timestamp;
  bool hasChanged;
};

// Initialize status monitor
void status_monitor_init();

// Update status (call from game loop)
void status_monitor_update_game_name(const char* name);
void status_monitor_update_score(uint32_t score);
void status_monitor_update_state(GameState state);
void status_monitor_update_input(bool left, bool right, bool action, bool alt);
void status_monitor_update_leds(const LEDColor* leds, int count);

// Get current status
GameStatus status_monitor_get();

// Check if status has changed (and clear the flag)
bool status_monitor_has_changed();

// Reset change flag
void status_monitor_clear_changed();

#endif // STATUS_MONITOR_H

