#include <Arduino.h>
#include <FastLED.h>

#define LED_PIN     16
#define NUM_LEDS    8
#define BRIGHTNESS  10
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

// Game selection: uncomment the game you want to compile
// #define GAME_01_PACMAN
// #define GAME_02_LAVA_RUN
// #define GAME_03_LAVA_STEALTH
// #define GAME_04_FLAPPY
// #define GAME_05_PONG
#define GAME_06_RGB_GUARDIAN
// #define GAME_07_RGB_GUARDIAN2
// #define GAME_08_PULSE_WARRIOR
// #define GAME_09_COLOR_RUNNER
// #define GAME_10_SPLATOON

CRGB leds[NUM_LEDS];

// Forward declarations
void game_setup();
void game_loop(uint32_t dt);

void setup() {
  Serial.begin(115200);
  delay(200);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  game_setup();
}

void loop() {
  static uint32_t last = millis();
  uint32_t now = millis();
  uint32_t dt = now - last;
  last = now;

  game_loop(dt);
}

// Include selected game
#ifdef GAME_01_PACMAN
  #include "games/game_01_pacman.cpp"
#elif defined(GAME_02_LAVA_RUN)
  #include "games/game_02_lava_run.cpp"
#elif defined(GAME_03_LAVA_STEALTH)
  #include "games/game_03_lava_stealth.cpp"
#elif defined(GAME_04_FLAPPY)
  #include "games/game_04_flappy.cpp"
#elif defined(GAME_05_PONG)
  #include "games/game_05_pong.cpp"
#elif defined(GAME_06_RGB_GUARDIAN)
  #include "games/game_06_rgb_guardian.cpp"
#elif defined(GAME_07_RGB_GUARDIAN2)
  #include "games/game_07_rgb_guardian2.cpp"
#elif defined(GAME_08_PULSE_WARRIOR)
  #include "games/game_08_pulse_warrior.cpp"
#elif defined(GAME_09_COLOR_RUNNER)
  #include "games/game_09_color_runner.cpp"
#elif defined(GAME_10_SPLATOON)
  #include "games/game_10_splatoon.cpp"
#else
  #error "No game selected! Uncomment one of the GAME_XX defines at the top of main.cpp"
#endif
