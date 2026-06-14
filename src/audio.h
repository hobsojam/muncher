#ifndef AUDIO_H
#define AUDIO_H

void audio_init(void);
void audio_update(void);
void audio_close(void);

void audio_play_chomp(void);
void audio_play_power(void);
void audio_play_ghost_eat(void);
void audio_play_death(void);
void audio_play_fruit_eat(void);

void audio_pause(void);
void audio_resume(void);

void audio_toggle_music_mute(void);
void audio_toggle_sfx_mute(void);
void audio_set_music_volume(float vol);   /* 0.0 – 1.0 */
void audio_set_sfx_volume(float vol);     /* 0.0 – 1.0 */
void audio_step_music_volume(float delta); /* clamps to [0,1] */
void audio_step_sfx_volume(float delta);   /* clamps to [0,1] */

#endif
