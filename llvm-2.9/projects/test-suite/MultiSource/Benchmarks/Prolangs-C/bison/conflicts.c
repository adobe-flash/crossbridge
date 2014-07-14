/* Find and resolve or report look-ahead conflicts for bison,
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
#include <stdlib.h>
#include "machine.h"
#include "new.h"
#include "files.h"
#include "gram.h"
#include "state.h"


#ifdef USG
#include <memory.h>
#define bcopy(src, dst, num) memcpy((dst), (src), (num))
#endif

#ifdef sparc
#include <alloca.h>
#endif

#define alloca1(x)  malloc(x)

extern char **tags;
extern int tokensetsize;
extern char *consistent;
extern short *accessing_symbol;
extern shifts **shift_table;
extern unsigned *LA;
extern short *LAruleno;
extern short *lookaheads;
extern int verboseflag;

/*
extern char *alloca ();
*/

char any_conflicts;
char *conflicts;
errs **err_table;
int expected_conflicts;


static unsigned *shiftset;
static unsigned *lookaheadset;
static int src_total;
static int rrc_total;
static int src_count;
static int rrc_count;

void set_conflicts(int state);
void resolve_sr_conflict(int state,int lookaheadnum);
void flush_shift(int state,int token);
void log_resolution(int state,int LAno,int token,char *resolution);
void total_conflicts(void);
void count_sr_conflicts(int state);
void count_rr_conflicts(int state);

void initialize_conflicts(void)
{
  register int i;
/*  register errs *sp; JF unused */

  conflicts = NEW2(nstates, char);
  shiftset = NEW2(tokensetsize, unsigned);
  lookaheadset = NEW2(tokensetsize, unsigned);

  err_table = NEW2(nstates, errs *);

  any_conflicts = 0;

  for (i = 0; i < nstates; i++)
    set_conflicts(i);
}



void set_conflicts(int state)
{
  register int i;
  register int k;
  register shifts *shiftp;
  register unsigned *fp2;
  register unsigned *fp3;
  register unsigned *fp4;
  register unsigned *fp1;
  register int symbol;

  if (consistent[state]) return;

  for (i = 0; i < tokensetsize; i++)
    lookaheadset[i] = 0;

  shiftp = shift_table[state];
  if (shiftp)
    {
      k = shiftp->nshifts;
      for (i = 0; i < k; i++)
	{
	  symbol = accessing_symbol[shiftp->shifts[i]];
	  if (ISVAR(symbol)) break;
	  SETBIT(lookaheadset, symbol);
	}
    }

  k = lookaheads[state + 1];
  fp4 = lookaheadset + tokensetsize;

  /* loop over all rules which require lookahead in this state */
  /* first check for shift-reduce conflict, and try to resolve using precedence  */

  for (i = lookaheads[state]; i < k; i++)
    if (rprec[LAruleno[i]])
      {
	fp1 = LA + i * tokensetsize;
	fp2 = fp1;
	fp3 = lookaheadset;
  
	while (fp3 < fp4)
	  {
	    if (*fp2++ & *fp3++)
	      {
		resolve_sr_conflict(state, i);
		break;
	      }
	  }
      }

  /* loop over all rules which require lookahead in this state */
  /* Check for conflicts not resolved above.  */

  for (i = lookaheads[state]; i < k; i++)
    {
      fp1 = LA + i * tokensetsize;
      fp2 = fp1;
      fp3 = lookaheadset;

      while (fp3 < fp4)
	{
	  if (*fp2++ & *fp3++)
	    {
	      conflicts[state] = 1;
	      any_conflicts = 1;
	    }
	}

      fp2 = fp1;
      fp3 = lookaheadset;

      while (fp3 < fp4)
	*fp3++ |= *fp2++;
    }
}

/* Attempt to resolve shift-reduce conflict for one rule
by means of precedence declarations.
It has already been checked that the rule has a precedence.
A conflict is resolved by modifying the shift or reduce tables
so that there is no longer a conflict.  */

void resolve_sr_conflict(int state,int lookaheadnum)
{
  register int i;
  register int mask;
  register unsigned *fp1;
  register unsigned *fp2;
  register int redprec;
  errs *errp = (errs *) alloca1 (sizeof(errs) + ntokens * sizeof(short));
  short *errtokens = errp->errs;

  /* find the rule to reduce by to get precedence of reduction  */
  redprec = rprec[LAruleno[lookaheadnum]];

  mask = 1;
  fp1 = LA + lookaheadnum * tokensetsize;
  fp2 = lookaheadset;
  for (i = 0; i < ntokens; i++)
    {
      if ((mask & *fp2 & *fp1) && sprec[i])
	/* shift-reduce conflict occurs for token number i and it has a precision.
	   The precedence of shifting is that of token i.  */
	{
	  if (sprec[i] < redprec)
	    {
	      if (verboseflag) log_resolution(state, lookaheadnum, i, "reduce");
	      *fp2 &= ~mask;  /* flush the shift for this token */
	      flush_shift(state, i);
	    }
	  else if (sprec[i] > redprec)
	    {
	      if (verboseflag) log_resolution(state, lookaheadnum, i, "shift");
	      *fp1 &= ~mask;  /* flush the reduce for this token */
	    }
	  else
	    {
	      /* Matching precedence levels.
		 For left association, keep only the reduction.
		 For right association, keep only the shift.
		 For nonassociation, keep neither.  */

	      switch (sassoc[i])
		{

		case RIGHT_ASSOC:
	          if (verboseflag) log_resolution(state, lookaheadnum, i, "shift");
		  break;

		case LEFT_ASSOC:
	          if (verboseflag) log_resolution(state, lookaheadnum, i, "reduce");
		  break;

		case NON_ASSOC:
	          if (verboseflag) log_resolution(state, lookaheadnum, i, "an error");
		  break;
		}

	      if (sassoc[i] != RIGHT_ASSOC)
		{
		  *fp2 &= ~mask;  /* flush the shift for this token */
		  flush_shift(state, i);
		}
	      if (sassoc[i] != LEFT_ASSOC)
	        {
		  *fp1 &= ~mask;  /* flush the reduce for this token */
		}
	      if (sassoc[i] == NON_ASSOC)
		{
		  /* Record an explicit error for this token.  */
		  *errtokens++ = i;
		}
	    }
	}

      mask <<= 1;
      if (mask == 0)
	{
	  mask = 1;
	  fp2++;  fp1++;
	}
    }
  errp->nerrs = errtokens - errp->errs;
  if (errp->nerrs)
    {
      /* Some tokens have been explicitly made errors.  Allocate
	 a permanent errs structure for this state, to record them.  */
      i = (char *) errtokens - (char *) errp;
      err_table[state] = (errs *) mallocate ((unsigned int)i);
      bcopy (errp, err_table[state], i);
    }
  else
    err_table[state] = 0;
}



/* turn off the shift recorded for the specified token in the specified state.
Used when we resolve a shift-reduce conflict in favor of the reduction.  */

void flush_shift(int state,int token)
{
  register shifts *shiftp;
  register int k, i;
/*  register unsigned symbol; JF unused */

  shiftp = shift_table[state];

  if (shiftp)
    {
      k = shiftp->nshifts;
      for (i = 0; i < k; i++)
	{
	  if (shiftp->shifts[i] && token == accessing_symbol[shiftp->shifts[i]])
	    (shiftp->shifts[i]) = 0;
	}
    }
}



void log_resolution(int state,int LAno,int token,char *resolution)
{
  fprintf(foutput,
	  "Conflict in state %d between rule %d and token %s resolved as %s.\n",
	  state, LAruleno[LAno], tags[token], resolution);
}



void conflict_log(void)
{
  register int i;

  src_total = 0;
  rrc_total = 0;

  for (i = 0; i < nstates; i++)
    {
      if (conflicts[i])
	{
	  count_sr_conflicts(i);
	  count_rr_conflicts(i);
	  src_total += src_count;
	  rrc_total += rrc_count;
	}
    }

  total_conflicts();
}
  


void verbose_conflict_log(void)
{
  register int i;

  src_total = 0;
  rrc_total = 0;

  for (i = 0; i < nstates; i++)
    {
      if (conflicts[i])
	{
	  count_sr_conflicts(i);
	  count_rr_conflicts(i);
	  src_total += src_count;
	  rrc_total += rrc_count;

	  fprintf(foutput, "State %d contains", i);

	  if (src_count == 1)
	    fprintf(foutput, " 1 shift/reduce conflict");
	  else if (src_count > 1)
	    fprintf(foutput, " %d shift/reduce conflicts", src_count);

	  if (src_count > 0 && rrc_count > 0)
	    fprintf(foutput, " and");

	  if (rrc_count == 1)
	    fprintf(foutput, " 1 reduce/reduce conflict");
	  else if (rrc_count > 1)
	    fprintf(foutput, " %d reduce/reduce conflicts", rrc_count);

	  putc('.', foutput);
	  putc('\n', foutput);
	}
    }

  total_conflicts();
}


static const char *mybasename(const char *str) {
  const char *base = strrchr(str, '/');
  return base ? base+1 : str;
}

void total_conflicts(void)
{
  if (src_total == expected_conflicts && rrc_total == 0)
    return;

  fflush(stdout);
  fprintf(stderr, "%s contains", mybasename(infile));

  if (src_total == 1)
    fprintf(stderr, " 1 shift/reduce conflict");
  else if (src_total > 1)
    fprintf(stderr, " %d shift/reduce conflicts", src_total);

  if (src_total > 0 && rrc_total > 0)
    fprintf(stderr, " and");

  if (rrc_total == 1)
    fprintf(stderr, " 1 reduce/reduce conflict");
  else if (rrc_total > 1)
    fprintf(stderr, " %d reduce/reduce conflicts", rrc_total);

  putc('.', stderr);
  putc('\n', stderr);
}



void count_sr_conflicts(int state)
{
  register int i;
  register int k;
  register int mask;
  register shifts *shiftp;
  register unsigned *fp1;
  register unsigned *fp2;
  register unsigned *fp3;
  register int symbol;

  src_count = 0;

  shiftp = shift_table[state];
  if (!shiftp) return;

  for (i = 0; i < tokensetsize; i++)
    {
      shiftset[i] = 0;
      lookaheadset[i] = 0;
    }

  k = shiftp->nshifts;
  for (i = 0; i < k; i++)
    {
      if (! shiftp->shifts[i]) continue;
      symbol = accessing_symbol[shiftp->shifts[i]];
      if (ISVAR(symbol)) break;
      SETBIT(shiftset, symbol);
    }

  k = lookaheads[state + 1];
  fp3 = lookaheadset + tokensetsize;

  for (i = lookaheads[state]; i < k; i++)
    {
      fp1 = LA + i * tokensetsize;
      fp2 = lookaheadset;

      while (fp2 < fp3)
	*fp2++ |= *fp1++;
    }

  fp1 = shiftset;
  fp2 = lookaheadset;

  while (fp2 < fp3)
    *fp2++ &= *fp1++;

  mask = 1;
  fp2 = lookaheadset;
  for (i = 0; i < ntokens; i++)
    {
      if (mask & *fp2)
	src_count++;

      mask <<= 1;
      if (mask == 0)
	{
	  mask = 1;
	  fp2++;
	}
    }
}

void count_rr_conflicts(int state)
{
  register int i;
  register int j;
  register int count;
  register unsigned mask;
  register unsigned *baseword;
  register unsigned *wordp;
  register int m;
  register int n;

  rrc_count = 0;

  m = lookaheads[state];
  n = lookaheads[state + 1];

  if (n - m < 2) return;

  mask = 1;
  baseword = LA + m * tokensetsize;
  for (i = 0; i < ntokens; i++)
    {
      wordp = baseword;

      count = 0;
      for (j = m; j < n; j++)
	{
	  if (mask & *wordp)
	    count++;

	  wordp += tokensetsize;
	}

      if (count >= 2) rrc_count++;

      mask <<= 1;
      if (mask == 0)
	{
	  mask = 1;
	  baseword++;
	}
    }
}



void print_reductions(int state)
{
  register int i;
  register int j;
  register int k;
  register unsigned *fp1;
  register unsigned *fp2;
  register unsigned *fp3;
  register unsigned *fp4;
  register int rule;
  register int symbol;
  register unsigned mask;
  register int m;
  register int n;
  register int default_LA;
  register int default_rule;
  register int cmax;
  register int count;
  register shifts *shiftp;
  register errs *errp;
  int nodefault = 0;

  for (i = 0; i < tokensetsize; i++)
    shiftset[i] = 0;

  shiftp = shift_table[state];
  if (shiftp)
    {
      k = shiftp->nshifts;
      for (i = 0; i < k; i++)
	{
	  if (! shiftp->shifts[i]) continue;
	  symbol = accessing_symbol[shiftp->shifts[i]];
	  if (ISVAR(symbol)) break;
	  /* if this state has a shift for the error token,
	     don't use a default rule.  */
	  if (symbol == error_token_number) nodefault = 1;
	  SETBIT(shiftset, symbol);
	}
    }

  errp = err_table[state];
  if (errp)
    {
      k = errp->nerrs;
      for (i = 0; i < k; i++)
	{
	  if (! errp->errs[i]) continue;
	  symbol = errp->errs[i];
	  SETBIT(shiftset, symbol);
	}
    }

  m = lookaheads[state];
  n = lookaheads[state + 1];

  if (n - m == 1 && ! nodefault)
    {
      default_rule = LAruleno[m];

      fp1 = LA + m * tokensetsize;
      fp2 = shiftset;
      fp3 = lookaheadset;
      fp4 = lookaheadset + tokensetsize;

      while (fp3 < fp4)
	*fp3++ = *fp1++ & *fp2++;

      mask = 1;
      fp3 = lookaheadset;

      for (i = 0; i < ntokens; i++)
	{
	  if (mask & *fp3)
	    fprintf(foutput, "    %-4s\t[reduce  %d  (%s)]\n",
		    tags[i], default_rule, tags[rlhs[default_rule]]);

	  mask <<= 1;
	  if (mask == 0)
	    {
	      mask = 1;
	      fp3++;
	    }
	}

      fprintf(foutput, "    $default\treduce  %d  (%s)\n\n",
	      default_rule, tags[rlhs[default_rule]]);
    }
  else if (n - m >= 1)
    {
      cmax = 0;
      default_LA = -1;
      fp4 = lookaheadset + tokensetsize;

      if (! nodefault)
	for (i = m; i < n; i++)
	  {
	    fp1 = LA + i * tokensetsize;
	    fp2 = shiftset;
	    fp3 = lookaheadset;
  
	    while (fp3 < fp4)
	      *fp3++ = *fp1++ & ( ~ (*fp2++));
  
	    count = 0;
	    mask = 1;
	    fp3 = lookaheadset;
	    for (j = 0; j < ntokens; j++)
	      {
		if (mask & *fp3)
		  count++;
  
		mask <<= 1;
		if (mask == 0)
		  {
		    mask = 1;
		    fp3++;
		  }
	      }
  
	    if (count > cmax)
	      {
		cmax = count;
		default_LA = i;
		default_rule = LAruleno[i];
	      }
  
	    fp2 = shiftset;
	    fp3 = lookaheadset;
  
	    while (fp3 < fp4)
	      *fp2++ |= *fp3++;
	  }

      for (i = 0; i < tokensetsize; i++)
        shiftset[i] = 0;

      if (shiftp)
        {
          k = shiftp->nshifts;
          for (i = 0; i < k; i++)
	    {
	      if (! shiftp->shifts[i]) continue;
	      symbol = accessing_symbol[shiftp->shifts[i]];
	      if (ISVAR(symbol)) break;
	      SETBIT(shiftset, symbol);
	    }
        }

      mask = 1;
      fp1 = LA + m * tokensetsize;
      fp2 = shiftset;
      for (i = 0; i < ntokens; i++)
	{
	  int defaulted = 0;

	  if (mask & *fp2)
	    count = 1;
	  else
	    count = 0;

	  fp3 = fp1;
	  for (j = m; j < n; j++)
	    {
	      if (mask & *fp3)
		{
		  if (count == 0)
		    {
		      if (j != default_LA)
			{
			  rule = LAruleno[j];
			  fprintf(foutput, "    %-4s\treduce  %d  (%s)\n",
				  tags[i], rule, tags[rlhs[rule]]);
			}
		      else defaulted = 1;

		      count++;
		    }
		  else
		    {
		      if (defaulted)
			{
			  rule = LAruleno[default_LA];
			  fprintf(foutput, "    %-4s\treduce  %d  (%s)\n",
				  tags[i], rule, tags[rlhs[rule]]);
			  defaulted = 0;
			}
		      rule = LAruleno[j];
		      fprintf(foutput, "    %-4s\t[reduce  %d  (%s)]\n",
			      tags[i], rule, tags[rlhs[rule]]);
		    }
		}

	      fp3 += tokensetsize;
	    }

	  mask <<= 1;
	  if (mask == 0)
	    {
	      mask = 1;
	      fp1++;
	    }
	}

      if (default_LA >= 0)
	{
	  fprintf(foutput, "    $default\treduce  %d  (%s)\n",
		  default_rule, tags[rlhs[default_rule]]);
	}

      putc('\n', foutput);
    }
}



void finalize_conflicts(void)
{
  FREE(conflicts);
  FREE(shiftset);
  FREE(lookaheadset);
}
