#ifndef GAME_SERVER_H
#define GAME_SERVER_H

#include "solid.h"
#include "fs.h"

/*---------------------------------------------------------------------------*/

#define RESPONSE    0.05f              /* Input smoothing time               */
#define ANGLE_BOUND 20.0f              /* Angle limit of floor tilting       */
#define VIEWR_BOUND 10.0f              /* Maximum rate of view rotation      */

/*---------------------------------------------------------------------------*/

int   game_server_init(const char *, int, int);
void  game_server_free(void);
void  game_server_step(float);

void  game_set_goal(void);
void  game_clr_goal(void);

void  game_set_ang(int, int);
void  game_set_pos(int, int);
void  game_set_x  (int);
void  game_set_z  (int);
void  game_set_cam(int);
void  game_set_rot(float);
void  game_set_fly(float, const struct s_file *);

/*---------------------------------------------------------------------------*/

int input_put(fs_file);
int input_get(fs_file);

/*---------------------------------------------------------------------------*/

#endif
