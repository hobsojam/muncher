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

static void write_raw_int(const char *path, int value) {
    FILE *f = fopen(path, "wb");
    fwrite(&value, sizeof(int), 1, f);
    fclose(f);
}

static void test_load_clamps_tampered_file_to_max(void) {
    cleanup();
    write_raw_int(TEST_PATH, 0x7FFFFFFF);
    TEST_ASSERT_EQUAL_INT(HISCORE_MAX, hiscore_load(TEST_PATH));
    cleanup();
}

static void test_load_negative_file_value_returns_zero(void) {
    cleanup();
    write_raw_int(TEST_PATH, -123456);
    TEST_ASSERT_EQUAL_INT(0, hiscore_load(TEST_PATH));
    cleanup();
}

static void test_save_clamps_above_max(void) {
    cleanup();
    hiscore_save(TEST_PATH, HISCORE_MAX + 1);
    TEST_ASSERT_EQUAL_INT(HISCORE_MAX, hiscore_load(TEST_PATH));
    cleanup();
}

static void test_save_and_load_at_max(void) {
    cleanup();
    hiscore_save(TEST_PATH, HISCORE_MAX);
    TEST_ASSERT_EQUAL_INT(HISCORE_MAX, hiscore_load(TEST_PATH));
    cleanup();
}

int main(void) {
    RUN_TEST(test_load_missing_file_returns_zero);
    RUN_TEST(test_save_then_load);
    RUN_TEST(test_save_zero_is_noop);
    RUN_TEST(test_save_negative_is_noop);
    RUN_TEST(test_save_overwrites_previous);
    RUN_TEST(test_load_clamps_tampered_file_to_max);
    RUN_TEST(test_load_negative_file_value_returns_zero);
    RUN_TEST(test_save_clamps_above_max);
    RUN_TEST(test_save_and_load_at_max);
    TESTS_SUMMARY();
}
