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
#include <SDL_thread.h>
#include <math.h>
#include <stdio.h>

#include "config.h"

/*---------------------------------------------------------------------------*/
#ifdef ENABLE_WII

#define _ENABLE_TILT
#include <libcwiimote/wiimote.h>
#include <libcwiimote/wiimote_api.h>

/*
 * This data structure tracks button changes, counting transitions so that
 * none are missed if the event handling thread falls significantly behind
 * the device IO thread.
 */

#define BUTTON_NC 0
#define BUTTON_DN 1
#define BUTTON_UP 2

struct button_state
{
    unsigned char curr;
    unsigned char last;
    unsigned char upc;
    unsigned char dnc;
};

static void set_button(struct button_state *B, int s)
{
    if ((B->curr == 0) != (s == 0))
    {
        if (B->curr)
        {
            B->upc++;
            B->curr = 0;
        }
        else
        {
            B->dnc++;
            B->curr = 1;
        }
    }
}

static int get_button(struct button_state *B)
{
    int ch = BUTTON_NC;

    if      (B->last == 1 && B->upc > 0)
    {
        B->upc--;
        B->last = 0;
        ch = BUTTON_UP;
    }
    else if (B->last == 0 && B->dnc > 0)
    {
        B->dnc--;
        B->last = 1;
        ch = BUTTON_DN;
    }

    return ch;
}

/*---------------------------------------------------------------------------*/

struct tilt_state
{
    int   status;
    float x;
    float z;
    struct button_state A;
    struct button_state B;
    struct button_state plus;
    struct button_state minus;
    struct button_state home;
    struct button_state L;
    struct button_state R;
    struct button_state U;
    struct button_state D;
};

static struct tilt_state state;
static SDL_mutex        *mutex  = NULL;
static SDL_Thread       *thread = NULL;

#define FILTER 8

static int tilt_func(void *data)
{
    wiimote_t wiimote = WIIMOTE_INIT;
    char      address[MAXSTR];

    config_get_s(CONFIG_WIIMOTE_ADDR, address, MAXSTR);

    if (strlen(address) > 0)
    {
        if (wiimote_connect(&wiimote, address) < 0)
            fprintf(stderr, "%s\n", wiimote_get_error());
        else
        {
            int running = 1;

            wiimote.mode.bits = WIIMOTE_MODE_ACC;
            wiimote.led.one   = 1;

            SDL_mutexP(mutex);
            state.status = running;
            SDL_mutexV(mutex);

            while (mutex && running && wiimote_is_open(&wiimote))
            {
                if (wiimote_update(&wiimote) < 0)
                    break;

                SDL_mutexP(mutex);
                {
                    running = state.status;

                    set_button(&state.A,     wiimote.keys.a);
                    set_button(&state.B,     wiimote.keys.b);
                    set_button(&state.plus,  wiimote.keys.plus);
                    set_button(&state.minus, wiimote.keys.minus);
                    set_button(&state.home,  wiimote.keys.home);
                    set_button(&state.L,     wiimote.keys.left);
                    set_button(&state.R,     wiimote.keys.right);
                    set_button(&state.U,     wiimote.keys.up);
                    set_button(&state.D,     wiimote.keys.down);

                    if (isnormal(wiimote.tilt.y))
                    {
                        state.x = (state.x * (FILTER - 1) +
                                   wiimote.tilt.y) / FILTER;
                    }
                    if (isnormal(wiimote.tilt.x))
                    {
                        state.z = (state.z * (FILTER - 1) +
                                   wiimote.tilt.x) / FILTER;
                    }
                }
                SDL_mutexV(mutex);
            }

            wiimote_disconnect(&wiimote);
        }
    }
    return 0;
}

void tilt_init(void)
{
    memset(&state, 0, sizeof (struct tilt_state));

    mutex  = SDL_CreateMutex();
    thread = SDL_CreateThread(tilt_func, NULL);
}

void tilt_free(void)
{
    int b = 0;

    if (mutex)
    {
        /* Get/set the status of the tilt sensor thread. */

        SDL_mutexP(mutex);
        b = state.status;
        state.status = 0;
        SDL_mutexV(mutex);

        /* Kill the thread and destroy the mutex. */

        SDL_KillThread(thread);
        SDL_DestroyMutex(mutex);

        mutex  = NULL;
        thread = NULL;
    }
}

int tilt_get_button(int *b, int *s)
{
    int ch = BUTTON_NC;

    if (mutex)
    {
        SDL_mutexP(mutex);
        {
            if      ((ch = get_button(&state.A)))
            {
                *b = config_get_d(CONFIG_JOYSTICK_BUTTON_A);
                *s = (ch == BUTTON_DN);
            }
            else if ((ch = get_button(&state.B)))
            {
                *b = config_get_d(CONFIG_JOYSTICK_BUTTON_B);
                *s = (ch == BUTTON_DN);
            }
            else if ((ch = get_button(&state.plus)))
            {
                *b = config_get_d(CONFIG_JOYSTICK_BUTTON_R);
                *s = (ch == BUTTON_DN);
            }
            else if ((ch = get_button(&state.minus)))
            {
                *b = config_get_d(CONFIG_JOYSTICK_BUTTON_L);
                *s = (ch == BUTTON_DN);
            }
            else if ((ch = get_button(&state.home)))
            {
                *b = config_get_d(CONFIG_JOYSTICK_BUTTON_EXIT);
                *s = (ch == BUTTON_DN);
            }
            else if ((ch = get_button(&state.L)))
            {
                *b = config_get_d(CONFIG_JOYSTICK_DPAD_L);
                *s = (ch == BUTTON_DN);
            }
            else if ((ch = get_button(&state.R)))
            {
                *b = config_get_d(CONFIG_JOYSTICK_DPAD_R);
                *s = (ch == BUTTON_DN);
            }
            else if ((ch = get_button(&state.U)))
            {
                *b = config_get_d(CONFIG_JOYSTICK_DPAD_U);
                *s = (ch == BUTTON_DN);
            }
            else if ((ch = get_button(&state.D)))
            {
                *b = config_get_d(CONFIG_JOYSTICK_DPAD_D);
                *s = (ch == BUTTON_DN);
            }
        }
        SDL_mutexV(mutex);
    }
    return ch;
}

float tilt_get_x(void)
{
    float x = 0.0f;

    if (mutex)
    {
        SDL_mutexP(mutex);
        x = state.x;
        SDL_mutexV(mutex);
    }

    return x;
}

float tilt_get_z(void)
{
    float z = 0.0f;

    if (mutex)
    {
        SDL_mutexP(mutex);
        z = state.z;
        SDL_mutexV(mutex);
    }

    return z;
}

int tilt_stat(void)
{
    int b = 0;

    if (mutex)
    {
        SDL_mutexP(mutex);
        b = state.status;
        SDL_mutexV(mutex);
    }
    return b;
}

#endif
/*---------------------------------------------------------------------------*/
#ifndef ENABLE_WII

void tilt_init(void)
{
}

void tilt_free(void)
{
}

int tilt_stat(void)
{
    return 0;
}

int  tilt_get_button(int *b, int *s)
{
    return 0;
}

void tilt_get_direct(int *x, int *y)
{
    *x = 1;
    *y = 1;
}

float tilt_get_x(void)
{
    return 0.0f;
}

float tilt_get_z(void)
{
    return 0.0f;
}

#endif
/*---------------------------------------------------------------------------*/
