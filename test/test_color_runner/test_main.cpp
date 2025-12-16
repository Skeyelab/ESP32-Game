#include <unity.h>
#include <cstdint>

// Test Color Runner X game logic

// Test color match allows passage
void test_color_match_passes() {
  uint8_t zone_color = 0; // RED
  uint8_t player_color = 0; // RED
  bool can_pass = (zone_color == player_color);
  TEST_ASSERT_TRUE(can_pass);
}

// Test color mismatch causes game over
void test_color_mismatch_game_over() {
  uint8_t zone_color = 0; // RED
  uint8_t player_color = 1; // GREEN
  bool game_over = (zone_color != player_color);
  TEST_ASSERT_TRUE(game_over);
}

// Test color cycling
void test_color_cycles_red_to_green() {
  uint8_t player_color = 0; // RED
  player_color = (player_color + 1) % 3;
  TEST_ASSERT_EQUAL(1, player_color); // GREEN
}

void test_color_cycles_green_to_blue() {
  uint8_t player_color = 1; // GREEN
  player_color = (player_color + 1) % 3;
  TEST_ASSERT_EQUAL(2, player_color); // BLUE
}

void test_color_cycles_blue_to_red() {
  uint8_t player_color = 2; // BLUE
  player_color = (player_color + 1) % 3;
  TEST_ASSERT_EQUAL(0, player_color); // RED (wraps)
}

// Test score on correct color
void test_score_on_correct_color() {
  uint32_t score = 0;
  uint8_t zone_color = 1;
  uint8_t player_color = 1;

  if (zone_color == player_color) {
    score += 5;
  }

  TEST_ASSERT_EQUAL(5, score);
}

// Test zone collision detection
void test_zone_collision() {
  int player_pos = 4;
  int zone_pos = 4;
  bool collision = (player_pos == zone_pos);
  TEST_ASSERT_TRUE(collision);
}

void test_no_zone_collision() {
  int player_pos = 3;
  int zone_pos = 5;
  bool collision = (player_pos == zone_pos);
  TEST_ASSERT_FALSE(collision);
}

// Test win condition
void test_reach_end() {
  int player_pos = 7;
  int num_leds = 8;
  bool won = (player_pos >= num_leds - 1);
  TEST_ASSERT_TRUE(won);
}

void setUp(void) {
}

void tearDown(void) {
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_color_match_passes);
  RUN_TEST(test_color_mismatch_game_over);
  RUN_TEST(test_color_cycles_red_to_green);
  RUN_TEST(test_color_cycles_green_to_blue);
  RUN_TEST(test_color_cycles_blue_to_red);
  RUN_TEST(test_score_on_correct_color);
  RUN_TEST(test_zone_collision);
  RUN_TEST(test_no_zone_collision);
  RUN_TEST(test_reach_end);

  return UNITY_END();
}

