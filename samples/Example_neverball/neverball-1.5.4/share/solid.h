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

#ifndef SOL_H
#define SOL_H

#include "glext.h"
#include "base_config.h"

/*
 * Some might  be taken  aback at  the terseness of  the names  of the
 * structure  members and  the variables  used by  the  functions that
 * access them.  Yes, yes, I know:  readability.  I  contend that once
 * the naming  convention is embraced, the names  become more readable
 * than any  verbose alternative, and their brevity  and uniformity do
 * more to augment readability than longVariableNames ever could.
 *
 * Members  and variables  are named  XY.   X determines  the type  of
 * structure to which the variable  refers.  Y determines the usage of
 * the variable.
 *
 * The Xs are as documented by struct s_file:
 *
 *     f  File          (struct s_file)
 *     m  Material      (struct s_mtrl)
 *     v  Vertex        (struct s_vert)
 *     e  Edge          (struct s_edge)
 *     s  Side          (struct s_side)
 *     t  Texture coord (struct s_texc)
 *     g  Geometry      (struct s_geom)
 *     l  Lump          (struct s_lump)
 *     n  Node          (struct s_node)
 *     p  Path          (struct s_path)
 *     b  Body          (struct s_body)
 *     h  Item          (struct s_item)
 *     z  Goal          (struct s_goal)
 *     j  Jump          (struct s_jump)
 *     x  Switch        (struct s_swch)
 *     r  Billboard     (struct s_bill)
 *     u  User          (struct s_ball)
 *     w  Viewpoint     (struct s_view)
 *     d  Dictionary    (struct s_dict)
 *     i  Index         (int)
 *     a  Text          (char)
 *
 * The Ys are as follows:
 *
 *     c  Counter
 *     p  Pointer
 *     v  Vector (array)
 *     0  Index of the first
 *     i  Index
 *     j  Subindex
 *     k  Subsubindex
 *
 * Thus "up" is a pointer to  a user structure.  "lc" is the number of
 * lumps.  "ei" and "ej" are  edge indices into some "ev" edge vector.
 * An edge is  defined by two vertices, so  an edge structure consists
 * of "vi" and "vj".  And so on.
 *
 * Those members that do not conform to this convention are explicitly
 * documented with a comment.
 *
 * These prefixes are still available: c k o q y.
 */

/*---------------------------------------------------------------------------*/

/* Material type flags */

#define M_OPAQUE       1
#define M_TRANSPARENT  2
#define M_REFLECTIVE   4
#define M_ENVIRONMENT  8
#define M_ADDITIVE    16
#define M_CLAMPED     32
#define M_DECAL       64
#define M_TWO_SIDED  128

/* Billboard types. */

#define B_EDGE     1
#define B_FLAT     2
#define B_ADDITIVE 4
#define B_NOFACE   8

/* Lump flags. */

#define L_DETAIL   1

/* Item types. */

#define ITEM_NONE       0
#define ITEM_COIN       1
#define ITEM_GROW       2
#define ITEM_SHRINK     3

/*---------------------------------------------------------------------------*/

struct s_mtrl
{
    float d[4];                                /* diffuse color              */
    float a[4];                                /* ambient color              */
    float s[4];                                /* specular color             */
    float e[4];                                /* emission color             */
    float h[1];                                /* specular exponent          */
    float angle;

    int fl;                                    /* material flags             */

    GLuint o;                                  /* OpenGL texture object      */
    char   f[PATHMAX];                         /* texture file name          */
};

struct s_vert
{
    float p[3];                                /* vertex position            */
};

struct s_edge
{
    int vi;
    int vj;
};

struct s_side
{
    float n[3];                                /* plane normal vector        */
    float d;                                   /* distance from origin       */
};

struct s_texc
{
    float u[2];                                /* texture coordinate         */
};

struct s_geom
{
    int mi;
    int ti, si, vi;
    int tj, sj, vj;
    int tk, sk, vk;
};

struct s_lump
{
    int fl;                                    /* lump flags                 */
    int v0, vc;
    int e0, ec;
    int g0, gc;
    int s0, sc;
};

struct s_node
{
    int si;
    int ni;
    int nj;
    int l0;
    int lc;
};

struct s_path
{
    float p[3];                                /* starting position          */
    float t;                                   /* travel time                */

    int pi;
    int f;                                     /* enable flag                */
    int s;                                     /* smooth flag                */
};

struct s_body
{
    float t;                                   /* time on current path       */

    GLuint ol;                                 /* opaque geometry list       */
    GLuint tl;                                 /* transparent geometry list  */
    GLuint rl;                                 /* reflective geometry list   */
    GLuint sl;                                 /* shadowed geometry list     */

    int pi;
    int ni;
    int l0;
    int lc;
    int g0;
    int gc;
};

struct s_item
{
    float p[3];                                /* position                   */
    int   t;                                   /* type                       */
    int   n;                                   /* value                      */
};

struct s_goal
{
    float p[3];                                /* position                   */
    float r;                                   /* radius                     */
};

struct s_swch
{
    float p[3];                                /* position                   */
    float r;                                   /* radius                     */
    int  pi;                                   /* the linked path            */

    float t0;                                  /* default timer              */
    float t;                                   /* current timer              */
    int   f0;                                  /* default state              */
    int   f;                                   /* current state              */
    int   i;                                   /* is invisible?              */
    int   e;                                   /* is a ball inside it?       */
};

struct s_bill
{
    int  fl;
    int  mi;
    float t;                                   /* repeat time interval       */
    float d;                                   /* distance                   */

    float w[3];                                /* width coefficients         */
    float h[3];                                /* height coefficients        */

    float rx[3];                               /* X rotation coefficients    */
    float ry[3];                               /* Y rotation coefficients    */
    float rz[3];                               /* Z rotation coefficients    */

    float p[3];
};

struct s_jump
{
    float p[3];                                /* position                   */
    float q[3];                                /* target position            */
    float r;                                   /* radius                     */
};

struct s_ball
{
    float e[3][3];                             /* basis of orientation       */
    float p[3];                                /* position vector            */
    float v[3];                                /* velocity vector            */
    float w[3];                                /* angular velocity vector    */
    float E[3][3];                             /* basis of pendulum          */
    float W[3];                                /* angular pendulum velocity  */
    float r;                                   /* radius                     */
};

struct s_view
{
    float p[3];
    float q[3];
};

struct s_dict
{
    int ai;
    int aj;
};

struct s_file
{
    int ac;
    int mc;
    int vc;
    int ec;
    int sc;
    int tc;
    int gc;
    int lc;
    int nc;
    int pc;
    int bc;
    int hc;
    int zc;
    int jc;
    int xc;
    int rc;
    int uc;
    int wc;
    int dc;
    int ic;

    char          *av;
    struct s_mtrl *mv;
    struct s_vert *vv;
    struct s_edge *ev;
    struct s_side *sv;
    struct s_texc *tv;
    struct s_geom *gv;
    struct s_lump *lv;
    struct s_node *nv;
    struct s_path *pv;
    struct s_body *bv;
    struct s_item *hv;
    struct s_goal *zv;
    struct s_jump *jv;
    struct s_swch *xv;
    struct s_bill *rv;
    struct s_ball *uv;
    struct s_view *wv;
    struct s_dict *dv;
    int           *iv;
};

/*---------------------------------------------------------------------------*/

int   sol_load_only_file(struct s_file *, const char *);
int   sol_load_only_head(struct s_file *, const char *);
int   sol_stor(struct s_file *, const char *);
void  sol_free(struct s_file *);

/*---------------------------------------------------------------------------*/

#endif
