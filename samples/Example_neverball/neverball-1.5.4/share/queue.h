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

#ifndef QUEUE_H
#define QUEUE_H

typedef struct queue *Queue;

Queue  queue_new(void);
void   queue_free(Queue);
int    queue_empty(Queue);
void   queue_enq(Queue, void *);
void  *queue_deq(Queue);

#endif
