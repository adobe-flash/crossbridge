/* libtek, a library of functions for tektronics 4010 compatible devices.
   Copyright (C) 1989 Free Software Foundation, Inc.

libtek is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY.  No author or distributor accepts responsibility to anyone for the
consequences of using it or for whether it serves any particular purpose or
works at all, unless he says so in writing.  Refer to the GNU General Public
License for full details.

Everyone is granted permission to copy, modify and redistribute libtek, but
only under the conditions described in the GNU General Public License.  A copy
of this license is supposed to have been given to you along with libtek so
you can know your rights and responsibilities.  It should be in a file named
COPYING.  Among other things, the copyright notice and this notice must be
preserved on all copies.  */

/* This file is the space routine, which is a standard part of the plot
   library.  It sets the lower left and upper right corners of the page.  The
   plot will be scaled so that these coners fit the largest renderable area on
   the page.  */

#include <math.h>
#include "libplot.h"
#include "extern.h"

double x_input_min = 0.;    /* minimum input x coordinate */
double y_input_min = 0.;    /* minimum input y coordinate */
/* Latex has troubles with plots larger than 6 inches (480). */
/* The range (60,660) to (540,180) centers the plot in the fig window. */
double x_output_min = 60.;   /* minimum output x coordinate */
double y_output_min = 660.;   /* minimum output y coordinate */
double x_output_max = 540.; /* maximum output x coordinate */
double y_output_max = 180.; /* maximum output y coordinate */
double scaleup = 1.;        /* maximum input to output scaleing of x and y */
double x_scale = 1.;        /* input to output scaleing for x */
double y_scale = 1.;        /* input to output scaleing for y */

int space (int x0, int y0, int x1, int y1)
{
  x_input_min = x0;
  y_input_min = y0;
  x_scale = (x1 - x0)/(x_output_max - x_output_min);
  y_scale = (y1 - y0)/(y_output_max - y_output_min);
  if (fabs(x_scale) > fabs(y_scale))
    scaleup = fabs (x_scale);
  else
    scaleup = fabs (y_scale);
  return 0;
}
