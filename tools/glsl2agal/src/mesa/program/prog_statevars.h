/*
 * Mesa 3-D graphics library
 * Version:  7.1
 *
 * Copyright (C) 1999-2007  Brian Paul   All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef PROG_STATEVARS_H
#define PROG_STATEVARS_H

#include "main/glheader.h"

struct gl_context;
struct gl_program_parameter_list;

/**
 * Number of STATE_* values we need to address any GL state.
 * Used to dimension arrays.
 */
#define STATE_LENGTH 5


/**
 * Used for describing GL state referenced from inside ARB vertex and
 * fragment programs.
 * A string such as "state.light[0].ambient" gets translated into a
 * sequence of tokens such as [ STATE_LIGHT, 0, STATE_AMBIENT ].
 *
 * For state that's an array, like STATE_CLIPPLANE, the 2nd token [1] should
 * always be the array index.
 */
typedef enum gl_state_index_ {
   STATE_MATERIAL = 100,  /* start at 100 so small ints are seen as ints */

   STATE_LIGHT,
   STATE_LIGHTMODEL_AMBIENT,
   STATE_LIGHTMODEL_SCENECOLOR,
   STATE_LIGHTPROD,

   STATE_TEXGEN,

   STATE_FOG_COLOR,
   STATE_FOG_PARAMS,

   STATE_CLIPPLANE,

   STATE_POINT_SIZE,
   STATE_POINT_ATTENUATION,

   STATE_MODELVIEW_MATRIX,
   STATE_PROJECTION_MATRIX,
   STATE_MVP_MATRIX,
   STATE_TEXTURE_MATRIX,
   STATE_PROGRAM_MATRIX,
   STATE_MATRIX_INVERSE,
   STATE_MATRIX_TRANSPOSE,
   STATE_MATRIX_INVTRANS,

   STATE_AMBIENT,
   STATE_DIFFUSE,
   STATE_SPECULAR,
   STATE_EMISSION,
   STATE_SHININESS,
   STATE_HALF_VECTOR,

   STATE_POSITION,       /**< xyzw = position */
   STATE_ATTENUATION,    /**< xyz = attenuation, w = spot exponent */
   STATE_SPOT_DIRECTION, /**< xyz = direction, w = cos(cutoff) */
   STATE_SPOT_CUTOFF,    /**< x = cutoff, yzw = undefined */

   STATE_TEXGEN_EYE_S,
   STATE_TEXGEN_EYE_T,
   STATE_TEXGEN_EYE_R,
   STATE_TEXGEN_EYE_Q,
   STATE_TEXGEN_OBJECT_S,
   STATE_TEXGEN_OBJECT_T,
   STATE_TEXGEN_OBJECT_R,
   STATE_TEXGEN_OBJECT_Q,

   STATE_TEXENV_COLOR,

   STATE_DEPTH_RANGE,

   STATE_VERTEX_PROGRAM,
   STATE_FRAGMENT_PROGRAM,

   STATE_ENV,
   STATE_LOCAL,

   STATE_INTERNAL,		/* Mesa additions */
   STATE_CURRENT_ATTRIB,        /* ctx->Current vertex attrib value */
   STATE_CURRENT_ATTRIB_MAYBE_VP_CLAMPED,        /* ctx->Current vertex attrib value after passthrough vertex processing */
   STATE_NORMAL_SCALE,
   STATE_TEXRECT_SCALE,
   STATE_FOG_PARAMS_OPTIMIZED,  /* for faster fog calc */
   STATE_POINT_SIZE_CLAMPED,    /* includes implementation dependent size clamp */
   STATE_POINT_SIZE_IMPL_CLAMP, /* for implementation clamp only in vs */
   STATE_LIGHT_SPOT_DIR_NORMALIZED,   /* pre-normalized spot dir */
   STATE_LIGHT_POSITION,              /* object vs eye space */
   STATE_LIGHT_POSITION_NORMALIZED,   /* object vs eye space */
   STATE_LIGHT_HALF_VECTOR,           /* object vs eye space */
   STATE_PT_SCALE,              /**< Pixel transfer RGBA scale */
   STATE_PT_BIAS,               /**< Pixel transfer RGBA bias */
   STATE_SHADOW_AMBIENT,        /**< ARB_shadow_ambient fail value; token[2] is texture unit index */
   STATE_FB_SIZE,               /**< (width-1, height-1, 0, 0) */
   STATE_FB_WPOS_Y_TRANSFORM,   /**< (1, 0, -1, height) if a FBO is bound, (-1, height, 1, 0) otherwise */
   STATE_ROT_MATRIX_0,          /**< ATI_envmap_bumpmap, rot matrix row 0 */
   STATE_ROT_MATRIX_1,          /**< ATI_envmap_bumpmap, rot matrix row 1 */
   STATE_INTERNAL_DRIVER	/* first available state index for drivers (must be last) */
} gl_state_index;



extern void
_mesa_load_state_parameters(struct gl_context *ctx,
                            struct gl_program_parameter_list *paramList);


extern GLbitfield
_mesa_program_state_flags(const gl_state_index state[STATE_LENGTH]);


extern char *
_mesa_program_state_string(const gl_state_index state[STATE_LENGTH]);


extern void
_mesa_load_tracked_matrices(struct gl_context *ctx);


#endif /* PROG_STATEVARS_H */
