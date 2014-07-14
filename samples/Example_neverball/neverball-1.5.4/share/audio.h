#ifndef AUDIO_H
#define AUDIO_H

/*---------------------------------------------------------------------------*/

void audio_init(void);
void audio_free(void);
void audio_play(const char *, float);

void audio_music_queue(const char *, float);
void audio_music_play(const char *);
void audio_music_stop(void);

void audio_music_fade_to(float, const char *);
void audio_music_fade_in(float);
void audio_music_fade_out(float);

void audio_timer(float);
void audio_volume(int, int);

/*---------------------------------------------------------------------------*/

#endif
