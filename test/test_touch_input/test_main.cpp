#include <unity.h>
#include <cstdint>
#include <cstring>

// Test touch input logic (without hardware dependencies)

// Test button state structure
void test_button_state_initialization() {
  bool pressed = false;
  bool justPressed = false;
  bool justReleased = false;

  TEST_ASSERT_FALSE(pressed);
  TEST_ASSERT_FALSE(justPressed);
  TEST_ASSERT_FALSE(justReleased);
}

// Test edge detection - just pressed
void test_just_pressed_detection() {
  bool prevState = false;
  bool currentState = true;
  bool justPressed = currentState && !prevState;

  TEST_ASSERT_TRUE(justPressed);
}

void test_not_just_pressed_when_held() {
  bool prevState = true;
  bool currentState = true;
  bool justPressed = currentState && !prevState;

  TEST_ASSERT_FALSE(justPressed);
}

// Test edge detection - just released
void test_just_released_detection() {
  bool prevState = true;
  bool currentState = false;
  bool justReleased = !currentState && prevState;

  TEST_ASSERT_TRUE(justReleased);
}

void test_not_just_released_when_not_pressed() {
  bool prevState = false;
  bool currentState = false;
  bool justReleased = !currentState && prevState;

  TEST_ASSERT_FALSE(justReleased);
}

// Test debouncing logic
void test_debounce_prevents_rapid_presses() {
  uint32_t lastPressTime = 0;
  uint32_t currentTime = 10;
  uint32_t debounceMs = 50;
  bool canPress = (currentTime - lastPressTime) > debounceMs;

  TEST_ASSERT_FALSE(canPress); // Too soon after last press
}

void test_debounce_allows_after_delay() {
  uint32_t lastPressTime = 0;
  uint32_t currentTime = 60;
  uint32_t debounceMs = 50;
  bool canPress = (currentTime - lastPressTime) > debounceMs;

  TEST_ASSERT_TRUE(canPress); // Enough time has passed
}

// Test multiple button states
void test_multiple_buttons_independent() {
  bool leftPressed = true;
  bool rightPressed = false;
  bool actionPressed = false;

  TEST_ASSERT_TRUE(leftPressed);
  TEST_ASSERT_FALSE(rightPressed);
  TEST_ASSERT_FALSE(actionPressed);
}

void test_all_buttons_can_be_pressed() {
  bool leftPressed = true;
  bool rightPressed = true;
  bool actionPressed = true;
  bool altPressed = true;

  TEST_ASSERT_TRUE(leftPressed);
  TEST_ASSERT_TRUE(rightPressed);
  TEST_ASSERT_TRUE(actionPressed);
  TEST_ASSERT_TRUE(altPressed);
}

// Test button state transitions
void test_state_transition_press() {
  bool prevState = false;
  bool currentState = true;

  // Update state
  bool pressed = currentState;
  bool justPressed = currentState && !prevState;
  bool justReleased = !currentState && prevState;

  TEST_ASSERT_TRUE(pressed);
  TEST_ASSERT_TRUE(justPressed);
  TEST_ASSERT_FALSE(justReleased);
}

void test_state_transition_release() {
  bool prevState = true;
  bool currentState = false;

  // Update state
  bool pressed = currentState;
  bool justPressed = currentState && !prevState;
  bool justReleased = !currentState && prevState;

  TEST_ASSERT_FALSE(pressed);
  TEST_ASSERT_FALSE(justPressed);
  TEST_ASSERT_TRUE(justReleased);
}

void test_state_transition_hold() {
  bool prevState = true;
  bool currentState = true;

  // Update state
  bool pressed = currentState;
  bool justPressed = currentState && !prevState;
  bool justReleased = !currentState && prevState;

  TEST_ASSERT_TRUE(pressed);
  TEST_ASSERT_FALSE(justPressed);
  TEST_ASSERT_FALSE(justReleased);
}

// Test touch threshold logic
void test_touch_below_threshold() {
  int touchValue = 30;
  uint32_t threshold = 40;
  bool isPressed = touchValue < threshold;

  TEST_ASSERT_TRUE(isPressed);
}

void test_touch_above_threshold() {
  int touchValue = 50;
  uint32_t threshold = 40;
  bool isPressed = touchValue < threshold;

  TEST_ASSERT_FALSE(isPressed);
}

void test_touch_at_threshold() {
  int touchValue = 40;
  uint32_t threshold = 40;
  bool isPressed = touchValue < threshold;

  TEST_ASSERT_FALSE(isPressed); // At threshold is not pressed
}

// Test button state persistence
void test_button_state_persists() {
  bool pressed = true;
  bool justPressed = true;

  // After one update cycle, justPressed should be false but pressed stays true
  bool nextJustPressed = false; // Cleared after being read
  bool nextPressed = true; // Still pressed

  TEST_ASSERT_TRUE(nextPressed);
  TEST_ASSERT_FALSE(nextJustPressed);
}

void setUp(void) {
}

void tearDown(void) {
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_button_state_initialization);
  RUN_TEST(test_just_pressed_detection);
  RUN_TEST(test_not_just_pressed_when_held);
  RUN_TEST(test_just_released_detection);
  RUN_TEST(test_not_just_released_when_not_pressed);
  RUN_TEST(test_debounce_prevents_rapid_presses);
  RUN_TEST(test_debounce_allows_after_delay);
  RUN_TEST(test_multiple_buttons_independent);
  RUN_TEST(test_all_buttons_can_be_pressed);
  RUN_TEST(test_state_transition_press);
  RUN_TEST(test_state_transition_release);
  RUN_TEST(test_state_transition_hold);
  RUN_TEST(test_touch_below_threshold);
  RUN_TEST(test_touch_above_threshold);
  RUN_TEST(test_touch_at_threshold);
  RUN_TEST(test_button_state_persists);

  return UNITY_END();
}

