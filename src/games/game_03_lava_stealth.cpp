// Lava Stealth
// Control a dot to avoid deadly lava and reach the green goal
// Unlike typical escape games, you have a "stealth mode" that lets you temporarily pass through danger

#include <Arduino.h>
#include <FastLED.h>

extern CRGB leds[];
extern const int NUM_LEDS;

static constexpr uint32_t TICK_MS = 100;
static constexpr uint32_t LAVA_ERUPT_MS = 1000;
static constexpr uint32_t LAVA_COOL_MS = 1500;
static constexpr uint32_t STEALTH_DURATION_MS = 2000;
static constexpr uint32_t STEALTH_COOLDOWN_MS = 5000;
static constexpr uint32_t PLAYER_MOVE_MS = 250;

static int playerPos = 0;
static int targetPos = NUM_LEDS - 1;
static bool lavaActive[NUM_LEDS];
static uint32_t lavaTimers[NUM_LEDS];
static bool stealthMode = false;
static uint32_t stealthTimer = 0;
static uint32_t stealthCooldown = 0;
static uint32_t tLavaCycle = 0;
static uint32_t tPlayerMove = 0;
static bool gameWon = false;
static bool gameOver = false;

static void resetGame() {
  playerPos = 0;
  targetPos = NUM_LEDS - 1;
  for (int i = 0; i < NUM_LEDS; i++) {
    lavaActive[i] = false;
    lavaTimers[i] = 0;
  }
  stealthMode = false;
  stealthTimer = 0;
  stealthCooldown = 0;
  tLavaCycle = 0;
  tPlayerMove = 0;
  gameWon = false;
  gameOver = false;
  FastLED.clear();
  FastLED.show();
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

static void updateLava() {
  for (int i = 1; i < NUM_LEDS - 1; i++) {
    if (lavaTimers[i] > 0) {
      lavaTimers[i] -= TICK_MS;
      if (lavaTimers[i] <= 0) {
        lavaActive[i] = !lavaActive[i];
        if (lavaActive[i]) {
          lavaTimers[i] = LAVA_ERUPT_MS;
        } else {
          lavaTimers[i] = LAVA_COOL_MS;
        }
      }
    } else {
      if ((esp_random() % 3) == 0) {
        lavaActive[i] = true;
        lavaTimers[i] = LAVA_ERUPT_MS;
      } else {
        lavaActive[i] = false;
        lavaTimers[i] = LAVA_COOL_MS;
      }
    }
  }
}

static void updateStealth() {
  if (stealthMode) {
    if (stealthTimer > TICK_MS) {
      stealthTimer -= TICK_MS;
    } else {
      stealthMode = false;
      stealthTimer = 0;
      stealthCooldown = STEALTH_COOLDOWN_MS;
    }
  } else {
    if (stealthCooldown > TICK_MS) {
      stealthCooldown -= TICK_MS;
    }
  }
  
  // TODO: Activate stealth on button press (when cooldown ready)
}

static void updatePlayer() {
  // TODO: Move based on input
  if (playerPos < targetPos) {
    playerPos++;
  }
}

static void checkGameState() {
  if (playerPos == targetPos && !gameWon) {
    gameWon = true;
    flashWin();
    delay(1000);
    resetGame();
  }
  
  if (lavaActive[playerPos] && !stealthMode && !gameOver) {
    gameOver = true;
    flashGameOver();
    delay(1000);
    resetGame();
  }
}

static void render() {
  fadeToBlackBy(leds, NUM_LEDS, 150);
  
  // Render lava zones
  for (int i = 0; i < NUM_LEDS; i++) {
    if (lavaActive[i]) {
      uint8_t intensity = 128 + (sin(millis() / 50.0) * 127);
      leds[i] = CRGB(intensity, intensity / 4, 0);
    }
  }
  
  // Render start (green)
  leds[0] = CRGB::Green;
  
  // Render end (blue)
  leds[targetPos] = CRGB::Blue;
  
  // Render player
  if (stealthMode) {
    // Cyan when in stealth
    leds[playerPos] = CRGB::Cyan;
  } else {
    leds[playerPos] = CRGB::White;
  }
  
  // Stealth cooldown indicator on first LED
  if (stealthCooldown > 0) {
    uint8_t cd = (uint8_t)((stealthCooldown * 255) / STEALTH_COOLDOWN_MS);
    leds[0] += CRGB(0, 0, cd);
  }
}

void game_setup() {
  randomSeed(esp_random());
  resetGame();
  Serial.println("Lava Stealth (8 LEDs) on GPIO 16");
  Serial.println("Note: Input controls not yet implemented");
}

void game_loop(uint32_t dt) {
  if (gameWon || gameOver) return;
  
  static uint32_t accum = 0;
  accum += dt;
  
  while (accum >= TICK_MS) {
    accum -= TICK_MS;
    
    tLavaCycle += TICK_MS;
    tPlayerMove += TICK_MS;
    
    updateLava();
    updateStealth();
    
    if (tPlayerMove >= PLAYER_MOVE_MS) {
      tPlayerMove = 0;
      updatePlayer();
    }
    
    checkGameState();
    render();
  }
  
  FastLED.show();
}

