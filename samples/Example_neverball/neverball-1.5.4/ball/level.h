#ifndef LEVEL_H
#define LEVEL_H

#include "base_config.h"
#include "score.h"
#include "progress.h"

/*---------------------------------------------------------------------------*/

struct level
{
    /* TODO: turn into an internal structure. */

    char file[PATHMAX];
    char shot[PATHMAX];
    char song[PATHMAX];

    char message[MAXSTR];

    char version[MAXSTR];
    char author[MAXSTR];

    int time; /* Time limit   */
    int goal; /* Coins needed */

    struct
    {
        struct score best_times;
        struct score fast_unlock;
        struct score most_coins;
    }
    score;

    /* Set information. */

    struct set *set;

    int  number;

    /* String representation of the number (eg. "IV") */
    char name[MAXSTR];

    int is_locked;
    int is_bonus;
    int is_completed;
};

int  level_load(const char *, struct level *);

/*---------------------------------------------------------------------------*/

int  level_exists(int);

void level_open  (int);
int  level_opened(int);

void level_complete (int);
int  level_completed(int);

int  level_time(int);
int  level_goal(int);
int  level_bonus(int);

const char *level_shot(int);
const char *level_file(int);
const char *level_name(int);
const char *level_msg (int);

/*---------------------------------------------------------------------------*/

int  level_score_update (int, int, int, int *, int *, int *);
void level_rename_player(int, int, int, int, const char *);

/*---------------------------------------------------------------------------*/

#endif
