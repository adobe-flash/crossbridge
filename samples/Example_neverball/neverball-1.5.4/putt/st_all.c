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

#include <math.h>

#include "hud.h"
#include "back.h"
#include "geom.h"
#include "gui.h"
#include "vec3.h"
#include "game.h"
#include "hole.h"
#include "audio.h"
#include "course.h"
#include "config.h"
#include "video.h"

#include "st_all.h"
#include "st_conf.h"

/*---------------------------------------------------------------------------*/

static SDL_Joystick *joystick = NULL;

void set_joystick(SDL_Joystick *j)
{
    joystick = j;
}

/*---------------------------------------------------------------------------*/

static char *number(int i)
{
    static char str[MAXSTR];

    sprintf(str, "%02d", i);

    return str;
}

static int score_card(const char  *title,
                      const float *c0,
                      const float *c1)
{
    int id, jd, kd, ld;

    int p1 = (curr_party() >= 1) ? 1 : 0, l1 = (curr_party() == 1) ? 1 : 0;
    int p2 = (curr_party() >= 2) ? 1 : 0, l2 = (curr_party() == 2) ? 1 : 0;
    int p3 = (curr_party() >= 3) ? 1 : 0, l3 = (curr_party() == 3) ? 1 : 0;
    int p4 = (curr_party() >= 4) ? 1 : 0, l4 = (curr_party() == 4) ? 1 : 0;

    int i;
    int n = curr_count() - 1;
    int m = curr_count() / 2;

    if ((id = gui_vstack(0)))
    {
        gui_label(id, title, GUI_MED, GUI_ALL, c0, c1);
        gui_space(id);

        if ((jd = gui_hstack(id)))
        {
            if ((kd = gui_varray(jd)))
            {
                if (p1) gui_label(kd, _("O"),         0, GUI_NE, 0, 0);
                if (p1) gui_label(kd, hole_out(0), 0, 0,           gui_wht, gui_wht);
                if (p1) gui_label(kd, hole_out(1), 0, GUI_SE * l1, gui_red, gui_wht);
                if (p2) gui_label(kd, hole_out(2), 0, GUI_SE * l2, gui_grn, gui_wht);
                if (p3) gui_label(kd, hole_out(3), 0, GUI_SE * l3, gui_blu, gui_wht);
                if (p4) gui_label(kd, hole_out(4), 0, GUI_SE * l4, gui_yel, gui_wht);
            }

            if ((kd = gui_harray(jd)))
                for (i = m; i > 0; i--)
                    if ((ld = gui_varray(kd)))
                    {
                        if (p1) gui_label(ld, number(i), 0, (i == 1) ? GUI_NW : 0, 0, 0);
                        if (p1) gui_label(ld, hole_score(i, 0), 0, 0, gui_wht, gui_wht);
                        if (p1) gui_label(ld, hole_score(i, 1), 0, 0, gui_red, gui_wht);
                        if (p2) gui_label(ld, hole_score(i, 2), 0, 0, gui_grn, gui_wht);
                        if (p3) gui_label(ld, hole_score(i, 3), 0, 0, gui_blu, gui_wht);
                        if (p4) gui_label(ld, hole_score(i, 4), 0, 0, gui_yel, gui_wht);
                    }
            if ((kd = gui_varray(jd)))
            {
                gui_filler(kd);
                if (p1) gui_label(kd, _("Par"), 0, GUI_NW,      gui_wht, gui_wht);
                if (p1) gui_label(kd, _("P1"),  0, GUI_SW * l1, gui_red, gui_wht);
                if (p2) gui_label(kd, _("P2"),  0, GUI_SW * l2, gui_grn, gui_wht);
                if (p3) gui_label(kd, _("P3"),  0, GUI_SW * l3, gui_blu, gui_wht);
                if (p4) gui_label(kd, _("P4"),  0, GUI_SW * l4, gui_yel, gui_wht);
            }
        }

        gui_space(id);

        if ((jd = gui_hstack(id)))
        {
            if ((kd = gui_varray(jd)))
            {
                if (p1) gui_label(kd, _("Tot"),    0, GUI_TOP, 0, 0);
                if (p1) gui_label(kd, hole_tot(0), 0, 0,           gui_wht, gui_wht);
                if (p1) gui_label(kd, hole_tot(1), 0, GUI_BOT * l1, gui_red, gui_wht);
                if (p2) gui_label(kd, hole_tot(2), 0, GUI_BOT * l2, gui_grn, gui_wht);
                if (p3) gui_label(kd, hole_tot(3), 0, GUI_BOT * l3, gui_blu, gui_wht);
                if (p4) gui_label(kd, hole_tot(4), 0, GUI_BOT * l4, gui_yel, gui_wht);
            }
            if ((kd = gui_varray(jd)))
            {
                if (p1) gui_label(kd, _("I"),     0, GUI_NE, 0, 0);
                if (p1) gui_label(kd, hole_in(0), 0, 0,           gui_wht, gui_wht);
                if (p1) gui_label(kd, hole_in(1), 0, GUI_SE * l1, gui_red, gui_wht);
                if (p2) gui_label(kd, hole_in(2), 0, GUI_SE * l2, gui_grn, gui_wht);
                if (p3) gui_label(kd, hole_in(3), 0, GUI_SE * l3, gui_blu, gui_wht);
                if (p4) gui_label(kd, hole_in(4), 0, GUI_SE * l4, gui_yel, gui_wht);
            }
            if ((kd = gui_harray(jd)))
                for (i = n; i > m; i--)
                    if ((ld = gui_varray(kd)))
                    {
                        if (p1) gui_label(ld, number(i), 0, (i == m+1) ? GUI_NW : 0, 0, 0);
                        if (p1) gui_label(ld, hole_score(i, 0), 0, 0, gui_wht, gui_wht);
                        if (p1) gui_label(ld, hole_score(i, 1), 0, 0, gui_red, gui_wht);
                        if (p2) gui_label(ld, hole_score(i, 2), 0, 0, gui_grn, gui_wht);
                        if (p3) gui_label(ld, hole_score(i, 3), 0, 0, gui_blu, gui_wht);
                        if (p4) gui_label(ld, hole_score(i, 4), 0, 0, gui_yel, gui_wht);
                    }
            if ((kd = gui_varray(jd)))
            {
                gui_filler(kd);
                if (p1) gui_label(kd, _("Par"), 0, GUI_NW,      gui_wht, gui_wht);
                if (p1) gui_label(kd, _("P1"),  0, GUI_SW * l1, gui_red, gui_wht);
                if (p2) gui_label(kd, _("P2"),  0, GUI_SW * l2, gui_grn, gui_wht);
                if (p3) gui_label(kd, _("P3"),  0, GUI_SW * l3, gui_blu, gui_wht);
                if (p4) gui_label(kd, _("P4"),  0, GUI_SW * l4, gui_yel, gui_wht);
            }
        }

        gui_layout(id, 0, 0);
    }

    return id;
}

/*---------------------------------------------------------------------------*/

static int shared_stick_basic(int id, int a, int v)
{
    int jd = 0;

    if (config_tst_d(CONFIG_JOYSTICK_AXIS_X, a))
        jd = gui_stick(id, v, 0);
    else if (config_tst_d(CONFIG_JOYSTICK_AXIS_Y, a))
        jd = gui_stick(id, 0, v);

    if (jd)
        gui_pulse(jd, 1.2f);

    return jd;
}

static void shared_stick(int id, int a, int v)
{
    shared_stick_basic(id, a, v);
}

/*---------------------------------------------------------------------------*/

#define TITLE_PLAY 1
#define TITLE_CONF 2
#define TITLE_EXIT 3

static int title_action(int i)
{
    audio_play(AUD_MENU, 1.0f);

    switch (i)
    {
    case TITLE_PLAY: return goto_state(&st_course);
    case TITLE_CONF: return goto_state(&st_conf);
    case TITLE_EXIT: return 0;
    }
    return 1;
}

static int title_enter(void)
{
    int id, jd, kd;

    /* Build the title GUI. */

    if ((id = gui_vstack(0)))
    {
        gui_label(id, "Neverputt", GUI_LRG, GUI_ALL, 0, 0);
        gui_space(id);

        if ((jd = gui_harray(id)))
        {
            gui_filler(jd);

            if ((kd = gui_varray(jd)))
            {
                gui_start(kd, sgettext("menu^Play"),    GUI_MED, TITLE_PLAY, 1);
                gui_state(kd, sgettext("menu^Options"), GUI_MED, TITLE_CONF, 0);
                gui_state(kd, sgettext("menu^Exit"),    GUI_MED, TITLE_EXIT, 0);
            }

            gui_filler(jd);
        }
        gui_layout(id, 0, 0);
    }

    course_init();
    course_rand();

    return id;
}

static void title_leave(int id)
{
    gui_delete(id);
}

static void title_paint(int id, float t)
{
    game_draw(0, t);
    gui_paint(id);
}

static void title_timer(int id, float dt)
{
    float g[3] = { 0.f, 0.f, 0.f };

    game_step(g, dt);
    game_set_fly(fcosf(time_state() / 10.f));

    gui_timer(id, dt);
}

static void title_point(int id, int x, int y, int dx, int dy)
{
    gui_pulse(gui_point(id, x, y), 1.2f);
}

static int title_click(int b, int d)
{
    return d && b == SDL_BUTTON_LEFT ? title_action(gui_token(gui_click())) : 1;
}

static int title_buttn(int b, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
            return title_action(gui_token(gui_click()));
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return title_action(TITLE_EXIT);
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

static int desc_id;
static int shot_id;

#define COURSE_BACK -1

static int course_action(int i)
{
    if (course_exists(i))
    {
        course_goto(i);
        goto_state(&st_party);
    }
    if (i == COURSE_BACK)
        goto_state(&st_title);

    return 1;
}

static int course_enter(void)
{
    int w = config_get_d(CONFIG_WIDTH);
    int h = config_get_d(CONFIG_HEIGHT);

    int id, jd, kd, ld, md, i = 0, j, n = course_count();
    int m = (int)(sqrt(n/2.0)*2);

    if ((id = gui_vstack(0)))
    {
        gui_label(id, _("Select Course"), GUI_MED, GUI_ALL, 0, 0);
        gui_space(id);

        if ((jd = gui_hstack(id)))
        {
            shot_id = gui_image(jd, course_shot(0), w / 3, h / 3);

            gui_filler(jd);

            if ((kd = gui_varray(jd)))
            {
                for(i = 0; i < n; i += m)
                {
                    if ((ld = gui_harray(kd)))
                    {
                        for (j = (m - 1); j >= 0; j--)
                        {
                            if (i + j < n)
                            {
                                md = gui_image(ld, course_shot(i + j),
                                               w / 3 / m, h / 3 / m);
                                gui_active(md, i + j, 0);

                                if (i + j == 0)
                                    gui_focus(md);
                            }
                            else
                                gui_space(ld);
                        }
                    }
                }
            }
        }

        gui_space(id);
        desc_id = gui_multi(id, _(course_desc(0)), GUI_SML, GUI_ALL, gui_yel, gui_wht);
        gui_space(id);

        if ((jd = gui_hstack(id)))
        {
            gui_filler(jd);
            gui_state(jd, _("Back"), GUI_SML, COURSE_BACK, 0);
        }

        gui_layout(id, 0, 0);
    }

    audio_music_fade_to(0.5f, "bgm/inter.ogg");

    return id;
}

static void course_leave(int id)
{
    gui_delete(id);
}

static void course_paint(int id, float t)
{
    game_draw(0, t);
    gui_paint(id);
}

static void course_timer(int id, float dt)
{
    gui_timer(id, dt);
}

static void course_point(int id, int x, int y, int dx, int dy)
{
    int jd;

    if ((jd = gui_point(id, x, y)))
    {
        int i = gui_token(jd);

        if (course_exists(i))
        {
            gui_set_image(shot_id, course_shot(i));
            gui_set_multi(desc_id, _(course_desc(i)));
        }
        gui_pulse(jd, 1.2f);
    }
}

static void course_stick(int id, int a, int v)
{
    int jd;

    if ((jd = shared_stick_basic(id, a, v)))
    {
        int i = gui_token(jd);

        if (course_exists(i))
        {
            gui_set_image(shot_id, course_shot(i));
            gui_set_multi(desc_id, _(course_desc(i)));
        }
        gui_pulse(jd, 1.2f);
    }
}

static int course_click(int b, int d)
{
    return d && b == SDL_BUTTON_LEFT ? course_action(gui_token(gui_click())) : 1;
}

static int course_buttn(int b, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
            return course_action(gui_token(gui_click()));
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return course_action(COURSE_BACK);
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

#define PARTY_T 0
#define PARTY_1 1
#define PARTY_2 2
#define PARTY_3 3
#define PARTY_4 4
#define PARTY_B 5

static int party_action(int i)
{
    switch (i)
    {
    case PARTY_1:
        audio_play(AUD_MENU, 1.f);
        hole_goto(1, 1);
        goto_state(&st_next);
        break;
    case PARTY_2:
        audio_play(AUD_MENU, 1.f);
        hole_goto(1, 2);
        goto_state(&st_next);
        break;
    case PARTY_3:
        audio_play(AUD_MENU, 1.f);
        hole_goto(1, 3);
        goto_state(&st_next);
        break;
    case PARTY_4:
        audio_play(AUD_MENU, 1.f);
        hole_goto(1, 4);
        goto_state(&st_next);
        break;
    case PARTY_B:
        audio_play(AUD_MENU, 1.f);
        goto_state(&st_course);
        break;
    }
    return 1;
}

static int party_enter(void)
{
    int id, jd;

    if ((id = gui_vstack(0)))
    {
        gui_label(id, _("Players?"), GUI_MED, GUI_ALL, 0, 0);
        gui_space(id);

        if ((jd = gui_harray(id)))
        {
            int p4 = gui_state(jd, "4", GUI_LRG, PARTY_4, 0);
            int p3 = gui_state(jd, "3", GUI_LRG, PARTY_3, 0);
            int p2 = gui_state(jd, "2", GUI_LRG, PARTY_2, 0);
            int p1 = gui_state(jd, "1", GUI_LRG, PARTY_1, 0);

            gui_set_color(p1, gui_red, gui_wht);
            gui_set_color(p2, gui_grn, gui_wht);
            gui_set_color(p3, gui_blu, gui_wht);
            gui_set_color(p4, gui_yel, gui_wht);

            gui_focus(p1);
        }

        gui_space(id);

        if ((jd = gui_hstack(id)))
        {
            gui_filler(jd);
            gui_state(jd, _("Back"), GUI_SML, PARTY_B, 0);
        }

        gui_layout(id, 0, 0);
    }

    return id;
}

static void party_leave(int id)
{
    gui_delete(id);
}

static void party_paint(int id, float t)
{
    game_draw(0, t);
    gui_paint(id);
}

static void party_timer(int id, float dt)
{
    gui_timer(id, dt);
}

static void party_point(int id, int x, int y, int dx, int dy)
{
    gui_pulse(gui_point(id, x, y), 1.2f);
}

static int party_click(int b, int d)
{
    return d && b == SDL_BUTTON_LEFT ? party_action(gui_token(gui_click())) : 1;
}

static int party_buttn(int b, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
            return party_action(gui_token(gui_click()));
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return party_action(PARTY_B);
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

static int paused = 0;

static struct state *st_continue;
static struct state *st_quit;

#define PAUSE_CONTINUE 1
#define PAUSE_QUIT     2

int goto_pause(struct state *s, int e)
{
    if (curr_state() == &st_pause)
        return 1;

    if (e && !config_tst_d(CONFIG_KEY_PAUSE, SDLK_ESCAPE))
        return goto_state(s);

    st_continue = curr_state();
    st_quit = s;
    paused = 1;

    return goto_state(&st_pause);
}

static int pause_action(int i)
{
    audio_play(AUD_MENU, 1.0f);

    switch(i)
    {
    case PAUSE_CONTINUE:
        return goto_state(st_continue ? st_continue : &st_title);

    case PAUSE_QUIT:
        return goto_state(st_quit);
    }
    return 1;
}

static int pause_enter(void)
{
    int id, jd, td;

    audio_music_fade_out(0.2f);

    if ((id = gui_vstack(0)))
    {
        td = gui_label(id, _("Paused"), GUI_LRG, GUI_ALL, 0, 0);
        gui_space(id);

        if ((jd = gui_harray(id)))
        {
            gui_state(jd, _("Quit"), GUI_SML, PAUSE_QUIT, 0);
            gui_start(jd, _("Continue"), GUI_SML, PAUSE_CONTINUE, 1);
        }

        gui_pulse(td, 1.2f);
        gui_layout(id, 0, 0);
    }

    hud_init();
    return id;
}

static void pause_leave(int id)
{
    gui_delete(id);
    hud_free();
    audio_music_fade_in(0.5f);
}

static void pause_paint(int id, float t)
{
    game_draw(0, t);
    gui_paint(id);
    hud_paint();
}

static void pause_timer(int id, float dt)
{
    gui_timer(id, dt);
}

static void pause_point(int id, int x, int y, int dx, int dy)
{
    gui_pulse(gui_point(id, x, y), 1.2f);
}

static int pause_click(int b, int d)
{
    return d && b == SDL_BUTTON_LEFT ? pause_action(gui_token(gui_click())) : 1;
}

static int pause_keybd(int c, int d)
{
    if (d && config_tst_d(CONFIG_KEY_PAUSE, c))
        return pause_action(PAUSE_CONTINUE);
    return 1;
}

static int pause_buttn(int b, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
            return pause_action(gui_token(gui_click()));
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return pause_action(PAUSE_CONTINUE);
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

static int shared_keybd(int c, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_KEY_PAUSE, c))
            return goto_pause(&st_over, 0);
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

static int num = 0;

static int next_enter(void)
{
    int id;
    char str[MAXSTR];

    sprintf(str, _("Hole %02d"), curr_hole());

    if ((id = gui_vstack(0)))
    {
        gui_label(id, str, GUI_MED, GUI_ALL, 0, 0);
        gui_space(id);

        gui_label(id, _("Player"), GUI_SML, GUI_TOP, 0, 0);

        switch (curr_player())
        {
        case 1:
            gui_label(id, "1", GUI_LRG, GUI_BOT, gui_red, gui_wht);
            if (curr_party() > 1) audio_play(AUD_PLAYER1, 1.f);
            break;
        case 2:
            gui_label(id, "2", GUI_LRG, GUI_BOT, gui_grn, gui_wht);
            if (curr_party() > 1) audio_play(AUD_PLAYER2, 1.f);
            break;
        case 3:
            gui_label(id, "3", GUI_LRG, GUI_BOT, gui_blu, gui_wht);
            if (curr_party() > 1) audio_play(AUD_PLAYER3, 1.f);
            break;
        case 4:
            gui_label(id, "4", GUI_LRG, GUI_BOT, gui_yel, gui_wht);
            if (curr_party() > 1) audio_play(AUD_PLAYER4, 1.f);
            break;
        }
        gui_layout(id, 0, 0);
    }

    hud_init();
    game_set_fly(1.f);

    if (paused)
        paused = 0;

    return id;
}

static void next_leave(int id)
{
    hud_free();
    gui_delete(id);
}

static void next_paint(int id, float t)
{
    game_draw(0, t);
    hud_paint();
    gui_paint(id);
}

static void next_timer(int id, float dt)
{
    gui_timer(id, dt);
}

static void next_point(int id, int x, int y, int dx, int dy)
{
    gui_pulse(gui_point(id, x, y), 1.2f);
}

static int next_click(int b, int d)
{
    return (d && b == SDL_BUTTON_LEFT) ? goto_state(&st_flyby) : 1;
}

static int next_keybd(int c, int d)
{
    if (d)
    {
        if (c == SDLK_F12)
            return goto_state(&st_poser);
        if (config_tst_d(CONFIG_KEY_PAUSE, c))
            return goto_pause(&st_over, 0);
        if ('0' <= c && c <= '9')
            num = num * 10 + c - '0';
    }
    return 1;
}

static int next_buttn(int b, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
        {
            if (num > 0)
            {
                hole_goto(num, -1);
                num = 0;
                return goto_state(&st_next);
            }
            return goto_state(&st_flyby);
        }
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return goto_pause(&st_over, 1);
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

static int poser_enter(void)
{
    game_set_fly(-1.f);
    return 0;
}

static void poser_paint(int id, float t)
{
    game_draw(1, t);
}

static int poser_buttn(int b, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
            return goto_state(&st_next);
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return goto_state(&st_next);
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

static int flyby_enter(void)
{
    if (paused)
        paused = 0;
    else
        hud_init();

    return 0;
}

static void flyby_leave(int id)
{
    hud_free();
}

static void flyby_paint(int id, float t)
{
    game_draw(0, t);
    hud_paint();
}

static void flyby_timer(int id, float dt)
{
    float t = time_state();

    if (dt > 0.f && t > 1.f)
        goto_state(&st_stroke);
    else
        game_set_fly(1.f - t);

    gui_timer(id, dt);
}

static int flyby_click(int b, int d)
{
    if (d && b == SDL_BUTTON_LEFT)
    {
        game_set_fly(0.f);
        return goto_state(&st_stroke);
    }
    return 1;
}

static int flyby_buttn(int b, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
        {
            game_set_fly(0.f);
            return goto_state(&st_stroke);
        }
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return goto_pause(&st_over, 1);
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

static int stroke_rotate = 0;
static int stroke_mag    = 0;

static int stroke_enter(void)
{
    hud_init();
    game_clr_mag();
    config_set_d(CONFIG_CAMERA, 2);
    video_set_grab(!paused);

    if (paused)
        paused = 0;

    return 0;
}

static void stroke_leave(int id)
{
    hud_free();
    video_clr_grab();
    config_set_d(CONFIG_CAMERA, 0);
}

static void stroke_paint(int id, float t)
{
    game_draw(0, t);
    hud_paint();
}

static void stroke_timer(int id, float dt)
{
    float g[3] = { 0.f, 0.f, 0.f };

    float k;

    if (SDL_GetModState() & KMOD_SHIFT ||
        (joystick && SDL_JoystickGetButton(joystick,
                                           config_get_d(CONFIG_JOYSTICK_BUTTON_B))))
        k = 0.25;
    else
        k = 1.0;

    game_set_rot(stroke_rotate * k);
    game_set_mag(stroke_mag * k);

    game_update_view(dt);
    game_step(g, dt);
}

static void stroke_point(int id, int x, int y, int dx, int dy)
{
    game_set_rot(dx);
    game_set_mag(dy);
}

static void stroke_stick(int id, int a, int v)
{
    if (v == 1) /* See 'loop' in main.c */
        v = 0;

    if (config_tst_d(CONFIG_JOYSTICK_AXIS_X, a))
        stroke_rotate = (6 * v) / JOY_MAX;
    else if (config_tst_d(CONFIG_JOYSTICK_AXIS_Y, a))
        stroke_mag = -((6 * v) / JOY_MAX);
}

static int stroke_click(int b, int d)
{
    return (d && b == SDL_BUTTON_LEFT) ? goto_state(&st_roll) : 1;
}

static int stroke_buttn(int b, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
            return goto_state(&st_roll);
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return goto_pause(&st_over, 1);
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

static int roll_enter(void)
{
    hud_init();

    if (paused)
        paused = 0;
    else
        game_putt();

    return 0;
}

static void roll_leave(int id)
{
    hud_free();
}

static void roll_paint(int id, float t)
{
    game_draw(0, t);
    hud_paint();
}

static void roll_timer(int id, float dt)
{
    float g[3] = { 0.0f, -9.8f, 0.0f };

    switch (game_step(g, dt))
    {
    case GAME_STOP: goto_state(&st_stop); break;
    case GAME_GOAL: goto_state(&st_goal); break;
    case GAME_FALL: goto_state(&st_fall); break;
    }
}

static int roll_buttn(int b, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return goto_pause(&st_over, 1);
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

static int goal_enter(void)
{
    int id;

    if ((id = gui_label(0, _("It's In!"), GUI_MED, GUI_ALL, gui_grn, gui_grn)))
        gui_layout(id, 0, 0);

    if (paused)
        paused = 0;
    else
        hole_goal();

    hud_init();

    return id;
}

static void goal_leave(int id)
{
    gui_delete(id);
    hud_free();
}

static void goal_paint(int id, float t)
{
    game_draw(0, t);
    gui_paint(id);
    hud_paint();
}

static void goal_timer(int id, float dt)
{
    if (time_state() > 3)
    {
        if (hole_next())
            goto_state(&st_next);
        else
            goto_state(&st_score);
    }
}

static int goal_click(int b, int d)
{
    if (b == SDL_BUTTON_LEFT && d == 1)
    {
        if (hole_next())
            goto_state(&st_next);
        else
            goto_state(&st_score);
    }
    return 1;
}

static int goal_buttn(int b, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
        {
            if (hole_next())
                goto_state(&st_next);
            else
                goto_state(&st_score);
        }
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return goto_pause(&st_over, 1);
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

static int stop_enter(void)
{
    if (paused)
        paused = 0;
    else
        hole_stop();

    hud_init();

    return 0;
}

static void stop_leave(int id)
{
    hud_free();
}

static void stop_paint(int id, float t)
{
    game_draw(0, t);
    hud_paint();
}

static void stop_timer(int id, float dt)
{
    float g[3] = { 0.f, 0.f, 0.f };

    game_update_view(dt);
    game_step(g, dt);

    if (time_state() > 1)
    {
        if (hole_next())
            goto_state(&st_next);
        else
            goto_state(&st_score);
    }
}

static int stop_click(int b, int d)
{
    if (b == SDL_BUTTON_LEFT && d == 1)
    {
        if (hole_next())
            goto_state(&st_next);
        else
            goto_state(&st_score);
    }
    return 1;
}

static int stop_buttn(int b, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
        {
            if (hole_next())
                goto_state(&st_next);
            else
                goto_state(&st_score);
        }
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return goto_pause(&st_over, 1);
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

static int fall_enter(void)
{
    int id;

    if ((id = gui_label(0, _("1 Stroke Penalty"), GUI_MED, GUI_ALL, gui_blk, gui_red)))
        gui_layout(id, 0, 0);

    if (paused)
        paused = 0;
    else
    {
        hole_fall();
/*        game_draw(0);*/ /*TODO: is this call ok? */  /* No, it's not. */
    }

    hud_init();

    return id;
}

static void fall_leave(int id)
{
    gui_delete(id);
    hud_free();
}

static void fall_paint(int id, float t)
{
    game_draw(0, t);
    gui_paint(id);
    hud_paint();
}

static void fall_timer(int id, float dt)
{
    if (time_state() > 3)
    {
        if (hole_next())
            goto_state(&st_next);
        else
            goto_state(&st_score);
    }
}

static int fall_click(int b, int d)
{
    if (b == SDL_BUTTON_LEFT && d == 1)
    {
        if (hole_next())
            goto_state(&st_next);
        else
            goto_state(&st_score);
    }
    return 1;
}

static int fall_buttn(int b, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
        {
            if (hole_next())
                goto_state(&st_next);
            else
                goto_state(&st_score);
        }
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return goto_pause(&st_over, 1);
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

static int score_enter(void)
{
    audio_music_fade_out(2.f);

    if (paused)
        paused = 0;

    return score_card(_("Scores"), gui_yel, gui_red);
}

static void score_leave(int id)
{
    gui_delete(id);
}

static void score_paint(int id, float t)
{
    game_draw(0, t);
    gui_paint(id);
}

static void score_timer(int id, float dt)
{
    gui_timer(id, dt);
}

static int score_click(int b, int d)
{
    if (b == SDL_BUTTON_LEFT && d == 1)
    {
        if (hole_move())
            return goto_state(&st_next);
        else
            return goto_state(&st_title);
    }
    return 1;
}

static int score_buttn(int b, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
        {
            if (hole_move())
                goto_state(&st_next);
            else
                goto_state(&st_score);
        }
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return goto_pause(&st_over, 1);
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

static int over_enter(void)
{
    audio_music_fade_out(2.f);
    return score_card(_("Final Scores"), gui_yel, gui_red);
}

static void over_leave(int id)
{
    gui_delete(id);
}

static void over_paint(int id, float t)
{
    game_draw(0, t);
    gui_paint(id);
}

static void over_timer(int id, float dt)
{
    gui_timer(id, dt);
}

static int over_click(int b, int d)
{
    return (d && b == SDL_BUTTON_LEFT) ? goto_state(&st_title) : 1;
}

static int over_buttn(int b, int d)
{
    if (d)
    {
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_A, b))
            return goto_state(&st_title);
        if (config_tst_d(CONFIG_JOYSTICK_BUTTON_EXIT, b))
            return goto_state(&st_title);
    }
    return 1;
}

/*---------------------------------------------------------------------------*/

struct state st_title = {
    title_enter,
    title_leave,
    title_paint,
    title_timer,
    title_point,
    shared_stick,
    NULL,
    title_click,
    NULL,
    title_buttn,
    1, 0
};

struct state st_course = {
    course_enter,
    course_leave,
    course_paint,
    course_timer,
    course_point,
    course_stick,
    NULL,
    course_click,
    NULL,
    course_buttn,
    1, 0
};

struct state st_party = {
    party_enter,
    party_leave,
    party_paint,
    party_timer,
    party_point,
    shared_stick,
    NULL,
    party_click,
    NULL,
    party_buttn,
    1, 0
};

struct state st_next = {
    next_enter,
    next_leave,
    next_paint,
    next_timer,
    next_point,
    shared_stick,
    NULL,
    next_click,
    next_keybd,
    next_buttn,
    1, 0
};

struct state st_poser = {
    poser_enter,
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

struct state st_flyby = {
    flyby_enter,
    flyby_leave,
    flyby_paint,
    flyby_timer,
    NULL,
    NULL,
    NULL,
    flyby_click,
    shared_keybd,
    flyby_buttn,
    1, 0
};

struct state st_stroke = {
    stroke_enter,
    stroke_leave,
    stroke_paint,
    stroke_timer,
    stroke_point,
    stroke_stick,
    NULL,
    stroke_click,
    shared_keybd,
    stroke_buttn,
    0, 0
};

struct state st_roll = {
    roll_enter,
    roll_leave,
    roll_paint,
    roll_timer,
    NULL,
    NULL,
    NULL,
    NULL,
    shared_keybd,
    roll_buttn,
    0, 0
};

struct state st_goal = {
    goal_enter,
    goal_leave,
    goal_paint,
    goal_timer,
    NULL,
    NULL,
    NULL,
    goal_click,
    shared_keybd,
    goal_buttn,
    0, 0
};

struct state st_stop = {
    stop_enter,
    stop_leave,
    stop_paint,
    stop_timer,
    NULL,
    NULL,
    NULL,
    stop_click,
    shared_keybd,
    stop_buttn,
    0, 0
};

struct state st_fall = {
    fall_enter,
    fall_leave,
    fall_paint,
    fall_timer,
    NULL,
    NULL,
    NULL,
    fall_click,
    shared_keybd,
    fall_buttn,
    0, 0
};

struct state st_score = {
    score_enter,
    score_leave,
    score_paint,
    score_timer,
    NULL,
    NULL,
    NULL,
    score_click,
    shared_keybd,
    score_buttn,
    0, 0
};

struct state st_over = {
    over_enter,
    over_leave,
    over_paint,
    over_timer,
    NULL,
    NULL,
    NULL,
    over_click,
    NULL,
    over_buttn,
    1, 0
};

struct state st_pause = {
    pause_enter,
    pause_leave,
    pause_paint,
    pause_timer,
    pause_point,
    shared_stick,
    NULL,
    pause_click,
    pause_keybd,
    pause_buttn,
    1, 0
};
