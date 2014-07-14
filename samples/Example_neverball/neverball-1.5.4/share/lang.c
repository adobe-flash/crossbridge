/*
 * Copyright (C) 2006 Jean Privat
 * Part of the Neverball Project http://icculus.org/neverball/
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
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "lang.h"
#include "common.h"
#include "base_config.h"
#include "fs.h"

/*---------------------------------------------------------------------------*/

#define DEFAULT_CODESET "UTF-8"

/*---------------------------------------------------------------------------*/

void lang_init(const char *domain)
{
#if ENABLE_NLS
    char *dir = strdup(getenv("NEVERBALL_LOCALE"));

    if (!dir)
    {
        if (path_is_abs(CONFIG_LOCALE))
            dir = strdup(CONFIG_LOCALE);
        else
            dir = concat_string(fs_base_dir(), "/", CONFIG_LOCALE, NULL);
    }

    errno = 0;

    if (!setlocale(LC_ALL, ""))
    {
        fprintf(stderr, "Failed to set LC_ALL to native locale: %s\n",
                errno ? strerror(errno) : "Unknown error");
    }

    /* The C locale is guaranteed (sort of) to be available. */

    setlocale(LC_NUMERIC, "C");

    bindtextdomain(domain, dir);
    bind_textdomain_codeset(domain, DEFAULT_CODESET);
    textdomain(domain);

    free(dir);
#else
    return;
#endif
}

const char *sgettext(const char *msgid)
{
#if ENABLE_NLS
    const char *msgval = gettext(msgid);
#else
    const char *msgval = msgid;
#endif

    if (msgval == msgid)
    {
        if ((msgval = strrchr(msgid, '^')))
            msgval++;
        else msgval = msgid;
    }
    return msgval;
}

const char *get_local_text(const char *msgid)
{
#if ENABLE_NLS
    char *msgstr, *domain = textdomain(NULL);

    bind_textdomain_codeset(domain, "");
    msgstr = gettext(msgid);
    bind_textdomain_codeset(domain, DEFAULT_CODESET);

    return msgstr;
#else
    return msgid;
#endif
}

/*---------------------------------------------------------------------------*/
