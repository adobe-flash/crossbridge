/* ARM NEON intrinsics include file.
   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 2, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING.  If not, write to the
   Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301, USA.  */

/* As a special exception, if you include this header file into source
   files compiled by GCC, this header file does not by itself cause
   the resulting executable to be covered by the GNU General Public
   License.  This exception does not however invalidate any other
   reasons why the executable file might be covered by the GNU General
   Public License.  */

/* llvm-gcc provides two different versions of the NEON types and
   intrinsics.  The default versions follow the standard definitions
   specified by ARM.  For backward compatibility with GCC, alternate
   versions are provided where the intrinsics will accept arguments with
   GCC's vector types instead of the "containerized vector" types
   specified by ARM.  Define the ARM_NEON_GCC_COMPATIBILITY macro to
   select these alternate versions of the NEON types and intrinsics.  */

#ifdef ARM_NEON_GCC_COMPATIBILITY
#include <arm_neon_gcc.h>
#else
#include <arm_neon_std.h>
#endif
