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


/* This file is the color routine, which is an extension to the plot
   library. It changes the color of the following drawing operations. */

#include "libplot.h"
#include "extern.h"

/* FGCOLOR_RED, FGCOLOR_GREEN and FGCOLOR_BLUE indicate the foreground
   color of all plot. For each the range of intensity is from 0 to 1 so
   that a value of (0,0,0) represents black and a value of (1,1,1)
   indicates white. */

double fgcolor_red=0., fgcolor_green=0., fgcolor_blue=0.;

int color ( int red, int green, int blue)
{
  fgcolor_red = red / 0xffff;
  fgcolor_green = green / 0xffff;
  fgcolor_blue = blue / 0xffff;
  return 0;
}
