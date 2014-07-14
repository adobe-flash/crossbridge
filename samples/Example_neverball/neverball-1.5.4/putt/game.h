#ifndef GAME_H
#define GAME_H

/*---------------------------------------------------------------------------*/

#define AUD_BIRDIE  "snd/birdie.ogg"
#define AUD_BOGEY   "snd/bogey.ogg"
#define AUD_BUMP    "snd/bink.ogg"
#define AUD_DOUBLE  "snd/double.ogg"
#define AUD_EAGLE   "snd/eagle.ogg"
#define AUD_JUMP    "snd/jump.ogg"
#define AUD_MENU    "snd/menu.ogg"
#define AUD_ONE     "snd/one.ogg"
#define AUD_PAR     "snd/par.ogg"
#define AUD_PENALTY "snd/penalty.ogg"
#define AUD_PLAYER1 "snd/player1.ogg"
#define AUD_PLAYER2 "snd/player2.ogg"
#define AUD_PLAYER3 "snd/player3.ogg"
#define AUD_PLAYER4 "snd/player4.ogg"
#define AUD_SWITCH  "snd/switch.ogg"
#define AUD_SUCCESS "snd/success.ogg"

/*---------------------------------------------------------------------------*/

#define MAX_DT   0.01666666            /* Maximum physics update cycle       */
#define MAX_DN  16                     /* Maximum subdivisions of dt         */
#define FOV     50.00f                 /* Field of view                      */
#define RESPONSE 0.05f                 /* Input smoothing time               */

#define GAME_NONE 0
#define GAME_STOP 1
#define GAME_GOAL 2
#define GAME_FALL 3

/*---------------------------------------------------------------------------*/

void  game_init(const char *);
void  game_free(void);

void  game_draw(int, float);
void  game_putt(void);
int   game_step(const float[3], float);

void  game_update_view(float);

void  game_set_rot(int);
void  game_clr_mag(void);
void  game_set_mag(int);
void  game_set_fly(float);

void  game_ball(int);
void  game_set_pos(float[3], float[3][3]);
void  game_get_pos(float[3], float[3][3]);

/*---------------------------------------------------------------------------*/

#endif
