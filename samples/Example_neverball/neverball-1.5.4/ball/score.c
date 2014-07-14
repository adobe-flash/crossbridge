/*
 * Copyright (C) 2007 Robert Kooima
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

#include <string.h>
#include "score.h"

/*---------------------------------------------------------------------------*/

static int score_time_comp(const struct score *S, int i, int j)
{
    if (S->timer[i] < S->timer[j])
        return 1;

    if (S->timer[i] == S->timer[j] && S->coins[i] > S->coins[j])
        return 1;

    return 0;
}

static int score_coin_comp(const struct score *S, int i, int j)
{
    if (S->coins[i] > S->coins[j])
        return 1;

    if (S->coins[i] == S->coins[j] && S->timer[i] < S->timer[j])
        return 1;

    return 0;
}

static void score_swap(struct score *S, int i, int j)
{
    char player[MAXNAM];
    int  tmp;

    strncpy(player,       S->player[i], MAXNAM - 1);
    strncpy(S->player[i], S->player[j], MAXNAM - 1);
    strncpy(S->player[j], player,       MAXNAM - 1);

    tmp         = S->timer[i];
    S->timer[i] = S->timer[j];
    S->timer[j] = tmp;

    tmp         = S->coins[i];
    S->coins[i] = S->coins[j];
    S->coins[j] = tmp;
}

/*---------------------------------------------------------------------------*/

static void score_insert(struct score *s, int i,
                         const char *player, int timer, int coins)
{
    strncpy(s->player[i], player, MAXNAM - 1);

    s->timer[i] = timer;
    s->coins[i] = coins;
}

void score_init_hs(struct score *s, int timer, int coins)
{
    score_insert(s, 0, "Hard",   timer, coins);
    score_insert(s, 1, "Medium", timer, coins);
    score_insert(s, 2, "Easy",   timer, coins);
    score_insert(s, 3, "",       timer, coins);
}

void score_time_insert(struct score *s, int *rank,
                       const char *player, int timer, int coins)
{
    int i;

    score_insert(s, 3, player, timer, coins);

    if (rank)
    {
        for (i = 2; i >= 0 && score_time_comp(s, i + 1, i); i--)
            score_swap(s, i + 1, i);

        *rank = i + 1;
    }
}

void score_coin_insert(struct score *s, int *rank,
                       const char *player, int timer, int coins)
{
    int i;

    score_insert(s, 3, player, timer, coins);

    if (rank)
    {
        for (i = 2; i >= 0 && score_coin_comp(s, i + 1, i); i--)
            score_swap(s, i + 1, i);

        *rank = i + 1;
    }
}

/*---------------------------------------------------------------------------*/
