#ifndef HOLE_H
#define HOLE_H

/*---------------------------------------------------------------------------*/

#define MAXHOL 28
#define MAXPLY 5

void  hole_init(const char *);
void  hole_free(void);
int   hole_exists(int);

char *hole_player(int);
char *hole_score(int, int);
char *hole_tot(int);
char *hole_out(int);
char *hole_in(int);

int  curr_hole(void);
int  curr_party(void);
int  curr_player(void);
int  curr_stroke(void);
int  curr_count(void);

const char *curr_scr(void);
const char *curr_par(void);

void hole_goto(int, int);
int  hole_next(void);
int  hole_move(void);
void hole_goal(void);
void hole_stop(void);
void hole_fall(void);

void hole_song(void);

/*---------------------------------------------------------------------------*/

#endif
