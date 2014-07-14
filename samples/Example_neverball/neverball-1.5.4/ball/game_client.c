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
#include <math.h>
#include <assert.h>

#include "glext.h"
#include "vec3.h"
#include "geom.h"
#include "item.h"
#include "back.h"
#include "part.h"
#include "ball.h"
#include "image.h"
#include "audio.h"
#include "solid_gl.h"
#include "config.h"
#include "video.h"

#include "game_client.h"
#include "game_common.h"
#include "game_proxy.h"

#include "cmd.h"

/*---------------------------------------------------------------------------*/

int game_compat_map;                    /* Client/server map compat flag     */

/*---------------------------------------------------------------------------*/

static int client_state = 0;

static struct s_file file;
static struct s_file back;

static int   reflective;                /* Reflective geometry used?         */

static float timer      = 0.f;          /* Clock time                        */

static int status = GAME_NONE;          /* Outcome of the game               */

static struct game_tilt tilt;           /* Floor rotation                    */

static float view_c[3];                 /* Current view center               */
static float view_p[3];                 /* Current view position             */
static float view_e[3][3];              /* Current view reference frame      */

static int   coins  = 0;                /* Collected coins                   */
static int   goal_e = 0;                /* Goal enabled flag                 */
static float goal_k = 0;                /* Goal animation                    */

static int   jump_e = 1;                /* Jumping enabled flag              */
static int   jump_b = 0;                /* Jump-in-progress flag             */
static float jump_dt;                   /* Jump duration                     */

static float fade_k = 0.0;              /* Fade in/out level                 */
static float fade_d = 0.0;              /* Fade in/out direction             */

static int ups;                         /* Updates per second                */
static int first_update;                /* First update flag                 */
static int curr_ball;                   /* Current ball index                */

struct
{
    int x, y;
} version;                              /* Current map version               */

/*---------------------------------------------------------------------------*/

static void game_run_cmd(const union cmd *cmd)
{
    static const float gup[] = { 0.0f, +9.8f, 0.0f };
    static const float gdn[] = { 0.0f, -9.8f, 0.0f };

    /*
     * Neverball <= 1.5.1 does not send explicit tilt axes, rotation
     * happens directly around view vectors.  So for compatibility if
     * at the time of receiving tilt angles we have not yet received
     * the tilt axes, we use the view vectors.
     */
    static int got_tilt_axes;

    float f[3];

    if (client_state)
    {
        struct s_item *hp;
        struct s_ball *up;

        float dt;
        int i;

        switch (cmd->type)
        {
        case CMD_END_OF_UPDATE:

            got_tilt_axes = 0;

            if (first_update)
            {
                first_update = 0;
                break;
            }

            /* Compute gravity for particle effects. */

            if (status == GAME_GOAL)
                game_tilt_grav(f, gup, &tilt);
            else
                game_tilt_grav(f, gdn, &tilt);

            /* Step particle, goal and jump effects. */

            if (ups > 0)
            {
                dt = 1.0f / (float) ups;

                if (goal_e && goal_k < 1.0f)
                    goal_k += dt;

                if (jump_b)
                {
                    jump_dt += dt;

                    if (1.0f < jump_dt)
                        jump_b = 0;
                }

                part_step(f, dt);
            }

            break;

        case CMD_MAKE_BALL:
            /* Allocate a new ball and mark it as the current ball. */

            if ((up = realloc(file.uv, sizeof (*up) * (file.uc + 1))))
            {
                file.uv = up;
                curr_ball = file.uc;
                file.uc++;
            }
            break;

        case CMD_MAKE_ITEM:
            /* Allocate and initialise a new item. */

            if ((hp = realloc(file.hv, sizeof (*hp) * (file.hc + 1))))
            {
                struct s_item h;

                v_cpy(h.p, cmd->mkitem.p);

                h.t = cmd->mkitem.t;
                h.n = cmd->mkitem.n;

                file.hv          = hp;
                file.hv[file.hc] = h;
                file.hc++;
            }

            break;

        case CMD_PICK_ITEM:
            /* Set up particle effects and discard the item. */

            assert(cmd->pkitem.hi < file.hc);

            hp = &file.hv[cmd->pkitem.hi];

            item_color(hp, f);
            part_burst(hp->p, f);

            hp->t = ITEM_NONE;

            break;

        case CMD_TILT_ANGLES:
            if (!got_tilt_axes)
                game_tilt_axes(&tilt, view_e);

            tilt.rx = cmd->tiltangles.x;
            tilt.rz = cmd->tiltangles.z;
            break;

        case CMD_SOUND:
            /* Play the sound, then free its file name. */

            if (cmd->sound.n)
            {
                audio_play(cmd->sound.n, cmd->sound.a);

                /*
                 * FIXME Command memory management should be done
                 * elsewhere and done properly.
                 */

                free(cmd->sound.n);
            }
            break;

        case CMD_TIMER:
            timer = cmd->timer.t;
            break;

        case CMD_STATUS:
            status = cmd->status.t;
            break;

        case CMD_COINS:
            coins = cmd->coins.n;
            break;

        case CMD_JUMP_ENTER:
            jump_b  = 1;
            jump_e  = 0;
            jump_dt = 0.0f;
            break;

        case CMD_JUMP_EXIT:
            jump_e = 1;
            break;

        case CMD_BODY_PATH:
            file.bv[cmd->bodypath.bi].pi = cmd->bodypath.pi;
            break;

        case CMD_BODY_TIME:
            file.bv[cmd->bodytime.bi].t = cmd->bodytime.t;
            break;

        case CMD_GOAL_OPEN:
            /*
             * Enable the goal and make sure it's fully visible if
             * this is the first update.
             */

            if (!goal_e)
            {
                goal_e = 1;
                goal_k = first_update ? 1.0f : 0.0f;
            }
            break;

        case CMD_SWCH_ENTER:
            file.xv[cmd->swchenter.xi].e = 1;
            break;

        case CMD_SWCH_TOGGLE:
            file.xv[cmd->swchtoggle.xi].f = !file.xv[cmd->swchtoggle.xi].f;
            break;

        case CMD_SWCH_EXIT:
            file.xv[cmd->swchexit.xi].e = 0;
            break;

        case CMD_UPDATES_PER_SECOND:
            ups = cmd->ups.n;
            break;

        case CMD_BALL_RADIUS:
            file.uv[curr_ball].r = cmd->ballradius.r;
            break;

        case CMD_CLEAR_ITEMS:
            if (file.hv)
            {
                free(file.hv);
                file.hv = NULL;
            }
            file.hc = 0;
            break;

        case CMD_CLEAR_BALLS:
            if (file.uv)
            {
                free(file.uv);
                file.uv = NULL;
            }
            file.uc = 0;
            break;

        case CMD_BALL_POSITION:
            v_cpy(file.uv[curr_ball].p, cmd->ballpos.p);
            break;

        case CMD_BALL_BASIS:
            v_cpy(file.uv[curr_ball].e[0], cmd->ballbasis.e[0]);
            v_cpy(file.uv[curr_ball].e[1], cmd->ballbasis.e[1]);

            v_crs(file.uv[curr_ball].e[2],
                  file.uv[curr_ball].e[0],
                  file.uv[curr_ball].e[1]);
            break;

        case CMD_BALL_PEND_BASIS:
            v_cpy(file.uv[curr_ball].E[0], cmd->ballpendbasis.E[0]);
            v_cpy(file.uv[curr_ball].E[1], cmd->ballpendbasis.E[1]);

            v_crs(file.uv[curr_ball].E[2],
                  file.uv[curr_ball].E[0],
                  file.uv[curr_ball].E[1]);
            break;

        case CMD_VIEW_POSITION:
            v_cpy(view_p, cmd->viewpos.p);
            break;

        case CMD_VIEW_CENTER:
            v_cpy(view_c, cmd->viewcenter.c);
            break;

        case CMD_VIEW_BASIS:
            v_cpy(view_e[0], cmd->viewbasis.e[0]);
            v_cpy(view_e[1], cmd->viewbasis.e[1]);

            v_crs(view_e[2], view_e[0], view_e[1]);

            break;

        case CMD_CURRENT_BALL:
            curr_ball = cmd->currball.ui;
            break;

        case CMD_PATH_FLAG:
            file.pv[cmd->pathflag.pi].f = cmd->pathflag.f;
            break;

        case CMD_STEP_SIMULATION:
            /*
             * Simulate body motion.
             *
             * This is done on the client side due to replay file size
             * concerns and isn't done as part of CMD_END_OF_UPDATE to
             * match the server state as closely as possible.  Body
             * time is still synchronised with the server on a
             * semi-regular basis and path indices are handled through
             * CMD_BODY_PATH, thus this code doesn't need to be as
             * sophisticated as sol_body_step.
             */

            dt = cmd->stepsim.dt;

            for (i = 0; i < file.bc; i++)
            {
                struct s_body *bp = file.bv + i;
                struct s_path *pp = file.pv + bp->pi;

                if (bp->pi >= 0 && pp->f)
                    bp->t += dt;
            }
            break;

        case CMD_MAP:

            /*
             * Note if the loaded map matches the server's
             * expectations. (No, this doesn't actually load a map,
             * yet.  Something else somewhere else does.)
             */

            free(cmd->map.name);
            game_compat_map = version.x == cmd->map.version.x;
            break;

        case CMD_TILT_AXES:
            got_tilt_axes = 1;
            v_cpy(tilt.x, cmd->tiltaxes.x);
            v_cpy(tilt.z, cmd->tiltaxes.z);
            break;

        case CMD_NONE:
        case CMD_MAX:
            break;
        }
    }
}

void game_client_step(fs_file demo_fp)
{
    union cmd *cmdp;

    while ((cmdp = game_proxy_deq()))
    {
        /*
         * Note: cmd_put is called first here because game_run_cmd
         * frees some command struct members.
         */

        if (demo_fp)
            cmd_put(demo_fp, cmdp);

        game_run_cmd(cmdp);

        free(cmdp);
    }
}

/*---------------------------------------------------------------------------*/

int  game_client_init(const char *file_name)
{
    char *back_name = NULL, *grad_name = NULL;
    int i;

    coins  = 0;
    status = GAME_NONE;

    if (client_state)
        game_client_free();

    if (!sol_load_gl(&file, file_name,
                     config_get_d(CONFIG_TEXTURES),
                     config_get_d(CONFIG_SHADOW)))
        return (client_state = 0);

    reflective = sol_reflective(&file);

    client_state = 1;

    game_tilt_init(&tilt);

    /* Initialize jump and goal states. */

    jump_e = 1;
    jump_b = 0;

    goal_e = 0;
    goal_k = 0.0f;

    /* Initialise the level, background, particles, fade, and view. */

    fade_k =  1.0f;
    fade_d = -2.0f;


    version.x = 0;
    version.y = 0;

    for (i = 0; i < file.dc; i++)
    {
        char *k = file.av + file.dv[i].ai;
        char *v = file.av + file.dv[i].aj;

        if (strcmp(k, "back") == 0) back_name = v;
        if (strcmp(k, "grad") == 0) grad_name = v;

        if (strcmp(k, "version") == 0)
            sscanf(v, "%d.%d", &version.x, &version.y);
    }

    /*
     * If the client map's version is 1, assume the map is compatible
     * with the server.  This ensures that 1.5.0 replays don't trigger
     * bogus map compatibility warnings.  (Post-1.5.0 replays will
     * have CMD_MAP override this.)
     */

    game_compat_map = version.x == 1;

    part_reset(GOAL_HEIGHT, JUMP_HEIGHT);

    ups          = 0;
    first_update = 1;

    back_init(grad_name, config_get_d(CONFIG_GEOMETRY));
    sol_load_gl(&back, back_name,
                config_get_d(CONFIG_TEXTURES), 0);

    return client_state;
}

void game_client_free(void)
{
    if (client_state)
    {
        game_proxy_clr();
        sol_free_gl(&file);
        sol_free_gl(&back);
        back_free();
    }
    client_state = 0;
}

/*---------------------------------------------------------------------------*/

int curr_clock(void)
{
    return (int) (timer * 100.f);
}

int curr_coins(void)
{
    return coins;
}

int curr_status(void)
{
    return status;
}

/*---------------------------------------------------------------------------*/

static void game_draw_balls(const struct s_file *fp,
                            const float *bill_M, float t)
{
    float c[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    float ball_M[16];
    float pend_M[16];

    m_basis(ball_M, fp->uv[0].e[0], fp->uv[0].e[1], fp->uv[0].e[2]);
    m_basis(pend_M, fp->uv[0].E[0], fp->uv[0].E[1], fp->uv[0].E[2]);

    glPushAttrib(GL_LIGHTING_BIT);
    glPushMatrix();
    {
        glTranslatef(fp->uv[0].p[0],
                     fp->uv[0].p[1] + BALL_FUDGE,
                     fp->uv[0].p[2]);
        glScalef(fp->uv[0].r,
                 fp->uv[0].r,
                 fp->uv[0].r);

        glColor4fv(c);
        ball_draw(ball_M, pend_M, bill_M, t);
    }
    glPopMatrix();
    glPopAttrib();
}

static void game_draw_items(const struct s_file *fp, float t)
{
    float r = 360.f * t;
    int hi;

    glPushAttrib(GL_LIGHTING_BIT);
    {
        item_push(ITEM_COIN);
        {
            for (hi = 0; hi < fp->hc; hi++)

                if (fp->hv[hi].t == ITEM_COIN && fp->hv[hi].n > 0)
                {
                    glPushMatrix();
                    {
                        glTranslatef(fp->hv[hi].p[0],
                                     fp->hv[hi].p[1],
                                     fp->hv[hi].p[2]);
                        glRotatef(r, 0.0f, 1.0f, 0.0f);
                        item_draw(&fp->hv[hi], r);
                    }
                    glPopMatrix();
                }
        }
        item_pull();

        item_push(ITEM_SHRINK);
        {
            for (hi = 0; hi < fp->hc; hi++)

                if (fp->hv[hi].t == ITEM_SHRINK)
                {
                    glPushMatrix();
                    {
                        glTranslatef(fp->hv[hi].p[0],
                                     fp->hv[hi].p[1],
                                     fp->hv[hi].p[2]);
                        glRotatef(r, 0.0f, 1.0f, 0.0f);
                        item_draw(&fp->hv[hi], r);
                    }
                    glPopMatrix();
                }
        }
        item_pull();

        item_push(ITEM_GROW);
        {
            for (hi = 0; hi < fp->hc; hi++)

                if (fp->hv[hi].t == ITEM_GROW)
                {
                    glPushMatrix();
                    {
                        glTranslatef(fp->hv[hi].p[0],
                                     fp->hv[hi].p[1],
                                     fp->hv[hi].p[2]);
                        glRotatef(r, 0.0f, 1.0f, 0.0f);
                        item_draw(&fp->hv[hi], r);
                    }
                    glPopMatrix();
                }
        }
        item_pull();
    }
    glPopAttrib();
}

static void game_draw_goals(const struct s_file *fp, const float *M, float t)
{
    if (goal_e)
    {
        int zi;

        /* Draw the goal particles. */

        glEnable(GL_TEXTURE_2D);
        {
            for (zi = 0; zi < fp->zc; zi++)
            {
                glPushMatrix();
                {
                    glTranslatef(fp->zv[zi].p[0],
                                 fp->zv[zi].p[1],
                                 fp->zv[zi].p[2]);

                    part_draw_goal(M, fp->zv[zi].r, goal_k, t);
                }
                glPopMatrix();
            }
        }
        glDisable(GL_TEXTURE_2D);

        /* Draw the goal column. */

        for (zi = 0; zi < fp->zc; zi++)
        {
            glPushMatrix();
            {
                glTranslatef(fp->zv[zi].p[0],
                             fp->zv[zi].p[1],
                             fp->zv[zi].p[2]);

                glScalef(fp->zv[zi].r,
                         goal_k,
                         fp->zv[zi].r);

                goal_draw();
            }
            glPopMatrix();
        }
    }
}

static void game_draw_jumps(const struct s_file *fp, const float *M, float t)
{
    int ji;

    glEnable(GL_TEXTURE_2D);
    {
        for (ji = 0; ji < fp->jc; ji++)
        {
            glPushMatrix();
            {
                glTranslatef(fp->jv[ji].p[0],
                             fp->jv[ji].p[1],
                             fp->jv[ji].p[2]);

                part_draw_jump(M, fp->jv[ji].r, 1.0f, t);
            }
            glPopMatrix();
        }
    }
    glDisable(GL_TEXTURE_2D);

    for (ji = 0; ji < fp->jc; ji++)
    {
        glPushMatrix();
        {
            glTranslatef(fp->jv[ji].p[0],
                         fp->jv[ji].p[1],
                         fp->jv[ji].p[2]);
            glScalef(fp->jv[ji].r,
                     1.0f,
                     fp->jv[ji].r);

            jump_draw(!jump_e);
        }
        glPopMatrix();
    }
}

static void game_draw_swchs(const struct s_file *fp)
{
    int xi;

    for (xi = 0; xi < fp->xc; xi++)
    {
        if (fp->xv[xi].i)
            continue;

        glPushMatrix();
        {
            glTranslatef(fp->xv[xi].p[0],
                         fp->xv[xi].p[1],
                         fp->xv[xi].p[2]);
            glScalef(fp->xv[xi].r,
                     1.0f,
                     fp->xv[xi].r);

            swch_draw(fp->xv[xi].f, fp->xv[xi].e);
        }
        glPopMatrix();
    }
}

/*---------------------------------------------------------------------------*/

static void game_draw_tilt(int d)
{
    const float *ball_p = file.uv->p;

    /* Rotate the environment about the position of the ball. */

    glTranslatef(+ball_p[0], +ball_p[1] * d, +ball_p[2]);
    glRotatef(-tilt.rz * d, tilt.z[0], tilt.z[1], tilt.z[2]);
    glRotatef(-tilt.rx * d, tilt.x[0], tilt.x[1], tilt.x[2]);
    glTranslatef(-ball_p[0], -ball_p[1] * d, -ball_p[2]);
}

static void game_refl_all(void)
{
    glPushMatrix();
    {
        game_draw_tilt(1);

        /* Draw the floor. */

        sol_refl(&file);
    }
    glPopMatrix();
}

/*---------------------------------------------------------------------------*/

static void game_draw_light(void)
{
    const float light_p[2][4] = {
        { -8.0f, +32.0f, -8.0f, 0.0f },
        { +8.0f, +32.0f, +8.0f, 0.0f },
    };
    const float light_c[2][4] = {
        { 1.0f, 0.8f, 0.8f, 1.0f },
        { 0.8f, 1.0f, 0.8f, 1.0f },
    };

    /* Configure the lighting. */

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_p[0]);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_c[0]);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_c[0]);

    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_POSITION, light_p[1]);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  light_c[1]);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_c[1]);
}

static void game_draw_back(int pose, int d, float t)
{
    if (pose == 2)
        return;

    glPushMatrix();
    {
        if (d < 0)
        {
            glRotatef(tilt.rz * 2, tilt.z[0], tilt.z[1], tilt.z[2]);
            glRotatef(tilt.rx * 2, tilt.x[0], tilt.x[1], tilt.x[2]);
        }

        glTranslatef(view_p[0], view_p[1] * d, view_p[2]);

        if (config_get_d(CONFIG_BACKGROUND))
        {
            /* Draw all background layers back to front. */

            sol_back(&back, BACK_DIST, FAR_DIST,  t);
            back_draw(0);
            sol_back(&back,         0, BACK_DIST, t);
        }
        else back_draw(0);
    }
    glPopMatrix();
}

static void game_clip_refl(int d)
{
    /* Fudge to eliminate the floor from reflection. */

    GLdouble e[4], k = -0.00001;

    e[0] = 0;
    e[1] = 1;
    e[2] = 0;
    e[3] = k;

    glClipPlane(GL_CLIP_PLANE0, e);
}

static void game_clip_ball(int d, const float *p)
{
    GLdouble r, c[3], pz[4], nz[4], ny[4];

    /* Compute the plane giving the front of the ball, as seen from view_p. */

    c[0] = p[0];
    c[1] = p[1] * d;
    c[2] = p[2];

    pz[0] = view_p[0] - c[0];
    pz[1] = view_p[1] - c[1];
    pz[2] = view_p[2] - c[2];

    r = sqrt(pz[0] * pz[0] + pz[1] * pz[1] + pz[2] * pz[2]);

    pz[0] /= r;
    pz[1] /= r;
    pz[2] /= r;
    pz[3] = -(pz[0] * c[0] +
              pz[1] * c[1] +
              pz[2] * c[2]);

    /* Find the plane giving the back of the ball, as seen from view_p. */

    nz[0] = -pz[0];
    nz[1] = -pz[1];
    nz[2] = -pz[2];
    nz[3] = -pz[3];

    /* Compute the plane giving the bottom of the ball. */

    ny[0] =  0.0;
    ny[1] = -1.0;
    ny[2] =  0.0;
    ny[3] = -(ny[0] * c[0] +
              ny[1] * c[1] +
              ny[2] * c[2]);

    /* Reflect these planes as necessary, and store them in the GL state. */

    pz[1] *= d;
    nz[1] *= d;
    ny[1] *= d;

    glClipPlane(GL_CLIP_PLANE1, nz);
    glClipPlane(GL_CLIP_PLANE2, pz);
    glClipPlane(GL_CLIP_PLANE3, ny);
}

static void game_draw_fore(int pose, const float *M, int d, float t)
{
    const float *ball_p = file.uv->p;
    const float  ball_r = file.uv->r;

    glPushMatrix();
    {
        /* Rotate the environment about the position of the ball. */

        game_draw_tilt(d);

        /* Compute clipping planes for reflection and ball facing. */

        game_clip_refl(d);
        game_clip_ball(d, ball_p);

        if (d < 0)
            glEnable(GL_CLIP_PLANE0);

        switch (pose)
        {
        case 1:
            sol_draw(&file, 0, 1);
            break;

        case 0:
            /* Draw the coins. */

            game_draw_items(&file, t);

            /* Draw the floor. */

            sol_draw(&file, 0, 1);

            /* Fall through. */

        case 2:

            /* Draw the ball shadow. */

            if (d > 0 && config_get_d(CONFIG_SHADOW))
            {
                shad_draw_set(ball_p, ball_r);
                sol_shad(&file);
                shad_draw_clr();
            }

            /* Draw the ball. */

            game_draw_balls(&file, M, t);

            break;
        }

        /* Draw the particles and light columns. */

        glEnable(GL_COLOR_MATERIAL);
        glDisable(GL_LIGHTING);
        glDepthMask(GL_FALSE);
        {
            glColor3f(1.0f, 1.0f, 1.0f);

            sol_bill(&file, M, t);
            part_draw_coin(M, t);

            glDisable(GL_TEXTURE_2D);
            {
                game_draw_goals(&file, M, t);
                game_draw_jumps(&file, M, t);
                game_draw_swchs(&file);
            }
            glEnable(GL_TEXTURE_2D);

            glColor3f(1.0f, 1.0f, 1.0f);
        }
        glDepthMask(GL_TRUE);
        glEnable(GL_LIGHTING);
        glDisable(GL_COLOR_MATERIAL);

        if (d < 0)
            glDisable(GL_CLIP_PLANE0);
    }
    glPopMatrix();
}

void game_draw(int pose, float t)
{
    float fov = (float) config_get_d(CONFIG_VIEW_FOV);

    if (jump_b) fov *= 2.f * fabsf(jump_dt - 0.5);

    if (client_state)
    {
        video_push_persp(fov, 0.1f, FAR_DIST);
        glPushMatrix();
        {
            float T[16], U[16], M[16], v[3];

            /* Compute direct and reflected view bases. */

            v[0] = +view_p[0];
            v[1] = -view_p[1];
            v[2] = +view_p[2];

            m_view(T, view_c, view_p, view_e[1]);
            m_view(U, view_c, v,      view_e[1]);

            m_xps(M, T);

            /* Apply the current view. */

            v_sub(v, view_c, view_p);

            glTranslatef(0.f, 0.f, -v_len(v));
            glMultMatrixf(M);
            glTranslatef(-view_c[0], -view_c[1], -view_c[2]);

            if (reflective && config_get_d(CONFIG_REFLECTION))
            {
                glEnable(GL_STENCIL_TEST);
                {
                    /* Draw the mirrors only into the stencil buffer. */

                    glStencilFunc(GL_ALWAYS, 1, 0xFFFFFFFF);
                    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
                    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
                    glDepthMask(GL_FALSE);

                    game_refl_all();

                    glDepthMask(GL_TRUE);
                    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
                    glStencilFunc(GL_EQUAL, 1, 0xFFFFFFFF);

                    /* Draw the scene reflected into color and depth buffers. */

                    glFrontFace(GL_CW);
                    glPushMatrix();
                    {
                        glScalef(+1.0f, -1.0f, +1.0f);

                        game_draw_light();
                        game_draw_back(pose,    -1, t);
                        game_draw_fore(pose, U, -1, t);
                    }
                    glPopMatrix();
                    glFrontFace(GL_CCW);
                }
                glDisable(GL_STENCIL_TEST);
            }

            /* Draw the scene normally. */

            game_draw_light();

            if (reflective)
            {
                if (config_get_d(CONFIG_REFLECTION))
                {
                    /* Draw background while preserving reflections. */

                    glEnable(GL_STENCIL_TEST);
                    {
                        glStencilFunc(GL_NOTEQUAL, 1, 0xFFFFFFFF);
                        game_draw_back(pose, +1, t);
                    }
                    glDisable(GL_STENCIL_TEST);

                    /* Draw mirrors. */

                    game_refl_all();
                }
                else
                {
                    /* Draw background. */

                    game_draw_back(pose, +1, t);

                    /*
                     * Draw mirrors, first fully opaque with a custom
                     * material color, then blending normally with the
                     * opaque surfaces using their original material
                     * properties.  (Keeps background from showing
                     * through.)
                     */

                    glEnable(GL_COLOR_MATERIAL);
                    {
                        glColor4f(0.0, 0.0, 0.05, 1.0);
                        game_refl_all();
                        glColor4f(1.0,  1.0,  1.0,  1.0);
                    }
                    glDisable(GL_COLOR_MATERIAL);

                    game_refl_all();
                }
            }
            else
            {
                game_draw_back(pose, +1, t);
                game_refl_all();
            }

            game_draw_fore(pose, T, +1, t);
        }
        glPopMatrix();
        video_pop_matrix();

        /* Draw the fade overlay. */

        fade_draw(fade_k);
    }
}

/*---------------------------------------------------------------------------*/

void game_look(float phi, float theta)
{
    view_c[0] = view_p[0] + fsinf(V_RAD(theta)) * fcosf(V_RAD(phi));
    view_c[1] = view_p[1] +                       fsinf(V_RAD(phi));
    view_c[2] = view_p[2] - fcosf(V_RAD(theta)) * fcosf(V_RAD(phi));
}

/*---------------------------------------------------------------------------*/

void game_kill_fade(void)
{
    fade_k = 0.0f;
    fade_d = 0.0f;
}

void game_step_fade(float dt)
{
    if ((fade_k < 1.0f && fade_d > 0.0f) ||
        (fade_k > 0.0f && fade_d < 0.0f))
        fade_k += fade_d * dt;

    if (fade_k < 0.0f)
    {
        fade_k = 0.0f;
        fade_d = 0.0f;
    }
    if (fade_k > 1.0f)
    {
        fade_k = 1.0f;
        fade_d = 0.0f;
    }
}

void game_fade(float d)
{
    fade_d = d;
}

/*---------------------------------------------------------------------------*/

const struct s_file *game_client_file(void)
{
    return &file;
}

/*---------------------------------------------------------------------------*/
