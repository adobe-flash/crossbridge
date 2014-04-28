/*
 * Copyright (C) 2007 Robert Kooima
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
#include "util.h"
#include "progress.h"
#include "audio.h"
#include "config.h"
#include "video.h"
#include "demo.h"

#include "game_common.h"
#include "game_server.h"
#include "game_client.h"

#include "st_fall_out.h"
#include "st_save.h"
#include "st_over.h"
#include "st_start.h"
#include "st_level.h"
#include "st_shared.h"
#include "st_play.h"

/*---------------------------------------------------------------------------*/

#define FALL_OUT_NEXT 1
#define FALL_OUT_SAME 2
#define FALL_OUT_SAVE 3
#define FALL_OUT_BACK 4
#define FALL_OUT_OVER 5

static int resume;

static int fall_out_action(int i)
{
    audio_play(AUD_MENU, 1.0f);

    switch (i)
    {
    case FALL_OUT_BACK:
        /* Fall through. */

    case FALL_OUT_OVER:
        progress_stop();
        return goto_state(&st_over);

    case FALL_OUT_SAVE:
        resume = 1;

        progress_stop();
        return goto_save(&st_fall_out, &st_fall_out);

    case FALL_OUT_NEXT:
        if (progress_next())
            return goto_state(&st_level);
        break;

    case FALL_OUT_SAME:
        if (progress_same())
            return goto_state(&st_level);
        break;
    }

    return 1;
}

static int fall_out_enter(void)
{
    int id, jd, kd;

    /* Reset hack. */
    resume = 0;

    if ((id = gui_vstack(0)))
    {
        kd = gui_label(id, _("Fall-out!"), GUI_LRG, GUI_ALL, gui_gry, gui_red);

        gui_space(id);

        if ((jd = gui_harray(id)))
        {
            if (progress_dead())
                gui_start(jd, _("Exit"), GUI_SML, FALL_OUT_OVER, 0);

            if (progress_next_avail())
                gui_start(jd, _("Next Level"),  GUI_SML, FALL_OUT_NEXT, 0);

            if (progress_same_avail())
                gui_start(jd, _("Retry Level"), GUI_SML, FALL_OUT_SAME, 0);

            if (demo_saved())
                gui_state(jd, _("Save Replay"), GUI_SML, FALL_OUT_SAVE, 0);
        }

        gui_space(id);

        gui_pulse(kd, 1.2f);
        gui_layout(id, 0, 0);
    }

    audio_music_fade_out(2.0f);
    /* audio_play(AUD_FALL, 1.0f); */

    video_clr_grab();

    return id;
}

static void fall_out_timer(int id, float dt)
{
    if (time_state() < 2.f)
    {
        game_server_step(dt);
        game_client_step(demo_file());
    }

    gui_timer(id, dt);
}

static int fall_out_keybd(int c, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_KEY_RESTART, c) && progress_same_avail())
        {
            if (progress_same())
                goto_state(&st_play_ready);
        }
    }
    return 1;
}

static int fall_out_buttn(int b, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
            return fall_out_action(gui_token(gui_click()));
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return fall_out_action(FALL_OUT_BACK);
    }
    return 1;
}

static void fall_out_leave(int id)
{
    /* HACK:  don't run animation if only "visiting" a state. */
    st_fall_out.timer = resume ? shared_timer : fall_out_timer;

    gui_delete(id);
}

/*---------------------------------------------------------------------------*/

struct state st_fall_out = {
    fall_out_enter,
    fall_out_leave,
    shared_paint,
    fall_out_timer,
    shared_point,
    shared_stick,
    NULL,
    shared_click,
    fall_out_keybd,
    fall_out_buttn,
    1, 0
};

