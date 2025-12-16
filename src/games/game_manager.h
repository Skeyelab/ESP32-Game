// Game manager - handles runtime game selection
#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <stdint.h>

// Game function pointer types
typedef void (*GameSetupFunc)();
typedef void (*GameLoopFunc)(uint32_t dt);

// Game information structure
struct GameInfo {
  uint8_t id;
  const char* name;
  GameSetupFunc setup;
  GameLoopFunc loop;
};

// Initialize game manager (loads saved game from EEPROM)
void game_manager_init();

// Set the current game by ID (0-10)
// Returns true if successful, false if invalid game ID
bool game_manager_set_game(uint8_t gameId);

// Get the current game ID
uint8_t game_manager_get_current_game();

// Get the current game name
const char* game_manager_get_current_game_name();

// Get total number of games
uint8_t game_manager_get_game_count();

// Get game info by ID (returns nullptr if invalid)
const GameInfo* game_manager_get_game_info(uint8_t gameId);

// Setup function for current game
void game_manager_setup();

// Loop function for current game
void game_manager_loop(uint32_t dt);

#endif // GAME_MANAGER_H

