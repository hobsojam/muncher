#include "test_framework.h"
#include "../src/map.c"

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

static void test_power_pellet(void) {
    map_init();
    // Row 3: "#o####..." — col 1 is 'o'
    TEST_ASSERT_EQUAL_INT(TILE_POWER, map[3][1]);
    TEST_ASSERT_EQUAL_INT(TILE_POWER, map[3][26]);
    TEST_ASSERT_EQUAL_INT(TILE_POWER, map[22][1]);
    TEST_ASSERT_EQUAL_INT(TILE_POWER, map[22][26]);
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
    RUN_TEST(test_power_pellet);
    RUN_TEST(test_ghost_door);
    RUN_TEST(test_tunnel_row_empty_edges);
    RUN_TEST(test_tunnel_row_dots);
    RUN_TEST(test_ghost_house_interior_empty);
    TESTS_SUMMARY();
}
