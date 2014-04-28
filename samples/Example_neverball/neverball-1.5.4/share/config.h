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

#ifndef CONFIG_H
#define CONFIG_H

/*
 * This file contains:
 * 1- some global config methods (stored in a config file)
 * 2- some SDL based functions
 *
 * If you're looking for constants, you should also see base_config.h
 */

#include <SDL.h>

#include "base_config.h"
#include "lang.h"

/*---------------------------------------------------------------------------*/

/* Integer options. */

extern int CONFIG_FULLSCREEN;
extern int CONFIG_WIDTH;
extern int CONFIG_HEIGHT;
extern int CONFIG_STEREO;
extern int CONFIG_CAMERA;
extern int CONFIG_TEXTURES;
extern int CONFIG_GEOMETRY;
extern int CONFIG_REFLECTION;
extern int CONFIG_MULTISAMPLE;
extern int CONFIG_MIPMAP;
extern int CONFIG_ANISO;
extern int CONFIG_BACKGROUND;
extern int CONFIG_SHADOW;
extern int CONFIG_AUDIO_BUFF;
extern int CONFIG_MOUSE_SENSE;
extern int CONFIG_MOUSE_INVERT;
extern int CONFIG_VSYNC;
extern int CONFIG_MOUSE_CAMERA_1;
extern int CONFIG_MOUSE_CAMERA_2;
extern int CONFIG_MOUSE_CAMERA_3;
extern int CONFIG_MOUSE_CAMERA_TOGGLE;
extern int CONFIG_MOUSE_CAMERA_L;
extern int CONFIG_MOUSE_CAMERA_R;
extern int CONFIG_NICE;
extern int CONFIG_FPS;
extern int CONFIG_SOUND_VOLUME;
extern int CONFIG_MUSIC_VOLUME;
extern int CONFIG_JOYSTICK;
extern int CONFIG_JOYSTICK_DEVICE;
extern int CONFIG_JOYSTICK_AXIS_X;
extern int CONFIG_JOYSTICK_AXIS_Y;
extern int CONFIG_JOYSTICK_AXIS_U;
extern int CONFIG_JOYSTICK_BUTTON_A;
extern int CONFIG_JOYSTICK_BUTTON_B;
extern int CONFIG_JOYSTICK_BUTTON_R;
extern int CONFIG_JOYSTICK_BUTTON_L;
extern int CONFIG_JOYSTICK_BUTTON_EXIT;
extern int CONFIG_JOYSTICK_CAMERA_1;
extern int CONFIG_JOYSTICK_CAMERA_2;
extern int CONFIG_JOYSTICK_CAMERA_3;
extern int CONFIG_JOYSTICK_DPAD_L;
extern int CONFIG_JOYSTICK_DPAD_R;
extern int CONFIG_JOYSTICK_DPAD_U;
extern int CONFIG_JOYSTICK_DPAD_D;
extern int CONFIG_JOYSTICK_CAMERA_TOGGLE;
extern int CONFIG_JOYSTICK_ROTATE_FAST;
extern int CONFIG_KEY_CAMERA_1;
extern int CONFIG_KEY_CAMERA_2;
extern int CONFIG_KEY_CAMERA_3;
extern int CONFIG_KEY_CAMERA_TOGGLE;
extern int CONFIG_KEY_CAMERA_R;
extern int CONFIG_KEY_CAMERA_L;
extern int CONFIG_VIEW_FOV;
extern int CONFIG_VIEW_DP;
extern int CONFIG_VIEW_DC;
extern int CONFIG_VIEW_DZ;
extern int CONFIG_ROTATE_FAST;
extern int CONFIG_ROTATE_SLOW;
extern int CONFIG_KEY_FORWARD;
extern int CONFIG_KEY_BACKWARD;
extern int CONFIG_KEY_LEFT;
extern int CONFIG_KEY_RIGHT;
extern int CONFIG_KEY_PAUSE;
extern int CONFIG_KEY_RESTART;
extern int CONFIG_KEY_SCORE_NEXT;
extern int CONFIG_KEY_ROTATE_FAST;
extern int CONFIG_CHEAT;
extern int CONFIG_STATS;
extern int CONFIG_UNIFORM;
extern int CONFIG_SCREENSHOT;
extern int CONFIG_LOCK_GOALS;

/* String options. */

extern int CONFIG_PLAYER;
extern int CONFIG_BALL_FILE;
extern int CONFIG_WIIMOTE_ADDR;
extern int CONFIG_REPLAY_NAME;

/*---------------------------------------------------------------------------*/

void config_init(void);
void config_load(void);
void config_save(void);

/*---------------------------------------------------------------------------*/

void config_set_d(int, int);
void config_tgl_d(int);
int  config_tst_d(int, int);
int  config_get_d(int);

void        config_set_s(int, const char *);
const char *config_get_s(int);

/*---------------------------------------------------------------------------*/

int  config_cheat(void);
void config_set_cheat(void);
void config_clr_cheat(void);

/*---------------------------------------------------------------------------*/

int config_screenshot(void);

/*---------------------------------------------------------------------------*/

#endif
