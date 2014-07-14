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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "demo.h"
#include "audio.h"
#include "solid.h"
#include "config.h"
#include "binary.h"
#include "common.h"
#include "level.h"
#include "array.h"
#include "dir.h"

#include "game_server.h"
#include "game_client.h"
#include "game_proxy.h"

/*---------------------------------------------------------------------------*/

#define MAGIC           0x52424EAF
#define DEMO_VERSION    9

#define DATELEN 20

static fs_file demo_fp;

/*---------------------------------------------------------------------------*/

static int demo_header_read(fs_file fp, struct demo *d)
{
    int magic;
    int version;
    int t;

    struct tm date;
    char datestr[DATELEN];

    get_index(fp, &magic);
    get_index(fp, &version);

    get_index(fp, &t);

    if (magic == MAGIC && version == DEMO_VERSION && t)
    {
        d->timer = t;

        get_index(fp, &d->coins);
        get_index(fp, &d->status);
        get_index(fp, &d->mode);

        get_string(fp, d->player, sizeof (d->player));
        get_string(fp, datestr, DATELEN);

        sscanf(datestr,
               "%d-%d-%dT%d:%d:%d",
               &date.tm_year,
               &date.tm_mon,
               &date.tm_mday,
               &date.tm_hour,
               &date.tm_min,
               &date.tm_sec);

        date.tm_year -= 1900;
        date.tm_mon  -= 1;
        date.tm_isdst = -1;

        d->date = make_time_from_utc(&date);

        get_string(fp, d->shot, PATHMAX);
        get_string(fp, d->file, PATHMAX);

        get_index(fp, &d->time);
        get_index(fp, &d->goal);
        get_index(fp, &d->goal_e);
        get_index(fp, &d->score);
        get_index(fp, &d->balls);
        get_index(fp, &d->times);

        return 1;
    }
    return 0;
}

static void demo_header_write(fs_file fp, struct demo *d)
{
    int magic = MAGIC;
    int version = DEMO_VERSION;
    int zero  = 0;

    char datestr[DATELEN];

    strftime(datestr, DATELEN, "%Y-%m-%dT%H:%M:%S", gmtime(&d->date));

    put_index(fp, &magic);
    put_index(fp, &version);
    put_index(fp, &zero);
    put_index(fp, &zero);
    put_index(fp, &zero);
    put_index(fp, &d->mode);

    put_string(fp, d->player);
    put_string(fp, datestr);

    put_string(fp, d->shot);
    put_string(fp, d->file);

    put_index(fp, &d->time);
    put_index(fp, &d->goal);
    put_index(fp, &d->goal_e);
    put_index(fp, &d->score);
    put_index(fp, &d->balls);
    put_index(fp, &d->times);
}

/*---------------------------------------------------------------------------*/

struct demo *demo_load(const char *path)
{
    fs_file fp;
    struct demo *d;

    d = NULL;

    if ((fp = fs_open(path, "r")))
    {
        d = calloc(1, sizeof (struct demo));

        if (demo_header_read(fp, d))
        {
            strncpy(d->filename, path, MAXSTR - 1);
            strncpy(d->name, base_name(d->filename, ".nbr"), PATHMAX - 1);
            d->name[PATHMAX - 1] = '\0';
        }
        else
        {
            free(d);
            d = NULL;
        }

        fs_close(fp);
    }

    return d;
}

void demo_free(struct demo *d)
{
    free(d);
}

/*---------------------------------------------------------------------------*/

static const char *demo_path(const char *name)
{
    static char path[MAXSTR];
    sprintf(path, "Replays/%s.nbr", name);
    return path;
}

/*---------------------------------------------------------------------------*/

int demo_exists(const char *name)
{
    return fs_exists(demo_path(name));
}

#define MAXSTRLEN(a) (sizeof ((a)) - 1)

const char *demo_format_name(const char *fmt,
                             const char *set,
                             const char *level)
{
    static char name[MAXSTR];
    int space_left;
    char *numpart;
    int i;

    if (!fmt)
        return NULL;

    memset(name, 0, sizeof (name));
    space_left = MAXSTRLEN(name);

    /* Construct name, replacing each format sequence as appropriate. */

    while (*fmt && space_left > 0)
    {
        if (*fmt == '%')
        {
            fmt++;

            switch (*fmt)
            {
            case 's':
                if (set)
                {
                    strncat(name, set, space_left);
                    space_left -= strlen(set);
                }
                break;

            case 'l':
                if (level)
                {
                    strncat(name, level, space_left);
                    space_left -= strlen(level);
                }
                break;

            case '%':
                strncat(name, "%", space_left);
                space_left--;
                break;

            case '\0':
                fputs(L_("Missing format character in replay name\n"), stderr);
                fmt--;
                break;

            default:
                fprintf(stderr, L_("Invalid format character in "
                                   "replay name: \"%%%c\"\n"), *fmt);
                break;
            }
        }
        else
        {
            strncat(name, fmt, 1);
            space_left--;
        }

        fmt++;
    }

    /*
     * Append a unique 2-digit number preceded by an underscore to the
     * file name, discarding characters if there's not enough space
     * left in the buffer.
     */

    if (space_left < strlen("_23"))
        numpart = name + MAXSTRLEN(name) - strlen("_23");
    else
        numpart = name + MAXSTRLEN(name) - space_left;

    for (i = 1; i < 100; i++)
    {
        sprintf(numpart, "_%02d", i);

        if (!demo_exists(name))
            break;
    }

    return name;
}

#undef MAXSTRLEN

/*---------------------------------------------------------------------------*/

int demo_play_init(const char *name, const struct level *level,
                   int mode, int t, int g, int e, int s, int b, int tt)
{
    struct demo demo;

    memset(&demo, 0, sizeof (demo));

    strncpy(demo.filename, demo_path(name), sizeof (demo.filename) - 1);

    demo.mode = mode;
    demo.date = time(NULL);

    strncpy(demo.player, config_get_s(CONFIG_PLAYER), sizeof (demo.player) - 1);

    strncpy(demo.shot, level->shot, PATHMAX - 1);
    strncpy(demo.file, level->file, PATHMAX - 1);

    demo.time   = t;
    demo.goal   = g;
    demo.goal_e = e;
    demo.score  = s;
    demo.balls  = b;
    demo.times  = tt;

    if ((demo_fp = fs_open(demo.filename, "w")))
    {
        demo_header_write(demo_fp, &demo);
        audio_music_fade_to(2.0f, level->song);

        /*
         * Init both client and server, then process the first batch
         * of commands generated by the server to sync client to
         * server.
         */

        if (game_client_init(level->file) &&
            game_server_init(level->file, t, e))
        {
            game_client_step(demo_fp);
            return 1;
        }
    }
    return 0;
}

void demo_play_stat(int status, int coins, int timer)
{
    if (demo_fp)
    {
        long pos = fs_tell(demo_fp);

        fs_seek(demo_fp, 8, SEEK_SET);

        put_index(demo_fp, &timer);
        put_index(demo_fp, &coins);
        put_index(demo_fp, &status);

        fs_seek(demo_fp, pos, SEEK_SET);
    }
}

void demo_play_stop(void)
{
    if (demo_fp)
    {
        fs_close(demo_fp);
        demo_fp = NULL;
    }
}

int demo_saved(void)
{
    return demo_exists(USER_REPLAY_FILE);
}

void demo_rename(const char *name)
{
    char src[MAXSTR];
    char dst[MAXSTR];

    if (name &&
        demo_exists(USER_REPLAY_FILE) &&
        strcmp(name, USER_REPLAY_FILE) != 0)
    {
        strncpy(src, demo_path(USER_REPLAY_FILE), sizeof (src) - 1);
        strncpy(dst, demo_path(name),             sizeof (dst) - 1);

        fs_rename(src, dst);
    }
}

void demo_rename_player(const char *name, const char *player)
{
#if 0
    char filename[MAXSTR];
    FILE *old_fp, *new_fp;
    struct demo d;

    assert(name);
    assert(player);

    /* TODO: make this reusable. */

    filename[sizeof (filename) - 1] = '\0';
    strncpy(filename, name, sizeof (filename) - 1);
    strncat(filename, REPLAY_EXT, sizeof (filename) - 1 - strlen(name));

    /*
     * Write out a temporary file containing the original replay data with a
     * new player name, then copy the resulting contents back to the original
     * file.
     *
     * (It is believed that the ugliness found here is outweighed by the
     * benefits of preserving the arbitrary-length property of all strings in
     * the replay.  In case of doubt, FIXME.)
     */

    if ((old_fp = fopen(config_user(filename), FMODE_RB)))
    {
        if ((new_fp = tmpfile()))
        {
            if (demo_header_read(old_fp, &d))
            {
                FILE *save_fp;

                /* Modify and write the header. */

                strncpy(d.player, player, sizeof (d.player));

                demo_header_write(new_fp, &d);

                /*
                 * Restore the last three fields not written by the above call.
                 */

                /* Hack, hack, hack. */

                save_fp = demo_fp;
                demo_fp = new_fp;

                demo_play_stat(d.status, d.coins, d.timer);

                demo_fp = save_fp;

                /* Copy the remaining data. */

                file_copy(old_fp, new_fp);

                /* Then copy everything back. */

                if (freopen(config_user(filename), FMODE_WB, old_fp))
                {
                    fseek(new_fp, 0L, SEEK_SET);
                    file_copy(new_fp, old_fp);
                }
            }
            fclose(new_fp);
        }
        fclose(old_fp);
    }
#endif
}

/*---------------------------------------------------------------------------*/

static struct demo  demo_replay;       /* The current demo */
static struct level demo_level_replay; /* The current level demo-ed*/

const struct demo *curr_demo_replay(void)
{
    return &demo_replay;
}

int demo_replay_init(const char *name, int *g, int *m, int *b, int *s, int *tt)
{
    demo_fp = fs_open(name, "r");

    if (demo_fp && demo_header_read(demo_fp, &demo_replay))
    {
        strncpy(demo_replay.filename, name, MAXSTR - 1);
        strncpy(demo_replay.name,
                base_name(demo_replay.filename, ".nbr"),
                PATHMAX - 1);

        if (level_load(demo_replay.file, &demo_level_replay))
        {
            demo_level_replay.time = demo_replay.time;
            demo_level_replay.goal = demo_replay.goal;
        }
        else
            return 0;

        if (g)  *g  = demo_replay.goal;
        if (m)  *m  = demo_replay.mode;
        if (b)  *b  = demo_replay.balls;
        if (s)  *s  = demo_replay.score;
        if (tt) *tt = demo_replay.times;

        /*
         * Init client and then read and process the first batch of
         * commands from the replay file.
         */

        if (g)
        {
            audio_music_fade_to(0.5f, demo_level_replay.song);

            return (game_client_init(demo_level_replay.file) &&
                    demo_replay_step(0.0f));
        }

        /* Likewise, but also queue a command to open the goal. */

        else if (game_client_init(demo_level_replay.file))
        {
            union cmd cmd;

            cmd.type = CMD_GOAL_OPEN;
            game_proxy_enq(&cmd);

            return demo_replay_step(0.0f);
        }
    }
    return 0;
}

/*
 * Read and enqueue commands from the replay file, up to and including
 * CMD_END_OF_UPDATE.  Return 0 on EOF.  Otherwise, run the commands
 * on the client and return 1.
 */
int demo_replay_step(float dt)
{
    if (demo_fp)
    {
        union cmd cmd;

        while (cmd_get(demo_fp, &cmd))
        {
            game_proxy_enq(&cmd);

            if (cmd.type == CMD_END_OF_UPDATE)
                break;
        }

        if (!fs_eof(demo_fp))
        {
            game_client_step(NULL);
            return 1;
        }
    }
    return 0;
}

void demo_replay_stop(int d)
{
    if (demo_fp)
    {
        fs_close(demo_fp);
        demo_fp = NULL;

        if (d) fs_remove(demo_replay.filename);
    }
}

/*---------------------------------------------------------------------------*/

fs_file demo_file(void) { return demo_fp; }

/*---------------------------------------------------------------------------*/
