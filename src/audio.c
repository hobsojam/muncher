#include "audio.h"
#include "audio_internal.h"
#include "raylib.h"

static Music s_music;
static Sound s_chomp;
static Sound s_power;
static Sound s_ghost_eat;
static Sound s_death;

static float s_music_vol   = 1.0f;
static float s_sfx_vol     = 1.0f;
static int   s_music_muted = 0;
static int   s_sfx_muted   = 0;

void audio_set_music_volume(float vol) {
    if (vol < 0.0f) vol = 0.0f;
    if (vol > 1.0f) vol = 1.0f;
    s_music_vol = vol;
    if (!s_music_muted) SetMusicVolume(s_music, s_music_vol);
}

void audio_set_sfx_volume(float vol) {
    if (vol < 0.0f) vol = 0.0f;
    if (vol > 1.0f) vol = 1.0f;
    s_sfx_vol = vol;
    SetSoundVolume(s_chomp,     s_sfx_vol);
    SetSoundVolume(s_power,     s_sfx_vol);
    SetSoundVolume(s_ghost_eat, s_sfx_vol);
    SetSoundVolume(s_death,     s_sfx_vol);
}

void audio_init(void) {
    InitAudioDevice();
    s_music     = LoadMusicStream("assets/music/theme.ogg");
    s_chomp     = LoadSound("assets/sounds/chomp.wav");
    s_power     = LoadSound("assets/sounds/power.wav");
    s_ghost_eat = LoadSound("assets/sounds/ghost_eat.wav");
    s_death     = LoadSound("assets/sounds/death.wav");
    audio_set_music_volume(0.5f);
    audio_set_sfx_volume(0.8f);
    PlayMusicStream(s_music);
}

void audio_update(void) {
    UpdateMusicStream(s_music);
}

void audio_close(void) {
    UnloadMusicStream(s_music);
    UnloadSound(s_chomp);
    UnloadSound(s_power);
    UnloadSound(s_ghost_eat);
    UnloadSound(s_death);
    CloseAudioDevice();
}

void audio_play_chomp(void) {
    if (s_sfx_muted) return;
    PlaySound(s_chomp);
}

void audio_play_power(void) {
    if (s_sfx_muted) return;
    PlaySound(s_power);
}

void audio_play_ghost_eat(void) {
    if (s_sfx_muted) return;
    PlaySound(s_ghost_eat);
}

void audio_play_death(void) {
    if (s_sfx_muted) return;
    PlaySound(s_death);
}

void audio_step_music_volume(float delta) { audio_set_music_volume(s_music_vol + delta); }
void audio_step_sfx_volume(float delta)   { audio_set_sfx_volume(s_sfx_vol   + delta); }

void audio_toggle_music_mute(void) {
    s_music_muted = !s_music_muted;
    SetMusicVolume(s_music, s_music_muted ? 0.0f : s_music_vol);
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
}

float audio_internal_music_volume(void) { return s_music_vol; }
float audio_internal_sfx_volume(void) { return s_sfx_vol; }
int audio_internal_music_muted(void) { return s_music_muted; }
int audio_internal_sfx_muted(void) { return s_sfx_muted; }
#endif
