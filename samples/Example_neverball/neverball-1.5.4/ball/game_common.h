#ifndef GAME_COMMON_H
#define GAME_COMMON_H

#include "lang.h"

/*---------------------------------------------------------------------------*/

#define AUD_MENU    "snd/menu.ogg"
#define AUD_START _("snd/select.ogg")
#define AUD_READY _("snd/ready.ogg")
#define AUD_SET   _("snd/set.ogg")
#define AUD_GO    _("snd/go.ogg")
#define AUD_BALL    "snd/ball.ogg"
#define AUD_BUMPS   "snd/bumplil.ogg"
#define AUD_BUMPM   "snd/bump.ogg"
#define AUD_BUMPL   "snd/bumpbig.ogg"
#define AUD_COIN    "snd/coin.ogg"
#define AUD_TICK    "snd/tick.ogg"
#define AUD_TOCK    "snd/tock.ogg"
#define AUD_SWITCH  "snd/switch.ogg"
#define AUD_JUMP    "snd/jump.ogg"
#define AUD_GOAL    "snd/goal.ogg"
#define AUD_SCORE _("snd/record.ogg")
#define AUD_FALL  _("snd/fall.ogg")
#define AUD_TIME  _("snd/time.ogg")
#define AUD_OVER  _("snd/over.ogg")
#define AUD_GROW    "snd/grow.ogg"
#define AUD_SHRINK  "snd/shrink.ogg"

/*---------------------------------------------------------------------------*/

enum
{
    GAME_NONE = 0,

    GAME_TIME,
    GAME_GOAL,
    GAME_FALL,

    GAME_MAX
};

const char *status_to_str(int);

/*---------------------------------------------------------------------------*/

enum
{
    VIEW_NONE = -1,

    VIEW_CHASE,
    VIEW_LAZY,
    VIEW_MANUAL,
    VIEW_TOPDOWN,

    VIEW_MAX
};

const char *view_to_str(int);

/*---------------------------------------------------------------------------*/

struct game_tilt
{
    float x[3], rx;
    float z[3], rz;
};

void game_tilt_init(struct game_tilt *);
void game_tilt_axes(struct game_tilt *, float view_e[3][3]);
void game_tilt_grav(float h[3], const float g[3], const struct game_tilt *);

/*---------------------------------------------------------------------------*/

#endif
