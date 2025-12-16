// RGB Guardian (formerly RGB Defender)
// Color-matching defense game

#include <Arduino.h>
#include <FastLED.h>
#include "../input/touch_input.h"
#ifdef ENABLE_NETWORKING
#include "../status/status_monitor.h"
#endif

extern CRGB leds[];
#define NUM_LEDS 8  // Must match main.cpp

static constexpr int DEF_POS = 3;
static constexpr uint32_t TICK_MS = 30;
static constexpr uint32_t SPAWN_EVERY_MS = 900;
static constexpr uint32_t ENEMY_STEP_EVERY_MS = 260;
static constexpr uint32_t BULLET_STEP_EVERY_MS = 130;

enum ColorId : uint8_t { C_RED=0, C_GREEN=1, C_BLUE=2 };

static CRGB colorFromId(ColorId c) {
  switch (c) {
    case C_RED:   return CRGB::Red;
    case C_GREEN: return CRGB::Green;
    default:      return CRGB::Blue;
  }
}

struct Enemy {
  bool active = false;
  int pos = 0;
  int dir = 0;
  ColorId color = C_RED;
};

struct Bullet {
  bool active = false;
  int pos = 0;
  int dir = 0;
  ColorId color = C_RED;
};

static Enemy enemy;
static Bullet bullet;
static ColorId weaponColor = C_RED;
static uint32_t score = 0;
static uint32_t tSpawn = 0, tEnemyStep = 0, tBulletStep = 0;

static ColorId randColor() { return (ColorId)(esp_random() % 3); }

static void resetGame() {
  enemy = Enemy{};
  bullet = Bullet{};
  weaponColor = C_RED;
  score = 0;
  tSpawn = tEnemyStep = tBulletStep = 0;
  FastLED.clear();
  FastLED.show();
}

static void flashGameOver() {
  for (int i = 0; i < 3; i++) {
    fill_solid(leds, NUM_LEDS, CRGB::Red);
    FastLED.show();
    delay(110);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    delay(110);
  }
}

static void spawnEnemyIfNone() {
  if (enemy.active) return;
  bool fromLeft = (esp_random() & 1) == 0;
  enemy.active = true;
  enemy.dir = fromLeft ? +1 : -1;
  enemy.pos = fromLeft ? 0 : (NUM_LEDS - 1);
  enemy.color = randColor();
}

static void fireTowardEnemy() {
  if (!enemy.active) return;
  if (bullet.active) return;
  bullet.active = true;
  bullet.pos = DEF_POS;
  bullet.dir = (enemy.pos < DEF_POS) ? -1 : +1;
  bullet.color = weaponColor;
}

static void stepEnemy() {
  if (!enemy.active) return;
  enemy.pos += enemy.dir;
  if (enemy.pos == DEF_POS) {
    flashGameOver();
    resetGame();
    return;
  }
  if (enemy.pos < 0 || enemy.pos >= NUM_LEDS) {
    enemy.active = false;
  }
}

static void stepBullet() {
  if (!bullet.active) return;
  bullet.pos += bullet.dir;
  if (bullet.pos < 0 || bullet.pos >= NUM_LEDS) {
    bullet.active = false;
  }
}

static void collisions() {
  if (!enemy.active || !bullet.active) return;
  if (enemy.pos != bullet.pos) return;
  if (enemy.color == bullet.color) {
    enemy.active = false;
    score++;
#ifdef ENABLE_NETWORKING
    status_monitor_update_score(score);
#endif
  }
  bullet.active = false;
}

static void render() {
  fadeToBlackBy(leds, NUM_LEDS, 80);
  leds[DEF_POS] = CRGB(25, 25, 25);
  leds[DEF_POS] += colorFromId(weaponColor) / 5;
  if (enemy.active) leds[constrain(enemy.pos, 0, NUM_LEDS - 1)] = colorFromId(enemy.color);
  if (bullet.active) leds[constrain(bullet.pos, 0, NUM_LEDS - 1)] += colorFromId(bullet.color);
  uint8_t level = (uint8_t)min<uint32_t>(score * 20, 120);
  leds[NUM_LEDS - 1] += CRGB(0, 0, level);
}

static void game_setup() {
  randomSeed(esp_random());
  resetGame();
  Serial.println("RGB Guardian (8 LEDs) on GPIO 16");
  Serial.println("Left touch: weapon color -, Right touch: weapon color +");
  Serial.println("Action touch: fire bullet");
#ifdef ENABLE_NETWORKING
  status_monitor_update_game_name("RGB Guardian");
  status_monitor_update_state(GAME_STATE_PLAYING);
  status_monitor_update_score(0);
#endif
}

static void game_loop(uint32_t dt) {
  static uint32_t accum = 0;
  accum += dt;

  while (accum >= TICK_MS) {
    accum -= TICK_MS;

    tSpawn += TICK_MS;
    tEnemyStep += TICK_MS;
    tBulletStep += TICK_MS;

    // Manual weapon color control
    if (touch_left_just_pressed()) {
      weaponColor = (ColorId)((weaponColor + 2) % 3);  // Cycle backward
    } else if (touch_right_just_pressed()) {
      weaponColor = (ColorId)((weaponColor + 1) % 3);  // Cycle forward
    }

    if (tSpawn >= SPAWN_EVERY_MS) {
      tSpawn = 0;
      spawnEnemyIfNone();
    }

    // Manual fire only
    if (touch_action_just_pressed()) {
      fireTowardEnemy();
    }

    if (tEnemyStep >= ENEMY_STEP_EVERY_MS) {
      tEnemyStep = 0;
      stepEnemy();
    }

    if (tBulletStep >= BULLET_STEP_EVERY_MS) {
      tBulletStep = 0;
      stepBullet();
    }

    collisions();
    render();
  }

  FastLED.show();
}







// Wrapper functions for game manager
void game_06_setup() {
  game_setup();
}

void game_06_loop(uint32_t dt) {
  game_loop(dt);
}
