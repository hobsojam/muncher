#include "test_framework.h"
#include "map.h"
#include "ghost.h"
#include "ghost_internal.h"

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
    TEST_ASSERT_EQUAL_INT(25, ghost_dist_sq(0, 0, 3, 4));
    TEST_ASSERT_EQUAL_INT(0,  ghost_dist_sq(5, 5, 5, 5));
    TEST_ASSERT_EQUAL_INT(2,  ghost_dist_sq(0, 0, 1, 1));
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
    TEST_ASSERT_EQUAL_INT(1, ghost_can_enter(-1, GHOST_TUNNEL_ROW));
    TEST_ASSERT_EQUAL_INT(1, ghost_can_enter(MAP_COLS, GHOST_TUNNEL_ROW));
}

static void test_ghost_can_enter_oob_non_tunnel(void) {
    map_init();
    TEST_ASSERT_EQUAL_INT(0, ghost_can_enter(-1, 0));
    TEST_ASSERT_EQUAL_INT(0, ghost_can_enter(0, -1));
    TEST_ASSERT_EQUAL_INT(0, ghost_can_enter(0, MAP_ROWS));
}

static void test_frighten_flash_off_when_timer_high(void) {
    TEST_ASSERT_EQUAL_INT(0, ghost_frighten_flashing(3.0f));
}

static void test_frighten_flash_on_when_timer_low_even_period(void) {
    /* t=1.6: (int)(1.6*5)=8, 8%2=0 → flashing */
    TEST_ASSERT_EQUAL_INT(1, ghost_frighten_flashing(1.6f));
}

static void test_frighten_flash_off_when_timer_low_odd_period(void) {
    /* t=1.8: (int)(1.8*5)=9, 9%2=1 → not flashing */
    TEST_ASSERT_EQUAL_INT(0, ghost_frighten_flashing(1.8f));
}

static void test_frighten_flash_off_above_threshold(void) {
    TEST_ASSERT_EQUAL_INT(0, ghost_frighten_flashing(GHOST_FRIGHTEN_FLASH_SECS + 0.1f));
}

static void test_internal_fright_timer_zero_after_init(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    TEST_ASSERT(ghost_internal_fright_timer() == 0.0f);
}

static void test_internal_fright_timer_set_after_frighten(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    ghosts_frighten(ghosts);
    TEST_ASSERT(ghost_internal_fright_timer() > 0.0f);
}

static void test_ghost_can_enter_door_blocked(void) {
    map_init();
    TEST_ASSERT_EQUAL_INT(0, ghost_can_enter(GHOST_HOUSE_CENTER_COL, 12));
}

/* ------------------------------------------------------------------ */
/* ghosts_init                                                          */
/* ------------------------------------------------------------------ */

static void test_ghosts_init_blinky_outside(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    TEST_ASSERT_EQUAL_INT(GHOST_HOUSE_CENTER_COL, ghosts[GHOST_BLINKY].col);
    TEST_ASSERT_EQUAL_INT(GHOST_HOUSE_EXIT_ROW,   ghosts[GHOST_BLINKY].row);
    TEST_ASSERT_EQUAL_INT(GMODE_SCATTER,           ghosts[GHOST_BLINKY].mode);
}

static void test_ghosts_init_others_in_house(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    TEST_ASSERT_EQUAL_INT(GMODE_HOUSE, ghosts[GHOST_PINKY].mode);
    TEST_ASSERT_EQUAL_INT(GMODE_HOUSE, ghosts[GHOST_INKY].mode);
    TEST_ASSERT_EQUAL_INT(GMODE_HOUSE, ghosts[GHOST_CLYDE].mode);
    /* All inside house rows */
    TEST_ASSERT(ghosts[GHOST_PINKY].row >= 13 && ghosts[GHOST_PINKY].row <= 15);
    TEST_ASSERT(ghosts[GHOST_INKY].row  >= 13 && ghosts[GHOST_INKY].row  <= 15);
    TEST_ASSERT(ghosts[GHOST_CLYDE].row >= 13 && ghosts[GHOST_CLYDE].row <= 15);
}

static void test_ghosts_init_release_timers_ordered(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    TEST_ASSERT(ghosts[GHOST_PINKY].release_timer  > 0.0f);
    TEST_ASSERT(ghosts[GHOST_INKY].release_timer   > ghosts[GHOST_PINKY].release_timer);
    TEST_ASSERT(ghosts[GHOST_CLYDE].release_timer  > ghosts[GHOST_INKY].release_timer);
}

/* ------------------------------------------------------------------ */
/* Ghost house bounce                                                   */
/* ------------------------------------------------------------------ */

static void test_house_ghost_stays_in_house_rows(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    Player player = {0}; player.col = 14; player.row = 29;
    /* Large release timers: ghosts stay in house for many small dt ticks */
    ghosts[GHOST_PINKY].release_timer = 999.0f;
    for (int tick = 0; tick < 40; tick++) {
        ghosts_update(ghosts, &player, 0.1f);
        TEST_ASSERT(ghosts[GHOST_PINKY].row >= 13);
        TEST_ASSERT(ghosts[GHOST_PINKY].row <= 15);
    }
}

/* ------------------------------------------------------------------ */
/* Ghost house exit                                                     */
/* ------------------------------------------------------------------ */

static void test_house_ghost_transitions_to_exiting_on_release(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    ghosts[GHOST_PINKY].release_timer = 0.1f;
    Player player = {0}; player.col = 14; player.row = 29;
    ghosts_update(ghosts, &player, 0.2f);
    TEST_ASSERT(ghosts[GHOST_PINKY].mode == GMODE_EXITING ||
                ghosts[GHOST_PINKY].mode == GMODE_SCATTER  ||
                ghosts[GHOST_PINKY].mode == GMODE_CHASE);
}

static void test_exiting_ghost_reaches_exit_row(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    /* Place Pinky at center col, inside house, already exiting */
    ghosts[GHOST_PINKY].col     = GHOST_HOUSE_CENTER_COL;
    ghosts[GHOST_PINKY].row     = 14;
    ghosts[GHOST_PINKY].dir_col = 0;
    ghosts[GHOST_PINKY].dir_row = -1;
    ghosts[GHOST_PINKY].mode    = GMODE_EXITING;
    Player player = {0}; player.col = 14; player.row = 29;
    /* 1 second at SPEED_NORMAL=6.5 covers 6+ tiles — enough for rows 14→11 */
    ghosts_update(ghosts, &player, 1.0f);
    TEST_ASSERT_EQUAL_INT(GHOST_HOUSE_EXIT_ROW, ghosts[GHOST_PINKY].row);
    TEST_ASSERT(ghosts[GHOST_PINKY].mode == GMODE_SCATTER ||
                ghosts[GHOST_PINKY].mode == GMODE_CHASE);
}

static void test_exiting_ghost_navigates_from_side(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    /* Clyde starts at col 16 — must navigate left to col 13 then up */
    ghosts[GHOST_CLYDE].col     = 16;
    ghosts[GHOST_CLYDE].row     = 14;
    ghosts[GHOST_CLYDE].dir_col = -1;
    ghosts[GHOST_CLYDE].dir_row = 0;
    ghosts[GHOST_CLYDE].mode    = GMODE_EXITING;
    Player player = {0}; player.col = 14; player.row = 29;
    ghosts_update(ghosts, &player, 2.0f);
    TEST_ASSERT_EQUAL_INT(GHOST_HOUSE_EXIT_ROW, ghosts[GHOST_CLYDE].row);
    TEST_ASSERT(ghosts[GHOST_CLYDE].mode == GMODE_SCATTER ||
                ghosts[GHOST_CLYDE].mode == GMODE_CHASE);
}

/* ------------------------------------------------------------------ */
/* frighten skips house / exiting ghosts                               */
/* ------------------------------------------------------------------ */

static void test_frighten_sets_blinky_only(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    ghosts_frighten(ghosts);
    TEST_ASSERT_EQUAL_INT(GMODE_FRIGHTENED, ghosts[GHOST_BLINKY].mode);
    TEST_ASSERT_EQUAL_INT(GMODE_HOUSE,      ghosts[GHOST_PINKY].mode);
    TEST_ASSERT_EQUAL_INT(GMODE_HOUSE,      ghosts[GHOST_INKY].mode);
    TEST_ASSERT_EQUAL_INT(GMODE_HOUSE,      ghosts[GHOST_CLYDE].mode);
}

static void test_frighten_skips_exiting_ghost(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    ghosts[GHOST_PINKY].mode = GMODE_EXITING;
    ghosts_frighten(ghosts);
    TEST_ASSERT_EQUAL_INT(GMODE_EXITING, ghosts[GHOST_PINKY].mode);
}

static void test_frighten_reverses_blinky_direction(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    int dc = ghosts[GHOST_BLINKY].dir_col;
    int dr = ghosts[GHOST_BLINKY].dir_row;
    ghosts_frighten(ghosts);
    TEST_ASSERT_EQUAL_INT(-dc, ghosts[GHOST_BLINKY].dir_col);
    TEST_ASSERT_EQUAL_INT(-dr, ghosts[GHOST_BLINKY].dir_row);
}

/* ------------------------------------------------------------------ */
/* ghost_respawn — puts ghost back in house                            */
/* ------------------------------------------------------------------ */

static void test_ghost_respawn_puts_in_house(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    ghosts[GHOST_BLINKY].col = 7; ghosts[GHOST_BLINKY].row = 5;
    ghost_respawn(&ghosts[GHOST_BLINKY]);
    TEST_ASSERT_EQUAL_INT(GMODE_HOUSE, ghosts[GHOST_BLINKY].mode);
    TEST_ASSERT(ghosts[GHOST_BLINKY].row >= 13 && ghosts[GHOST_BLINKY].row <= 15);
    TEST_ASSERT(ghosts[GHOST_BLINKY].release_timer > 0.0f);
}

static void test_ghost_respawn_sets_flash_timer(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    ghost_respawn(&ghosts[GHOST_BLINKY]);
    TEST_ASSERT(ghosts[GHOST_BLINKY].flash_timer > 0.0f);
}

static void test_ghost_respawn_captures_position(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    ghosts[GHOST_BLINKY].col = 7; ghosts[GHOST_BLINKY].row = 15;
    ghost_respawn(&ghosts[GHOST_BLINKY]);
    TEST_ASSERT_EQUAL_INT(7,  ghosts[GHOST_BLINKY].flash_col);
    TEST_ASSERT_EQUAL_INT(15, ghosts[GHOST_BLINKY].flash_row);
}

/* ------------------------------------------------------------------ */
/* scatter / chase target                                               */
/* ------------------------------------------------------------------ */

static void test_scatter_target_returns_scatter_pos(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    Player player = {0};
    player.col = 14; player.row = 29;
    int tc;
    int tr;
    ghost_get_target(&ghosts[GHOST_BLINKY], &player, ghosts, &tc, &tr);
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
    int tc;
    int tr;
    ghost_get_target(&ghosts[GHOST_BLINKY], &player, ghosts, &tc, &tr);
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
    int tc;
    int tr;
    ghost_get_target(&ghosts[GHOST_PINKY], &player, ghosts, &tc, &tr);
    TEST_ASSERT_EQUAL_INT(14, tc);
    TEST_ASSERT_EQUAL_INT(20, tr);
}

static void test_clyde_far_targets_player(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    ghosts[GHOST_CLYDE].mode = GMODE_CHASE;
    ghosts[GHOST_CLYDE].col = 1; ghosts[GHOST_CLYDE].row = 1;
    Player player = {0};
    player.col = 14; player.row = 20;
    int tc;
    int tr;
    ghost_get_target(&ghosts[GHOST_CLYDE], &player, ghosts, &tc, &tr);
    TEST_ASSERT_EQUAL_INT(14, tc);
    TEST_ASSERT_EQUAL_INT(20, tr);
}

static void test_clyde_near_targets_scatter(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    ghosts[GHOST_CLYDE].mode = GMODE_CHASE;
    ghosts[GHOST_CLYDE].col = 14; ghosts[GHOST_CLYDE].row = 20;
    Player player = {0};
    player.col = 14; player.row = 20;
    int tc;
    int tr;
    ghost_get_target(&ghosts[GHOST_CLYDE], &player, ghosts, &tc, &tr);
    TEST_ASSERT_EQUAL_INT(ghosts[GHOST_CLYDE].scatter_col, tc);
    TEST_ASSERT_EQUAL_INT(ghosts[GHOST_CLYDE].scatter_row, tr);
}

/* ------------------------------------------------------------------ */
/* movement                                                             */
/* ------------------------------------------------------------------ */

static void make_ghost_test_corridor(void) {
    for (int r = 0; r < MAP_ROWS; r++)
        for (int c = 0; c < MAP_COLS; c++)
            map[r][c] = TILE_WALL;
    for (int c = 0; c < MAP_COLS; c++)
        map[11][c] = TILE_EMPTY;
}

static void test_large_dt_consumes_multiple_ghost_tiles(void) {
    map_init();
    make_ghost_test_corridor();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    Player player = {0};
    player.col = 20; player.row = 11;
    ghosts[GHOST_BLINKY].move_t = 0.75f;
    ghosts_update(ghosts, &player, 0.25f);
    TEST_ASSERT_EQUAL_INT(11, ghosts[GHOST_BLINKY].col);
    TEST_ASSERT_EQUAL_INT(11, ghosts[GHOST_BLINKY].row);
    TEST_ASSERT(ghosts[GHOST_BLINKY].move_t < 1.0f);
}

static void test_very_large_dt_keeps_ghost_move_t_bounded(void) {
    map_init();
    make_ghost_test_corridor();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    Player player = {0};
    player.col = 20; player.row = 11;
    ghosts_update(ghosts, &player, 10.0f);
    TEST_ASSERT(ghosts[GHOST_BLINKY].col != GHOST_HOUSE_CENTER_COL);
    TEST_ASSERT(ghosts[GHOST_BLINKY].move_t < 1.0f);
}

/* ------------------------------------------------------------------ */
/* flash timer                                                          */
/* ------------------------------------------------------------------ */

static void test_ghosts_init_clears_flash_timer(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    for (int i = 0; i < GHOST_COUNT; i++)
        TEST_ASSERT_EQUAL_INT(0, (int)ghosts[i].flash_timer);
}

static void test_ghosts_update_ticks_flash_timer(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    ghosts[GHOST_BLINKY].flash_timer = 0.8f;
    Player player = {0}; player.col = 14; player.row = 29;
    ghosts_update(ghosts, &player, 0.1f);
    TEST_ASSERT(ghosts[GHOST_BLINKY].flash_timer < 0.8f);
}

static void test_flash_timer_does_not_go_negative(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    ghosts[GHOST_BLINKY].flash_timer = 0.05f;
    Player player = {0}; player.col = 14; player.row = 29;
    ghosts_update(ghosts, &player, 0.5f);
    TEST_ASSERT(ghosts[GHOST_BLINKY].flash_timer >= 0.0f);
}

static void test_ghosts_draw_flash_popup(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    ghosts[GHOST_BLINKY].flash_timer = 0.5f;
    ghosts[GHOST_BLINKY].flash_col   = 5;
    ghosts[GHOST_BLINKY].flash_row   = 10;
    ghosts_draw(ghosts, 0, 40);
    TEST_ASSERT(ghosts[GHOST_BLINKY].flash_timer > 0.0f);
}

int main(void) {
    RUN_TEST(test_frighten_flash_off_when_timer_high);
    RUN_TEST(test_frighten_flash_on_when_timer_low_even_period);
    RUN_TEST(test_frighten_flash_off_when_timer_low_odd_period);
    RUN_TEST(test_frighten_flash_off_above_threshold);
    RUN_TEST(test_internal_fright_timer_zero_after_init);
    RUN_TEST(test_internal_fright_timer_set_after_frighten);
    RUN_TEST(test_ghost_wrap_col_negative);
    RUN_TEST(test_ghost_wrap_col_overflow);
    RUN_TEST(test_ghost_wrap_col_normal);
    RUN_TEST(test_dist_sq);
    RUN_TEST(test_ghost_can_enter_wall);
    RUN_TEST(test_ghost_can_enter_open);
    RUN_TEST(test_ghost_can_enter_tunnel_oob);
    RUN_TEST(test_ghost_can_enter_oob_non_tunnel);
    RUN_TEST(test_ghost_can_enter_door_blocked);
    RUN_TEST(test_ghosts_init_blinky_outside);
    RUN_TEST(test_ghosts_init_others_in_house);
    RUN_TEST(test_ghosts_init_release_timers_ordered);
    RUN_TEST(test_house_ghost_stays_in_house_rows);
    RUN_TEST(test_house_ghost_transitions_to_exiting_on_release);
    RUN_TEST(test_exiting_ghost_reaches_exit_row);
    RUN_TEST(test_exiting_ghost_navigates_from_side);
    RUN_TEST(test_frighten_sets_blinky_only);
    RUN_TEST(test_frighten_skips_exiting_ghost);
    RUN_TEST(test_frighten_reverses_blinky_direction);
    RUN_TEST(test_ghost_respawn_puts_in_house);
    RUN_TEST(test_ghost_respawn_sets_flash_timer);
    RUN_TEST(test_ghost_respawn_captures_position);
    RUN_TEST(test_scatter_target_returns_scatter_pos);
    RUN_TEST(test_blinky_chase_targets_player);
    RUN_TEST(test_pinky_chase_targets_4_ahead);
    RUN_TEST(test_clyde_far_targets_player);
    RUN_TEST(test_clyde_near_targets_scatter);
    RUN_TEST(test_large_dt_consumes_multiple_ghost_tiles);
    RUN_TEST(test_very_large_dt_keeps_ghost_move_t_bounded);
    RUN_TEST(test_ghosts_init_clears_flash_timer);
    RUN_TEST(test_ghosts_update_ticks_flash_timer);
    RUN_TEST(test_flash_timer_does_not_go_negative);
    RUN_TEST(test_ghosts_draw_flash_popup);
    TESTS_SUMMARY();
}
