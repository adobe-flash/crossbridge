#ifndef SCORE_H
#define SCORE_H

#include "base_config.h"

/*---------------------------------------------------------------------------*/

#define NSCORE 3

struct score
{
    char player[NSCORE + 1][MAXNAM];

    int timer [NSCORE + 1]; /* Time elapsed    */
    int coins [NSCORE + 1]; /* Coins collected */
};

/*---------------------------------------------------------------------------*/

void score_init_hs(struct score *, int, int);

void score_time_insert(struct score *, int *, const char *, int, int);
void score_coin_insert(struct score *, int *, const char *, int, int);

/*---------------------------------------------------------------------------*/

#endif
