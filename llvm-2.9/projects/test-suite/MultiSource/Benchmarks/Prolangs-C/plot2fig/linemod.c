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


/* This file is the linemod routine, which is a standard part of the plot
   library.  It sets the line type according to the name contained in
   the string argument s */

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

int line_style = 0; /* solid */

float dash_length = 10.0; /* short dashed */

int linemod(char *s)
{
  if ((strcmp( s, "longdashed") == 0)
      || (strcmp( s, "long-dashed") == 0))
    {
      line_style = 1;
      dash_length = 20.0;
    }
  else if (strcmp( s, "disconnected") == 0)
    {
      line_style = 2;
      dash_length = 999.0;
    }
  else if ((strcmp( s, "dotdashed") == 0)
	   || (strcmp( s, "dot-dashed") == 0))
    {
      line_style = 1;
      dash_length = 4.0;
    }
  else if (strcmp( s, "dotted") == 0)
    {
      line_style = 2;
      dash_length = 2.0;
    }
  else if (strcmp( s, "solid") == 0)
    {
      line_style = 0;
      dash_length = 1.0;
    }
  else if ((strcmp( s, "shortdashed") == 0)
	   || (strcmp( s, "short-dashed") == 0))
    {
      line_style = 1;
      dash_length = 10.0;
    }
  else
    {
      fprintf( stderr, "Unrecognized line mode `%s' ignored. Using solid.\n", s);	      
      /* Set the line mode to SOLID if an unsupported mode is requested. */
      line_style = 0;
      dash_length = 1.0;
    }

  return 0;
}
