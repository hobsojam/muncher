#include "test_framework.h"
#include "map.h"
#include "player.h"
#include "player_internal.h"

static void test_wrap_col_normal(void) {
    TEST_ASSERT_EQUAL_INT(5, player_wrap_col(5));
}

static void test_wrap_col_negative(void) {
    TEST_ASSERT_EQUAL_INT(27, player_wrap_col(-1));
}

static void test_wrap_col_overflow(void) {
    TEST_ASSERT_EQUAL_INT(0, player_wrap_col(MAP_COLS));
}

static void test_can_enter_wall(void) {
    map_init();
    TEST_ASSERT_EQUAL_INT(0, player_can_enter(0, 0));
}

static void test_can_enter_dot(void) {
    map_init();
    TEST_ASSERT_EQUAL_INT(1, player_can_enter(1, 1));
}

static void test_can_enter_door_blocked(void) {
    map_init();
    TEST_ASSERT_EQUAL_INT(0, player_can_enter(13, 12));
}

static void test_can_enter_tunnel_oob(void) {
    map_init();
    TEST_ASSERT_EQUAL_INT(1, player_can_enter(-1, PLAYER_TUNNEL_ROW));
    TEST_ASSERT_EQUAL_INT(1, player_can_enter(MAP_COLS, PLAYER_TUNNEL_ROW));
}

static void test_can_enter_oob_non_tunnel(void) {
    map_init();
    TEST_ASSERT_EQUAL_INT(0, player_can_enter(-1, 0));
    TEST_ASSERT_EQUAL_INT(0, player_can_enter(0, -1));
    TEST_ASSERT_EQUAL_INT(0, player_can_enter(0, MAP_ROWS));
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

static void test_extra_life_not_awarded_below_threshold(void) {
    Player p;
    player_init(&p);
    p.score = 1499;
    player_check_extra_life(&p);
    TEST_ASSERT_EQUAL_INT(3, p.lives);
    TEST_ASSERT_EQUAL_INT(0, p.extra_life_flags);
}

static void test_extra_life_awarded_at_1500(void) {
    Player p;
    player_init(&p);
    p.score = 1500;
    player_check_extra_life(&p);
    TEST_ASSERT_EQUAL_INT(4, p.lives);
    TEST_ASSERT_EQUAL_INT(1, p.extra_life_flags & 1);
}

static void test_extra_life_not_awarded_twice_at_1500(void) {
    Player p;
    player_init(&p);
    p.score = 1500;
    player_check_extra_life(&p);
    player_check_extra_life(&p);
    TEST_ASSERT_EQUAL_INT(4, p.lives);
}

static void test_extra_life_awarded_at_5000(void) {
    Player p;
    player_init(&p);
    p.score = 5000;
    player_check_extra_life(&p);
    TEST_ASSERT_EQUAL_INT(5, p.lives);  /* both thresholds crossed */
    TEST_ASSERT_EQUAL_INT(3, p.extra_life_flags & 3);
}

static void test_extra_life_caps_at_max(void) {
    Player p;
    player_init(&p);
    p.lives = 5;
    p.score = 5000;
    player_check_extra_life(&p);
    TEST_ASSERT_EQUAL_INT(5, p.lives);
}

static void test_extra_life_flags_reset_on_init(void) {
    Player p;
    player_init(&p);
    p.score = 5000;
    player_check_extra_life(&p);
    player_init(&p);
    TEST_ASSERT_EQUAL_INT(0, p.extra_life_flags);
}

static void test_player_draw_alive_does_not_crash(void) {
    map_init();
    Player p;
    player_init(&p);
    /* death_progress = 0.0 means normal size; draw calls are no-ops in test stubs */
    player_draw(&p, 0, 0, 0.0f);
    TEST_ASSERT(1); /* reaching here without crash is the assertion */
}

static void test_player_draw_mid_shrink_does_not_crash(void) {
    map_init();
    Player p;
    player_init(&p);
    /* death_progress = 0.5 means half-size */
    player_draw(&p, 0, 0, 0.5f);
    TEST_ASSERT(1);
}

static void test_player_draw_fully_dead_does_not_crash(void) {
    map_init();
    Player p;
    player_init(&p);
    /* death_progress = 1.0 means radius <= 0, function should return early */
    player_draw(&p, 0, 0, 1.0f);
    TEST_ASSERT(1);
}

/* Feature 1: mouth chomp animation — draw must not crash at move_t extremes */
static void test_player_draw_chomp_closed_does_not_crash(void) {
    map_init();
    Player p;
    player_init(&p);
    p.move_t = 0.0f; /* tile boundary — sin=0, open_angle=0 → full circle */
    player_draw(&p, 0, 0, 0.0f);
    TEST_ASSERT(1);
}

static void test_player_draw_chomp_open_does_not_crash(void) {
    map_init();
    Player p;
    player_init(&p);
    p.move_t = 0.5f; /* mid-tile — sin=1, open_angle=30 → 60° gap */
    player_draw(&p, 0, 0, 0.0f);
    TEST_ASSERT(1);
}

/* Feature 2: player speed scales with level */
static void test_player_set_level_speed_level1(void) {
    Player p;
    player_init(&p);
    player_set_level_speed(&p, 1);
    TEST_ASSERT(p.speed >= 7.99f && p.speed <= 8.01f);
}

static void test_player_set_level_speed_level10(void) {
    Player p;
    player_init(&p);
    player_set_level_speed(&p, 10);
    /* 8.0 + (10-1)*0.1 = 8.9 */
    TEST_ASSERT(p.speed >= 8.89f && p.speed <= 8.91f);
}

static void test_player_set_level_speed_capped(void) {
    Player p;
    player_init(&p);
    player_set_level_speed(&p, 99);
    /* cap at 9.5f */
    TEST_ASSERT(p.speed >= 9.49f && p.speed <= 9.51f);
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
    RUN_TEST(test_extra_life_not_awarded_below_threshold);
    RUN_TEST(test_extra_life_awarded_at_1500);
    RUN_TEST(test_extra_life_not_awarded_twice_at_1500);
    RUN_TEST(test_extra_life_awarded_at_5000);
    RUN_TEST(test_extra_life_caps_at_max);
    RUN_TEST(test_extra_life_flags_reset_on_init);
    RUN_TEST(test_player_draw_alive_does_not_crash);
    RUN_TEST(test_player_draw_mid_shrink_does_not_crash);
    RUN_TEST(test_player_draw_fully_dead_does_not_crash);
    RUN_TEST(test_player_draw_chomp_closed_does_not_crash);
    RUN_TEST(test_player_draw_chomp_open_does_not_crash);
    RUN_TEST(test_player_set_level_speed_level1);
    RUN_TEST(test_player_set_level_speed_level10);
    RUN_TEST(test_player_set_level_speed_capped);
    TESTS_SUMMARY();
}
