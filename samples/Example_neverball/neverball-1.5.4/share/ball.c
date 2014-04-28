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

#include "vec3.h"
#include "glext.h"
#include "config.h"
#include "solid_gl.h"
#include "common.h"

/*---------------------------------------------------------------------------*/

static int has_solid = 0;
static int has_inner = 0;
static int has_outer = 0;

static struct s_file solid;
static struct s_file inner;
static struct s_file outer;

#define F_PENDULUM   1
#define F_DRAWBACK   2
#define F_DRAWCLIP   4
#define F_DEPTHMASK  8
#define F_DEPTHTEST 16

static int solid_flags;
static int inner_flags;
static int outer_flags;

static float solid_alpha;
static float inner_alpha;
static float outer_alpha;

/*---------------------------------------------------------------------------*/

#define SET(B, v, b) ((v) ? ((B) | (b)) : ((B) & ~(b)))

static int ball_opts(const struct s_file *fp, float *alpha)
{
    int flags = F_DEPTHTEST;
    int di;

    for (di = 0; di < fp->dc; ++di)
    {
        char *k = fp->av + fp->dv[di].ai;
        char *v = fp->av + fp->dv[di].aj;

        if (strcmp(k, "pendulum")  == 0)
            flags = SET(flags, atoi(v), F_PENDULUM);
        if (strcmp(k, "drawback")  == 0)
            flags = SET(flags, atoi(v), F_DRAWBACK);
        if (strcmp(k, "drawclip")  == 0)
            flags = SET(flags, atoi(v), F_DRAWCLIP);
        if (strcmp(k, "depthmask") == 0)
            flags = SET(flags, atoi(v), F_DEPTHMASK);
        if (strcmp(k, "depthtest") == 0)
            flags = SET(flags, atoi(v), F_DEPTHTEST);
        if (strcmp(k, "alphatest") == 0)
            sscanf(v, "%f", alpha);
    }

    return flags;
}

void ball_init(void)
{
    int T = config_get_d(CONFIG_TEXTURES);

    char *solid_file = concat_string(config_get_s(CONFIG_BALL_FILE),
                                     "-solid.sol", NULL);
    char *inner_file = concat_string(config_get_s(CONFIG_BALL_FILE),
                                     "-inner.sol", NULL);
    char *outer_file = concat_string(config_get_s(CONFIG_BALL_FILE),
                                     "-outer.sol", NULL);

    solid_flags = 0;
    inner_flags = 0;
    outer_flags = 0;

    solid_alpha = 1.0f;
    inner_alpha = 1.0f;
    outer_alpha = 1.0f;

    if ((has_solid = sol_load_gl(&solid, solid_file, T, 0)))
        solid_flags = ball_opts(&solid, &solid_alpha);

    if ((has_inner = sol_load_gl(&inner, inner_file, T, 0)))
        inner_flags = ball_opts(&inner, &inner_alpha);

    if ((has_outer = sol_load_gl(&outer, outer_file, T, 0)))
        outer_flags = ball_opts(&outer, &outer_alpha);

    free(solid_file);
    free(inner_file);
    free(outer_file);
}

void ball_free(void)
{
    if (has_outer) sol_free_gl(&outer);
    if (has_inner) sol_free_gl(&inner);
    if (has_solid) sol_free_gl(&solid);

    has_solid = has_inner = has_outer = 0;
}

/*---------------------------------------------------------------------------*/

static void ball_draw_solid(const float *ball_M,
                            const float *ball_bill_M, float t)
{
    if (has_solid)
    {
        const int mask = (solid_flags & F_DEPTHMASK);
        const int test = (solid_flags & F_DEPTHTEST);

        if (solid_alpha < 1.0f)
        {
            glEnable(GL_ALPHA_TEST);
            glAlphaFunc(GL_GEQUAL, solid_alpha);
        }

        glPushMatrix();
        {
            /* Apply the ball rotation. */

            glMultMatrixf(ball_M);

            /* Draw the solid billboard geometry. */

            if (solid.rc)
            {
                if (test == 0) glDisable(GL_DEPTH_TEST);
                if (mask == 0) glDepthMask(GL_FALSE);
                glDisable(GL_LIGHTING);
                {
                    sol_bill(&solid, ball_bill_M, t);
                }
                glEnable(GL_LIGHTING);
                if (mask == 0) glDepthMask(GL_TRUE);
                if (test == 0) glEnable(GL_DEPTH_TEST);
            }

            /* Draw the solid opaque and transparent geometry. */

            sol_draw(&solid, mask, test);
        }
        glPopMatrix();

        if (solid_alpha < 1.0f)
            glDisable(GL_ALPHA_TEST);
    }
}

static void ball_draw_inner(const float *pend_M,
                            const float *bill_M,
                            const float *pend_bill_M, float t)
{
    if (has_inner)
    {
        const int pend = (inner_flags & F_PENDULUM);
        const int mask = (inner_flags & F_DEPTHMASK);
        const int test = (inner_flags & F_DEPTHTEST);

        if (inner_alpha < 1.0f)
        {
            glEnable(GL_ALPHA_TEST);
            glAlphaFunc(GL_GEQUAL, inner_alpha);
        }

        /* Apply the pendulum rotation. */

        if (pend)
        {
            glPushMatrix();
            glMultMatrixf(pend_M);
        }

        /* Draw the inner opaque and transparent geometry. */

        sol_draw(&inner, mask, test);

        /* Draw the inner billboard geometry. */

        if (inner.rc)
        {
            if (test == 0) glDisable(GL_DEPTH_TEST);
            if (mask == 0) glDepthMask(GL_FALSE);
            glDisable(GL_LIGHTING);
            {
                if (pend)
                    sol_bill(&inner, pend_bill_M, t);
                else
                    sol_bill(&inner, bill_M,      t);
            }

            glEnable(GL_LIGHTING);
            if (mask == 0) glDepthMask(GL_TRUE);
            if (test == 0) glEnable(GL_DEPTH_TEST);
        }

        if (pend)
            glPopMatrix();

        if (inner_alpha < 1.0f)
            glDisable(GL_ALPHA_TEST);
    }
}

static void ball_draw_outer(const float *pend_M,
                            const float *bill_M,
                            const float *pend_bill_M, float t)
{
    if (has_outer)
    {
        const int pend = (outer_flags & F_PENDULUM);
        const int mask = (outer_flags & F_DEPTHMASK);
        const int test = (outer_flags & F_DEPTHTEST);

        if (outer_alpha < 1.0f)
        {
            glEnable(GL_ALPHA_TEST);
            glAlphaFunc(GL_GEQUAL, outer_alpha);
        }

       /* Apply the pendulum rotation. */

        if (pend)
        {
            glPushMatrix();
            glMultMatrixf(pend_M);
        }

        /* Draw the outer opaque and transparent geometry. */

        sol_draw(&outer, mask, test);

        /* Draw the outer billboard geometry. */

        if (outer.rc)
        {
            if (test == 0) glDisable(GL_DEPTH_TEST);
            if (mask == 0) glDepthMask(GL_FALSE);
            glDisable(GL_LIGHTING);
            {
                if (pend)
                    sol_bill(&outer, pend_bill_M, t);
                else
                    sol_bill(&outer, bill_M,      t);
            }
            glEnable(GL_LIGHTING);
            if (mask == 0) glDepthMask(GL_TRUE);
            if (test == 0) glEnable(GL_DEPTH_TEST);
        }

        if (pend)
            glPopMatrix();

        if (outer_alpha < 1.0f)
            glDisable(GL_ALPHA_TEST);
    }
}

/*---------------------------------------------------------------------------*/

static void ball_pass_inner(const float *ball_M,
                            const float *pend_M,
                            const float *bill_M,
                            const float *ball_bill_M,
                            const float *pend_bill_M, float t)
{
    /* Sort the inner ball using clip planes. */

    if      (inner_flags & F_DRAWCLIP)
    {
        glEnable(GL_CLIP_PLANE1);
        ball_draw_inner(        pend_M, bill_M,              pend_bill_M, t);
        glDisable(GL_CLIP_PLANE1);

        glEnable(GL_CLIP_PLANE2);
        ball_draw_inner(        pend_M, bill_M,              pend_bill_M, t);
        glDisable(GL_CLIP_PLANE2);
    }

    /* Sort the inner ball using face culling. */

    else if (inner_flags & F_DRAWBACK)
    {
        glCullFace(GL_FRONT);
        ball_draw_inner(        pend_M, bill_M,              pend_bill_M, t);
        glCullFace(GL_BACK);
        ball_draw_inner(        pend_M, bill_M,              pend_bill_M, t);
    }

    /* Draw the inner ball normally. */

    else
    {
        ball_draw_inner(        pend_M, bill_M,              pend_bill_M, t);
    }
}

static void ball_pass_solid(const float *ball_M,
                            const float *pend_M,
                            const float *bill_M,
                            const float *ball_bill_M,
                            const float *pend_bill_M, float t)
{
    /* Sort the solid ball with the inner ball using clip planes. */

    if      (solid_flags & F_DRAWCLIP)
    {
        glEnable(GL_CLIP_PLANE1);
        ball_draw_solid(ball_M,                 ball_bill_M, t);
        glDisable(GL_CLIP_PLANE1);

        ball_pass_inner(ball_M, pend_M, bill_M, ball_bill_M, pend_bill_M, t);

        glEnable(GL_CLIP_PLANE2);
        ball_draw_solid(ball_M,                 ball_bill_M, t);
        glDisable(GL_CLIP_PLANE2);
    }

    /* Sort the solid ball with the inner ball using face culling. */

    else if (solid_flags & F_DRAWBACK)
    {
        glCullFace(GL_FRONT);
        ball_draw_solid(ball_M,                 ball_bill_M, t);
        glCullFace(GL_BACK);

        ball_pass_inner(ball_M, pend_M, bill_M, ball_bill_M, pend_bill_M, t);
        ball_draw_solid(ball_M,                 ball_bill_M, t);
    }

    /* Draw the solid ball after the inner ball. */

    else
    {
        ball_pass_inner(ball_M, pend_M, bill_M, ball_bill_M, pend_bill_M, t);
        ball_draw_solid(ball_M,                 ball_bill_M, t);
    }
}

static void ball_pass_outer(const float *ball_M,
                            const float *pend_M,
                            const float *bill_M,
                            const float *ball_bill_M,
                            const float *pend_bill_M, float t)
{
    /* Sort the outer ball with the solid ball using clip planes. */

    if      (outer_flags & F_DRAWCLIP)
    {
        glEnable(GL_CLIP_PLANE1);
        ball_draw_outer(        pend_M, bill_M,              pend_bill_M, t);
        glDisable(GL_CLIP_PLANE1);

        ball_pass_solid(ball_M, pend_M, bill_M, ball_bill_M, pend_bill_M, t);

        glEnable(GL_CLIP_PLANE2);
        ball_draw_outer(        pend_M, bill_M,              pend_bill_M, t);
        glDisable(GL_CLIP_PLANE2);
    }

    /* Sort the outer ball with the solid ball using face culling. */

    else if (outer_flags & F_DRAWBACK)
    {
        glCullFace(GL_FRONT);
        ball_draw_outer(        pend_M, bill_M,              pend_bill_M, t);
        glCullFace(GL_BACK);

        ball_pass_solid(ball_M, pend_M, bill_M, ball_bill_M, pend_bill_M, t);
        ball_draw_outer(        pend_M, bill_M,              pend_bill_M, t);
    }

    /* Draw the outer ball after the solid ball. */

    else
    {
        ball_pass_solid(ball_M, pend_M, bill_M, ball_bill_M, pend_bill_M, t);
        ball_draw_outer(        pend_M, bill_M,              pend_bill_M, t);
    }
}

/*---------------------------------------------------------------------------*/

void ball_draw(const float *ball_M,
               const float *pend_M,
               const float *bill_M, float t)
{
    /* Compute transforms for ball and pendulum billboards. */

    float ball_T[16], ball_bill_M[16];
    float pend_T[16], pend_bill_M[16];

    m_xps(ball_T, ball_M);
    m_xps(pend_T, pend_M);
    m_xps(pend_T, pend_M);

    m_mult(ball_bill_M, ball_T, bill_M);
    m_mult(pend_bill_M, pend_T, bill_M);

    /* Go to GREAT pains to ensure all layers are drawn back-to-front. */

    ball_pass_outer(ball_M, pend_M, bill_M, ball_bill_M, pend_bill_M, t);
}

/*---------------------------------------------------------------------------*/
