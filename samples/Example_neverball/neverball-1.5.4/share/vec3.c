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

#include <stdio.h>
#include <math.h>

#include "vec3.h"

#define A 10
#define B 11
#define C 12
#define D 13
#define E 14
#define F 15

#define TINY 1e-5

/*---------------------------------------------------------------------------*/

void v_nrm(float *n, const float *v)
{
    float d = v_len(v);

    n[0] = v[0] / d;
    n[1] = v[1] / d;
    n[2] = v[2] / d;
}

void v_crs(float *u, const float *v, const float *w)
{
    u[0] = v[1] * w[2] - v[2] * w[1];
    u[1] = v[2] * w[0] - v[0] * w[2];
    u[2] = v[0] * w[1] - v[1] * w[0];
}

/*---------------------------------------------------------------------------*/

void m_cpy(float *M, const float *N)
{
    M[0] = N[0]; M[1] = N[1]; M[2] = N[2]; M[3] = N[3];
    M[4] = N[4]; M[5] = N[5]; M[6] = N[6]; M[7] = N[7];
    M[8] = N[8]; M[9] = N[9]; M[A] = N[A]; M[B] = N[B];
    M[C] = N[C]; M[D] = N[D]; M[E] = N[E]; M[F] = N[F];
}

void m_xps(float *M, const float *N)
{
    M[0] = N[0]; M[1] = N[4]; M[2] = N[8]; M[3] = N[C];
    M[4] = N[1]; M[5] = N[5]; M[6] = N[9]; M[7] = N[D];
    M[8] = N[2]; M[9] = N[6]; M[A] = N[A]; M[B] = N[E];
    M[C] = N[3]; M[D] = N[7]; M[E] = N[B]; M[F] = N[F];
}

int  m_inv(float *I, const float *N)
{
    double T[16], M[16];
    double d;
    int i;

    for (i = 0; i < 16; i++)
        M[i] = N[i];

    T[0] = +(M[5] * (M[A] * M[F] - M[B] * M[E]) -
             M[9] * (M[6] * M[F] - M[7] * M[E]) +
             M[D] * (M[6] * M[B] - M[7] * M[A]));
    T[1] = -(M[4] * (M[A] * M[F] - M[B] * M[E]) -
             M[8] * (M[6] * M[F] - M[7] * M[E]) +
             M[C] * (M[6] * M[B] - M[7] * M[A]));
    T[2] = +(M[4] * (M[9] * M[F] - M[B] * M[D]) -
             M[8] * (M[5] * M[F] - M[7] * M[D]) +
             M[C] * (M[5] * M[B] - M[7] * M[9]));
    T[3] = -(M[4] * (M[9] * M[E] - M[A] * M[D]) -
             M[8] * (M[5] * M[E] - M[6] * M[D]) +
             M[C] * (M[5] * M[A] - M[6] * M[9]));

    T[4] = -(M[1] * (M[A] * M[F] - M[B] * M[E]) -
             M[9] * (M[2] * M[F] - M[3] * M[E]) +
             M[D] * (M[2] * M[B] - M[3] * M[A]));
    T[5] = +(M[0] * (M[A] * M[F] - M[B] * M[E]) -
             M[8] * (M[2] * M[F] - M[3] * M[E]) +
             M[C] * (M[2] * M[B] - M[3] * M[A]));
    T[6] = -(M[0] * (M[9] * M[F] - M[B] * M[D]) -
             M[8] * (M[1] * M[F] - M[3] * M[D]) +
             M[C] * (M[1] * M[B] - M[3] * M[9]));
    T[7] = +(M[0] * (M[9] * M[E] - M[A] * M[D]) -
             M[8] * (M[1] * M[E] - M[2] * M[D]) +
             M[C] * (M[1] * M[A] - M[2] * M[9]));

    T[8] = +(M[1] * (M[6] * M[F] - M[7] * M[E]) -
             M[5] * (M[2] * M[F] - M[3] * M[E]) +
             M[D] * (M[2] * M[7] - M[3] * M[6]));
    T[9] = -(M[0] * (M[6] * M[F] - M[7] * M[E]) -
             M[4] * (M[2] * M[F] - M[3] * M[E]) +
             M[C] * (M[2] * M[7] - M[3] * M[6]));
    T[A] = +(M[0] * (M[5] * M[F] - M[7] * M[D]) -
             M[4] * (M[1] * M[F] - M[3] * M[D]) +
             M[C] * (M[1] * M[7] - M[3] * M[5]));
    T[B] = -(M[0] * (M[5] * M[E] - M[6] * M[D]) -
             M[4] * (M[1] * M[E] - M[2] * M[D]) +
             M[C] * (M[1] * M[6] - M[2] * M[5]));

    T[C] = -(M[1] * (M[6] * M[B] - M[7] * M[A]) -
             M[5] * (M[2] * M[B] - M[3] * M[A]) +
             M[9] * (M[2] * M[7] - M[3] * M[6]));
    T[D] = +(M[0] * (M[6] * M[B] - M[7] * M[A]) -
             M[4] * (M[2] * M[B] - M[3] * M[A]) +
             M[8] * (M[2] * M[7] - M[3] * M[6]));
    T[E] = -(M[0] * (M[5] * M[B] - M[7] * M[9]) -
             M[4] * (M[1] * M[B] - M[3] * M[9]) +
             M[8] * (M[1] * M[7] - M[3] * M[5]));
    T[F] = +(M[0] * (M[5] * M[A] - M[6] * M[9]) -
             M[4] * (M[1] * M[A] - M[2] * M[9]) +
             M[8] * (M[1] * M[6] - M[2] * M[5]));

    d = M[0] * T[0] + M[4] * T[4] + M[8] * T[8] + M[C] * T[C];

    if (fabs(d) > TINY)
    {
        I[0] = T[0] / d;
        I[1] = T[4] / d;
        I[2] = T[8] / d;
        I[3] = T[C] / d;
        I[4] = T[1] / d;
        I[5] = T[5] / d;
        I[6] = T[9] / d;
        I[7] = T[D] / d;
        I[8] = T[2] / d;
        I[9] = T[6] / d;
        I[A] = T[A] / d;
        I[B] = T[E] / d;
        I[C] = T[3] / d;
        I[D] = T[7] / d;
        I[E] = T[B] / d;
        I[F] = T[F] / d;

        return 1;
    }
    return 0;
}

/*---------------------------------------------------------------------------*/

void m_ident(float *M)
{
    M[0] = 1.f; M[4] = 0.f; M[8] = 0.f; M[C] = 0.f;
    M[1] = 0.f; M[5] = 1.f; M[9] = 0.f; M[D] = 0.f;
    M[2] = 0.f; M[6] = 0.f; M[A] = 1.f; M[E] = 0.f;
    M[3] = 0.f; M[7] = 0.f; M[B] = 0.f; M[F] = 1.f;
}

void m_basis(float *M,
             const float e0[3],
             const float e1[3],
             const float e2[3])
{
    M[0] = e0[0]; M[4] = e1[0]; M[8] = e2[0]; M[C] = 0.f;
    M[1] = e0[1]; M[5] = e1[1]; M[9] = e2[1]; M[D] = 0.f;
    M[2] = e0[2]; M[6] = e1[2]; M[A] = e2[2]; M[E] = 0.f;
    M[3] =   0.f; M[7] =   0.f; M[B] =   0.f; M[F] = 1.f;
}

/*---------------------------------------------------------------------------*/

void m_xlt(float *M, const float *v)
{
    M[0] = 1.f; M[4] = 0.f; M[8] = 0.f; M[C] = v[0];
    M[1] = 0.f; M[5] = 1.f; M[9] = 0.f; M[D] = v[1];
    M[2] = 0.f; M[6] = 0.f; M[A] = 1.f; M[E] = v[2];
    M[3] = 0.f; M[7] = 0.f; M[B] = 0.f; M[F] =  1.f;
}

void m_scl(float *M, const float *v)
{
    M[0] = v[0]; M[4] =  0.f; M[8] =  0.f; M[C] = 0.f;
    M[1] =  0.f; M[5] = v[1]; M[9] =  0.f; M[D] = 0.f;
    M[2] =  0.f; M[6] =  0.f; M[A] = v[2]; M[E] = 0.f;
    M[3] =  0.f; M[7] =  0.f; M[B] =  0.f; M[F] = 1.f;
}

void m_rot(float *M, const float *v, float a)
{
    float u[3];
    float U[16];
    float S[16];

    const float s = fsinf(a);
    const float c = fcosf(a);

    v_nrm(u, v);

    U[0] = u[0] * u[0]; U[4] = u[0] * u[1]; U[8] = u[0] * u[2];
    U[1] = u[1] * u[0]; U[5] = u[1] * u[1]; U[9] = u[1] * u[2];
    U[2] = u[2] * u[0]; U[6] = u[2] * u[1]; U[A] = u[2] * u[2];

    S[0] =   0.f; S[4] = -u[2]; S[8] =  u[1];
    S[1] =  u[2]; S[5] =   0.f; S[9] = -u[0];
    S[2] = -u[1]; S[6] =  u[0]; S[A] =   0.f;

    M[0] = U[0] + c * (1.f - U[0]) + s * S[0];
    M[1] = U[1] + c * (0.f - U[1]) + s * S[1];
    M[2] = U[2] + c * (0.f - U[2]) + s * S[2];
    M[3] = 0.f;
    M[4] = U[4] + c * (0.f - U[4]) + s * S[4];
    M[5] = U[5] + c * (1.f - U[5]) + s * S[5];
    M[6] = U[6] + c * (0.f - U[6]) + s * S[6];
    M[7] = 0.f;
    M[8] = U[8] + c * (0.f - U[8]) + s * S[8];
    M[9] = U[9] + c * (0.f - U[9]) + s * S[9];
    M[A] = U[A] + c * (1.f - U[A]) + s * S[A];
    M[B] = 0.f;
    M[C] = 0.f;
    M[D] = 0.f;
    M[E] = 0.f;
    M[F] = 1.f;
}

/*---------------------------------------------------------------------------*/

void m_mult(float *M, const float *N, const float *O)
{
    M[0] = N[0] * O[0] + N[4] * O[1] + N[8] * O[2] + N[C] * O[3];
    M[1] = N[1] * O[0] + N[5] * O[1] + N[9] * O[2] + N[D] * O[3];
    M[2] = N[2] * O[0] + N[6] * O[1] + N[A] * O[2] + N[E] * O[3];
    M[3] = N[3] * O[0] + N[7] * O[1] + N[B] * O[2] + N[F] * O[3];

    M[4] = N[0] * O[4] + N[4] * O[5] + N[8] * O[6] + N[C] * O[7];
    M[5] = N[1] * O[4] + N[5] * O[5] + N[9] * O[6] + N[D] * O[7];
    M[6] = N[2] * O[4] + N[6] * O[5] + N[A] * O[6] + N[E] * O[7];
    M[7] = N[3] * O[4] + N[7] * O[5] + N[B] * O[6] + N[F] * O[7];

    M[8] = N[0] * O[8] + N[4] * O[9] + N[8] * O[A] + N[C] * O[B];
    M[9] = N[1] * O[8] + N[5] * O[9] + N[9] * O[A] + N[D] * O[B];
    M[A] = N[2] * O[8] + N[6] * O[9] + N[A] * O[A] + N[E] * O[B];
    M[B] = N[3] * O[8] + N[7] * O[9] + N[B] * O[A] + N[F] * O[B];

    M[C] = N[0] * O[C] + N[4] * O[D] + N[8] * O[E] + N[C] * O[F];
    M[D] = N[1] * O[C] + N[5] * O[D] + N[9] * O[E] + N[D] * O[F];
    M[E] = N[2] * O[C] + N[6] * O[D] + N[A] * O[E] + N[E] * O[F];
    M[F] = N[3] * O[C] + N[7] * O[D] + N[B] * O[E] + N[F] * O[F];
}

void m_pxfm(float *v, const float *M, const float *w)
{
    float d = w[0] * M[3] + w[1] * M[7] + w[2] * M[B] + M[F];

    v[0] = (w[0] * M[0] + w[1] * M[4] + w[2] * M[8] + M[C]) / d;
    v[1] = (w[0] * M[1] + w[1] * M[5] + w[2] * M[9] + M[D]) / d;
    v[2] = (w[0] * M[2] + w[1] * M[6] + w[2] * M[A] + M[E]) / d;
}

void m_vxfm(float *v, const float *M, const float *w)
{
    v[0] = (w[0] * M[0] + w[1] * M[4] + w[2] * M[8]);
    v[1] = (w[0] * M[1] + w[1] * M[5] + w[2] * M[9]);
    v[2] = (w[0] * M[2] + w[1] * M[6] + w[2] * M[A]);
}

/*---------------------------------------------------------------------------*/

void m_view(float *M,
            const float *c,
            const float *p,
            const float *u)
{
    float x[3];
    float y[3];
    float z[3];

    v_sub(z, p, c);
    v_nrm(z, z);
    v_crs(x, u, z);
    v_nrm(x, x);
    v_crs(y, z, x);

    m_basis(M, x, y, z);
}

/*---------------------------------------------------------------------------*/
