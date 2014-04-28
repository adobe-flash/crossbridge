/*
 * Copyright (C) 2003 Robert Kooima
 *
 * NEVERPUTT is  free software; you can redistribute  it and/or modify
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
#include "hud.h"
#include "back.h"
#include "geom.h"
#include "ball.h"
#include "part.h"
#include "game.h"
#include "audio.h"
#include "config.h"
#include "video.h"

#include "st_conf.h"
#include "st_all.h"
#include "st_resol.h"

/*---------------------------------------------------------------------------*/

enum {
    CONF_FULL = 1,
    CONF_WIN,
    CONF_TEXHI,
    CONF_TEXLO,
    CONF_GEOHI,
    CONF_GEOLO,
    CONF_SHDON,
    CONF_SHDOF,
    CONF_BACK,
    CONF_RESOL
};

static int music_id[11];
static int sound_id[11];

static int conf_action(int i)
{
    int w = config_get_d(CONFIG_WIDTH);
    int h = config_get_d(CONFIG_HEIGHT);
    int s = config_get_d(CONFIG_SOUND_VOLUME);
    int m = config_get_d(CONFIG_MUSIC_VOLUME);
    int r = 1;

    audio_play(AUD_MENU, 1.0f);

    switch (i)
    {
    case CONF_FULL:
        goto_state(&st_null);
        r = video_mode(1, w, h);
        goto_state(&st_conf);
        break;

    case CONF_WIN:
        goto_state(&st_null);
        r = video_mode(0, w, h);
        goto_state(&st_conf);
        break;

    case CONF_TEXHI:
        goto_state(&st_null);
        config_set_d(CONFIG_TEXTURES, 1);
        goto_state(&st_conf);
        break;

    case CONF_TEXLO:
        goto_state(&st_null);
        config_set_d(CONFIG_TEXTURES, 2);
        goto_state(&st_conf);
        break;

    case CONF_GEOHI:
        goto_state(&st_null);
        config_set_d(CONFIG_GEOMETRY, 1);
        goto_state(&st_conf);
        break;

    case CONF_GEOLO:
        goto_state(&st_null);
        config_set_d(CONFIG_GEOMETRY, 0);
        goto_state(&st_conf);
        break;

    case CONF_SHDON:
        goto_state(&st_null);
        config_set_d(CONFIG_SHADOW, 1);
        goto_state(&st_conf);
        break;

    case CONF_SHDOF:
        goto_state(&st_null);
        config_set_d(CONFIG_SHADOW, 0);
        goto_state(&st_conf);
        break;

    case CONF_BACK:
        goto_state(&st_title);
        break;

    case CONF_RESOL:
        goto_state(&st_resol);
        break;

    default:
        if (100 <= i && i <= 110)
        {
            int n = i - 100;

            config_set_d(CONFIG_SOUND_VOLUME, n);
            audio_volume(n, m);
            audio_play(AUD_BUMP, 1.f);

            gui_toggle(sound_id[n]);
            gui_toggle(sound_id[s]);
        }
        if (200 <= i && i <= 210)
        {
            int n = i - 200;

            config_set_d(CONFIG_MUSIC_VOLUME, n);
            audio_volume(s, n);
            audio_play(AUD_BUMP, 1.f);

            gui_toggle(music_id[n]);
            gui_toggle(music_id[m]);
        }
    }

    return r;
}

static int conf_enter(void)
{
    int id, jd, kd;

    back_init("back/gui.png", config_get_d(CONFIG_GEOMETRY));

    /* Initialize the configuration GUI. */

    if ((id = gui_vstack(0)))
    {
        int f = config_get_d(CONFIG_FULLSCREEN);
        int t = config_get_d(CONFIG_TEXTURES);
        int g = config_get_d(CONFIG_GEOMETRY);
        int h = config_get_d(CONFIG_SHADOW);
        int s = config_get_d(CONFIG_SOUND_VOLUME);
        int m = config_get_d(CONFIG_MUSIC_VOLUME);

        char resolution[20];

        sprintf(resolution, "%d x %d",
                config_get_d(CONFIG_WIDTH),
                config_get_d(CONFIG_HEIGHT));

        if ((jd = gui_harray(id)))
        {
            gui_label(jd, _("Options"), GUI_SML, GUI_ALL, 0, 0);
            gui_space(jd);
            gui_start(jd, _("Back"),    GUI_SML, CONF_BACK, 0);
        }

        gui_space(id);

        if ((jd = gui_harray(id)) &&
            (kd = gui_harray(jd)))
        {
            gui_state(kd, _("No"),  GUI_SML, CONF_WIN,  (f == 0));
            gui_state(kd, _("Yes"), GUI_SML, CONF_FULL, (f == 1));

            gui_label(jd, _("Fullscreen"), GUI_SML, GUI_ALL, 0, 0);
        }

        if ((jd = gui_harray(id)) &&
            (kd = gui_harray(jd)))
        {
            gui_state(kd, resolution, GUI_SML, CONF_RESOL, 0);

            gui_label(jd, _("Resolution"), GUI_SML, GUI_ALL, 0, 0);
        }

        gui_space(id);

        if ((jd = gui_harray(id)) &&
            (kd = gui_harray(jd)))
        {
            gui_state(kd, _("Low"),  GUI_SML, CONF_TEXLO, (t == 2));
            gui_state(kd, _("High"), GUI_SML, CONF_TEXHI, (t == 1));

            gui_label(jd, _("Textures"), GUI_SML, GUI_ALL, 0, 0);
        }

        if ((jd = gui_harray(id)) &&
            (kd = gui_harray(jd)))
        {
            gui_state(kd, _("Low"),  GUI_SML, CONF_GEOLO, (g == 0));
            gui_state(kd, _("High"), GUI_SML, CONF_GEOHI, (g == 1));

            gui_label(jd, _("Geometry"), GUI_SML, GUI_ALL, 0, 0);
        }

        if ((jd = gui_harray(id)) &&
            (kd = gui_harray(jd)))
        {
            gui_state(kd, _("Off"),  GUI_SML, CONF_SHDOF, (h == 0));
            gui_state(kd, _("On"),   GUI_SML, CONF_SHDON, (h == 1));

            gui_label(jd, _("Shadow"), GUI_SML, GUI_ALL, 0, 0);
        }

        gui_space(id);

        if ((jd = gui_harray(id)) &&
            (kd = gui_harray(jd)))
        {
            /* A series of empty buttons forms the sound volume control. */

            sound_id[10] = gui_state(kd, NULL, GUI_SML, 110, (s == 10));
            sound_id[ 9] = gui_state(kd, NULL, GUI_SML, 109, (s ==  9));
            sound_id[ 8] = gui_state(kd, NULL, GUI_SML, 108, (s ==  8));
            sound_id[ 7] = gui_state(kd, NULL, GUI_SML, 107, (s ==  7));
            sound_id[ 6] = gui_state(kd, NULL, GUI_SML, 106, (s ==  6));
            sound_id[ 5] = gui_state(kd, NULL, GUI_SML, 105, (s ==  5));
            sound_id[ 4] = gui_state(kd, NULL, GUI_SML, 104, (s ==  4));
            sound_id[ 3] = gui_state(kd, NULL, GUI_SML, 103, (s ==  3));
            sound_id[ 2] = gui_state(kd, NULL, GUI_SML, 102, (s ==  2));
            sound_id[ 1] = gui_state(kd, NULL, GUI_SML, 101, (s ==  1));
            sound_id[ 0] = gui_state(kd, NULL, GUI_SML, 100, (s ==  0));

            gui_label(jd, _("Sound Volume"), GUI_SML, GUI_ALL, 0, 0);
        }

        if ((jd = gui_harray(id)) &&
            (kd = gui_harray(jd)))
        {
            /* A series of empty buttons forms the music volume control. */

            music_id[10] = gui_state(kd, NULL, GUI_SML, 210, (m == 10));
            music_id[ 9] = gui_state(kd, NULL, GUI_SML, 209, (m ==  9));
            music_id[ 8] = gui_state(kd, NULL, GUI_SML, 208, (m ==  8));
            music_id[ 7] = gui_state(kd, NULL, GUI_SML, 207, (m ==  7));
            music_id[ 6] = gui_state(kd, NULL, GUI_SML, 206, (m ==  6));
            music_id[ 5] = gui_state(kd, NULL, GUI_SML, 205, (m ==  5));
            music_id[ 4] = gui_state(kd, NULL, GUI_SML, 204, (m ==  4));
            music_id[ 3] = gui_state(kd, NULL, GUI_SML, 203, (m ==  3));
            music_id[ 2] = gui_state(kd, NULL, GUI_SML, 202, (m ==  2));
            music_id[ 1] = gui_state(kd, NULL, GUI_SML, 201, (m ==  1));
            music_id[ 0] = gui_state(kd, NULL, GUI_SML, 200, (m ==  0));

            gui_label(jd, _("Music Volume"), GUI_SML, GUI_ALL, 0, 0);
        }

        gui_layout(id, 0, 0);
    }

    audio_music_fade_to(0.5f, "bgm/inter.ogg");

    return id;
}

static void conf_leave(int id)
{
    gui_delete(id);
}

static void conf_paint(int id, float st)
{
    video_push_persp((float) config_get_d(CONFIG_VIEW_FOV), 0.1f, FAR_DIST);
    {
        back_draw(0);
    }
    video_pop_matrix();
    gui_paint(id);
}

static void conf_timer(int id, float dt)
{
    gui_timer(id, dt);
}

static void conf_point(int id, int x, int y, int dx, int dy)
{
    gui_pulse(gui_point(id, x, y), 1.2f);
}

static void conf_stick(int id, int a, int v)
{
    if (config_tst_d(CONFIG_JOYSTICK_AXIS_X, a))
        gui_pulse(gui_stick(id, v, 0), 1.2f);
    if (config_tst_d(CONFIG_JOYSTICK_AXIS_Y, a))
        gui_pulse(gui_stick(id, 0, v), 1.2f);
}

static int conf_click(int b, int d)
{
    if (b == SDL_BUTTON_LEFT && d == 1)
        return conf_action(gui_token(gui_click()));
    return 1;
}

static int conf_keybd(int c, int d)
{
    return (d && c == SDLK_ESCAPE) ? goto_state(&st_title) : 1;
}

static int conf_buttn(int b, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
            return conf_action(gui_token(gui_click()));
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_B, b))
            return goto_state(&st_title);
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return goto_state(&st_title);
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

static int null_enter(void)
{
    gui_free();
    swch_free();
    jump_free();
    flag_free();
    mark_free();
    ball_free();
    shad_free();

    return 0;
}

static void null_leave(int id)
{
    int g = config_get_d(CONFIG_GEOMETRY);

    shad_init();
    ball_init();
    mark_init(g);
    flag_init(g);
    jump_init(g);
    swch_init(g);
    gui_init();
}

/*---------------------------------------------------------------------------*/

struct state st_conf = {
    conf_enter,
    conf_leave,
    conf_paint,
    conf_timer,
    conf_point,
    conf_stick,
    NULL,
    conf_click,
    conf_keybd,
    conf_buttn,
    1, 0
};

struct state st_null = {
    null_enter,
    null_leave,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    1, 0
};
