/* crtend object for windows32 targets.
   Copyright (C) 2007  Free Software Foundation, Inc.

   Contributed by Danny Smith <dannysmith@users.sourceforge.net>

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2, or (at your option) any later
version.

In addition to the permissions in the GNU General Public License, the
Free Software Foundation gives you unlimited permission to link the
compiled version of this file into combinations with other programs,
and to distribute those combinations without any restriction coming
from the use of this file.  (The General Public License restrictions
do apply in other respects; for example, they cover modification of
the file, and distribution when not linked into a combine
executable.)

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

/* Target machine header files require this define. */
#define IN_LIBGCC2

/* auto-host.h is needed by cygming.h for HAVE_GAS_WEAK and here
   for HAVE_LD_RO_RW_SECTION_MIXING.  */  
#include "auto-host.h"
#include "tconfig.h"
#include "tsystem.h"
#include "coretypes.h"
#include "tm.h"
#include "unwind-dw2-fde.h"

#if defined(HAVE_LD_RO_RW_SECTION_MIXING)
# define EH_FRAME_SECTION_CONST const
#else
# define EH_FRAME_SECTION_CONST
#endif

#if DWARF2_UNWIND_INFO
/* Terminate the frame unwind info section with a 0 as a sentinel;
   this would be the 'length' field in a real FDE.  */

static EH_FRAME_SECTION_CONST int __FRAME_END__[]
  __attribute__ ((unused,  section(EH_FRAME_SECTION_NAME),
		  aligned(4)))
  = { 0 };
#endif

#if TARGET_USE_JCR_SECTION
/* Null terminate the .jcr section array.  */
static void *__JCR_END__[1] 
   __attribute__ ((unused, section(JCR_SECTION_NAME),
		   aligned(sizeof(void *))))
   = { 0 };
#endif


/*
extern void __gcco_register_frame (void); 
extern void __gcco_deregister_frame (void);
*/


/* Register the eh_frame. This has to be the first .ctor to  be invoked
   so we put its address in the .ctor section of the last object and
   give it a high priority (ld uses strcmp to sort sections by name).
   The definition is in crtbegin.c since we need to pull in.
   libgcc.a __register_frame_info. 
   Likewise for frame deregistration on exit from app or dll.  */

/* FIXME: Port constructor(N) here */

__asm__(
  ".section\t.ctors.zzzzz ,\"w\"\n\t"
  ".align 4\n\t"
   ".long\t___gcc_register_frame");

__asm__(
  ".section\t.dtors.zzzzz ,\"w\"\n\t"
  ".align 4\n\t"
   ".long\t___gcc_deregister_frame ");
