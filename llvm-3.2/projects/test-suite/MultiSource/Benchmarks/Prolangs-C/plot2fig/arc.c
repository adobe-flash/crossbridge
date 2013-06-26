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


/* This file is the arc routine, which is a standard part of the plot
   library.  It draws an arc with the center at xc,yc, the beginning at
   x0,y0 and the ending at x1,y1 */

#include <math.h>
#include <stdio.h>
#ifdef sequent
#include <strings.h>
#else
#include <string.h>
#endif
/*
extern int printf();
extern int fprintf();
#ifndef mips
extern char *sprintf();
#endif
extern int fputs(), fputc();
*/

#include "libplot.h"
#include "extern.h"

int arc (int xc, int yc, int x0, int y0, int x1, int y1)
{
  draw_line ();
  printf("%d %d %d %d %d %d %d %d %.3f %d %d %d %.3f %.3f %d %d %d %d %d %d\n",
	 5 /* arc object */, 1, line_style, 0 /* thickness */,
	 0 /* black */, 0, 0, fill_level,
	 dash_length, 0 /* clockwise */,
	 0, 0,
	 (int) ((xc - x_input_min)/ x_scale + x_output_min),
	 (int) ((yc - y_input_min)/ y_scale + y_output_min),
	 (int) ((x0 - x_input_min)/ x_scale + x_output_min),
	 (int) ((y0 - y_input_min)/ y_scale + y_output_min),
	 (int) ((x1 - x_input_min)/ x_scale + x_output_min),
	 (int) ((y1 - y_input_min)/ y_scale + y_output_min),
	 (int) ((x1 - x_input_min)/ x_scale + x_output_min),
	 (int) ((y1 - y_input_min)/ y_scale + y_output_min));

  move_nasko (xc, yc);
  return 0;
}
