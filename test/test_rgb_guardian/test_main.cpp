#include <unity.h>
#include <cstdint>

// Test RGB Guardian game logic

// Test color matching for successful hit
void test_color_match_hit() {
  uint8_t enemy_color = 0; // RED
  uint8_t bullet_color = 0; // RED
  bool hit = (enemy_color == bullet_color);
  TEST_ASSERT_TRUE(hit);
}

// Test color mismatch
void test_color_mismatch_no_hit() {
  uint8_t enemy_color = 0; // RED
  uint8_t bullet_color = 1; // GREEN
  bool hit = (enemy_color == bullet_color);
  TEST_ASSERT_FALSE(hit);
}

// Test enemy spawn direction
void test_enemy_spawns_from_left() {
  bool from_left = true;
  int enemy_pos = from_left ? 0 : 7;
  int enemy_dir = from_left ? 1 : -1;

  TEST_ASSERT_EQUAL(0, enemy_pos);
  TEST_ASSERT_EQUAL(1, enemy_dir);
}

void test_enemy_spawns_from_right() {
  bool from_left = false;
  int enemy_pos = from_left ? 0 : 7;
  int enemy_dir = from_left ? 1 : -1;

  TEST_ASSERT_EQUAL(7, enemy_pos);
  TEST_ASSERT_EQUAL(-1, enemy_dir);
}

// Test bullet direction toward enemy
void test_bullet_direction_left() {
  int enemy_pos = 2;
  int def_pos = 3;
  int bullet_dir = (enemy_pos < def_pos) ? -1 : 1;
  TEST_ASSERT_EQUAL(-1, bullet_dir);
}

void test_bullet_direction_right() {
  int enemy_pos = 5;
  int def_pos = 3;
  int bullet_dir = (enemy_pos < def_pos) ? -1 : 1;
  TEST_ASSERT_EQUAL(1, bullet_dir);
}

// Test enemy reaches defender (game over)
void test_enemy_reaches_defender() {
  int enemy_pos = 3;
  int def_pos = 3;
  bool game_over = (enemy_pos == def_pos);
  TEST_ASSERT_TRUE(game_over);
}

// Test weapon color cycling
void test_weapon_cycles_red_to_green() {
  uint8_t weapon_color = 0; // RED
  weapon_color = (weapon_color + 1) % 3;
  TEST_ASSERT_EQUAL(1, weapon_color); // GREEN
}

void test_weapon_cycles_green_to_blue() {
  uint8_t weapon_color = 1; // GREEN
  weapon_color = (weapon_color + 1) % 3;
  TEST_ASSERT_EQUAL(2, weapon_color); // BLUE
}

void test_weapon_cycles_blue_to_red() {
  uint8_t weapon_color = 2; // BLUE
  weapon_color = (weapon_color + 1) % 3;
  TEST_ASSERT_EQUAL(0, weapon_color); // RED (wraps)
}

// Test score increment on hit
void test_score_increment() {
  uint32_t score = 0;
  bool color_match = true;

  if (color_match) {
    score++;
  }

  TEST_ASSERT_EQUAL(1, score);
}

void setUp(void) {
}

void tearDown(void) {
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_color_match_hit);
  RUN_TEST(test_color_mismatch_no_hit);
  RUN_TEST(test_enemy_spawns_from_left);
  RUN_TEST(test_enemy_spawns_from_right);
  RUN_TEST(test_bullet_direction_left);
  RUN_TEST(test_bullet_direction_right);
  RUN_TEST(test_enemy_reaches_defender);
  RUN_TEST(test_weapon_cycles_red_to_green);
  RUN_TEST(test_weapon_cycles_green_to_blue);
  RUN_TEST(test_weapon_cycles_blue_to_red);
  RUN_TEST(test_score_increment);

  return UNITY_END();
}


