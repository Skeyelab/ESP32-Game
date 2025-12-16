// 1D Splatoon
// Paint the LED strip with your color
// Cover more area than the opponent to win

#include <Arduino.h>
#include <FastLED.h>

extern CRGB leds[];
extern const int NUM_LEDS;

static constexpr uint32_t TICK_MS = 100;
static constexpr uint32_t PAINT_MOVE_MS = 150;
static constexpr uint32_t GAME_DURATION_MS = 30000;  // 30 seconds

static int playerPos = 0;
static int opponentPos = NUM_LEDS - 1;
static uint8_t playerPaint[NUM_LEDS];  // 0 = neutral, 1 = player, 2 = opponent
static uint32_t playerScore = 0;
static uint32_t opponentScore = 0;
static uint32_t gameTimer = 0;
static bool gameOver = false;

static void resetGame() {
  playerPos = 0;
  opponentPos = NUM_LEDS - 1;
  for (int i = 0; i < NUM_LEDS; i++) {
    playerPaint[i] = 0;  // Neutral
  }
  playerScore = 0;
  opponentScore = 0;
  gameTimer = 0;
  gameOver = false;
  FastLED.clear();
  FastLED.show();
}

static void updatePaint() {
  // Player paints
  if (playerPos >= 0 && playerPos < NUM_LEDS) {
    if (playerPaint[playerPos] != 1) {
      if (playerPaint[playerPos] == 2) {
        opponentScore--;  // Remove opponent paint
      }
      playerPaint[playerPos] = 1;
      playerScore++;
    }
  }

  // Opponent paints (AI)
  if (opponentPos >= 0 && opponentPos < NUM_LEDS) {
    if (playerPaint[opponentPos] != 2) {
      if (playerPaint[opponentPos] == 1) {
        playerScore--;  // Remove player paint
      }
      playerPaint[opponentPos] = 2;
      opponentScore++;
    }
  }
}

static void updatePlayer() {
  // TODO: Move based on input
  // Auto-move for demo
  playerPos = (playerPos + 1) % NUM_LEDS;
}

static void updateOpponent() {
  // Simple AI: move toward areas with less opponent paint
  int bestPos = opponentPos;
  int minPaint = playerPaint[opponentPos];

  // Check nearby positions
  for (int i = -1; i <= 1; i++) {
    int checkPos = opponentPos + i;
    if (checkPos < 0 || checkPos >= NUM_LEDS) continue;

    if (playerPaint[checkPos] < minPaint ||
        (playerPaint[checkPos] == minPaint && (esp_random() & 1))) {
      bestPos = checkPos;
      minPaint = playerPaint[checkPos];
    }
  }

  opponentPos = bestPos;
}

static void checkGameOver() {
  if (gameTimer >= GAME_DURATION_MS) {
    gameOver = true;

    // Determine winner
    if (playerScore > opponentScore) {
      // Player wins - flash green
      for (int i = 0; i < 5; i++) {
        fill_solid(leds, NUM_LEDS, CRGB::Green);
        FastLED.show();
        delay(200);
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        FastLED.show();
        delay(200);
      }
    } else if (opponentScore > playerScore) {
      // Opponent wins - flash red
      for (int i = 0; i < 5; i++) {
        fill_solid(leds, NUM_LEDS, CRGB::Red);
        FastLED.show();
        delay(200);
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        FastLED.show();
        delay(200);
      }
    } else {
      // Tie - flash yellow
      for (int i = 0; i < 5; i++) {
        fill_solid(leds, NUM_LEDS, CRGB::Yellow);
        FastLED.show();
        delay(200);
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        FastLED.show();
        delay(200);
      }
    }

    delay(1000);
    resetGame();
  }
}

static void render() {
  fadeToBlackBy(leds, NUM_LEDS, 50);  // Less fade to maintain paint

  // Render painted areas
  for (int i = 0; i < NUM_LEDS; i++) {
    if (playerPaint[i] == 1) {
      // Player paint (green)
      leds[i] = CRGB(0, 128, 0);
    } else if (playerPaint[i] == 2) {
      // Opponent paint (red)
      leds[i] = CRGB(128, 0, 0);
    } else {
      // Neutral (dim white)
      leds[i] = CRGB(10, 10, 10);
    }
  }

  // Render player (bright green)
  leds[playerPos] = CRGB::Green;

  // Render opponent (bright red)
  leds[opponentPos] = CRGB::Red;

  // Score indicator - player on left, opponent on right
  uint8_t pBright = (uint8_t)min<uint32_t>((playerScore * 255) / NUM_LEDS, 255);
  uint8_t oBright = (uint8_t)min<uint32_t>((opponentScore * 255) / NUM_LEDS, 255);
  leds[0] += CRGB(0, pBright, 0);
  leds[NUM_LEDS - 1] += CRGB(oBright, 0, 0);
}

void game_setup() {
  randomSeed(esp_random());
  resetGame();
  Serial.println("1D Splatoon (8 LEDs) on GPIO 16");
  Serial.println("Note: Input controls not yet implemented");
}

void game_loop(uint32_t dt) {
  if (gameOver) return;

  static uint32_t accum = 0;
  accum += dt;

  gameTimer += dt;

  while (accum >= TICK_MS) {
    accum -= TICK_MS;

    updatePlayer();
    updateOpponent();
    updatePaint();
    checkGameOver();
    render();
  }

  FastLED.show();
}

