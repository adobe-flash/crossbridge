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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "base_config.h"
#include "glext.h"
#include "vec3.h"
#include "common.h"
#include "fs.h"
#include "dir.h"
#include "array.h"

/*---------------------------------------------------------------------------*/

static const char *pick_data_path(const char *arg_data_path)
{
    static char dir[MAXSTR];
    char *env;

    if (arg_data_path)
        return arg_data_path;

    if ((env = getenv("NEVERBALL_DATA")))
        return env;

    if (path_is_abs(CONFIG_DATA))
        return CONFIG_DATA;

    strncpy(dir, fs_base_dir(), sizeof (dir) - 1);
    strncat(dir, "/",           sizeof (dir) - strlen(dir) - 1);
    strncat(dir, CONFIG_DATA,   sizeof (dir) - strlen(dir) - 1);

    return dir;
}

static const char *pick_home_path(void)
{
    const char *path;

#ifdef _WIN32
    return (path = getenv("APPDATA")) ? path : fs_base_dir();
#else
    return (path = getenv("HOME")) ? path : fs_base_dir();
#endif
}

void config_paths(const char *arg_data_path)
{
    const char *data, *home, *user;

    /*
     * Scan in turn the game data and user directories for archives,
     * adding each archive to the search path.  Archives with names
     * further down the alphabet take precedence.  After each scan,
     * add the directory itself, taking precedence over archives added
     * so far.
     */

    /* Data directory. */

    data = pick_data_path(arg_data_path);

    fs_add_path_with_archives(data);

    /* User directory. */

    home = pick_home_path();
    user = concat_string(home, "/", CONFIG_USER, NULL);

    /* Set up directory for writing, create if needed. */

    if (!fs_set_write_dir(user))
    {
        if (fs_set_write_dir(home) && fs_mkdir(CONFIG_USER))
            fs_set_write_dir(user);
    }

    fs_add_path_with_archives(user);

    free((void *) user);
}

/*---------------------------------------------------------------------------*/
