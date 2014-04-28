#ifndef PROGRESS_H
#define PROGRESS_H

/*---------------------------------------------------------------------------*/

void progress_init(int);

int  progress_play(int level);
void progress_step(void);
void progress_stat(int status);
void progress_stop(void);
void progress_exit(void);

int  progress_next_avail(void);
int  progress_next(void);
int  progress_same_avail(void);
int  progress_same(void);

void progress_rename(int);

int  progress_replay(const char *);

int  progress_dead(void);
int  progress_done(void);
int  progress_last(void);

int  progress_lvl_high(void);
int  progress_set_high(void);

int  curr_level(void);
int  curr_balls(void);
int  curr_score(void);
int  curr_mode (void);
int  curr_bonus(void);
int  curr_goal (void);

int  progress_time_rank(void);
int  progress_goal_rank(void);
int  progress_coin_rank(void);

int  progress_times_rank(void);
int  progress_score_rank(void);

int  progress_reward_ball(int);

/*---------------------------------------------------------------------------*/

enum
{
    MODE_NONE = 0,

    MODE_CHALLENGE,
    MODE_NORMAL,

    MODE_MAX
};

const char *mode_to_str(int, int);

/*---------------------------------------------------------------------------*/

#endif
