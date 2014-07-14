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
#include <math.h>

#include "glext.h"
#include "part.h"
#include "vec3.h"
#include "image.h"

/*---------------------------------------------------------------------------*/

struct part
{
    float t;
    float a;
    float w;
    float c[3];
    float p[3];
    float v[3];
};

static struct part part_coin[PART_MAX_COIN];
static struct part part_goal[PART_MAX_GOAL];
static struct part part_jump[PART_MAX_JUMP];
static GLuint      part_text_star;
static GLuint      part_text_squiggle;
static GLuint      part_list;

static float goal_height;
static float jump_height;

/*---------------------------------------------------------------------------*/

#define PI 3.1415927f

static float rnd(float l, float h)
{
    return l + (h - l) * rand() / RAND_MAX;
}

/*---------------------------------------------------------------------------*/

void part_reset(float zh, float jh)
{
    int i;

    goal_height = zh;
    jump_height = jh;

    for (i = 0; i < PART_MAX_COIN; i++)
        part_coin[i].t = 0.0f;

    for (i = 0; i < PART_MAX_GOAL; i++)
    {
        float t = rnd(+0.1f,      +1.0f);
        float a = rnd(-1.0f * PI, +1.0f * PI);
        float w = rnd(-2.0f * PI, +2.0f * PI);

        part_goal[i].t = t;
        part_goal[i].a = V_DEG(a);
        part_goal[i].w = V_DEG(w);

        part_goal[i].c[0] = 1.0f;
        part_goal[i].c[1] = 1.0f;
        part_goal[i].c[2] = 0.0f;

        part_goal[i].p[0] = fsinf(a);
        part_goal[i].p[1] = (1.f - t) * goal_height;
        part_goal[i].p[2] = fcosf(a);

        part_goal[i].v[0] = 0.f;
        part_goal[i].v[1] = 0.f;
        part_goal[i].v[2] = 0.f;
    }

    for (i = 0; i < PART_MAX_JUMP; i++)
    {
        float t = rnd(+0.1f,      +1.0f);
        float a = rnd(-1.0f * PI, +1.0f * PI);
        float w = rnd(+0.5f,      +2.5f);

        float vy = rnd(+0.025f, +0.25f);

        part_jump[i].t = t;
        part_jump[i].a = V_DEG(a);
        part_jump[i].w = w;

        part_jump[i].c[0] = 1.0f;
        part_jump[i].c[1] = 1.0f;
        part_jump[i].c[2] = 1.0f;

        part_jump[i].p[0] = fsinf(a);
        part_jump[i].p[1] = (1.f - t) * jump_height;
        part_jump[i].p[2] = fcosf(a);

        part_jump[i].v[0] = 0.f;
        part_jump[i].v[1] = vy;
        part_jump[i].v[2] = 0.f;
    }
}

void part_init(float zh, float jh)
{
    memset(part_coin, 0, PART_MAX_COIN * sizeof (struct part));
    memset(part_goal, 0, PART_MAX_GOAL * sizeof (struct part));
    memset(part_jump, 0, PART_MAX_JUMP * sizeof (struct part));

    part_text_star     = make_image_from_file(IMG_PART_STAR);
    part_text_squiggle = make_image_from_file(IMG_PART_SQUIGGLE);

    part_list = glGenLists(1);

    glNewList(part_list, GL_COMPILE);
    {
        glBegin(GL_QUADS);
        {
            glTexCoord2f(0.f, 0.f);
            glVertex2f(-PART_SIZE, -PART_SIZE);

            glTexCoord2f(1.f, 0.f);
            glVertex2f(+PART_SIZE, -PART_SIZE);

            glTexCoord2f(1.f, 1.f);
            glVertex2f(+PART_SIZE, +PART_SIZE);

            glTexCoord2f(0.f, 1.f);
            glVertex2f(-PART_SIZE, +PART_SIZE);
        }
        glEnd();
    }
    glEndList();

    part_reset(zh, jh);
}

void part_free(void)
{
    if (glIsList(part_list))
        glDeleteLists(part_list, 1);

    if (glIsTexture(part_text_star))
        glDeleteTextures(1, &part_text_star);

    if (glIsTexture(part_text_squiggle))
        glDeleteTextures(1, &part_text_squiggle);
}

/*---------------------------------------------------------------------------*/

void part_burst(const float *p, const float *c)
{
    int i, n = 0;

    for (i = 0; n < 10 && i < PART_MAX_COIN; i++)
        if (part_coin[i].t <= 0.f)
        {
            float a = rnd(-1.0f * PI, +1.0f * PI);
            float b = rnd(+0.3f * PI, +0.5f * PI);
            float w = rnd(-4.0f * PI, +4.0f * PI);

            part_coin[i].p[0] = p[0];
            part_coin[i].p[1] = p[1];
            part_coin[i].p[2] = p[2];

            part_coin[i].v[0] = 4.f * fcosf(a) * fcosf(b);
            part_coin[i].v[1] = 4.f *            fsinf(b);
            part_coin[i].v[2] = 4.f * fsinf(a) * fcosf(b);

            part_coin[i].c[0] = c[0];
            part_coin[i].c[1] = c[1];
            part_coin[i].c[2] = c[2];

            part_coin[i].t = 1.f;
            part_coin[i].a = 0.f;
            part_coin[i].w = V_DEG(w);

            n++;
        }
}

/*---------------------------------------------------------------------------*/

static void part_fall(struct part *part, int n, const float *g, float dt)
{
    int i;

    for (i = 0; i < n; i++)
        if (part[i].t > 0.f)
        {
            part[i].t -= dt;

            part[i].v[0] += (g[0] * dt);
            part[i].v[1] += (g[1] * dt);
            part[i].v[2] += (g[2] * dt);

            part[i].p[0] += (part[i].v[0] * dt);
            part[i].p[1] += (part[i].v[1] * dt);
            part[i].p[2] += (part[i].v[2] * dt);
        }
}

static void part_spin(struct part *part, int n, const float *g, float dt)
{
    int i;

    for (i = 0; i < n; i++)
        if (part[i].t > 0.f)
        {
            part[i].a += 30.f * dt;

            part[i].p[0] = fsinf(V_RAD(part[i].a));
            part[i].p[2] = fcosf(V_RAD(part[i].a));
        }
}

void part_step(const float *g, float dt)
{
    int i;

    part_fall(part_coin, PART_MAX_COIN, g, dt);

    if (g[1] > 0.f)
        part_fall(part_goal, PART_MAX_GOAL, g, dt);
    else
        part_spin(part_goal, PART_MAX_GOAL, g, dt);

    for (i = 0; i < PART_MAX_JUMP; i++)
    {
        part_jump[i].p[1] += part_jump[i].v[1] * dt;

        if (part_jump[i].p[1] > jump_height)
            part_jump[i].p[1] = 0.0f;
    }
}

/*---------------------------------------------------------------------------*/

static void part_draw(const float *M,
                      const float *p, float r, float rz, float s)
{
    glPushMatrix();
    {
        glTranslatef(r * p[0], p[1], r * p[2]);
        glMultMatrixf(M);
        glRotatef(rz, 0.f, 0.f, 1.f);
        glScalef(s, s, 1.0f);

        glCallList(part_list);
    }
    glPopMatrix();
}

void part_draw_coin(const float *M, float t)
{
    int i;

    glBindTexture(GL_TEXTURE_2D, part_text_star);

    for (i = 0; i < PART_MAX_COIN; i++)
        if (part_coin[i].t > 0.f)
        {
            glColor4f(part_coin[i].c[0],
                      part_coin[i].c[1],
                      part_coin[i].c[2],
                      part_coin[i].t);

            part_draw(M, part_coin[i].p, 1.0f, t * part_coin[i].w, 1.0f);
        }
}

void part_draw_goal(const float *M, float radius, float a, float t)
{
    int i;

    glBindTexture(GL_TEXTURE_2D, part_text_star);

    glColor4f(1.0f, 1.0f, 0.0f, a);

    for (i = 0; i < PART_MAX_GOAL; i++)
        if (part_goal[i].t > 0.0f)
            part_draw(M, part_goal[i].p, radius - 0.05f,
                      t * part_goal[i].w, 1.0f);
}

void part_draw_jump(const float *M, float radius, float a, float t)
{
    int i;

    glBindTexture(GL_TEXTURE_2D, part_text_squiggle);

    for (i = 0; i < PART_MAX_JUMP; i++)
    {
        glColor4f(part_jump[i].c[0],
                  part_jump[i].c[1],
                  part_jump[i].c[2],
                  1.0f - part_jump[i].p[1] / jump_height);

        /*
         * X is the current time since some Epoch, Y is the time it
         * takes for a squiggle to grow to its full size and then
         * shrink again.  F is the current scale of the squiggle in
         * the interval [0.0, 1.0].  The ratio is offset by 0.5 to
         * have F = 1.0 when X = 0.0.
         */

#define F(x, y) fabsf(fsinf(((x) / (y) + 0.5f) * PI))

        part_draw(M, part_jump[i].p, radius - 0.05f,
                  0.0f, F(t, part_jump[i].w));

#undef F
    }
}

/*---------------------------------------------------------------------------*/
