/*
 * Copyright (C) 2003 Robert Kooima
 *
 * NEVERPUTT is  free software; you can redistribute  it and/or modify
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
#include <string.h>
#include <math.h>

#include "hole.h"
#include "glext.h"
#include "image.h"
#include "game.h"
#include "geom.h"
#include "hud.h"
#include "back.h"
#include "audio.h"
#include "config.h"
#include "fs.h"

/*---------------------------------------------------------------------------*/

struct hole
{
    char   file[MAXSTR];
    char   back[MAXSTR];
    char   song[MAXSTR];
    int    par;
};

static int hole;
static int party;
static int player;
static int count;
static int done;

static int         stat_v[MAXPLY];
static float       ball_p[MAXPLY][3];
static float       ball_e[MAXPLY][3][3];
static struct hole hole_v[MAXHOL];
static int        score_v[MAXHOL][MAXPLY];

/*---------------------------------------------------------------------------*/

static void hole_init_rc(const char *filename)
{
    fs_file fin;
    char buff[MAXSTR];

    hole   = 0;
    player = 0;
    count  = 0;
    done   = 0;

    /* Load the holes list. */

    if ((fin = fs_open(filename, "r")))
    {
        /* Skip shot and description. */

        if (fs_gets(buff, sizeof (buff), fin) &&
            fs_gets(buff, sizeof (buff), fin))
        {
            /* Read the list. */

            while (fs_gets(buff, sizeof (buff), fin) &&
                   sscanf(buff, "%s %s %d %s",
                          hole_v[count].file,
                          hole_v[count].back,
                          &hole_v[count].par,
                          hole_v[count].song) == 4)
                count++;
        }

        fs_close(fin);
    }
}

/*---------------------------------------------------------------------------*/

void hole_init(const char *filename)
{
    int i;

    memset(hole_v,  0, sizeof (struct hole) * MAXHOL);
    memset(score_v, 0, sizeof (int) * MAXPLY * MAXHOL);

    hole_init_rc(filename);

    for (i = 0; i < count; i++)
        score_v[i][0] = hole_v[i].par;
}

void hole_free(void)
{
    game_free();
    back_free();

    count = 0;
}

/*---------------------------------------------------------------------------*/

char *hole_player(int p)
{
    if (p == 0)               return _("Par");

    if (p == 1 && 1 <= party) return _("P1");
    if (p == 2 && 2 <= party) return _("P2");
    if (p == 3 && 3 <= party) return _("P3");
    if (p == 4 && 4 <= party) return _("P4");

    return NULL;
}

char *hole_score(int h, int p)
{
    static char str[MAXSTR];

    if (1 <= h && h <= hole)
    {
        if (h <= hole && 0 <= p && p <= party)
        {
            sprintf(str, "%d", score_v[h][p]);
            return str;
        }
    }
    return NULL;
}

char *hole_tot(int p)
{
    static char str[MAXSTR];

    int h, T = 0;

    if (p <= party)
    {
        for (h = 1; h <= hole && h < count; h++)
            T += score_v[h][p];

        sprintf(str, "%d", T);

        return str;
    }
    return NULL;
}

char *hole_out(int p)
{
    static char str[MAXSTR];

    int h, T = 0;

    if (p <= party)
    {
        for (h = 1; h <= hole && h <= count / 2; h++)
            T += score_v[h][p];

        sprintf(str, "%d", T);

        return str;
    }
    return NULL;
}

char *hole_in(int p)
{
    static char str[MAXSTR];

    int h, T = 0;
    int out = count / 2;

    if (hole > out && p <= party)
    {
        for (h = out + 1; h <= hole && h < count; h++)
            T += score_v[h][p];

        sprintf(str, "%d", T);

        return str;
    }
    return NULL;
}

/*---------------------------------------------------------------------------*/

int curr_hole(void)   { return hole;   }
int curr_party(void)  { return party;  }
int curr_player(void) { return player; }
int curr_count(void)  { return count;  }

const char *curr_scr(void)
{
    static char buf[8];

    sprintf(buf, "%d", score_v[hole][player]);

    return buf;
}

const char *curr_par(void)
{
    static char buf[8];

    sprintf(buf, "%d", score_v[hole][0]);

    return buf;
}

/*---------------------------------------------------------------------------*/

void hole_goto(int h, int p)
{
    int i;

    if (h < count)
    {
        if (h >= 0) hole  = h;
        if (p >= 0) party = p;

        player = (hole - 1) % party + 1;
        done   = 0;

        back_init(hole_v[hole].back, 1);
        game_init(hole_v[hole].file);

        for (i = 1; i <= party; i++)
        {
            game_get_pos(ball_p[i], ball_e[i]);
            stat_v[i] = 0;
        }
        game_ball(player);
        hole_song();
    }
}

int hole_next(void)
{
    if (done < party)
    {
        do
        {
            player = player % party + 1;
        }
        while (stat_v[player]);

        game_ball(player);
        game_get_pos(ball_p[player], ball_e[player]);

        return 1;
    }
    return 0;
}

int hole_move(void)
{
    if (hole + 1 < count)
    {
        hole++;

        game_free();
        back_free();

        hole_goto(hole, party);

        return 1;
    }
    return 0;
}

void hole_goal(void)
{
    score_v[hole][player]++;

    if (score_v[hole][player] == 1)
        audio_play(AUD_ONE, 1.0f);

    else if (score_v[hole][player] == score_v[hole][0] - 2)
        audio_play(AUD_EAGLE, 1.0f);
    else if (score_v[hole][player] == score_v[hole][0] - 1)
        audio_play(AUD_BIRDIE, 1.0f);
    else if (score_v[hole][player] == score_v[hole][0])
        audio_play(AUD_PAR, 1.0f);
    else if (score_v[hole][player] == score_v[hole][0] + 1)
        audio_play(AUD_BOGEY, 1.0f);
    else if (score_v[hole][player] == score_v[hole][0] + 2)
        audio_play(AUD_DOUBLE, 1.0f);
    else
        audio_play(AUD_SUCCESS, 1.0f);

    stat_v[player] = 1;
    done++;

    if (done == party)
        audio_music_fade_out(2.0f);
}

void hole_stop(void)
{
    score_v[hole][player]++;

    /* Cap scores at 12 or par plus 3. */

    if (score_v[hole][player] >= 12 &&
        score_v[hole][player] >= score_v[hole][0] + 3)
    {
        score_v[hole][player] = (score_v[hole][0] > 12 - 3) ? score_v[hole][0] + 3 : 12;
        stat_v[player] = 1;
        done++;
    }
}

void hole_fall(void)
{
    audio_play(AUD_PENALTY, 1.0f);

    /* Reset to the position of the putt, and apply a one-stroke penalty. */

    game_set_pos(ball_p[player], ball_e[player]);
    score_v[hole][player] += 2;

    /* Cap scores at 12 or par plus 3. */

    if (score_v[hole][player] >= 12 &&
        score_v[hole][player] >= score_v[hole][0] + 3)
    {
        score_v[hole][player] = (score_v[hole][0] > 12 - 3) ? score_v[hole][0] + 3 : 12;
        stat_v[player] = 1;
        done++;
    }
}

/*---------------------------------------------------------------------------*/

void hole_song(void)
{
    audio_music_fade_to(0.5f, hole_v[hole].song);
}
