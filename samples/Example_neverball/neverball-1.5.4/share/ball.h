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

#ifndef BALL_H
#define BALL_H

/*---------------------------------------------------------------------------*/

#define BALL_FUDGE 0.001f

void ball_init(void);
void ball_free(void);
void ball_draw(const float *,
               const float *,
               const float *, float);

/*---------------------------------------------------------------------------*/

#endif
