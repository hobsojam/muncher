#ifndef AUDIO_INTERNAL_H
#define AUDIO_INTERNAL_H

#ifdef MUNCHER_TEST
void audio_internal_reset_state(void);
float audio_internal_music_volume(void);
float audio_internal_sfx_volume(void);
int audio_internal_music_muted(void);
int audio_internal_sfx_muted(void);
int audio_internal_music_loaded(void);
int audio_internal_chomp_loaded(void);
int audio_internal_power_loaded(void);
int audio_internal_ghost_eat_loaded(void);
int audio_internal_death_loaded(void);
int audio_internal_fruit_eat_loaded(void);
void audio_internal_stub_reset(void);
void audio_internal_stub_set_music_load_success(int v);
void audio_internal_stub_set_sound_load_success(int v);
int audio_internal_stub_invalid_music_ops(void);
int audio_internal_stub_invalid_sound_ops(void);
#endif

#endif
