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

#include <SDL.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "config.h"
#include "common.h"
#include "fs.h"

/*---------------------------------------------------------------------------*/

/* Integer options. */

int CONFIG_FULLSCREEN;
int CONFIG_WIDTH;
int CONFIG_HEIGHT;
int CONFIG_STEREO;
int CONFIG_CAMERA;
int CONFIG_TEXTURES;
int CONFIG_GEOMETRY;
int CONFIG_REFLECTION;
int CONFIG_MULTISAMPLE;
int CONFIG_MIPMAP;
int CONFIG_ANISO;
int CONFIG_BACKGROUND;
int CONFIG_SHADOW;
int CONFIG_AUDIO_BUFF;
int CONFIG_MOUSE_SENSE;
int CONFIG_MOUSE_INVERT;
int CONFIG_VSYNC;
int CONFIG_MOUSE_CAMERA_1;
int CONFIG_MOUSE_CAMERA_2;
int CONFIG_MOUSE_CAMERA_3;
int CONFIG_MOUSE_CAMERA_TOGGLE;
int CONFIG_MOUSE_CAMERA_L;
int CONFIG_MOUSE_CAMERA_R;
int CONFIG_NICE;
int CONFIG_FPS;
int CONFIG_SOUND_VOLUME;
int CONFIG_MUSIC_VOLUME;
int CONFIG_JOYSTICK;
int CONFIG_JOYSTICK_DEVICE;
int CONFIG_JOYSTICK_AXIS_X;
int CONFIG_JOYSTICK_AXIS_Y;
int CONFIG_JOYSTICK_AXIS_U;
int CONFIG_JOYSTICK_BUTTON_A;
int CONFIG_JOYSTICK_BUTTON_B;
int CONFIG_JOYSTICK_BUTTON_R;
int CONFIG_JOYSTICK_BUTTON_L;
int CONFIG_JOYSTICK_BUTTON_EXIT;
int CONFIG_JOYSTICK_CAMERA_1;
int CONFIG_JOYSTICK_CAMERA_2;
int CONFIG_JOYSTICK_CAMERA_3;
int CONFIG_JOYSTICK_DPAD_L;
int CONFIG_JOYSTICK_DPAD_R;
int CONFIG_JOYSTICK_DPAD_U;
int CONFIG_JOYSTICK_DPAD_D;
int CONFIG_JOYSTICK_CAMERA_TOGGLE;
int CONFIG_JOYSTICK_ROTATE_FAST;
int CONFIG_KEY_CAMERA_1;
int CONFIG_KEY_CAMERA_2;
int CONFIG_KEY_CAMERA_3;
int CONFIG_KEY_CAMERA_TOGGLE;
int CONFIG_KEY_CAMERA_R;
int CONFIG_KEY_CAMERA_L;
int CONFIG_VIEW_FOV;
int CONFIG_VIEW_DP;
int CONFIG_VIEW_DC;
int CONFIG_VIEW_DZ;
int CONFIG_ROTATE_FAST;
int CONFIG_ROTATE_SLOW;
int CONFIG_KEY_FORWARD;
int CONFIG_KEY_BACKWARD;
int CONFIG_KEY_LEFT;
int CONFIG_KEY_RIGHT;
int CONFIG_KEY_PAUSE;
int CONFIG_KEY_RESTART;
int CONFIG_KEY_SCORE_NEXT;
int CONFIG_KEY_ROTATE_FAST;
int CONFIG_CHEAT;
int CONFIG_STATS;
int CONFIG_UNIFORM;
int CONFIG_SCREENSHOT;
int CONFIG_LOCK_GOALS;

/* String options. */

int CONFIG_PLAYER;
int CONFIG_BALL_FILE;
int CONFIG_WIIMOTE_ADDR;
int CONFIG_REPLAY_NAME;

/*---------------------------------------------------------------------------*/

static struct
{
    int        *sym;
    const char *name;
    const int   def;
    int         cur;
} option_d[] = {
    { &CONFIG_FULLSCREEN,   "fullscreen",   0 },
    { &CONFIG_WIDTH,        "width",        800 },
    { &CONFIG_HEIGHT,       "height",       600 },
    { &CONFIG_STEREO,       "stereo",       0 },
    { &CONFIG_CAMERA,       "camera",       0 },
    { &CONFIG_TEXTURES,     "textures",     1 },
    { &CONFIG_GEOMETRY,     "geometry",     1 },
    { &CONFIG_REFLECTION,   "reflection",   1 },
    { &CONFIG_MULTISAMPLE,  "multisample",  0 },
    { &CONFIG_MIPMAP,       "mipmap",       0 },
    { &CONFIG_ANISO,        "aniso",        0 },
    { &CONFIG_BACKGROUND,   "background",   1 },
    { &CONFIG_SHADOW,       "shadow",       1 },
    { &CONFIG_AUDIO_BUFF,   "audio_buff",   AUDIO_BUFF_HI },
    { &CONFIG_MOUSE_SENSE,  "mouse_sense",  300 },
    { &CONFIG_MOUSE_INVERT, "mouse_invert", 0 },
    { &CONFIG_VSYNC,        "vsync",        1 },

    { &CONFIG_MOUSE_CAMERA_1,      "mouse_camera_1",      0 },
    { &CONFIG_MOUSE_CAMERA_2,      "mouse_camera_2",      0 },
    { &CONFIG_MOUSE_CAMERA_3,      "mouse_camera_3",      0 },
    { &CONFIG_MOUSE_CAMERA_TOGGLE, "mouse_camera_toggle", SDL_BUTTON_MIDDLE },
    { &CONFIG_MOUSE_CAMERA_L,      "mouse_camera_l",      SDL_BUTTON_LEFT },
    { &CONFIG_MOUSE_CAMERA_R,      "mouse_camera_r",      SDL_BUTTON_RIGHT },

    { &CONFIG_NICE,         "nice",         0 },
    { &CONFIG_FPS,          "fps",          0 },
    { &CONFIG_SOUND_VOLUME, "sound_volume", 10 },
    { &CONFIG_MUSIC_VOLUME, "music_volume", 6 },

    { &CONFIG_JOYSTICK,               "joystick",               1 },
    { &CONFIG_JOYSTICK_DEVICE,        "joystick_device",        0 },
    { &CONFIG_JOYSTICK_AXIS_X,        "joystick_axis_x",        0 },
    { &CONFIG_JOYSTICK_AXIS_Y,        "joystick_axis_y",        1 },
    { &CONFIG_JOYSTICK_AXIS_U,        "joystick_axis_u",        2 },
    { &CONFIG_JOYSTICK_BUTTON_A,      "joystick_button_a",      0 },
    { &CONFIG_JOYSTICK_BUTTON_B,      "joystick_button_b",      1 },
    { &CONFIG_JOYSTICK_BUTTON_R,      "joystick_button_r",      2 },
    { &CONFIG_JOYSTICK_BUTTON_L,      "joystick_button_l",      3 },
    { &CONFIG_JOYSTICK_BUTTON_EXIT,   "joystick_button_exit",   4 },
    { &CONFIG_JOYSTICK_CAMERA_1,      "joystick_camera_1",      5 },
    { &CONFIG_JOYSTICK_CAMERA_2,      "joystick_camera_2",      6 },
    { &CONFIG_JOYSTICK_CAMERA_3,      "joystick_camera_3",      7 },
    { &CONFIG_JOYSTICK_DPAD_L,        "joystick_dpad_l",        8 },
    { &CONFIG_JOYSTICK_DPAD_R,        "joystick_dpad_r",        9 },
    { &CONFIG_JOYSTICK_DPAD_U,        "joystick_dpad_u",        10 },
    { &CONFIG_JOYSTICK_DPAD_D,        "joystick_dpad_d",        11 },
    { &CONFIG_JOYSTICK_CAMERA_TOGGLE, "joystick_camera_toggle", 12 },
    { &CONFIG_JOYSTICK_ROTATE_FAST,   "joystick_rotate_fast",   13 },

    { &CONFIG_KEY_CAMERA_1,      "key_camera_1",      SDLK_F1 },
    { &CONFIG_KEY_CAMERA_2,      "key_camera_2",      SDLK_F2 },
    { &CONFIG_KEY_CAMERA_3,      "key_camera_3",      SDLK_F3 },
    { &CONFIG_KEY_CAMERA_TOGGLE, "key_camera_toggle", SDLK_e },
    { &CONFIG_KEY_CAMERA_R,      "key_camera_r",      SDLK_d },
    { &CONFIG_KEY_CAMERA_L,      "key_camera_l",      SDLK_s },
    { &CONFIG_VIEW_FOV,          "view_fov",          50 },
    { &CONFIG_VIEW_DP,           "view_dp",           75 },
    { &CONFIG_VIEW_DC,           "view_dc",           25 },
    { &CONFIG_VIEW_DZ,           "view_dz",           200 },
    { &CONFIG_ROTATE_FAST,       "rotate_fast",       300 },
    { &CONFIG_ROTATE_SLOW,       "rotate_slow",       150 },
    { &CONFIG_KEY_FORWARD,       "key_forward",       SDLK_UP },
    { &CONFIG_KEY_BACKWARD,      "key_backward",      SDLK_DOWN },
    { &CONFIG_KEY_LEFT,          "key_left",          SDLK_LEFT },
    { &CONFIG_KEY_RIGHT,         "key_right",         SDLK_RIGHT },
    { &CONFIG_KEY_PAUSE,         "key_pause",         SDLK_ESCAPE },
    { &CONFIG_KEY_RESTART,       "key_restart",       SDLK_r },
    { &CONFIG_KEY_SCORE_NEXT,    "key_score_next",    SDLK_TAB },
    { &CONFIG_KEY_ROTATE_FAST,   "key_rotate_fast",   SDLK_LSHIFT },
    { &CONFIG_CHEAT,             "play",             0 },
    { &CONFIG_STATS,             "stats",             0 },
    { &CONFIG_UNIFORM,           "uniform",           0 },
    { &CONFIG_SCREENSHOT,        "screenshot",        0 },
    { &CONFIG_LOCK_GOALS,        "lock_goals",        0 }
};

static struct
{
    int        *sym;
    const char *name;
    const char *def;
    char       *cur;
} option_s[] = {
    { &CONFIG_PLAYER,       "player",       "" },
    { &CONFIG_BALL_FILE,    "ball_file",    "ball/basic-ball/basic-ball" },
    { &CONFIG_WIIMOTE_ADDR, "wiimote_addr", "" },
    { &CONFIG_REPLAY_NAME,  "replay_name",  "%s-%l" }
};

static int dirty = 0;

/*---------------------------------------------------------------------------*/

static void config_key(const char *s, int i)
{
    int c;

    config_set_d(i, option_d[i].def);

    for (c = 0; c < SDLK_LAST; c++)
        if (strcmp(s, SDL_GetKeyName((SDLKey) c)) == 0)
        {
            config_set_d(i, c);
            break;
        }
}

/*---------------------------------------------------------------------------*/

static void config_mouse(const char *s, int i)
{
    if      (strcmp(s, "none") == 0)
        config_set_d(i, 0);
    else if (strcmp(s, "left") == 0)
        config_set_d(i, SDL_BUTTON_LEFT);
    else if (strcmp(s, "right") == 0)
        config_set_d(i, SDL_BUTTON_RIGHT);
    else if (strcmp(s, "wheelup") == 0)
        config_set_d(i, SDL_BUTTON_WHEELUP);
    else if (strcmp(s, "middle") == 0)
        config_set_d(i, SDL_BUTTON_MIDDLE);
    else if (strcmp(s, "wheeldown") == 0)
        config_set_d(i, SDL_BUTTON_WHEELDOWN);
    else
        config_set_d(i, atoi(s));
}

static const char *config_mouse_name(int b)
{
    static char buff[sizeof ("256")];

    sprintf(buff, "%d", b);

    switch (b)
    {
    case 0:                    return "none";      break;
    case SDL_BUTTON_LEFT:      return "left";      break;
    case SDL_BUTTON_RIGHT:     return "right";     break;
    case SDL_BUTTON_WHEELUP:   return "wheelup";   break;
    case SDL_BUTTON_MIDDLE:    return "middle";    break;
    case SDL_BUTTON_WHEELDOWN: return "wheeldown"; break;
    default:                   return buff;        break;
    }
}

/*---------------------------------------------------------------------------*/

void config_init(void)
{
    int i;

    /*
     * Store index of each option in its associated config symbol and
     * initialise current values with defaults.
     */

    for (i = 0; i < ARRAYSIZE(option_d); i++)
    {
        *option_d[i].sym = i;
        config_set_d(i, option_d[i].def);
    }

    for (i = 0; i < ARRAYSIZE(option_s); i++)
    {
        *option_s[i].sym = i;
        config_set_s(i, option_s[i].def);
    }
}

/*
 * Scan a NUL-terminated string LINE according to the format
 * '^<space>?<key><space><value>$' and store pointers to the start of key and
 * value at DST_KEY and DST_VAL, respectively.  No memory is allocated to store
 * the strings;  instead, the memory pointed to by LINE modified in-place as
 * needed.
 *
 * Return 1 if LINE matches the format, return 0 otherwise.
 */

static int scan_key_and_value(char **dst_key, char **dst_val, char *line)
{
    if (line)
    {
        char *key, *val, *space;

        for (key = line; *key && isspace(*key); key++);

        if (*key)
        {
            if (dst_key)
                *dst_key = key;
        }
        else
            return 0;

        for (space = key; *space && !isspace(*space); space++);

        if (*space)
        {
            /* NUL-terminate the key, if necessary. */

            if (dst_key)
            {
                *space = '\0';
                space++;
            }
        }
        else
            return 0;

        for (val = space; *val && isspace(*val); val++);

        if (*val)
        {
            if (dst_val)
                *dst_val = val;
        }
        else
            return 0;

        return 1;
    }

    return 0;
}

void config_load(void)
{
    fs_file fh;

    if ((fh = fs_open(USER_CONFIG_FILE, "r")))
    {
        char *line, *key, *val;

        while (read_line(&line, fh))
        {
            if (scan_key_and_value(&key, &val, line))
            {
                int i;

                /* Look up an integer option by that name. */

                for (i = 0; i < ARRAYSIZE(option_d); i++)
                {
                    if (strcmp(key, option_d[i].name) == 0)
                    {
                        /* Translate some strings to integers. */

                        if (i == CONFIG_MOUSE_CAMERA_1      ||
                            i == CONFIG_MOUSE_CAMERA_2      ||
                            i == CONFIG_MOUSE_CAMERA_3      ||
                            i == CONFIG_MOUSE_CAMERA_TOGGLE ||
                            i == CONFIG_MOUSE_CAMERA_L      ||
                            i == CONFIG_MOUSE_CAMERA_R)
                        {
                            config_mouse(val, i);
                        }
                        else if (i == CONFIG_KEY_FORWARD       ||
                                 i == CONFIG_KEY_BACKWARD      ||
                                 i == CONFIG_KEY_LEFT          ||
                                 i == CONFIG_KEY_RIGHT         ||
                                 i == CONFIG_KEY_CAMERA_1      ||
                                 i == CONFIG_KEY_CAMERA_2      ||
                                 i == CONFIG_KEY_CAMERA_3      ||
                                 i == CONFIG_KEY_CAMERA_TOGGLE ||
                                 i == CONFIG_KEY_CAMERA_R      ||
                                 i == CONFIG_KEY_CAMERA_L      ||
                                 i == CONFIG_KEY_PAUSE         ||
                                 i == CONFIG_KEY_RESTART       ||
                                 i == CONFIG_KEY_SCORE_NEXT    ||
                                 i == CONFIG_KEY_ROTATE_FAST)
                        {
                            config_key(val, i);
                        }
                        else
                            config_set_d(i, atoi(val));

                        /* Stop looking. */

                        break;
                    }
                }

                /* Look up a string option by that name.*/

                for (i = 0; i < ARRAYSIZE(option_s); i++)
                {
                    if (strcmp(key, option_s[i].name) == 0)
                    {
                        config_set_s(i, val);
                        break;
                    }
                }
            }
            free(line);
        }
        fs_close(fh);

        dirty = 0;
    }
}

void config_save(void)
{
    fs_file fh;

    if (dirty && (fh = fs_open(USER_CONFIG_FILE, "w")))
    {
        int i;

        /* Write out integer options. */

        for (i = 0; i < ARRAYSIZE(option_d); i++)
        {
            const char *s = NULL;

            /* Translate some integers to strings. */

            if (i == CONFIG_MOUSE_CAMERA_1      ||
                i == CONFIG_MOUSE_CAMERA_2      ||
                i == CONFIG_MOUSE_CAMERA_3      ||
                i == CONFIG_MOUSE_CAMERA_TOGGLE ||
                i == CONFIG_MOUSE_CAMERA_L      ||
                i == CONFIG_MOUSE_CAMERA_R)
            {
                s = config_mouse_name(option_d[i].cur);
            }
            else if (i == CONFIG_KEY_FORWARD       ||
                     i == CONFIG_KEY_BACKWARD      ||
                     i == CONFIG_KEY_LEFT          ||
                     i == CONFIG_KEY_RIGHT         ||
                     i == CONFIG_KEY_CAMERA_1      ||
                     i == CONFIG_KEY_CAMERA_2      ||
                     i == CONFIG_KEY_CAMERA_3      ||
                     i == CONFIG_KEY_CAMERA_TOGGLE ||
                     i == CONFIG_KEY_CAMERA_R      ||
                     i == CONFIG_KEY_CAMERA_L      ||
                     i == CONFIG_KEY_PAUSE         ||
                     i == CONFIG_KEY_RESTART       ||
                     i == CONFIG_KEY_SCORE_NEXT    ||
                     i == CONFIG_KEY_ROTATE_FAST)
            {
                s = SDL_GetKeyName((SDLKey) option_d[i].cur);
            }
            else if (i == CONFIG_CHEAT)
            {
                if (!config_cheat())
                    continue;
            }

            if (s)
                fs_printf(fh, "%-25s %s\n", option_d[i].name, s);
            else
                fs_printf(fh, "%-25s %d\n", option_d[i].name, option_d[i].cur);
        }

        /* Write out string options. */

        for (i = 0; i < ARRAYSIZE(option_s); i++)
        {
            if (option_s[i].cur && *option_s[i].cur)
                fs_printf(fh, "%-25s %s\n", option_s[i].name, option_s[i].cur);
        }

        fs_close(fh);
    }

    dirty = 0;
}

/*---------------------------------------------------------------------------*/

void config_set_d(int i, int d)
{
    option_d[i].cur = d;
    dirty = 1;
}

void config_tgl_d(int i)
{
    option_d[i].cur = (option_d[i].cur ? 0 : 1);
    dirty = 1;
}

int config_tst_d(int i, int d)
{
    return (option_d[i].cur == d) ? 1 : 0;
}

int config_get_d(int i)
{
    return option_d[i].cur;
}

/*---------------------------------------------------------------------------*/

void config_set_s(int i, const char *src)
{
    if (option_s[i].cur)
        free(option_s[i].cur);

    option_s[i].cur = strdup(src);

    dirty = 1;
}

const char *config_get_s(int i)
{
    return option_s[i].cur;
}

/*---------------------------------------------------------------------------*/

int config_cheat(void)
{
    return config_get_d(CONFIG_CHEAT);
}

void config_set_cheat(void)
{
    config_set_d(CONFIG_CHEAT, 1);
}

void config_clr_cheat(void)
{
    config_set_d(CONFIG_CHEAT, 0);
}

/*---------------------------------------------------------------------------*/

int config_screenshot(void)
{
    return ++option_d[CONFIG_SCREENSHOT].cur;
}

/*---------------------------------------------------------------------------*/
