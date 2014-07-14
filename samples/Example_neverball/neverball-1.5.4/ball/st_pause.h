#ifndef ST_PAUSE_H
#define ST_PAUSE_H

#include "state.h"

extern struct state st_pause;

int  goto_pause(void);
int  is_paused(void);
void clear_pause(void);

#endif
