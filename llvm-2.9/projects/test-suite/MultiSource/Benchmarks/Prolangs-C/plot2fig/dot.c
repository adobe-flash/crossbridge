/* plot, unix plot file to graphics device translators.
   Copyright (C) 1989 Free Software Foundation, Inc.

   plot is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY.  No author or distributor accepts responsibility to
   anyone for the consequences of using it or for whether it serves any
   particular purpose or works at all, unless he says so in writing.
   Refer to the GNU General Public License for full details.
   
   Everyone is granted permission to copy, modify and redistribute plot,
   but only under the conditions described in the GNU General Public
   License.  A copy of this license is supposed to have been given to you
   along with plot so you can know your rights and responsibilities.  It
   should be in a file named COPYING.  Among other things, the copyright
   notice and this notice must be preserved on all copies.  */


/* This file is the dot routine, which is an extension to the plot
   library. this is an extentsion and may be modified.  Although I have
   seen support for this in other plot(1g) programs, I haven't yet found
   out exactly what `dot' is supposed to do, so it is not yet implemented
   fully. */

#include "libplot.h"
#include "extern.h"

int dot (int x, int y, int dx, int n, char pattern)
{
  point (x, y);
  return 0;
}
