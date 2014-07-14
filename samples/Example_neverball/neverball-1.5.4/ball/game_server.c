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

#include "vec3.h"
#include "item.h"
#include "solid_phys.h"
#include "config.h"
#include "binary.h"
#include "common.h"

#include "game_common.h"
#include "game_server.h"
#include "game_proxy.h"

#include "cmd.h"

/*---------------------------------------------------------------------------*/

static int server_state = 0;

static struct s_file file;

static float timer      = 0.f;          /* Clock time                        */
static int   timer_down = 1;            /* Timer go up or down?              */

static int status = GAME_NONE;          /* Outcome of the game               */

static struct game_tilt tilt;           /* Floor rotation                    */

static float view_a;                    /* Ideal view rotation about Y axis  */
static float view_dc;                   /* Ideal view distance above ball    */
static float view_dp;                   /* Ideal view distance above ball    */
static float view_dz;                   /* Ideal view distance behind ball   */

static float view_c[3];                 /* Current view center               */
static float view_v[3];                 /* Current view vector               */
static float view_p[3];                 /* Current view position             */
static float view_e[3][3];              /* Current view reference frame      */
static float view_k;

static int   coins  = 0;                /* Collected coins                   */
static int   goal_e = 0;                /* Goal enabled flag                 */
static float goal_k = 0;                /* Goal animation                    */
static int   jump_e = 1;                /* Jumping enabled flag              */
static int   jump_b = 0;                /* Jump-in-progress flag             */
static float jump_dt;                   /* Jump duration                     */
static float jump_p[3];                 /* Jump destination                  */
static float jump_w[3];                 /* View destination                  */

/*---------------------------------------------------------------------------*/

/*
 * This is an abstraction of the game's input state.  All input is
 * encapsulated here, and all references to the input by the game are
 * made here.  TODO: This used to have the effect of homogenizing
 * input for use in replay recording and playback, but it's not clear
 * how relevant this approach is with the introduction of the command
 * pipeline.
 *
 * x and z:
 *     -32767 = -ANGLE_BOUND
 *     +32767 = +ANGLE_BOUND
 *
 * r:
 *     -32767 = -VIEWR_BOUND
 *     +32767 = +VIEWR_BOUND
 *
 */

struct input
{
    short x;
    short z;
    short r;
    short c;
};

static struct input input_current;

static void input_init(void)
{
    input_current.x = 0;
    input_current.z = 0;
    input_current.r = 0;
    input_current.c = 0;
}

static void input_set_x(float x)
{
    if (x < -ANGLE_BOUND) x = -ANGLE_BOUND;
    if (x >  ANGLE_BOUND) x =  ANGLE_BOUND;

    input_current.x = (short) (32767.0f * x / ANGLE_BOUND);
}

static void input_set_z(float z)
{
    if (z < -ANGLE_BOUND) z = -ANGLE_BOUND;
    if (z >  ANGLE_BOUND) z =  ANGLE_BOUND;

    input_current.z = (short) (32767.0f * z / ANGLE_BOUND);
}

static void input_set_r(float r)
{
    if (r < -VIEWR_BOUND) r = -VIEWR_BOUND;
    if (r >  VIEWR_BOUND) r =  VIEWR_BOUND;

    input_current.r = (short) (32767.0f * r / VIEWR_BOUND);
}

static void input_set_c(int c)
{
    input_current.c = (short) c;
}

static float input_get_x(void)
{
    return ANGLE_BOUND * (float) input_current.x / 32767.0f;
}

static float input_get_z(void)
{
    return ANGLE_BOUND * (float) input_current.z / 32767.0f;
}

static float input_get_r(void)
{
    return VIEWR_BOUND * (float) input_current.r / 32767.0f;
}

static int input_get_c(void)
{
    return (int) input_current.c;
}

int input_put(fs_file fout)
{
    if (server_state)
    {
        put_short(fout, &input_current.x);
        put_short(fout, &input_current.z);
        put_short(fout, &input_current.r);
        put_short(fout, &input_current.c);

        return 1;
    }
    return 0;
}

int input_get(fs_file fin)
{
    if (server_state)
    {
        get_short(fin, &input_current.x);
        get_short(fin, &input_current.z);
        get_short(fin, &input_current.r);
        get_short(fin, &input_current.c);

        return (fs_eof(fin) ? 0 : 1);
    }
    return 0;
}

/*---------------------------------------------------------------------------*/

/*
 * Utility functions for preparing the "server" state and events for
 * consumption by the "client".
 */

static union cmd cmd;

static void game_cmd_map(const char *name, int ver_x, int ver_y)
{
    cmd.type          = CMD_MAP;
    cmd.map.name      = strdup(name);
    cmd.map.version.x = ver_x;
    cmd.map.version.y = ver_y;
    game_proxy_enq(&cmd);
}

static void game_cmd_eou(void)
{
    cmd.type = CMD_END_OF_UPDATE;
    game_proxy_enq(&cmd);
}

static void game_cmd_ups(void)
{
    cmd.type  = CMD_UPDATES_PER_SECOND;
    cmd.ups.n = UPS;
    game_proxy_enq(&cmd);
}

static void game_cmd_sound(const char *filename, float a)
{
    cmd.type = CMD_SOUND;

    cmd.sound.n = strdup(filename);
    cmd.sound.a = a;

    game_proxy_enq(&cmd);
}

#define audio_play(s, f) game_cmd_sound((s), (f))

static void game_cmd_goalopen(void)
{
    cmd.type = CMD_GOAL_OPEN;
    game_proxy_enq(&cmd);
}

static void game_cmd_updball(void)
{
    cmd.type = CMD_BALL_POSITION;
    memcpy(cmd.ballpos.p, file.uv[0].p, sizeof (float) * 3);
    game_proxy_enq(&cmd);

    cmd.type = CMD_BALL_BASIS;
    v_cpy(cmd.ballbasis.e[0], file.uv[0].e[0]);
    v_cpy(cmd.ballbasis.e[1], file.uv[0].e[1]);
    game_proxy_enq(&cmd);

    cmd.type = CMD_BALL_PEND_BASIS;
    v_cpy(cmd.ballpendbasis.E[0], file.uv[0].E[0]);
    v_cpy(cmd.ballpendbasis.E[1], file.uv[0].E[1]);
    game_proxy_enq(&cmd);
}

static void game_cmd_updview(void)
{
    cmd.type = CMD_VIEW_POSITION;
    memcpy(cmd.viewpos.p, view_p, sizeof (float) * 3);
    game_proxy_enq(&cmd);

    cmd.type = CMD_VIEW_CENTER;
    memcpy(cmd.viewcenter.c, view_c, sizeof (float) * 3);
    game_proxy_enq(&cmd);

    cmd.type = CMD_VIEW_BASIS;
    v_cpy(cmd.viewbasis.e[0], view_e[0]);
    v_cpy(cmd.viewbasis.e[1], view_e[1]);
    game_proxy_enq(&cmd);
}

static void game_cmd_ballradius(void)
{
    cmd.type         = CMD_BALL_RADIUS;
    cmd.ballradius.r = file.uv[0].r;
    game_proxy_enq(&cmd);
}

static void game_cmd_init_balls(void)
{
    cmd.type = CMD_CLEAR_BALLS;
    game_proxy_enq(&cmd);

    cmd.type = CMD_MAKE_BALL;
    game_proxy_enq(&cmd);

    game_cmd_updball();
    game_cmd_ballradius();
}

static void game_cmd_init_items(void)
{
    int i;

    cmd.type = CMD_CLEAR_ITEMS;
    game_proxy_enq(&cmd);

    for (i = 0; i < file.hc; i++)
    {
        cmd.type = CMD_MAKE_ITEM;

        v_cpy(cmd.mkitem.p, file.hv[i].p);

        cmd.mkitem.t = file.hv[i].t;
        cmd.mkitem.n = file.hv[i].n;

        game_proxy_enq(&cmd);
    }
}

static void game_cmd_pkitem(int hi)
{
    cmd.type      = CMD_PICK_ITEM;
    cmd.pkitem.hi = hi;
    game_proxy_enq(&cmd);
}

static void game_cmd_jump(int e)
{
    cmd.type = e ? CMD_JUMP_ENTER : CMD_JUMP_EXIT;
    game_proxy_enq(&cmd);
}

static void game_cmd_tiltangles(void)
{
    cmd.type = CMD_TILT_ANGLES;

    cmd.tiltangles.x = tilt.rx;
    cmd.tiltangles.z = tilt.rz;

    game_proxy_enq(&cmd);
}

static void game_cmd_tiltaxes(void)
{
    cmd.type = CMD_TILT_AXES;

    v_cpy(cmd.tiltaxes.x, tilt.x);
    v_cpy(cmd.tiltaxes.z, tilt.z);

    game_proxy_enq(&cmd);
}

static void game_cmd_timer(void)
{
    cmd.type    = CMD_TIMER;
    cmd.timer.t = timer;
    game_proxy_enq(&cmd);
}

static void game_cmd_coins(void)
{
    cmd.type    = CMD_COINS;
    cmd.coins.n = coins;
    game_proxy_enq(&cmd);
}

static void game_cmd_status(void)
{
    cmd.type     = CMD_STATUS;
    cmd.status.t = status;
    game_proxy_enq(&cmd);
}

/*---------------------------------------------------------------------------*/

static int   grow = 0;                  /* Should the ball be changing size? */
static float grow_orig = 0;             /* the original ball size            */
static float grow_goal = 0;             /* how big or small to get!          */
static float grow_t = 0.0;              /* timer for the ball to grow...     */
static float grow_strt = 0;             /* starting value for growth         */
static int   got_orig = 0;              /* Do we know original ball size?    */

#define GROW_TIME  0.5f                 /* sec for the ball to get to size.  */
#define GROW_BIG   1.5f                 /* large factor                      */
#define GROW_SMALL 0.5f                 /* small factor                      */

static int   grow_state = 0;            /* Current state (values -1, 0, +1)  */

static void grow_init(const struct s_file *fp, int type)
{
    if (!got_orig)
    {
        grow_orig  = fp->uv->r;
        grow_goal  = grow_orig;
        grow_strt  = grow_orig;

        grow_state = 0;

        got_orig   = 1;
    }

    if (type == ITEM_SHRINK)
    {
        switch (grow_state)
        {
        case -1:
            break;

        case  0:
            audio_play(AUD_SHRINK, 1.f);
            grow_goal = grow_orig * GROW_SMALL;
            grow_state = -1;
            grow = 1;
            break;

        case +1:
            audio_play(AUD_SHRINK, 1.f);
            grow_goal = grow_orig;
            grow_state = 0;
            grow = 1;
            break;
        }
    }
    else if (type == ITEM_GROW)
    {
        switch (grow_state)
        {
        case -1:
            audio_play(AUD_GROW, 1.f);
            grow_goal = grow_orig;
            grow_state = 0;
            grow = 1;
            break;

        case  0:
            audio_play(AUD_GROW, 1.f);
            grow_goal = grow_orig * GROW_BIG;
            grow_state = +1;
            grow = 1;
            break;

        case +1:
            break;
        }
    }

    if (grow)
    {
        grow_t = 0.0;
        grow_strt = fp->uv->r;
    }
}

static void grow_step(const struct s_file *fp, float dt)
{
    float dr;

    if (!grow)
        return;

    /* Calculate new size based on how long since you touched the coin... */

    grow_t += dt;

    if (grow_t >= GROW_TIME)
    {
        grow = 0;
        grow_t = GROW_TIME;
    }

    dr = grow_strt + ((grow_goal-grow_strt) * (1.0f / (GROW_TIME / grow_t)));

    /* No sinking through the floor! Keeps ball's bottom constant. */

    fp->uv->p[1] += (dr - fp->uv->r);
    fp->uv->r     =  dr;

    game_cmd_ballradius();
}

/*---------------------------------------------------------------------------*/

static void view_init(void)
{
    view_dp  = (float) config_get_d(CONFIG_VIEW_DP) / 100.0f;
    view_dc  = (float) config_get_d(CONFIG_VIEW_DC) / 100.0f;
    view_dz  = (float) config_get_d(CONFIG_VIEW_DZ) / 100.0f;
    view_k   = 1.0f;
    view_a   = 0.0f;

    view_c[0] = 0.f;
    view_c[1] = view_dc;
    view_c[2] = 0.f;

    view_p[0] =     0.f;
    view_p[1] = view_dp;
    view_p[2] = view_dz;

    view_e[0][0] = 1.f;
    view_e[0][1] = 0.f;
    view_e[0][2] = 0.f;
    view_e[1][0] = 0.f;
    view_e[1][1] = 1.f;
    view_e[1][2] = 0.f;
    view_e[2][0] = 0.f;
    view_e[2][1] = 0.f;
    view_e[2][2] = 1.f;
}

int game_server_init(const char *file_name, int t, int e)
{
    struct
    {
        int x, y;
    } version;

    int i;

    timer      = (float) t / 100.f;
    timer_down = (t > 0);
    coins      = 0;
    status     = GAME_NONE;

    if (server_state)
        game_server_free();

    if (!sol_load_only_file(&file, file_name))
        return (server_state = 0);

    server_state = 1;

    version.x = 0;
    version.y = 0;

    for (i = 0; i < file.dc; i++)
    {
        char *k = file.av + file.dv[i].ai;
        char *v = file.av + file.dv[i].aj;

        if (strcmp(k, "version") == 0)
            sscanf(v, "%d.%d", &version.x, &version.y);
    }

    input_init();

    game_tilt_init(&tilt);

    /* Initialize jump and goal states. */

    jump_e = 1;
    jump_b = 0;

    goal_e = e ? 1    : 0;
    goal_k = e ? 1.0f : 0.0f;

    /* Initialize the view. */

    view_init();

    /* Initialize ball size tracking... */

    got_orig = 0;
    grow = 0;

    sol_cmd_enq_func(game_proxy_enq);

    /* Queue client commands. */

    game_cmd_map(file_name, version.x, version.y);
    game_cmd_ups();
    game_cmd_timer();

    if (goal_e) game_cmd_goalopen();

    game_cmd_init_balls();
    game_cmd_init_items();

    return server_state;
}

void game_server_free(void)
{
    if (server_state)
    {
        sol_free(&file);
        server_state = 0;
    }
}

/*---------------------------------------------------------------------------*/

static void game_update_view(float dt)
{
    static int view_prev;

    float dc = view_dc * (jump_b ? 2.0f * fabsf(jump_dt - 0.5f) : 1.0f);
    float da = input_get_r() * dt * 90.0f;
    float k;

    float M[16], v[3], Y[3] = { 0.0f, 1.0f, 0.0f };

    /* Center the view about the ball. */

    v_cpy(view_c, file.uv->p);

    view_v[0] = -file.uv->v[0];
    view_v[1] =  0.0f;
    view_v[2] = -file.uv->v[2];

    /* Restore usable vectors. */

    if (view_prev == VIEW_TOPDOWN)
    {
        /* View basis. */

        v_inv(view_e[2], view_e[1]);
        v_cpy(view_e[1], Y);

        /* View position. */

        v_scl(v,    view_e[1], view_dp);
        v_mad(v, v, view_e[2], view_dz);
        v_add(view_p, v, file.uv->p);
    }

    view_prev = input_get_c();

    switch (input_get_c())
    {
    case VIEW_LAZY: /* Viewpoint chases the ball position. */

        v_sub(view_e[2], view_p, view_c);

        break;

    case VIEW_MANUAL:  /* View vector is given by view angle. */
    case VIEW_TOPDOWN: /* Crude top-down view. */

        view_e[2][0] = fsinf(V_RAD(view_a));
        view_e[2][1] = 0.0;
        view_e[2][2] = fcosf(V_RAD(view_a));

        break;

    case VIEW_CHASE: /* View vector approaches the ball velocity vector. */

        v_sub(view_e[2], view_p, view_c);
        v_nrm(view_e[2], view_e[2]);
        v_mad(view_e[2], view_e[2], view_v, v_dot(view_v, view_v) * dt / 4);

        break;
    }

    /* Apply manual rotation. */

    m_rot(M, Y, V_RAD(da));
    m_vxfm(view_e[2], M, view_e[2]);

    /* Orthonormalize the new view reference frame. */

    v_crs(view_e[0], view_e[1], view_e[2]);
    v_crs(view_e[2], view_e[0], view_e[1]);
    v_nrm(view_e[0], view_e[0]);
    v_nrm(view_e[2], view_e[2]);

    /* Compute the new view position. */

    k = 1.0f + v_dot(view_e[2], view_v) / 10.0f;

    view_k = view_k + (k - view_k) * dt;

    if (view_k < 0.5) view_k = 0.5;

    v_scl(v,    view_e[1], view_dp * view_k);
    v_mad(v, v, view_e[2], view_dz * view_k);
    v_add(view_p, v, file.uv->p);

    /* Compute the new view center. */

    v_cpy(view_c, file.uv->p);
    v_mad(view_c, view_c, view_e[1], dc);

    /* Note the current view angle. */

    view_a = V_DEG(fatan2f(view_e[2][0], view_e[2][2]));

    /* Override vectors for top-down view. */

    if (input_get_c() == VIEW_TOPDOWN)
    {
        v_inv(view_e[1], view_e[2]);
        v_cpy(view_e[2], Y);

        v_cpy(view_c, file.uv->p);
        v_mad(view_p, view_c, view_e[2], view_dz * 1.5f);
    }

    game_cmd_updview();
}

static void game_update_time(float dt, int b)
{
    if (goal_e && goal_k < 1.0f)
        goal_k += dt;

   /* The ticking clock. */

    if (b && timer_down)
    {
        if (timer < 600.f)
            timer -= dt;
        if (timer < 0.f)
            timer = 0.f;
    }
    else if (b)
    {
        timer += dt;
    }

    if (b) game_cmd_timer();
}

static int game_update_state(int bt)
{
    struct s_file *fp = &file;
    struct s_goal *zp;
    int hi;

    float p[3];

    /* Test for an item. */

    if (bt && (hi = sol_item_test(fp, p, ITEM_RADIUS)) != -1)
    {
        struct s_item *hp = &file.hv[hi];

        game_cmd_pkitem(hi);

        grow_init(fp, hp->t);

        if (hp->t == ITEM_COIN)
        {
            coins += hp->n;
            game_cmd_coins();
        }

        audio_play(AUD_COIN, 1.f);

        /* Discard item. */

        hp->t = ITEM_NONE;
    }

    /* Test for a switch. */

    if (sol_swch_test(fp, 0))
        audio_play(AUD_SWITCH, 1.f);

    /* Test for a jump. */

    if (jump_e == 1 && jump_b == 0 && sol_jump_test(fp, jump_p, 0) == 1)
    {
        jump_b  = 1;
        jump_e  = 0;
        jump_dt = 0.f;

        v_sub(jump_w, jump_p, fp->uv->p);
        v_add(jump_w, view_p, jump_w);

        audio_play(AUD_JUMP, 1.f);

        game_cmd_jump(1);
    }
    if (jump_e == 0 && jump_b == 0 && sol_jump_test(fp, jump_p, 0) == 0)
    {
        jump_e = 1;
        game_cmd_jump(0);
    }

    /* Test for a goal. */

    if (bt && goal_e && (zp = sol_goal_test(fp, p, 0)))
    {
        audio_play(AUD_GOAL, 1.0f);
        return GAME_GOAL;
    }

    /* Test for time-out. */

    if (bt && timer_down && timer <= 0.f)
    {
        audio_play(AUD_TIME, 1.0f);
        return GAME_TIME;
    }

    /* Test for fall-out. */

    if (bt && fp->uv[0].p[1] < fp->vv[0].p[1])
    {
        audio_play(AUD_FALL, 1.0f);
        return GAME_FALL;
    }

    return GAME_NONE;
}

static int game_step(const float g[3], float dt, int bt)
{
    if (server_state)
    {
        struct s_file *fp = &file;

        float h[3];

        /* Smooth jittery or discontinuous input. */

        tilt.rx += (input_get_x() - tilt.rx) * dt / RESPONSE;
        tilt.rz += (input_get_z() - tilt.rz) * dt / RESPONSE;

        game_tilt_axes(&tilt, view_e);

        game_cmd_tiltaxes();
        game_cmd_tiltangles();

        grow_step(fp, dt);

        game_tilt_grav(h, g, &tilt);

        if (jump_b)
        {
            jump_dt += dt;

            /* Handle a jump. */

            if (0.5f < jump_dt)
            {
                v_cpy(fp->uv->p, jump_p);
                v_cpy(view_p,    jump_w);
            }
            if (1.0f < jump_dt)
                jump_b = 0;
        }
        else
        {
            /* Run the sim. */

            float b = sol_step(fp, h, dt, 0, NULL);

            /* Mix the sound of a ball bounce. */

            if (b > 0.5f)
            {
                float k = (b - 0.5f) * 2.0f;

                if (got_orig)
                {
                    if      (fp->uv->r > grow_orig) audio_play(AUD_BUMPL, k);
                    else if (fp->uv->r < grow_orig) audio_play(AUD_BUMPS, k);
                    else                            audio_play(AUD_BUMPM, k);
                }
                else audio_play(AUD_BUMPM, k);
            }
        }

        game_cmd_updball();

        game_update_view(dt);
        game_update_time(dt, bt);

        return game_update_state(bt);
    }
    return GAME_NONE;
}

void game_server_step(float dt)
{
    static const float gup[] = { 0.0f, +9.8f, 0.0f };
    static const float gdn[] = { 0.0f, -9.8f, 0.0f };

    switch (status)
    {
    case GAME_GOAL: game_step(gup, dt, 0); break;
    case GAME_FALL: game_step(gdn, dt, 0); break;

    case GAME_NONE:
        if ((status = game_step(gdn, dt, 1)) != GAME_NONE)
            game_cmd_status();
        break;
    }

    game_cmd_eou();
}

/*---------------------------------------------------------------------------*/

void game_set_goal(void)
{
    audio_play(AUD_SWITCH, 1.0f);
    goal_e = 1;

    game_cmd_goalopen();
}

void game_clr_goal(void)
{
    goal_e = 0;
}

/*---------------------------------------------------------------------------*/

void game_set_x(int k)
{
    input_set_x(-ANGLE_BOUND * k / JOY_MAX);
}

void game_set_z(int k)
{
    input_set_z(+ANGLE_BOUND * k / JOY_MAX);
}

void game_set_ang(int x, int z)
{
    input_set_x(x);
    input_set_z(z);
}

void game_set_pos(int x, int y)
{
    input_set_x(input_get_x() + 40.0f * y / config_get_d(CONFIG_MOUSE_SENSE));
    input_set_z(input_get_z() + 40.0f * x / config_get_d(CONFIG_MOUSE_SENSE));
}

void game_set_cam(int c)
{
    input_set_c(c);
}

void game_set_rot(float r)
{
    input_set_r(r);
}

void game_set_fly(float k, const struct s_file *fp)
{
    float  x[3] = { 1.f, 0.f, 0.f };
    float  y[3] = { 0.f, 1.f, 0.f };
    float  z[3] = { 0.f, 0.f, 1.f };
    float c0[3] = { 0.f, 0.f, 0.f };
    float p0[3] = { 0.f, 0.f, 0.f };
    float c1[3] = { 0.f, 0.f, 0.f };
    float p1[3] = { 0.f, 0.f, 0.f };
    float  v[3];

    if (!fp) fp = &file;

    view_init();

    z[0] = fsinf(V_RAD(view_a));
    z[2] = fcosf(V_RAD(view_a));

    v_cpy(view_e[0], x);
    v_cpy(view_e[1], y);
    v_cpy(view_e[2], z);

    /* k = 0.0 view is at the ball. */

    if (fp->uc > 0)
    {
        v_cpy(c0, fp->uv[0].p);
        v_cpy(p0, fp->uv[0].p);
    }

    v_mad(p0, p0, y, view_dp);
    v_mad(p0, p0, z, view_dz);
    v_mad(c0, c0, y, view_dc);

    /* k = +1.0 view is s_view 0 */

    if (k >= 0 && fp->wc > 0)
    {
        v_cpy(p1, fp->wv[0].p);
        v_cpy(c1, fp->wv[0].q);
    }

    /* k = -1.0 view is s_view 1 */

    if (k <= 0 && fp->wc > 1)
    {
        v_cpy(p1, fp->wv[1].p);
        v_cpy(c1, fp->wv[1].q);
    }

    /* Interpolate the views. */

    v_sub(v, p1, p0);
    v_mad(view_p, p0, v, k * k);

    v_sub(v, c1, c0);
    v_mad(view_c, c0, v, k * k);

    /* Orthonormalize the view basis. */

    v_sub(view_e[2], view_p, view_c);
    v_crs(view_e[0], view_e[1], view_e[2]);
    v_crs(view_e[2], view_e[0], view_e[1]);
    v_nrm(view_e[0], view_e[0]);
    v_nrm(view_e[2], view_e[2]);

    game_cmd_updview();
}

/*---------------------------------------------------------------------------*/
