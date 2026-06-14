#ifndef AUDIO_H
#define AUDIO_H

/* No-op stubs for unit tests — matches src/audio.h interface */
static inline void audio_init(void)              {}
static inline void audio_update(void)            {}
static inline void audio_close(void)             {}
static inline void audio_play_chomp(void)        {}
static inline void audio_play_power(void)        {}
static inline void audio_play_ghost_eat(void)    {}
static inline void audio_play_death(void)        {}
static inline void audio_toggle_music_mute(void) {}
static inline void audio_toggle_sfx_mute(void)  {}
static inline void audio_set_music_volume(float v)  { (void)v; }
static inline void audio_set_sfx_volume(float v)    { (void)v; }
static inline void audio_step_music_volume(float v) { (void)v; }
static inline void audio_step_sfx_volume(float v)   { (void)v; }

#endif
