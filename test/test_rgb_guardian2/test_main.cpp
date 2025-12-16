#include <unity.h>
#include <cstdint>

// Test RGB Guardian 2 game logic (multiple enemies/bullets)

// Test multiple enemy tracking
void test_multiple_enemies_active() {
  bool enemies[2] = {true, false};
  int active_count = 0;
  for (int i = 0; i < 2; i++) {
    if (enemies[i]) active_count++;
  }
  TEST_ASSERT_EQUAL(1, active_count);
}

void test_all_enemies_active() {
  bool enemies[2] = {true, true};
  int active_count = 0;
  for (int i = 0; i < 2; i++) {
    if (enemies[i]) active_count++;
  }
  TEST_ASSERT_EQUAL(2, active_count);
}

// Test finding nearest enemy
void test_find_nearest_enemy() {
  int enemy_positions[2] = {2, 6};
  int def_pos = 3;
  int nearest = -1;
  int nearest_dist = 8;

  for (int i = 0; i < 2; i++) {
    int dist = abs(enemy_positions[i] - def_pos);
    if (dist < nearest_dist) {
      nearest_dist = dist;
      nearest = i;
    }
  }

  TEST_ASSERT_EQUAL(0, nearest); // Enemy at pos 2 is nearest
}

// Test multiple bullets
void test_multiple_bullets_active() {
  bool bullets[2] = {true, false};
  int active_count = 0;
  for (int i = 0; i < 2; i++) {
    if (bullets[i]) active_count++;
  }
  TEST_ASSERT_EQUAL(1, active_count);
}

// Test bullet-enemy collision with multiple entities
void test_bullet_hits_correct_enemy() {
  int bullet_pos = 4;
  int enemy_positions[2] = {3, 5};
  bool enemies_active[2] = {true, true};
  int hit_enemy = -1;

  for (int i = 0; i < 2; i++) {
    if (enemies_active[i] && enemy_positions[i] == bullet_pos) {
      hit_enemy = i;
      break;
    }
  }

  TEST_ASSERT_EQUAL(-1, hit_enemy); // No collision at pos 4
}

void test_bullet_hits_enemy_at_position() {
  int bullet_pos = 5;
  int enemy_positions[2] = {3, 5};
  bool enemies_active[2] = {true, true};
  int hit_enemy = -1;

  for (int i = 0; i < 2; i++) {
    if (enemies_active[i] && enemy_positions[i] == bullet_pos) {
      hit_enemy = i;
      break;
    }
  }

  TEST_ASSERT_EQUAL(1, hit_enemy); // Enemy 1 at pos 5
}

// Test score with multiple enemies
void test_score_multiple_hits() {
  uint32_t score = 0;
  bool hit1 = true;
  bool hit2 = true;

  if (hit1) score++;
  if (hit2) score++;

  TEST_ASSERT_EQUAL(2, score);
}

void setUp(void) {
}

void tearDown(void) {
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_multiple_enemies_active);
  RUN_TEST(test_all_enemies_active);
  RUN_TEST(test_find_nearest_enemy);
  RUN_TEST(test_multiple_bullets_active);
  RUN_TEST(test_bullet_hits_correct_enemy);
  RUN_TEST(test_bullet_hits_enemy_at_position);
  RUN_TEST(test_score_multiple_hits);

  return UNITY_END();
}


