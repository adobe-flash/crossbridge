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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "config.h"
#include "course.h"
#include "hole.h"
#include "fs.h"

/*---------------------------------------------------------------------------*/

struct course
{
    char holes[MAXSTR];

    char shot[MAXSTR];
    char desc[MAXSTR];
};

static int course_state = 0;

static int course;
static int count;

static struct course course_v[MAXCRS];

/*---------------------------------------------------------------------------*/

static int course_load(struct course *course, const char *filename)
{
    fs_file fin;
    int rc = 0;

    memset(course, 0, sizeof (*course));

    strncpy(course->holes, filename, MAXSTR - 1);

    if ((fin = fs_open(filename, "r")))
    {
        if (fs_gets(course->shot, sizeof (course->shot), fin) &&
            fs_gets(course->desc, sizeof (course->desc), fin))
        {
            strip_newline(course->shot);
            strip_newline(course->desc);

            rc = 1;
        }

        fs_close(fin);
    }

    return rc;
}

static int cmp_dir_items(const void *A, const void *B)
{
    const struct dir_item *a = A, *b = B;
    return strcmp(a->path, b->path);
}

static int course_is_loaded(const char *path)
{
    int i;

    for (i = 0; i < count; i++)
        if (strcmp(course_v[i].holes, path) == 0)
            return 1;

    return 0;
}

static int is_unseen_course(struct dir_item *item)
{
    return (strncmp(base_name(item->path, NULL), "holes-", 6) == 0 &&
            strcmp(item->path + strlen(item->path) - 4, ".txt") == 0 &&
            !course_is_loaded(item->path));
}

void course_init()
{
    fs_file fin;
    char *line;

    Array items;
    int i;

    if (course_state)
        course_free();

    count = 0;

    if ((fin = fs_open(COURSE_FILE, "r")))
    {
        while (count < MAXCRS && read_line(&line, fin))
        {
            if (course_load(&course_v[count], line))
                count++;

            free(line);
        }

        fs_close(fin);

        course_state = 1;
    }

    if ((items = fs_dir_scan("", is_unseen_course)))
    {
        array_sort(items, cmp_dir_items);

        for (i = 0; i < array_len(items) && count < MAXCRS; i++)
            if (course_load(&course_v[count], DIR_ITEM_GET(items, i)->path))
                count++;

        fs_dir_free(items);

        course_state = 1;
    }
}

int course_exists(int i)
{
    return (0 <= i && i < count);
}

int course_count(void)
{
    return count;
}

void course_goto(int i)
{
    hole_init(course_v[i].holes);
    course = i;
}

int course_curr(void)
{
    return course;
}

void course_free(void)
{
    hole_free();
    course_state = 0;
}

void course_rand(void)
{
    course_goto(rand() % count);
    hole_goto(rand() % curr_count(), 4);
}

/*---------------------------------------------------------------------------*/

const char *course_desc(int i)
{
    return course_exists(i) ? course_v[i].desc : "";
}

const char *course_shot(int i)
{
    return course_exists(i) ? course_v[i].shot : course_v[0].shot;
}

/*---------------------------------------------------------------------------*/
