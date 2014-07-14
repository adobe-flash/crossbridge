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

#include "gui.h"
#include "set.h"
#include "progress.h"
#include "audio.h"
#include "config.h"
#include "util.h"
#include "common.h"

#include "game_common.h"

#include "st_set.h"
#include "st_title.h"
#include "st_start.h"
#include "st_shared.h"

/*---------------------------------------------------------------------------*/

#define SET_STEP 6

static int total = 0;
static int first = 0;

static int shot_id;
static int desc_id;

static int do_init = 1;

static int set_action(int i)
{
    audio_play(AUD_MENU, 1.0f);

    switch (i)
    {
    case GUI_BACK:
        set_quit();
        return goto_state(&st_title);
        break;

    case GUI_PREV:

        first -= SET_STEP;

        do_init = 0;
        return goto_state(&st_set);

        break;

    case GUI_NEXT:

        first += SET_STEP;

        do_init = 0;
        return goto_state(&st_set);

        break;

    case GUI_NULL:
        return 1;
        break;

    default:
        if (set_exists(i))
        {
            set_goto(i);
            return goto_state(&st_start);
        }
    }

    return 1;
}

static void gui_set(int id, int i)
{
    if (set_exists(i))
        gui_state(id, set_name(i), GUI_SML, i, 0);
    else
        gui_label(id, "", GUI_SML, GUI_ALL, 0, 0);
}

static int set_enter(void)
{
    int w = config_get_d(CONFIG_WIDTH);
    int h = config_get_d(CONFIG_HEIGHT);

    int id, jd, kd;

    int i;

    if (do_init)
    {
        total = set_init();
        first = MIN(first, (total - 1) - ((total - 1) % SET_STEP));

        audio_music_fade_to(0.5f, "bgm/inter.ogg");
        audio_play(AUD_START, 1.f);
    }
    else do_init = 1;

    if ((id = gui_vstack(0)))
    {
        if ((jd = gui_hstack(id)))
        {
            gui_label(jd, _("Level Set"), GUI_SML, GUI_ALL, gui_yel, gui_red);
            gui_filler(jd);
            gui_navig(jd, first > 0, first + SET_STEP < total);
        }

        gui_space(id);

        if ((jd = gui_harray(id)))
        {
            shot_id = gui_image(jd, set_shot(first), 7 * w / 16, 7 * h / 16);

            if ((kd = gui_varray(jd)))
            {
                for (i = first; i < first + SET_STEP; i++)
                    gui_set(kd, i);
            }
        }

        gui_space(id);
        desc_id = gui_multi(id, " \\ \\ \\ \\ \\", GUI_SML, GUI_ALL,
                            gui_yel, gui_wht);

        gui_layout(id, 0, 0);
    }
    return id;
}

static void set_over(int i)
{
    gui_set_image(shot_id, set_shot(i));
    gui_set_multi(desc_id, set_desc(i));
}

static void set_point(int id, int x, int y, int dx, int dy)
{
    int jd = shared_point_basic(id, x, y);
    int i  = gui_token(jd);
    if (jd && set_exists(i))
        set_over(i);
}

static void set_stick(int id, int a, int v)
{
    int jd = shared_stick_basic(id, a, v);
    int i  = gui_token(jd);
    if (jd && set_exists(i))
        set_over(i);
}

static int set_buttn(int b, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
            return set_action(gui_token(gui_click()));
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return set_action(GUI_BACK);
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

struct state st_set = {
    set_enter,
    shared_leave,
    shared_paint,
    shared_timer,
    set_point,
    set_stick,
    shared_angle,
    shared_click,
    NULL,
    set_buttn,
    1, 0
};
