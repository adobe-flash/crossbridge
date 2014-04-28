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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL_endian.h>

#include "fs.h"

/*---------------------------------------------------------------------------*/

void put_float(fs_file fout, const float *f)
{
    const unsigned char *p = (const unsigned char *) f;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    fs_putc((int) p[3], fout);
    fs_putc((int) p[2], fout);
    fs_putc((int) p[1], fout);
    fs_putc((int) p[0], fout);
#else
    fs_putc((int) p[0], fout);
    fs_putc((int) p[1], fout);
    fs_putc((int) p[2], fout);
    fs_putc((int) p[3], fout);
#endif
}

void put_index(fs_file fout, const int *i)
{
    const unsigned char *p = (const unsigned char *) i;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    fs_putc((int) p[3], fout);
    fs_putc((int) p[2], fout);
    fs_putc((int) p[1], fout);
    fs_putc((int) p[0], fout);
#else
    fs_putc((int) p[0], fout);
    fs_putc((int) p[1], fout);
    fs_putc((int) p[2], fout);
    fs_putc((int) p[3], fout);
#endif
}

void put_short(fs_file fout, const short *s)
{
    const unsigned char *p = (const unsigned char *) s;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    fs_putc((int) p[1], fout);
    fs_putc((int) p[0], fout);
#else
    fs_putc((int) p[0], fout);
    fs_putc((int) p[1], fout);
#endif
}

void put_array(fs_file fout, const float *v, size_t n)
{
    size_t i;

    for (i = 0; i < n; i++)
        put_float(fout, v + i);
}

/*---------------------------------------------------------------------------*/

void get_float(fs_file fin, float *f)
{
    unsigned char *p = (unsigned char *) f;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    p[3] = (unsigned char) fs_getc(fin);
    p[2] = (unsigned char) fs_getc(fin);
    p[1] = (unsigned char) fs_getc(fin);
    p[0] = (unsigned char) fs_getc(fin);
#else
    p[0] = (unsigned char) fs_getc(fin);
    p[1] = (unsigned char) fs_getc(fin);
    p[2] = (unsigned char) fs_getc(fin);
    p[3] = (unsigned char) fs_getc(fin);
#endif
}

void get_index(fs_file fin, int *i)
{
    unsigned char *p = (unsigned char *) i;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    p[3] = (unsigned char) fs_getc(fin);
    p[2] = (unsigned char) fs_getc(fin);
    p[1] = (unsigned char) fs_getc(fin);
    p[0] = (unsigned char) fs_getc(fin);
#else
    p[0] = (unsigned char) fs_getc(fin);
    p[1] = (unsigned char) fs_getc(fin);
    p[2] = (unsigned char) fs_getc(fin);
    p[3] = (unsigned char) fs_getc(fin);
#endif
}

void get_short(fs_file fin, short *s)
{
    unsigned char *p = (unsigned char *) s;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    p[1] = (unsigned char) fs_getc(fin);
    p[0] = (unsigned char) fs_getc(fin);
#else
    p[0] = (unsigned char) fs_getc(fin);
    p[1] = (unsigned char) fs_getc(fin);
#endif
}

void get_array(fs_file fin, float *v, size_t n)
{
    size_t i;

    for (i = 0; i < n; i++)
        get_float(fin, v + i);
}

/*---------------------------------------------------------------------------*/

void put_string(fs_file fout, const char *s)
{
    fs_puts(s, fout);
    fs_putc('\0', fout);
}

void get_string(fs_file fin, char *s, int max)
{
    int c;

    while ((c = fs_getc(fin)) >= 0)
    {
        if (max > 0)
        {
            *s++ = c;
            max--;

            /* Terminate the string, but keep reading until NUL. */

            if (max == 0)
                *(s - 1) = 0;
        }

        if (c == 0)
            break;
    }
}

/*---------------------------------------------------------------------------*/
