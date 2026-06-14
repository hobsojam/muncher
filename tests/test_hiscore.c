#include "test_framework.h"
#include "hiscore.h"
#include <stdio.h>

#define TEST_PATH "test_hiscore_tmp.dat"

static void cleanup(void) { remove(TEST_PATH); }

static void test_load_missing_file_returns_zero(void) {
    cleanup();
    TEST_ASSERT_EQUAL_INT(0, hiscore_load(TEST_PATH));
}

static void test_save_then_load(void) {
    cleanup();
    hiscore_save(TEST_PATH, 12345);
    TEST_ASSERT_EQUAL_INT(12345, hiscore_load(TEST_PATH));
    cleanup();
}

static void test_save_zero_is_noop(void) {
    cleanup();
    hiscore_save(TEST_PATH, 0);
    TEST_ASSERT_EQUAL_INT(0, hiscore_load(TEST_PATH));
}

static void test_save_negative_is_noop(void) {
    cleanup();
    hiscore_save(TEST_PATH, -1);
    TEST_ASSERT_EQUAL_INT(0, hiscore_load(TEST_PATH));
}

static void test_save_overwrites_previous(void) {
    cleanup();
    hiscore_save(TEST_PATH, 100);
    hiscore_save(TEST_PATH, 9999);
    TEST_ASSERT_EQUAL_INT(9999, hiscore_load(TEST_PATH));
    cleanup();
}

int main(void) {
    RUN_TEST(test_load_missing_file_returns_zero);
    RUN_TEST(test_save_then_load);
    RUN_TEST(test_save_zero_is_noop);
    RUN_TEST(test_save_negative_is_noop);
    RUN_TEST(test_save_overwrites_previous);
    TESTS_SUMMARY();
}
