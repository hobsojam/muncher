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

static void test_respawn_clears_dead(void) {
    map_init();
    Player p; player_init(&p);
    p.dead = 1;
    player_respawn(&p);
    TEST_ASSERT_EQUAL_INT(0, p.dead);
}

/* ------------------------------------------------------------------ */
/* handle_ghost_collision — reacts to p->dead, not ghost positions     */
/* ------------------------------------------------------------------ */

static void test_dead_decrements_lives(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    Player p; player_init(&p);
    p.dead = 1;
    int game_over = 0;
    handle_ghost_collision(&p, ghosts, &game_over);
    TEST_ASSERT_EQUAL_INT(2, p.lives);
    TEST_ASSERT_EQUAL_INT(0, game_over);
}

static void test_dead_respawns_player(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    Player p; player_init(&p);
    p.col = 5; p.row = 5;
    p.dead = 1;
    int game_over = 0;
    handle_ghost_collision(&p, ghosts, &game_over);
    TEST_ASSERT_EQUAL_INT(14, p.col);
    TEST_ASSERT_EQUAL_INT(29, p.row);
}

static void test_last_life_sets_game_over(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    Player p; player_init(&p);
    p.lives = 1; p.dead = 1;
    int game_over = 0;
    handle_ghost_collision(&p, ghosts, &game_over);
    TEST_ASSERT_EQUAL_INT(1, game_over);
    TEST_ASSERT_EQUAL_INT(0, p.lives);
}

static void test_lives_never_go_negative(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    Player p; player_init(&p);
    p.lives = 0; p.dead = 1;
    int game_over = 0;
    handle_ghost_collision(&p, ghosts, &game_over);
    TEST_ASSERT(p.lives >= 0);
    TEST_ASSERT_EQUAL_INT(1, game_over);
}

static void test_not_dead_no_life_lost(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    Player p; player_init(&p);
    /* p->dead is 0 from player_init — no collision signalled */
    int game_over = 0;
    handle_ghost_collision(&p, ghosts, &game_over);
    TEST_ASSERT_EQUAL_INT(3, p.lives);
    TEST_ASSERT_EQUAL_INT(0, game_over);
}

static void test_dead_cleared_after_collision(void) {
    map_init();
    Ghost ghosts[GHOST_COUNT]; ghosts_init(ghosts);
    Player p; player_init(&p);
    p.dead = 1;
    int game_over = 0;
    handle_ghost_collision(&p, ghosts, &game_over);
    TEST_ASSERT_EQUAL_INT(0, p.dead);
}

int main(void) {
    RUN_TEST(test_player_init_lives);
    RUN_TEST(test_player_init_score);
    RUN_TEST(test_respawn_resets_position);
    RUN_TEST(test_respawn_preserves_score);
    RUN_TEST(test_respawn_preserves_lives);
    RUN_TEST(test_respawn_clears_dead);
    RUN_TEST(test_dead_decrements_lives);
    RUN_TEST(test_dead_respawns_player);
    RUN_TEST(test_last_life_sets_game_over);
    RUN_TEST(test_lives_never_go_negative);
    RUN_TEST(test_not_dead_no_life_lost);
    RUN_TEST(test_dead_cleared_after_collision);
    TESTS_SUMMARY();
}
