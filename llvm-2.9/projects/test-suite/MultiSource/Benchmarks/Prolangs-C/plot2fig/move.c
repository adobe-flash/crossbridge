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


/* This file is the move routine, which is a standard part of the plot
   library.  It switches the device to vector plot mode and plots a
   point at the given coordinate.
*/
#include <stdio.h>
#include "libplot.h"
#include "extern.h"

int move_nasko(int x, int y)
{
  draw_line ();
  last_x = x;
  last_y = y;
  PointsInLine=1;
  return 0;
}
