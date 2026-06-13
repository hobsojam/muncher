#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>

static int _tests_run    = 0;
static int _tests_failed = 0;
static const char *_current_test = "";

#define RUN_TEST(fn) do { \
    _current_test = #fn; \
    _tests_run++; \
    fn(); \
} while (0)

#define TEST_ASSERT_EQUAL_INT(expected, actual) do { \
    int _e = (expected), _a = (actual); \
    if (_e != _a) { \
        printf("FAIL  %s: expected %d but got %d (%s:%d)\n", \
               _current_test, _e, _a, __FILE__, __LINE__); \
        _tests_failed++; \
        return; \
    } \
} while (0)

#define TEST_ASSERT(cond) do { \
    if (!(cond)) { \
        printf("FAIL  %s: %s (%s:%d)\n", \
               _current_test, #cond, __FILE__, __LINE__); \
        _tests_failed++; \
        return; \
    } \
} while (0)

#define TESTS_SUMMARY() do { \
    int _passed = _tests_run - _tests_failed; \
    printf("%d/%d passed\n", _passed, _tests_run); \
    return _tests_failed ? 1 : 0; \
} while (0)

#endif
