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

#include <SDL.h>
#include <math.h>
#include <string.h>

#include "glext.h"
#include "hud.h"
#include "gui.h"
#include "progress.h"
#include "config.h"
#include "video.h"
#include "audio.h"

#include "game_common.h"
#include "game_client.h"

/*---------------------------------------------------------------------------*/

static int Lhud_id;
static int Rhud_id;
static int time_id;

static int coin_id;
static int ball_id;
static int scor_id;
static int goal_id;
static int view_id;
static int fps_id;

static float view_timer;

static void hud_fps(void)
{
    gui_set_count(fps_id, video_perf());
}

void hud_init(void)
{
    int id;
    const char *str_view;
    int v;

    if ((Rhud_id = gui_hstack(0)))
    {
        if ((id = gui_vstack(Rhud_id)))
        {
            gui_label(id, _("Coins"), GUI_SML, 0, gui_wht, gui_wht);
            gui_label(id, _("Goal"),  GUI_SML, 0, gui_wht, gui_wht);
        }
        if ((id = gui_vstack(Rhud_id)))
        {
            coin_id = gui_count(id, 100, GUI_SML, GUI_NW);
            goal_id = gui_count(id, 10,  GUI_SML, 0);
        }
        gui_layout(Rhud_id, +1, -1);
    }

    if ((Lhud_id = gui_hstack(0)))
    {
        if ((id = gui_vstack(Lhud_id)))
        {
            ball_id = gui_count(id, 10,   GUI_SML, GUI_NE);
            scor_id = gui_count(id, 1000, GUI_SML, 0);
        }
        if ((id = gui_vstack(Lhud_id)))
        {
            gui_label(id, _("Balls"), GUI_SML, 0, gui_wht, gui_wht);
            gui_label(id, _("Score"), GUI_SML, 0, gui_wht, gui_wht);
        }
        gui_layout(Lhud_id, -1, -1);
    }

    if ((time_id = gui_clock(0, 59999, GUI_MED, GUI_TOP)))
        gui_layout(time_id, 0, -1);


    /* Find the longest view name. */

    for (str_view = "", v = VIEW_NONE + 1; v < VIEW_MAX; v++)
        if (strlen(view_to_str(v)) > strlen(str_view))
            str_view = view_to_str(v);

    if ((view_id = gui_label(0, str_view, GUI_SML, GUI_SW, gui_wht, gui_wht)))
        gui_layout(view_id, 1, 1);

    if ((fps_id = gui_count(0, 1000, GUI_SML, GUI_SE)))
        gui_layout(fps_id, -1, 1);
}

void hud_free(void)
{
    gui_delete(Rhud_id);
    gui_delete(Lhud_id);
    gui_delete(time_id);
    gui_delete(view_id);
    gui_delete(fps_id);
}

void hud_paint(void)
{
    if (curr_mode() == MODE_CHALLENGE)
        gui_paint(Lhud_id);

    gui_paint(Rhud_id);
    gui_paint(time_id);

    if (config_get_d(CONFIG_FPS))
        gui_paint(fps_id);

    hud_view_paint();
}

void hud_update(int pulse)
{
    int clock = curr_clock();
    int coins = curr_coins();
    int goal  = curr_goal();
    int balls = curr_balls();
    int score = curr_score();

    int c_id;
    int last;

    if (!pulse)
    {
        /* reset the hud */

        gui_pulse(ball_id, 0.f);
        gui_pulse(time_id, 0.f);
        gui_pulse(coin_id, 0.f);
    }

    /* time and tick-tock */

    if (clock != (last = gui_value(time_id)))
    {
        gui_set_clock(time_id, clock);

        if (last > clock && pulse)
        {
            if (clock <= 1000 && (last / 100) > (clock / 100))
            {
                audio_play(AUD_TICK, 1.f);
                gui_pulse(time_id, 1.50);
            }
            else if (clock < 500 && (last / 50) > (clock / 50))
            {
                audio_play(AUD_TOCK, 1.f);
                gui_pulse(time_id, 1.25);
            }
        }
    }

    /* balls and score + select coin widget */

    switch (curr_mode())
    {
    case MODE_CHALLENGE:
        if (gui_value(ball_id) != balls) gui_set_count(ball_id, balls);
        if (gui_value(scor_id) != score) gui_set_count(scor_id, score);

        c_id = coin_id;
        break;

    default:
        c_id = coin_id;
        break;
    }


    /* coins and pulse */

    if (coins != (last = gui_value(c_id)))
    {
        last = coins - last;

        gui_set_count(c_id, coins);

        if (pulse && last > 0)
        {
            if      (last >= 10) gui_pulse(coin_id, 2.00f);
            else if (last >=  5) gui_pulse(coin_id, 1.50f);
            else                 gui_pulse(coin_id, 1.25f);

            if (goal > 0)
            {
                if      (last >= 10) gui_pulse(goal_id, 2.00f);
                else if (last >=  5) gui_pulse(goal_id, 1.50f);
                else                 gui_pulse(goal_id, 1.25f);
            }
        }
    }

    /* goal and pulse */

    if (goal != (last = gui_value(goal_id)))
    {
        gui_set_count(goal_id, goal);

        if (pulse && goal == 0 && last > 0)
            gui_pulse(goal_id, 2.00f);
    }

    if (config_get_d(CONFIG_FPS))
        hud_fps();
}

void hud_timer(float dt)
{

    hud_update(1);

    gui_timer(Rhud_id, dt);
    gui_timer(Lhud_id, dt);
    gui_timer(time_id, dt);

    hud_view_timer(dt);
}

/*---------------------------------------------------------------------------*/

void hud_view_pulse(int c)
{
    gui_set_label(view_id, view_to_str(c));
    gui_pulse(view_id, 1.2f);
    view_timer = 2.0f;
}

void hud_view_timer(float dt)
{
    view_timer -= dt;
    gui_timer(view_id, dt);
}

void hud_view_paint(void)
{
    if (view_timer > 0.0f)
        gui_paint(view_id);
}

/*---------------------------------------------------------------------------*/
