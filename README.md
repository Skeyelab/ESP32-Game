# ESP32-Game

A minimal, deterministic 1D LED game framework for ESP32, featuring **runtime game selection** via web interface. All 11 games are compiled into a single firmware image, and you can switch between them without recompiling!

## Features

- 🎮 **11 Complete Games** - All games available in a single firmware
- 🌐 **Web Interface** - Select games, monitor status, and view LED strip simulation
- 🔄 **Runtime Game Selection** - Switch games without recompiling
- 💾 **EEPROM Persistence** - Selected game persists across power cycles
- 📡 **AP Mode by Default** - Self-hosted WiFi access point (no router needed)
- 🎯 **Touch Controls** - Built-in ESP32 capacitive touch pins (no extra hardware)
- 🧪 **Unit Tests** - Comprehensive test suite (13 test suites, 100+ tests)

## Hardware Requirements

- **MCU**: ESP32 development board (esp32dev target)
- **LED Strip**: WS2812B (NeoPixel-style, GRB color order)
- **LED Data Pin**: GPIO 16
- **LED Count**: Configurable (default: 8)
- **Input**: ESP32 capacitive touch pins (no additional hardware needed!)
- **Power**: Adequate power supply for LED strip (WS2812B can draw significant current)

### Wiring

- Connect LED strip data line to **GPIO 16**
- Connect LED strip power (5V) and ground to appropriate power supply
- Ensure common ground between ESP32 and LED strip
- **Touch Input**: Touch these GPIO pins directly:
  - **Left**: GPIO 4 (Touch0)
  - **Right**: GPIO 15 (Touch3)
  - **Action**: GPIO 13 (Touch4)
  - **Alt**: GPIO 12 (Touch5)

You can touch these pins directly with your finger, or connect wires/foil pads for a more robust setup.

## Quick Start

### Prerequisites

- [PlatformIO](https://platformio.org/) installed (CLI or IDE extension)
- ESP32 development board
- WS2812B LED strip

### Installation

1. **Clone the repository:**
```bash
git clone https://github.com/Skeyelab/ESP32-Game.git
cd ESP32-Game
```

2. **Build and upload:**
```bash
pio run -e esp32dev -t upload
```

3. **Connect to WiFi:**
   - Look for WiFi network: `ESP32-Game`
   - Connect (no password required)
   - Open browser to: `http://192.168.4.1`

4. **Select a game:**
   - Use the web interface to choose from 11 available games
   - Game selection is saved and persists across power cycles

## Games

All 11 games are compiled into the firmware and accessible via the web interface:

1. **Test** - Simple test game to verify touch controls (move LED, change color, flash strip)
2. **1D Pacman** - Control a yellow dot to eat pellets while avoiding faster red ghosts. Power pellets let you temporarily turn the tables.
3. **Lava Run** - Control a white dot to cross unstable lava zones. The lava erupts intermittently, so you must move quickly when it's safe.
4. **Lava Stealth** - Avoid deadly lava and reach the green goal. You have a "stealth mode" that lets you temporarily pass through danger.
5. **FlappyBird** - Control a bird to navigate through gaps in obstacles. Press to "flap" and move up, gravity pulls down.
6. **1D Pong** - Classic Pong game on a 1D LED strip. Control paddle to bounce ball, opponent AI.
7. **RGB Guardian** - Color-matching defense game. Enemies spawn with random colors, match bullet color to enemy color to score.
8. **RGB Guardian 2** - Enhanced version with multiple enemies and improved mechanics.
9. **Pulse Warrior** - Rhythm-based game where you must time actions with pulses. Press at the right moment to score points.
10. **Color Runner X** - Run through colored zones, matching your color to pass through. Change your color to match obstacles.
11. **1D Splatoon** - Paint the LED strip with your color. Cover more area than the opponent to win.

## Web Interface

The ESP32 runs a web server in AP (Access Point) mode by default, creating its own WiFi network.

### Accessing the Dashboard

1. **Connect to WiFi**: Look for network `ESP32-Game` (no password)
2. **Open Browser**: Navigate to `http://192.168.4.1`
3. **Select Game**: Use the game selection interface to switch between games
4. **Monitor Status**: View real-time game status, score, and LED strip simulation

### Features

- **Game Selection**: Switch between all 11 games instantly
- **Real-time Status**: Game name, score, and state (playing/game over/won/paused)
- **LED Strip Simulation**: Visual representation of the physical LED strip
- **Input Status**: Monitor which touch buttons are currently pressed
- **Auto-refresh**: Dashboard updates every 500ms automatically

### API Endpoints

- `GET /` - HTML dashboard
- `GET /status` - JSON status with game info, score, state, input, and LED colors
- `GET /games` - List of all available games with IDs
- `GET /game/current` - Current game ID and name
- `POST /game/select` - Switch game (send `{"gameId": 0}` JSON body)

## Configuration

### LED Configuration

Key settings in `src/main.cpp`:

```cpp
#define LED_PIN     16        // Data pin (locked)
#define NUM_LEDS    8         // Number of LEDs in your strip
#define BRIGHTNESS  10        // Brightness level (0-255)
#define LED_TYPE    WS2812B   // LED chip type
#define COLOR_ORDER GRB       // Color order
```

### Network Configuration

The ESP32 defaults to AP mode. To use WiFi connection instead:

1. Edit `src/config/wifi_config.h`:
```cpp
#define WIFI_SSID "YourWiFiName"
#define WIFI_PASSWORD "YourWiFiPassword"
```

2. Modify `src/main.cpp` to connect to WiFi before starting AP mode (see code comments)

### MQTT Configuration (Optional)

MQTT is disabled in AP mode (no internet connection). To enable:

1. Configure in `src/config/mqtt_config.h`
2. Modify `src/main.cpp` to enable MQTT client

## Architecture

### Game Manager System

All games are managed through a centralized `game_manager` system:

- **Game Registry**: All 11 games registered with unique IDs (0-10)
- **Runtime Selection**: Switch games via web interface or API
- **EEPROM Persistence**: Selected game saved to EEPROM (survives power cycles)
- **Function Pointers**: Each game exposes `game_XX_setup()` and `game_XX_loop()` functions

### Project Structure

```
ESP32-Game/
├── src/
│   ├── main.cpp              # Main entry point
│   ├── input/                # Input abstraction
│   │   ├── touch_input.h
│   │   └── touch_input.cpp
│   ├── games/                # Game implementations
│   │   ├── game_manager.h    # Game manager system
│   │   ├── game_manager.cpp
│   │   ├── game_00_test.cpp
│   │   ├── game_01_pacman.cpp
│   │   └── ... (all 11 games)
│   ├── status/               # Status monitoring
│   │   ├── status_monitor.h
│   │   └── status_monitor.cpp
│   ├── network/              # Network components
│   │   ├── wifi_manager.h/cpp
│   │   ├── web_server.h/cpp
│   │   └── mqtt_client.h/cpp
│   └── config/               # Configuration files
│       ├── wifi_config.h
│       └── mqtt_config.h
├── test/                     # Unit tests
│   ├── test_game_manager/
│   ├── test_touch_input/
│   ├── test_pacman/
│   └── ... (all game tests)
├── platformio.ini            # Build configuration
├── .github/workflows/        # CI/CD
│   └── ci.yml
├── AGENTS.md                 # Development guidelines
└── README.md                 # This file
```

## Testing

This project includes comprehensive unit tests using PlatformIO's Unity testing framework.

### Running Tests

```bash
# Run all tests
pio test -e native

# Run specific test suite
pio test -e native -f test_pacman
```

### Test Coverage

- **13 Test Suites** covering all games and systems:
  - `test_game_manager` - Game manager and runtime selection
  - `test_touch_input` - Touch input system (button states, debouncing)
  - `test_game_logic` - Core game mechanics
  - Individual game tests for all 11 games

- **100+ Tests** covering:
  - Game logic and mechanics
  - Collision detection
  - Score calculations
  - State transitions
  - Input handling
  - Boundary checks

### CI/CD

GitHub Actions automatically:
- Runs all unit tests on every push/PR
- Builds ESP32 firmware
- Uploads firmware as artifact

## Development

### Adding a New Game

1. Create `src/games/game_XX_name.cpp` following the existing pattern
2. Implement:
   - `static void game_setup()` - Initialize game
   - `static void game_loop(uint32_t dt)` - Game update loop
   - `void game_XX_setup()` - Wrapper function (calls game_setup)
   - `void game_XX_loop(uint32_t dt)` - Wrapper function (calls game_loop)
3. Register in `src/games/game_manager.cpp`:
   - Add to `GAMES[]` array with ID, name, and function pointers
4. Add tests in `test/test_XX_name/`

### Coding Guidelines

See [AGENTS.md](AGENTS.md) for detailed development guidelines:

- Use struct over class (unless inheritance needed)
- Avoid dynamic memory allocation
- Prefer compile-time configuration
- Keep deterministic, frame-rate-independent behavior
- Preserve GPIO 16 for LED output
- Use FastLED library for LED control

## Contributing

1. Read [AGENTS.md](AGENTS.md) for guidelines
2. Follow established patterns
3. Add tests for new features
4. Run tests before submitting: `pio test -e native`
5. Ensure builds succeed: `pio run -e esp32dev`

## Philosophy

This project values:
- **Physical feedback** over UI complexity
- **Simple visuals** with tight timing
- **Hackability** over polish
- **Deterministic behavior** for reliable gameplay

If a change makes the system harder to reason about on real hardware, it's probably the wrong change.

## License

[Add your license here]
