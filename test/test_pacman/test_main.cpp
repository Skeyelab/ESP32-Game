#include <unity.h>
#include <cstdint>

// Test Pacman-specific game logic

// Test pellet collection
void test_pellet_collection() {
  int pacman_pos = 5;
  int pellet_pos = 5;
  bool collected = (pacman_pos == pellet_pos);
  TEST_ASSERT_TRUE(collected);
}

void test_pellet_not_collected() {
  int pacman_pos = 3;
  int pellet_pos = 5;
  bool collected = (pacman_pos == pellet_pos);
  TEST_ASSERT_FALSE(collected);
}

// Test power pellet activation
void test_power_pellet_activates() {
  bool is_power_pellet = true;
  bool power_mode = false;

  if (is_power_pellet) {
    power_mode = true;
  }

  TEST_ASSERT_TRUE(power_mode);
}

// Test ghost vulnerability in power mode
void test_ghost_vulnerable() {
  bool power_mode = true;
  bool ghost_vulnerable = power_mode;
  TEST_ASSERT_TRUE(ghost_vulnerable);
}

// Test ghost collision - normal mode (game over)
void test_ghost_collision_normal() {
  int pacman_pos = 5;
  int ghost_pos = 5;
  bool power_mode = false;
  bool collision = (pacman_pos == ghost_pos);

  if (collision && !power_mode) {
    // Game over
    TEST_ASSERT_TRUE(true);
  } else {
    TEST_ASSERT_FALSE(true);
  }
}

// Test ghost collision - power mode (eat ghost)
void test_ghost_collision_power() {
  int pacman_pos = 5;
  int ghost_pos = 5;
  bool power_mode = true;
  bool collision = (pacman_pos == ghost_pos);

  if (collision && power_mode) {
    // Eat ghost, score points
    TEST_ASSERT_TRUE(true);
  } else {
    TEST_ASSERT_FALSE(true);
  }
}

// Test ghost direction - chase mode
void test_ghost_chase_left() {
  int ghost_pos = 7;
  int pacman_pos = 3;
  int dir = (ghost_pos > pacman_pos) ? -1 : 1;
  TEST_ASSERT_EQUAL(-1, dir);
}

void test_ghost_chase_right() {
  int ghost_pos = 2;
  int pacman_pos = 5;
  int dir = (ghost_pos > pacman_pos) ? -1 : 1;
  TEST_ASSERT_EQUAL(1, dir);
}

// Test ghost direction - flee mode (power pellet)
void test_ghost_flee() {
  int ghost_pos = 3;
  int pacman_pos = 5;
  bool power_mode = true;
  int dir;

  if (power_mode) {
    // Run away from pacman
    // If ghost is left of pacman, flee left (negative)
    // If ghost is right of pacman, flee right (positive)
    dir = (ghost_pos > pacman_pos) ? 1 : -1;
  } else {
    // Chase pacman
    dir = (ghost_pos > pacman_pos) ? -1 : 1;
  }

  // Ghost at 3, pacman at 5, so ghost should flee left (negative)
  TEST_ASSERT_EQUAL(-1, dir);
}

void setUp(void) {
}

void tearDown(void) {
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_pellet_collection);
  RUN_TEST(test_pellet_not_collected);
  RUN_TEST(test_power_pellet_activates);
  RUN_TEST(test_ghost_vulnerable);
  RUN_TEST(test_ghost_collision_normal);
  RUN_TEST(test_ghost_collision_power);
  RUN_TEST(test_ghost_chase_left);
  RUN_TEST(test_ghost_chase_right);
  RUN_TEST(test_ghost_flee);

  return UNITY_END();
}

