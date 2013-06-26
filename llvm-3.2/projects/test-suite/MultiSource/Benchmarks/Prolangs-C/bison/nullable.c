/* Part of the bison parser generator,
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

/* set up nullable, a vector saying which nonterminals can expand into the null string.
   nullable[i - ntokens] is nonzero if symbol i can do so.  */

#include <stdio.h>
#include "types.h"
#include "gram.h"
#include "new.h"


char *nullable;

void set_nullable(void)
{
  register short *r;
  register short *s1;
  register short *s2;
  register int ruleno;
  register int symbol;
  register shorts *p;

  short *squeue;
  short *rcount;
  shorts **rsets;
  shorts *relts;
  char any_tokens;
  short *r1;

#ifdef	TRACE
  fprintf(stderr, "Entering set_nullable");
#endif

  nullable = NEW2(nvars, char) - ntokens;

  squeue = NEW2(nvars, short);
  s1 = s2 = squeue;

  rcount = NEW2(nrules + 1, short);
  rsets = NEW2(nvars, shorts *) - ntokens;
  relts = NEW2(nitems + nvars + 1, shorts);
  p = relts;

  r = ritem;
  while (*r)
    {
      if (*r < 0)
	{
	  symbol = rlhs[-(*r++)];
	  if (!nullable[symbol])
	    {
	      nullable[symbol] = 1;
	      *s2++ = symbol;
	    }
	}
      else
	{
	  r1 = r;
	  any_tokens = 0;
	  for (symbol = *r++; symbol > 0; symbol = *r++)
	    {
	      if (ISTOKEN(symbol))
		any_tokens = 1;
	    }

	  if (!any_tokens)
	    {
	      ruleno = -symbol;
	      r = r1;
	      for (symbol = *r++; symbol > 0; symbol = *r++)
		{
		  rcount[ruleno]++;
		  p->next = rsets[symbol];
		  p->value = ruleno;
		  rsets[symbol] = p;
		  p++;
		}
	    }
	}
    }

  while (s1 < s2)
    {
      p = rsets[*s1++];
      while (p)
	{
	  ruleno = p->value;
	  p = p->next;
	  if (--rcount[ruleno] == 0)
	    {
	      symbol = rlhs[ruleno];
	      if (!nullable[symbol])
		{
		  nullable[symbol] = 1;
		  *s2++ = symbol;
		}
	    }
	}
    }

  FREE(squeue);
  FREE(rcount);
  FREE(rsets + ntokens);
  FREE(relts);
}

void free_nullable(void)
{
  FREE(nullable + ntokens);
}
