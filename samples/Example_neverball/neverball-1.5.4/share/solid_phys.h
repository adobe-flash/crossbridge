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

#ifndef SOL_PHYS_H
#define SOL_PHYS_H

#include "solid.h"
#include "cmd.h"

/*---------------------------------------------------------------------------*/

void sol_body_p(float p[3], const struct s_file *, const struct s_body *);

/*---------------------------------------------------------------------------*/

float sol_step(struct s_file *, const float *, float, int, int *);

int   sol_jump_test(struct s_file *, float *, int);
int   sol_swch_test(struct s_file *, int);

struct s_goal *sol_goal_test(struct s_file *, float *, int);
int            sol_item_test(struct s_file *, float *, float);

/*---------------------------------------------------------------------------*/

void sol_cmd_enq_func(void (*enq_fn) (const union cmd *));

/*---------------------------------------------------------------------------*/

#endif
