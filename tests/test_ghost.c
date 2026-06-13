#include "test_framework.h"
#include "../src/map.c"
#include "../src/ghost.c"

// ghost_wrap_col, ghost_can_enter, dist_sq, get_target are static in ghost.c;
// including the .c makes them visible here.

static void test_ghost_wrap_col_negative(void) {
    TEST_ASSERT_EQUAL_INT(27, ghost_wrap_col(-1));
}

static void test_ghost_wrap_col_overflow(void) {
    TEST_ASSERT_EQUAL_INT(0, ghost_wrap_col(MAP_COLS));
}

static void test_ghost_wrap_col_normal(void) {
    TEST_ASSERT_EQUAL_INT(5, ghost_wrap_col(5));
}

static void test_dist_sq(void) {
    TEST_ASSERT_EQUAL_INT(25, dist_sq(0, 0, 3, 4));
    TEST_ASSERT_EQUAL_INT(0,  dist_sq(5, 5, 5, 5));
    TEST_ASSERT_EQUAL_INT(2,  dist_sq(0, 0, 1, 1));
}

static void test_ghost_can_enter_wall(void) {
    map_init();
    TEST_ASSERT_EQUAL_INT(0, ghost_can_enter(0, 0));
}

static void test_ghost_can_enter_open(void) {
    map_init();
    TEST_ASSERT_EQUAL_INT(1, ghost_can_enter(1, 1));
}

static void test_ghost_can_enter_tunnel_oob(void) {
    map_init();
    TEST_ASSERT_EQUAL_INT(1, ghost_can_enter(-1, TUNNEL_ROW));
    TEST_ASSERT_EQUAL_INT(1, ghost_can_enter(MAP_COLS, TUNNEL_ROW));
}

static void test_ghost_can_enter_oob_non_tunnel(void) {
    map_init();
    TEST_ASSERT_EQUAL_INT(0, ghost_can_enter(-1, 0));
    TEST_ASSERT_EQUAL_INT(0, ghost_can_enter(0, -1));
    TEST_ASSERT_EQUAL_INT(0, ghost_can_enter(0, MAP_ROWS));
}

static void test_ghosts_init_positions(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    TEST_ASSERT_EQUAL_INT(9,  ghosts[GHOST_BLINKY].col);
    TEST_ASSERT_EQUAL_INT(11, ghosts[GHOST_BLINKY].row);
    TEST_ASSERT_EQUAL_INT(18, ghosts[GHOST_PINKY].col);
    TEST_ASSERT_EQUAL_INT(11, ghosts[GHOST_PINKY].row);
}

static void test_ghosts_init_scatter_mode(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    for (int i = 0; i < GHOST_COUNT; i++) {
        TEST_ASSERT_EQUAL_INT(GMODE_SCATTER, ghosts[i].mode);
    }
}

static void test_scatter_target_returns_scatter_pos(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    Player player = {0};
    player.col = 14; player.row = 29;
    int tc, tr;
    get_target(&ghosts[GHOST_BLINKY], &player, ghosts, &tc, &tr);
    TEST_ASSERT_EQUAL_INT(ghosts[GHOST_BLINKY].scatter_col, tc);
    TEST_ASSERT_EQUAL_INT(ghosts[GHOST_BLINKY].scatter_row, tr);
}

static void test_blinky_chase_targets_player(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    ghosts[GHOST_BLINKY].mode = GMODE_CHASE;
    Player player = {0};
    player.col = 10; player.row = 20;
    int tc, tr;
    get_target(&ghosts[GHOST_BLINKY], &player, ghosts, &tc, &tr);
    TEST_ASSERT_EQUAL_INT(10, tc);
    TEST_ASSERT_EQUAL_INT(20, tr);
}

static void test_pinky_chase_targets_4_ahead(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    ghosts[GHOST_PINKY].mode = GMODE_CHASE;
    Player player = {0};
    player.col = 10; player.row = 20;
    player.dir_col = 1; player.dir_row = 0;
    int tc, tr;
    get_target(&ghosts[GHOST_PINKY], &player, ghosts, &tc, &tr);
    TEST_ASSERT_EQUAL_INT(14, tc);
    TEST_ASSERT_EQUAL_INT(20, tr);
}

static void test_clyde_far_targets_player(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    ghosts[GHOST_CLYDE].mode = GMODE_CHASE;
    // Place Clyde far from player (dist_sq > 64)
    ghosts[GHOST_CLYDE].col = 1; ghosts[GHOST_CLYDE].row = 1;
    Player player = {0};
    player.col = 14; player.row = 20;
    int tc, tr;
    get_target(&ghosts[GHOST_CLYDE], &player, ghosts, &tc, &tr);
    TEST_ASSERT_EQUAL_INT(14, tc);
    TEST_ASSERT_EQUAL_INT(20, tr);
}

static void test_clyde_near_targets_scatter(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    ghosts[GHOST_CLYDE].mode = GMODE_CHASE;
    // Place Clyde adjacent to player (dist_sq <= 64)
    ghosts[GHOST_CLYDE].col = 14; ghosts[GHOST_CLYDE].row = 20;
    Player player = {0};
    player.col = 14; player.row = 20;
    int tc, tr;
    get_target(&ghosts[GHOST_CLYDE], &player, ghosts, &tc, &tr);
    TEST_ASSERT_EQUAL_INT(ghosts[GHOST_CLYDE].scatter_col, tc);
    TEST_ASSERT_EQUAL_INT(ghosts[GHOST_CLYDE].scatter_row, tr);
}

static void test_frighten_sets_mode(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    ghosts_frighten(ghosts);
    for (int i = 0; i < GHOST_COUNT; i++) {
        TEST_ASSERT_EQUAL_INT(GMODE_FRIGHTENED, ghosts[i].mode);
    }
}

static void test_frighten_reverses_direction(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    // Blinky starts dir=(1,0)
    ghosts_frighten(ghosts);
    TEST_ASSERT_EQUAL_INT(-1, ghosts[GHOST_BLINKY].dir_col);
    TEST_ASSERT_EQUAL_INT(0,  ghosts[GHOST_BLINKY].dir_row);
}

int main(void) {
    RUN_TEST(test_ghost_wrap_col_negative);
    RUN_TEST(test_ghost_wrap_col_overflow);
    RUN_TEST(test_ghost_wrap_col_normal);
    RUN_TEST(test_dist_sq);
    RUN_TEST(test_ghost_can_enter_wall);
    RUN_TEST(test_ghost_can_enter_open);
    RUN_TEST(test_ghost_can_enter_tunnel_oob);
    RUN_TEST(test_ghost_can_enter_oob_non_tunnel);
    RUN_TEST(test_ghosts_init_positions);
    RUN_TEST(test_ghosts_init_scatter_mode);
    RUN_TEST(test_scatter_target_returns_scatter_pos);
    RUN_TEST(test_blinky_chase_targets_player);
    RUN_TEST(test_pinky_chase_targets_4_ahead);
    RUN_TEST(test_clyde_far_targets_player);
    RUN_TEST(test_clyde_near_targets_scatter);
    RUN_TEST(test_frighten_sets_mode);
    RUN_TEST(test_frighten_reverses_direction);
    TESTS_SUMMARY();
}
