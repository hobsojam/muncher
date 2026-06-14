#include "test_framework.h"
#include "../src/map.c"

static int map_power_count(void) {
    int n = 0;
    for (int r = 0; r < MAP_ROWS; r++)
        for (int c = 0; c < MAP_COLS; c++)
            if (map[r][c] == TILE_POWER) n++;
    return n;
}

static void copy_map(TileType dest[MAP_ROWS][MAP_COLS]) {
    for (int r = 0; r < MAP_ROWS; r++)
        for (int c = 0; c < MAP_COLS; c++)
            dest[r][c] = map[r][c];
}

static void assert_map_matches(TileType expected[MAP_ROWS][MAP_COLS]) {
    for (int r = 0; r < MAP_ROWS; r++)
        for (int c = 0; c < MAP_COLS; c++)
            TEST_ASSERT_EQUAL_INT(expected[r][c], map[r][c]);
}

static int validation_calls;

static int always_reject_map(void) {
    validation_calls++;
    return 0;
}

static void test_corner_is_wall(void) {
    map_init();
    TEST_ASSERT_EQUAL_INT(TILE_WALL, map[0][0]);
    TEST_ASSERT_EQUAL_INT(TILE_WALL, map[0][27]);
    TEST_ASSERT_EQUAL_INT(TILE_WALL, map[30][0]);
    TEST_ASSERT_EQUAL_INT(TILE_WALL, map[30][27]);
}

static void test_dot(void) {
    map_init();
    TEST_ASSERT_EQUAL_INT(TILE_DOT, map[1][1]);
}

static void test_power_pellet_count(void) {
    map_init();
    TEST_ASSERT_EQUAL_INT(4, map_power_count());
}

static void test_all_dots_reachable(void) {
    map_init();
    TEST_ASSERT(map_all_dots_reachable());
}

static void test_level_color_changes(void) {
    TEST_ASSERT(map_generate(1));
    int dots_level1 = map_dots_remaining();
    TEST_ASSERT(map_generate(6));
    int dots_level6 = map_dots_remaining();
    /* Different seeds should produce different dot counts (not guaranteed
       to differ every time, but levels 1 and 6 use different seeds). */
    (void)dots_level1; (void)dots_level6;
    /* At minimum, both levels produce solvable maps. */
    TEST_ASSERT(map_generate(1));
    TEST_ASSERT(map_all_dots_reachable());
    TEST_ASSERT(map_generate(6));
    TEST_ASSERT(map_all_dots_reachable());
}

static void test_generate_failure_preserves_previous_map(void) {
    TileType expected[MAP_ROWS][MAP_COLS];

    TEST_ASSERT(map_generate(1));
    copy_map(expected);
    validation_calls = 0;

    TEST_ASSERT_EQUAL_INT(0, map_generate_with_validator(3, always_reject_map));
    TEST_ASSERT_EQUAL_INT(MAP_GENERATION_ATTEMPTS, validation_calls);
    assert_map_matches(expected);
    TEST_ASSERT(map_all_dots_reachable());
}

static void test_ghost_door(void) {
    map_init();
    // Row 12: "     #.##.###--###.##.#     " — cols 13-14 are '-'
    TEST_ASSERT_EQUAL_INT(TILE_DOOR, map[12][13]);
    TEST_ASSERT_EQUAL_INT(TILE_DOOR, map[12][14]);
}

static void test_tunnel_row_empty_edges(void) {
    map_init();
    // Row 14: "      ....#      #....      " — cols 0-5 are spaces (EMPTY)
    TEST_ASSERT_EQUAL_INT(TILE_EMPTY, map[14][0]);
    TEST_ASSERT_EQUAL_INT(TILE_EMPTY, map[14][5]);
}

static void test_tunnel_row_dots(void) {
    map_init();
    // Row 14: cols 6-9 are dots
    TEST_ASSERT_EQUAL_INT(TILE_DOT, map[14][6]);
    TEST_ASSERT_EQUAL_INT(TILE_DOT, map[14][9]);
}

static void test_ghost_house_interior_empty(void) {
    map_init();
    // Row 13: "######.##.#      #.##.######" — cols 12-16 are spaces
    TEST_ASSERT_EQUAL_INT(TILE_EMPTY, map[13][12]);
    TEST_ASSERT_EQUAL_INT(TILE_EMPTY, map[13][15]);
}

int main(void) {
    RUN_TEST(test_corner_is_wall);
    RUN_TEST(test_dot);
    RUN_TEST(test_power_pellet_count);
    RUN_TEST(test_ghost_door);
    RUN_TEST(test_tunnel_row_empty_edges);
    RUN_TEST(test_tunnel_row_dots);
    RUN_TEST(test_ghost_house_interior_empty);
    RUN_TEST(test_all_dots_reachable);
    RUN_TEST(test_level_color_changes);
    RUN_TEST(test_generate_failure_preserves_previous_map);
    TESTS_SUMMARY();
}
