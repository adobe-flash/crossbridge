#ifndef ST_DEMO_H
#define ST_DEMO_H

#include "state.h"

extern struct state st_demo;
extern struct state st_demo_play;
extern struct state st_demo_end;
extern struct state st_demo_del;
extern struct state st_demo_compat;

void demo_play_goto(int);

#endif
