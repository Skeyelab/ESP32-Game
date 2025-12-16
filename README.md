# ESP32-Game

A minimal, deterministic 1D LED game framework for ESP32, designed to support multiple simple arcade-style games (Pong, Flappy-style, Pacman-style, etc.) rendered on a single-dimensional WS2812B LED strip.

## Overview

This project provides a hardware-focused framework for building games on a one-dimensional LED array. The codebase prioritizes:

- Reliable LED timing on ESP32
- Clear separation between game logic and rendering
- Easy extension with new games and input methods
- Deterministic, frame-rate-independent behavior

## Hardware Requirements

- **MCU**: ESP32 development board (esp32dev target)
- **LED Strip**: WS2812B (NeoPixel-style, GRB color order)
- **LED Data Pin**: GPIO 16 (locked unless explicitly changed)
- **LED Count**: Configurable via `NUM_LEDS` define
- **Power**: Ensure adequate power supply for your LED strip (WS2812B LEDs can draw significant current at full brightness)

### Wiring

- Connect the LED strip data line to GPIO 16
- Connect LED strip power (5V) and ground to appropriate power supply
- Ensure common ground between ESP32 and LED strip

## Quick Start

### Prerequisites

- [PlatformIO](https://platformio.org/) installed (CLI or IDE extension)
- ESP32 development board
- WS2812B LED strip

### Installation

1. Clone this repository:
```bash
git clone <repository-url>
cd ESP32-Game
```

2. Open the project in PlatformIO (or use PlatformIO CLI)

3. Build and upload:
```bash
pio run -t upload
```

4. Monitor serial output (optional):
```bash
pio device monitor
```

### Expected Behavior

The game "RGB Defender" will start automatically. You should see:
- Serial output: "RGB Defender (8 LEDs) on GPIO 16"
- A defender position (dim white LED) at position 3
- Colored enemies (red, green, or blue) spawning from either end and moving toward the defender
- Auto-firing bullets matching the current weapon color
- Score displayed as blue brightness on the last LED
- Game over flash (red) when an enemy reaches the defender

## Configuration

Key configuration options are defined in `src/main.cpp`:

- `NUM_LEDS`: Number of LEDs in your strip (default: 8)
- `BRIGHTNESS`: LED brightness level, 0-255 (default: 64)
- `LED_PIN`: Data pin for LED strip (default: 16, locked)
- `LED_TYPE`: LED chip type (default: WS2812B)
- `COLOR_ORDER`: Color order (default: GRB)

Adjust these values to match your hardware setup before building.

### Game-Specific Configuration

Additional game tuning constants in `src/main.cpp`:

- `DEF_POS`: Defender position (default: 3)
- `TICK_MS`: Fixed timestep interval (default: 30ms)
- `SPAWN_EVERY_MS`: Enemy spawn interval (default: 900ms)
- `ENEMY_STEP_EVERY_MS`: Enemy movement speed (default: 260ms)
- `BULLET_STEP_EVERY_MS`: Bullet movement speed (default: 130ms)
- `FIRE_EVERY_MS`: Auto-fire interval (default: 260ms)
- `WEAPON_CYCLE_MS`: Weapon color cycle time (default: 600ms)

## Gameplay

**RGB Defender** is a color-matching defense game:

- **Defender**: Fixed position (LED 3) with dim white light tinted by current weapon color
- **Enemies**: Spawn from either end with random colors (red, green, or blue), moving toward the defender
- **Bullets**: Auto-fire toward the nearest enemy using the current weapon color
- **Scoring**: Match bullet color to enemy color to destroy the enemy and score points
- **Weapon**: Cycles through red → green → blue automatically
- **Game Over**: If an enemy reaches the defender position, the game resets with a red flash

The score is displayed as blue brightness on the last LED (LED 7).

## Current Status

The project currently includes **10 complete games**, each as a separate firmware:

1. **1D Pacman** - Control a yellow dot to eat pellets while avoiding faster red ghosts. Power pellets let you temporarily turn the tables.
2. **Lava Run** - Control a white dot to cross unstable lava zones. The lava erupts intermittently, so you must move quickly when it's safe.
3. **Lava Stealth** - Avoid deadly lava and reach the green goal. You have a "stealth mode" that lets you temporarily pass through danger.
4. **FlappyBird** - Control a bird to navigate through gaps in obstacles. Press to "flap" and move up, gravity pulls down.
5. **1D Pong** - Classic Pong game on a 1D LED strip. Control paddle to bounce ball, opponent AI.
6. **RGB Guardian** - Color-matching defense game. Enemies spawn with random colors, match bullet color to enemy color to score.
7. **RGB Guardian 2** - Enhanced version with multiple enemies and improved mechanics.
8. **Pulse Warrior** - Rhythm-based game where you must time actions with pulses. Press at the right moment to score points.
9. **Color Runner X** - Run through colored zones, matching your color to pass through. Change your color to match obstacles.
10. **1D Splatoon** - Paint the LED strip with your color. Cover more area than the opponent to win.

All games use fixed timestep game loops for deterministic behavior.

## Project Goals & Architecture

This framework is designed to support multiple games following a simple interface:

```cpp
struct Game {
  virtual const char* name();
  virtual void reset();
  virtual void update(uint32_t dtMs);
  virtual void render();
};
```

### Planned Features

- Button / encoder input abstraction
- Multiple selectable games
- Simple menu rendered on LEDs
- Sound output (PWM or I2S)
- ESP32-S3 compatibility

See [AGENTS.md](AGENTS.md) for detailed development guidelines and architecture decisions.

## Testing

This project includes unit tests using PlatformIO's Unity testing framework. Tests focus on game logic without hardware dependencies.

### Running Tests

Run all tests:
```bash
pio test -e native
```

Run a specific test:
```bash
pio test -e native -f test_game_logic
```

### Test Structure

- `test/test_game_logic/` - Core game mechanics (collisions, boundaries, timers, movement)
- `test/test_pacman/` - Pacman-specific logic (pellets, ghosts, power mode)
- `test/test_lava_run/` - Lava Run game logic (lava zones, movement, win conditions)
- `test/test_lava_stealth/` - Lava Stealth game logic (stealth mode, cooldown, passing through danger)
- `test/test_flappy/` - FlappyBird game logic (obstacles, gravity, collisions)
- `test/test_pong/` - Pong game logic (ball bouncing, scoring, AI movement)
- `test/test_rgb_guardian/` - RGB Guardian game logic (color matching, enemy/bullet collisions, weapon cycling)
- `test/test_rgb_guardian2/` - RGB Guardian 2 game logic (multiple enemies/bullets, nearest enemy targeting)
- `test/test_pulse_warrior/` - Pulse Warrior game logic (pulse timing, target hitting, combo system)
- `test/test_color_runner/` - Color Runner X game logic (color matching zones, passing through correct colors)
- `test/test_splatoon/` - Splatoon game logic (painting, scoring, win conditions)

**Total: 96 tests covering all 10 games**

Tests are run on the native platform (no hardware required) and validate game logic, collision detection, score calculations, and state transitions.

## Project Structure

```
ESP32-Game/
├── src/
│   ├── main.cpp          # Main entry point with game selection
│   └── games/            # Individual game implementations
│       ├── game_01_pacman.cpp
│       ├── game_02_lava_run.cpp
│       ├── game_03_lava_stealth.cpp
│       ├── game_04_flappy.cpp
│       ├── game_05_pong.cpp
│       ├── game_06_rgb_guardian.cpp
│       ├── game_07_rgb_guardian2.cpp
│       ├── game_08_pulse_warrior.cpp
│       ├── game_09_color_runner.cpp
│       └── game_10_splatoon.cpp
├── test/                 # Unit tests
│   ├── test_game_logic/
│   ├── test_pacman/
│   ├── test_lava_run/
│   ├── test_flappy/
│   ├── test_pong/
│   └── test_splatoon/
├── platformio.ini        # PlatformIO build configuration
├── AGENTS.md             # Development guidelines and architecture docs
└── README.md             # This file
```

## Contributing

This project follows specific coding conventions and architectural principles. Before contributing:

1. Read [AGENTS.md](AGENTS.md) for detailed guidelines
2. Follow the established patterns (struct over class, no dynamic allocation, etc.)
3. Keep changes minimal and testable
4. Preserve deterministic behavior and GPIO 16 for LED output
5. Add tests for new game logic when appropriate
6. Run tests before submitting: `pio test -e native`

## Philosophy

This project values:
- Physical feedback over UI complexity
- Simple visuals with tight timing
- Hackability over polish

If a change makes the system harder to reason about on real hardware, it is probably the wrong change.

## License

[Add your license here]

