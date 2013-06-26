/* Generate transitive closure of a matrix,
   Copyright (C) 1984 Bob Corbett and Free Software Foundation, Inc.

BISON is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY.  No author or distributor accepts responsibility to anyone
for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the BISON General Public License for full details.

Everyone is granted permission to copy, modify and redistribute BISON,
but only under the conditions described in the BISON General Public
License.  A copy of this license is supposed to have been given to you
along with BISON so you can know your rights and responsibilities.  It
should be in a file named COPYING.  Among other things, the copyright
notice and this notice must be preserved on all copies.

 In other words, you are welcome to use, share and improve this program.
 You are forbidden to forbid anyone else to use, share and improve
 what you give them.   Help stamp out software-hoarding!  */

#include <stdio.h>
#include "machine.h"


/* given n by n matrix of bits R, modify its contents
   to be the transive closure of what was given.  */

void TC(unsigned *R,int n)
{
  register int rowsize;
  register unsigned mask;
  register unsigned *rowj;
  register unsigned *rp;
  register unsigned *rend;
  register unsigned *ccol;

  unsigned *relend;
  unsigned *cword;
  unsigned *rowi;

  rowsize = WORDSIZE(n) * sizeof(unsigned);
  relend = (unsigned *) ((char *) R + (n * rowsize));

  cword = R;
  mask = 1;
  rowi = R;
  while (rowi < relend)
    {
      ccol = cword;
      rowj = R;

      while (rowj < relend)
	{
	  if (*ccol & mask)
	    {
	      rp = rowi;
	      rend = (unsigned *) ((char *) rowj + rowsize);

	      while (rowj < rend)
		*rowj++ |= *rp++;
	    }
	  else
	    {
	      rowj = (unsigned *) ((char *) rowj + rowsize);
	    }

	  ccol = (unsigned *) ((char *) ccol + rowsize);
	}

      mask <<= 1;
      if (mask == 0)
	{
	  mask = 1;
	  cword++;
	}

      rowi = (unsigned *) ((char *) rowi + rowsize);
    }
}


/* Reflexive Transitive Closure.  Same as TC
   and then set all the bits on the diagonal of R.  */

void RTC(unsigned *R,int n)
{
  register int rowsize;
  register unsigned mask;
  register unsigned *rp;
  register unsigned *relend;

  TC(R, n);

  rowsize = WORDSIZE(n) * sizeof(unsigned);
  relend = (unsigned *) ((char *) R + n*rowsize);

  mask = 1;
  rp = R;
  while (rp < relend)
    {
      *rp |= mask;

      mask <<= 1;
      if (mask == 0)
	{
	  mask = 1;
	  rp++;
	}

      rp = (unsigned *) ((char *) rp + rowsize);
    }
}
