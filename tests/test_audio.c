#include "test_framework.h"
#include "audio.h"
#include "audio_internal.h"

/* Reset to a clean, known state before each test */
static void reset(void) {
    audio_internal_stub_reset();
    audio_init();
    audio_internal_reset_state();
}

static void test_set_music_volume_clamps_low(void) {
    reset();
    audio_set_music_volume(-1.0f);
    TEST_ASSERT(audio_internal_music_volume() == 0.0f);
}

static void test_set_music_volume_clamps_high(void) {
    reset();
    audio_set_music_volume(2.0f);
    TEST_ASSERT(audio_internal_music_volume() == 1.0f);
}

static void test_set_music_volume_stores(void) {
    reset();
    audio_set_music_volume(0.25f);
    TEST_ASSERT(audio_internal_music_volume() == 0.25f);
}

static void test_set_sfx_volume_clamps_low(void) {
    reset();
    audio_set_sfx_volume(-0.5f);
    TEST_ASSERT(audio_internal_sfx_volume() == 0.0f);
}

static void test_set_sfx_volume_clamps_high(void) {
    reset();
    audio_set_sfx_volume(1.5f);
    TEST_ASSERT(audio_internal_sfx_volume() == 1.0f);
}

static void test_set_sfx_volume_stores(void) {
    reset();
    audio_set_sfx_volume(0.25f);
    TEST_ASSERT(audio_internal_sfx_volume() == 0.25f);
}

static void test_step_music_volume_up(void) {
    reset();
    audio_set_music_volume(0.5f);
    audio_step_music_volume(0.25f);
    TEST_ASSERT(audio_internal_music_volume() == 0.75f);
}

static void test_step_music_volume_clamps_high(void) {
    reset();
    audio_set_music_volume(1.0f);
    audio_step_music_volume(0.5f);
    TEST_ASSERT(audio_internal_music_volume() == 1.0f);
}

static void test_step_music_volume_down(void) {
    reset();
    audio_set_music_volume(0.5f);
    audio_step_music_volume(-0.25f);
    TEST_ASSERT(audio_internal_music_volume() == 0.25f);
}

static void test_step_music_volume_clamps_low(void) {
    reset();
    audio_set_music_volume(0.0f);
    audio_step_music_volume(-0.5f);
    TEST_ASSERT(audio_internal_music_volume() == 0.0f);
}

static void test_step_sfx_volume_up(void) {
    reset();
    audio_set_sfx_volume(0.5f);
    audio_step_sfx_volume(0.25f);
    TEST_ASSERT(audio_internal_sfx_volume() == 0.75f);
}

static void test_step_sfx_volume_clamps_high(void) {
    reset();
    audio_set_sfx_volume(1.0f);
    audio_step_sfx_volume(0.5f);
    TEST_ASSERT(audio_internal_sfx_volume() == 1.0f);
}

static void test_toggle_music_mute(void) {
    reset();
    TEST_ASSERT_EQUAL_INT(0, audio_internal_music_muted());
    audio_toggle_music_mute();
    TEST_ASSERT_EQUAL_INT(1, audio_internal_music_muted());
    audio_toggle_music_mute();
    TEST_ASSERT_EQUAL_INT(0, audio_internal_music_muted());
}

static void test_toggle_sfx_mute(void) {
    reset();
    TEST_ASSERT_EQUAL_INT(0, audio_internal_sfx_muted());
    audio_toggle_sfx_mute();
    TEST_ASSERT_EQUAL_INT(1, audio_internal_sfx_muted());
    audio_toggle_sfx_mute();
    TEST_ASSERT_EQUAL_INT(0, audio_internal_sfx_muted());
}

static void test_set_music_volume_while_muted_preserves_stored_vol(void) {
    reset();
    audio_toggle_music_mute();
    audio_set_music_volume(0.25f);
    TEST_ASSERT(audio_internal_music_volume() == 0.25f);
    TEST_ASSERT_EQUAL_INT(1, audio_internal_music_muted());
}

/* Exercise both branches of the sfx-muted guard in each play function */
static void test_play_sounds_when_unmuted(void) {
    reset();
    audio_play_chomp();
    audio_play_power();
    audio_play_ghost_eat();
    audio_play_death();
}

static void test_play_sounds_when_muted(void) {
    reset();
    audio_toggle_sfx_mute();
    audio_play_chomp();
    audio_play_power();
    audio_play_ghost_eat();
    audio_play_death();
}

static void test_audio_update_no_crash(void) {
    reset();
    audio_update();
}

static void test_audio_close_no_crash(void) {
    reset();
    audio_close();
}

static void test_audio_pause_no_invalid_ops(void) {
    audio_internal_stub_reset();
    audio_init();
    audio_pause();
    TEST_ASSERT_EQUAL_INT(0, audio_internal_stub_invalid_music_ops());
    audio_close();
}

static void test_audio_resume_no_invalid_ops(void) {
    audio_internal_stub_reset();
    audio_init();
    audio_resume();
    TEST_ASSERT_EQUAL_INT(0, audio_internal_stub_invalid_music_ops());
    audio_close();
}

static void test_audio_pause_no_crash_when_music_not_loaded(void) {
    reset(); /* resets s_music_loaded to 0 */
    audio_pause();
    TEST_ASSERT_EQUAL_INT(0, audio_internal_stub_invalid_music_ops());
}

static void test_audio_resume_no_crash_when_music_not_loaded(void) {
    reset();
    audio_resume();
    TEST_ASSERT_EQUAL_INT(0, audio_internal_stub_invalid_music_ops());
}

static void test_failed_music_load_is_ignored(void) {
    audio_internal_stub_reset();
    audio_internal_stub_set_music_load_success(0);

    audio_init();
    audio_set_music_volume(0.25f);
    audio_toggle_music_mute();
    audio_update();
    audio_close();

    TEST_ASSERT_EQUAL_INT(0, audio_internal_music_loaded());
    TEST_ASSERT_EQUAL_INT(0, audio_internal_stub_invalid_music_ops());
}

static void test_failed_sound_loads_are_ignored(void) {
    audio_internal_stub_reset();
    audio_internal_stub_set_sound_load_success(0);

    audio_init();
    audio_set_sfx_volume(0.25f);
    audio_play_chomp();
    audio_play_power();
    audio_play_ghost_eat();
    audio_play_death();
    audio_close();

    TEST_ASSERT_EQUAL_INT(0, audio_internal_chomp_loaded());
    TEST_ASSERT_EQUAL_INT(0, audio_internal_power_loaded());
    TEST_ASSERT_EQUAL_INT(0, audio_internal_ghost_eat_loaded());
    TEST_ASSERT_EQUAL_INT(0, audio_internal_death_loaded());
    TEST_ASSERT_EQUAL_INT(0, audio_internal_stub_invalid_sound_ops());
}

int main(void) {
    RUN_TEST(test_set_music_volume_clamps_low);
    RUN_TEST(test_set_music_volume_clamps_high);
    RUN_TEST(test_set_music_volume_stores);
    RUN_TEST(test_set_sfx_volume_clamps_low);
    RUN_TEST(test_set_sfx_volume_clamps_high);
    RUN_TEST(test_set_sfx_volume_stores);
    RUN_TEST(test_step_music_volume_up);
    RUN_TEST(test_step_music_volume_clamps_high);
    RUN_TEST(test_step_music_volume_down);
    RUN_TEST(test_step_music_volume_clamps_low);
    RUN_TEST(test_step_sfx_volume_up);
    RUN_TEST(test_step_sfx_volume_clamps_high);
    RUN_TEST(test_toggle_music_mute);
    RUN_TEST(test_toggle_sfx_mute);
    RUN_TEST(test_set_music_volume_while_muted_preserves_stored_vol);
    RUN_TEST(test_play_sounds_when_unmuted);
    RUN_TEST(test_play_sounds_when_muted);
    RUN_TEST(test_audio_update_no_crash);
    RUN_TEST(test_audio_close_no_crash);
    RUN_TEST(test_audio_pause_no_invalid_ops);
    RUN_TEST(test_audio_resume_no_invalid_ops);
    RUN_TEST(test_audio_pause_no_crash_when_music_not_loaded);
    RUN_TEST(test_audio_resume_no_crash_when_music_not_loaded);
    RUN_TEST(test_failed_music_load_is_ignored);
    RUN_TEST(test_failed_sound_loads_are_ignored);
    TESTS_SUMMARY();
}
