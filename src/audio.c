#include "audio.h"
#include "audio_internal.h"
#include "raylib.h"

static Music s_music;
static Sound s_chomp;
static Sound s_power;
static Sound s_ghost_eat;
static Sound s_death;
static Sound s_fruit_eat;
static Sound s_extra_life;

static int s_music_loaded      = 0;
static int s_chomp_loaded      = 0;
static int s_power_loaded      = 0;
static int s_ghost_eat_loaded  = 0;
static int s_death_loaded      = 0;
static int s_fruit_eat_loaded  = 0;
static int s_extra_life_loaded = 0;

static float s_music_vol   = 1.0f;
static float s_sfx_vol     = 1.0f;
static int   s_music_muted = 0;
static int   s_sfx_muted   = 0;

static void set_sound_volume_if_loaded(Sound sound, int loaded, float vol) {
    if (loaded) SetSoundVolume(sound, vol);
}

static void unload_sound_if_loaded(Sound sound, int *loaded) {
    if (!*loaded) return;
    UnloadSound(sound);
    *loaded = 0;
}

static void play_sound_if_loaded(Sound sound, int loaded) {
    if (s_sfx_muted || !loaded) return;
    PlaySound(sound);
}

void audio_set_music_volume(float vol) {
    if (vol < 0.0f) vol = 0.0f;
    if (vol > 1.0f) vol = 1.0f;
    s_music_vol = vol;
    if (s_music_loaded && !s_music_muted) SetMusicVolume(s_music, s_music_vol);
}

void audio_set_sfx_volume(float vol) {
    if (vol < 0.0f) vol = 0.0f;
    if (vol > 1.0f) vol = 1.0f;
    s_sfx_vol = vol;
    set_sound_volume_if_loaded(s_chomp,      s_chomp_loaded,      s_sfx_vol);
    set_sound_volume_if_loaded(s_power,      s_power_loaded,      s_sfx_vol);
    set_sound_volume_if_loaded(s_ghost_eat,  s_ghost_eat_loaded,  s_sfx_vol);
    set_sound_volume_if_loaded(s_death,      s_death_loaded,      s_sfx_vol);
    set_sound_volume_if_loaded(s_fruit_eat,  s_fruit_eat_loaded,  s_sfx_vol);
    set_sound_volume_if_loaded(s_extra_life, s_extra_life_loaded, s_sfx_vol);
}

void audio_init(void) {
    InitAudioDevice();
    s_music      = LoadMusicStream("assets/music/theme.ogg");
    s_chomp      = LoadSound("assets/sounds/chomp.wav");
    s_power      = LoadSound("assets/sounds/power.wav");
    s_ghost_eat  = LoadSound("assets/sounds/ghost_eat.wav");
    s_death      = LoadSound("assets/sounds/death.wav");
    s_fruit_eat  = LoadSound("assets/sounds/fruit_eat.wav");
    s_extra_life = LoadSound("assets/sounds/extra_life.wav");
    s_music_loaded      = IsMusicValid(s_music);
    s_chomp_loaded      = IsSoundValid(s_chomp);
    s_power_loaded      = IsSoundValid(s_power);
    s_ghost_eat_loaded  = IsSoundValid(s_ghost_eat);
    s_death_loaded      = IsSoundValid(s_death);
    s_fruit_eat_loaded  = IsSoundValid(s_fruit_eat);
    s_extra_life_loaded = IsSoundValid(s_extra_life);
    audio_set_music_volume(0.5f);
    audio_set_sfx_volume(0.8f);
    if (s_music_loaded) PlayMusicStream(s_music);
}

void audio_update(void) {
    if (s_music_loaded) UpdateMusicStream(s_music);
}

void audio_close(void) {
    if (s_music_loaded) {
        UnloadMusicStream(s_music);
        s_music_loaded = 0;
    }
    unload_sound_if_loaded(s_chomp,      &s_chomp_loaded);
    unload_sound_if_loaded(s_power,      &s_power_loaded);
    unload_sound_if_loaded(s_ghost_eat,  &s_ghost_eat_loaded);
    unload_sound_if_loaded(s_death,      &s_death_loaded);
    unload_sound_if_loaded(s_fruit_eat,  &s_fruit_eat_loaded);
    unload_sound_if_loaded(s_extra_life, &s_extra_life_loaded);
    CloseAudioDevice();
}

void audio_play_chomp(void) {
    play_sound_if_loaded(s_chomp, s_chomp_loaded);
}

void audio_play_power(void) {
    play_sound_if_loaded(s_power, s_power_loaded);
}

void audio_play_ghost_eat(void) {
    play_sound_if_loaded(s_ghost_eat, s_ghost_eat_loaded);
}

void audio_play_death(void) {
    play_sound_if_loaded(s_death, s_death_loaded);
}

void audio_play_fruit_eat(void) {
    play_sound_if_loaded(s_fruit_eat, s_fruit_eat_loaded);
}

void audio_play_extra_life(void) {
    play_sound_if_loaded(s_extra_life, s_extra_life_loaded);
}

void audio_step_music_volume(float delta) { audio_set_music_volume(s_music_vol + delta); }
void audio_step_sfx_volume(float delta)   { audio_set_sfx_volume(s_sfx_vol   + delta); }

void audio_pause(void)  { if (s_music_loaded) PauseMusicStream(s_music); }
void audio_resume(void) { if (s_music_loaded) ResumeMusicStream(s_music); }

void audio_toggle_music_mute(void) {
    s_music_muted = !s_music_muted;
    if (s_music_loaded) SetMusicVolume(s_music, s_music_muted ? 0.0f : s_music_vol);
}

void audio_toggle_sfx_mute(void) {
    s_sfx_muted = !s_sfx_muted;
}

#ifdef MUNCHER_TEST
void audio_internal_reset_state(void) {
    s_music_vol = 1.0f;
    s_sfx_vol = 1.0f;
    s_music_muted = 0;
    s_sfx_muted = 0;
    s_music_loaded = 0;
    s_chomp_loaded = 0;
    s_power_loaded = 0;
    s_ghost_eat_loaded = 0;
    s_death_loaded = 0;
    s_fruit_eat_loaded  = 0;
    s_extra_life_loaded = 0;
}

float audio_internal_music_volume(void) { return s_music_vol; }
float audio_internal_sfx_volume(void) { return s_sfx_vol; }
int audio_internal_music_muted(void) { return s_music_muted; }
int audio_internal_sfx_muted(void) { return s_sfx_muted; }
int audio_internal_music_loaded(void)      { return s_music_loaded; }
int audio_internal_chomp_loaded(void)      { return s_chomp_loaded; }
int audio_internal_power_loaded(void)      { return s_power_loaded; }
int audio_internal_ghost_eat_loaded(void)  { return s_ghost_eat_loaded; }
int audio_internal_death_loaded(void)      { return s_death_loaded; }
int audio_internal_fruit_eat_loaded(void)  { return s_fruit_eat_loaded; }
int audio_internal_extra_life_loaded(void) { return s_extra_life_loaded; }
void audio_internal_stub_reset(void)                     { RaylibStubResetAudio(); }
void audio_internal_stub_set_music_load_success(int v)   { raylib_stub_music_load_success = v; }
void audio_internal_stub_set_sound_load_success(int v)   { raylib_stub_sound_load_success = v; }
int audio_internal_stub_invalid_music_ops(void)          { return raylib_stub_invalid_music_operations; }
int audio_internal_stub_invalid_sound_ops(void)          { return raylib_stub_invalid_sound_operations; }
#endif
