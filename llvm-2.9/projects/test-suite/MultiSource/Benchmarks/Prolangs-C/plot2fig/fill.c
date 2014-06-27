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

/* This file is the fill routine, which is an extension to the plot
   library.  It changes the grey level of the fill pattern for all closed
   paths of the following drawing operations. */

#include "libplot.h"
#include "extern.h"

int fill_level = 0; /* default, no fill */

/* FILL sets the intensity of the filler for closed paths.  LEVEL ranges
   from 1 to 0xFFFF. A value of 1 represents black and a value of 0xFFFF
   indicates white. A value of 0 represents no fill - transparent. */

int fill (int level)
{
  if (level == 0)
    {
      fill_level = 0;
    }
  else
    {
      fill_level = 5 * ((double)level - 1.)/0xFFFE;
      /* The value of fill level should be between 1 and 5,
	 which is enforced here. */
      if (fill_level > 5)
	{
	  fill_level = 5;
	}
      else if (fill_level < 1)
	{
	  fill_level = 1;
	}
    }
  return 0;
}
