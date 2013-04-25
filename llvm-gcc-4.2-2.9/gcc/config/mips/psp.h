/* Support for Sony's Playstation Portable (PSP).
   Copyright (C) 2005 Free Software Foundation, Inc.
   Contributed by Marcus R. Brown <mrbrown@ocgnet.org>

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#undef TARGET_PSP
#define TARGET_PSP 1

/* Override the startfile spec to include crt0.o. */
#undef STARTFILE_SPEC
#define STARTFILE_SPEC "crt0%O%s crti%O%s crtbegin%O%s"

/* Get rid of the .pdr section. */
#undef SUBTARGET_ASM_SPEC
#define SUBTARGET_ASM_SPEC "-mno-pdr" 
