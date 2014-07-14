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
#include "progress.h"
#include "audio.h"
#include "config.h"
#include "st_shared.h"

#include "game_server.h"
#include "game_client.h"

#include "st_level.h"
#include "st_play.h"
#include "st_start.h"
#include "st_over.h"

/*---------------------------------------------------------------------------*/

static int level_enter(void)
{
    int id, jd, kd;

    if ((id = gui_vstack(0)))
    {
        if ((jd = gui_hstack(id)))
        {
            gui_filler(jd);

            if ((kd = gui_vstack(jd)))
            {
                const char *ln = level_name (curr_level());
                int b          = level_bonus(curr_level());

                char setattr[MAXSTR], lvlattr[MAXSTR];

                if (b)
                    sprintf(lvlattr, _("Bonus Level %s"), ln);
                else
                    sprintf(lvlattr, _("Level %s"), ln);

                if (curr_mode() == MODE_CHALLENGE)
                    sprintf(setattr, "%s: %s", set_name(curr_set()),
                            mode_to_str(MODE_CHALLENGE, 1));
                else
                    sprintf(setattr, "%s", set_name(curr_set()));

                gui_label(kd, lvlattr, b ? GUI_MED : GUI_LRG, GUI_TOP,
                          b ? gui_wht : 0, b ? gui_grn : 0);
                gui_label(kd, setattr, GUI_SML,               GUI_BOT,
                          gui_wht,         gui_wht);
            }
            gui_filler(jd);
        }
        gui_space(id);

        gui_multi(id, level_msg(curr_level()),
                  GUI_SML, GUI_ALL,
                  gui_wht, gui_wht);

        gui_layout(id, 0, 0);
    }

    game_set_fly(1.f, NULL);
    game_client_step(NULL);

    return id;
}

static void level_timer(int id, float dt)
{
    game_step_fade(dt);
}

static int level_click(int b, int d)
{
    return (b == SDL_BUTTON_LEFT && d == 1) ? goto_state(&st_play_ready) : 1;
}

static int level_keybd(int c, int d)
{
    if (d && c == SDLK_F12)
        return goto_state(&st_poser);
    return 1;
}

static int level_buttn(int b, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
        {
            return goto_state(&st_play_ready);
        }
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
        {
            progress_stop();
            return goto_state(&st_over);
        }
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

static void poser_paint(int id, float t)
{
    game_draw(1, t);
}

static int poser_buttn(int c, int d)
{
    if (d && config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, c))
        return goto_state(&st_level);

    return 1;
}

/*---------------------------------------------------------------------------*/

struct state st_level = {
    level_enter,
    shared_leave,
    shared_paint,
    level_timer,
    NULL,
    NULL,
    NULL,
    level_click,
    level_keybd,
    level_buttn,
    1, 0
};

struct state st_poser = {
    NULL,
    NULL,
    poser_paint,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    poser_buttn,
    1, 0
};
