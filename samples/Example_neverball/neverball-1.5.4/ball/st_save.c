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
#include <ctype.h>

#include "gui.h"
#include "util.h"
#include "audio.h"
#include "config.h"
#include "demo.h"
#include "progress.h"
#include "text.h"
#include "common.h"

#include "game_common.h"

#include "st_shared.h"
#include "st_save.h"

extern struct state st_save;
extern struct state st_clobber;

static char filename[MAXSTR];

/*---------------------------------------------------------------------------*/

static struct state *ok_state;
static struct state *cancel_state;

int goto_save(struct state *ok, struct state *cancel)
{
    const char *name;

    name = demo_format_name(config_get_s(CONFIG_REPLAY_NAME),
                            set_id(curr_set()),
                            level_name(curr_level()));

    strncpy(filename, name, sizeof (filename) - 1);

    ok_state     = ok;
    cancel_state = cancel;

    return goto_state(&st_save);
}

/*---------------------------------------------------------------------------*/

static int file_id;

#define SAVE_SAVE   -1
#define SAVE_CANCEL -2

static int save_action(int i)
{
    char *n;

    audio_play(AUD_MENU, 1.0f);

    switch (i)
    {
    case SAVE_SAVE:
        n = filename;

        if (strlen(n) == 0)
            return 1;

        if (demo_exists(n))
            return goto_state(&st_clobber);
        else
        {
            demo_rename(n);
            return goto_state(ok_state);
        }

    case SAVE_CANCEL:
        return goto_state(cancel_state);

    case GUI_CL:
        gui_keyboard_lock();
        break;

    case GUI_BS:
        if (text_del_char(filename))
            gui_set_label(file_id, filename);
        break;

    default:
        if (!path_is_sep(i) && text_add_char(i, filename, sizeof (filename)))
            gui_set_label(file_id, filename);
    }
    return 1;
}

static int enter_id;

static int save_enter(void)
{
    int id, jd;

    if ((id = gui_vstack(0)))
    {
        gui_label(id, _("Replay Name"), GUI_MED, GUI_ALL, 0, 0);
        gui_space(id);

        file_id = gui_label(id, " ", GUI_MED, GUI_ALL, gui_yel, gui_yel);

        gui_space(id);
        gui_keyboard(id);
        gui_space(id);

        if ((jd = gui_harray(id)))
        {
            enter_id = gui_start(jd, _("Save"), GUI_SML, SAVE_SAVE, 0);
            gui_space(jd);
            gui_state(jd, _("Cancel"), GUI_SML, SAVE_CANCEL, 0);
        }

        gui_layout(id, 0, 0);

        gui_set_trunc(file_id, TRUNC_HEAD);
        gui_set_label(file_id, filename);
    }

    SDL_EnableUNICODE(1);

    return id;
}

static void save_leave(int id)
{
    SDL_EnableUNICODE(0);
    gui_delete(id);
}

static int save_keybd(int c, int d)
{
    if (d)
    {
        gui_focus(enter_id);

        if (c == '\b' || c == 0x7F)
            return save_action(GUI_BS);
        if (c >= ' ')
            return save_action(c);
    }
    return 1;
}

static int save_buttn(int b, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
        {
            int c = gui_token(gui_click());

            if (c >= 0 && !GUI_ISMSK(c))
                return save_action(gui_keyboard_char(c));
            else
                return save_action(c);
        }
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return save_action(SAVE_CANCEL);
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

static int clobber_action(int i)
{
    audio_play(AUD_MENU, 1.0f);

    if (i == SAVE_SAVE)
    {
        demo_rename(filename);
        return goto_state(ok_state);
    }
    return goto_state(&st_save);
}

static int clobber_enter(void)
{
    int id, jd, kd;
    int file_id;

    if ((id = gui_vstack(0)))
    {
        kd = gui_label(id, _("Overwrite?"), GUI_MED, GUI_ALL, gui_red, gui_red);

        file_id = gui_label(id, "MMMMMMMM", GUI_MED, GUI_ALL, gui_yel, gui_yel);

        if ((jd = gui_harray(id)))
        {
            gui_start(jd, _("No"),  GUI_SML, SAVE_CANCEL, 1);
            gui_state(jd, _("Yes"), GUI_SML, SAVE_SAVE,   0);
        }

        gui_pulse(kd, 1.2f);
        gui_layout(id, 0, 0);

        gui_set_trunc(file_id, TRUNC_TAIL);
        gui_set_label(file_id, filename);
    }

    return id;
}

static int clobber_buttn(int b, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
            return clobber_action(gui_token(gui_click()));
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return clobber_action(SAVE_CANCEL);
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

struct state st_save = {
    save_enter,
    save_leave,
    shared_paint,
    shared_timer,
    shared_point,
    shared_stick,
    shared_angle,
    shared_click,
    save_keybd,
    save_buttn,
    1, 0
};

struct state st_clobber = {
    clobber_enter,
    shared_leave,
    shared_paint,
    shared_timer,
    shared_point,
    shared_stick,
    shared_angle,
    shared_click,
    NULL,
    clobber_buttn,
    1, 0
};
