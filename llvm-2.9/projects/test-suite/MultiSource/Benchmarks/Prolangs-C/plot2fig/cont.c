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


/* This file is the cont routine, which is a standard part of the plot
   library. It continues a line from the last point drawn to the point
   specified by x and y.
   */
#include <stdio.h>
#include "libplot.h"
#include "extern.h"

int last_x=0, last_y = 0; /* location of the last coordinates used */

int PointsInLine=0;

int cont(int x, int y)
{
  if (PointsInLine <= 1)
    {
      printf( "%d %d %d %d %d %d %d %d %.3f %d %d %d\n\t",
	     2,			/* polyline object */
	     1,			/* polyline subtype */
	     line_style,	/* style */
	     1,			/* thickness */
	     0,			/* color */
	     0,			/* depth */
	     0,			/* pen */
	     fill_level,	/* area fill */
	     dash_length,	/* style val */
	     0,			/* radius */
	     0,			/* forward arrow */
	     0			/* backward arrow */
	     );
    }
  if (PointsInLine == 1)
    {
      printf(" %d %d",
	     (int) ((last_x - x_input_min)/ x_scale + x_output_min),
	     (int) ((last_y - y_input_min)/ y_scale + y_output_min));
    }

  PointsInLine++;
  printf(" %d %d",
	 (int) ((x - x_input_min)/ x_scale + x_output_min),
	 (int) ((y - y_input_min)/ y_scale + y_output_min));
  last_x = x;
  last_y = y;
  
  return 0;
}
