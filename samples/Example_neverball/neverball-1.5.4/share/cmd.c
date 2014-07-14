/*
 * Copyright (C) 2009 Neverball contributors
 *
 * NEVERBALL is  free software; you can redistribute  it and/or modify
 * it under the  terms of the GNU General  Public License as published
 * by the Free  Software Foundation; either version 2  of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT  ANY  WARRANTY;  without   even  the  implied  warranty  of
 * MERCHANTABILITY or  FITNESS FOR A PARTICULAR PURPOSE.   See the GNU
 * General Public License for more details.
 */

#include <stdio.h>
#include <assert.h>

#include "cmd.h"
#include "binary.h"
#include "base_config.h"
#include "common.h"

/*---------------------------------------------------------------------------*/

static int cmd_stats = 0;

/*---------------------------------------------------------------------------*/

/*
 * Let's pretend these aren't Ridiculously Convoluted Macros from
 * Hell, and that all this looks pretty straight-forward.  (In all
 * fairness, the macros have paid off.)
 *
 * A command's "write" and "read" functions are defined by calling the
 * PUT_FUNC or the GET_FUNC macro, respectively, with the command type
 * as argument, followed by the body of the function (which has
 * variables "fp" and "cmd" available), and finalised with the
 * END_FUNC macro, which must be terminated with a semi-colon.  Before
 * the function definitions, the BYTES macro must be redefined for
 * each command to an expression evaluating to the number of bytes
 * that the command will occupy in the file.  (See existing commands
 * for examples.)
 */

#define PUT_FUNC(t)                                                     \
    static void cmd_put_ ## t(fs_file fp, const union cmd *cmd) {       \
    const char *cmd_name = #t;                                          \
                                                                        \
    /* This is a write, so BYTES should be safe to eval already. */     \
    short cmd_bytes = BYTES;                                            \
                                                                        \
    /* Write command size info (right after the command type). */       \
    put_short(fp, &cmd_bytes);                                          \
                                                                        \
    /* Start the stats output. */                                       \
    if (cmd_stats) printf("put");                                       \

#define GET_FUNC(t)                                             \
    static void cmd_get_ ## t(fs_file fp, union cmd *cmd) {     \
    const char *cmd_name = #t;                                  \
                                                                \
    /* This is a read, so we'll have to eval BYTES later. */    \
    short cmd_bytes = -1;                                       \
                                                                \
    /* Start the stats output. */                               \
    if (cmd_stats) printf("get");

#define END_FUNC                                                        \
    if (cmd_bytes < 0) cmd_bytes = BYTES;                               \
                                                                        \
    /* Finish the stats output. */                                      \
    if (cmd_stats) printf("\t%s\t%d\n", cmd_name, cmd_bytes);           \
    } struct dummy              /* Allows a trailing semi-colon. */

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES 0

PUT_FUNC(CMD_END_OF_UPDATE) { } END_FUNC;
GET_FUNC(CMD_END_OF_UPDATE) { } END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES 0

PUT_FUNC(CMD_MAKE_BALL) { } END_FUNC;
GET_FUNC(CMD_MAKE_BALL) { } END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES (ARRAY_BYTES(3) + INDEX_BYTES + INDEX_BYTES)

PUT_FUNC(CMD_MAKE_ITEM)
{
    put_array(fp, cmd->mkitem.p, 3);
    put_index(fp, &cmd->mkitem.t);
    put_index(fp, &cmd->mkitem.n);
}
END_FUNC;

GET_FUNC(CMD_MAKE_ITEM)
{
    get_array(fp, cmd->mkitem.p, 3);
    get_index(fp, &cmd->mkitem.t);
    get_index(fp, &cmd->mkitem.n);
}
END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES INDEX_BYTES

PUT_FUNC(CMD_PICK_ITEM)
{
    put_index(fp, &cmd->pkitem.hi);
}
END_FUNC;

GET_FUNC(CMD_PICK_ITEM)
{
    get_index(fp, &cmd->pkitem.hi);
}
END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES (FLOAT_BYTES + FLOAT_BYTES)

PUT_FUNC(CMD_TILT_ANGLES)
{
    put_float(fp, &cmd->tiltangles.x);
    put_float(fp, &cmd->tiltangles.z);
}
END_FUNC;

GET_FUNC(CMD_TILT_ANGLES)
{
    get_float(fp, &cmd->tiltangles.x);
    get_float(fp, &cmd->tiltangles.z);
}
END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES (STRING_BYTES(cmd->sound.n) + FLOAT_BYTES)

PUT_FUNC(CMD_SOUND)
{
    put_string(fp, cmd->sound.n);
    put_float(fp, &cmd->sound.a);
}
END_FUNC;

GET_FUNC(CMD_SOUND)
{
    static char buff[MAXSTR];

    get_string(fp, buff, sizeof (buff));
    get_float(fp, &cmd->sound.a);

    cmd->sound.n = strdup(buff);
}
END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES FLOAT_BYTES

PUT_FUNC(CMD_TIMER)
{
    put_float(fp, &cmd->timer.t);
}
END_FUNC;

GET_FUNC(CMD_TIMER)
{
    get_float(fp, &cmd->timer.t);
}
END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES INDEX_BYTES

PUT_FUNC(CMD_STATUS)
{
    put_index(fp, &cmd->status.t);
}
END_FUNC;

GET_FUNC(CMD_STATUS)
{
    get_index(fp, &cmd->status.t);
}
END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES INDEX_BYTES

PUT_FUNC(CMD_COINS)
{
    put_index(fp, &cmd->coins.n);
}
END_FUNC;

GET_FUNC(CMD_COINS)
{
    get_index(fp, &cmd->coins.n);
}
END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES 0

PUT_FUNC(CMD_JUMP_ENTER) { } END_FUNC;
GET_FUNC(CMD_JUMP_ENTER) { } END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES 0

PUT_FUNC(CMD_JUMP_EXIT) { } END_FUNC;
GET_FUNC(CMD_JUMP_EXIT) { } END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES (INDEX_BYTES + INDEX_BYTES)

PUT_FUNC(CMD_BODY_PATH)
{
    put_index(fp, &cmd->bodypath.bi);
    put_index(fp, &cmd->bodypath.pi);
}
END_FUNC;

GET_FUNC(CMD_BODY_PATH)
{
    get_index(fp, &cmd->bodypath.bi);
    get_index(fp, &cmd->bodypath.pi);
}
END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES (INDEX_BYTES + FLOAT_BYTES)

PUT_FUNC(CMD_BODY_TIME)
{
    put_index(fp, &cmd->bodytime.bi);
    put_float(fp, &cmd->bodytime.t);
}
END_FUNC;

GET_FUNC(CMD_BODY_TIME)
{
    get_index(fp, &cmd->bodytime.bi);
    get_float(fp, &cmd->bodytime.t);
}
END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES 0

PUT_FUNC(CMD_GOAL_OPEN) { } END_FUNC;
GET_FUNC(CMD_GOAL_OPEN) { } END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES INDEX_BYTES

PUT_FUNC(CMD_SWCH_ENTER)
{
    put_index(fp, &cmd->swchenter.xi);
}
END_FUNC;

GET_FUNC(CMD_SWCH_ENTER)
{
    get_index(fp, &cmd->swchenter.xi);
}
END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES INDEX_BYTES

PUT_FUNC(CMD_SWCH_TOGGLE)
{
    put_index(fp, &cmd->swchenter.xi);
}
END_FUNC;

GET_FUNC(CMD_SWCH_TOGGLE)
{
    get_index(fp, &cmd->swchenter.xi);
}
END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES INDEX_BYTES

PUT_FUNC(CMD_SWCH_EXIT)
{
    put_index(fp, &cmd->swchenter.xi);
}
END_FUNC;

GET_FUNC(CMD_SWCH_EXIT)
{
    get_index(fp, &cmd->swchenter.xi);
}
END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES INDEX_BYTES

PUT_FUNC(CMD_UPDATES_PER_SECOND)
{
    put_index(fp, &cmd->ups.n);
}
END_FUNC;

GET_FUNC(CMD_UPDATES_PER_SECOND)
{
    get_index(fp, &cmd->ups.n);
}
END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES FLOAT_BYTES

PUT_FUNC(CMD_BALL_RADIUS)
{
    put_float(fp, &cmd->ballradius.r);
}
END_FUNC;

GET_FUNC(CMD_BALL_RADIUS)
{
    get_float(fp, &cmd->ballradius.r);
}
END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES 0

PUT_FUNC(CMD_CLEAR_ITEMS) { } END_FUNC;
GET_FUNC(CMD_CLEAR_ITEMS) { } END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES 0

PUT_FUNC(CMD_CLEAR_BALLS) { } END_FUNC;
GET_FUNC(CMD_CLEAR_BALLS) { } END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES ARRAY_BYTES(3)

PUT_FUNC(CMD_BALL_POSITION)
{
    put_array(fp, cmd->ballpos.p, 3);
}
END_FUNC;

GET_FUNC(CMD_BALL_POSITION)
{
    get_array(fp, cmd->ballpos.p, 3);
}
END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES (ARRAY_BYTES(3) + ARRAY_BYTES(3))

PUT_FUNC(CMD_BALL_BASIS)
{
    put_array(fp, cmd->ballbasis.e[0], 3);
    put_array(fp, cmd->ballbasis.e[1], 3);
}
END_FUNC;

GET_FUNC(CMD_BALL_BASIS)
{
    get_array(fp, cmd->ballbasis.e[0], 3);
    get_array(fp, cmd->ballbasis.e[1], 3);
}
END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES (ARRAY_BYTES(3) + ARRAY_BYTES(3))

PUT_FUNC(CMD_BALL_PEND_BASIS)
{
    put_array(fp, cmd->ballpendbasis.E[0], 3);
    put_array(fp, cmd->ballpendbasis.E[1], 3);
}
END_FUNC;

GET_FUNC(CMD_BALL_PEND_BASIS)
{
    get_array(fp, cmd->ballpendbasis.E[0], 3);
    get_array(fp, cmd->ballpendbasis.E[1], 3);
}
END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES ARRAY_BYTES(3)

PUT_FUNC(CMD_VIEW_POSITION)
{
    put_array(fp, cmd->viewpos.p, 3);
}
END_FUNC;

GET_FUNC(CMD_VIEW_POSITION)
{
    get_array(fp, cmd->viewpos.p, 3);
}
END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES ARRAY_BYTES(3)

PUT_FUNC(CMD_VIEW_CENTER)
{
    put_array(fp, cmd->viewcenter.c, 3);
}
END_FUNC;

GET_FUNC(CMD_VIEW_CENTER)
{
    get_array(fp, cmd->viewcenter.c, 3);
}
END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES (ARRAY_BYTES(3) + ARRAY_BYTES(3))

PUT_FUNC(CMD_VIEW_BASIS)
{
    put_array(fp, cmd->viewbasis.e[0], 3);
    put_array(fp, cmd->viewbasis.e[1], 3);
}
END_FUNC;

GET_FUNC(CMD_VIEW_BASIS)
{
    get_array(fp, cmd->viewbasis.e[0], 3);
    get_array(fp, cmd->viewbasis.e[1], 3);
}
END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES INDEX_BYTES

PUT_FUNC(CMD_CURRENT_BALL)
{
    put_index(fp, &cmd->currball.ui);
}
END_FUNC;

GET_FUNC(CMD_CURRENT_BALL)
{
    get_index(fp, &cmd->currball.ui);
}
END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES (INDEX_BYTES + INDEX_BYTES)

PUT_FUNC(CMD_PATH_FLAG)
{
    put_index(fp, &cmd->pathflag.pi);
    put_index(fp, &cmd->pathflag.f);
}
END_FUNC;

GET_FUNC(CMD_PATH_FLAG)
{
    get_index(fp, &cmd->pathflag.pi);
    get_index(fp, &cmd->pathflag.f);
}
END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES FLOAT_BYTES

PUT_FUNC(CMD_STEP_SIMULATION)
{
    put_float(fp, &cmd->stepsim.dt);
}
END_FUNC;

GET_FUNC(CMD_STEP_SIMULATION)
{
    get_float(fp, &cmd->stepsim.dt);
}
END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES STRING_BYTES(cmd->map.name) + INDEX_BYTES * 2

PUT_FUNC(CMD_MAP)
{
    put_string(fp, cmd->map.name);

    put_index(fp, &cmd->map.version.x);
    put_index(fp, &cmd->map.version.y);
}
END_FUNC;

GET_FUNC(CMD_MAP)
{
    char buff[MAXSTR];

    get_string(fp, buff, sizeof (buff));

    cmd->map.name = strdup(buff);

    get_index(fp, &cmd->map.version.x);
    get_index(fp, &cmd->map.version.y);
}
END_FUNC;

/*---------------------------------------------------------------------------*/

#undef BYTES
#define BYTES ARRAY_BYTES(3) * 2

PUT_FUNC(CMD_TILT_AXES)
{
    put_array(fp, cmd->tiltaxes.x, 3);
    put_array(fp, cmd->tiltaxes.z, 3);
}
END_FUNC;

GET_FUNC(CMD_TILT_AXES)
{
    get_array(fp, cmd->tiltaxes.x, 3);
    get_array(fp, cmd->tiltaxes.z, 3);
}
END_FUNC;

/*---------------------------------------------------------------------------*/

#define PUT_CASE(t) case t: cmd_put_ ## t(fp, cmd); break
#define GET_CASE(t) case t: cmd_get_ ## t(fp, cmd); break

int cmd_put(fs_file fp, const union cmd *cmd)
{
    if (!fp || !cmd)
        return 0;

    assert(cmd->type > CMD_NONE && cmd->type < CMD_MAX);

    fs_putc(cmd->type, fp);

    switch (cmd->type)
    {
        PUT_CASE(CMD_END_OF_UPDATE);
        PUT_CASE(CMD_MAKE_BALL);
        PUT_CASE(CMD_MAKE_ITEM);
        PUT_CASE(CMD_PICK_ITEM);
        PUT_CASE(CMD_TILT_ANGLES);
        PUT_CASE(CMD_SOUND);
        PUT_CASE(CMD_TIMER);
        PUT_CASE(CMD_STATUS);
        PUT_CASE(CMD_COINS);
        PUT_CASE(CMD_JUMP_ENTER);
        PUT_CASE(CMD_JUMP_EXIT);
        PUT_CASE(CMD_BODY_PATH);
        PUT_CASE(CMD_BODY_TIME);
        PUT_CASE(CMD_GOAL_OPEN);
        PUT_CASE(CMD_SWCH_ENTER);
        PUT_CASE(CMD_SWCH_TOGGLE);
        PUT_CASE(CMD_SWCH_EXIT);
        PUT_CASE(CMD_UPDATES_PER_SECOND);
        PUT_CASE(CMD_BALL_RADIUS);
        PUT_CASE(CMD_CLEAR_ITEMS);
        PUT_CASE(CMD_CLEAR_BALLS);
        PUT_CASE(CMD_BALL_POSITION);
        PUT_CASE(CMD_BALL_BASIS);
        PUT_CASE(CMD_BALL_PEND_BASIS);
        PUT_CASE(CMD_VIEW_POSITION);
        PUT_CASE(CMD_VIEW_CENTER);
        PUT_CASE(CMD_VIEW_BASIS);
        PUT_CASE(CMD_CURRENT_BALL);
        PUT_CASE(CMD_PATH_FLAG);
        PUT_CASE(CMD_STEP_SIMULATION);
        PUT_CASE(CMD_MAP);
        PUT_CASE(CMD_TILT_AXES);

    case CMD_NONE:
    case CMD_MAX:
        break;
    }

    return !fs_eof(fp);
}

int cmd_get(fs_file fp, union cmd *cmd)
{
    int type;
    short size;

    if (!fp || !cmd)
        return 0;

    if ((type = fs_getc(fp)) >= 0)
    {
        get_short(fp, &size);

        /* Discard unrecognised commands. */

        if (type >= CMD_MAX)
        {
            fs_seek(fp, size, SEEK_CUR);
            type = CMD_NONE;
        }

        cmd->type = type;

        switch (cmd->type)
        {
            GET_CASE(CMD_END_OF_UPDATE);
            GET_CASE(CMD_MAKE_BALL);
            GET_CASE(CMD_MAKE_ITEM);
            GET_CASE(CMD_PICK_ITEM);
            GET_CASE(CMD_TILT_ANGLES);
            GET_CASE(CMD_SOUND);
            GET_CASE(CMD_TIMER);
            GET_CASE(CMD_STATUS);
            GET_CASE(CMD_COINS);
            GET_CASE(CMD_JUMP_ENTER);
            GET_CASE(CMD_JUMP_EXIT);
            GET_CASE(CMD_BODY_PATH);
            GET_CASE(CMD_BODY_TIME);
            GET_CASE(CMD_GOAL_OPEN);
            GET_CASE(CMD_SWCH_ENTER);
            GET_CASE(CMD_SWCH_TOGGLE);
            GET_CASE(CMD_SWCH_EXIT);
            GET_CASE(CMD_UPDATES_PER_SECOND);
            GET_CASE(CMD_BALL_RADIUS);
            GET_CASE(CMD_CLEAR_ITEMS);
            GET_CASE(CMD_CLEAR_BALLS);
            GET_CASE(CMD_BALL_POSITION);
            GET_CASE(CMD_BALL_BASIS);
            GET_CASE(CMD_BALL_PEND_BASIS);
            GET_CASE(CMD_VIEW_POSITION);
            GET_CASE(CMD_VIEW_CENTER);
            GET_CASE(CMD_VIEW_BASIS);
            GET_CASE(CMD_CURRENT_BALL);
            GET_CASE(CMD_PATH_FLAG);
            GET_CASE(CMD_STEP_SIMULATION);
            GET_CASE(CMD_MAP);
            GET_CASE(CMD_TILT_AXES);

        case CMD_NONE:
        case CMD_MAX:
            break;
        }

        return !fs_eof(fp);
    }
    return 0;
}

/*---------------------------------------------------------------------------*/
