#include <unity.h>
#include <cstdint>

// Test Pulse Warrior game logic

// Test pulse movement toward target
void test_pulse_moves_toward_target() {
  int pulse_pos = 2;
  int target_pos = 5;

  if (pulse_pos < target_pos) {
    pulse_pos++;
  }

  TEST_ASSERT_EQUAL(3, pulse_pos);
}

void test_pulse_moves_away_from_target() {
  int pulse_pos = 6;
  int target_pos = 3;

  if (pulse_pos > target_pos) {
    pulse_pos--;
  }

  TEST_ASSERT_EQUAL(5, pulse_pos);
}

// Test perfect hit
void test_perfect_hit() {
  int pulse_pos = 4;
  int target_pos = 4;
  bool hit = (pulse_pos == target_pos);
  TEST_ASSERT_TRUE(hit);
}

// Test miss
void test_miss() {
  int pulse_pos = 3;
  int target_pos = 4;
  bool hit = (pulse_pos == target_pos);
  TEST_ASSERT_FALSE(hit);
}

// Test combo system
void test_combo_increment() {
  uint32_t combo = 0;
  bool hit = true;

  if (hit) {
    combo++;
  }

  TEST_ASSERT_EQUAL(1, combo);
}

void test_combo_reset_on_miss() {
  uint32_t combo = 5;
  bool missed = true;

  if (missed) {
    combo = 0;
  }

  TEST_ASSERT_EQUAL(0, combo);
}

// Test score with combo
void test_score_with_combo() {
  uint32_t score = 0;
  uint32_t combo = 3;
  bool hit = true;

  if (hit) {
    score += 10 + combo;
  }

  TEST_ASSERT_EQUAL(13, score);
}

// Test pulse timer expiration
void test_pulse_expires() {
  bool pulse_active = true;
  uint32_t pulse_timer = 100;
  uint32_t tick_ms = 150;

  // Handle unsigned underflow
  if (pulse_timer > tick_ms) {
    pulse_timer -= tick_ms;
  } else {
    pulse_timer = 0;
  }

  if (pulse_timer <= 0) {
    pulse_active = false;
  }

  TEST_ASSERT_FALSE(pulse_active);
}

void setUp(void) {
}

void tearDown(void) {
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_pulse_moves_toward_target);
  RUN_TEST(test_pulse_moves_away_from_target);
  RUN_TEST(test_perfect_hit);
  RUN_TEST(test_miss);
  RUN_TEST(test_combo_increment);
  RUN_TEST(test_combo_reset_on_miss);
  RUN_TEST(test_score_with_combo);
  RUN_TEST(test_pulse_expires);

  return UNITY_END();
}

