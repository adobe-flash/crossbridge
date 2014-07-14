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
#include <math.h>
#include <assert.h>

#include "solid.h"
#include "config.h"
#include "level.h"
#include "set.h"

/*---------------------------------------------------------------------------*/

static void scan_level_attribs(struct level *l, const struct s_file *fp)
{
    int i;

    int have_goal = 0, have_time = 0;
    int need_bt_easy = 0, need_fu_easy = 0, need_mc_easy = 0;

    for (i = 0; i < fp->dc; i++)
    {
        char *k = fp->av + fp->dv[i].ai;
        char *v = fp->av + fp->dv[i].aj;

        if (strcmp(k, "message") == 0)
            strncpy(l->message, v, MAXSTR - 1);
        else if (strcmp(k, "song") == 0)
            strncpy(l->song, v, PATHMAX - 1);
        else if (strcmp(k, "shot") == 0)
            strncpy(l->shot, v, PATHMAX - 1);
        else if (strcmp(k, "goal") == 0)
        {
            l->goal = atoi(v);
            have_goal = 1;
        }
        else if (strcmp(k, "time") == 0)
        {
            l->time = atoi(v);
            have_time = 1;
        }
        else if (strcmp(k, "time_hs") == 0)
        {
            switch (sscanf(v, "%d %d %d",
                           &l->score.best_times.timer[0],
                           &l->score.best_times.timer[1],
                           &l->score.best_times.timer[2]))
            {
            case 2: need_bt_easy = 1; break;
            case 3:                   break;

            default:
                /* TODO, complain loudly? */
                break;
            }
        }
        else if (strcmp(k, "goal_hs") == 0)
        {
            switch (sscanf(v, "%d %d %d",
                           &l->score.fast_unlock.timer[0],
                           &l->score.fast_unlock.timer[1],
                           &l->score.fast_unlock.timer[2]))
            {
            case 2: need_fu_easy = 1; break;
            case 3:                   break;

            default:
                /* TODO, complain loudly? */
                break;
            }
        }
        else if (strcmp(k, "coin_hs") == 0)
        {
            switch (sscanf(v, "%d %d %d",
                           &l->score.most_coins.coins[0],
                           &l->score.most_coins.coins[1],
                           &l->score.most_coins.coins[2]))
            {
            case 2: need_mc_easy = 1; break;
            case 3:                   break;

            default:
                /* TODO, complain loudly? */
                break;
            }
        }
        else if (strcmp(k, "version") == 0)
            strncpy(l->version, v, MAXSTR - 1);
        else if (strcmp(k, "author") == 0)
            strncpy(l->author, v, MAXSTR - 1);
        else if (strcmp(k, "bonus") == 0)
            l->is_bonus = atoi(v) ? 1 : 0;
    }

    if (have_goal)
    {
        if (need_mc_easy)
            l->score.most_coins.coins[2] = l->goal;

        l->score.fast_unlock.coins[0] =
            l->score.fast_unlock.coins[1] =
            l->score.fast_unlock.coins[2] = l->goal;
    }

    if (have_time)
    {
        if (need_bt_easy)
            l->score.best_times.timer[2] = l->time;
        if (need_fu_easy)
            l->score.fast_unlock.timer[2] = l->time;

        l->score.most_coins.timer[0] =
            l->score.most_coins.timer[1] =
            l->score.most_coins.timer[2] = l->time;
    }
}

int level_load(const char *filename, struct level *level)
{
    struct s_file sol;

    int money;
    int i;

    memset(level, 0, sizeof (struct level));
    memset(&sol,  0, sizeof (sol));

    if (!sol_load_only_head(&sol, filename))
    {
        fprintf(stderr, L_("Failure to load level file '%s'\n"), filename);
        return 0;
    }

    strncpy(level->file, filename, PATHMAX - 1);

    score_init_hs(&level->score.best_times, 59999, 0);
    score_init_hs(&level->score.fast_unlock, 59999, 0);
    score_init_hs(&level->score.most_coins, 59999, 0);

    money = 0;

    for (i = 0; i < sol.hc; i++)
        if (sol.hv[i].t == ITEM_COIN)
            money += sol.hv[i].n;

    level->score.most_coins.coins[0] = money;

    scan_level_attribs(level, &sol);

    /* Compute initial hs default values */

#define HOP(t, c) \
    if (t[2] c t[0]) \
        t[0] = t[1] = t[2]; \
    else if (t[2] c t[1]) \
        t[1] = (t[0] + t[2]) / 2

    HOP(level->score.best_times.timer, <=);
    HOP(level->score.fast_unlock.timer, <=);
    HOP(level->score.most_coins.coins, >=);

    sol_free(&sol);

    return 1;
}

/*---------------------------------------------------------------------------*/

int  level_exists(int i)
{
    return set_level_exists(curr_set(), i);
}

void level_open(int i)
{
    if (level_exists(i))
        get_level(i)->is_locked = 0;
}

int  level_opened(int i)
{
    return level_exists(i) && !get_level(i)->is_locked;
}

void level_complete(int i)
{
    if (level_exists(i))
        get_level(i)->is_completed = 1;
}

int  level_completed(int i)
{
    return level_exists(i) && get_level(i)->is_completed;
}

int  level_time (int i)
{
    assert(level_exists(i));
    return get_level(i)->time;
}

int  level_goal (int i)
{
    assert(level_exists(i));
    return get_level(i)->goal;
}

int  level_bonus(int i)
{
    return level_exists(i) && get_level(i)->is_bonus;
}

const char *level_shot(int i)
{
    return level_exists(i) ? get_level(i)->shot : NULL;
}

const char *level_file(int i)
{
    return level_exists(i) ? get_level(i)->file : NULL;
}

const char *level_name(int i)
{
    return level_exists(i) ? get_level(i)->name : NULL;
}

const char *level_msg(int i)
{
    if (level_exists(i) && strlen(get_level(i)->message) > 0)
        return _(get_level(i)->message);

    return NULL;
}

/*---------------------------------------------------------------------------*/

int level_score_update(int level,
                       int timer,
                       int coins,
                       int *time_rank,
                       int *goal_rank,
                       int *coin_rank)
{
    struct level *l = get_level(level);
    const char *player =  config_get_s(CONFIG_PLAYER);

    score_time_insert(&l->score.best_times,  time_rank, player, timer, coins);
    score_time_insert(&l->score.fast_unlock, goal_rank, player, timer, coins);
    score_coin_insert(&l->score.most_coins,  coin_rank, player, timer, coins);

    if ((time_rank && *time_rank < 3) ||
        (goal_rank && *goal_rank < 3) ||
        (coin_rank && *coin_rank < 3))
        return 1;
    else
        return 0;
}

void level_rename_player(int level,
                         int time_rank,
                         int goal_rank,
                         int coin_rank,
                         const char *player)
{
    struct level *l = get_level(level);

    strncpy(l->score.best_times.player [time_rank], player, MAXNAM - 1);
    strncpy(l->score.fast_unlock.player[goal_rank], player, MAXNAM - 1);
    strncpy(l->score.most_coins.player [coin_rank], player, MAXNAM - 1);
}

/*---------------------------------------------------------------------------*/

