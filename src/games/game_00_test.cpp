// Test Game
// Simple test to verify touch controls work
// Left/Right: Move LED
// Action: Change color
// Alt: Flash all LEDs

#include <Arduino.h>
#include <FastLED.h>
#include "../input/touch_input.h"
#ifdef ENABLE_NETWORKING
#include "../status/status_monitor.h"
#endif

extern CRGB leds[];
#define NUM_LEDS 8  // Must match main.cpp

static int ledPos = NUM_LEDS / 2;
static uint8_t colorIndex = 0;
static const CRGB colors[] = {
  CRGB::Red,
  CRGB::Green,
  CRGB::Blue,
  CRGB::Yellow,
  CRGB::Cyan,
  CRGB::Magenta,
  CRGB::White
};
static const int numColors = sizeof(colors) / sizeof(colors[0]);
static uint32_t flashTimer = 0;
static bool flashing = false;

static void resetGame() {
  ledPos = NUM_LEDS / 2;
  colorIndex = 0;
  flashTimer = 0;
  flashing = false;
  FastLED.clear();
  FastLED.show();
}

static void flashAll() {
  flashing = true;
  flashTimer = 200; // Flash for 200ms
  fill_solid(leds, NUM_LEDS, colors[colorIndex]);
  FastLED.show();
}

static void updatePosition() {
  if (touch_left_just_pressed() && ledPos > 0) {
    ledPos--;
  }
  if (touch_right_just_pressed() && ledPos < NUM_LEDS - 1) {
    ledPos++;
  }
}

static void updateColor() {
  if (touch_action_just_pressed()) {
    colorIndex = (colorIndex + 1) % numColors;
  }
}

static void updateFlash() {
  if (touch_alt_just_pressed()) {
    flashAll();
  }

  if (flashing) {
    if (flashTimer > 0) {
      flashTimer--;
    } else {
      flashing = false;
      FastLED.clear();
    }
  }
}

static void render() {
  if (!flashing) {
    fadeToBlackBy(leds, NUM_LEDS, 50);
    leds[ledPos] = colors[colorIndex];
  }
}

static void game_setup() {
  resetGame();
  Serial.println("=== TOUCH CONTROL TEST ===");
  Serial.println("Left touch (GPIO 4): Move LED left");
  Serial.println("Right touch (GPIO 15): Move LED right");
  Serial.println("Action touch (GPIO 13): Change color");
  Serial.println("Alt touch (GPIO 12): Flash all LEDs");
  Serial.println("Current color: Red");
#ifdef ENABLE_NETWORKING
  status_monitor_update_game_name("Test");
  status_monitor_update_state(GAME_STATE_PLAYING);
  status_monitor_update_score(0);
#endif
}

static void game_loop(uint32_t dt) {
  static uint32_t accum = 0;
  accum += dt;

  // Update at ~30fps
  while (accum >= 33) {
    accum -= 33;

    updatePosition();
    updateColor();
    updateFlash();
    render();
  }

  FastLED.show();
}

// Wrapper functions for game manager
void game_00_setup() {
  game_setup();
}

void game_00_loop(uint32_t dt) {
  game_loop(dt);
}

