/*
 *  Copyright (C) 2007  Neverball contributors
 *
 *  This  program is  free software;  you can  redistribute  it and/or
 *  modify it  under the  terms of the  GNU General Public  License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  This program  is distributed in the  hope that it  will be useful,
 *  but  WITHOUT ANY WARRANTY;  without even  the implied  warranty of
 *  MERCHANTABILITY or FITNESS FOR  A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a  copy of the GNU General Public License
 *  along  with this  program;  if  not, write  to  the Free  Software
 *  Foundation,  Inc.,   59  Temple  Place,  Suite   330,  Boston,  MA
 *  02111-1307 USA
 */

#ifndef COMMON_H
#define COMMON_H

#include <time.h>
#include <stdio.h>
#include "fs.h"

#ifdef __GNUC__
#define NULL_TERMINATED __attribute__ ((__sentinel__))
#else
#define NULL_TERMINATED
#endif

#define ARRAYSIZE(a) (sizeof (a) / sizeof ((a)[0]))

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

int   read_line(char **, fs_file);
char *strip_newline(char *);

char *dupe_string(const char *);
char *concat_string(const char *first, ...) NULL_TERMINATED;
char *trunc_string(const char *src, char *dst, int len);

#ifdef strdup
#undef strdup
#endif
#define strdup dupe_string

time_t make_time_from_utc(struct tm *);
const char *date_to_str(time_t);

int  file_exists(const char *);
int  file_rename(const char *, const char *);
void file_copy(FILE *fin, FILE *fout);

int path_is_sep(int);
int path_is_abs(const char *);

char       *base_name(const char *name, const char *suffix);
const char *dir_name(const char *name);

char *path_resolve(const char *ref, const char *rel);

int rand_between(int low, int high);

#endif
