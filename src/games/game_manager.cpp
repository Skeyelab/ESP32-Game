// Game manager implementation

#include "game_manager.h"
#include <EEPROM.h>
#include <Arduino.h>

// Forward declarations for all game wrapper functions
extern void game_00_setup();
extern void game_00_loop(uint32_t dt);
extern void game_01_setup();
extern void game_01_loop(uint32_t dt);
extern void game_02_setup();
extern void game_02_loop(uint32_t dt);
extern void game_03_setup();
extern void game_03_loop(uint32_t dt);
extern void game_04_setup();
extern void game_04_loop(uint32_t dt);
extern void game_05_setup();
extern void game_05_loop(uint32_t dt);
extern void game_06_setup();
extern void game_06_loop(uint32_t dt);
extern void game_07_setup();
extern void game_07_loop(uint32_t dt);
extern void game_08_setup();
extern void game_08_loop(uint32_t dt);
extern void game_09_setup();
extern void game_09_loop(uint32_t dt);
extern void game_10_setup();
extern void game_10_loop(uint32_t dt);

// Game registry - all available games
static const GameInfo GAMES[] = {
  {0, "Test", game_00_setup, game_00_loop},
  {1, "Pacman", game_01_setup, game_01_loop},
  {2, "Lava Run", game_02_setup, game_02_loop},
  {3, "Lava Stealth", game_03_setup, game_03_loop},
  {4, "FlappyBird", game_04_setup, game_04_loop},
  {5, "Pong", game_05_setup, game_05_loop},
  {6, "RGB Guardian", game_06_setup, game_06_loop},
  {7, "RGB Guardian 2", game_07_setup, game_07_loop},
  {8, "Pulse Warrior", game_08_setup, game_08_loop},
  {9, "Color Runner X", game_09_setup, game_09_loop},
  {10, "Splatoon", game_10_setup, game_10_loop}
};

static const uint8_t NUM_GAMES = sizeof(GAMES) / sizeof(GAMES[0]);
static uint8_t currentGameId = 0;
static constexpr uint8_t EEPROM_GAME_ID_ADDR = 0;

void game_manager_init() {
  // Initialize EEPROM (1 byte for game ID)
  EEPROM.begin(1);

  // Load saved game ID from EEPROM
  uint8_t savedGameId = EEPROM.read(EEPROM_GAME_ID_ADDR);

  // Validate saved game ID
  if (savedGameId < NUM_GAMES) {
    currentGameId = savedGameId;
  } else {
    // Invalid saved ID, default to game 0
    currentGameId = 0;
    EEPROM.write(EEPROM_GAME_ID_ADDR, 0);
    EEPROM.commit();
  }

  Serial.print("Game manager initialized. Current game: ");
  Serial.print(currentGameId);
  Serial.print(" (");
  Serial.print(GAMES[currentGameId].name);
  Serial.println(")");
}

bool game_manager_set_game(uint8_t gameId) {
  if (gameId >= NUM_GAMES) {
    return false;
  }

  if (currentGameId != gameId) {
    currentGameId = gameId;

    // Save to EEPROM
    EEPROM.write(EEPROM_GAME_ID_ADDR, gameId);
    EEPROM.commit();

    Serial.print("Switched to game: ");
    Serial.print(gameId);
    Serial.print(" (");
    Serial.print(GAMES[gameId].name);
    Serial.println(")");

    // Call setup for the new game
    if (GAMES[gameId].setup) {
      GAMES[gameId].setup();
    }
  }

  return true;
}

uint8_t game_manager_get_current_game() {
  return currentGameId;
}

const char* game_manager_get_current_game_name() {
  if (currentGameId < NUM_GAMES) {
    return GAMES[currentGameId].name;
  }
  return "Unknown";
}

uint8_t game_manager_get_game_count() {
  return NUM_GAMES;
}

const GameInfo* game_manager_get_game_info(uint8_t gameId) {
  if (gameId < NUM_GAMES) {
    return &GAMES[gameId];
  }
  return nullptr;
}

void game_manager_setup() {
  if (currentGameId < NUM_GAMES && GAMES[currentGameId].setup) {
    GAMES[currentGameId].setup();
  }
}

void game_manager_loop(uint32_t dt) {
  if (currentGameId < NUM_GAMES && GAMES[currentGameId].loop) {
    GAMES[currentGameId].loop(dt);
  }
}

