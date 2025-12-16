AGENTS.md

Project Overview

This project is an ESP32-based 1D LED game framework built with PlatformIO and the Arduino framework. It targets WS2812B (NeoPixel-style) LED strips and is designed to support multiple simple arcade-style games (Pong, Flappy-style, Pacman-style, etc.) rendered on a single-dimensional LED array.

The codebase is intentionally minimal, deterministic, and hardware-focused. The primary goals are:
	•	Reliable LED timing on ESP32
	•	Clear separation between game logic and rendering
	•	Easy extension with new games and input methods

This file documents how humans and AI agents should work within this repository.

⸻

Hardware Assumptions
	•	MCU: ESP32 (classic esp32dev target)
	•	LED Strip: WS2812B (GRB)
	•	LED Data Pin: GPIO 16 (locked unless explicitly changed)
	•	LED Count: Configurable via NUM_LEDS
	•	Inputs: None initially (buttons / encoders may be added later)

All code changes must respect these defaults unless the user explicitly requests otherwise.

⸻

Build System
	•	Tooling: PlatformIO
	•	Framework: Arduino
	•	Primary Config: platformio.ini
	•	Libraries:
	•	FastLED (preferred over Adafruit_NeoPixel for ESP32 timing)

Agents should:
	•	Avoid adding unnecessary dependencies
	•	Prefer compile-time configuration (#define) over runtime allocation
	•	Keep memory usage predictable

⸻

Code Structure & Conventions

Files
	•	src/main.cpp is the authoritative entry point
	•	New files may be added later (e.g. games/, input/) but should remain optional

Style Guidelines
	•	Use plain C++, no STL containers unless clearly justified
	•	Favor struct over class unless inheritance is required
	•	Avoid dynamic memory allocation (new, malloc)
	•	Prefer explicit state machines over implicit behavior

Timing Model
	•	Games should run on a fixed timestep using millis()
	•	Rendering and updates should be logically separated

Example pattern:

update(dt);
render();
FastLED.show();

Agents must preserve deterministic behavior and avoid frame-rate-dependent logic.

⸻

Game Architecture (Planned)

Games are expected to follow a simple interface:

struct Game {
  virtual const char* name();
  virtual void reset();
  virtual void update(uint32_t dtMs);
  virtual void render();
};

Agents adding games should:
	•	Keep each game self-contained
	•	Avoid cross-game global state
	•	Use the shared LED framebuffer (CRGB leds[])

⸻

What Agents SHOULD Do
	•	Add small, testable features incrementally
	•	Keep examples runnable on first flash
	•	Prefer clarity over cleverness
	•	Ask before introducing hardware assumptions (buttons, sound, WiFi, BLE)

⸻

What Agents SHOULD NOT Do
	•	Do not introduce networking unless explicitly requested
	•	Do not refactor working code without a clear reason
	•	Do not assume a display, filesystem, or RTOS usage

⸻

Extension Roadmap (Informational)

These are expected future steps but not yet implemented:
	•	Button / encoder input abstraction
	•	Multiple selectable games
	•	Simple menu rendered on LEDs
	•	Sound output (PWM or I2S)
	•	ESP32-S3 compatibility

Agents may reference this roadmap but should not implement it without instruction.

⸻

Philosophy

This project values:
	•	Physical feedback over UI complexity
	•	Simple visuals with tight timing
	•	Hackability over polish

If a change makes the system harder to reason about on real hardware, it is probably the wrong change.

⸻

Contact / Authority

The project owner provides final direction. When in doubt:
	•	Preserve GPIO 16 for LED output
	•	Preserve FastLED usage
	•	Preserve deterministic behavior


