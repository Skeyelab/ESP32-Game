// Color Runner X
// Run through colored zones, matching your color to pass through
// Change your color to match obstacles

#include <Arduino.h>
#include <FastLED.h>
#include "../input/touch_input.h"
#ifdef ENABLE_NETWORKING
#include "../status/status_monitor.h"
#endif

extern CRGB leds[];
#define NUM_LEDS 8  // Must match main.cpp

static constexpr uint32_t TICK_MS = 100;
static constexpr uint32_t ZONE_SPAWN_MS = 1200;
static constexpr uint32_t ZONE_MOVE_MS = 200;
static constexpr uint32_t COLOR_CHANGE_MS = 500;

enum ColorId : uint8_t { C_RED=0, C_GREEN=1, C_BLUE=2 };

static CRGB colorFromId(ColorId c) {
  switch (c) {
    case C_RED:   return CRGB::Red;
    case C_GREEN: return CRGB::Green;
    default:      return CRGB::Blue;
  }
}

struct Zone {
  bool active = false;
  int pos = 0;
  ColorId color = C_RED;
};

static int playerPos = 0;
static ColorId playerColor = C_RED;
static Zone zones[3];
static uint32_t score = 0;
static uint32_t tZoneSpawn = 0;
static uint32_t tZoneMove = 0;
static uint32_t tColorChange = 0;
static bool gameOver = false;

static void resetGame() {
  playerPos = 0;
  playerColor = C_RED;
  for (int i = 0; i < 3; i++) zones[i] = Zone{};
  score = 0;
  tZoneSpawn = tZoneMove = tColorChange = 0;
  gameOver = false;
  FastLED.clear();
  FastLED.show();
}

static void flashGameOver() {
  for (int i = 0; i < 3; i++) {
    fill_solid(leds, NUM_LEDS, CRGB::Red);
    FastLED.show();
    delay(150);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    delay(150);
  }
}

static void flashWin() {
  for (int i = 0; i < 3; i++) {
    fill_solid(leds, NUM_LEDS, CRGB::Green);
    FastLED.show();
    delay(150);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    delay(150);
  }
}

static ColorId randColor() { return (ColorId)(esp_random() % 3); }

static void spawnZone() {
  for (int i = 0; i < 3; i++) {
    if (!zones[i].active) {
      zones[i].active = true;
      zones[i].pos = NUM_LEDS - 1;
      zones[i].color = randColor();
      return;
    }
  }
}

static void updateZones() {
  for (int i = 0; i < 3; i++) {
    if (!zones[i].active) continue;

    zones[i].pos--;

    if (zones[i].pos < 0) {
      zones[i].active = false;
      score++;
#ifdef ENABLE_NETWORKING
      status_monitor_update_score(score);
#endif
    }
  }
}

static void updatePlayer() {
  // Move based on input
  if (touch_action_just_pressed() && playerPos < NUM_LEDS - 1) {
    playerPos++;
  }

  // Change color on button press
  if (touch_alt_just_pressed()) {
    playerColor = (ColorId)((playerColor + 1) % 3);
  }
}

static void checkCollisions() {
  if (gameOver) return;

  for (int i = 0; i < 3; i++) {
    if (!zones[i].active) continue;
    if (zones[i].pos != playerPos) continue;

    if (zones[i].color != playerColor) {
      // Wrong color - game over
      gameOver = true;
#ifdef ENABLE_NETWORKING
      status_monitor_update_state(GAME_STATE_GAME_OVER);
#endif
      flashGameOver();
      delay(1000);
      resetGame();
#ifdef ENABLE_NETWORKING
      status_monitor_update_state(GAME_STATE_PLAYING);
      status_monitor_update_score(0);
#endif
      return;
    } else {
      // Correct color - pass through
      zones[i].active = false;
      score += 5;
#ifdef ENABLE_NETWORKING
      status_monitor_update_score(score);
#endif
    }
  }

  // Win condition: reach end
  if (playerPos >= NUM_LEDS - 1) {
    flashWin();
    delay(1000);
    resetGame();
  }
}

static void render() {
  fadeToBlackBy(leds, NUM_LEDS, 120);

  // Render zones
  for (int i = 0; i < 3; i++) {
    if (zones[i].active && zones[i].pos >= 0 && zones[i].pos < NUM_LEDS) {
      leds[zones[i].pos] = colorFromId(zones[i].color);
    }
  }

  // Render player
  leds[playerPos] = colorFromId(playerColor);

  // Score indicator
  uint8_t brightness = (uint8_t)min<uint32_t>(score * 5, 255);
  leds[NUM_LEDS - 1] += CRGB(0, brightness, brightness);
}

static void game_setup() {
  randomSeed(esp_random());
  resetGame();
  Serial.println("Color Runner X (8 LEDs) on GPIO 16");
  Serial.println("Action touch: move forward, Alt touch: change color");
#ifdef ENABLE_NETWORKING
  status_monitor_update_game_name("Color Runner X");
  status_monitor_update_state(GAME_STATE_PLAYING);
  status_monitor_update_score(0);
#endif
}

static void game_loop(uint32_t dt) {
  if (gameOver) return;

  static uint32_t accum = 0;
  accum += dt;

  while (accum >= TICK_MS) {
    accum -= TICK_MS;

    tZoneSpawn += TICK_MS;
    tZoneMove += TICK_MS;
    tColorChange += TICK_MS;

    if (tZoneSpawn >= ZONE_SPAWN_MS) {
      tZoneSpawn = 0;
      spawnZone();
    }

    if (tZoneMove >= ZONE_MOVE_MS) {
      tZoneMove = 0;
      updateZones();
    }

    updatePlayer();
    checkCollisions();
    render();
  }

  FastLED.show();
}







// Wrapper functions for game manager
void game_09_setup() {
  game_setup();
}

void game_09_loop(uint32_t dt) {
  game_loop(dt);
}
