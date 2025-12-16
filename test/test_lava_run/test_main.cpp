#include <unity.h>
#include <cstdint>

// Test Lava Run game logic

// Test lava zone activation
void test_lava_activates() {
  bool lava_active = false;
  uint32_t timer = 0;
  uint32_t threshold = 1000;

  timer = 1200; // Past threshold
  if (timer >= threshold) {
    lava_active = true;
  }

  TEST_ASSERT_TRUE(lava_active);
}

// Test player safe movement
void test_move_when_safe() {
  int player_pos = 3;
  int next_pos = 4;
  bool lava_active[8] = {false, false, false, false, false, false, false, false};

  if (!lava_active[next_pos]) {
    player_pos = next_pos;
  }

  TEST_ASSERT_EQUAL(4, player_pos);
}

// Test player blocked by lava
void test_blocked_by_lava() {
  int player_pos = 3;
  int next_pos = 4;
  bool lava_active[8] = {false, false, false, false, true, false, false, false};

  if (!lava_active[next_pos]) {
    player_pos = next_pos;
  }

  TEST_ASSERT_EQUAL(3, player_pos); // Should not move
}

// Test win condition
void test_reach_goal() {
  int player_pos = 7;
  int target_pos = 7;
  bool won = (player_pos == target_pos);
  TEST_ASSERT_TRUE(won);
}

// Test game over on lava
void test_game_over_lava() {
  int player_pos = 4;
  bool lava_active[8] = {false, false, false, false, true, false, false, false};
  bool game_over = lava_active[player_pos];
  TEST_ASSERT_TRUE(game_over);
}

// Test lava timing cycle
void test_lava_timing_erupt() {
  uint32_t timer = 0;
  uint32_t erupt_duration = 800;

  timer = erupt_duration;
  bool should_cool = (timer >= erupt_duration);

  if (should_cool) {
    timer = 0; // Reset for cool phase
  }

  TEST_ASSERT_EQUAL(0, timer);
}

void setUp(void) {
}

void tearDown(void) {
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_lava_activates);
  RUN_TEST(test_move_when_safe);
  RUN_TEST(test_blocked_by_lava);
  RUN_TEST(test_reach_goal);
  RUN_TEST(test_game_over_lava);
  RUN_TEST(test_lava_timing_erupt);

  return UNITY_END();
}

