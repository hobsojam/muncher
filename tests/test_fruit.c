#include "test_framework.h"
#include "map.h"
#include "player.h"
#include "fruit.h"

/* ------------------------------------------------------------------ */
/* fruit_init                                                           */
/* ------------------------------------------------------------------ */

static void test_fruit_init_not_active(void) {
    Fruit f;
    fruit_init(&f);
    TEST_ASSERT_EQUAL_INT(0, f.active);
}

static void test_fruit_init_not_eaten(void) {
    Fruit f;
    fruit_init(&f);
    TEST_ASSERT_EQUAL_INT(0, f.eaten);
}

static void test_fruit_init_position(void) {
    Fruit f;
    fruit_init(&f);
    TEST_ASSERT_EQUAL_INT(FRUIT_COL, f.col);
    TEST_ASSERT_EQUAL_INT(FRUIT_ROW, f.row);
}

static void test_fruit_init_score(void) {
    Fruit f;
    fruit_init(&f);
    TEST_ASSERT_EQUAL_INT(100, f.score);
}

/* ------------------------------------------------------------------ */
/* fruit_update — spawn trigger                                         */
/* ------------------------------------------------------------------ */

static void test_fruit_spawns_at_half_dots(void) {
    map_init();
    Fruit f; fruit_init(&f);
    Player p; player_init(&p);
    int total = 100;
    fruit_update(&f, &p, total / 2, total, 0.0f);
    TEST_ASSERT_EQUAL_INT(1, f.active);
}

static void test_fruit_no_spawn_before_half(void) {
    map_init();
    Fruit f; fruit_init(&f);
    Player p; player_init(&p);
    int total = 100;
    fruit_update(&f, &p, total / 2 + 1, total, 0.0f);
    TEST_ASSERT_EQUAL_INT(0, f.active);
}

static void test_fruit_no_spawn_when_zero_total(void) {
    map_init();
    Fruit f; fruit_init(&f);
    Player p; player_init(&p);
    fruit_update(&f, &p, 0, 0, 0.0f);
    TEST_ASSERT_EQUAL_INT(0, f.active);
}

/* ------------------------------------------------------------------ */
/* fruit_update — player collision                                      */
/* ------------------------------------------------------------------ */

static void test_fruit_player_collect_adds_score(void) {
    map_init();
    Fruit f; fruit_init(&f);
    Player p; player_init(&p);
    p.col = FRUIT_COL; p.row = FRUIT_ROW;
    f.active = 1; f.timer = 5.0f;
    fruit_update(&f, &p, 0, 100, 0.1f);
    TEST_ASSERT_EQUAL_INT(100, p.score);
}

static void test_fruit_player_collect_deactivates(void) {
    map_init();
    Fruit f; fruit_init(&f);
    Player p; player_init(&p);
    p.col = FRUIT_COL; p.row = FRUIT_ROW;
    f.active = 1; f.timer = 5.0f;
    fruit_update(&f, &p, 0, 100, 0.1f);
    TEST_ASSERT_EQUAL_INT(0, f.active);
    TEST_ASSERT_EQUAL_INT(1, f.eaten);
}

static void test_fruit_no_score_when_eaten(void) {
    map_init();
    Fruit f; fruit_init(&f);
    Player p; player_init(&p);
    p.col = FRUIT_COL; p.row = FRUIT_ROW;
    f.active = 0;
    f.eaten  = 1;
    fruit_update(&f, &p, 0, 100, 0.1f);
    TEST_ASSERT_EQUAL_INT(0, p.score);
}

/* ------------------------------------------------------------------ */
/* fruit_update — timer expiry                                          */
/* ------------------------------------------------------------------ */

static void test_fruit_expires_after_timer(void) {
    map_init();
    Fruit f; fruit_init(&f);
    Player p; player_init(&p);
    f.active = 1; f.timer = 0.05f;
    fruit_update(&f, &p, 0, 100, 0.1f);
    TEST_ASSERT_EQUAL_INT(0, f.active);
}

static void test_fruit_no_respawn_after_eaten(void) {
    map_init();
    Fruit f; fruit_init(&f);
    Player p; player_init(&p);
    f.eaten = 1;
    fruit_update(&f, &p, 50, 100, 0.0f);
    TEST_ASSERT_EQUAL_INT(0, f.active);
}

/* ------------------------------------------------------------------ */
/* fruit_update — score popup                                           */
/* ------------------------------------------------------------------ */

static void test_fruit_popup_timer_set_on_eat(void) {
    map_init();
    Fruit f; fruit_init(&f);
    Player p; player_init(&p);
    f.active = 1;
    p.col = f.col; p.row = f.row;
    fruit_update(&f, &p, 0, 100, 0.0f);
    TEST_ASSERT(f.popup_timer > 0.0f);
}

static void test_fruit_popup_timer_ticks_down(void) {
    map_init();
    Fruit f; fruit_init(&f);
    Player p; player_init(&p);
    f.active = 1;
    p.col = f.col; p.row = f.row;
    fruit_update(&f, &p, 0, 100, 0.0f);
    float t0 = f.popup_timer;
    fruit_update(&f, &p, 0, 100, 0.1f);
    TEST_ASSERT(f.popup_timer < t0);
}

static void test_fruit_popup_timer_zero_on_init(void) {
    Fruit f;
    fruit_init(&f);
    TEST_ASSERT(f.popup_timer == 0.0f);
}

/* ------------------------------------------------------------------ */
/* fruit_draw — smoke tests (exercises raylib stubs for coverage)       */
/* ------------------------------------------------------------------ */

static void test_fruit_draw_active_no_crash(void) {
    Fruit f; fruit_init(&f);
    f.active = 1;
    fruit_draw(&f, 0, 0);
}

static void test_fruit_draw_popup_no_crash(void) {
    Fruit f; fruit_init(&f);
    f.popup_timer = 0.5f;
    fruit_draw(&f, 0, 0);
}

static void test_fruit_draw_inactive_no_crash(void) {
    Fruit f; fruit_init(&f);
    fruit_draw(&f, 0, 0);
}

int main(void) {
    RUN_TEST(test_fruit_init_not_active);
    RUN_TEST(test_fruit_init_not_eaten);
    RUN_TEST(test_fruit_init_position);
    RUN_TEST(test_fruit_init_score);
    RUN_TEST(test_fruit_spawns_at_half_dots);
    RUN_TEST(test_fruit_no_spawn_before_half);
    RUN_TEST(test_fruit_no_spawn_when_zero_total);
    RUN_TEST(test_fruit_player_collect_adds_score);
    RUN_TEST(test_fruit_player_collect_deactivates);
    RUN_TEST(test_fruit_no_score_when_eaten);
    RUN_TEST(test_fruit_expires_after_timer);
    RUN_TEST(test_fruit_no_respawn_after_eaten);
    RUN_TEST(test_fruit_popup_timer_zero_on_init);
    RUN_TEST(test_fruit_popup_timer_set_on_eat);
    RUN_TEST(test_fruit_popup_timer_ticks_down);
    RUN_TEST(test_fruit_draw_inactive_no_crash);
    RUN_TEST(test_fruit_draw_active_no_crash);
    RUN_TEST(test_fruit_draw_popup_no_crash);
    TESTS_SUMMARY();
}
