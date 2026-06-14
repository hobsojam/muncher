#include "test_framework.h"
#include "map.h"
#include "ghost.h"
#include "player.h"
#include "collision.h"

/* ------------------------------------------------------------------ */
/* player_on_ghost                                                      */
/* ------------------------------------------------------------------ */

static void test_player_on_ghost_true(void) {
    map_init();
    Player p; player_init(&p);
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    p.col = 5; p.row = 7;
    ghosts[GHOST_BLINKY].col = 5; ghosts[GHOST_BLINKY].row = 7;
    TEST_ASSERT_EQUAL_INT(1, player_on_ghost(&p, &ghosts[GHOST_BLINKY]));
}

static void test_player_on_ghost_false_col(void) {
    map_init();
    Player p; player_init(&p);
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    p.col = 5; p.row = 7;
    ghosts[GHOST_BLINKY].col = 6; ghosts[GHOST_BLINKY].row = 7;
    TEST_ASSERT_EQUAL_INT(0, player_on_ghost(&p, &ghosts[GHOST_BLINKY]));
}

static void test_player_on_ghost_false_row(void) {
    map_init();
    Player p; player_init(&p);
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    p.col = 5; p.row = 7;
    ghosts[GHOST_BLINKY].col = 5; ghosts[GHOST_BLINKY].row = 8;
    TEST_ASSERT_EQUAL_INT(0, player_on_ghost(&p, &ghosts[GHOST_BLINKY]));
}

/* ------------------------------------------------------------------ */
/* handle_collision — no overlap                                        */
/* ------------------------------------------------------------------ */

static void test_no_collision_no_dead(void) {
    map_init();
    Player p; player_init(&p);
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    /* ghosts_init places ghosts at row 11; player starts at row 29 */
    handle_collision(&p, ghosts);
    TEST_ASSERT_EQUAL_INT(0, p.dead);
}

/* ------------------------------------------------------------------ */
/* handle_collision — normal ghost kills player                         */
/* ------------------------------------------------------------------ */

static void test_scatter_ghost_sets_dead(void) {
    map_init();
    Player p; player_init(&p);
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    ghosts[GHOST_BLINKY].mode = GMODE_SCATTER;
    ghosts[GHOST_BLINKY].col = p.col; ghosts[GHOST_BLINKY].row = p.row;
    handle_collision(&p, ghosts);
    TEST_ASSERT_EQUAL_INT(1, p.dead);
}

static void test_chase_ghost_sets_dead(void) {
    map_init();
    Player p; player_init(&p);
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    ghosts[GHOST_PINKY].mode = GMODE_CHASE;
    ghosts[GHOST_PINKY].col = p.col; ghosts[GHOST_PINKY].row = p.row;
    handle_collision(&p, ghosts);
    TEST_ASSERT_EQUAL_INT(1, p.dead);
}

/* ------------------------------------------------------------------ */
/* handle_collision — frightened ghost gives score, no death            */
/* ------------------------------------------------------------------ */

static void test_frightened_ghost_adds_score(void) {
    map_init();
    Player p; player_init(&p);
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    ghosts[GHOST_BLINKY].mode = GMODE_FRIGHTENED;
    ghosts[GHOST_BLINKY].col = p.col; ghosts[GHOST_BLINKY].row = p.row;
    handle_collision(&p, ghosts);
    TEST_ASSERT_EQUAL_INT(200, p.score);
}

static void test_frightened_ghost_no_dead(void) {
    map_init();
    Player p; player_init(&p);
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    ghosts[GHOST_BLINKY].mode = GMODE_FRIGHTENED;
    ghosts[GHOST_BLINKY].col = p.col; ghosts[GHOST_BLINKY].row = p.row;
    handle_collision(&p, ghosts);
    TEST_ASSERT_EQUAL_INT(0, p.dead);
}

static void test_frightened_ghost_respawns(void) {
    map_init();
    Player p; player_init(&p);
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    ghosts[GHOST_BLINKY].mode = GMODE_FRIGHTENED;
    ghosts[GHOST_BLINKY].col = p.col; ghosts[GHOST_BLINKY].row = p.row;
    handle_collision(&p, ghosts);
    /* after respawn, ghost must not still be on the player's tile */
    int still_on_player = (ghosts[GHOST_BLINKY].col == p.col && ghosts[GHOST_BLINKY].row == p.row);
    TEST_ASSERT_EQUAL_INT(0, still_on_player);
}

/* ------------------------------------------------------------------ */
/* handle_collision — multiple ghosts                                   */
/* ------------------------------------------------------------------ */

static void test_scatter_ghost_overrides_score(void) {
    map_init();
    Player p; player_init(&p);
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    /* Blinky scatter on player → dead */
    ghosts[GHOST_BLINKY].mode = GMODE_SCATTER;
    ghosts[GHOST_BLINKY].col = p.col; ghosts[GHOST_BLINKY].row = p.row;
    /* Pinky frightened also on player — should still give score */
    ghosts[GHOST_PINKY].mode = GMODE_FRIGHTENED;
    ghosts[GHOST_PINKY].col = p.col; ghosts[GHOST_PINKY].row = p.row;
    handle_collision(&p, ghosts);
    TEST_ASSERT_EQUAL_INT(1, p.dead);
}

static void test_two_frightened_ghosts_score(void) {
    map_init();
    Player p; player_init(&p);
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    ghosts[GHOST_BLINKY].mode = GMODE_FRIGHTENED;
    ghosts[GHOST_BLINKY].col = p.col; ghosts[GHOST_BLINKY].row = p.row;
    ghosts[GHOST_PINKY].mode = GMODE_FRIGHTENED;
    ghosts[GHOST_PINKY].col = p.col; ghosts[GHOST_PINKY].row = p.row;
    handle_collision(&p, ghosts);
    TEST_ASSERT_EQUAL_INT(400, p.score);
}

/* ------------------------------------------------------------------ */
/* player_init — dead starts at 0                                       */
/* ------------------------------------------------------------------ */

static void test_player_init_dead_zero(void) {
    map_init();
    Player p; player_init(&p);
    TEST_ASSERT_EQUAL_INT(0, p.dead);
}

int main(void) {
    RUN_TEST(test_player_on_ghost_true);
    RUN_TEST(test_player_on_ghost_false_col);
    RUN_TEST(test_player_on_ghost_false_row);
    RUN_TEST(test_no_collision_no_dead);
    RUN_TEST(test_scatter_ghost_sets_dead);
    RUN_TEST(test_chase_ghost_sets_dead);
    RUN_TEST(test_frightened_ghost_adds_score);
    RUN_TEST(test_frightened_ghost_no_dead);
    RUN_TEST(test_frightened_ghost_respawns);
    RUN_TEST(test_scatter_ghost_overrides_score);
    RUN_TEST(test_two_frightened_ghosts_score);
    RUN_TEST(test_player_init_dead_zero);
    TESTS_SUMMARY();
}
