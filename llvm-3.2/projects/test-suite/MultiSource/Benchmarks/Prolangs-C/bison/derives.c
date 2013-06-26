/* Match rules with nonterminals for bison,
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

/* set_derives finds, for each variable (nonterminal), which rules can derive it.
   It sets up the value of derives so that
   derives[i - ntokens] points to a vector of rule numbers, terminated with a zero.  */

#include <stdio.h>
#include <stdlib.h>
#include "new.h"
#include "types.h"
#include "gram.h"

short **derives;

void set_derives(void)
{
  register int i;
  register int lhs;
  register shorts *p;
  register short *q;
  register shorts **dset;
  register shorts *delts;

  dset = NEW2(nvars, shorts *) - ntokens;
  delts = NEW2(nrules + 1, shorts);

  p = delts;
  for (i = nrules; i > 0; i--)
    {
      lhs = rlhs[i];
      p->next = dset[lhs];
      p->value = i;
      dset[lhs] = p;
      p++;
    }

  derives = NEW2(nvars, short *) - ntokens;
  q = NEW2(nvars + nrules, short);

  for (i = ntokens; i < nsyms; i++)
    {
      derives[i] = q;
      p = dset[i];
      while (p)
	{
	  *q++ = p->value;
	  p = p->next;
	}
      *q++ = -1;
    }

#ifdef	DEBUG
  print_derives();
#endif

  FREE(dset + ntokens);
  FREE(delts);
}


void free_derives(void)
{
  FREE(derives[ntokens]);
  FREE(derives + ntokens);
}



#ifdef	DEBUG

void print_derives(void)
{
  register int i;
  register short *sp;

  extern char **tags;

  printf("\n\n\nDERIVES\n\n");

  for (i = ntokens; i < nsyms; i++)
    {
      printf("%s derives", tags[i]);
      for (sp = derives[i]; *sp > 0; sp++)
	{
	  printf("  %d", *sp);
	}
      putchar('\n');
    }

  putchar('\n');
}

#endif
