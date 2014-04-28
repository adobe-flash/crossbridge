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

#ifndef VEC_H
#define VEC_H

#include <math.h>

#define V_PI 3.1415927f

#define V_RAD(d) (d * V_PI / 180.f)
#define V_DEG(r) (r * 180.f / V_PI)

#define fsinf(a)      ((float) sin((double) a))
#define fcosf(a)      ((float) cos((double) a))
#define ftanf(a)      ((float) tan((double) a))
#define fabsf(a)      ((float) fabs((double) a))
#define fsqrtf(a)     ((float) sqrt((double) a))
#define facosf(a)     ((float) acos((double) a))
#define fmodf(x,y)    ((float) fmod((double) x, (double) y))
#define fatan2f(x, y) ((float) atan2((double) x, (double) y))

/*---------------------------------------------------------------------------*/

#define v_dot(u, v)  ((u)[0] * (v)[0] + (u)[1] * (v)[1] + (u)[2] * (v)[2])
#define v_len(u)     fsqrtf(v_dot(u, u))

#define v_cpy(u, v) { \
    (u)[0] = (v)[0];  \
    (u)[1] = (v)[1];  \
    (u)[2] = (v)[2];  \
}

#define v_inv(u, v) { \
    (u)[0] = -(v)[0]; \
    (u)[1] = -(v)[1]; \
    (u)[2] = -(v)[2]; \
}

#define v_scl(u, v, k) {   \
    (u)[0] = (v)[0] * (k); \
    (u)[1] = (v)[1] * (k); \
    (u)[2] = (v)[2] * (k); \
}

#define v_add(u, v, w) {      \
    (u)[0] = (v)[0] + (w)[0]; \
    (u)[1] = (v)[1] + (w)[1]; \
    (u)[2] = (v)[2] + (w)[2]; \
}

#define v_sub(u, v, w) {      \
    (u)[0] = (v)[0] - (w)[0]; \
    (u)[1] = (v)[1] - (w)[1]; \
    (u)[2] = (v)[2] - (w)[2]; \
}

#define v_mid(u, v, w) {              \
    (u)[0] = ((v)[0] + (w)[0]) / 2.f; \
    (u)[1] = ((v)[1] + (w)[1]) / 2.f; \
    (u)[2] = ((v)[2] + (w)[2]) / 2.f; \
}

#define v_mad(u, p, v, t) {         \
    (u)[0] = (p)[0] + (v)[0] * (t); \
    (u)[1] = (p)[1] + (v)[1] * (t); \
    (u)[2] = (p)[2] + (v)[2] * (t); \
}

/*---------------------------------------------------------------------------*/


void   v_nrm(float *, const float *);
void   v_crs(float *, const float *, const float *);

void   m_cpy(float *, const float *);
void   m_xps(float *, const float *);
int    m_inv(float *, const float *);

void   m_ident(float *);
void   m_basis(float *, const float *,
                        const float *,
                        const float *);
void   m_xlt(float *, const float *);
void   m_scl(float *, const float *);
void   m_rot(float *, const float *, float);

void   m_mult(float *, const float *, const float *);
void   m_pxfm(float *, const float *, const float *);
void   m_vxfm(float *, const float *, const float *);

void   m_view(float *, const float *,
                       const float *,
                       const float *);

#endif
