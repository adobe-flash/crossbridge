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

#include <math.h>
#include <stdio.h>
#ifdef sequent
#include <strings.h>
#else
#include <string.h>
#endif
#include <ctype.h>
#include "libplot.h"
#include "extern.h"

/* ALABEL takes three arguments X_JUSTIFY, Y_JUSTIFY, and S and places
   the label S according to the x and y axis adjustments specified in
   X_JUSTIFY and Y_JUSTIFY respectively.  X_JUSTIFY is a character
   containing either l, c, or r for left, center or right justified with
   respect to the current x coordinate.  Y_JUSTIFY is a character
   containing either b, c, or t for placing the bottom center or top of
   the label even with the current y coordinate. S is a string containing
   the label. The current point is moved to follow the end of the text. */

int alabel (int x_justify, int y_justify, char *s)
{
/* int i;  this variable is never used */
  char *p;
  int x_justification = 0;
  double y_offset = 0.;

  draw_line ();
  switch( x_justify) {
    case 'l':
     x_justification = 0;
     break;
    case 'c':
     x_justification = 1;
     break;
    case 'r':
     x_justification = 2;
     break;
   }
  switch( y_justify) {
    case 't':
     y_offset = 1.1;
     break;
    case 'c':
     y_offset = 0.5;
     break;
    case 'b':
     y_offset = 0.0;
     break;
   }

  /* ignore leading white space */
  p = s;
  while (isspace(*p))
    p++;

  /* only output the string if it contains non-whitespace characters. */
  if (strlen(p))
    printf( "%d %d %d %d %d %d %d %.3f %d %d %d %d %d %s\1\n", 
	   4,			/* text object */
	   x_justification,	/* horzontal justification */
	   font_id,		/* font */
	   font_size,		/* point size */
	   0,			/* pen */
	   0,			/* color */
	   0,			/* depth */
	   text_rotation,	/* angle of rotation (float) */
	   0,			/* flags */
	   8,			/* height (pixels) */
	   8,			/* length (pixels) */
	   (int) ((last_x - x_input_min)/ x_scale + x_output_min),
	   (int) ((last_y - y_input_min)/ y_scale + y_output_min
		  + font_size * y_offset * 72 / 80),
	   p);
  return 0;
}
