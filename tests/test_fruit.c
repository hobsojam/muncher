#include "test_framework.h"
#include "map.h"
#include "player.h"
#include "fruit.h"

/* ------------------------------------------------------------------ */
/* fruit_init                                                           */
/* ------------------------------------------------------------------ */

static void test_fruit_init_not_active(void) {
    Fruit f;
    fruit_init(&f, 1);
    TEST_ASSERT_EQUAL_INT(0, f.active);
}

static void test_fruit_init_not_eaten(void) {
    Fruit f;
    fruit_init(&f, 1);
    TEST_ASSERT_EQUAL_INT(0, f.eaten);
}

static void test_fruit_init_position(void) {
    Fruit f;
    fruit_init(&f, 1);
    TEST_ASSERT_EQUAL_INT(FRUIT_COL, f.col);
    TEST_ASSERT_EQUAL_INT(FRUIT_ROW, f.row);
}

static void test_fruit_init_score(void) {
    Fruit f;
    fruit_init(&f, 1);
    TEST_ASSERT_EQUAL_INT(100, f.score);
}

/* ------------------------------------------------------------------ */
/* fruit_init — level-based type and score                             */
/* ------------------------------------------------------------------ */

static void test_fruit_level1_cherry(void) {
    Fruit f;
    fruit_init(&f, 1);
    TEST_ASSERT_EQUAL_INT(100, f.score);
    TEST_ASSERT_EQUAL_INT(FRUIT_TYPE_CHERRY, f.type);
}

static void test_fruit_level2_strawberry(void) {
    Fruit f;
    fruit_init(&f, 2);
    TEST_ASSERT_EQUAL_INT(300, f.score);
    TEST_ASSERT_EQUAL_INT(FRUIT_TYPE_STRAWBERRY, f.type);
}

static void test_fruit_level5_apple(void) {
    Fruit f;
    fruit_init(&f, 5);
    TEST_ASSERT_EQUAL_INT(1000, f.score);
    TEST_ASSERT_EQUAL_INT(FRUIT_TYPE_APPLE, f.type);
}

static void test_fruit_level6_grapes(void) {
    Fruit f;
    fruit_init(&f, 6);
    TEST_ASSERT_EQUAL_INT(2000, f.score);
    TEST_ASSERT_EQUAL_INT(FRUIT_TYPE_GRAPES, f.type);
}

static void test_fruit_level99_grapes_clamped(void) {
    Fruit f;
    fruit_init(&f, 99);
    TEST_ASSERT_EQUAL_INT(2000, f.score);
    TEST_ASSERT_EQUAL_INT(FRUIT_TYPE_GRAPES, f.type);
}

static void test_fruit_level3_score_added_on_eat(void) {
    map_init();
    Fruit f;
    fruit_init(&f, 3);
    Player p; player_init(&p);
    p.col = FRUIT_COL; p.row = FRUIT_ROW;
    f.active = 1; f.timer = 5.0f;
    fruit_update(&f, &p, 0, 100, 0.1f);
    TEST_ASSERT_EQUAL_INT(500, p.score);
}

/* ------------------------------------------------------------------ */
/* fruit_update — spawn trigger                                         */
/* ------------------------------------------------------------------ */

static void test_fruit_spawns_at_half_dots(void) {
    map_init();
    Fruit f; fruit_init(&f, 1);
    Player p; player_init(&p);
    int total = 100;
    fruit_update(&f, &p, total / 2, total, 0.0f);
    TEST_ASSERT_EQUAL_INT(1, f.active);
}

static void test_fruit_no_spawn_before_half(void) {
    map_init();
    Fruit f; fruit_init(&f, 1);
    Player p; player_init(&p);
    int total = 100;
    fruit_update(&f, &p, total / 2 + 1, total, 0.0f);
    TEST_ASSERT_EQUAL_INT(0, f.active);
}

static void test_fruit_no_spawn_when_zero_total(void) {
    map_init();
    Fruit f; fruit_init(&f, 1);
    Player p; player_init(&p);
    fruit_update(&f, &p, 0, 0, 0.0f);
    TEST_ASSERT_EQUAL_INT(0, f.active);
}

/* ------------------------------------------------------------------ */
/* fruit_update — player collision                                      */
/* ------------------------------------------------------------------ */

static void test_fruit_player_collect_adds_score(void) {
    map_init();
    Fruit f; fruit_init(&f, 1);
    Player p; player_init(&p);
    p.col = FRUIT_COL; p.row = FRUIT_ROW;
    f.active = 1; f.timer = 5.0f;
    fruit_update(&f, &p, 0, 100, 0.1f);
    TEST_ASSERT_EQUAL_INT(100, p.score);
}

static void test_fruit_player_collect_deactivates(void) {
    map_init();
    Fruit f; fruit_init(&f, 1);
    Player p; player_init(&p);
    p.col = FRUIT_COL; p.row = FRUIT_ROW;
    f.active = 1; f.timer = 5.0f;
    fruit_update(&f, &p, 0, 100, 0.1f);
    TEST_ASSERT_EQUAL_INT(0, f.active);
    TEST_ASSERT_EQUAL_INT(1, f.eaten);
}

static void test_fruit_no_score_when_eaten(void) {
    map_init();
    Fruit f; fruit_init(&f, 1);
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
    Fruit f; fruit_init(&f, 1);
    Player p; player_init(&p);
    f.active = 1; f.timer = 0.05f;
    fruit_update(&f, &p, 0, 100, 0.1f);
    TEST_ASSERT_EQUAL_INT(0, f.active);
}

static void test_fruit_no_respawn_after_eaten(void) {
    map_init();
    Fruit f; fruit_init(&f, 1);
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
    Fruit f; fruit_init(&f, 1);
    Player p; player_init(&p);
    f.active = 1;
    p.col = f.col; p.row = f.row;
    fruit_update(&f, &p, 0, 100, 0.0f);
    TEST_ASSERT(f.popup_timer > 0.0f);
}

static void test_fruit_popup_timer_ticks_down(void) {
    map_init();
    Fruit f; fruit_init(&f, 1);
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
    fruit_init(&f, 1);
    TEST_ASSERT(f.popup_timer == 0.0f);
}

/* ------------------------------------------------------------------ */
/* fruit_draw — smoke tests (exercises raylib stubs for coverage)       */
/* ------------------------------------------------------------------ */

static void test_fruit_draw_active_no_crash(void) {
    Fruit f; fruit_init(&f, 1);
    f.active = 1;
    fruit_draw(&f, 0, 0);
}

static void test_fruit_draw_popup_no_crash(void) {
    Fruit f; fruit_init(&f, 1);
    f.popup_timer = 0.5f;
    fruit_draw(&f, 0, 0);
}

/* Float+fade: popup at nearly-zero timer (late progress, nearly transparent) */
static void test_fruit_draw_popup_late_progress_no_crash(void) {
    Fruit f; fruit_init(&f, 1);
    f.popup_timer = 0.04f; /* progress ~0.95 — nearly faded */
    fruit_draw(&f, 10, 40);
    TEST_ASSERT(f.popup_timer > 0.0f);
}

/* Float+fade: popup at full timer (early progress, fully opaque, minimal drift) */
static void test_fruit_draw_popup_early_progress_no_crash(void) {
    Fruit f; fruit_init(&f, 3); /* peach: score=500 */
    f.popup_timer = 0.79f; /* progress ~0.0125 */
    fruit_draw(&f, 0, 0);
    TEST_ASSERT(f.popup_timer > 0.0f);
}

static void test_fruit_draw_inactive_no_crash(void) {
    Fruit f; fruit_init(&f, 1);
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
    /* Feature 1 — fruit popup float+fade smoke tests */
    RUN_TEST(test_fruit_draw_popup_late_progress_no_crash);
    RUN_TEST(test_fruit_draw_popup_early_progress_no_crash);
    RUN_TEST(test_fruit_level1_cherry);
    RUN_TEST(test_fruit_level2_strawberry);
    RUN_TEST(test_fruit_level5_apple);
    RUN_TEST(test_fruit_level6_grapes);
    RUN_TEST(test_fruit_level99_grapes_clamped);
    RUN_TEST(test_fruit_level3_score_added_on_eat);
    TESTS_SUMMARY();
}
