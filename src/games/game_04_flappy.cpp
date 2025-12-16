// FlappyBird
// Control a bird to navigate through gaps in obstacles
// Press to "flap" and move up, gravity pulls down

#include <Arduino.h>
#include <FastLED.h>
#include "../input/touch_input.h"

extern CRGB leds[];
#define NUM_LEDS 8  // Must match main.cpp

static constexpr uint32_t TICK_MS = 50;
static constexpr uint32_t OBSTACLE_SPAWN_MS = 1500;
static constexpr uint32_t OBSTACLE_MOVE_MS = 200;
static constexpr uint32_t GRAVITY_MS = 150;
static constexpr int GAP_SIZE = 2;

struct Obstacle {
  bool active = false;
  int pos = 0;
  int gapTop = 0;  // Top of gap
};

static int birdPos = NUM_LEDS / 2;
static int birdVel = 0;  // Velocity: positive = down, negative = up
static Obstacle obstacles[2];
static uint32_t score = 0;
static uint32_t tObstacleSpawn = 0;
static uint32_t tObstacleMove = 0;
static uint32_t tGravity = 0;
static bool gameOver = false;

static void resetGame() {
  birdPos = NUM_LEDS / 2;
  birdVel = 0;
  for (int i = 0; i < 2; i++) obstacles[i] = Obstacle{};
  score = 0;
  tObstacleSpawn = tObstacleMove = tGravity = 0;
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

static void spawnObstacle() {
  for (int i = 0; i < 2; i++) {
    if (!obstacles[i].active) {
      obstacles[i].active = true;
      obstacles[i].pos = NUM_LEDS - 1;
      // Random gap position
      obstacles[i].gapTop = 2 + (esp_random() % (NUM_LEDS - GAP_SIZE - 2));
      return;
    }
  }
}

static void updateBird() {
  // Apply gravity
  birdVel += 1;  // Gravity pulls down

  // Flap on button press
  if (touch_action_just_pressed()) {
    birdVel -= 3;
  }

  birdPos += birdVel;

  // Boundary check
  if (birdPos < 0) {
    birdPos = 0;
    birdVel = 0;
  }
  if (birdPos >= NUM_LEDS) {
    birdPos = NUM_LEDS - 1;
    birdVel = 0;
  }
}

static void updateObstacles() {
  for (int i = 0; i < 2; i++) {
    if (!obstacles[i].active) continue;

    obstacles[i].pos--;

    if (obstacles[i].pos < 0) {
      obstacles[i].active = false;
      score++;
    }
  }
}

static void checkCollisions() {
  if (gameOver) return;

  // Check boundary collisions
  if (birdPos <= 0 || birdPos >= NUM_LEDS - 1) {
    gameOver = true;
    flashGameOver();
    resetGame();
    return;
  }

  // Check obstacle collisions
  for (int i = 0; i < 2; i++) {
    if (!obstacles[i].active) continue;
    if (obstacles[i].pos != 0) continue;  // Only check when obstacle is at bird position

    int gapBottom = obstacles[i].gapTop + GAP_SIZE;
    if (birdPos < obstacles[i].gapTop || birdPos >= gapBottom) {
      gameOver = true;
      flashGameOver();
      resetGame();
      return;
    }
  }
}

static void render() {
  fadeToBlackBy(leds, NUM_LEDS, 120);

  // Render obstacles
  for (int i = 0; i < 2; i++) {
    if (!obstacles[i].active) continue;

    int gapBottom = obstacles[i].gapTop + GAP_SIZE;
    for (int j = 0; j < NUM_LEDS; j++) {
      if (j < obstacles[i].gapTop || j >= gapBottom) {
        if (obstacles[i].pos >= 0 && obstacles[i].pos < NUM_LEDS) {
          leds[obstacles[i].pos] = CRGB::Red;
        }
      }
    }
  }

  // Render bird (yellow)
  leds[birdPos] = CRGB::Yellow;

  // Score indicator
  uint8_t brightness = (uint8_t)min<uint32_t>(score * 10, 255);
  leds[NUM_LEDS - 1] += CRGB(0, brightness, 0);
}

void game_setup() {
  randomSeed(esp_random());
  resetGame();
  Serial.println("FlappyBird (8 LEDs) on GPIO 16");
  Serial.println("Action touch: flap/jump");
}

void game_loop(uint32_t dt) {
  if (gameOver) return;

  static uint32_t accum = 0;
  accum += dt;

  while (accum >= TICK_MS) {
    accum -= TICK_MS;

    tObstacleSpawn += TICK_MS;
    tObstacleMove += TICK_MS;
    tGravity += TICK_MS;

    if (tObstacleSpawn >= OBSTACLE_SPAWN_MS) {
      tObstacleSpawn = 0;
      spawnObstacle();
    }

    if (tObstacleMove >= OBSTACLE_MOVE_MS) {
      tObstacleMove = 0;
      updateObstacles();
    }

    if (tGravity >= GRAVITY_MS) {
      tGravity = 0;
      updateBird();
    }

    checkCollisions();
    render();
  }

  FastLED.show();
}


