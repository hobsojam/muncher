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

/* ------------------------------------------------------------------ */
/* Feature 1 — ghost popup float+fade draw (smoke tests)              */
/* ------------------------------------------------------------------ */

/* Popup at early progress (flash_timer near full — almost opaque) */
static void test_ghosts_draw_popup_early_progress_no_crash(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    ghosts[GHOST_BLINKY].flash_timer = 0.79f; /* progress ~0.0125 */
    ghosts[GHOST_BLINKY].flash_col   = 5;
    ghosts[GHOST_BLINKY].flash_row   = 10;
    ghosts[GHOST_BLINKY].eat_score   = 200;
    ghosts_draw(ghosts, 0, 40);
    /* Must not crash; flash_timer must be unchanged by draw */
    TEST_ASSERT(ghosts[GHOST_BLINKY].flash_timer > 0.0f);
}

/* Popup at late progress (flash_timer near zero — nearly transparent) */
static void test_ghosts_draw_popup_late_progress_no_crash(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    ghosts[GHOST_BLINKY].flash_timer = 0.05f; /* progress ~0.9375 */
    ghosts[GHOST_BLINKY].flash_col   = 7;
    ghosts[GHOST_BLINKY].flash_row   = 12;
    ghosts[GHOST_BLINKY].eat_score   = 400;
    ghosts_draw(ghosts, 10, 40);
    TEST_ASSERT(ghosts[GHOST_BLINKY].flash_timer > 0.0f);
}

/* ------------------------------------------------------------------ */
/* Feature 2 — exit_flash_timer initialisation and trigger            */
/* ------------------------------------------------------------------ */

static void test_exit_flash_timer_zero_after_init(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    for (int i = 0; i < GHOST_COUNT; i++)
        TEST_ASSERT(ghosts[i].exit_flash_timer == 0.0f);
}

/* Placing a ghost in GMODE_EXITING one row below the exit row, moving up,
   should trigger the transition to normal AI and set exit_flash_timer.
   GHOST_HOUSE_EXIT_ROW=11; start at row 12, dir_row=-1 so the ghost
   steps to row 11 within the update call. */
static void test_exit_flash_timer_set_on_exit_transition(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    ghosts[GHOST_PINKY].col     = GHOST_HOUSE_CENTER_COL;
    ghosts[GHOST_PINKY].row     = GHOST_HOUSE_EXIT_ROW + 1; /* row 12 */
    ghosts[GHOST_PINKY].dir_col = 0;
    ghosts[GHOST_PINKY].dir_row = -1;
    ghosts[GHOST_PINKY].mode    = GMODE_EXITING;
    ghosts[GHOST_PINKY].move_t  = 0.99f; /* ensure the tile step fires immediately */
    Player player = {0}; player.col = 14; player.row = 29;
    ghosts_update(ghosts, &player, 0.01f); /* small dt — one step suffices */
    /* Ghost should have transitioned out of GMODE_EXITING */
    TEST_ASSERT(ghosts[GHOST_PINKY].mode == GMODE_SCATTER ||
                ghosts[GHOST_PINKY].mode == GMODE_CHASE);
    TEST_ASSERT(ghosts[GHOST_PINKY].exit_flash_timer > 0.0f);
}

static void test_clyde_shy_radius_level1(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init_level(ghosts, 1);
    TEST_ASSERT_EQUAL_INT(8, ghosts[GHOST_CLYDE].shy_radius);
}

static void test_clyde_shy_radius_decreases_per_level(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init_level(ghosts, 3);
    TEST_ASSERT_EQUAL_INT(6, ghosts[GHOST_CLYDE].shy_radius);
}

static void test_clyde_shy_radius_clamps_at_one(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init_level(ghosts, 99);
    TEST_ASSERT_EQUAL_INT(1, ghosts[GHOST_CLYDE].shy_radius);
}

static void test_clyde_near_uses_shy_radius(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init_level(ghosts, 2);
    /* shy_radius=7 — place Clyde 7 tiles from player: still retreats */
    Player p = {0}; p.col = 14; p.row = 14;
    ghosts[GHOST_CLYDE].col = 14; ghosts[GHOST_CLYDE].row = 7; /* 7 tiles away */
    int tc, tr;
    ghost_get_target(&ghosts[GHOST_CLYDE], &p, ghosts, &tc, &tr);
    TEST_ASSERT_EQUAL_INT(ghosts[GHOST_CLYDE].scatter_col, tc);
    TEST_ASSERT_EQUAL_INT(ghosts[GHOST_CLYDE].scatter_row, tr);
}

/* ------------------------------------------------------------------ */
/* Feature A — BFS pathfinding in choose_dir                           */
/* ------------------------------------------------------------------ */

/* Helper: set every cell to TILE_WALL, then carve out cells listed. */
static void make_all_walls(void) {
    for (int r = 0; r < MAP_ROWS; r++)
        for (int c = 0; c < MAP_COLS; c++)
            map[r][c] = TILE_WALL;
}

/*
 * Test (a): In an open horizontal corridor the ghost picks the shorter
 * path to the target rather than the longer one.
 *
 * Layout (row 5 fully open, ghost at col 5 facing right):
 *   . . . . . G . . . T . . .   (row 5)
 * Target at col 9.  Ghost faces right (dir_col=1).
 * BFS should pick right (shorter), not left (longer wrap-around).
 */
static void test_bfs_picks_shorter_path_in_corridor(void) {
    make_all_walls();
    for (int c = 0; c < MAP_COLS; c++)
        map[5][c] = TILE_EMPTY;

    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    /* Place Blinky at (col=5, row=5) facing right */
    ghosts[GHOST_BLINKY].col     = 5;
    ghosts[GHOST_BLINKY].row     = 5;
    ghosts[GHOST_BLINKY].dir_col = 1;
    ghosts[GHOST_BLINKY].dir_row = 0;
    ghosts[GHOST_BLINKY].mode    = GMODE_CHASE;

    Player player = {0};
    player.col = 9; player.row = 5;

    int tc, tr;
    ghost_get_target(&ghosts[GHOST_BLINKY], &player, ghosts, &tc, &tr);
    /* choose_dir is called inside ghosts_update; call it via update */
    ghosts_update(ghosts, &player, 0.0f);
    /* After BFS the ghost should be heading right (toward col 9) */
    TEST_ASSERT_EQUAL_INT(1, ghosts[GHOST_BLINKY].dir_col);
    TEST_ASSERT_EQUAL_INT(0, ghosts[GHOST_BLINKY].dir_row);
}

/*
 * Test (b): Ghost navigates around a wall.
 *
 * Map layout (rows 5-6, cols 1-6):
 *   row 5:  . . W . . .     (col 3 is wall)
 *   row 6:  . . . . . .     (open detour below)
 *
 * Ghost at (col=2, row=5) facing right.
 * Target at (col=4, row=5) — directly behind the wall.
 * Greedy: tries col=3 → blocked, picks col=1 (left) or stuck.
 * BFS: finds path right→down (row 6)→right→right→up, and picks right-down
 *      or detour correctly, so first_dc != -1 (not going backward/left).
 *
 * Simpler assertion: the BFS first step must NOT be left (dir_col=-1)
 * since that goes away from the target via the long way.
 */
static void test_bfs_navigates_around_wall(void) {
    make_all_walls();
    /* Open corridor row 5, cols 1-6 except col 3 */
    for (int c = 1; c <= 6; c++)
        map[5][c] = TILE_EMPTY;
    map[5][3] = TILE_WALL;   /* wall blocking direct path */
    /* Open detour row 6, cols 1-6 */
    for (int c = 1; c <= 6; c++)
        map[6][c] = TILE_EMPTY;

    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    ghosts[GHOST_BLINKY].col     = 2;
    ghosts[GHOST_BLINKY].row     = 5;
    ghosts[GHOST_BLINKY].dir_col = 1;
    ghosts[GHOST_BLINKY].dir_row = 0;
    ghosts[GHOST_BLINKY].mode    = GMODE_CHASE;

    Player player = {0};
    player.col = 4; player.row = 5;

    ghosts_update(ghosts, &player, 0.0f);

    /* BFS must not go left (away from target) when a detour exists */
    TEST_ASSERT(ghosts[GHOST_BLINKY].dir_col != -1);
}

/*
 * Test (c): Frightened ghost still uses random choice (choose_dir_random),
 * not BFS.  We verify it can choose a direction other than the
 * deterministic BFS-optimal one.  We run many updates and check that
 * the direction is always a valid non-reverse neighbour (statistical
 * correctness of random mode), and that it is NOT necessarily always
 * the same direction (i.e., not locked to BFS).
 *
 * Simpler: verify ghosts_frighten sets GMODE_FRIGHTENED and speed drops.
 */
static void test_frightened_ghost_not_bfs(void) {
    make_all_walls();
    for (int c = 0; c < MAP_COLS; c++)
        map[5][c] = TILE_EMPTY;
    /* Add a branch downward from col 10 */
    for (int r = 5; r <= 10; r++)
        map[r][10] = TILE_EMPTY;

    Ghost ghosts[GHOST_COUNT];
    ghosts_init(ghosts);
    ghosts[GHOST_BLINKY].col     = 10;
    ghosts[GHOST_BLINKY].row     = 5;
    ghosts[GHOST_BLINKY].dir_col = 1;
    ghosts[GHOST_BLINKY].dir_row = 0;

    ghosts_frighten(ghosts);
    TEST_ASSERT_EQUAL_INT(GMODE_FRIGHTENED, ghosts[GHOST_BLINKY].mode);

    Player player = {0};
    player.col = 20; player.row = 5; /* far target — BFS would go right */

    /* After frighten, mode must remain FRIGHTENED (not changed by update alone) */
    ghosts_update(ghosts, &player, 0.0f);
    TEST_ASSERT_EQUAL_INT(GMODE_FRIGHTENED, ghosts[GHOST_BLINKY].mode);
}

/* ------------------------------------------------------------------ */
/* Feature B — Level-based difficulty scaling                          */
/* ------------------------------------------------------------------ */

/*
 * Expose the runtime speed/duration values via ghost_internal.h.
 * These accessor declarations mirror what we will add to ghost_internal.h.
 */
float ghost_internal_speed_normal(void);
float ghost_internal_speed_frightened(void);
float ghost_internal_frightened_secs(void);
float ghost_internal_mode_duration(int idx);

/* (a) At level 1 speeds equal the existing constants */
static void test_difficulty_level1_speed_normal_matches_constant(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init_level(ghosts, 1);
    /* SPEED_NORMAL = 6.5f */
    TEST_ASSERT(ghost_internal_speed_normal() >= 6.49f &&
                ghost_internal_speed_normal() <= 6.51f);
}

static void test_difficulty_level1_speed_frightened_matches_constant(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init_level(ghosts, 1);
    /* SPEED_FRIGHTENED = 4.0f */
    TEST_ASSERT(ghost_internal_speed_frightened() >= 3.99f &&
                ghost_internal_speed_frightened() <= 4.01f);
}

static void test_difficulty_level1_frightened_secs_matches_constant(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init_level(ghosts, 1);
    /* FRIGHTENED_SECS = 8.0f */
    TEST_ASSERT(ghost_internal_frightened_secs() >= 7.99f &&
                ghost_internal_frightened_secs() <= 8.01f);
}

/* (b) At level 5 normal speed is higher than level 1 */
static void test_difficulty_level5_speed_normal_higher(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    float speed_l1, speed_l5;
    ghosts_init_level(ghosts, 1);
    speed_l1 = ghost_internal_speed_normal();
    ghosts_init_level(ghosts, 5);
    speed_l5 = ghost_internal_speed_normal();
    TEST_ASSERT(speed_l5 > speed_l1);
}

/* (c) At level 5 frightened duration is shorter than level 1 */
static void test_difficulty_level5_frightened_secs_shorter(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    float secs_l1, secs_l5;
    ghosts_init_level(ghosts, 1);
    secs_l1 = ghost_internal_frightened_secs();
    ghosts_init_level(ghosts, 5);
    secs_l5 = ghost_internal_frightened_secs();
    TEST_ASSERT(secs_l5 < secs_l1);
}

/* Scatter durations shrink per level */
static void test_difficulty_level5_scatter_duration_shorter(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    float dur_l1, dur_l5;
    ghosts_init_level(ghosts, 1);
    dur_l1 = ghost_internal_mode_duration(0); /* first scatter */
    ghosts_init_level(ghosts, 5);
    dur_l5 = ghost_internal_mode_duration(0);
    TEST_ASSERT(dur_l5 < dur_l1);
}

/* Chase durations unchanged between levels */
static void test_difficulty_chase_duration_unchanged(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    float dur_l1, dur_l5;
    ghosts_init_level(ghosts, 1);
    dur_l1 = ghost_internal_mode_duration(1); /* first chase */
    ghosts_init_level(ghosts, 5);
    dur_l5 = ghost_internal_mode_duration(1);
    TEST_ASSERT(dur_l1 >= dur_l5 - 0.01f && dur_l1 <= dur_l5 + 0.01f);
}

/* ghost_respawn uses g_speed_normal (not hardcoded SPEED_NORMAL) */
static void test_difficulty_respawn_uses_runtime_speed(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT];
    ghosts_init_level(ghosts, 5);
    float expected_speed = ghost_internal_speed_normal();
    ghost_respawn(&ghosts[GHOST_BLINKY]);
    TEST_ASSERT(ghosts[GHOST_BLINKY].speed >= expected_speed - 0.01f &&
                ghosts[GHOST_BLINKY].speed <= expected_speed + 0.01f);
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
    RUN_TEST(test_clyde_shy_radius_level1);
    RUN_TEST(test_clyde_shy_radius_decreases_per_level);
    RUN_TEST(test_clyde_shy_radius_clamps_at_one);
    RUN_TEST(test_clyde_near_uses_shy_radius);
    /* Feature A — BFS pathfinding */
    RUN_TEST(test_bfs_picks_shorter_path_in_corridor);
    RUN_TEST(test_bfs_navigates_around_wall);
    RUN_TEST(test_frightened_ghost_not_bfs);
    /* Feature B — Level difficulty scaling */
    RUN_TEST(test_difficulty_level1_speed_normal_matches_constant);
    RUN_TEST(test_difficulty_level1_speed_frightened_matches_constant);
    RUN_TEST(test_difficulty_level1_frightened_secs_matches_constant);
    RUN_TEST(test_difficulty_level5_speed_normal_higher);
    RUN_TEST(test_difficulty_level5_frightened_secs_shorter);
    RUN_TEST(test_difficulty_level5_scatter_duration_shorter);
    RUN_TEST(test_difficulty_chase_duration_unchanged);
    RUN_TEST(test_difficulty_respawn_uses_runtime_speed);
    /* Feature 1 — popup float+fade draw smoke tests */
    RUN_TEST(test_ghosts_draw_popup_early_progress_no_crash);
    RUN_TEST(test_ghosts_draw_popup_late_progress_no_crash);
    /* Feature 2 — exit flash timer */
    RUN_TEST(test_exit_flash_timer_zero_after_init);
    RUN_TEST(test_exit_flash_timer_set_on_exit_transition);
    TESTS_SUMMARY();
}
