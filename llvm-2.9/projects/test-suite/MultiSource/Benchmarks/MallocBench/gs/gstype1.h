/* Copyright (C) 1990 Aladdin Enterprises.  All rights reserved.
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

/* gstype1.h */
/* Client interface to Adobe Type 1 font rouines for GhostScript library */

/* Encrypt/decrypt procedures */
typedef ushort crypt_state;
#define crypt_charstring_seed 4330
int gs_type1_encrypt(P4(byte *dest, byte *src, uint len, crypt_state *));
int gs_type1_decrypt(P4(byte *dest, byte *src, uint len, crypt_state *));

/* CharString interpreter */
typedef struct gs_type1_state_s gs_type1_state;
extern int gs_type1_state_sizeof;
struct gs_show_enum_s;
struct gs_type1_data_s;
int gs_type1_init(P6(gs_type1_state *pis, struct gs_show_enum_s *penum,
		  int charpath_flag, int paint_type,
		  byte *charstring, struct gs_type1_data_s *pdata));
/* Continue interpreting a Type 1 CharString. */
/* If str != 0, it is taken as the byte string to interpret. */
/* Return 0 on successful completion, <0 on error, */
/* (code<<1)+1 for seac, or (N+1)<<1 for callothersubr(N). */
int gs_type1_interpret(P2(gs_type1_state *, byte *));
/* Pop a (fixed) number off the internal stack */
int gs_type1_pop(P2(gs_type1_state *, fixed *));
