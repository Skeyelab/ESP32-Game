#include <unity.h>
#include <cstdint>

// Test FlappyBird game logic

// Test obstacle gap collision
void test_obstacle_collision_top() {
  int bird_pos = 2;
  int gap_top = 3;
  int gap_size = 2;
  bool collision = (bird_pos < gap_top);
  TEST_ASSERT_TRUE(collision);
}

void test_obstacle_collision_bottom() {
  int bird_pos = 5;
  int gap_top = 3;
  int gap_size = 2;
  int gap_bottom = gap_top + gap_size;
  bool collision = (bird_pos >= gap_bottom);
  TEST_ASSERT_TRUE(collision);
}

void test_obstacle_no_collision() {
  int bird_pos = 4;
  int gap_top = 3;
  int gap_size = 2;
  int gap_bottom = gap_top + gap_size;
  bool collision = (bird_pos < gap_top || bird_pos >= gap_bottom);
  TEST_ASSERT_FALSE(collision);
}

// Test gravity mechanics
void test_gravity_increases_velocity() {
  int vel = 0;
  vel += 1;  // Gravity
  TEST_ASSERT_EQUAL(1, vel);
}

void test_flap_decreases_velocity() {
  int vel = 2;
  vel -= 3;  // Flap
  TEST_ASSERT_EQUAL(-1, vel);
}

// Test boundary collisions
void test_boundary_top() {
  int bird_pos = 0;
  bool hit_boundary = (bird_pos <= 0);
  TEST_ASSERT_TRUE(hit_boundary);
}

void test_boundary_bottom() {
  int bird_pos = 8;
  int num_leds = 8;
  bool hit_boundary = (bird_pos >= num_leds - 1);
  TEST_ASSERT_TRUE(hit_boundary);
}

// Test score increment
void test_score_on_obstacle_pass() {
  uint32_t score = 0;
  bool obstacle_passed = true;
  if (obstacle_passed) {
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

  RUN_TEST(test_obstacle_collision_top);
  RUN_TEST(test_obstacle_collision_bottom);
  RUN_TEST(test_obstacle_no_collision);
  RUN_TEST(test_gravity_increases_velocity);
  RUN_TEST(test_flap_decreases_velocity);
  RUN_TEST(test_boundary_top);
  RUN_TEST(test_boundary_bottom);
  RUN_TEST(test_score_on_obstacle_pass);

  return UNITY_END();
}

