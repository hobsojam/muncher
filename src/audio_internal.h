#ifndef AUDIO_INTERNAL_H
#define AUDIO_INTERNAL_H

#ifdef MUNCHER_TEST
void audio_internal_reset_state(void);
float audio_internal_music_volume(void);
float audio_internal_sfx_volume(void);
int audio_internal_music_muted(void);
int audio_internal_sfx_muted(void);
#endif

#endif
