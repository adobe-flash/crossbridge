/*
 * Copyright (C) 2003 Robert Kooima
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

#include <string.h>

#include "gui.h"
#include "set.h"
#include "util.h"
#include "demo.h"
#include "progress.h"
#include "audio.h"
#include "config.h"
#include "st_shared.h"

#include "game_common.h"

#include "st_done.h"
#include "st_start.h"
#include "st_name.h"

/*---------------------------------------------------------------------------*/

#define DONE_OK   1

/* Bread crumbs. */

static int new_name;
static int resume;

static int done_action(int i)
{
    audio_play(AUD_MENU, 1.0f);

    switch (i)
    {
    case DONE_OK:
        return goto_state(&st_start);

    case GUI_NAME:
        new_name = 1;
        return goto_name(&st_done, &st_done, 0);

    case GUI_MOST_COINS:
    case GUI_BEST_TIMES:
    case GUI_FAST_UNLOCK:
        gui_score_set(i);
        resume = 1;
        return goto_state(&st_done);
    }
    return 1;
}

static int done_enter(void)
{
    const char *s1 = _("New Set Record");
    const char *s2 = _("Set Complete");

    int id;

    int high = progress_set_high();

    if (new_name)
    {
        progress_rename(1);
        new_name = 0;
    }

    if ((id = gui_vstack(0)))
    {
        int gid;

        if (high)
            gid = gui_label(id, s1, GUI_MED, GUI_ALL, gui_grn, gui_grn);
        else
            gid = gui_label(id, s2, GUI_MED, GUI_ALL, gui_blu, gui_grn);

        gui_space(id);
        gui_score_board(id, GUI_MOST_COINS | GUI_BEST_TIMES, 1, high);
        gui_space(id);

        gui_start(id, _("Select Level"), GUI_SML, DONE_OK, 0);

        if (!resume)
            gui_pulse(gid, 1.2f);

        gui_layout(id, 0, 0);
    }

    set_score_board(set_coin_score(curr_set()), progress_score_rank(),
                    set_time_score(curr_set()), progress_times_rank(),
                    NULL, -1);

    /* Reset hack. */
    resume = 0;

    return id;
}

static int done_keybd(int c, int d)
{
    if (d && config_tst_d(CONFIG_KEY_SCORE_NEXT, c))
        return done_action(gui_score_next(gui_score_get()));

    return 1;
}

static int done_buttn(int b, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
            return done_action(gui_token(gui_click()));
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return done_action(DONE_OK);
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

struct state st_done = {
    done_enter,
    shared_leave,
    shared_paint,
    shared_timer,
    shared_point,
    shared_stick,
    shared_angle,
    shared_click,
    done_keybd,
    done_buttn,
    1, 0
};
