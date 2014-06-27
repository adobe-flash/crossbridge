/* Copyright (C) 1989, 1990 Aladdin Enterprises.  All rights reserved.
   Distributed by Free Software Foundation, Inc.

This file is part of Ghostscript.

Ghostscript is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY.  No author or distributor accepts responsibility
to anyone for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.  Refer
to the Ghostscript General Public License for full details.

Everyone is granted permission to copy, modify and redistribute
Ghostscript, but only under the conditions described in the Ghostscript
General Public License.  A copy of this license is supposed to have been
given to you along with Ghostscript so you can know your rights and
responsibilities.  It should be in a file named COPYING.  Among other
things, the copyright notice and this notice must be preserved on all
copies.  */

/* gxmatrix.h */
/* Internal matrix routines for GhostScript library */
#include "gsmatrix.h"

/* A matrix with a cached fixed-point copy of the translation. */
/* This is only used by a few routines; they are responsible */
/* for ensuring the validity of the cache. */
typedef struct gs_matrix_fixed_s {
	_matrix_body;
	fixed tx_fixed, ty_fixed;
} gs_matrix_fixed;

/* Coordinate transformations to fixed point */
int	gs_point_transform2fixed(P4(gs_matrix_fixed *, floatp, floatp, gs_fixed_point *)),
	gs_distance_transform2fixed(P4(gs_matrix_fixed *, floatp, floatp, gs_fixed_point *));

/* Macro for testing whether matrix coefficients are zero, */
/* for shortcuts when the matrix has no skew. */
#define is_skewed(pmat) !is_fzero2((pmat)->xy, (pmat)->yx)
