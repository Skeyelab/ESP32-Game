#include <unity.h>
#include <cstdint>

// Test 1D Splatoon game logic

// Test paint application
void test_player_paints_neutral() {
  uint8_t paint[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int player_pos = 3;

  if (paint[player_pos] != 1) {
    paint[player_pos] = 1;
  }

  TEST_ASSERT_EQUAL(1, paint[player_pos]);
}

void test_player_overpaints_opponent() {
  uint8_t paint[8] = {0, 0, 0, 2, 0, 0, 0, 0};  // Opponent at pos 3
  int player_pos = 3;
  uint32_t opponent_score = 1;

  if (paint[player_pos] == 2) {
    opponent_score--;  // Remove opponent paint
  }
  paint[player_pos] = 1;

  TEST_ASSERT_EQUAL(1, paint[player_pos]);
  TEST_ASSERT_EQUAL(0, opponent_score);
}

// Test score calculation
void test_score_increment_on_paint() {
  uint32_t player_score = 0;
  uint8_t paint[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  int pos = 3;

  if (paint[pos] != 1) {
    paint[pos] = 1;
    player_score++;
  }

  TEST_ASSERT_EQUAL(1, player_score);
}

void test_score_decrement_on_overpaint() {
  uint32_t player_score = 5;
  uint8_t paint[8] = {1, 1, 1, 1, 1, 0, 0, 0};
  int opponent_pos = 2;

  if (paint[opponent_pos] == 1) {
    player_score--;
  }

  TEST_ASSERT_EQUAL(4, player_score);
}

// Test win condition
void test_player_wins() {
  uint32_t player_score = 5;
  uint32_t opponent_score = 3;
  bool player_wins = (player_score > opponent_score);
  TEST_ASSERT_TRUE(player_wins);
}

void test_opponent_wins() {
  uint32_t player_score = 2;
  uint32_t opponent_score = 6;
  bool opponent_wins = (opponent_score > player_score);
  TEST_ASSERT_TRUE(opponent_wins);
}

void test_tie() {
  uint32_t player_score = 4;
  uint32_t opponent_score = 4;
  bool tie = (player_score == opponent_score);
  TEST_ASSERT_TRUE(tie);
}

// Test AI movement logic
void test_ai_moves_to_less_painted() {
  uint8_t paint[8] = {1, 0, 0, 0, 0, 0, 0, 0};  // Only pos 0 painted
  int ai_pos = 0;
  int best_pos = ai_pos;
  int min_paint = paint[ai_pos];

  // Check nearby positions
  for (int i = -1; i <= 1; i++) {
    int check_pos = ai_pos + i;
    if (check_pos < 0 || check_pos >= 8) continue;
    if (paint[check_pos] < min_paint) {
      best_pos = check_pos;
      min_paint = paint[check_pos];
    }
  }

  // At pos 0, can check pos 1 (right)
  // Pos 0 has paint=1, pos 1 has paint=0, so should move to pos 1
  TEST_ASSERT_EQUAL(1, best_pos);
}

void setUp(void) {
}

void tearDown(void) {
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_player_paints_neutral);
  RUN_TEST(test_player_overpaints_opponent);
  RUN_TEST(test_score_increment_on_paint);
  RUN_TEST(test_score_decrement_on_overpaint);
  RUN_TEST(test_player_wins);
  RUN_TEST(test_opponent_wins);
  RUN_TEST(test_tie);
  RUN_TEST(test_ai_moves_to_less_painted);

  return UNITY_END();
}

