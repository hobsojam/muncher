#include "test_framework.h"
#include "map.h"
#include "ghost.h"
#include "player.h"
#include "lives.h"

/* ------------------------------------------------------------------ */
/* player_init / player_respawn                                        */
/* ------------------------------------------------------------------ */

static void test_player_init_lives(void) {
    map_init();
    Player p; player_init(&p);
    TEST_ASSERT_EQUAL_INT(3, p.lives);
}

static void test_player_init_score(void) {
    map_init();
    Player p; player_init(&p);
    TEST_ASSERT_EQUAL_INT(0, p.score);
}

static void test_respawn_resets_position(void) {
    map_init();
    Player p; player_init(&p);
    p.col = 1; p.row = 1;
    player_respawn(&p);
    TEST_ASSERT_EQUAL_INT(14, p.col);
    TEST_ASSERT_EQUAL_INT(29, p.row);
}

static void test_respawn_preserves_score(void) {
    map_init();
    Player p; player_init(&p);
    p.score = 99;
    player_respawn(&p);
    TEST_ASSERT_EQUAL_INT(99, p.score);
}

static void test_respawn_preserves_lives(void) {
    map_init();
    Player p; player_init(&p);
    p.lives = 2;
    player_respawn(&p);
    TEST_ASSERT_EQUAL_INT(2, p.lives);
}

/* ------------------------------------------------------------------ */
/* handle_ghost_collision — lives decrement                            */
/* ------------------------------------------------------------------ */

static void test_normal_ghost_decrements_lives(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    ghosts[GHOST_BLINKY].col = 5; ghosts[GHOST_BLINKY].row = 10;

    Player p; player_init(&p);
    p.col = 5; p.row = 10;
    int game_over = 0;
    handle_ghost_collision(&p, ghosts, &game_over);

    TEST_ASSERT_EQUAL_INT(2, p.lives);
    TEST_ASSERT_EQUAL_INT(0, game_over);
}

static void test_death_respawns_player(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    ghosts[GHOST_BLINKY].col = 5; ghosts[GHOST_BLINKY].row = 10;

    Player p; player_init(&p);
    p.col = 5; p.row = 10;
    int game_over = 0;
    handle_ghost_collision(&p, ghosts, &game_over);

    TEST_ASSERT_EQUAL_INT(14, p.col);
    TEST_ASSERT_EQUAL_INT(29, p.row);
}

static void test_last_life_sets_game_over(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    ghosts[GHOST_BLINKY].col = 5; ghosts[GHOST_BLINKY].row = 10;

    Player p; player_init(&p);
    p.col = 5; p.row = 10; p.lives = 1;
    int game_over = 0;
    handle_ghost_collision(&p, ghosts, &game_over);

    TEST_ASSERT_EQUAL_INT(1, game_over);
    TEST_ASSERT_EQUAL_INT(0, p.lives);
}

static void test_lives_never_go_negative(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    ghosts[GHOST_BLINKY].col = 5; ghosts[GHOST_BLINKY].row = 10;

    Player p; player_init(&p);
    p.col = 5; p.row = 10; p.lives = 0;
    int game_over = 0;
    handle_ghost_collision(&p, ghosts, &game_over);

    TEST_ASSERT(p.lives >= 0);
    TEST_ASSERT_EQUAL_INT(1, game_over);
}

static void test_frightened_ghost_no_life_lost(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    ghosts[GHOST_BLINKY].mode = GMODE_FRIGHTENED;
    ghosts[GHOST_BLINKY].col  = 5; ghosts[GHOST_BLINKY].row = 10;

    Player p; player_init(&p);
    p.col = 5; p.row = 10;
    int game_over = 0;
    handle_ghost_collision(&p, ghosts, &game_over);

    TEST_ASSERT_EQUAL_INT(3, p.lives);
    TEST_ASSERT_EQUAL_INT(0, game_over);
}

static void test_no_overlap_no_life_lost(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);

    Player p; player_init(&p);
    p.col = 14; p.row = 29;
    int game_over = 0;
    handle_ghost_collision(&p, ghosts, &game_over);

    TEST_ASSERT_EQUAL_INT(3, p.lives);
    TEST_ASSERT_EQUAL_INT(0, game_over);
}

static void test_chase_ghost_decrements_lives(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    ghosts[GHOST_PINKY].mode = GMODE_CHASE;
    ghosts[GHOST_PINKY].col  = 5; ghosts[GHOST_PINKY].row = 10;

    Player p; player_init(&p);
    p.col = 5; p.row = 10;
    int game_over = 0;
    handle_ghost_collision(&p, ghosts, &game_over);

    TEST_ASSERT_EQUAL_INT(2, p.lives);
}

int main(void) {
    RUN_TEST(test_player_init_lives);
    RUN_TEST(test_player_init_score);
    RUN_TEST(test_respawn_resets_position);
    RUN_TEST(test_respawn_preserves_score);
    RUN_TEST(test_respawn_preserves_lives);
    RUN_TEST(test_normal_ghost_decrements_lives);
    RUN_TEST(test_death_respawns_player);
    RUN_TEST(test_last_life_sets_game_over);
    RUN_TEST(test_lives_never_go_negative);
    RUN_TEST(test_frightened_ghost_no_life_lost);
    RUN_TEST(test_no_overlap_no_life_lost);
    RUN_TEST(test_chase_ghost_decrements_lives);
    TESTS_SUMMARY();
}
