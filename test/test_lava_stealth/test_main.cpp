#include <unity.h>
#include <cstdint>

// Test Lava Stealth game logic

// Test stealth mode activation
void test_stealth_activates() {
  bool stealth_mode = false;
  uint32_t stealth_cooldown = 0;

  if (stealth_cooldown == 0) {
    stealth_mode = true;
  }

  TEST_ASSERT_TRUE(stealth_mode);
}

// Test stealth prevents damage
void test_stealth_passes_through_lava() {
  int player_pos = 4;
  bool lava_active[8] = {false, false, false, false, true, false, false, false};
  bool stealth_mode = true;
  bool game_over = false;

  if (lava_active[player_pos] && !stealth_mode) {
    game_over = true;
  }

  TEST_ASSERT_FALSE(game_over); // Should not die in stealth
}

// Test normal mode takes damage
void test_normal_mode_dies_in_lava() {
  int player_pos = 4;
  bool lava_active[8] = {false, false, false, false, true, false, false, false};
  bool stealth_mode = false;
  bool game_over = false;

  if (lava_active[player_pos] && !stealth_mode) {
    game_over = true;
  }

  TEST_ASSERT_TRUE(game_over);
}

// Test stealth cooldown
void test_stealth_cooldown() {
  uint32_t stealth_cooldown = 5000;
  uint32_t tick_ms = 100;

  stealth_cooldown -= tick_ms;

  TEST_ASSERT_EQUAL(4900, stealth_cooldown);
}

// Test stealth duration
void test_stealth_duration() {
  bool stealth_mode = true;
  uint32_t stealth_timer = 2000;
  uint32_t tick_ms = 100;

  stealth_timer -= tick_ms;

  if (stealth_timer <= 0) {
    stealth_mode = false;
  }

  TEST_ASSERT_TRUE(stealth_mode); // Still active
}

// Test stealth expires
void test_stealth_expires() {
  bool stealth_mode = true;
  uint32_t stealth_timer = 100;
  uint32_t tick_ms = 150;

  // Handle unsigned underflow
  if (stealth_timer > tick_ms) {
    stealth_timer -= tick_ms;
  } else {
    stealth_timer = 0;
  }

  if (stealth_timer <= 0) {
    stealth_mode = false;
  }

  TEST_ASSERT_FALSE(stealth_mode);
}

// Test win condition
void test_reach_goal() {
  int player_pos = 7;
  int target_pos = 7;
  bool won = (player_pos == target_pos);
  TEST_ASSERT_TRUE(won);
}

void setUp(void) {
}

void tearDown(void) {
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_stealth_activates);
  RUN_TEST(test_stealth_passes_through_lava);
  RUN_TEST(test_normal_mode_dies_in_lava);
  RUN_TEST(test_stealth_cooldown);
  RUN_TEST(test_stealth_duration);
  RUN_TEST(test_stealth_expires);
  RUN_TEST(test_reach_goal);

  return UNITY_END();
}

