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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <stdarg.h>
#include <assert.h>

#include "common.h"
#include "fs.h"

#define MAXSTR 256

/*---------------------------------------------------------------------------*/

int read_line(char **dst, fs_file fin)
{
    char buff[MAXSTR];

    char *line, *new;
    size_t len0, len1;

    line = NULL;

    while (fs_gets(buff, sizeof (buff), fin))
    {
        /* Append to data read so far. */

        if (line)
        {
            new  = concat_string(line, buff, NULL);
            free(line);
            line = new;
        }
        else
        {
            line = strdup(buff);
        }

        /* Strip newline, if any. */

        len0 = strlen(line);
        strip_newline(line);
        len1 = strlen(line);

        if (len1 != len0)
        {
            /* We hit a newline, clean up and break. */
            line = realloc(line, len1 + 1);
            break;
        }
    }

    return (*dst = line) ? 1 : 0;
}

char *strip_newline(char *str)
{
    char *c = str + strlen(str) - 1;

    while (c >= str && (*c == '\n' || *c =='\r'))
        *c-- = '\0';

    return str;
}

char *dupe_string(const char *src)
{
    char *dst = NULL;

    if (src && (dst = malloc(strlen(src) + 1)))
        strcpy(dst, src);

    return dst;
}

char *concat_string(const char *first, ...)
{
    char *full;

    if ((full = strdup(first)))
    {
        const char *part;
        va_list ap;

        va_start(ap, first);

        while ((part = va_arg(ap, const char *)))
        {
            char *new;

            if ((new = realloc(full, strlen(full) + strlen(part) + 1)))
            {
                full = new;
                strcat(full, part);
            }
            else
            {
                free(full);
                full = NULL;
                break;
            }
        }

        va_end(ap);
    }

    return full;
}

char *trunc_string(const char *src, char *dst, int len)
{
    static const char ell[] = "...";

    assert(len > sizeof (ell));

    if (dst[len - 1] = '\0', strncpy(dst, src, len), dst[len - 1] != '\0')
        strcpy(dst + len - sizeof (ell), ell);

    return dst;
}

time_t make_time_from_utc(struct tm *tm)
{
    struct tm local, *utc;
    time_t t;

    t = mktime(tm);

    local = *localtime(&t);
    utc   =  gmtime(&t);

    local.tm_year += local.tm_year - utc->tm_year;
    local.tm_mon  += local.tm_mon  - utc->tm_mon ;
    local.tm_mday += local.tm_mday - utc->tm_mday;
    local.tm_hour += local.tm_hour - utc->tm_hour;
    local.tm_min  += local.tm_min  - utc->tm_min ;
    local.tm_sec  += local.tm_sec  - utc->tm_sec ;

    return mktime(&local);
}

const char *date_to_str(time_t i)
{
    static char str[sizeof ("YYYY-mm-dd HH:MM:SS")];
    strftime(str, sizeof (str), "%Y-%m-%d %H:%M:%S", localtime(&i));
    return str;
}

int file_exists(const char *name)
{
    FILE *fp;

    if ((fp = fopen(name, "r")))
    {
        fclose(fp);
        return 1;
    }
    return 0;
}

int file_rename(const char *src, const char *dst)
{
#ifdef _WIN32
    if (file_exists(dst))
        remove(dst);
#endif
    return rename(src, dst);
}

void file_copy(FILE *fin, FILE *fout)
{
    char   buff[MAXSTR];
    size_t size;

    while ((size = fread(buff, 1, sizeof (buff), fin)) > 0)
        fwrite(buff, 1, size, fout);
}

/*---------------------------------------------------------------------------*/

int path_is_sep(int c)
{
#ifdef _WIN32
    return c == '/' || c == '\\';
#else
    return c == '/';
#endif
}

int path_is_abs(const char *path)
{
    if (path_is_sep(path[0]))
        return 1;

#ifdef _WIN32
    if (isalpha(path[0]) && path[1] == ':' && path_is_sep(path[2]))
        return 1;
#endif

    return 0;
}

static char *path_last_sep(const char *path)
{
    char *sep;

    sep = strrchr(path, '/');

#ifdef _WIN32
    if (!sep)
    {
        sep = strrchr(path, '\\');
    }
    else
    {
        char *tmp;

        if ((tmp = strrchr(sep, '\\')))
            sep = tmp;
    }
#endif

    return sep;
}

char *base_name(const char *name, const char *suffix)
{
    static char buf[MAXSTR];
    char *base;

    if (!name)
        return NULL;

    /* Remove the directory part. */

    base = path_last_sep(name);

    strncpy(buf, base ? base + 1 : name, sizeof (buf) - 1);

    /* Remove the suffix. */

    if (suffix)
    {
        int l = strlen(buf) - strlen(suffix);

        if (l >= 0 && strcmp(buf + l, suffix) == 0)
            buf[l] = '\0';
    }

    return buf;
}

const char *dir_name(const char *name)
{
    static char buff[MAXSTR];

    char *sep;

    strncpy(buff, name, sizeof (buff) - 1);

    if ((sep = path_last_sep(buff)))
    {
        if (sep == buff)
            return "/";

        *sep = '\0';

        return buff;
    }

    return ".";
}

/*
 * Given a path to a file REF and another path REL relative to REF,
 * construct and return a new path that can be used to refer to REL
 * directly.
 */
char *path_resolve(const char *ref, const char *rel)
{
    static char new[MAXSTR * 2];

    if (path_is_abs(rel))
    {
        strncpy(new, rel, sizeof (new) - 1);
        return new;
    }

    strncpy(new, dir_name(ref), sizeof (new) - 1);
    strncat(new, "/",           sizeof (new) - strlen(new) - 1);
    strncat(new, rel,           sizeof (new) - strlen(new) - 1);

    return new;
}

/*---------------------------------------------------------------------------*/

int rand_between(int low, int high)
{
    return low + rand() / (RAND_MAX / (high - low + 1) + 1);
}

/*---------------------------------------------------------------------------*/
