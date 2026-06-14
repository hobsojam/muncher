#include "test_framework.h"
#include "map.h"

static void test_initial_dots_positive(void) {
    map_init();
    TEST_ASSERT(map_dots_remaining() > 0);
}

static void test_all_cleared_returns_zero(void) {
    map_init();
    for (int r = 0; r < MAP_ROWS; r++)
        for (int c = 0; c < MAP_COLS; c++)
            map[r][c] = TILE_EMPTY;
    TEST_ASSERT_EQUAL_INT(0, map_dots_remaining());
}

static void test_one_dot_cleared_decrements_count(void) {
    map_init();
    int initial = map_dots_remaining();
    /* map[1][1] is a TILE_DOT per the layout */
    map[1][1] = TILE_EMPTY;
    TEST_ASSERT_EQUAL_INT(initial - 1, map_dots_remaining());
}

int main(void) {
    RUN_TEST(test_initial_dots_positive);
    RUN_TEST(test_all_cleared_returns_zero);
    RUN_TEST(test_one_dot_cleared_decrements_count);
    TESTS_SUMMARY();
}
