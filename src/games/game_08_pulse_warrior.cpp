// Pulse Warrior
// Rhythm-based game where you must time actions with pulses
// Press at the right moment to score points

#include <Arduino.h>
#include <FastLED.h>
#include "../input/touch_input.h"

extern CRGB leds[];
#define NUM_LEDS 8  // Must match main.cpp

static constexpr uint32_t TICK_MS = 50;
static constexpr uint32_t PULSE_INTERVAL_MS = 800;
static constexpr uint32_t PULSE_DURATION_MS = 200;
static constexpr uint32_t TARGET_WINDOW_MS = 150;

static int pulsePos = 0;
static int targetPos = NUM_LEDS / 2;
static bool pulseActive = false;
static uint32_t pulseTimer = 0;
static uint32_t score = 0;
static uint32_t combo = 0;
static uint32_t tPulse = 0;

static void resetGame() {
  pulsePos = 0;
  targetPos = NUM_LEDS / 2;
  pulseActive = false;
  pulseTimer = 0;
  score = 0;
  combo = 0;
  tPulse = 0;
  FastLED.clear();
  FastLED.show();
}

static void spawnPulse() {
  if (pulseActive) return;

  pulseActive = true;
  pulsePos = 0;
  pulseTimer = PULSE_DURATION_MS;
  targetPos = 2 + (esp_random() % (NUM_LEDS - 4));  // Random target, not at edges
}

static void updatePulse() {
  if (!pulseActive) return;

  pulseTimer -= TICK_MS;

  if (pulseTimer <= 0) {
    pulseActive = false;
    combo = 0;  // Reset combo on miss
    return;
  }

  // Pulse moves toward target
  if (pulsePos < targetPos) {
    pulsePos++;
  } else if (pulsePos > targetPos) {
    pulsePos--;
  }
}

static void checkHit() {
  if (!pulseActive) return;

  // Check button press
  if (touch_action_just_pressed() && pulsePos == targetPos) {
    // Perfect hit!
    score += 10 + combo;
    combo++;
    pulseActive = false;

    // Flash success
    fill_solid(leds, NUM_LEDS, CRGB::Green);
    FastLED.show();
    delay(50);
  }
}

static void render() {
  fadeToBlackBy(leds, NUM_LEDS, 150);

  // Render target position (dim white)
  leds[targetPos] = CRGB(30, 30, 30);

  // Render pulse
  if (pulseActive) {
    // Intensity based on proximity to target
    int dist = abs(pulsePos - targetPos);
    uint8_t intensity = 255 - (dist * 40);
    if (intensity > 255) intensity = 255;

    leds[pulsePos] = CRGB(intensity, intensity / 2, 0);  // Orange pulse

    // Highlight when close to target
    if (dist <= 1) {
      leds[targetPos] = CRGB::Yellow;
    }
  }

  // Score/combo indicator
  uint8_t comboBright = (uint8_t)min<uint32_t>(combo * 20, 255);
  leds[NUM_LEDS - 1] += CRGB(0, comboBright, comboBright);
}

void game_setup() {
  randomSeed(esp_random());
  resetGame();
  Serial.println("Pulse Warrior (8 LEDs) on GPIO 16");
  Serial.println("Action touch: hit when pulse reaches target");
}

void game_loop(uint32_t dt) {
  static uint32_t accum = 0;
  accum += dt;

  while (accum >= TICK_MS) {
    accum -= TICK_MS;

    tPulse += TICK_MS;

    if (tPulse >= PULSE_INTERVAL_MS) {
      tPulse = 0;
      spawnPulse();
    }

    updatePulse();
    checkHit();
    render();
  }

  FastLED.show();
}


