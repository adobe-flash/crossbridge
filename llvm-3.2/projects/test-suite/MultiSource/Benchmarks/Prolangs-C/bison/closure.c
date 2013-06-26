/* Subroutines for bison
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

/* subroutines of file LR0.c.

Entry points:

  closure (items, n)

Given a vector of item numbers items, of length n,
set up ruleset and itemset to indicate what rules could be run
and which items could be accepted when those items are the active ones.

ruleset contains a bit for each rule.  closure sets the bits
for all rules which could potentially describe the next input to be read.

itemset is a vector of item numbers; itemsetend points to just beyond the end
 of the part of it that is significant.
closure places there the indices of all items which represent units of
input that could arrive next.

  initialize_closure (n)

Allocates the itemset and ruleset vectors,
and precomputes useful data so that closure can be called.
n is the number of elements to allocate for itemset.

  finalize_closure ()

Frees itemset, ruleset and internal data.

*/

#include <stdio.h>
#include "machine.h"
#include "new.h"
#include "gram.h"

extern short **derives;

short *itemset;
short *itemsetend;
static unsigned *ruleset;

/* internal data.  See comments before set_fderives and set_firsts.  */
static unsigned *fderives;
static unsigned *firsts;

/* number of words required to hold a bit for each rule */
static int rulesetsize;

/* number of words required to hold a bit for each variable */
static int varsetsize;

extern void RTC(unsigned *R,int n);

void set_fderives(void);
void set_firsts(void);

void initialize_closure(int n)
{
  itemset = NEW2(n, short);

  rulesetsize = WORDSIZE(nrules + 1);
  ruleset = NEW2(rulesetsize, unsigned);

  set_fderives();
}

/* set fderives to an nvars by nrules matrix of bits
   indicating which rules can help derive the beginning of the data
   for each nonterminal.  For example, if symbol 5 can be derived as
   the sequence of symbols 8 3 20, and one of the rules for deriving
   symbol 8 is rule 4, then the [5 - ntokens, 4] bit in fderives is set.  */

void set_fderives(void)
{
  register unsigned *rrow;
  register unsigned *vrow;
  register int j;
  register unsigned mask;
  register unsigned cword;
  register short *rp;

  int ruleno;
  int i;

  fderives = NEW2(nvars * rulesetsize, unsigned) - ntokens * rulesetsize;

  set_firsts();

  rrow = fderives + ntokens * rulesetsize;

  for (i = ntokens; i < nsyms; i++)
    {
      vrow = firsts + ((i - ntokens) * varsetsize);
      cword = *vrow++;
      mask = 1;
      for (j = ntokens; j < nsyms; j++)
	{
	  if (cword & mask)
	    {
	      rp = derives[j];
	      while ((ruleno = *rp++) > 0)
		{
		  SETBIT(rrow, ruleno);
		}
	    }

	  mask <<= 1;
	  if (mask == 0 && j + 1 < nsyms)
	    {
	      cword = *vrow++;
	      mask = 1;
	    }
	}

      vrow += varsetsize;
      rrow += rulesetsize;
    }

#ifdef	DEBUG
  print_fderives();
#endif

  FREE(firsts);
}

/* set firsts to be an nvars by nvars bit matrix indicating which items
   can represent the beginning of the input corresponding to which other items.
   For example, if some rule expands symbol 5 into the sequence of symbols 8 3 20,
   the symbol 8 can be the beginning of the data for symbol 5,
   so the bit [8 - ntokens, 5 - ntokens] in firsts is set. */

void set_firsts(void)
{
  register unsigned *row;
/*   register int done; JF unused */
  register int symbol;
  register short *sp;
  register int rowsize;

  int i;

  varsetsize = rowsize = WORDSIZE(nvars);

  firsts = NEW2(nvars * rowsize, unsigned);

  row = firsts;
  for (i = ntokens; i < nsyms; i++)
    {
      sp = derives[i];
      while (*sp >= 0)
	{
	  symbol = ritem[rrhs[*sp++]];
	  if (ISVAR(symbol))
	    {
	      symbol -= ntokens;
	      SETBIT(row, symbol);
	    }
	}

      row += rowsize;
    }

  RTC(firsts, nvars);

#ifdef	DEBUG
  print_firsts();
#endif
}

void closure(short *core,int n)
{
  register int ruleno;
  register unsigned word;
  register unsigned mask;
  register short *csp;
  register unsigned *dsp;
  register unsigned *rsp;

  short *csend;
  unsigned *rsend;
  int symbol;
  int itemno;

  rsp = ruleset;
  rsend = ruleset + rulesetsize;
  csend = core + n;

  if (n == 0)
    {
      dsp = fderives + start_symbol * rulesetsize;
      while (rsp < rsend)
	*rsp++ = *dsp++;
    }
  else
    {
      while (rsp < rsend)
	*rsp++ = 0;

      csp = core;
      while (csp < csend)
	{
	  symbol = ritem[*csp++];
	  if (ISVAR(symbol))
	    {
	      dsp = fderives + symbol * rulesetsize;
	      rsp = ruleset;
	      while (rsp < rsend)
		*rsp++ |= *dsp++;
	    }
	}
    }

  ruleno = 0;
  itemsetend = itemset;
  csp = core;
  rsp = ruleset;
  while (rsp < rsend)
    {
      word = *rsp++;
      if (word == 0)
	{
	  ruleno += BITS_PER_WORD;
	}
      else
	{
	  mask = 1;
	  while (mask)
	    {
	      if (word & mask)
		{
		  itemno = rrhs[ruleno];
		  while (csp < csend && *csp < itemno)
		    *itemsetend++ = *csp++;
		  *itemsetend++ = itemno;
		}

	      mask <<= 1;
	      ruleno++;
	    }
	}
    }

  while (csp < csend)
    *itemsetend++ = *csp++;

#ifdef	DEBUG
  print_closure(n);
#endif
}

void finalize_closure(void)
{
  FREE(itemset);
  FREE(ruleset);
  FREE(fderives + ntokens * rulesetsize);
}

#ifdef	DEBUG

void print_closure(int n)
{
  register short *isp;

  printf("\n\nn = %d\n\n", n);
  for (isp = itemset; isp < itemsetend; isp++)
    printf("   %d\n", *isp);
}

void print_firsts(void)
{
  register int i;
  register int j;
  register unsigned *rowp;
  register unsigned cword;
  register unsigned mask;

  extern char **tags;

  printf("\n\n\nFIRSTS\n\n");

  for (i = ntokens; i < nsyms; i++)
    {
      printf("\n\n%s firsts\n\n", tags[i]);

      rowp = firsts + ((i - ntokens) * vrowsize);

      cword = *rowp++;
      mask = 1;
      for (j = 0; j < nsyms; j++)
	{
	  if (cword & mask)
	    printf("   %s\n", tags[j + ntokens]);

	  mask <<= 1;

	  if (mask == 0 && j + 1 < nsyms)
	    {
	      cword = *rowp++;
	      mask = 1;
	    }
	}
    }
}

void print_fderives(void)
{
  register int i;
  register int j;
  register unsigned *rp;
  register unsigned cword;
  register unsigned mask;

  extern char **tags;

  printf("\n\n\nFDERIVES\n");

  for (i = ntokens; i < nsyms; i++)
    {
      printf("\n\n%s derives\n\n", tags[i]);
      rp = fderives + i * rrowsize;
      cword = *rp++;
      mask = 1;
      for (j = 0; j <= nrules; j++)
        {
	  if (cword & mask)
	    printf("   %d\n", j);

	  mask <<= 1;
	  if (mask == 0 && j + 1 < nrules)
	    {
	      cword = *rp++;
	      mask = 1;
	    }
	}
    }

  fflush(stdout);
}

#endif
