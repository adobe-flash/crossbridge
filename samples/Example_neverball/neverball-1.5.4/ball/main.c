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

/*---------------------------------------------------------------------------*/

#include <SDL.h>
#include <stdio.h>
#include <string.h>
#include <AS3/AS3.h>

#include "glext.h"
#include "config.h"
#include "video.h"
#include "image.h"
#include "audio.h"
#include "demo.h"
#include "progress.h"
#include "gui.h"
#include "set.h"
#include "tilt.h"
#include "fs.h"
#include "common.h"

#include "st_conf.h"
#include "st_title.h"
#include "st_demo.h"
#include "st_level.h"
#include "st_pause.h"

const char TITLE[] = "Neverball " VERSION;
const char ICON[] = "icon/neverball.png";

/*---------------------------------------------------------------------------*/

static void shot(void)
{
    static char filename[MAXSTR];

    sprintf(filename, "Screenshots/screen%05d.png", config_screenshot());
    image_snap(filename);
}

/*---------------------------------------------------------------------------*/

static void toggle_wire(void)
{
    static int wire = 0;

    if (wire)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
        wire = 0;
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        wire = 1;
    }
}

/*---------------------------------------------------------------------------*/

static int loop(void)
{
    SDL_Event e;
    int d = 1;
    int c;

    /* Process SDL events. */

    while (d && SDL_PollEvent(&e))
    {
        switch (e.type)
        {
        case SDL_QUIT:
            return 0;

        case SDL_MOUSEMOTION:
            st_point(+e.motion.x,
                     -e.motion.y + config_get_d(CONFIG_HEIGHT),
                     +e.motion.xrel,
                     config_get_d(CONFIG_MOUSE_INVERT)
                     ? +e.motion.yrel : -e.motion.yrel);
            break;

        case SDL_MOUSEBUTTONDOWN:
            d = st_click(e.button.button, 1);
            break;

        case SDL_MOUSEBUTTONUP:
            d = st_click(e.button.button, 0);
            break;

        case SDL_KEYDOWN:

            c = e.key.keysym.sym;

            if (config_tst_d(CONFIG_KEY_FORWARD, c))
                st_stick(config_get_d(CONFIG_JOYSTICK_AXIS_Y), -JOY_MAX);

            else if (config_tst_d(CONFIG_KEY_BACKWARD, c))
                st_stick(config_get_d(CONFIG_JOYSTICK_AXIS_Y), +JOY_MAX);

            else if (config_tst_d(CONFIG_KEY_LEFT, c))
                st_stick(config_get_d(CONFIG_JOYSTICK_AXIS_X), -JOY_MAX);

            else if (config_tst_d(CONFIG_KEY_RIGHT, c))
                st_stick(config_get_d(CONFIG_JOYSTICK_AXIS_X), +JOY_MAX);

            else switch (c)
            {
            case SDLK_F10:   shot();                    break;
            case SDLK_F9:    config_tgl_d(CONFIG_FPS);  break;
            case SDLK_F8:    config_tgl_d(CONFIG_NICE); break;

            case SDLK_F7:
                if (config_cheat())
                    toggle_wire();
                break;

            case SDLK_RETURN:
                d = st_buttn(config_get_d(CONFIG_JOYSTICK_BUTTON_A), 1);
                break;
            case SDLK_ESCAPE:
                d = st_buttn(config_get_d(CONFIG_JOYSTICK_BUTTON_EXIT), 1);
                break;

            default:
                if (SDL_EnableUNICODE(-1))
                    d = st_keybd(e.key.keysym.unicode, 1);
                else
                    d = st_keybd(e.key.keysym.sym, 1);
            }

            break;

        case SDL_KEYUP:

            c = e.key.keysym.sym;

            if      (config_tst_d(CONFIG_KEY_FORWARD, c))
                st_stick(config_get_d(CONFIG_JOYSTICK_AXIS_Y), 1);

            else if (config_tst_d(CONFIG_KEY_BACKWARD, c))
                st_stick(config_get_d(CONFIG_JOYSTICK_AXIS_Y), 1);

            else if (config_tst_d(CONFIG_KEY_LEFT, c))
                st_stick(config_get_d(CONFIG_JOYSTICK_AXIS_X), 1);

            else if (config_tst_d(CONFIG_KEY_RIGHT, c))
                st_stick(config_get_d(CONFIG_JOYSTICK_AXIS_X), 1);

            else switch (c)
            {
            case SDLK_RETURN:
                d = st_buttn(config_get_d(CONFIG_JOYSTICK_BUTTON_A), 0);
                break;
            case SDLK_ESCAPE:
                d = st_buttn(config_get_d(CONFIG_JOYSTICK_BUTTON_EXIT), 0);
                break;

            default:
                d = st_keybd(e.key.keysym.sym, 0);
            }

        case SDL_ACTIVEEVENT:
            if (e.active.state == SDL_APPINPUTFOCUS)
                if (e.active.gain == 0 && video_get_grab())
                    goto_pause();
            break;

        case SDL_JOYAXISMOTION:
            st_stick(e.jaxis.axis, e.jaxis.value);
            break;

        case SDL_JOYBUTTONDOWN:
            d = st_buttn(e.jbutton.button, 1);
            break;

        case SDL_JOYBUTTONUP:
            d = st_buttn(e.jbutton.button, 0);
            break;
        }
    }

    /* Process events via the tilt sensor API. */

    if (tilt_stat())
    {
        int b;
        int s;

        st_angle((int) tilt_get_x(),
                 (int) tilt_get_z());

        while (tilt_get_button(&b, &s))
        {
            const int X = config_get_d(CONFIG_JOYSTICK_AXIS_X);
            const int Y = config_get_d(CONFIG_JOYSTICK_AXIS_Y);
            const int L = config_get_d(CONFIG_JOYSTICK_DPAD_L);
            const int R = config_get_d(CONFIG_JOYSTICK_DPAD_R);
            const int U = config_get_d(CONFIG_JOYSTICK_DPAD_U);
            const int D = config_get_d(CONFIG_JOYSTICK_DPAD_D);

            if (b == L || b == R || b == U || b == D)
            {
                static int pad[4] = { 0, 0, 0, 0 };

                /* Track the state of the D-pad buttons. */

                if      (b == L) pad[0] = s;
                else if (b == R) pad[1] = s;
                else if (b == U) pad[2] = s;
                else if (b == D) pad[3] = s;

                /* Convert D-pad button events into joystick axis motion. */

                if      (pad[0] && !pad[1]) st_stick(X, -JOY_MAX);
                else if (pad[1] && !pad[0]) st_stick(X, +JOY_MAX);
                else                        st_stick(X,        1);

                if      (pad[2] && !pad[3]) st_stick(Y, -JOY_MAX);
                else if (pad[3] && !pad[2]) st_stick(Y, +JOY_MAX);
                else                        st_stick(Y,        1);
            }
            else d = st_buttn(b, s);
        }
    }

    return d;
}

/*---------------------------------------------------------------------------*/

static char *data_path = NULL;
static char *demo_path = NULL;

#define usage \
    L_(                                                                   \
        "Usage: %s [options ...]\n"                                       \
        "Options:\n"                                                      \
        "  -h, --help                show this usage message.\n"          \
        "  -v, --version             show version.\n"                     \
        "  -d, --data <dir>          use 'dir' as game data directory.\n" \
        "  -r, --replay <file>       play the replay 'file'.\n"           \
    )

#define argument_error(option) { \
    fprintf(stderr, L_("Option '%s' requires an argument.\n"),  option); \
}

static void parse_args(int argc, char **argv)
{
    int i;

    /* Scan argument list. */

    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help")    == 0)
        {
            printf(usage, argv[0]);
            exit(EXIT_SUCCESS);
        }

        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
        {
            printf("%s\n", VERSION);
            exit(EXIT_SUCCESS);
        }

        if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--data")    == 0)
        {
            if (i + 1 == argc)
            {
                argument_error(argv[i]);
                exit(EXIT_FAILURE);
            }
            data_path = argv[++i];
            continue;
        }

        if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--replay")  == 0)
        {
            if (i + 1 == argc)
            {
                argument_error(argv[i]);
                exit(EXIT_FAILURE);
            }
            demo_path = argv[++i];
            continue;
        }

        /* Assume a single unrecognised argument is a replay name. */

        if (argc == 2)
        {
            demo_path = argv[i];
            break;
        }
    }
}

#undef usage
#undef argument_error

/*---------------------------------------------------------------------------*/

static int is_replay(struct dir_item *item)
{
    return strcmp(item->path + strlen(item->path) - 4, ".nbr") == 0;
}

static int is_score(struct dir_item *item)
{
    return strncmp(item->path, "neverballhs-", sizeof ("neverballhs-") - 1) == 0;
}

static void make_dirs_and_migrate(void)
{
    Array items;
    int i;

    const char *src;
    char *dst;

    if (fs_mkdir("Replays"))
    {
        if ((items = fs_dir_scan("", is_replay)))
        {
            for (i = 0; i < array_len(items); i++)
            {
                src = DIR_ITEM_GET(items, i)->path;
                dst = concat_string("Replays/", src, NULL);
                fs_rename(src, dst);
                free(dst);
            }

            fs_dir_free(items);
        }
    }

    if (fs_mkdir("Scores"))
    {
        if ((items = fs_dir_scan("", is_score)))
        {
            for (i = 0; i < array_len(items); i++)
            {
                src = DIR_ITEM_GET(items, i)->path;
                dst = concat_string("Scores/",
                                    src + sizeof ("neverballhs-") - 1,
                                    ".txt",
                                    NULL);
                fs_rename(src, dst);
                free(dst);
            }

            fs_dir_free(items);
        }
    }

    fs_mkdir("Screenshots");
}

static int t1, t0, uniform;

int main(int argc, char *argv[])
{
    SDL_Joystick *joy = NULL;
    
    if (!fs_init(argv[0]))
    {
        fprintf(stderr, "Failure to initialize virtual file system: %s\n",
                fs_error());
        return 1;
    }

    lang_init("neverball");

    parse_args(argc, argv);

    config_paths(data_path);
    make_dirs_and_migrate();

    /* Initialize SDL system and subsystems */

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) == -1)
    {
        fprintf(stderr, "%s\n", SDL_GetError());
        return 1;
    }

    /* Intitialize the configuration */

    config_init();
    config_load();

    /* unlock all levels by defalut */
    config_set_cheat();

    /* Initialize the joystick. */

    if (config_get_d(CONFIG_JOYSTICK) && SDL_NumJoysticks() > 0)
    {
        joy = SDL_JoystickOpen(config_get_d(CONFIG_JOYSTICK_DEVICE));
        if (joy)
            SDL_JoystickEventState(SDL_ENABLE);
    }

    /* Initialize the audio. */

    audio_init();
    tilt_init();

    /* Initialize the video. */

    if (!video_init(TITLE, ICON))
        return 1;

    init_state(&st_null);

    /* Initialise demo playback. */

    if (demo_path && fs_add_path(dir_name(demo_path)) &&
        progress_replay(base_name(demo_path, NULL)))
    {
        demo_play_goto(1);
        goto_state(&st_demo_play);
    }
    else
        goto_state(&st_title);

    /* Run the main game loop. */

    uniform = config_get_d(CONFIG_UNIFORM);
    t0 = SDL_GetTicks();

#ifdef __AVM2__
	/* 
	Console.as will get a pointer to mainLoopTick() and
	call that every frame instead.
	This way we effectively integrate the game loop with the
	FlashPlayer event loop (so we don't hang in here as main() 
	is called from within the FlashPlayer event loop).
	 */
	
	/*
	Here we throw an exception so that we can break the control
	out of main() without returninng.
	The code in Console.as will take care of calling the 
	factored out game loop code by calling mainLoopTick() periodically.
	*/
    AS3_GoAsync();
#endif

    while (loop())
    {
        t1 = SDL_GetTicks();

        if (uniform)
        {
            /* Step the game uniformly, as configured. */

            int u;

            for (u = 0; u < abs(uniform); ++u)
            {
                st_timer(DT);
                t0 += (int) (DT * 1000);
            }
        }
        else
        {
            /* Step the game state at least up to the current time. */

            while (t1 > t0)
            {
                st_timer(DT);
                t0 += (int) (DT * 1000);
            }
        }

        /* Render. */

        st_paint(0.001f * t0);
        video_swap();

        if (uniform < 0)
            shot();

        if (config_get_d(CONFIG_NICE))
            SDL_Delay(1);
    }

    /* Gracefully close the game */

    if (joy)
        SDL_JoystickClose(joy);

    tilt_free();
    SDL_Quit();

    config_save();

    return 0;
}

#ifdef __AVM2__
void mainLoopTick()
{
	loop();

	t1 = SDL_GetTicks();
		
	if (uniform)
	{
		/* Step the game uniformly, as configured. */
			
		int u;
			
		for (u = 0; u < abs(uniform); ++u)
		{
			st_timer(DT);
			t0 += (int) (DT * 1000);
		}
	}
	else
	{
		/* Step the game state at least up to the current time. */
			
		while (t1 > t0)
		{
			st_timer(DT);
			t0 += (int) (DT * 1000);
		}
	}
		
	/* Render. */
		
	st_paint(0.001f * t0);
	video_swap();
		
	if (uniform < 0)
		shot();
		
	if (config_get_d(CONFIG_NICE))
		SDL_Delay(1);
}
#endif

/*---------------------------------------------------------------------------*/

