#ifndef GAME_CLIENT_H
#define GAME_CLIENT_H

#include <stdio.h>
#include "solid.h"
#include "fs.h"

/*---------------------------------------------------------------------------*/

int   game_client_init(const char *);
void  game_client_free(void);
void  game_client_step(fs_file);

int   curr_clock(void);
int   curr_coins(void);
int   curr_status(void);

void  game_draw(int, float);

void  game_look(float, float);

void  game_kill_fade(void);
void  game_step_fade(float);
void  game_fade(float);

/*---------------------------------------------------------------------------*/

extern int           game_compat_map;
const struct s_file *game_client_file(void);

/*---------------------------------------------------------------------------*/

#endif
