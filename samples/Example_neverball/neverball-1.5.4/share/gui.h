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

#ifndef GUI_H
#define GUI_H

#include "glext.h"
#include "lang.h"

/*---------------------------------------------------------------------------*/

#define GUI_FACE N_("ttf/DejaVuSans-Bold.ttf")

#define GUI_SML  0
#define GUI_MED  1
#define GUI_LRG  2

#define GUI_NW   1
#define GUI_SW   2
#define GUI_NE   4
#define GUI_SE   8

#define GUI_LFT  (GUI_NW  | GUI_SW)
#define GUI_RGT  (GUI_NE  | GUI_SE)
#define GUI_TOP  (GUI_NW  | GUI_NE)
#define GUI_BOT  (GUI_SW  | GUI_SE)
#define GUI_ALL  (GUI_TOP | GUI_BOT)

extern const GLfloat gui_wht[4];
extern const GLfloat gui_yel[4];
extern const GLfloat gui_red[4];
extern const GLfloat gui_blu[4];
extern const GLfloat gui_grn[4];
extern const GLfloat gui_blk[4];
extern const GLfloat gui_gry[4];

/*---------------------------------------------------------------------------*/

void gui_init(void);
void gui_free(void);

/*---------------------------------------------------------------------------*/

enum trunc
{
    TRUNC_NONE,
    TRUNC_HEAD,
    TRUNC_TAIL
};

void gui_set_label(int, const char *);
void gui_set_image(int, const char *);
void gui_set_multi(int, const char *);
void gui_set_count(int, int);
void gui_set_clock(int, int);
void gui_set_color(int, const float *, const float *);
void gui_set_trunc(int, enum trunc);

/*---------------------------------------------------------------------------*/

int  gui_harray(int);
int  gui_varray(int);
int  gui_hstack(int);
int  gui_vstack(int);
int  gui_filler(int);

int  gui_image(int, const char *, int, int);
int  gui_start(int, const char *, int, int, int);
int  gui_state(int, const char *, int, int, int);
int  gui_label(int, const char *, int, int, const float *, const float *);
int  gui_multi(int, const char *, int, int, const float *, const float *);
int  gui_count(int, int, int, int);
int  gui_clock(int, int, int, int);
int  gui_space(int);

/*---------------------------------------------------------------------------*/

void gui_dump(int, int);
void gui_layout(int, int, int);
int  gui_search(int, int, int);
int  gui_active(int, int, int);
int  gui_delete(int);

/*---------------------------------------------------------------------------*/

void gui_paint(int);
void gui_pulse(int, float);
void gui_timer(int, float);
int  gui_point(int, int, int);
int  gui_stick(int, int, int);
void gui_stuck(void);
int  gui_click(void);
void gui_focus(int);

int  gui_token(int);
int  gui_value(int);
void gui_toggle(int);

/*---------------------------------------------------------------------------*/

#endif
