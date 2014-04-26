/* Target definitions for x86_64 running Darwin.
   Copyright (C) 2006 Free Software Foundation, Inc.
   Contributed by Apple Computer Inc.

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
the Free Software Foundation, 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.  */

#undef TARGET_VERSION
#define TARGET_VERSION fprintf (stderr, " (x86_64 Darwin)");

#undef  DARWIN_ARCH_SPEC
#define DARWIN_ARCH_SPEC "%{m32:i386;:x86_64}"

#undef  DARWIN_SUBARCH_SPEC
#define DARWIN_SUBARCH_SPEC DARWIN_ARCH_SPEC

/* APPLE LOCAL begin kext 6400713 */
#undef ASM_SPEC
#define ASM_SPEC "-arch %(darwin_arch) -force_cpusubtype_ALL \
  %{mkernel|static|fapple-kext:%{m32:-static}}"
/* APPLE LOCAL end kext 6400713 */

#undef SUBTARGET_EXTRA_SPECS
#define SUBTARGET_EXTRA_SPECS                                   \
  /* APPLE LOCAL 6015949 */					\
  DARWIN_EXTRA_SPECS                                            \
  { "darwin_arch", DARWIN_ARCH_SPEC },                          \
  { "darwin_crt2", "" },                                        \
  { "darwin_subarch", DARWIN_SUBARCH_SPEC },
