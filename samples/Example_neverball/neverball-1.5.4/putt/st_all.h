#ifndef ST_TITLE_H
#define ST_TITLE_H

#include "state.h"

extern struct state st_title;
extern struct state st_course;
extern struct state st_party;
extern struct state st_next;
extern struct state st_poser;
extern struct state st_flyby;
extern struct state st_stroke;
extern struct state st_roll;
extern struct state st_goal;
extern struct state st_stop;
extern struct state st_fall;
extern struct state st_score;
extern struct state st_over;
extern struct state st_pause;

void set_joystick(SDL_Joystick *);

int goto_pause(struct state *, int);

#endif
