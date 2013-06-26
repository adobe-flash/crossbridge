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


/* This file is the circle routine, which is a standard part of the plot
   library. It draws a circle given the center x,y and the radius */

#include <math.h>
#include <stdio.h>
#ifdef sequent
#include <strings.h>
#else
#include <string.h>
#endif
#include "libplot.h"
#include "extern.h"
/*
extern int printf();
extern int fprintf();
#ifndef mips
extern char *sprintf();
#endif
extern int fputs(), fputc();
*/

int circle (int x, int y, int r)
{
  arc (x, y, x+r, y, x+r, y);
  return 0;
}
