/*
 * Copyright (C) 2008 Jānis Rūcis
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

#include <SDL_keyboard.h>
#include <string.h>

#include "lang.h"
#include "common.h"

/*---------------------------------------------------------------------------*/

/* Initial template generated from $SDL/src/events/SDL_keyboard.c */

static const char *table[][2] = {
    /* Translators,
     *
     * This is a mostly-complete list of human-readable SDL key names.  There
     * might not be corresponding names for all these keys in your language,
     * and that's perfectly fine -- in such cases just copy the source string.
     */
    { "backspace", N_("Backspace") },
    { "tab", N_("Tab") },
    { "clear", N_("Clear") },
    { "return", N_("Return") },
    { "pause", N_("Pause") },
    { "escape", N_("Escape") },
    { "space", N_("Space") },
    { "delete", N_("Delete") },
    { "enter", N_("Enter") },
    { "equals", N_("Equals") },
    { "up", N_("Up") },
    { "down", N_("Down") },
    { "right", N_("Right") },
    { "left", N_("Left") },
    { "down", N_("Down") },
    { "insert", N_("Insert") },
    { "home", N_("Home") },
    { "end", N_("End") },
    { "page up", N_("Page Up") },
    { "page down", N_("Page Down") },
    { "f1", N_("F1") },
    { "f2", N_("F2") },
    { "f3", N_("F3") },
    { "f4", N_("F4") },
    { "f5", N_("F5") },
    { "f6", N_("F6") },
    { "f7", N_("F7") },
    { "f8", N_("F8") },
    { "f9", N_("F9") },
    { "f10", N_("F10") },
    { "f11", N_("F11") },
    { "f12", N_("F12") },
    { "f13", N_("F13") },
    { "f14", N_("F14") },
    { "f15", N_("F15") },
    { "numlock", N_("Num Lock") },
    { "caps lock", N_("Caps Lock") },
    { "scroll lock", N_("Scroll Lock") },
    { "right shift", N_("Right Shift") },
    { "left shift", N_("Left Shift") },
    { "right ctrl", N_("Right CTRL") },
    { "left ctrl", N_("Left CTRL") },
    { "right alt", N_("Right Alt") },
    { "left alt", N_("Left Alt") },
    { "right meta", N_("Right Meta") },
    { "left meta", N_("Left Meta") },
    { "left super", N_("Left Super") },
    { "right super", N_("Right Super") },
    { "alt gr", N_("Alt Gr") },
    { "compose", N_("Compose") },
    { "help", N_("Help") },
    { "print screen", N_("Print Screen") },
    { "sys req", N_("Sys Req") },
    { "break", N_("Break") },
    { "menu", N_("Menu") },
    { "power", N_("Power") },
    { "euro", N_("Euro") },
    { "undo", N_("Undo") },
};

/*---------------------------------------------------------------------------*/

const char *pretty_keyname(SDLKey key)
{
    const char *ugly_keyname;
    int i;

    if ((ugly_keyname = SDL_GetKeyName(key)) == NULL)
        return NULL;

    for (i = 0; i < ARRAYSIZE(table); i++)
        if (strcmp(table[i][0], ugly_keyname) == 0)
            return _(table[i][1]);

    return ugly_keyname;
}

/*---------------------------------------------------------------------------*/
