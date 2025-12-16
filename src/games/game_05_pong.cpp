// 1D Pong
// Classic Pong game on a 1D LED strip
// Control paddle to bounce ball, opponent AI

#include <Arduino.h>
#include <FastLED.h>
#include "../input/touch_input.h"
#ifdef ENABLE_NETWORKING
#include "../status/status_monitor.h"
#endif

extern CRGB leds[];
#define NUM_LEDS 8  // Must match main.cpp

static constexpr uint32_t TICK_MS = 50;
static constexpr uint32_t BALL_MOVE_MS = 100;
static constexpr uint32_t AI_MOVE_MS = 150;

static int playerPaddle = 0;
static int aiPaddle = NUM_LEDS - 1;
static int ballPos = NUM_LEDS / 2;
static int ballDir = 1;  // +1 right, -1 left
static uint32_t playerScore = 0;
static uint32_t aiScore = 0;
static uint32_t tBallMove = 0;
static uint32_t tAiMove = 0;

static void resetGame() {
  playerPaddle = 0;
  aiPaddle = NUM_LEDS - 1;
  ballPos = NUM_LEDS / 2;
  ballDir = (esp_random() & 1) ? 1 : -1;
  tBallMove = tAiMove = 0;
  FastLED.clear();
  FastLED.show();
}

static void updateBall() {
  ballPos += ballDir;

  // Collision with player paddle
  if (ballPos == playerPaddle && ballDir < 0) {
    ballDir = 1;  // Bounce right
    ballPos = playerPaddle + 1;
  }

  // Collision with AI paddle
  if (ballPos == aiPaddle && ballDir > 0) {
    ballDir = -1;  // Bounce left
    ballPos = aiPaddle - 1;
  }

  // Score points
  if (ballPos < 0) {
    aiScore++;
#ifdef ENABLE_NETWORKING
    status_monitor_update_score(playerScore);  // Report player score
#endif
    resetGame();
    return;
  }
  if (ballPos >= NUM_LEDS) {
    playerScore++;
#ifdef ENABLE_NETWORKING
    status_monitor_update_score(playerScore);
#endif
    resetGame();
    return;
  }
}

static void updateAI() {
  // Simple AI: move toward ball
  if (ballPos > aiPaddle && aiPaddle > 0) {
    aiPaddle--;
  } else if (ballPos < aiPaddle && aiPaddle < NUM_LEDS - 1) {
    aiPaddle++;
  }
}

static void render() {
  fadeToBlackBy(leds, NUM_LEDS, 200);

  // Render player paddle (green)
  leds[playerPaddle] = CRGB::Green;

  // Render AI paddle (red)
  leds[aiPaddle] = CRGB::Red;

  // Render ball (white)
  if (ballPos >= 0 && ballPos < NUM_LEDS) {
    leds[ballPos] = CRGB::White;
  }

  // Score indicators
  // Player score on left side
  if (playerScore > 0) {
    uint8_t pBright = (uint8_t)min<uint32_t>(playerScore * 30, 255);
    leds[0] += CRGB(0, pBright, 0);
  }

  // AI score on right side
  if (aiScore > 0) {
    uint8_t aBright = (uint8_t)min<uint32_t>(aiScore * 30, 255);
    leds[NUM_LEDS - 1] += CRGB(aBright, 0, 0);
  }
}

static void game_setup() {
  randomSeed(esp_random());
  resetGame();
  Serial.println("1D Pong (8 LEDs) on GPIO 16");
  Serial.println("Left touch: move paddle left, Right touch: move paddle right");
#ifdef ENABLE_NETWORKING
  status_monitor_update_game_name("Pong");
  status_monitor_update_state(GAME_STATE_PLAYING);
  status_monitor_update_score(0);
#endif
}

static void game_loop(uint32_t dt) {
  static uint32_t accum = 0;
  accum += dt;

  while (accum >= TICK_MS) {
    accum -= TICK_MS;

    tBallMove += TICK_MS;
    tAiMove += TICK_MS;

    if (tBallMove >= BALL_MOVE_MS) {
      tBallMove = 0;
      updateBall();
    }

    if (tAiMove >= AI_MOVE_MS) {
      tAiMove = 0;
      updateAI();
    }

    // Update player paddle based on input
    if (touch_left_just_pressed() && playerPaddle > 0) {
      playerPaddle--;
    } else if (touch_right_just_pressed() && playerPaddle < NUM_LEDS - 1) {
      playerPaddle++;
    }

    render();
  }

  FastLED.show();
}







// Wrapper functions for game manager
void game_05_setup() {
  game_setup();
}

void game_05_loop(uint32_t dt) {
  game_loop(dt);
}
