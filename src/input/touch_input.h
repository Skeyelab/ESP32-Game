// Touch input abstraction for ESP32
// Uses capacitive touch pins as buttons

#ifndef TOUCH_INPUT_H
#define TOUCH_INPUT_H

#include <Arduino.h>

// Touch pin configuration
// ESP32 touch pins: 0, 2, 4, 12, 13, 14, 15, 27, 32, 33
// Using pins that don't conflict with common uses
#define TOUCH_PIN_LEFT   4   // Touch0
#define TOUCH_PIN_RIGHT  15  // Touch3
#define TOUCH_PIN_ACTION 13  // Touch4
#define TOUCH_PIN_ALT    12  // Touch5

// Touch threshold (lower = more sensitive)
// Typical values: 20-80, adjust based on your setup
#define TOUCH_THRESHOLD  40

// Debounce time in milliseconds
#define TOUCH_DEBOUNCE_MS 50

// Button states
struct ButtonState {
  bool pressed;
  bool justPressed;
  bool justReleased;
};

// Input state
struct InputState {
  ButtonState left;
  ButtonState right;
  ButtonState action;
  ButtonState alt;
};

// Initialize touch input
void touch_input_init();

// Update input state (call in loop)
void touch_input_update();

// Get current input state
InputState touch_input_get();

// Helper functions for individual buttons
bool touch_left_pressed();
bool touch_left_just_pressed();
bool touch_right_pressed();
bool touch_right_just_pressed();
bool touch_action_pressed();
bool touch_action_just_pressed();
bool touch_alt_pressed();
bool touch_alt_just_pressed();

#endif // TOUCH_INPUT_H

