/*
 * Copyright (C) 2009 Neverball contributors
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

#include <stdlib.h>
#include "list.h"

/*
 * Allocate and return a list cell initialised with FIRST and REST as
 * "data" and "next" members, respectively.
 */
List list_cons(void *first, List rest)
{
    List new;

    if ((new = malloc(sizeof (*new))))
    {
        new->data = first;
        new->next = rest;
    }

    return new;
}

/*
 * Free the list cell FIRST and return the "next" member. The "data"
 * member is not freed.
 */
List list_rest(List first)
{
    List rest = first->next;
    free(first);
    return rest;
}
