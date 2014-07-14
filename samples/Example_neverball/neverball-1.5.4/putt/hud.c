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

#include <SDL.h>
#include <math.h>

#include "gui.h"
#include "hud.h"
#include "hole.h"
#include "config.h"

/*---------------------------------------------------------------------------*/

static int Lhud_id;
static int Rhud_id;
static int fps_id;

/*---------------------------------------------------------------------------*/

void hud_init(void)
{
    static const float *color[5] = {
        gui_wht,
        gui_red,
        gui_grn,
        gui_blu,
        gui_yel
    };
    int i = curr_player();

    if ((Lhud_id = gui_hstack(0)))
    {
        gui_label(Lhud_id, curr_scr(), GUI_MED, GUI_NE, color[i], gui_wht);
        gui_label(Lhud_id, _("Score"), GUI_SML, 0,      gui_wht,  gui_wht);
        gui_layout(Lhud_id, -1, -1);
    }
    if ((Rhud_id = gui_hstack(0)))
    {
        gui_label(Rhud_id, curr_par(), GUI_MED, 0,      color[i], gui_wht);
        gui_label(Rhud_id, _("Par"),   GUI_SML, GUI_NW, gui_wht,  gui_wht);
        gui_layout(Rhud_id, +1, -1);
    }
    if ((fps_id = gui_count(0, 1000, GUI_SML, GUI_SE)))
        gui_layout(fps_id, -1, +1);
}

void hud_free(void)
{
    gui_delete(Lhud_id);
    gui_delete(Rhud_id);
    gui_delete(fps_id);
}

/*---------------------------------------------------------------------------*/

void hud_paint(void)
{
    static int fps   = 0;
    static int then  = 0;
    static int count = 0;

    int now = SDL_GetTicks();

    if (now - then > 250)
    {
        fps   = count * 1000 / (now - then);
        then  = now;
        count = 0;

        gui_set_count(fps_id, fps);
    }
    else count++;

    if (config_get_d(CONFIG_FPS))
        gui_paint(fps_id);

    gui_paint(Rhud_id);
    gui_paint(Lhud_id);
}

/*---------------------------------------------------------------------------*/
