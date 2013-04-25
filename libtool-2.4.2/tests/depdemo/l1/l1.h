/* l1.h -- interface to a trivial library

   Copyright (C) 1998-1999 Thomas Tanner
   Copyright (C) 2006, 2010 Free Software Foundation, Inc.
   Written by Thomas Tanner, 1998

   This file is part of GNU Libtool.

GNU Libtool is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License, or (at your option) any later version.

GNU Libtool is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Libtool; see the file COPYING.  If not, a copy
can be downloaded from  http://www.gnu.org/licenses/gpl.html,
or obtained by writing to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

/* Only include this header file once. */
#ifndef _L1_H_
#define _L1_H_ 1

#include "sysdep.h"

#if (defined _WIN32 || defined _WIN32_WCE) && !defined __GNUC__
# ifdef BUILDING_LIBL1
#  ifdef DLL_EXPORT
#   define LIBL1_SCOPE extern __declspec (dllexport)
#  endif
# else
#  define LIBL1_SCOPE extern __declspec (dllimport)
# endif
#endif
#ifndef LIBL1_SCOPE
# define LIBL1_SCOPE extern
#endif

__BEGIN_DECLS
LIBL1_SCOPE int var_l1;
int	func_l1 __P((int));
__END_DECLS

#endif /* !_L1_H_ */
