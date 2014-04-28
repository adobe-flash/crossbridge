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
#include "util.h"
#include "progress.h"
#include "audio.h"
#include "config.h"
#include "st_shared.h"
#include "common.h"

#include "game_common.h"

#include "st_set.h"
#include "st_over.h"
#include "st_level.h"
#include "st_start.h"
#include "st_title.h"

/*---------------------------------------------------------------------------*/

#define START_BACK        -1
#define START_CHALLENGE   -2
#define START_OPEN_GOALS  -3
#define START_LOCK_GOALS  -4

static int shot_id;
static int file_id;
static int challenge_id;

/*---------------------------------------------------------------------------*/

/* Create a level selector button based upon its existence and status. */

static void gui_level(int id, int i)
{
    const GLfloat *fore = 0, *back = 0;

    int jd;

    if (!level_exists(i))
    {
        gui_label(id, " ", GUI_SML, GUI_ALL, gui_blk, gui_blk);
        return;
    }

    if (level_opened(i))
    {
        fore = level_bonus(i)     ? gui_grn : gui_wht;
        back = level_completed(i) ? fore    : gui_yel;
    }

    jd = gui_label(id, level_name(i), GUI_SML, GUI_ALL, back, fore);

    if (level_opened(i) || config_cheat())
        gui_active(jd, i, 0);
}

static void start_over_level(int i)
{
    if (level_opened(i) || config_cheat())
    {
        gui_set_image(shot_id, level_shot(i));

        set_score_board(&get_level(i)->score.most_coins,  -1,
                        &get_level(i)->score.best_times,  -1,
                        &get_level(i)->score.fast_unlock, -1);

        if (file_id)
            gui_set_label(file_id, level_file(i));
    }
}

static void start_over(int id, int pulse)
{
    int i;

    if (id == 0)
        return;

    if (pulse)
        gui_pulse(id, 1.2f);

    i = gui_token(id);

    if (i == START_CHALLENGE || i == START_BACK)
    {
        gui_set_image(shot_id, set_shot(curr_set()));

        set_score_board(set_coin_score(curr_set()), -1,
                        set_time_score(curr_set()), -1,
                        NULL, -1);
    }

    if (i >= 0 && !GUI_ISMSK(i))
        start_over_level(i);
}

/*---------------------------------------------------------------------------*/

static int start_action(int i)
{
    audio_play(AUD_MENU, 1.0f);

    switch (i)
    {
    case START_BACK:
        return goto_state(&st_set);

    case START_CHALLENGE:
        if (config_cheat())
        {
            progress_init(curr_mode() == MODE_CHALLENGE ?
                          MODE_NORMAL : MODE_CHALLENGE);
            gui_toggle(challenge_id);
            return 1;
        }
        else
        {
            progress_init(MODE_CHALLENGE);
            return start_action(0);
        }
        break;

    case GUI_MOST_COINS:
    case GUI_BEST_TIMES:
    case GUI_FAST_UNLOCK:
        gui_score_set(i);
        return goto_state(&st_start);

    case START_OPEN_GOALS:
        config_set_d(CONFIG_LOCK_GOALS, 0);
        return goto_state(&st_start);

    case START_LOCK_GOALS:
        config_set_d(CONFIG_LOCK_GOALS, 1);
        return goto_state(&st_start);

    default:
        if (progress_play(i))
            return goto_state(&st_level);
        break;
    }

    return 1;
}

static int start_enter(void)
{
    int w = config_get_d(CONFIG_WIDTH);
    int h = config_get_d(CONFIG_HEIGHT);
    int i, j;

    int id, jd, kd, ld;

    progress_init(MODE_NORMAL);

    if ((id = gui_vstack(0)))
    {
        if ((jd = gui_hstack(id)))
        {

            gui_label(jd, set_name(curr_set()), GUI_SML, GUI_ALL,
                      gui_yel, gui_red);
            gui_filler(jd);
            gui_start(jd, _("Back"),  GUI_SML, START_BACK, 0);
        }

        gui_space(id);

        if ((jd = gui_harray(id)))
        {
            if (config_cheat())
            {
                if ((kd = gui_vstack(jd)))
                {
                    shot_id = gui_image(kd, set_shot(curr_set()),
                                        6 * w / 16, 6 * h / 16);
                    file_id = gui_label(kd, " ", GUI_SML, GUI_ALL,
                                        gui_yel, gui_red);
                }
            }
            else
            {
                shot_id = gui_image(jd, set_shot(curr_set()),
                                    7 * w / 16, 7 * h / 16);
            }

            if ((kd = gui_varray(jd)))
            {
                for (i = 0; i < 5; i++)
                    if ((ld = gui_harray(kd)))
                        for (j = 4; j >= 0; j--)
                            gui_level(ld, i * 5 + j);

                challenge_id = gui_state(kd, _("Challenge"),
                                         GUI_SML, START_CHALLENGE,
                                         curr_mode() == MODE_CHALLENGE);
            }
        }
        gui_space(id);
        gui_score_board(id, (GUI_MOST_COINS |
                             GUI_BEST_TIMES |
                             GUI_FAST_UNLOCK), 0, 0);
        gui_space(id);

        if ((jd = gui_hstack(id)))
        {
            gui_filler(jd);

            if ((kd = gui_harray(jd)))
            {
                /* TODO, replace the whitespace hack with something sane. */

                gui_state(kd,
                          /* Translators: adjust the amount of whitespace here
                           * as necessary for the buttons to look good. */
                          _("   No   "), GUI_SML, START_OPEN_GOALS,
                          config_get_d(CONFIG_LOCK_GOALS) == 0);

                gui_state(kd, _("Yes"), GUI_SML, START_LOCK_GOALS,
                          config_get_d(CONFIG_LOCK_GOALS) == 1);
            }

            gui_space(jd);

            gui_label(jd, _("Lock Goals of Completed Levels?"),
                      GUI_SML, GUI_ALL, 0, 0);

            gui_filler(jd);
        }

        gui_layout(id, 0, 0);

        set_score_board(NULL, -1, NULL, -1, NULL, -1);
    }

    audio_music_fade_to(0.5f, "bgm/inter.ogg");

    return id;
}

static void start_point(int id, int x, int y, int dx, int dy)
{
    start_over(gui_point(id, x, y), 1);
}

static void start_stick(int id, int a, int v)
{
    int x = (config_tst_d(CONFIG_JOYSTICK_AXIS_X, a)) ? v : 0;
    int y = (config_tst_d(CONFIG_JOYSTICK_AXIS_Y, a)) ? v : 0;

    start_over(gui_stick(id, x, y), 1);
}

static int start_keybd(int c, int d)
{
    if (d)
    {
        if (c == SDLK_c && config_cheat())
        {
            set_cheat();
            return goto_state(&st_start);
        }
        else if (c == SDLK_F12 && config_cheat())
        {
            char *dir = concat_string("Screenshots/shot-",
                                      set_id(curr_set()), NULL);
            int i;

            fs_mkdir(dir);

            /* Iterate over all levels, taking a screenshot of each. */

            for (i = 0; i < MAXLVL; i++)
                if (level_exists(i))
                    level_snap(i, dir);

            free(dir);
        }
        else if (config_tst_d(CONFIG_KEY_SCORE_NEXT, c))
        {
            int active = gui_click();

            if (start_action(gui_score_next(gui_score_get())))
            {
                /* HACK ALERT
                 *
                 * This assumes that 'active' is a valid widget ID even after
                 * the above start_action has recreated the entire widget
                 * hierarchy.  Maybe it is.  Maybe it isn't.
                 */
                gui_focus(active);
                start_over(active, 0);

                return 1;
            }
            else
                return 0;
        }
    }

    return 1;
}

static int start_buttn(int b, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
            return start_action(gui_token(gui_click()));
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return start_action(START_BACK);
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

struct state st_start = {
    start_enter,
    shared_leave,
    shared_paint,
    shared_timer,
    start_point,
    start_stick,
    shared_angle,
    shared_click,
    start_keybd,
    start_buttn,
    1, 0
};
