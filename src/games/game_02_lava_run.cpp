// Lava Run
// Control a white dot to cross unstable lava zones
// The lava erupts intermittently, so you must move quickly when it's safe

#include <Arduino.h>
#include <FastLED.h>

extern CRGB leds[];
extern const int NUM_LEDS;

static constexpr uint32_t TICK_MS = 100;
static constexpr uint32_t LAVA_ERUPT_MS = 800;
static constexpr uint32_t LAVA_COOL_MS = 1200;
static constexpr uint32_t PLAYER_MOVE_MS = 200;

static int playerPos = 0;
static int targetPos = NUM_LEDS - 1;
static bool lavaActive[NUM_LEDS];
static uint32_t lavaTimers[NUM_LEDS];
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
  // Randomly activate/deactivate lava zones
  for (int i = 1; i < NUM_LEDS - 1; i++) {  // Not on start/end
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
      // Initialize
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

static void updatePlayer() {
  // TODO: Move based on input
  // For demo: auto-advance when safe
  if (playerPos < targetPos && !lavaActive[playerPos + 1]) {
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
  
  if (lavaActive[playerPos] && !gameOver) {
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
      // Pulsing red/orange
      uint8_t intensity = 128 + (sin(millis() / 50.0) * 127);
      leds[i] = CRGB(intensity, intensity / 4, 0);
    }
  }
  
  // Render start (green)
  leds[0] = CRGB::Green;
  
  // Render end (blue)
  leds[targetPos] = CRGB::Blue;
  
  // Render player (white)
  leds[playerPos] = CRGB::White;
}

void game_setup() {
  randomSeed(esp_random());
  resetGame();
  Serial.println("Lava Run (8 LEDs) on GPIO 16");
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
    
    if (tPlayerMove >= PLAYER_MOVE_MS) {
      tPlayerMove = 0;
      updatePlayer();
    }
    
    checkGameState();
    render();
  }
  
  FastLED.show();
}

