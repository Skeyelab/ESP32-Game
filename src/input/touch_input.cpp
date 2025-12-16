// Touch input implementation

#include "touch_input.h"

static InputState inputState;
static uint32_t lastUpdate = 0;

// Previous states for edge detection
static bool prevLeft = false;
static bool prevRight = false;
static bool prevAction = false;
static bool prevAlt = false;

// Last press times for debouncing
static uint32_t lastLeftPress = 0;
static uint32_t lastRightPress = 0;
static uint32_t lastActionPress = 0;
static uint32_t lastAltPress = 0;

// Read touch pin with threshold
static bool readTouchPin(int pin, uint32_t threshold) {
  int touchValue = touchRead(pin);
  return touchValue < threshold;
}

void touch_input_init() {
  // Touch pins don't need pinMode, but we can set them up
  // touchRead() works directly on touch-capable pins

  // Initialize state
  inputState.left = {false, false, false};
  inputState.right = {false, false, false};
  inputState.action = {false, false, false};
  inputState.alt = {false, false, false};

  prevLeft = prevRight = prevAction = prevAlt = false;
  lastLeftPress = lastRightPress = lastActionPress = lastAltPress = 0;
  lastUpdate = millis();

  Serial.println("Touch input initialized");
  Serial.print("Left: GPIO "); Serial.println(TOUCH_PIN_LEFT);
  Serial.print("Right: GPIO "); Serial.println(TOUCH_PIN_RIGHT);
  Serial.print("Action: GPIO "); Serial.println(TOUCH_PIN_ACTION);
  Serial.print("Alt: GPIO "); Serial.println(TOUCH_PIN_ALT);
}

void touch_input_update() {
  uint32_t now = millis();

  // Read touch pins with debouncing
  bool leftPressed = false;
  bool rightPressed = false;
  bool actionPressed = false;
  bool altPressed = false;

  if (readTouchPin(TOUCH_PIN_LEFT, TOUCH_THRESHOLD)) {
    if (now - lastLeftPress > TOUCH_DEBOUNCE_MS) {
      leftPressed = true;
      lastLeftPress = now;
    }
  }

  if (readTouchPin(TOUCH_PIN_RIGHT, TOUCH_THRESHOLD)) {
    if (now - lastRightPress > TOUCH_DEBOUNCE_MS) {
      rightPressed = true;
      lastRightPress = now;
    }
  }

  if (readTouchPin(TOUCH_PIN_ACTION, TOUCH_THRESHOLD)) {
    if (now - lastActionPress > TOUCH_DEBOUNCE_MS) {
      actionPressed = true;
      lastActionPress = now;
    }
  }

  if (readTouchPin(TOUCH_PIN_ALT, TOUCH_THRESHOLD)) {
    if (now - lastAltPress > TOUCH_DEBOUNCE_MS) {
      altPressed = true;
      lastAltPress = now;
    }
  }

  // Update left button state
  inputState.left.pressed = leftPressed;
  inputState.left.justPressed = leftPressed && !prevLeft;
  inputState.left.justReleased = !leftPressed && prevLeft;
  prevLeft = leftPressed;

  // Update right button state
  inputState.right.pressed = rightPressed;
  inputState.right.justPressed = rightPressed && !prevRight;
  inputState.right.justReleased = !rightPressed && prevRight;
  prevRight = rightPressed;

  // Update action button state
  inputState.action.pressed = actionPressed;
  inputState.action.justPressed = actionPressed && !prevAction;
  inputState.action.justReleased = !actionPressed && prevAction;
  prevAction = actionPressed;

  // Update alt button state
  inputState.alt.pressed = altPressed;
  inputState.alt.justPressed = altPressed && !prevAlt;
  inputState.alt.justReleased = !altPressed && prevAlt;
  prevAlt = altPressed;

  lastUpdate = now;
}

InputState touch_input_get() {
  return inputState;
}

bool touch_left_pressed() {
  return inputState.left.pressed;
}

bool touch_left_just_pressed() {
  return inputState.left.justPressed;
}

bool touch_right_pressed() {
  return inputState.right.pressed;
}

bool touch_right_just_pressed() {
  return inputState.right.justPressed;
}

bool touch_action_pressed() {
  return inputState.action.pressed;
}

bool touch_action_just_pressed() {
  return inputState.action.justPressed;
}

bool touch_alt_pressed() {
  return inputState.alt.pressed;
}

bool touch_alt_just_pressed() {
  return inputState.alt.justPressed;
}

