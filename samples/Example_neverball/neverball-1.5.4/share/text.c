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

#include <string.h>
#include "text.h"

/*---------------------------------------------------------------------------*/

int text_add_char(Uint32 unicode, char *string, int maxbytes)
{
    size_t pos = strlen(string);
    int l;

    if      (unicode < 0x80)    l = 1;
    else if (unicode < 0x0800)  l = 2;
    else if (unicode < 0x10000) l = 3;
    else                        l = 4;

    if (pos + l >= maxbytes)
        return 0;

    if (unicode < 0x80)
        string[pos++] = (char) unicode;
    else if (unicode < 0x0800)
    {
        string[pos++] = (char) ((unicode >> 6) | 0xC0);
        string[pos++] = (char) ((unicode & 0x3F) | 0x80);
    }
    else if (unicode < 0x10000)
    {
        string[pos++] = (char) ((unicode >> 12) | 0xE0);
        string[pos++] = (char) (((unicode >> 6) & 0x3F) | 0x80);
        string[pos++] = (char) ((unicode & 0x3F) | 0x80);
    }
    else
    {
        string[pos++] = (char) ((unicode >> 18) | 0xF0);
        string[pos++] = (char) (((unicode >> 12) & 0x3F) | 0x80);
        string[pos++] = (char) (((unicode >> 6) & 0x3F) | 0x80);
        string[pos++] = (char) ((unicode & 0x3F) | 0x80);
    }

    string[pos++] = 0;

    return l;
}

int text_del_char(char *string)
{
    int pos = (int) strlen(string) - 1;

    while (pos >= 0 && ((string[pos] & 0xC0) == 0x80))
        string[pos--] = 0;

    if (pos >= 0)
    {
        string[pos] = 0;
        return 1;
    }

    return 0;
}

int text_length(const char *string)
{
    int result = 0;

    while (*string != '\0')
        if ((*string++ & 0xC0) != 0x80)
            result++;

    return result;
}

/*---------------------------------------------------------------------------*/
