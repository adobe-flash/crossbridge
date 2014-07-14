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

#ifndef ITEM_H
#define ITEM_H

#include "solid.h"

#define ITEM_RADIUS 0.15f

void item_color(const struct s_item *, float *);
void item_init(void);
void item_free(void);

void item_push(int);
void item_draw(const struct s_item *, float);
void item_pull(void);

#endif
