// 1D Pacman
// Control a yellow dot to eat pellets while avoiding faster red ghosts
// Power pellets let you temporarily turn the tables

#include <Arduino.h>
#include <FastLED.h>

extern CRGB leds[];
extern const int NUM_LEDS;

static constexpr uint32_t TICK_MS = 50;
static constexpr uint32_t PELLET_SPAWN_MS = 2000;
static constexpr uint32_t GHOST_SPAWN_MS = 3000;
static constexpr uint32_t GHOST_MOVE_MS = 400;
static constexpr uint32_t POWER_PELLET_DURATION_MS = 5000;

struct Pellet {
  bool active = false;
  int pos = 0;
  bool isPowerPellet = false;
};

struct Ghost {
  bool active = false;
  int pos = 0;
  int dir = 1;  // +1 right, -1 left
};

static int pacmanPos = NUM_LEDS / 2;
static int pacmanDir = 0;  // -1 left, 0 none, +1 right
static Pellet pellets[4];
static Ghost ghost;
static uint32_t score = 0;
static uint32_t powerPelletTimer = 0;
static bool powerMode = false;
static uint32_t tPelletSpawn = 0;
static uint32_t tGhostSpawn = 0;
static uint32_t tGhostMove = 0;

static void resetGame() {
  pacmanPos = NUM_LEDS / 2;
  pacmanDir = 0;
  for (int i = 0; i < 4; i++) pellets[i] = Pellet{};
  ghost = Ghost{};
  score = 0;
  powerPelletTimer = 0;
  powerMode = false;
  tPelletSpawn = tGhostSpawn = tGhostMove = 0;
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

static void spawnPellet() {
  for (int i = 0; i < 4; i++) {
    if (!pellets[i].active) {
      pellets[i].active = true;
      pellets[i].pos = esp_random() % NUM_LEDS;
      pellets[i].isPowerPellet = (esp_random() % 10) == 0;  // 10% chance
      return;
    }
  }
}

static void spawnGhost() {
  if (ghost.active) return;
  ghost.active = true;
  ghost.pos = (esp_random() & 1) ? 0 : (NUM_LEDS - 1);
  ghost.dir = (ghost.pos == 0) ? 1 : -1;
}

static void updatePacman() {
  if (pacmanDir != 0) {
    pacmanPos += pacmanDir;
    if (pacmanPos < 0) pacmanPos = NUM_LEDS - 1;
    if (pacmanPos >= NUM_LEDS) pacmanPos = 0;
  }
}

static void updateGhost() {
  if (!ghost.active) return;
  
  if (powerMode) {
    // In power mode, ghost runs away
    if (ghost.pos > pacmanPos) ghost.dir = 1;
    else if (ghost.pos < pacmanPos) ghost.dir = -1;
    else ghost.dir = (esp_random() & 1) ? 1 : -1;
  } else {
    // Normal mode: ghost chases pacman
    if (ghost.pos > pacmanPos) ghost.dir = -1;
    else if (ghost.pos < pacmanPos) ghost.dir = 1;
    else ghost.dir = (esp_random() & 1) ? 1 : -1;
  }
  
  ghost.pos += ghost.dir;
  if (ghost.pos < 0) ghost.pos = NUM_LEDS - 1;
  if (ghost.pos >= NUM_LEDS) ghost.pos = 0;
}

static void checkCollisions() {
  // Check pellet collection
  for (int i = 0; i < 4; i++) {
    if (pellets[i].active && pellets[i].pos == pacmanPos) {
      pellets[i].active = false;
      if (pellets[i].isPowerPellet) {
        powerMode = true;
        powerPelletTimer = POWER_PELLET_DURATION_MS;
      }
      score++;
    }
  }
  
  // Check ghost collision
  if (ghost.active && ghost.pos == pacmanPos) {
    if (powerMode) {
      // Eat ghost
      ghost.active = false;
      score += 10;
    } else {
      // Game over
      flashGameOver();
      resetGame();
    }
  }
}

static void render() {
  fadeToBlackBy(leds, NUM_LEDS, 100);
  
  // Render pellets
  for (int i = 0; i < 4; i++) {
    if (pellets[i].active) {
      if (pellets[i].isPowerPellet) {
        leds[pellets[i].pos] = CRGB::White;
      } else {
        leds[pellets[i].pos] = CRGB(64, 64, 0);  // Dim yellow
      }
    }
  }
  
  // Render ghost
  if (ghost.active) {
    if (powerMode) {
      // Blue when vulnerable
      leds[ghost.pos] = CRGB::Blue;
    } else {
      leds[ghost.pos] = CRGB::Red;
    }
  }
  
  // Render pacman
  leds[pacmanPos] = CRGB::Yellow;
  
  // Score indicator on last LED
  uint8_t brightness = (uint8_t)min<uint32_t>(score * 5, 255);
  leds[NUM_LEDS - 1] += CRGB(0, brightness, 0);
}

void game_setup() {
  randomSeed(esp_random());
  resetGame();
  Serial.println("1D Pacman (8 LEDs) on GPIO 16");
  Serial.println("Note: Input controls not yet implemented");
}

void game_loop(uint32_t dt) {
  static uint32_t accum = 0;
  accum += dt;
  
  while (accum >= TICK_MS) {
    accum -= TICK_MS;
    
    tPelletSpawn += TICK_MS;
    tGhostSpawn += TICK_MS;
    tGhostMove += TICK_MS;
    
    if (powerMode) {
      if (powerPelletTimer > TICK_MS) {
        powerPelletTimer -= TICK_MS;
      } else {
        powerMode = false;
        powerPelletTimer = 0;
      }
    }
    
    if (tPelletSpawn >= PELLET_SPAWN_MS) {
      tPelletSpawn = 0;
      spawnPellet();
    }
    
    if (tGhostSpawn >= GHOST_SPAWN_MS) {
      tGhostSpawn = 0;
      spawnGhost();
    }
    
    if (tGhostMove >= GHOST_MOVE_MS) {
      tGhostMove = 0;
      updateGhost();
    }
    
    // TODO: Update pacman based on input
    // For now, auto-move for demo
    if (pacmanDir == 0) {
      pacmanDir = (esp_random() & 1) ? 1 : -1;
    }
    updatePacman();
    
    checkCollisions();
    render();
  }
  
  FastLED.show();
}

