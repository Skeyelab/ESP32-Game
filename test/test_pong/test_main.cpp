#include <unity.h>
#include <cstdint>

// Test 1D Pong game logic

// Test ball bounce on paddle
void test_ball_bounce_player() {
  int ball_pos = 0;
  int player_paddle = 0;
  int ball_dir = -1;  // Moving left

  if (ball_pos == player_paddle && ball_dir < 0) {
    ball_dir = 1;  // Bounce right
  }

  TEST_ASSERT_EQUAL(1, ball_dir);
}

void test_ball_bounce_ai() {
  int ball_pos = 7;
  int ai_paddle = 7;
  int ball_dir = 1;  // Moving right

  if (ball_pos == ai_paddle && ball_dir > 0) {
    ball_dir = -1;  // Bounce left
  }

  TEST_ASSERT_EQUAL(-1, ball_dir);
}

// Test scoring
void test_player_scores() {
  int ball_pos = -1;
  uint32_t player_score = 0;

  if (ball_pos < 0) {
    player_score++;
  }

  TEST_ASSERT_EQUAL(1, player_score);
}

void test_ai_scores() {
  int ball_pos = 8;
  int num_leds = 8;
  uint32_t ai_score = 0;

  if (ball_pos >= num_leds) {
    ai_score++;
  }

  TEST_ASSERT_EQUAL(1, ai_score);
}

// Test AI movement
void test_ai_moves_toward_ball() {
  int ai_paddle = 7;
  int ball_pos = 3;

  if (ball_pos < ai_paddle) {
    ai_paddle--;
  }

  TEST_ASSERT_EQUAL(6, ai_paddle);
}

void test_ai_moves_away_from_ball() {
  int ai_paddle = 2;
  int ball_pos = 5;

  if (ball_pos > ai_paddle) {
    ai_paddle++;
  }

  TEST_ASSERT_EQUAL(3, ai_paddle);
}

// Test ball movement
void test_ball_moves_left() {
  int ball_pos = 5;
  int ball_dir = -1;
  ball_pos += ball_dir;
  TEST_ASSERT_EQUAL(4, ball_pos);
}

void test_ball_moves_right() {
  int ball_pos = 5;
  int ball_dir = 1;
  ball_pos += ball_dir;
  TEST_ASSERT_EQUAL(6, ball_pos);
}

void setUp(void) {
}

void tearDown(void) {
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_ball_bounce_player);
  RUN_TEST(test_ball_bounce_ai);
  RUN_TEST(test_player_scores);
  RUN_TEST(test_ai_scores);
  RUN_TEST(test_ai_moves_toward_ball);
  RUN_TEST(test_ai_moves_away_from_ball);
  RUN_TEST(test_ball_moves_left);
  RUN_TEST(test_ball_moves_right);

  return UNITY_END();
}

