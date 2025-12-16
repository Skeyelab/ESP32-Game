// RGB Guardian 2
// Enhanced version with multiple enemies and improved mechanics

#include <Arduino.h>
#include <FastLED.h>

extern CRGB leds[];
extern const int NUM_LEDS;

static constexpr int DEF_POS = 3;
static constexpr uint32_t TICK_MS = 30;
static constexpr uint32_t SPAWN_EVERY_MS = 700;
static constexpr uint32_t ENEMY_STEP_EVERY_MS = 220;
static constexpr uint32_t BULLET_STEP_EVERY_MS = 120;
static constexpr uint32_t FIRE_EVERY_MS = 240;
static constexpr uint32_t WEAPON_CYCLE_MS = 500;
static constexpr int MAX_ENEMIES = 2;
static constexpr int MAX_BULLETS = 2;

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

static Enemy enemies[MAX_ENEMIES];
static Bullet bullets[MAX_BULLETS];
static ColorId weaponColor = C_RED;
static uint32_t score = 0;
static uint32_t tSpawn = 0, tEnemyStep = 0, tBulletStep = 0, tFire = 0, tWeapon = 0;

static ColorId randColor() { return (ColorId)(esp_random() % 3); }

static void resetGame() {
  for (int i = 0; i < MAX_ENEMIES; i++) enemies[i] = Enemy{};
  for (int i = 0; i < MAX_BULLETS; i++) bullets[i] = Bullet{};
  weaponColor = C_RED;
  score = 0;
  tSpawn = tEnemyStep = tBulletStep = tFire = tWeapon = 0;
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

static void spawnEnemy() {
  // Find inactive enemy slot
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (enemies[i].active) continue;

    bool fromLeft = (esp_random() & 1) == 0;
    enemies[i].active = true;
    enemies[i].dir = fromLeft ? +1 : -1;
    enemies[i].pos = fromLeft ? 0 : (NUM_LEDS - 1);
    enemies[i].color = randColor();
    return;
  }
}

static void fireBullet() {
  // Find nearest enemy
  int nearestEnemy = -1;
  int nearestDist = NUM_LEDS;

  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (!enemies[i].active) continue;
    int dist = abs(enemies[i].pos - DEF_POS);
    if (dist < nearestDist) {
      nearestDist = dist;
      nearestEnemy = i;
    }
  }

  if (nearestEnemy < 0) return;

  // Find inactive bullet slot
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (bullets[i].active) continue;

    bullets[i].active = true;
    bullets[i].pos = DEF_POS;
    bullets[i].dir = (enemies[nearestEnemy].pos < DEF_POS) ? -1 : +1;
    bullets[i].color = weaponColor;
    return;
  }
}

static void stepEnemies() {
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (!enemies[i].active) continue;

    enemies[i].pos += enemies[i].dir;

    if (enemies[i].pos == DEF_POS) {
      flashGameOver();
      resetGame();
      return;
    }

    if (enemies[i].pos < 0 || enemies[i].pos >= NUM_LEDS) {
      enemies[i].active = false;
    }
  }
}

static void stepBullets() {
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (!bullets[i].active) continue;

    bullets[i].pos += bullets[i].dir;
    if (bullets[i].pos < 0 || bullets[i].pos >= NUM_LEDS) {
      bullets[i].active = false;
    }
  }
}

static void collisions() {
  for (int b = 0; b < MAX_BULLETS; b++) {
    if (!bullets[b].active) continue;

    for (int e = 0; e < MAX_ENEMIES; e++) {
      if (!enemies[e].active) continue;
      if (enemies[e].pos != bullets[b].pos) continue;

      if (enemies[e].color == bullets[b].color) {
        enemies[e].active = false;
        score++;
      }
      bullets[b].active = false;
      break;
    }
  }
}

static void render() {
  fadeToBlackBy(leds, NUM_LEDS, 80);

  // Defender
  leds[DEF_POS] = CRGB(25, 25, 25);
  leds[DEF_POS] += colorFromId(weaponColor) / 5;

  // Enemies
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (enemies[i].active) {
      leds[constrain(enemies[i].pos, 0, NUM_LEDS - 1)] = colorFromId(enemies[i].color);
    }
  }

  // Bullets
  for (int i = 0; i < MAX_BULLETS; i++) {
    if (bullets[i].active) {
      leds[constrain(bullets[i].pos, 0, NUM_LEDS - 1)] += colorFromId(bullets[i].color);
    }
  }

  // Score
  uint8_t level = (uint8_t)min<uint32_t>(score * 20, 120);
  leds[NUM_LEDS - 1] += CRGB(0, 0, level);
}

void game_setup() {
  randomSeed(esp_random());
  resetGame();
  Serial.println("RGB Guardian 2 (8 LEDs) on GPIO 16");
}

void game_loop(uint32_t dt) {
  static uint32_t accum = 0;
  accum += dt;

  while (accum >= TICK_MS) {
    accum -= TICK_MS;

    tSpawn += TICK_MS;
    tEnemyStep += TICK_MS;
    tBulletStep += TICK_MS;
    tFire += TICK_MS;
    tWeapon += TICK_MS;

    if (tWeapon >= WEAPON_CYCLE_MS) {
      tWeapon = 0;
      weaponColor = (ColorId)((weaponColor + 1) % 3);
    }

    if (tSpawn >= SPAWN_EVERY_MS) {
      tSpawn = 0;
      spawnEnemy();
    }

    if (tFire >= FIRE_EVERY_MS) {
      tFire = 0;
      fireBullet();
    }

    if (tEnemyStep >= ENEMY_STEP_EVERY_MS) {
      tEnemyStep = 0;
      stepEnemies();
    }

    if (tBulletStep >= BULLET_STEP_EVERY_MS) {
      tBulletStep = 0;
      stepBullets();
    }

    collisions();
    render();
  }

  FastLED.show();
}

