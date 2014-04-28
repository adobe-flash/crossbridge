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

#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "gui.h"
#include "util.h"
#include "config.h"

/*---------------------------------------------------------------------------*/

static int is_special_name(const char *n)
{
    return (strcmp(n, N_("Hard"))   == 0 ||
            strcmp(n, N_("Medium")) == 0 ||
            strcmp(n, N_("Easy"))   == 0);
}

/*---------------------------------------------------------------------------*/

static int coin_btn_id;
static int time_btn_id;
static int goal_btn_id;

static void set_score_color(int id, int hi,
                            const GLfloat *c0,
                            const GLfloat *c1)
{
    if (hi >= 0)
    {
        if (hi < NSCORE)
            gui_set_color(id, c0, c0);
        else
            gui_set_color(id, c1, c1);
    }
}

/*---------------------------------------------------------------------------*/

static int score_label;

static int score_coin[4];
static int score_name[4];
static int score_time[4];

static int score_extra_row;

/* Build a top three score list with default values. */

static void gui_scores(int id, int e)
{
    const char *s = "1234567";

    int j, jd, kd, ld, md;

    score_extra_row = e;

    if ((jd = gui_hstack(id)))
    {
        gui_filler(jd);

        if ((kd = gui_vstack(jd)))
        {
            score_label = gui_label(kd, _("Unavailable"),
                                    GUI_SML, GUI_TOP, 0, 0);

            if ((ld = gui_hstack(kd)))
            {
                if ((md = gui_vstack(ld)))
                {
                    for (j = 0; j < NSCORE - 1; j++)
                        score_coin[j] = gui_count(md, 1000, GUI_SML, 0);

                    score_coin[j++] = gui_count(md, 1000, GUI_SML, GUI_SE);

                    if (e)
                    {
                        gui_space(md);
                        score_coin[j++] = gui_count(md, 1000, GUI_SML, GUI_RGT);
                    }
                }

                if ((md = gui_vstack(ld)))
                {
                    for (j = 0; j < NSCORE; j++)
                    {
                        score_name[j] = gui_label(md, s, GUI_SML, 0,
                                                  gui_yel, gui_wht);
                        gui_set_trunc(score_name[j], TRUNC_TAIL);
                    }

                    if (e)
                    {
                        gui_space(md);
                        score_name[j++] = gui_label(md, s, GUI_SML, 0,
                                                    gui_yel, gui_wht);
                        gui_set_trunc(score_name[j - 1], TRUNC_TAIL);
                    }
                }

                if ((md = gui_vstack(ld)))
                {
                    for (j = 0; j < NSCORE - 1; j++)
                        score_time[j] = gui_clock(md, 359999, GUI_SML, 0);

                    score_time[j++] = gui_clock(md, 359999, GUI_SML, GUI_SW);

                    if (e)
                    {
                        gui_space(md);
                        score_time[j++] = gui_clock(md, 359999, GUI_SML, GUI_LFT);
                    }
                }
            }
        }
        gui_filler(jd);
    }
}

/* Set the top three score list values. */

static void gui_set_scores(const char *label, const struct score *s, int hilite)
{
    const char *name;
    int j;

    if (s == NULL)
    {
        gui_set_label(score_label, _("Unavailable"));

        for (j = 0; j < NSCORE + score_extra_row ; j++)
        {
            gui_set_count(score_coin[j], -1);
            gui_set_label(score_name[j], "");
            gui_set_clock(score_time[j], -1);
        }
    }
    else
    {
        gui_set_label(score_label, label);

        for (j = 0; j < NSCORE + score_extra_row; j++)
        {
            name = s->player[j];

            if (j == hilite)
                set_score_color(score_name[j], j, gui_grn, gui_red);
            else
                gui_set_color(score_name[j], gui_yel, gui_wht);

            gui_set_count(score_coin[j], s->coins[j]);
            gui_set_label(score_name[j], is_special_name(name) ? _(name) : name);
            gui_set_clock(score_time[j], s->timer[j]);
        }
    }
}

/*---------------------------------------------------------------------------*/

static int score_type = GUI_MOST_COINS;

void gui_score_board(int pd, unsigned int types, int e, int h)
{
    int id, jd, kd, ld;

    assert((types & GUI_MOST_COINS)  == GUI_MOST_COINS ||
           (types & GUI_BEST_TIMES)  == GUI_BEST_TIMES ||
           (types & GUI_FAST_UNLOCK) == GUI_FAST_UNLOCK );

    /* Make sure current score type matches the spec. */

    while ((types & score_type) != score_type)
        score_type = gui_score_next(score_type);

    if ((id = gui_hstack(pd)))
    {
        gui_filler(id);

        if ((jd = gui_hstack(id)))
        {
            gui_filler(jd);

            if ((kd = gui_vstack(jd)))
            {
                gui_filler(kd);

                if ((types & GUI_MOST_COINS) == GUI_MOST_COINS)
                {
                    coin_btn_id = gui_state(kd, _("Most Coins"),
                                            GUI_SML, GUI_MOST_COINS,
                                            score_type == GUI_MOST_COINS);
                }
                if ((types & GUI_BEST_TIMES) == GUI_BEST_TIMES)
                {
                    time_btn_id = gui_state(kd, _("Best Times"),
                                            GUI_SML, GUI_BEST_TIMES,
                                            score_type == GUI_BEST_TIMES);
                }
                if ((types & GUI_FAST_UNLOCK) == GUI_FAST_UNLOCK)
                {
                    goal_btn_id = gui_state(kd, _("Fast Unlock"),
                                            GUI_SML, GUI_FAST_UNLOCK,
                                            score_type == GUI_FAST_UNLOCK);
                }

                if (h)
                {
                    gui_space(kd);

                    if ((ld = gui_hstack(kd)))
                    {
                        gui_filler(ld);
                        gui_state(ld, _("Change Name"), GUI_SML, GUI_NAME, 0);
                        gui_filler(ld);
                    }
                }

                gui_filler(kd);
            }

            gui_filler(jd);
        }

        gui_filler(id);
        gui_scores(id, e);
        gui_filler(id);
    }
}

void set_score_board(const struct score *smc, int hmc,
                     const struct score *sbt, int hbt,
                     const struct score *sfu, int hfu)
{
    switch (score_type)
    {
    case GUI_MOST_COINS:
        gui_set_scores(_("Most Coins"), smc, hmc);
        break;

    case GUI_BEST_TIMES:
        gui_set_scores(_("Best Times"), sbt, hbt);
        break;

    case GUI_FAST_UNLOCK:
        gui_set_scores(_("Fast Unlock"), sfu, hfu);
        break;
    }

    set_score_color(coin_btn_id, hmc, gui_grn, gui_wht);
    set_score_color(time_btn_id, hbt, gui_grn, gui_wht);
    set_score_color(goal_btn_id, hfu, gui_grn, gui_wht);
}

void gui_score_set(int t)
{
    score_type = t;
}

int  gui_score_get(void)
{
    return score_type;
}

int  gui_score_next(int t)
{
    switch (t)
    {
    case GUI_MOST_COINS:  return GUI_BEST_TIMES;
    case GUI_BEST_TIMES:  return GUI_FAST_UNLOCK;
    case GUI_FAST_UNLOCK: return GUI_MOST_COINS;

    default:
        return GUI_MOST_COINS;
    }
}

/*---------------------------------------------------------------------------*/

static int lock = 1;
static int keyd[127];

void gui_keyboard(int id)
{
    int jd, kd, ld;

    lock = 1;

    if ((jd = gui_hstack(id)))
    {
        gui_filler(jd);

        if ((kd = gui_vstack(jd)))
        {
            if ((ld = gui_hstack(kd)))
            {
                gui_filler(ld);

                keyd['9'] = gui_state(ld, "9", GUI_SML, '9', 0);
                keyd['8'] = gui_state(ld, "8", GUI_SML, '8', 0);
                keyd['7'] = gui_state(ld, "7", GUI_SML, '7', 0);
                keyd['6'] = gui_state(ld, "6", GUI_SML, '6', 0);
                keyd['5'] = gui_state(ld, "5", GUI_SML, '5', 0);
                keyd['4'] = gui_state(ld, "4", GUI_SML, '4', 0);
                keyd['3'] = gui_state(ld, "3", GUI_SML, '3', 0);
                keyd['2'] = gui_state(ld, "2", GUI_SML, '2', 0);
                keyd['1'] = gui_state(ld, "1", GUI_SML, '1', 0);
                keyd['0'] = gui_state(ld, "0", GUI_SML, '0', 0);
                gui_filler(ld);
            }
            if ((ld = gui_hstack(kd)))
            {
                gui_filler(ld);
                keyd['J'] = gui_state(ld, "J", GUI_SML, 'J', 0);
                keyd['I'] = gui_state(ld, "I", GUI_SML, 'I', 0);
                keyd['H'] = gui_state(ld, "H", GUI_SML, 'H', 0);
                keyd['G'] = gui_state(ld, "G", GUI_SML, 'G', 0);
                keyd['F'] = gui_state(ld, "F", GUI_SML, 'F', 0);
                keyd['E'] = gui_state(ld, "E", GUI_SML, 'E', 0);
                keyd['D'] = gui_state(ld, "D", GUI_SML, 'D', 0);
                keyd['C'] = gui_state(ld, "C", GUI_SML, 'C', 0);
                keyd['B'] = gui_state(ld, "B", GUI_SML, 'B', 0);
                keyd['A'] = gui_state(ld, "A", GUI_SML, 'A', 0);
                gui_filler(ld);
            }
            if ((ld = gui_hstack(kd)))
            {
                gui_filler(ld);
                keyd['T'] = gui_state(ld, "T", GUI_SML, 'T', 0);
                keyd['S'] = gui_state(ld, "S", GUI_SML, 'S', 0);
                keyd['R'] = gui_state(ld, "R", GUI_SML, 'R', 0);
                keyd['Q'] = gui_state(ld, "Q", GUI_SML, 'Q', 0);
                keyd['P'] = gui_state(ld, "P", GUI_SML, 'P', 0);
                keyd['O'] = gui_state(ld, "O", GUI_SML, 'O', 0);
                keyd['N'] = gui_state(ld, "N", GUI_SML, 'N', 0);
                keyd['M'] = gui_state(ld, "M", GUI_SML, 'M', 0);
                keyd['L'] = gui_state(ld, "L", GUI_SML, 'L', 0);
                keyd['K'] = gui_state(ld, "K", GUI_SML, 'K', 0);
                gui_filler(ld);
            }
            if ((ld = gui_hstack(kd)))
            {
                gui_filler(ld);
                gui_state(ld, "<", GUI_SML, GUI_BS, 0);
                keyd['Z'] = gui_state(ld, "Z", GUI_SML, 'Z', 0);
                keyd['Y'] = gui_state(ld, "Y", GUI_SML, 'Y', 0);
                keyd['X'] = gui_state(ld, "X", GUI_SML, 'X', 0);
                keyd['W'] = gui_state(ld, "W", GUI_SML, 'W', 0);
                keyd['V'] = gui_state(ld, "V", GUI_SML, 'V', 0);
                keyd['U'] = gui_state(ld, "U", GUI_SML, 'U', 0);
                gui_state(ld, _("caps"), GUI_SML, GUI_CL, 0);
                gui_filler(ld);
            }
        }
        gui_filler(jd);
    }
}

void gui_keyboard_lock(void)
{
    lock = lock ? 0 : 1;

    gui_set_label(keyd['A'], lock ? "A" : "a");
    gui_set_label(keyd['B'], lock ? "B" : "b");
    gui_set_label(keyd['C'], lock ? "C" : "c");
    gui_set_label(keyd['D'], lock ? "D" : "d");
    gui_set_label(keyd['E'], lock ? "E" : "e");
    gui_set_label(keyd['F'], lock ? "F" : "f");
    gui_set_label(keyd['G'], lock ? "G" : "g");
    gui_set_label(keyd['H'], lock ? "H" : "h");
    gui_set_label(keyd['I'], lock ? "I" : "i");
    gui_set_label(keyd['J'], lock ? "J" : "j");
    gui_set_label(keyd['K'], lock ? "K" : "k");
    gui_set_label(keyd['L'], lock ? "L" : "l");
    gui_set_label(keyd['M'], lock ? "M" : "m");
    gui_set_label(keyd['N'], lock ? "N" : "n");
    gui_set_label(keyd['O'], lock ? "O" : "o");
    gui_set_label(keyd['P'], lock ? "P" : "p");
    gui_set_label(keyd['Q'], lock ? "Q" : "q");
    gui_set_label(keyd['R'], lock ? "R" : "r");
    gui_set_label(keyd['S'], lock ? "S" : "s");
    gui_set_label(keyd['T'], lock ? "T" : "t");
    gui_set_label(keyd['U'], lock ? "U" : "u");
    gui_set_label(keyd['V'], lock ? "V" : "v");
    gui_set_label(keyd['W'], lock ? "W" : "w");
    gui_set_label(keyd['X'], lock ? "X" : "x");
    gui_set_label(keyd['Y'], lock ? "Y" : "y");
    gui_set_label(keyd['Z'], lock ? "Z" : "z");
}

char gui_keyboard_char(char c)
{
    return lock ? toupper(c) : tolower(c);
}

/*---------------------------------------------------------------------------*/

/*
 * XXX Watch  out when  using these  functions. Be  sure to  check for
 * GUI_NULL in addition to GUI_NEXT and GUI_PREV when using the latter
 * two as labels for a switch with a default label.
 */

int gui_navig(int id, int prev, int next)
{
    int jd;

    if ((jd = gui_hstack(id)))
    {
        if (next || prev)
        {
            gui_maybe(jd, _("Next"), GUI_NEXT, GUI_NULL, next);
            gui_maybe(jd, _("Prev"), GUI_PREV, GUI_NULL, prev);
        }

        gui_space(jd);

        gui_start(jd, _("Back"), GUI_SML, GUI_BACK, 0);
    }
    return jd;
}

int gui_maybe(int id, const char *label, int etoken, int dtoken, int enabled)
{
    int bd;

    if (!enabled)
    {
        bd = gui_state(id, label, GUI_SML, dtoken, 0);
        gui_set_color(bd, gui_gry, gui_gry);
    }
    else
        bd = gui_state(id, label, GUI_SML, etoken, 0);

    return bd;
}

/*---------------------------------------------------------------------------*/
