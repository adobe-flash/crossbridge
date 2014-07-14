/*
 * Copyright (C) 2003 Robert Kooima
 *
 * NEVERBALL is  free software; you can redistribute  it and/or modify
 * it under the  terms of the GNU General  Public License as published
 * by the Free  Software Foundation; either version 2  of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the GNU
 * General Public License for more details.
 */

#include <SDL.h>
#include <AS3/AS3.h>

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "audio.h"
#include "common.h"
#include "fs.h"
#include "fs_ov.h"

/*---------------------------------------------------------------------------*/

#define AUDIO_RATE 44100
#define AUDIO_CHAN 2

struct voice
{
    OggVorbis_File  vf;
    float          amp;
    float         damp;
    int           chan;
    int           play;
    int           loop;
    char         *name;
    struct voice *next;
};

static int   audio_state = 0;
static float sound_vol   = 1.0f;
static float music_vol   = 1.0f;

static SDL_AudioSpec spec;

static struct voice *music  = NULL;
static struct voice *queue  = NULL;
static struct voice *voices = NULL;
static short        *buffer = NULL;

static ov_callbacks callbacks = {
    fs_ov_read, fs_ov_seek, fs_ov_close, fs_ov_tell
};

/*---------------------------------------------------------------------------*/

#define MIX(d, s) {                           \
        int n = (int) (d) + (int) (s);        \
        if      (n >  32767) (d) =  32767;    \
        else if (n < -32768) (d) = -32768;    \
        else                 (d) = (short) n; \
    }

static int voice_step(struct voice *V, float volume, Uint8 *stream, int length)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    int order = 1;
#else
    int order = 0;
#endif

    short *obuf = (short *) stream;
    char  *ibuf = (char  *) buffer;

    int i, b = 0, n = 1, c = 0, r = 0;

    /* Compute the total request size for the current stream. */

    if (V->chan == 1) r = length / 2;
    if (V->chan == 2) r = length    ;

    /* While data is coming in and data is still needed... */

    while (n > 0 && r > 0)
    {
        /* Read audio from the stream. */

        if ((n = (int) ov_read(&V->vf, ibuf, r, order, 2, 1, &b)) > 0)
        {
            /* Mix mono audio. */

            if (V->chan == 1)
                for (i = 0; i < n / 2; i += 1)
                {
                    short M = (short) (V->amp * volume * buffer[i]);

                    MIX(obuf[c], M); c++;
                    MIX(obuf[c], M); c++;

                    V->amp += V->damp;

                    if (V->amp < 0.0) V->amp = 0.0;
                    if (V->amp > 1.0) V->amp = 1.0;
                }

            /* Mix stereo audio. */

            if (V->chan == 2)
                for (i = 0; i < n / 2; i += 2)
                {
                    short L = (short) (V->amp * volume * buffer[i + 0]);
                    short R = (short) (V->amp * volume * buffer[i + 1]);

                    MIX(obuf[c], L); c++;
                    MIX(obuf[c], R); c++;

                    V->amp += V->damp;

                    if (V->amp < 0.0) V->amp = 0.0;
                    if (V->amp > 1.0) V->amp = 1.0;
                }

            r -= n;
        }
        else
        {
            /* We're at EOF.  Loop or end the voice. */

            if (V->loop)
            {
                ov_raw_seek(&V->vf, 0);
                n = 1;
            }
            else return 1;
        }
    }
    return 0;
}

static struct voice *voice_init(const char *filename, float a)
{
    struct voice *V;
    fs_file      fp;

    /* Allocate and initialize a new voice structure. */

    if ((V = (struct voice *) calloc(1, sizeof (struct voice))))
    {
        /* Note the name. */

        V->name = strdup(filename);

        /* Attempt to open the named Ogg stream. */

        if ((fp = fs_open(filename, "r")))
        {
            if (ov_open_callbacks(fp, &V->vf, NULL, 0, callbacks) == 0)
            {
                vorbis_info *info = ov_info(&V->vf, -1);

                /* On success, configure the voice. */

                V->amp  = a;
                V->damp = 0;
                V->chan = info->channels;
                V->play = 1;
                V->loop = 0;

                if (V->amp > 1.0) V->amp = 1.0;
                if (V->amp < 0.0) V->amp = 0.0;

                /* The file will be closed when the Ogg is cleared. */
            }
            else fs_close(fp);
        }
    }
    return V;
}

static void voice_free(struct voice *V)
{
    ov_clear(&V->vf);

    free(V->name);
    free(V);
}

/*---------------------------------------------------------------------------*/

static const int audioBufferLength = 16384;
Uint8 *audioBuffer = NULL;

void audio_step(void *data, Uint8 *_stream, int _length)
{
    Uint8 *stream = audioBuffer;
    int length = audioBufferLength;
    struct voice *V = voices;
    struct voice *P = NULL;

    if(audioBuffer == NULL)
    {
        stream = audioBuffer = malloc(audioBufferLength);
    }

    /* Zero the output buffer. */

    memset(stream, 0, length);

    /* Mix the background music. */

    if (music)
    {
        voice_step(music, music_vol, stream, length);

        /* If the track has faded out, move to a queued track. */

        if (music->amp <= 0.0f && music->damp < 0.0f && queue)
        {
            voice_free(music);
            music = queue;
            queue = NULL;
        }
    }

    /* Iterate over all active voices. */

    while (V)
    {
        /* Mix this voice. */

        if (V->play && voice_step(V, sound_vol, stream, length))
        {
            /* Delete a finished voice... */

            struct voice *T = V;

            if (P)
                V = P->next = V->next;
            else
                V = voices  = V->next;

            voice_free(T);
        }
        else
        {
            /* ... or continue to the next. */

            P = V;
            V = V->next;
        }
    }
}

/*---------------------------------------------------------------------------*/

void audio_init(void)
{
    audio_state = 0;

    /* Configure the audio. */

    spec.format   = AUDIO_S16SYS;
    spec.channels = AUDIO_CHAN;
    spec.samples  = config_get_d(CONFIG_AUDIO_BUFF);
    spec.freq     = AUDIO_RATE;
    spec.callback = audio_step;

    /* Allocate an input buffer. */

    if ((buffer = (short *) malloc(spec.samples * 4)))
    {
        /* Start the audio thread. */

        #ifdef __AVM2__
        audio_state = 1;
        #else
        if (SDL_OpenAudio(&spec, NULL) == 0)
        {
            audio_state = 1;
            SDL_PauseAudio(0);
        }
        else fprintf(stderr, "%s\n", SDL_GetError());
        #endif
    }

    /* Set the initial volumes. */

    audio_volume(config_get_d(CONFIG_SOUND_VOLUME),
                 config_get_d(CONFIG_MUSIC_VOLUME));
}

void audio_free(void)
{
    /* Halt the audio thread. */

    SDL_CloseAudio();

    /* Release the input buffer. */

    free(buffer);

    /* Ogg streams and voice structure remain open to allow quality setting. */
}

void audio_play(const char *filename, float a)
{
    if (audio_state)
    {
        struct voice *V;

        /* If we're already playing this sound, preempt the running copy. */

        SDL_LockAudio();
        {
            for (V = voices; V; V = V->next)
                if (strcmp(V->name, filename) == 0)
                {
                    ov_raw_seek(&V->vf, 0);

                    V->amp = a;

                    if (V->amp > 1.0) V->amp = 1.0;
                    if (V->amp < 0.0) V->amp = 0.0;

                    SDL_UnlockAudio();
                    return;
                }
        }
        SDL_UnlockAudio();

        /* Create a new voice structure. */

        V = voice_init(filename, a);

        /* Add it to the list of sounding voices. */

        SDL_LockAudio();
        {
            V->next = voices;
            voices  = V;
        }
        SDL_UnlockAudio();
    }
}

/*---------------------------------------------------------------------------*/

void audio_music_play(const char *filename)
{
    if (audio_state)
    {
        audio_music_stop();

        SDL_LockAudio();
        {
            if ((music = voice_init(filename, 0.0f)))
            {
                music->loop = 1;
            }
        }
        SDL_UnlockAudio();
    }
}

void audio_music_queue(const char *filename, float t)
{
    if (audio_state)
    {
        SDL_LockAudio();
        {
            if ((queue = voice_init(filename, 0.0f)))
            {
                queue->loop = 1;

                if (t > 0.0f)
                    queue->damp = +1.0f / (AUDIO_RATE * t);
            }
        }
        SDL_UnlockAudio();
    }
}

void audio_music_stop(void)
{
    if (audio_state)
    {
        SDL_LockAudio();
        {
            if (music)
            {
                voice_free(music);
            }
            music = NULL;
        }
        SDL_UnlockAudio();
    }
}

/*---------------------------------------------------------------------------*/

void audio_music_fade_out(float t)
{
    SDL_LockAudio();
    {
        if (music) music->damp = -1.0f / (AUDIO_RATE * t);
    }
    SDL_UnlockAudio();
}

void audio_music_fade_in(float t)
{
    SDL_LockAudio();
    {
        if (music) music->damp = +1.0f / (AUDIO_RATE * t);
    }
    SDL_UnlockAudio();
}

void audio_music_fade_to(float t, const char *filename)
{
    if (music)
    {
        if (strcmp(filename, music->name) != 0)
        {
            audio_music_fade_out(t);
            audio_music_queue(filename, t);
        }
        else
        {
            /*
             * We're fading to the current track.  Chances are,
             * whatever track is still in the queue, we don't want to
             * hear it anymore.
             */

            if (queue)
            {
                voice_free(queue);
                queue = NULL;
            }

            audio_music_fade_in(t);
        }
    }
    else
    {
        audio_music_play(filename);
        audio_music_fade_in(t);
    }
}

void audio_volume(int s, int m)
{
    sound_vol = (float) s / 10.0f;
    music_vol = (float) m / 10.0f;
}

/*---------------------------------------------------------------------------*/
