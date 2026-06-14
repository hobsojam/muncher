#include "test_framework.h"
#include "../src/map.c"
#include "../src/player.c"

// wrap_col and can_enter are static in player.c; including the .c makes them visible here.

static void test_wrap_col_normal(void) {
    TEST_ASSERT_EQUAL_INT(5, wrap_col(5));
}

static void test_wrap_col_negative(void) {
    TEST_ASSERT_EQUAL_INT(27, wrap_col(-1));
}

static void test_wrap_col_overflow(void) {
    TEST_ASSERT_EQUAL_INT(0, wrap_col(MAP_COLS));
}

static void test_can_enter_wall(void) {
    map_init();
    TEST_ASSERT_EQUAL_INT(0, can_enter(0, 0));
}

static void test_can_enter_dot(void) {
    map_init();
    TEST_ASSERT_EQUAL_INT(1, can_enter(1, 1));
}

static void test_can_enter_door_blocked(void) {
    map_init();
    TEST_ASSERT_EQUAL_INT(0, can_enter(13, 12));
}

static void test_can_enter_tunnel_oob(void) {
    map_init();
    TEST_ASSERT_EQUAL_INT(1, can_enter(-1, TUNNEL_ROW));
    TEST_ASSERT_EQUAL_INT(1, can_enter(MAP_COLS, TUNNEL_ROW));
}

static void test_can_enter_oob_non_tunnel(void) {
    map_init();
    TEST_ASSERT_EQUAL_INT(0, can_enter(-1, 0));
    TEST_ASSERT_EQUAL_INT(0, can_enter(0, -1));
    TEST_ASSERT_EQUAL_INT(0, can_enter(0, MAP_ROWS));
}

static void test_score_dot(void) {
    map_init();
    Player p;
    player_init(&p);
    // Player at col=14, row=29. col=15 is a dot in row 29.
    p.dir_col = 1; p.dir_row = 0;
    p.move_t  = 0.875f;
    player_update(&p, 0.125f); // move_t += 8*0.125 = 1.0 → crosses tile
    TEST_ASSERT_EQUAL_INT(1, p.score);
    TEST_ASSERT_EQUAL_INT(0, p.ate_power);
}

static void test_score_power_pellet(void) {
    map_init();
    Player p;
    player_init(&p);
    map[29][15] = TILE_POWER;
    p.dir_col = 1; p.dir_row = 0;
    p.move_t  = 0.875f;
    player_update(&p, 0.125f);
    TEST_ASSERT_EQUAL_INT(10, p.score);
    TEST_ASSERT_EQUAL_INT(1, p.ate_power);
}

static void test_large_dt_consumes_multiple_player_tiles(void) {
    map_init();
    Player p;
    player_init(&p);
    map[29][15] = TILE_DOT;
    map[29][16] = TILE_DOT;
    map[29][17] = TILE_DOT;
    p.dir_col = 1; p.dir_row = 0;
    p.next_dir_col = 1; p.next_dir_row = 0;
    p.move_t = 0.75f;
    player_update(&p, 0.25f);
    TEST_ASSERT_EQUAL_INT(16, p.col);
    TEST_ASSERT_EQUAL_INT(2, p.score);
    TEST_ASSERT(p.move_t < 1.0f);
}

static void test_very_large_dt_keeps_player_move_t_bounded(void) {
    map_init();
    Player p;
    player_init(&p);
    for (int c = 15; c <= 26; c++) map[29][c] = TILE_EMPTY;
    p.dir_col = 1; p.dir_row = 0;
    p.next_dir_col = 1; p.next_dir_row = 0;
    player_update(&p, 10.0f);
    TEST_ASSERT(p.col > 14);
    TEST_ASSERT(p.col <= 22);
    TEST_ASSERT(p.move_t < 1.0f);
}

static void test_player_init_position(void) {
    map_init();
    Player p;
    player_init(&p);
    TEST_ASSERT_EQUAL_INT(14, p.col);
    TEST_ASSERT_EQUAL_INT(29, p.row);
    TEST_ASSERT_EQUAL_INT(0,  p.score);
    TEST_ASSERT_EQUAL_INT(TILE_EMPTY, map[29][14]);
}

int main(void) {
    RUN_TEST(test_wrap_col_normal);
    RUN_TEST(test_wrap_col_negative);
    RUN_TEST(test_wrap_col_overflow);
    RUN_TEST(test_can_enter_wall);
    RUN_TEST(test_can_enter_dot);
    RUN_TEST(test_can_enter_door_blocked);
    RUN_TEST(test_can_enter_tunnel_oob);
    RUN_TEST(test_can_enter_oob_non_tunnel);
    RUN_TEST(test_score_dot);
    RUN_TEST(test_score_power_pellet);
    RUN_TEST(test_large_dt_consumes_multiple_player_tiles);
    RUN_TEST(test_very_large_dt_keeps_player_move_t_bounded);
    RUN_TEST(test_player_init_position);
    TESTS_SUMMARY();
}
